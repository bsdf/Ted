/************************************************************************/
/*									*/
/*  Printing related functionality.					*/
/*									*/
/*  TODO: Look at http://www.pwg.org/standards.html 			*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<stdlib.h>
#   include	<string.h>

#   include	<locale.h>
#   include	<errno.h>

#   include	<sioPipe.h>
#   include	<sioStdout.h>
#   include	<sioFileio.h>
#   include	<sioMemory.h>

#   include	<appPaper.h>
#   include	<appSystem.h>
#   include	"appFrame.h"

#   include	"appPrintJob.h"
#   include	<psNup.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Print a document.							*/
/*									*/
/*  1)  Avoid user oriented numeric formatting in a PostScript file,	*/
/*	that is meant to be read by computers.				*/
/*  2)  Use mkstemp to make the name of a temporary file. Though there	*/
/*	is no reason here, we simply conform to the fashionable		*/
/*	paranoia to make 'gcc' shut up.					*/
/*									*/
/************************************************************************/

int appCallPrintFunction(	SimpleOutputStream *		sos,
				const PrintJob *		pj )
    {
    EditApplication *	ea= pj->pjApplication;
    int			rval= 0;

    /*  1  */
    setlocale( LC_NUMERIC, "C" );

    if  ( (*ea->eaPrintDocument)( sos, pj, pj->pjPrintGeometry ) )
	{ LDEB(1); rval= -1;	}

    /*  1  */
    setlocale( LC_NUMERIC, "" );

    return rval;
    }

static int appDocBuildTmpfileCommand(	MemoryBuffer *		target,
					const char *		format,
					const MemoryBuffer *	fileName )
    {
    int				rval= 0;
    SimpleOutputStream *	sos= (SimpleOutputStream *)0;

    const char *	head;
    const char *	from;

    sos= sioOutMemoryOpen( target );
    if  ( ! sos )
	{ XDEB(sos); rval= -1; goto ready;	}

    from= head= format;
    while( *head )
	{
	if  ( head[0] == '%' && head[1] == 'f' )
	    {
	    if  ( head > from )
		{
		if  ( sioOutWriteBytes( sos, (const unsigned char *)from, 
						head- from ) != head- from )
		    { LDEB(head- from); rval= -1; goto ready;	}
		}

	    if  ( sioOutWriteBytes( sos, fileName->mbBytes,
				    fileName->mbSize ) != fileName->mbSize )
		{ LDEB(fileName->mbSize); rval= -1; goto ready;	}

	    head += 2; from= head; continue;
	    }

	head++;
	}

    if  ( head > from )
	{
	if  ( sioOutWriteBytes( sos, (const unsigned char *)from, 
					head- from ) != head- from )
	    { LDEB(head- from); rval= -1; goto ready;	}
	}

  ready:

    if  ( sos && sioOutClose( sos ) )
	{ XDEB(sos); rval= -1;	}

    return rval;
    }

int appPrintDocument(	int				printer,
			const PrintJob *		pj )
    {
    int				rval= 0;
    EditApplication *		ea= pj->pjApplication;
    PrintDestination *		pd= ea->eaPrintDestinations+ printer;

    SimpleOutputStream *	sos= (SimpleOutputStream *)0;

    MemoryBuffer		command;
    MemoryBuffer		scratchName;

    const char *		template= "/tmp/tprXXXXXX";

    utilInitMemoryBuffer( &command );
    utilInitMemoryBuffer( &scratchName );

    switch( pd->pdPrintKind )
	{
	case APPprinterPIPE:
	    {
	    if  ( utilMemoryBufferSetString( &command, pd->pdCommand ) )
		{ LDEB(1); rval= -1; goto ready;	}

	    sos= sioOutPipeOpen( &command );
	    if  ( ! sos )
		{ SXDEB(pd->pdCommand,sos); rval= -1; goto ready;	}

	    appCallPrintFunction( sos, pj );
	    }

	    goto ready;

	case APPprinterTMPFILE:

	    sos= sioOutFileioOpenTempFile( &scratchName, template );
	    if  ( ! sos )
		{ SXDEB(template,sos); rval= -1; goto ready;	}

	    if  ( appDocBuildTmpfileCommand( &command,
					    pd->pdCommand, &scratchName ) )
		{
		LSDEB(pd->pdCommandLength,pd->pdCommand);
		rval= -1; goto ready;
		}

	    appCallPrintFunction( sos, pj );

	    sioOutClose( sos ); sos= (SimpleOutputStream *)0;

	    if  ( system( utilMemoryBufferGetString( &command ) ) )
		{ LDEB(errno);	}

	    goto ready;

	case APPprinterTOFILE:
	default:
	    LDEB(pd->pdPrintKind);
	    rval= -1; goto ready;
	}

  ready:

    if  ( sos && sioOutClose( sos ) )
	{ SDEB(pd->pdCommand); rval= -1;	}

    if  ( ! utilMemoryBufferIsEmpty( &scratchName ) )
	{
	if  ( appRemoveFile( &scratchName ) )
	    { SDEB(pd->pdCommand); rval= -1;	}
	}

    utilCleanMemoryBuffer( &command );
    utilInitMemoryBuffer( &scratchName );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Fill a PrintJob for an EditDocument.				*/
/*									*/
/************************************************************************/

void appPrintJobForEditDocument(	PrintJob *		pj,
					EditDocument *		ed,
					const PrintGeometry *	pg )
    {
    EditApplication *	ea= ed->edApplication;

    pj->pjPrivateData= ed->edPrivateData;
    pj->pjFormat= ed->edFormat;
    pj->pjDrawingSurface= ed->edDrawingSurface;
    pj->pjApplication= ea;
    pj->pjTitle= utilMemoryBufferGetString( &(ed->edTitle) );
    pj->pjPrintGeometry= pg;

    return;
    }

void appApplicationSettingsToPrintGeometry(
					PrintGeometry *		pg,
					EditApplication *	ea )
    {
    pg->pgUsePostScriptFilters= ea->eaUsePostScriptFiltersInt > 0;
    pg->pgUsePostScriptIndexedImages= ea->eaUsePostScriptIndexedImagesInt > 0;
    pg->pgEmbedFonts= ea->eaEmbedFontsInt > 0;
    pg->pg7Bits= ea->ea7BitsPostScriptInt > 0;

    pg->pgSkipEmptyPages= ea->eaSkipEmptyPagesInt > 0;
    pg->pgSkipBlankPages= ea->eaSkipBlankPagesInt > 0;
    pg->pgOmitHeadersOnEmptyPages= ea->eaOmitHeadersOnEmptyPagesInt > 0;

    if  ( ea->eaCustomPsSetupFilename )
	{ pg->pgCustomPsSetupFilename= ea->eaCustomPsSetupFilename; }

    return;
    }

/************************************************************************/
/*									*/
/*  Start a print job for a command line call.				*/
/*									*/
/************************************************************************/

static int appPrintJobForCommand(	PrintJob *		pj,
					PrintGeometry *		pg,
					EditApplication *	ea,
					const char *		fromName )
    {
    int			rval= 0;
    MemoryBuffer	filename;
    const int		readOnly= 1;
    int			suggestStdin= 0;
    int			formatHint= -1;

    utilInitMemoryBuffer( &filename );

    if  ( utilMemoryBufferSetString( &filename, fromName ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( ea->eaMakePrivateData	)
	{
	pj->pjPrivateData= (*ea->eaMakePrivateData)();
	if  ( ! pj->pjPrivateData )
	    { XDEB(pj->pjPrivateData); rval= -1; goto ready; }
	}

    formatHint= appDocumentGetOpenFormat( &suggestStdin,
			ea->eaFileExtensions, ea->eaFileExtensionCount,
			&filename, formatHint );

    if  ( (*ea->eaOpenDocument)( ea, pj->pjPrivateData, &(pj->pjFormat),
			    ea->eaToplevel.atTopWidget, (APP_WIDGET)0,
			    readOnly, suggestStdin, formatHint, &filename ) )
	{ SDEB((char *)fromName); rval= -1; goto ready; }

    pj->pjDrawingSurface= (DrawingSurface)0;
    pj->pjApplication= ea;
    pj->pjTitle= fromName;
    pj->pjPrintGeometry= pg;

  ready:

    utilCleanMemoryBuffer( &filename );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Initial setup for command line printing.				*/
/*									*/
/************************************************************************/

static void appPrintFinishCommandRun(	EditApplication *	ea,
					PrintJob *		pj )
    {
    (*ea->eaFreeDocument)( pj->pjPrivateData, pj->pjFormat );

    return;
    }

static int appPrintStartCommandRun(	EditApplication *	ea,
					PrintJob *		pj,
					PrintGeometry *		pg,
					const char *		paperString,
					const char *		fromName )
    {
    DocumentRectangle	drScreenIgnored;
    DocumentRectangle	drVisibleIgnored;
    const int		sheetSize= 0;

    pg->pgSheetGeometry= ea->eaDefaultDocumentGeometry;

    if  ( paperString )
	{
	DocumentGeometry *	dg= &(pg->pgSheetGeometry);
	int			paperFormat;

	if  ( utilPaperSizeFromString( &paperFormat,
					    &(dg->dgPageWideTwips),
					    &(dg->dgPageHighTwips),
					    ea->eaUnitInt, paperString ) )
	    { SDEB(paperString); return -1;	}
	}

    if  ( appPrintJobForCommand( pj, pg, ea, fromName ) )
	{ SDEB(fromName); return -1;	}

    if  ( ea->eaSuggestPageSetup )
	{ (*ea->eaSuggestPageSetup)( pg, pj->pjPrivateData, sheetSize ); }

    if  ( (*ea->eaLayoutDocument)( &drScreenIgnored, &drVisibleIgnored,
					    pj->pjPrivateData, pj->pjFormat,
					    (DrawingSurface)0,
					    &(ea->eaPostScriptFontList),
					    &(pg->pgSheetGeometry) ) )
	{
	SDEB(fromName);
	appPrintFinishCommandRun( ea, pj );
	return -1;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Command line Print To File entrypoint.				*/
/*									*/
/************************************************************************/

int appPrintToFile(	EditApplication *	ea,
			const char *		fromName,
			const char *		toName,
			const char *		paperString )
    {
    int				rval= 0;

    SimpleOutputStream *	sos;

    PrintGeometry		pg;
    PrintJob			pj;

    psInitPrintGeometry( &pg );
    appApplicationSettingsToPrintGeometry( &pg, ea );

    if  ( ! strcmp( fromName, toName ) )
	{ SSDEB(fromName,toName); rval= -1; goto ready;	}

    if  ( appPrintStartCommandRun( ea, &pj, &pg, paperString, fromName ) )
	{ SDEB(fromName); rval= -1; goto ready;	}

    if  ( ! strcmp( toName, "-ps-" )		||
	  ! strcmp( toName, "-" )		)
	{ sos= sioOutStdoutOpen();		}
    else{ sos= sioOutFileioOpenS( toName );	}

    if  ( ! sos )
	{
	SXDEB(toName,sos);
	appPrintFinishCommandRun( ea, &pj );
	rval= -1; goto ready;
	}

    if  ( appCallPrintFunction( sos, &pj ) )
	{ SDEB(fromName); rval= -1;	}

    sioOutClose( sos );

    appPrintFinishCommandRun( ea, &pj );

  ready:

    psCleanPrintGeometry( &pg );

    return rval;
    }

/************************************************************************/

int appSaveToPs(		EditApplication *	ea,
				DrawingSurface		ds,
				SimpleOutputStream *	sos,
				void *			privateData,
				const MemoryBuffer *	documentTitle,
				int			format )
    {
    DocumentRectangle		drScreenIgnored;
    DocumentRectangle		drVisibleIgnored;
    const int			sheetSize= 1;

    int				rval= 0;
    PrintGeometry		pg;
    PrintJob			pj;

    psInitPrintGeometry( &pg );
    appApplicationSettingsToPrintGeometry( &pg, ea );

    pj.pjPrivateData= privateData;
    pj.pjFormat= format;
    pj.pjTitle= utilMemoryBufferGetString( documentTitle );
    pj.pjDrawingSurface= ds;
    pj.pjApplication= ea;
    pj.pjPrintGeometry= &pg;

    if  ( ea->eaSuggestPageSetup )
	{ (*ea->eaSuggestPageSetup)( &pg, pj.pjPrivateData, sheetSize ); }

    if  ( (*ea->eaLayoutDocument)( &drScreenIgnored, &drVisibleIgnored,
					    pj.pjPrivateData, pj.pjFormat,
					    (DrawingSurface)0,
					    &(ea->eaPostScriptFontList),
					    &(pg.pgSheetGeometry) ) )
	{ SDEB(pj.pjTitle); rval= -1; goto ready;	}

    if  ( appCallPrintFunction( sos, &pj ) )
	{ LDEB(format); rval= -1; goto ready;	}

  ready:

    psCleanPrintGeometry( &pg );
    return rval;
    }

/************************************************************************/
/*									*/
/*  Command line Print To Printer entrypoint.				*/
/*									*/
/************************************************************************/

int appPrintToPrinter(	EditApplication *	ea,
			const char *		fromName,
			const char *		toName,
			const char *		paperString )
    {
    int			rval= 0;
    int			printer;

    PrintGeometry	pg;
    PrintJob		pj;

    psInitPrintGeometry( &pg );
    appApplicationSettingsToPrintGeometry( &pg, ea );

    if  ( ! ea->eaPrintDestinationsCollected	&&
	  appGetPrintDestinations( ea )		)
	{ LDEB(1); rval= -1; goto ready;	}

    printer= ea->eaDefaultPrintDestination;

    if  ( toName )
	{
	int				i;
	const PrintDestination *	pd;

	pd= ea->eaPrintDestinations;
	for ( i= 0; i < ea->eaPrintDestinationCount; pd++, i++ )
	    {
	    if  ( ! strcmp( toName, pd->pdPrinterName ) )
		{ printer= i; break;	}
	    }

	if  ( i >= ea->eaPrintDestinationCount )
	    { SDEB(toName); rval= -1; goto ready;	}
	}

    if  ( printer < 0 )
	{ LDEB(printer); rval= -1; goto ready;	}

    if  ( appPrintStartCommandRun( ea, &pj, &pg, paperString, fromName ) )
	{ SDEB(fromName); rval= -1; goto ready;	}

    if  ( appPrintDocument( printer, &pj ) )
	{ SDEB(fromName); rval= -1;	}

    appPrintFinishCommandRun( ea, &pj );

  ready:

    psCleanPrintGeometry( &pg );
    return rval;
    }

