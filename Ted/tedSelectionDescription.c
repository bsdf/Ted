/************************************************************************/
/*									*/
/*  Ted: Management of the selection and the current position.		*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	"tedApp.h"
#   include	"tedRuler.h"
#   include	"tedLayout.h"
#   include	"tedSelect.h"
#   include	"tedAppResources.h"
#   include	"tedToolFront.h"
#   include	"tedDocument.h"
#   include	<docParaParticules.h>
#   include	<docEditCommand.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Is the selection a position between two chars?			*/
/*									*/
/************************************************************************/

int tedHasIBarSelection(	const EditDocument *  ed )
    {
    const TedDocument *	td= (const TedDocument *)ed->edPrivateData;

    return docIsIBarSelection( &(td->tdSelection) );
    }

int tedHasSelection(	const EditDocument *	ed )
    {
    const TedDocument *		td= (const TedDocument *)ed->edPrivateData;
    const DocumentSelection *	ds= &(td->tdSelection);

    if  ( ds->dsHead.dpNode )
	{ return 1;	}

    return 0;
    }

int tedGetSelection(	DocumentSelection *		ds,
			SelectionGeometry *		sg,
			SelectionDescription *		sd,
			DocumentTree **			pEi,
			BufferItem **			pBodySectNode,
			const EditDocument *		ed )
    {
    const TedDocument *		td= (const TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;
    const DocumentSelection *	dss= &(td->tdSelection);

    BufferItem *		bodySectNode= (BufferItem *)0;
    DocumentTree *		ei= (DocumentTree *)0;

    if  ( ! dss->dsHead.dpNode )
	{ return 1;	}

    if  ( pEi || pBodySectNode )
	{
	if  ( docGetTreeOfNode( &ei, &bodySectNode, bd, dss->dsHead.dpNode ) )
	    { LDEB(1); return -1;	}
	}

    *ds= td->tdSelection;
    *sg= td->tdSelectionGeometry;
    *sd= td->tdSelectionDescription;
    if  ( pBodySectNode )
	{ *pBodySectNode= bodySectNode;	}
    if  ( pEi )
	{ *pEi= ei;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Determine the derived properties such as line number and		*/
/*  coordinates of the current selection.				*/
/*									*/
/************************************************************************/

void tedDelimitCurrentSelection(	EditDocument *		ed )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    DocumentSelection		dsDoc;
    SelectionGeometry		sgDoc;
    SelectionDescription	sdDoc;

    BufferItem *		bodySectNode;

    int				lastLine;

    LayoutContext		lc;

    layoutInitContext( &lc );
    tedSetScreenLayoutContext( &lc, ed );

    if  ( tedGetSelection( &dsDoc, &sgDoc, &sdDoc,
				    (DocumentTree **)0, &bodySectNode, ed ) )
	{ LDEB(1); return;	}

    lastLine= sdDoc.sdIsIBarSelection &&
	( td->tdSelectionGeometry.sgHead.pgPositionFlags & POSflagLINE_HEAD );

    tedSelectionGeometry( &(td->tdSelectionGeometry), &dsDoc, bodySectNode,
							    lastLine, &lc );

    return;
    }

/************************************************************************/
/*									*/
/*  Adapt tools and rulers to the current position.			*/
/*									*/
/************************************************************************/

static void tedAdaptOptions(	TedDocument *			td,
				const unsigned char *		cmdEnabled,
				const SelectionDescription *	sd )
    {
    const EditTrace *	et= &(td->tdEditTrace);

    /* modifications */

    guiEnableWidget( td->tdInsHyperlinkOption,
				    cmdEnabled[EDITcmdSET_HYPERLINK] );

    guiEnableWidget( td->tdInsBookmarkOption,
				    cmdEnabled[EDITcmdSET_BOOKMARK] );

    guiEnableWidget( td->tdCutOption, cmdEnabled[EDITcmdDELETE_SELECTION] );

    guiEnableWidget( td->tdPasteOption, cmdEnabled[EDITcmdREPLACE] );

    guiEnableWidget( td->tdInsPictOption, cmdEnabled[EDITcmdREPLACE] );
    guiEnableWidget( td->tdInsFileOption, cmdEnabled[EDITcmdREPLACE] );
    guiEnableWidget( td->tdInsSymbolOption, cmdEnabled[EDITcmdREPLACE] );

    guiEnableWidget( td->tdFormatOneParaOption,
					cmdEnabled[EDITcmdMERGE_PARAS] );

    guiEnableWidget( td->tdInsInsertFootnoteOption,
					cmdEnabled[EDITcmdINS_NOTE] );

    guiEnableWidget( td->tdInsInsertEndnoteOption,
					cmdEnabled[EDITcmdINS_NOTE] );

    guiEnableWidget( td->tdInsInsertChftnsepOption,
				    cmdEnabled[EDITcmdREPLACE_FTNSEP] );
    guiEnableWidget( td->tdInsInsertTableOption,
				    cmdEnabled[EDITcmdINSERT_TABLE] );
    guiEnableWidget( td->tdInsInsertPageNumberOption, 
				    cmdEnabled[EDITcmdREPLACE] );
    guiEnableWidget( td->tdInsInsertLineBreakOption,
				    cmdEnabled[EDITcmdREPLACE] );
    guiEnableWidget( td->tdInsInsertPageBreakOption,
				    cmdEnabled[EDITcmdREPLACE_BODY_LEVEL] );
    guiEnableWidget( td->tdInsInsertColumnBreakOption,
				    cmdEnabled[EDITcmdREPLACE_BODY_LEVEL] );
    guiEnableWidget( td->tdInsInsertSectBreakOption,
				    cmdEnabled[EDITcmdSPLIT_SECT] );

    guiEnableWidget( td->tdTabInsertTableOption,
				    cmdEnabled[EDITcmdINSERT_TABLE] );
    guiEnableWidget( td->tdTabAddRowOption,
				    cmdEnabled[EDITcmdAPPEND_ROW] );
    guiEnableWidget( td->tdTabAddColumnOption,
				    cmdEnabled[EDITcmdINSERT_COLUMN] );
    guiEnableWidget( td->tdTabDeleteTableOption,
				    cmdEnabled[EDITcmdDELETE_TABLE] );
    guiEnableWidget( td->tdTabDeleteRowOption,
				    cmdEnabled[EDITcmdDELETE_ROW] );
    guiEnableWidget( td->tdTabDeleteColumnOption,
				    cmdEnabled[EDITcmdDELETE_COLUMN] );

    guiEnableWidget( td->tdFontBoldOption,
				    cmdEnabled[EDITcmdUPD_SPAN_PROPS] );
    guiEnableWidget( td->tdFontItalicOption,
				    cmdEnabled[EDITcmdUPD_SPAN_PROPS] );
    guiEnableWidget( td->tdFontUnderlinedOption,
				    cmdEnabled[EDITcmdUPD_SPAN_PROPS] );
    guiEnableWidget( td->tdFontSuperscriptOption,
				    cmdEnabled[EDITcmdUPD_SPAN_PROPS] );
    guiEnableWidget( td->tdFontSubscriptOption,
				    cmdEnabled[EDITcmdUPD_SPAN_PROPS] );
    guiEnableWidget( td->tdFontPasteOption,
				    cmdEnabled[EDITcmdUPD_SPAN_PROPS] );

    guiEnableWidget( td->tdFormatPasteRulerOption,
				    cmdEnabled[EDITcmdUPD_PARA_PROPS] );
    guiEnableWidget( td->tdFormatIncreaseIndentOption,
				    cmdEnabled[EDITcmdUPD_PARA_PROPS] );
    guiEnableWidget( td->tdFormatDecreaseIndentOption,
				    cmdEnabled[EDITcmdUPD_PARA_PROPS] );
    guiEnableWidget( td->tdFormatAlignLeftOption,
				    cmdEnabled[EDITcmdUPD_PARA_PROPS] );
    guiEnableWidget( td->tdFormatAlignRightOption,
				    cmdEnabled[EDITcmdUPD_PARA_PROPS] );
    guiEnableWidget( td->tdFormatAlignCenterOption,
				    cmdEnabled[EDITcmdUPD_PARA_PROPS] );
    guiEnableWidget( td->tdFormatAlignJustifyOption,
				    cmdEnabled[EDITcmdUPD_PARA_PROPS] );

    {
    int			n;
    const int		direction= -1;

    n= docEditGetTraceStep( (const TraceStep **)0, (int *)0,
						direction, et, et->etIndex );

    guiEnableWidget( td->tdUndoOption, n >= 0 );
    }

    {
    const TraceStep *	ts= (const TraceStep *)0;
    int			n;
    const int		direction= 1;
    int			isRepeat;

    n= docEditGetTraceStep( &ts, &isRepeat, direction, et, et->etIndex );
    if  ( n < 0 )
	{ guiEnableWidget( td->tdRepeatOption, 0 );	}
    else{
	if  ( ! isRepeat )
	    { guiEnableWidget( td->tdRepeatOption, 1 );	}
	else{
	    int	cmd= ts->tsCommand;

	    if  ( cmd == EDITcmdEXTEND_REPLACE )
		{ cmd= EDITcmdREPLACE;	}
	    if  ( cmd < 0 || cmd >= EDITcmd_COUNT )
		{ LDEB(cmd);  guiEnableWidget( td->tdRepeatOption, 0 );	}
	    else{
		if  ( cmd == EDITcmdDEL_FIELD		||
		      cmd == EDITcmdUPD_FIELD		)
		    {
		    guiEnableWidget( td->tdRepeatOption,
				cmdEnabled[cmd]				&&
				sd->sdHeadFieldKind == ts->tsFieldKind	);
		    }
		else{
		    guiEnableWidget( td->tdRepeatOption, cmdEnabled[cmd] );
		    }
		}
	    }
	}
    }

    /* read-only */

    guiEnableWidget( td->tdCopyOption, ! sd->sdIsIBarSelection );

    guiEnableWidget( td->tdTabSelectTableOption, sd->sdHeadInTable );
    guiEnableWidget( td->tdTabSelectRowOption, sd->sdHeadInTable );
    guiEnableWidget( td->tdTabSelectColumnOption, sd->sdHeadInTable );

    return;
    }

void tedAdaptToolsToSelection(	EditDocument *		ed )
    {
    EditApplication *		ea= ed->edApplication;
    TedAppResources *		tar= (TedAppResources *)ea->eaResourceData;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;
    DocumentTree *		ei;
    BufferItem *		bodySectNode;

    unsigned char		cmdEnabled[EDITcmd_COUNT];

    if  ( tedGetSelection( &ds, &sg, &sd, &ei, &bodySectNode, ed ) )
	{ LDEB(1); return;	}

    docEnableEditCommands( cmdEnabled, &sd );

    tedDocAdaptTopRuler( ed, &ds, &sg, &sd, bodySectNode );

    tedSetBottomRulerValues( ed->edBottomRuler, ed->edBottomRulerWidget,
			sd.sdTailPage,
			sd.sdDocumentPages,
			sd.sdTailSection,
			sd.sdDocumentSections );

    tedAdaptFontIndicatorsToSelection( ed );

    tedAdaptOptions( td, cmdEnabled, &(td->tdSelectionDescription) );

    if  ( tar->tarInspector )
	{
	const int	choosePage= 0;

	tedFormatToolAdaptToSelection( tar->tarInspector, ed, choosePage, ei,
					    &(td->tdSelection),
					    &(td->tdSelectionGeometry),
					    &(td->tdSelectionDescription),
					    cmdEnabled );
	}
    }

/************************************************************************/
/*									*/
/*  Adapt format tool to the circumstances.				*/
/*									*/
/************************************************************************/

void tedAdaptFormatToolToDocument(	EditDocument *	ed,
					int		choosePage )
    {
    EditApplication *		ea= ed->edApplication;
    TedAppResources *		tar= (TedAppResources *)ea->eaResourceData;

    DocumentTree *		ei;
    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    unsigned char		cmdEnabled[EDITcmd_COUNT];

    if  ( ! tar->tarInspector )
	{ return;	}

    if  ( tedGetSelection( &ds, &sg, &sd, &ei, (BufferItem **)0, ed ) )
	{ LDEB(1); return;	}

    docEnableEditCommands( cmdEnabled, &sd );

    tedFormatToolAdaptToSelection( tar->tarInspector, ed, choosePage,
					    ei, &ds, &sg, &sd, cmdEnabled );

    return;
    }

