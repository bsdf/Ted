/************************************************************************/
/*									*/
/*  Simple io streams using X11 window properties.			*/
/*									*/
/************************************************************************/

#   include	"appGuiBase.h"

#   include	<sioGeneral.h>

# ifdef __cplusplus
extern "C"
    {
# endif

extern SimpleInputStream * sioInOpenPaste(	APP_WIDGET		w,
						APP_SELECTION_EVENT *	event );

extern SimpleOutputStream * sioOutOpenCopy(	APP_WIDGET		w,
						APP_SELECTION_EVENT *	event );

# ifdef __cplusplus
    }
# endif

