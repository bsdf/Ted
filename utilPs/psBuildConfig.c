/************************************************************************/
/*									*/
/*  Utility routines to build font configuration.			*/
/*									*/
/************************************************************************/

#   include	"utilPsConfig.h"

#   include	<stdio.h>
#   include	<stdlib.h>
#   include	<string.h>
#   include	<ctype.h>

#   include	<locale.h>

#   include	<reg.h>

#   include	<sioFileio.h>
#   include	<sioPfb.h>
#   include	<sioPipe.h>
#   include	<appSystem.h>
#   include	<utilMemoryBuffer.h>
#   include	<utilMemoryBufferPrintf.h>
#   include	"psTtf.h"
#   include	"psBuildConfigFiles.h"
#   include	"psReadWriteFontInfo.h"

#   include	<appDebugon.h>

#   ifdef __VMS
    static const char * const		afmExtension= "AFM";
    static const char * const		pfbExtension= "PFB";
    static const char * const		pfaExtension= "PFA";
#   else
    static const char * const		afmExtension= "afm";
    static const char * const		pfbExtension= "pfb";
    static const char * const		pfaExtension= "pfa";
#   endif
    static const char * const		ttfExtension= "ttf";
    static const char * const		TTFExtension= "TTF";

static const char * PS_FontNamePattern=
			"^/FontName[ \t]*/([^ \t]*)[ \t][ \t]*def";

static const char * PS_LocalAfmDir= "localfonts";

/************************************************************************/

typedef struct AfmDirBuilder
    {
    MemoryBuffer		adbAfmDirectory;
    MemoryBuffer		adbAfmCommand;
    MemoryBuffer		adbCwd;
    regProg *			adbFontNameProg;
				/**
				 *  Owned by the caller. To be 
				 *  augmented here.
				 */
    PostScriptFontList *	adbPostScriptFontList;
    int				adbAfmFlags;
    } AfmDirBuilder;

static void psInitAfmDirBuilder(	AfmDirBuilder *	adb )
    {
    utilInitMemoryBuffer( &(adb->adbAfmDirectory) );
    utilInitMemoryBuffer( &(adb->adbAfmCommand) );
    utilInitMemoryBuffer( &(adb->adbCwd) );
    adb->adbFontNameProg= (regProg *)0;
    adb->adbPostScriptFontList= (PostScriptFontList *)0;
    adb->adbAfmFlags= 0;
    }

static void psCleanAfmDirBuilder(	AfmDirBuilder *	adb )
    {
    utilCleanMemoryBuffer( &(adb->adbAfmDirectory) );
    utilCleanMemoryBuffer( &(adb->adbAfmCommand) );
    utilCleanMemoryBuffer( &(adb->adbCwd) );

    if  ( adb->adbFontNameProg )
	{ regFree( adb->adbFontNameProg );	}
    }

static int psStartAfmDirBuilder(	AfmDirBuilder *		adb,
					PostScriptFontList *	psfl,
					int			ignoreKerning,
					const MemoryBuffer *	afmDirectory,
					const MemoryBuffer *	psDirectory )
    {
    int			rval= 0;
    const int		relativeIsFile= 0;
    const int		options= 0;

    const char *	afmBase= "gs -sthefont=%s -q %s/gsfontmetrics.ps -c quit";

    MemoryBuffer	local;

    utilInitMemoryBuffer( &local );

    adb->adbPostScriptFontList= psfl;

    if  ( ignoreKerning )
	{ adb->adbAfmFlags |= PSflagIGNORE_KERNING;	}

    if  ( utilMemoryBufferSetString( &local, PS_LocalAfmDir ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( afmDirectory )
	{
	if  ( appAbsoluteName( &(adb->adbAfmDirectory),
				&local, relativeIsFile, afmDirectory ) < 0 )
	    { SDEB(PS_LocalAfmDir); rval= -1; goto ready;	}
	}

    adb->adbFontNameProg= regCompile( PS_FontNamePattern, options );
    if  ( ! adb->adbFontNameProg )
	{ XDEB(adb->adbFontNameProg); rval= -1; goto ready;	}

    if  ( psDirectory )
	{
	utilMemoryBufferPrintf( &(adb->adbAfmCommand), afmBase, "%s",
				utilMemoryBufferGetString( psDirectory ) );
	}

    if  ( appCurrentDirectory( &(adb->adbCwd) ) < 0 )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    utilCleanMemoryBuffer( &local );

    return rval;
    }

static int psGetNameExt(	MemoryBuffer *		file,
				MemoryBuffer *		extension,
				const AfmDirBuilder *	adb,
				const char *		fontFileName )
    {
    int				rval= 0;
    MemoryBuffer		filename;

    utilInitMemoryBuffer( &filename );

    if  ( utilMemoryBufferSetString( &filename, fontFileName ) )
	{ SDEB(fontFileName); rval= -1; goto ready;	}

    if  ( appFileGetFileExtension( extension, &filename ) < 0 )
	{ LDEB(1); rval= -1; goto ready;	}
    if  ( utilMemoryBufferIsEmpty( extension ) )
	{ rval= -1; rval= 1; goto ready;	}

    if  ( ! appFileNameIsAbsolute( fontFileName ) )
	{
	int		l;
	const int	relativeIsFile= 0;

	l= appAbsoluteName( file, &filename, relativeIsFile, &(adb->adbCwd) );
	if  ( l < 0 )
	    { LDEB(l); rval= -1; rval= 1; goto ready;	}
	}
    else{
	if  ( utilCopyMemoryBuffer( file, &filename ) )
	    { LDEB(1);	}
	}

  ready:

    utilCleanMemoryBuffer( &filename );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Find the font name for a Postcript type 1 font.			*/
/*									*/
/************************************************************************/

static int psFontNameFromPf(	MemoryBuffer *		fontname,
				SimpleInputStream *	sisFont,
				regProg *		prog )
    {
    int			rval= 1;
    char		scratch[550+1];

    while( sioInGetString( scratch, sizeof(scratch)-1, sisFont ) )
	{
	ExpressionMatch		em;

	scratch[sizeof(scratch)-1]= '\0';

	if  ( regFindLeftToRight( &em, prog, scratch, 0, strlen( scratch ) ) )
	    {
	    int		from;
	    int		past;

	    regGetMatch( &from, &past, &em, 0 );

	    if  ( utilMemoryBufferSetBytes( fontname,
				(unsigned char *)scratch+ from, past- from ) )
		{ LLDEB(past,from); rval= -1; break;	}

	    rval= 0; break;
	    }
	}

    if  ( rval )
	{ LDEB(rval);	}

    return rval;
    }

static AfmFontInfo * psFillAfiForPf(
				SimpleInputStream *	sisFont,
				const AfmDirBuilder *	adb,
				const MemoryBuffer *	filename )
    {
    int			rval= 0;

    MemoryBuffer	fontname;
    MemoryBuffer	command;

    SimpleInputStream *	sisCmd= (SimpleInputStream *)0;
    AfmFontInfo *	afi= (AfmFontInfo *)0;

    utilInitMemoryBuffer( &fontname );
    utilInitMemoryBuffer( &command );

    afi= (AfmFontInfo *)malloc( sizeof(AfmFontInfo) );
    if  ( ! afi )
	{ XDEB(afi); rval= -1; goto ready;	}
    psInitAfmFontInfo( afi );

    if  ( psFontNameFromPf( &fontname, sisFont, adb->adbFontNameProg ) )
	{ LDEB(1); rval= -1; goto ready;	}

    utilMemoryBufferPrintf( &command,
		    utilMemoryBufferGetString( &(adb->adbAfmCommand) ),
		    utilMemoryBufferGetString( &fontname ) );

    sisCmd= sioInPipeOpen( &command );
    if  ( ! sisCmd )
	{
	SXDEB(utilMemoryBufferGetString(&command),sisCmd);
	rval= -1; goto ready;
	}

    rval= psAfmReadAfm( sisCmd, afi, adb->adbAfmFlags );
    if  ( rval  )
	{
	SLDEB(utilMemoryBufferGetString(&command),rval); rval= -1;
	rval= -1; goto ready;
	}

    if  ( utilCopyMemoryBuffer( &(afi->afiFontFileName), filename ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( psGetUnicodesFromGlyphNames( afi ) )
	{ SDEB(afi->afiFullName);	}
    if  ( psGetAlternateGlyphs( afi ) )
	{ SDEB(afi->afiFullName);	}
    if  ( psResolveFallbackGlyph( afi ) )
	{ SDEB(afi->afiFullName);	}

  ready:

    if  ( rval && afi )
	{
	psFreeAfmFontInfo( afi );
	afi= (AfmFontInfo *)0;
	}

    utilCleanMemoryBuffer( &fontname );
    utilCleanMemoryBuffer( &command );

    if  ( sisCmd )
	{ sioInClose( sisCmd );	}

    return afi;
    }

static int psFontNameFromPfa(	MemoryBuffer *		fontname,
				regProg *		prog,
				const MemoryBuffer *	filename )
    {
    int			rval= 0;
    SimpleInputStream *	sisFile= (SimpleInputStream *)0;

    sisFile= sioInFileioOpen( filename );
    if  ( ! sisFile )
	{ XDEB(sisFile); rval= -1; goto ready;	}

    rval= psFontNameFromPf( fontname, sisFile, prog );
    if  ( rval )
	{ LDEB(rval);	}

  ready:

    if  ( sisFile )
	{ sioInClose( sisFile );	}

    return rval;
    }

static AfmFontInfo * psFillAfiForPfa(	const AfmDirBuilder *	adb,
					const MemoryBuffer *	filename )
    {
    AfmFontInfo *	afi= (AfmFontInfo *)0;
    SimpleInputStream *	sisFile= (SimpleInputStream *)0;

    sisFile= sioInFileioOpen( filename );
    if  ( ! sisFile )
	{ XDEB(sisFile); goto ready;	}

    afi= psFillAfiForPf( sisFile, adb, filename );
    if  ( ! afi )
	{ XDEB(afi);	}

  ready:

    if  ( sisFile )
	{ sioInClose( sisFile );	}

    return afi;
    }

static int psFontNameFromPfb(	MemoryBuffer *		fontname,
				regProg *		prog,
				const MemoryBuffer *	filename )
    {
    int			rval= 0;
    SimpleInputStream *	sisFile= (SimpleInputStream *)0;
    SimpleInputStream *	sisFont= (SimpleInputStream *)0;

    sisFile= sioInFileioOpen( filename );
    if  ( ! sisFile )
	{ XDEB(sisFile); goto ready;	}

    sisFont= sioInPfbOpen( sisFile );
    if  ( ! sisFont )
	{ XDEB(sisFont); goto ready;	}

    rval= psFontNameFromPf( fontname, sisFont, prog );
    if  ( rval )
	{ LDEB(rval);	}

  ready:

    if  ( sisFont )
	{ sioInClose( sisFont );	}
    if  ( sisFile )
	{ sioInClose( sisFile );	}

    return rval;
    }

static AfmFontInfo * psFillAfiForPfb(	const AfmDirBuilder *	adb,
					const MemoryBuffer *	filename )
    {
    AfmFontInfo *	afi= (AfmFontInfo *)0;
    SimpleInputStream *	sisFile= (SimpleInputStream *)0;
    SimpleInputStream *	sisFont= (SimpleInputStream *)0;

    sisFile= sioInFileioOpen( filename );
    if  ( ! sisFile )
	{ XDEB(sisFile); goto ready;	}

    sisFont= sioInPfbOpen( sisFile );
    if  ( ! sisFont )
	{ XDEB(sisFont); goto ready;	}

    afi= psFillAfiForPf( sisFont, adb, filename );
    if  ( ! afi )
	{ XDEB(afi);	}

  ready:
    if  ( sisFont )
	{ sioInClose( sisFont );	}
    if  ( sisFile )
	{ sioInClose( sisFile );	}

    return afi;
    }

/************************************************************************/
/*									*/
/*  Find the font name for a TTF file. This is an expensive way. That	*/
/*  is not a problem as we rarely do this.				*/
/*									*/
/************************************************************************/

static AfmFontInfo * psFillAfiForTtf(	const MemoryBuffer *	filename )
    {
    int			rval= 0;
    AfmFontInfo *	afi= (AfmFontInfo *)0;
    SimpleInputStream *	sisFile= (SimpleInputStream *)0;

    afi= (AfmFontInfo *)malloc( sizeof(AfmFontInfo) );
    if  ( ! afi )
	{ XDEB(afi); rval= -1; goto ready;	}
    psInitAfmFontInfo( afi );

    sisFile= sioInFileioOpen( filename );
    if  ( ! sisFile )
	{ XDEB(sisFile); rval= -1; goto ready;	}

    if  ( psTtfToAfi( afi, filename, sisFile ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    if  ( rval && afi )
	{
	psFreeAfmFontInfo( afi );
	afi= (AfmFontInfo *)0;
	}

    if  ( sisFile )
	{ sioInClose( sisFile );	}

    return afi;
    }

static AfmFontInfo * psFillAfiForAfm(	int			afmFlags,
					const MemoryBuffer *	filename )
    {
    int			rval= 0;
    AfmFontInfo *	afi= (AfmFontInfo *)0;
    SimpleInputStream *	sisAfm= (SimpleInputStream *)0;

    afi= (AfmFontInfo *)malloc( sizeof(AfmFontInfo) );
    if  ( ! afi )
	{ XDEB(afi); rval= -1; goto ready;	}
    psInitAfmFontInfo( afi );

    sisAfm= sioInFileioOpen( filename );
    if  ( ! sisAfm )
	{ XDEB(sisAfm); rval= -1; goto ready;	}

    if  ( psAfmReadAfm( sisAfm, afi, afmFlags ) )
	{ SDEB(utilMemoryBufferGetString(filename)); rval= -1; goto ready; }

  ready:

    if  ( rval && afi )
	{
	psFreeAfmFontInfo( afi );
	afi= (AfmFontInfo *)0;
	}

    if  ( sisAfm )
	{ sioInClose( sisAfm );	}

    return afi;
    }

static int psFontNameFromTtf(	MemoryBuffer *		fontname,
				const MemoryBuffer *	filename )
    {
    int			rval= 0;
    AfmFontInfo *	afi= (AfmFontInfo*)0;

    afi= psFillAfiForTtf( filename );
    if  ( ! afi )
	{ XDEB(afi); goto ready;	}

    if  ( utilMemoryBufferSetString( fontname, afi->afiFontName ) )
	{ LDEB(1); rval= -1;	}

  ready:

    if  ( afi )
	{ psFreeAfmFontInfo( afi );	}

    return rval;
    }

static int psFontNameFromAfm(	MemoryBuffer *		fontname,
				MemoryBuffer *		filename )
    {
    int			rval= 0;
    AfmFontInfo *	afi= (AfmFontInfo*)0;

    const int		afmFlags= PSflagDEFER_METRICS | PSflagIGNORE_KERNING;

    afi= psFillAfiForAfm( afmFlags, filename );
    if  ( ! afi )
	{ SDEB(utilMemoryBufferGetString(filename)); rval= -1; goto ready; }

    if  ( utilMemoryBufferSetString( fontname, afi->afiFontName ) )
	{ LDEB(1); rval= -1;	}

    if  ( utilCopyMemoryBuffer( filename, &(afi->afiFontFileName) ) )
	{ LDEB(1); rval= -1;	}

  ready:

    if  ( afi )
	{ psFreeAfmFontInfo( afi );	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Scan a list of files and issue GhostScript compatible Fontmap lines	*/
/*  that can be added to a GhostScript Fontmap file. The purpose of	*/
/*  this function is to make the fonts known to GhostScript. This means	*/
/*  that we cannot use GhostScript yet.					*/
/*									*/
/************************************************************************/

int psFontmapForFiles(		SimpleOutputStream *	sosOut,
				int			fileCount,
				const char * const *	fileNames )
    {
    int			rval= 0;
    int			f;

    const int		ignoreKerning= 1;

    AfmDirBuilder	adb;
    MemoryBuffer	file;
    MemoryBuffer	extension;
    MemoryBuffer	fontname;

    utilInitMemoryBuffer( &file );
    utilInitMemoryBuffer( &extension );
    utilInitMemoryBuffer( &fontname );

    psInitAfmDirBuilder( &adb );

    if  ( psStartAfmDirBuilder( &adb, (PostScriptFontList *)0, ignoreKerning,
						(const MemoryBuffer *)0,
						(const MemoryBuffer *)0 ) )
	{ LDEB(1); rval= -1; goto ready;	}

    for ( f= 0; f < fileCount; f++ )
	{
	int	ret;

	utilEmptyMemoryBuffer( &fontname );

	ret= psGetNameExt( &file, &extension, &adb, fileNames[f] );
	if  ( ret )
	    { LDEB(ret); rval= -1; goto ready;	}

	if  ( appFileGetFileExtension( &extension, &file ) < 0 )
	    { LDEB(1); rval= -1; goto ready;	}
	if  ( utilMemoryBufferIsEmpty( &extension ) )
	    { SDEB(fileNames[f]); rval= -1; continue;	}

	if  ( utilMemoryBufferEqualsString( &extension, pfaExtension ) )
	    {
	    if  ( psFontNameFromPfa( &fontname, adb.adbFontNameProg, &file ) )
		{ SDEB(fileNames[f]); rval= -1;	}
	    }

	if  ( utilMemoryBufferEqualsString( &extension, pfbExtension ) )
	    {
	    if  ( psFontNameFromPfb( &fontname, adb.adbFontNameProg, &file ) )
		{ SDEB(fileNames[f]); rval= -1;	}
	    }

	if  ( utilMemoryBufferEqualsString( &extension, afmExtension ) )
	    {
	    if  ( psFontNameFromAfm( &fontname, &file ) )
		{ SDEB(fileNames[f]); rval= -1;	}
	    }

	if  ( utilMemoryBufferEqualsString( &extension, ttfExtension ) ||
	      utilMemoryBufferEqualsString( &extension, TTFExtension ) )
	    {
	    if  ( psFontNameFromTtf( &fontname, &file ) )
		{ SDEB(fileNames[f]); rval= -1;	}
	    }

	/*  Note that the afm reader potentially modifies the file name */

	if  ( ! utilMemoryBufferIsEmpty( &fontname )	&&
	      ! utilMemoryBufferIsEmpty( &file )	)
	    {
	    sioOutPrintf( sosOut, "/%s (%s) ;\n",
		    utilMemoryBufferGetString( &fontname ),
		    utilMemoryBufferGetString( &file ) );
	    }
	}

  ready:

    psCleanAfmDirBuilder( &adb );

    utilCleanMemoryBuffer( &file );
    utilCleanMemoryBuffer( &extension );
    utilCleanMemoryBuffer( &fontname );

    return rval;
    }

/************************************************************************/

static int psSaveAfmFile(	AfmFontInfo *		afi,
				AfmDirBuilder *		adb )
    {
    int				rval= 0;
    const int			relativeIsFile= 0;
    unsigned char *		s;

    MemoryBuffer		relative;

    utilInitMemoryBuffer( &relative );

    if  ( utilMemoryBufferSetString( &relative, afi->afiFontName ) )
	{ LDEB(1); rval= -1; goto ready;	}
    if  ( utilMemoryBufferAppendBytes( &relative,
				    (const unsigned char *)".afm", 4 ) )
	{ LDEB(4); rval= -1; goto ready;	}

    s= relative.mbBytes;
    while( *s )
	{
	if  ( isspace( *s ) )
	    { *s= '-';	}
	s++;
	}
    if  ( appAbsoluteName( &(afi->afiAfmFileName), &relative,
				relativeIsFile, &(adb->adbAfmDirectory) ) < 0 )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( psPostScriptFontListAddInfo( adb->adbPostScriptFontList, afi ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    utilCleanMemoryBuffer( &relative );

    return rval;
    }

static int psAfmFromPfaFile(	const MemoryBuffer *	filename,
				void *			through )
    {
    int			rval= 0;
    AfmDirBuilder *	adb= (AfmDirBuilder *)through;
    AfmFontInfo *	afi= (AfmFontInfo *)0;

    /* Broken symlinks */
    if  ( appTestFileExists( filename ) )
	{ goto ready;	}

    afi= psFillAfiForPfa( adb, filename );
    if  ( ! afi )
	{ XDEB(afi); goto ready;	} /* do not fail */

    if  ( psSaveAfmFile( afi, adb ) )
	{ LDEB(1); rval= -1; goto ready;	}

    afi= (AfmFontInfo *)0; /* steal */

  ready:

    if  ( afi )
	{ psFreeAfmFontInfo( afi );	}

    return rval;
    }

static int psAfmFromPfbFile(	const MemoryBuffer *	filename,
				void *			through )
    {
    int			rval= 0;
    AfmDirBuilder *	adb= (AfmDirBuilder *)through;
    AfmFontInfo *	afi= (AfmFontInfo *)0;

    /* Broken symlinks */
    if  ( appTestFileExists( filename ) )
	{ goto ready;	}

    afi= psFillAfiForPfb( adb, filename );
    if  ( ! afi )
	{ XDEB(afi); goto ready;	} /* do not fail */

    if  ( psSaveAfmFile( afi, adb ) )
	{ LDEB(1); rval= -1; goto ready;	}

    afi= (AfmFontInfo *)0; /* steal */

  ready:

    if  ( afi )
	{ psFreeAfmFontInfo( afi );	}

    return rval;
    }

static int psAfmFromTtfFile(	const MemoryBuffer *	filename,
				void *			through )
    {
    int			rval= 0;
    AfmDirBuilder *	adb= (AfmDirBuilder *)through;

    AfmFontInfo *	afi= (AfmFontInfo *)0;

    /* Broken symlinks */
    if  ( appTestFileExists( filename ) )
	{ goto ready;	}

    afi= psFillAfiForTtf( filename );
    if  ( ! afi )
	{ XDEB(afi); goto ready;	} /* do not fail */

    if  ( psSaveAfmFile( afi, adb ) )
	{ LDEB(1); rval= -1; goto ready;	}

    afi= (AfmFontInfo *)0;

  ready:

    if  ( afi )
	{ psFreeAfmFontInfo( afi );	}

    return rval;
    }

static int psAfmFromAfmFile(	const MemoryBuffer *	filename,
				void *			through )
    {
    int			rval= 0;
    AfmDirBuilder *	adb= (AfmDirBuilder *)through;

    AfmFontInfo *	afi= (AfmFontInfo *)0;

    /* Broken symlinks */
    if  ( appTestFileExists( filename ) )
	{ goto ready;	}

    afi= psFillAfiForAfm( adb->adbAfmFlags, filename );
    if  ( ! afi )
	{ LDEB(1); goto ready;	} /* do not fail */

    if  ( psSaveAfmFile( afi, adb ) )
	{ LDEB(1); rval= -1; goto ready;	}

    afi= (AfmFontInfo *)0;

  ready:

    if  ( afi )
	{ psFreeAfmFontInfo( afi );	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Build an afm directory: Include all fonts that are on the		*/
/*  GhostScript library path.						*/
/*									*/
/************************************************************************/

int psGSLibAfmDirectory(	PostScriptFontList *	psfl,
				int			ignoreKerning,
				const MemoryBuffer *	afmDirectory,
				const MemoryBuffer *	psDirectory )
    {
    int				rval= 0;
    SimpleInputStream *		sisCmd= (SimpleInputStream *)0;

    AfmDirBuilder		adb;
    MemoryBuffer		dir;
    MemoryBuffer		libCommand;

    char			scratch[500+1];

    const char *		libBase= "gs -q %s/gslibpath.ps -c quit";

    utilInitMemoryBuffer( &dir );
    utilInitMemoryBuffer( &libCommand );

    psInitAfmDirBuilder( &adb );

    if  ( psStartAfmDirBuilder( &adb, psfl, ignoreKerning,
					    afmDirectory, psDirectory ) )
	{ LDEB(1); rval= -1; goto ready;	}

    utilMemoryBufferPrintf( &libCommand, libBase,
				utilMemoryBufferGetString( psDirectory ) );

    sisCmd= sioInPipeOpen( &libCommand );
    if  ( ! sisCmd )
	{ SXDEB(libBase,sisCmd); rval= -1; goto ready;	}

    setlocale( LC_NUMERIC, "C" );

    while( sioInGetString( scratch, sizeof(scratch)- 1, sisCmd ) )
	{
	int		l;

	scratch[sizeof(scratch)- 1]= '\0';
	l= strlen( scratch );
	if  ( l < 2 )
	    { LDEB(l); rval= -1; goto ready;	}
	if  ( scratch[l-1] != '\n' )
	    { SDEB(scratch); rval= -1; goto ready;	}
	scratch[l-1]= '\0';

	if  ( utilMemoryBufferSetString( &dir, scratch ) )
	    { LDEB(1); rval= -1; goto ready;	}

	if  ( ! appFileNameIsAbsolute( scratch ) )
	    { continue;	}
	if  ( appTestDirectory( &dir ) )
	    { continue;	}

	l= appForAllFiles( &dir, pfaExtension, &adb, psAfmFromPfaFile );
	if  ( l )
	    { SDEB(scratch); rval= -1;	}
	l= appForAllFiles( &dir, pfbExtension, &adb, psAfmFromPfbFile );
	if  ( l )
	    { SDEB(scratch); rval= -1;	}
	l= appForAllFiles( &dir, ttfExtension, &adb, psAfmFromTtfFile );
	if  ( l )
	    { SDEB(scratch); rval= -1;	}
	l= appForAllFiles( &dir, TTFExtension, &adb, psAfmFromTtfFile );
	if  ( l )
	    { SDEB(scratch); rval= -1;	}
	}

  ready:

    setlocale( LC_NUMERIC, "" );

    if  ( sisCmd )
	{ sioInClose( sisCmd );	}

    psCleanAfmDirBuilder( &adb );

    utilCleanMemoryBuffer( &dir );
    utilCleanMemoryBuffer( &libCommand );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Build afm directory files for a list of font files.			*/
/*									*/
/************************************************************************/

int psAfmForFontFiles(	PostScriptFontList *	psfl,
			int			ignoreKerning,
			int			fileCount,
			char **			fileNames,
			const MemoryBuffer *	afmDirectory,
			const MemoryBuffer *	psDirectory )
    {
    int				rval= 0;
    int				f;


    AfmDirBuilder		adb;
    MemoryBuffer		file;
    MemoryBuffer		extension;

    utilInitMemoryBuffer( &file );
    utilInitMemoryBuffer( &extension );

    psInitAfmDirBuilder( &adb );

    if  ( psStartAfmDirBuilder( &adb, psfl, ignoreKerning,
						afmDirectory, psDirectory ) )
	{ LDEB(1); rval= -1; goto ready;	}

    setlocale( LC_NUMERIC, "C" );

    for ( f= 0; f < fileCount; f++ )
	{
	int	ret;

	ret= psGetNameExt( &file, &extension, &adb, fileNames[f] );
	if  ( ret < 0 )
	    { LDEB(ret); rval= -1; goto ready;	}
	if  ( ret > 0 )
	    { LDEB(ret); rval= -1; continue;	}

	if  ( utilMemoryBufferEqualsString( &extension, pfaExtension ) )
	    {
	    if  ( psAfmFromPfaFile( &file, &adb ) )
		{ SDEB(fileNames[f]); rval= -1;	}

	    continue;
	    }

	if  ( utilMemoryBufferEqualsString( &extension, pfbExtension ) )
	    {
	    if  ( psAfmFromPfbFile( &file, &adb ) )
		{ SDEB(fileNames[f]); rval= -1;	}

	    continue;
	    }

	if  ( utilMemoryBufferEqualsString( &extension, afmExtension ) )
	    {
	    if  ( psAfmFromAfmFile( &file, &adb ) )
		{ SDEB(fileNames[f]); rval= -1;	}

	    continue;
	    }

	if  ( utilMemoryBufferEqualsString( &extension, ttfExtension ) ||
	      utilMemoryBufferEqualsString( &extension, TTFExtension ) )
	    {
	    if  ( psAfmFromTtfFile( &file, &adb ) )
		{ SDEB(fileNames[f]); rval= -1;	}

	    continue;
	    }

	SDEB(fileNames[f]); rval= -1; continue;
	}

  ready:

    setlocale( LC_NUMERIC, "" );

    psCleanAfmDirBuilder( &adb );

    utilCleanMemoryBuffer( &file );
    utilCleanMemoryBuffer( &extension );

    return rval;
    }

int psAfmToGSFontmap(	SimpleOutputStream *	sosFontDir,
			const char *		afmFileName )
    {
    return psFontmapForFiles( sosFontDir, 1, &(afmFileName) );
    }

int psFontFileToAfm(	SimpleOutputStream *	sosAfm,
			int			omitKernPairs,
			const char *		fontFileName,
			const MemoryBuffer *	psDirectory )
    {
    int			rval= 0;
    int			ret;
    AfmFontInfo *	afi= (AfmFontInfo *)0;

    AfmDirBuilder	adb;
    MemoryBuffer	file;
    MemoryBuffer	extension;

    utilInitMemoryBuffer( &file );
    utilInitMemoryBuffer( &extension );

    psInitAfmDirBuilder( &adb );

    if  ( psStartAfmDirBuilder( &adb, (PostScriptFontList *)0, omitKernPairs,
				    (const MemoryBuffer *)0, psDirectory ) )
	{ LDEB(1); rval= -1; goto ready;	}

    setlocale( LC_NUMERIC, "C" );

    ret= psGetNameExt( &file, &extension, &adb, fontFileName );
    if  ( ret < 0 )
	{ LDEB(ret); rval= -1; goto ready;	}
    if  ( ret > 0 )
	{ LDEB(ret); rval= -1; goto ready;	}

    if  ( utilMemoryBufferEqualsString( &extension, pfaExtension ) )
	{
	afi= psFillAfiForPfa( &adb, &file );
	if  ( ! afi )
	    { SXDEB(fontFileName,afi); rval= -1; goto ready;	}
	}

    if  ( utilMemoryBufferEqualsString( &extension, pfbExtension ) )
	{
	afi= psFillAfiForPfb( &adb, &file );
	if  ( ! afi )
	    { SXDEB(fontFileName,afi); rval= -1; goto ready;	}
	}

    if  ( utilMemoryBufferEqualsString( &extension, afmExtension ) )
	{
	afi= psFillAfiForAfm( adb.adbAfmFlags, &file );
	if  ( ! afi )
	    { SXDEB(fontFileName,afi); rval= -1; goto ready;	}
	}

    if  ( utilMemoryBufferEqualsString( &extension, ttfExtension ) ||
	  utilMemoryBufferEqualsString( &extension, TTFExtension ) )
	{
	afi= psFillAfiForTtf( &file );
	if  ( ! afi )
	    { SXDEB(fontFileName,afi); rval= -1; goto ready;	}
	}

    if  ( ! afi )
	{ XDEB(afi); rval= -1; goto ready;	}

    if  ( psWriteAfmFile( sosAfm, adb.adbAfmFlags, afi ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    setlocale( LC_NUMERIC, "" );

    psCleanAfmDirBuilder( &adb );

    utilCleanMemoryBuffer( &file );
    utilCleanMemoryBuffer( &extension );

    if  ( afi )
	{ psFreeAfmFontInfo( afi );	}

    return rval;
    }
