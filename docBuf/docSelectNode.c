/************************************************************************/
/*									*/
/*  Get/Move/Set Selections.						*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	<appDebugon.h>

#   include	"docBuf.h"
#   include	"docTreeNode.h"
#   include	"docNodeTree.h"

/************************************************************************/
/*									*/
/*  Implementation of a 'Select All' menu option.			*/
/*  Extend selection to whole document tree.				*/
/*									*/
/************************************************************************/

int docSelectWholeBody(		DocumentSelection *	ds,
				BufferDocument *	bd )
    {
    BufferItem *		selRootNode= (BufferItem *)0;

    if  ( ds->dsHead.dpNode && ds->dsTail.dpNode )
	{
	selRootNode= docGetSelectionRoot( (DocumentTree **)0,
					    (BufferItem **)0, bd, ds );
	}

    if  ( ! selRootNode )
	{ selRootNode= bd->bdBody.dtRoot;	}

    while( selRootNode && selRootNode->biParent )
	{ selRootNode= selRootNode->biParent; }
    if  ( ! selRootNode )
	{ XDEB(selRootNode); return -1;	}

    if  ( docHeadPosition( &(ds->dsHead), selRootNode ) )
	{ LDEB(1); return -1;	}
    ds->dsHead.dpStroff= 0;
    if  ( docTailPosition( &(ds->dsTail), selRootNode ) )
	{ LDEB(1); return -1;	}
    ds->dsTail.dpStroff= docParaStrlen( ds->dsTail.dpNode );

    ds->dsAnchor= ds->dsHead;
    ds->dsDirection= 1;

    docSetSelectionScope( ds );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Change the selection to cover whole sections.			*/
/*									*/
/*  Depending on the 'direction' argument:				*/
/*	<  0:	The previous one.					*/
/*	== 0:	The current one.					*/
/*	>  0:	The next one.						*/
/*									*/
/************************************************************************/

int docSelectWholeSection(	DocumentSelection *	ds,
				int			direction )
    {
    BufferItem *		sectNode0= docGetSectNode( ds->dsHead.dpNode );
    BufferItem *		sectNode1= docGetSectNode( ds->dsTail.dpNode );

    if  ( ! sectNode0 || ! sectNode1 )
	{ XXDEB(sectNode0,sectNode1); return -1;	}

    if  ( sectNode0->biParent != sectNode1->biParent )
	{ XXDEB(sectNode0->biParent,sectNode1->biParent); return -1;	}

    if  ( direction > 0 )
	{
	if  ( docTailPosition( &(ds->dsTail), sectNode1 ) )
	    { LDEB(1); return -1;	}
	if  ( docNextPosition( &(ds->dsTail) ) )
	    { return 1;	}

	sectNode0= sectNode1= docGetSectNode( ds->dsTail.dpNode );
	}

    if  ( direction < 0 )
	{
	if  ( docHeadPosition( &(ds->dsHead), sectNode0 ) )
	    { LDEB(1); return -1;	}
	if  ( docPrevPosition( &(ds->dsHead) ) )
	    { return 1;	}

	sectNode0= sectNode1= docGetSectNode( ds->dsHead.dpNode );
	}

    if  ( docHeadPosition( &(ds->dsHead), sectNode0 ) )
	{ LDEB(1); return -1;	}
    if  ( docTailPosition( &(ds->dsTail), sectNode1 ) )
	{ LDEB(1); return -1;	}

    if  ( direction >= 0 )
	{ ds->dsAnchor= ds->dsHead; ds->dsDirection=  1;	}
    else{ ds->dsAnchor= ds->dsTail; ds->dsDirection= -1;	}

    ds->dsCol0= ds->dsCol1= -1;
    docSetSelectionScope( ds );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Change the selection to cover whole paragraphs.			*/
/*									*/
/*  Depending on the 'direction' argument:				*/
/*	<  0:	The previous one.					*/
/*	== 0:	The current one.					*/
/*	>  0:	The next one.						*/
/*									*/
/************************************************************************/

int docSelectWholeParagraph(	DocumentSelection *	ds,
				int			direction )
    {
    BufferItem *		paraNode0= ds->dsHead.dpNode;
    BufferItem *		paraNode1= ds->dsTail.dpNode;

    if  ( paraNode0->biParent != paraNode1->biParent )
	{ XXDEB(paraNode0->biParent,paraNode1->biParent); return -1;	}

    if  ( direction > 0 )
	{
	if  ( docTailPosition( &(ds->dsTail), paraNode1 ) )
	    { LDEB(1); return -1;	}
	if  ( docNextPosition( &(ds->dsTail) ) )
	    { return 1;	}

	paraNode0= paraNode1= ds->dsTail.dpNode;
	}

    if  ( direction < 0 )
	{
	if  ( docHeadPosition( &(ds->dsHead), paraNode0 ) )
	    { LDEB(1); return -1;	}
	if  ( docPrevPosition( &(ds->dsHead) ) )
	    { return 1;	}

	paraNode0= paraNode1= ds->dsHead.dpNode;
	}

    if  ( docHeadPosition( &(ds->dsHead), paraNode0 ) )
	{ LDEB(1); return -1;	}
    if  ( docTailPosition( &(ds->dsTail), paraNode1 ) )
	{ LDEB(1); return -1;	}

    if  ( direction >= 0 )
	{ ds->dsAnchor= ds->dsHead; ds->dsDirection=  1;	}
    else{ ds->dsAnchor= ds->dsTail; ds->dsDirection= -1;	}

    ds->dsCol0= ds->dsCol1= -1;
    docSetSelectionScope( ds );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Change the selection to cover whole rows.				*/
/*									*/
/*  Depending on the 'direction' argument:				*/
/*	<  0:	The previous one.					*/
/*	== 0:	The current one.					*/
/*	>  0:	The next one.						*/
/*									*/
/************************************************************************/

int docSelectRow(		DocumentSelection *	ds,
				int			direction,
				int			allColumns )
    {
    BufferItem *		cellNode0= docGetCellNode( ds->dsHead.dpNode );
    BufferItem *		cellNode1= docGetCellNode( ds->dsTail.dpNode );
    BufferItem *		rowNode0= docGetRowNode( cellNode0 );
    BufferItem *		rowNode1= docGetRowNode( cellNode1 );

    if  ( ! rowNode0 || ! rowNode1 )
	{ XXDEB(rowNode0,rowNode1); return -1;	}

    if  ( rowNode0->biParent != rowNode1->biParent )
	{ XXDEB(rowNode0->biParent,rowNode1->biParent); return -1;	}

    if  ( direction != 0 )
	{
	int		row0, row1;

	if  ( docDelimitTable( ds->dsHead.dpNode, (BufferItem **)0,
					(int *)0, &row0, (int *)0, &row1 ) )
	    { LDEB(1); return -1;	}

	if  ( direction > 0 )
	    {
	    if  ( docTailPosition( &(ds->dsTail), rowNode1 ) )
		{ LDEB(1); return -1;	}
	    if  ( docNextPosition( &(ds->dsTail) ) )
		{ return 1;	}
	    rowNode1= docGetRowNode( ds->dsTail.dpNode );
	    if  ( ! rowNode1 || rowNode1->biNumberInParent > row1 )
		{ return 1;	}

	    if  ( docTailPosition( &(ds->dsHead), rowNode0 ) )
		{ LDEB(1); return -1;	}
	    if  ( docNextPosition( &(ds->dsHead) ) )
		{ return 1;	}
	    rowNode0= docGetRowNode( ds->dsHead.dpNode );
	    if  ( ! rowNode0 || rowNode0->biNumberInParent > row1 )
		{ return 1;	}
	    }

	if  ( direction < 0 )
	    {
	    if  ( docHeadPosition( &(ds->dsHead), rowNode0 ) )
		{ LDEB(1); return -1;	}
	    if  ( docPrevPosition( &(ds->dsHead) ) )
		{ return 1;	}
	    rowNode0= docGetRowNode( ds->dsHead.dpNode );
	    if  ( ! rowNode0 || rowNode0->biNumberInParent < row0 )
		{ return 1;	}

	    if  ( docHeadPosition( &(ds->dsTail), rowNode1 ) )
		{ LDEB(1); return -1;	}
	    if  ( docPrevPosition( &(ds->dsTail) ) )
		{ return 1;	}
	    rowNode1= docGetRowNode( ds->dsTail.dpNode );
	    if  ( ! rowNode1 || rowNode1->biNumberInParent < row0 )
		{ return 1;	}
	    }
	}

    if  ( allColumns )
	{
	if  ( docHeadPosition( &(ds->dsHead), rowNode0 ) )
	    { LDEB(1); return -1;	}
	if  ( docTailPosition( &(ds->dsTail), rowNode1 ) )
	    { LDEB(1); return -1;	}
	}
    else{
	if  ( cellNode0->biNumberInParent >= rowNode0->biChildCount ||
	      cellNode0->biNumberInParent >= rowNode1->biChildCount )
	    { return 1;	}
	cellNode0= rowNode0->biChildren[cellNode0->biNumberInParent];

	if  ( cellNode1->biNumberInParent >= rowNode0->biChildCount ||
	      cellNode1->biNumberInParent >= rowNode1->biChildCount )
	    { return 1;	}
	cellNode1= rowNode1->biChildren[cellNode1->biNumberInParent];

	if  ( docHeadPosition( &(ds->dsHead), cellNode0 ) )
	    { LDEB(1); return -1;	}
	if  ( docTailPosition( &(ds->dsTail), cellNode1 ) )
	    { LDEB(1); return -1;	}
	}

    if  ( direction >= 0 )
	{ ds->dsAnchor= ds->dsHead; ds->dsDirection=  1;	}
    else{ ds->dsAnchor= ds->dsTail; ds->dsDirection= -1;	}

    ds->dsCol0= ds->dsCol1= -1;
    docSetSelectionScope( ds );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Change the selection to cover whole cells/columns.			*/
/*									*/
/*  Depending on the 'direction' argument:				*/
/*	<  0:	The previous one.					*/
/*	== 0:	The current one.					*/
/*	>  0:	The next one.						*/
/*									*/
/************************************************************************/

int docSelectWholeCell(		DocumentSelection *	ds,
				int			direction,
				int			allRows )
    {
    BufferItem *	cellNode0= docGetCellNode( ds->dsHead.dpNode );
    BufferItem *	cellNode1= docGetCellNode( ds->dsTail.dpNode );
    BufferItem *	rowNode0;
    BufferItem *	rowNode1;
    BufferItem *	parentNode;

    DocumentPosition	dpHead;
    DocumentPosition	dpTail;

    if  ( ! cellNode0 || ! cellNode1 )
	{ XXDEB(cellNode0,cellNode1); return -1;	}

    rowNode0= docGetRowNode( cellNode0 );
    rowNode1= docGetRowNode( cellNode1 );

    if  ( ! rowNode0 || ! rowNode1 )
	{ XXDEB(rowNode0,rowNode1); return -1;	}

    if  ( rowNode0->biParent != rowNode1->biParent )
	{ XXDEB(rowNode0->biParent,rowNode1->biParent); return -1;	}

    parentNode= rowNode0->biParent;

    if  ( allRows )
	{
	int		col;
	int		row, row0, row1;

	if  ( docDelimitTable( ds->dsHead.dpNode, &parentNode,
						&col, &row0, &row, &row1 ) )
	    { LDEB(1); return -1;	}

	rowNode0= parentNode->biChildren[row0];
	rowNode1= parentNode->biChildren[row1];

	if  ( cellNode0->biNumberInParent >= rowNode0->biChildCount )
	    {
	    LLDEB(cellNode0->biNumberInParent,rowNode0->biChildCount);
	    return -1;
	    }
	cellNode0= rowNode0->biChildren[cellNode0->biNumberInParent];

	if  ( cellNode1->biNumberInParent >= rowNode1->biChildCount )
	    {
	    LLDEB(cellNode1->biNumberInParent,rowNode1->biChildCount);
	    return -1;
	    }
	cellNode1= rowNode1->biChildren[cellNode1->biNumberInParent];
	}

    if  ( direction > 0 )
	{
	if  ( docTailPosition( &(ds->dsTail), cellNode1 ) )
	    { LDEB(1); return -1;	}
	if  ( docNextPosition( &(ds->dsTail) ) )
	    { return 1;	}
	cellNode1= docGetCellNode( ds->dsTail.dpNode );
	if  ( ! cellNode1 || cellNode1->biParent != rowNode1 )
	    { return 1;	}
	if  ( cellNode1->biNumberInParent >= rowNode0->biChildCount )
	    { return 1;	}

	if  ( docTailPosition( &(ds->dsHead), cellNode0 ) )
	    { LDEB(1); return -1;	}
	if  ( docNextPosition( &(ds->dsHead) ) )
	    { return 1;	}
	cellNode0= docGetCellNode( ds->dsHead.dpNode );
	if  ( ! cellNode0 || cellNode0->biParent != rowNode0 )
	    { return 1;	}
	if  ( cellNode0->biNumberInParent >= rowNode1->biChildCount )
	    { return 1;	}
	}

    if  ( direction < 0 )
	{
	if  ( docHeadPosition( &(ds->dsHead), cellNode0 ) )
	    { LDEB(1); return -1;	}
	if  ( docPrevPosition( &(ds->dsHead) ) )
	    { return 1;	}
	cellNode0= docGetCellNode( ds->dsHead.dpNode );
	if  ( ! cellNode0 || cellNode0->biParent != rowNode0 )
	    { return 1;	}
	/*  No need to check against a wider row */

	if  ( docHeadPosition( &(ds->dsTail), cellNode1 ) )
	    { LDEB(1); return -1;	}
	if  ( docPrevPosition( &(ds->dsTail) ) )
	    { return 1;	}
	cellNode1= docGetCellNode( ds->dsTail.dpNode );
	if  ( ! cellNode1 || cellNode1->biParent != rowNode1 )
	    { return 1;	}
	/*  No need to check against a wider row */
	}

    if  ( docHeadPosition( &dpHead, cellNode0 ) )
	{ LDEB(1); return -1;	}
    if  ( docTailPosition( &dpTail, cellNode1 ) )
	{ LDEB(1); return -1;	}

    if  ( cellNode0 == cellNode1 )
	{
	int	rowspan= 1;
	int	colspan= 1;

	docTableDetermineCellspans( &rowspan, &colspan, cellNode0 );

	if  ( rowspan > 1 || colspan > 1 )
	    {
	    rowNode1= parentNode->biChildren[
				rowNode0->biNumberInParent+ rowspan- 1];
	    cellNode1= rowNode1->biChildren[
				cellNode0->biNumberInParent+ colspan- 1];

	    if  ( docTailPosition( &dpTail, cellNode1 ) )
		{ LDEB(1); return -1;	}
	    }
	}

    docSetRangeSelection( ds, &dpHead, &dpTail, direction );

    return 0;
    }

/************************************************************************/

int docSelectColumn(	DocumentSelection *	ds,
			int			col )
    {
    const int		direction= 0;
    const int		allRows= 1;

    BufferItem *	paraNode= ds->dsHead.dpNode;
    BufferItem *	rowNode= docGetRowNode( paraNode );

    if  ( ! rowNode )
	{ XDEB(rowNode); return -1;	}
    if  ( col < 0 || col >= rowNode->biChildCount )
	{ LLDEB(col,rowNode->biChildCount); return -1;	}

    if  ( docHeadPosition( &(ds->dsHead), rowNode->biChildren[col] ) )
	{ LDEB(col); return -1;	}
    if  ( docTailPosition( &(ds->dsTail), rowNode->biChildren[col] ) )
	{ LDEB(col); return -1;	}

    if  ( docSelectWholeCell( ds, direction, allRows ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Change the selection to cover the whole table.			*/
/*									*/
/************************************************************************/

int docSelectWholeTable(	DocumentSelection *	ds )
    {
    BufferItem *		rowNode0= docGetRowNode( ds->dsHead.dpNode );
    BufferItem *		rowNode1= docGetRowNode( ds->dsTail.dpNode );

    BufferItem *		parentNode;
    int				row0;
    int				row1;

    if  ( ! rowNode0 || ! rowNode1 )
	{ XXDEB(rowNode0,rowNode1); return -1;	}

    if  ( rowNode0->biParent != rowNode1->biParent )
	{ XXDEB(rowNode0->biParent,rowNode1->biParent); return -1;	}

    if  ( docDelimitTable( ds->dsHead.dpNode, &parentNode, (int *)0,
						&row0, (int *)0, &row1 ) )
	{ LDEB(1); return -1;	}

    rowNode0= parentNode->biChildren[row0];
    rowNode1= parentNode->biChildren[row1];

    if  ( docHeadPosition( &(ds->dsHead), rowNode0 ) )
	{ LDEB(1); return -1;	}
    if  ( docTailPosition( &(ds->dsTail), rowNode1 ) )
	{ LDEB(1); return -1;	}

    ds->dsAnchor= ds->dsHead; ds->dsDirection=  1;
    ds->dsCol0= ds->dsCol1= -1;

    docSetSelectionScope( ds );

    return 0;
    }

