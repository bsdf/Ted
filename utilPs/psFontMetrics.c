/************************************************************************/
/*									*/
/*  Derive text metrics from font metrics.				*/
/*									*/
/************************************************************************/
#   include	"utilPsConfig.h"

#   include	<ctype.h>

#   include	"psFontMetrics.h"
#   include	<uniUtf8.h>
#   include	<ucdGeneralCategory.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Calculate the width of a string.					*/
/*									*/
/*  1)  Assume that space is encoded as space.				*/
/*  2)  Contrary to expectations, kerning is an aplication subject. The	*/
/*	printer by itself does not do kerning.				*/
/*									*/
/************************************************************************/

int psCalculateStringExtents(	DocumentRectangle *	dr,
				const char *		s,
				int			len,
				int			twipsSize,
				int			withKerning,
				int			vswap,
				const AfmFontInfo *	afi )
    {
    int				off= 0;
    long			x1= 0L;
    long			unitsWide= 0L;

    long			x0= 0;
    long			y1= 0;
    long			y0= 0;
    long			prevX1= 0;

    const AfmCharMetric *	prev= (const AfmCharMetric *)0;

    const IndexMapping *	glyphMap= &(afi->afiUnicodeToGlyphMapping);

    if  ( off < len )
	{
	unsigned short		uni;
	int			glyph;
	int			step= uniGetUtf8( &uni, s+ off );
	const AfmCharMetric *	acm;

	if  ( step < 1 )
	    { LDEB(step); return -1;	}
	glyph= utilIndexMappingGetU( glyphMap, uni );
	if  ( glyph < 0 )
	    { glyph= afi->afiFallbackGlyph;	}
	acm= afi->afiMetrics[glyph];

	x0= acm->acmBBox.drX0;
	y1= acm->acmBBox.drY1;
	y0= acm->acmBBox.drY0;

	prevX1= unitsWide;
	unitsWide += acm->acmWX;
	if  ( ! ucdIsZs( uni ) )
	    { x1= prevX1+ acm->acmBBox.drX1; }

	prev= acm; off += step;
	}

    while( off < len )
	{
	unsigned short		uni;
	int			glyph;
	int			step= uniGetUtf8( &uni, s+ off );
	const AfmCharMetric *	acm;

	if  ( step < 1 )
	    { XXLDEB(s[off],s[off+1],step); return -1;	}
	glyph= utilIndexMappingGetU( glyphMap, uni );
	if  ( glyph < 0 )
	    { glyph= afi->afiFallbackGlyph;	}
	acm= afi->afiMetrics[glyph];

	/*  2  */
	if  ( withKerning && prev )
	    {
	    int			j;
	    AfmKerningPair *	akp= prev->acmKernPairs;

	    for ( j= 0; j < prev->acmKernPairCount; akp++, j++ )
		{
		if  ( akp->akpPosition == glyph )
		    { unitsWide += akp->akpXVec; break; }
		}
	    }

	if  ( y1 < acm->acmBBox.drY1 )
	    { y1=  acm->acmBBox.drY1;	}
	if  ( y0 > acm->acmBBox.drY0 )
	    { y0=  acm->acmBBox.drY0;	}

	prevX1= unitsWide;
	unitsWide += acm->acmWX;
	if  ( ! ucdIsZs( uni ) )
	    { x1= prevX1+ acm->acmBBox.drX1; }

	prev= acm; off += step;
	}

    if  ( vswap )
	{
	dr->drX0= ( twipsSize*  x0+ 500 )/1000;
	dr->drY1= ( twipsSize* -y0- 500 )/1000;
	dr->drY0= ( twipsSize* -y1- 500 )/1000;
	dr->drX1= ( twipsSize*  x1+ 500 )/1000;
	}
    else{
	dr->drX0= ( twipsSize*  x0+ 500 )/1000;
	dr->drY1= ( twipsSize*  y1+ 500 )/1000;
	dr->drY0= ( twipsSize*  y0+ 500 )/1000;
	dr->drX1= ( twipsSize*  x1+ 500 )/1000;
	}

    return ( twipsSize* unitsWide+ 500 )/1000;
    }

/****************************************************************/
/*								*/
/*  Return the underline position/thickness for this font.	*/
/*  NOTE that this is abused with a twipssize in pixels.	*/
/*								*/
/****************************************************************/

void psUnderlineGeometry(	int *			pPos,
				int *			pThick,
				int			baseline,
				int			twipsSize,
				const AfmFontInfo *	afi )
    {
    *pPos= baseline- ( twipsSize* afi->afiUnderlinePosition+ 500 )/ 1000;
    *pThick= ( twipsSize* afi->afiUnderlineThickness+ 500 )/ 1000;

    return;
    }

void psStrikethroughGeometry(	int *			pPos,
				int *			pThick,
				int			baseline,
				int			twipsSize,
				const AfmFontInfo *	afi )
    {
    int		xHeight= afi->afiXHeight;
    int		pos;

    if  ( xHeight == 0 )
	{ xHeight= 500;	}
    pos= xHeight/ 2;
    pos += afi->afiUnderlineThickness/ 2;

    *pPos= baseline- ( twipsSize* pos+ 500 )/ 1000;
    *pThick= ( twipsSize* afi->afiUnderlineThickness+ 500 )/ 1000;

    return;
    }

int psFontBBox(			DocumentRectangle *	drBBox,
				DocumentRectangle *	drAscDesc,
				int			twipsSize,
				int			vswap,
				const AfmFontInfo *	afi )
    {
    if  ( vswap )
	{
	drBBox->drX0= ( twipsSize*  afi->afiFontBBox.drX0+ 500 )/1000;
	drBBox->drY1= ( twipsSize* -afi->afiFontBBox.drY0- 500 )/1000;
	drBBox->drY0= ( twipsSize* -afi->afiFontBBox.drY1- 500 )/1000;
	drBBox->drX1= ( twipsSize*  afi->afiFontBBox.drX1+ 500 )/1000;

	*drAscDesc= *drBBox;
	drAscDesc->drY1= ( twipsSize* -afi->afiDescender- 500 )/1000;
	drAscDesc->drY0= ( twipsSize* -afi->afiAscender-  500 )/1000;
	}
    else{
	drBBox->drX0= ( twipsSize*  afi->afiFontBBox.drX0+ 500 )/1000;
	drBBox->drY1= ( twipsSize*  afi->afiFontBBox.drY1+ 500 )/1000;
	drBBox->drY0= ( twipsSize*  afi->afiFontBBox.drY0+ 500 )/1000;
	drBBox->drX1= ( twipsSize*  afi->afiFontBBox.drX1+ 500 )/1000;

	*drAscDesc= *drBBox;
	drAscDesc->drY1= ( twipsSize*  afi->afiAscender+  500 )/1000;
	drAscDesc->drY0= ( twipsSize*  afi->afiDescender+ 500 )/1000;
	}

    return drBBox->drX1;
    }

int psGetSuperBaseline(	int *			pSuperBaseline,
			int			baseline,
			int			fontSizeTwips,
			const AfmFontInfo *	afi )
    {
#   if 0
    int suprSizeTwips= SUPERSUB_SIZE( fontSizeTwips );

    int	ascYF= ( fontSizeTwips* afi->afiAscender+ 500 )/ 1000;
    int	ascYS= ( suprSizeTwips* afi->afiAscender+ 500 )/ 1000;

    *pSuperBaseline= baseline- ( ascYF- ascYS );
#   endif

    int fAsc= afi->afiAscender;
    int sAsc= SUPERSUB_SIZE( afi->afiAscender );

    *pSuperBaseline= baseline- ( fontSizeTwips* ( fAsc- sAsc )+ 500 )/ 1000;

    return 0;
    }

int psGetSubBaseline(	int *			pSubBaseline,
			int			baseline,
			int			fontSizeTwips,
			const AfmFontInfo *	afi )
    {
#   if 0
    int subsSizeTwips= SUPERSUB_SIZE( fontSizeTwips );

    int	descYF= ( fontSizeTwips* afi->afiDescender+ 500 )/ 1000;
    int	descYS= ( subsSizeTwips* afi->afiDescender+ 500 )/ 1000;

    *pSubBaseline= baseline+ ( descYS- descYF );
#   endif

    int fDesc= afi->afiDescender;
    int sDesc= SUPERSUB_SIZE( afi->afiDescender );

    *pSubBaseline= baseline+ ( fontSizeTwips* ( sDesc- fDesc )+ 500 )/ 1000;

    return 0;
    }

int psGetSmallcapsSize(	int *			pSmallcapsSize,
			int			fontSizeTwips,
			const AfmFontInfo *	afi )
    {
    int		xHeight= afi->afiXHeight;
    int		capHeight= afi->afiCapHeight;

    if  ( xHeight == 0 )
	{ xHeight= 500;	}
    if  ( capHeight == 0 )
	{ capHeight= afi->afiAscender;	}
    if  ( capHeight == 0 )
	{ capHeight= 700;	}

    *pSmallcapsSize= ( xHeight* fontSizeTwips )/ capHeight;
    return 0;
    }
