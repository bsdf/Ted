#   include	"appFrameConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>

#   include	"appFrame.h"
#   include	"appGuiBase.h"
#   include	"guiWidgets.h"
#   include	"guiWidgetsImpl.h"

#   include	<appDebugon.h>

#   ifdef USE_MOTIF

#   include	<X11/Xatom.h>
#   include	<Xm/RowColumn.h>
#   include	<Xm/Label.h>
#   include	<Xm/ToggleB.h>
#   include	<Xm/PanedW.h>
#   include	<Xm/MwmUtil.h>
#   include	<Xm/Protocols.h>
#   include	<Xm/DrawnB.h>
#   include	<Xm/Separator.h>
#   include	<Xm/ScrollBar.h>
#   include	<Xm/DrawingA.h>

/************************************************************************/
/*									*/
/*  Destroy all children of a widget.					*/
/*									*/
/*  1)  Unmanaging should be done by the Xt toolkit but is not always	*/
/*	done correctly. Doing it here does no harm anyway.		*/
/*									*/
/************************************************************************/

void appEmptyParentWidget(	Widget		parent )
    {
    WidgetList		children;
    WidgetList		save;
    Cardinal		childCount;

    XtVaGetValues( parent,
			XmNchildren,		&children,
			XmNnumChildren,		&childCount,
			NULL );

    if  ( childCount == 0 )
	{ return;	}

    /*  1  */
    XtUnmanageChildren( children, childCount );

    save= (WidgetList)malloc( childCount* sizeof(Widget) );
    if  ( save )
	{
	int	i;

	for ( i= childCount- 1; i >= 0; i-- )
	    { save[i]= children[i];	}

	for ( i= childCount- 1; i >= 0; i-- )
	    { XtDestroyWidget( save[i] );	}

	free( save );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Use the text of an option and the name of the application as a	*/
/*  widget title.							*/
/*									*/
/************************************************************************/

void appSetShellTitle(	APP_WIDGET		shell,
			Widget			option,
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

    XtVaSetValues( shell,
		    XmNtitle, title,
		    NULL );

    free( title );
    }

/************************************************************************/
/*									*/
/*  1)  Remember te initial size of a shell as its minimum size.	*/
/*									*/
/*  2)  Remember te initial size of a shell as its minimum and its	*/
/*	maximum size. This event handler can be used to prevent the	*/
/*	user from resizing a dialog. E.G. because KDE does not listen	*/
/*	to the window manager hints about this. On the other hand some	*/
/*	window managers (mwm on vms!) cause this event handler to be	*/
/*	called too early. That results in dialogs that are MUCH too	*/
/*	small.								*/
/*									*/
/************************************************************************/

/*  1  */
void appSetSizeAsMinimum(	Widget			w,
				void *			through,
				XEvent *		event,
				Boolean *		pRefused )
    {
    XConfigureEvent *		cevent= &(event->xconfigure);

    if  ( cevent->type != ConfigureNotify )
	{ return;	}

    XtVaSetValues( w,	XmNminWidth,	cevent->width,
			XmNminHeight,	cevent->height,
			NULL );

    XtRemoveEventHandler( w, StructureNotifyMask, False,
					    appSetSizeAsMinimum, through );

    *pRefused= 1;

    return;
    }

/*  2  */
# define FIX_SIZE 0
# if FIX_SIZE
static void appFixSize(	Widget			w,
			void *			through,
			XEvent *		event,
			Boolean *		pRefused )
    {
    XConfigureEvent *		cevent= &(event->xconfigure);

    if  ( cevent->type != ConfigureNotify )
	{ return;	}

    XtVaSetValues( w,	XmNminWidth,	cevent->width,
			XmNmaxWidth,	cevent->width,
			XmNminHeight,	cevent->height,
			XmNmaxHeight,	cevent->height,
			NULL );

    XtRemoveEventHandler( w, StructureNotifyMask, False,
					    appFixSize, through );

    *pRefused= 1;

    return;
    }
# endif

/************************************************************************/
/*									*/
/*  Names of the X11 events.						*/
/*									*/
/************************************************************************/

const char * const APP_X11EventNames[]=
    {
    "0", "1", "KeyPress", "KeyRelease", "ButtonPress", "ButtonRelease",
    "MotionNotify", "EnterNotify", "LeaveNotify", "FocusIn", "FocusOut",
    "KeymapNotify", "Expose", "GraphicsExpose", "NoExpose", "VisibilityNotify",
    "CreateNotify", "DestroyNotify", "UnmapNotify", "MapNotify", "MapRequest",
    "ReparentNotify", "ConfigureNotify", "ConfigureRequest", "GravityNotify",
    "ResizeRequest", "CirculateNotify", "CirculateRequest", "PropertyNotify",
    "SelectionClear", "SelectionRequest", "SelectionNotify", "ColormapNotify",
    "ClientMessage", "MappingNotify"
    };


APP_WIDGET appMakeToggleInRow(	APP_WIDGET		row,
				const char *		text,
				APP_TOGGLE_CALLBACK_T	callback,
				void *			through,
				int			col,
				int			colspan )
    {
    Arg		al[20];
    int		ac= 0;

    XmString	labelString;
    Widget	toggle;

    if  ( ! text )
	{ text= "";	}

    labelString= XmStringCreateLocalized( (char *)text );

    ac= 0;
    XtSetArg( al[ac], XmNlabelString,		labelString ); ac++;
    XtSetArg( al[ac], XmNtopAttachment,		XmATTACH_FORM ); ac++;
    XtSetArg( al[ac], XmNbottomAttachment,	XmATTACH_FORM ); ac++;
    XtSetArg( al[ac], XmNleftAttachment,	XmATTACH_POSITION ); ac++;
    XtSetArg( al[ac], XmNleftPosition,		col ); ac++;
    XtSetArg( al[ac], XmNrightAttachment,	XmATTACH_POSITION ); ac++;
    XtSetArg( al[ac], XmNrightPosition,		col+ colspan ); ac++;
 
    XtSetArg( al[ac], XmNtopOffset,		0 ); ac++;
    XtSetArg( al[ac], XmNbottomOffset,		0 ); ac++;
    XtSetArg( al[ac], XmNleftOffset,		0 ); ac++;
    XtSetArg( al[ac], XmNrightOffset,		0 ); ac++;

    XtSetArg( al[ac], XmNrecomputeSize,		True ); ac++;

    XtSetArg( al[ac], XmNmarginTop,		TBrmargT ); ac++;

    XtSetArg( al[ac], XmNalignment,		XmALIGNMENT_BEGINNING ); ac++;

    toggle= XmCreateToggleButton( row, WIDGET_NAME, al, ac );

    if  ( callback )
	{ XtAddCallback( toggle, XmNvalueChangedCallback, callback, through ); }

    XmStringFree( labelString );

    XtManageChild( toggle );

    return toggle;
    }

void appMakeImageInColumn(	APP_WIDGET *		pLabel,
				APP_WIDGET		column,
				APP_BITMAP_IMAGE	pixmap,
				APP_BITMAP_MASK		mask )
    {
    Widget		label;

    Arg			al[20];
    int			ac= 0;

    ac= 0;
    XtSetArg( al[ac], XmNlabelType,		XmPIXMAP ); ac++;
    XtSetArg( al[ac], XmNskipAdjust,		True ); ac++;
    XtSetArg( al[ac], XmNrecomputeSize,		True ); ac++;
    XtSetArg( al[ac], XmNallowResize,		True ); ac++;
    XtSetArg( al[ac], XmNalignment,		XmALIGNMENT_BEGINNING ); ac++;

    XtSetArg( al[ac], XmNlabelPixmap,		pixmap ); ac++;

    label= XmCreateLabel( column, WIDGET_NAME, al, ac );

    XtManageChild( label );

    appMotifTurnOfSashTraversal( column );

    *pLabel= label; return;
    }

void appMakeColumnToggle(	Widget *		pToggle,
				Widget			column,
				XtCallbackProc		callback,
				void *			through,
				const char *		labelText,
				int			set )
    {
    Widget		toggle;

    XmString		labelString= (XmString)0;

    Arg			al[20];
    int			ac= 0;

    if  ( labelText )
	{ labelString= XmStringCreateLocalized( (char *)labelText ); }

    ac= 0;
    XtSetArg( al[ac], XmNskipAdjust,		True ); ac++;
    XtSetArg( al[ac], XmNrecomputeSize,		True ); ac++;
    XtSetArg( al[ac], XmNalignment,		XmALIGNMENT_BEGINNING ); ac++;
    XtSetArg( al[ac], XmNset,			set ); ac++;

    XtSetArg( al[ac], XmNmarginTop,		TBcmargT ); ac++;
    XtSetArg( al[ac], XmNmarginHeight,		TBcmargH ); ac++;

    if  ( labelString )
	{ XtSetArg( al[ac], XmNlabelString,	labelString ); ac++; }

    toggle= XmCreateToggleButton( column, WIDGET_NAME, al, ac );

    if  ( callback )
	{ XtAddCallback( toggle, XmNvalueChangedCallback, callback, through ); }

    if  ( labelString )
	{ XmStringFree( labelString );	}

    XtManageChild( toggle );

    *pToggle= toggle;
    }

void appMakeColumnDrawing(	Widget *		pButton,
				Widget			column,
				XtCallbackProc		exposeCallback,
				XtCallbackProc		pushedCallback,
				void *			through,
				int			width )
    {
    Widget		button;

    Arg			al[20];
    int			ac= 0;

    XmString		labelString= XmStringCreateLocalized( (char *)" " );

    ac= 0;
    XtSetArg( al[ac], XmNskipAdjust,		True ); ac++;
    XtSetArg( al[ac], XmNrecomputeSize,		True ); ac++;
    XtSetArg( al[ac], XmNwidth,			width ); ac++;

    XtSetArg( al[ac], XmNmarginTop,		1 ); ac++;
    XtSetArg( al[ac], XmNmarginHeight,		4 ); ac++;

    XtSetArg( al[ac], XmNlabelString,		labelString ); ac++;

    button= XmCreateDrawnButton( column, WIDGET_NAME, al, ac );

    if  ( exposeCallback )
	{
	XtAddCallback( button, XmNexposeCallback, exposeCallback, through );
	}

    if  ( pushedCallback )
	{
	XtAddCallback( button, XmNactivateCallback, pushedCallback, through );
	}

    XtManageChild( button );

    XmStringFree( labelString );

    *pButton= button;
    }

/************************************************************************/
/*									*/
/*  Make a tool with a vertical organisation.				*/
/*									*/
/************************************************************************/

void appMakeVerticalTool(	APP_WIDGET *		pShell,
				APP_WIDGET *		pPaned,
				EditApplication *	ea,
				APP_BITMAP_IMAGE	iconPixmap,
				APP_BITMAP_MASK		iconMask,
				int			userResizable,
				APP_WIDGET		option,
				APP_CLOSE_CALLBACK_T	closeCallback,
				void *			through )
    {
    Widget		shell;
    Widget		paned;

    Arg			al[20];
    int			ac= 0;

    MwmHints		hints;

    hints.flags= MWM_HINTS_FUNCTIONS|MWM_HINTS_DECORATIONS;

    hints.functions=	MWM_FUNC_MOVE		|
			MWM_FUNC_MINIMIZE	|
			MWM_FUNC_CLOSE		;
    hints.decorations=	MWM_DECOR_BORDER	|
			MWM_DECOR_TITLE		|
			MWM_DECOR_MENU		|
			MWM_DECOR_MINIMIZE	;

    if  ( userResizable )
	{
	hints.functions |=	MWM_FUNC_RESIZE;
	hints.decorations |=	MWM_DECOR_RESIZEH;
	}

    ac=0;
    XtSetArg( al[ac], XmNdeleteResponse,	XmDO_NOTHING ); ac++;
    XtSetArg( al[ac], XmNinput,			True ); ac++;
    XtSetArg( al[ac], XmNallowShellResize,	True ); ac++;
    XtSetArg( al[ac], XmNuseAsyncGeometry,	True ); ac++;
    XtSetArg( al[ac], XmNwaitForWm,		False ); ac++; /* LessTif BUG */
    XtSetArg( al[ac], XmNwmTimeout,		0 ); ac++; /* LessTif BUG */
    XtSetArg( al[ac], XmNmwmDecorations,	hints.decorations ); ac++;
    XtSetArg( al[ac], XmNmwmFunctions,		hints.functions ); ac++;

    if  ( iconPixmap )
	{ XtSetArg( al[ac], XmNiconPixmap,	iconPixmap ); ac++; }

#   ifdef USE_X11_R5
    shell= XtAppCreateShell( ea->eaApplicationName, WIDGET_NAME,
			    applicationShellWidgetClass,
			    XtDisplay( ea->eaToplevel.atTopWidget ), al, ac );
#   else
    shell= XtAppCreateShell( ea->eaApplicationName, WIDGET_NAME,
			    topLevelShellWidgetClass,
			    XtDisplay( ea->eaToplevel.atTopWidget ), al, ac );
#   endif

    XtAddEventHandler( shell, StructureNotifyMask, False,
					    appSetSizeAsMinimum, through );

    appSetShellTitle( shell, option, ea->eaApplicationName );

#   if FIX_SIZE
    See above.
    if  ( ! userResizable )
	{
	XtAddEventHandler( shell, StructureNotifyMask, False,
						    appFixSize, through );
	}
#   endif

    appSetCloseCallback( shell, ea, closeCallback, through );

    ac= 0;
    XtSetArg( al[ac], XmNsashWidth,		1 ); ac++;
    XtSetArg( al[ac], XmNsashHeight,		1 ); ac++;
    XtSetArg( al[ac], XmNseparatorOn,		False ); ac++;
    XtSetArg( al[ac], XmNmarginWidth,		PWmargW ); ac++;
    XtSetArg( al[ac], XmNmarginHeight,		PWmargH ); ac++;
    XtSetArg( al[ac], XmNspacing,		PWspacing ); ac++;
    paned= XmCreatePanedWindow( shell, WIDGET_NAME, al, ac );

    XtManageChild( paned );

    *pShell= shell, *pPaned= paned; return;
    }

/************************************************************************/
/*									*/
/*  For debugging purposes: Give widgets a name of the form file(line)	*/
/*  for where they are made.						*/
/*									*/
/************************************************************************/

char * appWidgetName(	char *	file,
			int	line )
    {
    static char **	names;
    static int		nameCount;

    char **		freshNames;
    char *		freshName;

    int			i;

    freshNames= (char **)realloc( names, ( nameCount+ 1 )* sizeof(char *) );
    if  ( ! freshNames )
	{ XDEB(freshNames); return file;	}
    names= freshNames;

    freshName= (char *)malloc( strlen( file )+ 11 );
    if  ( ! freshName )
	{ XDEB(freshName); return file;	}

    sprintf( freshName, "%s(%d)", file, line );

    for ( i= 0; i < nameCount; freshNames++, i++ )
	{
	if  ( ! strcmp( freshNames[0], freshName ) )
	    { free( freshName ); return freshNames[0];	}
	}

    names[nameCount++]= freshName;

    return freshName;
    }

/************************************************************************/
/*									*/
/*  Insert a column in a top level widget (Window)			*/
/*									*/
/************************************************************************/

void appGuiInsertColumnInWindow(	APP_WIDGET *	pColumn,
					APP_WIDGET	parent )
    {
    APP_WIDGET		column;

    Arg			al[20];
    int			ac= 0;

    ac= 0;
    XtSetArg( al[ac],	XmNsashWidth,		1 ); ac++;
    XtSetArg( al[ac],	XmNsashHeight,		1 ); ac++;
    XtSetArg( al[ac],	XmNseparatorOn,		False ); ac++;
    XtSetArg( al[ac],	XmNmarginWidth,		0 ); ac++;
    XtSetArg( al[ac],	XmNmarginHeight,	0 ); ac++;
    XtSetArg( al[ac],	XmNspacing,		0 ); ac++;

    column= XmCreatePanedWindow( parent, WIDGET_NAME, al, ac );

    XtManageChild( column );

    *pColumn= column; return;
    }

/************************************************************************/
/*									*/
/*  Insert a menubar as the first row of a column.			*/
/*									*/
/************************************************************************/

void appGuiInsertMenubarInColumn(	APP_WIDGET *	pMenubar,
					APP_WIDGET	parent )
    {
    Widget		menuBar;

    Arg			al[20];
    int			ac= 0;

    ac= 0;
    XtSetArg( al[ac], XmNskipAdjust,		True ); ac++;
    XtSetArg( al[ac], XmNallowResize,		True ); ac++;
    XtSetArg( al[ac], XmNtraversalOn,		False ); ac++;
    XtSetArg( al[ac], XmNresizeHeight,		True ); ac++;

    menuBar= XmCreateMenuBar( parent, WIDGET_NAME, al, ac );

    XtManageChild( menuBar );

    *pMenubar= menuBar; return;
    }

/************************************************************************/
/*									*/
/*  Insert a separator as a row of a column.				*/
/*									*/
/************************************************************************/

void appGuiInsertSeparatorInColumn(	APP_WIDGET *	pSeparator,
					APP_WIDGET	column )
    {
    Arg			al[20];
    int			ac= 0;

    *pSeparator= XmCreateSeparator( column, WIDGET_NAME, al, ac );

    XtManageChild( *pSeparator );

    return;
    }

/************************************************************************/
/*									*/
/*  Change the label/state of a toggle button.				*/
/*									*/
/************************************************************************/

void appGuiSetToggleLabel(	APP_WIDGET		toggle,
				const char *		text )
    {
    XmString	labelString;

    labelString= XmStringCreateLocalized( (char *)text );
    XtVaSetValues( toggle,
			XmNlabelString,	labelString,
			NULL );

    XmStringFree( labelString );
    }

void appGuiSetToggleState(	APP_WIDGET		toggle,
				int			set )
    {
    XmToggleButtonSetState( toggle, set != 0, False );
    }

int appGuiGetToggleState(	APP_WIDGET		toggle )
    {
    return XmToggleButtonGetState( toggle ) != 0;
    }

int appGuiGetToggleStateFromCallbackMotif( void *	voidcbs )
    {
    XmToggleButtonCallbackStruct *	cbs;

    cbs= (XmToggleButtonCallbackStruct *)voidcbs;

    return cbs->set;
    }

/************************************************************************/
/*									*/
/*  Enable/Disable a widget.						*/
/*									*/
/*  NOTE that for text edit boxes there is a different routine.		*/
/*									*/
/************************************************************************/

void guiEnableWidget(	APP_WIDGET		w,
			int			on_off )
    {
    XtSetSensitive( w, on_off != 0 );
    }

void appGuiSetWidgetVisible(	APP_WIDGET		w,
				int			on_off )
    {
    APP_WIDGET		parent= XtParent( w );

    XtVaSetValues( w,	XmNmappedWhenManaged,	on_off != 0,
			NULL );

    if  ( parent && XmIsRowColumn( parent ) )
	{
	if  ( on_off )
	    { XtManageChild( w );	}
	else{ XtUnmanageChild( w );	}
	}
    }

/************************************************************************/
/*									*/
/*  Show a shell widget. Because of the RIDICULOUS and ABSURD situation	*/
/*  the you cannot rely on a shell window to be visible after we tell	*/
/*  it to become visible, we handle a series of events in a loop until	*/
/*  it becomes visible.							*/
/*									*/
/************************************************************************/

void appShowShellWidget(		EditApplication *	ea,
					APP_WIDGET		shell )
    {
    XtRealizeWidget( shell );

    XtVaSetValues( shell, XmNinitialState, NormalState, NULL );
    XtMapWidget( shell );
    XRaiseWindow( XtDisplay( shell ), XtWindow( shell ) );

    XSync( XtDisplay( shell ), False );

    appGuiMotifWaitForWindow( shell, ea->eaContext );
    }

/************************************************************************/
/*									*/
/*  Hide a shell widget. Because of the RIDICULOUS and ABSURD situation	*/
/*									*/
/************************************************************************/

void appHideShellWidget(		APP_WIDGET		shell )
    {
    if  ( XtIsRealized( shell ) )
	{ XtUnmapWidget( shell );	}
    }

void appDestroyShellWidget(		APP_WIDGET		shell )
    {
    XtDestroyWidget( shell );
    }

void appGuiGetScrollbarValues(		int *		pValue,
					int *		pSliderSize,
					APP_WIDGET	scrollbar )
    {
    XtVaGetValues( scrollbar,
				    XmNvalue,		pValue,
				    XmNsliderSize,	pSliderSize,
				    NULL );
    return;
    }

void appGuiSetScrollbarValues(		APP_WIDGET	scrollbar,
					int		value,
					int		sliderSize )
    {
    const Boolean		notify= False;

    XmScrollBarSetValues( scrollbar, value, sliderSize, 0,0, notify );

    return;
    }

int appGuiGetScrollbarValueFromCallback( APP_WIDGET		scrollbar,
					void *			voidcbs )
    {
    XmScrollBarCallbackStruct *	cbs= (XmScrollBarCallbackStruct *)voidcbs;

    return cbs->value;
    }

/************************************************************************/
/*									*/
/*  Set the title of the window.					*/
/*									*/
/*  See http://standards.freedesktop.org/wm-spec/wm-spec-1.3.html	*/
/*  for a better way: set _NET_WM_NAME to an UTF-8 value.		*/
/*									*/
/************************************************************************/

void appGuiSetShellTitle(		APP_WIDGET		shell,
					const MemoryBuffer *	fullTitle )
    {
    XtVaSetValues( shell,
			XmNtitle,	utilMemoryBufferGetString( fullTitle ),
			NULL );

    return;
    }

void appGuiSetIconTitle(		APP_WIDGET		shell,
					const MemoryBuffer *	iconName )
    {
    XtVaSetValues( shell,
			XmNiconName,	utilMemoryBufferGetString( iconName ),
			NULL );

    return;
    }

/************************************************************************/
/*									*/
/*  Make drawing areas. Typically these are small drawings so it does	*/
/*  no harm to redraw the whole drawing area after a resize.		*/
/*									*/
/************************************************************************/

static void appGuiSendExposeForResize(	APP_WIDGET		w,
					void *			through,
					void *			voidcbs )
    {
    if  ( XtIsRealized( w ) )
	{
	XClearArea( XtDisplay( w ), XtWindow( w ), 0, 0, 0, 0, True );
	}
    }

void appGuiMakeDrawingAreaInColumn( APP_WIDGET *		pDrawing,
				    APP_WIDGET			column,
				    int				wide,
				    int				high,
				    int				heightResizable,
				    APP_EVENT_HANDLER_T		redraw,
				    void *			through )
    {
    APP_WIDGET	drawing;

    Arg		al[20];
    int		ac;

    /*
    XtSetArg( al[ac],	XmNsensitive,		False ); ac++;
    */

    ac= 0;
    if  ( high > 0 )
	{
	XtSetArg( al[ac], XmNheight,		high ); ac++;
	}

    if  ( high <= 0 || heightResizable )
	{ XtSetArg( al[ac], XmNskipAdjust,	False ); ac++;	}
    else{ XtSetArg( al[ac], XmNskipAdjust,	True ); ac++;	}

    if  ( wide > 0 )
	{
	XtSetArg( al[ac], XmNwidth,		wide ); ac++;
	}
    XtSetArg( al[ac],	XmNtraversalOn,		False ); ac++;
    XtSetArg( al[ac],	XmNnavigationType,	XmNONE ); ac++;

    drawing= XmCreateDrawingArea( column, WIDGET_NAME, al, ac );

    if  ( redraw )
	{
	XtAddEventHandler( drawing, ExposureMask, False, redraw, through );
	}

    XtAddCallback( drawing, XmNresizeCallback, appGuiSendExposeForResize,
								(void *)0 );

    XtManageChild( drawing );

    *pDrawing= drawing;
    }

/************************************************************************/
/*									*/
/*  Calculate magnification factors.					*/
/*									*/
/************************************************************************/

void appGetPixelsPerTwip(	EditApplication *	ea )
    {
#   if 0
    Display *		display= XtDisplay( ea->eaToplevel.atTopWidget );
    int			screen= DefaultScreen( display );

    double		horPixPerMM;
    double		verPixPerMM;
    double		xfac;
    double		yfac;

    horPixPerMM= ( (double)DisplayWidth( display, screen ) )/
					DisplayWidthMM( display, screen );
    verPixPerMM= ( (double)DisplayHeight( display, screen ) )/
					DisplayHeightMM( display, screen );

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
/*  Run a loop while the left mouse button is down.			*/
/*									*/
/************************************************************************/

typedef struct DragLoop
    {
    XtAppContext		dlContext;

    XtEventHandler		dlUpHandler;
    XtEventHandler		dlMoveHandler;
    int				dlTimerInterval;
    XtTimerCallbackProc		dlTimerHandler;
    void *			dlThrough;

    int				dlHalted;
    XtIntervalId		dlIntervalId;
    } DragLoop;

static void appDragTick(	void *		vdl,
				XtIntervalId *	xii )
    {
    DragLoop *		dl= (DragLoop *)vdl;

    (*dl->dlTimerHandler)( dl->dlThrough, xii );

    if  ( dl->dlTimerHandler && dl->dlTimerInterval > 0 )
	{
	dl->dlIntervalId= XtAppAddTimeOut( dl->dlContext,
			    dl->dlTimerInterval, appDragTick, (void *)dl );
	}
    return;
    }

static void appDragMouseUp(		APP_WIDGET		w,
					void *			vdl,
					APP_EVENT *		event,
					Boolean *		pRefused )
    {
    DragLoop *		dl= (DragLoop *)vdl;

    if  ( dl->dlUpHandler )
	{ (*dl->dlUpHandler)( w, dl->dlThrough, event, pRefused );	}

    dl->dlHalted= 1;
    }

/************************************************************************/
/*									*/
/*  Handle a movement event, but as an optimization ignore it if the	*/
/*  next even is a similar one: Apparently we are running after the	*/
/*  mouse.								*/
/*									*/
/************************************************************************/

static void appDragMouseMove(		APP_WIDGET		w,
					void *			vdl,
					APP_EVENT *		event,
					Boolean *		pRefused )
    {
    DragLoop *		dl= (DragLoop *)vdl;
    Display *		display= XtDisplay( w );

    if  ( QLength( display ) > 0 )
	{
	XEvent			nextEvent;

	XPeekEvent( display, &nextEvent );

	if  ( nextEvent.type == event->type			&&
	      nextEvent.xmotion.window == event->xmotion.window	)
	    { return;	}
	}

    if  ( dl->dlMoveHandler )
	{ (*dl->dlMoveHandler)( w, dl->dlThrough, event, pRefused );	}
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

    dl.dlContext= ea->eaContext;

    dl.dlUpHandler= upHandler;
    dl.dlMoveHandler= moveHandler;
    dl.dlTimerInterval= timerInterval;
    dl.dlTimerHandler= timerHandler;
    dl.dlThrough= through;

    dl.dlHalted= 0;
    dl.dlIntervalId= (XtIntervalId)0;

    if  ( dl.dlTimerHandler && dl.dlTimerInterval > 0 )
	{
	dl.dlIntervalId= XtAppAddTimeOut( dl.dlContext,
				dl.dlTimerInterval, appDragTick, (void *)&dl );
	}

    XtInsertEventHandler( w, ButtonReleaseMask, False,
				    appDragMouseUp, (void *)&dl, XtListHead );
    XtInsertEventHandler( w, Button1MotionMask, False,
				    appDragMouseMove, (void *)&dl, XtListHead );

    while( ! dl.dlHalted )
	{ XtAppProcessEvent( dl.dlContext, XtIMAll );	}

    if  ( dl.dlIntervalId )
	{ XtRemoveTimeOut( dl.dlIntervalId );	}

    XtRemoveEventHandler( w, ButtonReleaseMask, False,
					    appDragMouseUp, (void *)&dl );
    XtRemoveEventHandler( w, Button1MotionMask, False,
					    appDragMouseMove, (void *)&dl );
    return;
    }

void appGuiSetFocusChangeHandler(	APP_WIDGET		w,
					APP_EVENT_HANDLER_T	handler,
					void *			through )
    {
    XtAddEventHandler( w, FocusChangeMask, False, handler, through );
    }

void appSetDestroyCallback(	APP_WIDGET		shell,
				APP_DESTROY_CALLBACK_T	destroyCallback,
				void *			through )
    {
    if  ( destroyCallback )
	{
	XtAddCallback( shell, XmNdestroyCallback,
						destroyCallback, through );
	}
    }

void appSetCloseCallback(	APP_WIDGET		shell,
				EditApplication *	ea,
				APP_CLOSE_CALLBACK_T	closeCallback,
				void *			through )
    {
    if  ( closeCallback && ea->eaCloseAtom > 0 )
	{
	XtVaSetValues( shell,
			    XmNdeleteResponse,		XmDO_NOTHING,
			    NULL );

	XmAddWMProtocolCallback( shell, ea->eaCloseAtom,
						    closeCallback, through );
	}
    }

void appGuiFocusToWidget(	APP_WIDGET		w )
    {
    XmProcessTraversal( w, XmTRAVERSE_CURRENT );
    }

/************************************************************************/
/*									*/
/*  Cope with stubborn fluxbox.						*/
/*									*/
/************************************************************************/

void appGuiMotifSetFocusToWindow(	APP_WIDGET		w )
    {
    XWindowAttributes	xwa;

    if  ( ! XtIsRealized( w ) )
	{ return;	}

    XGetWindowAttributes( XtDisplay( w ), XtWindow( w ), &xwa );

    if  ( xwa.map_state != IsViewable )
	{ return;	}

    XSetInputFocus( XtDisplay( w ), XtWindow( w ), RevertToNone, CurrentTime );
    }

void appGuiMotifWaitForWindow(	APP_WIDGET		shell,
				XtAppContext		context )
    {
    const int		eventCount= 350;
    int			i;

    for ( i= 0; i < eventCount; i++ )
	{
	if  ( XtIsRealized( shell ) )
	    {
	    XWindowAttributes	xwa;

	    XGetWindowAttributes( XtDisplay( shell ), XtWindow( shell ), &xwa );
	    if  ( xwa.map_state != IsUnmapped )
		{ break;	}
	    }

	XtAppProcessEvent( context, XtIMAll );
	}

    if  ( i >= eventCount )
	{ LDEB(i);	}

    return;
    }

# ifdef XP_PRINTNAME
# ifdef FAKE_XP

XPContext XpGetContext( Display * display )
    { return (XPContext)0; }

char *XpGetOneAttribute(	Display *	display,
				XPContext	context,
				XPAttributes	type,
				char * attribute_name )
    { return "XpGetOneAttribute"; }

Bool XpSetImageResolution(	Display * display,
				XPContext  print_context,
				int  image_res,
				int * prev_res_return )
    { *prev_res_return= 100; return False; }

Bool XpQueryExtension(	Display *display,
			int *event_base_return,
			int *error_base_return )
    { return False;	}

void XpStartPage(	Display *display,
			Window window )
    { return;	}

void XpEndPage(	Display *display )
    { return;	}

void XpEndJob(	Display *display )
    { return;	}

Status XpGetPageDimensions(	Display *display,
				XPContext print_context,
				unsigned short *width,
				unsigned short *height,
				XRectangle *reproducible_area )
    { return 0; }

Screen *XpGetScreenOfContext (	Display *display,
				XPContext print_context )
    { return (Screen *)0; }

Status XpGetDocumentData (	Display *data_display,
				XPContext context,
				XPSaveProc save_proc,
				XPFinishProc finish_proc,
				XPointer client_data )
    { return 0;	}

Status XpGetPdmStartParams (	Display *print_display,
				Window print_window,
				XPContext print_context,
				Display *video_display,
				Window video_window,
				Display **selection_display_return,
				Atom *selection_return,
				Atom *type_return,
				int *format_return,
				unsigned char **data_return,
				int *nelements_return )
    { return 0;	}

void XpSelectInput (	Display *display,
			XPContext context,
			unsigned long event_mask )
    { return;	}

# endif /* FAKE_XP */
# endif /* XP_PRINTNAME */

#   endif
