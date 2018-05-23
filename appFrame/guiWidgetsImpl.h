/************************************************************************/
/*									*/
/*  Basic structures for a Gui document manipulator application.	*/
/*									*/
/************************************************************************/

#   ifndef	GUI_WIDGETS_IMPL_H
#   define	GUI_WIDGETS_IMPL_H

#   include	<stdio.h>

#   include	"appGuiBase.h"

# ifdef __cplusplus
extern "C"
    {
# endif

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void appEmptyParentWidget(	APP_WIDGET		parent );

/************************************************************************/
/*									*/
/*  Event handler to set minimum size.					*/
/*									*/
/************************************************************************/

extern APP_EVENT_HANDLER_H( appSetSizeAsMinimum, w, through, event );

# ifdef __cplusplus
    }
# endif

#   endif
