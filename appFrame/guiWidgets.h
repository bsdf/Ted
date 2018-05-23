/************************************************************************/
/*									*/
/*  Basic structures for a Gui document manipulator application.	*/
/*									*/
/************************************************************************/

#   ifndef	GUI_WIDGETS_H
#   define	GUI_WIDGETS_H

#   include	<stdio.h>

#   include	"appGuiBase.h"

# ifdef __cplusplus
extern "C"
    {
# endif

/************************************************************************/
/*									*/
/*  A menu, menu item in an application.				*/
/*									*/
/************************************************************************/

typedef enum MenuItemType
    {
    ITEMtyOPTION= 0,
    ITEMtyTOGGLE_OFF,
    ITEMtyTOGGLE_ON,
    ITEMtySEPARATOR
    } MenuItemType;

/************************************************************************/
/*									*/
/*  Possible responses in one of Question dialogs.			*/
/*									*/
/************************************************************************/

typedef enum DialogResponse
    {
    AQDrespFAILURE= -1,
    AQDrespNONE= 0,
    AQDrespYES,
    AQDrespNO,
    AQDrespCANCEL,
    AQDrespCLOSED,	/*  Cancel, but dialog is closed by the user */
    AQDrespOK,
    AQDresp_COUNT
    } DialogResponse;

/************************************************************************/
/*									*/
/*  Describes a document generically.					*/
/*									*/
/************************************************************************/

typedef struct DocumentWidget
    {
    APP_WIDGET			dwWidget;
    APP_INPUT_CONTEXT		dwInputContext;

				/****************************************/
				/*  Event handling.			*/
				/*  Note that this is not a full GUI	*/
				/*  toolkit!				*/
				/****************************************/
    void *			dwOwner;
    void			(*dwGotString)(	void *		owner,
						const char *	value,
						int		length );
    void			(*dwGotKey)(	void *		owner,
						int		key,
						unsigned int	state );
    } DocumentWidget;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void appMakeButtonInRow(	APP_WIDGET *		pButton,
				APP_WIDGET		buttonRow,
				const char *		text,
				APP_BUTTON_CALLBACK_T	callback,
				void *			through,
				int			position,
				int			colspan,
				int			showAsDefault );

extern void appMakeButtonInColumn(
				APP_WIDGET *		pButton,
				APP_WIDGET		column,
				const char *		buttonText,
				APP_BUTTON_CALLBACK_T	callback,
				void *			through,
				int			showAsDefault );

extern void appMakeLabelInRow(		APP_WIDGET *		pLabel,
					APP_WIDGET		row,
					int			column,
					int			colspan,
					const char *		labelText );

extern void appMakeLabelInHBox(		APP_WIDGET *		pLabel,
					APP_WIDGET		hbox,
					const char *		labelText );

extern void appMakeTextInRow(		APP_WIDGET *		pText,
					APP_WIDGET		row,
					int			column,
					int			colspan,
					int			textColumns,
					int			textEnabled );

extern void appMakeTextInHBox(		APP_WIDGET *		pText,
					APP_WIDGET		hbox,
					int			textColumns,
					int			textEnabled );

extern APP_WIDGET appMakeToggleInRow(	APP_WIDGET		buttonRow,
					const char *		text,
					APP_TOGGLE_CALLBACK_T	callback,
					void *			through,
					int			col,
					int			colspan );

extern void appMakeColumnFrameInColumn(	APP_WIDGET *		pFrame,
					APP_WIDGET *		pPaned,
					APP_WIDGET		parent,
					const char *		title );

extern void appMakeLabelInColumn(	APP_WIDGET *		pLabel,
					APP_WIDGET		row,
					const char *		labelText );

extern void appMakeTextInColumn(	APP_WIDGET *	pText,
					APP_WIDGET	column,
					int		textColumns,
					int		textEnabled );

extern void appMakeColumnToggle(	APP_WIDGET *		pToggle,
					APP_WIDGET		column,
					APP_TOGGLE_CALLBACK_T	callback,
					void *			through,
					const char *		labelText,
					int			set );

extern void appMakeColumnDrawing(	APP_WIDGET *		pButton,
					APP_WIDGET		column,
					APP_DRAW_BUTTON_CALLBACK exposeCallback,
					APP_BUTTON_CALLBACK_T	pushedCallback,
					void *			through,
					int			width );

extern char * 	appWidgetName(		char *			file,
					int			line );

extern void guiEnableText(		APP_WIDGET	text,
					int		enabled );

extern char *	appGetStringFromTextWidget(	APP_WIDGET	w );
extern void	appFreeStringFromTextWidget(	char *		s );

extern char *	appGetTextFromMenuOption(	APP_WIDGET	w );
extern void	appFreeTextFromMenuOption(	char *		s );

extern void appStringToTextWidget(	APP_WIDGET		w,
					const char *		s );

extern void appGuiChangeButtonText(	APP_WIDGET		button,
					const char *		label );

extern APP_WIDGET appMakeRowInColumn(	APP_WIDGET	parent,
					int		columnCount,
					int		heightResizable );

extern void appMakeColumnInRow(		APP_WIDGET *	pColumn,
					APP_WIDGET	row,
					int		position,
					int		colspan );

extern void appMakeHBoxInRow(		APP_WIDGET *	pHbox,
					APP_WIDGET	row,
					int		position,
					int		colspan );

extern void appMakeRowFrameInColumn(	APP_WIDGET *		pFrame,
					APP_WIDGET *		pRow,
					APP_WIDGET		parent,
					int			columnCount,
					const char *		title );

extern void appTextSelectContents(	APP_WIDGET	w,
					int		from,
					int		upto );

extern void appGuiMakeDrawingAreaInColumn( APP_WIDGET *		pDrawing,
				APP_WIDGET			column,
				int				wide,
				int				high,
				int				heightResizable,
				APP_EVENT_HANDLER_T		redraw,
				void *				through );

extern void appGuiChangeLabelText( APP_WIDGET			w,
				const char *			labelt );

extern void appDocumentSetInputContext(		APP_INPUT_METHOD	im,
						DocumentWidget *	dw );

# ifdef USE_GTK
    extern int appGuiGetMenuToggleStateFromCallbackGtk(
					APP_WIDGET		toggle );
    # define appGuiGetMenuToggleStateFromCallback( t, e ) \
			    appGuiGetMenuToggleStateFromCallbackGtk( (t) )
# else
    extern int appGuiGetMenuToggleStateFromCallback(
					APP_WIDGET		toggle,
					void *			voidcbs );
# endif

# ifdef USE_GTK
    extern int appGuiGetScrollbarValueFromCallbackGtk(
					APP_WIDGET		scrollbar );

    # define appGuiGetScrollbarValueFromCallback( bar, e ) \
			    appGuiGetScrollbarValueFromCallbackGtk( (bar) )
# else
    extern int appGuiGetScrollbarValueFromCallback(
					APP_WIDGET		scrollbar,
					void *			e );
# endif

/**
 *  Construct a list of values in the column
 *
 *  @param visibleItems: The number of items to display.
 *	if visibleItems > 0: Allocate sufficient space for this many items. 
 *		and use a scrollbar if there are items more in the list.
 *	if visibleItems == 0: Adapt to the available space in the parent.
 *		Use a scrollbar if there are more items in the list than fit 
 *		in the assigned space.
 *	if visibleItems < 0: Resize with the number of itemsd in the list.
 *
 */
extern void appGuiMakeListInColumn(	APP_WIDGET *		pList,
					APP_WIDGET		column,
					int			visibleItems,
					APP_LIST_CALLBACK_T	listCallback,
					APP_BUTTON_CALLBACK_T	actionCallback,
					void *			through );

extern void appGuiSetTypingCallbackForText(
					APP_WIDGET		text,
					APP_TXTYPING_CALLBACK_T	callBack,
					void *			through );

extern void appGuiSetGotValueCallbackForText(
				    APP_WIDGET			text,
				    APP_TXACTIVATE_CALLBACK_T	callBack,
				    void *			through );

extern void appGuiGetScrollbarValues(	int *		pValue,
					int *		pSliderSize,
					APP_WIDGET	scrollbar );

extern void appGuiSetScrollbarValues(	APP_WIDGET	scrollbar,
					int		value,
					int		sliderSize );

extern void appGuiEmptyListWidget(	APP_WIDGET		list );

extern void appGuiAddValueToListWidget(	APP_WIDGET		list,
					int			pos,
					const char *		value );

extern void appGuiReplaceValueInListWidget(
					APP_WIDGET		list,
					int			pos,
					const char *		value );

extern void appGuiDeletePositionFromListWidget(
					APP_WIDGET		list,
					int			pos );

extern void appGuiSelectPositionInListWidget(	APP_WIDGET	list,
						int		position );
extern void appGuiSelectValueInListWidget(	APP_WIDGET	list,
						const char *	value );

extern void appGuiRemoveSelectionFromListWidget(	APP_WIDGET	list );

extern void appGuiMakeSliderInRow(	APP_WIDGET *		pSlider,
					APP_WIDGET		row,
					int			column,
					int			colspan,
					APP_BUTTON_CALLBACK_T	callback,
					void *			through );

extern void appGuiSetSlider(		APP_WIDGET		slider,
					int			minVal,
					int			maxVal,
					int			curVal );

extern void appGuiSetSliderValue(	APP_WIDGET		slider,
					int			curVal );

extern int appGuiGetValueFromSliderCallback(	int *		pValue,
						APP_WIDGET	w,
						void *		voidscs );

extern int appGuiGetPositionFromListCallback(
					    APP_WIDGET		list,
					    APP_LIST_CHOICE *	voidlcs );

extern char * appGuiGetStringFromListCallback(
					    APP_WIDGET		list,
					    APP_LIST_CHOICE *	voidlcs );

extern void appFreeStringFromListCallback(	char *		s );

extern void appGuiInsertSeparatorInColumn(	APP_WIDGET *	pSeparator,
						APP_WIDGET	parent );

extern void appSetShellTitle(		APP_WIDGET		shell,
					APP_WIDGET		option,
					const char *		title );

extern void appGuiInsertColumnInWindow(	APP_WIDGET *	pColumn,
					APP_WIDGET	parent );

extern void appGuiInsertMenubarInColumn( APP_WIDGET *	pMenubar,
					APP_WIDGET	parent );

extern void appMakeImageInColumn(	APP_WIDGET *		pLabel,
					APP_WIDGET		column,
					APP_BITMAP_IMAGE	pixmap,
					APP_BITMAP_MASK		mask );

extern void appGuiSetToggleLabel(	APP_WIDGET		toggle,
					const char *		text );

extern void appGuiSetToggleState(	APP_WIDGET		toggle,
					int			set );

extern int appGuiGetToggleState(	APP_WIDGET		toggle );

extern void guiEnableWidget(		APP_WIDGET		w,
					int			on_off );

extern void appGuiSetWidgetVisible(	APP_WIDGET		w,
					int			on_off );

extern void appHideShellWidget(		APP_WIDGET		shell );
extern void appDestroyShellWidget(	APP_WIDGET		shell );

extern void appRefuseTextValue(		APP_WIDGET	w );

# ifdef __cplusplus
    }
# endif

#   endif
