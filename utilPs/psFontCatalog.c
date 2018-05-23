#   include	"utilPsConfig.h"

#   include	<stdio.h>
#   include	<ctype.h>
#   include	<stdlib.h>

#   include	<locale.h>

#   include	"psPostScriptFontList.h"
#   include	"psReadWriteFontInfo.h"
#   include	<sioFileio.h>
#   include	<appSystem.h>

#   include	<appDebugon.h>

#   ifdef __VMS
    static const char * const		afmExtension= "AFM";
#   else
    static const char * const		afmExtension= "afm";
#   endif

static const char * PS_LocalAfmDir= "localfonts";

typedef struct AfmCatalog
    {
    int				acAfmFlags;
    PostScriptFontList *	acFontList;
    } AfmCatalog;

/************************************************************************/
/*									*/
/*  Make a catalog of postscript fonts for the font chooser.		*/
/*									*/
/*  The code assumes that afmDirectory is some kind of a resource, that	*/
/*  does not vary over different calls to the routine.			*/
/*									*/
/*  A:  Consume one file.						*/
/*  B:  Ignore errors in certain hopeless files.			*/
/*									*/
/************************************************************************/

#   define	FILEL	400

/*  A  */
static int psRememberAfmFile(	const MemoryBuffer *	filename,
				const AfmCatalog *	ac )
    {
    int				rval= 0;
    SimpleInputStream *		sisAfm= (SimpleInputStream *)0;
    int				res;

    AfmFontInfo *		afi= (AfmFontInfo *)0;

    if  ( utilMemoryBufferIsEmpty( filename ) )
	{ LDEB(filename->mbSize); rval= -1; goto ready;	}

    afi= (AfmFontInfo *)malloc( sizeof(AfmFontInfo) );
    if  ( ! afi )
	{ XDEB(afi); rval= -1; goto ready;	}

    psInitAfmFontInfo( afi );

    sisAfm= sioInFileioOpen( filename );
    if  ( ! sisAfm )
	{
	SXDEB(utilMemoryBufferGetString(filename),sisAfm);
	goto ready; /* no failure ! */
	}

    res= psAfmReadAfm( sisAfm, afi, ac->acAfmFlags );
    if  ( res < 0 )
	{
	SXDEB(utilMemoryBufferGetString(filename),sisAfm);
	goto ready; /* no failure ! */
	}

    if  ( utilCopyMemoryBuffer( &(afi->afiAfmFileName), filename ) )
	{ LDEB(1); goto ready;	}

    if  ( psGetUnicodesFromGlyphNames( afi ) )
	{ SDEB(afi->afiFullName);	}
    if  ( psGetAlternateGlyphs( afi ) )
	{ SDEB(afi->afiFullName);	}
    if  ( psResolveFallbackGlyph( afi ) )
	{ SDEB(afi->afiFullName);	}

    /*  Free memory: metrics are reread when the font is really used */
    psRemoveMetricsFromInfo( afi );
    afi->afiMetricsDeferred= 1;
    afi->afiResolveMetrics= psGetDeferredMetrics;

    if  ( ! afi->afiFamilyName )
	{ XDEB(afi->afiFamilyName); goto ready; }

    /*
    if  ( adaptNames )
	{ psMapVisibleNamesToBase35( afi );	}
    */

    if  ( psPostScriptFontListAddInfo( ac->acFontList, afi ) )
	{ LDEB(1); rval= -1; goto ready;	}
    afi= (AfmFontInfo *)0; /* stolen */

  ready:

    if  ( afi )
	{ psFreeAfmFontInfo( afi );	}
    if  ( sisAfm )
	{ sioInClose( sisAfm );		}

    return rval;
    }

static int psGotAfmFile(	const MemoryBuffer *	filename,
				void *			through )
    {
    return psRememberAfmFile( filename, (AfmCatalog *)through );
    }

static int psGotLocalAfmFile(	const MemoryBuffer *	filename,
				void *			through )
    {
    return psRememberAfmFile( filename, (AfmCatalog *)through );
    }

/************************************************************************/

static int psLocalDirectoryName( MemoryBuffer *		localDirectory,
				const MemoryBuffer *	afmDirectory )
    {
    int				rval= 0;
    const int			relativeIsFile= 0;

    MemoryBuffer		local;

    utilInitMemoryBuffer( &local );

    if  ( utilMemoryBufferSetString( &local, PS_LocalAfmDir ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( appAbsoluteName( localDirectory,
			    &local, relativeIsFile, afmDirectory ) < 0 )
	{ SDEB(PS_LocalAfmDir); rval= -1; goto ready;	}

  ready:

    utilInitMemoryBuffer( &local );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Make a catalog of all the fonts for which an AFM file is found.	*/
/*									*/
/*  1)  Collect the information from all AFM files.			*/
/*	NOTE: Afm files are in the "C" locale.				*/
/*  2)  Count families.							*/
/*	NOTE: Width variants of the same family are treated like	*/
/*	different families here.					*/
/*  3)  Allocate space for the families.				*/
/*  4)  Divide the faces over the families.				*/
/*									*/
/************************************************************************/

int psFontCatalog(	PostScriptFontList *	psfl,
			int			ignoreKerning,
			const MemoryBuffer *	afmDirectory )
    {
    int				rval= 0;
    int				j;

    MemoryBuffer		localDirectory;

    AfmCatalog			ac;

    utilInitMemoryBuffer( &localDirectory );

    ac.acFontList= psfl;
    ac.acAfmFlags= PSflagDEFER_METRICS;
    if  ( ignoreKerning )
	{
	ac.acAfmFlags |= PSflagIGNORE_KERNING;
	}

    if  ( psLocalDirectoryName( &localDirectory, afmDirectory ) )
	{ LDEB(1); rval= -1; goto ready;	}

    /*  1  */
    setlocale( LC_NUMERIC, "C" );

    if  ( ! appTestDirectory( &localDirectory ) )
	{
	j= appForAllFiles( &localDirectory, afmExtension,
						&ac, psGotLocalAfmFile );
	if  ( j )
	    { LDEB(j);	}
	}

    j= appForAllFiles( afmDirectory, afmExtension, &ac, psGotAfmFile );

    setlocale( LC_NUMERIC, "" );

    if  ( j )
	{ LDEB(j); rval= -1; goto ready;	}

    if  ( psPostScriptFontListInfosToFamilies( psfl ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    utilCleanMemoryBuffer( &localDirectory );

    return rval;
    }

/************************************************************************/

int psSaveAfms(		const PostScriptFontList *	psfl,
			int				omitKernPairs,
			const MemoryBuffer *		afmDirectory )
    {
    int				rval= 0;
    int				i;

    MemoryBuffer		localDirectory;

    SimpleOutputStream *	sosAfm= (SimpleOutputStream *)0;

    utilInitMemoryBuffer( &localDirectory );

    if  ( psLocalDirectoryName( &localDirectory, afmDirectory ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( appTestDirectory( &localDirectory )	&&
	  appMakeDirectory( &localDirectory )	)
	{ LDEB(1); rval= -1; goto ready;	}

    for ( i= 0; i < psfl->psflInfoCount; i++ )
	{
	AfmFontInfo *	afi= psfl->psflInfos[i];

	if  ( ! afi || utilMemoryBufferIsEmpty( &(afi->afiAfmFileName ) ) )
	    { continue;	}

	sosAfm= sioOutFileioOpen( &(afi->afiAfmFileName) );
	if  ( ! sosAfm )
	    { XDEB(sosAfm); rval= -1; goto ready;	}

	if  ( psWriteAfmFile( sosAfm, omitKernPairs, afi ) )
	    { SDEB(afi->afiFontName); rval= -1; goto ready;	}

	sioOutClose( sosAfm ); sosAfm= (SimpleOutputStream *)0;
	}

  ready:

    if  ( sosAfm )
	{ sioOutClose( sosAfm );	}

    utilCleanMemoryBuffer( &localDirectory );

    return rval;
    }

int psGetDeferredMetrics(	AfmFontInfo *		afi )
    {
    int			rval= 0;
    SimpleInputStream *	sisAfm= (SimpleInputStream *)0;
    int			afmFlags= 0;

    int			res;
    AfmFontInfo		afiMetrics;

    psInitAfmFontInfo( &afiMetrics );

    if  ( afi->afiIgnoreKerning )
	{ afmFlags |= PSflagIGNORE_KERNING;	}

    if  ( utilMemoryBufferIsEmpty( &(afi->afiAfmFileName) ) )
	{ LDEB(afi->afiAfmFileName.mbSize); rval= -1; goto ready; }

    sisAfm= sioInFileioOpen( &(afi->afiAfmFileName) );
    if  ( ! sisAfm )
	{ XDEB(sisAfm); rval= -1; goto ready; }

    setlocale( LC_NUMERIC, "C" );

    res= psAfmReadAfm( sisAfm, &afiMetrics, afmFlags );
    if  ( res < 0 )
	{ LDEB(res); rval= -1; goto ready; }

    psRemoveMetricsFromInfo( afi );
    afi->afiMetrics= afiMetrics.afiMetrics;
    afi->afiMetricCount= afiMetrics.afiMetricCount;
    afi->afiNameToMetric= afiMetrics.afiNameToMetric;

    afiMetrics.afiMetrics= (AfmCharMetric **)0;
    afiMetrics.afiMetricCount= 0;
    afiMetrics.afiNameToMetric= (void *)0;

    afi->afiMetricsDeferred= 0;

    if  ( psGetUnicodesFromGlyphNames( afi ) )
	{ SDEB(afi->afiFullName);	}
    if  ( psGetAlternateGlyphs( afi ) )
	{ SDEB(afi->afiFullName);	}
    if  ( psResolveFallbackGlyph( afi ) )
	{ SDEB(afi->afiFullName);	}

  ready:

    setlocale( LC_NUMERIC, "" );

    psCleanAfmFontInfo( &afiMetrics );

    if  ( sisAfm )
	{ sioInClose( sisAfm );	}

    return rval;
    }

