/************************************************************************/
/*									*/
/*  Layout of a table row in the document.				*/
/*									*/
/************************************************************************/

#   include	"docLayoutConfig.h"

#   include	<stddef.h>

#   include	"docLayout.h"
#   include	<docTreeType.h>
#   include	<docTreeNode.h>
#   include	"docRowLayout.h"
#   include	<docDebug.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Shift cell contents of the cells in a row that are not top-aligned.	*/
/*									*/
/************************************************************************/

static int docLayoutRowValignCells(
				BufferItem *			rowNode,
				BlockFrame *			bf,
				const LayoutPosition *		lpHere,
				const ParagraphLayoutPosition *	rowPlp,
				const LayoutJob *		lj )
    {
    int				col;
    const CellProperties *	cp= rowNode->biRowCells;
    const ParagraphLayoutJob *	plj;

    cp= rowNode->biRowCells;
    plj= rowPlp->pspChildren;
    for ( col= 0; col < rowNode->biChildCount; cp++, plj++, col++ )
	{
	const CellProperties *	thisCp= cp;
	BufferItem *		cellNode= rowNode->biChildren[col];
	int			rowTop= rowNode->biTopPosition.lpPageYTwips;
	int			rowHigh;
	int			cellHigh;

	int			paraFrom= 0;
	int			paraUpto= cellNode->biChildCount;

	LayoutPosition		lpTop;

	if  ( cellNode->biCellRowspan > 1 )
	    { continue;	}

	if  ( cp->cpVerticalMerge == CELLmergeFOLLOW )
	    {
	    if  ( cellNode->biCellMergedCellTopRow < 0	)
		{ continue;	}
	    else{
		BufferItem *	topRowNode;

		int		topRow= cellNode->biCellMergedCellTopRow;
		int		topCol= cellNode->biCellMergedCellTopCol;

		topRowNode= rowNode->biParent->biChildren[topRow];
		rowTop= topRowNode->biTopPosition.lpPageYTwips;
		cellNode= topRowNode->biChildren[topCol];
		thisCp= topRowNode->biRowCells+ topCol;
		}
	    }

	lpTop= cellNode->biTopPosition;
	rowHigh= lpHere->lpPageYTwips- rowTop;
	cellHigh= cellNode->biBelowPosition.lpPageYTwips-
				    cellNode->biTopPosition.lpPageYTwips;

	if  ( cellHigh >= rowHigh )
	    { continue;	}

	switch( thisCp->cpValign )
	    {
	    case DOCtvaTOP:
		continue;

	    case DOCtvaCENTERED:
		cellNode->biCellTopInset= ( rowHigh- cellHigh )/ 2;
		lpTop.lpPageYTwips += cellNode->biCellTopInset;

		docRedoParaStripLayout( lj, bf, &lpTop,
						cellNode, paraFrom, paraUpto );
		continue;

	    case DOCtvaBOTTOM:
		cellNode->biCellTopInset= rowHigh- cellHigh;
		lpTop.lpPageYTwips += cellNode->biCellTopInset;

		docRedoParaStripLayout( lj, bf, &lpTop,
						cellNode, paraFrom, paraUpto );
		continue;

	    default:
		LLDEB(col,thisCp->cpValign);
		continue;
	    }
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Finish row layout.							*/
/*									*/
/*  1)  The bottom is below all cells.					*/
/*  2)  If there is a bottom inset, step over it.			*/
/*  3)  Stretch height to the minimum height (If any)			*/
/*  4)  Stretch height to the exact given height (If any)		*/
/*  5)  Fix the current position if a fixed-height row exceeds its	*/
/*	fixed height.							*/
/*									*/
/************************************************************************/

static void docLayoutFinishRow(	LayoutPosition *		lpHere,
				BufferItem *			rowNode,
				BlockFrame *			bf,
				const LayoutJob *		lj,
				ParagraphLayoutPosition *	rowPlp )
    {
    int				col;
    CellProperties *		cp;
    ParagraphLayoutJob *	plj;

    /*  1  */
    cp= rowNode->biRowCells;
    plj= rowPlp->pspChildren;
    for ( col= 0; col < rowNode->biChildCount; cp++, plj++, col++ )
	{
	BufferItem *			cellNode= rowNode->biChildren[col];
	ParagraphLayoutPosition *	plp= &(plj->pljPos);

	if  ( cp->cpVerticalMerge == CELLmergeFOLLOW		&&
	      cellNode->biCellMergedCellTopRow >= 0		)
	    {
	    docLayoutPushBottomDown( &(rowNode->biRowBelowAllCellsPosition),
					&(plj->cljMergedCellBelowPosion) );
	    docLayoutPushBottomDown( lpHere,
					&(plj->cljMergedCellBelowPosion) );
	    continue;
	    }

	if  ( CELL_MERGED( cp ) )
	    { continue;	}

	cellNode->biBelowPosition= plp->plpPos;

	docLayoutPushBottomDown(
		    &(rowNode->biRowBelowAllCellsPosition), &(plp->plpPos) );

	if  ( cellNode->biCellRowspan <= 1 )
	    { docLayoutPushBottomDown( lpHere, &(plp->plpPos) ); }
	}

    if  ( DOC_SAME_FRAME( lpHere, &(rowNode->biTopPosition) ) )
	{
	int		rowHeightTwips;

	rowHeightTwips= lpHere->lpPageYTwips-
				    rowNode->biTopPosition.lpPageYTwips;

	/*  3: if rowNode->biRowHeightTwips > 0   */
	if  ( rowHeightTwips < rowNode->biRowHeightTwips )
	    {
	    lpHere->lpPageYTwips +=  rowNode->biRowHeightTwips- rowHeightTwips;
	    lpHere->lpAtTopOfColumn= 0;
	    }

	/*  4: if rowNode->biRowHeightTwips < 0  */
	if  ( rowHeightTwips < -rowNode->biRowHeightTwips )
	    {
	    lpHere->lpPageYTwips += -rowNode->biRowHeightTwips- rowHeightTwips;
	    lpHere->lpAtTopOfColumn= 0;
	    }

	/*  5  */
	if  ( rowNode->biRowHeightTwips < 0			&&
	      rowHeightTwips > -rowNode->biRowHeightTwips	)
	    {
	    /* LLDEB(rowNode->biRowHeightTwips,rowHeightTwips); */
	    lpHere->lpPageYTwips -= rowHeightTwips+ rowNode->biRowHeightTwips;
	    }

	if  ( docLayoutRowValignCells( rowNode, bf, lpHere, rowPlp, lj ) )
	    { LDEB(1);	}
	}

    docLayoutPushBottomDown( &(rowNode->biRowBelowAllCellsPosition), lpHere );

    return;
    }

/************************************************************************/
/*									*/
/*  Layout as much of a table row as fits on the current page.		*/
/*									*/
/*  1)  For a fixed height row, determine the amount of space left for	*/
/*	it.								*/
/*  2)  For all columns in the table..					*/
/*  3)  Cells in a row span or a colspan play no role...		*/
/*  4)  This column was already exhausted on a previous page..		*/
/*  5)  Format paragraphs inside this cell.				*/
/*  6)  Remember how far we have descended on the page.			*/
/*  7)  Anything left over that must be moved to the next page?		*/
/*  7a) Distinguish between the situation where a fixed height row has	*/
/*	reached its capacity and that where the page is full.		*/
/*									*/
/************************************************************************/

static int docLayoutRowPageStrip(
			    LayoutPosition *		lpHere,
			    int *			pToNextColumn,
			    int *			pSomeAtHead,
			    BufferItem *		rowNode,
			    ParagraphLayoutPosition *	rowPlp,
			    BlockFrame *		bf,
			    const LayoutJob *		lj )
    {
    int				col;

    int				continueInNextFrame= 0;
    int				someAtHead= 0;

    LayoutPosition		lpBottom;
    const CellProperties *	cp;
    ParagraphLayoutJob *	plj;

    lpBottom= *lpHere;

    continueInNextFrame= 0;

    if  ( rowPlp->pspChildCount != rowNode->biChildCount )
	{ LLDEB(rowPlp->pspChildCount,rowNode->biChildCount); return -1; }

    /*  1  */

    /*  2  */
    cp= rowNode->biRowCells;
    plj= rowPlp->pspChildren;
    for ( col= 0; col < rowNode->biChildCount; cp++, plj++, col++ )
	{
	BufferItem *			cellNode= rowNode->biChildren[col];
	ParagraphLayoutPosition *	plp= &(plj->pljPos);
	int				stopCode= FORMATstopREADY;

	/*  3  */
	if  ( CELL_MERGED( cp ) )
	    { continue;	}

	/*  4  */
	if  ( docLayoutStripDone( plp, plj ) )
	    { continue;	}

	/*  5  */
	plp->plpPos= *lpHere;

	docCellStripFrame( cellNode, bf, plj );

	if  ( docLayoutStripChildren( &stopCode, plj, bf, lj, cellNode ) )
	    { LDEB(1); return -1;	}

	/*  6  */
	docLayoutPushBottomDown(
		    &(rowNode->biRowBelowAllCellsPosition), &(plp->plpPos) );
	if  ( cellNode->biCellRowspan <= 1 )
	    { docLayoutPushBottomDown( &lpBottom, &(plp->plpPos) );	}

	/*  7,7a  */
	if  ( cellNode->biTreeType == DOCinBODY		&&
	      stopCode == FORMATstopBLOCK_FULL		)
	    { continueInNextFrame= 1;	}
	}

    cp= rowNode->biRowCells;
    plj= rowPlp->pspChildren;
    for ( col= 0; col < rowNode->biChildCount; cp++, plj++, col++ )
	{
	const ParagraphLayoutPosition *	plp= &(plj->pljPos);

	if  ( docLayoutAtStripHead( &(plj->pljPos) ) )
	    {
	    if  ( ! docLayoutStripDone( plp, plj ) )
		{ someAtHead= 1;	}
	    }
	else{ rowPlp->pspChildAdvanced= 1;	}
	}

    *lpHere= lpBottom;
    *pToNextColumn= continueInNextFrame;
    *pSomeAtHead= someAtHead;

    if  ( ! continueInNextFrame )
	{ docLayoutFinishRow( lpHere, rowNode, bf, lj, rowPlp );	}

    return 0;
    }

/************************************************************************/

static int docLayoutStartRowCells(
				BufferItem *			rowNode,
				const BufferDocument *		bd,
				const LayoutPosition *		lpHere,
				const BlockFrame *		bf,
				ParagraphLayoutPosition *	rowPlp )
    {
    CellProperties *		cp;
    int				col;
    int				l, r;
    ParagraphLayoutJob *	cellPlj;

    int				nextRow= rowNode->biNumberInParent+ 1;
    const BufferItem *		nextRowBi= (const BufferItem *)0;

    if  ( nextRow < rowNode->biRowTablePast )
	{ nextRowBi= rowNode->biParent->biChildren[nextRow]; }

    rowNode->biRowBelowAllCellsPosition= *lpHere;

    /*  2  */
    cellPlj= rowPlp->pspChildren;
    r= rowNode->biRowLeftIndentTwips;
    cp= rowNode->biRowCells;
    for ( col= 0; col < rowNode->biChildCount; cp++, cellPlj++, col++ )
	{
	BufferItem *	cellNode= rowNode->biChildren[col];
	const int	para= 0;
	const int	line= 0;
	const int	part= 0;
	int		colspan= 1;

	const int	recursively= 0;

	docDelimitTables( cellNode, recursively );

	cellNode->biCellTopInset= 0;

	docBeginParagraphLayoutProgress( cellPlj, para, line, part,
					    cellNode->biChildCount, lpHere );

	cellNode->biTopPosition= *lpHere;

	l= r; r= cp->cpRightBoundaryTwips;
	if  ( cp->cpHorizontalMerge == CELLmergeHEAD )
	    { r= docGetCellRight( &colspan, cellNode );	}

	/*  3  */
	cellNode->biCellRowspan= 1;
	if  ( cp->cpVerticalMerge != CELLmergeNONE )
	    {
	    docRowLayoutRowspanAdmin( cellNode, rowNode, nextRowBi, nextRow,
							cp, l, r, cellPlj );
	    }

	cp += colspan- 1;
	cellPlj += colspan- 1;
	col += colspan- 1;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Calculate the layout of a row in the document.			*/
/*									*/
/*  1)  Do not advance by row top inset. This is done between the top	*/
/*	of the paragraph and its first line. [This is what MS-Word	*/
/*	does.]								*/
/*  2)  Begin formatting.						*/
/*  3)  Keep track of the rowspan of the cells.				*/
/*									*/
/************************************************************************/

static int docLayoutStartRow(	int *				pToNextColumn,
				BufferItem *			rowNode,
				const BufferDocument *		bd,
				LayoutPosition *		lpHere,
				const BlockFrame *		bf,
				ParagraphLayoutPosition *	rowPlp )
    {
    const int			atRowTop= 1;

    int				minHeightTwips= rowNode->biRowHeightTwips;

    if  ( docParagraphLayoutPosClaimChildren( rowPlp, rowNode->biChildCount ) )
	{ LDEB(rowNode->biChildCount); return -1;	}

    if  ( rowNode->biRowHeightTwips < 0 )
	{ minHeightTwips= -rowNode->biRowHeightTwips;	}

    /*  3a  */
    if  ( lpHere->lpPageYTwips+ minHeightTwips > bf->bfFlowRect.drY1	&&
	  ! lpHere->lpAtTopOfColumn					)
	{ *pToNextColumn= 1; return 0;	}

    rowNode->biRowAboveHeaderPosition= rowNode->biTopPosition;
    rowNode->biRowPrecededByHeader= 0;

    /*  1  */
    /*  NO! lpHere->lpPageYTwips += rowNode->biRowTopInset; */

    {
    int rti= rowNode->biRowTopInset;
    docLayoutCalculateRowTopInset( &rti, bd, rowNode, atRowTop );
    rowNode->biRowTopInset= rti;
    }

    if  ( docLayoutStartRowCells( rowNode, bd, lpHere, bf, rowPlp ) )
	{ LDEB(1); return -1;	}

    *pToNextColumn= 0;
    return 0;
    }

/************************************************************************/
/*									*/
/*  Reserve space for a table header.					*/
/*									*/
/************************************************************************/

static void docLayoutRowSkipHeaderHeight( LayoutPosition *	lp,
					BufferItem *		rowNode,
					int			atTopOfRow )
    {
    const BufferItem *	headerBi;
    int			headerHeight;

    if  ( rowNode->biRowTableHeaderRow < 0 )
	{ LDEB(rowNode->biRowTableHeaderRow); return;	}
    if  ( rowNode->biRowIsTableHeader )
	{ LDEB(rowNode->biRowIsTableHeader); return;	}

    headerBi= rowNode->biParent->biChildren[rowNode->biRowTableHeaderRow];
    if  ( ! headerBi->biRowIsTableHeader )
	{ LDEB(headerBi->biRowIsTableHeader);	}

    if  ( headerBi->biBelowPosition.lpPage	!=
	  headerBi->biTopPosition.lpPage	)
	{
	LLDEB(headerBi->biBelowPosition.lpPage,headerBi->biTopPosition.lpPage);
	return;
	}

    if  ( atTopOfRow )
	{
	rowNode->biRowAboveHeaderPosition= *lp;
	rowNode->biRowPrecededByHeader= 1;
	}

    headerHeight= headerBi->biBelowPosition.lpPageYTwips- 
					headerBi->biTopPosition.lpPageYTwips;

    lp->lpAtTopOfColumn= 0;
    lp->lpPageYTwips += headerHeight;

    return;
    }

/************************************************************************/
/*									*/
/*  Continue a table row on the next page.				*/
/*									*/
/*  1)  Finish the footnotes that we found on the current page.		*/
/*  2)  Move to the next page. (Column in multi column sections)	*/
/*  3)  Skipping to a subsequent page.. Allocate space for the table	*/
/*	header.								*/
/*  4)  Reserve space for the top border of the cells.			*/
/*  5)  Initialize the layout of the different table columns.		*/
/*									*/
/************************************************************************/

static int docRowLayoutToNextColumn(
				BufferItem *		rowNode,
				LayoutPosition *	lpHere,
				const LayoutJob *	lj,
				int			atTopOfRow,
				BlockFrame *		bf )
    {
    const LayoutContext *	lc= &(lj->ljContext);

    int				inset= 0;
    const int			belowText= 0;
    const int			atRowTop= 0;
    LayoutPosition		lpBelowNotes;

    /*  1  */
    if  ( BF_HAS_FOOTNOTES( bf )				&&
	  ( rowNode->biTreeType == DOCinBODY		||
	    rowNode->biTreeType == DOCinENDNOTE	)		&&
	  docLayoutFootnotesForColumn( &lpBelowNotes, lpHere, bf,
							belowText, lj )	)
	{ LDEB(1); return -1;	}

    /*  2  */
    docLayoutToNextColumn( lpHere, bf, rowNode, lj );

    /*  3  */
    if  ( ! rowNode->biRowIsTableHeader		&&
	  rowNode->biRowTableHeaderRow >= 0	)
	{ docLayoutRowSkipHeaderHeight( lpHere, rowNode, atTopOfRow ); }

    /*  4  */
    docLayoutCalculateRowTopInset( &inset, lc->lcDocument, rowNode, atRowTop );
    lpHere->lpPageYTwips += inset;

    return 0;
    }

static int docRowToNextColumn(	LayoutPosition *	lpHere,
				BufferItem *		rowNode,
				const LayoutJob *	lj,
				int			atTopOfRow,
				BlockFrame *		bf,
				ParagraphLayoutJob *	rowPlj )
    {
    CellProperties *		cp;
    int				col;

    ParagraphLayoutJob *	plj;

    LayoutPosition		lpBefore;

    lpBefore= *lpHere;

    if  ( docRowLayoutToNextColumn( rowNode, lpHere, lj, atTopOfRow, bf ) )
	{ LDEB(atTopOfRow); return -1;	}

    /*  5  */
    cp= rowNode->biRowCells;
    plj= rowPlj->pljPos.pspChildren;
    for ( col= 0; col < rowNode->biChildCount; cp++, plj++, col++ )
	{
	const BufferItem *	cellNode= rowNode->biChildren[col];
	int			advanced;
	int			advanceAnyway= 0;

	if  ( docLayoutStripDone( &(plj->pljPos), plj ) )
	    { continue;	}

	if  ( cellNode->biChildren[plj->pljPos0.pspChild]->biLevel ==
								DOClevROW )
	    { continue;	}

	docCommitStripLayout( &advanced, advanceAnyway, plj,
				lpBefore.lpPage, lpBefore.lpColumn, cellNode );
	}

    return 0;
    }

/************************************************************************/
/* 									*/
/*  Calculate the layout of a nested table row.				*/
/* 									*/
/*  1)  If the row does not fit in the current page frame, the		*/
/*	the situation is hopeless: Proceed on this page: The row will	*/
/*	be split.							*/
/* 									*/
/************************************************************************/

int docLayoutRowInStrip(	int *				pStopCode,
				ParagraphLayoutPosition *	rowPlp,
				BlockFrame *			bf,
				const LayoutJob *		lj,
				int				cellTopInset,
				BufferItem *			rowNode )
    {
    const LayoutContext *	lc= &(lj->ljContext);
    int				stopCode= FORMATstopREADY;
    int				toNextColumn= 0;
    int				someAtHead= 0;

    if  ( ! rowPlp->pspChildAdvanced )
	{
	rowNode->biTopPosition= rowPlp->plpPos;

	if  ( docLayoutStartRow( &toNextColumn, rowNode,
			    lc->lcDocument, &(rowPlp->plpPos), bf, rowPlp ) )
	    { LDEB(rowNode->biChildCount); return -1;	}

	/*  1  */
	if  ( toNextColumn )
	    { /*LDEB(toNextColumn);*/ toNextColumn= 0;	}
	}

    toNextColumn= 0;
    if  ( docLayoutRowPageStrip( &(rowPlp->plpPos),
					    &toNextColumn, &someAtHead,
					    rowNode, rowPlp, bf, lj ) )
	{ LDEB(1); return -1;	}

    if  ( toNextColumn )
	{
	stopCode= FORMATstopBLOCK_FULL;
	*pStopCode= FORMATstopBLOCK_FULL;
	return stopCode;
	}
    else{
	docStripLayoutNextChild( rowPlp );
	stopCode= FORMATstopREADY;
	*pStopCode= FORMATstopREADY;

	docLayoutFinishNodeLayout( (int *)0, rowNode, lj, &(rowPlp->plpPos) );

	return stopCode;
	}
    }

/************************************************************************/
/*									*/
/*  Calculate the layout of a table row.				*/
/*									*/
/*  NOTE  Table headers that appear by themselves as the last table row	*/
/*	on the page should have been pushed to the next page. Some	*/
/*	thought reveals however that the result on the formatting of	*/
/*	the rest of the document is not altered by moving the header	*/
/*	row to the next page. For that reason, and to keep the 		*/
/*	formatter simple, we simply ignore the situation. To		*/
/*	compensate, the drawing code simply skips the row in its home	*/
/*	position and the visible result is the same.			*/
/*									*/
/*  1)  Sanity check against crashes.					*/
/*	This fix is a thourough memory leak. It is hardly ever		*/
/*	activated and it prevents crashes with incorrect rtf files.	*/
/*  2)  Allocate memory to monitor the progress of the layout of the	*/
/*	cells in the row.						*/
/*  3)  Initialize and place as much row content on the current page	*/
/*	as possible.							*/
/*  3a) As an optimization: It does not make sense to start a row that	*/
/*	has a minimum height on a page where it does not fit.		*/
/*	The test is not sufficient: Footnotes in the row can use the	*/
/*	space in such a way that on our way, we will discover that we	*/
/*	have no space left.						*/
/*  4)  Some of the content of the row did not fit on the current page.	*/
/*	Find out whether the row must be restarted on the next page.	*/
/*	(This does not make sense if the top of the row already is at	*/
/*	the top of the page.)						*/
/*  5)  When there are cells that remain empty on this page.. restart	*/
/*	on the next page.						*/
/*  6)  Go to the next page.						*/
/*  7)  If there is a reason, reinitialize the row layout job, to move	*/
/*	the whole row to the next page.					*/
/*  8)  Continue or restart (7) on the next page.			*/
/*  9)  As long as necessary, move to the next page and place content	*/
/*	there. (The exceptions above only apply for the first page, as	*/
/*	it is clearly impossible to keep the contents of a table higher	*/
/*	than one page on one page)					*/
/*									*/
/*  Measurements with MS-Word reveal the following:			*/
/*  if  trrh <  0, then the height of the row is -trrh.			*/
/*  if  trrh >= 0, then the height of the row is the maximum of the	*/
/*	height of the contents and the value of trrh. Contrary to the	*/
/*	exact height case, the width of the horizontal border is	*/
/*	added to the height of the row.					*/
/*  In the situation where two rows are on top of eachother, the	*/
/*  fattest border between them is used with a preference for the top	*/
/*  border of the bottom row if they have the same witdh.		*/
/*									*/
/************************************************************************/

int docLayoutRowNode(	int *				pStopCode,
			LayoutPosition *		lpBelow,
			const LayoutPosition *		lpTop,
			BufferItem *			rowNode,
			BlockFrame *			bf,
			int				stayInThisColumn,
			const LayoutJob *		lj )
    {
    const LayoutContext *	lc= &(lj->ljContext);
    int				stopCode= FORMATstopREADY;
    int				rval= 0;
    int				toNextColumn;
    int				someAtHead= 0;

    ParagraphLayoutJob		rowPlj;

    LayoutPosition		lpHere;

    {
    const int		line= 0;
    const int		part= 0;

    docInitParagraphLayoutJob( &rowPlj );
    docBeginParagraphLayoutProgress( &rowPlj,
				    rowNode->biNumberInParent,
				    line, part, rowNode->biNumberInParent+ 1,
				    lpTop );
    }

    lpHere= *lpTop;

    /*  1  */
    if  ( ! docIsRowNode( rowNode ) )
	{ LLDEB(rowNode->biNumberInParent,rowNode->biRowCellCount); return -1; }

    if  ( rowNode->biRowCellCount < rowNode->biChildCount )
	{
	LLDEB(rowNode->biRowCellCount,rowNode->biChildCount);
	docListNode( 0, rowNode, 0 );
	rowNode->biChildCount= rowNode->biRowCellCount;
	}

    /*  2,3  */
    toNextColumn= 0;

    /*  3a  */
    if  ( docLayoutStartRow( &toNextColumn, rowNode,
			    lc->lcDocument, &lpHere, bf, &(rowPlj.pljPos) ) )
	{ LDEB(rowNode->biChildCount); rval= -1; goto ready;	}

    if  ( toNextColumn )
	{
	if  ( stayInThisColumn )
	    { stopCode= FORMATstopFRAME_FULL; goto ready; }
	}
    else{
	if  ( docLayoutRowPageStrip( &lpHere, &toNextColumn, &someAtHead,
					rowNode, &(rowPlj.pljPos), bf, lj ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}

    /*  4  */
    if  ( toNextColumn					&&
	  ! rowNode->biTopPosition.lpAtTopOfColumn	)
	{
	int				restartRow;

	if  ( stayInThisColumn )
	    { stopCode= FORMATstopFRAME_FULL; goto ready; }

	/*  5, 7  */
	restartRow= BI_ROW_IS_ONE_PAGE( rowNode ) ||
				    ! rowPlj.pljPos.pspChildAdvanced ||
				    someAtHead;
	/*  6  */
	if  ( docRowToNextColumn( &lpHere, rowNode, lj,
						restartRow, bf, &rowPlj ) )
	    { LDEB(toNextColumn); rval= -1; goto ready;	}

	/*  7  */
	if  ( restartRow )
	    {
	    rowNode->biTopPosition= lpHere;

	    if  ( rowNode->biRowIsTableHeader		||
		  rowNode->biRowTableHeaderRow < 0	)
		{ rowNode->biRowAboveHeaderPosition= rowNode->biTopPosition; }

	    if  ( docLayoutStartRowCells( rowNode, lc->lcDocument,
					    &lpHere, bf, &(rowPlj.pljPos) ) )
		{ LDEB(rowNode->biChildCount); rval= -1; goto ready;	}
	    toNextColumn= 0; /* irrelevant here */
	    }

	/*  8  */
	if  ( docLayoutRowPageStrip( &lpHere, &toNextColumn, &someAtHead,
					rowNode, &(rowPlj.pljPos), bf, lj ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}

    /*  9  */
    while( toNextColumn )
	{
	const int	atTopOfRow= 0;

	if  ( stayInThisColumn )
	    { stopCode= FORMATstopFRAME_FULL; goto ready; }

	if  ( docRowToNextColumn( &lpHere, rowNode, lj,
						atTopOfRow, bf, &rowPlj ) )
	    { LDEB(toNextColumn); rval= -1; goto ready;	}

	if  ( docLayoutRowPageStrip( &lpHere, &toNextColumn, &someAtHead,
					rowNode, &(rowPlj.pljPos), bf, lj ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}

  ready:

    docCleanParagraphLayoutJob( &rowPlj );

    if  ( ! rval )
	{
	*pStopCode= stopCode;
	*lpBelow= lpHere;
	}

    return rval;
    }

