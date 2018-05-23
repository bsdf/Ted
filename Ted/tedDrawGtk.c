/************************************************************************/
/*									*/
/*  Ted, Screen drawing and forcing drawing through			*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<string.h>

#   include	"tedApp.h"
#   include	"tedDraw.h"
#   include	"tedSelect.h"
#   include	"tedLayout.h"
#   include	"tedDocument.h"

#   include	<appDebugon.h>

#   ifdef USE_GTK

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
    APP_WIDGET			w= ed->edDocumentWidget.dwWidget;
    GdkWindow *			win= w->window;

    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    GdkWindowAttr		xswa;
    int				i;
    gint			attributesMask;

    DocumentRectangle		drObj;
    Point2DI			xp[RESIZE_COUNT];

    static const int		font_cursors[RESIZE_COUNT]=
				    {
				    GDK_BOTTOM_LEFT_CORNER,
				    GDK_BOTTOM_SIDE,
				    GDK_BOTTOM_RIGHT_CORNER,

				    GDK_LEFT_SIDE,
				    GDK_RIGHT_SIDE,

				    GDK_TOP_LEFT_CORNER,
				    GDK_TOP_SIDE,
				    GDK_TOP_RIGHT_CORNER,
				    };

    static GdkCursor *		moveCursor;
    static GdkCursor *		cursors[RESIZE_COUNT];

    if  ( ! moveCursor )
	{
	moveCursor= gdk_cursor_new( GDK_FLEUR );

	for ( i= 0; i < RESIZE_COUNT; i++ )
	    { cursors[i]= gdk_cursor_new( font_cursors[i] ); }
	}

    memset( &xswa, 0, sizeof(xswa) ); /* make valgrind shut up */
    xswa.wclass= GDK_INPUT_ONLY;
    xswa.window_type= GDK_WINDOW_CHILD;
    attributesMask= GDK_WA_X | GDK_WA_Y;

    tedGetObjectRectangle( &drObj, xp, io, pg, lc, ed );
    for ( i= 0; i < RESIZE_COUNT; i++ )
	{
	xp[i].x -= drObj.drX0;
	xp[i].y -= drObj.drY0;
	}

    if  ( ! td->tdObjectWindow )
	{
	xswa.cursor= moveCursor;
	xswa.x= drObj.drX0- lc->lcOx;
	xswa.y= drObj.drY0- lc->lcOy;
	xswa.width= drObj.drX1- drObj.drX0+ 1;
	xswa.height= drObj.drY1- drObj.drY0+ 1;
	td->tdObjectWindow= gdk_window_new( win, &xswa, attributesMask );
	/* The document will get the events */
	gdk_window_set_events(td->tdObjectWindow,(GdkEventMask)0);

	for ( i= 0; i < RESIZE_COUNT; i++ )
	    {
	    xswa.cursor= cursors[i];
	    xswa.x= xp[i].x;
	    xswa.y= xp[i].y;
	    xswa.width= RESIZE_BLOCK;
	    xswa.height= RESIZE_BLOCK;
	    td->tdObjectResizeWindows[i]= gdk_window_new( td->tdObjectWindow,
					 &xswa, attributesMask|GDK_WA_CURSOR );

	    /* The document will get the events */
	    gdk_window_set_events(td->tdObjectResizeWindows[i],(GdkEventMask)0);
	    }

	for ( i= 0; i < RESIZE_COUNT; i++ )
	    { gdk_window_show( td->tdObjectResizeWindows[i] );	}
	}
    else{
	gdk_window_move_resize( td->tdObjectWindow,
				drObj.drX0- lc->lcOx, drObj.drY0- lc->lcOy,
				drObj.drX1- drObj.drX0+ 1,
				drObj.drY1- drObj.drY0+ 1 );

	for ( i= 0; i < RESIZE_COUNT; i++ )
	    {
	    gdk_window_move( td->tdObjectResizeWindows[i],
							xp[i].x, xp[i].y );
	    }
	}

    gdk_window_show( td->tdObjectWindow );
    }

void tedHideObjectWindows(	EditDocument *	ed )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    if  ( ! td->tdObjectWindow )
	{ XDEB(td->tdObjectWindow); return;	}

    gdk_window_hide( td->tdObjectWindow );
    }

/************************************************************************/
/*									*/
/*  Blinking cursor: GTK specific code.					*/
/*									*/
/*  Thanks to Andrea Frome who contributed the original code.		*/
/*									*/
/************************************************************************/

#   define	TED_BLINK_VISIBLE	(800L)
#   define	TED_BLINK_INVISIBLE	(400L)

static int tedHideIBar(	void *		voided );

static int tedShowIBar(	void *		voided )
    {
    EditDocument *		ed= (EditDocument *)voided;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;
    DocumentRectangle		drPixels;

    LayoutContext		lc;

    layoutInitContext( &lc );
    tedSetScreenLayoutContext( &lc, ed );

    td->tdShowIBarId= (guint)0;

    if  ( tedGetSelection( &ds, &sg, &sd,
			    (DocumentTree **)0, (struct BufferItem **)0, ed ) )
	{ LDEB(1); return 0;	}

    tedGetIBarRect( &drPixels, &(sg.sgHead), &lc );
    tedDrawIBar( &drPixels, &lc );

    td->tdHideIBarId= gtk_timeout_add( TED_BLINK_VISIBLE,
						tedHideIBar, (void *)ed );
    return 0;
    }

static int tedHideIBar(	void *		voided )
    {
    EditDocument *		ed= (EditDocument *)voided;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    td->tdHideIBarId= (guint)0;

    tedUndrawIBar( ed );

    td->tdShowIBarId= gtk_timeout_add( TED_BLINK_INVISIBLE,
						tedShowIBar, (void *)ed );
    return 0;
    }

void tedStartCursorBlink(	EditDocument *	ed )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    tedStopCursorBlink( ed );

    td->tdHideIBarId= gtk_timeout_add( TED_BLINK_VISIBLE,
						tedHideIBar, (void *)ed );
    }

void tedStopCursorBlink(	EditDocument *	ed )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    if  ( td->tdHideIBarId )
	{ gtk_timeout_remove( td->tdHideIBarId ); }

    if  ( td->tdShowIBarId )
	{
	DocumentSelection		ds;
	SelectionGeometry		sg;
	SelectionDescription		sd;
	DocumentRectangle		drPixels;

	LayoutContext			lc;

	layoutInitContext( &lc );
	tedSetScreenLayoutContext( &lc, ed );

	gtk_timeout_remove( td->tdShowIBarId );

	if  ( tedGetSelection( &ds, &sg, &sd,
			   (DocumentTree **)0, (struct BufferItem **)0, ed ) )
	    { LDEB(1); return;	}

	tedGetIBarRect( &drPixels, &(sg.sgHead), &lc );
	tedDrawIBar( &drPixels, &lc );
	}

    td->tdHideIBarId= (guint)0;
    td->tdShowIBarId= (guint)0;

    return;
    }

void tedCleanCursorBlink(	TedDocument *	td )
    {
    if  ( td->tdHideIBarId )
	{ gtk_timeout_remove( td->tdHideIBarId ); }
    if  ( td->tdShowIBarId )
	{ gtk_timeout_remove( td->tdShowIBarId ); }

    td->tdHideIBarId= (guint)0;
    td->tdShowIBarId= (guint)0;
    }

int tedMakeDocumentCursor(	EditApplication *	ea )
    {
    if  ( ! ea->eaDocumentCursor )
	{
	ea->eaDocumentCursor= gdk_cursor_new( GDK_XTERM );
	if  ( ! ea->eaDocumentCursor )
	    { XDEB(ea->eaDocumentCursor);	}
	}

    return 0;
    }

#   endif
