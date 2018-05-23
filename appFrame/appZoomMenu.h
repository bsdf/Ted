/************************************************************************/
/*									*/
/*  Infrastructure to make a 'Zoom' document menu.			*/
/*									*/
/************************************************************************/

#   ifndef		APP_ZOOM_MENU_H
#   define		APP_ZOOM_MENU_H

#   include	"appGuiBase.h"
#   include	"appFrame.h"
#   include	<geo2DInteger.h>

#   define	ZOOM_COUNT	7

extern void appZoomChangeFactor( APP_WIDGET		option,
				APP_WIDGET		menu,
				APP_WIDGET *		options,
				int			optionCount,
				int *			pLogSqrt2Magnification,
				double *		pMagnification,
				DocumentRectangle *	drSelected );

extern void appUpdateZoomMenu(	APP_WIDGET	menu,
				APP_WIDGET *	options,
				int		optionCount,
				int		logs2mag );

extern void appMakeZoomMenu(	EditApplication *	ea,
				EditDocument *		ed,
				APP_WIDGET *		pZoomMenu,
				APP_WIDGET		menubar,
				int			logSqrt2Magnification,
				const char *		zoomText,
				AppMenuItem *		optionItems,
				APP_WIDGET *		options,
				int			optionCount );

#   endif	/*	APP_ZOOM_MENU_H	*/
