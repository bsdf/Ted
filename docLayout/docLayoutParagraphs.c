/************************************************************************/
/*									*/
/*  Layout of a document. Layout of a series of paragraphs in a common	*/
/*  parent.								*/
/*									*/
/************************************************************************/

#   include	"docLayoutConfig.h"

#   include	<stddef.h>

#   include	"docLayout.h"
#   include	<docPageGrid.h>
#   include	<docTreeType.h>
#   include	<docTreeNode.h>
#   include	<docNodeTree.h>
#   include	<docTextLine.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Preparation for vertical alignment of tabe cells. It is also used	*/
/*  to position the contents of a frame.				*/
/*									*/
/************************************************************************/

void docRedoParaStripLayout(	const LayoutJob *		lj,
				BlockFrame *			bf,
				const LayoutPosition *		lpFrom,
				BufferItem *			cellNode,
				int				childFrom,
				int				childUpto )
    {
    const LayoutContext *	lc= &(lj->ljContext);
    int				stopCode= FORMATstopREADY;
    int				para;
    BufferItem *		paraBi0= cellNode->biChildren[childFrom];

    int				xStopCode= 0;

    ParagraphLayoutPosition	plpRedo;
    const int			isRedo= 1;

    docInitParagraphLayoutPosition( &plpRedo );

    docStripLayoutStartChild( &plpRedo, childFrom );
    plpRedo.plpPos= *lpFrom;

    /*  1  */
    if  ( docLayoutStartParagraph( lj, &xStopCode, paraBi0, bf, &plpRedo ) )
	{ LDEB(1); return;	}

    if  ( xStopCode != FORMATstopREADY )
	{ LDEB(xStopCode); return;	}

    for ( para= childFrom; para < childUpto; para++ )
	{
	int		changed= 0;
	int		accepted;
	BufferItem *	paraNode= cellNode->biChildren[para];

	docStripLayoutStartChild( &plpRedo, para );

	paraNode->biTopPosition= plpRedo.plpPos;

	accepted= docLayoutParaLines( &stopCode, isRedo,
					&(plpRedo.plpParagraphFrame),
					&(plpRedo.plpPos), &(plpRedo.pspLine),
					bf, lj, paraNode, plpRedo.pspPart );
	if  ( accepted < 0 )
	    { LDEB(accepted); break;	}

	docLayoutSetNodeBottom( &changed, paraNode, &(plpRedo.plpPos),
					    lc, lj->ljChangedRectanglePixels );
	}

    return;
    }

/************************************************************************/

static int docLayoutChildInStrip(
			int *				pStopCode,
			ParagraphLayoutPosition *	plp,
			BlockFrame *			bf,
			const LayoutJob *		lj,
			int				cellTopInset,
			BufferItem *			childNode )
    {
    switch( childNode->biLevel )
	{
	case DOClevPARA:
	    return docLayoutParagraphInStrip( pStopCode, plp, bf, lj,
						    cellTopInset, childNode );
	case DOClevROW:
	    return docLayoutRowInStrip( pStopCode, plp, bf, lj,
						    cellTopInset, childNode );
	default:
	    LDEB(childNode->biLevel); return -1;
	}
    }

/************************************************************************/

static int docLayoutGetChildFrame(	int *			pFrameNumber,
					FrameProperties *	fp,
					const BufferDocument *	bd,
					const BufferItem *	childNode )
    {
    int		isFrame= 0;
    int		frameNumber;

    switch( childNode->biLevel )
	{
	case DOClevPARA:
	    frameNumber= childNode->biParaFrameNumber;
	    break;
	case DOClevROW:
	    frameNumber= childNode->biRowFrameNumber;
	    break;
	default:
	    LDEB(childNode->biLevel);
	    frameNumber= -1;
	    break;
	}

    if  ( frameNumber >= 0 )
	{
	docGetFramePropertiesByNumber( fp, bd, frameNumber );
	isFrame= DOCisFRAME( fp );
	}

    *pFrameNumber= frameNumber;
    return isFrame;
    }

/************************************************************************/
/*									*/
/*  Layout as much of a series of paragraph as fits on the current	*/
/*  page. (Actually the column on the page).				*/
/*									*/
/*  a)  Do not format children that belong to different frames.		*/
/*									*/
/************************************************************************/

int docLayoutStripChildren(	int *				pStopCode,
				ParagraphLayoutJob *		plj,
				BlockFrame *			bfFlow,
				const LayoutJob *		lj,
				BufferItem *			cellNode )
    {
    const LayoutContext *	lc= &(lj->ljContext);
    ParagraphLayoutPosition *	plp= &(plj->pljPos);
    const int			childFrom= plp->pspChild;
    BufferItem *		childNode= cellNode->biChildren[childFrom];

    BlockFrame *		bf= bfFlow;

    const BufferDocument *	bd= lc->lcDocument;

    FrameProperties		fpBegin;
    int				frameBegin;
    int				wasFrame;

    FrameProperties		fpRow;
    int				frameRow= -1;
    int				rowIsFrame= 0;

    BufferItem *		rowBi;

    if  ( ! docIsCellNode( cellNode ) )
	{ SDEB(docLevelStr(cellNode->biLevel)); return -1;	}
    if  ( childFrom >= cellNode->biChildCount )
	{ LLDEB(childFrom,cellNode->biChildCount); return -1;	}

    rowBi= docGetRowNode( cellNode );
    if  ( rowBi )
	{
	DocumentPosition	dp;

	if  ( ! docHeadPosition( &dp, rowBi ) )
	    {
	    rowIsFrame= docLayoutGetChildFrame( &frameRow, &fpRow,
							    bd, dp.dpNode );
	    }
	}

    /*  a  */
    childNode= cellNode->biChildren[plj->pljPos.pspChild];
    wasFrame= docLayoutGetChildFrame( &frameBegin, &fpBegin, bd, childNode );
    if  ( wasFrame && ( ! rowIsFrame || frameRow != frameBegin ) )
	{
	BlockFrame			bfTextFrame;
	ParagraphLayoutPosition		plpTextFrame;
	int				frameHeight= fpBegin.fpHighTwips;

	if  ( frameHeight < 0 )
	    { frameHeight= -frameHeight;	}

	plpTextFrame= *plp;
	plp= &plpTextFrame;

	docLayoutInitBlockFrame( &bfTextFrame );
	docLayoutSetTextFrame( &bfTextFrame, &(plp->plpPos),
						bf, &fpBegin, frameHeight );

	plp->plpPos.lpPageYTwips= bfTextFrame.bfContentRect.drY0;
	bf= &bfTextFrame;
	}

    while( plp->pspChild < plj->pljChildUpto )
	{
	int			stopCode= FORMATstopREADY;
	int			ret;

	FrameProperties		fpHere;
	int			frameHere;
	int			isFrame;

	childNode= cellNode->biChildren[plp->pspChild];
	isFrame= docLayoutGetChildFrame( &frameHere, &fpHere, bd, childNode );

	/*  a  */
	if  ( isFrame != wasFrame				||
	      ( wasFrame && ( frameHere != frameBegin )	)	)
	    {
	    if  ( wasFrame )
		{
		docLayoutFinishFrame( &fpBegin, bf, bfFlow, lj,
			&(plj->pljPos), cellNode, childFrom, plp->pspChild );

		plj->pljPos= *plp;
		}

	    *pStopCode= FORMATstopNEXT_FRAME;
	    break;
	    }

	ret= docLayoutChildInStrip( &stopCode, plp, bf, lj,
					cellNode->biCellTopInset, childNode );
	if  ( ret < 0 )
	    { LDEB(ret); return -1;		}

	switch( ret )
	    {
	    case FORMATstopBLOCK_FULL:
	    case FORMATstopPAGE_BREAK:
	    case FORMATstopCOLUMN_BREAK:
		*pStopCode= stopCode;
		break;

	    case FORMATstopPARTIAL:
	    case FORMATstopREADY:
		continue;

	    default:
		LDEB(ret); continue;
	    }

	break;
	}

    if  ( docLayoutStripDone( plp, plj ) )
	{ *pStopCode= FORMATstopREADY;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Find a start point for the formatter to layout lines. A certain	*/
/*  work ahead is necessary to decide whether work on line layout can	*/
/*  be used or not. docCommitStripLayout() is used to decide what	*/
/*  work is final and what possibly is to be redone.			*/
/*									*/
/*  docCommitStripLayout() is called when a column is full. In	*/
/*  this case, everything that is not final yet needs to be moved to	*/
/*  the next page.							*/
/*									*/
/*  -   Insure that the constraints comming from Widow/Orphan control	*/
/*	are been obeyed.						*/
/*  -   Insure that paragraphs that should not be divided over pages	*/
/*	are not split.							*/
/*  -   Insure that paragraphs that should be with the next one as a	*/
/*	header really are with it. (Experimenting with Word shows that	*/
/*	paragraphs that should be with the next one should be		*/
/*	completely with it.)						*/
/*									*/
/*  When any of the above is violated, find a solution that respects	*/
/*  the requirements by pushing offending paragraphs or lines down.	*/
/*									*/
/*  docCommitStripLayout() shifts 'plp0' to the last position that is	*/
/*  not below 'plp1' that is not influenced by the layout of position	*/
/*  on 'plp1' or below. Everything before 'plp1' is formatted and	*/
/*  plp1->plpPos is the position where formatting would continue if	*/
/*  docCommitStripLayout() moved 'plp0' onto 'plp1'. (As it often does)	*/
/*									*/
/*  REGULAR APPROACH							*/
/*  ================							*/
/*  a)  Avoid superfluous work and the handling of this exception	*/
/*	below: Commit an explit page break.				*/
/*  b)	If the current position is a legal position for a page break..	*/
/*	break here.							*/
/*									*/
/*  'Keep With Next' and 'Widow/Orphan Contol'				*/
/*  ==========================================				*/
/*  1)  Find the place to begin. Anything on a previous page is final:	*/
/*	It will not be shifted to the next page. (Paragraph)		*/
/*  2)	Idem: Line. The line found here might very well be irrelevant	*/
/*	because a different paragraph to commit is found below.		*/
/*  3)	Commit all paragraphs before the current one that do not have	*/
/*	the 'Keep with Next' flag.					*/
/*	flag.								*/
/*  4)	If the paragraph to be committed is before the current one	*/
/*	commit the position we have found. This can be:			*/
/*	-   The first line on the current page (from 2)			*/
/*	    [ This case is however excluded by the exception code (0)].	*/
/*	-   The head of a series of KeepWithNext paragraphs. (from 3)	*/
/*  5)	When this paragraph is to be kept together, restart from its	*/
/*	beginning.							*/
/*  6)	When Widow/Orphan control is active.. Do not commit the first	*/
/*	line before the whole paragraph is committed.			*/
/*  7)	The special cases of the last line of a paragraph with widow	*/
/*	orphan control and a paragraph with three lines are covered	*/
/*	in docLayoutStripChildren() at the end of a paragraph.		*/
/*	So no exceptions have to be made here. Also note the		*/
/*	preparations in docAdjustParaLayout().				*/
/*									*/
/*  8)	And finally when no exception applies, start from where we are	*/
/*	now.								*/
/*									*/
/************************************************************************/

static void docCommitStripLayout_x(
				ParagraphLayoutPosition *	plp0,
				const ParagraphLayoutPosition *	plp1,
				int				advanceAnyway,
				int				page,
				int				column,
				const BufferItem *		cellNode )
    {
    const BufferItem *		paraBi0;
    const BufferItem *		paraBi1;
    const TextLine *		tl;

    int				para;

    int				line0;

    paraBi0= cellNode->biChildren[plp0->pspChild];
    paraBi1= (const BufferItem *)0;

    if  ( paraBi0->biLevel != DOClevPARA )
	{ LSDEB(paraBi0->biLevel,docLevelStr(paraBi0->biLevel)); return; }

    /*  a  */
    if  ( plp1->pspChild < cellNode->biChildCount )
	{
	int		line1;

	paraBi1= cellNode->biChildren[plp1->pspChild];

	line1= plp1->pspLine;

	if  ( line1- 1 >= 0				&&
	      line1- 1 < paraBi1->biParaLineCount	)
	    {
	    const TextLine *	tl= paraBi1->biParaLines+ line1- 1;

	    if  ( tl->tlFlags & TLflagBLOCKBREAK )
		{
		*plp0= *plp1;
		return;
		}
	    }
	}

    /*  b  */
    if  ( paraBi1			&&
	  paraBi1->biParaKeepOnPage	&&
	  ! paraBi1->biParaKeepWithNext	&&
	  plp1->pspLine > 0		)
	{
	int		line= 1;

	if  ( paraBi1->biParaWidowControl )
	    { line++;	}

	if  ( plp1->pspLine >= line )
	    {
	    *plp0= *plp1;
	    return;
	    }
	}

    /*  1  */
    while( plp0->pspChild < plp1->pspChild )
	{
	if  ( paraBi0->biBelowPosition.lpPage >  page )
	    { break;	}
	if  ( paraBi0->biBelowPosition.lpPage == page		&&
	      paraBi0->biBelowPosition.lpColumn >= column	)
	    { break;	}

	docStripLayoutNextChild( plp0 );
	paraBi0= cellNode->biChildren[plp0->pspChild];
	}

    /*  2  */
    if  ( plp0->pspChild < plp1->pspChild )
	{ line0= paraBi0->biParaLineCount;	}
    else{ line0= plp1->pspLine;			}

    tl= paraBi0->biParaLines+ plp0->pspLine;
    while( plp0->pspLine < line0 && plp0->pspLine < paraBi0->biParaLineCount )
	{
	if  ( tl->tlTopPosition.lpPage >  page )
	    { break;	}
	if  ( tl->tlTopPosition.lpPage == page	&&
	      tl->tlTopPosition.lpColumn >= column	)
	    { break;	}

	plp0->pspLine++; tl++;
	}

    /*  3  */
    for ( para= plp0->pspChild; para < plp1->pspChild; para++ )
	{
	const BufferItem *	paraNode= cellNode->biChildren[para];

	if  ( advanceAnyway || ! paraNode->biParaKeepWithNext )
	    {
	    docStripLayoutStartChild( plp0, para+ 1 );

	    if  ( plp0->pspChild < cellNode->biChildCount )
		{ paraBi0= cellNode->biChildren[plp0->pspChild];	}
	    /*  else .. return below */
	    }
	}

    /*  4  */
    if  ( plp0->pspChild < plp1->pspChild )
	{ return;	}

    if  ( plp0->pspChild != plp1->pspChild )
	{ LLLLDEB(page,column,plp0->pspChild,plp1->pspChild );	}

    if  ( plp0->pspChild >= cellNode->biChildCount )
	{ return;	}

    /*  5  */
    if  ( ! advanceAnyway			&&
	  ( paraBi0->biParaKeepOnPage	||
	    paraBi0->biParaKeepWithNext	)	)
	{
	docStripLayoutStartChild( plp0, plp0->pspChild );
	return;
	}

    /*  6  */
    if  ( ! advanceAnyway		&&
	  paraBi1			&&
	  paraBi1->biParaWidowControl	&&
	  plp1->pspLine == 1		&&
	  paraBi1->biParaLineCount >= 1	)
	{
	tl= paraBi1->biParaLines+ 0;

	if  ( tl->tlFirstParticule+ tl->tlParticuleCount <
					paraBi1->biParaParticuleCount	)
	    {
	    docStripLayoutStartChild( plp0, plp0->pspChild );
	    return;
	    }
	}

    /*  7  */

    /*  8  */
    *plp0= *plp1;

    return;
    }

/************************************************************************/
/*									*/
/*  Find out whether the fact that layout has proceeded to plj->pljPos	*/
/*  makes any layout final. If so.. Move pljPos0 past what has become	*/
/*  final.								*/
/*									*/
/************************************************************************/

# define docSetLayoutProgress( plpt, plpf ) \
    { \
    (plpt)->pspChild= (plpf)->pspChild; \
    (plpt)->pspChildAdvanced= (plpf)->pspChildAdvanced; \
    (plpt)->pspPart= (plpf)->pspPart; \
    (plpt)->pspLine= (plpf)->pspLine; \
    }

static int docCompareLayoutProgress(
			const ParagraphLayoutPosition *	plp0,
			const ParagraphLayoutPosition *	plp1 )
    {
    if  ( plp1->pspChild > plp0->pspChild )
	{ return  1;	}
    if  ( plp1->pspChild < plp0->pspChild )
	{ return -1;	}

    if  ( plp1->pspPart > plp0->pspPart )
	{ return  1;	}
    if  ( plp1->pspPart < plp0->pspPart )
	{ return -1;	}

    return 0;
    }

void docCommitStripLayout(
				int *				pAdvanced,
				int				advanceAnyway,
				ParagraphLayoutJob *		plj,
				int				page,
				int				column,
				const BufferItem *		cellNode )
    {
    int				advanced= 0;
    ParagraphLayoutPosition	plp0;

    plp0= plj->pljPos0;

    docCommitStripLayout_x( &plp0, &(plj->pljPos),
				    advanceAnyway, page, column, cellNode );

    advanced= ( docCompareLayoutProgress( &plp0, &(plj->pljPos0) ) < 0 );
    if  ( advanced )
	{
	docSetLayoutProgress( &(plj->pljPos0), &plp0 );
	docSetLayoutProgress( &(plj->pljPos), &plp0 );
	}
    else{
#	if 0
	int cmp= docCompareLayoutProgress( &(plj->pljPos0), &(plj->pljPos) );
	if  ( cmp >= 0 )
	    { LDEB(cmp);	}
#	endif
	/* Prevent loops: Be sure to advance */
	docSetLayoutProgress( &(plj->pljPos0), &(plj->pljPos) );
	}

    *pAdvanced= advanced;
    }

/************************************************************************/
/*									*/
/*  Find out where to start formatting a strip.				*/
/*  After some thought: That is the position that would be committed	*/
/*  if the strip were formatted upto the stat position.			*/
/*									*/
/************************************************************************/

void docFindStripLayoutOrigin(	ParagraphLayoutJob *		plj,
				int				page,
				int				column,
				const BufferItem *		cellNode )
    {
    int				advanceAnyway= 0;

    docCommitStripLayout_x( &(plj->pljPos0), &(plj->pljPos),
					advanceAnyway, page, column, cellNode );

    return;
    }

/************************************************************************/
/*									*/
/*  Format the lines in a series of paragraphs. On the way keep an	*/
/*  administration on where to restart formatting at a page break.	*/
/*									*/
/*  1)  Place as much as fits on the first page.			*/
/*  2)  While unformatted paragraphs remain: place some more on		*/
/*	subsequent pages.						*/
/*  3)  First place the footnotes on the page.				*/
/*  4)  Then find out where to restart the layout job on the next page	*/
/*	Widow/Orphan control etc can cause some of the text that we	*/
/*	already formatted to go to the next page.			*/
/*  5)  Skip to the next page.						*/
/*  6)  Determine available space on the next page.			*/
/*  7)  Place as much as fits on the next page.				*/
/*									*/
/************************************************************************/

int docLayoutStackedStrip(	BufferItem *			cellNode,
				BlockFrame *			bf,
				const LayoutJob *		lj,
				ParagraphLayoutJob *		plj )
    {
    LayoutPosition		lpBefore;

    int				prevAdvanced= 1;
    int				advanceAnyway= 0;
    int				stopCode= FORMATstopREADY;
    int				prevStopCode= FORMATstopREADY;

    lpBefore= plj->pljPos.plpPos;

    /*  1  */
    if  ( docLayoutStripChildren( &stopCode, plj, bf, lj, cellNode ) )
	{ LDEB(1); return -1;	}

    /*  2  */
    while( stopCode != FORMATstopREADY		&&
	   stopCode != FORMATstopFRAME_FULL	)
	{
	int				advanced;
	const int			belowText= 0;
	LayoutPosition			lpBelowNotes;

	/*  3  */
	if  ( BF_HAS_FOOTNOTES( bf )					&&
	      ( cellNode->biTreeType == DOCinBODY		||
	        cellNode->biTreeType == DOCinENDNOTE	)	&&
	      ! bf->bfFootnotesPlaced					&&
	      docLayoutFootnotesForColumn( &lpBelowNotes,
			    &(plj->pljPos.plpPos), bf, belowText, lj )	)
	    { LDEB(1); return -1;	}

	/*  4  */
	docCommitStripLayout( &advanced, advanceAnyway, plj,
					lpBefore.lpPage, lpBefore.lpColumn,
					cellNode );

	/*  5  */
	switch( stopCode )
	    {
	    case FORMATstopBLOCK_FULL:
	    case FORMATstopCOLUMN_BREAK:
		docLayoutToNextColumn( &(plj->pljPos.plpPos), bf, cellNode, lj );
		break;

	    case FORMATstopPAGE_BREAK:
		docLayoutToNextColumn( &(plj->pljPos.plpPos), bf, cellNode, lj );
		while( plj->pljPos.plpPos.lpColumn > 0 )
		    {
		    docLayoutToNextColumn( &(plj->pljPos.plpPos),
							    bf, cellNode, lj );
		    }
		break;

	    default:
		break;
	    }

	if  ( ! advanced )
	    {
	    if  ( ! prevAdvanced )
		{
		if  ( advanceAnyway )
		    {
		    LLLDEB(prevAdvanced,advanced,advanceAnyway);
		    LLDEB(prevStopCode,stopCode);
		    RECTDEB(&(plj->pljPos.plpParagraphFrame.pfParaContentRect));

		    docStripLayoutNextChild( &(plj->pljPos) );
		    docSetLayoutProgress( &(plj->pljPos0), &(plj->pljPos) );
		    advanced= 1;

		    if  ( docLayoutStripDone( &(plj->pljPos), plj ) )
			{ break;	}
		    }
		else{ advanceAnyway= 1;	}
		}
	    }

	prevAdvanced= advanced;

	/*  6  */
	docCellStripFrame( cellNode, bf, plj );

	/*  7  */
	lpBefore= plj->pljPos.plpPos;

	prevStopCode= stopCode;
	if  ( docLayoutStripChildren( &stopCode, plj, bf, lj, cellNode ) )
	    { LDEB(1); return -1;	}
	}

    if  ( cellNode->biChildCount > 0 )
	{ cellNode->biTopPosition= cellNode->biChildren[0]->biTopPosition; }

    return 0;
    }

/************************************************************************/
/*									*/
/*  Calculate the formatting frame for a paragraph layout.		*/
/*									*/
/************************************************************************/

void docCellStripFrame(		BufferItem *			cellNode,
				const BlockFrame *		bf,
				ParagraphLayoutJob *		plj )
					
    {
    BufferItem *		childNode;
    ParagraphFrame *		pf= &(plj->pljPos.plpParagraphFrame);

    childNode= cellNode->biChildren[plj->pljPos.pspChild];

    if  ( childNode->biLevel == DOClevPARA )
	{ docParagraphFrameTwips( pf, bf, childNode );	}
    else{ docCellFrameTwips( pf, bf, cellNode );	}

    return;
    }

