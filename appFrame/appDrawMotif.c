/************************************************************************/
/*									*/
/*  Drawing primitives for X11/Motif.					*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>

#   include	<geo2DInteger.h>
#   include	"drawImpl.h"
#   include	"drawDrawingSurfaceImpl.h"
#   include	"drawUtilMotif.h"

#   include	<appDebugon.h>

#   ifdef USE_MOTIF

/************************************************************************/
/*									*/
/*  Collect exposures for optimised drawing.				*/
/*									*/
/************************************************************************/

#   define	LOG_EXPOSES	0

/************************************************************************/

#   ifdef USE_XFT

XftDraw * appMotifXftDrawCreate(	Display *		display,
					int			screen,
					Drawable		drawable,
					AppXftColorList *	xftColorList )
    {
    Colormap		cmap= DefaultColormap( display, screen );
    Visual *		vis= DefaultVisual( display, screen );
    XftDraw *		xftDraw;

    xftColorList->axclDisplay= display;
    xftColorList->axclVisual= vis;
    xftColorList->axclColormap= cmap;

    xftDraw= XftDrawCreate( xftColorList->axclDisplay, drawable,
						xftColorList->axclVisual,
						xftColorList->axclColormap );

    return xftDraw;
    }

#   endif

#   endif
