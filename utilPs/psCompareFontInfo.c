#   include	"utilPsConfig.h"

#   include	<string.h>
#   include	<ctype.h>

#   include	<math.h>

#   include	"psCompareFontInfo.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Compare two font info's						*/
/*									*/
/************************************************************************/

static int psCompareFontInfos(	const AfmFontInfo *	afi1,
				const AfmFontInfo *	afi2,
				int			includeFamily )
    {
    int		cmp;
    int		slant1;
    int		slant2;

    if  ( includeFamily )
	{
	cmp= strcmp( afi1->afiFamilyName, afi2->afiFamilyName );

	if  ( cmp > 0 )
	    { return  1;	}
	if  ( cmp < 0 )
	    { return -1;	}
	}

    if  ( afi1->afiWidthInt > afi1->afiWidthInt )
	{ return  1;	}
    if  ( afi1->afiWidthInt < afi1->afiWidthInt )
	{ return -1;	}

    if  ( afi1->afiWeightInt > afi1->afiWeightInt )
	{ return  1;	}
    if  ( afi1->afiWeightInt < afi1->afiWeightInt )
	{ return -1;	}

    slant1= FONT_IS_SLANTED( afi1 );
    slant2= FONT_IS_SLANTED( afi2 );

    if  ( slant1 > slant2 )
	{ return  1;	}
    if  ( slant1 < slant2 )
	{ return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Compare routine for sorting the faces of a font.			*/
/*									*/
/*  Serves two purposes:						*/
/*									*/
/*  1)  Present them in a predictable order in the font chooser.	*/
/*  2)  Makes it easier to present width variants as different		*/
/*	families.							*/
/*									*/
/************************************************************************/

int psFontCompareInfosIncludingFamily(	const void *	voidpafi1,
					const void *	voidpafi2 )
    {
    AfmFontInfo * const *	pafi1= (AfmFontInfo * const *)voidpafi1;
    AfmFontInfo * const *	pafi2= (AfmFontInfo * const *)voidpafi2;
    const AfmFontInfo *		afi1= *pafi1;
    const AfmFontInfo *		afi2= *pafi2;

    const int			includeFamily= 1;

    return psCompareFontInfos( afi1, afi2, includeFamily );
    }

int psFontCompareInfosExcludingFamily(	const void *	voidpafi1,
					const void *	voidpafi2 )
    {
    AfmFontInfo * const *	pafi1= (AfmFontInfo * const *)voidpafi1;
    AfmFontInfo * const *	pafi2= (AfmFontInfo * const *)voidpafi2;
    const AfmFontInfo *		afi1= *pafi1;
    const AfmFontInfo *		afi2= *pafi2;

    const int			includeFamily= 0;

    return psCompareFontInfos( afi1, afi2, includeFamily );
    }

/************************************************************************/
/*									*/
/*  1)  Give an arbitrary measure of the distance between two faces,	*/
/*	typically of the same font.					*/
/*  2)  Compare two faces for the ordering in a fonts tool.		*/
/*									*/
/************************************************************************/

void psFontFaceDistance(	int *			pDifCount,
				double *		pDistance,
				const AfmFontInfo *	afi1,
				int			isSlanted,
				int			weight )
    {
    double	d;

    double	distance= 0.0;
    int		difCount= 0;
    int		slanted1= FONT_IS_SLANTED( afi1 );

    if  ( afi1->afiWeightInt != weight )
	{
	d= weight- afi1->afiWeightInt;
	d /= ( FONTweight_MAX- FONTweight_MIN );

	distance += d* d;
	difCount++;
	}

    if  ( slanted1 != isSlanted )
	{
	d= isSlanted- slanted1;
	/* d /= 1.0; */

	distance += d* d;
	difCount++;
	}

    distance /= 2.0;
    distance= sqrt( distance );

    *pDifCount= difCount;
    *pDistance= distance;

    return;
    }

