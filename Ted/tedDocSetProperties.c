/************************************************************************/
/*									*/
/*  Format changes to the document/selection.				*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	<docLayout.h>
#   include	"tedEdit.h"
#   include	"tedDocFront.h"
#   include	"tedLayout.h"
#   include	"tedDocument.h"
#   include	<docRtfTrace.h>
#   include	<docParaRulerAdmin.h>
#   include	<docEditCommand.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Change properties of the current selection.				*/
/*									*/
/************************************************************************/

int tedEditChangeSelectionPropertiesImpl(
				TedEditOperation *		teo,
				const DocumentSelection *	ds,

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
    int				rval = 0;
    EditOperation *		eo= &(teo->teoEo);
    BufferDocument *		bd= eo->eoDocument;

    PropertyMask		selTaDoneMask;
    PropertyMask		selPpDoneMask;
    PropertyMask		selCpDoneMask;
    PropertyMask		selRpDoneMask;
    PropertyMask		selSpDoneMask;
    PropertyMask		docDpDoneMask;

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

    utilPropMaskClear( &selTaDoneMask );
    utilPropMaskClear( &selPpDoneMask );
    utilPropMaskClear( &selCpDoneMask );
    utilPropMaskClear( &selRpDoneMask );
    utilPropMaskClear( &selSpDoneMask );
    utilPropMaskClear( &docDpDoneMask );

    if  ( docChangeSelectionProperties( eo, ds,
					&selTaDoneMask, taSetMask, taSet,
					&selPpDoneMask, ppSetMask, ppSet,
					&selCpDoneMask, cpSetMask, cpSet,
					&selRpDoneMask, rpSetMask, rpSet,
					&selSpDoneMask, spSetMask, spSet,
					&docDpDoneMask, dpSetMask, dpSet ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( taSetMask )
	{
	PropertyMask	taDoneMask;

	utilPropMaskClear( &taDoneMask );

	utilUpdateTextAttribute( &taDoneMask, &(teo->teoSavedTextAttribute),
							    taSetMask, taSet );
	if  ( ! utilPropMaskIsEmpty( &taDoneMask ) )
	    {
	    teo->teoSavedTextAttributeNumber= docTextAttributeNumber( bd,
					    &(teo->teoSavedTextAttribute) );
	    }
	}

    if  ( dpSetMask || spSetMask )
	{
	PropertyMask	dgDoneMask;
	PropertyMask	dgMask;
	PropertyMask	mask;

	utilPropMaskClear( &dgDoneMask );
	utilPropMaskClear( &dgMask );
	utilPropMaskFill( &dgMask, DGprop_COUNT );

	utilPropMaskAnd( &mask, &selSpDoneMask, &dgMask );
	utilPropMaskOr( &dgDoneMask, &dgDoneMask, &mask );

	utilPropMaskAnd( &mask, &docDpDoneMask, &dgMask );
	utilPropMaskOr( &dgDoneMask, &dgDoneMask, &mask );

	if  ( ! utilPropMaskIsEmpty( &dgDoneMask ) )
	    { teo->teoRefreshScreenRectangle= 1;	}
	}

  ready:

    return rval;
    }

/************************************************************************/
/*									*/
/*  Change properties of the current selection.				*/
/*									*/
/************************************************************************/

int tedEditChangeSelectionProperties(
				TedEditOperation *		teo,
				const DocumentSelection *	ds,
				int				level,
				int				command,

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
    int				rval = 0;
    EditOperation *		eo= &(teo->teoEo);

    DocumentSelection		dsTraced= *ds;

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

    if  ( tedEditStartStep( teo, command ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( teo->teoEditTrace )
	{
	if  ( docRtfTraceOldProperties( &dsTraced, eo, level,
					taSetMask, ppSetMask,
					cpSetMask, rpSetMask,
					spSetMask, dpSetMask ) )
	    { LDEB(1); rval= -1; goto ready;	}

	if  ( docRtfTraceNewProperties( eo,
					taSetMask, taSet,
					ppSetMask, ppSet,
					cpSetMask, cpSet,
					rpSetMask, rpSet,
					spSetMask, spSet,
					dpSetMask, dpSet ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}

    if  ( tedEditChangeSelectionPropertiesImpl( teo, &dsTraced,
						    taSetMask, taSet,
						    ppSetMask, ppSet,
						    cpSetMask, cpSet,
						    rpSetMask, rpSet,
						    spSetMask, spSet,
						    dpSetMask, dpSet ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( cpSetMask )
	{
	EditRange *		er= &(eo->eoSelectedRange);
	DocumentSelection	dsNew;

	docInitDocumentSelection( &dsNew );

	if  ( docSelectionForEditPositionsInTree( &dsNew, eo->eoTree,
					    &(er->erHead), &(er->erTail) ) )
	    { LDEB(1); return -1;	}

	if  ( dsNew.dsCol0 >= 0 && dsNew.dsCol1 >= 0 )
	    { docAvoidMergedTail( &dsNew, er );	}
	}

    docSetEditRange( &(eo->eoAffectedRange), &dsTraced );

    if  ( tedEditFinishOldSelection( teo ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( teo->teoEditTrace )
	{
	docRtfTraceNewPosition( eo, (const SelectionScope *)0, SELposALL );
	}

    /*  Activate text attribute setting for future typing */
    if  ( docIsIBarSelection( ds ) && taSetMask )
	{
	EditDocument *		ed= teo->teoEditDocument;
	TedDocument *		td= (TedDocument *)ed->edPrivateData;
	BufferDocument *	bd= td->tdDocument;
	SelectionDescription *	sd= &(td->tdSelectionDescription);
	TextAttribute *		ta= &(sd->sdTextAttribute);

	PropertyMask		taDoneMask;

	utilPropMaskClear( &taDoneMask );

	utilUpdateTextAttribute( &taDoneMask, ta, taSetMask, taSet );

	if  ( ! utilPropMaskIsEmpty( &taDoneMask ) )
	    {
	    const IndexMapping *	a2s= &(td->tdAttributeToScreenFont);

	    sd->sdTextAttributeNumber= docTextAttributeNumber( bd, ta );

	    td->tdCurrentScreenFont= utilIndexMappingGet( a2s,
			    td->tdSelectionDescription.sdTextAttributeNumber );
	    }
	}

    tedFinishEditOperation( teo );

  ready:

    return rval;
    }

static int tedDocChangeSelectionProperties(
				EditDocument *			ed,
				int				command,

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
				const DocumentProperties *	dpSet,
				
				int				traced )
    {
    int				rval= 0;
    const int			level= DOClevSPAN;

    TedEditOperation		teo;
    EditOperation *		eo= &(teo.teoEo);
    SelectionGeometry		sg;
    SelectionDescription	sd;

    DocumentSelection		ds;

    const int			fullWidth= 1;

    tedStartEditOperation( &teo, &sg, &sd, ed, fullWidth, traced );

    docEditOperationGetSelection( &ds, eo );

    if  ( tedEditChangeSelectionProperties( &teo, &ds, level, command,
						taSetMask, taSet,
						ppSetMask, ppSet,
						cpSetMask, cpSet,
						rpSetMask, rpSet,
						spSetMask, spSet,
						dpSetMask, dpSet ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    tedCleanEditOperation( &teo );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Utility routines around tedDocChangeSelectionProperties():		*/
/*									*/
/************************************************************************/

int tedDocChangeSectionProperties( EditDocument *		ed,
				const PropertyMask *		spSetMask,
				const SectionProperties *	spSet,
				int				traced )
    {
    if  ( tedDocChangeSelectionProperties( ed, EDITcmdUPD_SECT_PROPS,
		    (const PropertyMask *)0, (const TextAttribute *)0,
		    (const PropertyMask *)0, (const ParagraphProperties *)0,
		    (const PropertyMask *)0, (const CellProperties *)0,
		    (const PropertyMask *)0, (const RowProperties *)0,
		    spSetMask, spSet,
		    (const PropertyMask *)0, (const DocumentProperties *)0,
		    traced ) )
	{ XDEB(spSetMask); return -1;	}

    return 0;
    }

int tedDocChangeParagraphProperties(
				EditDocument *			ed,
				const PropertyMask *		ppSetMask,
				const ParagraphProperties *	ppSet,
				int				traced )
    {
    if  ( tedDocChangeSelectionProperties( ed, EDITcmdUPD_PARA_PROPS,
		    (const PropertyMask *)0, (const TextAttribute *)0,
		    ppSetMask, ppSet,
		    (const PropertyMask *)0, (const CellProperties *)0,
		    (const PropertyMask *)0, (const RowProperties *)0,
		    (const PropertyMask *)0, (const SectionProperties *)0,
		    (const PropertyMask *)0, (const DocumentProperties *)0,
		    traced ) )
	{ XDEB(ppSetMask); return -1;	}

    return 0;
    }

void tedDocChangeTextAttribute(		EditDocument *		ed,
					const PropertyMask *	taSetMask,
					const TextAttribute *	taSet,
					int			traced )
    {
    LayoutContext		lc;

    layoutInitContext( &lc );
    tedSetScreenLayoutContext( &lc, ed );

    if  ( tedDocChangeSelectionProperties( ed, EDITcmdUPD_SPAN_PROPS,
		    taSetMask, taSet,
		    (const PropertyMask *)0, (const ParagraphProperties *)0,
		    (const PropertyMask *)0, (const CellProperties *)0,
		    (const PropertyMask *)0, (const RowProperties *)0,
		    (const PropertyMask *)0, (const SectionProperties *)0,
		    (const PropertyMask *)0, (const DocumentProperties *)0,
		    traced ) )
	{ LDEB(1);	}

    return;
    }

/************************************************************************/
/*									*/
/*  Change table properties.						*/
/*									*/
/************************************************************************/

int tedDocSetTableProperties(	EditDocument *		ed,
				int			wholeRow,
				int			wholeColumn,
				const PropertyMask *	cpSetMask,
				const CellProperties *	cpSet,
				const PropertyMask *	rpSetMask,
				const RowProperties *	rpSet,
				int			traced )
    {
    int				rval= 0;

    TedEditOperation		teo;
    EditOperation *		eo= &(teo.teoEo);
    SelectionGeometry		sg;
    SelectionDescription	sd;

    DocumentSelection		ds;

    int				level= DOClevCELL;
    int				command= EDITcmdUPD_TABLE_PROPS;

    const int			fullWidth= 1;

    tedStartEditOperation( &teo, &sg, &sd, ed, fullWidth, traced );

    docInitDocumentSelection( &ds );

    docEditOperationGetSelection( &ds, eo );

    if  ( wholeRow )
	{
	if  ( wholeColumn )
	    { level= DOClevTABLE;	}
	else{ level= DOClevROW;		}
	}
    else{
	if  ( wholeColumn )
	    { level= DOClevCOLUMN;	}
	else{ level= DOClevCELL;	}
	}

    if  ( tedEditChangeSelectionProperties( &teo, &ds,
		    level, command,
		    (const PropertyMask *)0, (const TextAttribute *)0,
		    (const PropertyMask *)0, (const ParagraphProperties *)0,
		    cpSetMask, cpSet, rpSetMask, rpSet,
		    (const PropertyMask *)0, (const SectionProperties *)0,
		    (const PropertyMask *)0, (const DocumentProperties *)0 ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    tedCleanEditOperation( &teo );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Change the tabs of a paragraph.					*/
/*									*/
/************************************************************************/

int tedDocSetParagraphTabs(	EditDocument *		ed,
				const TabStopList *	tsl,
				int			traced )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;

    ParagraphProperties		ppSet;
    PropertyMask		ppSetMask;

    docInitParagraphProperties( &ppSet );
    utilPropMaskClear( &ppSetMask );
    PROPmaskADD( &ppSetMask, PPpropTAB_STOPS );

    ppSet.ppTabStopListNumber= docTabStopListNumber( bd, tsl );

    return tedDocChangeParagraphProperties( ed, &ppSetMask, &ppSet, traced );
    }

/************************************************************************/
/*									*/
/*  Change properties of all sections.					*/
/*									*/
/*  9)  Eventually this routine must be combined with the set document	*/
/*	properties routine. Now it is called in conjunction with its	*/
/*	colleague that takes care of reformats and redraws.		*/
/*									*/
/************************************************************************/

int tedDocChangeAllSectionProperties(
				EditDocument *			ed,
				const PropertyMask *		spSetMask,
				const SectionProperties *	spSet,
				const PropertyMask *		dpSetMask,
				const DocumentProperties *	dpSet,
				int				traced )
    {
    int				rval= 0;

    TedEditOperation		teo;
    EditOperation *		eo= &(teo.teoEo);
    SelectionGeometry		sg;
    SelectionDescription	sd;

    DocumentSelection		ds;

    const int			fullWidth= 1;

    tedStartEditOperation( &teo, &sg, &sd, ed, fullWidth, traced );

    docInitDocumentSelection( &ds );

    if  ( docSelectWholeBody( &ds, eo->eoDocument ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( tedEditChangeSelectionProperties( &teo, &ds,
		    DOClevSECT, EDITcmdUPD_SECTDOC_PROPS,
		    (const PropertyMask *)0, (const TextAttribute *)0,
		    (const PropertyMask *)0, (const ParagraphProperties *)0,
		    (const PropertyMask *)0, (const CellProperties *)0,
		    (const PropertyMask *)0, (const RowProperties *)0,
		    spSetMask, spSet, dpSetMask, dpSet ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    tedCleanEditOperation( &teo );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Change document properties.						*/
/*									*/
/************************************************************************/

int tedDocSetDocumentProperties( EditDocument *			ed,
				const PropertyMask *		dpSetMask,
				const DocumentProperties *	dpSet,
				int				traced )
    {
    if  ( tedDocChangeSelectionProperties( ed, EDITcmdUPD_DOC_PROPS,
		    (const PropertyMask *)0, (const TextAttribute *)0,
		    (const PropertyMask *)0, (const ParagraphProperties *)0,
		    (const PropertyMask *)0, (const CellProperties *)0,
		    (const PropertyMask *)0, (const RowProperties *)0,
		    (const PropertyMask *)0, (const SectionProperties *)0,
		    dpSetMask, dpSet,
		    traced ) )
	{ XDEB(dpSetMask); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Menu options to change paragraph alignment.				*/
/*									*/
/************************************************************************/

void tedDocFormatSetParaAlignment(	EditDocument *		ed,
					int			align,
					int			traced )
    {
    ParagraphProperties		ppSet;
    PropertyMask		ppSetMask;

    utilPropMaskClear( &ppSetMask );
    PROPmaskADD( &ppSetMask, PPpropALIGNMENT );

    docInitParagraphProperties( &ppSet );
    ppSet.ppAlignment= align;

    if  ( tedDocChangeParagraphProperties( ed, &ppSetMask, &ppSet, traced ) )
	{ LDEB(1);	}

    docCleanParagraphProperties( &ppSet );

    return;
    }

/************************************************************************/
/*									*/
/*  Menu options to change paragraph alignment.				*/
/*									*/
/************************************************************************/

void tedDocSetParaOutlineLevel(		EditDocument *		ed,
					int			level,
					int			traced )
    {
    ParagraphProperties		ppSet;
    PropertyMask		ppSetMask;

    utilPropMaskClear( &ppSetMask );
    PROPmaskADD( &ppSetMask, PPpropOUTLINELEVEL );

    docInitParagraphProperties( &ppSet );
    ppSet.ppOutlineLevel= level;

    if  ( tedDocChangeParagraphProperties( ed, &ppSetMask, &ppSet, traced ) )
	{ LDEB(1);	}

    docCleanParagraphProperties( &ppSet );

    return;
    }

/************************************************************************/
/*									*/
/*  Change the document geometry: Callback from the page layout tool	*/
/*  etc.								*/
/*									*/
/************************************************************************/

void tedDocSetPageLayout(	EditDocument *			ed,
				const PropertyMask *		dgSetMask,
				const DocumentGeometry *	dgSet,
				int				wholeDocument,
				int				traced )
    {
    SectionProperties		spSet;
    DocumentProperties		dpSet;

    docInitSectionProperties( &spSet );
    docInitDocumentProperties( &dpSet );

    utilUpdDocumentGeometry( (PropertyMask *)0, &(spSet.spDocumentGeometry),
							    dgSetMask, dgSet );
    utilUpdDocumentGeometry( (PropertyMask *)0, &(dpSet.dpGeometry),
							    dgSetMask, dgSet );

    if  ( wholeDocument )
	{
	if  ( tedDocChangeAllSectionProperties( ed,
						dgSetMask, &spSet,
						dgSetMask, &dpSet,
						traced ) )
	    { LDEB(wholeDocument); goto ready;	}
	}
    else{
	if  ( tedDocChangeSectionProperties( ed, dgSetMask, &spSet, traced ) )
	    { XDEB(dgSetMask); goto ready;	}
	}

  ready:

    docCleanSectionProperties( &spSet );
    docCleanDocumentProperties( &dpSet );

    return;
    }

/************************************************************************/
/*									*/
/*  Increase/Decrease the indentation level of the paragraphs in the	*/
/*  selection.								*/
/*									*/
/************************************************************************/

int tedFormatShiftIndent(	EditDocument *	ed,
				int		step,
				int		traced )
    {
    int				rval= 0;

    SelectionGeometry		sg;
    SelectionDescription	sd;

    TedEditOperation		teo;
    EditOperation *		eo= &(teo.teoEo);

    PropertyMask		ppSetMask;

    DocumentSelection		dsTraced;

    const int			fullWidth= 1;

    utilPropMaskClear( &ppSetMask );
    PROPmaskADD( &ppSetMask, PPpropLISTLEVEL );
    PROPmaskADD( &ppSetMask, PPpropLEFT_INDENT );
    PROPmaskADD( &ppSetMask, PPpropFIRST_INDENT );

    tedStartEditOperation( &teo, &sg, &sd, ed, fullWidth, traced );

    if  ( step >= 0 )
	{
	if  ( tedEditStartStep( &teo, EDITcmdSHIFT_RIGHT ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}
    else{
	if  ( tedEditStartStep( &teo, EDITcmdSHIFT_LEFT ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}

    if  ( teo.teoEditTrace )
	{
	if  ( docRtfTraceOldProperties( &dsTraced, eo, DOClevPARA,
				    (PropertyMask *)0, &ppSetMask,
				    (PropertyMask *)0, (PropertyMask *)0,
				    (PropertyMask *)0, (PropertyMask *)0 ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}

    if  ( docEditShiftIndent( eo, step ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( tedEditFinishOldSelection( &teo ) )
	{ LDEB(1);	}

    if  ( teo.teoEditTrace )
	{
	docRtfTraceNewPosition( eo, (const SelectionScope *)0, SELposALL );
	}

    tedFinishEditOperation( &teo );

  ready:

    tedCleanEditOperation( &teo );

    return rval;
    }

