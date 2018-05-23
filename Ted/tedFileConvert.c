/************************************************************************/
/*									*/
/*  Ted, main module.							*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<string.h>
#   include	<ctype.h>
#   include	<ctype.h>

#   include	"tedApp.h"

#   include	<psBuildConfigFiles.h>
#   include	<sioFileio.h>
#   include	<sioStdout.h>
#   include	<appFindX11Fonts.h>
#   include	<appMatchFont.h>
#   include	<appSystem.h>
#   include	<docFontsDocuments.h>
#   include	"tedFileConvert.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Miscelaneous file conversion calls.					*/
/*									*/
/************************************************************************/

static SimpleOutputStream * tedFileConvertOpenOut(
					    const char *	nameOut )
    {
    SimpleOutputStream *	sosOut= (SimpleOutputStream *)0;

    if  ( ! strcmp( nameOut, "-" ) )
	{
	sosOut= sioOutStdoutOpen();
	if  ( ! sosOut )
	    { SXDEB(nameOut,sosOut);	}
	}
    else{
	sosOut= sioOutFileioOpenS( nameOut );
	if  ( ! sosOut )
	    { SXDEB(nameOut,sosOut);	}
	}

    return sosOut;
    }

# if 0

static int tedFileConvert(
		    EditApplication *	ea,
		    const char *	nameIn,
		    const char *	nameOut,
		    int (*cvf)(	EditApplication *	ea,
				SimpleOutputStream *	_sosOut,
				const char *		_nameIn,
				SimpleInputStream *	_sisIn ) )
    {
    int				rval= 0;
    SimpleOutputStream *	sosOut= (SimpleOutputStream *)0;
    SimpleInputStream *		sisIn= (SimpleInputStream *)0;

    if  ( ! strcmp( nameIn, "-" ) )
	{
	sisIn= sioInStdinOpen();
	if  ( ! sisIn )
	    { SXDEB(nameIn,sisIn); rval= -1; goto ready;	}
	}
    else{
	sisIn= sioInFileioOpenS( nameIn );
	if  ( ! sisIn )
	    { SXDEB(nameIn,sisIn); rval= -1; goto ready;	}
	}

    sosOut= tedFileConvertOpenOut( nameOut );
    if  ( ! sosOut )
	{ SXDEB(nameOut,sosOut); rval= -1; goto ready;	}

    if  ( (*cvf)( ea, sosOut, nameIn, sisIn ) )
	{ SSDEB(nameOut,nameIn); rval= -1; goto ready;	}

  ready:

    if  ( sosOut )
	{ sioOutClose( sosOut );	}
    if  ( sisIn )
	{ sioInClose( sisIn );	}

    return rval;
    }

# endif

int tedTtfToAfm(		EditApplication *	ea,
				const char *		prog,
				const char *		call,
				int			argc,
				char **			argv )
    {
    int				rval= 2;
    SimpleOutputStream *	sosOut= (SimpleOutputStream *)0;

    MemoryBuffer		psDir;

    utilInitMemoryBuffer( &psDir );

    if  ( argc < 2 )
	{ SLDEB(argv[0],argc); rval= 1; goto ready;	}

    if  ( utilMemoryBufferSetString( &psDir, PSSCRIPT_DIR ) )
	{ LDEB(1); rval= -1; goto ready;	}

    sosOut= tedFileConvertOpenOut( argv[1] );
    if  ( ! sosOut )
	{ SXDEB(argv[1],sosOut); rval= -1; goto ready;	}

    if  ( psFontFileToAfm( sosOut, ea->eaUseKerningInt < 0, argv[0], &psDir ) )
	{ SSSDEB(call,argv[0],argv[1]); rval= -1; goto ready;	}

  ready:

    utilCleanMemoryBuffer( &psDir );

    if  ( sosOut )
	{ sioOutClose( sosOut );	}

    return rval;
    }

int tedAfmToGSFontmap(		EditApplication *	ea,
				const char *		prog,
				const char *		call,
				int			argc,
				char **			argv )
    {
    int				rval= 2;
    SimpleOutputStream *	sosOut= (SimpleOutputStream *)0;

    if  ( argc < 2 )
	{ SLDEB(argv[0],argc); rval= 1; goto ready;	}

    sosOut= tedFileConvertOpenOut( argv[1] );
    if  ( ! sosOut )
	{ SXDEB(argv[1],sosOut); rval= -1; goto ready;	}

    if  ( psAfmToGSFontmap( sosOut, argv[0] ) )
	{ SSSDEB(call,argv[0],argv[1]); rval= -1; goto ready;	}

  ready:

    if  ( sosOut )
	{ sioOutClose( sosOut );	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Build an afm directory file for a particular font file.		*/
/*									*/
/************************************************************************/

int tedAfmForFontFiles(	EditApplication *		ea,
			const char *			prog,
			const char *			call,
			int				argc,
			char **				argv )
    {
    int			ret= argc;

    PostScriptFontList	psfl;
    MemoryBuffer	afmDir;
    MemoryBuffer	psDir;

    utilInitMemoryBuffer( &afmDir );
    utilInitMemoryBuffer( &psDir );

    psInitPostScriptFontList( &psfl );

    if  ( ! ea->eaAfmDirectory )
	{ XDEB(ea->eaAfmDirectory); ret= -1; goto ready;	}
    if  ( utilMemoryBufferSetString( &afmDir, ea->eaAfmDirectory ) )
	{ LDEB(1); ret= -1; goto ready;	}
    if  ( utilMemoryBufferSetString( &psDir, PSSCRIPT_DIR ) )
	{ LDEB(1); ret= -1; goto ready;	}

    if  ( psAfmForFontFiles( &psfl, ea->eaUseKerningInt < 0, argc, argv,
							&afmDir, &psDir ) )
	{
	SSSDEB(call,ea->eaAfmDirectory,PSSCRIPT_DIR);
	ret= -1; goto ready;
	}

    if  ( ea->eaToplevel.atTopWidget				&&
	  appFindX11Fonts( ea->eaToplevel.atTopWidget, &psfl )	)
	{ XDEB(ea->eaToplevel.atTopWidget); ret= -1; goto ready;	}

    if  ( psSaveAfms( &psfl, ea->eaUseKerningInt < 0, &afmDir ) )
	{ SDEB(ea->eaAfmDirectory); ret= -1; goto ready;	}

  ready:

    utilCleanMemoryBuffer( &afmDir );
    utilCleanMemoryBuffer( &psDir );

    psCleanPostScriptFontList( &psfl );

    return ret;
    }

int tedFontsDocuments(	EditApplication *		ea,
			const char *			prog,
			const char *			call,
			int				argc,
			char **				argv )
    {
    int				rval= 1;

    MemoryBuffer		templDir;
    MemoryBuffer		outDir;
    MemoryBuffer		scriptRelative;
    MemoryBuffer		scriptAbsoluteFrom;
    MemoryBuffer		scriptAbsoluteTo;

    const int			relativeIsFile= 0;

    utilInitMemoryBuffer( &templDir );
    utilInitMemoryBuffer( &outDir );
    utilInitMemoryBuffer( &scriptRelative );
    utilInitMemoryBuffer( &scriptAbsoluteFrom );
    utilInitMemoryBuffer( &scriptAbsoluteTo );

    if  ( argc < 1 )
	{ LDEB(argc); rval= -1; goto ready;	}

    if  ( utilMemoryBufferSetString( &templDir, FONT_TPL_DIR ) )
	{ LDEB(1); rval= -1; goto ready;	}
    if  ( utilMemoryBufferSetString( &outDir, argv[0] ) )
	{ LDEB(1); rval= -1; goto ready;	}
    if  ( utilMemoryBufferSetString( &scriptRelative, "checkfontsdocs.sh" ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( appAbsoluteName( &scriptAbsoluteFrom,
			    &scriptRelative, relativeIsFile, &templDir ) < 0 )
	{ SDEB(utilMemoryBufferGetString(&templDir)); rval= -1; goto ready; }
    if  ( appAbsoluteName( &scriptAbsoluteTo,
			    &scriptRelative, relativeIsFile, &outDir ) < 0 )
	{ SDEB(utilMemoryBufferGetString(&outDir)); rval= -1; goto ready; }

    if  ( appPostScriptFontCatalog( ea ) )
	{ SDEB(ea->eaAfmDirectory); rval= -1; goto ready;	}

    if  ( appGetDeferredFontMetricsForList( &(ea->eaPostScriptFontList) ) )
	{ SDEB(ea->eaAfmDirectory); rval= -1; /*goto ready;*/	}

    if  ( appTestDirectory( &outDir )	&&
	  appMakeDirectories( &outDir )	)
	{ SDEB(utilMemoryBufferGetString(&outDir)); rval= -1; goto ready; }

    if  ( appCopyFile( &scriptAbsoluteTo, &scriptAbsoluteFrom ) )
	{ SDEB(argv[0]); rval= -1; goto ready;	}

    if  ( docFontsDocuments( &(ea->eaPostScriptFontList),
						    &templDir, &outDir ) )
	{ SDEB(argv[0]); rval= -1; goto ready;	}

  ready:

    utilCleanMemoryBuffer( &templDir );
    utilCleanMemoryBuffer( &outDir );
    utilCleanMemoryBuffer( &scriptRelative );
    utilCleanMemoryBuffer( &scriptAbsoluteFrom );
    utilCleanMemoryBuffer( &scriptAbsoluteTo );

    return rval;
    }

