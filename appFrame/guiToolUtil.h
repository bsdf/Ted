/************************************************************************/
/*									*/
/*  Basic structures for a Gui document manipulator application.	*/
/*									*/
/************************************************************************/

#   ifndef	GUI_TOOL_UTIL_H
#   define	GUI_TOOL_UTIL_H

#   include	<stdio.h>

#   include	"appGuiBase.h"
#   include	"guiOptionmenu.h"

# ifdef __cplusplus
extern "C"
    {
# endif

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void guiToolMake2BottonRow(	APP_WIDGET *		pRow,
					APP_WIDGET		parent,
					APP_WIDGET *		pLeftButton,
					APP_WIDGET *		pRightButton,
					const char *		leftLabel,
					const char *		rightLabel,
					APP_BUTTON_CALLBACK_T	leftCallback,
					APP_BUTTON_CALLBACK_T	rightCallback,
					void *			through );

extern void guiToolMake3ButtonRow(	APP_WIDGET *		pRow,
					APP_WIDGET		parent,
					APP_WIDGET *		pLeftButton,
					APP_WIDGET *		pMiddleButton,
					APP_WIDGET *		pRightButton,
					const char *		leftLabel,
					const char *		middleLabel,
					const char *		rightLabel,
					APP_BUTTON_CALLBACK_T	leftCallback,
					APP_BUTTON_CALLBACK_T	middleCallback,
					APP_BUTTON_CALLBACK_T	rightCallback,
					void *			through );

extern void guiToolMake2ToggleRow(	APP_WIDGET *		pRow,
					APP_WIDGET		parent,
					APP_WIDGET *		pLeftToggle,
					APP_WIDGET *		pRightToggle,
					const char *		leftText,
					const char *		rightText,
					APP_TOGGLE_CALLBACK_T	leftCallback,
					APP_TOGGLE_CALLBACK_T	rightCallback,
					void *			leftThrough,
					void *			rightThrough );

extern void guiToolMake3ToggleRow(	APP_WIDGET *		pRow,
					APP_WIDGET		parent,
					APP_WIDGET *		pLeftToggle,
					APP_WIDGET *		pMiddletToggle,
					APP_WIDGET *		pRightToggle,
					const char *		leftText,
					const char *		middleText,
					const char *		rightText,
					APP_TOGGLE_CALLBACK_T	leftCallback,
					APP_TOGGLE_CALLBACK_T	middleCallback,
					APP_TOGGLE_CALLBACK_T	rightCallback,
					void *			through );

extern void guiToolMakeToggleAndLabelRow(
					APP_WIDGET *		pRow,
					APP_WIDGET		parent,
					APP_WIDGET *		pToggle,
					APP_WIDGET *		pLabel,
					const char *		toggleText,
					const char *		labelText,
					APP_TOGGLE_CALLBACK_T	toggleCallback,
					void *			through );

extern void guiToolMakeLabelAndToggleRow(
					APP_WIDGET *		pRow,
					APP_WIDGET		parent,
					APP_WIDGET *		pLabel,
					APP_WIDGET *		pToggle,
					const char *		labelText,
					const char *		toggleText,
					APP_TOGGLE_CALLBACK_T	toggleCallback,
					void *			through );

extern void guiToolMakeLabelAndMenuRow(	APP_WIDGET *		pRow,
					AppOptionmenu *		aom,
					APP_WIDGET *		pLabel,
					APP_WIDGET		parent,
					const char *		labelText,
					OptionmenuCallback	callBack,
					void *			target );

extern void appFillInspectorMenu(	int			count,
					int			current,
					APP_WIDGET *		items,
					const char * const *	texts,
					AppOptionmenu *		aom );

extern void guiToolMakeToggleAndTextRow( APP_WIDGET *		pRow,
					APP_WIDGET *		pToggle,
					APP_WIDGET *		pText,
					APP_WIDGET		parent,
					const char *		labelText,
					APP_TOGGLE_CALLBACK_T	callback,
					void *			through,
					int			textColumns,
					int			textEnabled );

extern void guiToolMakeLabelAndTextRow(	APP_WIDGET *		pRow,
					APP_WIDGET *		pLabel,
					APP_WIDGET *		pText,
					APP_WIDGET		parent,
					const char *		labelText,
					int			textColumns,
					int			textEnabled );

# ifdef __cplusplus
    }
# endif

#   endif
