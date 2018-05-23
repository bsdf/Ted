#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>

#   include	"tedRuler.h"
#   include	<appFrame.h>
#   include	<appRuler.h>
#   include	<guiWidgetDrawingSurface.h>
#   include	<guiDrawingWidget.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Vertical rulers.							*/
/*									*/
/************************************************************************/

static APP_EVENT_HANDLER_H( tedVerticalRulerConfigure, w, voidrd, event )
    {
    RulerData *		rd= (RulerData *)voidrd;

    appHorizontalRulerGetSizeFromConfigureEvent( rd, w, event );

    return;
    }

void tedFreeVerticalRuler(	void *		voidrd )
    {
    RulerData *		rd= (RulerData *)voidrd;

    appCleanRulerData( rd );
    free( rd );

    return;
    }

/************************************************************************/
/*									*/
/*  Draw a bottom ruler.						*/
/*									*/
/************************************************************************/

static void tedDrawVerticalRuler(	APP_WIDGET		w,
					RulerData *		rd,
					DocumentRectangle *	drClip )
    {
    DrawingSurface	ds= rd->rdDrawingSurface;

    if  ( rd->rdSizeAlong == 0 )
	{
	int		wide;
	int		high;

	guiDrawGetSizeOfWidget( &wide, &high, w );

	rd->rdSizeAlong= wide;
	rd->rdSizeAcross= high;
	}

    drawSetClipRect( ds, drClip );

    drawSetForegroundColor( ds, &(rd->rdBackgroundColor) );
    drawFillRectangle( ds, drClip );

    drawNoClipping( ds );
    }

static APP_EVENT_HANDLER_H( tedRedrawVerticalRuler, w, voidrd, event )
    {
    RulerData *		rd= (RulerData *)voidrd;

    DocumentRectangle	drClip;

    if  ( ! rd->rdDrawingSurface )
	{
	const int	vertical= 0;
	const int	fontSizeHintPixels= 2* rd->rdSizeAcross/ 3- 2;
	const double	magnification= 1.0;

	appRulerMakeDrawingSurface( rd, vertical,
				    fontSizeHintPixels, magnification, w );
	}

    guiCollectExposures( &drClip, w, event );
    tedDrawVerticalRuler( w, rd, &drClip );
    }

static void * tedDocMakeVerticalRuler(	EditDocument *		ed,
					APP_WIDGET		w )
    {
    EditApplication *		ea= ed->edApplication;

    RulerData *			rd;

    rd= (RulerData *)malloc( sizeof( RulerData ) );
    if  ( ! rd )
	{ XDEB(rd); return (void *)0;	}

    appInitRulerData( rd );

    rd->rdPostScriptFontList= &(ea->eaPostScriptFontList);

    guiDrawSetConfigureHandler( w, tedVerticalRulerConfigure, (void *)rd );
    guiDrawSetRedrawHandler( w, tedRedrawVerticalRuler, (void *)rd );

    return (void *)rd;
    }

int tedDocSetLeftRuler(	EditDocument *		ed )
    {
    ed->edLeftRuler= tedDocMakeVerticalRuler( ed, ed->edLeftRulerWidget );

    return 0;
    }

int tedDocSetRightRuler(	EditDocument *		ed )
    {
    ed->edRightRuler= tedDocMakeVerticalRuler( ed, ed->edRightRulerWidget );

    return 0;
    }

void tedVerticalRulerSetBackground(	void *			voidrd,
					const RGB8Color *	back )
    {
    RulerData *		rd= (RulerData *)voidrd;

    appRulerSetBackground( rd, back );
    }

