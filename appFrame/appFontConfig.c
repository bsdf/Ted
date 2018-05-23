#   include	"appFrameConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<stdlib.h>
#   include	<string.h>

#   include	"drawImpl.h"
#   include	"appScreenFont.h"
#   include	<psGlyphs.h>
#   include	<psFontName.h>
#   include	<uniLegacyEncoding.h>
#   include	"appMatchFont.h"
#   include	<uniAlternatives.h>

#   include	<appDebugon.h>

#   ifdef USE_FONTCONFIG			/*  {{	*/

#   include	<fontconfig/fontconfig.h>
#   include	<ft2build.h>
#   include	FT_FREETYPE_H

static int		APP_FontConfigInitialized= 0;
static FcConfig *	APP_FcConfig= 0;

/* Cope with older freetype versions: */
# if ! defined(FT_ENCODING_UNICODE) && defined(ft_encoding_unicode)
# define FT_ENCODING_UNICODE ft_encoding_unicode
# endif
# if ! defined(FT_ENCODING_MS_SYMBOL) && defined(ft_encoding_symbol)
# define FT_ENCODING_MS_SYMBOL ft_encoding_symbol
# endif

static const char * appFtErrorStr( int e );

static int		APP_FreeTypeInitialized= 0;
static FT_Library	APP_FT_Library;

static void appFcScaleRect(	DocumentRectangle *	abb,
				int			upm )
    {
    abb->drX0= ( 1000* abb->drX0 )/ upm;
    abb->drX1= ( 1000* abb->drX1 )/ upm;
    abb->drY0= ( 1000* abb->drY0 )/ upm;
    abb->drY1= ( 1000* abb->drY1 )/ upm;
    }

static void appFcScaleInt(	int *		pV,
				int		upm )
    {
    *pV= ( 1000* *pV )/ upm;
    }

/************************************************************************/
/*									*/
/*  Set a mapping from character encoding to glyph index for this face.	*/
/*  Base the mapping on the unicode encoding of the face. The encoding	*/
/*  has already been set as the current encoding.			*/
/*									*/
/************************************************************************/

static int appFcGetEncodingsFromUnicode(AfmFontInfo *		afi,
					const FT_Face		ftFace )
    {
    FT_ULong	charcode;
    FT_UInt	glyphIdx;

    charcode= FT_Get_First_Char( ftFace, &glyphIdx );
    while( glyphIdx != 0 )
	{
	if  ( glyphIdx < afi->afiMetricCount )
	    {
	    if  ( charcode == 'x' )
		{ afi->afiXHeight= afi->afiMetrics[glyphIdx]->acmBBox.drY1; }
	    if  ( charcode == 'H' )
		{ afi->afiCapHeight= afi->afiMetrics[glyphIdx]->acmBBox.drY1; }

	    /*  Only support UTF16 */
	    if  ( charcode <= 0xffff )
		{
		if  ( psFontInfoSetGlyphUnicode( afi, glyphIdx, charcode ) )
		    { SLXDEB(afi->afiFontName,glyphIdx,charcode); return -1; }

		if  ( ! FT_HAS_GLYPH_NAMES( ftFace )	&&
		      ! afi->afiMetrics[glyphIdx]->acmN	)
		    {
		    psFontInfoSetGlyphName( afi, glyphIdx,
					psUnicodeToGlyphName( charcode ) );
		    }
		}
	    }

	charcode= FT_Get_Next_Char( ftFace, charcode, &glyphIdx );
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Set a mapping from character encoding to glyph index for this face.	*/
/*  Base the mapping on the known mapping from dingbats to unicode.	*/
/*									*/
/************************************************************************/

static int appFcGetEncodingsFromAdobeDingbats(
					AfmFontInfo *		afi,
					const FT_Face		ftFace )
    {
    FT_ULong	charcode;
    FT_UInt	glyphIdx;

    charcode= FT_Get_First_Char( ftFace, &glyphIdx );
    while ( glyphIdx != 0 )
	{
	if  ( charcode < 256 )
	    {
	    int unicode= uniDingbatsGlyphUnicodes[charcode];

	    if  ( unicode >= 0						&&
		  psFontInfoSetGlyphUnicode( afi, glyphIdx, unicode )	)
		{ LLLDEB(glyphIdx,charcode,unicode); return -1;	}
	    }
	else{ LDEB(charcode);	}

	charcode= FT_Get_Next_Char( ftFace, charcode, &glyphIdx );
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Set a mapping from character encoding to glyph index for this face.	*/
/*  Base the mapping on a preset encoding for the face.			*/
/*									*/
/************************************************************************/

static int appFcGetDefaultEncoding(	AfmFontInfo *		afi,
					const FT_Face		ftFace )
    {
    int		ch;

    for ( ch= 0; ch < 256; ch++ )
	{
	int		code= ch;
	int		glyphIdx;

	glyphIdx= FT_Get_Char_Index( ftFace, code );
	if  ( glyphIdx <= 0 )
	    {
	    code= uniGetAlternative( code );
	    if  ( code >= 0 )
		{ glyphIdx= FT_Get_Char_Index( ftFace, code );	}
	    }

	/* HACK MS Symbol */
	if  ( glyphIdx <= 0 )
	    { glyphIdx= FT_Get_Char_Index( ftFace, 61440+ ch );	}

	if  ( glyphIdx <= 0 || glyphIdx >= afi->afiMetricCount )
	    {
	    /* SLDEB(afi->afiFullName,glyphIdx); */
	    continue;
	    }

	if  ( code == 'x' )
	    { afi->afiXHeight= afi->afiMetrics[glyphIdx]->acmBBox.drY1; }
	if  ( code == 'H' )
	    { afi->afiCapHeight= afi->afiMetrics[glyphIdx]->acmBBox.drY1; }

	if  ( psFontInfoSetGlyphCode( afi, glyphIdx, ch ) )
	    { LLDEB(glyphIdx,ch); return -1;	}
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Diagnostic routine: Show the encodings of a font.			*/
/*									*/
/************************************************************************/

static void appFcShowCharmaps(	const AfmFontInfo *	afi,
				FT_Face			ftFace )
    {
    int		cm;

    SLDEB(afi->afiFullName,ftFace->num_charmaps);
    for ( cm= 0; cm < ftFace->num_charmaps; cm++ )
	{
	FT_CharMap *	ftCharMap= ftFace->charmaps+ cm;
	char 		s[5];
	int		fterror;

	FT_ULong	charcode;
	FT_UInt		glyphIdx;

	s[0]= ( (*ftCharMap)->encoding >> 24 ) & 0xff;
	s[1]= ( (*ftCharMap)->encoding >> 16 ) & 0xff;
	s[2]= ( (*ftCharMap)->encoding >>  8 ) & 0xff;
	s[3]= ( (*ftCharMap)->encoding       ) & 0xff;
	s[4]= '\0';
	LSLLDEB(cm,s,(*ftCharMap)->platform_id,(*ftCharMap)->encoding_id);

	fterror= FT_Set_Charmap( ftFace, *ftCharMap );
	if  ( fterror )
	    { LSLDEB(cm,s,fterror); continue;	}

	charcode= FT_Get_First_Char( ftFace, &glyphIdx );
	while ( glyphIdx != 0 )
	    {
	    charcode= FT_Get_Next_Char( ftFace, charcode, &glyphIdx );

	    if  ( charcode < 256 )
		{
		unsigned int	ch= charcode&0xff;
		if  ( glyphIdx < afi->afiMetricCount )
		    { CLSDEB(ch,glyphIdx,afi->afiMetrics[glyphIdx]->acmN); }
		else{ CLLDEB(ch,glyphIdx,afi->afiMetricCount); }
		}
	    else{
		if  ( glyphIdx < afi->afiMetricCount )
		    { XLSDEB(charcode,glyphIdx,afi->afiMetrics[glyphIdx]->acmN); }
		else{ XLLDEB(charcode,glyphIdx,afi->afiMetricCount); }
		}
	    }
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Get the metrics of a font. As, with freetype, this can be an	*/
/*  expensive operation. This is deferred until the font is actually	*/
/*  used.								*/
/*									*/
/*  1)  Open the face.							*/
/*  2)  Get the name that can be used in PostScript output.		*/
/*  3)  Get metrics for the face as a whole. XHeight and CapHeight are	*/
/*	approximated at first and will be updated when the 'x' or the	*/
/*	'H' are encountered on the road.				*/
/*  4)  Get metrics and the name of the individual glyphs.		*/
/*									*/
/************************************************************************/

int appFcGetFontMetrics(	AfmFontInfo *	afi )
    {
    int			rval= 0;
    FT_Face		ftFace= (FT_Face)0;
    int			fterror;
    int			glyphIdx;
    const char *	fontName;
    int			invalidComposites= 0;

    /*  1  */
    fterror= FT_New_Face( APP_FT_Library,
			utilMemoryBufferGetString( &(afi->afiFontFileName) ),
			afi->afiFontFileIndex,
			&ftFace );
    if  ( fterror )
	{
	LLDEB(afi->afiFontFileIndex,fterror);
	return -1;
	}

    /*  2  */
    fontName= FT_Get_Postscript_Name( ftFace );
    if  ( fontName )
	{
	char *	s;
	char *	fn= strdup( fontName );
	if  ( ! fn )
	    { XDEB(fn); rval= -1; goto ready;	}

	s= fn;
	while( *s )
	    {
	    if  ( *s == ' ' )
		{ *s=   '-';	}
	    s++;
	    }

	if  ( afi->afiFontName )
	    { free( afi->afiFontName );	}

	afi->afiFontName= fn;
	}

    /*  3  */
    afi->afiFontBBox.drX0= ftFace->bbox.xMin;
    afi->afiFontBBox.drX1= ftFace->bbox.xMax;
    afi->afiFontBBox.drY0= ftFace->bbox.yMin;
    afi->afiFontBBox.drY1= ftFace->bbox.yMax;

    afi->afiAscender= ftFace->ascender;
    afi->afiDescender= ftFace->descender;
    afi->afiUnderlinePosition= ftFace->underline_position;
    afi->afiUnderlineThickness= ftFace->underline_thickness;

    afi->afiCapHeight= afi->afiAscender;
    afi->afiXHeight= ftFace->units_per_EM/ 2;

    appFcScaleRect( &(afi->afiFontBBox), ftFace->units_per_EM );
    appFcScaleInt( &(afi->afiAscender), ftFace->units_per_EM );
    appFcScaleInt( &(afi->afiDescender), ftFace->units_per_EM );
    appFcScaleInt( &(afi->afiUnderlinePosition), ftFace->units_per_EM );
    appFcScaleInt( &(afi->afiUnderlineThickness), ftFace->units_per_EM );

    appFcScaleInt( &(afi->afiCapHeight), ftFace->units_per_EM );
    appFcScaleInt( &(afi->afiXHeight), ftFace->units_per_EM );

    /*  4  */
    for ( glyphIdx= 0; glyphIdx < ftFace->num_glyphs; glyphIdx++ )
	{
	DocumentRectangle	abb;
	int			C= -1;
	int			WX;
	char *			N= (char *)0;
	char			n[200];

	fterror= FT_Load_Glyph( ftFace, glyphIdx,
			    FT_LOAD_NO_SCALE|FT_LOAD_IGNORE_TRANSFORM );
	if  ( fterror )
	    {
	    if  ( fterror == FT_Err_Invalid_Composite )
		{
		if  ( invalidComposites )
		    { continue;	}

		invalidComposites= 1;
		}

	    LSDEB(glyphIdx,appFtErrorStr(fterror));
	    continue;
	    }

	WX= ftFace->glyph->metrics.horiAdvance;
	abb.drX0= ftFace->glyph->metrics.horiBearingX;
	abb.drX1= abb.drX0+ ftFace->glyph->metrics.width;
	abb.drY1= ftFace->glyph->metrics.horiBearingY;
	abb.drY0= abb.drY1- ftFace->glyph->metrics.height;

	appFcScaleInt( &WX, ftFace->units_per_EM );
	appFcScaleRect( &abb, ftFace->units_per_EM );

	if  ( FT_HAS_GLYPH_NAMES( ftFace ) )
	    {
	    if  ( FT_Get_Glyph_Name( ftFace, glyphIdx, n, sizeof(n) ) )
		{ LDEB(glyphIdx);	}
	    else{ N= n;			}
	    }

	if  ( psFontInfoAddMetric( afi, C, WX, N, &abb ) )
	    { LDEB(1); rval= -1; goto ready;	}

	/* TTF default char */
	if  ( afi->afiFallbackGlyph < 0		&&
	      N && ! strcmp( N, ".notdef" )	)
	    { afi->afiFallbackGlyph= glyphIdx;	}
	}

    /*  Derive Dingbats mapping via known mapping */
    if  ( ! strcmp(afi->afiFamilyName, "Dingbats" )		||
          ! strcmp(afi->afiFamilyName, "ITC Zapf Dingbats" )	)
	{
	fterror= FT_Select_Charmap( ftFace, FT_ENCODING_ADOBE_CUSTOM );
	if  ( ! fterror )
	    { appFcGetEncodingsFromAdobeDingbats( afi, ftFace );	}
	}

    /*  5  */
    fterror= FT_Select_Charmap( ftFace, FT_ENCODING_UNICODE );
    if  ( ! fterror )
	{ appFcGetEncodingsFromUnicode( afi, ftFace );	}

    if  ( afi->afiMetricsDeferred )
	{
	fterror= FT_Select_Charmap( ftFace, FT_ENCODING_MS_SYMBOL );
	if  ( ! fterror )
	    { appFcGetDefaultEncoding( afi, ftFace );	}
	}

    if  ( psGetUnicodesFromGlyphNames( afi ) )
	{ SDEB(afi->afiFamilyName);	}
    if  ( psGetAlternateGlyphs( afi ) )
	{ SDEB(afi->afiFamilyName);	}
    if  ( psResolveFallbackGlyph( afi ) )
	{ SDEB(afi->afiFamilyName);	}

    afi->afiMetricsDeferred= 0;

    if  ( afi->afiMetricsDeferred )
	{
	SDEB(afi->afiFontName);
	appFcShowCharmaps( afi, ftFace );
	afi->afiMetricsDeferred= 0; rval= -1; goto ready;
	}
  ready:

    if  ( ftFace )
	{ FT_Done_Face( ftFace );	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Handle one font in the list of fonts that we retrieved through	*/
/*  fontconfig.								*/
/*									*/
/*  If the font implements some of the font-encodings supported by Ted,	*/
/*  it is remembered in the PostScript font list.			*/
/*									*/
/************************************************************************/

static int appFcListOneFont(	PostScriptFontList *	psfl,
				FcPattern *		font )
    {
    int			rval= 0;

    FcChar8 *		family= NULL;
    FcChar8 *		style= NULL;
    int			width= -1;
    int			weight= -1;
    int			slant= -1;
    int			spacing= -1;
    FcChar8 *		file= (FcChar8 *)0;
    int			idx= 0;

    char *		fontName= (char *)0;
    char *		fullName= (char *)0;
    AfmFontInfo *	afi= (AfmFontInfo *)0;

    FcPatternGetString( font, FC_FAMILY, 0, &family );
    FcPatternGetString( font, FC_STYLE, 0, &style );
    FcPatternGetInteger( font, FC_WEIGHT, 0, &weight );
    FcPatternGetInteger( font, FC_WIDTH, 0, &width );
    FcPatternGetInteger( font, FC_SLANT, 0, &slant );
    FcPatternGetInteger( font, FC_SPACING, 0, &spacing );

    if  ( FcPatternGetString( font, FC_FILE, 0, &file ) != FcResultMatch )
	{ SDEB((char *)family); goto ready; }
    FcPatternGetInteger( font, FC_INDEX, 0, &idx );

    if  ( ! family )
	{ XDEB(family); goto ready;	}
    if  ( ! style )
	{ style= (FcChar8 *)"";		}

    /************************/
    {
    char *	s;

    fontName= (char *)malloc(
	    strlen( (char *)family )+ 1+ strlen( (char *)style )+ 1 );
    fullName= (char *)malloc(
	    strlen( (char *)family )+ 1+ strlen( (char *)style )+ 1 );

    if  ( strlen( (char *)style ) > 0 )
	{
	sprintf( fontName, "%s-%s", (char *)family, (char *)style );
	sprintf( fullName, "%s %s", (char *)family, (char *)style );
	}
    else{
	strcpy( fontName, (char *)family );
	strcpy( fullName, (char *)family );
	}

    s= fontName;
    while( *s )
	{
	if  ( *s == ' ' )
	    { *s=   '-';	}
	s++;
	}
    }
    /************************/

    /* A duplicate from a different file? */
    afi= psPostScriptFontListGetFontInfoByFontName( psfl, fontName );
    if  ( afi )
	{
	if  ( psPostScriptFontListSetFontInfoForFaceFile( psfl, afi,
							    (char *)file ) )
	    { SDEB((char *)file); rval= -1; goto ready;	}

	goto ready;
	}

    afi= (AfmFontInfo *)malloc( sizeof(AfmFontInfo) );
    if  ( ! afi )
	{ XDEB(afi); rval= -1; goto ready;	}
    psInitAfmFontInfo( afi );

    afi->afiFontName= fontName; fontName= (char *)0; /* steal */
    afi->afiFullName= fullName; fullName= (char *)0; /* steal */

    afi->afiFamilyName= strdup( (char *)family );
    afi->afiWeightStr= strdup( psWeightStr( weight ) );
    afi->afiWeightInt= weight;
    afi->afiWidthStr= strdup( psWidthStr( width ) );
    afi->afiWidthInt= width;

    if  ( slant > ( FC_SLANT_ROMAN+ 2* FC_SLANT_ITALIC )/ 3 )
	{ afi->afiItalicAngle= -13.0; /* typical negative value */	}
    else{ afi->afiItalicAngle= 0.0;					}

    afi->afiIsFixedPitch= spacing != FC_PROPORTIONAL;

    if  ( utilMemoryBufferSetString( &(afi->afiFontFileName), (char *)file ) )
	{ LDEB(1); rval= -1; goto ready;	}
    afi->afiFontFileIndex= idx;

    afi->afiMetricsDeferred= 1;
    afi->afiResolveMetrics= appFcGetFontMetrics;
    if  ( ! afi->afiMetricsDeferred )
	{
	if  ( appFcGetFontMetrics( afi ) )
	    { SLDEB((char *)file,idx); goto ready;	}
	}

    /* No: This is a font as it is found on the machine: use it without 
     * modifications.
    psMapVisibleNamesToBase35( afi );
    */

    if  ( psPostScriptFontListAddInfo( psfl, afi ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    if  ( fontName )
	{ free( fontName );	}
    if  ( fullName )
	{ free( fullName );	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Include fonts that can be accessed via fontconfig/freetype/xft in	*/
/*  the PostScript font list.						*/
/*									*/
/************************************************************************/

int appFcListFonts(	PostScriptFontList *	psfl )
    {
    int			rval= 0;
    FcPattern *		pat= (FcPattern *)0;
    FcObjectSet *	os= (FcObjectSet *)0;
    FcFontSet *		fs= (FcFontSet *)0;

    int			i;

    if  ( ! APP_FontConfigInitialized )
	{
	if  ( FcInit() != FcTrue )
	    { LDEB(1); rval= -1; goto ready;	}

	APP_FontConfigInitialized= 1;
	}

    if  ( ! APP_FreeTypeInitialized )
	{
	int	fterror= FT_Init_FreeType( &APP_FT_Library );

	if  ( fterror )
	    { LDEB(fterror); rval= -1; goto ready;	}

	APP_FreeTypeInitialized= 1;
	}

    pat= FcPatternCreate();
    if  ( ! pat )
	{ XDEB(pat); rval= -1; goto ready;	}
    if  ( ! FcPatternAddBool( pat, FC_SCALABLE, FcTrue ) )
	{ SDEB(FC_SCALABLE); rval= -1; goto ready;	}

    os= FcObjectSetBuild(	FC_FAMILY,
				FC_WIDTH,
				FC_STYLE,
				FC_FILE,
				FC_INDEX,
				FC_CHARSET,
				FC_SLANT,
				FC_WEIGHT,
				FC_SPACING,
			(char *)0 );
    if  ( ! os )
	{ XDEB(os); rval= -1; goto ready;	}

    fs= FcFontList( APP_FcConfig, pat, os );
    if  ( ! fs )
	{ XDEB(fs); rval= -1; goto ready;	}

    for ( i= 0; i < fs->nfont; i++ )
	{
	if  ( appFcListOneFont( psfl, fs->fonts[i] ) )
	    { LDEB(i); rval= -1; goto ready;	}
	}

    if  ( psPostScriptFontListInfosToFamilies( psfl ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    if  ( fs )
	{ FcFontSetDestroy( fs );	}
    if  ( os )
	{ FcObjectSetDestroy( os );	}
    if  ( pat )
	{ FcPatternDestroy( pat );	}

    return rval;
    }

/************************************************************************/

static int appFcAddChar(	int		unicode,
				void *		vcharset )
    {
    FcCharSetAddChar( (FcCharSet *)vcharset, unicode );

    return 0;
    }

/************************************************************************/

# define LOG_MATCHES 0

static AfmFontInfo * appFcGetFontInfoFromFont(
				FcPattern *			font,
				const PostScriptFontList *	psfl )
    {
    AfmFontInfo *	afi= (AfmFontInfo *)0;
    FcChar8 *		file= (FcChar8 *)0;
#   if LOG_MATCHES
    FcChar8 *		family= (FcChar8 *)0;
    FcChar8 *		style= (FcChar8 *)0;

    FcPatternGetString( font, FC_FAMILY, 0, &family );
    FcPatternGetString( font, FC_STYLE, 0, &style );
#   endif
    FcPatternGetString( font, FC_FILE, 0, &file );

#   if LOG_MATCHES
    appDebug( "TRY: file=\"%s\" family= \"%s\" style=\"%s\"\n",
				(char *)file, (char *)family, (char *)style );
#   endif

    afi= psPostScriptFontListGetFontInfoByFaceFile( psfl, (char *)file );
    if  ( ! afi )
	{ /*SXDEB((char *)file,afi);*/	}

    return afi;
    }

static AfmFontInfo * appFcGetFontInfoFromList(
				FcPattern *			pat,
				const PostScriptFontList *	psfl )
    {
    FcResult		fcResult;
    FcFontSet *		fs= (FcFontSet *)0;
    AfmFontInfo *	afi= (AfmFontInfo *)0;
    int			i;

    fs= FcFontSort( APP_FcConfig, pat, FcFalse, (FcCharSet **)0, &fcResult );
    if  ( ! fs )
	{ XLDEB(fs,fcResult); goto ready;	}

    for ( i= 0; i < fs->nfont; i++ )
	{
	FcPattern *	font= fs->fonts[i];

	afi= appFcGetFontInfoFromFont( font, psfl );
	if  ( afi )
	    {
#	    if LOG_MATCHES
	    appDebug( "GOT: \"%s\"\n", afi->afiFontName );
#	    endif
	    break;
	    }
	}

  ready:

    if  ( fs )
	{ FcFontSetDestroy( fs );	}

    return afi;
    }

/************************************************************************/
/*									*/
/*  THE font matching call for fonts via fontconfig:			*/
/*  Find the PostScript font family that matches a document font.	*/
/*									*/
/*  The implicit assumption is that the PostScript font list is		*/
/*  constructed with appFcListFonts().					*/
/*									*/
/************************************************************************/

AfmFontInfo * appFcGetFontInfoForAttribute(
				const char *			familyName,
				int				styleInt,
				const IndexSet *		unicodesUsed,
				const TextAttribute *		ta,
				const PostScriptFontList *	psfl )
    {
    AfmFontInfo *		rval= (AfmFontInfo *)0;

    FcPattern *			pat= (FcPattern *)0;
    FcPattern *			found= (FcPattern *)0;
    FcResult			fcResult;

    FcCharSet *			fcCharset= (FcCharSet *)0;

    int				slant= FC_SLANT_ROMAN;
    int				weight= FC_WEIGHT_REGULAR;

    int				width= FONTwidthNORMAL;
    char			scratch[200+1];

    if  ( unicodesUsed )
	{
	fcCharset= FcCharSetCreate();
	if  ( ! fcCharset )
	    { XDEB(fcCharset); goto ready;	}
	if  ( utilIndexSetForAll( unicodesUsed, appFcAddChar, fcCharset ) < 0 )
	    { LDEB(1); goto ready;	}
	}

    if  ( ta->taFontIsBold )
	{ weight= FC_WEIGHT_BOLD;	}
    if  ( ta->taFontIsSlanted )
	{ slant= FC_SLANT_ITALIC;	}

    if  ( psRemoveWidthFromName( scratch, sizeof(scratch)- 1, &width,
							    familyName ) < 0 )
	{ SDEB(familyName); goto ready;	}

    /*  HACK */
    if  ( ! strcmp( scratch, "ITC Zapf Dingbats" ) )
	{ strcpy( scratch, "Dingbats");	}

    pat= FcPatternCreate();
    if  ( ! pat )
	{ XDEB(pat); goto ready;	}
    if  ( ! FcPatternAddBool( pat, FC_SCALABLE, FcTrue ) )
	{ SDEB(FC_SCALABLE); goto ready;	}
    if  ( ! FcPatternAddString( pat, FC_FAMILY, (FcChar8 *)scratch ) )
	{ SDEB(FC_FAMILY); goto ready;	}
    if  ( ! FcPatternAddInteger( pat, FC_WIDTH, width ) )
	{ SDEB(FC_WIDTH); goto ready;	}
    if  ( ! FcPatternAddInteger( pat, FC_WEIGHT, weight ) )
	{ SDEB(FC_WEIGHT); goto ready;	}
    if  ( ! FcPatternAddInteger( pat, FC_SLANT, slant ) )
	{ SDEB(FC_SLANT); goto ready;	}
    if  ( fcCharset && ! FcPatternAddCharSet( pat, FC_CHARSET, fcCharset ) )
	{ SDEB(FC_CHARSET); goto ready;	}

    FcConfigSubstitute( APP_FcConfig, pat, FcMatchPattern );
    FcDefaultSubstitute( pat );

#   if LOG_MATCHES
    appDebug( "FIND: fam=\"%s\", width=%s\n",
				    scratch, psWidthStr( width ) );
#   endif

    found= FcFontMatch( APP_FcConfig, pat, &fcResult );
    if  ( ! found )
	{ SXLDEB(scratch,found,fcResult); goto ready;	}
    rval= appFcGetFontInfoFromFont( found, psfl );

    if  ( ! rval )
	{ rval= appFcGetFontInfoFromList( pat, psfl );	}

    if  ( ! rval )
	{ SXDEB(scratch,rval); goto ready;	}

  ready:

    if  ( pat )
	{ FcPatternDestroy( pat );	}
    if  ( found )
	{ FcPatternDestroy( found );	}
    if  ( fcCharset )
	{ FcCharSetDestroy( fcCharset );	}

    return rval;
    }

#   endif				/*  }}	*/

#   ifdef USE_XFT			/*  {{	*/

/************************************************************************/

static int appFontAddOneCharToSet(	int		val,
					void *		voidfcs )
    {
    FcCharSetAddChar((FcCharSet *)voidfcs, val );
    return 0;
    }

/************************************************************************/
/*									*/
/*  Open an XFT font corresponding to a (PostScript) font face in the	*/
/*  font catalog.							*/
/*									*/
/************************************************************************/

XftFont * drawOpenXftFont(	DrawScreenFont *	dsf )
    {
    const AfmFontInfo *	afi= dsf->apfPsFontInfo;
    XftFont *		rval= (XftFont *)0;

    int			screen;

    const char *	fc_charset= (const char *)0;
    FcCharSet *		fcCharset= (FcCharSet *)0;
    int			typeCharset= FcTypeVoid;

    int			width= FONTwidthNORMAL;
    char		scratch[200+1];

    if  ( psRemoveWidthFromName( scratch, sizeof(scratch)- 1, &width,
						    afi->afiFamilyName ) < 0 )
	{ SDEB(afi->afiFamilyName); goto ready;	}

    if  ( dsf->dsfUnicodesUsed )
	{
	fcCharset= FcCharSetCreate();
	fc_charset= FC_CHARSET;
	typeCharset= FcTypeCharSet;

	/* IndexSet -> FcCharSet */
	utilIndexSetForAll( dsf->dsfUnicodesUsed,
				appFontAddOneCharToSet, (void *)fcCharset );
 	}

    if  ( ! dsf->dsfDisplay )
	{ XDEB(dsf->dsfDisplay); return (XftFont *)0;	}
    screen= DefaultScreen( dsf->dsfDisplay );

    /*  2  */
    rval= XftFontOpen( dsf->dsfDisplay, screen,
	    FC_FAMILY, FcTypeString,
			scratch,
	    FC_WIDTH, FcTypeInteger,
			afi->afiWidthInt,
	    FC_PIXEL_SIZE, FcTypeDouble,
			(double)dsf->dsfSizePixels,
	    FC_SLANT, FcTypeInteger,
			(FONT_IS_SLANTED(afi)?FC_SLANT_ITALIC:FC_SLANT_ROMAN),
	    FC_WEIGHT, FcTypeInteger,
			(FONT_IS_BOLD(afi)?FC_WEIGHT_BOLD:FC_WEIGHT_REGULAR),
	    fc_charset, typeCharset,
			fcCharset,
	    NULL );

ready:

    if  ( fcCharset )
	{ FcCharSetDestroy( fcCharset );	}

    return rval;
    }

#   endif				/*  }}	*/

#   ifdef USE_FONTCONFIG
#	undef FT_ERRORDEF_
#	undef FT_NOERRORDEF_
#	define FT_ERRORDEF_( d, n, s ) { n, s, },
#	define FT_NOERRORDEF_( d, n, s ) { n, s, },

static const char * appFtErrorStr( int e )
    {
    int			i;
    static char		buf[20];

    typedef struct n2s { int n; const char * s; } n2s;

    static const n2s xftn2s[]=
    {
#   include <freetype/fterrdef.h>
    };

    for ( i= 0; i < sizeof(xftn2s)/sizeof(n2s); i++ )
	{
	if  ( xftn2s[i].n == e )
	    { return xftn2s[i].s;	}
	}

    sprintf( buf, "0x%x", e );
    return buf;
    }

#   endif
