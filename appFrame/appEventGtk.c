/************************************************************************/
/*									*/
/*  Drawing primitives for X11/Motif.					*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>

#   include	<geo2DInteger.h>
#   include	"guiDrawingWidget.h"

#   include	<appDebugon.h>

#   ifdef USE_GTK

int guiDrawGetSizeFromConfigureEvent(	int *		pWide,
					int *		pHigh,
					APP_WIDGET	w,
					APP_EVENT *	event )
    {
    GdkEventConfigure *	cevent= &(event->configure);

    if  ( event->type != GDK_CONFIGURE )
	{ return 1;	}

    *pWide= cevent->width; *pHigh= cevent->height; return 0;
    }

int guiGetCoordinatesFromMouseButtonEvent(
					int *			pX,
					int *			pY,
					int *			pButton,
					int *			pUpDown,
					int *			pSeq,
					unsigned int *		pKeyState,
					const APP_WIDGET	w,
					const APP_EVENT *	event )
    {
    const GdkEventButton *	bevent= &(event->button);

    switch( event->type )
	{
	case GDK_BUTTON_PRESS:
	    *pUpDown= +1; *pSeq= 1; break;

	case GDK_2BUTTON_PRESS:
	    *pUpDown= +1; *pSeq= 2; break;

	case GDK_3BUTTON_PRESS:
	    *pUpDown= +1; *pSeq= 3; break;

	case GDK_BUTTON_RELEASE:
	    *pUpDown= -1; *pSeq= 1; break;

	default:
	    LDEB(event->type);
	    return 1;
	}

    *pButton= bevent->button;
    *pX= bevent->x; *pY= bevent->y;

    *pKeyState= bevent->state;

    return 0;
    }
					
int guiGetCoordinatesRelativeToWidget(	int *			pX,
					int *			pY,
					const APP_WIDGET	w )
    {
    gint		winX;
    gint		winY;
    GdkModifierType	mask;

    gdk_window_get_pointer( w->window, &winX, &winY, &mask );

    *pX= winX; *pY= winY;
    return 0;
    }

int guiGetCoordinatesFromMouseMoveEvent(	int *		pX,
						int *		pY,
						APP_WIDGET	w,
						APP_EVENT *	event )
    {
    GdkEventMotion *	mevent= &(event->motion);

    if  ( event->type != GDK_MOTION_NOTIFY )
	{ return 1;	}

    *pX= mevent->x; *pY= mevent->y; return 0;
    }

void guiDrawSetRedrawHandler(	APP_WIDGET		w,
				APP_EVENT_HANDLER_T	handler,
				void *			through )
    {
    gtk_signal_connect( GTK_OBJECT( w ), "expose_event",
					    (GtkSignalFunc)handler, through );
    }

void guiDrawSetConfigureHandler(	APP_WIDGET		w,
					APP_EVENT_HANDLER_T	handler,
					void *			through )
    {
    gtk_signal_connect( GTK_OBJECT( w ), "configure_event",
					    (GtkSignalFunc)handler, through );
    }

void guiDrawSetButtonPressHandler(	APP_WIDGET		w,
					APP_EVENT_HANDLER_T	handler,
					void *			through )
    {
    gtk_widget_add_events( w, GDK_BUTTON_PRESS_MASK );

    gtk_signal_connect( GTK_OBJECT( w ), "button_press_event",
					    (GtkSignalFunc)handler, through );
    }

/*
 *  Install a scoll handler. For older versions of GTK the event is 
 *  handled as a (mouse) button press event.
 */
void guiDrawSetScrollHandler(		APP_WIDGET		w,
					APP_EVENT_HANDLER_T	handler,
					void *			through )
    {
#   if GTK_MAJOR_VERSION >= 2
    gtk_widget_add_events( w, GDK_SCROLL_MASK );

    gtk_signal_connect( GTK_OBJECT( w ), "scroll_event",
					    (GtkSignalFunc)handler, through );
#   endif
    }

int guiDrawGetInoutFromFocusEvent(	int *			pInOut,
					APP_WIDGET		w,
					APP_EVENT *		event )
    {
    switch( event->type )
	{
	case GDK_FOCUS_CHANGE:
	    if  ( event->focus_change.in )
		{ *pInOut=  1; return 0; }
	    if  ( ! event->focus_change.in )
		{ *pInOut= -1; return 0; }
	    LDEB(event->focus_change.in); return 1;

	default:
	    LDEB(event->type); return 1;
	}
    }

int guiDrawGetSizeOfWidget(		int *		pWide,
					int *		pHigh,
					APP_WIDGET	w )
    {
    gint		wide;
    gint		high;

    wide= w->allocation.width;
    high= w->allocation.height;

    *pWide= wide; *pHigh= high; return 0;
    }

#   endif
