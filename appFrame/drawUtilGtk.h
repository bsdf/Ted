/************************************************************************/
/*									*/
/*  Gtk/Gdk implementation utility functions. Always use via		*/
/*  drawUtilImpl.h							*/
/*									*/
/************************************************************************/

#   ifdef USE_GTK /* { */

#   include	"appGuiBase.h"
#   include	"drawImpl.h"
#   include	"drawColors.h"
#   include	"appXftColorList.h"
#   include	<bitmap.h>

/************************************************************************/
/*									*/
/*  Utility Routines							*/
/*									*/
/************************************************************************/

extern int drawUtilGtkMakeImage(
			APP_IMAGE **			pPimage,
			int				toWide,
			int				toHigh,
			AppColors *			ac,
			const RasterImage *		abi,
			const DocumentRectangle *	drSrc );

extern XftDraw * appGtkXftDrawCreate(	GdkDrawable *		drawable,
					AppXftColorList *	axcl );

extern AppColors * guiGetColorsGtk( void );

extern void guiCollectExposuresGtk(	DocumentRectangle *	drClip,
					GdkWindow *		window,
					APP_EVENT *		event );

extern void appDrawGtkSetXFillRule(	GdkGC *			gc,
					int			rule );

#   endif	/* } USE_GTK */
