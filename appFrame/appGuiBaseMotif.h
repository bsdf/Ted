/************************************************************************/
/*									*/
/*  Motif definitions for appGuiBase.h. NEVER INCLUDE DIRECTLY, BUT	*/
/*  VIA appGuiBase.h							*/
/*									*/
/************************************************************************/

#   ifdef USE_MOTIF /* { */

#   include	<X11/Xlib.h>
#   include	<X11/Intrinsic.h>
#   include	<Xm/Xm.h>
#   include	<X11/IntrinsicP.h>

#   if XtVersion < 11006
#	define	USE_X11_R5	1
#   endif

#   define	USE_X11_FONTS	1

#   if 1
#	define	WIDGET_NAME	(char *)""
#   else
#	define	WIDGET_NAME	appWidgetName( __FILE__, __LINE__ )
#   endif


#   define KEY_CONTROL_MASK		ControlMask
#   define KEY_SHIFT_MASK		ShiftMask

typedef		XIC			APP_INPUT_CONTEXT;
typedef		XIM			APP_INPUT_METHOD;
typedef		KeySym			(APP_KEY_VALUE);

typedef		GC			APP_GC;

typedef		Widget			APP_WIDGET;

typedef		Pixmap			(APP_BITMAP_IMAGE);
typedef		Pixmap			(APP_BITMAP_MASK);
typedef		XColor			(APP_COLOR_RGB);
typedef		Drawable		(APP_DRAWABLE);
typedef		Window			(APP_WINDOW);
typedef		Cursor			(APP_CURSOR);
typedef		Atom			(APP_ATOM);
typedef		XPoint			(APP_POINT);
typedef		XImage			(APP_IMAGE);

typedef		XtCallbackProc		(APP_CALLBACK_FUNC);

typedef		XtCallbackProc		(APP_MENU_CALLBACK);
typedef		XtCallbackProc		(APP_DRAW_BUTTON_CALLBACK);

typedef		XtTimerCallbackProc	(APP_TIMER_CALLBACK);

typedef		XEvent			(APP_EVENT);
typedef		XButtonEvent		(APP_MOUSE_CLICK_EVENT);

typedef		XEvent			(APP_SELECTION_EVENT);

/************************************************************************/
/*									*/
/*  Callback/Handler definitions.					*/
/*									*/
/************************************************************************/

typedef void (*APP_GIVE_COPY)(	Widget			w,
				void *			through,
				APP_SELECTION_EVENT *	e,
				Boolean *		pRefused );

typedef void (*APP_PASTE_REPLY)( Widget			w,
				void *			through,
				APP_SELECTION_EVENT *	e,
				Boolean *		pRefused );

#define		APP_GIVE_COPY(n,w,e,t)					\
		void n(			Widget			w,	\
					void *			t,	\
					APP_SELECTION_EVENT *	e,	\
					Boolean *		pRefused )

#define		APP_PASTE_REPLY(n,w,e,t)				\
		void n(			Widget			w,	\
					void *			t,	\
					APP_SELECTION_EVENT *	e,	\
					Boolean *		pRefused )

/****/
#define		APP_TOGGLE_CALLBACK_H(n,w,t,e)				\
		void n(			Widget			w,	\
					void *			t,	\
					void *			e	)

typedef		APP_TOGGLE_CALLBACK_H ((*APP_TOGGLE_CALLBACK_T),w,t,e);

/****/
#define		APP_SCROLLBAR_CALLBACK_H(n,w,t,e)			\
		void n(			Widget			w,	\
					void *			t,	\
					void *			e	)

typedef		APP_SCROLLBAR_CALLBACK_H ((*APP_SCROLLBAR_CALLBACK_T),w,t,e);

/****/
#define		APP_SLIDER_CALLBACK_H(n,w,t,e)			\
		void n(			Widget			w,	\
					void *			t,	\
					void *			e	)

typedef		APP_SLIDER_CALLBACK_H ((*APP_SLIDER_CALLBACK_T),w,t,e);

/****/
#define		APP_MENU_CALLBACK_H(n,w,t,e)				\
		void n(			Widget			w,	\
					void *			t,	\
					void *			e	)

typedef		APP_MENU_CALLBACK_H ((*APP_MENU_CALLBACK_T),w,t,e);

# define appGuiGetToggleStateFromCallback( w, e ) \
			appGuiGetToggleStateFromCallbackMotif( (e) )

extern int appGuiGetToggleStateFromCallbackMotif( void * e );

/****/
#define		APP_BUTTON_CALLBACK_H(n,w,t)				\
		void n(			Widget			w,	\
					void *			t,	\
					void *			pbcs	)

typedef		APP_BUTTON_CALLBACK_H ((*APP_BUTTON_CALLBACK_T),w,t);

/****/
#define		APP_EVENT_HANDLER_H(n,w,t,e)			\
		void n(			Widget		w,	\
					void *		t,	\
					XEvent *	e,	\
					Boolean *	pRefused )

typedef		APP_EVENT_HANDLER_H ((*APP_EVENT_HANDLER_T),w,t,e);

/****/
#define		APP_CLOSE_CALLBACK_H(n,w,t)				\
		void n(			Widget			w,	\
					void *			t,	\
					void *			e	)

typedef		APP_CLOSE_CALLBACK_H ((*APP_CLOSE_CALLBACK_T),w,t);

/****/
#define		APP_TXTYPING_CALLBACK_H(n,w,t)				\
		void n(			Widget			w,	\
					void *			t,	\
					void *			e	)

typedef		APP_TXTYPING_CALLBACK_H ((*APP_TXTYPING_CALLBACK_T),w,t);

/****/
#define		APP_TXACTIVATE_CALLBACK_H(n,w,t)			\
		void n(			Widget			w,	\
					void *			t,	\
					void *			e	)

typedef		APP_TXACTIVATE_CALLBACK_H ((*APP_TXACTIVATE_CALLBACK_T),w,t);

/****/
typedef		void			APP_LIST_CHOICE;

#define		APP_LIST_CALLBACK_H(n,w,t,c)				\
		void n(			Widget			w,	\
					void *			t,	\
					APP_LIST_CHOICE *	c )

typedef		APP_LIST_CALLBACK_H ((*APP_LIST_CALLBACK_T),w,t,c);

/****/
#define		APP_DESTROY_CALLBACK_H(n,w,t)				\
		void n(			Widget			w,	\
					void *			t,	\
					void *			e	)

typedef		APP_DESTROY_CALLBACK_H ((*APP_DESTROY_CALLBACK_T),w,t);

/****/

#define		APP_TIMER_HANDLER(n,d)					\
		void n(			void *			d,	\
					XtIntervalId *		xii	)

typedef struct AppDialog
    {
    Widget	adTopWidget;
    Widget	adDialog;
    int		adResponse;
    int		adContinue;
    } AppDialog;

typedef struct AppToplevel
    {
    Widget		atTopWidget;
    } AppToplevel;

/************************************************************************/
/*									*/
/*  Common values for geometry related resources.			*/
/*									*/
/************************************************************************/

#   define	PWmargW		3
#   define	PWmargH		3
#   define	PWspacing	3

#   define	TXmargH		2

#   define	TBrmargT	4
#   define	TBcmargT	1
#   define	TBcmargH	3

/************************************************************************/
/*									*/
/*  Line attributes.							*/
/*									*/
/************************************************************************/

#   define	LINEstyleSOLID		LineSolid
#   define	LINEstyleON_OFF_DASH	LineOnOffDash
#   define	LINEstyleDOUBLE_DASH	LineDoubleDash

#   define	LINEcapNOT_LAST		CapNotLast
#   define	LINEcapBUTT		CapButt
#   define	LINEcapROUND		CapRound
#   define	LINEcapPROJECTING	CapProjecting

#   define	LINEjoinMITER		JoinMiter
#   define	LINEjoinROUND		JoinRound
#   define	LINEjoinBEVEL		JoinBevel

/************************************************************************/
/*									*/
/*  Utility Routines							*/
/*									*/
/************************************************************************/

extern void appFinishOptionmenuMotif(		APP_WIDGET	menu,
						APP_WIDGET	pulldown );

extern void appGuiMotifSetFocusToWindow(	APP_WIDGET	w );

extern void appGuiMotifWaitForWindow(	APP_WIDGET		shell,
					XtAppContext		context );

extern void appMotifTurnOfSashTraversal(	APP_WIDGET	column );

extern const char * const APP_X11EventNames[];

#   endif	/* } USE_MOTIF */
