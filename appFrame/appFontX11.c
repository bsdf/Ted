/************************************************************************/
/*									*/
/*  Management of X11 fonts: Translation of understandable fonts to	*/
/*  X11 structures.							*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"
#   include	"appGuiBase.h"
#   include	"appScreenFont.h"
#   include	"drawDrawingSurfaceImpl.h"
#   include	<utilLongestCommonSubstring.h>
#   include	<uniLegacyEncoding.h>
#   include	<uniLegacyMapping.h>
#   include	"appFindX11Fonts.h"
#   include	"drawImpl.h"

typedef struct XFontFaceSize
    {
    short int	xffsPixelSize;
    short int	xffsListIndex;
    } XFontFaceSize;

#   ifdef	USE_X11_FONTS

#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<ctype.h>
#   include	<string.h>

#   include	<appDebugon.h>
#   include	<psFontName.h>
#   include	<utilTree.h>
#   include	<psCompareFontInfo.h>

#   ifdef USE_GTK
#	include	<gdk/gdkx.h>
#   endif

#   include	<bitmap.h>

/************************************************************************/
/*									*/
/*  Strings used for the analysis of X11 font names.			*/
/*									*/
/*  1)  The asterisk to insert in a font name for all properties that	*/
/*	we consider to be irrelevant.					*/
/*  2)  The templete/format to make X11 font names.			*/
/*  3)  A series of encodings with information on the X11 encoding	*/
/*	strings and on the encodings for which they can be used.	*/
/*  4)  This is the combination of character set and encoding. Given	*/
/*	a particular value, XDrawString() draws the same string with	*/
/*	the same array of characters.					*/
/*									*/
/************************************************************************/

/*  1  */
static const char *	EditFontAnyTemplate= "*";
static const char *	EditFontAnyRegistry_EncodingTemplate= "*-*";

/*  2  */
# define	EditFontFamilyTemplate \
			    "-%s-%s-%s-%s-%s-%s-%s-%s-%s-%s-%s-%s-%s"

static void appInitXFontFaceSize(	XFontFaceSize *	xffs )
    {
    xffs->xffsPixelSize= 0;
    xffs->xffsListIndex= -1;
    }

# define appCleanXFontFaceSize( xffs ) /* nothing */

typedef struct XFontTypeface
    {
    char *		xftWeightStr;
    char *		xftSlantStr;
    unsigned char	xftWeight;
    unsigned char	xftIsBold;
    unsigned char	xftIsSlanted;
    unsigned char	xftIsScalable;
    unsigned char	xftIsFixedWidth;

    XFontFaceSize *	xftFaceSizes;
    int			xftFaceSizeCount;
    int			xftScalableListIndex;
    } XFontTypeface;

static void appInitXFontTypeface(	XFontTypeface *	xft )
    {
    xft->xftWeightStr= (char *)0;
    xft->xftSlantStr= (char *)0;
    xft->xftWeight= FONTweightMEDIUM;
    xft->xftIsBold= 0;
    xft->xftIsSlanted= 0;
    xft->xftIsScalable= 0;
    xft->xftIsFixedWidth= 0;

    xft->xftFaceSizes= (XFontFaceSize *)0;
    xft->xftFaceSizeCount= 0;

    xft->xftScalableListIndex= -1;

    return;
    }

static void appCleanXFontTypeface(	XFontTypeface *	xft )
    {
    int		i;

    if  ( xft->xftWeightStr )
	{ free( xft->xftWeightStr );	}
    if  ( xft->xftSlantStr )
	{ free( xft->xftSlantStr );	}

    for ( i= 0; i < xft->xftFaceSizeCount; i++ )
	{ appCleanXFontFaceSize( &(xft->xftFaceSizes[i]) );	}

    if  ( xft->xftFaceSizes )
	{ free( xft->xftFaceSizes );	}

    return;
    }

typedef struct XFontFamily
    {
    char *		xffFamily;
    char *		xffFamilyLower;
    char *		xffFoundry;
    int			xffSwdth;

    XFontTypeface *	xffFaces;
    int			xffFaceCount;
    } XFontFamily;

static void appInitXFontFamily(	XFontFamily *	xff )
    {
    xff->xffFamily= (char *)0;
    xff->xffFamilyLower= (char *)0;
    xff->xffFoundry= (char *)0;
    xff->xffSwdth= FONTwidthNORMAL;

    xff->xffFaces= (XFontTypeface *)0;
    xff->xffFaceCount= 0;
    }

static void appCleanXFontFamily(	XFontFamily *	xff )
    {
    int		i;

    if  ( xff->xffFamily )
	{ free( xff->xffFamily );	}
    if  ( xff->xffFamilyLower )
	{ free( xff->xffFamilyLower );	}

    if  ( xff->xffFoundry )
	{ free( xff->xffFoundry );	}

    for ( i= 0; i < xff->xffFaceCount; i++ )
	{ appCleanXFontTypeface( &(xff->xffFaces[i]) ); }

    if  ( xff->xffFaces )
	{ free( xff->xffFaces );	}
    }

/*  3  */
typedef struct AppFontEncoding
    {
    const char *	feX11Reg_Enc;
    int			feBytesPerChar;
    const int *		feGlyphUnicodes;

    int			feX11Reg_Enc_Strlen;
    IndexMapping	feSymbolToByteMapping;
    IndexSet		feSymbolSet;

    XFontFamily *	afeXfontFamilies;
    int			afeXfontFamilyCount;
    char **		afeXFontList;	/* from XListFonts()		*/
    int			afeXFontCount;	/* from XListFonts()		*/
    } AppFontEncoding;

static AppFontEncoding	AppFontXEncodings[]=
{

    { "iso10646-1",		2, },

    { "iso8859-15",		1, uniIso8859_15GlyphUnicodes,	},
    { "iso8859-2",		1, uniIso8859_2GlyphUnicodes,	},
    { "iso8859-5",		1, uniIso8859_5GlyphUnicodes,	},
    { "iso8859-7",		1, uniIso8859_7GlyphUnicodes,	},
    { "iso8859-9",		1, uniIso8859_9GlyphUnicodes,	},
    { "iso8859-13",		1, uniIso8859_13GlyphUnicodes,	},
    { "koi8-u",			1, uniKoi8RGlyphUnicodes,	},
    { "koi8-r",			1, uniKoi8RGlyphUnicodes,	},
    { "iso8859-1",		1, uniIso8859_1GlyphUnicodes,	},
    { "adobe-fontspecific",	1, },
    { "urw-fontspecific",	1, },

};

static const int AppFontXEncodingCount=
			    sizeof(AppFontXEncodings)/sizeof(AppFontEncoding);

/************************************************************************/
/*									*/
/*  Make a font query string.						*/
/*									*/
/************************************************************************/

static void appFontQueryString(	char *		target,
				const char *	foundry,
				const char *	family,
				const char *	weight,
				const char *	slant,
				const char *	swdth,
				const char *	pxlsz,
				const char *	registry_encoding )
    {
    const char *	anySizeTemplate= EditFontAnyTemplate;

    /*  Cope with silly Xlib */
    if  ( ! strcmp( pxlsz, "0" ) )
	{ anySizeTemplate= "0";	}

    sprintf( target, EditFontFamilyTemplate,
		foundry,
		family,
		weight,
		slant,
		swdth,			/* swdth		*/
		EditFontAnyTemplate,	/* adstyl		*/
		pxlsz,
		anySizeTemplate,	/* ptsz			*/
		anySizeTemplate,	/* resx			*/
		anySizeTemplate,	/* resy			*/
		EditFontAnyTemplate,	/* spacing		*/
		EditFontAnyTemplate,	/* average width	*/
		registry_encoding );

    return;
    }

/************************************************************************/
/*									*/
/*  Extract properties from an X font selection string.			*/
/*									*/
/************************************************************************/

static int appFontExtractString(	const char *	s,
					const char **	pPast,
					char *		target )
    {
    if  ( *s != '-' )
	{ SDEB(s); return -1;	}
    s++;
    while( *s && *s != '-' )
	{ *(target++)= *(s++); }
    *target= '\0';

    *pPast= s; return 0;
    }

static int appFontSkipString(	const char *	s,
				const char **	pPast )
    {
    if  ( *s != '-' )
	{ SDEB(s); return -1;	}
    s++;
    while( *s && *s != '-' )
	{ s++; }

    *pPast= s; return 0;
    }

static int appFontSplitXFont(	const char *	fontname,
				char *		foundry,
				char *		family,
				char *		weight,
				char *		slant,
				char *		swdth,
				char *		pxlszStr,
				char *		spacing,
				char *		avgwdth,
				char *		registry,
				char *		encoding )
    {
    const char *	s;

    s= fontname;

    if  ( appFontExtractString( s, &s, foundry ) )
	{ SDEB(fontname); return -1;	}

    if  ( appFontExtractString( s, &s, family ) )
	{ SDEB(fontname); return -1;	}

    if  ( appFontExtractString( s, &s, weight ) )
	{ SDEB(fontname); return -1;	}

    if  ( appFontExtractString( s, &s, slant ) )
	{ SDEB(fontname); return -1;	}

    if  ( appFontExtractString( s, &s, swdth ) )
	{ SDEB(fontname); return -1;	}

    if  ( appFontSkipString( s, &s ) ) /* adstyl */
	{ SDEB(fontname); return -1;	}

    if  ( appFontExtractString( s, &s, pxlszStr ) )
	{ SDEB(fontname); return -1;	}

    if  ( appFontSkipString( s, &s ) ) /* ptsz */
	{ SDEB(fontname); return -1;	}

    if  ( appFontSkipString( s, &s ) ) /* resx */
	{ SDEB(fontname); return -1;	}

    if  ( appFontSkipString( s, &s ) ) /* resy */
	{ SDEB(fontname); return -1;	}

    if  ( appFontExtractString( s, &s, spacing ) )
	{ SDEB(fontname); return -1;	}

    if  ( appFontExtractString( s, &s, avgwdth ) )
	{ SDEB(fontname); return -1;	}

    if  ( appFontExtractString( s, &s, registry ) )
	{ SDEB(fontname); return -1;	}

    if  ( appFontExtractString( s, &s, encoding ) )
	{ SDEB(fontname); return -1;	}

    if  ( s[0] )
	{ SSDEB(fontname,s); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Make a catalog of X11 fonts.					*/
/*									*/
/************************************************************************/

static int appCmpFacePixelSizes(	const void *	vxffs1,
					const void *	vxffs2 )
    {
    const XFontFaceSize *	xffs1= (const XFontFaceSize *)vxffs1;
    const XFontFaceSize *	xffs2= (const XFontFaceSize *)vxffs2;

    if  ( xffs1->xffsPixelSize > xffs2->xffsPixelSize )
	{ return  1; }

    if  ( xffs1->xffsPixelSize < xffs2->xffsPixelSize )
	{ return -1; }

    return 0;
    }

static int appFontXCatalog(	Display *		display,
				AppFontEncoding *	afe )
    {
    char			fontFormat[120];
	
    int				idx;

    XFontFamily *		xff;
    XFontTypeface *		xft;
    int				fam;
    int				face;

    if  ( afe->afeXFontList )
	{ return 0;	}

    appFontQueryString( fontFormat,
		EditFontAnyTemplate,	/* foundry,		*/
		EditFontAnyTemplate,	/* family,		*/
		EditFontAnyTemplate,	/* weight,		*/
		EditFontAnyTemplate,	/* slant,		*/
		EditFontAnyTemplate,	/* swdth,		*/
		EditFontAnyTemplate,	/* pxlsz,		*/
		afe->feX11Reg_Enc );	/* registry,encoding	*/

    afe->afeXFontList= XListFonts( display, fontFormat,
						4000, &(afe->afeXFontCount) );
    if  ( afe->afeXFontCount == 4000 )
	{ LLDEB(afe->afeXFontCount,4000);	}

    for ( idx= 0; idx < afe->afeXFontCount; idx++ )
	{
	char			foundry[40];
	char			family[40];

	char			weightStr[40];
	char			slantStr[40];
	char			swdthStr[40];

	char			ignore[40];
	char			spacing[40];
	char			avgwdth[40];

	char			pxlszStr[40];
	int			pxlsz;
	int			swdth;

	if  ( strlen( afe->afeXFontList[idx] ) >= X_FONTNAME_MAX )
	    { SDEB(afe->afeXFontList[idx]); continue; }

	if  ( appFontSplitXFont( afe->afeXFontList[idx],
					foundry,
					family,
					weightStr,
					slantStr,
					swdthStr,
					pxlszStr,
					spacing,
					avgwdth,
					ignore,		/*  registry  */
					ignore ) )	/*  encoding  */
	    { LSDEB(idx,afe->afeXFontList[idx]); continue;	}

	if  ( strcmp( spacing, "m" ) && strcmp( spacing, "p" ) )
	    {
	    if  ( strcmp( spacing, "c" ) )
		{ SSDEB(spacing,afe->afeXFontList[idx]);	}

	    continue;
	    }

	if  ( strcmp( pxlszStr, "0" ) && ! strcmp( avgwdth, "0" ) )
	    { /*SSDEB(pxlszStr,avgwdth);*/ continue;	}

	if  ( psFontGetWidth( &swdth, swdthStr ) )
	    { SSDEB(swdthStr,afe->afeXFontList[idx]); continue;	}

	xff= afe->afeXfontFamilies;
	for ( fam= 0; fam < afe->afeXfontFamilyCount; xff++, fam++ )
	    {
	    if  ( ! strcmp( xff->xffFamily, family )	&&
		  ! strcmp( xff->xffFoundry, foundry )	&&
		  xff->xffSwdth == swdth		)
		{ break;	}
	    }

	if  ( fam >= afe->afeXfontFamilyCount )
	    {
	    char *	s;

	    xff= (XFontFamily *)realloc( afe->afeXfontFamilies,
			(afe->afeXfontFamilyCount+1)* sizeof(XFontFamily) );
	    if  ( ! xff )
		{ LXDEB(afe->afeXfontFamilyCount,xff); return -1;	}
	    afe->afeXfontFamilies= xff;

	    xff += afe->afeXfontFamilyCount;

	    appInitXFontFamily( xff );

	    xff->xffFamily= strdup( family );
	    xff->xffFamilyLower= strdup( family );
	    xff->xffFoundry= strdup( foundry );
	    xff->xffSwdth= swdth;

	    if  ( ! xff->xffFamily	||
		  ! xff->xffFoundry	||
		  ! xff->xffFamilyLower	)
		{
		XXDEB(xff->xffFamily,xff->xffFoundry);
		appCleanXFontFamily( xff );
		return -1;
		}

	    s= xff->xffFamilyLower;
	    while( *s )
		{ *s= tolower( *s ); s++;	}

	    afe->afeXfontFamilyCount++;
	    }

	xft= xff->xffFaces;
	for ( face= 0; face < xff->xffFaceCount; xft++, face++ )
	    {
	    if  ( ! strcmp( xft->xftWeightStr, weightStr )	&&
		  ! strcmp( xft->xftSlantStr, slantStr )	)
		{ break;	}
	    }

	if  ( face >= xff->xffFaceCount )
	    {
	    xft= (XFontTypeface *)realloc( xff->xffFaces,
				(xff->xffFaceCount+1)* sizeof(XFontTypeface) );
	    if  ( ! xft )
		{ LXDEB(xff->xffFaceCount,xft); return -1;	}
	    xff->xffFaces= xft;

	    xft += xff->xffFaceCount;

	    appInitXFontTypeface( xft );

	    if  ( psFontGetWeight( &(xft->xftWeight), weightStr ) )
		{ SSDEB(afe->afeXFontList[idx],weightStr); continue;	}

	    if  ( xft->xftWeight > FONTweightMEDIUM )
		{ xft->xftIsBold= 1;	}
	    else{ xft->xftIsBold= 0;	}

	    if  ( strcmp( slantStr, "r" )	)
		{
		if  ( ! strcmp( slantStr, "i" )	||
		      ! strcmp( slantStr, "o" )	)
		    { xft->xftIsSlanted= 1;	}
		else{ SDEB(slantStr); continue;		}
		}

	    xft->xftIsScalable= 0;
	    xft->xftIsFixedWidth= ! strcmp( spacing, "m" );

	    xft->xftWeightStr= strdup( weightStr );
	    xft->xftSlantStr= strdup( slantStr );
	    if  ( ! xft->xftWeightStr || ! xft->xftSlantStr )
		{
		XXDEB(xft->xftWeightStr,xft->xftSlantStr);
		appCleanXFontTypeface( xft );
		return -1;
		}

	    xff->xffFaceCount++;
	    }

	pxlsz= atoi( pxlszStr );
	if  ( pxlsz == 0 )
	    {
	    xft->xftIsScalable= 1;
	    xft->xftScalableListIndex= idx;
	    }
	else{
	    XFontFaceSize *	xffs= xft->xftFaceSizes;
	    int			sz;

	    for ( sz= 0; sz < xft->xftFaceSizeCount; xffs++, sz++ )
		{
		if  ( xffs->xffsPixelSize == pxlsz )
		    { break;	}
		}

	    /* duplicate? */
	    if  ( sz < xft->xftFaceSizeCount )
		{
		/*
		SLLSDEB("DUPLICATE",idx,sz,afe->afeXFontList[idx]);
		LSDEB(xffs->xffsListIndex,
				afe->afeXFontList[xffs->xffsListIndex]);
		*/
		continue;
		}

	    xffs= (XFontFaceSize *)realloc( xft->xftFaceSizes,
			( xft->xftFaceSizeCount+ 1 )* sizeof( XFontFaceSize ) );
	    if  ( ! xffs )
		{ LXDEB(xft->xftFaceSizeCount,xffs); return -1;	}
	    xft->xftFaceSizes= xffs;

	    xffs += xft->xftFaceSizeCount;

	    appInitXFontFaceSize( xffs );

	    xffs->xffsPixelSize= pxlsz;
	    xffs->xffsListIndex= idx;

	    xft->xftFaceSizeCount++;
	    }
	}

    xff= afe->afeXfontFamilies;
    for ( fam= 0; fam < afe->afeXfontFamilyCount; xff++, fam++ )
	{
	xft= xff->xffFaces;
	for ( face= 0; face < xff->xffFaceCount; xft++, face++ )
	    {
	    if  ( xft->xftFaceSizeCount > 1 )
		{
		qsort( xft->xftFaceSizes, xft->xftFaceSizeCount,
				sizeof(XFontFaceSize), appCmpFacePixelSizes );
		}
	    }
	}

    /* NO! XFreeFontNames( afe->afeXFontList ); */

    return 0;
    }

/************************************************************************/
/*									*/
/*  Find an X11 font with a postscript style font.			*/
/*									*/
/*  1)  Obtain a list of the available X11 fonts.			*/
/*  2)  Try to find an almost exact match on the family name.		*/
/*	Have a preference for fonts with a foundry.			*/
/*  3)  Do not rely too much on scalable fonts. They look ugly and do	*/
/*	not always work transparently with remote displays.		*/
/*									*/
/*  a)  Scalable fonts tend to be a little wider than the corresponding	*/
/*	PostScript fonts. Just subtract a pixel.			*/
/*									*/
/************************************************************************/

static int appFontXFontUsingQuery(	char *			target,
					const char *		queryFormat,
					int			pixelSize )
    {
    const char *	before= queryFormat;
    const char *	after;
    int			beforeSize;
    int			afterSize;
    int			i;

    /*  a  */
    if  ( 0 && pixelSize > 10 )
	{ pixelSize--;	}

    if  ( *before != '-' )
	{ SDEB(queryFormat); return -1; }

    for ( i= 0; i < 6; i++ )
	{
	before= strchr( before+ 1, '-' );
	if  ( ! before )
	    { SDEB(queryFormat); return -1;	}
	}

    after= strchr( before+ 1, '-' );
    if  ( ! after )
	{ SDEB(queryFormat); return -1;	}

    beforeSize= before- queryFormat+ 1;
    afterSize= strlen( after );
    if  ( beforeSize+ 3+ afterSize >= X_FONTNAME_MAX )
	{ SDEB(queryFormat); return -1;	}

    memcpy( target, queryFormat, beforeSize );
    sprintf( target+ beforeSize, "%d", pixelSize );
    strcpy( target+ strlen( target ), after );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Make an estimate of the similarity of an X11 font family and a	*/
/*  PostScript font family. All we have is the names and the set width.	*/
/*  If results depend on the details of this routine, something else	*/
/*  is wrong: System configuration. But evidently, tweaking a little	*/
/*  does no harm.							*/
/*									*/
/************************************************************************/

static int appFontX11ScoreSimilarity(	const XFontFamily *	xff,
					const char *		psName,
					int			psNameLen,
					int			psWidth )
    {
    int		common;
    int		wdif;
    int		wrange= FONTwidth_MAX- FONTwidth_MIN;

    common= utilLongestCommonSubstring( xff->xffFamilyLower, psName );
    if  ( common < 5 && common < ( 2* psNameLen )/ 3 )
	{ return 0;	}

    wdif= psWidth- xff->xffSwdth;
    if  ( wdif < 0 )
	{ wdif= -wdif;	}

    return ( 100* common )/ psNameLen+
				( 30* ( wrange- wdif ) )/ wrange;
    }

/************************************************************************/
/*									*/
/*  Determine the X11 font name to use with a particular document font.	*/
/*									*/
/************************************************************************/

static int appFontXFontName(	char *			target,
				Display *		display,
				const AfmFontInfo *	afi,
				AppFontEncoding *	afe,
				int			anyFont,
				int			pixelSize,
				const char *		psName,
				int			psNameLen,
				int			psWidth )
    {
    const XFontFamily *		xff= (const XFontFamily *)0;
    const XFontTypeface *	xft;

    int				fam= 0;
    int				face;

    const char *		t1Entry= (const char *)0;
    const char *		scalableEntry= (const char *)0;

    int				best;
    int				f;

    if  ( ! afi || ! afe )
	{ XXDEB(afi,afe); return -1;	}

    if  ( afe->feX11Reg_Enc_Strlen == 0 )
	{ afe->feX11Reg_Enc_Strlen= strlen( afe->feX11Reg_Enc );	}

    for ( f= 0; f < afi->afiX11FontCount; f++ )
	{
	int		l11= strlen( afi->afiX11Fonts[f] );
	int		o11= l11- afe->feX11Reg_Enc_Strlen;

	if  ( o11 > 0							&&
	      afi->afiX11Fonts[f][o11-1] == '-'				&&
	      ! strcmp( afi->afiX11Fonts[f]+ o11, afe->feX11Reg_Enc )	)
	    { t1Entry= afi->afiX11Fonts[f]; break;	}
	}

    /*  1  */
    appFontXCatalog( display, afe );

    if  ( ! afe->afeXfontFamilies )
	{ return -1;	}

    /*  2  */
    xff= afe->afeXfontFamilies;
    best= 0;
    fam= -1;
    for ( f= 0; f < afe->afeXfontFamilyCount; xff++, f++ )
	{
	int	score;

	score= appFontX11ScoreSimilarity( xff, psName, psNameLen, psWidth );

	if  ( score > best )
	    { fam= f; best= score;	}
	}

    if  ( fam < 0 )
	{
	if  ( t1Entry )
	    {
	    if  ( appFontXFontUsingQuery( target, t1Entry, pixelSize ) )
		{ SDEB(t1Entry); return -1;	}

	    return 0;
	    }

	/* Arbitrarily pick first one */
	if  ( anyFont )
	    { fam= 0;		}
	else{ return 1;		}
	}

    xff= afe->afeXfontFamilies+ fam;

    {
    int		faceFound= xff->xffFaceCount;
    int		difCountFound= 999;
    double	distanceFound= 999;

    xft= xff->xffFaces;
    for ( face= 0; face < xff->xffFaceCount; xft++, face++ )
	{
	int		difCount;
	double		distance;

	psFontFaceDistance( &difCount, &distance, afi,
					xft->xftIsSlanted, xft->xftWeight );

	if  ( difCount < difCountFound		&&
	      difCount < 2			&&
	      distance < distanceFound		&&
	      distance < 0.3			)
	    {
	    faceFound= face;
	    difCountFound= difCount;
	    distanceFound= distance;
	    }
	}

    if  ( faceFound >= xff->xffFaceCount )
	{
	if  ( t1Entry )
	    {
	    if  ( appFontXFontUsingQuery( target, t1Entry, pixelSize ) )
		{ SDEB(t1Entry); return -1;	}

	    return 0;
	    }

	if  ( anyFont && xff->xffFaceCount > 0 )
	    { faceFound= 0;	}
	else{ return 1;		}
	}

    face= faceFound;
    xft= xff->xffFaces+ face;
    }

    /*  8  */
    if  ( xft->xftFaceSizeCount > 0 )
	{
	int		i;
	int		s;
	int		dv;

	for ( i= 0; i < xft->xftFaceSizeCount; i++ )
	    {
	    if  ( xft->xftFaceSizes[i].xffsPixelSize >= pixelSize )
		{ break;	}
	    }

	if  ( i >= xft->xftFaceSizeCount )
	    { i= xft->xftFaceSizeCount-1;	}

	if  ( i > 0						&&
	      xft->xftFaceSizes[i].xffsPixelSize > pixelSize	)
	    {
	    int		d0;
	    int		d1;

	    d0= ( 100* pixelSize )/ xft->xftFaceSizes[i-1].xffsPixelSize;
	    d1= ( 100* xft->xftFaceSizes[i].xffsPixelSize )/ pixelSize;

	    if  ( d1 > 105 || d0 < d1 )
		{ i--;	}
	    }

	s= xft->xftFaceSizes[i].xffsPixelSize;
	if  ( s >= pixelSize )
	    { dv= ( 100* s )/ pixelSize;	}
	else{ dv= ( 100* pixelSize )/ s;	}

	if  ( dv <= 115					||
	      ( ! t1Entry			&&
	        xft->xftScalableListIndex < 0	)	)
	    {
	    int	idx= xft->xftFaceSizes[i].xffsListIndex;

	    if  ( strlen( afe->afeXFontList[idx] ) >= X_FONTNAME_MAX )
		{ SDEB(afe->afeXFontList[idx]); }

	    strcpy( target, afe->afeXFontList[idx] );

	    return 0;
	    }
	}

    if  ( t1Entry )
	{ scalableEntry= t1Entry;	}
    else{
	if  ( xft->xftScalableListIndex < 0 )
	    { LDEB(xft->xftScalableListIndex); return -1;	}

	scalableEntry= afe->afeXFontList[xft->xftScalableListIndex];
	}

    if  ( appFontXFontUsingQuery( target, scalableEntry, pixelSize ) )
	{ SLDEB(scalableEntry,pixelSize); return -1;	}

    return 0;
    }

/************************************************************************/

static int appFontGetEncodingInfo(	const IndexMapping **	pIm,
					const IndexSet **	pIs,
					const int **		pUnicodes,
					AppFontEncoding *	fe,
					const char *		fontName )
    {
    IndexMapping *	im= &(fe->feSymbolToByteMapping);
    IndexSet *		is= &(fe->feSymbolSet);
    const int *		unicodes= fe->feGlyphUnicodes;

    if  ( fe->feBytesPerChar == 1 && ! unicodes )
	{
	if  ( ! strcmp( fontName, "ITC Zapf Dingbats" ) ||
	      ! strcmp( fontName, "ZapfDingbats" )	||
	      ! strcmp( fontName, "Dingbats" )	)
	    {
	    im= &UNI_DingbatsToGlyphMapping;
	    is= &UNI_DingbatsCodeSet;
	    unicodes= uniDingbatsGlyphUnicodes;
	    }
	else{
	    if  ( ! strcmp( fontName, "Symbol" ) )
		{
		im= &UNI_SymbolToGlyphMapping;
		is= &UNI_SymbolCodeSet;
		unicodes= uniSymbolGlyphUnicodes;
		}
	    else{ /*SDEB(fontName);*/ return 1;		}
	    }
	}

    if  ( unicodes && utilIndexMappingIsEmpty( im ) )
	{
	if  ( utilIndexMappingBuildBackward( im, unicodes, 256 )	)
	    { LDEB(1); return -1;	}
	if  ( utilIndexSetAddArray( is, unicodes, 256 )	)
	    { LDEB(1); return -1;	}
	}

    *pUnicodes= unicodes;
    *pIm= im;
    *pIs= is;
    return 0;
    }

/************************************************************************/

static int appXfontHasCode(	const XFontStruct *	xfs,
				int			code )
    {
    int		min_byte= xfs->min_char_or_byte2;
    int		max_byte= xfs->max_char_or_byte2;
    int		offset;

    if  ( xfs->min_byte1 == 0 && xfs->max_byte1 == 0 )
	{
	if  ( code < min_byte || code > max_byte )
	    { return 0;	}

	offset= code- min_byte;
	}
    else{
	int	D= max_byte- min_byte+ 1;
	int	byte1= code/ D;
	int	byte2= code% D;

	if  ( byte1 < xfs->min_byte1 || byte1 > xfs->max_byte1 )
	    { return 0;	}
	if  ( byte2 < min_byte || byte2 > max_byte )
	    { return 0;	}
	
	offset= D* ( byte1- xfs->min_byte1 )+ code- min_byte;
	}

    if  ( xfs->all_chars_exist )
	{ return 1;	}
    if  ( ! xfs->per_char )
	{
	/*LLXDEB(xfs->all_chars_exist,offset,xfs->per_char);*/
	return 1;
	}

    return xfs->per_char[offset].width > 0;
    }

/************************************************************************/

static void appXFontSymbols(	EncodedScreenFontList *		esfl,
				const EncodedScreenFont *	sf,
				IndexSet *			isRequired,
				const int *			unicodes,
				int				font )
    {
    const XFontStruct *	xfs;

#   ifdef USE_MOTIF
    xfs= sf->esfFontStruct;
#   endif

#   ifdef USE_GTK
#   if GTK_MAJOR_VERSION >= 2
    xfs= (XFontStruct *)gdk_x11_font_get_xfont( sf->esfFontStruct );
#   else
    xfs= GDK_FONT_XFONT( sf->esfFontStruct );
#   endif
#   endif

    if  ( unicodes )
	{
	int		code;

	for ( code= 0; code < 256; code++ )
	    {
	    int	symbol= unicodes[code];

	    if  ( symbol < 0 || ! appXfontHasCode( xfs, code ) )
		{ continue;	}

	    if  ( utilIndexMappingGet( &(esfl->esflSymbolToFont), symbol )
									>= 0 )
		{ continue;	}

	    if  ( utilIndexMappingPut( &(esfl->esflSymbolToFont),
							    symbol, font ) )
		{ LLDEB(symbol,font);	}

	    utilIndexSetRemove( isRequired, symbol );
	    }
	}
    else{
	if  ( sf->esfIsTwoByte )
	    {
	    int		symbol;

	    for ( symbol= 0; symbol < 256* 256; symbol++ )
		{
		if  ( appXfontHasCode( xfs, symbol ) )
		    {
		    if  ( utilIndexMappingGet( &(esfl->esflSymbolToFont),
								symbol ) >= 0 )
			{ continue;	}

		    if  ( utilIndexMappingPut( &(esfl->esflSymbolToFont),
							    symbol, font ) )
			{ LLDEB(symbol,font);	}

		    utilIndexSetRemove( isRequired, symbol );
		    }
		}
	    }
	else{ LDEB(sf->esfIsTwoByte);	}
	}

    return;
    }

/************************************************************************/

static int appFontX11TryEncoding(	EncodedScreenFont *	sf,
					EncodedScreenFontList *	esfl,
					DrawScreenFont *	dsf,
					int			font,
					AppFontEncoding *	fe,
					const AfmFontInfo *	afi,
					int			sizePixels,
					const char *		psName,
					int			psNameLen,
					int			psWidth,
					int			anyFont,
					IndexSet *		isRequired )
    {
    int				rval= 0;
    int				res;

    const IndexMapping *	im;
    const IndexSet *		isProvided;
    const int *			unicodes;

    IndexSet			isCandidate;

    utilInitIndexSet( &isCandidate );

    res= appFontGetEncodingInfo( &im, &isProvided, &unicodes,
						fe, afi->afiFamilyName );
    if  ( res < 0 )
	{ LDEB(res); rval= -1; goto ready;	}
    if  ( res > 0 )
	{ rval= 1; goto ready;			}

    utilCleanIndexSet( &isCandidate );
    utilInitIndexSet( &isCandidate );
    res= utilIndexSetIntersect( &isCandidate, isRequired, isProvided );
    if  ( res < 0 )
	{ LDEB(res); rval= -1; goto ready;	}
    if  ( res == 0 )
	{ rval= 1; goto ready;			}

    if  ( appFontXFontName( sf->esfFontName, dsf->dsfDisplay, afi, fe, anyFont,
				    sizePixels, psName, psNameLen, psWidth ) )
	{ rval= 1; goto ready;	}

    if  ( ! dsf->apfFontName[0] )
	{
	if  ( strlen( sf->esfFontName ) >= X_FONTNAME_MAX )
	    { SDEB(sf->esfFontName);				}
	else{ strcpy( dsf->apfFontName, sf->esfFontName );	}
	}

    sf->esfIsTwoByte= fe->feBytesPerChar == 2;
    sf->esfSymbolToByteMapping= im;

#   ifdef USE_MOTIF
    sf->esfFontStruct= XLoadQueryFont( dsf->dsfDisplay, sf->esfFontName );
#   endif

#   ifdef USE_GTK
    sf->esfFontStruct= gdk_font_load( sf->esfFontName );
#   endif

    if  ( ! sf->esfFontStruct )
	{
	SXDEB(sf->esfFontName,sf->esfFontStruct);
	drawCleanEncodedScreenFont( dsf, sf );
	drawInitEncodedScreenFont( sf );
	rval= 1; goto ready;
	}

    appXFontSymbols( esfl, sf, isRequired, unicodes, font );

  ready:

    utilCleanIndexSet( &isCandidate );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Get the X font with a font.						*/
/*									*/
/*  1)  Determine the full font height in pixels.			*/
/*  2)  Open the font for normal use.					*/
/*  3)  Obtain some font properties.					*/
/*  4)  Try to obtain a slightly smaller font for smallcaps.		*/
/*									*/
/************************************************************************/

# define SFCOUNT 12

int appFontOpenX11Fonts(	DrawScreenFont *		dsf )
    {
    int				rval= 0;
    const AfmFontInfo *		afi= dsf->apfPsFontInfo;
    char			psName[120+1];
    int				psNameLen;
    int				psWidth= FONTwidthNORMAL;

    int				count= 0;
    int				encIdx;

    EncodedScreenFontList *	esfl;
    EncodedScreenFont *		esfs= (EncodedScreenFont *)0;
    EncodedScreenFont *		sf;

    EncodedScreenFont		screenFonts[2*SFCOUNT];

    int				anyFont;
    IndexSet			isRequired;

    utilInitIndexSet( &isRequired );

    if  ( ! dsf )
	{ XDEB(dsf); return -1;	}
    esfl= &(dsf->dsfEncodedFonts);

    for ( encIdx= 0; encIdx < AppFontXEncodingCount; encIdx++ )
	{ drawInitEncodedScreenFont( screenFonts+ encIdx );	}

    if  ( AppFontXEncodingCount != SFCOUNT )
	{ LLDEB(AppFontXEncodingCount,SFCOUNT);	}

    if  ( psRemoveWidthFromName( psName, sizeof(psName)- 1, &psWidth,
						    afi->afiFamilyName ) < 0 )
	{ SDEB(afi->afiFamilyName); rval= -1; goto ready;	}
    for ( psNameLen= 0; psName[psNameLen]; psNameLen++ )
	{ psName[psNameLen]= tolower( psName[psNameLen] ); }

    if  ( dsf->dsfUnicodesUsed )
	{
	if  ( utilCopyIndexSet( &isRequired, dsf->dsfUnicodesUsed ) < 0 )
	    { LDEB(1); return -1;	}
	}

    /*  2  */
    sf= screenFonts;
    for ( anyFont= 0; anyFont <= 1; anyFont++ )
	{
	AppFontEncoding *	fe= AppFontXEncodings;

	for ( encIdx= 0; encIdx < AppFontXEncodingCount; fe++, encIdx++ )
	    {
	    int			res;

	    res= appFontX11TryEncoding( sf, esfl, dsf, count, fe, afi,
					dsf->dsfSizePixels, psName, psNameLen,
					psWidth, anyFont, &isRequired );
	    if  ( res < 0 )
		{ LDEB(res); rval= -1; goto ready;	}
	    if  ( res > 0 )
		{ continue;			}

	    count++; sf++;
	    }
	}

    if  ( count == 0 )
	{
	SLLDEB(afi->afiFamilyName,dsf->dsfSizePixels,count);
	rval= -1; goto ready;
	}

    esfs= malloc( count * sizeof(EncodedScreenFont) );
    if  ( ! esfs )
	{ LXDEB(count,esfs); rval= -1; goto ready;	}

    for ( encIdx= 0; encIdx < count; encIdx++ )
	{ esfs[encIdx]= screenFonts[encIdx]; }

    esfl->esfFonts= esfs; esfs= (EncodedScreenFont *)0; /* steal*/
    esfl->esfFontCount= count; count= 0; /* steal */

  ready:

    utilCleanIndexSet( &isRequired );

    if  ( esfs )
	{
	for ( encIdx= 0; encIdx < count; encIdx++ )
	    { drawCleanEncodedScreenFont( dsf, esfs+ encIdx );	}

	free( esfs );
	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Find the X11 fonts corresponding to the PostScript fonts in a font	*/
/*  list. The match is made on the basis of the full name of the fonts.	*/
/*									*/
/************************************************************************/

# ifndef XA_FACE_NAME
static Atom XA_FACE_NAME;
# endif

int appFindX11Fonts(	APP_WIDGET			topw,
			PostScriptFontList *		psfl )
    {
    int			rval= 0;
    int			i;

    void *		full2afi= (void *)0;
    const int		ownKeys= 0;

    char		fontFormat[120];
    char **		fontNames= (char **)0;
    XFontStruct *	fontInfos= (XFontStruct *)0;
    int			fontCount= 0;

#   ifdef USE_MOTIF
    Display *		display= XtDisplay( topw );
#   endif

#   ifdef USE_GTK
    Display *		display= GDK_DISPLAY();
#   endif


#   ifndef XA_FACE_NAME
    if  ( ! XA_FACE_NAME )
	{
	XA_FACE_NAME= XInternAtom( display, "FACE_NAME", TRUE );
	if  ( ! XA_FACE_NAME )
	    { XDEB(XA_FACE_NAME); rval= -1; goto ready;	}
	}
#   endif

    full2afi= utilTreeMakeTree( ownKeys );
    if  ( ! full2afi )
	{ XDEB(full2afi); rval= -1; goto ready;	}

    for ( i= 0; i < psfl->psflInfoCount; i++ )
	{
	AfmFontInfo *	afi= psfl->psflInfos[i];

	if  ( utilTreeStoreValue( full2afi, (void **)0, (const char **)0,
						    afi->afiFullName, afi ) )
	    { SDEB(afi->afiFullName); rval= -1; goto ready;	}
	}

    appFontQueryString( fontFormat,
		EditFontAnyTemplate,		/* foundry,		*/
		EditFontAnyTemplate,		/* family,		*/
		EditFontAnyTemplate,		/* weight,		*/
		EditFontAnyTemplate,		/* slant,		*/
		EditFontAnyTemplate,		/* swdth,		*/
		"0",				/* pxlsz: scalable	*/
		EditFontAnyRegistry_EncodingTemplate );	/* registry,enc	*/

    fontNames= XListFontsWithInfo( display, fontFormat, 6000,
						    &fontCount, &fontInfos );

    if  ( fontCount == 6000 )
	{ LDEB(fontCount);	}

    for ( i= 0; i < fontCount; i++ )
	{
	Atom	faceNameA;

	if  ( XGetFontProperty( &(fontInfos[i]), XA_FACE_NAME, &faceNameA ) )
	    {
	    char *		faceName= XGetAtomName( display, faceNameA );
	    AfmFontInfo *	afi;

	    afi= (AfmFontInfo *)utilTreeGetEQ( full2afi,
						(const char **)0, faceName );
	    if  ( afi )
		{
		if  ( psAddX11FontToInfo( afi, fontNames[i] ) )
		    { SDEB(fontNames[i]); rval= -1; goto ready;	}
		}
	    }
	}

  ready:

    if  ( fontNames )
	{ XFreeFontInfo( fontNames, fontInfos, fontCount );	}

    if  ( full2afi )
	{ utilTreeFreeTree( full2afi, (UTIL_TREE_CALLBACK)0, (void *)0 ); }

    return rval;
    }

/************************************************************************/
/*									*/
/*  Get the name and the encoding of a font.				*/
/*									*/
/************************************************************************/

void addDrawX11GetFontProperties(	EncodedScreenFont *	esf,
					DrawScreenFont *	dsf )
    {
    EncodedScreenFontList *	esfl= &(dsf->dsfEncodedFonts);
    AppFontEncoding *		fe;

    int				i;

    int				prevDash;
    int				len;
    XFontStruct *		xfs= (XFontStruct *)0;
    Display *			display;

    int				dash;

    static const char * an[]=
	{
	"FOUNDRY",
	"FAMILY_NAME",
	"WEIGHT_NAME",
	"SLANT",
	"SETWIDTH_NAME",
	"ADD_STYLE_NAME",
	"PIXEL_SIZE",
	"POINT_SIZE",
	"RESOLUTION_X",
	"RESOLUTION_Y",
	"SPACING",
	"AVERAGE_WIDTH",
	"CHARSET_REGISTRY",
	"CHARSET_ENCODING"
	};

    static Atom na[14];

#   ifdef USE_MOTIF
    xfs= esf->esfFontStruct;
    display= dsf->dsfDisplay;
#   endif

#   ifdef USE_GTK
#   if GTK_MAJOR_VERSION >= 2
    xfs= (XFontStruct *)gdk_x11_font_get_xfont( esf->esfFontStruct );
#   else
    xfs= GDK_FONT_XFONT( esf->esfFontStruct );
#   endif

    if  ( esf->esfFontStruct->type != GDK_FONT_FONT )
	{ LDEB(esf->esfFontStruct->type); return;	}
    display= GDK_DISPLAY();
#   endif

    if  ( na[0] == None )
	{
	for ( i= 0; i < 14; i++ )
	    { na[i]= XInternAtom( display, an[i], False );	}
	}

    len= 0;
    prevDash= dash= -1;
    for ( i= 0; i < 14; i++ )
	{
	Atom		a= None;
	char *		as= (char *)0;
	int		al;

	char		scratch[15];

	scratch[0]= '\0';

	if  ( XGetFontProperty( xfs, na[i], &a ) )
	    {
	    if  ( i <= 5 || i >= 12 || i == 10 )
		{ as= XGetAtomName( display, a );		}
	    else{ as= scratch; sprintf( scratch, "%ld", a );	}
	    }

	if  ( ! as )
	    { as= scratch;	}
	al= strlen( as );

	if  ( len+ 1+ al >= X_FONTNAME_MAX )
	    { LLDEB(len+ 1+ al,X_FONTNAME_MAX); break;	}

	prevDash= dash; dash= len;
	esf->esfFontName[len++]= '-';
	strcpy( esf->esfFontName+ len, as ); len += al;

	if  ( as != scratch )
	    { XFree( as );	}
	}

#   if 0
    {
    const char *	name= gdk_x11_font_get_name( esf->esfFontStruct );

    len= strlen( name );
    prevDash= -1;

    if  ( len > X_FONTNAME_MAX )
	{ SLDEB(name, X_FONTNAME_MAX); return;	}
    strcpy( esf->esfFontName, name );

    for ( i= 0; i < 13; i++ )
	{
	char *	d= strchr( name+ prevDash+ 1, '-' );

	if  ( ! d )
	    { XDEB(d); break;	}
	prevDash= d- name;
	}
    if  ( i < 13 )
	{ LSDEB(i,name); return;	}
    }
#   endif

    for ( i= 0; i < len; i++ )
	{ esf->esfFontName[i]= tolower( esf->esfFontName[i] );	}

    fe= AppFontXEncodings;
    for ( i= 0; i < AppFontXEncodingCount; fe++, i++ )
	{
	if  ( ! strcmp( esf->esfFontName+ prevDash+ 1, fe->feX11Reg_Enc ) )
	    {
	    IndexMapping *	im= &(fe->feSymbolToByteMapping);
	    const int *		unicodes= fe->feGlyphUnicodes;

	    if  ( unicodes )
		{
		int		code;
		const int	font= 0;

		if  ( utilIndexMappingIsEmpty( im )			&&
		      utilIndexMappingBuildBackward( im, unicodes, 256 ) )
		    { LDEB(1); return;	}

		esf->esfSymbolToByteMapping= im;

		for ( code= 0; code < 256; code++ )
		    {
		    int	symbol= unicodes[code];

		    if  ( symbol < 0 || ! appXfontHasCode( xfs, code ) )
			{ continue;	}

		    if  ( utilIndexMappingPut( &(esfl->esflSymbolToFont),
							    symbol, font ) )
			{ LLDEB(symbol,font);	}
		    }
		}
	    }
	}

    if  ( ! esf->esfSymbolToByteMapping )
	{ SXDEB(esf->esfFontName,esf->esfSymbolToByteMapping);	}
    }

#   endif

int guiSystemFont(	DrawingSurface		ds,
			APP_FONT *		xfs )
    {
    NumberedPropertiesList *	npl= &(ds->dsScreenFontAdmin);
    DrawScreenFont *		dsf= (DrawScreenFont *)0;
    int				screenFont;

    /* HACK */
    dsf= utilPagedListClaimItemAtEnd( &screenFont, &(npl->nplPagedList) );
    if  ( ! dsf )
	{ XDEB(dsf); return -1;	}

    dsf->dsfEncodedFonts.esfFonts= malloc( sizeof(EncodedScreenFont) );
    if  ( ! dsf->dsfEncodedFonts.esfFonts )
	{ XDEB(dsf->dsfEncodedFonts.esfFonts); return -1;	}
    drawInitEncodedScreenFont( dsf->dsfEncodedFonts.esfFonts );
    dsf->dsfEncodedFonts.esfFontCount= 1;

    dsf->dsfEncodedFonts.esfFonts[0].esfFontStruct= xfs;

#   ifdef USE_MOTIF
    dsf->dsfDisplay= ds->dsDisplay;
    dsf->dsfDrawable= ds->dsDrawable;
    dsf->dsfGc= ds->dsGc;
    dsf->dsfEncodedFonts.esfFonts[0].esfFontBorrowed= 1;
#   endif

#   ifdef USE_GTK
    dsf->dsfDrawable= ds->dsDrawable;
    dsf->dsfGc= ds->dsGc;
    dsf->dsfEncodedFonts.esfFonts[0].esfFontBorrowed= 0;
#   endif

#   ifdef USE_XFT
    dsf->dsfXftDrawable= ds->dsXftDrawable;
#   endif

#   ifdef USE_X11_FONTS
    addDrawX11GetFontProperties( dsf->dsfEncodedFonts.esfFonts, dsf );
    strcpy( dsf->apfFontName, dsf->dsfEncodedFonts.esfFonts->esfFontName );
#   endif

    return screenFont;
    }

