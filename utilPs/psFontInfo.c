#   include	"utilPsConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<ctype.h>

#   include	"psFontInfo.h"
#   include	"psGlyphs.h"
#   include	<utilTree.h>
#   include	<uniAlternatives.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Bookkeeping and management of AFM information.			*/
/*									*/
/************************************************************************/

static void psInitAfmCodeList(	AfmCodeList *	acl )
    {
    acl->aclCodes= (unsigned short int *)0;
    acl->aclCodeCount= 0;
    }

static void psInitAfmCharMetric(	AfmCharMetric *	acm )
    {
    psInitAfmCodeList( &(acm->acmDefaultCodeList) );
    psInitAfmCodeList( &(acm->acmUnicodeCodeList) );

    acm->acmWX= 0;;
    geoInitRectangle ( &(acm->acmBBox) );
    acm->acmN= (char *)0;
    acm->acmKernPairs= (AfmKerningPair *)0;
    acm->acmKernPairCount= 0;
    acm->acmGlyphIndex= -1;
    }

static void psCleanAfmCodeList(	AfmCodeList *	acl )
    {
    if  ( acl->aclCodes )
	{ free( acl->aclCodes );	}
    }

static void psFreeAfmCharMetric(	AfmCharMetric *	acm )
    {
    psCleanAfmCodeList( &(acm->acmDefaultCodeList) );
    psCleanAfmCodeList( &(acm->acmUnicodeCodeList) );

    if  ( acm->acmN )
	{ free( acm->acmN );		}
    if  ( acm->acmKernPairs )
	{ free( acm->acmKernPairs );	}

    free( acm );
    }

static int psFontMetricAddCodeToList(	AfmCodeList *		acl,
					AfmFontInfo *		afi,
					int			C )
    {
    int			i;
    unsigned short *	fresh;

    if  ( C < 0 )
	{ LDEB(C); return -1;	}

    for ( i= 0; i < acl->aclCodeCount; i++ )
	{
	if  ( acl->aclCodes[i] == C )
	    { return 0;	}
	}

    fresh= (unsigned short *)realloc( acl->aclCodes,
			    ( acl->aclCodeCount+ 1 )* sizeof(unsigned short) );
    if  ( ! fresh )
	{ LXDEB(acl->aclCodeCount,fresh); return -1;	}
    acl->aclCodes= fresh;

    acl->aclCodes[acl->aclCodeCount++]= C;

    return 0;
    }

/************************************************************************/

int psGetUnicodesFromGlyphNames(	AfmFontInfo *	afi )
    {
    int		glyphIndex;
    int		rval= 0;

    IndexMapping *	tgm= &(afi->afiUnicodeToGlyphMapping);

    for ( glyphIndex= 0; glyphIndex < afi->afiMetricCount; glyphIndex++ )
	{
	AfmCharMetric *	acm= afi->afiMetrics[glyphIndex];
	if  ( acm && acm->acmN )
	    {
	    int		code= psGlyphNameToUnicode( acm->acmN );

	    if  ( code < 0 )
		{ /*SDEB(acm->acmN);*/ continue;	}

	    if  ( psFontMetricAddCodeToList( &(acm->acmUnicodeCodeList),
								afi, code ) )
		{ LDEB(code); return -1;	}

	    if  ( utilIndexMappingGet( tgm, code ) < 0 )
		{
		utilIndexMappingPut( tgm, code, glyphIndex );

		if  ( utilIndexSetAdd( &(afi->afiUnicodesProvided), code ) )
		    { LDEB(code);	}
		}
	    }
	}

    return rval;
    }

int psGetAlternateGlyphs(	AfmFontInfo *	afi )
    {
    if  ( uniMapToAlternatives( &(afi->afiUnicodeToGlyphMapping) ) )
	{ SDEB(afi->afiFullName);	}

    if  ( uniIncludeWithAlternatives( &(afi->afiUnicodesProvided) ) )
	{ SDEB(afi->afiFullName);	}

    return 0;
    }

int psResolveFallbackGlyph(	AfmFontInfo *	afi )
    {
    IndexMapping *	tgm= &(afi->afiUnicodeToGlyphMapping);
    int			space;

    if  ( afi->afiFallbackGlyph >= 0 )
	{ return 0;	}

    space= utilIndexMappingGet( tgm, ' ' );
    if  ( space >= 0 )
	{ afi->afiFallbackGlyph= space; return 0;	}

    SDEB(afi->afiFullName); return -1;
    }

/************************************************************************/
/*									*/
/*  Initialise an AfmFontInfo						*/
/*									*/
/************************************************************************/

void psInitAfmFontInfo(	AfmFontInfo *	afi )
    {
    afi->afiFaceNumber= -1;

    afi->afiFontName= (char *)0;
    afi->afiFullName= (char *)0;
    afi->afiNotice= (char *)0;
    afi->afiVersion= (char *)0;
    afi->afiFamilyName= (char *)0;
    afi->afiFamilyName_Orig= (char *)0;
    afi->afiWeightStr= (char *)0;
    afi->afiWeightInt= FONTweightREGULAR;
    afi->afiWidthStr= (char *)0;
    afi->afiWidthInt= FONTwidthNORMAL;

    afi->afiItalicAngle= 0.0;
    afi->afiTanItalicAngle= 0.0;

    afi->afiIsFixedPitch= 0;
    geoInitRectangle( &(afi->afiFontBBox) );
    afi->afiUnderlinePosition= 0;
    afi->afiUnderlineThickness= 0;
    afi->afiEncodingScheme= (char *)0;
    afi->afiCapHeight= 0;
    afi->afiXHeight= 0;
    afi->afiAscender= 0;
    afi->afiDescender= 0;
    afi->afiCharacterSet= (char *)0;
    afi->afiVendor= (char *)0;

    afi->afiResourceName= (char *)0;
    utilInitMemoryBuffer( &(afi->afiFontFileName) );
    afi->afiFontFileIndex= 0;

    utilInitMemoryBuffer( &(afi->afiAfmFileName) );
    afi->afiMetricsDeferred= 0;
    afi->afiIgnoreKerning= 0;
    afi->afiMetricCount= 0;
    afi->afiMetrics= (AfmCharMetric **)0;
    afi->afiNameToMetric= (void *)0;
    afi->afiFallbackGlyph= 0;

    afi->afiX11FontCount= 0;
    afi->afiX11Fonts= (char **)0;

    utilInitIndexMapping( &(afi->afiUnicodeToGlyphMapping) );
    utilInitIndexSet( &(afi->afiUnicodesProvided) );

    /**/
    afi->afiStyle= (char *)0;

    afi->afiResolveMetrics= (ResolveFontMetrics)0;

    return;
    }

void psRemoveMetricsFromInfo(	AfmFontInfo *	afi )
    {
    int		i;

    for ( i= 0; i < afi->afiMetricCount; i++ )
	{
	if  ( afi->afiMetrics[i] )
	    { psFreeAfmCharMetric( afi->afiMetrics[i] );	}
	}

    if  ( afi->afiMetrics )
	{ free( afi->afiMetrics );}

    afi->afiMetricCount= 0;
    afi->afiMetrics= (AfmCharMetric **)0;

    if  ( afi->afiNameToMetric )
	{
	utilTreeFreeTree( afi->afiNameToMetric,
					(UTIL_TREE_CALLBACK)0, (void *)0 );
	afi->afiNameToMetric= (void *)0;
	}

    return;
    }

void psCleanAfmFontInfo(	AfmFontInfo *	afi )
    {
    int		i;

    if  ( afi->afiFontName )
	{ free( afi->afiFontName );	}
    if  ( afi->afiFullName )
	{ free( afi->afiFullName );	}
    if  ( afi->afiNotice )
	{ free( afi->afiNotice );	}
    if  ( afi->afiVersion )
	{ free( afi->afiVersion );	}
    if  ( afi->afiFamilyName )
	{ free( afi->afiFamilyName );	}
    if  ( afi->afiFamilyName_Orig )
	{ free( afi->afiFamilyName_Orig );	}
    if  ( afi->afiWeightStr )
	{ free( afi->afiWeightStr );	}
    if  ( afi->afiWidthStr )
	{ free( afi->afiWidthStr );	}
    if  ( afi->afiEncodingScheme )
	{ free( afi->afiEncodingScheme );}
    if  ( afi->afiCharacterSet )
	{ free( afi->afiCharacterSet );}
    if  ( afi->afiVendor )
	{ free( afi->afiVendor );}
    if  ( afi->afiResourceName )
	{ free( afi->afiResourceName );}

    utilCleanMemoryBuffer( &(afi->afiFontFileName) );
    utilCleanMemoryBuffer( &(afi->afiAfmFileName) );

    for ( i= 0; i < afi->afiX11FontCount; i++ )
	{
	if  ( afi->afiX11Fonts[i] )
	    { free( afi->afiX11Fonts[i] );	}
	}

    if  ( afi->afiX11Fonts )
	{ free( afi->afiX11Fonts );}

    psRemoveMetricsFromInfo( afi );

    /* afi->afiStyle is a reference to another string */

    utilCleanIndexMapping( &(afi->afiUnicodeToGlyphMapping) );
    utilCleanIndexSet( &(afi->afiUnicodesProvided) );

    return;
    }

void psFreeAfmFontInfo(	AfmFontInfo *	afi )
    {
    psCleanAfmFontInfo(	afi );
    free( afi );
    }

/************************************************************************/
/*									*/
/*  Add one code for the default encoding to a char metric.		*/
/*									*/
/************************************************************************/

static int psFontMetricAddCode(		AfmCharMetric *		acm,
					AfmFontInfo *		afi,
					int			C )
    {
    if  ( psFontMetricAddCodeToList( &(acm->acmDefaultCodeList), afi, C ) )
	{ LDEB(C); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Add one metic to a font info.					*/
/*									*/
/************************************************************************/

int psFontInfoAddMetric(		AfmFontInfo *			afi,
					int				C,
					int				WX,
					const char *			N,
					const DocumentRectangle *	abb )
    {
    AfmCharMetric **	fresh;
    AfmCharMetric *	acm;
    int			duplicate= 0;

    if  ( ! afi->afiNameToMetric )
	{
	const int	ownKeys= 0;

	afi->afiNameToMetric= utilTreeMakeTree( ownKeys );
	if  ( ! afi->afiNameToMetric )
	    { XDEB(afi->afiNameToMetric); return -1;	}
	}

    if  ( N && N[0] )
	{
	acm= (AfmCharMetric *)utilTreeGetEQ( afi->afiNameToMetric, (const char **)0, N );

	if  ( acm )
	    {
	    if  ( C >= 0 && psFontMetricAddCode( acm, afi, C ) )
		{ LDEB(C); return -1;	}

	    duplicate= 1;
	    }
	}

    fresh= (AfmCharMetric **)realloc( afi->afiMetrics,
			( afi->afiMetricCount+ 1 )* sizeof(AfmCharMetric *) );
    if  ( ! fresh )
	{ LXDEB(C,fresh); return -1;	}
    afi->afiMetrics= fresh;

    fresh[afi->afiMetricCount]= acm= (AfmCharMetric *)malloc(
						    sizeof(AfmCharMetric) );
    if  ( ! acm )
	{ XDEB(acm); return -1;	}

    psInitAfmCharMetric( acm );

    acm->acmWX= (int)WX;
    if  ( N && ! duplicate )
	{
	acm->acmN= strdup( N );

	if  ( utilTreeStoreValue( afi->afiNameToMetric,
					    (void **)0, (const char **)0,
					    acm->acmN, acm ) )
	    { LDEB(afi->afiMetricCount);	}
	}

    acm->acmBBox= *abb;
    acm->acmGlyphIndex= afi->afiMetricCount++;

    if  ( C >= 0 && psFontMetricAddCode( acm, afi, C ) )
	{ LDEB(C); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Set/Add the default encoding for a glyph.				*/
/*  As this is the wrong way round in the AFM file format, more than	*/
/*  one code can map to the same glyph.					*/
/*									*/
/************************************************************************/

int psFontInfoSetGlyphCode(	AfmFontInfo *		afi,
				int			glyphIndex,
				int			C )
    {
    if  ( glyphIndex < 0			||
	  glyphIndex >= afi->afiMetricCount	||
	  ! afi->afiMetrics[glyphIndex]		)
	{ LLDEB(glyphIndex,afi->afiMetricCount); return -1;	}

    if  ( psFontMetricAddCode( afi->afiMetrics[glyphIndex], afi, C ) )
	{ LDEB(C); return -1;	}

    return 0;
    }

int psFontInfoSetGlyphUnicode(	AfmFontInfo *		afi,
				int			glyphIndex,
				int			code )
    {
    AfmCharMetric *	acm;
    IndexMapping *	tgm= &(afi->afiUnicodeToGlyphMapping);

    if  ( glyphIndex < 0			||
	  glyphIndex >= afi->afiMetricCount	||
	  ! afi->afiMetrics[glyphIndex]		)
	{ LLDEB(glyphIndex,afi->afiMetricCount); return -1;	}

    acm= afi->afiMetrics[glyphIndex];

    if  ( psFontMetricAddCodeToList( &(acm->acmUnicodeCodeList), afi, code ) )
	{ LDEB(code); return -1;	}

    if  ( utilIndexMappingGet( tgm, code ) < 0 )
	{
	utilIndexMappingPut( tgm, code, glyphIndex );

	if  ( utilIndexSetAdd( &(afi->afiUnicodesProvided), code ) )
	    { LDEB(code);	}
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Remember the name of a glyph.					*/
/*									*/
/************************************************************************/

int psFontInfoSetGlyphName(	AfmFontInfo *		afi,
				int			glyphIndex,
				const char *		glyphName )
    {
    if  ( glyphIndex < 0			||
	  glyphIndex >= afi->afiMetricCount	||
	  ! afi->afiMetrics[glyphIndex]		)
	{ LLDEB(glyphIndex,afi->afiMetricCount); return -1;	}

    if  ( afi->afiMetrics[glyphIndex]->acmN )
	{
	utilTreeDeleteEQ( afi->afiNameToMetric,
				afi->afiMetrics[glyphIndex]->acmN,
				(UTIL_TREE_CALLBACK)0, (void *)0 );
	free( afi->afiMetrics[glyphIndex]->acmN );
	}

    afi->afiMetrics[glyphIndex]->acmN= strdup( glyphName );
    if  ( ! afi->afiMetrics[glyphIndex]->acmN )
	{ XDEB(afi->afiMetrics[glyphIndex]->acmN); return -1;	}

    if  ( utilTreeStoreValue( afi->afiNameToMetric,
					(void **)0, (const char **)0,
					afi->afiMetrics[glyphIndex]->acmN,
					afi->afiMetrics[glyphIndex] ) )
	{ LDEB(afi->afiMetricCount);	}

    return 0;
    }

const char * psFontInfoGetGlyphName(	const AfmFontInfo *	afi,
					int			glyphIndex )
    {
    if  ( glyphIndex < 0			||
	  glyphIndex >= afi->afiMetricCount	||
	  ! afi->afiMetrics[glyphIndex]		)
	{ LLDEB(glyphIndex,afi->afiMetricCount); return (const char *)0; }

    return afi->afiMetrics[glyphIndex]->acmN;
    }

int psFontInfoGetGlyphIndex(	const AfmFontInfo *	afi,
				const char *		glyphName )
    {
    AfmCharMetric *	acm;

    if  ( ! afi->afiNameToMetric )
	{
	if  ( afi->afiMetricCount > 0 )
	    { LXDEB(afi->afiMetricCount,afi->afiNameToMetric);	}

	return -1;
	}

    acm= (AfmCharMetric *)utilTreeGetEQ( afi->afiNameToMetric,
						(const char **)0, glyphName );
    if  ( ! acm )
	{ return -1;	}

    return acm->acmGlyphIndex;
    }

/************************************************************************/
/*									*/
/*  Add one metic to a font info.					*/
/*									*/
/************************************************************************/

int psAddX11FontToInfo(		AfmFontInfo *			afi,
				const char *			fontName )
    {
    int		i;
    char **	fresh;

    for ( i= 0; i < afi->afiX11FontCount; i++ )
	{
	if  ( ! strcmp( afi->afiX11Fonts[i], fontName ) )
	    { return 0;	}
	}

    fresh= (char **)realloc( afi->afiX11Fonts,
			( afi->afiX11FontCount+ 1 )* sizeof(char *) );
    if  ( ! fresh )
	{ XDEB(fresh); return -1;	}
    afi->afiX11Fonts= fresh;

    fresh[afi->afiX11FontCount]= strdup( fontName );
    if  ( ! fresh[afi->afiX11FontCount] )
	{ XDEB(fresh[afi->afiX11FontCount]); return -1;	}

    afi->afiX11FontCount++;

    return 0;
    }

