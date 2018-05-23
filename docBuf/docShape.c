/************************************************************************/
/*									*/
/*  Shapes.. Administration.						*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	<stdlib.h>
#   include	<math.h>

#   include	<geoAffineTransform.h>

#   include	"docShape.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Return the rectangle and its normalized version for a shape .	*/
/*									*/
/************************************************************************/

void docShapeGetRects(	DocumentRectangle *		drHere,
			DocumentRectangle *		drNorm,
			const DocumentRectangle *	drTwips,
			const DrawingShape *		ds )
    {
    *drHere= *drTwips;

    if  ( DSflipHORIZONTAL( ds ) )
	{ int sw= drHere->drX0; drHere->drX0= drHere->drX1; drHere->drX1= sw; }

    if  ( DSflipVERTICAL( ds ) )
	{ int sw= drHere->drY0; drHere->drY0= drHere->drY1; drHere->drY1= sw; }

    geoNormalizeRectangle( drNorm, drHere );

    return;
    }

/************************************************************************/
/*									*/
/*  Calculate the rectangle for a child shape.				*/
/*									*/
/************************************************************************/

void docShapeGetChildRect(	DocumentRectangle *		drChild,
				const DrawingShape *		dsChild,
				const DocumentRectangle *	dr,
				const DrawingShape *		ds )
    {
    const ShapeDrawing *	sd= &(ds->dsDrawing);
    const ShapeDrawing *	sdChild= &(dsChild->dsDrawing);

    double			xscale;
    double			yscale;

    xscale= ( 1.0* ( dr->drX1- dr->drX0 ) )/
				( sd->sdGroupRect.drX1- sd->sdGroupRect.drX0 );
    yscale= ( 1.0* ( dr->drY1- dr->drY0 ) )/
				( sd->sdGroupRect.drY1- sd->sdGroupRect.drY0 );

    drChild->drX0= dr->drX0+ xscale* 
			    ( sdChild->sdRelRect.drX0- sd->sdGroupRect.drX0 );
    drChild->drY0= dr->drY0+ yscale*
			    ( sdChild->sdRelRect.drY0- sd->sdGroupRect.drY0 );
    drChild->drX1= dr->drX0+ xscale* 
			    ( sdChild->sdRelRect.drX1- sd->sdGroupRect.drX0 );
    drChild->drY1= dr->drY0+ yscale*
			    ( sdChild->sdRelRect.drY1- sd->sdGroupRect.drY0 );

    return;
    }

/************************************************************************/
/*									*/
/*  Get the transform to draw this shape.				*/
/*									*/
/************************************************************************/

void docShapeStartShapeTransform(
				AffineTransform2D *		at,
				const DrawingShape *		ds,
				const DocumentRectangle *	dr,
				int				xSize,
				int				ySize )
    {
    double			xs= 1.0;
    double			ys= 1.0;

    AffineTransform2D		att;

    if  ( dr->drX1 != dr->drX0 )
	{ xs= ( dr->drX1- dr->drX0 )/ ( 1.0* xSize );	}
    if  ( dr->drY1 != dr->drY0 )
	{ ys= ( dr->drY1- dr->drY0 )/ ( 1.0* ySize );	}

    geoScaleAffineTransform2D( at, xs, ys );

    geoTranslationAffineTransform2D( &att,
				( dr->drX1+ dr->drX0 )/ 2.0,
				( dr->drY1+ dr->drY0 )/ 2.0 );
    geoAffineTransform2DProduct( at, &att, at );

    docShapeInternalTransform( &att, ds );
    geoAffineTransform2DProduct( at, at, &att );

    return;
    }


/************************************************************************/
/*									*/
/*  Return the transform for a shape.					*/
/*									*/
/************************************************************************/

void docShapeInternalTransform(	AffineTransform2D *	at,
				const DrawingShape *	ds )
    {
    const ShapeDrawing *	sd= &(ds->dsDrawing);

    if  ( sd->sdRotation != 0 )
	{
	geoRotationAffineTransform2D( at, ( M_PI* sd->sdRotation )/
							( 180.0 * 65536.0 ) );
	}
    else{
	geoIdentityAffineTransform2D( at );
	}

    if  ( DSflipHORIZONTAL( ds ) )
	{
	AffineTransform2D	att;

	geoScaleAffineTransform2D( &att, -1.0, 1.0 );
	geoAffineTransform2DProduct( at, at, &att );
	}

    if  ( DSflipVERTICAL( ds ) )
	{
	AffineTransform2D	att;

	geoScaleAffineTransform2D( &att, 1.0, -1.0 );
	geoAffineTransform2DProduct( at, at, &att );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Get the fill color of a shape.					*/
/*									*/
/************************************************************************/

int docShapeGetFill(	int *			pFill,
			RGB8Color *		rgb8,
			const DrawingShape *	ds )
    {
    const ShapeDrawing *	sd= &(ds->dsDrawing);
    int				fill= 1;

    if  ( ! sd->sd_fFilled )
	{ *pFill= 0; return 0; }
    if  ( sd->sdFillOpacity == 0 )
	{ *pFill= 0; return 0; }

    switch( sd->sdFillType )
	{
	case DSfillSOLID:
	    *rgb8= sd->sdFillColor;
	    fill= 1;
	    break;

	case DSfillSHADE_START_TO_END:
	case DSfillSHADE_BOUNDS_TO_END:
	case DSfillSHADE_OUTLINE_TO_END:
	case DSfillSHADE_ANGLE:
	    /*LDEB(sd->sdFillType);*/
	    *rgb8= sd->sdFillColor;
	    fill= 1;
	    break;

	default:
	    LDEB(sd->sdFillType);
	    return -1;
	}

    *pFill= fill;
    return 0;
    }

/************************************************************************/
/*									*/
/*  Get the line/stroke color of a shape.				*/
/*									*/
/************************************************************************/

int docShapeGetLine(	int *			pLine,
			RGB8Color *		rgb8,
			const DrawingShape *	ds )
    {
    const ShapeDrawing *	sd= &(ds->dsDrawing);
    int				line= 1;

    if  ( ! sd->sd_fLine )
	{ *pLine= 0; return 0; }

    switch( sd->sdLineType )
	{
	case DSlineSOLID:
	    *rgb8= sd->sdLineColor;
	    line= 1;
	    break;

	default:
	    LDEB(sd->sdLineType);
	    return -1;
	}

    *pLine= line;
    return 0;
    }

/************************************************************************/
/*									*/
/*  Initialize/Clean a shape.						*/
/*									*/
/************************************************************************/

void docCleanDrawingShape(	DrawingShape *		ds )
    {
    utilCleanMemoryBuffer( &(ds->dsPictureData) );

    /* NO! The children will be cleaned via the paged list callbacks
    for ( i= 0; i < ds->dsChildCount; i++ )
	{
	if  ( ds->dsChildren[i] )
	    { docCleanDrawingShape( ds->dsChildren[i] );	}
	}
    */

    if  ( ds->dsChildren )
	{ free( ds->dsChildren ); }

    docCleanShapeDrawing( &(ds->dsDrawing) );

    if  ( ds->dsDocumentTree.dtRoot )
	{ LPDEB(ds->dsShapeNumber,ds->dsDocumentTree.dtRoot);	}

    bmCleanRasterImage( &(ds->dsRasterImage) );

    return;
    }

void docInitShapeAllocated(	DrawingShape *	ds )
    {
    docInitShapeDrawingAllocated( &(ds->dsDrawing) );

    utilInitMemoryBuffer( &(ds->dsPictureData) );

    ds->dsChildCount= 0;
    ds->dsChildren= (DrawingShape **)0;

    docInitSelectionScope( &(ds->dsSelectionScope) );
    docInitDocumentTree( &(ds->dsDocumentTree) );

    ds->dsDrawingSurface= (struct DrawingSurface *)0;
    bmInitRasterImage( &(ds->dsRasterImage) );

    return;
    }

void docInitDrawingShape(	DrawingShape *	ds )
    {
			/************************************************/
			/*  Backbone.					*/
			/************************************************/
    docInitShapeAllocated( ds );
    ds->dsShapeNumber= -1;

			/************************************************/
			/*  Properties.					*/
			/************************************************/

    docInitShapeProperties( &(ds->dsShapeProperties) );

			/************************************************/
			/*  Position.					*/
			/************************************************/
    ds->dsIsChildShape= 0;

			/************************************************/
			/*  Drawing.					*/
			/************************************************/
    docInitShapeDrawing( &(ds->dsDrawing) );

    docInitPictureProperties( &(ds->dsPictureProperties) );

    return;
    }
