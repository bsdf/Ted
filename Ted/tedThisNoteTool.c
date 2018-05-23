/************************************************************************/
/*									*/
/*  A tool to manage the style of notes in the document.		*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdio.h>
#   include	<stddef.h>
#   include	<limits.h>

#   include	<docEvalField.h>

#   include	"tedThisNoteTool.h"
#   include	"tedAppFront.h"
#   include	<docTreeType.h>
#   include	<guiToolUtil.h>
#   include	<guiTextUtil.h>
#   include	<docTreeNode.h>
#   include	<docNodeTree.h>
#   include	<docDocumentNote.h>
#   include	<docNotes.h>
#   include	<docEditCommand.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Refresh the 'This Note' page of the format tool.			*/
/*									*/
/************************************************************************/

static int docThisNoteToolDefaultText(
				const ThisNoteTool *		tnt,
				NoteProperties *		np )
    {
    if  ( docFormatChftnField( &(np->npFixedText),
		&(tnt->tntNotesPropertiesSet), np, tnt->tntNoteNumber )	)
	{ LDEB(tnt->tntNoteNumber);	}

    return 0;
    }

static void tedThisNoteToolRefreshFixedTextSet(
				ThisNoteTool *			tnt )
    {
    if  ( docThisNoteToolDefaultText( tnt, &(tnt->tntNotePropertiesSet) ) )
	{ LDEB(1);	}

    if  ( utilCopyMemoryBuffer( &(tnt->tntNotePropertiesChosen.npFixedText),
				&(tnt->tntNotePropertiesSet.npFixedText) ) )
	{ LDEB(2);	}
    }

static void tedThisNoteToolRefreshFixedTextChosen(
				ThisNoteTool *			tnt )
    {
    if  ( docThisNoteToolDefaultText( tnt, &(tnt->tntNotePropertiesChosen) ) )
	{ LDEB(1);	}

    guiBufferToText( tnt->tntFixedTextText,
			    &(tnt->tntNotePropertiesChosen.npFixedText) );
    }

static void tedThisNoteToolRefreshWidgets(	ThisNoteTool *	tnt )
    {
    guiEnableWidget( tnt->tntToNoteRefButton, tnt->tntInsideNote );
    guiEnableWidget( tnt->tntToNoteButton, ! tnt->tntInsideNote );

    appGuiSetToggleState( tnt->tntFootnoteToggle,
	    tnt->tntNotePropertiesChosen.npTreeType == DOCinFOOTNOTE );

    appGuiSetToggleState( tnt->tntEndnoteToggle,
	    tnt->tntNotePropertiesChosen.npTreeType == DOCinENDNOTE );

    appGuiSetToggleState( tnt->tntFixedTextToggle,
				tnt->tntCanChange			&&
				! tnt->tntNotePropertiesChosen.npAutoNumber );

    guiEnableText( tnt->tntFixedTextText,
				tnt->tntCanChange			&&
				! tnt->tntNotePropertiesChosen.npAutoNumber );

    guiBufferToText( tnt->tntFixedTextText,
			    &(tnt->tntNotePropertiesChosen.npFixedText) );


    return;
    }

static void tedThisNoteToolRefreshNoteWidgets(	ThisNoteTool *	tnt )
    {
    tnt->tntInProgrammaticChange++;
    tedThisNoteToolRefreshWidgets( tnt );
    tnt->tntInProgrammaticChange--;
    }

void tedRefreshThisNoteTool(	ThisNoteTool *			tnt,
				int *				pEnabled,
				int *				pPref,
				InspectorSubject *		is,
				const DocumentSelection *	ds,
				const SelectionDescription *	sd,
				BufferDocument *		bd,
				const unsigned char *		cmdEnabled )
    {
    const DocumentProperties *	dp= &(bd->bdProperties);

    DocumentField *	dfNote= (DocumentField *)0;
    DocumentNote *	dn= (DocumentNote *)0;
    int			selInNote= 0;

    if  ( ! ds || ! sd->sdHasNote )
	{ *pEnabled= 0; return;	}

    tnt->tntNotesPropertiesSet= dp->dpNotesProps;

    tnt->tntInsideNote= 0;
    tnt->tntCanChange= 0;
    docCleanNoteProperties( &(tnt->tntNotePropertiesSet) );
    docInitNoteProperties( &(tnt->tntNotePropertiesSet) );
    tnt->tntNotePropertiesSet.npTreeType= DOCinBODY;

    tnt->tntNoteNumber= 0;

    dfNote= docGetSelectedNote( &dn, &selInNote, bd, ds );
    if  ( dfNote )
	{
	const BufferItem *	bodySectNode;

	bodySectNode= docGetBodySectNode( dn->dnDocumentTree.dtRoot, bd );

	tnt->tntNotesPropertiesSet= bodySectNode->biSectNotesProperties;

	if  ( docCopyNoteProperties( &(tnt->tntNotePropertiesSet),
						&(dn->dnNoteProperties) ) )
	    { LDEB(1);	}

	tnt->tntNoteNumber= dn->dnNoteNumber;
	tnt->tntInsideNote= selInNote;

	if  ( docThisNoteToolDefaultText( tnt, &(tnt->tntNotePropertiesSet) ) )
	    { LDEB(1);	}

	guiEnableWidget( is->isNextButton,
			    docGetNextNoteInDocument( (DocumentNote **)0, bd,
			    dfNote, -1 ) != (DocumentField *)0 );
	guiEnableWidget( is->isPrevButton,
			    docGetPrevNoteInDocument( (DocumentNote **)0, bd,
			    dfNote, -1 ) != (DocumentField *)0 );

	(*pPref) += 4;
	}
    else{ XDEB(dfNote); *pEnabled= 0; return;	}

    if  ( docCopyNoteProperties( &(tnt->tntNotePropertiesChosen),
					    &(tnt->tntNotePropertiesSet) ) )
	{ LDEB(1);	}

    if  ( tnt->tntNotePropertiesSet.npAutoNumber		&&
	  tnt->tntNotePropertiesSet.npTreeType != DOCinBODY	)
	{ tedThisNoteToolRefreshFixedTextSet( tnt );	}

    tnt->tntCanChange= cmdEnabled[EDITcmdUPD_NOTE];

    tedThisNoteToolRefreshWidgets( tnt );

    guiEnableWidget( tnt->tntNoteTypeRow,  tnt->tntCanChange );
    guiEnableWidget( tnt->tntFixedTextRow,  tnt->tntCanChange );

    guiEnableWidget( is->isApplyButton, tnt->tntCanChange );
    guiEnableWidget( is->isRevertButton, tnt->tntCanChange );

    *pEnabled= 1;
    return;
    }

/************************************************************************/
/*									*/
/*  Jump back and forth between a note and its reference.		*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedThisNoteToolToNoteRef, w, voidtnt )
    {
    ThisNoteTool *	tnt= (ThisNoteTool *)voidtnt;

    tedAppGotoNoteRef( tnt->tntApplication );

    return;
    }

static APP_BUTTON_CALLBACK_H( tedThisNoteToolToNoteDef, w, voidtnt )
    {
    ThisNoteTool *	tnt= (ThisNoteTool *)voidtnt;

    tedAppGotoNoteDef( tnt->tntApplication );

    return;
    }

/************************************************************************/
/*									*/
/*  Navigation between the notes in the document.			*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedThisNoteToolNextNote, w, voidtnt )
    {
    ThisNoteTool *	tnt= (ThisNoteTool *)voidtnt;

    tedAppNextNote( tnt->tntApplication );

    return;
    }

static APP_BUTTON_CALLBACK_H( tedThisNoteToolPrevNote, w, voidtnt )
    {
    ThisNoteTool *	tnt= (ThisNoteTool *)voidtnt;

    tedAppPrevNote( tnt->tntApplication );

    return;
    }

/************************************************************************/
/*									*/
/*  Set/Revert buttons for the current note have been pushed.		*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedChangeNotePushed, w, voidtnt )
    {
    ThisNoteTool *		tnt= (ThisNoteTool *)voidtnt;
    PropertyMask		npCmpMask;
    PropertyMask		npSetMask;

    utilPropMaskClear( &npCmpMask );
    utilPropMaskClear( &npSetMask );

    utilPropMaskFill( &npCmpMask, NOTEprop_COUNT );

    appBufferFromTextWidget( &(tnt->tntNotePropertiesChosen.npFixedText),
							tnt->tntFixedTextText );
    if  ( ! tnt->tntNotePropertiesChosen.npAutoNumber		&&
	  utilMemoryBufferIsEmpty( &(tnt->tntNotePropertiesChosen.npFixedText) ) )
	{
	appStringToTextWidget( tnt->tntFixedTextText, "?" );
	appRefuseTextValue( tnt->tntFixedTextText );
	return;
	}

    docNotePropertyDifference( &npSetMask, &(tnt->tntNotePropertiesSet),
				&npCmpMask, &(tnt->tntNotePropertiesChosen) );

    if  ( tnt->tntNotePropertiesChosen.npAutoNumber )
	{ PROPmaskUNSET( &npSetMask, NOTEpropFIXED_TEXT );	}

    if  ( ! utilPropMaskIsEmpty( &npSetMask ) )
	{
	tedAppChangeCurrentNote( tnt->tntApplication,
				&npSetMask, &(tnt->tntNotePropertiesChosen) );
	}

    return;
    }

static APP_BUTTON_CALLBACK_H( tedRevertNotePushed, w, voidtnt )
    {
    ThisNoteTool *	tnt= (ThisNoteTool *)voidtnt;

    if  ( docCopyNoteProperties( &(tnt->tntNotePropertiesChosen),
					    &(tnt->tntNotePropertiesSet) ) )
	{ LDEB(1);	}

    tedThisNoteToolRefreshNoteWidgets( tnt );

    return;
    }

/************************************************************************/
/*									*/
/*  One of the toggles to decide whether the current note is a footnote	*/
/*  or endnote has been toggled.					*/
/*									*/
/************************************************************************/

static APP_TOGGLE_CALLBACK_H( tedFootnoteToggled, w, voidtnt, voidtbcs )
    {
    ThisNoteTool *	tnt= (ThisNoteTool *)voidtnt;
    int			set;

    if  ( tnt->tntInProgrammaticChange )
	{ return;	}

    set= appGuiGetToggleStateFromCallback( w, voidtbcs );

    if  ( set && tnt->tntNotePropertiesChosen.npTreeType != DOCinFOOTNOTE )
	{
	tnt->tntNotePropertiesChosen.npTreeType= DOCinFOOTNOTE;

	if  ( tnt->tntNotePropertiesChosen.npAutoNumber )
	    { tedThisNoteToolRefreshFixedTextChosen( tnt );	}

	tedThisNoteToolRefreshNoteWidgets( tnt );
	}

    return;
    }

static APP_TOGGLE_CALLBACK_H( tedEndnoteToggled, w, voidtnt, voidtbcs )
    {
    ThisNoteTool *	tnt= (ThisNoteTool *)voidtnt;
    int			set;

    if  ( tnt->tntInProgrammaticChange )
	{ return;	}

    set= appGuiGetToggleStateFromCallback( w, voidtbcs );

    if  ( set && tnt->tntNotePropertiesChosen.npTreeType != DOCinENDNOTE )
	{
	tnt->tntNotePropertiesChosen.npTreeType= DOCinENDNOTE;
	
	if  ( tnt->tntNotePropertiesChosen.npAutoNumber )
	    { tedThisNoteToolRefreshFixedTextChosen( tnt );	}

	tedThisNoteToolRefreshNoteWidgets( tnt );
	}

    return;
    }

static APP_TOGGLE_CALLBACK_H( tedFixedNoteTextToggled, w, voidtnt, voidtbcs )
    {
    ThisNoteTool *	tnt= (ThisNoteTool *)voidtnt;
    int			set;

    if  ( tnt->tntInProgrammaticChange )
	{ return;	}

    set= appGuiGetToggleStateFromCallback( w, voidtbcs );

    if  ( tnt->tntNotePropertiesChosen.npAutoNumber == set )
	{
	tnt->tntNotePropertiesChosen.npAutoNumber= ! set;

	if  ( tnt->tntNotePropertiesChosen.npAutoNumber )
	    { tedThisNoteToolRefreshFixedTextChosen( tnt );	}
	}

    tedThisNoteToolRefreshNoteWidgets( tnt );

    return;
    }

void tedFormatFillThisNotePage(	ThisNoteTool *			tnt,
				const ThisNotePageResources *	npr,
				InspectorSubject *		is,
				APP_WIDGET			pageWidget,
				const InspectorSubjectResources * isr )
    {
    const int	textColumns= 6;

    /**/
    tnt->tntPageResources= npr;
    tnt->tntInProgrammaticChange= 0;

    docInitFootEndNotesProperties( &(tnt->tntNotesPropertiesSet) );

    docInitNoteProperties( &(tnt->tntNotePropertiesSet) );
    docInitNoteProperties( &(tnt->tntNotePropertiesChosen) );

    guiToolMake2ToggleRow( &(tnt->tntNoteTypeRow), pageWidget,
		    &(tnt->tntFootnoteToggle), &(tnt->tntEndnoteToggle),
		    npr->nprFootnoteText, npr->nprEndnoteText,
		    tedFootnoteToggled,
		    tedEndnoteToggled, (void *)tnt, (void *)tnt );

    guiToolMakeToggleAndTextRow( &(tnt->tntFixedTextRow),
		    &(tnt->tntFixedTextToggle),
		    &(tnt->tntFixedTextText), pageWidget,
		    npr->nprFixedTextText,
		    tedFixedNoteTextToggled, (void *)tnt,
		    textColumns, 1 );

    guiToolMake2BottonRow( &(tnt->tntToOtherRow), pageWidget,
		    &(tnt->tntToNoteRefButton), &(tnt->tntToNoteButton),
		    npr->nprToNoteRefText, npr->nprToNoteText,
		    tedThisNoteToolToNoteRef, tedThisNoteToolToNoteDef,
		    (void *)tnt );

    guiToolMake2BottonRow( &(is->isNextPrevRow), pageWidget,
		    &(is->isPrevButton), &(is->isNextButton),
		    isr->isrPrevButtonText, isr->isrNextButtonText,
		    tedThisNoteToolPrevNote, tedThisNoteToolNextNote,
		    (void *)tnt );

    guiToolMake2BottonRow( &(is->isApplyRow), pageWidget,
		    &(is->isRevertButton), &(is->isApplyButton),
		    isr->isrRevert, isr->isrApplyToSubject,
		    tedRevertNotePushed, tedChangeNotePushed,
		    (void *)tnt );

    return;
    }

/************************************************************************/
/*									*/
/*  Clean the notes tool.						*/
/*									*/
/************************************************************************/

void tedFormatCleanThisNoteTool(	ThisNoteTool *	tnt )
    {
    docCleanNoteProperties( &(tnt->tntNotePropertiesSet) );
    docCleanNoteProperties( &(tnt->tntNotePropertiesChosen) );

    return;
    }

void tedFormatInitThisNoteTool(	ThisNoteTool *	tnt )
    {
    tnt->tntApplication= (EditApplication *)0;
    tnt->tntInspector= (AppInspector *)0;
    tnt->tntPageResources= (ThisNotePageResources *)0;

    tnt->tntInProgrammaticChange= 0;

    docInitNoteProperties( &(tnt->tntNotePropertiesSet) );
    docInitNoteProperties( &(tnt->tntNotePropertiesChosen) );

    tnt->tntInsideNote= 0;
    tnt->tntCanChange= 0;

    tnt->tntNoteTypeRow= (APP_WIDGET)0;
    tnt->tntFootnoteToggle= (APP_WIDGET)0;
    tnt->tntEndnoteToggle= (APP_WIDGET)0;

    tnt->tntFixedTextRow= (APP_WIDGET)0;
    tnt->tntFixedTextToggle= (APP_WIDGET)0;
    tnt->tntFixedTextText= (APP_WIDGET)0;

    tnt->tntToOtherRow= (APP_WIDGET)0;
    tnt->tntToNoteRefButton= (APP_WIDGET)0;
    tnt->tntToNoteButton= (APP_WIDGET)0;

    return;
    }

/************************************************************************/
/*									*/
/*  Get notes tool resources.						*/
/*									*/
/************************************************************************/

static AppConfigurableResource TED_TedThisNoteSubjectResourceTable[]=
    {
    APP_RESOURCE( "formatToolThisNote",
		offsetof(InspectorSubjectResources,isrSubjectName),
		"This Note" ),

    APP_RESOURCE( "formatToolNextNote",
		offsetof(InspectorSubjectResources,isrNextButtonText),
		"Next Note" ),
    APP_RESOURCE( "formatToolPreviousNote",
		offsetof(InspectorSubjectResources,isrPrevButtonText),
		"Previous Note" ),

    APP_RESOURCE( "formatToolRevertNote",
		offsetof(InspectorSubjectResources,isrApplyToSubject),
		"Apply to Note" ),
    APP_RESOURCE( "formatToolChangeNote",
		offsetof(InspectorSubjectResources,isrRevert),
		"Revert" ),
    };

static AppConfigurableResource TED_TedThisNoteToolResourceTable[]=
    {
    /**/
    APP_RESOURCE( "formatToolNoteIsFootnote",
		offsetof(ThisNotePageResources,nprFootnoteText),
		"Footnote" ),
    APP_RESOURCE( "formatToolNoteIsEndnote",
		offsetof(ThisNotePageResources,nprEndnoteText),
		"Endnote" ),

    APP_RESOURCE( "formatToolNoteHasFixedText",
		offsetof(ThisNotePageResources,nprFixedTextText),
		"Fixed Text" ),

    APP_RESOURCE( "formatToolToNoteRef",
		offsetof(ThisNotePageResources,nprToNoteRefText),
		"Find Note" ),
    APP_RESOURCE( "formatToolToNote",
		offsetof(ThisNotePageResources,nprToNoteText),
		"Edit Note" ),
    };

void tedFormatToolGetThisNoteResourceTable( EditApplication *		ea,
					ThisNotePageResources *		npr,
					InspectorSubjectResources *	isr )
    {
    static int	gotToolResources= 0;
    static int	gotSubjectResources= 0;

    if  ( ! gotToolResources )
	{
	appGuiGetResourceValues( &gotToolResources, ea, (void *)npr,
				TED_TedThisNoteToolResourceTable,
				sizeof(TED_TedThisNoteToolResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    if  ( ! gotSubjectResources )
	{
	appGuiGetResourceValues( &gotSubjectResources, ea, (void *)isr,
				TED_TedThisNoteSubjectResourceTable,
				sizeof(TED_TedThisNoteSubjectResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    return;
    }
