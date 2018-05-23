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
#   include "drawUtilGtk.h"
#   include "drawImpl.h"
#   include "drawImplXft.h"
#   include <appDebugon.h>
#   include "drawDrawingSurfacePrivate.h"
#   include "guiDrawingWidget.h"

#   ifdef USE_GTK

void guiExposeDrawingWidget(	APP_WIDGET	nativeWidget )
    {
    GdkRectangle	rect;

    if  ( ! nativeWidget->window )
	{ gtk_widget_realize( nativeWidget );	}

    rect.x= 0;
    rect.y= 0;
    rect.width= 0;
    rect.height= 0;

    gdk_drawable_get_size( nativeWidget->window,
					&(rect.width), &(rect.height) );

    gdk_window_invalidate_rect( nativeWidget->window, &rect, True );

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

    if  ( ! nativeWidget->window )
	{ gtk_widget_realize( nativeWidget );	}

#   if GTK_MAJOR_VERSION < 2
    gdk_window_clear_area_e( nativeWidget->window, x, y, wide, high );
#   else

    {
    GdkRectangle	rect;

    rect.x= x;
    rect.y= y;
    rect.width= wide;
    rect.height= high;

    gdk_window_invalidate_rect( nativeWidget->window, &rect, TRUE );
    }

#   endif

    return;
    }

#   define	LOG_EXPOSES	0

void guiCollectExposuresGtk(	DocumentRectangle *	drClip,
				GdkWindow *		window,
				APP_EVENT *		event )
    {
    drClip->drX0= event->expose.area.x;
    drClip->drY0= event->expose.area.y;
    drClip->drX1= event->expose.area.x+ event->expose.area.width;
    drClip->drY1= event->expose.area.y+ event->expose.area.height;

#   if LOG_EXPOSES
    appDebug( "======: [%4d+%4d]x[%4d+%4d]\n",
		    drClip->drX0,
		    drClip->drX1- drClip->drX0+ 1,
		    drClip->drY0,
		    drClip->drY1- drClip->drY0+ 1 );
#   endif

    while( TRUE )
	{
	GdkEvent *		nxEvent;
	DocumentRectangle	drMore;

	nxEvent= gdk_event_peek();
	if  ( ! nxEvent )
	    { break;	}

	if  ( nxEvent->type != GDK_EXPOSE	||
	      nxEvent->any.window != window	)
	    { gdk_event_free( nxEvent ); break;	}

	gdk_event_free( nxEvent );

	nxEvent= gdk_event_get();
	if  ( ! nxEvent )
	    { XDEB(nxEvent); break;	}

	drMore.drX0= nxEvent->expose.area.x;
	drMore.drY0= nxEvent->expose.area.y;
	drMore.drX1= nxEvent->expose.area.x+ nxEvent->expose.area.width;
	drMore.drY1= nxEvent->expose.area.y+ nxEvent->expose.area.height;

	gdk_event_free( nxEvent );

#	if LOG_EXPOSES
	appDebug( "++++++: [%4d+%4d]x[%4d+%4d]\n",
			drMore.drX0,
			drMore.drX1- drMore.drX0+ 1,
			drMore.drY0,
			drMore.drY1- drMore.drY0+ 1 );
#	endif

	geoUnionRectangle( drClip, drClip, &drMore );

#	if LOG_EXPOSES
	appDebug( "......: [%4d+%4d]x[%4d+%4d]\n",
			drClip->drX0,
			drClip->drX1- drClip->drX0+ 1,
			drClip->drY0,
			drClip->drY1- drClip->drY0+ 1 );
#	endif
	}

    return;
    }

void guiCollectExposures(	DocumentRectangle *	drClip,
				APP_WIDGET		nativeWidget,
				APP_EVENT *		event )
    {
    guiCollectExposuresGtk( drClip, nativeWidget->window, event );
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
    DrawingSurface	ds= malloc(sizeof(struct DrawingSurface));

    if  ( ! nativeWidget->window )
	{ gtk_widget_realize( nativeWidget );	}

    if  ( ! ds )
	{ PDEB(ds); return ds;	}
    drawInitDrawingSurface( ds );

    ds->dsColors= guiGetColorsGtk();
    if  ( ! ds->dsColors )
	{
	XDEB(ds->dsGc);
	drawFreeDrawingSurface( ds );
	return (DrawingSurface)0;
	}
    ds->dsDrawable= nativeWidget->window;
    ds->dsAvoidFontconfig= avoidFontconfig;

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

#	if GTK_MAJOR_VERSION >= 2
	if  ( ds->dsXftDrawable )
	    { gtk_widget_set_double_buffered( nativeWidget, FALSE ); }
#	endif

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
#   if GTK_MAJOR_VERSION >= 2
    GtkWidget *		labelw= gtk_bin_get_child( GTK_BIN( nativeLabel ) );
#   else
    GtkWidget *		labelw= GTK_BIN( nativeLabel )->child;
#   endif
    const int		fontSizeHintPixels= -1;

    if  ( ! GTK_IS_BIN( nativeLabel ) )
	{ XDEB(nativeLabel);	}

    return drawOpenDefaultFont( ds, psfl, labelw, fontSizeHintPixels );
    }

/************************************************************************/
/*									*/
/*  Get colors.								*/
/*									*/
/************************************************************************/

void guiGetForegroundColor(	RGB8Color *		rgb8,
				APP_WIDGET		nativeWidget )
    {
    GtkStyle *			gs= gtk_widget_get_style( nativeWidget );
    const APP_COLOR_RGB *	xc= &(gs->fg[GTK_STATE_NORMAL]);

    rgb8->rgb8Red= xc->red/ 256;
    rgb8->rgb8Green= xc->green/ 256;
    rgb8->rgb8Blue= xc->blue/ 256;
    rgb8->rgb8Alpha= 255;

    return;
    }

void guiGetBackgroundColor(	RGB8Color *		rgb8,
				APP_WIDGET		nativeWidget )
    {
    GtkStyle *			gs= gtk_widget_get_style( nativeWidget );
    const APP_COLOR_RGB *	xc= &(gs->bg[GTK_STATE_NORMAL]);

    rgb8->rgb8Red= xc->red/ 256;
    rgb8->rgb8Green= xc->green/ 256;
    rgb8->rgb8Blue= xc->blue/ 256;
    rgb8->rgb8Alpha= 255;

    return;
    }

void guiGetTopShadowColor(	RGB8Color *		rgb8,
				APP_WIDGET		nativeWidget )
    {
    guiGetBackgroundColor( rgb8, nativeWidget );

    utilGetTopShadowColor( rgb8, rgb8 );

    return;
    }

void guiGetBottomShadowColor(	RGB8Color *		rgb8,
				APP_WIDGET		nativeWidget )
    {
    guiGetBackgroundColor( rgb8, nativeWidget );

    utilGetBottomShadowColor( rgb8, rgb8 );

    return;
    }

#   endif
