/************************************************************************/
/*									*/
/*  Basic screen font management.					*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>
#   include	<stdio.h>

#   include	<geo2DInteger.h>
#   include	"drawScreenFontUtil.h"
#   include	"drawScreenFontAdmin.h"
#   include	"drawDrawingSurfaceImpl.h"

#   include	<appDebugon.h>

void drawFontUnderlineGeometry(		int *			pThick,
					int *			pY,
					const DrawingSurface	ds,
					int			screenFont,
					int			baselinePixels )
    {
    const NumberedPropertiesList *	npl= &(ds->dsScreenFontAdmin);
    const DrawScreenFont *		dsf;

    dsf= drawGetScreenFontByNumber( npl, screenFont );
    if  ( ! dsf )
	{  LXDEB(screenFont,dsf); return;	}

    *pThick= dsf->apfUnderlineThicknessPixels;
    *pY= baselinePixels+ dsf->apfUnderlinePositionPixels;
    return;
    }

void drawFontStrikethroughGeometry(	int *			pThick,
					int *			pY,
					const DrawingSurface	ds,
					int			screenFont,
					int			baselinePixels )
    {
    const NumberedPropertiesList *	npl= &(ds->dsScreenFontAdmin);
    const DrawScreenFont *		dsf;

    dsf= drawGetScreenFontByNumber( npl, screenFont );
    if  ( ! dsf )
	{  LXDEB(screenFont,dsf); return;	}

    *pThick= dsf->apfStrikethroughThicknessPixels;
    *pY= baselinePixels+ dsf->apfStrikethroughPositionPixels;
    return;
    }

void drawFontGetSuperBaseline(	int *			pSuperBaseline,
				int			baselinePixels,
				const DrawingSurface	ds,
				int			screenFont )
    {
    const NumberedPropertiesList *	npl= &(ds->dsScreenFontAdmin);
    const DrawScreenFont *		dsf;

    dsf= drawGetScreenFontByNumber( npl, screenFont );
    if  ( ! dsf )
	{  LXDEB(screenFont,dsf); return;	}

    *pSuperBaseline= baselinePixels+ dsf->apfSuperBaseline;
    return;
    }

void drawFontGetSubBaseline(	int *			pSubBaseline,
				int			baselinePixels,
				const DrawingSurface	ds,
				int			screenFont )
    {
    const NumberedPropertiesList *	npl= &(ds->dsScreenFontAdmin);
    const DrawScreenFont *		dsf;

    dsf= drawGetScreenFontByNumber( npl, screenFont );
    if  ( ! dsf )
	{  LXDEB(screenFont,dsf); return;	}

    *pSubBaseline= baselinePixels+ dsf->apfSubBaseline;
    return;
    }

