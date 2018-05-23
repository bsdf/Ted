#   include	"appFrameConfig.h"

#   include	<stdio.h>

#   include	"guiWidgets.h"
#   include	"appDrawnPulldown.h"
#   include	"guiWidgetDrawingSurface.h"
#   include	"guiDrawingWidget.h"

#   include	<appDebugon.h>

void appDrawnPulldownDrawArrow( const DocumentRectangle *	drClip,
				int *				pRestWide,
				int *				pRestHigh,
				APP_WIDGET			w,
				AppDrawnPulldown *		adp )
    {
    DrawingSurface	ds= adp->adpInplaceDrawingSurface;

    int			wide;
    int			high;

    int			x0;
    int			wi;
    int			m;
    int			d;

    guiDrawGetSizeOfWidget( &wide, &high, w );

    wi= ( 2* high )/ 3;
    x0= wide- wi;
    m= x0+ wi/ 2;
    d= wi/ 4;

    drawSetForegroundColor( ds, &(adp->adpInplaceBackgroundColor) );
    drawFillRectangle( ds, drClip );

    if  ( adp->adpEnabled )
	{
	Point2DI	points[4];

	points[0].x= m- d;
	points[0].y= high/ 3;
	points[1].x= m+ d;
	points[1].y= high/ 3;
	points[2].x= m;
	points[2].y= ( 2* high )/ 3;

	points[3]= points[0];

	drawSetForegroundColorBlack( ds );
	drawFillPolygon( ds, points, 3 );
	}

    *pRestWide= wide- wi;
    *pRestHigh= high;
    return;
    }

void appInitDrawnPulldown(		AppDrawnPulldown *		adp )
    {
    adp->adpPulldownShell= (APP_WIDGET)0;
    adp->adpPulldownDrawing= (APP_WIDGET)0;
    adp->adpInplaceDrawing= (APP_WIDGET)0;
    adp->adpClickHandler= (APP_EVENT_HANDLER_T)0;
    adp->adpPulldown= (AppDrawnPulldownPuldown)0;
    adp->adpThrough= (void *)0;
    adp->adpMouseX= -1;
    adp->adpMouseY= -1;

    adp->adpEnabled= 1;

    adp->adpInplaceDrawingSurface= (DrawingSurface)0;
    adp->adpPulldownDrawingSurface= (DrawingSurface)0;
    }

void appCleanDrawnPulldown(		AppDrawnPulldown *		adp )
    {
    if  ( adp->adpInplaceDrawingSurface )
	{ drawFreeDrawingSurface( adp->adpInplaceDrawingSurface ); }
    if  ( adp->adpPulldownDrawingSurface )
	{ drawFreeDrawingSurface( adp->adpPulldownDrawingSurface ); }
    }

/************************************************************************/
/*									*/
/*  Finish a drawn pulldown after it has been realized: Allocate the	*/
/*  drawing surface for its in place part.				*/
/*									*/
/************************************************************************/

int appFinishDrawnPulldownInplace(	AppDrawnPulldown *	adp )
    {
    if  ( ! adp->adpInplaceDrawingSurface )
	{
	const int	avoidFontconfig= 0;

	adp->adpInplaceDrawingSurface= guiDrawingSurfaceForNativeWidget(
				    adp->adpInplaceDrawing, avoidFontconfig );
	if  ( ! adp->adpInplaceDrawingSurface )
	    { XDEB(adp->adpInplaceDrawingSurface);	}

	guiGetBackgroundColor( &(adp->adpInplaceBackgroundColor),
						    adp->adpInplaceDrawing );
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Finish a drawn pulldown after it has been opened: Allocate the	*/
/*  drawing surface for its pulldown part.				*/
/*									*/
/************************************************************************/

int appFinishDrawnPulldownPulldown(	AppDrawnPulldown *	adp )
    {
    if  ( ! adp->adpPulldownDrawingSurface )
	{
	const int	avoidFontconfig= 0;

	adp->adpPulldownDrawingSurface= guiDrawingSurfaceForNativeWidget(
				    adp->adpPulldownDrawing, avoidFontconfig );
	if  ( ! adp->adpPulldownDrawingSurface )
	    { XDEB(adp->adpPulldownDrawingSurface);	}
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Force a redraw of a drawn puldown.					*/
/*									*/
/************************************************************************/

void appExposeDrawnPulldownInplace(	AppDrawnPulldown *	adp )
    {
    guiExposeDrawingWidget( adp->adpInplaceDrawing );
    return;
    }
