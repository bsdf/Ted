#   include	"appFrameConfig.h"
#   include	"appGuiBase.h"

#   ifdef USE_GTK

typedef struct ListAction
    {
    GtkWidget *			laWidget;
    GtkWidget *			laParentWidget;
    GtkAdjustment *		laAdjustment;
    APP_LIST_CALLBACK_T		laListCallback;
    APP_BUTTON_CALLBACK_T	laActionCallback;
    void *			laThrough;

    int				laListHigh;
    int				laParentHigh;
    int				laItemCount;
    int				laSelectedPosition;
    } ListAction;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern GtkWidget * appListGtkMakeParent( GtkAdjustment **	pVAdjustment,
					GtkWidget *		column );

extern void appListGtkSetVisibleSize(	GtkWidget *		tree,
					GtkWidget *		scrolled_window,
					int			visibleItems );

extern ListAction * appListGtkSetCallbacks(
					GtkWidget *		tree,
					GtkWidget *		scrolled_window,
					GtkAdjustment *		adjustment,
					APP_LIST_CALLBACK_T	listCallback,
					APP_BUTTON_CALLBACK_T	actionCallback,
					void *			through );


extern ListAction * appListGtkGetListAction(
					GtkWidget *		widget );

extern void appGuiGtkSetListAdjustment(	ListAction *		la,
					int			position );

#   endif
