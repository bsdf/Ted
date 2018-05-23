/************************************************************************/
/*									*/
/*  Ted: Copy/Paste implementation.					*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	"tedApp.h"
#   include	"tedSelect.h"
#   include	"tedCopyPasteImpl.h"
#   include	"tedEdit.h"
#   include	"tedDocument.h"
#   include	<docRtfReadWrite.h>
#   include	<docPlainReadWrite.h>
#   include	<docRtfFlags.h>
#   include	<docTreeNode.h>
#   include	<docEditCommand.h>

#   include	<sioFileio.h>
#   include	<sioMemory.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Write copy/paste to file for debugging purposes.			*/
/*									*/
/************************************************************************/

void tedSaveSelectionToFile(	BufferDocument *		bd,
				const DocumentSelection *	ds,
				int				rtfFlags,
				const char *			filename )
    {
    SimpleOutputStream *	sos;

    SDEB(filename);
    sos= sioOutFileioOpenS( filename );
    if  ( ! sos )
	{ SPDEB( filename, sos );	}
    else{
	if  ( docRtfSaveDocument( sos, bd, ds, rtfFlags ) )
	    { LDEB(1); 	}
	if  ( sioOutClose( sos ) )
	    { LDEB(1);	}
	}

    return;
    }

void tedSaveSelectionTxtToFile(	BufferDocument *		bd,
				const char *			filename )
    {
    SimpleOutputStream *	sos;

    SDEB(filename);
    sos= sioOutFileioOpenS( filename );
    if  ( ! sos )
	{ SPDEB( filename, sos );	}
    else{
	if  ( docPlainSaveDocument( sos, bd, (DocumentSelection *)0, 0 ) )
	    { LDEB(1); 	}
	if  ( sioOutClose( sos ) )
	    { LDEB(1);	}
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Save the current selection in rtf format.				*/
/*									*/
/************************************************************************/

int tedDocSaveSelectionRtf(	MemoryBuffer *		mb,
				DocumentSelection *	ds,
				SelectionDescription *	sd,
				EditDocument *		ed )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    SimpleOutputStream *	sos;

    SelectionGeometry		sg;

    const int			rtfFlags= RTFflagNO_BOOKMARKS;

    if  ( ! tedHasSelection( ed ) || tedHasIBarSelection( ed ) )
	{ return -1; }

    if  ( tedGetSelection( ds, &sg, sd,
			    (DocumentTree **)0, (struct BufferItem **)0, ed ) )
	{ return -1;	}

    sos= sioOutMemoryOpen( mb );
    if  ( ! sos )
	{ XDEB(sos); return -1;    }

    if  ( docRtfSaveDocument( sos, td->tdDocument, ds, rtfFlags ) )
	{ LDEB(1); sioOutClose( sos ); return -1;	}

    if  ( sioOutClose( sos ) )
	{ LDEB(1); return -1;	}

    if  ( getenv( "TED_SAVE_COPIES" ) )
	{
	tedSaveSelectionToFile( td->tdDocument, ds, rtfFlags,
							"/tmp/saved.rtf" );
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Save the selection in order to make it available if someone asks	*/
/*  for a paste.							*/
/*									*/
/************************************************************************/

int tedDocCopySelection(	EditDocument *	ed )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    DocumentPosition		dpObject;
    int				partObject;
    InsertedObject *		io;

    DocumentSelection		ds;
    SelectionDescription	sd;

    if  ( tedDocSaveSelectionRtf( &(td->tdCopiedSelection), &ds, &sd, ed ) )
	{ LDEB(1); return -1;	}

    bmCleanRasterImage( &(td->tdCopiedImage) );
    bmInitRasterImage( &(td->tdCopiedImage) );

    docInitDocumentPosition( &dpObject );

    if  ( sd.sdIsObjectSelection					&&
	  ! docGetObjectSelection( &ds, td->tdDocument,
				      &partObject, &dpObject, &io )	)
	{
	if  ( tedSaveObjectPicture( &(td->tdCopiedImage), io )	)
	    { LDEB(1);	}
	}

    return 0;
    }

/************************************************************************/

static int tedGetRulerFromPaste(	ParagraphProperties *	ppSet,
					PropertyMask *		ppSetMask,
					EditOperation *		eo,
					const MemoryBuffer *	filename,
					BufferDocument *	bdFrom )
    {
    int				rval= 0;

    const int			forceAttributeTo= -1;
    DocumentPosition		dp;

    DocumentCopyJob		dcj;

    docInitDocumentCopyJob( &dcj );

    if  ( docSet2DocumentCopyJob( &dcj, eo, bdFrom, &(bdFrom->bdBody),
					filename, forceAttributeTo ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( docDocumentHead( &dp, bdFrom ) )
	{ LDEB(1); rval= -1; goto ready;	}

    utilPropMaskClear( ppSetMask );
    PROPmaskADD( ppSetMask, PPpropLEFT_INDENT );
    PROPmaskADD( ppSetMask, PPpropFIRST_INDENT );
    PROPmaskADD( ppSetMask, PPpropRIGHT_INDENT );
    PROPmaskADD( ppSetMask, PPpropALIGNMENT );
    PROPmaskADD( ppSetMask, PPpropTAB_STOPS );

    PROPmaskADD( ppSetMask, PPpropLISTOVERRIDE );
    PROPmaskADD( ppSetMask, PPpropOUTLINELEVEL );
    PROPmaskADD( ppSetMask, PPpropLISTLEVEL );

    if  ( docUpdParaProperties( (PropertyMask *)0, ppSet, ppSetMask,
						&(dp.dpNode->biParaProperties),
						&(dcj.dcjAttributeMap) ) )
	{ LDEB(1); rval= -1; goto ready; }

  ready:

    docCleanDocumentCopyJob( &dcj );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Apply the Ruler of the first paragraph in the pasted document to	*/
/*  the current selection.						*/
/*									*/
/************************************************************************/

int tedApplyPastedRuler(		EditDocument *		ed,
					BufferDocument *	bdFrom,
					int			traced )
    {
    int				rval= 0;

    ParagraphProperties		ppSet;
    PropertyMask		ppSetMask;

    TedEditOperation		teo;
    EditOperation *		eo= &(teo.teoEo);
    SelectionGeometry		sg;
    SelectionDescription	sd;

    DocumentSelection		ds;

    const int			fullWidth= 1;

    docInitParagraphProperties( &ppSet );

    tedStartEditOperation( &teo, &sg, &sd, ed, fullWidth, traced );

    docEditOperationGetSelection( &ds, eo );

    if  ( tedGetRulerFromPaste( &ppSet, &ppSetMask,
					    eo, &(ed->edFilename), bdFrom ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( tedEditChangeSelectionProperties( &teo, &ds,
		    DOClevPARA, EDITcmdUPD_PARA_PROPS,
		    (const PropertyMask *)0, (const TextAttribute *)0,
		    &ppSetMask, &ppSet,
		    (const PropertyMask *)0, (const CellProperties *)0,
		    (const PropertyMask *)0, (const RowProperties *)0,
		    (const PropertyMask *)0, (const SectionProperties *)0,
		    (const PropertyMask *)0, (const DocumentProperties *)0 ) )
	{ LDEB(1); rval= -1; goto ready; }

    /* tedEditChangeSelectionProperties() finishes the TedEditOperation */

  ready:

    tedCleanEditOperation( &teo );
    docCleanParagraphProperties( &ppSet );

    return rval;
    }

/************************************************************************/

static int tedGetAttributesFromPaste(	TextAttribute *		taSetTo,
					PropertyMask *		taSetMask,
					EditOperation *		eo,
					const MemoryBuffer *	filename,
					BufferDocument *	bdFrom )
    {
    int				rval= 0;

    const int			forceAttributeTo= -1;
    PropertyMask		taOnlyMask;

    DocumentSelection		dsAll;

    TextAttribute		taSetFrom;

    DocumentCopyJob		dcj;

    docInitDocumentCopyJob( &dcj );

    if  ( docSet2DocumentCopyJob( &dcj, eo, bdFrom, &(bdFrom->bdBody),
					filename, forceAttributeTo ) )
	{ LDEB(1); rval= -1; goto ready;	}

    docInitDocumentSelection( &dsAll );
    if  ( docSelectWholeBody( &dsAll, bdFrom ) )
	{ LDEB(1); rval= -1; goto ready;	}

    utilPropMaskClear( taSetMask );

    utilInitTextAttribute( &taSetFrom );
    utilInitTextAttribute( taSetTo );

    docGetSelectionAttributes( bdFrom, &dsAll, taSetMask, &taSetFrom );
    docMapTextAttribute( taSetTo, &taSetFrom, &dcj );

    utilPropMaskClear( &taOnlyMask );

    PROPmaskADD( &taOnlyMask, TApropFONT_NUMBER );
    PROPmaskADD( &taOnlyMask, TApropFONTSIZE );
    PROPmaskADD( &taOnlyMask, TApropFONTBOLD );
    PROPmaskADD( &taOnlyMask, TApropFONTSLANTED );
    PROPmaskADD( &taOnlyMask, TApropTEXTUNDERLINED );
    PROPmaskADD( &taOnlyMask, TApropSTRIKETHROUGH );

    /* PROPmaskADD( &taOnlyMask, TApropSUPERSUB ); */
    /* PROPmaskADD( &taOnlyMask, TApropSMALLCAPS ); */
    /* PROPmaskADD( &taOnlyMask, TApropCAPITALS ); */

    PROPmaskADD( &taOnlyMask, TApropTEXT_COLOR );
    PROPmaskADD( &taOnlyMask, TApropTEXT_STYLE );
    PROPmaskADD( &taOnlyMask, TApropBORDER );
    PROPmaskADD( &taOnlyMask, TApropSHADING );

    utilPropMaskAnd( taSetMask, taSetMask, &taOnlyMask );

  ready:

    docCleanDocumentCopyJob( &dcj );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Apply the text attributes of the first position in the pasted	*/
/*  document to the current selection.					*/
/*									*/
/************************************************************************/

int tedApplyPastedFont(		EditDocument *		ed,
				BufferDocument *	bdFrom,
				int			traced )
    {
    int				rval= 0;

    DocumentSelection		dsAll;

    TextAttribute		taSet;
    PropertyMask		taSetMask;

    TedEditOperation		teo;
    EditOperation *		eo= &(teo.teoEo);
    SelectionGeometry		sg;
    SelectionDescription	sd;

    DocumentSelection		ds;

    const int			fullWidth= 1;

    docInitDocumentSelection( &dsAll );

    tedStartEditOperation( &teo, &sg, &sd, ed, fullWidth, traced );

    docEditOperationGetSelection( &ds, eo );

    if  ( tedGetAttributesFromPaste( &taSet, &taSetMask,
					    eo, &(ed->edFilename), bdFrom ) )
	{ LDEB(1); rval= -1; goto ready; }

    if  ( tedEditChangeSelectionProperties( &teo, &ds,
		    DOClevSPAN, EDITcmdUPD_SPAN_PROPS,
		    &taSetMask, &taSet,
		    (const PropertyMask *)0, (const ParagraphProperties *)0,
		    (const PropertyMask *)0, (const CellProperties *)0,
		    (const PropertyMask *)0, (const RowProperties *)0,
		    (const PropertyMask *)0, (const SectionProperties *)0,
		    (const PropertyMask *)0, (const DocumentProperties *)0 ) )
	{ LDEB(1); rval= -1; goto ready; }

    /* tedEditChangeSelectionProperties() finishes the TedEditOperation */

  ready:

    tedCleanEditOperation( &teo );

    return rval;
    }

