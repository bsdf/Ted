/************************************************************************/
/*									*/
/*  Drawing surface implementation implementation.			*/
/*									*/
/************************************************************************/

#   include "appFrameConfig.h"

#   include <drawDrawingSurface.h>
#   include <appDebugon.h>
#   include <uniUtf8.h>

int drawSetForegroundColorWhite(	DrawingSurface	ds )
    {
    RGB8Color	rgb8;

    utilRGB8SolidWhite( &rgb8 );

    return drawSetForegroundColor( ds, &rgb8 );
    }

int drawSetForegroundColorBlack(	DrawingSurface	ds )
    {
    RGB8Color	rgb8;

    utilRGB8SolidBlack( &rgb8 );

    return drawSetForegroundColor( ds, &rgb8 );
    }

/************************************************************************/
/*									*/
/*  Draw a single unicode symbol.					*/
/*									*/
/************************************************************************/

int drawGetSymbolExtents(	DocumentRectangle *	drText,
				DrawingSurface		ds,
				int			x0,
				int			y0,
				int			screenFont,
				int			symbol )
    {
    int				step;
    char			scratch[10];

    step= uniPutUtf8( scratch, symbol );
    if  ( step < 1 )
	{ return -1;	}
    scratch[step]= '\0';

    return drawGetTextExtents( drText, ds, x0, y0, screenFont, scratch, step );
    }


void drawSymbol(		DrawingSurface		ds,
				int			x,
				int			y,
				int			screenFont,
				int			symbol )
    {
    int				step;
    char			scratch[10];

    step= uniPutUtf8( scratch, symbol );
    if  ( step < 1 )
	{ return;	}
    scratch[step]= '\0';

    drawString( ds, x, y, screenFont, scratch, step );

    return;
    }

