#   include	"docLayoutConfig.h"

#   include	"docDraw.h"
#   include	<docPageGrid.h>
#   include	"docRowLayout.h"
#   include	<docTreeNode.h>
#   include	<docTextLine.h>

#   include	<appDebugon.h>

/************************************************************************/

static int docDrawRowPageStrip(	BufferItem *			rowNode,
				const LayoutPosition *		lpThisFrame,
				BlockFrame *			bf,
				void *				through,
				DrawingContext *		dc,
				const BlockOrigin *		bo );

/************************************************************************/
/*									*/
/*  Draw part of the grid originating from a table row.			*/
/*									*/
/*  4)  For all cells.....						*/
/*  5)  Skip cells that are included in the colspan or rowspan of	*/
/*	another cell. The left/top cell of a span has to draw the	*/
/*	borders and the background.					*/
/*									*/
/************************************************************************/

static int docDrawCellOrnamentsForRow(
			const BufferItem *			rowNode,
			const BlockFrame *			bf,
			const BlockOrigin *			bo,
			void *					through,
			DrawingContext *			dc,
			int					atRowTop,
			int					atRowBottom,
			const LayoutPosition *			lpTop )
    {
    const LayoutContext *		lc= &(dc->dcLayoutContext);
    const BufferDocument *		bd= lc->lcDocument;
    const CellProperties *		cp;
    int					col;

    if  ( ! dc->dcDrawOrnaments )
	{ return 0;	}

    /*  4  */
    cp= rowNode->biRowCells;
    for ( col= 0; col < rowNode->biChildCount; cp++, col++ )
	{
	BufferItem *			cellNode= rowNode->biChildren[col];

	BlockOrnaments			cellOrnaments;
	DocumentRectangle		drCell;
	DocumentRectangle		drOutside;
	DocumentRectangle		drInside;

	docInitBlockOrnaments( &cellOrnaments );

	/*  5  */
	if  ( CELL_MERGED( cp ) )
	    { continue;	}

	docCellRectangleTwips( &drCell, bf, cellNode );

	drCell.drX0 += bo->boXShift;
	drCell.drX1 += bo->boXShift;
	drCell.drY0= lpTop->lpPageYTwips;
	drCell.drY1 += bo->boYShift;

	docGetCellOrnaments( &cellOrnaments, &drOutside, &drInside, &drCell,
			bd, rowNode, col,
			atRowTop, atRowBottom, dc->dcDrawTableGrid );

	if  ( (*dc->dcDrawOrnaments)( &cellOrnaments, lpTop->lpPage,
					&drOutside, &drInside, through, dc ) )
	    { LDEB(1); return -1;	}
	}

    return 0;
    }

/************************************************************************/

static int docDrawAdvanceRowInCell(
				void *				through,
				BufferItem *			rowNode,
				LayoutPosition *		lpThisFrame,
				LayoutPosition *		lpBelow,
				BlockFrame *			bf,
				DrawingContext *		dc,
				const BlockOrigin *		bo )
    {
    if  ( docDrawRowPageStrip( rowNode, lpThisFrame, bf, through, dc, bo ) )
	{ LDEB(1); return -1;	}

    /*  Only used in same frame */
    *lpBelow= rowNode->biBelowPosition;

    return 0;
    }

/************************************************************************/

static int docDrawAdvanceParaInCell(
				void *				through,
				BufferItem *			paraBi,
				const LayoutPosition *		lpThisFrame,
				LayoutPosition *		lpBelow,
				BlockFrame *			bf,
				DrawingContext *		dc,
				const BlockOrigin *		bo )
    {
    ParagraphDrawingStrip	pds;
    const int			countAfter= 0;

    ParagraphFrame		pf;
    int				line= 0;

    LayoutPosition		lpShadeTop= *lpThisFrame;

    docParagraphFrameTwips( &pf, bf, paraBi );

    while( line < paraBi->biParaLineCount )
	{
	TextLine *	tl= paraBi->biParaLines+ line;
	LayoutPosition	lpLine;

	docShiftPosition( &lpLine, bo, &(tl->tlTopPosition) );

	if  ( ! DOC_COLUMN_AFTER( lpThisFrame, &lpLine ) )
	    { break;	}

	line++;
	}

    /**/
    if  ( docDrawParagraphStrip( through, &pds, paraBi, countAfter,
				&lpShadeTop, line, &pf, dc, lpThisFrame, bo ) )
	{ LDEB(line); return -1;	}

    /*  Only used in same frame */
    *lpBelow= pds.pdsShadeBelow;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Print as much of a table-cell/column in a row as fits on the	*/
/*  current page.							*/
/*									*/
/*  1)  Skip entries that end before the current page.			*/
/*  2)  Draw entries that do not start after the current page.		*/
/*									*/
/************************************************************************/

static int docDrawCellPageStrip( void *				through,
				const BufferItem *		cellNode,
				const LayoutPosition *		lpThisFrame,
				BlockFrame *			bf,
				DrawingContext *		dc,
				const BlockOrigin *		bo )
				
    {
    int			child= 0;

    LayoutPosition	lpShadeTop= *lpThisFrame;
    LayoutPosition	lpShadeBelow= *lpThisFrame;

    /*  1  */
    while( child < cellNode->biChildCount )
	{
	BufferItem *		childNode= cellNode->biChildren[child];
	const LayoutPosition *	belowChildPos;
	LayoutPosition		lpChildBottom;

	if  ( docIsRowNode( childNode ) )
	    { belowChildPos= &(childNode->biRowBelowAllCellsPosition);	}
	else{ belowChildPos= &(childNode->biBelowPosition);		}

	docShiftPosition( &lpChildBottom, bo, belowChildPos );
	if  ( ! DOC_COLUMN_AFTER( lpThisFrame, &lpChildBottom ) )
	    { break;	}

	child++;
	}

    /*  2  */
    while( child < cellNode->biChildCount )
	{
	BufferItem *	childNode= cellNode->biChildren[child];
	LayoutPosition	lpChildTop;

	docShiftPosition( &lpChildTop, bo, &(childNode->biTopPosition) );

	if  ( DOC_COLUMN_AFTER( &lpChildTop, lpThisFrame ) )
	    { break;	}

	switch( childNode->biLevel )
	    {
	    case DOClevPARA:
		if  ( docDrawAdvanceParaInCell( through, childNode,
				    &lpShadeTop, &lpShadeBelow, bf, dc, bo ) )
		    { LDEB(1); return -1;	}
		break;

	    case DOClevROW:
		if  ( docDrawAdvanceRowInCell( through, childNode,
				    &lpShadeTop, &lpShadeBelow, bf, dc, bo ) )
		    { LDEB(1); return -1;	}
		break;

	    default:
		LDEB(childNode->biLevel);
		break;
	    }

	/*  Only used in same frame */
	lpShadeTop= lpShadeBelow;
	child++;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Print as much of a table row as fits on the current page.		*/
/*									*/
/*  1)  Find the bottom of this row on the page: We need this to draw	*/
/*	the cell ornaments before we draw the row contents.		*/
/*									*/
/************************************************************************/

static int docDrawRowPageStrip(	BufferItem *			rowNode,
				const LayoutPosition *		lpThisFrame,
				BlockFrame *			bf,
				void *				through,
				DrawingContext *		dc,
				const BlockOrigin *		bo )
    {
    int				col;

    LayoutPosition		lpTop;
    const CellProperties *	cp;

    int				atRowTop= 0;
    int				atRowBottom= 0;

    if  ( DOC_SAME_FRAME( lpThisFrame,
				&(rowNode->biTopPosition) ) )
	{ atRowTop= 1;	}
    if  ( DOC_SAME_FRAME( lpThisFrame,
				&(rowNode->biRowBelowAllCellsPosition) ) )
	{ atRowBottom= 1;	}

    lpTop= *lpThisFrame;

    if  ( ! rowNode->biRowIsTableHeader )
	{
	if  ( atRowTop )
	    {
	    docShiftPosition( &lpTop, bo, &(rowNode->biTopPosition) );

	    if  ( rowNode->biRowPrecededByHeader )
		{
		int		high;

		if  ( docDrawTableHeader( &high, rowNode, bf, through,
				    dc, &(rowNode->biRowAboveHeaderPosition) ) )
		    { LDEB(rowNode->biRowPrecededByHeader); return -1;	}
		}
	    }
	else{
	    if  ( rowNode->biRowTableHeaderRow >= 0 )
		{
		int		high;

		if  ( docDrawTableHeader( &high, rowNode, bf, through,
							    dc, lpThisFrame ) )
		    { LDEB(rowNode->biRowIsTableHeader); return -1;	}
		}
	    }
	}

    if  ( docDrawCellOrnamentsForRow( rowNode, bf, bo, through, dc,
					    atRowTop, atRowBottom, &lpTop ) )
	{ LDEB(1); return -1;	}

    cp= rowNode->biRowCells;
    for ( col= 0; col < rowNode->biChildCount; cp++, col++ )
	{
	BufferItem *	cellNode= rowNode->biChildren[col];

	int		selected;
	int		cmp;

	if  ( CELL_MERGED( cp ) )
	    { continue;	}

	cmp= docCompareCellPositionToSelection( &selected, cellNode,
						    dc->dcSelection );
	if  ( cmp > 0 )
	    { break;    }
	if  ( cmp < 0 || ! selected )
	    { continue; }

	if  ( docDrawCellPageStrip( through, cellNode, &lpTop, bf, dc, bo ) )
	    { LDEB(1); return -1;	}
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Draw a table header.						*/
/*									*/
/*  1)  Find the header that belongs to this row.			*/
/*  2)  Determine where the header would be drawn to find the		*/
/*	horizontal offset for header drawing.				*/
/*									*/
/************************************************************************/

int docDrawTableHeader(	int *				pHigh,
			const BufferItem *		rowNode,
			const BlockFrame *		bfRef,
			void *				through,
			DrawingContext *		dc,
			const LayoutPosition *		lpHeader )
    {
    int				rval= 0;
    const LayoutContext *	lc= &(dc->dcLayoutContext);
    BufferItem *		headerBi;
    BlockFrame			bfHeader;

    BlockOrigin			bo;

    docInitBlockOrigin( &bo );
    docLayoutInitBlockFrame( &bfHeader );

    /*  1  */
    if  ( rowNode->biRowTableHeaderRow < 0 )
	{ LDEB(rowNode->biRowTableHeaderRow); rval= -1; goto ready;	}

    headerBi= rowNode->biParent->biChildren[rowNode->biRowTableHeaderRow];

    /*  2  */
    docBlockFrameTwips( &bfHeader, headerBi,
			    lc->lcDocument,
			    headerBi->biTopPosition.lpPage,
			    headerBi->biTopPosition.lpColumn );

    bo.boOverrideFrame= 1;
    bo.boXShift= bfRef->bfContentRect.drX0- bfHeader.bfContentRect.drX0;
    bo.boYShift= lpHeader->lpPageYTwips- headerBi->biTopPosition.lpPageYTwips;
    bo.boFrameOverride= *lpHeader;

    if  ( docDrawRowNode( headerBi, through, dc, &bo ) )
	{ LDEB(1); rval= -1; goto ready;	}

    *pHigh= headerBi->biBelowPosition.lpPageYTwips- 
					headerBi->biTopPosition.lpPageYTwips;

  ready:

    docLayoutCleanBlockFrame( &bfHeader );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Draw a table row by successively drawing the portions that fit in	*/
/*  the current (newspaper style) column and moving to the next column.	*/
/*									*/
/*  NOTE that any drawing routine can be recursive.			*/
/*									*/
/************************************************************************/

int docDrawRowNode(	BufferItem *			rowNode,
			void *				through,
			DrawingContext *		dc,
			const BlockOrigin *		bo )
    {
    const LayoutContext *	lc= &(dc->dcLayoutContext);
    int				rval= 0;
    LayoutPosition		lpThisFrame;
    BlockFrame			bf;

    docLayoutInitBlockFrame( &bf );
    docBlockFrameTwips( &bf, rowNode, lc->lcDocument,
					    rowNode->biTopPosition.lpPage,
					    rowNode->biTopPosition.lpColumn );

    docShiftPosition( &lpThisFrame, bo, &(rowNode->biTopPosition) );

    if  ( docDrawRowPageStrip( rowNode, &lpThisFrame, &bf, through, dc, bo ) )
	{ LDEB(1); rval= -1; goto ready;	}

    while( DOC_COLUMN_AFTER( &(rowNode->biRowBelowAllCellsPosition),
							    &lpThisFrame ) )
	{
	int			ret;

	/*  1  */
	ret= docDrawToNextColumn( rowNode, rowNode, through,
						    &lpThisFrame, &bf, dc );
	if  ( ret < 0 )
	    { SLDEB(docLevelStr(rowNode->biLevel),ret); rval= -1; goto ready; }
	if  ( ret > 0 )
	    { break;	}

	if  ( docDrawRowPageStrip( rowNode,
				    &lpThisFrame, &bf, through, dc, bo ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}

  ready:

    docLayoutCleanBlockFrame( &bf );

    return rval;
    }

