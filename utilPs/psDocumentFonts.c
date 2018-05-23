/************************************************************************/
/*									*/
/*  Print Postscript.							*/
/*									*/
/************************************************************************/

#   include	"utilPsConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<string.h>

#   include	"psPrint.h"
#   include	"psGlyphs.h"
#   include	<utilTree.h>
#   include	<geo2DInteger.h>
#   include	<utilFontmap.h>
#   include	<appSystem.h>
#   include	"psFace.h"
#   include	<sioFileio.h>
#   include	<sioPfb.h>
#   include	"psTtf.h"
#   include	<ucdGeneralCategory.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  List the font names that appear in a document.			*/
/*									*/
/************************************************************************/

static int utilPsGetEmbedFromFilename(	const MemoryBuffer *	fontFile )
    {
    int			rval= PSembedNO;

    MemoryBuffer	extension;

    utilInitMemoryBuffer( &extension );

    if  ( appFileGetFileExtension( &extension, fontFile ) )
	{ LDEB(1); goto ready;	}
    if  ( utilMemoryBufferIsEmpty( &extension ) )
	{ goto ready;	}

    if  ( utilMemoryBufferEqualsString( &extension, "pfb" ) 	||
	  utilMemoryBufferEqualsString( &extension, "PFB" ) 	)
	{ rval= PSembedBTOA; goto ready;	}

    if  ( utilMemoryBufferEqualsString( &extension, "pfa" ) 	||
	  utilMemoryBufferEqualsString( &extension, "PFA" ) 	)
	{ rval= PSembedCOPY; goto ready;	}

    if  ( utilMemoryBufferEqualsString( &extension, "ttf" ) 	||
	  utilMemoryBufferEqualsString( &extension, "TTF" ) 	)
	{ rval= PSembedTTFTO42; goto ready;	}

    if  ( utilMemoryBufferEqualsString( &extension, "ttc" ) 	||
	  utilMemoryBufferEqualsString( &extension, "TTC" ) 	)
	{ rval= PSembedTTCTO42; goto ready;	}

  ready:

    utilCleanMemoryBuffer( &extension );

    return rval;
    }

static int utilPsGetEmbed(		PostScriptFace *	psf,
					const MemoryBuffer *	fontDirectory )
    {
    int			res= 0;

    MemoryBuffer	absolute;
    MemoryBuffer	relative;

    utilInitMemoryBuffer( &absolute );
    utilInitMemoryBuffer( &relative );

    if  ( ! utilMemoryBufferIsEmpty( &(psf->psfAfi->afiFontFileName) ) )
	{
	if  ( appTestFileExists( &(psf->psfAfi->afiFontFileName) ) )
	    { res= PSembedNO; goto ready;	}

	res= utilPsGetEmbedFromFilename( &(psf->psfAfi->afiFontFileName) );
	if  ( res == PSembedNO )
	    { goto ready;	}

	if  ( utilCopyMemoryBuffer( &(psf->psfFontFileName),
					&(psf->psfAfi->afiFontFileName) ) )
	    { LDEB(1); res= PSembedNO; goto ready;	}
	psf->psfFontFileIndex= psf->psfAfi->afiFontFileIndex;

	psf->psfEmbed= res;
	}
    else{
	const char *	fontFile;

	const int	relativeIsFile= 0;
	int		l;

	fontFile= utilFontmapGetEntry( psf->psfAfi->afiFontName );
	if  ( ! fontFile )
	    { res= PSembedNO; goto ready;	}
	if  ( utilMemoryBufferSetString( &relative, fontFile ) )
	    { LDEB(1); res= PSembedNO; goto ready;	}

	l= appAbsoluteName( &absolute, &relative,
					    relativeIsFile, fontDirectory );
	if  ( l <= 0 )
	    { res= PSembedNO; goto ready;	}
	if  ( appTestFileExists( &absolute ) )
	    { res= PSembedNO; goto ready;	}

	res= utilPsGetEmbedFromFilename( &absolute );
	if  ( res == PSembedNO )
	    { goto ready;	}

	if  ( utilCopyMemoryBuffer( &(psf->psfFontFileName), &absolute ) )
	    { LDEB(1); res= PSembedNO; goto ready;	}
	psf->psfFontFileIndex= 0;

	psf->psfEmbed= res;
	}

  ready:

    utilCleanMemoryBuffer( &absolute );
    utilCleanMemoryBuffer( &relative );

    return res;
    }

/************************************************************************/
/*									*/
/*  Emit DSC comments about the fonts in the document.			*/
/*									*/
/*  1)  List the fonts that are used in the document.			*/
/*  2)  List the fonts that are embedded in the document.		*/
/*  3)  List 1-2: The fonts that are still needed.			*/
/*									*/
/************************************************************************/

void psDscListFontNames(	SimpleOutputStream *		sos,
				int				embedFonts,
				const PostScriptTypeList *	pstl )
    {
    PostScriptFace *	psf;
    const char *	fontName;
    int			done;

    if  ( ! pstl->pstlFaceTree )
	{ return;	}

    done= 0;
    psf= (PostScriptFace *)utilTreeGetFirst( pstl->pstlFaceTree, &fontName );
    /*  1  */
    while( psf )
	{
	if  ( done > 0 )
	    {
	    sioOutPrintf( sos, "%%%%+ %s\n", psf->psfAfi->afiFontName );
	    }
	else{
	    sioOutPrintf( sos, "%%%%DocumentFonts: %s\n",
					    psf->psfAfi->afiFontName );
	    }

	done++;
	psf= (PostScriptFace *)utilTreeGetNext( pstl->pstlFaceTree, &fontName );
	}

    /*  2  */
    if  ( embedFonts )
	{
	done= 0;
	psf= (PostScriptFace *)utilTreeGetFirst( pstl->pstlFaceTree,
								&fontName );
	while( psf )
	    {
	    if  ( psf->psfEmbed == PSembedUNKNOWN )
		{ utilPsGetEmbed( psf, &(pstl->pstlFontDirectory) );	}

	    if  ( psf->psfEmbed > 0 )
		{
		if  ( done > 0 )
		    {
		    sioOutPrintf( sos, "%%%%+ %s\n", psf->psfAfi->afiFontName );
		    }
		else{
		    sioOutPrintf( sos, "%%%%DocumentSuppliedFonts: %s\n",
						    psf->psfAfi->afiFontName );
		    }

		done++;
		}

	    psf= (PostScriptFace *)utilTreeGetNext( pstl->pstlFaceTree,
								&fontName );
	    }
	}

    /*  3  */
    done= 0;
    psf= (PostScriptFace *)utilTreeGetFirst( pstl->pstlFaceTree, &fontName );
    while( psf )
	{
	if  ( ! embedFonts || psf->psfEmbed <= 0 )
	    {
	    if  ( done > 0 )
		{
		sioOutPrintf( sos, "%%%%+ %s\n", psf->psfAfi->afiFontName );
		}
	    else{
		sioOutPrintf( sos, "%%%%DocumentNeededFonts: %s\n",
						psf->psfAfi->afiFontName );
		}

	    done++;
	    }

	psf= (PostScriptFace *)utilTreeGetNext( pstl->pstlFaceTree, &fontName );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Make function names for setting the fonts in a list.		*/
/*									*/
/*  1)  Define all standard encodings.					*/
/*  2)  Define all encoded fonts.					*/
/*									*/
/************************************************************************/

static void psDefineEncodedFont(
				SimpleOutputStream *	sos,
				const AfmFontInfo *	afi,
				const char *		encodedFontName,
				const char *		encodingArrayName )
    {
#   if 0
    sioOutPrintf( sos, "/%s findfont dup length dict begin\n",
							afi->afiFontName );
    sioOutPrintf( sos, "  {\n" );
    sioOutPrintf( sos, "    1 index /FID ne\n" );
    sioOutPrintf( sos, "      { def } { pop pop } ifelse\n" );
    sioOutPrintf( sos, "  } forall\n");
    sioOutPrintf( sos, "  /Encoding %s def currentdict\n", encodingArrayName );
    sioOutPrintf( sos, "end " );

    sioOutPrintf( sos, "/%s exch definefont pop\n\n", encodedFontName );
#   else
    sioOutPrintf( sos, "/%s %s /%s dcpf\n",
		    encodedFontName, encodingArrayName, afi->afiFontName );
#   endif
    return;
    }

int psIncludeFonts(	SimpleOutputStream *		sos,
			const PostScriptTypeList *	pstl )
    {
    int				rval= 0;
    const char *		fontName;
    PostScriptFace *		psf;

    SimpleInputStream *	sisFile= (SimpleInputStream *)0;
    SimpleInputStream *	sisPfb= (SimpleInputStream *)0;
    SimpleInputStream *	sis= (SimpleInputStream *)0;

    psf= (PostScriptFace *)utilTreeGetFirst( pstl->pstlFaceTree, &fontName );
    while( psf )
	{
	if  ( psf->psfEmbed == PSembedUNKNOWN )
	    { utilPsGetEmbed( psf, &(pstl->pstlFontDirectory) );	}

	if  ( psf->psfEmbed > 0 )
	    {
	    const AfmFontInfo *	afi= psf->psfAfi;

	    sisFile= sioInFileioOpen( &(psf->psfFontFileName) );
	    if  ( ! sisFile )
		{ XDEB(sisFile); rval= -1; goto ready;	}
	    sis= sisFile;

	    sioOutPrintf( sos, "\n" );
	    sioOutPrintf( sos, "%%###{   F_%d:\n", afi->afiFaceNumber );
	    sioOutPrintf( sos, "%%###    %s\n", afi->afiFullName );
	    sioOutPrintf( sos, "%%### :: %s\n",
			utilMemoryBufferGetString( &(psf->psfFontFileName) ) );
	    sioOutPrintf( sos, "%%###\n" );

	    sioOutPrintf( sos, "%%%%BeginFont: %s\n",
					    afi->afiFontName );

	    switch( psf->psfEmbed )
		{
		int		c;
		int		atBol;

		case PSembedBTOA:
		    sisPfb= sioInPfbOpen( sisFile );
		    if  ( ! sisPfb )
			{ XDEB(sisPfb); rval= -1; goto ready; }

		    sis= sisPfb;

		    /*FALLTHROUGH*/
		case PSembedCOPY:
		    atBol= 1;
		    while( ( c= sioInGetByte( sis ) ) != EOF )
			{
			if  ( sioOutPutByte( c, sos ) < 0 )
			    { rval= -1; goto ready;	}
			atBol= c == '\n';
			}

		    if  ( ! atBol )
			{
			if  ( sioOutPutByte( '\n', sos ) < 0 )
			    { rval= -1; goto ready;	}
			}

		    break;

		case PSembedTTFTO42:
		    if  ( psTtfToPt42( sos, &(psf->psfFontFileName), sisFile ) )
			{ LDEB(1); rval= -1; goto ready; }
		    break;

		case PSembedTTCTO42:
		    if  ( psTtcToPt42( sos,
				    &(psf->psfFontFileName),
				    psf->psfFontFileIndex,
				    sisFile ) )
			{ LDEB(1); rval= -1; goto ready; }
		    break;

		default:
		    LDEB(psf->psfEmbed);
		    rval= -1; goto ready;
		}

	    sioOutPrintf( sos, "%%%%EndFont %s\n", afi->afiFontName );
	    sioOutPrintf( sos, "%%###}   F_%d:\n", afi->afiFaceNumber );

	    if  ( sisPfb )
		{ sioInClose( sisPfb ); sisPfb= (SimpleInputStream *)0;	}
	    if  ( sisFile )
		{ sioInClose( sisFile ); sisFile= (SimpleInputStream *)0; }
	    }

	psf= (PostScriptFace *)utilTreeGetNext( pstl->pstlFaceTree, &fontName );
	}

  ready:

    if  ( sisPfb )
	{ sioInClose( sisPfb );	}
    if  ( sisFile )
	{ sioInClose( sisFile );	}

    return rval;
    }

/************************************************************************/

static void psSetStdEncodingArrayName(	char *	encodingArrayName,
					int	page )
    {
    sprintf( encodingArrayName, "CP_%02X", page );
    return;
    }

static void psSetFaceEncodingArrayName(	char *	encodingArrayName,
					int	face,
					int	page )
    {
    sprintf( encodingArrayName, "FCP_%d_%02X", face, page );
    return;
    }

/************************************************************************/
/*									*/
/*  Emit a standard font encoding page: Mapping all positions to their	*/
/*  respective standard unicode names.					*/
/*									*/
/*  1)  Store an array with the desired name in the current dictionary.	*/
/*  2)  Store '.notdef' in all positions of the array.			*/
/*  3)  Store glyph names in the array.					*/
/*									*/
/************************************************************************/

static void psEmitStandardFontEncodingPage(	SimpleOutputStream *	sos,
						int			page )
    {
    int			i;
    char		name[20];

    psSetStdEncodingArrayName( name, page );

    sioOutPrintf( sos, "sstdcp %% %s {\n", name );

    /*  3  */
    for ( i= 0; i < UPP; i++ )
	{
	int	code= page* UPP+ i;

	if  ( ! ucdIsCn( code ) )
	    {
	    sioOutPrintf( sos, "  dup %3d /%s put\n",
					i, psUnicodeToGlyphName( code ) );
	    }
	}

    sioOutPrintf( sos, "readonly /%s exch def %% }\n\n", name );
    }

/************************************************************************/
/*									*/
/*  Overwrite the non-standard glyphs in a page of the font encoding.	*/
/*									*/
/************************************************************************/

static void psListNonStdPage(	SimpleOutputStream *		sos,
				const AfmFontInfo *		afi,
				int				page )
    {
    int		i;

    for ( i= 0; i < UPP; i++ )
	{
	int		code= page* UPP+ i;

	if  ( ! ucdIsCn( code ) )
	    {
	    int				glyph;

	    glyph= utilIndexMappingGet( &(afi->afiUnicodeToGlyphMapping), code );
	    if  ( glyph < 0 )
		{ /*LDEB(glyph);*/	}
	    else{
		const AfmCharMetric *	acm= afi->afiMetrics[glyph];
		const char *		stdName= psUnicodeToGlyphName( code );

		if  ( acm && acm->acmN && acm->acmN[0]	&&
		      strcmp( stdName, acm->acmN )	)
		    {
		    sioOutPrintf( sos, "  dup %d /%s put\n", i, acm->acmN );
		    }
		}
	    }
	}
    }

static void psSetEncodedFontName(	char *			encodedFontName,
					const PostScriptFace *	psf,
					int			page )
    {
    const AfmFontInfo *		afi= psf->psfAfi;

    sprintf( encodedFontName, "F_%d_%02X", afi->afiFaceNumber, page );
    }

void psSetFontName(		char *			fontName,
				const AfmFontInfo *	afi )
    { sprintf( fontName, "F_%d", afi->afiFaceNumber ); }

static void psListFontEncodingPages(
				SimpleOutputStream *		sos,
				const PostScriptFace *		psf )
    {
    const AfmFontInfo *		afi= psf->psfAfi;
    int				page;
    int				done= 0;

    for ( page= 0; page < UNPAGE; page++ )
	{
	char		stdEncodingArrayName[20];
	char		faceEncodingArrayName[20];

	psSetStdEncodingArrayName( stdEncodingArrayName, page );
	psSetStdEncodingArrayName( faceEncodingArrayName, page );

	if  ( psf->psfPageUsed[page] )
	    {
	    char		encodedFontName[20];

	    psSetEncodedFontName( encodedFontName, psf, page );

	    if  ( psf->psfPageNonStd[page] )
		{
		psSetFaceEncodingArrayName( faceEncodingArrayName,
						afi->afiFaceNumber, page );

		sioOutPrintf( sos, "\n/%s %s 256 array copy\n",
						    faceEncodingArrayName,
						    stdEncodingArrayName );

		psListNonStdPage( sos, afi, page );

		sioOutPrintf( sos, "readonly def\n\n" );
		}

	    psDefineEncodedFont( sos, afi,
				    encodedFontName, faceEncodingArrayName );
	    done++;
	    }
	}

    sioOutPrintf( sos, "\n" );
    }

void psSelectFontProcedures(	SimpleOutputStream *		sos,
				const PostScriptTypeList *	pstl,
				int				allFonts )
    {
    const char *		fontName;
    PostScriptFace *		psf;
    int				page;

    int				faceNumber= 1;

    for ( page= 0; page < UNPAGE; page++ )
	{
	if  ( pstl->pstlPageUsed[page] )
	    { psEmitStandardFontEncodingPage( sos, page );	}
	}

    /*  2  */
    psf= (PostScriptFace *)utilTreeGetFirst( pstl->pstlFaceTree, &fontName );
    while( psf )
	{
	if  ( psf->psfAppearsInText || allFonts )
	    {
	    char		compositeName[20];
	    const AfmFontInfo *	afi= psf->psfAfi;
	    int			childCount;

	    int			encoding[UNPAGE];

	    for ( page= 0; page < UNPAGE; page++ )
		{ encoding[page]= 0;	}

	    sprintf( compositeName, "CF_%d", afi->afiFaceNumber );

	    sioOutPrintf( sos, "\n" );
	    sioOutPrintf( sos, "%%###    %s:\n", compositeName );
	    sioOutPrintf( sos, "%%###    %s\n", afi->afiFullName );
	    sioOutPrintf( sos, "%%### -> %s\n", afi->afiFontName );
	    sioOutPrintf( sos, "%%###\n\n" );

	    psListFontEncodingPages( sos, psf );

	    sioOutPrintf( sos, "/%s dup <<\n", compositeName );
	    sioOutPrintf( sos, "  /FontType 0\n" );
	    sioOutPrintf( sos, "  /FMapType 2\n" ); /* 8/8 */
	    sioOutPrintf( sos, "  /FontMatrix [ 1 0 0 1 0 0 ]\n" );
	    sioOutPrintf( sos, "  /FDepVector [\n" );

	    childCount= 0;
	    for ( page= 0; page < UNPAGE; page++ )
		{
		if  ( psf->psfPageUsed[page] )
		    {
		    char	encodedFontName[20];

		    psSetEncodedFontName( encodedFontName, psf, page );
		    sioOutPrintf( sos, "    /%s findfont %% %d\n",
					    encodedFontName, childCount );
		    encoding[page]= childCount++;
		    }
		}

	    sioOutPrintf( sos, "  ]\n" );
	    sioOutPrintf( sos, "  /Encoding [" );

	    sioOutPrintf( sos, "\n%%  " );
	    for ( page= 0; page < 16; page++ )
		{ sioOutPrintf( sos, "  +%X", page );	}

	    for ( page= 0; page < UNPAGE; page++ )
		{
		if  ( page % 16 == 0 )
		    {
		    if  ( page > 0 )
			{ sioOutPrintf( sos, "   %% %X+..", (page- 1)/16 ); }

		    sioOutPrintf( sos, "\n   " );
		    }

		sioOutPrintf( sos, "%4d", encoding[page] );
		}

	    sioOutPrintf( sos, " ] %% %X+..\n", (page- 1)/16  );
	    sioOutPrintf( sos, ">> definefont\n" );

	    sioOutPrintf( sos, "/F_%d\t{\n", afi->afiFaceNumber );
	    sioOutPrintf( sos, "  6 array identmatrix\n" );
	    sioOutPrintf( sos, "  dup 0 3 index put\n" );
	    sioOutPrintf( sos, "  dup 3 3 index neg put\n" );
	    sioOutPrintf( sos, "  /%s exch selectfont\n", compositeName );
	    sioOutPrintf( sos, "  } bind def\n" );
	    }

	psf= (PostScriptFace *)utilTreeGetNext( pstl->pstlFaceTree, &fontName );
	faceNumber++;
	}

    sioOutPrintf( sos, "\n" );

    return;
    }

