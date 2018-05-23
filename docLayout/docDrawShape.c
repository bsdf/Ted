/************************************************************************/
/*									*/
/*  Ted, generic shape drawing functionality.				*/
/*									*/
/************************************************************************/

#   include	"docLayoutConfig.h"

#   include	<stddef.h>
#   include	<limits.h>

#   include	"docDraw.h"
#   include	"docLayout.h"
#   include	<docShape.h>

#   include	<appDebugon.h>

int docDrawShapeGetFill(	int *			pFill,
				RGB8Color *		rgb8,
				const DrawingShape *	ds,
				DrawingContext *	dc,
				void *			through )
    {
    int		fill= 1;

    if  ( docShapeGetFill( &fill, rgb8, ds ) )
	{ LDEB(1); return -1;	}

    if  ( fill )
	{ docDrawSetColorRgb( dc, through, rgb8 );	}

    *pFill= fill;
    return 0;
    }

int docDrawShapeGetLine(	int *			pLine,
				RGB8Color *		rgb8,
				const DrawingShape *	ds,
				DrawingContext *	dc,
				void *			through )
    {
    int		line= 1;

    if  ( docShapeGetLine( &line, rgb8, ds ) )
	{ LDEB(1); return -1;	}

    if  ( line )
	{ docDrawSetColorRgb( dc, through, rgb8 );	}

    *pLine= line;
    return 0;
    }


/************************************************************************/
/*									*/
/*  Traverse a tree of shapes and draw the leaves.			*/
/*									*/
/*  1)  Clip leaf shapes: It is unknown whether chidren can protrude	*/
/*	from their parents. As a matter of fact, it is even unclear	*/
/*	whether a free form can leave its rectangle.			*/
/*									*/
/************************************************************************/

static int docDrawDrawingShape(	const DocumentRectangle *	drTwips,
				const struct BufferItem *	bodySectNode,
				int				page,
				int				column,
				DrawingShape *			ds,
				DrawingContext *		dc,
				void *				through )
    {
    const LayoutContext *	lc= &(dc->dcLayoutContext);
    int				rval= 0;

    DocumentRectangle		drHere;
    DocumentRectangle		drNorm;

    docShapeGetRects( &drHere, &drNorm, drTwips, ds );

    if  ( ds->dsDrawing.sdShapeType == SHPtyGROUP )
	{
	int			child;

	for ( child= 0; child < ds->dsChildCount; child++ )
	    {
	    DrawingShape *	dsChild= ds->dsChildren[child];
	    DocumentRectangle	drChild;

	    docShapeGetChildRect( &drChild, dsChild, &drHere, ds );

	    if  ( docDrawDrawingShape( &drChild, bodySectNode, page, column,
						    dsChild, dc, through ) )
		{ LDEB(child); rval= -1;	}
	    }
	}
    else{
	if  ( dc->dcClipRect )
	    {
	    DocumentRectangle	drPixels;

	    docGetPixelRect( &drPixels, lc, drTwips, page );
	    geoNormalizeRectangle( &drPixels, &drPixels );

	    if  ( ! geoIntersectRectangle( (DocumentRectangle *)0,
						dc->dcClipRect, &drPixels ) )
		{ return 0;	}
	    }

	if  ( dc->dcDrawShape						&&
	      (*dc->dcDrawShape)( drTwips, page, ds, dc, through )	)
	    { LDEB(1); rval= -1;	}

	dc->dcCurrentTextAttributeSet= 0;
	dc->dcCurrentColorSet= 0;

	if  ( ds->dsDrawing.sd_WrapText == SHAPEtextWRAP_NONE )
	    { drHere.drX1= INT_MAX;	}

	if  ( ds->dsDocumentTree.dtRoot					&&
	      docDrawShapeText( &drHere, bodySectNode,
					page, column, ds, dc, through )	)
	    { LDEB(page); rval= -1;	}
	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Draw the text inside a shape.					*/
/*									*/
/************************************************************************/

int docDrawShapeText(	const DocumentRectangle *	drHere,
			const struct BufferItem *	bodySectNode,
			int				page,
			int				column,
			DrawingShape *			ds,
			DrawingContext *		dc,
			void *				through )
    {
    const LayoutContext *	lc= &(dc->dcLayoutContext);

    LayoutPosition		lpBelow;

    docInitLayoutPosition( &lpBelow );

    if  ( page != ds->dsDocumentTree.dtPageFormattedFor		||
	  column != ds->dsDocumentTree.dtColumnFormattedFor	)
	{
	if  ( docShapeCheckTextLayout( ds, drHere, (DocumentRectangle *)0,
		    bodySectNode, page, column, lc, dc->dcInitLayoutExternal ) )
	    { LDEB(page); return -1;	}
	}

    if  ( docDrawNode( &lpBelow, ds->dsDocumentTree.dtRoot, through, dc ) )
	{ LDEB(1);	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Draw the root shape of an object. I.E: The shape that is directly	*/
/*  inserted in the document. Its children are reached through		*/
/*  recursion.								*/
/*									*/
/************************************************************************/

int docDrawShape(	DrawingContext *		dc,
			void *				through,
			const struct BufferItem *	bodySectNode,
			const InsertedObject *		io )
    {
    const LayoutContext *	lc= &(dc->dcLayoutContext);
    DrawingShape *		ds= io->ioDrawingShape;

    int				page= io->ioY0Position.lpPage;
    int				column= io->ioY0Position.lpColumn;

    DocumentRectangle		drTwips;

    if  ( ! ds )
	{ XDEB(ds); return 0;	}

    docPlaceRootShapeRect( &drTwips, &(io->ioDrawingShape->dsShapeProperties),
				io->ioX0Twips, io->ioY0Position.lpPageYTwips );

    if  ( dc->dcClipRect )
	{
	DocumentRectangle	drPixels;

	docGetPixelRect( &drPixels, lc, &drTwips, page );
	geoNormalizeRectangle( &drPixels, &drPixels );

	if  ( ! geoIntersectRectangle( (DocumentRectangle *)0,
					    &drPixels, dc->dcClipRect ) )
	    { return 0;	}
	}

    docDrawDrawingShape( &drTwips, bodySectNode, page, column,
							ds, dc, through );
    return 0;
    }

