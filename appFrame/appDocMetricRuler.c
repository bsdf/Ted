/************************************************************************/
/*									*/
/*  Rulers, Ted specific functionality.					*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	"guiWidgets.h"
#   include	"appMetricRuler.h"
#   include	"guiWidgetDrawingSurface.h"
#   include	"guiDrawingWidget.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Draw a selection rectangle over the document. Strictly spoken this	*/
/*  has nothing to do with metric rulers as drawing is done on the	*/
/*  document widget.							*/
/*									*/
/************************************************************************/

void appDrawRectangleSelection(	DocumentRectangle *		drClip,
				DocumentRectangle *		drSelected,
				DrawingSurface			ds,
				const DocumentRectangle *	drScreen,
				int				ox,
				int				oy )
    {
    DocumentRectangle	drMark;

    if  ( drSelected->drX0 < 0	&&
	  drSelected->drX1 < 0	&&
	  drSelected->drY0 < 0	&&
	  drSelected->drY1 < 0	)
	{ return;	}

    drawSetForegroundColorBlack( ds );

    if  ( drSelected->drX0 >= 0 )
	{
	drMark= *drScreen;
	drMark.drX0= drMark.drX1= drSelected->drX0- 1;

	geoShiftRectangle( &drMark, -ox, -oy );
	drawFillRectangle( ds, &drMark );
	}

    if  ( drSelected->drX1 >= 0 )
	{
	drMark= *drScreen;
	drMark.drX0= drMark.drX1= drSelected->drX1;

	geoShiftRectangle( &drMark, -ox, -oy );
	drawFillRectangle( ds, &drMark );
	}

    if  ( drSelected->drY0 >= 0 )
	{
	drMark= *drScreen;
	drMark.drY0= drMark.drY1= drSelected->drY0 -1;

	geoShiftRectangle( &drMark, -ox, -oy );
	drawFillRectangle( ds, &drMark );
	}

    if  ( drSelected->drY1 >= 0 )
	{
	drMark= *drScreen;
	drMark.drY0= drMark.drY1= drSelected->drY1;

	geoShiftRectangle( &drMark, -ox, -oy );
	drawFillRectangle( ds, &drMark );
	}

    drawSetForegroundColorWhite( ds );

    if  ( drSelected->drX0 >= 0 )
	{
	drMark= *drScreen;
	drMark.drX0= drMark.drX1= drSelected->drX0- 2;

	geoShiftRectangle( &drMark, -ox, -oy );
	drawFillRectangle( ds, &drMark );
	}

    if  ( drSelected->drX1 >= 0 )
	{
	drMark= *drScreen;
	drMark.drX0= drMark.drX1= drSelected->drX1- 1;

	geoShiftRectangle( &drMark, -ox, -oy );
	drawFillRectangle( ds, &drMark );
	}

    if  ( drSelected->drY0 >= 0 )
	{
	drMark= *drScreen;
	drMark.drY0= drMark.drY1= drSelected->drY0- 2;

	geoShiftRectangle( &drMark, -ox, -oy );
	drawFillRectangle( ds, &drMark );
	}

    if  ( drSelected->drY1 >= 0 )
	{
	drMark= *drScreen;
	drMark.drY0= drMark.drY1= drSelected->drY1+ 1;

	geoShiftRectangle( &drMark, -ox, -oy );
	drawFillRectangle( ds, &drMark );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Remove the rectangle selection and redraw the appropriate		*/
/*  rectangles.								*/
/*									*/
/************************************************************************/

static void appMetricRulerExposeX0(
				const DocumentRectangle *	drVisible,
				int				x0,
				APP_WIDGET			w )
    {
    int			ox= drVisible->drX0;
    int			high= drVisible->drY1- drVisible->drY0;

    DocumentRectangle	drExpose;

    drExpose.drX0= x0- ox- 2;
    drExpose.drX1= x0+ 1;
    drExpose.drY0= 0;
    drExpose.drY1= high- 1;

    guiExposeDrawingWidgetRectangle( w, &drExpose );
    }

static void appMetricRulerExposeY0(
				const DocumentRectangle *	drVisible,
				int				y0,
				APP_WIDGET			w )
    {
    int			oy= drVisible->drY0;
    int			wide= drVisible->drX1- drVisible->drX0;

    DocumentRectangle	drExpose;

    drExpose.drX0= 0;
    drExpose.drX1= wide- 1;
    drExpose.drY0= y0- oy- 2;
    drExpose.drY1= y0+ 1;

    guiExposeDrawingWidgetRectangle( w, &drExpose );
    }

static void appMetricRulerExposeX1(
				const DocumentRectangle *	drVisible,
				int				x1,
				APP_WIDGET			w )
    {
    int			ox= drVisible->drX0;
    int			high= drVisible->drY1- drVisible->drY0;

    DocumentRectangle	drExpose;

    drExpose.drX0= x1- ox;
    drExpose.drX1= x1+ 1;
    drExpose.drY0= 0;
    drExpose.drY1= high- 1;

    guiExposeDrawingWidgetRectangle( w, &drExpose );
    }

static void appMetricRulerExposeY1(
				const DocumentRectangle *	drVisible,
				int				y1,
				APP_WIDGET			w )
    {
    int			oy= drVisible->drY0;
    int			wide= drVisible->drX1- drVisible->drX0;

    DocumentRectangle	drExpose;

    drExpose.drX0= 0;
    drExpose.drX1= wide- 1;
    drExpose.drY0= y1- oy;
    drExpose.drY1= y1+ 1;

    guiExposeDrawingWidgetRectangle( w, &drExpose );
    }

void appRemoveRectangleSelection( const DocumentRectangle *	drVisible,
				DocumentRectangle *		drSelected,
				APP_WIDGET			w )
    {
    if  ( drSelected->drX0 >= 0 )
	{
	appMetricRulerExposeX0( drVisible, drSelected->drX0, w );
	drSelected->drX0= -1;
	}

    if  ( drSelected->drX1 >= 0 )
	{
	appMetricRulerExposeX1( drVisible, drSelected->drX1, w );
	drSelected->drX1= -1;
	}

    if  ( drSelected->drY0 >= 0 )
	{
	appMetricRulerExposeY0( drVisible, drSelected->drY0, w );
	drSelected->drY0= -1;
	}

    if  ( drSelected->drY1 >= 0 )
	{
	appMetricRulerExposeY1( drVisible, drSelected->drY1, w );
	drSelected->drY1= -1;
	}

    return;
    }

void appSetHorRectangleSelection( DocumentRectangle *	drVisible,
				DocumentRectangle *	drSelected,
				int			x0Screen,
				int			x1Screen,
				APP_WIDGET		w )
    {
    if  ( drSelected->drX0 != x0Screen )
	{
	if  ( drSelected->drX0 >= 0 )
	    {
	    appMetricRulerExposeX0( drVisible, drSelected->drX0, w );
	    }

	if  ( x0Screen >= 0 )
	    {
	    appMetricRulerExposeX0( drVisible, x0Screen, w );
	    }

	drSelected->drX0= x0Screen;
	}

    if  ( drSelected->drX1 != x1Screen )
	{
	if  ( drSelected->drX1 >= 0 )
	    {
	    appMetricRulerExposeX1( drVisible, drSelected->drX1, w );
	    }

	if  ( x1Screen >= 0 )
	    {
	    appMetricRulerExposeX1( drVisible, x1Screen, w );
	    }

	drSelected->drX1= x1Screen;
	}

    return;
    }


void appSetVertRectangleSelection( DocumentRectangle *	drVisible,
				DocumentRectangle *	drSelected,
				int			y0Screen,
				int			y1Screen,
				APP_WIDGET		w )
    {
    if  ( drSelected->drY0 != y0Screen )
	{
	if  ( drSelected->drY0 >= 0 )
	    {
	    appMetricRulerExposeY0( drVisible, drSelected->drY0, w );
	    }

	if  ( y0Screen >= 0 )
	    {
	    appMetricRulerExposeY0( drVisible, y0Screen, w );
	    }

	drSelected->drY0= y0Screen;
	}

    if  ( drSelected->drY1 != y1Screen )
	{
	if  ( drSelected->drY1 >= 0 )
	    {
	    appMetricRulerExposeY1( drVisible, drSelected->drY1, w );
	    }

	if  ( y1Screen >= 0 )
	    {
	    appMetricRulerExposeY1( drVisible, y1Screen, w );
	    }

	drSelected->drY1= y1Screen;
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Make the ruler administration for the top ruler widget of a		*/
/*  document.								*/
/*									*/
/************************************************************************/

void appDocSetMetricTopRuler(	EditDocument *		ed,
				int			topRulerHeight,
				double			magnification,
				int			unitInt,
				APP_EVENT_HANDLER_T	mouseDown )
    {
    EditApplication *		ea= ed->edApplication;

    ed->edTopRuler= appMakeMetricRuler(

		    topRulerHeight,		/*  sizeAcross		*/
		    ea->eaMagnification,
		    magnification,		/*  magnification	*/
		    &(ea->eaPostScriptFontList),

		    ed->edLeftRulerWidePixels,	/*  minUnused		*/
		    ed->edRightRulerWidePixels,/*  maxUnused		*/

		    ed->edFullRect.drX0,	/*  documentC0		*/
		    ed->edFullRect.drX1,	/*  documentC1		*/
		    ed->edVisibleRect.drX0,	/*  visisbleC0		*/
		    ed->edVisibleRect.drX1,	/*  visisbleC1		*/

		    unitInt );			/*  whatUnit		*/

    guiDrawSetConfigureHandler( ed->edTopRulerWidget,
		    appHorizontalRulerConfigure, (void *)ed->edTopRuler );

    guiDrawSetRedrawHandler( ed->edTopRulerWidget,
		    appRedrawHorizontalMetricRuler, (void *)ed->edTopRuler );

    guiDrawSetButtonPressHandler( ed->edTopRulerWidget,
						mouseDown, (void *)ed );
#   ifdef USE_GTK
    gtk_widget_add_events( ed->edTopRulerWidget, GDK_POINTER_MOTION_MASK );
#   endif

    return;
    }

/************************************************************************/
/*									*/
/*  Make the ruler administration for the left ruler widget of a	*/
/*  document.								*/
/*									*/
/************************************************************************/

void appDocSetMetricLeftRuler(	EditDocument *		ed,
				int			leftRulerWidth,
				double			magnification,
				int			unitInt,
				APP_EVENT_HANDLER_T	mouseDown )
    {
    EditApplication *		ea= ed->edApplication;

    ed->edLeftRuler= appMakeMetricRuler(
		    leftRulerWidth,		/*  sizeAcross		*/
		    ea->eaMagnification,
		    magnification,		/*  magnification	*/
		    &(ea->eaPostScriptFontList),

		    0, 0,			/*  {min,max}Unused	*/

		    ed->edFullRect.drY0,	/*  documentC0		*/
		    ed->edFullRect.drY1,	/*  documentC1		*/
		    ed->edVisibleRect.drY0,	/*  visisbleC0		*/
		    ed->edVisibleRect.drY1,	/*  visisbleC1		*/

		    unitInt );			/*  whatUnit		*/

    guiDrawSetConfigureHandler( ed->edLeftRulerWidget,
			appVerticalRulerConfigure, (void *)ed->edLeftRuler );

    guiDrawSetRedrawHandler( ed->edLeftRulerWidget,
			appRedrawVerticalMetricRuler, ed->edLeftRuler );

    guiDrawSetButtonPressHandler( ed->edLeftRulerWidget,
						    mouseDown, (void *)ed );
#   ifdef USE_GTK
    gtk_widget_add_events( ed->edTopRulerWidget, GDK_POINTER_MOTION_MASK );
#   endif

    return;
    }
