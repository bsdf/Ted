#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<string.h>

#   include	"tedRuler.h"
#   include	<appFrame.h>
#   include	<appRuler.h>
#   include	<guiWidgetDrawingSurface.h>
#   include	<guiDrawingWidget.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Bottom Ruler..							*/
/*									*/
/************************************************************************/

typedef struct TedBottomRuler
    {
    RulerData		tbrRulerData;

    int			tbrPage;
    int			tbrPages;
    int			tbrSection;
    int			tbrSections;

    const char *	tbrPageFormat;
    char *		tbrPageString;
    int			tbrPageStringLength;
    } TedBottomRuler;

void * tedMakeBottomRuler(	const PostScriptFontList *	psfl,
				int			height,
				int			leftRulerWidth,
				int			rightRulerWidth,
				const char *		pageFormat )
    {
    TedBottomRuler *		tbr;
    RulerData *			rd;

    int				l;
    const char *		s;

    tbr= (TedBottomRuler *)malloc( sizeof( TedBottomRuler ) );
    if  ( ! tbr )
	{ XDEB(tbr); return (void *)0;	}
    rd= &(tbr->tbrRulerData);

    appInitRulerData( rd );

    rd->rdPostScriptFontList= psfl;

    tbr->tbrPageFormat= pageFormat;
    tbr->tbrPage= -1;
    tbr->tbrPages= 0;
    tbr->tbrSection= -1;
    tbr->tbrSections= 0;

    rd->rdMinUsed= leftRulerWidth;

    rd->rdSizeAcross= height;
    rd->rdSizeAlong= 0;

    l= 0; s= tbr->tbrPageFormat;
    while( *s )
	{
	if  ( s[0] == '%' 		&&
	      ( s[1] == 'd'	||
	        s[1] == 'p'	||
	        s[1] == 'P'	||
		s[1] == 's'	||
		s[1] == 'S'	)	)
	    { l += 10; s++; continue;	}

	l++; s++;
	}

    tbr->tbrPageString= malloc( l+ 1 );
    if  ( ! tbr->tbrPageString )
	{ LXDEB(l,tbr->tbrPageString); free( tbr ); return (void *)0;	}
    tbr->tbrPageString[0]= '\0';
    tbr->tbrPageStringLength= 0;

    return (void *)tbr;
    }

APP_EVENT_HANDLER_H( tedBottomRulerConfigure, w, voidtbr, event )
    {
    TedBottomRuler *	tbr= (TedBottomRuler *)voidtbr;
    RulerData *		rd= &(tbr->tbrRulerData);

    appHorizontalRulerGetSizeFromConfigureEvent( rd, w, event );

    return;
    }

void tedFreeBottomRuler(	void *		voidtbr )
    {
    TedBottomRuler *		tbr= (TedBottomRuler *)voidtbr;

    if  ( tbr->tbrPageString )
	{ free( tbr->tbrPageString );	}

    appCleanRulerData( &(tbr->tbrRulerData) );

    free( tbr );

    return;
    }

void tedBottomRulerSetBackground(	void *			voidtbr,
					const RGB8Color *	back )
    {
    TedBottomRuler *	tbr= (TedBottomRuler *)voidtbr;
    RulerData *		rd= &(tbr->tbrRulerData);

    appRulerSetBackground( rd, back );
    }

/************************************************************************/
/*									*/
/*  Set values and format string.					*/
/*									*/
/************************************************************************/

static void tedRulerFormatPageNumber(	TedBottomRuler *	tbr )
    {
    const char *	fr;
    char *		to;

    fr= tbr->tbrPageFormat;
    to= tbr->tbrPageString;
    while( *fr )
	{
	char	scratch[11];

	if  ( fr[0] != '%' )
	    { *(to++)= *(fr++); continue;	}

	switch( fr[1] )
	    {
	    case 'd':  /* backward compatibility */
	    case 'p':
		sprintf( scratch, "%d", tbr->tbrPage+ 1 );
		break;
	    case 'P':
		sprintf( scratch, "%d", tbr->tbrPages );
		break;
	    case 's':
		sprintf( scratch, "%d", tbr->tbrSection+ 1 );
		break;
	    case 'S':
		sprintf( scratch, "%d", tbr->tbrSections );
		break;
	    default:
		*(to++)= *(fr++); continue;
	    }

	strcpy( to, scratch ); to += strlen( to ); fr += 2;
	continue;
	}

    *to= '\0';
    tbr->tbrPageStringLength= to- tbr->tbrPageString;
    return;
    }

/************************************************************************/
/*									*/
/*  Reflect current selection.						*/
/*									*/
/************************************************************************/

void tedSetBottomRulerValues(	void *			voidtbr,
				APP_WIDGET		w,
				int			page,
				int			pages,
				int			section,
				int			sections )
    {
    TedBottomRuler *	tbr= (TedBottomRuler *)voidtbr;
    int			changed= 0;

    if  ( tbr->tbrPage != page )
	{ tbr->tbrPage=   page; changed= 1;	}
    if  ( tbr->tbrPages != pages )
	{ tbr->tbrPages=   pages; changed= 1;	}
    if  ( tbr->tbrSection != section )
	{ tbr->tbrSection=   section; changed= 1;	}
    if  ( tbr->tbrSections != sections )
	{ tbr->tbrSections=   sections; changed= 1;	}

    if  ( changed )
	{
	tedRulerFormatPageNumber( tbr );

	guiExposeDrawingWidget( w );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Draw a bottom ruler.						*/
/*									*/
/************************************************************************/

static void tedDrawBottomRuler(	APP_WIDGET		w,
				TedBottomRuler *	tbr,
				DocumentRectangle *	drClip )
    {
    RulerData *		rd= &(tbr->tbrRulerData);
    DrawingSurface	ds= rd->rdDrawingSurface;
    int			x;
    int			y;

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

    x= rd->rdMinUsed;
    y= rd->rdSizeAcross- rd->rdSizeAcross/ 3;

    drawSetForegroundColor( ds, &(rd->rdForegroundColor) );
    drawString( ds, x, y, rd->rdScreenFont,
			    tbr->tbrPageString, tbr->tbrPageStringLength );

    drawNoClipping( ds );
    }

APP_EVENT_HANDLER_H( tedRedrawBottomRuler, w, voidtbr, event )
    {
    TedBottomRuler *	tbr= (TedBottomRuler *)voidtbr;
    RulerData *		rd= &(tbr->tbrRulerData);

    int			ox= 0;
    int			oy= 0;

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
    geoShiftRectangle( &drClip, ox, oy );
    tedDrawBottomRuler( w, tbr, &drClip );
    }
