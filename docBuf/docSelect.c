/************************************************************************/
/*									*/
/*  Get/Move/Set Selections.						*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	<appDebugon.h>

#   include	"docBuf.h"
#   include	"docNodeTree.h"
#   include	"docDebug.h"
#   include	"docParaString.h"
#   include	"docParaParticules.h"

/************************************************************************/
/*									*/
/*  Move to the first/last position.					*/
/*									*/
/************************************************************************/

int docGotoFirstPosition(	DocumentPosition *	dp,
				BufferItem *		node )
    {
    while( node )
	{
	if  ( node->biLevel == DOClevPARA )
	    {
	    docSetDocumentPosition( dp, node, docParaFirstStroff( node ) );
	    return 0;
	    }

	if  ( node->biChildCount == 0 )
	    { /*LDEB(node->biChildCount);*/ return -1;	}

	node= node->biChildren[0];
	}

    /*XDEB(node);*/ return -1;
    }

int docHeadPosition(	DocumentPosition *	dp,
			BufferItem *		node )
    {
    while( node )
	{
	if  ( node->biLevel == DOClevPARA )
	    {
	    docSetDocumentPosition( dp, node, 0 );
	    return 0;
	    }

	if  ( node->biChildCount == 0 )
	    { /*LDEB(node->biChildCount);*/ return -1;	}

	node= node->biChildren[0];
	}

    /*XDEB(node);*/ return -1;
    }

int docGotoLastPosition(	DocumentPosition *	dp,
				BufferItem *		node )
    {
    while( node )
	{
	if  ( node->biLevel == DOClevPARA )
	    {
	    docSetDocumentPosition( dp, node, docParaLastStroff( node ) );
	    return 0;
	    }

	if  ( node->biChildCount == 0 )
	    { /*LDEB(node->biChildCount);*/ return -1;	}

	node= node->biChildren[node->biChildCount- 1];
	}

    /*XDEB(node);*/ return -1;
    }

int docTailPosition(	DocumentPosition *	dp,
			BufferItem *		node )
    {
    while( node )
	{
	if  ( node->biLevel == DOClevPARA )
	    {
	    docSetDocumentPosition( dp, node, docParaStrlen( node ) );
	    return 0;
	    }

	if  ( node->biChildCount == 0 )
	    { /*LDEB(node->biChildCount);*/ return -1;	}

	node= node->biChildren[node->biChildCount- 1];
	}

    /*XDEB(node);*/ return -1;
    }

int docDocumentHead(	DocumentPosition *	dp,
			BufferDocument *	bd )
    {
    if  ( docHeadPosition( dp, bd->bdBody.dtRoot ) )
	{ return -1;	}

    return 0;
    }

int docDocumentTail(	DocumentPosition *	dp,
				BufferDocument *	bd )
    {
    if  ( docTailPosition( dp, bd->bdBody.dtRoot ) )
	{ return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Keep out the read-only field at the Head of certain kinds of	*/
/*  paragraphs.								*/
/*									*/
/************************************************************************/

void docAvoidParaHeadField(	DocumentPosition *	dp,
				int *			pPart,
				const BufferDocument *	bd )
    {
    DocumentField *	dfHead= (DocumentField *)0;
    DocumentSelection	dsInsideHead;
    DocumentSelection	dsAroundHead;
    int			partBegin= -1;
    int			partEnd= -1;

    int	fieldKind= docParaHeadFieldKind( dp->dpNode, bd );
    if  ( fieldKind < 0 )
	{ return;	}

    if  ( docDelimitParaHeadField( &dfHead, &dsInsideHead, &dsAroundHead,
					&partBegin, &partEnd, dp->dpNode, bd ) )
	{ LDEB(1);	}
    else{
	if  ( dp->dpStroff < dsAroundHead.dsTail.dpStroff )
	    { dp->dpStroff=  dsAroundHead.dsTail.dpStroff;	}

	if  ( pPart && *pPart <= partBegin )
	    { *pPart= partBegin+ 1;	}
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Move to the next/previous position.					*/
/*									*/
/************************************************************************/

# define docIsMergedCell( node ) ( \
    (node)->biParent && \
    docIsRowNode( (node)->biParent ) && \
    (node)->biNumberInParent < (node)->biParent->biRowCellCount && \
    CELL_MERGED( &( (node)->biParent->biRowCells[(node)->biNumberInParent] ) ) )

static BufferItem * docNextNode(	BufferItem *	node,
					int		level )
    {
    for (;;)
	{
	if  ( ! node->biParent )
	    { return (BufferItem *)0;	}

	if  ( node->biNumberInParent < node->biParent->biChildCount- 1 )
	    {
	    node= node->biParent->biChildren[node->biNumberInParent+ 1];

	    if  ( docIsMergedCell( node ) )
		{ continue;	}

	    while( node->biLevel < level	&&
		   node->biChildCount > 0	)
		{
		node= node->biChildren[0];

		if  ( docIsMergedCell( node ) )
		    { break; }
		}

	    if  ( docIsMergedCell( node ) )
		{ continue; }

	    if  ( node->biLevel == level )
		{ return node;	}
	    }
	else{ node= node->biParent;	}
	}
    }

BufferItem *	docNextParagraph(	BufferItem *	node )
    { return docNextNode( node, DOClevPARA );	}

BufferItem *	docNextSection(	BufferItem *	node )
    { return docNextNode( node, DOClevSECT );	}

static BufferItem * docPrevNode(	BufferItem *	node,
					int		level )
    {
    for (;;)
	{
	if  ( ! node->biParent )
	    { return (BufferItem *)0;	}

	if  ( node->biNumberInParent > 0 )
	    {
	    node= node->biParent->biChildren[node->biNumberInParent- 1];

	    if  ( docIsMergedCell( node ) )
		{ continue;	}

	    while( node->biLevel < level	&&
		   node->biChildCount > 0	)
		{
		node= node->biChildren[node->biChildCount- 1];

		if  ( docIsMergedCell( node ) )
		    { break; }
		}

	    if  ( docIsMergedCell( node ) )
		{ continue; }

	    if  ( node->biLevel == level )
		{ return node;	}
	    }
	else{ node= node->biParent;	}
	}
    }

BufferItem *	docPrevParagraph(	BufferItem *	node )
    { return docPrevNode( node, DOClevPARA );	}

BufferItem *	docPrevSection(		BufferItem *	node )
    { return docPrevNode( node, DOClevSECT );	}

/************************************************************************/
/*									*/
/*  Move to the Next/Previous word.					*/
/*									*/
/*  1)  Words start after a particule ending in a space or at the	*/
/*	beginning of a paragraph.					*/
/*									*/
/************************************************************************/

int docNextWord(	DocumentPosition *	dp )
    {
    BufferItem *		paraBi= dp->dpNode;
    int				stroff= dp->dpStroff;

    if  ( stroff == docParaStrlen( paraBi ) )
	{
	paraBi= docNextParagraph( paraBi );
	if  ( ! paraBi )
	    { return 1;	}

	stroff= 0;
	}

    stroff= docParaNextWord( paraBi, stroff );
    docSetDocumentPosition( dp, paraBi, stroff );

    return 0;
    }

int docPrevWord(	DocumentPosition *	dp )
    {
    BufferItem *		paraBi= dp->dpNode;
    int				stroff= dp->dpStroff;

    if  ( stroff == 0 )
	{
	paraBi= docPrevParagraph( paraBi );
	if  ( ! paraBi )
	    { return 1;	}

	stroff= docParaStrlen( paraBi );
	}

    stroff= docParaPrevWord( paraBi, stroff );
    docSetDocumentPosition( dp, paraBi, stroff );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Move one line up.							*/
/*									*/
/*  1)  Go to previous line of the paragraph or to the last line of the	*/
/*	last paragraph before this one that has any lines.		*/
/*  2)  In a clumsy attempt to implement vertical movement in tables,	*/
/*	try to stay in the same column of a table.			*/
/*									*/
/************************************************************************/

int docLineUp(		int *			pLine,
			DocumentPosition *	dp,
			int			line )
    {
    TextLine *		tl;
    BufferItem *	node= dp->dpNode;

    line--;

    /*  1  */
    while( node )
	{
	if  ( node->biParaLineCount == 0 )
	    { LLDEB(docNumberOfParagraph(node),node->biParaLineCount);	}

	if  ( node->biLevel == DOClevPARA		&&
	      line < node->biParaLineCount	&& /* against crashes */
	      line >= 0				)
	    {
	    tl= node->biParaLines+ line;

	    dp->dpNode= node;
	    dp->dpStroff= tl->tlStroff;

	    if  ( pLine )
		{ *pLine= line;	}
	    return 0;
	    }

	/*  2  */
	if  ( node->biLevel == DOClevPARA		&&
	      node->biParaTableNesting > 0	&&
	      node->biNumberInParent == 0		)
	    {
	    int			col;
	    int			row0;
	    int			row;
	    int			row1;

	    BufferItem *	parentNode;

	    if  ( docDelimitTable( node, &parentNode, &col, &row0, &row, &row1 ) )
		{ LDEB(1); return -1;	}

	    if  ( row > row0 )
		{
		BufferItem *	rowNode= parentNode->biChildren[row-1];

		if  ( col < rowNode->biChildCount )
		    {
		    BufferItem *	cellNode= rowNode->biChildren[col];

		    node= cellNode->biChildren[cellNode->biChildCount-1];
		    line= node->biParaLineCount- 1;
		    continue;
		    }
		}
	    }

	node= docPrevParagraph( node );
	if  ( ! node )
	    { break;	}
	line= node->biParaLineCount- 1;
	}

    return -1;
    }

/************************************************************************/
/*									*/
/*  Move one line down.							*/
/*									*/
/*  1)  Go to the next line of the paragraph or to the first line of	*/
/*	the first paragraph after this one that has any lines.		*/
/*  2)  In a clumsy attempt to implement vertical movement in tables,	*/
/*	try to stay in the same column of a table.			*/
/*									*/
/************************************************************************/

int docLineDown(	int *			pLine,
			DocumentPosition *	dp,
			int			line )
    {
    TextLine *		tl;
    BufferItem *	node= dp->dpNode;

    line++;

    /*  1  */
    while( node )
	{
	if  ( node->biLevel == DOClevPARA		&&
	      line < node->biParaLineCount	&&
	      line >= 0				)   /*  against crashes  */
	    {
	    tl= node->biParaLines+ line;

	    dp->dpNode= node;
	    dp->dpStroff= tl->tlStroff;

	    if  ( pLine )
		{ *pLine= line;	}
	    return 0;
	    }

	/*  2  */
	if  ( node->biLevel == DOClevPARA					&&
	      node->biParaTableNesting > 0				&&
	      node->biNumberInParent == node->biParent->biChildCount- 1	)
	    {
	    int			col;
	    int			row0;
	    int			row;
	    int			row1;

	    BufferItem *	parentNode;

	    if  ( docDelimitTable( node, &parentNode, &col, &row0, &row, &row1 ) )
		{ LDEB(1); return -1;	}

	    if  ( row < row1 )
		{
		BufferItem *	rowNode= parentNode->biChildren[row+1];

		if  ( col < rowNode->biChildCount )
		    {
		    BufferItem *	cellNode= rowNode->biChildren[col];

		    node= cellNode->biChildren[0];
		    line= 0;
		    continue;
		    }
		}
	    }

	node= docNextParagraph( node );
	line= 0;
	}

    return -1;
    }

/************************************************************************/
/*									*/
/*  Move to the beginning/end of a line					*/
/*									*/
/************************************************************************/

int docLineHead(	DocumentPosition *	dp,
			int			positionFlags )
    {
    int			line;
    int			flags= 0;
    const int		lastOne= PARAfindFIRST;

    if  ( positionFlags & POSflagLINE_HEAD )
	{ return 0;	}

    if  ( docFindLineOfPosition( &line, &flags, dp, lastOne ) )
	{ LDEB(dp->dpStroff); return -1;	}

    dp->dpStroff= dp->dpNode->biParaLines[line].tlStroff;

    return 0;
    }

int docLineTail(	DocumentPosition *	dp,
			int			positionFlags )
    {
    int			line;
    int			flags= 0;
    const int		lastOne= PARAfindLAST;
    const TextLine *	tl;

    if  ( positionFlags & POSflagLINE_TAIL )
	{ return 0;	}

    if  ( docFindLineOfPosition( &line, &flags, dp, lastOne ) )
	{ LDEB(dp->dpStroff); return -1;	}

    tl= &(dp->dpNode->biParaLines[line]);
    dp->dpStroff= tl->tlStroff+ tl->tlStrlen;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Get buffer positions for a text line.				*/
/*									*/
/************************************************************************/

void docLineSelection(	DocumentSelection *	dsLine,
			int *			pPartLineBegin,
			int *			pPartLineEnd,
			const BufferItem *	node,
			int			line )
    {
    const TextLine *	tl= node->biParaLines+ line;

    if  ( node->biLevel != DOClevPARA )
	{ LLDEB(node->biLevel,DOClevPARA); return;	}
    if  ( line < 0 || line >= node->biParaLineCount )
	{ LLDEB(line,node->biParaLineCount); return;	}

    docInitDocumentSelection( dsLine );

    dsLine->dsHead.dpNode= (BufferItem *)node;
    dsLine->dsHead.dpStroff= tl->tlStroff;

    dsLine->dsTail.dpNode= (BufferItem *)node;
    dsLine->dsTail.dpStroff= tl->tlStroff+ tl->tlStrlen;

    dsLine->dsAnchor= dsLine->dsHead;
    dsLine->dsDirection= 1;

    dsLine->dsCol0= dsLine->dsCol1= -1;

    docSetSelectionScope( dsLine );

    *pPartLineBegin= tl->tlFirstParticule;
    *pPartLineEnd= tl->tlFirstParticule+ tl->tlParticuleCount- 1;

    return;
    }

/************************************************************************/
/*									*/
/*  Delimit a 'Word' in the document.					*/
/*									*/
/*  A 'Word' is one of the following:					*/
/*  1)	A continguous stretch of text. It is not interrupted by either	*/
/*	white space or control particules. Any white space after the	*/
/*	word is included in the word.					*/
/*  2)	An object or an image.						*/
/*									*/
/************************************************************************/

void docWordSelection(	DocumentSelection *		dsWord,
			int *				pIsObject,
			const DocumentPosition *	dpAround )
    {
    TextParticule *	tp;

    BufferItem *	paraBi= dpAround->dpNode;
    int			part;

    if  ( paraBi->biLevel != DOClevPARA )
	{ LLDEB(paraBi->biLevel,DOClevPARA); return;	}

    if  ( docFindParticuleOfPosition( &part, (int *)0,
						dpAround, PARAfindLAST ) )
	{ LDEB(dpAround->dpStroff); return;	}

    tp= paraBi->biParaParticules+ part;
    while( part < paraBi->biParaParticuleCount- 1		&&
	   tp->tpStroff+ tp->tpStrlen <= dpAround->dpStroff	)
	{ tp++; part++;	}

    if  ( tp->tpStroff == dpAround->dpStroff	&&
	  part > 0				&&
	  tp[-1].tpKind == DOCkindOBJECT	)
	{
	docSetParaSelection( dsWord, paraBi, 1,
					tp[-1].tpStroff, tp[-1].tpStrlen );
	*pIsObject= 1;
	return;
	}

    if  ( tp->tpKind == DOCkindOBJECT )
	{
	docSetParaSelection( dsWord, paraBi, 1, tp->tpStroff, tp->tpStrlen );
	*pIsObject= 1;
	return;
	}
    else{
	int	stroffHead;
	int	stroffTail;

	docParaHeadOfWord( &stroffHead, paraBi, part );
	docParaTailOfWord( &stroffTail, paraBi, part );

	docSetParaSelection( dsWord, paraBi, 1,
					stroffHead, stroffTail- stroffHead );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Translate a TableRectangle to a DocumentSelection.			*/
/*									*/
/************************************************************************/

int docTableRectangleSelection(	DocumentSelection *	ds,
				BufferItem **		pSelParentBi,
				BufferDocument *	bd,
				const TableRectangle *	tr )
    {
    DocumentSelection	dsNew;

    BufferItem *	selSectBi;
    BufferItem *	selParentBi;
    BufferItem *	rowNode;
    BufferItem *	cellNode;

    docInitDocumentSelection( &dsNew );

    /*******/

    selParentBi= docGetSelectionRoot( (DocumentTree **)0, (BufferItem **)0,
								    bd, ds );
    if  ( ! selParentBi )
	{ XDEB(selParentBi); return -1;	}

    if  ( selParentBi->biLevel == DOClevPARA )
	{ selParentBi= selParentBi->biParent;	}
    if  ( selParentBi->biLevel == DOClevCELL )
	{ selParentBi= selParentBi->biParent;	}
    if  ( selParentBi->biLevel == DOClevROW )
	{ selParentBi= selParentBi->biParent;	}

    selSectBi= docGetSectNode( selParentBi );
    if  ( ! selSectBi )
	{ XDEB(selSectBi); return -1;	}

    /*******/

    if  ( tr->trRow0 < 0					||
	  tr->trRow0 >= selParentBi->biChildCount		)
	{ LLDEB(tr->trRow0,selParentBi->biChildCount); return -1;	}
    rowNode= selParentBi->biChildren[tr->trRow0];

    if  ( tr->trCol0 < 0			||
	  tr->trCol0 >= rowNode->biChildCount	)
	{ LLDEB(tr->trCol0,rowNode->biChildCount); return -1;	}
    cellNode= rowNode->biChildren[tr->trCol0];

    if  ( docHeadPosition( &dsNew.dsHead, cellNode ) )
	{ LDEB(0); return -1;	}

    /*******/

    if  ( tr->trRow1 < 0				||
	  tr->trRow1 >= selParentBi->biChildCount	)
	{ LLDEB(tr->trRow0,selParentBi->biChildCount); return -1;	}
    rowNode= selParentBi->biChildren[tr->trRow1];

    if  ( tr->trCol1 < 0			||
	  tr->trCol1 >= rowNode->biChildCount	)
	{ LLDEB(tr->trCol1,rowNode->biChildCount); return -1;	}
    cellNode= rowNode->biChildren[tr->trCol1];

    if  ( docTailPosition( &dsNew.dsTail, cellNode ) )
	{ LDEB(0); return -1;	}

    /*******/

    dsNew.dsCol0= tr->trCol0;
    dsNew.dsCol1= tr->trCol1;
    dsNew.dsDirection= 1;
    dsNew.dsAnchor= dsNew.dsHead;

    if  ( tr->trRow1 < tr->trRow0					||
	  ( tr->trRow1 == tr->trRow0 && tr->trCol1 < tr->trCol0 )	)
	{
	dsNew.dsDirection= -1;
	dsNew.dsAnchor= dsNew.dsTail;
	}

    docSetSelectionScope( &dsNew );

    *ds= dsNew;
    *pSelParentBi= selParentBi;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Is the selection exactly an object?					*/
/*									*/
/************************************************************************/

int docGetObjectSelection(	const DocumentSelection *	ds,
				const BufferDocument *		bd,
				int *				pPart,
				DocumentPosition *		dpObject,
				InsertedObject **		pIo )
    {
    if  ( ds->dsHead.dpNode				&&
	  ds->dsTail.dpNode == ds->dsHead.dpNode	)
	{
	BufferItem *		paraBi= ds->dsHead.dpNode;
	int			part;
	TextParticule *		tp;

	if  ( docFindParticuleOfPosition( &part, (int *)0,
						&(ds->dsHead), PARAfindLAST ) )
	    { LDEB(ds->dsHead.dpStroff); return -1;	}

	tp= paraBi->biParaParticules+ part;

	if  ( tp->tpKind == DOCkindOBJECT			&&
	      ds->dsHead.dpStroff == tp->tpStroff		&&
	      ds->dsTail.dpStroff == tp->tpStroff+ tp->tpStrlen	)
	    {
	    *pPart= part;
	    *dpObject= ds->dsHead;
	    *pIo= docGetObject( bd, tp->tpObjectNumber );

	    return 0;
	    }
	}

    return 1;
    }

/************************************************************************/
/*									*/
/*  Select the whole frame around a position. Make no difficulties if	*/
/*  the position is not in a frame: simply select the neghbourhood that	*/
/*  is not a frame. (Usually the whole cell.)				*/
/*									*/
/************************************************************************/

int docSelectFrameOfPosition(	DocumentSelection *		ds,
				const DocumentPosition *	dp )
    {
    BufferItem *	cellNode;
    int			para0;
    int			para1;
    int			frameNumber;

    DocumentPosition	dpHead;
    DocumentPosition	dpTail;

    const int		direction= 1;

    docInitDocumentPosition( &dpHead );
    docInitDocumentPosition( &dpTail );

    if  ( ! dp->dpNode )
	{ XDEB(dp->dpNode); return 1;	}

    frameNumber= dp->dpNode->biParaFrameNumber;
    cellNode= dp->dpNode->biParent;
    para0= para1= dp->dpNode->biNumberInParent;

    while( para0 > 0 )
	{
	if  ( cellNode->biChildren[para0- 1]->biParaFrameNumber != frameNumber )
	    { break;	}
	para0--;
	}

    while( para1 < cellNode->biChildCount- 1 )
	{
	if  ( cellNode->biChildren[para1+ 1]->biParaFrameNumber != frameNumber )
	    { break;	}
	para1++;
	}

    if  ( docHeadPosition( &dpHead, cellNode->biChildren[para0] ) )
	{ LDEB(1); return 1;	}
    if  ( docTailPosition( &dpTail, cellNode->biChildren[para1] ) )
	{ LDEB(1); return 1;	}

    docSetRangeSelection( ds, &dpHead, &dpTail, direction );

    return 0;
    }

