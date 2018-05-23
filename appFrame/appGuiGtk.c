#   include	"appFrameConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<string.h>

#   include	"appFrame.h"
#   include	"guiWidgetsGtk.h"
#   include	"guiWidgetsImpl.h"
#   include	"guiDrawingWidget.h"

#   include	<appDebugon.h>

#   ifdef USE_GTK

/************************************************************************/
/*									*/
/*  Destroy all children of a widget.					*/
/*									*/
/************************************************************************/

void appEmptyParentWidget(	APP_WIDGET		parent )
    {
    gtk_container_foreach( GTK_CONTAINER( parent ),
				    (GtkCallback)gtk_widget_destroy, NULL );
    }

/************************************************************************/
/*									*/
/*  Make the initial connection to the gui environment.			*/
/*									*/
/************************************************************************/

int appGuiInitApplication(	EditApplication *	ea,
				int *			pArgc,
				char ***		pArgv )
    {
    int			argc= *pArgc;
    char **		argv= *pArgv;

    GtkWidget *		gWindow;
    GtkAccelGroup *	gAccelGroup;

    gtk_set_locale();

    if  ( ea->eaStyleToolInt >= 0 )
	{ gtk_rc_add_default_file( CONFDIR "/ted.gtkrc-2.0" );	}

    gtk_init( &argc, &argv );

    gWindow= gtk_window_new( GTK_WINDOW_TOPLEVEL );
    ea->eaToplevel.atTopWidget= gWindow;

    /* user can't minimize or maximize */
    gtk_window_set_policy( GTK_WINDOW( gWindow), FALSE, FALSE, FALSE);
    gtk_window_set_position( GTK_WINDOW( gWindow), GTK_WIN_POS_CENTER);

    appSetCloseCallback( gWindow, ea, appAppWmClose, (void*)ea );

    /* 
     * Facilities for making menu option accelerators.
     */
    gAccelGroup= gtk_accel_group_new();
    ea->eaToplevel.atAccelGroup= gAccelGroup;

#   if GTK_MAJOR_VERSION < 2
    gtk_accel_group_attach( gAccelGroup, GTK_OBJECT( gWindow));
#   else
    gtk_window_add_accel_group( GTK_WINDOW( gWindow ), gAccelGroup );
#   endif

    /* 
     * Do this here so that we don't get an error when
     * we create the pixmap using the window.
     */
    gtk_widget_realize( gWindow );

    ea->eaScreenPixelsWide= gdk_screen_width();
    ea->eaScreenPixelsHigh= gdk_screen_height();

    *pArgc= argc;
    *pArgv= argv;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Insert a column in a top level widget (Window)			*/
/*									*/
/************************************************************************/

void appGuiInsertColumnInWindow(	APP_WIDGET *	pColumn,
					APP_WIDGET	shell )
    {
    GtkWidget *		column;

    column= gtk_vbox_new( FALSE, COLUMN_SPACING_GTK );

    gtk_widget_show( column );

    gtk_container_add( GTK_CONTAINER( shell ), column );

    *pColumn= column; return;
    }

/************************************************************************/
/*									*/
/*  Insert a menubar as the first row of a column.			*/
/*									*/
/************************************************************************/

void appGuiInsertMenubarInColumn(	APP_WIDGET *	pMenubar,
					APP_WIDGET	column )
    {
    GtkWidget *		menubar= gtk_menu_bar_new();

    gtk_widget_show( menubar );

    gtk_box_pack_start( GTK_BOX( column ), menubar, FALSE, TRUE, 0 );

    *pMenubar= menubar;
    return;
    }

/************************************************************************/
/*									*/
/*  Calculate magnification factors.					*/
/*									*/
/************************************************************************/

void appGetPixelsPerTwip( EditApplication *	ea )
    {
#   if 0
    double		horPixPerMM;
    double		verPixPerMM;
    double		xfac;
    double		yfac;

    horPixPerMM= (double)gdk_screen_width()/ (double)gdk_screen_width_mm();
    verPixPerMM= (double)gdk_screen_height()/ (double)gdk_screen_height_mm();

    xfac=  ( 25.4/ ( 20.0* 72.0 ) )* horPixPerMM;
    yfac=  ( 25.4/ ( 20.0* 72.0 ) )* verPixPerMM;

    ea->eaPixelsPerTwip= xfac;
#   else
    ea->eaPixelsPerTwip= 96.0/ ( 20* 72 );
#   endif

    return;
    }

/************************************************************************/
/*									*/
/*  Enable/Dispable a widget.						*/
/*									*/
/*  NOTE that for text edit boxes there is a different routine.		*/
/*									*/
/************************************************************************/

void guiEnableWidget(	APP_WIDGET		w,
			int			on_off )
    {
    gtk_widget_set_sensitive( w, on_off != 0 );
    }


void appGuiSetWidgetVisible(	APP_WIDGET		w,
				int			on_off )
    {
    if  ( on_off )
	{ gtk_widget_show( w );	}
    else{ gtk_widget_hide( w );	}
    }

/************************************************************************/
/*									*/
/*  Use the text of an option and the name of the application as a	*/
/*  widget title.							*/
/*									*/
/************************************************************************/

void appSetShellTitle(	APP_WIDGET		shell,
			APP_WIDGET		option,
			const char *		applicationName )
    {
    char *	title;

    if  ( option )
	{
	char *		s;

	s= appGetTextFromMenuOption( option );

	title= (char *)malloc( strlen( applicationName )+ 1+ strlen( s )+ 1 );
	sprintf( title, "%s %s", applicationName, s );

	appFreeTextFromMenuOption( s );
	}
    else{ title= strdup( applicationName );	}

    gtk_window_set_title( GTK_WINDOW( shell ), title );

    free( title );
    }

void appGuiSetShellTitle(	APP_WIDGET		shell,
				const MemoryBuffer *	fullTitle )
    {
    gtk_window_set_title( GTK_WINDOW( shell ),
				    utilMemoryBufferGetString( fullTitle ) );

    return;
    }

void appGuiSetIconTitle(	APP_WIDGET		shell,
				const MemoryBuffer *	iconName )
    {
    gdk_window_set_icon_name( shell->window,
				    utilMemoryBufferGetString( iconName ) );
    return;
    }

void appGuiSetScrollbarValues(		APP_WIDGET	scrollbar,
					int		value,
					int		sliderSize )
    {
    GtkAdjustment *	adjustment;

    adjustment= gtk_range_get_adjustment( GTK_RANGE( scrollbar ) );

    adjustment->value= value;
    adjustment->page_size= sliderSize;

    gtk_adjustment_changed( adjustment );

    return;
    }

void appGuiGetScrollbarValues(		int *		pValue,
					int *		pSliderSize,
					APP_WIDGET	scrollbar )
    {
    GtkAdjustment *	adjustment;

    adjustment= gtk_range_get_adjustment( GTK_RANGE( scrollbar ) );

    *pValue= adjustment->value;
    *pSliderSize= adjustment->page_size;

    return;
    }

void appShowShellWidget(		EditApplication *	ea,
					APP_WIDGET		shell )
    {
    gtk_widget_show( shell );
    gtk_window_present( GTK_WINDOW( shell ) );
    gdk_window_raise( shell->window );
    }

void appHideShellWidget(		APP_WIDGET		shell )
    {
    gtk_widget_hide( shell );
    }

void appDestroyShellWidget(		APP_WIDGET		shell )
    {
    gtk_widget_destroy( shell );
    }

/************************************************************************/
/*									*/
/*  Insert a separator as a row of a column.				*/
/*									*/
/************************************************************************/

void appGuiInsertSeparatorInColumn(	APP_WIDGET *	pSeparator,
					APP_WIDGET	column )
    {
    GtkWidget *		separator= gtk_hseparator_new();

    gtk_box_pack_start( GTK_BOX( column ), separator, FALSE, TRUE, 0 );

    gtk_widget_show( separator );

    *pSeparator= separator;

    return;
    }

void appGuiMakeDrawingAreaInColumn( APP_WIDGET *		pDrawing,
				    APP_WIDGET			column,
				    int				wide,
				    int				high,
				    int				heightResizable,
				    APP_EVENT_HANDLER_T		redraw,
				    void *			through )
    {
    GtkWidget *	drawing= gtk_drawing_area_new();
    gboolean	expand= FALSE;
    gboolean	fill= TRUE;

    if  ( wide < 0 )
	{ wide= 0;	}
    if  ( high < 0 )
	{ high= 0;	}

    if  ( high <= 0 || heightResizable )
	{ expand= TRUE;	}

    gtk_box_pack_start( GTK_BOX( column ), drawing, expand, fill, 0 );

    gtk_drawing_area_size( GTK_DRAWING_AREA( drawing ), wide, high );

    gtk_widget_show( drawing );

    *pDrawing= drawing;

    if  ( redraw )
	{ guiDrawSetRedrawHandler( drawing, redraw, through );	}

    return;
    }

/************************************************************************/
/*									*/
/*  Make a tool with a vertical organisation.				*/
/*									*/
/************************************************************************/

void appMakeVerticalTool(	APP_WIDGET *		pShell,
				APP_WIDGET *		pColumn,
				EditApplication *	ea,
				APP_BITMAP_IMAGE	iconPixmap,
				APP_BITMAP_MASK		iconMask,
				int			userResizable,
				APP_WIDGET		option,
				APP_CLOSE_CALLBACK_T	closeCallback,
				void *			through )
    {
    GtkWidget *		shell;
    GtkWidget *		column;

    shell= gtk_window_new( GTK_WINDOW_TOPLEVEL );

    appSetCloseCallback( shell, ea, closeCallback, through );

    appSetShellTitle( shell, option, ea->eaApplicationName );

    column= gtk_vbox_new( FALSE, COLUMN_SPACING_GTK );

    gtk_container_set_border_width( GTK_CONTAINER( shell ), WINDOW_BORDER_GTK );
    gtk_container_add( GTK_CONTAINER( shell ), column );

    gtk_widget_show( column );

    *pShell= shell;
    *pColumn= column;
    return;
    }

/************************************************************************/
/*									*/
/*  Insert a frame in a column. The frame is to be a column itself.	*/
/*									*/
/************************************************************************/

static void appGuiGtkMakeFrameInColumn(	APP_WIDGET *		pFrame,
					APP_WIDGET *		pTable,
					APP_WIDGET		parent,
					APP_WIDGET		child,
					const char *		title )
    {
    GtkWidget *		frame;
    GtkWidget *		table;

    frame= gtk_frame_new( title );

    gtk_frame_set_shadow_type( GTK_FRAME( frame ), GTK_SHADOW_ETCHED_IN );

    gtk_box_pack_start( GTK_BOX( parent ), frame, FALSE, TRUE, 0 );

    gtk_widget_show( frame );

    table= gtk_table_new( 1, 1, TRUE );

    gtk_container_add( GTK_CONTAINER( frame ), table );

    gtk_widget_show( table );

    gtk_table_attach( GTK_TABLE( table ),
			child,
			0, 1,
			0, 1,
			GTK_FILL | GTK_EXPAND | GTK_SHRINK,
			GTK_FILL | GTK_EXPAND | GTK_SHRINK,
			FRAME_BORDER_GTK, FRAME_BORDER_GTK );

    gtk_widget_show( child );

    *pFrame= frame; *pTable= table; return;
    }

void appMakeColumnFrameInColumn(	APP_WIDGET *		pFrame,
					APP_WIDGET *		pPaned,
					APP_WIDGET		parent,
					const char *		title )
    {
    GtkWidget *		frame;
    GtkWidget *		table;
    GtkWidget *		paned;

    paned= gtk_vbox_new( FALSE, COLUMN_SPACING_GTK );

    appGuiGtkMakeFrameInColumn( &frame, &table, parent, paned, title );

    *pFrame= frame; *pPaned= paned; return;
    }

/************************************************************************/
/*									*/
/*  Set the title of a frame.						*/
/*									*/
/************************************************************************/

int appGuiSetFrameTitle(		APP_WIDGET		frame,
					const char *		title )
    {
    gtk_frame_set_label( GTK_FRAME( frame ), (const char *)title );
    return 0;
    }

/************************************************************************/
/*									*/
/*  Insert a frame in a column. The frame is to be a row itself.	*/
/*									*/
/************************************************************************/

void appMakeRowFrameInColumn(	APP_WIDGET *		pFrame,
				APP_WIDGET *		pRow,
				APP_WIDGET		parent,
				int			columnCount,
				const char *		title )
    {
    GtkWidget *		frame;
    GtkWidget *		table;
    GtkWidget *		row;

    row= gtk_table_new( 1, columnCount, TRUE );

    appGuiGtkMakeFrameInColumn( &frame, &table, parent, row, title );

    *pFrame= frame; *pRow= row; return;
    }

int appGuiGetScrollbarValueFromCallbackGtk( APP_WIDGET		scrollbar )
    {
    GtkAdjustment *	adjustment= (GtkAdjustment *)scrollbar;

    return adjustment->value;
    }

void appMakeImageInColumn(	APP_WIDGET *		pImage,
				APP_WIDGET		column,
				APP_BITMAP_IMAGE	pixmap,
				APP_BITMAP_MASK		mask )
    {
    GtkWidget *		image= gtk_pixmap_new( pixmap, mask );

    gtk_box_pack_start( GTK_BOX( column ), image, FALSE, TRUE, 0 );

    gtk_widget_show( image );

    *pImage= image; return;
    }

/************************************************************************/
/*									*/
/*  Run a loop while the left mouse button is down.			*/
/*									*/
/************************************************************************/

typedef struct DragLoop
    {
    APP_EVENT_HANDLER_T		dlUpHandler;
    APP_EVENT_HANDLER_T		dlMoveHandler;
    int				dlTimerInterval;
    GtkFunction			dlTimerHandler;
    void *			dlThrough;

    guint			dlIntervalId;

    guint			dlUpId;
    guint			dlDownId;
    guint			dlMoveId;

    GtkWidget *			dlWidget;
    } DragLoop;

static void appDragMouseUp(		APP_WIDGET		w,
					APP_EVENT *		event,
					void *			vdl )
    {
    DragLoop *		dl= (DragLoop *)vdl;

    if  ( dl->dlIntervalId )
	{
	gtk_timeout_remove( dl->dlIntervalId );
	dl->dlIntervalId= 0;
	}

    if  ( event && dl->dlUpHandler )
	{ (*dl->dlUpHandler)( w, event, dl->dlThrough );	}

    gtk_main_quit();
    }

static void appDragMouseMove(		APP_WIDGET		w,
					APP_EVENT *		event,
					void *			vdl )
    {
    DragLoop *		dl= (DragLoop *)vdl;

    if  ( ! ( event->motion.state & GDK_BUTTON1_MASK ) )
	{
	/* really happens: XXDEB(event->motion.state,GDK_BUTTON1_MASK); */
	appDragMouseUp( w, event, vdl );
	return;
	}

    if  ( dl->dlMoveHandler )
	{ (*dl->dlMoveHandler)( w, event, dl->dlThrough );	}
    }

static int appDragTick(	void *		vdl )
    {
    DragLoop *		dl= (DragLoop *)vdl;

    gint		winX;
    gint		winY;
    GdkModifierType	mask= 0;

    gdk_window_get_pointer( dl->dlWidget->window, &winX, &winY, &mask );

    if  ( ! ( mask & GDK_BUTTON1_MASK ) )
	{
	/* really happens: XXDEB(mask,GDK_BUTTON1_MASK); */
	appDragMouseUp( dl->dlWidget, (APP_EVENT *)0, vdl );
	return 0;
	}

    if  ( dl->dlTimerHandler )
	{ (*dl->dlTimerHandler)( dl->dlThrough );	}

    if  ( dl->dlTimerInterval > 0 )
	{
	dl->dlIntervalId= gtk_timeout_add( dl->dlTimerInterval,
						    appDragTick, (void *)dl );
	}

    return 0;
    }

void appRunDragLoop(	APP_WIDGET		w,
			EditApplication *	ea,
			APP_EVENT *		downEvent,
			APP_EVENT_HANDLER_T	upHandler,
			APP_EVENT_HANDLER_T	moveHandler,
			int			timerInterval,
			APP_TIMER_CALLBACK	timerHandler,
			void *			through )
    {
    DragLoop		dl;

    dl.dlUpHandler= upHandler;
    dl.dlMoveHandler= moveHandler;
    dl.dlTimerInterval= timerInterval;
    dl.dlTimerHandler= timerHandler;
    dl.dlThrough= through;

    dl.dlIntervalId= 0;
    dl.dlWidget= w;

    if  ( dl.dlTimerInterval == 0 )
	{ dl.dlTimerInterval= 200;	}

    gtk_widget_add_events( w,
			GDK_BUTTON_RELEASE_MASK | GDK_BUTTON1_MOTION_MASK );

    dl.dlUpId= gtk_signal_connect( GTK_OBJECT( w ),
					    "button_release_event",
					    (GtkSignalFunc)appDragMouseUp,
					    (void *)&dl );
    dl.dlDownId= gtk_signal_connect( GTK_OBJECT( w ),
					    "button_press_event",
					    (GtkSignalFunc)appDragMouseUp,
					    (void *)&dl );
    dl.dlMoveId= gtk_signal_connect( GTK_OBJECT( w ),
					    "motion_notify_event",
					    (GtkSignalFunc)appDragMouseMove,
					    (void *)&dl );

    if  ( dl.dlTimerInterval > 0 )
	{
	dl.dlIntervalId= gtk_timeout_add( dl.dlTimerInterval,
						appDragTick, (void *)&dl );
	}

    /* Almost hangs X
    gdk_pointer_grab( w->window, FALSE, GDK_BUTTON1_MOTION_MASK,
					    (GdkWindow *)0,
					    (GdkCursor *)0,
					    GDK_CURRENT_TIME );
    */

    gtk_main();

    /*
    gdk_pointer_ungrab( GDK_CURRENT_TIME );
    */

    if  ( dl.dlIntervalId )
	{ gtk_timeout_remove( dl.dlIntervalId );	}

    gtk_signal_disconnect( GTK_OBJECT( w ), dl.dlUpId );
    gtk_signal_disconnect( GTK_OBJECT( w ), dl.dlDownId );
    gtk_signal_disconnect( GTK_OBJECT( w ), dl.dlMoveId );

    return;
    }

void appGuiSetFocusChangeHandler(	APP_WIDGET		w,
					APP_EVENT_HANDLER_T	handler,
					void *			through )
    {
    gtk_widget_add_events( w, GDK_FOCUS_CHANGE_MASK );

    gtk_signal_connect( GTK_OBJECT( w ),
				    "focus_in_event",
				    (GtkSignalFunc)handler, through );

    gtk_signal_connect( GTK_OBJECT( w ), "focus_out_event",
				    (GtkSignalFunc)handler, through );
    }

void appSetDestroyCallback(	APP_WIDGET		shell,
				APP_DESTROY_CALLBACK_T	destroyCallback,
				void *			through )
    {
    if  ( destroyCallback )
	{
	/*
	gtk_signal_connect( GTK_OBJECT( shell ), "destroy_event",
				    (GtkSignalFunc)destroyCallback, through );
	gtk_signal_connect( GTK_OBJECT( shell ), "destroy-event",
				    (GtkSignalFunc)destroyCallback, through );
	*/
	gtk_signal_connect( GTK_OBJECT( shell ), "destroy",
				    (GtkSignalFunc)destroyCallback, through );
	}
    }

void appSetCloseCallback(	APP_WIDGET		shell,
				EditApplication *	ea,
				APP_CLOSE_CALLBACK_T	closeCallback,
				void *			through )
    {
    if  ( closeCallback )
	{
	gtk_signal_connect( GTK_OBJECT( shell ), "delete_event",
				    (GtkSignalFunc)closeCallback, through );
	}
    }

int appGuiGtkSetChildLabel(	GtkWidget *		w,
				const char *		s )
    {
    int		rval= 0;

    if  ( GTK_IS_LABEL( w ) )
	{ gtk_label_set_text( GTK_LABEL( w ), s ); return 1;	}

    if  ( GTK_IS_CONTAINER( w ) )
	{
	GList *	glf;
	GList *	gl;

	gl= glf= gtk_container_children( GTK_CONTAINER( w ) );

	while( gl )
	    {
	    if  ( appGuiGtkSetChildLabel( (GtkWidget *)gl->data, s ) )
		{ rval= 1; break;	}

	    gl= gl->next;
	    }

	if  ( glf )
	    { g_list_free( glf );	}
	}

    return rval;
    }

int appGuiGtkGetChildLabel(	char **			pLabel,
				GtkWidget *		w )
    {
    int		rval= 0;

    if  ( GTK_IS_LABEL( w ) )
	{ gtk_label_get( GTK_LABEL( w ), pLabel ); return 1;	}

    if  ( GTK_IS_CONTAINER( w ) )
	{
	GList *	glf;
	GList *	gl;

	gl= glf= gtk_container_children( GTK_CONTAINER( w ) );

	while( gl )
	    {
	    if  ( appGuiGtkGetChildLabel( pLabel, (GtkWidget *)gl->data ) )
		{ rval= 1; break;	}

	    gl= gl->next;
	    }

	if  ( glf )
	    { g_list_free( glf );	}
	}

    return rval;
    }

void appGuiFocusToWidget(	APP_WIDGET		w )
    {
    gtk_widget_grab_focus( w );
    }

void appGuiGtkListTree(		int			indent,
				APP_WIDGET		w )
    {
    GType		type= GTK_WIDGET_TYPE( w );
    const char *	typeName;
    const char *	instanceName= gtk_widget_get_name( w );

    typeName= g_type_name( type );
    appDebug( "%*s%s%s%s (%d..%d x %d..%d)\n", 2* indent, "",
				typeName,
				( instanceName?" ":"" ),
				( instanceName?instanceName:"" ),
				w->allocation.x,
				w->allocation.x+ w->allocation.width,
				w->allocation.y,
				w->allocation.y+ w->allocation.height );

    if  ( GTK_IS_CONTAINER( w ) )
	{
	GList *	glf;
	GList *	gl;

	gl= glf= gtk_container_children( GTK_CONTAINER( w ) );

	while( gl )
	    {
	    appGuiGtkListTree( indent+ 1, (GtkWidget *)gl->data );

	    gl= gl->next;
	    }

	if  ( glf )
	    { g_list_free( glf );	}
	}

    return;
    }

#   endif
