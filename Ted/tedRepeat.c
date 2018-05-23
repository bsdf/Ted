/************************************************************************/
/*									*/
/*  Editor Repeat functionality.					*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	"tedEdit.h"
#   include	"tedSelect.h"
#   include	"tedDocFront.h"
#   include	"tedDocument.h"
#   include	<docRtfTrace.h>
#   include	<docSelect.h>
#   include	<docField.h>
#   include	<docTrace.h>
#   include	<docTreeNode.h>
#   include	<docNotes.h>
#   include	<docDocumentNote.h>
#   include	<docNodeTree.h>
#   include	<docEditCommand.h>
#   include	<docRtfTraceImpl.h>
#   include	<docParaParticules.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Repeat/redo insert header/footer.					*/
/*									*/
/*  Simply succeed on xisting headers and footers: Makes the		*/
/*  implementation of the UI logic much simpler.			*/
/*									*/
/************************************************************************/

static int tedRepeatInsertHeaderFooter(	EditDocument *		ed,
					const EditStep *	es,
					int			traced )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;
    DocumentProperties *	dp= &(bd->bdProperties);

    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    DocumentTree *		dt;
    BufferItem *		bodySectNode;
    int				treeType= es->esNewSelectionScope.ssTreeType;

    if  ( tedGetSelection( &ds, &sg, &sd, &dt, &bodySectNode, ed ) )
	{ LDEB(1); return -1;	}

    if  ( treeType != sd.sdHeaderTypeForSelection		&&
    	  treeType != sd.sdFooterTypeForSelection		&&
	  treeType != ds.dsHead.dpNode->biTreeType		)
	{ SDEB(docTreeTypeStr(treeType)); return -1;	}

    if  ( docSectionHasHeaderFooter( bodySectNode, (unsigned char *)0,
							    dp, treeType ) )
	{ /* SDEB(docTreeTypeStr(treeType)); */ return 0;	}

    return tedDocInsertHeaderFooter( ed, treeType, traced );
    }

/************************************************************************/
/*									*/
/*  Repeat/redo delete header/footer.					*/
/*									*/
/*  Simply succeed on missing headers and footers: Makes the		*/
/*  implementation of the UI logic much simpler.			*/
/*									*/
/************************************************************************/

static int tedRepeatDeleteHeaderFooter(	EditDocument *		ed,
					const EditStep *	es,
					int			traced )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;
    DocumentProperties *	dp= &(bd->bdProperties);

    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    DocumentTree *		dt;
    BufferItem *		bodySectNode;
    int				treeType= es->esOldSelectionScope.ssTreeType;

    if  ( tedGetSelection( &ds, &sg, &sd, &dt, &bodySectNode, ed ) )
	{ LDEB(1); return -1;	}

    if  ( treeType != sd.sdHeaderTypeForSelection		&&
    	  treeType != sd.sdFooterTypeForSelection		&&
	  treeType != ds.dsHead.dpNode->biTreeType		)
	{ SDEB(docTreeTypeStr(treeType)); return -1;	}

    if  ( ! docSectionHasHeaderFooter( bodySectNode, (unsigned char *)0,
							    dp, treeType ) )
	{ /* SDEB(docTreeTypeStr(treeType)); */ return 0;	}

    return tedDocDeleteHeaderFooter( ed, treeType, traced );
    }

/************************************************************************/
/*									*/
/*  Repeat the modification of a field.					*/
/*									*/
/************************************************************************/

static int tedRepeatUpdateField(	EditDocument *		ed,
					const EditStep *	es,
					int			traced )
    {
    int				rval= 0;

    TedEditOperation		teo;
    EditOperation *		eo= &(teo.teoEo);
    SelectionGeometry		sg;
    SelectionDescription	sd;

    DocumentSelection		ds;

    const DocumentField *	dfFrom;
    DocumentField *		dfTo;

    const int			fullWidth= 0;

    tedStartEditOperation( &teo, &sg, &sd, ed, fullWidth, traced );

    docEditOperationGetSelection( &ds, eo );

    dfFrom= docTraceGetFromField( es );
    if  ( ! dfFrom )
	{ XDEB(dfFrom); rval= -1; goto ready;	}

    dfTo= docFindTypedFieldForPosition( eo->eoDocument, &(ds.dsHead),
							dfFrom->dfKind, 0 );
    if  ( ! dfTo )
	{ XDEB(dfTo); rval= -1; goto ready;	}

    if  ( tedDocUpdField( &teo, dfTo, &(dfFrom->dfInstructions) ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    tedCleanEditOperation( &teo );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Repeat the creation of a field.					*/
/*									*/
/************************************************************************/

static int tedRepeatSetField(	EditDocument *		ed,
				const EditStep *	es,
				int			traced )
    {
    int				rval= 0;

    TedEditOperation		teo;
    EditOperation *		eo= &(teo.teoEo);
    SelectionGeometry		sg;
    SelectionDescription	sd;

    DocumentSelection		ds;

    const DocumentField *	dfFrom;
    FieldInstructions		fiFrom;

    const int			fullWidth= 0;

    docInitFieldInstructions( &fiFrom );

    tedStartEditOperation( &teo, &sg, &sd, ed, fullWidth, traced );

    docEditOperationGetSelection( &ds, eo );

    dfFrom= docTraceGetFromField( es );
    if  ( ! dfFrom )
	{ XDEB(dfFrom); rval= -1; goto ready;	}

    if  ( docCopyFieldInstructions( &fiFrom, &(dfFrom->dfInstructions) ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( dfFrom->dfKind == DOCfkBOOKMARK				&&
	  docSuggestNewBookmarkName( &(fiFrom.fiComponents[1].icBuffer),
					  eo->eoDocument, &ds ) < 0	)
	{ LDEB(dfFrom->dfKind); rval= -1; goto ready;	}

    if  ( tedDocSetField( &teo, &ds, es->esCommand, dfFrom->dfKind, &fiFrom,
					&(es->esNewStyle.dsTextMask),
					&(es->esNewStyle.dsTextAttribute) ) )
	{ LDEB(dfFrom->dfKind); rval= -1; goto ready;	}

  ready:

    docCleanFieldInstructions( &fiFrom );
    tedCleanEditOperation( &teo );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Repeat a property update.						*/
/*									*/
/************************************************************************/

static int tedRepeatUpdProps(	EditDocument *		ed,
				const EditStep *	es,
				int			traced )
    {
    int				rval= 0;

    TedEditOperation		teo;
    EditOperation *		eo= &(teo.teoEo);
    SelectionGeometry		sg;
    SelectionDescription	sd;

    DocumentSelection		ds;

    const int			fullWidth= 0;

    tedStartEditOperation( &teo, &sg, &sd, ed, fullWidth, traced );

    docEditOperationGetSelection( &ds, eo );

    if  ( tedEditChangeSelectionProperties( &teo, &ds,
		es->esPropLevel, es->esCommand,
		&(es->esNewStyle.dsTextMask), &(es->esNewStyle.dsTextAttribute),
		&(es->esNewStyle.dsParaMask), &(es->esNewStyle.dsParaProps),
		&(es->esNewStyle.dsCellMask), &(es->esNewStyle.dsCellProps),
		&(es->esNewStyle.dsRowMask), &(es->esNewStyle.dsRowProps),
		&(es->esNewStyle.dsSectMask), &(es->esNewStyle.dsSectProps),
		&(es->esNewDocPropMask), &(es->esNewDocProps) ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    tedCleanEditOperation( &teo );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Repeat a object/picture property update.				*/
/*									*/
/************************************************************************/

static int tedRepeatUpdObject(	EditDocument *		ed,
				const EditStep *	es,
				int			traced )
    {
    const PictureProperties *	pipFrom;

    pipFrom= docTraceGetFromPictureProps( es );
    if  ( ! pipFrom )
	{ XDEB(pipFrom); return -1;	}

    return tedDocSetImageProperties( ed, &(es->esPicturePropMask), pipFrom, traced );
    }

/************************************************************************/
/*									*/
/*  Repeat a foot/end note property update.				*/
/*									*/
/************************************************************************/

static int tedRepeatUpdNote(	EditDocument *		ed,
				const EditStep *	es,
				int			traced )
    {
    const DocumentField *	dfFrom;
    const DocumentNote *	dnFrom= (DocumentNote *)0;

    dfFrom= docTraceGetFromField( es );
    if  ( ! dfFrom )
	{ XDEB(dfFrom); return -1;	}
    dnFrom= docGetNoteOfField( dfFrom, es->esSourceDocument );
    if  ( ! dnFrom )
	{ XDEB(dnFrom); return -1;	}

    return tedDocChangeCurrentNote( ed, &(es->esNotePropMask),
					&(dnFrom->dnNoteProperties), traced );
    }

/************************************************************************/
/*									*/
/*  Repeat creation of a note.						*/
/*									*/
/************************************************************************/

static int tedRepeatInsNote(	EditDocument *		ed,
				const EditStep *	es,
				int			traced )
    {
    const DocumentField *	dfFrom;
    const DocumentNote *	dnFrom= (DocumentNote *)0;

    dfFrom= docTraceGetFromField( es );
    if  ( ! dfFrom )
	{ XDEB(dfFrom); return -1;	}
    dnFrom= docGetNoteOfField( dfFrom, es->esSourceDocument );
    if  ( ! dnFrom )
	{ XDEB(dnFrom); return -1;	}

    return tedDocInsertNote( ed, dnFrom->dnNoteProperties.npTreeType, traced );
    }

/************************************************************************/
/*									*/
/*  Repeat insert table.						*/
/*									*/
/************************************************************************/

static int tedRepeatInsertTable(	EditDocument *		ed,
					const EditStep *	es,
					int			traced )
    {
    DocumentPosition	dp;
    BufferItem *	rowNode;

    if  ( docHeadPosition( &dp, es->esSourceDocument->bdBody.dtRoot ) )
	{ LDEB(1); return -1;	}

    rowNode= docGetRowNode( dp.dpNode );
    if  ( ! rowNode )
	{
	if  ( docNextPosition( &dp ) )
	    { XDEB(rowNode); return -1;	}

	rowNode= docGetRowNode( dp.dpNode );
	if  ( ! rowNode )
	    {
	    XDEB(rowNode);
	    docListNode(0,es->esSourceDocument->bdBody.dtRoot,0);
	    return -1;
	    }
	}

    return tedInsertTable( ed,
			    rowNode->biRowTablePast- rowNode->biRowTableFirst,
			    rowNode->biRowCellCount, traced );
    }

/************************************************************************/
/*									*/
/*  Repeat a replace action.						*/
/*									*/
/************************************************************************/

static int tedRepeatReplace(	EditDocument *		ed,
				const EditStep *	es,
				int			traced )
    {
    int				rval= 0;

    DocumentCopyJob		dcj;

    docInitDocumentCopyJob( &dcj );

    if  ( docEditRangeIsIBar( &(es->esNewEditRange) ) )
	{
	if  ( tedDocDeleteSelection( ed, es->esCommand, traced ) )
	    { LDEB(0); rval= -1; goto ready;	}
	}
    else{
	TedEditOperation	teo;
	EditOperation *		eo= &(teo.teoEo);
	SelectionGeometry	sg;
	SelectionDescription	sd;

	const int		fullWidth= 0;

	tedStartEditOperation( &teo, &sg, &sd, ed, fullWidth, traced );

	if  ( docSetTraceDocumentCopyJob( &dcj, eo, es->esSourceDocument ) )
	    { LDEB(1); rval= -1; goto ready;	}

	dcj.dcjCopyHeadParaProperties=
			( sg.sgHead.pgPositionFlags & POSflagPARA_HEAD ) != 0;
	dcj.dcjCopyTailParaProperties=
			( sg.sgTail.pgPositionFlags & POSflagPARA_TAIL ) != 0;

	if  ( tedEditIncludeDocument( &teo, &dcj,
				    es->esCommand, es->esSelectionPosition ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}

  ready:

    docCleanDocumentCopyJob( &dcj );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Redo or Repeat.							*/
/*									*/
/************************************************************************/

int tedDocRepeat(	EditDocument *		ed )
    {
    int				rval= 0;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    EditTrace *			et= &(td->tdEditTrace);
    BufferDocument *		bd= td->tdDocument;

    EditStep			es;
    const int			direction= 1;
    int				isRepeat= 0;

    docInitEditStep( &es );

    if  ( docEditReadTraceStep( &es, &isRepeat, direction, et, bd ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( ! isRepeat )
	{
	DocumentSelection	dsNew;

	if  ( docSelectionForEditPositionsInDoc( &dsNew, bd,
					    &(es.esOldSelectionScope),
					    &(es.esOldEditRange.erHead),
					    &(es.esOldEditRange.erTail) ) )
	    { LDEB(1); rval= -1; goto ready;	}

	td->tdSelection= dsNew;
	tedDescribeSelection( ed );

	et->etIndex= es.esTraceIndex+ es.esStepCount;
	}

#   if 0
    appDebug( "%s%s: %d.%d..%d.%d -> %d.%d..%d.%d\n",
		isRepeat?"Repeat ":"Redo ",
		docRtfGetTraceTag(es.esCommand),
		es.esOldEditRange.erHead.epParaNr,
		es.esOldEditRange.erHead.epStroff,
		es.esOldEditRange.erTail.epParaNr,
		es.esOldEditRange.erTail.epStroff,
		es.esNewEditRange.erHead.epParaNr,
		es.esNewEditRange.erHead.epStroff,
		es.esNewEditRange.erTail.epParaNr,
		es.esNewEditRange.erTail.epStroff );
#   endif

    switch( es.esCommand )
	{
	case EDITcmdMERGE_PARAS:
	    if  ( tedMergeParagraphsInSelection( ed, isRepeat ) )
		{ LDEB(es.esCommand); rval= -1; goto ready;	}
	    break;
	case EDITcmdINSERT_TABLE:
	    if  ( tedRepeatInsertTable( ed, &es, isRepeat ) )
		{ LDEB(es.esCommand); rval= -1; goto ready;	}
	    break;
	case EDITcmdDELETE_SELECTION:
	case EDITcmdDELETE_SELECTION_DEL:
	case EDITcmdDELETE_SELECTION_BS:
	case EDITcmdREPLACE:
	case EDITcmdREPLACE_FTNSEP:
	case EDITcmdREPLACE_BODY_LEVEL:
	    if  ( tedRepeatReplace( ed, &es, isRepeat ) )
		{ LDEB(es.esCommand); rval= -1; goto ready;	}
	    break;
	case EDITcmdINS_NOTE:
	    if  ( tedRepeatInsNote( ed, &es, isRepeat ) )
		{ LDEB(es.esCommand); rval= -1; goto ready;	}
	    break;

	case EDITcmdDELETE_PARA:
	    if  ( tedDocDeleteSelectedParagraphs( ed, isRepeat ) )
		{ LDEB(es.esCommand); rval= -1; goto ready;	}
	    break;
	case EDITcmdDELETE_COLUMN:
	    if  ( tedDocDeleteColumn( ed, isRepeat ) )
		{ LDEB(es.esCommand); rval= -1; goto ready;	}
	    break;
	case EDITcmdDELETE_ROW:
	    if  ( tedDocDeleteRow( ed, isRepeat ) )
		{ LDEB(es.esCommand); rval= -1; goto ready;	}
	    break;
	case EDITcmdDELETE_TABLE:
	    if  ( tedDocDeleteTable( ed, isRepeat ) )
		{ LDEB(es.esCommand); rval= -1; goto ready;	}
	    break;
	case EDITcmdDELETE_SECT:
	    if  ( tedDocDeleteSelectedSections( ed, isRepeat ) )
		{ LDEB(es.esCommand); rval= -1; goto ready;	}
	    break;
	case EDITcmdDELETE_HEADER:
	case EDITcmdDELETE_FOOTER:
	    if  ( tedRepeatDeleteHeaderFooter( ed, &es, isRepeat ) )
		{ LDEB(es.esCommand); rval= -1; goto ready;	}
	    break;

	case EDITcmdINSERT_PARA:
	case EDITcmdAPPEND_PARA:
	    if  ( tedDocInsertParagraph( ed,
				es.esCommand == EDITcmdAPPEND_PARA, isRepeat ) )
		{ LDEB(es.esCommand); rval= -1; goto ready;	}
	    break;
	case EDITcmdINSERT_COLUMN:
	case EDITcmdAPPEND_COLUMN:
	    if  ( tedDocAddColumnToTable( ed,
			    es.esCommand == EDITcmdAPPEND_COLUMN, isRepeat ) )
		{ LDEB(es.esCommand); rval= -1; goto ready;	}
	    break;
	case EDITcmdINSERT_ROW:
	case EDITcmdAPPEND_ROW:
	    if  ( tedDocAddRowToTable( ed,
			    es.esCommand == EDITcmdAPPEND_ROW, isRepeat ) )
		{ LDEB(es.esCommand); rval= -1; goto ready;	}
	    break;
	case EDITcmdINSERT_SECT:
	case EDITcmdAPPEND_SECT:
	    if  ( tedDocInsertSection( ed,
			    es.esCommand == EDITcmdAPPEND_SECT, isRepeat ) )
		{ LDEB(es.esCommand); rval= -1; goto ready;	}
	    break;
	case EDITcmdSPLIT_SECT:
	    if  ( tedDocInsertSectBreak( ed, isRepeat ) )
		{ LDEB(es.esCommand); rval= -1; goto ready;	}
	    break;
	case EDITcmdINSERT_HEADER:
	case EDITcmdINSERT_FOOTER:
	    if  ( tedRepeatInsertHeaderFooter( ed, &es, isRepeat ) )
		{ LDEB(es.esCommand); rval= -1; goto ready;	}
	    break;

	case EDITcmdUPD_FIELD:
	    if  ( tedRepeatUpdateField( ed, &es, isRepeat ) )
		{ LDEB(es.esCommand); rval= -1; goto ready;	}
	    break;
	case EDITcmdSET_HYPERLINK:
	case EDITcmdSET_BOOKMARK:
	    if  ( tedRepeatSetField( ed, &es, isRepeat ) )
		{ LDEB(es.esCommand); rval= -1; goto ready;	}
	    break;
	case EDITcmdDEL_FIELD:
	    if  ( tedDocFlattenTypedField( ed, es.esFieldKind,
					&(es.esNewStyle.dsTextMask),
					&(es.esNewStyle.dsTextAttribute),
					isRepeat ) )
		{ SDEB(docFieldKindStr(es.esFieldKind)); rval= -1; goto ready; }
	    break;

	case EDITcmdUPD_SPAN_PROPS:
	case EDITcmdUPD_PARA_PROPS:
	case EDITcmdUPD_TABLE_PROPS:
	case EDITcmdUPD_SECT_PROPS:
	case EDITcmdUPD_SECTDOC_PROPS:
	case EDITcmdUPD_DOC_PROPS:
	    if  ( tedRepeatUpdProps( ed, &es, isRepeat ) )
		{ LDEB(es.esCommand); rval= -1; goto ready;	}
	    break;

	case EDITcmdUPD_OBJECT:
	    if  ( tedRepeatUpdObject( ed, &es, isRepeat ) )
		{ LDEB(es.esCommand); rval= -1; goto ready;	}
	    break;
	case EDITcmdUPD_NOTE:
	    if  ( tedRepeatUpdNote( ed, &es, isRepeat ) )
		{ LDEB(es.esCommand); rval= -1; goto ready;	}
	    break;

	case EDITcmdUPD_LIST:
	    if  ( tedDocChangeCurrentList( ed, &(es.esDocumentList), isRepeat ) )
		{ LDEB(es.esCommand); rval= -1; goto ready;	}
	    break;
	case EDITcmdSET_NEW_LIST:
	    if  ( tedDocSetNewList( ed, isRepeat ) )
		{ LDEB(es.esCommand); rval= -1; goto ready;	}
	    break;

	case EDITcmdSHIFT_ROWS_DOWN:
	    if  ( tedEditShiftRowsInTable( ed,  1, isRepeat ) )
		{ LDEB(es.esCommand); rval= -1; goto ready; }
	    break;

	case EDITcmdSHIFT_ROWS_UP:
	    if  ( tedEditShiftRowsInTable( ed, -1, isRepeat ) )
		{ LDEB(es.esCommand); rval= -1; goto ready; }
	    break;

	case EDITcmdSHIFT_RIGHT:
	    if  ( tedFormatShiftIndent( ed,  1, isRepeat ) )
		{ LDEB(es.esCommand); rval= -1; goto ready;	}
	    break;
	case EDITcmdSHIFT_LEFT:
	    if  ( tedFormatShiftIndent( ed, -1, isRepeat ) )
		{ LDEB(es.esCommand); rval= -1; goto ready;	}
	    break;

	case EDITcmd_OPEN:
	case EDITcmd_SAVE:
	case EDITcmd_NEW:
	default:
	    LSDEB(es.esCommand,docRtfGetTraceTag(es.esCommand));
	    rval= -1; goto ready;
	}

  ready:

    docCleanEditStep( &es );

    return rval;
    }

