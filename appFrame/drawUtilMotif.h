/************************************************************************/
/*									*/
/*  Motif implementation utility functions. Always use via		*/
/*  drawUtilImpl.h							*/
/*									*/
/************************************************************************/

#   ifdef USE_MOTIF /* { */

#   include	"appGuiBase.h"
#   include	"drawImpl.h"
#   include	"drawColors.h"
#   include	"appXftColorList.h"

#   include	<X11/Xlib.h>
#   include	<bitmap.h>

extern const char * const APP_VisualClasses[];

/************************************************************************/
/*									*/
/*  Utility Routines							*/
/*									*/
/************************************************************************/

extern int drawUtilMotifMakeImage(
			Display *			display,
			int				screen,
			APP_IMAGE **			pPimage,
			int				toWide,
			int				toHigh,
			AppColors *			ac,
			const RasterImage *		abi,
			const DocumentRectangle *	drSel );

extern XftDraw * appMotifXftDrawCreate(	Display *		display,
					int			screen,
					Drawable		drawable,
					AppXftColorList *	xftColorList );

extern AppColors *	guiGetColorsMotif( Display *	display );

#   endif	/* } USE_MOTIF */
