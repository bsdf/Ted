/************************************************************************/
/*									*/
/*  Translation of GUI Drawing primitives to the actual implementing	*/
/*  GUI system. As we do not have the pretentence to introduce our own	*/
/*  GUI layer, as much as possible is done through macros. (#defines)	*/
/*									*/
/************************************************************************/

#   ifndef	APP_GUI_BASE_H
#   define	APP_GUI_BASE_H

#   ifdef __VMS
#	if defined(USE_MOTIF) || defined(USE_GTK)
#	    include	"vms_x_fix.h"
#	    undef XtDisplay
#	    undef XtScreen
#	    undef XtWindow
#	    undef XtIsRealized
#	    undef XtParent
#	endif
#   endif

	    /* yes BOTH! for dependencies in makefile */
#   include "appGuiBaseMotif.h"
#   include "appGuiBaseGtk.h"

/************************************************************************/
/*									*/
/*   Decide whether we use ugly hand-crafted dialogs or use standard	*/
/*   system ones.							*/
/*									*/
/************************************************************************/

# ifdef USE_MOTIF
#	define USE_OWN_DIALOGS	1
# endif

# ifdef USE_GTK
#   if GTK_MAJOR_VERSION < 2
#	define USE_GTK_DIALOGS	0
#	define USE_OWN_DIALOGS	1
#   else
#	define USE_GTK_DIALOGS	1
#	define USE_OWN_DIALOGS	0
#   endif
# else
#	define USE_GTK_DIALOGS	0
# endif


/************************************************************************/
/*									*/
/*   Routine declarations.						*/
/*									*/
/************************************************************************/

extern int appKeysymX11ToUnicode(	unsigned int	keysym );

#   endif	/* APP_GUI_BASE_H */
