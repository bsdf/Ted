/************************************************************************/
/*									*/
/*  Delimit tables and table rectangles.				*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	<appDebugon.h>

#   include	"docBuf.h"
#   include	"docTreeNode.h"
#   include	"docNodeTree.h"

/************************************************************************/
/*									*/
/*  Find the range of rows for a 'table'				*/
/*									*/
/************************************************************************/

int docDelimitTable(	const BufferItem *	paraNode,
			BufferItem **		pParentNode,
			int *			pCol,
			int *			pRow0,
			int *			pRow,
			int *			pRow1 )
    {
    BufferItem *	rowNode;
    BufferItem *	parentNode;
    int			col;

    if  ( paraNode->biLevel != DOClevPARA	||
	  paraNode->biParaTableNesting == 0	)
	{ /* LLDEB(paraNode->biLevel,paraNode->biParaInTable); */ return -1; }

    rowNode= paraNode->biParent->biParent;
    parentNode= rowNode->biParent;
    col= paraNode->biParent->biNumberInParent;

    if  ( pParentNode )
	{ *pParentNode= parentNode;		}
    if  ( pCol )
	{ *pCol= col;				}
    if  ( pRow0 )
	{ *pRow0= rowNode->biRowTableFirst;	}
    if  ( pRow )
	{ *pRow= rowNode->biNumberInParent;	}
    if  ( pRow1 )
	{ *pRow1= rowNode->biRowTablePast- 1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Determine the spans of a table cell.				*/
/*									*/
/************************************************************************/

int docGetCellBottom(		LayoutPosition *	lpBottom,
				const BufferItem *	cellNode )
    {
    const BufferItem *		rowNode= cellNode->biParent;
    int				col= cellNode->biNumberInParent;
    const BufferItem *		parentNode= rowNode->biParent;

    int				row0= rowNode->biNumberInParent;
    int				rowp= rowNode->biRowTablePast;
    const CellProperties *	cp= &(rowNode->biRowCells[col]);

    const LayoutPosition *	lpBelow= &(rowNode->biBelowPosition);

    int				rowspan= 1;

    if  ( cp->cpVerticalMerge == CELLmergeFOLLOW )
	{ rowspan= 0;	}
    else{
	if  ( cp->cpVerticalMerge == CELLmergeHEAD )
	    {
	    int		nRow;
	    int		csp= 1;
	    int		l, r;

	    if  ( col == 0 )
		{ l= rowNode->biRowLeftIndentTwips;	}
	    else{ l= cp[-1].cpRightBoundaryTwips;	}
	    r= docGetCellRight( &csp, cellNode );

	    for ( nRow= row0+ 1; nRow < rowp; nRow++ )
		{
		int			ncsp= 1;
		const BufferItem *	nRowBi= parentNode->biChildren[nRow];
		int			nCol;

		nCol= docGetMatchingCell( &ncsp, nRowBi, l, r );
		if  ( nCol < 0 )
		    { break;	}

		if  ( nRowBi->biRowCells[nCol].cpVerticalMerge !=
							    CELLmergeFOLLOW )
		    { break;	}

		lpBelow= &(nRowBi->biBelowPosition);
		rowspan++;
		}
	    }
	}

    if  ( lpBottom )
	{ *lpBottom= *lpBelow;	}

    return rowspan;
    }

void docTableDetermineCellspans(	int *			pRowspan,
					int *			pColspan,
					const BufferItem *	cellNode )
    {
    const BufferItem *		rowNode= cellNode->biParent;
    int				col= cellNode->biNumberInParent;

    const CellProperties *	cp= &(rowNode->biRowCells[col]);

    int				colspan= 1;
    int				rowspan= 1;

    if  ( cp->cpHorizontalMerge == CELLmergeFOLLOW )
	{ colspan= 0;	}
    else{
	if  ( cp->cpHorizontalMerge == CELLmergeHEAD )
	    { /* right= */ docGetCellRight( &colspan, cellNode );	}
	}

    rowspan= docGetCellBottom( (LayoutPosition *)0, cellNode );

    if  ( pRowspan )
	{ *pRowspan= rowspan;	}
    if  ( pColspan )
	{ *pColspan= colspan;	}

    return;
    }

/************************************************************************/
/*									*/
/*  Determine the rectangular area as it is selected in a table.	*/
/*									*/
/*  This funtion fails if the whole selection is not inside ONE table.	*/
/*									*/
/*  1)  Find the table environment of the beginning of the selection.	*/
/*  2)  Find the table environment of the end of the selection.		*/
/*  3)  Different section -> Different table.				*/
/*  4)  Different begin or different end -> Different table.		*/
/*  5)  Paranoia checks.						*/
/*  6)  Delimit rectangle.						*/
/*  7)  If the selection is inside one cell determine its		*/
/*	colspan/rowspan.						*/
/*									*/
/************************************************************************/

int docGetTableRectangle(	TableRectangle *		tr,
				const DocumentSelection *	ds )
    {
    BufferItem *	parentNode0;
    BufferItem *	parentNode1;

    TableRectangle	trHead;
    TableRectangle	trTail;

    /*  1  */
    if  ( docDelimitTable( ds->dsHead.dpNode, &parentNode0,
						&(trHead.trCol0),
						&(trHead.trRow00),
						&(trHead.trRow0),
						&(trHead.trRow11) ) )
	{ /* LDEB(1); */ return 1;	}

    /*  2  */
    if  ( docDelimitTable( ds->dsTail.dpNode, &parentNode1,
						&(trTail.trCol0),
						&(trTail.trRow00),
						&(trTail.trRow0),
						&(trTail.trRow11) ) )
	{ /* LDEB(1); */ return 1;	}

    /*  3  */
    if  ( parentNode0 != parentNode1 )
	{ /* XXDEB(parentNode0,parentNode1); */ return 1;	}

    /*  4  */
    if  ( trHead.trRow00 != trTail.trRow00	||
	  trHead.trRow11 != trTail.trRow11	)
	{ return 1;	}

    /*  5  */
    if  ( trHead.trCol0 > trTail.trCol0 )
	{ return 1;	}
    if  ( trHead.trRow0 > trTail.trRow0 )
	{ return 1;	}

    /*  6  */
    tr->trCol0= trHead.trCol0;
    tr->trCol1= trTail.trCol0;
    tr->trCol11= parentNode0->biChildren[trHead.trRow0]->biChildCount- 1;

    tr->trRow00= trHead.trRow00;
    tr->trRow0= trHead.trRow0;
    tr->trRow1= trTail.trRow0;
    tr->trRow11= trHead.trRow11;

    tr->trCellColspan= 0;
    tr->trCellRowspan= 0;

    tr->trIsSingleCell= (	tr->trRow0 == tr->trRow1	&&
				tr->trCol0 == tr->trCol1	);

    tr->trIsColSlice= (		tr->trRow0 == tr->trRow00	&&
    				tr->trRow1 == tr->trRow11	);

    tr->trIsRowSlice= (		tr->trCol0 == 0			&&
				tr->trCol1 == tr->trCol11	);

    tr->trIsTableSlice= tr->trIsColSlice || tr->trIsRowSlice;
    tr->trIsWholeTable= tr->trIsColSlice && tr->trIsRowSlice;

    /*  7  */
    if  ( tr->trIsSingleCell )
	{
	const BufferItem *	rowNode0= parentNode0->biChildren[trHead.trRow0];
	const BufferItem *	cellNode0= rowNode0->biChildren[tr->trCol0];

	docTableDetermineCellspans(
			&(tr->trCellRowspan), &(tr->trCellColspan), cellNode0 );
	}

    tr->trInTableHeader=
		parentNode0->biChildren[trHead.trRow0]->biRowIsTableHeader;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Delimit the tables in a section.					*/
/*									*/
/************************************************************************/

void docDelimitTables(	BufferItem *		parentNode,
			int			recursively )
    {
    int		n;
    int		headerRow= -1;

    n= 0;
    while( n < parentNode->biChildCount )
	{
	BufferItem *	rowNodeFirst;
	BufferItem *	rowNode;

	int		first;
	int		past;

	rowNodeFirst= parentNode->biChildren[n];
	if  ( ! docIsRowNode( rowNodeFirst ) )
	    {
	    if  ( recursively && rowNodeFirst->biLevel != DOClevPARA )
		{ docDelimitTables( rowNodeFirst, recursively );	}

	    if  ( rowNodeFirst->biLevel == DOClevROW )
		{
		rowNodeFirst->biRowTableHeaderRow= -1;
		rowNodeFirst->biRowTableFirst= -1;
		rowNodeFirst->biRowTablePast= -1;
		}

	    headerRow= -1;
	    n++; continue;
	    }

	first= n;
	past= n+ 1;

	while( past < parentNode->biChildCount )
	    {
	    rowNode= parentNode->biChildren[past];

	    if  ( ! docIsRowNode( rowNode ) )
		{ break;	}

	    /*
	    if  ( ! docApproximatelyAlignedColumns(
					&(rowNodeFirst->biRowProperties),
					&(rowNode->biRowProperties) )	)
		{ break;	}
	    */

	    past++; 
	    }

	while( n < past )
	    {
	    rowNode= parentNode->biChildren[n];

	    if  ( recursively && rowNode->biLevel != DOClevPARA )
		{ docDelimitTables( rowNode, recursively );	}

	    if  ( rowNode->biRowIsTableHeader )
		{ headerRow= n;	}

	    rowNode->biRowTableHeaderRow= headerRow;
	    rowNode->biRowTableFirst= first;
	    rowNode->biRowTablePast= past;

	    n++;
	    }
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Get colspan and right border of a cell.				*/
/*									*/
/************************************************************************/

int docGetCellRight(	int *			pColspan,
			const BufferItem *	cellNode )
    {
    const BufferItem *	rowNode= cellNode->biParent;
    int			col= cellNode->biNumberInParent;

    return docCellRight( pColspan, &(rowNode->biRowProperties), col );
    }

/************************************************************************/
/*									*/
/*  Find the cell with left and right borders as given.			*/
/*									*/
/************************************************************************/

int docGetMatchingCell(		int *			pColspan,
				const BufferItem *	rowNode,
				int			l,
				int			r )
    {
    int				col;
    int				ll, rr;
    const CellProperties *	cp;

    if  ( rowNode->biLevel != DOClevROW )
	{ SDEB(docLevelStr(rowNode->biLevel)); return -1;	}

    /*  2  */
    rr= rowNode->biRowLeftIndentTwips;
    cp= rowNode->biRowCells;
    for ( col= 0; col < rowNode->biChildCount; cp++, col++ )
	{
	int		colspan= 1;

	if  ( cp->cpHorizontalMerge == CELLmergeFOLLOW )
	    { continue;	}

	ll= rr; rr= cp->cpRightBoundaryTwips;
	if  ( cp->cpHorizontalMerge == CELLmergeHEAD )
	    { rr= docGetCellRight( &colspan, rowNode->biChildren[col] ); }

	if  ( ll == l && rr == r )
	    { *pColspan= colspan; return col;	}
	}

    return -1;
    }

/************************************************************************/
/*									*/
/*  Get the cell and row properties for a node.				*/
/*									*/
/************************************************************************/

const CellProperties *	docGetCellProperties(
					const RowProperties **	pRp,
					const BufferItem *	cellNode )
    {
    const BufferItem *		rowNode= cellNode->biParent;
    const RowProperties *	rp= &(rowNode->biRowProperties);
    int				col= cellNode->biNumberInParent;
    const CellProperties *	cp= &(rp->rpCells[col]);

    if  ( ! docIsRowNode( rowNode ) )
	{
	SSDEB(docLevelStr(rowNode->biLevel),docLevelStr(cellNode->biLevel));
	return (const CellProperties *)0;
	}

    if  ( pRp )
	{ *pRp= rp;	}

    return cp;
    }

void docAvoidMergedTail(	DocumentSelection *	dsNew,
				EditRange *		er )
    {
    BufferItem *	cellNode= docGetCellNode( dsNew->dsTail.dpNode );

    if  ( cellNode )
	{
	const RowProperties *	rp;
	const CellProperties *	cp;

	cp= docGetCellProperties( &rp, cellNode );
	if  ( cp && CELL_MERGED( cp ) )
	    {
	    cellNode= docGetCellNode( dsNew->dsHead.dpNode );

	    if  ( docSetNodeSelection( dsNew, cellNode ) )
		{ LDEB(1);				}
	    else{ docSetEditRange( er, dsNew );	}
	    }
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Save cell properties that are representative for the cells in a	*/
/*  table rectangle. Typically, that is the properties of the		*/
/*  upper-left cell. For a bigger rectangle, the borders are adapted	*/
/*  to those on the outside of the rectangle.				*/
/*									*/
/************************************************************************/

void docSetCellRectangleProperties(	CellProperties *		cpSet,
					const BufferItem *		rowNode,
					const TableRectangle *		tr,
					const DocumentAttributeMap *	dam )
    {
    const RowProperties *	rp= &(rowNode->biRowProperties);

    if  ( docCopyCellProperties( cpSet, &(rp->rpCells[tr->trCol0]), dam ) )
	{ LDEB(1); return;	}

    if  ( tr->trCol1 > tr->trCol0 )
	{
	int	fromNumber= rp->rpCells[tr->trCol1].cpRightBorderNumber;

	if  ( fromNumber >= 0 && dam && dam->damBorderMap )
	    { fromNumber= dam->damBorderMap[fromNumber];	}

	cpSet->cpRightBorderNumber= fromNumber;
	}

    if  ( tr->trRow1 > tr->trRow0 )
	{
	const RowProperties *	rp1;
	int 			fromNumber;

	rp1= &(rowNode->biParent->biChildren[tr->trRow1]->biRowProperties);

	fromNumber= rp1->rpCells[tr->trCol0].cpBottomBorderNumber;
	if  ( fromNumber >= 0 && dam && dam->damBorderMap )
	    { fromNumber= dam->damBorderMap[fromNumber];	}

	cpSet->cpBottomBorderNumber= fromNumber;
	}

    return;
    }

