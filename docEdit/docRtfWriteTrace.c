/************************************************************************/
/*									*/
/*  Trace modifications to a document.					*/
/*									*/
/************************************************************************/

#   include	"docEditConfig.h"

#   include	<stdio.h>
#   include	<ctype.h>
#   include	<string.h>

#   include	<appDebugon.h>

#   include	<sioFd.h>
#   include	<utilMD5.h>

#   include	"docRtfTrace.h"
#   include	"docEditCommand.h"

#   include	<docRtfTraceImpl.h>
#   include	<docRtfWriterImpl.h>
#   include	<docRtfFlags.h>
#   include	<docRtfTags.h>
#   include	<docTreeType.h>

/************************************************************************/
/*									*/
/*  Save contents.							*/
/*									*/
/************************************************************************/

static int docRtfTraceContents(	RtfWriter *			rw,
				const char *			tag,
				const DocumentSelection *	ds )
    {
    int			rval= 0;

    int			saveContents= 0;

    if  ( ! docIsIBarSelection( ds )			||
	  ( rw->rwSaveFlags & RTFflagSAVE_SOMETHING )	)
	{ saveContents= 1;	}

    if  ( saveContents )
	{
	docRtfWriteNextLine( rw );
	docRtfWriteDestinationBegin( rw, tag );

#	if 0
	Why?
	{
	PropertyMask	dpSaveMask;

	if  ( docRtfDocPropMask( &dpSaveMask,
					&(rw->rwDocument->bdProperties) ) )
	    { LDEB(1); rval= -1; goto ready;	}

	if  ( docRtfSaveDocumentProperties( rw, &dpSaveMask,
					&(rw->rwDocument->bdProperties) ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}
#	endif

	if  ( ds )
	    {
	    docRtfWriteDestinationBegin( rw, RTFtag__SelOpen );
	    docRtfWriteDestinationEnd( rw );
	    }

	docRtfWriteSwitchToPlain( rw );
	docCleanParagraphProperties( &(rw->rwcParagraphProperties) );
	docInitParagraphProperties( &(rw->rwcParagraphProperties) );

	if  ( docRtfWriteSelection( rw, ds ) )
	    { LDEB(1); rval= -1; goto ready;	}

	docRtfWriteNextLine( rw );
	docRtfWriteDestinationEnd( rw );
	}

  ready:

    return rval;
    }

/************************************************************************/
/*									*/
/*  Save the new properties of a property update.			*/
/*									*/
/************************************************************************/

int docRtfTraceNewProperties(	EditOperation *			eo,
				const PropertyMask *		taSetMask,
				const TextAttribute *		taSet,

				const PropertyMask *		ppSetMask,
				const ParagraphProperties *	ppSet,

				const PropertyMask *		cpSetMask,
				const CellProperties *		cpSet,

				const PropertyMask *		rpSetMask,
				const RowProperties *		rpSet,

				const PropertyMask *		spSetMask,
				const SectionProperties *	spSet,

				const PropertyMask *		dpSetMask,
				const DocumentProperties *	dpSet )
    {
    RtfWriter *	rw= eo->eoTraceWriter;

    if  ( taSetMask && utilPropMaskIsEmpty( taSetMask ) )
	{ taSetMask= (const PropertyMask *)0;	}
    if  ( ppSetMask && utilPropMaskIsEmpty( ppSetMask ) )
	{ ppSetMask= (const PropertyMask *)0;	}
    if  ( cpSetMask && utilPropMaskIsEmpty( cpSetMask ) )
	{ cpSetMask= (const PropertyMask *)0;	}
    if  ( rpSetMask && utilPropMaskIsEmpty( rpSetMask ) )
	{ rpSetMask= (const PropertyMask *)0;	}
    if  ( spSetMask && utilPropMaskIsEmpty( spSetMask ) )
	{ spSetMask= (const PropertyMask *)0;	}
    if  ( dpSetMask && utilPropMaskIsEmpty( dpSetMask ) )
	{ dpSetMask= (const PropertyMask *)0;	}

    docRtfWriteNextLine( rw );
    docRtfWriteDestinationBegin( rw, RTFtag_NPR );

    if  ( dpSetMask )
	{ docRtfSaveDocumentProperties( rw, -1, dpSetMask, dpSet );	}
    if  ( spSetMask )
	{ docRtfSaveSectionProperties( rw, spSetMask, spSet );		}
    if  ( rpSetMask )
	{
	const int	col0= -1, col1= -1;

	docRtfSaveRowProperties( rw, rpSetMask, rpSet, col0, col1 );
	}
    if  ( cpSetMask )
	{
	const int	shiftLeft= 0;

	docRtfSaveCellProperties( rw, cpSetMask, cpSet, shiftLeft );
	}

    docRtfWriteTag( rw, RTFtag_pard );

    if  ( ppSetMask )
	{ docRtfSaveParagraphProperties( rw, ppSetMask, ppSet );	}

    docRtfWriteTag( rw, RTFtag_plain );

    if  ( taSetMask )
	{ docRtfSaveTextAttribute( rw, taSetMask, taSet );		}

    /* NO: just the properties docRtfWriteTag( rw, RTFtag_par ); */
    docRtfWriteNextLine( rw );

    docRtfWriteDestinationEnd( rw );
    return 0;
    }

/************************************************************************/
/*									*/
/*  Keep track of image properties.					*/
/*									*/
/************************************************************************/

static int docRtfTraceImageProperties(
				EditOperation *			eo,
				const char *			tag,
				const PropertyMask *		pipSetMask,
				const PictureProperties *	pipSet )
    {
    RtfWriter *	rw= eo->eoTraceWriter;

    docRtfWriteNextLine( rw );
    docRtfWriteDestinationBegin( rw, tag );
    docRtfWriteDestinationBegin( rw, RTFtag_pict );

    if  ( docRtfSavePictureTags( rw, pipSetMask, pipSet ) )
	{ LDEB(1); return -1;	}

    docRtfWriteDestinationEnd( rw );
    docRtfWriteDestinationEnd( rw );
    return 0;
    }

int docRtfTraceOldImageProperties(
				EditOperation *			eo,
				const PropertyMask *		pipSetMask,
				const PictureProperties *	pipSet )
    {
    return docRtfTraceImageProperties( eo, RTFtag_OTX,
							pipSetMask, pipSet );
    }

int docRtfTraceNewImageProperties(
				EditOperation *			eo,
				const PropertyMask *		pipSetMask,
				const PictureProperties *	pipSet )
    {
    return docRtfTraceImageProperties( eo, RTFtag_NTX,
							pipSetMask, pipSet );
    }

static int docRtfTraceList(	EditOperation *		eo,
				const char *		tag,
				const DocumentList *	dl )
    {
    RtfWriter *	rw= eo->eoTraceWriter;

    docRtfWriteNextLine( rw );
    docRtfWriteDestinationBegin( rw, tag );

    if  ( docRtfWriteListProps( rw, dl ) )
	{ LDEB(1); return -1;	}

    docRtfWriteDestinationEnd( rw );
    return 0;
    }

int docRtfTraceOldList(		EditOperation *			eo,
				const DocumentList *		dl )
    {
    return docRtfTraceList( eo, RTFtag_OTX, dl );
    }

int docRtfTraceNewList(		EditOperation *			eo,
				const DocumentList *		dl )
    {
    return docRtfTraceList( eo, RTFtag_NTX, dl );
    }

/************************************************************************/
/*									*/
/*  Keep track of note properties.					*/
/*									*/
/************************************************************************/

static int docRtfTraceNoteProperties(
				EditOperation *			eo,
				const char *			textTag,
				const PropertyMask *		npSetMask,
				const NoteProperties *		npSet )
    {
    RtfWriter *	rw= eo->eoTraceWriter;

    docRtfWriteNextLine( rw );
    docRtfWriteDestinationBegin( rw, textTag );

    if  ( npSet->npAutoNumber )
	{ docRtfWriteTag( rw, "chftn" );	}
    else{
	docRtfWriteFontEncodedString( rw,
				(const char *)npSet->npFixedText.mbBytes,
				npSet->npFixedText.mbSize );
	}

    if  ( PROPmaskISSET( npSetMask, NOTEpropAUTO_NUMBER ) )
	{
	docRtfWriteFlagTag( rw, "FtnAuto", npSet->npAutoNumber );
	}

    docRtfWriteDestinationBegin( rw, RTFtag_footnote );

    if  ( PROPmaskISSET( npSetMask, NOTEpropTREE_TYPE ) )
	{
	docRtfWriteAltTag( rw, "ftnalt", "NoFtnalt",
					npSet->npTreeType == DOCinENDNOTE );
	}

    docRtfWriteFontEncodedString( rw, "?", 1 );

    docRtfWriteDestinationEnd( rw );
    docRtfWriteDestinationEnd( rw );

    return 0;
    }

int docRtfTraceOldNoteProperties(
				EditOperation *			eo,
				const PropertyMask *		npSetMask,
				const NoteProperties *		npSet )
    {
    return docRtfTraceNoteProperties( eo, RTFtag_OTX,
							npSetMask, npSet );
    }

int docRtfTraceNewNoteProperties(
				EditOperation *			eo,
				const PropertyMask *		npSetMask,
				const NoteProperties *		npSet )
    {
    return docRtfTraceNoteProperties( eo, RTFtag_NTX,
							npSetMask, npSet );
    }

/************************************************************************/
/*									*/
/*  Save the old data before a modification.				*/
/*									*/
/************************************************************************/

int docRtfTraceOldContentsLow(	const EditOperation *		eo,
				const DocumentSelection *	ds,
				unsigned int			flags )
    {
    int			rval= 0;

    const char *	tag= RTFtag_OTX;

    RtfWriter *		rw= eo->eoTraceWriter;
    int			savedFlags= rw->rwSaveFlags;

    rw->rwSaveFlags |= ( flags | RTFflagUNENCODED );

    rval= docRtfTraceContents( rw, tag, ds );

    rw->rwSaveFlags= savedFlags;

    return rval;
    }

/************************************************************************/
/*									*/
/*  Save the old data before a modification.				*/
/*									*/
/************************************************************************/

int docRtfTraceOldContents(	DocumentSelection *		ds,
				const EditOperation *		eo,
				int				level,
				unsigned int			flags )
    {
    const int		direction= 0;

    docEditOperationGetSelection( ds, eo );

    switch( level )
	{
	case DOClevSPAN:
	    return docRtfTraceOldContentsLow( eo, ds, flags );
	    break;
	case DOClevPARA:
	    docSelectWholeParagraph( ds, direction );
	    return docRtfTraceOldContentsLow( eo, ds, flags );
	    break;

	case DOClevCELL:
	    docSelectWholeCell( ds, direction, 0 );
	    return docRtfTraceOldContentsLow( eo, ds, flags );
	    break;
	case DOClevCOLUMN:
	    docSelectWholeCell( ds, direction, 1 );
	    return docRtfTraceOldContentsLow( eo, ds, flags );
	    break;
	case DOClevROW:
	    docSelectRow( ds, direction, 1 );
	    return docRtfTraceOldContentsLow( eo, ds, flags );
	    break;
	case DOClevTABLE:
	    docSelectWholeTable( ds );
	    return docRtfTraceOldContentsLow( eo, ds, flags );
	    break;

	case DOClevSECT:
	    docSelectWholeSection( ds, direction );
	    return docRtfTraceOldContentsLow( eo, ds, flags );
	    break;
	default:
	    LDEB(level); return -1;
	}
    }

int docRtfTraceOldProperties(	DocumentSelection *		ds,
				EditOperation *			eo,
				int				level,
				const PropertyMask *		taSetMask,
				const PropertyMask *		ppSetMask,
				const PropertyMask *		cpSetMask,
				const PropertyMask *		rpSetMask,
				const PropertyMask *		spSetMask,
				const PropertyMask *		dpSetMask )
    {
    RtfWriter *	rw= eo->eoTraceWriter;

    int		flags= RTFflagSAVE_SOMETHING;

    if  ( taSetMask && utilPropMaskIsEmpty( taSetMask ) )
	{ taSetMask= (const PropertyMask *)0;	}
    if  ( ppSetMask && utilPropMaskIsEmpty( ppSetMask ) )
	{ ppSetMask= (const PropertyMask *)0;	}
    if  ( cpSetMask && utilPropMaskIsEmpty( cpSetMask ) )
	{ cpSetMask= (const PropertyMask *)0;	}
    if  ( rpSetMask && utilPropMaskIsEmpty( rpSetMask ) )
	{ rpSetMask= (const PropertyMask *)0;	}
    if  ( spSetMask && utilPropMaskIsEmpty( spSetMask ) )
	{ spSetMask= (const PropertyMask *)0;	}
    if  ( dpSetMask && utilPropMaskIsEmpty( dpSetMask ) )
	{ dpSetMask= (const PropertyMask *)0;	}

    if  ( taSetMask )
	{ flags |= RTFflagSAVE_SOMETHING;	}
    else{
	flags |= RTFflagOMIT_TEXT;

	if  ( ! ppSetMask )
	    {
	    flags |= RTFflagOMIT_PARAS;
	    }
	}

    if  ( cpSetMask || rpSetMask )
	{ flags |= RTFflagEMIT_CELL;	}

    rw->rwPpExtraMask= ppSetMask;
    rw->rwCpExtraMask= cpSetMask;
    rw->rwRpExtraMask= rpSetMask;
    rw->rwSpExtraMask= spSetMask;

    docRtfWriteArgTag( eo->eoTraceWriter, RTFtag_PRL, level );

    if  ( docRtfTraceOldContents( ds, eo, level, flags ) )
	{ LDEB(1); return -1;	}

    rw->rwPpExtraMask= (const PropertyMask *)0;
    rw->rwCpExtraMask= (const PropertyMask *)0;
    rw->rwRpExtraMask= (const PropertyMask *)0;
    rw->rwSpExtraMask= (const PropertyMask *)0;

    return 0;
    }

int docRtfTraceHeaderFooter(	EditOperation *			eo,
				const DocumentTree *		dt )
    {
    RtfWriter *	rw= eo->eoTraceWriter;

    docRtfWriteNextLine( rw );
    docRtfWriteDestinationBegin( rw, RTFtag_OTX );

    if  ( dt->dtPageSelectedUpon >= 0 )
	{
	docRtfWriteArgTag( eo->eoTraceWriter, RTFtag_OPG,
						    dt->dtPageSelectedUpon );
	if  ( dt->dtColumnSelectedIn >= 0 )
	    {
	    docRtfWriteArgTag( eo->eoTraceWriter, RTFtag_OCO,
						    dt->dtColumnSelectedIn );
	    }
	}

    if  ( docRtfSaveHeaderFooter( rw, dt ) )
	{ LDEB(1); return -1;	}

    docRtfWriteTag( rw, RTFtag_pard );
    docRtfWriteTag( rw, RTFtag_plain );
    docRtfWriteTag( rw, RTFtag_par );
    docRtfWriteNextLine( rw );

    docRtfWriteDestinationEnd( rw );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Save the new data after a modification.				*/
/*									*/
/************************************************************************/

int docRtfTraceNewContents(	EditOperation *			eo,
				int				posWhere )
    {
    int			rval= 0;
    EditRange *		er= &(eo->eoAffectedRange);
    DocumentSelection	ds;

    RtfWriter *		rw= eo->eoTraceWriter;
    int			savedFlags= rw->rwSaveFlags;

    docSelectionForEditPositionsInTree( &ds, eo->eoTree,
						&(er->erHead), &(er->erTail) );

    rw->rwSaveFlags |= RTFflagUNENCODED;

    if  ( docRtfTraceContents( eo->eoTraceWriter, RTFtag_NTX, &ds ) )
	{ LDEB(1); rval= -1; goto ready;	}

    docRtfTraceNewPosition( eo, (const SelectionScope *)0, posWhere );

  ready:

    rw->rwSaveFlags= savedFlags;

    return rval;
    }

/************************************************************************/
/*									*/
/*  The tags that indicate where the selection was placed after the	*/
/*  modification.							*/
/*									*/
/************************************************************************/

static const char * const DOCrtfSEL_TAGS[]=
    {
    RTFtag_SEH,
    RTFtag_SEN,
    RTFtag_SAL,
    RTFtag_SET,
    RTFtag_SEB,
    RTFtag_SEA
    };

static const int DOCrtfSEL_TAG_COUNT= sizeof(DOCrtfSEL_TAGS)/sizeof(char *);

/************************************************************************/
/*									*/
/*  Trace the position at the end of the exit operation.		*/
/*  If we switch tree (header,footer,note,body) rememer so.		*/
/*									*/
/************************************************************************/

int docRtfTraceNewPosition(	EditOperation *			eo,
				const SelectionScope *		ssNew,
				int				posWhere )
    {
    EditRange *		er= &(eo->eoAffectedRange);

    if  ( ssNew )
	{
	docRtfWriteArgTag( eo->eoTraceWriter, RTFtag_NTT,
							ssNew->ssTreeType );
	docRtfWriteArgTag( eo->eoTraceWriter, RTFtag_NSN,
							ssNew->ssSectNr );
	docRtfWriteArgTag( eo->eoTraceWriter, RTFtag_NOSN,
							ssNew->ssOwnerSectNr );
	docRtfWriteArgTag( eo->eoTraceWriter, RTFtag_NON,
							ssNew->ssOwnerNumber );
	}

    docRtfWriteArgTag( eo->eoTraceWriter, RTFtag_NHP, er->erHead.epParaNr );
    docRtfWriteArgTag( eo->eoTraceWriter, RTFtag_NHS, er->erHead.epStroff );
    docRtfWriteArgTag( eo->eoTraceWriter, RTFtag_NTP, er->erTail.epParaNr );
    docRtfWriteArgTag( eo->eoTraceWriter, RTFtag_NTS, er->erTail.epStroff );

    if  ( posWhere >= 0 && posWhere < DOCrtfSEL_TAG_COUNT )
	{ docRtfWriteTag( eo->eoTraceWriter, DOCrtfSEL_TAGS[posWhere] ); }
    else{ LLDEB(posWhere,DOCrtfSEL_TAG_COUNT);				}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Start a trace item.							*/
/*  1)  Remember the operation.						*/
/*  2)  Remember the selection scope: In what tree was the selection?	*/
/*  3)  Remember the selection.						*/
/*									*/
/************************************************************************/

static int docTraceStartStepLow( EditOperation *		eo,
				EditTrace *			et,
				int				command,
				int				fieldKind )
    {
    int			flags= RTFflagUNENCODED;
    const char *	tag= docRtfGetTraceTag( command );

    if  ( ! tag )
	{ LXDEB(command,tag); return -1;	}

    docInitTraceStep( &(et->etThisStep) );
    et->etThisStep.tsCommand= command;
    et->etThisStep.tsFieldKind= fieldKind;
    et->etThisStep.tsTraceOffset= 0;

    et->etCount= et->etIndex;

    if  ( et->etIndex > 0 )
	{
	const TraceStep *	ts;

	ts= (const TraceStep *)utilPagedListGetItemByNumber(
				    &(et->etTraceSteps), et->etIndex- 1 );
	if  ( ! ts )
	    { LXDEB(et->etIndex,ts); return -1;	}

	et->etThisStep.tsTraceOffset= ts->tsTraceOffset+ ts->tsByteCount;
	}

    eo->eoTraceStream= sioOutFdOpenAt( et->etTraceFileHandle,
						et->etThisStep.tsTraceOffset );
    if  ( ! eo->eoTraceStream )
	{ XDEB(eo->eoTraceStream); return -1;	}
    eo->eoTraceWriter= docRtfOpenWriter( eo->eoTraceStream,
						    eo->eoDocument, flags );
    if  ( ! eo->eoTraceWriter )
	{ XDEB(eo->eoTraceWriter); return -1;	}

    /*  1  */
    docRtfWriteDestinationBegin( eo->eoTraceWriter, tag );

    return 0;
    }

int docTraceStartStep(	EditOperation *			eo,
			EditTrace *			et,
			int				command,
			int				fieldKind )
    {
    if  ( docTraceStartStepLow( eo, et, command, fieldKind ) )
	{ LDEB(command); return -1;	}

    /*  2  */
    docRtfWriteArgTag( eo->eoTraceWriter, RTFtag_OTT,
					eo->eoSelectionScope.ssTreeType );
    docRtfWriteArgTag( eo->eoTraceWriter, RTFtag_OSN,
					eo->eoSelectionScope.ssSectNr );
    docRtfWriteArgTag( eo->eoTraceWriter, RTFtag_OOSN,
					eo->eoSelectionScope.ssOwnerSectNr );
    docRtfWriteArgTag( eo->eoTraceWriter, RTFtag_OON,
					eo->eoSelectionScope.ssOwnerNumber );

    /*  3  */
    docRtfWriteArgTag( eo->eoTraceWriter, RTFtag_OHP,
					eo->eoSelectedRange.erHead.epParaNr );
    docRtfWriteArgTag( eo->eoTraceWriter, RTFtag_OHS,
					eo->eoSelectedRange.erHead.epStroff );
    docRtfWriteArgTag( eo->eoTraceWriter, RTFtag_OTP,
					eo->eoSelectedRange.erTail.epParaNr );
    docRtfWriteArgTag( eo->eoTraceWriter, RTFtag_OTS,
					eo->eoSelectedRange.erTail.epStroff );

    if  ( eo->eoCol0 >= 0 )
	{
	docRtfWriteArgTag( eo->eoTraceWriter, RTFtag_OHC, eo->eoCol0 );
	}

    if  ( eo->eoCol1 >= 0 )
	{
	docRtfWriteArgTag( eo->eoTraceWriter, RTFtag_OTC, eo->eoCol1 );
	}

    return 0;
    }

int docTraceStartReplace(	DocumentSelection *		dsTraced,
				EditOperation *			eo,
				EditTrace *			et,
				int				command,
				int				level,
				unsigned int			flags )
    {
    if  ( docTraceStartStep( eo, et, command, -1 ) )
	{ LDEB(1); return -1;	}

    if  ( docRtfTraceOldContents( dsTraced, eo, level, flags ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Extend a replace.							*/
/*									*/
/*  Replacements start with a regular replace step. If the user		*/
/*  continues to type at the end of the replacement, the replace is	*/
/*  extended. As long as the user types 'the same word', the extent is	*/
/*  replaced with an extent that holds the whole extent.		*/
/*									*/
/************************************************************************/

int docTraceExtendReplace(	EditOperation *			eo,
				EditTrace *			et,
				int				command,
				int				level,
				unsigned int			flags )
    {
    int			rval= 0;
    const TraceStep *	ts;

    const int		fieldKind= -1;

    if  ( et->etIndex < 1 )
	{ LDEB(et->etIndex); return -1;		}

    ts= (const TraceStep *)utilPagedListGetItemByNumber(
				    &(et->etTraceSteps), et->etIndex- 1 );
    if  ( ! ts )
	{ LXDEB(et->etIndex,ts); return -1;	}

    switch( ts->tsCommand )
	{
	case EDITcmdREPLACE:
	    break;

	case EDITcmdEXTEND_REPLACE:
	    if  ( et->etIndex < 2 )
		{ LLDEB(ts->tsCommand,et->etIndex); return -1;	}
	    et->etIndex--;
	    break;

	default:
	    LLDEB(command,ts->tsCommand); return -1;
	}

    if  ( docTraceStartStepLow( eo, et, command, fieldKind ) )
	{ LDEB(1); return -1;	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Remember the old/new field instructions of a field.			*/
/*									*/
/************************************************************************/

static int docRtfTraceFieldImpl(	EditOperation *			eo,
					const DocumentField *		df,
					const char *			tag )
    {
    RtfWriter *	rw= 	eo->eoTraceWriter;

    DocumentSelection	dsInsideField;
    DocumentSelection	dsAroundField;

    int			headPart;
    int			tailPart;

    if  ( docDelimitFieldInDoc( &dsInsideField, &dsAroundField,
				&headPart, &tailPart, eo->eoDocument, df ) )
	{ LDEB(1); return -1; }

    docRtfWriteNextLine( rw );
    docRtfWriteDestinationBegin( rw, tag );

    docRtfWriteTag( rw, RTFtag_pard );
    docRtfWriteTag( rw, RTFtag_plain );
    docRtfSaveFieldHead( rw, dsAroundField.dsHead.dpNode, headPart );
    docRtfWriteRawBytes( rw, " ", 1 );
    docRtfSaveFieldTail( rw, dsAroundField.dsTail.dpNode, tailPart );
    docRtfWriteTag( rw, RTFtag_par );

    docRtfWriteDestinationEnd( rw );

    return 0;
    }

int docRtfTraceFieldKind(	EditOperation *			eo,
				int				kind )

    {
    docRtfWriteArgTag( eo->eoTraceWriter, RTFtag_FLK, kind );
    return 0;
    }

int docRtfTraceOldField(	EditOperation *			eo,
				const DocumentField *		df )

    {
    docRtfTraceFieldKind( eo, df->dfKind );

    return docRtfTraceFieldImpl( eo, df, RTFtag_OTX );
    }

int docRtfTraceNewField(	EditOperation *			eo,
				const DocumentField *		df )
    { return docRtfTraceFieldImpl( eo, df, RTFtag_NTX );	}

/************************************************************************/
/*									*/
/*  Finish a trace step.						*/
/*									*/
/************************************************************************/

int docRtfTraceCloseTrace(	EditOperation *			eo,
				EditTrace *			et )
    {
    TraceStep *	ts= (TraceStep *)0;

    docRtfWriteDestinationEnd( eo->eoTraceWriter );
    docRtfWriteNextLine( eo->eoTraceWriter );

    docRtfCloseWriter( eo->eoTraceWriter );
    eo->eoTraceWriter= (RtfWriter *)0;

    et->etThisStep.tsByteCount= sioOutGetBytesWritten( eo->eoTraceStream );

    sioOutClose( eo->eoTraceStream );
    eo->eoTraceStream= (SimpleOutputStream *)0;

    ts= utilPagedListClaimItem( &(et->etTraceSteps), et->etIndex );
    if  ( ! ts )
	{ LXDEB(et->etIndex,ts); return -1;	}
    et->etIndex++;
    et->etCount++;

    *ts= et->etThisStep;

    utilPagedListSetSize( &(et->etTraceSteps), et->etCount );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Start a trace: Store the digest and the current date and time.	*/
/*									*/
/************************************************************************/

int docRtfTraceVersion(		int			command,
				const MemoryBuffer *	filename,
				const char *		digest64,
				const struct tm *	revtim,
				EditTrace *		et )
    {
    int			rval= 0;

    EditOperation	eo;
    RtfWriter *		rw;

    const int		fieldKind= -1;
    const int		addSemicolon= 0;

    docInitEditOperation( &eo );

    if  ( docTraceStartStepLow( &eo, et, command, fieldKind ) )
	{ LDEB(command); rval= -1; goto ready;	}

    rw= eo.eoTraceWriter;

    /*  HACK: Must go to the props.. not the document */
    docRtfWriteDestinationBegin( rw, RTFtag_NPR );

    docRtfSaveDate( rw, RTFtag_revtim, revtim );

    if  ( digest64 )
	{
	docRtfWriteDocEncodedStringDestination( rw, RTFtag__generator,
				digest64, strlen(digest64), addSemicolon );
	}

    if  ( filename && ! utilMemoryBufferIsEmpty( filename ) )
	{
	const char *	fn= utilMemoryBufferGetString( filename );

	docRtfWriteDocEncodedStringDestination( rw, RTFtag_hlinkbase,
				fn, strlen(fn), addSemicolon );
	}

    docRtfWriteDestinationEnd( rw );

    if  ( docRtfTraceCloseTrace( &eo, et ) )
	{ LDEB(command); rval= -1; goto ready;	}

  ready:

    docCleanEditOperation( &eo );

    return rval;
    }

