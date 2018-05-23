/************************************************************************/
/*									*/
/*  Trace modifications to a document.					*/
/*									*/
/************************************************************************/

#   include	"docEditConfig.h"

#   include	<stdio.h>
#   include	<ctype.h>

#   include	<appDebugon.h>

#   include	<sioFd.h>

#   include	"docRtfTrace.h"
#   include	"docEditCommand.h"

#   include	<docRtfTraceImpl.h>
#   include	<docRtfReaderImpl.h>
#   include	<docTreeType.h>
#   include	<docRtfTags.h>
#   include	<docRtfFlags.h>

/************************************************************************/

static int docRtfReadVersion(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rrc )
    {
    int		res;

    if  ( rcw->rcwID == rrc->rrcTraceReadWhat )
	{
	if  ( rrc->rrcTraceCommand != EDITcmdUPD_NOTE )
	    { rrc->rrcSelectionScope.ssTreeType= DOCinBODY;	}

	utilPropMaskClear( &(rrc->rrcPicturePropMask) );
	docInitPictureProperties( &(rrc->rrcPictureProperties) );

	res= docRtfReadGroup( rcw, 0, 0, rrc,
				docRtfDocumentGroups,
				docRtfTextParticule,
				(RtfCommitGroup)0 );
	if  ( res )
	    { SLDEB(rcw->rcwWord,res);	}
	}
    else{
	res= docRtfSkipGroup( rcw, arg, rrc );
	if  ( res )
	    { SLDEB(rcw->rcwWord,res);	}
	}

    return res;
    }

static int docRtfCommitNewProps(	const RtfControlWord *	rcw,
					RtfReader *		rrc )
    {
    if  ( docRtfStoreStyleProperties( rrc ) )
	{ LDEB(1);	}

    return 0;
    }

static int docRtfReadNewProps(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rrc )
    {
    RtfReadingState *	rrs= rrc->rrcState;
    int			res;

    docCleanDocumentStyle( &(rrc->rrcStyle) );
    docInitDocumentStyle( &(rrc->rrcStyle) );

    docRtfResetParagraphProperties( rrs );
    docRtfResetTextAttribute( rrs, rrc->rrDocument );

    rrc->rrcTraceInProps++;

    res= docRtfReadGroup( rcw, 0, 0, rrc,
			    docRtfDocumentGroups,
			    docRtfRefuseText,
			    docRtfCommitNewProps );
    if  ( res )
	{ SLDEB(rcw->rcwWord,res);	}

    rrc->rrcTraceInProps--;

    docRtfResetParagraphProperties( rrs );
    docRtfResetTextAttribute( rrs, rrc->rrDocument );

    return res;
    }

static RtfControlWord	docRtfTraceDetailGroups[]=
    {
	RTF_DEST_XX( RTFtag_NTX,	 1, docRtfReadVersion ),
	RTF_DEST_XX( RTFtag_NPR,	 1, docRtfReadNewProps ),
	RTF_DEST_XX( RTFtag_OTX,	-1, docRtfReadVersion ),

	{ (char *)0, 0, 0 }
    };

/************************************************************************/
/*									*/
/*  Read a trace step.							*/
/*									*/
/************************************************************************/

static int docRtfReadTraceStep(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rrc )
    {
    int		res;

    rrc->rrcTraceCommand= rcw->rcwID;

    if  ( rrc->rrcTraceCommand == EDITcmdUPD_NOTE )
	{ rrc->rrcSelectionScope.ssTreeType= DOCinFOOTNOTE;	}

    res= docRtfReadGroup( rcw, 0, 0, rrc,
				docRtfTraceDetailGroups,
				(RtfAddTextParticule)0,
				(RtfCommitGroup)0 );
    if  ( res )
	{ SLDEB(rcw->rcwWord,res);	}

    return res;
    }

/************************************************************************/
/*									*/
/*  Read a trace item.							*/
/*									*/
/************************************************************************/

# define RTF_TRACE_CMD( word, id ) \
		    RTF_DEST_XX( (word), (id), docRtfReadTraceStep )

static RtfControlWord	docRtfTraceGroups[]=
{
    RTF_TRACE_CMD( RTFtag_SLI,		EDITcmdSET_HYPERLINK ),
    RTF_TRACE_CMD( RTFtag_SBK,		EDITcmdSET_BOOKMARK ),
    RTF_TRACE_CMD( RTFtag_DFL,		EDITcmdDEL_FIELD ),
    RTF_TRACE_CMD( RTFtag_UFL,		EDITcmdUPD_FIELD ),
    RTF_TRACE_CMD( RTFtag_ULI,		EDITcmdUPD_LIST ),
    RTF_TRACE_CMD( RTFtag_NLI,		EDITcmdSET_NEW_LIST ),
    RTF_TRACE_CMD( RTFtag_NNO,		EDITcmdINS_NOTE ),
    RTF_TRACE_CMD( RTFtag_UNO,		EDITcmdUPD_NOTE ),
    RTF_TRACE_CMD( RTFtag_UTA,		EDITcmdUPD_SPAN_PROPS ),
    RTF_TRACE_CMD( RTFtag_UPP,		EDITcmdUPD_PARA_PROPS ),
    RTF_TRACE_CMD( RTFtag_UTP,		EDITcmdUPD_TABLE_PROPS ),
    RTF_TRACE_CMD( RTFtag_USP,		EDITcmdUPD_SECT_PROPS ),
    RTF_TRACE_CMD( RTFtag_USDP,		EDITcmdUPD_SECTDOC_PROPS ),
    RTF_TRACE_CMD( RTFtag_UDP,		EDITcmdUPD_DOC_PROPS ),
    RTF_TRACE_CMD( RTFtag_REP,		EDITcmdREPLACE ),
    RTF_TRACE_CMD( RTFtag_REPF,		EDITcmdREPLACE_FTNSEP ),
    RTF_TRACE_CMD( RTFtag_REPB,		EDITcmdREPLACE_BODY_LEVEL ),
    RTF_TRACE_CMD( RTFtag_DELS,		EDITcmdDELETE_SELECTION ),
    RTF_TRACE_CMD( RTFtag_DELF,		EDITcmdDELETE_SELECTION_DEL ),
    RTF_TRACE_CMD( RTFtag_DELB,		EDITcmdDELETE_SELECTION_BS ),
    RTF_TRACE_CMD( RTFtag_REX,		EDITcmdEXTEND_REPLACE ),
    RTF_TRACE_CMD( RTFtag_MPA,		EDITcmdMERGE_PARAS ),

    RTF_TRACE_CMD( RTFtag_DPA,		EDITcmdDELETE_PARA ),
    RTF_TRACE_CMD( RTFtag_DSE,		EDITcmdDELETE_SECT ),
    RTF_TRACE_CMD( RTFtag_DTB,		EDITcmdDELETE_TABLE ),
    RTF_TRACE_CMD( RTFtag_DRW,		EDITcmdDELETE_ROW ),
    RTF_TRACE_CMD( RTFtag_DCO,		EDITcmdDELETE_COLUMN ),

    RTF_TRACE_CMD( RTFtag_IPA,		EDITcmdINSERT_PARA ),
    RTF_TRACE_CMD( RTFtag_APA,		EDITcmdAPPEND_PARA ),
    RTF_TRACE_CMD( RTFtag_ITB,		EDITcmdINSERT_TABLE ),
    RTF_TRACE_CMD( RTFtag_ISE,		EDITcmdINSERT_SECT ),
    RTF_TRACE_CMD( RTFtag_ASE,		EDITcmdAPPEND_SECT ),
    RTF_TRACE_CMD( RTFtag_SSE,		EDITcmdSPLIT_SECT ),

    RTF_TRACE_CMD( RTFtag_IRW,		EDITcmdINSERT_ROW ),
    RTF_TRACE_CMD( RTFtag_ARW,		EDITcmdAPPEND_ROW ),
    RTF_TRACE_CMD( RTFtag_ICO,		EDITcmdINSERT_COLUMN ),
    RTF_TRACE_CMD( RTFtag_ACO,		EDITcmdAPPEND_COLUMN ),

    RTF_TRACE_CMD( RTFtag_IHD,		EDITcmdINSERT_HEADER ),
    RTF_TRACE_CMD( RTFtag_IFT,		EDITcmdINSERT_FOOTER ),
    RTF_TRACE_CMD( RTFtag_DHD,		EDITcmdDELETE_HEADER ),
    RTF_TRACE_CMD( RTFtag_DFT,		EDITcmdDELETE_FOOTER ),

    RTF_TRACE_CMD( RTFtag_UOB,		EDITcmdUPD_OBJECT ),

    RTF_TRACE_CMD( RTFtag_SRWD,		EDITcmdSHIFT_ROWS_DOWN ),
    RTF_TRACE_CMD( RTFtag_SRWU,		EDITcmdSHIFT_ROWS_UP ),
    RTF_TRACE_CMD( RTFtag_SPR,		EDITcmdSHIFT_RIGHT ),
    RTF_TRACE_CMD( RTFtag_SPL,		EDITcmdSHIFT_LEFT ),

    RTF_TRACE_CMD( "",			EDITcmd_EDIT_COUNT ),

    RTF_TRACE_CMD( RTFtag_Open,		EDITcmd_OPEN ),
    RTF_TRACE_CMD( RTFtag_Save,		EDITcmd_SAVE ),
    RTF_TRACE_CMD( RTFtag_New,		EDITcmd_NEW ),

    { (char *)0, 0, 0 }
};

const int docRtfTraceGroupCount=
		    sizeof(docRtfTraceGroups)/sizeof(RtfControlWord)- 1;

const char *	docRtfGetTraceTag(	int	command )
    {
    const RtfControlWord *	rcw;

    if  ( docRtfTraceGroupCount != EDITcmd_COUNT		||
	  command < 0						||
	  command >= docRtfTraceGroupCount			)
	{
	LLLDEB(command,docRtfTraceGroupCount,EDITcmd_COUNT);
	return (const char *)0;
	}

    rcw= docRtfTraceGroups+ command;
    if  ( rcw->rcwID != command )
	{ LLSDEB(command,rcw->rcwID,rcw->rcwWord); return (const char *)0; }

    return rcw->rcwWord;
    }

static int docRtfReadEditStep(	EditStep *			es,
				SimpleInputStream *		sis,
				int				readOld,
				int				readNew,
				const BufferDocument *		bdRef )
    {
    int				rval= 0;
    int				res;
    BufferDocument *		bd= (BufferDocument *)0;
    RtfReader *			rr= (RtfReader *)0;
    const RtfControlWord *	rcw;

    char			controlWord[TEDszRTFCONTROL+1];
    int				gotArg;
    int				arg= -1;
    int				readWhat= 0;
    int				c;

    const int			recursively= 1;

    const DocumentAttributeMap * const dam0= (const DocumentAttributeMap *)0;

    if  ( readOld && readNew )
	{ LLDEB(readOld,readNew); rval= -1; goto ready;	}
    else{
	if  ( readOld )
	    { readWhat= -1;	}
	if  ( readNew )
	    { readWhat=  1;	}
	}

    bd= docEditStepMakeSourceDocument( es, bdRef );
    if  ( ! bd )
	{ XDEB(bd); rval= -1; goto ready;	}

    rr= docRtfOpenReader( sis, bd, RTFflagUNENCODED );

    rr->rrcTree= &(rr->rrDocument->bdBody);
    rr->rrcNode= rr->rrcTree->dtRoot;
    rr->rrcLevel= DOClevBODY;
    rr->rrcTraceReadWhat= readWhat;
    rr->rrcTraceSelectionPosition= SELposTAIL;
    rr->rrcTraceFieldKind= -1;

    /* Make shallow copies of the document properties */
    if  ( bdRef )
	{
	bd->bdProperties.dpDefaultFont= bdRef->bdProperties.dpDefaultFont;
	}

    res= docRtfFindControl( rr, &c, controlWord, &gotArg, &arg );
    if  ( res != RTFfiCTRLGROUP )
	{ LDEB(res); rval= -1; goto ready; }

    rcw= docRtfFindWord( controlWord, docRtfTraceGroups );
    if  ( ! rcw )
	{ SXDEB(controlWord,rcw); rval= -1; goto ready; }
    if  ( rcw->rcwType != RTCtypeDEST )
	{ SLDEB(rcw->rcwWord,rcw->rcwType); rval= -1; goto ready;	}

    res= docRtfApplyControlWord( rcw, gotArg, arg, rr );
    if  ( res )
	{ LDEB(1); rval= -1; goto ready; }

    docDelimitTables( rr->rrDocument->bdBody.dtRoot, recursively );

    es->esCommand= rr->rrcTraceCommand;
    es->esSelectionPosition= rr->rrcTraceSelectionPosition;
    es->esFieldKind= rr->rrcTraceFieldKind;

    es->esOldSelectionScope= rr->rrcTraceOldSelectionScope;
    es->esOldEditRange= rr->rrcTraceOldRange;
    es->esOldCol0= rr->rrcTraceOldCol0;
    es->esOldCol1= rr->rrcTraceOldCol1;
    es->esOldPage= rr->rrcTraceOldPage;
    es->esOldColumn= rr->rrcTraceOldColumn;

    es->esNewSelectionScope= rr->rrcTraceNewSelectionScope;
    es->esNewEditRange= rr->rrcTraceNewRange;
    es->esNewCol0= rr->rrcTraceNewCol0;
    es->esNewCol1= rr->rrcTraceNewCol1;
    es->esNewPage= rr->rrcTraceNewPage;
    es->esNewColumn= rr->rrcTraceNewColumn;

    es->esPropLevel= rr->rrcTracePropLevel;

    if  ( es->esCommand == EDITcmdUPD_SPAN_PROPS	||
	  es->esCommand == EDITcmdUPD_PARA_PROPS	||
	  es->esCommand == EDITcmdUPD_TABLE_PROPS	||
	  es->esCommand == EDITcmdUPD_SECT_PROPS	||
	  es->esCommand == EDITcmdUPD_SECTDOC_PROPS	||
	  es->esCommand == EDITcmdUPD_DOC_PROPS		||
	  es->esCommand == EDITcmdSET_HYPERLINK		||
	  es->esCommand == EDITcmdSET_BOOKMARK		||
	  es->esCommand == EDITcmdDEL_FIELD		||
	  es->esCommand == EDITcmdSET_NEW_LIST		)
	{
	if  ( docUpdRowProperties( &(rr->rrcStyle.dsRowMask),
		    &(rr->rrcStyle.dsRowProps),
		    &(rr->rrcRowPropertyMask), &(rr->rrcRowProperties),
		    (const DocumentAttributeMap *)0 ) )
	    { LDEB(es->esCommand); rval= -1; goto ready;	}

	if  ( docCopyStyle( &(es->esNewStyle), &(rr->rrcStyle), dam0 )	)
	    { LDEB(es->esCommand); rval= -1; goto ready;	}

	if  ( PROPmaskISSET( &(es->esNewStyle.dsRowMask),RPpropCELL_LAYOUT ) ||
	      PROPmaskISSET( &(es->esNewStyle.dsRowMask),RPpropCELL_PROPS ) )
	    { PROPmaskUNSET( &(es->esNewStyle.dsCellMask), CLpropCELLX ); }
	}

    if  ( docUpdDocumentProperties(
	    (PropertyMask *)0, &(es->esNewDocProps),
	    &(rr->rrcDocPropertyMask), &(rr->rrcDocumentProperties), dam0 ) )
	{ LDEB(es->esCommand); rval= -1; goto ready;	}
    es->esNewDocPropMask= rr->rrcDocPropertyMask;

    es->esPicturePropMask= rr->rrcPicturePropMask;

    es->esNotePropMask= rr->rrcNotePropertyMask;
    docInitNoteProperties( &(rr->rrcNoteProperties) );

    es->esDocumentList= rr->rrcDocumentList;
    docInitDocumentList( &(rr->rrcDocumentList) );

  ready:

    if  ( rr )
	{ docRtfCloseReader( rr ); 	}

    return rval;
    }

/************************************************************************/

int docRtfSelectionScopeProperty(	const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rrc )
    {
    SelectionScope *	ssOld= &(rrc->rrcTraceOldSelectionScope);
    SelectionScope *	ssNew= &(rrc->rrcTraceNewSelectionScope);

    switch( rcw->rcwID )
	{
	case SSpropTREE_TYPE:
	    ssOld->ssTreeType= ssNew->ssTreeType= arg;
	    return 0;
	case SSpropSECT_NR:
	    ssOld->ssSectNr= ssNew->ssSectNr= arg;
	    return 0;
	case SSpropOWNER_SECT_NR:
	    ssOld->ssOwnerSectNr= ssNew->ssOwnerSectNr= arg;
	    return 0;
	case SSpropOWNER_NR:
	    ssOld->ssOwnerNumber= ssNew->ssOwnerNumber= arg;
	    return 0;

	case SSpropPAGE:
	    rrc->rrcTraceOldPage= rrc->rrcTraceNewPage= arg;
	    return 0;
	case SSpropCOLUMN:
	    rrc->rrcTraceOldColumn= rrc->rrcTraceNewColumn= arg;
	    return 0;

	case SSprop_COUNT+ SSpropTREE_TYPE:
	    ssNew->ssTreeType= arg;
	    return 0;
	case SSprop_COUNT+ SSpropSECT_NR:
	    ssNew->ssSectNr= arg;
	    return 0;
	case SSprop_COUNT+ SSpropOWNER_SECT_NR:
	    ssNew->ssOwnerSectNr= arg;
	    return 0;
	case SSprop_COUNT+ SSpropOWNER_NR:
	    ssNew->ssOwnerNumber= arg;
	    return 0;

	case SSprop_COUNT+ SSpropPAGE:
	    rrc->rrcTraceNewPage= arg;
	    return 0;
	case SSprop_COUNT+ SSpropCOLUMN:
	    rrc->rrcTraceNewColumn= arg;
	    return 0;

	default:
	    SDEB(rcw->rcwWord); return -1;
	}
    }

int docRtfEditRangeProperty(		const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rrc )
    {
    switch( rcw->rcwID )
	{
	case TRACEposOLD_HEAD_COL:
	    rrc->rrcTraceOldCol0= arg;
	    return 0;
	case TRACEposOLD_HEAD_PARA:
	    rrc->rrcTraceOldRange.erHead.epParaNr= arg;
	    return 0;
	case TRACEposOLD_HEAD_STROFF:
	    rrc->rrcTraceOldRange.erHead.epStroff= arg;
	    return 0;

	case TRACEposOLD_TAIL_COL:
	    rrc->rrcTraceOldCol1= arg;
	    return 0;
	case TRACEposOLD_TAIL_PARA:
	    rrc->rrcTraceOldRange.erTail.epParaNr= arg;
	    return 0;
	case TRACEposOLD_TAIL_STROFF:
	    rrc->rrcTraceOldRange.erTail.epStroff= arg;
	    return 0;

	case TRACEposNEW_HEAD_COL:
	    rrc->rrcTraceNewCol0= arg;
	    return 0;
	case TRACEposNEW_HEAD_PARA:
	    rrc->rrcTraceNewRange.erHead.epParaNr= arg;
	    return 0;
	case TRACEposNEW_HEAD_STROFF:
	    rrc->rrcTraceNewRange.erHead.epStroff= arg;
	    return 0;

	case TRACEposNEW_TAIL_COL:
	    rrc->rrcTraceNewCol1= arg;
	    return 0;
	case TRACEposNEW_TAIL_PARA:
	    rrc->rrcTraceNewRange.erTail.epParaNr= arg;
	    return 0;
	case TRACEposNEW_TAIL_STROFF:
	    rrc->rrcTraceNewRange.erTail.epStroff= arg;
	    return 0;

	case TRACEposSELECTED:
	    rrc->rrcTraceSelectionPosition= rcw->rcwEnumValue;
	    return 0;

	case TRACEposPROP_LEVEL:
	    rrc->rrcTracePropLevel= arg;
	    return 0;
	case TRACEposFIELD_KIND:
	    rrc->rrcTraceFieldKind= arg;
	    return 0;

	default:
	    SDEB(rcw->rcwWord); return -1;
	}
    }

/************************************************************************/

static int docRtfReadTraceStepN( EditStep *			es,
				int				readOld,
				int				readNew,
				const EditTrace *		et,
				int				n,
				const BufferDocument *		bdRef )
    {
    int				rval= 0;
    SimpleInputStream *		sis= (SimpleInputStream *)0;
    const TraceStep *		ts;

    ts= (const TraceStep *)utilPagedListGetItemByNumber(
						    &(et->etTraceSteps), n );
    if  ( ! ts )
	{ LXDEB(n,ts); rval= -1; goto ready;	}

    sis= sioInFdOpenAt( et->etTraceFileHandle, ts->tsTraceOffset );
    if  ( ! sis )
	{ LPDEB(et->etTraceFileHandle,sis); rval= -1; goto ready;	}

    if  ( docRtfReadEditStep( es, sis, readOld, readNew, bdRef ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    if  ( sis )
	{ sioInClose( sis );	}

    return rval;
    }

static int docRtfReadExtendedReplace(
				EditStep *			es,
				int				readOld,
				int				readNew,
				const EditTrace *		et,
				int				n,
				const BufferDocument *		bdRef )
    {
    int			rval= 0;
    EditStep		esExt;

    docInitEditStep( &esExt );

    if  ( docRtfReadTraceStepN( es, readOld, readNew, et, n, bdRef ) )
	{ LDEB(1); rval= -1; goto ready;	}
    if  ( es->esCommand != EDITcmdREPLACE )
	{ LDEB(es->esCommand); rval= -1; goto ready;	}

    if  ( docRtfReadTraceStepN( &esExt, readOld, readNew, et, n+ 1, bdRef ) )
	{ LDEB(1); rval= -1; goto ready;	}
    if  ( esExt.esCommand != EDITcmdEXTEND_REPLACE )
	{ LDEB(es->esCommand); rval= -1; goto ready;	}

    es->esNewEditRange= esExt.esNewEditRange;

    /* Not used
    es->esNewSelectionScope= esExt.esNewSelectionScope;
    es->esNewCol0= esExt.esNewCol0;
    es->esNewCol1= esExt.esNewCol1;
    es->esNewPage= esExt.esNewPage;
    es->esNewColumn= esExt.esNewColumn;
    */

    if  ( readNew )
	{
	BufferDocument *	swap;
	int			inte;

	swap= es->esSourceDocument;
	      es->esSourceDocument= esExt.esSourceDocument;
			            esExt.esSourceDocument= swap;

	inte= es->esSourceIsIntermediary;
	      es->esSourceIsIntermediary= esExt.esSourceIsIntermediary;
					  esExt.esSourceIsIntermediary= inte;
	}

  ready:

    docCleanEditStep( &esExt );

    return rval;
    }

int docEditReadTraceStep(	EditStep *			es,
				int *				pIsRepeat,
				int				direction,
				const EditTrace *		et,
				const BufferDocument *		bdRef )
    {
    int			rval= 0;
    int			n;
    const TraceStep *	ts= (const TraceStep *)0;

    const int		readNew= direction > 0;
    const int		readOld= direction < 0;
    int			isRepeat= 0;

    n= docEditGetTraceStep( &ts, &isRepeat, direction, et, et->etIndex );
    if  ( n < 0 )
	{ LDEB(n); rval= -1; goto ready;	}

    switch( ts->tsCommand )
	{
	case EDITcmdEXTEND_REPLACE:
	    if  ( docRtfReadExtendedReplace( es, readOld, readNew,
							    et, n- 1, bdRef ) )
		{ LDEB(1); rval= -1; goto ready;	}
	    es->esTraceIndex= n- 1;
	    es->esStepCount= 2;
	    break;

	case EDITcmdREPLACE:
	    ts= (const TraceStep *)0;
	    if  ( n < et->etCount- 1 )
		{
		ts= (const TraceStep *)utilPagedListGetItemByNumber(
						&(et->etTraceSteps), n+ 1 );
		if  ( ! ts )
		    { LXDEB(n,ts); rval= -1; goto ready;	}
		}

	    if  ( ts && ts->tsCommand == EDITcmdEXTEND_REPLACE )
		{
		if  ( docRtfReadExtendedReplace( es, readOld, readNew,
								et, n, bdRef ) )
		    { LDEB(1); rval= -1; goto ready;	}
		es->esTraceIndex= n;
		es->esStepCount= 2;
		}
	    else{
		if  ( docRtfReadTraceStepN( es, readOld, readNew,
								et, n, bdRef ) )
		    { LDEB(1); rval= -1; goto ready;	}
		es->esTraceIndex= n;
		es->esStepCount= 1;
		}
	    break;

	default:
	    if  ( docRtfReadTraceStepN( es, readOld, readNew, et, n, bdRef ) )
		{ LDEB(1); rval= -1; goto ready;	}
	    es->esTraceIndex= n;
	    es->esStepCount= 1;
	    break;
	}

    if  ( pIsRepeat )
	{ *pIsRepeat= isRepeat;	}

  ready:

    return rval;
    }

int docRtfScanEditTrace(	const EditTrace *		et,
				SimpleInputStream *		sis,
				HandleEditStep			handleStep,
				void *				through,
				int				readOld,
				int				readNew,
				const BufferDocument *		bdRef )
    {
    int			rval= 0;

    TraceStep		ts;
    EditStep		es;
    int			step= 0;
    int			exhausted= 0;

    docInitTraceStep( &ts );
    docInitEditStep( &es );

    step= 0;
    ts.tsTraceOffset= 0;

    while( ! exhausted )
	{
	int	res;
	long	endOffset;

	ts.tsTraceOffset= sioInGetBytesRead( sis );

	if  ( docRtfReadEditStep( &es, sis, readOld, readNew, bdRef ) )
	    { LDEB(1); rval= -1; goto ready;	}

	{
	int	c= sioInGetByte( sis );

	while ( c == '\r' || c == '\n' )
	    { c= sioInGetByte( sis );	}

	if  ( c == EOF )
	    { exhausted= 1;			}
	else{ sioInUngetLastRead( sis );	}
	}

	endOffset= sioInGetBytesRead( sis );

	ts.tsCommand= es.esCommand;
	ts.tsFieldKind= es.esFieldKind;
	/* ts.tsTraceOffset= .. set above */
	ts.tsByteCount= endOffset- ts.tsTraceOffset;

	res= (*handleStep)( &ts, &es, step, through );
	if  ( res < 0 )
	    { LDEB(res); rval= -1; goto ready;	}
	if  ( res > 0 )
	    { goto ready;	}

	step++;
	docCleanEditStep( &es ); docInitEditStep( &es );
	}

  ready:

    docCleanEditStep( &es );

    return rval;
    }
