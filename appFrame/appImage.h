#   ifndef	APP_IMAGE_H
#   define	APP_IMAGE_H

#   include	<bitmap.h>

#   include	"appGuiBase.h"

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int appImgPastePixmap(		RasterImage *		abi,
					APP_WIDGET		w,
					APP_SELECTION_EVENT *	event );

#   ifdef USE_MOTIF
    extern int appGetImageMotif(	RasterImage *		abi,
					const APP_IMAGE *	xim,
					Display *		display );
#   endif

#   endif
