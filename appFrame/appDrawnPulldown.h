/************************************************************************/
/*									*/
/*  Drawn pulldowns: For color chooser etc.				*/
/*									*/
/************************************************************************/

#   include	"guiWidgets.h"
#   include	<drawDrawingSurface.h>

struct AppDrawnPulldown;
typedef void (*AppDrawnPulldownPuldown)( void * through );

typedef struct AppDrawnPulldown
    {
    APP_WIDGET			adpInplaceDrawing;
    APP_WIDGET			adpPulldownShell;
    APP_WIDGET			adpPulldownDrawing;
    APP_EVENT_HANDLER_T		adpClickHandler;
    AppDrawnPulldownPuldown	adpPulldown;
    void *			adpThrough;
    int				adpMouseX;
    int				adpMouseY;
    unsigned char		adpEnabled;

    DrawingSurface		adpInplaceDrawingSurface;
    DrawingSurface		adpPulldownDrawingSurface;
    RGB8Color			adpInplaceBackgroundColor;

#   ifdef USE_MOTIF
    Time			adpFirstDownTime;
#   endif
    } AppDrawnPulldown;

/************************************************************************/
/*									*/
/*  Declarations.							*/
/*									*/
/************************************************************************/

extern void appMakeDrawnPulldownInColumn(
				AppDrawnPulldown *		adp,
				APP_EVENT_HANDLER_T		redrawInplace,
				APP_EVENT_HANDLER_T		redrawPulldown,
				APP_EVENT_HANDLER_T		clickHandler,
				AppDrawnPulldownPuldown		pullDown,
				APP_WIDGET			column,
				void *				through );

extern void appMakeDrawnPulldownInRow(
				AppDrawnPulldown *		adp,
				APP_EVENT_HANDLER_T		redrawInplace,
				APP_EVENT_HANDLER_T		redrawPulldown,
				APP_EVENT_HANDLER_T		clickHandler,
				AppDrawnPulldownPuldown		pullDown,
				APP_WIDGET			row,
				int				column,
				int				colspan,
				void *				through );

extern void appGuiEnableDrawnPulldown(	AppDrawnPulldown *	adp,
					int			sensitive );

extern void appInitDrawnPulldown(	AppDrawnPulldown *	adp );
extern void appCleanDrawnPulldown(	AppDrawnPulldown *	adp );

extern void appGuiSetDrawnPulldownHeight(
				    AppDrawnPulldown *	adp,
				    int			height );

extern void appGuiSetDrawnPulldownStrips(
				    AppDrawnPulldown *	adp,
				    int			strips );

extern int appGuiDrawnPulldownGetStrip(
				int *				pStrip,
				const AppDrawnPulldown *	adp,
				const APP_WIDGET		w,
				const APP_EVENT *		mouseEvent );

extern void appDrawnPulldownDrawArrow(	const DocumentRectangle * drClip,
					int *			pRestWide,
					int *			pRestHigh,
					APP_WIDGET		w,
					AppDrawnPulldown *	adp );

extern int appFinishDrawnPulldownInplace(	AppDrawnPulldown *	adp );
extern int appFinishDrawnPulldownPulldown(	AppDrawnPulldown *	adp );

extern void appExposeDrawnPulldownInplace(	AppDrawnPulldown *	adp );

