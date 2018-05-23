/************************************************************************/
/*									*/
/*  Drawing primitives for X11/Motif.					*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<limits.h>

#   include	<geo2DInteger.h>
#   include	"drawImpl.h"

#   include	<appDebugon.h>

#   ifdef USE_XFT

#   include	"drawImplXft.h"

int drawStringXft(		const DrawScreenFont *	dsf,
				AppXftColorList *	axcl,
				int			x,
				int			y,
				const char *		s,
				int			len )
    {
    AppXftColor *	axc= &(axcl->axclCurrentColor);

    if  ( ! dsf->dsfXftDrawable || ! dsf->dsfXftFont )
	{ return -1;	}

    if  ( ! axc->axcXftColorAllocated )
	{
	appXftAllocateColor( axc, axcl );

	if  ( ! axc->axcXftColorAllocated )
	    { return -1;	}
	}

    XftDrawStringUtf8( dsf->dsfXftDrawable, &(axc->axcXftColor),
				dsf->dsfXftFont, x, y, (FcChar8 *)s, len );

    return 0;
    }

int drawFillRectangleXft(	XftDraw *			draw,
				AppXftColorList *		axcl,
				const DocumentRectangle *	dr )
    {
    AppXftColor *	axc= &(axcl->axclCurrentColor);

    if  ( ! draw )
	{ return -1;	}

    if  ( ! axc->axcXftColorAllocated )
	{
	appXftAllocateColor( axc, axcl );

	if  ( ! axc->axcXftColorAllocated )
	    { return -1;	}
	}

    XftDrawRect( draw, &(axc->axcXftColor),
			dr->drX0, dr->drY0,
			dr->drX1- dr->drX0+ 1, dr->drY1- dr->drY0+ 1 );
    return 0;
    }

int drawTextExtentsXft(		DocumentRectangle *	drText,
				int			x,
				int			y,
				const DrawScreenFont *	hDsf,
				const DrawScreenFont *	vDsf,
				const char *		s,
				int			len )
    {
    XGlyphInfo		glyphInfo;

    if  ( ! hDsf->dsfXftFont || ! vDsf->dsfXftFont )
	{ return -1;	}

    XftTextExtentsUtf8( hDsf->dsfDisplay,
			hDsf->dsfXftFont, (FcChar8 *)s, len, &glyphInfo );

    drText->drX0= x;
    drText->drX1= drText->drX0+ glyphInfo.xOff- 1;
    drText->drY0= y- vDsf->dsfXftFont->ascent;
    drText->drY1= y+ vDsf->dsfXftFont->descent;

    return 0;
    }

#   endif

