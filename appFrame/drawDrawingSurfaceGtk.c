/************************************************************************/
/*									*/
/*  Drawing surface implementation implementation.			*/
/*									*/
/************************************************************************/

#   include "appFrameConfig.h"

#   include "guiWidgetDrawingSurface.h"
#   include <drawDrawingSurface.h>
#   include "drawDrawingSurfaceImpl.h"
#   include "drawScreenFontImpl.h"
#   include "drawTextImplX11.h"
#   include "drawScreenFontAdmin.h"
#   include "drawUtilGtk.h"
#   include "drawImplXft.h"
#   include "drawImpl.h"
#   include <appDebugon.h>

#   ifdef USE_GTK

/************************************************************************/

DrawingSurface drawMakeDrawingSurfaceForParent(
				DrawingSurface			parent,
				int				wide,
				int				high )
    {
    int			depth= gdk_visual_get_system()->depth;
    DrawingSurface	ds= malloc(sizeof(struct DrawingSurface));

    if  ( ! ds )
	{ PDEB(ds); return ds;	}
    drawInitDrawingSurface( ds );

    ds->dsColors= parent->dsColors;
    ds->dsDrawable= gdk_pixmap_new( parent->dsDrawable, wide, high, depth );
    ds->dsAvoidFontconfig= parent->dsAvoidFontconfig;
    ds->dsIsPixmap= 1;

    if  ( ! ds->dsDrawable )
	{
	XDEB(ds->dsDrawable);
	drawFreeDrawingSurface( ds );
	return (DrawingSurface)0;
	}

    ds->dsGc= gdk_gc_new( ds->dsDrawable );
    if  ( ! ds->dsGc )
	{
	XDEB(ds->dsGc);
	drawFreeDrawingSurface( ds );
	return (DrawingSurface)0;
	}

#   ifdef USE_XFT
    if  ( ! ds->dsAvoidFontconfig )
	{
	ds->dsXftDrawable= appGtkXftDrawCreate(
				ds->dsDrawable, &(ds->dsXftColorList) );
	if  ( ! ds->dsXftDrawable )
	    { XDEB(ds->dsXftDrawable);	}
	}
#   endif

    return ds;
    }

DrawingSurface drawMakeDrawingSurfaceForImageAndParent(
				DrawingSurface			parent,
				const RasterImage *		abi,
				const DocumentRectangle *	drSrc,
				int				wide,
				int				high )
    {
    DrawingSurface	ds;
    APP_IMAGE *		xim= (APP_IMAGE *)0;

    ds= drawMakeDrawingSurfaceForParent( parent, wide, high );
    if  ( ! ds )
	{ XDEB(ds); return ds;	}

    if  ( drawUtilGtkMakeImage( &xim, wide, high, ds->dsColors, abi, drSrc ) )
	{ drawFreeDrawingSurface( ds ); return (DrawingSurface)0;	}

    gdk_draw_image( ds->dsDrawable, ds->dsGc, xim, 0, 0, 0, 0, wide, high );

    gdk_image_destroy( xim );

    return ds;
    }

/************************************************************************/

int drawRasterImage(		DrawingSurface			ds,
				const DocumentRectangle *	drDest,
				const RasterImage *		abi,
				const DocumentRectangle *	drSrc )
    {
    APP_IMAGE *		xim= (APP_IMAGE *)0;
    int			pixelsWide= drDest->drX1- drDest->drX0+ 1;
    int			pixelsHigh= drDest->drY1- drDest->drY0+ 1;

    if  ( drawUtilGtkMakeImage( &xim, pixelsWide, pixelsHigh,
						ds->dsColors, abi, drSrc ) )
	{ LLDEB(pixelsWide,pixelsHigh); return -1;	}

    gdk_draw_image( ds->dsDrawable, ds->dsGc, xim, 0, 0,
						drDest->drX0, drDest->drY0,
						pixelsWide, pixelsHigh );

    gdk_image_destroy( xim );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Cause subsequent drawing to be done in a certain color.		*/
/*									*/
/************************************************************************/

int drawSetForegroundColor(	DrawingSurface		ds,
				const RGB8Color *	rgb8 )
    {
    APP_COLOR_RGB		xc;

    if  ( appColorRgb( &xc, ds->dsColors,
				    rgb8->rgb8Red,
				    rgb8->rgb8Green,
				    rgb8->rgb8Blue ) )
	{ LDEB(1); return -1;	}

    gdk_gc_set_foreground( ds->dsGc, &xc );

#   ifdef USE_XFT
    appSolidXftColor( &(ds->dsXftColorList.axclCurrentColor), &xc );
#   endif

    return 0;
    }

/************************************************************************/
/*									*/
/*  Destroy a drawing surface. Only free resources allocated by the	*/
/*  DrawingSurface itself.						*/
/*									*/
/*  1)  Older versions of GTK/XFT crash here if the cleaning action is	*/
/*	called from a destroy callback. MdD Jun 2007.			*/
/*									*/
/************************************************************************/

void drawFreeDrawingSurface(	DrawingSurface		ds )
    {
#   ifdef USE_XFT
    appCleanAppXftColorList( &(ds->dsXftColorList) );

    /*  1  */
    if  ( ds->dsXftDrawable )
	{ XftDrawDestroy( ds->dsXftDrawable );	}
#   endif

    if  ( ds->dsGc )
	{ gdk_gc_unref( ds->dsGc );	}

    if  ( ds->dsPoints )
	{ free( ds->dsPoints );	}

    if  ( ds->dsDrawable && ds->dsIsPixmap )
	{ gdk_pixmap_unref( ds->dsDrawable );	}

    free( ds );
    }

/************************************************************************/
/*									*/
/*  Fill a rectangle.							*/
/*									*/
/************************************************************************/

void drawFillRectangle(	DrawingSurface			ds,
			const DocumentRectangle *	dr )
    {
#   ifdef USE_XFT
    if  ( ! drawFillRectangleXft( ds->dsXftDrawable,
						&(ds->dsXftColorList), dr ) )
	{ return;	}
#   endif

    gdk_draw_rectangle( ds->dsDrawable, ds->dsGc, TRUE,
			dr->drX0, dr->drY0,
			dr->drX1- dr->drX0+ 1, dr->drY1- dr->drY0+ 1 );
    }

void drawRectangle(	DrawingSurface			ds,
			const DocumentRectangle *	dr )
    {
    gdk_draw_rectangle( ds->dsDrawable, ds->dsGc, FALSE,
			dr->drX0, dr->drY0,
			dr->drX1- dr->drX0+ 1, dr->drY1- dr->drY0+ 1 );
    }

/************************************************************************/
/*									*/
/*  Draw a line.							*/
/*									*/
/************************************************************************/

int drawLine(		DrawingSurface			ds,
			int				x0,
			int				y0,
			int				x1,
			int				y1 )
    {
    gdk_draw_line( ds->dsDrawable, ds->dsGc, x0, y0, x1, y1 );
    return 0;
    }

/************************************************************************/
/*									*/
/*  Convert points to native type.					*/
/*									*/
/************************************************************************/

static int drawSetPoints(	DrawingSurface		ds,
				const Point2DI *	points,
				int			pointCount )
    {
    int			i;

    if  ( ds->dsPointCount < pointCount )
	{
	APP_POINT *	fresh= (APP_POINT *)realloc( ds->dsPoints,
					    pointCount* sizeof(APP_POINT) );
	if  ( ! fresh )
	    { LXDEB(pointCount,fresh); return -1;	}

	ds->dsPoints= fresh;
	ds->dsPointCount= pointCount;
	}

    for ( i= 0; i < pointCount; i++ )
	{
	ds->dsPoints[i].x= points[i].x;
	ds->dsPoints[i].y= points[i].y;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Draw a series of lines.						*/
/*									*/
/************************************************************************/

int drawLines(		DrawingSurface			ds,
			const Point2DI *		points,
			int				pointCount,
			int				close )
    {
    int		i;

    if  ( drawSetPoints( ds, points, pointCount ) )
	{ LDEB(pointCount); return -1;	}

    pointCount--;

    for ( i= 0; i < pointCount; i++ )
	{
	gdk_draw_line( ds->dsDrawable, ds->dsGc,
				    ds->dsPoints[i+0].x, ds->dsPoints[i+0].y,
				    ds->dsPoints[i+1].x, ds->dsPoints[i+1].y );
	}

    if  ( close )
	{
	gdk_draw_line( ds->dsDrawable, ds->dsGc,
				    ds->dsPoints[i].x, ds->dsPoints[i].y,
				    ds->dsPoints[0].x, ds->dsPoints[0].y );
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Fill a polygon.							*/
/*									*/
/************************************************************************/

int drawFillPolygon(	DrawingSurface			ds,
			const Point2DI *		points,
			int				pointCount )
    {
    if  ( drawSetPoints( ds, points, pointCount ) )
	{ LDEB(pointCount); return -1;	}

    gdk_draw_polygon( ds->dsDrawable, ds->dsGc, TRUE,
						ds->dsPoints, pointCount );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Set line attributes.						*/
/*									*/
/************************************************************************/

static const int LineStyleMap[LineStyle_Count]=
    { GDK_LINE_SOLID, GDK_LINE_ON_OFF_DASH, GDK_LINE_DOUBLE_DASH,	};

static const int CapStyleMap[LineCap_Count]=
    { GDK_CAP_NOT_LAST, GDK_CAP_BUTT, GDK_CAP_ROUND, GDK_CAP_PROJECTING	};

static const int JoinStyleMap[LineJoin_Count]=
    { GDK_JOIN_MITER, GDK_JOIN_ROUND, GDK_JOIN_BEVEL	};

int drawSetLineAttributes(	DrawingSurface		ds,
				int			lineWidth,
				int			lineStyle,
				int			capStyle,
				int			joinStyle,
				const unsigned char *	dashList,
				int			dashCount )
    {
    gdk_gc_set_line_attributes( ds->dsGc, lineWidth,
				LineStyleMap[lineStyle],
				CapStyleMap[capStyle],
				JoinStyleMap[joinStyle] );

    if  ( lineStyle != LineStyleSolid )
	{
	if  ( ! dashList || dashCount == 0 )
	    { XLDEB(dashList,dashCount);	}
	else{
	    gdk_gc_set_dashes( ds->dsGc, 0, (gint8 *)dashList, dashCount );
	    }
	}

    return 0;
    }

void drawArc(			DrawingSurface		ds,
				const Arc2DI *		arc )
    {
    gdk_draw_arc( ds->dsDrawable, ds->dsGc, FALSE,
				    arc->a2diRect.drX0, arc->a2diRect.drY0,
				    arc->a2diRect.drX1- arc->a2diRect.drX0+ 1,
				    arc->a2diRect.drY1- arc->a2diRect.drY0+ 1,
				    arc->a2diAngleFrom, arc->a2diAngleStep );
    }

void drawFillArc(		DrawingSurface		ds,
				const Arc2DI *		arc )
    {
    gdk_draw_arc( ds->dsDrawable, ds->dsGc, TRUE,
				    arc->a2diRect.drX0, arc->a2diRect.drY0,
				    arc->a2diRect.drX1- arc->a2diRect.drX0+ 1,
				    arc->a2diRect.drY1- arc->a2diRect.drY0+ 1,
				    arc->a2diAngleFrom, arc->a2diAngleStep );
    }

/************************************************************************/
/*									*/
/*  Set a cached system color as the current foreground color.		*/
/*									*/
/************************************************************************/

void drawSetSystemColor(	DrawingSurface			ds,
				APP_COLOR_RGB *			xc )
    {
    gdk_gc_set_foreground( ds->dsGc, xc );

#   ifdef USE_XFT
    appSolidXftColor( &(ds->dsXftColorList.axclCurrentColor), xc );
#   endif
    }

/************************************************************************/
/*									*/
/*  Draw a string.							*/
/*									*/
/************************************************************************/

int drawString(		DrawingSurface			ds,
			int				x0,
			int				y0,
			int				screenFont,
			const char *			s,
			int				len )
    {
    const NumberedPropertiesList *	npl= &(ds->dsScreenFontAdmin);
    DrawScreenFont *			dsf;

    TextProgress			tp;

    dsf= drawGetScreenFontByNumber( npl, screenFont );
    if  ( ! dsf )
	{ LXDEB(screenFont,dsf); return -1;	}

    /* HACK: Font might have been allocated before the widget was realized */
    if  ( ! dsf->dsfDrawable )
	{ dsf->dsfDrawable= ds->dsDrawable;	}
    if  ( ! dsf->dsfGc )
	{ dsf->dsfGc= ds->dsGc;	}

#   ifdef USE_XFT
    if  ( ! dsf->dsfXftDrawable )
	{ dsf->dsfXftDrawable= ds->dsXftDrawable;	}

    if  ( dsf->dsfXftDrawable						&&
	  dsf->dsfXftFont						&&
	  ! drawStringXft( dsf, &(ds->dsXftColorList), x0, y0, s, len )	)
	{ return 0;	}
#   endif

    tp.tpX= x0;
    tp.tpY= y0;

    drawHandleTextSegmentsX11( &tp, dsf, s, len, drawTextSegment8X11,
			    drawTextSegment16X11, &(dsf->dsfEncodedFonts) );

    return 0;
    }

#   ifdef USE_GTK
#   include	<gdk/gdkx.h>
#   endif

int drawOpenScreenFont(		DrawingSurface			ds,
				const AfmFontInfo *		afi,
				int				pixelSize,
				const IndexSet *		unicodesWanted)
    {
    DrawScreenFont *	dsf;
    int			fresh;
    int			screenFont;

    screenFont= drawGetScreenFont( &dsf, &fresh, &(ds->dsScreenFontAdmin),
					    unicodesWanted, afi, pixelSize );
    if  ( screenFont < 0 )
	{ LDEB(screenFont); return -1;	}

    if  ( fresh )
	{
	if  ( ! dsf->dsfDisplay )
	    { dsf->dsfDisplay= GDK_DISPLAY();	}
	if  ( ! dsf->dsfDrawable )
	    { dsf->dsfDrawable= ds->dsDrawable;	}
	if  ( ! dsf->dsfGc )
	    { dsf->dsfGc= ds->dsGc;	}

	if  ( drawFontOpenScreenFont( dsf, ds->dsAvoidFontconfig ) )
	    { LDEB(1); return -1;	}
	}

    return screenFont;
    }

void drawNoClipping(		DrawingSurface		ds )
    {
#   ifdef USE_XFT
    if  ( ds->dsXftDrawable )
	{
	XftDrawSetClip( ds->dsXftDrawable, 0 );
	}
#   endif

    gdk_gc_set_clip_rectangle( ds->dsGc, (GdkRectangle *)0 );

    return;
    }

void drawSetClipRect(	DrawingSurface			ds,
			const DocumentRectangle *	drClip )
    {
    GdkRectangle		gRect;

    gRect.x= drClip->drX0;
    gRect.y= drClip->drY0;
    gRect.width= drClip->drX1- drClip->drX0+ 1;
    gRect.height= drClip->drY1- drClip->drY0+ 1;

#   ifdef USE_XFT
    if  ( ds->dsXftDrawable )
	{
	XRectangle		xRect;

	xRect.x= drClip->drX0;
	xRect.y= drClip->drY0;
	xRect.width= drClip->drX1- drClip->drX0+ 1;
	xRect.height= drClip->drY1- drClip->drY0+ 1;

	XftDrawSetClipRectangles( ds->dsXftDrawable, 0, 0, &xRect, 1 );
	}
#   endif

    gdk_gc_set_clip_rectangle( ds->dsGc, &gRect );

    return;
    }

/************************************************************************/

void drawChildSurface(		DrawingSurface			ds,
				const DrawingSurface		child,
				int				xDest,
				int				yDest,
				const DocumentRectangle *	drChild )
    {
    if  ( ! child )
	{ XDEB(child); return;	}

    gdk_draw_pixmap( ds->dsDrawable, ds->dsGc, child->dsDrawable,
				drChild->drX0, drChild->drY0, xDest, yDest,
				drChild->drX1- drChild->drX0+ 1,
				drChild->drY1- drChild->drY0+ 1 );

    return;
    }


/************************************************************************/

void drawMoveArea(		DrawingSurface			ds,
				int				xDest,
				int				yDest,
				const DocumentRectangle *	drSrc )
    {
    gdk_window_copy_area( ds->dsDrawable, ds->dsGc, xDest, yDest,
				ds->dsDrawable,
				drSrc->drX0, drSrc->drY0,
				drSrc->drX1- drSrc->drX0+ 1,
				drSrc->drY1- drSrc->drY0+ 1 );
    }

#   endif
