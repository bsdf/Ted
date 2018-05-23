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
#   include "drawScreenFontAdmin.h"
#   include "drawUtilMotif.h"
#   include "drawImpl.h"
#   include "drawImplXft.h"
#   include <appDebugon.h>
#   include "drawDrawingSurfacePrivate.h"
#   include "guiDrawingWidget.h"

#   ifdef USE_MOTIF

void guiExposeDrawingWidget(	APP_WIDGET	nativeWidget )
    {
    if  ( ! XtWindow( nativeWidget ) )
	{ XDEB(XtWindow(nativeWidget)); return;	}
    if  ( ! XtIsRealized( nativeWidget ) )
	{ LDEB(XtIsRealized(nativeWidget)); return;	}

    XClearArea( XtDisplay( nativeWidget ), XtWindow( nativeWidget ),
							0, 0, 0, 0, True );

    return;
    }

void guiExposeDrawingWidgetRectangle(
				APP_WIDGET			nativeWidget,
				const DocumentRectangle *	drExpose )
    {
    int		x= drExpose->drX0;
    int		y= drExpose->drY0;
    int		wide= drExpose->drX1- drExpose->drX0+ 1;
    int		high= drExpose->drY1- drExpose->drY0+ 1;

    XEvent	xev;

    if  ( ! XtWindow( nativeWidget ) )
	{ XDEB(XtWindow(nativeWidget)); return;	}
    if  ( ! XtIsRealized( nativeWidget ) )
	{ LDEB(XtIsRealized(nativeWidget)); return;	}

    xev.type= Expose;
    xev.xexpose.serial= 0;
    xev.xexpose.send_event= True;
    xev.xexpose.display= XtDisplay( nativeWidget );
    xev.xexpose.window= XtWindow( nativeWidget );
    xev.xexpose.x= x;
    xev.xexpose.y= y;
    xev.xexpose.width= wide;
    xev.xexpose.height= high;
    xev.xexpose.count= 0;

    if  ( XSendEvent( XtDisplay( nativeWidget ), XtWindow( nativeWidget ),
					0, ExposureMask, &xev ) != Success )
	{ /*LDEB(1);*/	}

    return;
    }

#   define	LOG_EXPOSES	0

static
void guiCollectExposuresMotif(	DocumentRectangle *	drClip,
				Display *		display,
				Drawable		window,
				APP_EVENT *		event )
    {
    drClip->drX0= (int)event->xexpose.x;
    drClip->drY0= (int)event->xexpose.y;
    drClip->drX1= (int)event->xexpose.x+ (int)event->xexpose.width;
    drClip->drY1= (int)event->xexpose.y+ (int)event->xexpose.height;

#   if LOG_EXPOSES
    appDebug( "E=====: [%4d+%4d]x[%4d+%4d]\n",
		    drClip->drX0,
		    drClip->drX1- drClip->drX0+ 1,
		    drClip->drY0,
		    drClip->drY1- drClip->drY0+ 1 );
#   endif

    while( QLength( display ) > 0 )
	{
	XEvent			nextEvent;
	DocumentRectangle	drMore;

	XPeekEvent( display, &nextEvent );

	if  ( nextEvent.type != Expose && nextEvent.type != GraphicsExpose )
	    { break;	}
	if  ( nextEvent.xexpose.window != window )
	    { break;	}

	XNextEvent( display, &nextEvent );
	drMore.drX0= (int)nextEvent.xexpose.x;
	drMore.drY0= (int)nextEvent.xexpose.y;
	drMore.drX1= (int)nextEvent.xexpose.x+ (int)nextEvent.xexpose.width;
	drMore.drY1= (int)nextEvent.xexpose.y+ (int)nextEvent.xexpose.height;

#	if LOG_EXPOSES
	appDebug( "E+++++: [%4d+%4d]x[%4d+%4d]\n",
			drMore.drX0,
			drMore.drX1- drMore.drX0+ 1,
			drMore.drY0,
			drMore.drY1- drMore.drY0+ 1 );
#	endif

	geoUnionRectangle( drClip, drClip, &drMore );

#	if LOG_EXPOSES
	appDebug( "E.....: [%4d+%4d]x[%4d+%4d]\n",
			drClip->drX0,
			drClip->drX1- drClip->drX0+ 1,
			drClip->drY0,
			drClip->drY1- drClip->drY0+ 1 );
#	endif
	}

    return;

    return;
    }

void guiCollectExposures(	DocumentRectangle *	drClip,
				APP_WIDGET		nativeWidget,
				APP_EVENT *		event )
    {
    guiCollectExposuresMotif( drClip,
		XtDisplay( nativeWidget ), XtWindow( nativeWidget ), event );
    }

/************************************************************************/
/*									*/
/*  Make a drawing surface for a screen widget.				*/
/*									*/
/************************************************************************/

DrawingSurface guiDrawingSurfaceForNativeWidget(
					APP_WIDGET	nativeWidget,
					int		avoidFontconfig )
    {
    DrawingSurface	ds;

    if  ( ! XtIsRealized( nativeWidget ) )
	{ LDEB(XtIsRealized(nativeWidget)); return (DrawingSurface)0;	}

    ds= malloc(sizeof(struct DrawingSurface));
    if  ( ! ds )
	{ PDEB(ds); return ds;	}
    drawInitDrawingSurface( ds );

    ds->dsColors= guiGetColorsMotif( XtDisplay( nativeWidget ) );
    if  ( ! ds->dsColors )
	{
	XDEB(ds->dsGc);
	drawFreeDrawingSurface( ds );
	return (DrawingSurface)0;
	}
    ds->dsDrawable= XtWindow( nativeWidget );
    ds->dsDisplay= XtDisplay( nativeWidget );
    ds->dsScreen= DefaultScreen( ds->dsDisplay );
    ds->dsAvoidFontconfig= avoidFontconfig;

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

/************************************************************************/
/*									*/
/*  Get the font of a label widget.					*/
/*									*/
/************************************************************************/

int guiGetLabelFont(	DrawingSurface			ds,
			const PostScriptFontList *	psfl,
			APP_WIDGET			nativeLabel )
    {
    const int		fontSizeHintPixels= -1;

    return drawOpenDefaultFont( ds, psfl, nativeLabel, fontSizeHintPixels );
    }

/************************************************************************/
/*									*/
/*  Get colors.								*/
/*									*/
/************************************************************************/

static void guiGetColor(	RGB8Color *		rgb8,
				const char *		xmName,
				APP_WIDGET		nativeWidget )
    {
    APP_COLOR_RGB	xc;
    Display *		display= XtDisplay( nativeWidget );
    int			screen= DefaultScreen( display );
    Colormap		cmap= DefaultColormap( display, screen );

    if  ( ! XtIsRealized( nativeWidget ) )
	{ LDEB(XtIsRealized(nativeWidget)); return;	}

    XtVaGetValues( nativeWidget,	xmName,		&(xc.pixel),
					NULL );

    XQueryColor( display, cmap, &xc );

    rgb8->rgb8Red= xc.red/ 256;
    rgb8->rgb8Green= xc.green/ 256;
    rgb8->rgb8Blue= xc.blue/ 256;
    rgb8->rgb8Alpha= 255;

    return;
    }

void guiGetForegroundColor(	RGB8Color *		rgb8,
				APP_WIDGET		nativeWidget )
    {
    guiGetColor( rgb8, XmNforeground, nativeWidget );
    return;
    }

void guiGetBackgroundColor(	RGB8Color *		rgb8,
				APP_WIDGET		nativeWidget )
    {
    guiGetColor( rgb8, XmNbackground, nativeWidget );
    return;
    }

void guiGetTopShadowColor(	RGB8Color *		rgb8,
				APP_WIDGET		nativeWidget )
    {
    guiGetColor( rgb8, XmNtopShadowColor, nativeWidget );
    return;
    }

void guiGetBottomShadowColor(	RGB8Color *		rgb8,
				APP_WIDGET		nativeWidget )
    {
    guiGetColor( rgb8, XmNbottomShadowColor, nativeWidget );
    return;
    }

#   endif
