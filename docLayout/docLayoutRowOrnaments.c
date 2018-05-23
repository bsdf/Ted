/************************************************************************/
/*									*/
/*  Layout of a table row in the document.				*/
/*									*/
/************************************************************************/

#   include	"docLayoutConfig.h"

#   include	<stddef.h>

#   include	<docBuf.h>
#   include	<docTreeNode.h>
#   include	<docBlockOrnaments.h>
#   include	"docRowLayout.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Decide what top border to use for a table cell (If any)		*/
/*									*/
/*  1)  Get the table row above the current row.			*/
/*  2)  If a cell has a bottom border, and the next one a top border,	*/
/*	MS-Word only draws the bottom border of the topmost cell.	*/
/*  3)  By default use the cells top border.				*/
/*  4)  Use the table top border at the top of a page.			*/
/*									*/
/************************************************************************/

static void docGetCellTopBorder( BorderProperties *		pBpTop,
				int *				pBpTopNr,
				int *				pUseAbove,
				const BufferDocument *		bd,
				const BufferItem *		rowNode,
				int				col,
				int				atRowTop )
    {
    const BufferItem *		parentBi= rowNode->biParent;
    const CellProperties *	cp= &(rowNode->biRowCells[col]);

    const BufferItem *		rowBiAbove= (const BufferItem *)0;

    const CellProperties *	aboveCpC= (const CellProperties *)0;
    int				useAbove= 0;

    BorderProperties		bpAbove;
    BorderProperties		bpTop;
    int				aboveNr= -1;
    int				topNr;

    /*  1  */
    if  ( rowNode->biNumberInParent > rowNode->biRowTableFirst )
	{
	if  ( rowNode->biNumberInParent <= 0 )
	    {
	    SDEB(docTreeTypeStr(rowNode->biTreeType));
	    LDEB(rowNode->biNumberInParent);
	    }
	else{ rowBiAbove= parentBi->biChildren[rowNode->biNumberInParent- 1]; }
	}

    if  ( rowBiAbove )
	{
	int		csp= 1;
	int		cspa= 1;
	int		l, r;
	int		aboveCol;

	if  ( col == 0 )
	    { l= rowNode->biRowLeftIndentTwips;	}
	else{ l= cp[-1].cpRightBoundaryTwips;	}
	r= cp->cpRightBoundaryTwips;
	if  ( cp->cpHorizontalMerge == CELLmergeHEAD )
	    { r= docGetCellRight( &csp, rowNode->biChildren[col] );	}

	aboveCol= docGetMatchingCell( &cspa, rowBiAbove, l, r );
	if  ( aboveCol >= 0 )
	    { aboveCpC= rowBiAbove->biRowCells+ aboveCol;	}
	}

    /*  2  */
    if  ( aboveCpC							&&
	  aboveCpC->cpBottomBorderNumber != cp->cpTopBorderNumber	)
	{
	aboveNr= aboveCpC->cpBottomBorderNumber;
	docGetBorderPropertiesByNumber( &bpAbove, bd, aboveNr );
	if  ( DOCisBORDER( &bpAbove ) )
	    { useAbove= 1;	}
	}

    /*  3  */
    topNr= cp->cpTopBorderNumber;
    docGetBorderPropertiesByNumber( &bpTop, bd, topNr );

    /*  2  */
    if  ( useAbove )
	{
	bpTop= bpAbove;
	topNr= aboveNr;
	}

    /*  4  */
    if  ( rowNode->biNumberInParent > rowNode->biRowTableFirst	&&
          ! atRowTop						)
	{
	docGetBorderPropertiesByNumber( &bpTop,
					bd, rowNode->biRowTopBorderNumber );
	topNr= rowNode->biRowTopBorderNumber;
	useAbove= 0;
	}

    *pBpTop= bpTop;
    *pBpTopNr= topNr;
    *pUseAbove= useAbove;

    return;
    }

/************************************************************************/
/*									*/
/*  Decide what bottom border to use for a table cell (If any)		*/
/*									*/
/*  0)  Paranoia checks: Bugs that I actually once programmed.		*/
/*  1)  Get the table row below the current row.			*/
/*  2)  By default use the cells bottom border.				*/
/*  3)  If a cell has a bottom boder, and the next one a top border,	*/
/*	MS-Word only draws the bottom border of the topmost cell.	*/
/*  4)  Use the table bottom border at the bottom of a page.		*/
/*									*/
/************************************************************************/

void docGetCellBottomBorder(	BorderProperties *		pBpBottom,
				int *				pBpBottomNr,
				int *				pUseBelow,
				const BufferDocument *		bd,
				const BufferItem *		rowNode,
				int				col,
				int				atRowBottom )
    {
    const BufferItem *		parentBi= rowNode->biParent;
    const CellProperties *	cp= &(rowNode->biRowCells[col]);

    const BufferItem *		rowBiBelow= (const BufferItem *)0;

    const CellProperties *	belowCpC= (const CellProperties *)0;
    int				useBelow= 0;

    BorderProperties		bpBelow;
    BorderProperties 		bpBottom;
    int				belowNr;
    int				bottomNr;

    /*  0  */
    if  ( col >= rowNode->biRowCellCount )
	{ LLDEB(col,rowNode->biRowCellCount);	}
    if  ( rowNode->biRowTablePast < 0				||
	  rowNode->biRowTablePast > rowNode->biParent->biChildCount	)
	{ LLDEB(rowNode->biRowTablePast,rowNode->biParent->biChildCount); }

    /*  1  */
    if  ( rowNode->biNumberInParent < rowNode->biRowTablePast- 1 )
	{ rowBiBelow= parentBi->biChildren[rowNode->biNumberInParent+ 1];	}

    if  ( rowBiBelow )
	{
	int		csp= 1;
	int		cspb= 1;
	int		l, r;
	int		belowCol;

	if  ( col == 0 )
	    { l= rowNode->biRowLeftIndentTwips;	}
	else{ l= cp[-1].cpRightBoundaryTwips;	}
	r= cp->cpRightBoundaryTwips;
	if  ( cp->cpHorizontalMerge == CELLmergeHEAD )
	    { r= docGetCellRight( &csp, rowNode->biChildren[col] );	}

	belowCol= docGetMatchingCell( &cspb, rowBiBelow, l, r );
	if  ( belowCol >= 0 )
	    { belowCpC= rowBiBelow->biRowCells+ belowCol;	}
	}

    /*  2  */
    bottomNr= cp->cpBottomBorderNumber;
    docGetBorderPropertiesByNumber( &bpBottom, bd, bottomNr );

    /*  3  */
    if  ( belowCpC )
	{
	belowNr= belowCpC->cpBottomBorderNumber;
	docGetBorderPropertiesByNumber( &bpBelow, bd, belowNr );

	if  ( ! DOCisBORDER( &bpBottom )		&&
	      belowCpC					&&
	      DOCisBORDER( &bpBelow )	)
	    {
	    useBelow= 1;
	    bottomNr= belowNr;
	    }
	}

    /*  4  */
    if  ( rowNode->biNumberInParent < rowNode->biRowTablePast- 1	&&
	  ! atRowBottom						)
	{
	bottomNr= rowNode->biRowBottomBorderNumber;
	docGetBorderPropertiesByNumber( &bpBottom, bd, bottomNr );
	useBelow= 0;
	}

    *pBpBottom= bpBottom;
    *pBpBottomNr= bottomNr;
    *pUseBelow= useBelow;

    return;
    }

/************************************************************************/
/*									*/
/*  Draw part of the grid originating from a table row.			*/
/*									*/
/*  3)  Is this cell shaded?						*/
/*  6)  If a cell has a right boder, and the next one a left border,	*/
/*	MS-Word only draws the right border of the leftmost cell.	*/
/*	( The same applies in the vertical direction. )			*/
/*  7)  Base the decisions on whether to draw a table grid on the same	*/
/*	criteria.							*/
/*  8)  Draw cell top border.						*/
/*  9)  Draw cell left border.						*/
/* 10)  Draw cell right border.						*/
/* 11)  Draw cell bottom border.					*/
/*									*/
/* Experimentation with MS-Word reveals:				*/
/* a)	The the top border is drawn inside the cell and that the	*/
/*	formatter allocates space for it.				*/
/* b)	That the left and right borders are half in the cell. Typically	*/
/*	covering part of the horizontal gap that the table reserves.	*/
/* c)	That the bottom border is completely below the cell. So inside	*/
/*	the table it comes at the same location as the top border of	*/
/*	the next row.							*/
/*									*/
/************************************************************************/

static int docCellLeftBorderDrawn(
				BorderProperties *		bpLeft,
				int *				bpLeftNr,
				int *				pProp,
				DocumentRectangle *		drOutside,
				DocumentRectangle *		drInside,
				const DocumentRectangle *	drCell,
				const BufferDocument *		bd,
				const BufferItem *		rowNode,
				int				col,
				int				drawTableGrid )
    {
    int				rval= 0;
    const CellProperties *	cp= rowNode->biRowCells+ col;

    int				leftHasBorder= 0;
    int				hasLeftBorder= 0;

    int				thick= 0;
    int				space;

    /*  6  */
    if  ( col > 0							&&
	  docBorderNumberIsBorder( bd, cp[-1].cpRightBorderNumber )	)
	{ leftHasBorder= 1;	}

    docGetBorderPropertiesByNumber( bpLeft, bd, cp->cpLeftBorderNumber );
    hasLeftBorder= DOCisBORDER( bpLeft );

    if  ( leftHasBorder						&&
	  cp[-1].cpRightBorderNumber != cp->cpLeftBorderNumber	)
	{
	docGetBorderPropertiesByNumber( bpLeft, bd,
					    cp[-1].cpRightBorderNumber );
	thick= docBorderThick( &space, bpLeft );

	drOutside->drX0= drCell->drX0+ 1- thick/ 2;
	drInside->drX0= drOutside->drX0+ thick;
	*bpLeftNr= cp[-1].cpRightBorderNumber;
	}
    else{
	/*  9  */
	if  ( hasLeftBorder )
	    {
	    *pProp= ORNdrawLEFT_BORDER; rval= 1;

	    thick= docBorderThick( &space, bpLeft );

	    drOutside->drX0= drCell->drX0- thick/ 2;
	    drInside->drX0= drOutside->drX0+ thick;
	    *bpLeftNr= cp->cpLeftBorderNumber;
	    }
	}

    /*  7  */
    if  ( drawTableGrid )
	{
	if  ( ! hasLeftBorder && ! leftHasBorder )
	    { *pProp= ORNdrawLEFT_GRID; rval= 1;	}
	}

    return rval;
    }

static int docCellRightBorderDrawn(
				BorderProperties *		bpRight,
				int *				bpRightNr,
				int *				pProp,
				DocumentRectangle *		drOutside,
				DocumentRectangle *		drInside,
				const DocumentRectangle *	drCell,
				const BufferDocument *		bd,
				const BufferItem *		rowNode,
				int				col,
				int				drawTableGrid )
    {
    int				rval= 0;
    const CellProperties *	cp= rowNode->biRowCells+ col;

    int				thick;
    int				space;

    int				drawCellRight= 1;

    int				rightHasBorder= 0;
    int				hasRightBorder= 0;

    if  ( col < rowNode->biChildCount- 1	&&
	  cp[1].cpHorizontalMerge ==CELLmergeFOLLOW	)
	{ drawCellRight= 0;	}

    /*  6  */
    if  ( col < rowNode->biChildCount- 1					&&
	  docBorderNumberIsBorder( bd, cp[1].cpLeftBorderNumber )	)
	{ rightHasBorder= 1;	}

    docGetBorderPropertiesByNumber( bpRight, bd, cp->cpRightBorderNumber );
    hasRightBorder= DOCisBORDER( bpRight );

    if  ( ! hasRightBorder && rightHasBorder )
	{
	docGetBorderPropertiesByNumber( bpRight, bd,
						cp[1].cpLeftBorderNumber );
	*bpRightNr= cp[1].cpLeftBorderNumber;
	}

    /*  10  */
    if  ( drawCellRight && hasRightBorder )
	{
	*pProp= ORNdrawRIGHT_BORDER; rval= 1;

	thick= docBorderThick( &space, bpRight );
	drInside->drX1= drCell->drX1- thick/ 2;
	drOutside->drX1= drInside->drX1+ thick;
	*bpRightNr= cp->cpRightBorderNumber;
	}

    /*  7  */
    if  ( drawTableGrid )
	{
	if  ( ! hasRightBorder && ! rightHasBorder )
	    { *pProp= ORNdrawRIGHT_GRID; rval= 1;	}
	}

    return rval;
    }

static int docCellTopBorderDrawn( BorderProperties *		bpTop,
				int *				bpTopNr,
				int *				pProp,
				DocumentRectangle *		drOutside,
				DocumentRectangle *		drInside,
				const DocumentRectangle *	drCell,
				const BufferDocument *		bd,
				const BufferItem *		rowNode,
				int				col,
				int				atRowTop,
				int				drawTableGrid )
    {
    int				rval= 0;
    int				useAbove= 0;

    docGetCellTopBorder( bpTop, bpTopNr, &useAbove, bd, rowNode, col, atRowTop );

    if  ( useAbove )
	{
	int	thick;
	int	space;

	thick= docBorderThick( &space, bpTop );

	drInside->drY0= drCell->drY0+ thick;
	}

    /*  8  */
    if  ( ! useAbove && DOCisBORDER( bpTop ) )
	{
	int	thick;
	int	space;

	*pProp= ORNdrawTOP_BORDER; rval= 1;

	thick= docBorderThick( &space, bpTop );

	/* border inside cell */
	drInside->drY0= drCell->drY0+ 1+ thick;
	}

    /*  7  */
    if  ( drawTableGrid )
	{
	if  ( ! DOCisBORDER( bpTop )		&&
	      ! useAbove			)
	    {
	    *pProp= ORNdrawTOP_GRID; rval= 1;
	    /* inside cell, width determined by screen drawer */
	    }
	}

    return rval;
    }

static int docCellBottomBorderDrawn(
				BorderProperties *		bpBottom,
				int *				bpBottomNr,
				int *				pProp,
				DocumentRectangle *		drOutside,
				DocumentRectangle *		drInside,
				const DocumentRectangle *	drCell,
				const BufferDocument *		bd,
				const BufferItem *		rowNode,
				int				col,
				int				atRowBottom,
				int				drawTableGrid )
    {
    int				rval= 0;

    int				thick;
    int				space;

    int				useBelow= 0;

    docGetCellBottomBorder( bpBottom, bpBottomNr, &useBelow,
					bd, rowNode, col, atRowBottom );

    /*  11  */
    if  ( DOCisBORDER( bpBottom ) )
	{
	*pProp= ORNdrawBOTTOM_BORDER; rval= 1;

	thick= docBorderThick( &space, bpBottom );
	drOutside->drY1= drCell->drY1+ thick;
	}

    /*  7  */
    if  ( drawTableGrid )
	{
	if  ( ! DOCisBORDER( bpBottom )		&&
	      ! useBelow			)
	    { *pProp= ORNdrawBOTTOM_GRID; rval= 1;	}
	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Retrieve the borders and shading of a cell.				*/
/*									*/
/************************************************************************/

void docGetCellOrnaments(
			BlockOrnaments *		cellOrnaments,
			DocumentRectangle *		drOutside,
			DocumentRectangle *		drInside,
			const DocumentRectangle *	drCell,
			const BufferDocument *		bd,
			const BufferItem *		rowNode,
			int				col,
			int				atRowTop,
			int				atRowBottom,
			int				drawTableGrid )
    {
    const CellProperties *	cp= rowNode->biRowCells+ col;
    const RowProperties *	rp= &(rowNode->biRowProperties);
    int				shadingNumber;

    int				prop= 0;

    *drOutside= *drCell;
    *drInside= *drCell;

    /*  3  */
    shadingNumber= cp->cpShadingNumber;
    if  ( shadingNumber == 0 )
	{ shadingNumber= rp->rpShadingNumber;	}

    if  ( shadingNumber != 0 )
	{
	docGetItemShadingByNumber( &(cellOrnaments->boShading), bd,
							    shadingNumber );

	PROPmaskADD( &(cellOrnaments->boPropMask), ORNdrawSHADE );
	}

    if  ( docCellTopBorderDrawn( &(cellOrnaments->boTopBorder),
		    &(cellOrnaments->boTopBorderNumber),
		    &prop, drOutside, drInside, drCell,
		    bd, rowNode, col, atRowTop, drawTableGrid ) )
	{ PROPmaskADD( &(cellOrnaments->boPropMask), prop );	}

    if  ( docCellLeftBorderDrawn( &(cellOrnaments->boLeftBorder),
		    &(cellOrnaments->boLeftBorderNumber),
		    &prop, drOutside, drInside, drCell,
		    bd, rowNode, col, drawTableGrid ) )
	{ PROPmaskADD( &(cellOrnaments->boPropMask), prop );	}

    if  ( docCellRightBorderDrawn( &(cellOrnaments->boRightBorder),
		    &(cellOrnaments->boRightBorderNumber),
		    &prop, drOutside, drInside, drCell,
		    bd, rowNode, col, drawTableGrid ) )
	{ PROPmaskADD( &(cellOrnaments->boPropMask), prop );	}

    if  ( docCellBottomBorderDrawn( &(cellOrnaments->boBottomBorder),
		    &(cellOrnaments->boBottomBorderNumber),
		    &prop, drOutside, drInside, drCell,
		    bd, rowNode, col, atRowBottom, drawTableGrid ) )
	{ PROPmaskADD( &(cellOrnaments->boPropMask), prop );	}

    return;
    }

/************************************************************************/
/*									*/
/*  Calculate layout insets for a (table) row.				*/
/*									*/
/*  This routine is also used to calculate the inset that is used to	*/
/*  accomodate space for the bottom border of the previous row. This	*/
/*  inset is not only used in real rows, but also in the immediate	*/
/*  successor of a real row.						*/
/*									*/
/*  1)  Reserve space for the top border of all cells.			*/
/*									*/
/************************************************************************/

void docLayoutCalculateRowTopInset(	int *			pInset,
					const BufferDocument *	bd,
					const BufferItem *	rowNode,
					int			atRowTop )
    {
    int				col;
    const CellProperties *	cp;

    *pInset= 0;

    /*  1  */
    cp= rowNode->biRowCells;
    for ( col= 0; col < rowNode->biChildCount; cp++, col++ )
	{
	int			useAbove= 0;
	BorderProperties	bpTop;
	int			topNr;

	if  ( CELL_MERGED( cp ) )
	    { continue;	}

	docGetCellTopBorder( &bpTop, &topNr,
					&useAbove, bd, rowNode, col, atRowTop );

	docStretchInsetForBorder( pInset, &bpTop );
	}

    return;
    }

