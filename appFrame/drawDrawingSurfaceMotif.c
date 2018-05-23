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
#   include "drawUtilMotif.h"
#   include "drawImpl.h"
#   include "drawImplXft.h"
#   include <appDebugon.h>

#   ifdef USE_MOTIF

/************************************************************************/

DrawingSurface drawMakeDrawingSurfaceForParent(
				DrawingSurface			parent,
				int				wide,
				int				high )
    {
    int			depth;
    DrawingSurface	ds= malloc(sizeof(struct DrawingSurface));

    if  ( ! ds )
	{ PDEB(ds); return ds;	}
    drawInitDrawingSurface( ds );

    depth= DefaultDepth( parent->dsDisplay, parent->dsScreen );

    ds->dsDisplay= parent->dsDisplay;
    ds->dsScreen= parent->dsScreen;

    ds->dsColors= parent->dsColors;
    ds->dsDrawable= XCreatePixmap( parent->dsDisplay, parent->dsDrawable,
							wide, high, depth );
    ds->dsAvoidFontconfig= parent->dsAvoidFontconfig;
    ds->dsIsPixmap= 1;

    if  ( ! ds->dsDrawable )
	{
	XDEB(ds->dsDrawable);
	drawFreeDrawingSurface( ds );
	return (DrawingSurface)0;
	}

    ds->dsGc= XCreateGC( ds->dsDisplay, ds->dsDrawable, 0, NULL );
    if  ( ! ds->dsGc )
	{
	XDEB(ds->dsGc);
	drawFreeDrawingSurface( ds );
	return (DrawingSurface)0;
	}

#   ifdef USE_XFT
    if  ( ! ds->dsAvoidFontconfig )
	{
	ds->dsXftDrawable= appMotifXftDrawCreate(
				ds->dsDisplay, ds->dsScreen,
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
				int				pixelsWide,
				int				pixelsHigh )
    {
    DrawingSurface	ds;
    APP_IMAGE *		xim= (APP_IMAGE *)0;

    ds= drawMakeDrawingSurfaceForParent( parent, pixelsWide, pixelsHigh );
    if  ( ! ds )
	{ XDEB(ds); return ds;	}

    if  ( drawUtilMotifMakeImage( ds->dsDisplay, ds->dsScreen,
					    &xim, pixelsWide, pixelsHigh,
					    ds->dsColors, abi, drSrc ) )
	{ drawFreeDrawingSurface( ds ); return (DrawingSurface)0;	}

    XPutImage( ds->dsDisplay, ds->dsDrawable, ds->dsGc, xim, 0, 0, 0, 0,
						pixelsWide, pixelsHigh );

    XDestroyImage( xim );

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

    if  ( drawUtilMotifMakeImage( ds->dsDisplay, ds->dsScreen,
					    &xim, pixelsWide, pixelsHigh,
					    ds->dsColors, abi, drSrc ) )
	{ LLDEB(pixelsWide,pixelsHigh); return -1;	}

    XPutImage( ds->dsDisplay, ds->dsDrawable, ds->dsGc, xim, 0, 0,
						drDest->drX0, drDest->drY0,
						pixelsWide, pixelsHigh );

    XDestroyImage( xim );

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

    XSetForeground( ds->dsDisplay, ds->dsGc, xc.pixel );

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
	{ XFreeGC( ds->dsDisplay, ds->dsGc );	}

    if  ( ds->dsPoints )
	{ free( ds->dsPoints );	}

    if  ( ds->dsIsPixmap )
	{ XFreePixmap( ds->dsDisplay, ds->dsDrawable );	}

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

    XFillRectangle( ds->dsDisplay, ds->dsDrawable, ds->dsGc,
			dr->drX0, dr->drY0,
			dr->drX1- dr->drX0+ 1, dr->drY1- dr->drY0+ 1 );
    }

void drawRectangle(	DrawingSurface			ds,
			const DocumentRectangle *	dr )
    {
    XDrawRectangle( ds->dsDisplay, ds->dsDrawable, ds->dsGc,
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
    XDrawLine( ds->dsDisplay, ds->dsDrawable, ds->dsGc, x0, y0, x1, y1 );
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
	XDrawLine( ds->dsDisplay, ds->dsDrawable, ds->dsGc,
				    ds->dsPoints[i+0].x, ds->dsPoints[i+0].y,
				    ds->dsPoints[i+1].x, ds->dsPoints[i+1].y );
	}

    if  ( close )
	{
	XDrawLine( ds->dsDisplay, ds->dsDrawable, ds->dsGc,
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

    XFillPolygon( ds->dsDisplay, ds->dsDrawable, ds->dsGc,
						ds->dsPoints, pointCount,
						Complex, CoordModeOrigin );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Set line attributes.						*/
/*									*/
/************************************************************************/

static const int LineStyleMap[LineStyle_Count]=
    { LineSolid, LineOnOffDash, LineDoubleDash,	};

static const int CapStyleMap[LineCap_Count]=
    { CapNotLast, CapButt, CapRound, CapProjecting	};

static const int JoinStyleMap[LineJoin_Count]=
    { JoinMiter, JoinRound, JoinBevel	};

int drawSetLineAttributes(	DrawingSurface		ds,
				int			lineWidth,
				int			lineStyle,
				int			capStyle,
				int			joinStyle,
				const unsigned char *	dashList,
				int			dashCount )
    {
    XSetLineAttributes( ds->dsDisplay, ds->dsGc, lineWidth,
				LineStyleMap[lineStyle],
				CapStyleMap[capStyle],
				JoinStyleMap[joinStyle] );

    if  ( lineStyle != LineStyleSolid )
	{
	if  ( ! dashList || dashCount == 0 )
	    { XLDEB(dashList,dashCount);	}
	else{
	    XSetDashes( ds->dsDisplay, ds->dsGc,
					0, (char *)dashList, dashCount );
	    }
	}

    return 0;
    }

void drawArc(			DrawingSurface		ds,
				const Arc2DI *		arc )
    {
    XDrawArc( ds->dsDisplay, ds->dsDrawable, ds->dsGc,
				    arc->a2diRect.drX0, arc->a2diRect.drY0,
				    arc->a2diRect.drX1- arc->a2diRect.drX0+ 1,
				    arc->a2diRect.drY1- arc->a2diRect.drY0+ 1,
				    arc->a2diAngleFrom, arc->a2diAngleStep );
    }

void drawFillArc(		DrawingSurface		ds,
				const Arc2DI *		arc )
    {
    XFillArc( ds->dsDisplay, ds->dsDrawable, ds->dsGc,
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
    XSetForeground( ds->dsDisplay, ds->dsGc, xc->pixel );

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

    /* HACK: Font might have been allocated before the widget wasrealized */
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
	    { dsf->dsfDisplay= ds->dsDisplay;	}
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

    XSetClipMask( ds->dsDisplay, ds->dsGc, None );

    return;
    }

void drawSetClipRect(	DrawingSurface			ds,
			const DocumentRectangle *	drClip )
    {
    XRectangle	xRect;

    xRect.x= drClip->drX0;
    xRect.y= drClip->drY0;
    xRect.width= drClip->drX1- drClip->drX0+ 1;
    xRect.height= drClip->drY1- drClip->drY0+ 1;

    if  ( xRect.width < 1 || xRect.height < 1 )
	{ LLDEB(xRect.width,xRect.height); return;	}

#   ifdef USE_XFT
    if  ( ds->dsXftDrawable )
	{
	XftDrawSetClipRectangles( ds->dsXftDrawable, 0, 0, &xRect, 1 );
	}
#   endif

    XSetClipRectangles( ds->dsDisplay, ds->dsGc, 0, 0, &xRect, 1, Unsorted );

    return;
    }

/************************************************************************/

void drawChildSurface(		DrawingSurface			ds,
				const DrawingSurface		child,
				int				xDest,
				int				yDest,
				const DocumentRectangle *	drChild )
    {
    if  ( drChild->drX1 >= drChild->drX0	&&
	  drChild->drY1 >= drChild->drY0	)
	{
	XCopyArea( ds->dsDisplay, child->dsDrawable, ds->dsDrawable, ds->dsGc,
				drChild->drX0, drChild->drY0,
				drChild->drX1- drChild->drX0+ 1,
				drChild->drY1- drChild->drY0+ 1,
				xDest, yDest );
	}

    return;
    }


/************************************************************************/

void drawMoveArea(		DrawingSurface			ds,
				int				xDest,
				int				yDest,
				const DocumentRectangle *	drSrc )
    {
    XCopyArea( ds->dsDisplay, ds->dsDrawable, ds->dsDrawable, ds->dsGc,
				drSrc->drX0, drSrc->drY0,
				drSrc->drX1- drSrc->drX0+ 1,
				drSrc->drY1- drSrc->drY0+ 1,
				xDest, yDest );
    }

#   endif
