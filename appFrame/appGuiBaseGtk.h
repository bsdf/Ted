/************************************************************************/
/*									*/
/*  GTK definitions for appGuiBase.h. NEVER INCLUDE DIRECTLY, BUT	*/
/*  VIA appGuiBase.h							*/
/*									*/
/************************************************************************/

#   ifdef USE_GTK	/*  { */

#   include	<gtk/gtk.h>

#   define	USE_X11_FONTS	1

/************************************************************************/
/*									*/
/*  Basic GUI primitives.						*/
/*									*/
/************************************************************************/

#   define KEY_CONTROL_MASK		GDK_CONTROL_MASK
#   define KEY_SHIFT_MASK		GDK_SHIFT_MASK

# ifdef GTK_TYPE_IM_CONTEXT
    typedef	GtkIMContext *		APP_INPUT_CONTEXT;
# else
    typedef	int			APP_INPUT_CONTEXT;
# endif

typedef		int			APP_INPUT_METHOD;
typedef		guint			(APP_KEY_VALUE);

					/********************************/
					/*  Widgets and widget types.	*/
					/********************************/
typedef		GtkWidget		(*APP_WIDGET);

					/********************************/
					/*  Drawing related types.	*/
					/********************************/
typedef		GdkGC			(*APP_GC);
typedef		GdkPixmap		(*APP_BITMAP_IMAGE);
typedef		GdkBitmap		(*APP_BITMAP_MASK);
typedef		GdkColor		(APP_COLOR_RGB);
typedef		GdkDrawable		(*APP_DRAWABLE);
typedef		GdkWindow		(*APP_WINDOW);
typedef		GdkCursor		(*APP_CURSOR);
typedef		GdkAtom			(APP_ATOM);
typedef		GdkPoint		(APP_POINT);
typedef		GdkImage		(APP_IMAGE);

typedef		GtkSignalFunc		(APP_CALLBACK_FUNC);

typedef		GtkSignalFunc		(APP_MENU_CALLBACK);
typedef		GtkSignalFunc		(APP_DRAW_BUTTON_CALLBACK);
typedef		GtkFunction		(APP_TIMER_CALLBACK);

typedef		GdkEvent		(APP_EVENT);
typedef		GdkEventButton		(APP_MOUSE_CLICK_EVENT);

typedef		GtkSelectionData	(APP_SELECTION_EVENT);

/************************************************************************/
/*									*/
/*  Callback/Handler definitions.					*/
/*									*/
/************************************************************************/

typedef void (*APP_GIVE_COPY)(		GtkWidget *		w,
					APP_SELECTION_EVENT *	event,
					guint			info,
					guint			tim,
					void *			through );

typedef void (*APP_PASTE_REPLY)(	GtkWidget *		w,
					APP_SELECTION_EVENT *	gsd,
					guint			tim,
					void *			voided );

#define		APP_GIVE_COPY(n,w,e,t)					\
		void n(			GtkWidget *		w,	\
					APP_SELECTION_EVENT *	e,	\
					guint			info,	\
					guint			tim,	\
					void *			t	)

#define		APP_PASTE_REPLY(n,w,e,t)				\
		void n(			GtkWidget *		w,	\
					APP_SELECTION_EVENT *	e,	\
					guint			tim,	\
					void *			t	)

/****/
#define		APP_TOGGLE_CALLBACK_H(n,w,t,e)				\
		void n(			GtkWidget *		w,	\
					void *			t	)

typedef		APP_TOGGLE_CALLBACK_H ((*APP_TOGGLE_CALLBACK_T),w,t,e);

/****/
#define		APP_SCROLLBAR_CALLBACK_H(n,w,t,e)			\
		void n(			GtkWidget *		w,	\
					void *			t	)

typedef		APP_SCROLLBAR_CALLBACK_H ((*APP_SCROLLBAR_CALLBACK_T),w,t,e);

/****/
#define		APP_SLIDER_CALLBACK_H(n,w,t,e)			\
		void n(			GtkWidget *		w,	\
					void *			t	)

typedef		APP_SLIDER_CALLBACK_H ((*APP_SLIDER_CALLBACK_T),w,t,e);

/****/
#define		APP_MENU_CALLBACK_H(n,w,t,e)				\
		void n(			GtkWidget *		w,	\
					void *			t	)

typedef		APP_MENU_CALLBACK_H ((*APP_MENU_CALLBACK_T),w,t,e);

# define appGuiGetToggleStateFromCallback( w, e ) \
			appGuiGetToggleStateFromCallbackGtk( (w) )

extern int appGuiGetToggleStateFromCallbackGtk( GtkWidget * w );

/****/
#define		APP_BUTTON_CALLBACK_H(n,w,t)				\
		void n(			GtkWidget *		w,	\
					void *			t	)

typedef		APP_BUTTON_CALLBACK_H ((*APP_BUTTON_CALLBACK_T),w,t);

/****/
#define		APP_EVENT_HANDLER_H(n,w,t,e)				\
		void n(			GtkWidget *		w,	\
					GdkEvent *		e,	\
					void *			t	)

typedef		APP_EVENT_HANDLER_H ((*APP_EVENT_HANDLER_T),w,t,e);

/****/
#define		APP_CLOSE_CALLBACK_H(n,w,t)				\
		void n(			GtkWidget *		w,	\
					GdkEvent *		e,	\
					void *			t	)

typedef		APP_CLOSE_CALLBACK_H ((*APP_CLOSE_CALLBACK_T),w,t);

/****/
#define		APP_TXTYPING_CALLBACK_H(n,w,t)				\
		void n(			GtkWidget *		w,	\
					void *			t	)

typedef		APP_TXTYPING_CALLBACK_H ((*APP_TXTYPING_CALLBACK_T),w,t);

/****/
#define		APP_TXACTIVATE_CALLBACK_H(n,w,t)			\
		void n(			GtkWidget *		w,	\
					void *			t	)

typedef		APP_TXACTIVATE_CALLBACK_H ((*APP_TXACTIVATE_CALLBACK_T),w,t);

/****/

typedef		GtkTreeSelection	APP_LIST_CHOICE;

#define		APP_LIST_CALLBACK_H(n,w,t,c)				\
		void n(			GtkWidget *		w,	\
					APP_LIST_CHOICE *	c,	\
					void *			t	)

typedef		APP_LIST_CALLBACK_H ((*APP_LIST_CALLBACK_T),w,t,c);

/****/
# if 0
For "destroy-event" that does not arrive
#define		APP_DESTROY_CALLBACK_H(n,w,t)				\
		void n(			GtkWidget *		w,	\
					GdkEvent *		e,	\
					void *			t	)

# else
#define		APP_DESTROY_CALLBACK_H(n,w,t)				\
		void n(			GtkWidget *		w,	\
					void *			t	)

typedef		APP_DESTROY_CALLBACK_H ((*APP_DESTROY_CALLBACK_T),w,t);
# endif


/****/
#define		APP_TIMER_HANDLER(n,d)					\
		int n(			void *			d	)


typedef struct AppDialog
    {
    GtkWidget *		adTopWidget;
    int			adResponse;
    guint		adCloseId;
    GtkAccelGroup *	adAccelGroup;
    } AppDialog;

typedef struct AppToplevel
    {
    GtkWidget *		atTopWidget;
    GtkAccelGroup *	atAccelGroup;
    } AppToplevel;

/*
#   define	COLUMN_SPACING_GTK	1
#   define	WINDOW_BORDER_GTK	4
#   define	FRAME_BORDER_GTK	4
#   define	ROW_XPADDING_GTK	3
#   define	ROW_YPADDING_GTK	2
*/

#   define	COLUMN_SPACING_GTK	1
#   define	WINDOW_BORDER_GTK	4
#   define	FRAME_BORDER_GTK	4
#   define	ROW_XPADDING_GTK	1
#   define	ROW_YPADDING_GTK	0

/************************************************************************/
/*									*/
/*  Line attributes.							*/
/*									*/
/************************************************************************/

#   define	LINEstyleSOLID		GDK_LINE_SOLID
#   define	LINEstyleON_OFF_DASH	GDK_LINE_ON_OFF_DASH
#   define	LINEstyleDOUBLE_DASH	GDK_LINE_DOUBLE_DASH

#   define	LINEcapNOT_LAST		GDK_CAP_NOT_LAST
#   define	LINEcapBUTT		GDK_CAP_BUTT
#   define	LINEcapROUND		GDK_CAP_ROUND
#   define	LINEcapPROJECTING	GDK_CAP_PROJECTING

#   define	LINEjoinMITER		GDK_JOIN_MITER
#   define	LINEjoinROUND		GDK_JOIN_ROUND
#   define	LINEjoinBEVEL		GDK_JOIN_BEVEL

# endif		/* } USE_GTK */

