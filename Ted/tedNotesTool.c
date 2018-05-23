/************************************************************************/
/*									*/
/*  A tool to manage the style of notes in the document.		*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdio.h>
#   include	<stddef.h>
#   include	<limits.h>

#   include	"tedNotesTool.h"
#   include	"tedAppFront.h"
#   include	<docTreeType.h>
#   include	<guiToolUtil.h>
#   include	<guiTextUtil.h>
#   include	<docTreeNode.h>
#   include	<docNodeTree.h>
#   include	<docEditCommand.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  What property values are relevant for footnotes/endnotes?		*/
/*									*/
/************************************************************************/

static const int TED_FootnoteRestarts[]=
    {
    FTN_RST_CONTINUOUS,
    FTN_RST_PER_SECTION,
    FTN_RST_PER_PAGE,
    };

static const int TED_FootnoteRestartCount= sizeof(TED_FootnoteRestarts)/sizeof(int);

static const int TED_EndnoteRestarts[]=
    {
    FTN_RST_CONTINUOUS,
    FTN_RST_PER_SECTION,
    };

static const int TED_EndnoteRestartCount= sizeof(TED_EndnoteRestarts)/sizeof(int);

static const int TED_EndnotePlacements[]=
    {
    /*
    FTNplacePAGE_END,
    */
    FTNplaceSECT_END,
    FTNplaceDOC_END,
    };

static const int TED_EndnotePlacementCount= sizeof(TED_EndnotePlacements)/sizeof(int);

/************************************************************************/
/*									*/
/*  Refresh the 'Notes' page of the format tool.			*/
/*									*/
/************************************************************************/

static void tedSetNotesMenu(	AppOptionmenu *		aom,
				int			val,
				const int *		values,
				int			valueCount )
    {
    int		i;

    for ( i= 0; i < valueCount; i++ )
	{
	if  ( values[i] == val )
	    { appSetOptionmenu( aom, i ); return;	}
	}

    LLDEB(val,valueCount);
    appSetOptionmenu( aom, -1 );
    return;
    }

static void tedNotesRefreshNotesStartNumber(	NotesTool *		nt,
						const NotesProperties *	np )
    {
    if  ( np->npRestart == FTN_RST_CONTINUOUS )
	{
	appIntegerToTextWidget( nt->ntStartNumberText, np->npStartNumber );
	guiEnableText( nt->ntStartNumberText, 1 );
	}
    else{
	appStringToTextWidget( nt->ntStartNumberText, "" );
	guiEnableText( nt->ntStartNumberText, 0 );
	}

    return;
    }

static void tedNotesRefreshNotePropertiesTool(
					NotesTool *		nt,
					const NotesProperties *	np,
					const int *		restarts,
					int			restartCount,
					const int *		placements,
					int			placementCount )
    {
    tedNotesRefreshNotesStartNumber( nt, np );

    if  ( placements )
	{
	guiEnableWidget( nt->ntPlacementRow,
			nt->ntCanChangeSelection || nt->ntCanChangeDocument );
	appGuiEnableOptionmenu( &(nt->ntPlacementOptionmenu),
			nt->ntCanChangeSelection || nt->ntCanChangeDocument );
	tedSetNotesMenu( &(nt->ntPlacementOptionmenu),
					    np->npPlacement,
					    placements, placementCount );
	}
    else{
	if  ( nt->ntPlacementOptionmenu.aomInplace )
	    {
	    guiEnableWidget( nt->ntPlacementRow, 0 );
	    appGuiEnableOptionmenu( &(nt->ntPlacementOptionmenu), 0 );
	    appSetOptionmenu( &(nt->ntPlacementOptionmenu), -1 );
	    }
	}

    tedSetNotesMenu( &(nt->ntRestartOptionmenu),
					    np->npRestart,
					    restarts, restartCount );

    appSetOptionmenu( &(nt->ntJustifyOptionmenu),
					    np->npJustification );
    appSetOptionmenu( &(nt->ntStyleOptionmenu),
					    np->npNumberStyle );

    return;
    }

static void tedNotesToolRefreshNotesWidgets(	NotesTool *	nt )
    {
    if  ( nt->ntNoteTreeType == DOCinFOOTNOTE )
	{
	tedNotesRefreshNotePropertiesTool( nt,
			&(nt->ntNotesPropertiesChosen.fepFootnotesProps),
			TED_FootnoteRestarts,
			TED_FootnoteRestartCount,
			(int *)0, 0 );
	}

    if  ( nt->ntNoteTreeType == DOCinENDNOTE )
	{
	tedNotesRefreshNotePropertiesTool( nt,
			&(nt->ntNotesPropertiesChosen.fepEndnotesProps),
			TED_EndnoteRestarts,
			TED_EndnoteRestartCount,
			TED_EndnotePlacements,
			TED_EndnotePlacementCount );
	}

    return;
    }

void tedRefreshNotesTool(	NotesTool *			nt,
				int *				pEnabled,
				int *				pPref,
				InspectorSubject *		is,
				const DocumentSelection *	ds,
				const SelectionDescription *	sd,
				BufferDocument *		bd,
				const unsigned char *		cmdEnabled )
    {
    const DocumentProperties *	dp= &(bd->bdProperties);

    const BufferItem *		bodySectNode= (BufferItem *)0;

    if  ( ds )
	{
	bodySectNode= docGetBodySectNode( ds->dsHead.dpNode, bd );
	if  ( ! bodySectNode )
	    { XDEB(bodySectNode);	}
	}

    nt->ntNotesPropertiesSetDocument= dp->dpNotesProps;
    nt->ntNotesPropertiesSetSelection= dp->dpNotesProps;

    if  ( bodySectNode )
	{
	nt->ntNotesPropertiesSetSelection=
			    bodySectNode->biSectProperties.spNotesProperties;
	}

    nt->ntNotesPropertiesChosen= nt->ntNotesPropertiesSetSelection;

    nt->ntCanChangeSelection= cmdEnabled[EDITcmdUPD_SECT_PROPS];
    nt->ntCanChangeDocument= cmdEnabled[EDITcmdUPD_SECTDOC_PROPS];

    tedNotesToolRefreshNotesWidgets( nt );

    guiEnableWidget( nt->ntJustifyRow,
			nt->ntCanChangeSelection || nt->ntCanChangeDocument );
    if  ( nt->ntPlacementRow )
	{
	guiEnableWidget( nt->ntPlacementRow,
			nt->ntCanChangeSelection || nt->ntCanChangeDocument );
	}

    guiEnableWidget( nt->ntRestartRow,
			nt->ntCanChangeSelection || nt->ntCanChangeDocument );
    guiEnableWidget( nt->ntStyleRow,
			nt->ntCanChangeSelection || nt->ntCanChangeDocument );
    guiEnableWidget( nt->ntStartNumberRow,
			nt->ntCanChangeSelection || nt->ntCanChangeDocument );
    guiEnableText( nt->ntStartNumberText,
			nt->ntCanChangeSelection || nt->ntCanChangeDocument );

    guiEnableWidget( nt->ntRevertSelectionWidget,
			nt->ntCanChangeSelection || nt->ntCanChangeDocument );
    guiEnableWidget( nt->ntChangeSelectionWidget,
			nt->ntCanChangeSelection || nt->ntCanChangeDocument );

    guiEnableWidget( is->isRevertButton,
			nt->ntCanChangeSelection || nt->ntCanChangeDocument );
    guiEnableWidget( is->isApplyButton,
			nt->ntCanChangeSelection || nt->ntCanChangeDocument );

    *pEnabled= 1;
    return;
    }

/************************************************************************/
/*									*/
/*  Set/Revert buttons for the document properties have been pushed.	*/
/*									*/
/************************************************************************/

static int tedNotesToolGetStartNumber(	NotesTool *		nt,
					NotesProperties *	np )
    {
    if  ( np->npRestart == FTN_RST_CONTINUOUS )
	{
	if  ( appGetIntegerFromTextWidget( nt->ntStartNumberText,
				    &(np->npStartNumber), 1, 0, INT_MAX, 0 ) )
	    { return -1;	}
	}

    return 0;
    }

static APP_TXACTIVATE_CALLBACK_H( tedNotesFootRestartChanged, w, voidnt )
    {
    NotesTool *		nt= (NotesTool *)voidnt;
    NotesProperties *	np= &(nt->ntNotesPropertiesChosen.fepFootnotesProps);

    if  ( np->npRestart != FTN_RST_CONTINUOUS		||
	  tedNotesToolGetStartNumber( nt, np )		)
	{ return;	}

    return;
    }

static APP_TXACTIVATE_CALLBACK_H( tedNotesEndRestartChanged, w, voidnt )
    {
    NotesTool *			nt= (NotesTool *)voidnt;
    NotesProperties *		np= &(nt->ntNotesPropertiesChosen.fepEndnotesProps);

    if  ( np->npRestart != FTN_RST_CONTINUOUS		||
	  tedNotesToolGetStartNumber( nt, np )		)
	{ return;	}

    return;
    }

static int tedNotesToolSetProperties(	NotesTool *	nt,
					int		wholeDocument )
    {
    int				rval= 0;

    PropertyMask		dpSetMask;
    PropertyMask		spSetMask;

    DocumentProperties		dpNew;
    SectionProperties		spNew;

    docInitSectionProperties( &spNew );
    docInitDocumentProperties( &dpNew );

    utilPropMaskClear( &dpSetMask );
    utilPropMaskClear( &spSetMask );

    if  ( nt->ntNoteTreeType == DOCinFOOTNOTE )
	{
	tedNotesToolGetStartNumber( nt,
		    &(nt->ntNotesPropertiesChosen.fepFootnotesProps) );
	}

    if  ( nt->ntNoteTreeType == DOCinENDNOTE )
	{
	tedNotesToolGetStartNumber( nt,
		    &(nt->ntNotesPropertiesChosen.fepEndnotesProps) );
	}

    /**/
    dpNew.dpNotesProps= nt->ntNotesPropertiesChosen;
    spNew.spNotesProperties= nt->ntNotesPropertiesChosen;

    {
    PropertyMask	cmpMask;

    utilPropMaskClear( &cmpMask );
    docFillDocNotesMask( &cmpMask );

    docFootEndNotesPropertyDifference( &dpSetMask,
				&(dpNew.dpNotesProps), &cmpMask,
				&(nt->ntNotesPropertiesSetDocument),
				DOCdocNOTE_PROP_MAP );

    }

    {
    PropertyMask	cmpMask;

    utilPropMaskClear( &cmpMask );
    docFillSectNotesMask( &cmpMask );

    docFootEndNotesPropertyDifference( &spSetMask,
				&(spNew.spNotesProperties), &cmpMask,
				&(nt->ntNotesPropertiesSetSelection),
				DOCsectNOTE_PROP_MAP );
    }

    if  ( wholeDocument )
	{
	if  ( tedAppChangeAllSectionProperties( nt->ntApplication,
							&spSetMask, &spNew,
							&dpSetMask, &dpNew ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}
    else{
	if  ( tedAppChangeSectionProperties( nt->ntApplication,
							&spSetMask, &spNew ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}

  ready:

    docCleanSectionProperties( &spNew );
    docCleanDocumentProperties( &dpNew );

    return rval;
    }

static APP_BUTTON_CALLBACK_H( tedNotesToolChangeDocumentPushed, w, voidnt )
    {
    NotesTool *			nt= (NotesTool *)voidnt;
    const int			wholeDocument= 1;

    tedNotesToolSetProperties( nt, wholeDocument );
    }

static APP_BUTTON_CALLBACK_H( tedNotesToolChangeSelectionPushed, w, voidnt )
    {
    NotesTool *			nt= (NotesTool *)voidnt;
    const int			wholeDocument= 0;

    tedNotesToolSetProperties( nt, wholeDocument );
    }

static APP_BUTTON_CALLBACK_H( tedNotesToolRevertDocumentPushed, w, voidnt )
    {
    NotesTool *		nt= (NotesTool *)voidnt;

    nt->ntNotesPropertiesChosen= nt->ntNotesPropertiesSetDocument;
    tedNotesToolRefreshNotesWidgets( nt );

    return;
    }

static APP_BUTTON_CALLBACK_H( tedNotesToolRevertSelectionPushed, w, voidnt )
    {
    NotesTool *		nt= (NotesTool *)voidnt;

    nt->ntNotesPropertiesChosen= nt->ntNotesPropertiesSetSelection;
    tedNotesToolRefreshNotesWidgets( nt );

    return;
    }

/************************************************************************/
/*									*/
/*  Menu callbacks.							*/
/*									*/
/************************************************************************/

static void tedFootnoteStyleChosen(	int		n,
					void *		voidnt )
    {
    NotesTool *		nt= (NotesTool *)voidnt;
    NotesProperties *	np= &(nt->ntNotesPropertiesChosen.fepFootnotesProps);

    if  ( n < 0 || n >= FTNstyle_COUNT )
	{ LDEB(n); return;	}

    np->npNumberStyle= n;

    return;
    }

static void tedEndnoteStyleChosen(	int		n,
					void *		voidnt )
    {
    NotesTool *		nt= (NotesTool *)voidnt;
    NotesProperties *	np= &(nt->ntNotesPropertiesChosen.fepEndnotesProps);

    if  ( n < 0 || n >= FTNstyle_COUNT )
	{ LDEB(n); return;	}

    np->npNumberStyle= n;

    return;
    }

static void tedFootnoteRestartChosen(	int		n,
					void *		voidnt )
    {
    NotesTool *		nt= (NotesTool *)voidnt;
    NotesProperties *	np= &(nt->ntNotesPropertiesChosen.fepFootnotesProps);

    if  ( n < 0 || n >= TED_FootnoteRestartCount )
	{ LLDEB(n,TED_FootnoteRestartCount); return;	}

    np->npRestart= TED_FootnoteRestarts[n];

    tedNotesRefreshNotesStartNumber( nt, np );

    return;
    }

static void tedEndnoteRestartChosen(	int		n,
					void *		voidnt )
    {
    NotesTool *		nt= (NotesTool *)voidnt;
    NotesProperties *	np= &(nt->ntNotesPropertiesChosen.fepEndnotesProps);

    if  ( n < 0 || n >= TED_EndnoteRestartCount )
	{ LLDEB(n,TED_EndnoteRestartCount); return;	}

    np->npRestart= TED_EndnoteRestarts[n];

    tedNotesRefreshNotesStartNumber( nt, np );

    return;
    }

static void tedEndnotePlacementChosen(	int		n,
					void *		voidnt )
    {
    NotesTool *		nt= (NotesTool *)voidnt;
    NotesProperties *	np= &(nt->ntNotesPropertiesChosen.fepEndnotesProps);

    if  ( n < 0 || n >= TED_EndnotePlacementCount )
	{ LLDEB(n,TED_EndnotePlacementCount); return;	}

    if  ( TED_EndnotePlacements[n] < 0			||
	  TED_EndnotePlacements[n] >= FTNplace_COUNT	)
	{ LDEB(TED_EndnotePlacements[n]); return;	}

    np->npPlacement= TED_EndnotePlacements[n];

    return;
    }

static void tedFootnoteJustifyChosen(	int		n,
					void *		voidnt )
    {
    NotesTool *		nt= (NotesTool *)voidnt;
    NotesProperties *	np= &(nt->ntNotesPropertiesChosen.fepFootnotesProps);

    if  ( n < 0 || n >= FTNjustify_COUNT )
	{ LDEB(n); return;	}

    np->npJustification= n;

    return;
    }

static void tedEndnoteJustifyChosen(	int		n,
					void *		voidnt )
    {
    NotesTool *		nt= (NotesTool *)voidnt;
    NotesProperties *	np= &(nt->ntNotesPropertiesChosen.fepEndnotesProps);

    if  ( n < 0 || n >= FTNjustify_COUNT )
	{ LDEB(n); return;	}

    np->npJustification= n;

    return;
    }

/************************************************************************/
/*									*/
/*  Make the 'Notes' page of the format tool.				*/
/*									*/
/************************************************************************/

static void tedNotesMakeNotePropertiesTool(
				APP_WIDGET			pageWidget,
				const char *			title,
				NotesTool *			nt,
				const NotesPageResources *	npr,
				APP_TXACTIVATE_CALLBACK_T	startCB,
				OptionmenuCallback		justifyCB,
				OptionmenuCallback		placementCB,
				OptionmenuCallback		restartCB,
				OptionmenuCallback		styleCB )
    {
    const int	textColumns= 6;

    guiToolMakeLabelAndMenuRow( &(nt->ntJustifyRow), &(nt->ntJustifyOptionmenu),
			    &(nt->ntJustifyLabel),
			    pageWidget, npr->nprJustifyText,
			    justifyCB, (void *)nt );

    if  ( placementCB )
	{
	guiToolMakeLabelAndMenuRow( &(nt->ntPlacementRow),
			    &(nt->ntPlacementOptionmenu),
			    &(nt->ntPlacementLabel),
			    pageWidget, npr->nprPositionText,
			    placementCB, (void *)nt );
	}

    guiToolMakeLabelAndMenuRow( &(nt->ntRestartRow), &(nt->ntRestartOptionmenu),
			    &(nt->ntRestartLabel),
			    pageWidget, npr->nprRestartText,
			    restartCB, (void *)nt );

    guiToolMakeLabelAndMenuRow( &(nt->ntStyleRow), &(nt->ntStyleOptionmenu),
			    &(nt->ntStyleLabel), pageWidget, npr->nprStyleText,
			    styleCB, (void *)nt );

    guiToolMakeLabelAndTextRow( &(nt->ntStartNumberRow),
			&(nt->ntStartNumberLabel), &(nt->ntStartNumberText),
			pageWidget, npr->nprFirstNumberText,
			textColumns, 1 );

    if  ( startCB )
	{
	appGuiSetGotValueCallbackForText( nt->ntStartNumberText,
						    startCB, (void *)nt );
	}

    return;
    }

void tedFormatFillNotesPage(	NotesTool *			nt,
				const NotesPageResources *	npr,
				InspectorSubject *		is,
				APP_WIDGET			pageWidget,
				const InspectorSubjectResources * isr,
				int				treeType )
    {
    APP_WIDGET	row= (APP_WIDGET )0;

    /**/
    nt->ntPageResources= npr;
    nt->ntInProgrammaticChange= 0;

    nt->ntRevertSelectionWidget= (APP_WIDGET)0;
    nt->ntChangeSelectionWidget= (APP_WIDGET)0;


    docInitFootEndNotesProperties( &(nt->ntNotesPropertiesSetDocument) );
    docInitFootEndNotesProperties( &(nt->ntNotesPropertiesSetSelection) );
    docInitFootEndNotesProperties( &(nt->ntNotesPropertiesChosen) );

    nt->ntNoteTreeType= treeType;

    /**************/

    if  ( nt->ntNoteTreeType == DOCinFOOTNOTE )
	{
	tedNotesMakeNotePropertiesTool( pageWidget, npr->nprFootnotesText, nt,
					npr, tedNotesFootRestartChanged,
					tedFootnoteJustifyChosen,
					(OptionmenuCallback)0,
					tedFootnoteRestartChosen,
					tedFootnoteStyleChosen );
	}

    if  ( nt->ntNoteTreeType == DOCinENDNOTE )
	{
	tedNotesMakeNotePropertiesTool( pageWidget, npr->nprEndnotesText, nt,
					npr, tedNotesEndRestartChanged,
					tedEndnoteJustifyChosen,
					tedEndnotePlacementChosen,
					tedEndnoteRestartChosen,
					tedEndnoteStyleChosen );
	}

    /**************/

    guiToolMake2BottonRow( &row, pageWidget,
				&(nt->ntRevertSelectionWidget),
				&(nt->ntChangeSelectionWidget),
				npr->nprRevertSelectionText,
				npr->nprChangeSelectionText,
				tedNotesToolRevertSelectionPushed,
				tedNotesToolChangeSelectionPushed,
				(void *)nt );

    guiToolMake2BottonRow( &(is->isApplyRow), pageWidget,
				&(is->isRevertButton),
				&(is->isApplyButton),
				isr->isrRevert,
				isr->isrApplyToSubject,
				tedNotesToolRevertDocumentPushed,
				tedNotesToolChangeDocumentPushed,
				(void *)nt );

    return;
    }

static void tedNotePropertiesToolFillStyleChooser(
				NotesTool *			nt,
				const NotesPageResources *	npr )
    {
    appFillInspectorMenu( FTNjustify_COUNT, FTNjustifyBELOW_TEXT,
		    nt->ntJustifyOptions, npr->nprJustifyMenuTexts,
		    &(nt->ntJustifyOptionmenu) );

    appFillInspectorMenu( FTNstyle_COUNT, FTNstyleNAR,
		    nt->ntStyleOptions, npr->nprNumberStyleMenuTexts,
		    &(nt->ntStyleOptionmenu) );

    return;
    }

static void tedNotePropertiesToolFillChoosers(
			NotesTool *			nt,
			const NotesPageResources *	npr,
			const int *			restarts,
			int				restartCount,
			const int *			placements,
			int				placementCount )
    {
    int		i;

    for ( i= 0; i < restartCount; i++ )
	{
	nt->ntRestartOptions[i]=
		appAddItemToOptionmenu( &(nt->ntRestartOptionmenu),
			npr->nprRestartMenuTexts[restarts[i]] );
	}

    for ( i= 0; i < placementCount; i++ )
	{
	nt->ntPlacementOptions[i]=
		appAddItemToOptionmenu( &(nt->ntPlacementOptionmenu),
			npr->nprPlacementMenuTexts[placements[i]] );
	}

    return;
    }

void tedFormatFillNotesChoosers(	NotesTool *			nt,
					const NotesPageResources *	npr )
    {
    tedNotePropertiesToolFillStyleChooser( nt, npr );

    if  ( nt->ntNoteTreeType == DOCinFOOTNOTE )
	{
	tedNotePropertiesToolFillChoosers( nt, npr,
				TED_FootnoteRestarts,
				TED_FootnoteRestartCount,
				(int *)0, 0 );
	}

    if  ( nt->ntNoteTreeType == DOCinENDNOTE )
	{
	tedNotePropertiesToolFillChoosers( nt, npr,
				TED_EndnoteRestarts,
				TED_EndnoteRestartCount,
				TED_EndnotePlacements,
				TED_EndnotePlacementCount );
	}
    }

void tedFormatFinishNotesPage(		NotesTool *			nt,
					const NotesPageResources *	npr )
    {
    if  ( nt->ntJustifyOptionmenu.aomInplace )
	{ appOptionmenuRefreshWidth( &(nt->ntJustifyOptionmenu) );	}

    if  ( nt->ntPlacementOptionmenu.aomInplace )
	{ appOptionmenuRefreshWidth( &(nt->ntPlacementOptionmenu) );	}

    if  ( nt->ntRestartOptionmenu.aomInplace )
	{ appOptionmenuRefreshWidth( &(nt->ntRestartOptionmenu) );	}

    if  ( nt->ntStyleOptionmenu.aomInplace )
	{ appOptionmenuRefreshWidth( &(nt->ntStyleOptionmenu) );	}

    return;
    }

/************************************************************************/
/*									*/
/*  Clean the notes tool.						*/
/*									*/
/************************************************************************/

void tedFormatCleanNotesTool(	NotesTool *	nt )
    {
    /* clean ntNotesPropertiesSetDocument */
    /* clean ntNotesPropertiesSetSelection */
    /* clean ntNotesPropertiesChosen */

    return;
    }

void tedFormatInitNotesTool(	NotesTool *		nt )
    {
    int		i;

    nt->ntNoteTreeType= DOCinBODY;

    nt->ntApplication= (EditApplication *)0;
    nt->ntInspector= (AppInspector *)0;
    nt->ntPageResources= (NotesPageResources *)0;

    nt->ntInProgrammaticChange= 0;

    docInitFootEndNotesProperties( &(nt->ntNotesPropertiesSetDocument) );
    docInitFootEndNotesProperties( &(nt->ntNotesPropertiesSetSelection) );
    docInitFootEndNotesProperties( &(nt->ntNotesPropertiesChosen) );

    nt->ntRevertSelectionWidget= (APP_WIDGET)0;
    nt->ntChangeSelectionWidget= (APP_WIDGET)0;

    nt->ntJustifyRow= (APP_WIDGET)0;
    appInitOptionmenu( &(nt->ntJustifyOptionmenu) );
    for ( i= 0; i < FTNjustify_COUNT; i++ )
	{ nt->ntJustifyOptions[i]= (APP_WIDGET)0;	}

    nt->ntPlacementRow= (APP_WIDGET)0;
    appInitOptionmenu( &(nt->ntPlacementOptionmenu) );
    for ( i= 0; i < FTNplace_COUNT; i++ )
	{ nt->ntPlacementOptions[i]= (APP_WIDGET)0;	}

    nt->ntRestartRow= (APP_WIDGET)0;
    appInitOptionmenu( &(nt->ntRestartOptionmenu) );
    for ( i= 0; i < FTN_RST__COUNT; i++ )
	{ nt->ntRestartOptions[i]= (APP_WIDGET)0;	}

    nt->ntStyleRow= (APP_WIDGET)0;
    appInitOptionmenu( &(nt->ntStyleOptionmenu) );
    for ( i= 0; i < FTNstyle_COUNT; i++ )
	{ nt->ntStyleOptions[i]= (APP_WIDGET)0;	}

    nt->ntStartNumberText= (APP_WIDGET)0;

    return;
    }

/************************************************************************/
/*									*/
/*  Get notes tool resources.						*/
/*									*/
/************************************************************************/

static AppConfigurableResource TED_TedNotesSubjectResourceTable[]=
    {
    APP_RESOURCE( "formatToolNotes",
		offsetof(InspectorSubjectResources,isrSubjectName),
		"Notes" ),
    APP_RESOURCE( "formatToolChangeNotes",
		offsetof(InspectorSubjectResources,isrApplyToSubject),
		"Apply to Document" ),
    APP_RESOURCE( "tableToolRevertDocumentNotesProperties",
		offsetof(InspectorSubjectResources,isrRevert),
		"Revert to Document" ),
    };

static AppConfigurableResource TED_TedNotesToolResourceTable[]=
    {
    APP_RESOURCE( "tableToolRevertSection",
		offsetof(NotesPageResources,nprRevertSelectionText),
		"Revert to Selection" ),
    APP_RESOURCE( "tableToolChangeSelection",
		offsetof(NotesPageResources,nprChangeSelectionText),
		"Apply to Selection" ),
    /**/
#   if 0
    APP_RESOURCE( "formatToolThisNote",
		offsetof(NotesPageResources,nprCurrentNoteText),
		"This Note" ),
#   endif

    APP_RESOURCE( "formatToolFootnotes",
		offsetof(NotesPageResources,nprFootnotesText),
		"Footnotes" ),
    APP_RESOURCE( "formatToolEndnotes",
		offsetof(NotesPageResources,nprEndnotesText),
		"Endnotes" ),

    /**/
    APP_RESOURCE( "formatToolNotesFirstNumber",
		offsetof(NotesPageResources,nprFirstNumberText),
		"First Number" ),
    APP_RESOURCE( "formatToolNotesJustify",
		offsetof(NotesPageResources,nprJustifyText),
		"Justify" ),
    APP_RESOURCE( "formatToolNotesPosition",
		offsetof(NotesPageResources,nprPositionText),
		"Position" ),
    APP_RESOURCE( "formatToolNotesRestart",
		offsetof(NotesPageResources,nprRestartText),
		"Numbering" ),
    APP_RESOURCE( "formatToolNotesNumberStyle",
		offsetof(NotesPageResources,nprStyleText),
		"Number Format" ),
    /**/
    APP_RESOURCE( "formatToolNoteNumberStyleNar",
	    offsetof(NotesPageResources,nprNumberStyleMenuTexts[FTNstyleNAR]),
	    "1, 2, 3" ),
    APP_RESOURCE( "formatToolNoteNumberStyleNalc",
	    offsetof(NotesPageResources,nprNumberStyleMenuTexts[FTNstyleNALC]),
	    "a, b, c" ),
    APP_RESOURCE( "formatToolNoteNumberStyleNauc",
	    offsetof(NotesPageResources,nprNumberStyleMenuTexts[FTNstyleNAUC]),
	    "A, B, C" ),
    APP_RESOURCE( "formatToolNoteNumberStyleNrlc",
	    offsetof(NotesPageResources,nprNumberStyleMenuTexts[FTNstyleNRLC]),
	    "i, ii, iii" ),
    APP_RESOURCE( "formatToolNoteNumberStyleNruc",
	    offsetof(NotesPageResources,nprNumberStyleMenuTexts[FTNstyleNRUC]),
	    "I, II, III" ),
    APP_RESOURCE( "formatToolNoteNumberStyleNchi",
	    offsetof(NotesPageResources,nprNumberStyleMenuTexts[FTNstyleNCHI]),
	    "*, +, #, $" ),
    /**/
    APP_RESOURCE( "formatToolNoteNumberingContinuous",
	offsetof(NotesPageResources,nprRestartMenuTexts[FTN_RST_CONTINUOUS]),
	"Continuous" ),
    APP_RESOURCE( "formatToolNoteNumberingPerSection",
	offsetof(NotesPageResources,nprRestartMenuTexts[FTN_RST_PER_SECTION]),
	"Per Section" ),
    APP_RESOURCE( "formatToolNoteNumberingPerPage",
	offsetof(NotesPageResources,nprRestartMenuTexts[FTN_RST_PER_PAGE]),
	"Per Page" ),
    /**/
    APP_RESOURCE( "formatToolNotePlacementSectEnd",
	offsetof(NotesPageResources,nprPlacementMenuTexts[FTNplaceSECT_END]),
	"End of Section" ),
    APP_RESOURCE( "formatToolNotePlacementDocEnd",
	offsetof(NotesPageResources,nprPlacementMenuTexts[FTNplaceDOC_END]),
	"End of Document" ),

    APP_RESOURCE( "formatToolNoteJustifyBelowText",
	offsetof(NotesPageResources,nprJustifyMenuTexts[FTNjustifyBELOW_TEXT]),
	"Below Text" ),
    APP_RESOURCE( "formatToolNoteJustifyPageBottom",
	offsetof(NotesPageResources,nprJustifyMenuTexts[FTNjustifyPAGE_BOTTOM]),
	"Page Bottom" ),
    };

void tedFormatToolGetNotesResourceTable( EditApplication *		ea,
					NotesPageResources *		npr,
					InspectorSubjectResources *	isr )
    {
    static int	gotToolResources= 0;
    static int	gotSubjectResources= 0;

    if  ( ! gotToolResources )
	{
	appGuiGetResourceValues( &gotToolResources, ea, (void *)npr,
				TED_TedNotesToolResourceTable,
				sizeof(TED_TedNotesToolResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    if  ( ! gotSubjectResources )
	{
	appGuiGetResourceValues( &gotSubjectResources, ea, (void *)isr,
				TED_TedNotesSubjectResourceTable,
				sizeof(TED_TedNotesSubjectResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    return;
    }
