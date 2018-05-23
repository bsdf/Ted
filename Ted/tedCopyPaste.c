/************************************************************************/
/*									*/
/*  Ted: Copy/Paste related functionality.				*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	"tedApp.h"
#   include	"tedSelect.h"
#   include	"tedAppResources.h"
#   include	"tedCopyPasteImpl.h"
#   include	"tedEdit.h"
#   include	"tedDocument.h"
#   include	"tedDocFront.h"
#   include	<docRtfReadWrite.h>
#   include	<docPlainReadWrite.h>
#   include	<appImage.h>
#   include	<docRtfFlags.h>
#   include	<docEditCommand.h>

#   include	<sioMemory.h>
#   include	<sioXprop.h>
#   include	<sioFileio.h>
#   include	<bmio.h>

#   include	<appDebugon.h>

void tedDocCut(			EditDocument *	ed )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    if  ( tedGetSelection( &ds, &sg, &sd,
			    (DocumentTree **)0, (struct BufferItem **)0, ed ) )
	{ return;	}

    if  ( ! sd.sdIsTableSlice && ! sd.sdCanReplace )
	{ LLDEB(sd.sdIsTableSlice,sd.sdCanReplace); return;	}

    if  ( tedDocCopySelection( ed ) )
	{ LDEB(1); return;	}

    if  ( sd.sdIsTableSlice )
	{ tedDeleteTableSliceSelection( ed, td->tdTraced );	}
    else{
	tedDocDeleteSelection( ed, EDITcmdDELETE_SELECTION, td->tdTraced );
	}

    if  ( appDocOwnSelection( ed, "CLIPBOARD",
		TedClipboardTextTargets, TedClipboardTextTargetCount ) )
	{ LDEB(1); 	}

    return;
    }

void tedDocCopy(		EditDocument *	ed )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    const int			scrolledX= 0;
    const int			scrolledY= 0;

    if  ( tedGetSelection( &ds, &sg, &sd,
			    (DocumentTree **)0, (struct BufferItem **)0, ed ) )
	{ return;	}

    if  ( tedDocCopySelection( ed ) )
	{ return;	}

    td->tdVisibleSelectionCopied= 1;

    appDocExposeRectangle( ed, &(sg.sgRectangle), scrolledX, scrolledY );

    if  ( appDocOwnSelection( ed, "CLIPBOARD",
		TedClipboardTextTargets, TedClipboardTextTargetCount ) )
	{ LDEB(1);	}

    return;
    }

/************************************************************************/
/*									*/
/*  Ted, Copy/Paste functionality.					*/
/*									*/
/************************************************************************/

void tedClipboardLost(	APP_WIDGET			w,
			void *				voided,
			APP_EVENT *			event )
    {
    EditDocument *		ed= (EditDocument *)voided;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    if  ( tedGetSelection( &ds, &sg, &sd,
			    (DocumentTree **)0, (struct BufferItem **)0, ed ) )
	{ return;	}

    td->tdVisibleSelectionCopied= 0;

    if  ( tedHasSelection( ed ) && ! sd.sdIsIBarSelection )
	{
	const int	scrolledX= 0;
	const int	scrolledY= 0;

	appDocExposeRectangle( ed, &(sg.sgRectangle), scrolledX, scrolledY );
	}
    }

void tedPrimaryLost(	APP_WIDGET			w,
			void *				voided,
			APP_EVENT *			event )
    {
    EditDocument *		ed= (EditDocument *)voided;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    td->tdOwnsPrimarySelection= 0;

    return;
    }

/************************************************************************/
/*									*/
/*  Return the selection to a requestor.				*/
/*									*/
/************************************************************************/

APP_GIVE_COPY( tedCopyClipboardRtf, w, event, voided )
    {
    EditDocument *	ed= (EditDocument *)voided;
    TedDocument *	td= (TedDocument *)ed->edPrivateData;

    SimpleOutputStream *	sos;

    if  ( utilMemoryBufferIsEmpty( &(td->tdCopiedSelection) ) )
	{ LDEB(td->tdCopiedSelection.mbSize); return; }

    sos= sioOutOpenCopy( w, event );
    if  ( ! sos )
	{ XDEB(sos); return;	}

    if  ( sioOutWriteBytes( sos, td->tdCopiedSelection.mbBytes,
				    td->tdCopiedSelection.mbSize ) <= 0 )
	{ LDEB(td->tdCopiedSelection.mbSize); sioOutClose( sos ); return; }

    sioOutClose( sos );

    if  ( getenv( "TED_SAVE_COPIES" ) )
	{
	sos= sioOutFileioOpenS( "/tmp/returned.rtf" );
	if  ( sos )
	    {
	    if  ( sioOutWriteBytes( sos, td->tdCopiedSelection.mbBytes,
					td->tdCopiedSelection.mbSize ) <= 0 )
		{ LDEB(td->tdCopiedSelection.mbSize); }

	    sioOutClose( sos );
	    }
	}

    return;
    }

static void tedCopyString(	const MemoryBuffer *	mb,
				APP_WIDGET		w,
				APP_SELECTION_EVENT *	event,
				EditDocument *		ed )
    {
    SimpleOutputStream *	sos;

    BufferDocument *		bd;
    SimpleInputStream *		sis;

    const unsigned int		rtfFlags= 0;

    if  ( utilMemoryBufferIsEmpty( mb ) )
	{ LDEB(mb->mbSize); return; }

    sos= sioOutOpenCopy( w, event );
    if  ( ! sos )
	{ XDEB(sos); return;	}

    sis= sioInMemoryOpen( mb );
    if  ( ! sis )
	{ XDEB(sis); sioOutClose( sos ); return;	}

    bd= docRtfReadFile( sis, rtfFlags );
    sioInClose( sis );

    if  ( ! bd )
	{ XDEB(bd); sioOutClose( sos ); return; }

    if  ( getenv( "TED_SAVE_COPIES" ) )
	{ tedSaveSelectionTxtToFile( bd, "/tmp/returned.txt" );	}

    if  ( docPlainSaveDocument( sos, bd, (DocumentSelection *)0, 0 ) )
	{ LDEB(1);	}

    docFreeDocument( bd );
    sioOutClose( sos );

    return;
    }

APP_GIVE_COPY( tedCopyPrimaryString, w, event, voided )
    {
    EditDocument *		ed= (EditDocument *)voided;

    DocumentSelection		ds;
    SelectionDescription	sd;

    MemoryBuffer		mb;

    utilInitMemoryBuffer( &mb );

    if  ( ! tedDocSaveSelectionRtf( &mb, &ds, &sd, ed ) )
	{ tedCopyString( &mb, w, event, ed );	}

    utilCleanMemoryBuffer( &mb );

    return;
    }

APP_GIVE_COPY( tedCopyClipboardString, w, event, voided )
    {
    EditDocument *		ed= (EditDocument *)voided;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    tedCopyString( &(td->tdCopiedSelection), w, event, ed );
    return;
    }

static int tedCopyImageFile(	APP_WIDGET		w,
				APP_SELECTION_EVENT *	event,
				void *			voided,
				bmWriteBitmap		writeBitmap )
    {
    int				rval= 0;
    EditDocument *		ed= (EditDocument *)voided;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    SimpleOutputStream *	sos= (SimpleOutputStream *)0;

    if  ( ! td->tdCopiedImage.riBytes )
	{ XDEB(td->tdCopiedImage.riBytes); rval= -1; goto ready;	}

    sos= sioOutOpenCopy( w, event );
    if  ( ! sos )
	{ XDEB(sos); rval= -1; goto ready;	}

    if  ( (*writeBitmap)( &(td->tdCopiedImage.riDescription),
					td->tdCopiedImage.riBytes, sos ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:
    if  ( sos )
	{ sioOutClose( sos );	}

    return rval;
    }

APP_GIVE_COPY( tedCopyClipboardPng, w, event, voided )
    {
    if  ( tedCopyImageFile( w, event, voided, bmPngWritePng ) )
	{ LDEB(1);	}

    return;
    }

APP_GIVE_COPY( tedCopyClipboardJfif, w, event, voided )
    {
    if  ( tedCopyImageFile( w, event, voided, bmJpegWriteJfif ) )
	{ LDEB(1);	}

    return;
    }

APP_GIVE_COPY( tedCopyFontTed, w, event, voided )
    {
    EditDocument *		ed= (EditDocument *)voided;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    SimpleOutputStream *	sos;

    if  ( utilMemoryBufferIsEmpty( &(td->tdCopiedFont) ) )
	{ LDEB(td->tdCopiedFont.mbSize); return; }

    sos= sioOutOpenCopy( w, event );
    if  ( ! sos )
	{ XDEB(sos); return;	}

    if  ( sioOutWriteBytes( sos, td->tdCopiedFont.mbBytes,
					td->tdCopiedFont.mbSize ) <= 0 )
	{ LDEB(1); sioOutClose( sos ); return;	}

    sioOutClose( sos );

    return;
    }

/************************************************************************/
/*									*/
/*  Copy the ruler as it was saved on the document struct when the	*/
/*  menu option was clicked.						*/
/*									*/
/************************************************************************/

APP_GIVE_COPY( tedCopyRulerTed, w, event, voided )
    {
    EditDocument *		ed= (EditDocument *)voided;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    SimpleOutputStream *	sos;

    if  ( utilMemoryBufferIsEmpty( &(td->tdCopiedRuler) ) )
	{ LDEB(td->tdCopiedRuler.mbSize); return; }

    sos= sioOutOpenCopy( w, event );
    if  ( ! sos )
	{ XDEB(sos); return;	}

    if  ( sioOutWriteBytes( sos, td->tdCopiedRuler.mbBytes,
					td->tdCopiedRuler.mbSize ) <= 0 )
	{ LDEB(1); sioOutClose( sos ); return;	}

    sioOutClose( sos );

    return;
    }

/************************************************************************/
/*									*/
/*  Finish a paste action.						*/
/*									*/
/************************************************************************/

typedef BufferDocument * (*TED_READ) (
				SimpleInputStream *		sis,
				EditApplication *		ea,
				const DocumentGeometry *	dg );

typedef int	(*TED_INCLUDE) (	EditDocument *		ed,
					BufferDocument *	bd,
					int			traced );

static BufferDocument * tedPasteRtfReadFile(
				SimpleInputStream *		sis,
				EditApplication *		ea,
				const DocumentGeometry *	dg )
    {
    const unsigned int	rtfFlags= 0;

    return docRtfReadFile( sis, rtfFlags );
    }

static BufferDocument * tedPastePlainReadFile(
				SimpleInputStream *		sis,
				EditApplication *		ea,
				const DocumentGeometry *	dg )
    {
    int		longestPara;

    return docPlainReadFile( sis, &longestPara, dg );
    }

static int tedPasteClipboardGeneric(	APP_WIDGET		w,
					EditDocument *		ed,
					APP_SELECTION_EVENT *	event,
					TED_READ		readDoc,
					TED_INCLUDE		includeDoc )
    {
    int				rval= 0;
    EditApplication *		ea= ed->edApplication;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    SimpleInputStream *		sis= (SimpleInputStream *)0;
    BufferDocument *		bd= (BufferDocument *)0;

    TedAppResources *		tar= (TedAppResources *)ea->eaResourceData;

    tedDetermineDefaultSettings( tar );

    sis= sioInOpenPaste( w, event );
    if  ( ! sis )
	{ XDEB(sis); rval= -1; goto ready;	}

    bd= (*readDoc)( sis, ea, &(ea->eaDefaultDocumentGeometry) );

    if  ( ! bd )
	{ XDEB( bd ); rval= -1; goto ready; }

    if  ( getenv( "TED_SAVE_COPIES" ) )
	{
	const int			rtfFlags= 0;
	const DocumentSelection * const	ds= (const DocumentSelection *)0;

	tedSaveSelectionToFile( bd, ds, rtfFlags, "/tmp/included.rtf" );
	}

    if  ( (*includeDoc)( ed, bd, td->tdTraced ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    if  ( bd )
	{ docFreeDocument( bd );	}
    if  ( sis )
	{ sioInClose( sis );		}

    return rval;
    }

APP_PASTE_REPLY( tedPasteClipboardRtf, w, event, voided )
    {
    EditDocument *	ed= (EditDocument *)voided;

    tedPasteClipboardGeneric( w, ed, event,
			    tedPasteRtfReadFile, tedIncludeRtfDocument );

    }

APP_PASTE_REPLY( tedPasteClipboardString, w, event, voided )
    {
    EditDocument *	ed= (EditDocument *)voided;

    tedPasteClipboardGeneric( w, ed, event,
			    tedPastePlainReadFile, tedIncludePlainDocument );
    }

APP_PASTE_REPLY( tedPastePrimaryString, w, event, voided )
    {
    EditDocument *	ed= (EditDocument *)voided;

    tedPasteClipboardGeneric( w, ed, event,
			    tedPastePlainReadFile, tedIncludePlainDocument );
    }

static int tedPasteImageFile(	APP_WIDGET		w,
				APP_SELECTION_EVENT *	event,
				EditDocument *		ed,
				bmReadBitmap		readBitmap,
				const char *		nameHint )
    {
    int			rval= 0;
    TedDocument *	td= (TedDocument *)ed->edPrivateData;

    SimpleInputStream *	sis= (SimpleInputStream *)0;
    RasterImage *	ri= (RasterImage *)0;

    MemoryBuffer	filename;

    utilInitMemoryBuffer( &filename );

    if  ( ! td->tdSelectionDescription.sdCanReplace )
	{ LDEB(td->tdSelectionDescription.sdCanReplace); rval= -1; goto ready;	}

    if  ( utilMemoryBufferSetString( &filename, nameHint ) )
	{ SDEB(nameHint); rval= -1; goto ready;	}

    ri= (RasterImage *)malloc( sizeof(RasterImage) );
    if  ( ! ri )
	{ XDEB(ri); rval= -1; goto ready;	}
    bmInitRasterImage( ri );

    sis= sioInOpenPaste( w, event );
    if  ( ! sis )
	{ XDEB(sis); rval= -1; goto ready;	}

    if  ( (*readBitmap)( &(ri->riDescription), &(ri->riBytes), sis ) )
	{ LDEB(1); rval= -1; goto ready;	}
    ri->riFormat= bmSuggestFormat( &(filename),
					ri->riFormat, &(ri->riDescription) );

    if  ( tedReplaceSelectionWithRasterImage( ed, ri, td->tdTraced ) )
	{ LDEB(1); rval= -1; goto ready; }

    ri= (RasterImage *)0; /* stolen */

  ready:

    utilCleanMemoryBuffer( &filename );

    if  ( ri )
	{ bmFreeRasterImage( ri );	}
    if  ( sis )
	{ sioInClose( sis );	}

    return rval;
    }

APP_PASTE_REPLY( tedPasteClipboardPng, w, event, voided )
    {
    if  ( tedPasteImageFile( w, event, (EditDocument *)voided,
						bmPngReadPng, "some.png" ) )
	{ LDEB(1);	}

    return;
    }

APP_PASTE_REPLY( tedPasteClipboardJfif, w, event, voided )
    {
    if  ( tedPasteImageFile( w, event, (EditDocument *)voided,
						bmJpegReadJfif, "some.jpeg" ) )
	{ LDEB(1);	}

    return;
    }

/************************************************************************/

APP_PASTE_REPLY( tedPasteRulerTed, w, event, voided )
    {
    EditDocument *	ed= (EditDocument *)voided;

    tedPasteClipboardGeneric( w, ed, event,
			    tedPasteRtfReadFile, tedApplyPastedRuler );
    return;
    }

/************************************************************************/

APP_PASTE_REPLY( tedPasteFontTed, w, event, voided )
    {
    EditDocument *	ed= (EditDocument *)voided;

    tedPasteClipboardGeneric( w, ed, event,
			    tedPasteRtfReadFile, tedApplyPastedFont );
    return;
    }

/************************************************************************/
/*									*/
/*  'Copy Font' menu option.						*/
/*									*/
/************************************************************************/

void tedCopyFont( EditDocument *	ed )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    SimpleOutputStream *	sos;

    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    const int			rtfFlags= RTFflagNO_BOOKMARKS|RTFflagSAVE_SOMETHING;

    if  ( tedGetSelection( &ds, &sg, &sd,
			    (DocumentTree **)0, (struct BufferItem **)0, ed ) )
	{ LDEB(1); return;	}

    /*  2  */
    sos= sioOutMemoryOpen( &(td->tdCopiedFont) );
    if  ( ! sos )
	{ XDEB(sos); return;	}

    if  ( docRtfSaveDocument( sos, td->tdDocument, &ds, rtfFlags ) )
	{ LDEB(1); sioOutClose( sos ); return;	}

    if  ( sioOutClose( sos ) )
	{ LDEB(1); return;	}

    if  ( getenv( "TED_SAVE_COPIES" ) )
	{
	tedSaveSelectionToFile( td->tdDocument, &ds,
					rtfFlags, "/tmp/savedfont.rtf" );
	}

    appDocOwnSelection( ed, "RTFFONT", TedFontTargets, TedFontTargetCount );
    }

/************************************************************************/
/*									*/
/*  Callbacks for the Copy/Paste ruler menu options.			*/
/*									*/
/*  1)  The ruler paste is a selection paste with the selection		*/
/*	constrained to the last position of the first paragraph in the	*/
/*	selection.							*/
/*  2)  Remember the bytes expecting that someone will ask for them.	*/
/*  3)  Tell that we have a ruler paste available.			*/
/*									*/
/************************************************************************/

void tedCopyRuler( EditDocument *	ed )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    SimpleOutputStream *	sos;

    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    DocumentSelection		dsPara;

    const int			rtfFlags= RTFflagNO_BOOKMARKS;

    if  ( tedGetSelection( &ds, &sg, &sd,
			(DocumentTree **)0, (struct BufferItem **)0, ed ) )
	{ LDEB(1); return;	}

    /*  1  */
    dsPara= ds;
    docTailPosition( &(dsPara.dsHead), dsPara.dsHead.dpNode );
    docSetIBarSelection( &dsPara, &(dsPara.dsHead) );

    /*  2  */
    sos= sioOutMemoryOpen( &(td->tdCopiedRuler) );
    if  ( ! sos )
	{ XDEB(sos); return;	}

    if  ( docRtfSaveDocument( sos, td->tdDocument, &dsPara, rtfFlags ) )
	{ LDEB(1); sioOutClose( sos ); return;	}

    if  ( sioOutClose( sos ) )
	{ LDEB(1); return;	}

    if  ( getenv( "TED_SAVE_COPIES" ) )
	{
	tedSaveSelectionToFile( td->tdDocument, &dsPara,
					rtfFlags, "/tmp/savedruler.rtf" );
	}

    /*  2  */
    appDocOwnSelection( ed, "RTFRULER", TedRulerTargets, TedRulerTargetCount );
    }

/************************************************************************/

void tedManagePrimarySelection(	EditDocument *		ed )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    if  ( ! tedHasIBarSelection( ed ) )
	{
	if  ( appDocOwnSelection( ed, "PRIMARY",
					    TedPrimaryTargets,
					    TedPrimaryTargetCount ) )
	    { LDEB(1);				}
	else{ td->tdOwnsPrimarySelection= 1;	}
	}
    else{
	if  ( td->tdOwnsPrimarySelection )
	    {
	    appDocReleaseSelection( ed, "PRIMARY" );
	    td->tdOwnsPrimarySelection= 0;
	    }
	}

    return;
    }

