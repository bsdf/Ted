/************************************************************************/
/*									*/
/*  Option menus.							*/
/*									*/
/************************************************************************/

#   include	"appGuiBase.h"

#   ifndef	GUI_OPTMENU_H
#   define	GUI_OPTMENU_H

typedef void (*OptionmenuCallback)(	int		n,
					void *		through );

#   ifdef USE_GTK

typedef struct AppOptionmenu
    {
    APP_WIDGET			aomInplace;
    APP_WIDGET			aomPulldown;

    OptionmenuCallback		aomCallback;
    void *			aomTarget;
    } AppOptionmenu;

#   endif

#   ifdef USE_MOTIF

typedef struct AppOptionmenu
    {
    APP_WIDGET			aomInplace;
    APP_WIDGET			aomPulldown;

    OptionmenuCallback		aomCallback;
    void *			aomTarget;

    int				aomOptionsVisible;
    } AppOptionmenu;

#   endif

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void appMakeOptionmenuInColumn(	AppOptionmenu *		aom,
					APP_WIDGET		parent,
					OptionmenuCallback	callBack,
					void *			through );

extern void appMakeOptionmenuInRow(	AppOptionmenu *		aom,
					APP_WIDGET		row,
					int			column,
					int			colspan,
					OptionmenuCallback	callBack,
					void *			through );

extern void appOptionmenuRefreshWidth(	AppOptionmenu *		aom );

extern void appEmptyOptionmenu(		AppOptionmenu *		aom );

extern void appGuiEnableOptionmenu(	AppOptionmenu *		aom,
					int			sensitive );

extern APP_WIDGET appAddItemToOptionmenu( AppOptionmenu *	aom,
					const char *		label );

extern void appDeleteItemFromOptionmenu( AppOptionmenu *	aom,
					APP_WIDGET		item );

extern void appSetOptionmenu(		AppOptionmenu *		aom,
					int			num );

extern void appInitOptionmenu(		AppOptionmenu *		aom );

extern void appOptionmenuItemSetVisibility(	APP_WIDGET	w,
						int		visible );

#   endif		/* GUI_OPTMENU_H */
