/************************************************************************/
/*									*/
/*  Layout utility functions relating to DrawingShapes			*/
/*									*/
/************************************************************************/

#   include	"docLayoutConfig.h"

#   include	<stddef.h>

#   include	<docPageGrid.h>
#   include	<docTreeNode.h>
#   include	<docShape.h>
#   include	"docLayout.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Remember that the layout calculations for a shape must be redone.	*/
/*									*/
/************************************************************************/

void docDrawingShapeInvalidateTextLayout(	DrawingShape *	ds )
    {
    int		child;

    docInvalidateTreeLayout( &(ds->dsDocumentTree) );

    for ( child= 0; child < ds->dsChildCount; child++ )
	{ docDrawingShapeInvalidateTextLayout( ds->dsChildren[child] ); }

    return;
    }

/************************************************************************/
/*									*/
/*  Calculate the page rectangle for a shape.				*/
/*									*/
/************************************************************************/

void docShapePageY(		LayoutPosition *	lpShapeTop,
				LayoutPosition *	lpBelowShape,
				BlockFrame *		bfShape,
				const DrawingShape *	ds,
				const BufferItem *	paraBi,
				const LayoutPosition *	lpLineTop,
				const BlockFrame *	bf )
    {
    const ShapeProperties *	sp= &(ds->dsShapeProperties);
    int				yRefProp= sp->spYReference;

    DocumentRectangle		drShape;
    int				shapeHigh;

    docPlaceRootShapeRect( &drShape, &(ds->dsShapeProperties), 0, 0 );
    shapeHigh= drShape.drY1= drShape.drY0+ 1;

    if  ( yRefProp == FYrefIGNORE )
	{ yRefProp= ds->dsDrawing.sdYReference; }

    *lpShapeTop= *lpLineTop;

    docLayoutFrameY( bfShape,
			yRefProp, ds->dsDrawing.sdYPosition, sp->spRect.drY0,
			lpLineTop, &(paraBi->biTopPosition),
			bf, -shapeHigh, shapeHigh );

    lpShapeTop->lpPageYTwips= bfShape->bfContentRect.drY0;

    *lpBelowShape= *lpShapeTop;
    lpBelowShape->lpPageYTwips += shapeHigh;

    return;
    }

void docShapePageRectangle(	LayoutPosition *	lpShapeTop,
				LayoutPosition *	lpBelowShape,
				int *			pX0,
				int *			pX1,
				const DrawingShape *	ds,
				const BufferItem *	paraBi,
				const LayoutPosition *	lpLineTop,
				const ParagraphFrame *	pfRef,
				const BlockFrame *	bfRef,
				int			xChar )
    {
    const ShapeProperties *	sp= &(ds->dsShapeProperties);
    int				xRefProp= sp->spXReference;

    DocumentRectangle		drShape;
    int				shapeWide;
    BlockFrame			bfShape;

    docLayoutInitBlockFrame( &bfShape );

    docPlaceRootShapeRect( &drShape, &(ds->dsShapeProperties), 0, 0 );
    shapeWide= drShape.drX1= drShape.drX0+ 1;

    docShapePageY( lpShapeTop, lpBelowShape, &bfShape,
					    ds, paraBi, lpLineTop, bfRef );

    if  ( xRefProp == FXrefIGNORE )
	{ xRefProp= ds->dsDrawing.sdXReference; }

    docLayoutFrameX( &bfShape,
	    xRefProp, ds->dsDrawing.sdXPosition, sp->spRect.drX0,
	    paraBi->biParaTableNesting > 0 && ds->dsDrawing.sd_fLayoutInCell,
	    pfRef, bfRef, xChar, shapeWide );

    *pX0= bfShape.bfContentRect.drX0;
    *pX1= bfShape.bfContentRect.drX0+ shapeWide;

    docLayoutCleanBlockFrame( &bfShape );

    return;
    }

/************************************************************************/
/*									*/
/*  Recalculate the layout of a text box (shptxt) if necessary.		*/
/*									*/
/************************************************************************/

int docShapeCheckTextLayout(
			DrawingShape *			ds,
			const DocumentRectangle *	drTwips,
			DocumentRectangle *		drChanged,
			const BufferItem *		bodySectNode,
			int				page,
			int				column,
			const LayoutContext *		lc,
			INIT_LAYOUT_EXTERNAL		initLayoutExternal )
    {
    DocumentGeometry	dgBox;

    int			wide;

    utilInitDocumentGeometry( &dgBox );

    if  ( drTwips->drX0 <= drTwips->drX1 )
	{
	wide= drTwips->drX1- drTwips->drX0;
	dgBox.dgLeftMarginTwips= drTwips->drX0;
	}
    else{
	wide= drTwips->drX0- drTwips->drX1;
	dgBox.dgLeftMarginTwips= drTwips->drX1;
	}

    if  ( drTwips->drY0 <= drTwips->drY1 )
	{
	/* high= drTwips->drY1- drTwips->drY0; */
	dgBox.dgTopMarginTwips= drTwips->drY0;
	}
    else{
	/* high= drTwips->drY0- drTwips->drY1; */
	dgBox.dgTopMarginTwips= drTwips->drY1;
	}

    dgBox.dgRightMarginTwips=  dgBox.dgPageWideTwips- 
					dgBox.dgLeftMarginTwips- wide;

    dgBox.dgLeftMarginTwips += EMUtoTWIPS( ds->dsDrawing.sd_dxTextLeft );
    dgBox.dgTopMarginTwips += EMUtoTWIPS( ds->dsDrawing.sd_dyTextTop );
    dgBox.dgRightMarginTwips += EMUtoTWIPS( ds->dsDrawing.sd_dxTextRight );
    /* Irrelevant: */
    dgBox.dgBottomMarginTwips= 0;

    ds->dsDocumentTree.dtRoot->biSectDocumentGeometry= dgBox;

    if  ( page != ds->dsDocumentTree.dtPageFormattedFor		||
	  column != ds->dsDocumentTree.dtColumnFormattedFor	)
	{
	/*  We do not expect the tree to change height here	*/
	const int	adjustDocument= 0;

	if  ( docLayoutDocumentTree( &(ds->dsDocumentTree),
		drChanged, page, column, dgBox.dgTopMarginTwips,
		bodySectNode, lc, initLayoutExternal, adjustDocument ) )
	    { LLDEB(page,column); return -1;	}
	}

    return 0;
    }
