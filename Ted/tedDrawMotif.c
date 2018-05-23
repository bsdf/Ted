/************************************************************************/
/*									*/
/*  Ted, Screen drawing and forcing drawing through			*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>

#   include	"tedApp.h"
#   include	"tedDocument.h"
#   include	"tedLayout.h"
#   include	"tedDraw.h"
#   include	"tedRuler.h"
#   include	"tedSelect.h"

#   include	<docTreeNode.h>

#   include	<appDebugon.h>

#   ifdef USE_MOTIF

#   include	<X11/cursorfont.h>

/************************************************************************/
/*									*/
/*  Create the windows that are used to reshape selected objects.	*/
/*									*/
/************************************************************************/

void tedSetObjectWindows(	EditDocument *			ed,
				const PositionGeometry *	pg,
				const InsertedObject *		io,
				const LayoutContext *		lc )
    {
    Display *			display= XtDisplay( ed->edDocumentWidget.dwWidget );
    Window			win= XtWindow( ed->edDocumentWidget.dwWidget );

    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    XSetWindowAttributes	xswa;
    int				i;

    DocumentRectangle		drObj;
    Point2DI			xp[RESIZE_COUNT];

    static const int		font_cursors[RESIZE_COUNT]=
				    {
				    XC_bottom_left_corner,
				    XC_bottom_side,
				    XC_bottom_right_corner,

				    XC_left_side,
				    XC_right_side,

				    XC_top_left_corner,
				    XC_top_side,
				    XC_top_right_corner,
				    };

    static Cursor		moveCursor;
    static Cursor		cursors[RESIZE_COUNT];

    if  ( ! XtIsRealized( ed->edDocumentWidget.dwWidget ) )
	{ XtRealizeWidget( ed->edDocumentWidget.dwWidget );	}

    if  ( ! moveCursor )
	{
	moveCursor= XCreateFontCursor( display, XC_fleur );

	for ( i= 0; i < RESIZE_COUNT; i++ )
	    { cursors[i]= XCreateFontCursor( display, font_cursors[i] ); }
	}

    tedGetObjectRectangle( &drObj, xp, io, pg, lc, ed );
    for ( i= 0; i < RESIZE_COUNT; i++ )
	{
	xp[i].x -= drObj.drX0;
	xp[i].y -= drObj.drY0;
	}

    if  ( ! td->tdObjectWindow )
	{
	xswa.cursor= moveCursor;
	td->tdObjectWindow= XCreateWindow( display, win,
				drObj.drX0- lc->lcOx, drObj.drY0- lc->lcOy,
				drObj.drX1- drObj.drX0+ 1,
				drObj.drY1- drObj.drY0+ 1,
				0, CopyFromParent, InputOnly, CopyFromParent,
				0L, &xswa );

	for ( i= 0; i < RESIZE_COUNT; i++ )
	    {
	    xswa.cursor= cursors[i];

	    td->tdObjectResizeWindows[i]=
			XCreateWindow( display, td->tdObjectWindow,
			    xp[i].x, xp[i].y,
			    RESIZE_BLOCK, RESIZE_BLOCK,
			    0, CopyFromParent, CopyFromParent, CopyFromParent,
			    CWCursor, &xswa );
	    }

	for ( i= 0; i < RESIZE_COUNT; i++ )
	    { XMapRaised( display, td->tdObjectResizeWindows[i] ); }
	}
    else{
	XMoveResizeWindow( display, td->tdObjectWindow,
				drObj.drX0- lc->lcOx, drObj.drY0- lc->lcOy,
				drObj.drX1- drObj.drX0+ 1,
				drObj.drY1- drObj.drY0+ 1 );

	for ( i= 0; i < RESIZE_COUNT; i++ )
	    {
	    XMoveWindow( display, td->tdObjectResizeWindows[i],
							xp[i].x, xp[i].y );
	    }
	}

    XMapRaised( display, td->tdObjectWindow );
    }

void tedHideObjectWindows(	EditDocument *	ed )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    if  ( ! td->tdObjectWindow )
	{ XDEB(td->tdObjectWindow); return;	}

    XUnmapWindow( XtDisplay( ed->edDocumentWidget.dwWidget ),
						    td->tdObjectWindow );
    }

/************************************************************************/
/*									*/
/*  Blinking cursor: MOTIF specific code.				*/
/*									*/
/************************************************************************/

#   define	TED_BLINK_VISIBLE	(800L)
#   define	TED_BLINK_INVISIBLE	(400L)

static void tedHideIBar(	void *		voided,
				XtIntervalId *	intervalId );

static void tedShowIBar(	void *		voided,
				XtIntervalId *	intervalId )
    {
    EditDocument *		ed= (EditDocument *)voided;
    EditApplication *		ea= ed->edApplication;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;
    DocumentRectangle		drPixels;

    LayoutContext		lc;

    layoutInitContext( &lc );
    tedSetScreenLayoutContext( &lc, ed );

    td->tdShowIBarId= (XtIntervalId)0;

    if  ( tedGetSelection( &ds, &sg, &sd,
			    (DocumentTree **)0, (BufferItem **)0, ed ) )
	{ LDEB(1); return;	}

    tedGetIBarRect( &drPixels, &(sg.sgHead), &lc );
    tedDrawIBar( &drPixels, &lc );

    td->tdHideIBarId= XtAppAddTimeOut( ea->eaContext, TED_BLINK_VISIBLE,
						tedHideIBar, (void *)ed );
    }

static void tedHideIBar(	void *		voided,
				XtIntervalId *	intervalId )
    {
    EditDocument *		ed= (EditDocument *)voided;
    EditApplication *		ea= ed->edApplication;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    td->tdHideIBarId= (XtIntervalId)0;

    tedUndrawIBar( ed );

    td->tdShowIBarId= XtAppAddTimeOut( ea->eaContext, TED_BLINK_INVISIBLE,
						tedShowIBar, (void *)ed );
    }

void tedStartCursorBlink(	EditDocument *	ed )
    {
    EditApplication *		ea= ed->edApplication;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    tedStopCursorBlink( ed );

    td->tdHideIBarId= XtAppAddTimeOut( ea->eaContext, TED_BLINK_VISIBLE,
						tedHideIBar, (void *)ed );
    }

void tedStopCursorBlink(	EditDocument *	ed )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    if  ( td->tdHideIBarId )
	{ XtRemoveTimeOut( td->tdHideIBarId ); }

    if  ( td->tdShowIBarId )
	{
	DocumentSelection	ds;
	SelectionGeometry	sg;
	SelectionDescription	sd;
	DocumentRectangle	drPixels;

	LayoutContext			lc;

	layoutInitContext( &lc );
	tedSetScreenLayoutContext( &lc, ed );
	XtRemoveTimeOut( td->tdShowIBarId );

	if  ( ! tedGetSelection( &ds, &sg, &sd,
				(DocumentTree **)0, (BufferItem **)0, ed ) )
	    {
	    tedGetIBarRect( &drPixels, &(sg.sgHead), &lc );
	    tedDrawIBar( &drPixels, &lc );
	    }
	}

    td->tdHideIBarId= (XtIntervalId)0;
    td->tdShowIBarId= (XtIntervalId)0;

    return;
    }

void tedCleanCursorBlink(	TedDocument *	td )
    {
    if  ( td->tdHideIBarId )
	{ XtRemoveTimeOut( td->tdHideIBarId ); }
    if  ( td->tdShowIBarId )
	{ XtRemoveTimeOut( td->tdShowIBarId ); }

    td->tdHideIBarId= (XtIntervalId)0;
    td->tdShowIBarId= (XtIntervalId)0;
    }

int tedMakeDocumentCursor(	EditApplication *	ea )
    {
    if  ( ! ea->eaDocumentCursor )
	{
	ea->eaDocumentCursor= XCreateFontCursor(
			    XtDisplay( ea->eaToplevel.atTopWidget), XC_xterm );
	if  ( ! ea->eaDocumentCursor )
	    { LDEB(ea->eaDocumentCursor); return -1;	}
	}

    return 0;
    }

#   endif

