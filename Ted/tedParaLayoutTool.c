/************************************************************************/
/*									*/
/*  The 'Paragraph' page of the format tool.				*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdio.h>
#   include	<stddef.h>
#   include	<limits.h>

#   include	<appUnit.h>

#   include	"tedParaLayoutTool.h"
#   include	"tedAppFront.h"
#   include	"tedToolUtil.h"
#   include	"tedLayout.h"
#   include	<docTreeType.h>
#   include	<guiToolUtil.h>
#   include	<guiTextUtil.h>
#   include	<docTreeNode.h>
#   include	<docEditCommand.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Update the controls for SpaceAbove/SpaceBelow.			*/
/*									*/
/************************************************************************/

static void tedFormatToolRefreshSpaceAround(	APP_WIDGET	row,
						APP_WIDGET	toggle,
						APP_WIDGET	text,
						int		space,
						int		canChange )
    {
    if  ( space == 0 )
	{
	appStringToTextWidget( text, "" );
	guiEnableText( text, 0 );

	appGuiSetToggleState( toggle, 0 );
	}
    else{
	appLengthToTextWidget( text, space, UNITtyPOINTS );
	guiEnableWidget( row, canChange );
	guiEnableText( text, canChange );

	appGuiSetToggleState( toggle, 1 );
	}

    guiEnableWidget( toggle, canChange );
    }

static void tedFormatToolRefreshParagraphPage(	ParagraphLayoutTool *	plt )
    {
    ParagraphProperties *	pp= &(plt->ptPropertiesChosen);

    guiEnableWidget( plt->pltListLevelRow, plt->pltCanChange );
    guiEnableWidget( plt->pltAlignmentRow, plt->pltCanChange );

    if  ( pp->ppListOverride > 0 )
	{
	appIntegerToTextWidget( plt->pltListLevelText, pp->ppListLevel+ 1 );

	guiEnableText( plt->ptFirstIndentText, 0 );
	guiEnableText( plt->ptLeftIndentText, 0 );
	}
    else{
	appStringToTextWidget( plt->pltListLevelText, "" );

	guiEnableText( plt->ptFirstIndentText, plt->pltCanChange );
	guiEnableText( plt->ptLeftIndentText, plt->pltCanChange );
	}

    guiEnableWidget( plt->ptFirstIndentRow, plt->pltCanChange );
    guiEnableWidget( plt->ptLeftIndentRow, plt->pltCanChange );

    appLengthToTextWidget( plt->ptFirstIndentText,
	    pp->ppLeftIndentTwips+ pp->ppFirstIndentTwips, UNITtyPOINTS );

    appLengthToTextWidget( plt->ptLeftIndentText,
				    pp->ppLeftIndentTwips, UNITtyPOINTS );

    guiEnableWidget( plt->ptRightIndentRow, plt->pltCanChange );
    guiEnableText( plt->ptRightIndentText, plt->pltCanChange );
    appLengthToTextWidget( plt->ptRightIndentText,
				    pp->ppRightIndentTwips, UNITtyPOINTS );

    tedFormatToolRefreshSpaceAround( plt->pltSpaceAboveRow,
					    plt->pltSpaceAboveToggle,
					    plt->pltSpaceAboveText,
					    pp->ppSpaceBeforeTwips,
					    plt->pltCanChange );

    tedFormatToolRefreshSpaceAround( plt->pltSpaceBelowRow,
					    plt->pltSpaceBelowToggle,
					    plt->pltSpaceBelowText,
					    pp->ppSpaceAfterTwips,
					    plt->pltCanChange );

    tedFormatEnableHeightChooser( &(plt->ptLineDistChooser),
						plt->pltCanChange );
    tedFormatRefreshHeightChooser( &(plt->ptLineDistChooser),
						pp->ppLineSpacingTwips );

    appGuiEnableOptionmenu( &(plt->pltAlignOptionmenu), plt->pltCanChange );
    appSetOptionmenu( &(plt->pltAlignOptionmenu), pp->ppAlignment );

    appGuiSetToggleState( plt->ptOnNewPageToggle,
					pp->ppBreakKind == DOCibkPAGE );
    guiEnableWidget( plt->ptKeepOnPageToggle, plt->pltCanChange );
    appGuiSetToggleState( plt->ptKeepOnPageToggle, pp->ppKeepOnPage );

    guiEnableWidget( plt->ptWidowControlToggle, plt->pltCanChange );
    appGuiSetToggleState( plt->ptWidowControlToggle, pp->ppWidowControl );

    guiEnableWidget( plt->ptKeepWithNextToggle, plt->pltCanChange );
    appGuiSetToggleState( plt->ptKeepWithNextToggle, pp->ppKeepWithNext );

    return;
    }

void tedRefreshParaLayoutTool(	ParagraphLayoutTool *		plt,
				int *				pEnabled,
				int *				pPref,
				InspectorSubject *		is,
				const DocumentSelection *	ds,
				const SelectionGeometry *	sg,
				const SelectionDescription *	sd,
				const unsigned char *		cmdEnabled )
    {
    const ParagraphProperties *		pp;

    PropertyMask			ppSetMask;

    BufferItem *			paraNode= ds->dsHead.dpNode;

    pp= &(paraNode->biParaProperties);

    utilPropMaskClear( &ppSetMask );
    utilPropMaskFill( &ppSetMask, PPprop_FULL_COUNT );
    PROPmaskUNSET( &ppSetMask, PPpropTABLE_NESTING );

    if  ( docUpdParaProperties( (PropertyMask *)0, &(plt->ptPropertiesChosen),
					    &ppSetMask, pp,
					    (const DocumentAttributeMap *)0 ) )
	{ LDEB(1); return ;	}

    if  ( docUpdParaProperties( (PropertyMask *)0, &(plt->ptPropertiesSet),
					    &ppSetMask, pp,
					    (const DocumentAttributeMap *)0 ) )
	{ LDEB(1); return ;	}

    plt->pltCanChange= cmdEnabled[EDITcmdUPD_PARA_PROPS];

    guiEnableWidget( plt->ptOnNewPageToggle,
				    plt->pltCanChange &&
				    paraNode->biTreeType == DOCinBODY &&
				    pp->ppTableNesting == 0 );

    tedRefreshParaSubjectControls( is, ds, sg, sd, cmdEnabled );

    tedFormatToolRefreshParagraphPage( plt );

    *pEnabled= 1;
    return;
    }

/************************************************************************/
/*									*/
/*  The 'Revert' button on the paragraph tool has been pushed.		*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedFormatParaRevertPushed, w, voidplt )
    {
    ParagraphLayoutTool *	plt= (ParagraphLayoutTool *)voidplt;

    PropertyMask		ppSetMask;

    utilPropMaskClear( &ppSetMask );
    utilPropMaskFill( &ppSetMask, PPprop_FULL_COUNT );
    PROPmaskUNSET( &ppSetMask, PPpropTABLE_NESTING );

    docUpdParaProperties( (PropertyMask *)0, &(plt->ptPropertiesChosen),
					&ppSetMask, &(plt->ptPropertiesSet),
					(const DocumentAttributeMap *)0 );

    tedFormatToolRefreshParagraphPage( plt );

    return;
    }

/************************************************************************/
/*									*/
/*  Check user input.							*/
/*									*/
/************************************************************************/

static APP_TXACTIVATE_CALLBACK_H( tedParaFirstIndentChanged, w, voidplt )
    {
    ParagraphLayoutTool *	plt= (ParagraphLayoutTool *)voidplt;
    ParagraphProperties *	pp= &(plt->ptPropertiesChosen);

    int				lipfi;
    int				changed;

    lipfi=  pp->ppLeftIndentTwips+ pp->ppFirstIndentTwips;

    tedFormatValidateDimension( &lipfi, &changed,
					    plt->ptFirstIndentText, lipfi );
    return;
    }

static APP_TXACTIVATE_CALLBACK_H( tedParaLeftIndentChanged, w, voidplt )
    {
    ParagraphLayoutTool *	plt= (ParagraphLayoutTool *)voidplt;
    ParagraphProperties *	pp= &(plt->ptPropertiesChosen);

    int				value;
    int				changed;

    tedFormatValidateDimension( &value, &changed,
			    plt->ptLeftIndentText, pp->ppLeftIndentTwips );

    return;
    }

static APP_TXACTIVATE_CALLBACK_H( tedParaRightIndentChanged, w, voidplt )
    {
    ParagraphLayoutTool *	plt= (ParagraphLayoutTool *)voidplt;
    ParagraphProperties *	pp= &(plt->ptPropertiesChosen);

    int				value;
    int				changed;

    tedFormatValidateDimension( &value, &changed,
			    plt->ptRightIndentText, pp->ppRightIndentTwips );

    return;
    }

static APP_TXACTIVATE_CALLBACK_H( tedParaSpaceAboveChanged, w, voidplt )
    {
    ParagraphLayoutTool *	plt= (ParagraphLayoutTool *)voidplt;
    ParagraphProperties *	pp= &(plt->ptPropertiesChosen);

    int				value;
    int				changed;

    tedFormatValidateDimension( &value, &changed,
			    plt->pltSpaceAboveText, pp->ppSpaceBeforeTwips );

    return;
    }

static APP_TXACTIVATE_CALLBACK_H( tedParaSpaceBelowChanged, w, voidplt )
    {
    ParagraphLayoutTool *	plt= (ParagraphLayoutTool *)voidplt;
    ParagraphProperties *	pp= &(plt->ptPropertiesChosen);

    int				value;
    int				changed;

    tedFormatValidateDimension( &value, &changed,
			    plt->pltSpaceBelowText, pp->ppSpaceAfterTwips );

    return;
    }

static APP_TXACTIVATE_CALLBACK_H( tedParaLineDistanceChanged, w, voidplt )
    {
    ParagraphLayoutTool *	plt= (ParagraphLayoutTool *)voidplt;
    ParagraphProperties *	pp= &(plt->ptPropertiesChosen);

    int				value;
    int				changed;

    tedHeightToolValidateDimension( &value, &changed,
			&(plt->ptLineDistChooser), pp->ppLineSpacingTwips );

    return;
    }

/************************************************************************/
/*									*/
/*  Previous/Next Paragraph buttons have been pushed.			*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedFormatPrevPara, w, voidplt )
    {
    ParagraphLayoutTool *	plt= (ParagraphLayoutTool *)voidplt;
    EditApplication *		ea= plt->ptApplication;

    tedAppSelectWholeParagraph( ea, -1 );

    return;
    }

static APP_BUTTON_CALLBACK_H( tedFormatNextPara, w, voidplt )
    {
    ParagraphLayoutTool *	plt= (ParagraphLayoutTool *)voidplt;
    EditApplication *		ea= plt->ptApplication;

    tedAppSelectWholeParagraph( ea, 1 );

    return;
    }

static APP_BUTTON_CALLBACK_H( tedFormatSelectPara, w, voidplt )
    {
    ParagraphLayoutTool *	plt= (ParagraphLayoutTool *)voidplt;
    EditApplication *		ea= plt->ptApplication;

    tedAppSelectWholeParagraph( ea, 0 );

    return;
    }

static APP_BUTTON_CALLBACK_H( tedLayoutDeletePara, w, voidplt )
    {
    ParagraphLayoutTool *	plt= (ParagraphLayoutTool *)voidplt;
    EditApplication *		ea= plt->ptApplication;

    tedAppDeleteSelectedParagraphs( ea );
    }

static APP_BUTTON_CALLBACK_H( tedLayoutInsertPara, w, voidplt )
    {
    ParagraphLayoutTool *	plt= (ParagraphLayoutTool *)voidplt;
    EditApplication *		ea= plt->ptApplication;

    tedAppInsertParagraph( ea, 0 );

    return;
    }

static APP_BUTTON_CALLBACK_H( tedLayoutAppendPara, w, voidplt )
    {
    ParagraphLayoutTool *	plt= (ParagraphLayoutTool *)voidplt;
    EditApplication *		ea= plt->ptApplication;

    tedAppInsertParagraph( ea, 1 );

    return;
    }

/************************************************************************/
/*									*/
/*  The 'Change' button on the paragraph page has been pushed.		*/
/*									*/
/************************************************************************/

static int tedFormatToolGetSpaceAround(		int *		pValue,
						int *		pChanged,
						APP_WIDGET	text,
						APP_WIDGET	toggle,
						int		value )
    {
    int			state;
    int			changed= 0;

    const int		minValue= 1;
    const int		adaptToMin= 0;
    const int		maxValue= INT_MAX;
    const int		adaptToMax= 0;

    state= appGuiGetToggleState( toggle ) != 0;
    if  ( ! state )
	{ *pChanged= ( value != 0 ); *pValue= 0; return 0;	}

    state= value;
    if  ( appGetLengthFromTextWidget( text,
			    &state, &changed, UNITtyPOINTS,
			    minValue, adaptToMin, maxValue, adaptToMax ) )
	{ return -1;	}

    if  ( changed )
	{ value= state;	}

    *pChanged= changed; *pValue=value; return 0;
    }

/************************************************************************/
/*									*/
/*  Retrieve values from controls.					*/
/*									*/
/************************************************************************/

static int tedParaLayoutGetChosen(	ParagraphLayoutTool *	plt )
    {
    ParagraphProperties *	ppChosen= &(plt->ptPropertiesChosen);

    int				li;
    int				lipfi;

    int				value;
    int				changed;

    const int			minValue= 0;
    const int			maxValue= INT_MAX;
    const int			adaptToMax= 0;

    lipfi= ppChosen->ppLeftIndentTwips+ ppChosen->ppFirstIndentTwips;
    if  ( appGetLengthFromTextWidget( plt->ptFirstIndentText,
					&lipfi, &changed, UNITtyPOINTS,
					INT_MIN, 0, maxValue, adaptToMax ) )
	{ return -1;	}

    li= ppChosen->ppLeftIndentTwips;
    if  ( appGetLengthFromTextWidget( plt->ptLeftIndentText,
					&li, &changed, UNITtyPOINTS,
					minValue, 1, maxValue, adaptToMax ) )
	{ return -1;	}

    ppChosen->ppLeftIndentTwips= li;
    ppChosen->ppFirstIndentTwips= lipfi- li;

    value= ppChosen->ppRightIndentTwips;
    if  ( appGetLengthFromTextWidget( plt->ptRightIndentText,
					&value, &changed, UNITtyPOINTS,
					minValue, 1, maxValue, adaptToMax ) )
	{ return -1;	}
    ppChosen->ppRightIndentTwips= value;

    value= ppChosen->ppLineSpacingTwips;
    if  ( tedFormatToolGetHeight( &value, &(plt->ptLineDistChooser) ) )
	{ return -1;	}
    ppChosen->ppLineSpacingTwips= value;

    value= ppChosen->ppSpaceBeforeTwips;
    if  ( tedFormatToolGetSpaceAround( &value, &changed,
		    plt->pltSpaceAboveText, plt->pltSpaceAboveToggle, value ) )
	{ return -1;	}
    ppChosen->ppSpaceBeforeTwips= value;

    value= ppChosen->ppSpaceAfterTwips;
    if  ( tedFormatToolGetSpaceAround( &value, &changed,
		    plt->pltSpaceBelowText, plt->pltSpaceBelowToggle, value ) )
	{ return -1;	}
    ppChosen->ppSpaceAfterTwips= value;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Change paragraph layout.						*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedLayoutChangePara, w, voidplt )
    {
    ParagraphLayoutTool *	plt= (ParagraphLayoutTool *)voidplt;
    EditApplication *		ea= plt->ptApplication;
    const ParagraphProperties *	ppSet= &(plt->ptPropertiesSet);
    const ParagraphProperties *	ppChosen= &(plt->ptPropertiesChosen);

    PropertyMask		ppDifMask;
    PropertyMask		ppCmpMask;

    if  ( tedParaLayoutGetChosen( plt ) )
	{ return;	}

    utilPropMaskClear( &ppCmpMask );
    utilPropMaskClear( &ppDifMask );
    utilPropMaskFill( &ppCmpMask, PPprop_FULL_COUNT );
    PROPmaskUNSET( &ppCmpMask, PPpropTABLE_NESTING );

    docParaPropertyDifference( &ppDifMask, ppSet, &ppCmpMask, ppChosen );

    if  ( tedAppChangeParagraphProperties( ea, &ppDifMask, ppChosen ) )
	{ LDEB(1);	}

    return;
    }

/************************************************************************/
/*									*/
/*  One of the Space toggles was activated:				*/
/*									*/
/************************************************************************/

static APP_TOGGLE_CALLBACK_H( tedFormatSpaceAboveToggled, w, voidplt, voidtbcs )
    {
    ParagraphLayoutTool *		plt= (ParagraphLayoutTool *)voidplt;
    ParagraphProperties *		pp= &(plt->ptPropertiesChosen);

    int					set;

    set= appGuiGetToggleStateFromCallback( w, voidtbcs );

    if  ( set )
	{
	if  ( plt->ptPropertiesSet.ppSpaceBeforeTwips != 0 )
	    {
	    pp->ppSpaceBeforeTwips=
			    plt->ptPropertiesSet.ppSpaceBeforeTwips;
	    }
	else{
	    EditDocument *	ed= plt->ptApplication->eaCurrentDocument;
	    int			lineHeight;

	    if  ( ! ed || tedGetParaLineHeight( &lineHeight, ed ) )
		{ XLDEB(ed,set);			}
	    else{ pp->ppSpaceBeforeTwips= lineHeight;	}
	    }
	}
    else{
	pp->ppSpaceBeforeTwips= 0;
	}

    tedFormatToolRefreshSpaceAround( plt->pltSpaceAboveRow,
					    plt->pltSpaceAboveToggle,
					    plt->pltSpaceAboveText,
					    pp->ppSpaceBeforeTwips,
					    plt->pltCanChange );

    return;
    }

static APP_TOGGLE_CALLBACK_H( tedFormatSpaceBelowToggled, w, voidplt, voidtbcs )
    {
    ParagraphLayoutTool *		plt= (ParagraphLayoutTool *)voidplt;
    ParagraphProperties *		pp= &(plt->ptPropertiesChosen);
    
    int					set;

    set= appGuiGetToggleStateFromCallback( w, voidtbcs );

    if  ( set )
	{
	if  ( plt->ptPropertiesSet.ppSpaceAfterTwips != 0 )
	    {
	    pp->ppSpaceAfterTwips=
			    plt->ptPropertiesSet.ppSpaceAfterTwips;
	    }
	else{
	    EditDocument *	ed= plt->ptApplication->eaCurrentDocument;
	    int			lineHeight;

	    if  ( ! ed || tedGetParaLineHeight( &lineHeight, ed ) )
		{ XLDEB(ed,set);			}
	    else{ pp->ppSpaceAfterTwips= lineHeight;	}
	    }
	}
    else{
	pp->ppSpaceAfterTwips= 0;
	}

    tedFormatToolRefreshSpaceAround( plt->pltSpaceBelowRow,
						plt->pltSpaceBelowToggle,
						plt->pltSpaceBelowText,
						pp->ppSpaceAfterTwips,
						plt->pltCanChange );

    return;
    }

static void tedParaAlignmentChosen(	int		alignment,
					void *		voidplt )
    {
    ParagraphLayoutTool *		plt= (ParagraphLayoutTool *)voidplt;
    ParagraphProperties *		pp= &(plt->ptPropertiesChosen);

    if  ( alignment < 0 || alignment >= DOCtha_COUNT )
	{ LLDEB(alignment,DOCtha_COUNT); return;	}

    pp->ppAlignment= alignment;

    return;
    }

/************************************************************************/
/*									*/
/*  A line Spacing was chosen with the menu.				*/
/*									*/
/************************************************************************/

static void tedParaLineDistChosen(	int		how,
					void *		voidplt )
    {
    ParagraphLayoutTool *	plt= (ParagraphLayoutTool *)voidplt;
    HeightChooser *		hc= &(plt->ptLineDistChooser);

    int				defaultValue;

    defaultValue= plt->ptPropertiesChosen.ppLineSpacingTwips;
    if  ( defaultValue == 0 )
	{ defaultValue= plt->ptPropertiesSet.ppLineSpacingTwips;	}
    if  ( defaultValue == 0 )
	{ defaultValue= 240;	}

    if  ( how < 0 || how >= HC__COUNT )
	{ LDEB(how); return;	}

    tedFormatHeightChosen( how, hc, defaultValue );

    return;
    }

static APP_TOGGLE_CALLBACK_H( tedParaOnNewPageToggled, w, voidplt, voidtbcs )
    {
    ParagraphLayoutTool *	plt= (ParagraphLayoutTool *)voidplt;
    ParagraphProperties *	pp= &(plt->ptPropertiesChosen);

    int				set;

    set= appGuiGetToggleStateFromCallback( w, voidtbcs );

    pp->ppBreakKind= DOCibkNONE;
    if  ( ( set != 0 ) && pp->ppTableNesting == 0 )
	{ pp->ppBreakKind= DOCibkPAGE;	}

    return;
    }

static APP_TOGGLE_CALLBACK_H( tedParaKeepOnPageToggled, w, voidplt, voidtbcs )
    {
    ParagraphLayoutTool *	plt= (ParagraphLayoutTool *)voidplt;
    ParagraphProperties *	pp= &(plt->ptPropertiesChosen);

    int				set;

    set= appGuiGetToggleStateFromCallback( w, voidtbcs );

    pp->ppKeepOnPage= ( set != 0 );

    return;
    }

static APP_TOGGLE_CALLBACK_H( tedParaWidowControlToggled, w, voidplt, voidtbcs )
    {
    ParagraphLayoutTool *	plt= (ParagraphLayoutTool *)voidplt;
    ParagraphProperties *	pp= &(plt->ptPropertiesChosen);

    int				set;

    set= appGuiGetToggleStateFromCallback( w, voidtbcs );

    pp->ppWidowControl= ( set != 0 );

    return;
    }

static APP_TOGGLE_CALLBACK_H( tedParaKeepWithNextToggled, w, voidplt, voidtbcs )
    {
    ParagraphLayoutTool *	plt= (ParagraphLayoutTool *)voidplt;
    ParagraphProperties *	pp= &(plt->ptPropertiesChosen);

    int				set;

    set= appGuiGetToggleStateFromCallback( w, voidtbcs );

    pp->ppKeepWithNext= ( set != 0 );

    return;
    }

/************************************************************************/
/*									*/
/*  Make the paragraph layout page.					*/
/*									*/
/************************************************************************/

void tedFormatFillParagraphLayoutPage(
			ParagraphLayoutTool *			plt,
			const ParagraphLayoutPageResources *	plpr,
			InspectorSubject *			is,
			APP_WIDGET				pageWidget,
			const InspectorSubjectResources *	isr )
    {
    const int		textColumns= 10;

    /**/
    plt->ptPageResources= plpr;

    /**/
    docInitParagraphProperties( &(plt->ptPropertiesSet) );
    docInitParagraphProperties( &(plt->ptPropertiesChosen) );

    /**/
    guiToolMakeLabelAndTextRow( &(plt->pltListLevelRow),
		    (APP_WIDGET *)0, &(plt->pltListLevelText),
		    pageWidget, plpr->plprParaListLevel, textColumns, 0 );

    /**/
    guiToolMakeLabelAndTextRow( &(plt->ptFirstIndentRow),
		    &(plt->ptFirstIndentLabel), &(plt->ptFirstIndentText),
		    pageWidget, plpr->plprParaFirstIndent, textColumns, 1 );

    appGuiSetGotValueCallbackForText( plt->ptFirstIndentText,
				    tedParaFirstIndentChanged, (void *)plt );
    /**/
    guiToolMakeLabelAndTextRow( &(plt->ptLeftIndentRow),
		    &(plt->ptLeftIndentLabel), &(plt->ptLeftIndentText),
		    pageWidget, plpr->plprParaLeftIndent, textColumns, 1 );

    appGuiSetGotValueCallbackForText( plt->ptLeftIndentText,
				    tedParaLeftIndentChanged, (void *)plt );
    /**/
    guiToolMakeLabelAndTextRow( &(plt->ptRightIndentRow),
		    &(plt->ptRightIndentLabel), &(plt->ptRightIndentText),
		    pageWidget, plpr->plprParaRightIndent, textColumns, 1 );

    appGuiSetGotValueCallbackForText( plt->ptRightIndentText,
				    tedParaRightIndentChanged, (void *)plt );
    /**/
    guiToolMakeLabelAndMenuRow( &(plt->pltAlignmentRow),
			&(plt->pltAlignOptionmenu), &(plt->pltAlignmentLabel),
			pageWidget, plpr->pprParaAlignment,
			tedParaAlignmentChosen, (void *)plt );

    /**/
    tedFormatMakeHeightRow( (void *)plt, pageWidget,
						&(plt->ptLineDistChooser),
						tedParaLineDistanceChanged,
						tedParaLineDistChosen );

    /**/
    guiToolMakeToggleAndTextRow( &(plt->pltSpaceAboveRow),
		    &(plt->pltSpaceAboveToggle), &(plt->pltSpaceAboveText),
		    pageWidget, plpr->pprParaSpaceAbove,
		    tedFormatSpaceAboveToggled, (void *)plt,
		    textColumns, 1 );

    appGuiSetGotValueCallbackForText( plt->pltSpaceAboveText,
				    tedParaSpaceAboveChanged, (void *)plt );
    /**/
    guiToolMakeToggleAndTextRow( &(plt->pltSpaceBelowRow),
		    &(plt->pltSpaceBelowToggle), &(plt->pltSpaceBelowText),
		    pageWidget, plpr->pprParaSpaceBelow,
		    tedFormatSpaceBelowToggled, (void *)plt,
		    textColumns, 1 );

    appGuiSetGotValueCallbackForText( plt->pltSpaceBelowText,
				    tedParaSpaceBelowChanged, (void *)plt );

    /**/
    guiToolMake2ToggleRow( &(plt->pltPageRow), pageWidget,
		    &(plt->ptOnNewPageToggle), &(plt->ptKeepOnPageToggle),
		    plpr->pprOnNewPage, plpr->pprOnOnePage,
		    tedParaOnNewPageToggled,
		    tedParaKeepOnPageToggled, (void *)plt, (void *)plt );

    /**/
    guiToolMake2ToggleRow( &(plt->pltKeepRow), pageWidget,
		    &(plt->ptWidowControlToggle), &(plt->ptKeepWithNextToggle),
		    plpr->pprWidctrl, plpr->pprKeepWithNext,
		    tedParaWidowControlToggled,
		    tedParaKeepWithNextToggled, (void *)plt, (void *)plt );

    /**/
    guiToolMake2BottonRow( &(is->isNextPrevRow), pageWidget,
			&(is->isPrevButton), &(is->isNextButton),
			isr->isrPrevButtonText, isr->isrNextButtonText,
			tedFormatPrevPara, tedFormatNextPara, plt );

    {
    APP_WIDGET	row;

    guiToolMake2BottonRow( &row, pageWidget,
			&(is->isSelectButton), &(is->isDeleteButton),
			isr->isrSelectButtonText, isr->isrDeleteButtonText,
			tedFormatSelectPara, tedLayoutDeletePara, plt );

    guiToolMake2BottonRow( &row, pageWidget,
			&(is->isInsertButton), &(is->isAppendButton),
			isr->isrInsertButtonText, isr->isrAppendButtonText,
			tedLayoutInsertPara, tedLayoutAppendPara, plt );
    }

    guiToolMake2BottonRow( &(is->isApplyRow), pageWidget,
			&(is->isRevertButton), &(is->isApplyButton),
			isr->isrRevert, isr->isrApplyToSubject,
			tedFormatParaRevertPushed, tedLayoutChangePara, plt );

    return;
    }

/************************************************************************/
/*									*/
/*  Finish the paragraph page.						*/
/*									*/
/************************************************************************/

void tedParaLayoutToolFillChoosers(
			    ParagraphLayoutTool *			plt,
			    const ParagraphLayoutPageResources *	plpr )
    {
    tedFormatFillHeightChooser( &(plt->ptLineDistChooser),
					    plpr->pprParaLineDistFree,
					    plpr->pprParaLineDistAtLeast,
					    plpr->pprParaLineDistExactly );

    appFillInspectorMenu( DOCtha_COUNT, DOCthaLEFT,
			plt->pltAlignItems, plpr->pprParaAlignMenuTexts,
			&(plt->pltAlignOptionmenu) );

    return;
    }

void tedFinishParaLayoutPage(	ParagraphLayoutTool *			plt,
				const ParagraphLayoutPageResources *	plpr )
    {
    appOptionmenuRefreshWidth( &(plt->ptLineDistChooser.hcOptionmenu) );

    appOptionmenuRefreshWidth( &(plt->pltAlignOptionmenu) );

    return;
    }

/************************************************************************/
/*									*/
/*  Clean the paragraph tool.						*/
/*									*/
/************************************************************************/

void tedFormatCleanParaLayoutTool(	ParagraphLayoutTool *		plt )
    {
    docCleanParagraphProperties( &(plt->ptPropertiesSet) );
    docCleanParagraphProperties( &(plt->ptPropertiesChosen) );
    
    return;
    }

/************************************************************************/
/*									*/
/*  Get paragraph tool resources.					*/
/*									*/
/************************************************************************/

static AppConfigurableResource TED_TedParagraphSubjectResourceTable[]=
    {
    APP_RESOURCE( "formatToolParaLayout",
	offsetof(InspectorSubjectResources,isrSubjectName),
	"Paragraph Formatting" ),
    APP_RESOURCE( "formatToolChangePara",
	offsetof(InspectorSubjectResources,isrApplyToSubject),
	"Apply to Paragraph" ),
    APP_RESOURCE( "tableToolRevert",
	offsetof(InspectorSubjectResources,isrRevert),
	"Revert" ),
    APP_RESOURCE( "formatToolNextPara",
	offsetof(InspectorSubjectResources,isrNextButtonText),
	"Next Paragraph" ),
    APP_RESOURCE( "formatToolPreviousPara",
	offsetof(InspectorSubjectResources,isrPrevButtonText),
	"Previous Paragraph" ),
    APP_RESOURCE( "formatToolSelectPara",
	offsetof(InspectorSubjectResources,isrSelectButtonText),
	"Select Paragraph" ),
    APP_RESOURCE( "formatToolDeletePara",
	offsetof(InspectorSubjectResources,isrDeleteButtonText),
	"Delete Paragraph" ),
    APP_RESOURCE( "formatToolInsertPara",
	offsetof(InspectorSubjectResources,isrInsertButtonText),
	"Insert Paragraph" ),
    APP_RESOURCE( "formatToolAppendPara",
	offsetof(InspectorSubjectResources,isrAppendButtonText),
	"Append Paragraph" ),
    };

static AppConfigurableResource TED_TedParagraphToolResourceTable[]=
    {
    /**/
    APP_RESOURCE( "formatToolParaListLevel",
	offsetof(ParagraphLayoutPageResources,plprParaListLevel),
	"List Level" ),
    APP_RESOURCE( "formatToolParaFirstIndent",
	offsetof(ParagraphLayoutPageResources,plprParaFirstIndent),
	"First Line Indent" ),
    APP_RESOURCE( "formatToolParaLeftIndent",
	offsetof(ParagraphLayoutPageResources,plprParaLeftIndent),
	"Left Indent" ),
    APP_RESOURCE( "formatToolParaRightIndent",
	offsetof(ParagraphLayoutPageResources,plprParaRightIndent),
	"Right Indent" ),

    /**/
    APP_RESOURCE( "formatToolParaAlignment",
	offsetof(ParagraphLayoutPageResources,pprParaAlignment),
	"Alignment" ),
    APP_RESOURCE( "formatToolParaAlignLeft",
	offsetof(ParagraphLayoutPageResources,
				    pprParaAlignMenuTexts[DOCthaLEFT]),
	"Left" ),
    APP_RESOURCE( "formatToolParaAlignRight",
	offsetof(ParagraphLayoutPageResources,
				    pprParaAlignMenuTexts[DOCthaRIGHT]),
	"Right" ),
    APP_RESOURCE( "formatToolParaAlignCentered",
	offsetof(ParagraphLayoutPageResources,
				    pprParaAlignMenuTexts[DOCthaCENTERED]),
	"Centered" ),
    APP_RESOURCE( "formatToolParaAlignJustified",
	offsetof(ParagraphLayoutPageResources,
				    pprParaAlignMenuTexts[DOCthaJUSTIFIED]),
	"Justified" ),

    /**/
    APP_RESOURCE( "formatToolLineDistFree",
	offsetof(ParagraphLayoutPageResources,pprParaLineDistFree),
	"Spacing Free" ),
    APP_RESOURCE( "formatToolLineDistExactly",
	offsetof(ParagraphLayoutPageResources,pprParaLineDistExactly),
	"Spacing Exactly" ),
    APP_RESOURCE( "formatToolLineDistAtLeast",
	offsetof(ParagraphLayoutPageResources,pprParaLineDistAtLeast),
	"Spacing at Least" ),
    /**/
    APP_RESOURCE( "formatToolParaSpaceAbove",
	offsetof(ParagraphLayoutPageResources,pprParaSpaceAbove),
	"Space Above" ),
    APP_RESOURCE( "formatToolParaSpaceBelow",
	offsetof(ParagraphLayoutPageResources,pprParaSpaceBelow),
	"Space Below" ),

    /**/
    APP_RESOURCE( "formatToolParaOnNewPage",
	offsetof(ParagraphLayoutPageResources,pprOnNewPage),
	"Start on New Page" ),
    APP_RESOURCE( "formatToolParaOnOnePage",
	offsetof(ParagraphLayoutPageResources,pprOnOnePage),
	"Keep on One Page" ),
    /**/
    APP_RESOURCE( "formatToolParaWidctrl",
	offsetof(ParagraphLayoutPageResources,pprWidctrl),
	"Widow/Orphan Control" ),
    APP_RESOURCE( "formatToolParaKeepWithNext",
	offsetof(ParagraphLayoutPageResources,pprKeepWithNext),
	"Keep with Next" ),
    };

void tedFormatToolGetParaLayoutResourceTable(
					EditApplication *		ea,
					ParagraphLayoutPageResources *	plpr,
					InspectorSubjectResources *	isr )
    {
    static int	gotToolResources= 0;
    static int	gotSubjectResources= 0;

    if  ( ! gotToolResources )
	{
	appGuiGetResourceValues( &gotToolResources, ea, (void *)plpr,
				TED_TedParagraphToolResourceTable,
				sizeof(TED_TedParagraphToolResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    if  ( ! gotSubjectResources )
	{
	appGuiGetResourceValues( &gotSubjectResources, ea, (void *)isr,
				TED_TedParagraphSubjectResourceTable,
				sizeof(TED_TedParagraphSubjectResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    return;
    }
