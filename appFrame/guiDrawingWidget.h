/************************************************************************/
/*									*/
/*  A widget in the screen GUI. (Implementation)			*/
/*									*/
/************************************************************************/

#   include "appGuiBase.h"
#   include <psPostScriptFontList.h>
#   include <utilColor.h>

/************************************************************************/
/*									*/
/*  Routine Declarations.						*/
/*									*/
/************************************************************************/

extern void guiExposeDrawingWidget(	APP_WIDGET	nativeWidget );
extern void guiExposeDrawingWidgetRectangle(
				APP_WIDGET			nativeWidget,
				const DocumentRectangle *	dr );

extern void guiCollectExposures(	DocumentRectangle *	drClip,
					APP_WIDGET		nativeWidget,
					APP_EVENT *		exposeEvent );

extern void guiGetForegroundColor(	RGB8Color *		rgb8,
					APP_WIDGET		nativeWidget );

extern void guiGetBackgroundColor(	RGB8Color *		rgb8,
					APP_WIDGET		nativeWidget );

extern void guiGetTopShadowColor(	RGB8Color *		rgb8,
					APP_WIDGET		nativeWidget );

extern void guiGetBottomShadowColor(	RGB8Color *		rgb8,
					APP_WIDGET		nativeWidget );

extern int guiDrawGetSizeOfWidget(		int *		pWide,
						int *		pHigh,
						APP_WIDGET	w );

extern int guiDrawGetSizeFromConfigureEvent(	int *		pWide,
						int *		pHigh,
						APP_WIDGET	w,
						APP_EVENT *	event );

extern int guiGetCoordinatesFromMouseButtonEvent(
					int *			pX,
					int *			pY,
					int *			pButton,
					int *			pUpDown,
					int *			pSeq,
					unsigned int *		pKeyState,
					const APP_WIDGET	w,
					const APP_EVENT *	event );

extern int guiGetCoordinatesRelativeToWidget(
					int *			pX,
					int *			pY,
					const APP_WIDGET	w );

extern int guiGetCoordinatesFromMouseMoveEvent( int *		pX,
						int *		pY,
						APP_WIDGET	w,
						APP_EVENT *	event );

extern void guiDrawSetRedrawHandler(	APP_WIDGET		w,
					APP_EVENT_HANDLER_T	handler,
					void *			through );

extern void guiDrawSetConfigureHandler(	APP_WIDGET		w,
					APP_EVENT_HANDLER_T	handler,
					void *			through );

extern void guiDrawSetButtonPressHandler( APP_WIDGET		w,
					APP_EVENT_HANDLER_T	handler,
					void *			through );

extern void guiDrawSetScrollHandler( 	APP_WIDGET		w,
					APP_EVENT_HANDLER_T	handler,
					void *			through );

extern int guiDrawGetInoutFromFocusEvent(	int *		pInOut,
						APP_WIDGET	w,
						APP_EVENT *	event );

