/************************************************************************/
/*									*/
/*  Drawing surface implementation implementation.			*/
/*									*/
/************************************************************************/

#   include "appFrameConfig.h"

#   include "guiWidgetDrawingSurface.h"
#   include <drawDrawingSurface.h>
#   include "drawDrawingSurfaceImpl.h"
#   include "drawScreenFontImpl.h"
#   include "drawScreenFontAdmin.h"
#   include <appDebugon.h>

int drawGetUnderlineGeometry(		int *			pThick,
					int *			pY,
					const DrawingSurface	ds,
					int			screenFont,
					int			baselinePixels )
    {
    const NumberedPropertiesList *	npl= &(ds->dsScreenFontAdmin);
    const DrawScreenFont *		dsf;

    dsf= drawGetScreenFontByNumber( npl, screenFont );
    if  ( ! dsf )
	{  LXDEB(screenFont,dsf); return -1;	}

    *pThick= dsf->apfUnderlineThicknessPixels;
    *pY= baselinePixels+ dsf->apfUnderlinePositionPixels;

    return 0;
    }

int drawGetStrikethroughGeometry(	int *			pThick,
					int *			pY,
					const DrawingSurface	ds,
					int			screenFont,
					int			baselinePixels )
    {
    const NumberedPropertiesList *	npl= &(ds->dsScreenFontAdmin);
    const DrawScreenFont *		dsf;

    dsf= drawGetScreenFontByNumber( npl, screenFont );
    if  ( ! dsf )
	{  LXDEB(screenFont,dsf); return -1;	}

    *pThick= dsf->apfStrikethroughThicknessPixels;
    *pY= baselinePixels+ dsf->apfStrikethroughPositionPixels;

    return 0;
    }

int drawGetSuperBaseline(	int *			pSuperBaseline,
				const DrawingSurface	ds,
				int			screenFont,
				int			baselinePixels )
    {
    const NumberedPropertiesList *	npl= &(ds->dsScreenFontAdmin);
    const DrawScreenFont *		dsf;

    dsf= drawGetScreenFontByNumber( npl, screenFont );
    if  ( ! dsf )
	{  LXDEB(screenFont,dsf); return -1;	}

    *pSuperBaseline= baselinePixels+ dsf->apfSuperBaseline;

    return 0;
    }

int drawGetSubBaseline(		int *			pSubBaseline,
				const DrawingSurface	ds,
				int			screenFont,
				int			baselinePixels )
    {
    const NumberedPropertiesList *	npl= &(ds->dsScreenFontAdmin);
    const DrawScreenFont *		dsf;

    dsf= drawGetScreenFontByNumber( npl, screenFont );
    if  ( ! dsf )
	{  LXDEB(screenFont,dsf); return -1;	}

    *pSubBaseline= baselinePixels+ dsf->apfSubBaseline;
    return 0;
    }

int drawGetTextExtents(	DocumentRectangle *		drText,
			const DrawingSurface		ds,
			int				x0,
			int				y0,
			int				screenFont,
			const char *			s,
			int				len )
    {
    const NumberedPropertiesList *	npl= &(ds->dsScreenFontAdmin);

    return drawTextExtentsImpl( drText, x0, y0, npl, screenFont, s, len );
    }

