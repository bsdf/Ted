#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<ctype.h>
#   include	<string.h>

#   include	<sioFileio.h>
#   include	<sioFd.h>
#   include	<sioStdout.h>
#   include	<sioPipe.h>
#   include	<sioMD5.h>
#   include	<utilMD5.h>
#   include	<docHtmlWrite.h>
#   include	<textOfficeCharset.h>
#   include	<appQuestion.h>
#   include	<appSystem.h>
#   include	<psPrint.h>

#   include	"tedApp.h"
#   include	"tedRuler.h"
#   include	"tedAppResources.h"
#   include	"tedLayout.h"
#   include	"tedDocument.h"

#   include	<docPsPrint.h>
#   include	<docSvgDraw.h>
#   include	<docDebug.h>
#   include	<docScreenLayout.h>
#   include	<docRtfReadWrite.h>
#   include	<docPlainReadWrite.h>
#   include	<docField.h>
#   include	<docParaParticules.h>
#   include	<docRecalculateTocField.h>
#   include	<docRtfTrace.h>
#   include	<docEditCommand.h>

#   include	<appPrintJob.h>
#   include	<guiDrawingWidget.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Recalculate all fields in the document. (If needed)			*/
/*									*/
/************************************************************************/

static int tedRefreshRecalculateFields(	int *			pReachedBottom,
					RecalculateFields *	rf,
					DOC_CLOSE_OBJECT	closeObject,
					TedDocument *		td,
					const LayoutContext *	lc )
    {
    BufferDocument *		bd= td->tdDocument;
    const int			page= 0;

    rf->rfDocument= bd;
    rf->rfCloseObject= docScreenCloseObject;
    rf->rfUpdateFlags= FIELDdoDOC_FORMATTED|FIELDdoDOC_COMPLETE|FIELDdoDOC_INFO;
    rf->rfFieldsUpdated= 0;

    rf->rfBodySectNode= bd->bdBody.dtRoot->biChildren[0];

    if  ( docRecalculateTocFields( rf ) )
	{ LDEB(1); return -1;	}

    if  ( docRecalculateTextLevelFieldsInDocumentTree( rf, &(bd->bdBody),
				    bd->bdBody.dtRoot->biChildren[0], page ) )
	{ LDEB(1); return -1;	}

    if  ( rf->rfFieldsUpdated > 0			&&
	  tedLayoutDocumentBody( pReachedBottom, lc )	)
	{ LDEB(1); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Print a document to PostScript.					*/
/*									*/
/************************************************************************/

int tedPrintDocument(	SimpleOutputStream *		sos,
			const PrintJob *		pj,
			const PrintGeometry *		pg )
    {
    int				rval= 0;

    EditApplication *		ea= pj->pjApplication;
    TedDocument *		td= (TedDocument *)pj->pjPrivateData;
    BufferDocument *		bd= td->tdDocument;
    DocumentProperties *	dp= &(bd->bdProperties);

    time_t			now;

    double			shadingMesh= -1;
    TedAppResources *		tar= (TedAppResources *)ea->eaResourceData;
    int				emitOutline= tar->tarPdfOutlineInt > 0;

    RecalculateFields		rf;
    LayoutContext		lc;

    MemoryBuffer		fontDir;

    utilInitMemoryBuffer( &fontDir );

    if  ( ea->eaFontDirectory						&&
	  utilMemoryBufferSetString( &fontDir, ea->eaFontDirectory )	)
	{ XDEB(ea->eaFontDirectory); rval= -1; goto ready;	}

    if  ( tar )
	{ shadingMesh= tar->tarShadingMeshPointsDouble;	}

    now= time( (time_t *)0 );
    dp->dpPrintim= *localtime( &now );

    docInitRecalculateFields( &rf );
    layoutInitContext( &lc );
    tedSetDocumentLayoutContext( &lc, pj->pjDrawingSurface,
					&(ea->eaPostScriptFontList), td );

    if  ( tedRefreshRecalculateFields( (int *)0, &rf,
					docScreenCloseObject, td, &lc ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( docPsPrintDocument( sos, pj->pjTitle,
				    ea->eaApplicationName,
				    ea->eaReference,
				    &fontDir,
				    shadingMesh, emitOutline, &lc, pg ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    utilCleanMemoryBuffer( &fontDir );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Save a document to SVG.						*/
/*									*/
/************************************************************************/

static int tedSaveSvg(	SimpleOutputStream *		sos,
			EditApplication *		ea,
			TedDocument *			td,
			DrawingSurface			ds )
    {
    RecalculateFields		rf;
    LayoutContext		lc;

    docInitRecalculateFields( &rf );
    layoutInitContext( &lc );
    tedSetDocumentLayoutContext( &lc, ds, &(ea->eaPostScriptFontList), td );

    if  ( tedRefreshRecalculateFields( (int *)0, &rf,
					    docScreenCloseObject, td, &lc ) )
	{ LDEB(1); return -1;	}

    if  ( docSvgDrawDocument( sos, ea->eaApplicationName,
					    ea->eaReference, &lc ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

/************************************************************************/

static SimpleOutputStream * tedOpenSaveOutput(
					const DocumentProperties *	dp,
					int			format,
					int			suggestStdout,
					const MemoryBuffer *	filename )
    {
    SimpleOutputStream *	sosDoc= (SimpleOutputStream *)0;
    SimpleOutputStream *	rval= (SimpleOutputStream *)0;
    MemoryBuffer		pdfCommand;

    utilInitMemoryBuffer( &pdfCommand );

    if  ( format == TEDdockindPDF )
	{
	const int	wide= dp->dpGeometry.dgPageWideTwips;
	const int	high= dp->dpGeometry.dgPageHighTwips;

	if  ( suggestStdout )
	    {
	    if  ( psSetToPdfCommand( &pdfCommand, wide, high,
						(const MemoryBuffer *)0 ) )
		{ LDEB(1); goto ready;	}
	    }
	else{
	    if  ( psSetToPdfCommand( &pdfCommand, wide, high, filename ) )
		{ LDEB(1); goto ready;	}
	    }

	sosDoc= sioOutPipeOpen( &pdfCommand );
	if  ( ! sosDoc )
	    { /* SXDEB(filename,sosDoc); */ goto ready;	}
	}
    else{
	if  ( suggestStdout )
	    {
	    sosDoc= sioOutStdoutOpen();
	    if  ( ! sosDoc )
		{ /* SXDEB(filename,sosDoc); */ goto ready;	}
	    }
	else{
	    sosDoc= sioOutFileioOpen( filename );
	    if  ( ! sosDoc )
		{ /* SXDEB(filename,sosDoc); */ goto ready;	}
	    }
	}

    rval= sosDoc; sosDoc= (SimpleOutputStream *)0; /* steal */

  ready:

    utilCleanMemoryBuffer( &pdfCommand );

    if  ( sosDoc )
	{ sioOutClose( sosDoc );	}

    return rval;
    }

/************************************************************************/

static int tedSaveDocumentImpl(		EditApplication *	ea,
					DrawingSurface		ds,
					SimpleOutputStream *	sos,
					TedDocument *		td,
					int			format,
					const MemoryBuffer *	documentTitle,
					const MemoryBuffer *	filename )
    {
    time_t			now;
    BufferDocument *		bd= td->tdDocument;
    DocumentProperties *	dp= &(bd->bdProperties);

    const PostScriptFontList *	psfl= &(ea->eaPostScriptFontList);

    DocumentRectangle		drScreenIgnored;
    DocumentRectangle		drVisibleIgnored;

    switch( format )
	{
	case TEDdockindRTF:
	    now= time( (time_t *)0 );
	    dp->dpRevtim= *localtime( &now );

	    if  ( docRtfSaveDocument( sos, bd, (const DocumentSelection *)0, 0 ) )
		{ LDEB(1); return -1;	}
	    break;

	case TEDdockindTEXT_OPEN:
	    LDEB(format);
	    /*FALLTHROUGH*/

	case TEDdockindTEXT_SAVE_WIDE:
	    if  ( docPlainSaveDocument( sos, bd,
					(const DocumentSelection *)0, 0 ) )
		{ LDEB(1); return -1;	}
	    break;

	case TEDdockindTEXT_SAVE_FOLDED:
	    if  ( docPlainSaveDocument( sos, bd,
					(const DocumentSelection *)0, 1 ) )
		{ LDEB(1); return -1;	}
	    break;

	case TEDdockindHTML_FILES:
	    {
	    RecalculateFields	rf;
	    LayoutContext	lc;

	    docInitRecalculateFields( &rf );
	    layoutInitContext( &lc );
	    tedSetDocumentLayoutContext( &lc, ds, psfl, td );

	    if  ( ! ds )
		{
		if  ( tedFirstRecalculateFields( &rf,
						(DOC_CLOSE_OBJECT)0, bd ) )
		    { LDEB(1); return -1;	}

		if  ( tedLayoutDocument( &drScreenIgnored, &drVisibleIgnored,
					    td, format, (DrawingSurface)0,
					    psfl, &(dp->dpGeometry) ) )
		    {
		    SDEB(utilMemoryBufferGetString(documentTitle));
		    return -1;
		    }
		}

	    if  ( docHtmlSaveDocument( sos, bd, filename, &lc ) )
		{ LDEB(1); return -1;	}
	    }
	    break;

	case TEDdockindEML:
	    {
	    const char *	mimeBoundary= "-----------MimeBoundary";
	    RecalculateFields	rf;
	    LayoutContext	lc;

	    docInitRecalculateFields( &rf );
	    layoutInitContext( &lc );
	    tedSetDocumentLayoutContext( &lc, ds, psfl, td );

	    if  ( ! ds )
		{
		if  ( tedFirstRecalculateFields( &rf,
						(DOC_CLOSE_OBJECT)0, bd ) )
		    { LDEB(1); return -1;	}

		if  ( tedLayoutDocument( &drScreenIgnored, &drVisibleIgnored,
					    td, format, (DrawingSurface)0,
					    psfl, &(dp->dpGeometry) ) )
		    {
		    SDEB(utilMemoryBufferGetString(documentTitle));
		    return -1;
		    }
		}

	    if  ( docEmlSaveDocument( sos, bd, mimeBoundary, &lc ) )
		{ SDEB(mimeBoundary); return -1;	}
	    }
	    break;

	case TEDdockindEPUB:
	    {
	    RecalculateFields	rf;
	    LayoutContext	lc;

	    docInitRecalculateFields( &rf );
	    layoutInitContext( &lc );
	    tedSetDocumentLayoutContext( &lc, ds, psfl, td );

	    if  ( ! ds )
		{
		if  ( tedFirstRecalculateFields( &rf, (DOC_CLOSE_OBJECT)0, bd ) )
		    { LDEB(1); return -1;	}

		if  ( tedLayoutDocument( &drScreenIgnored, &drVisibleIgnored,
					    td, format, (DrawingSurface)0,
					    psfl, &(dp->dpGeometry) ) )
		    {
		    SDEB(utilMemoryBufferGetString(documentTitle));
		    return -1;
		    }
		}

	    if  ( docEpubSaveDocument( sos, bd, &lc ) )
		{ LDEB(format); return -1;	}
	    }
	    break;

	case TEDdockindPS:
	case TEDdockindPDF:
	    if  ( appSaveToPs( ea, ds, sos, td, documentTitle, format ) )
		{ LDEB(format); return -1;	}
	    break;

	case TEDdockindSVG:
	    {
	    LayoutContext	lc;

	    layoutInitContext( &lc );
	    tedSetDocumentLayoutContext( &lc, ds, psfl, td );

	    if  ( ! ds )
		{
		if  ( tedLayoutDocument( &drScreenIgnored, &drVisibleIgnored,
					    td, format, (DrawingSurface)0,
					    psfl, &(dp->dpGeometry) ) )
		    {
		    SDEB(utilMemoryBufferGetString(documentTitle));
		    return -1;
		    }
		}

	    if  ( tedSaveSvg( sos, ea, td, ds ) )
		{
		SDEB(utilMemoryBufferGetString(documentTitle));
		return -1;
		}
	    }

	    break;

	default:
	    LDEB(format); return -1;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Save a document.							*/
/*									*/
/************************************************************************/

int tedSaveDocument(	EditApplication *	ea,
			DrawingSurface		ds,
			const void *		privateData,
			int			format,
			const MemoryBuffer *	documentTitle,
			int			suggestStdout,
			const MemoryBuffer *	filename,
			int			isDocName )
    {
    int				rval= 0;
    TedDocument *		td= (TedDocument *)privateData;
    EditTrace *			etOld= &(td->tdEditTrace);
    BufferDocument *		bd= td->tdDocument;
    DocumentProperties *	dp= &(bd->bdProperties);

    SimpleOutputStream *	sosDoc= (SimpleOutputStream *)0;
    SimpleOutputStream *	sosMD5= (SimpleOutputStream *)0;
    SimpleOutputStream *	sos= (SimpleOutputStream *)0;

    unsigned char		digest[MD5_DIGEST_SIZE_BYTES];
    int				saveDigest= 0;

    EditTrace			etNew;
    int				renameTrace= 0;

    int				fdTrace= FDerrOTHER;

    docInitEditTrace( &etNew );

    if  ( isDocName )
	{
	if  ( docEditTraceSetDocumentName( &etNew, filename,
						    TedTraceExtension ) )
	    { LDEB(1); rval= -1; goto ready;	}

	if  ( etOld->etTraceStatus == TRACING_RELATIVE )
	    {
	    if  ( ! utilEqualMemoryBuffer( &(etNew.etTraceFileName),
						&(etOld->etTraceFileName ) ) )
		{ renameTrace= 1;	}
	    }

	if  ( etOld->etTraceStatus == TRACING_TEMP	||
	      etOld->etTraceStatus == TRACING_ANON	)
	    { renameTrace= 1;	}

	if  ( renameTrace )
	    {
	    fdTrace= docEditTraceTryRelative( &etNew, filename,
							TedTraceExtension );

	    if  ( fdTrace < 0 && etNew.etTraceStatus == TRACING_NO )
		{ LCDEB(fdTrace,etNew.etTraceStatus); rval= -1; goto ready; }
	    if  ( fdTrace < 0 && etNew.etTraceStatus == TRACING_EXIST )
		{ LCDEB(fdTrace,etNew.etTraceStatus); rval= -1; goto ready; }
	    }
	}

    sosDoc= tedOpenSaveOutput( dp, format, suggestStdout, filename );
    if  ( ! sosDoc )
	{ /* SXDEB(filename,sosDoc); */ rval= -1; goto ready;	}

    sos= sosDoc;
    if  ( isDocName && td->tdTraced )
	{
	sosMD5= sioOutMD5Open( digest, sosDoc );
	if  ( ! sosMD5 )
	    { XDEB(sosMD5); rval= -1; goto ready;	}

	sos= sosMD5;
	saveDigest= 1;
	}

    if  ( suggestStdout && format == TEDdockindHTML_FILES )
	{ filename= (const MemoryBuffer *)0;	}

    if  ( tedSaveDocumentImpl( ea, ds, sos, td, format, documentTitle, filename ) )
	{ LDEB(format); rval= -1; goto ready;	}

    /*  flush to finalize digest */
    if  ( sosMD5 )
	{
	sioOutClose( sosMD5 ); sosMD5= (SimpleOutputStream *)0;

	utilMD5ToBase64( etNew.etBaseMD5Digest64, digest );
	}

    if  ( renameTrace							&&
	  etNew.etTraceStatus == TRACING_ACCES				&&
	  ! strcmp( etOld->etBaseMD5Digest64, etNew.etBaseMD5Digest64 )	)
	{ renameTrace= 0;	}

    if  ( renameTrace )
	{
	const int	restart= 0;
	const int	exclusive= 0;

	if  ( etNew.etTraceStatus == TRACING_ACCES )
	    {
	    fdTrace= docEditTraceTryTemp( &etNew, TedTraceExtension );

	    if  ( fdTrace < 0 )
		{ LCDEB(fdTrace,etNew.etTraceStatus); rval= -1; goto ready; }
	    }

	sioFdCloseFile( etNew.etTraceFileHandle ); etNew.etTraceFileHandle= -1;
	sioFdCloseFile( etOld->etTraceFileHandle ); etOld->etTraceFileHandle= -1;

	if  ( appRenameFile( &(etNew.etTraceFileName),
						&(etOld->etTraceFileName) ) )
	    { LDEB(1); rval= -1; goto ready;	}

	if  ( utilCopyMemoryBuffer( &(etOld->etTraceFileName),
						&(etNew.etTraceFileName) ) )
	    { LDEB(1); rval= -1; goto ready;	}
	etOld->etTraceStatus= etNew.etTraceStatus;

	/*  Name just changed */
	if  ( docEditTraceOpenTrace( etOld, restart, exclusive ) < 0 )
	    {
	    SDEB(utilMemoryBufferGetString(&(etOld->etTraceFileName)));
	    rval= -1; goto ready;
	    }
	}

    if  ( saveDigest )
	{
	utilMD5ToBase64( etOld->etBaseMD5Digest64, digest );

	if  ( docRtfTraceVersion( EDITcmd_SAVE, filename,
						    etOld->etBaseMD5Digest64,
						    &(dp->dpRevtim), etOld ) )
	    { LDEB(saveDigest); rval= -1; goto ready;	}

	etOld->etBase= etOld->etIndex- 1;
	etOld->etTyping= TYPING_NO;
	}

    if  ( isDocName )
	{ etOld->etIsRecovery= 0;	}

  ready:

    if  ( sosMD5 )
	{ sioOutClose( sosMD5 );	}
    if  ( sosDoc )
	{ sioOutClose( sosDoc );	}
    
    docCleanEditTrace( &etNew );

    return rval;
    }

