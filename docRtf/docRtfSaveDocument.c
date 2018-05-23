/************************************************************************/
/*									*/
/*  Save a BufferDocument into an RTF file.				*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<stdio.h>
#   include	<ctype.h>

#   include	<appDebugon.h>

#   include	"docRtfReadWrite.h"
#   include	"docRtfWriterImpl.h"
#   include	"docRtfTags.h"
#   include	<docNotes.h>
#   include	<docTreeType.h>

int docRtfSaveDocument(		SimpleOutputStream *		sos,
				BufferDocument *		bd,
				const DocumentSelection *	ds,
				int				flags )
    {
    int				rval= 0;
    RtfWriter *			rwc= (RtfWriter *)0;

    PropertyMask		dpSaveMask;

    struct DocumentNote *	dn;
    int				hasFootNotes= 0;
    int				hasEndNotes= 0;
    int				fet;

    rwc= docRtfOpenWriter( sos, bd, flags );
    if  ( ! rwc )
	{ XDEB(rwc); rval= -1; goto ready;	}

    if  ( docRtfDocPropMask( &dpSaveMask, &(bd->bdProperties) ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( bd->bdProperties.dpFontList->dflFontCount > 0 )
	{ PROPmaskADD( &dpSaveMask, DPpropFONTTABLE );	}
    if  ( bd->bdProperties.dpColorPalette->cpColorCount > 0 )
	{ PROPmaskADD( &dpSaveMask, DPpropCOLORTABLE );	}
    if  ( bd->bdProperties.dpListAdmin->laListTable.dltListCount > 0 )
	{ PROPmaskADD( &dpSaveMask, DPpropLISTTABLE );	}
    if  ( bd->bdProperties.dpListAdmin->laListOverrideTable.lotOverrideCount > 0 )
	{ PROPmaskADD( &dpSaveMask, DPpropLISTOVERRIDETABLE );	}
    if  ( bd->bdStyleSheet.dssStyleCount > 0 )
	{ PROPmaskADD( &dpSaveMask, DPpropSTYLESHEET );	}

    if  ( ! utilMemoryBufferIsEmpty( &(bd->bdProperties.dpGeneratorWrite) ) )
	{ PROPmaskADD( &dpSaveMask, DPpropGENERATOR );	}

    if  ( docRtfWriteBuildFontAdmin( rwc ) )
	{ LDEB(1); rval= -1; goto ready;	}

    docRtfWriteDestinationBegin( rwc, "rtf1\\ansi" );
    docRtfWriteNextLine( rwc );

    if  ( docGetFirstNoteOfDocument( &dn, bd, DOCinFOOTNOTE ) )
	{
	hasFootNotes= 1;
	PROPmaskADD( &dpSaveMask, DPpropFOOTNOTE_JUSTIFICATION );
	}

    if  ( docGetFirstNoteOfDocument( &dn, bd, DOCinENDNOTE ) )
	{
	hasEndNotes= 1;
	PROPmaskADD( &dpSaveMask, DPpropENDNOTE_JUSTIFICATION );
	}

    if  ( hasEndNotes )
	{
	if  ( hasFootNotes )
	    { fet= 2;	}
	else{ fet= 1;	}
	}
    else{ fet= 0;	}

    if  ( docRtfSaveDocumentProperties( rwc, fet,
					&dpSaveMask, &(bd->bdProperties) ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( ds )
	{
	docRtfWriteDestinationBegin( rwc, RTFtag__SelOpen );
	docRtfWriteDestinationEnd( rwc );
	}

    if  ( docRtfSaveDocNotesSeparators( rwc, bd ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( docRtfWriteSelection( rwc, ds ) )
	{ LDEB(1); rval= -1; goto ready; }

    docRtfWriteDestinationEnd( rwc );
    docRtfWriteNextLine( rwc );

  ready:

    if  ( rwc )
	{ docRtfCloseWriter( rwc );	}

    return rval;
    }

