/************************************************************************/
/*									*/
/*  The Row page of the Format Tool.					*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdio.h>
#   include	<stddef.h>
#   include	<limits.h>

#   include	"tedRowTool.h"
#   include	"tedAppFront.h"
#   include	"tedToolUtil.h"
#   include	"tedDocFront.h"
#   include	<docTreeType.h>
#   include	<guiToolUtil.h>
#   include	<guiTextUtil.h>
#   include	<docTreeNode.h>
#   include	<docNodeTree.h>
#   include	<docEditCommand.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Refresh the row page.						*/
/*									*/
/************************************************************************/

static void tedFormatToolRefreshRowPage( RowTool *	rt )
    {
    const RowProperties *	rp= &(rt->rtRowPropertiesChosen);
    const CellProperties *	cp= &(rt->rtCellPropertiesChosen);
    const TableRectangle *	tr= &(rt->rtTableRectangle);

    EditDocument *		ed;
    int				traced;
    BufferDocument *		bd= tedFormatCurDoc( &ed, &traced, rt->rtApplication );

    if  ( ! bd )
	{ XDEB(bd); return;	}

    appIntervalToTextWidget( rt->rtRowText,
				    tr->trRow0- tr->trRow00+ 1,
				    tr->trRow1- tr->trRow00+ 1 );

    guiEnableWidget( rt->rtRowRow, rt->rtCanChange );
    tedFormatEnableHeightChooser( &(rt->rtHeightChooser),
							rt->rtCanChange );
    tedFormatRefreshHeightChooser( &(rt->rtHeightChooser),
							rp->rpHeightTwips );

    tedRefreshRowMarginsTool( &(rt->rtMarginsTool), rp, rt->rtCanChange );

    guiEnableWidget( rt->rtBordersFrame, rt->rtCanChange );
    tedEnableBorderTool( &(rt->rtTopBorderTool), rt->rtCanChange );
    tedEnableBorderTool( &(rt->rtBottomBorderTool), rt->rtCanChange );

    tedBorderToolSetPropertiesByNumber( &(rt->rtTopBorderTool), bd,
						    cp->cpTopBorderNumber );
    tedBorderToolSetPropertiesByNumber( &(rt->rtBottomBorderTool), bd,
						    cp->cpBottomBorderNumber );

    tedEnableShadingTool( &(rt->rtShadingTool), rt->rtCanChange );
    tedSetShadingToolByNumber( &(rt->rtShadingTool), bd, rp->rpShadingNumber );

    guiEnableWidget( rt->rtKeepOnPageToggle, rt->rtCanChange );
    guiEnableWidget( rt->rtIsTableHeaderToggle, rt->rtCanChange );
    appGuiSetToggleState( rt->rtKeepOnPageToggle, rp->rpKeepOnOnePage );
    appGuiSetToggleState( rt->rtIsTableHeaderToggle, rp->rpIsTableHeader );

    if  ( rt->rtKeepfollowToggle )
	{
	guiEnableWidget( rt->rtKeepfollowToggle, rt->rtCanChange );
	appGuiSetToggleState( rt->rtKeepfollowToggle, rp->rp_Keepfollow );
	}

    /*
    appGuiSetToggleState( rt->rtAutofitToggle, rp->rpAutofit );
    */
    }

void tedRefreshRowTool(		RowTool *			rt,
				int *				pEnabled,
				int *				pPref,
				InspectorSubject *		is,
				const DocumentSelection *	ds,
				const SelectionGeometry *	sg,
				const SelectionDescription *	sd,
				BufferDocument *		bd,
				const unsigned char *		cmdEnabled )
    {
    BufferItem *		rowNode;

    const DocumentAttributeMap * const dam0= (const DocumentAttributeMap *)0;

    rt->rtTableRectangle= sd->sdTableRectangle;
    if  ( ! sd->sdInOneTable )
	{ *pEnabled= 0; return; }

    rowNode= docGetRowNode( ds->dsHead.dpNode );
    if  ( ! rowNode )
	{ XDEB(rowNode); *pEnabled= 0; return;	}

    tedRowMarginToolGetMargins( &(rt->rtMarginsTool), rowNode, bd );

    if  ( docCopyRowProperties( &(rt->rtRowPropertiesSet),
					&(rowNode->biRowProperties), dam0 ) )
	{ LDEB(1); return;	}
    if  ( docCopyRowProperties( &(rt->rtRowPropertiesChosen),
					&(rt->rtRowPropertiesSet), dam0 ) )
	{ LDEB(1); return;	}

    docSetCellRectangleProperties( &(rt->rtCellPropertiesSet),
				    rowNode, &(rt->rtTableRectangle), dam0 );

    if  ( docCopyCellProperties( &(rt->rtCellPropertiesChosen),
					&(rt->rtCellPropertiesSet), dam0 ) )
	{ LDEB(1); return;	}

    guiEnableWidget( is->isMoveUpButton,
				    cmdEnabled[EDITcmdSHIFT_ROWS_UP] );
    guiEnableWidget( is->isMoveDownButton,
				    cmdEnabled[EDITcmdSHIFT_ROWS_DOWN] );

    rt->rtCanChange= cmdEnabled[EDITcmdUPD_TABLE_PROPS];

    {
    const TableRectangle *	tr= &(rt->rtTableRectangle);
    
    guiEnableWidget( is->isPrevButton, tr->trRow0 > tr->trRow00 );
    guiEnableWidget( is->isNextButton, tr->trRow1 < tr->trRow11 );

    guiEnableWidget( rt->rtIsTableHeaderToggle,
				rt->rtCanChange				&&
				rowNode->biTreeType == DOCinBODY	&&
				tr->trRow0 == tr->trRow00		&&
				tr->trRow1 == tr->trRow00		);
    }

    tedFormatToolRefreshRowPage( rt );

    guiEnableWidget( is->isDeleteButton, cmdEnabled[EDITcmdDELETE_ROW] );

    guiEnableWidget( is->isInsertButton, cmdEnabled[EDITcmdINSERT_ROW] );
    guiEnableWidget( is->isAppendButton, cmdEnabled[EDITcmdAPPEND_ROW] );

    guiEnableWidget( is->isRevertButton, rt->rtCanChange );
    guiEnableWidget( is->isApplyButton, rt->rtCanChange );

    *pEnabled= 1;
    return;
    }

/************************************************************************/
/*									*/
/*  The 'Change' button of the row tool has been pushed.		*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedFormatChangeRowPushed, w, voidrt )
    {
    RowTool *			rt= (RowTool *)voidrt;
    RowProperties *		rpChosen= &(rt->rtRowPropertiesChosen);
    CellProperties *		cpChosen= &(rt->rtCellPropertiesChosen);

    PropertyMask		rpSetMask;
    PropertyMask		cpSetMask;
    PropertyMask		rpAllMask;
    PropertyMask		cpAllMask;

    const int			wholeRow= 1;
    const int			wholeColumn= 0;

    /**/
    EditDocument *		ed;
    int				traced;
    BufferDocument *		bd= tedFormatCurDoc( &ed, &traced, rt->rtApplication );

    const DocumentAttributeMap * const dam0= (const DocumentAttributeMap *)0;

    if  ( ! bd )
	{ XDEB(bd); return;	}

    if  ( tedFormatToolGetHeight( &(rpChosen->rpHeightTwips),
						&(rt->rtHeightChooser) ) )
	{ goto ready;	}

    if  ( tedRowMarginToolGetValues( rpChosen, &(rt->rtMarginsTool) ) )
	{ goto ready;	}

    /***/
    if  ( tedBorderToolGetNumber( &(cpChosen->cpTopBorderNumber), (int *)0,
					    &(rt->rtTopBorderTool), bd ) )
	{ goto ready;	}
    if  ( tedBorderToolGetNumber( &(cpChosen->cpBottomBorderNumber), (int *)0,
					    &(rt->rtBottomBorderTool), bd ) )
	{ goto ready;	}

    {
    PropertyMask		isSetMask;

    utilPropMaskClear( &isSetMask );

    if  ( tedShadingToolGetShadingNumber( &(rpChosen->rpShadingNumber),
				    &isSetMask, &(rt->rtShadingTool), bd ) )
	{ return;	}
    }

    /***/
    utilPropMaskFill( &rpAllMask, RPprop_FULL_COUNT );
    utilPropMaskClear( &rpSetMask );

    utilPropMaskFill( &cpAllMask, CLprop_COUNT );
    utilPropMaskClear( &cpSetMask );

    docRowPropertyDifference( &rpSetMask, &(rt->rtRowPropertiesSet),
						&rpAllMask, rpChosen, dam0 );
    docCellPropertyDifference( &cpSetMask, &(rt->rtCellPropertiesSet),
						&cpAllMask, cpChosen, dam0 );

    tedDocSetTableProperties( ed, wholeRow, wholeColumn,
			&cpSetMask, cpChosen, &rpSetMask, rpChosen, traced );

  ready:

    docCleanBorderProperties( &bp );

    return;
    }

/************************************************************************/
/*									*/
/*  The row height changed, check it.					*/
/*									*/
/************************************************************************/

static APP_TXACTIVATE_CALLBACK_H( tedRowHeightChanged, w, voidrt )
    {
    RowTool *		rt= (RowTool *)voidrt;
    int			value;

    if  ( ! tedFormatToolGetHeight( &value, &(rt->rtHeightChooser) ) )
	{
	tedFormatRefreshHeightChooser( &(rt->rtHeightChooser), value );
	}
    }

static APP_TXACTIVATE_CALLBACK_H( tedRowCellMarginChanged, w, voidrt )
    {
    RowTool *		rt= (RowTool *)voidrt;

    tedMarginToolCheckCellMargin( &(rt->rtMarginsTool),
					    &(rt->rtRowPropertiesChosen) );
    }

static APP_TXACTIVATE_CALLBACK_H( tedRowLeftIndentChanged, w, voidrt )
    {
    RowTool *		rt= (RowTool *)voidrt;

    tedMarginToolCheckLeftIndent( &(rt->rtMarginsTool),
						&(rt->rtRowPropertiesChosen) );
    }

/************************************************************************/
/*									*/
/*  The 'Revert' button of the row tool has been pushed.		*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedFormatRevertRowPushed, w, voidrt )
    {
    RowTool *	rt= (RowTool *)voidrt;

    const DocumentAttributeMap * const dam0= (const DocumentAttributeMap *)0;

    docCopyRowProperties( &(rt->rtRowPropertiesChosen),
					&(rt->rtRowPropertiesSet), dam0 );
    docCopyCellProperties( &(rt->rtCellPropertiesChosen),
					&(rt->rtCellPropertiesSet), dam0 );

    tedFormatToolRefreshRowPage( rt );

    return;
    }

static APP_BUTTON_CALLBACK_H( tedFormatPreviousRow, w, voidrt )
    {
    RowTool *		rt= (RowTool *)voidrt;
    EditApplication *	ea= rt->rtApplication;

    const int		direction= -1;
    const int		allColumns= 1;

    tedAppSelectRow( ea, direction, allColumns );

    return;
    }

static APP_BUTTON_CALLBACK_H( tedFormatSelectRow, w, voidrt )
    {
    RowTool *		rt= (RowTool *)voidrt;
    EditApplication *	ea= rt->rtApplication;

    const int		direction= 0;
    const int		allColumns= 1;

    tedAppSelectRow( ea, direction, allColumns );

    return;
    }

static APP_BUTTON_CALLBACK_H( tedFormatNextRow, w, voidrt )
    {
    RowTool *		rt= (RowTool *)voidrt;
    EditApplication *	ea= rt->rtApplication;

    const int		direction= 1;
    const int		allColumns= 1;

    tedAppSelectRow( ea, direction, allColumns );

    return;
    }

/************************************************************************/
/*									*/
/*  The 'Delete' button of the row tool has been pushed.		*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedFormatDeleteRow, w, voidrt )
    {
    RowTool *		rt= (RowTool *)voidrt;
    EditApplication *	ea= rt->rtApplication;

    tedAppDeleteRow( ea );

    return;
    }

/************************************************************************/
/*									*/
/*  Insert/Append Row buttons have been pushed.				*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedFormatInsertRow, w, voidrt )
    {
    RowTool *		rt= (RowTool *)voidrt;
    EditApplication *	ea= rt->rtApplication;

    const int		after= 0;

    tedAppAddRowToTable( ea, after );
    }

static APP_BUTTON_CALLBACK_H( tedFormatAppendRow, w, voidrt )
    {
    RowTool *		rt= (RowTool *)voidrt;
    EditApplication *	ea= rt->rtApplication;

    const int		after= 1;

    tedAppAddRowToTable( ea, after );
    }

static APP_BUTTON_CALLBACK_H( tedRowToolMoveRowDown, w, voidrt )
    {
    RowTool *		rt= (RowTool *)voidrt;
    EditApplication *	ea= rt->rtApplication;

    tedAppShiftRowsInTable( ea, +1 );
    }

static APP_BUTTON_CALLBACK_H( tedRowToolMoveRowUp, w, voidrt )
    {
    RowTool *		rt= (RowTool *)voidrt;
    EditApplication *	ea= rt->rtApplication;

    tedAppShiftRowsInTable( ea, -1 );
    }

static void tedRowHeightChosen(		int		how,
					void *		voidrt )
    {
    RowTool *		rt= (RowTool *)voidrt;
    HeightChooser *	hc= &(rt->rtHeightChooser);

    int			defaultValue;

    defaultValue= rt->rtRowPropertiesChosen.rpHeightTwips;
    if  ( defaultValue == 0 )
	{ defaultValue= rt->rtRowPropertiesSet.rpHeightTwips;	}
    if  ( defaultValue == 0 )
	{ defaultValue= 240;	}

    if  ( how < 0 || how >= HC__COUNT )
	{ LDEB(how); return;	}

    tedFormatHeightChosen( how, hc, defaultValue );

    return;
    }

/************************************************************************/
/*									*/
/*  A toggle was activated.						*/
/*									*/
/************************************************************************/

static APP_TOGGLE_CALLBACK_H( tedRowToggleKeepOnPage, w, voidrt, voidtbcs )
    {
    RowTool *				rt= (RowTool *)voidrt;
    RowProperties *			rp= &(rt->rtRowPropertiesChosen);
    int					set;

    set= appGuiGetToggleStateFromCallback( w, voidtbcs );

    rp->rpKeepOnOnePage= set;

    return;
    }

static APP_TOGGLE_CALLBACK_H( tedRowToggleIsTableHeader, w, voidrt, voidtbcs )
    {
    RowTool *				rt= (RowTool *)voidrt;
    RowProperties *			rp= &(rt->rtRowPropertiesChosen);
    int					set;

    set= appGuiGetToggleStateFromCallback( w, voidtbcs );

    rp->rpIsTableHeader= set;

    return;
    }

static APP_TOGGLE_CALLBACK_H( tedRowToggleKeepWithNext, w, voidrt, voidtbcs )
    {
    RowTool *				rt= (RowTool *)voidrt;
    RowProperties *			rp= &(rt->rtRowPropertiesChosen);
    int					set;

    set= appGuiGetToggleStateFromCallback( w, voidtbcs );

    rp->rp_Keepfollow= set;

    return;
    }

/************************************************************************/
/*									*/
/*  Callback from the general color chooser: a color was chosen.	*/
/*									*/
/************************************************************************/

/* values are arbitrary but cannot be equal to real RPpropSOMETHING values */
# define RPprop_SHADE_FORE_COLOR RPprop_COUNT+ 1
# define RPprop_SHADE_BACK_COLOR RPprop_COUNT+ 2

static void tedRowToolGotColor(		void *			voidrt,
					int 			which,
					const RGB8Color *	rgb8 )
    {
    RowTool *			rt= (RowTool *)voidrt;

    switch( which )
	{
	case CLpropTOP_BORDER:
	    tedBorderSetExplicitColorChoice( &(rt->rtTopBorderTool), rgb8 );
	    break;

	case CLpropBOTTOM_BORDER:
	    tedBorderSetExplicitColorChoice( &(rt->rtBottomBorderTool), rgb8 );
	    break;

	case RPprop_SHADE_BACK_COLOR:
	case RPprop_SHADE_FORE_COLOR:
	    tedShadeSetExplicitColorChoice( &(rt->rtShadingTool),
							    which, rgb8 );
	    break;


	default:
	    LDEB(which); return;
	}
    }

/************************************************************************/

static void tedRowToolMakeBorderFrame(
				RowTool *			rt,
				const RowPageResources *	rpr,
				AppInspector *			ai,
				int				subjectPage,
				APP_WIDGET			pageWidget )
    {
    appMakeColumnFrameInColumn( &(rt->rtBordersFrame), &(rt->rtBordersPaned),
						pageWidget, rpr->rprBorders );

    tedMakeBorderTool( &(rt->rtTopBorderTool), ai, rt->rtBordersPaned,
	    rpr->rprTopBorder, &(rpr->rprBorderToolResources),
	    subjectPage, CLpropTOP_BORDER );

    tedMakeBorderTool( &(rt->rtBottomBorderTool), ai, rt->rtBordersPaned,
	    rpr->rprBottomBorder, &(rpr->rprBorderToolResources),
	    subjectPage, CLpropBOTTOM_BORDER );

    return;
    }

/************************************************************************/
/*									*/
/*  Make a row tool. I.E. the 'Row' page of the format tool.		*/
/*									*/
/************************************************************************/

void tedFormatFillRowPage(  RowTool *				rt,
			    const RowPageResources *		rpr,
			    AppInspector *			ai,
			    int					subjectPage,
			    InspectorSubject *			is,
			    APP_WIDGET				pageWidget,
			    const InspectorSubjectResources *	isr )
    {
    const int	textColumns= 10;

    /**/
    is->isPrivate= rt;
    is->isGotColor= tedRowToolGotColor;

    rt->rtPageResources= rpr;
    rt->rtCanChange= 1;

    /**************/
    guiToolMakeLabelAndTextRow( &(rt->rtRowRow),
			    &(rt->rtRowLabel), &(rt->rtRowText),
			    pageWidget, isr->isrSubjectName, textColumns, 0 );

    /**************/
    tedFormatMakeHeightRow( rt, pageWidget, &(rt->rtHeightChooser),
				    tedRowHeightChanged, tedRowHeightChosen );

    /**************/
    tedFormatFillRowMarginsTool( &(rt->rtMarginsTool),
			    &(rpr->rprMarginsResources),
			    tedRowLeftIndentChanged, tedRowCellMarginChanged,
			    (void *)rt, pageWidget );

    /**************/
    tedRowToolMakeBorderFrame( rt, rpr, ai, subjectPage, pageWidget );

    /**/
    tedFormatMakeShadingTool( &(rt->rtShadingTool), ai, pageWidget,
		/* rpr->rprShading, */ (const char *)0,
		&(rpr->rprShadingResources),
		subjectPage, RPprop_SHADE_FORE_COLOR, RPprop_SHADE_BACK_COLOR,
		(TedShadingToolCallback)0, (void *)rt );

    /**************/
    guiToolMake2ToggleRow( &(rt->rtKeepHeaderRow), pageWidget,
	    &(rt->rtKeepOnPageToggle), &(rt->rtIsTableHeaderToggle),
	    rpr->rprKeepOnPage, rpr->rprIsTableHeader,
	    tedRowToggleKeepOnPage, tedRowToggleIsTableHeader,
	    (void *)rt, (void *)rt );

    guiToolMakeToggleAndLabelRow( &(rt->rtKeepfollowRow), pageWidget,
	    &(rt->rtKeepfollowToggle), &(rt->rtKeepfollowLabel),
	    rpr->rprKeepWithNext, "",
	    tedRowToggleKeepWithNext, (void *)rt );

    /**************/
    guiToolMake2BottonRow( &(is->isNextPrevRow), pageWidget,
	    &(is->isPrevButton), &(is->isNextButton),
	    isr->isrPrevButtonText, isr->isrNextButtonText,
	    tedFormatPreviousRow, tedFormatNextRow, (void *)rt );

    {
    APP_WIDGET	row;

    guiToolMake2BottonRow( &row, pageWidget,
	    &(is->isSelectButton), &(is->isDeleteButton),
	    isr->isrSelectButtonText, isr->isrDeleteButtonText,
	    tedFormatSelectRow, tedFormatDeleteRow, (void *)rt );

    guiToolMake2BottonRow( &row, pageWidget,
	    &(is->isInsertButton), &(is->isAppendButton),
	    isr->isrInsertButtonText, isr->isrAppendButtonText,
	    tedFormatInsertRow, tedFormatAppendRow, (void *)rt );

    guiToolMake2BottonRow( &row, pageWidget,
	    &(is->isMoveUpButton), &(is->isMoveDownButton),
	    isr->isrMoveUpButtonText, isr->isrMoveDownButtonText,
	    tedRowToolMoveRowUp, tedRowToolMoveRowDown, (void *)rt );
    }

    guiToolMake2BottonRow( &(is->isApplyRow), pageWidget,
	    &(is->isRevertButton), &(is->isApplyButton),
	    isr->isrRevert, isr->isrApplyToSubject,
	    tedFormatRevertRowPushed, tedFormatChangeRowPushed, (void *)rt );

    return;
    }

void tedRowToolFillChoosers(		RowTool *			rt,
					const RowPageResources *	rpr )
    {
    tedFormatFillHeightChooser( &(rt->rtHeightChooser),
					    rpr->rprHeightFree,
					    rpr->rprHeightAtLeast,
					    rpr->rprHeightExactly );
    }

void tedFormatFinishRowPage( 	RowTool *			rt,
				const RowPageResources *	rpr )
    {
    const PostScriptFontList *	psfl;

    psfl= &(rt->rtApplication->eaPostScriptFontList);

    appOptionmenuRefreshWidth( &(rt->rtHeightChooser.hcOptionmenu) );

    tedFinishBorderTool( &(rt->rtTopBorderTool), psfl );
    tedFinishBorderTool( &(rt->rtBottomBorderTool), psfl );

    tedFinishShadingTool( &(rt->rtShadingTool), psfl );
    }

/************************************************************************/
/*									*/
/*  Clean a row tool.							*/
/*									*/
/************************************************************************/

void tedInitRowTool(	RowTool *	rt )
    {
    rt->rtApplication= (EditApplication *)0;
    rt->rtInspector= (AppInspector *)0;
    rt->rtPageResources= (const RowPageResources *)0;

    docInitRowProperties( &(rt->rtRowPropertiesSet) );
    docInitRowProperties( &(rt->rtRowPropertiesChosen) );
    docInitCellProperties( &(rt->rtCellPropertiesSet) );
    docInitCellProperties( &(rt->rtCellPropertiesChosen) );

    docInitTableRectangle( &(rt->rtTableRectangle ) );

    rt->rtCanChange= 0;

    rt->rtRowRow= (APP_WIDGET)0;
    rt->rtRowLabel= (APP_WIDGET)0;
    rt->rtRowText= (APP_WIDGET)0;
    tedInitHeightChooser( &(rt->rtHeightChooser) );

    tedInitRowMarginsTool( &(rt->rtMarginsTool) );

    tedInitBorderTool( &(rt->rtTopBorderTool) );
    tedInitBorderTool( &(rt->rtBottomBorderTool) );

    tedInitShadingTool( &(rt->rtShadingTool) );

    rt->rtKeepOnPageToggle= (APP_WIDGET)0;
    rt->rtIsTableHeaderToggle= (APP_WIDGET)0;

    return;
    }

void tedCleanRowTool(	RowTool *	rt )
    {
    docCleanRowProperties( &(rt->rtRowPropertiesSet) );
    docCleanRowProperties( &(rt->rtRowPropertiesChosen) );
    docCleanCellProperties( &(rt->rtCellPropertiesSet) );
    docCleanCellProperties( &(rt->rtCellPropertiesChosen) );

    tedCleanBorderTool( &(rt->rtTopBorderTool) );
    tedCleanBorderTool( &(rt->rtBottomBorderTool) );

    tedCleanShadingTool( &(rt->rtShadingTool) );

    return;
    }

/************************************************************************/
/*									*/
/*  Get row tool resources.						*/
/*									*/
/************************************************************************/

static AppConfigurableResource TED_TedRowSubjectResourceTable[]=
    {
    APP_RESOURCE( "tableToolRow",
		offsetof(InspectorSubjectResources,isrSubjectName),
		"Row" ),
    APP_RESOURCE( "tableToolChangeRow",
		offsetof(InspectorSubjectResources,isrApplyToSubject),
		"Apply to Row" ),
    APP_RESOURCE( "tableToolRevert",
		offsetof(InspectorSubjectResources,isrRevert),
		"Revert" ),
    APP_RESOURCE( "tableToolNextRow",
		offsetof(InspectorSubjectResources,isrNextButtonText),
		"Next Row" ),
    APP_RESOURCE( "tableToolPreviousRow",
		offsetof(InspectorSubjectResources,isrPrevButtonText),
		"Previous Row" ),
    APP_RESOURCE( "tableToolSelectRow",
		offsetof(InspectorSubjectResources,isrSelectButtonText),
		"Select Row" ),
    APP_RESOURCE( "tableToolDeleteRow",
		offsetof(InspectorSubjectResources,isrDeleteButtonText),
		"Delete Row" ),
    APP_RESOURCE( "tableToolInsertRow",
		offsetof(InspectorSubjectResources,isrInsertButtonText),
		"Insert Row" ),
    APP_RESOURCE( "tableToolAppendRow",
		offsetof(InspectorSubjectResources,isrAppendButtonText),
		"Append Row" ),
    APP_RESOURCE( "rowToolMoveDown",
		offsetof(InspectorSubjectResources,isrMoveDownButtonText),
		"Move Down" ),
    APP_RESOURCE( "rowToolMoveUp",
		offsetof(InspectorSubjectResources,isrMoveUpButtonText),
		"Move Up" ),
    };

static AppConfigurableResource TED_TedRowToolResourceTable[]=
    {
    APP_RESOURCE( "tableToolHeightFree",
	    offsetof(RowPageResources,rprHeightFree),
	    "Height Free" ),
    APP_RESOURCE( "tableToolHeightExactly",
	    offsetof(RowPageResources,rprHeightExactly),
	    "Height Exactly" ),
    APP_RESOURCE( "tableToolHeightAtLeast",
	    offsetof(RowPageResources,rprHeightAtLeast),
	    "Height at Least" ),
    /**/
    APP_RESOURCE( "tableToolCellMargin",
	    offsetof(RowPageResources,rprMarginsResources.rmtrCellMargin),
	    "Cell Margin" ),
    APP_RESOURCE( "tableToolLeftIndent",
	    offsetof(RowPageResources,rprMarginsResources.rmtrLeftIndent),
	    "Left Margin" ),
    /**/
    APP_RESOURCE( "tableToolRowBorders",
	    offsetof(RowPageResources,rprBorders),
	    "Borders" ),
    APP_RESOURCE( "tableToolRowBorderTop",
	    offsetof(RowPageResources,rprTopBorder),
	    "Top" ),
    APP_RESOURCE( "tableToolRowBorderBottom",
	    offsetof(RowPageResources,rprBottomBorder),
	    "Bottom" ),

    APP_RESOURCE( "formatToolRowBorderColorChooserAutomatic",
	    offsetof(RowPageResources,rprBorderToolResources.
			    btrColorChooserResources.ccrAutomaticColor),
	    "Automatic" ),
    APP_RESOURCE( "formatToolRowBorderColorChooserMoreColors",
	    offsetof(RowPageResources,rprBorderToolResources.
			    btrColorChooserResources.ccrMoreColors),
	    "More Colors..." ),

    /**/
    APP_RESOURCE( "formatToolRowShading",
	    offsetof(RowPageResources,rprShading),
	    "Shading" ),

    APP_RESOURCE( "formatToolRowShadingPattern",
	    offsetof(RowPageResources,rprShadingResources.strPattern),
	    "Pattern" ),
    APP_RESOURCE( "formatToolRowShadingLevel",
	    offsetof(RowPageResources,rprShadingResources.strLevel),
	    "Level" ),
    APP_RESOURCE( "formatToolRowShadingForeColor",
	    offsetof(RowPageResources,rprShadingResources.strForeColor),
	    "Foreground" ),
    APP_RESOURCE( "formatToolRowShadingBackColor",
	    offsetof(RowPageResources,rprShadingResources.strBackColor),
	    "Background" ),

    /**/
    APP_RESOURCE( "formatToolRowShadingBackColorChooserNoBackground",
	    offsetof(RowPageResources,rprShadingResources.
			    strBackColorChooserResources.ccrAutomaticColor),
	    "No Background" ),
    APP_RESOURCE( "formatToolRowShadingBackColorChooserMoreColors",
	    offsetof(RowPageResources,rprShadingResources.
			    strBackColorChooserResources.ccrMoreColors),
	    "More Colors..." ),

    APP_RESOURCE( "formatToolRowShadingForeColorChooserAutomatic",
	    offsetof(RowPageResources,rprShadingResources.
			    strForeColorChooserResources.ccrAutomaticColor),
	    "Automatic" ),
    APP_RESOURCE( "formatToolRowShadingForeColorChooserMoreColors",
	    offsetof(RowPageResources,rprShadingResources.
			    strForeColorChooserResources.ccrMoreColors),
	    "More Colors..." ),


    /**/
    APP_RESOURCE( "formatToolRowKeepOnPage",
	    offsetof(RowPageResources,rprKeepOnPage),
	    "Keep on One Page" ),
    APP_RESOURCE( "formatToolRowIsTableHeader",
	    offsetof(RowPageResources,rprIsTableHeader),
	    "Is Table Header" ),

    APP_RESOURCE( "formatToolRowKeepWithNext",
	    offsetof(RowPageResources,rprKeepWithNext),
	    "Keep with Next" ),

    /**/
    APP_RESOURCE( "formatToolRowBorderWidth",
	    offsetof(RowPageResources, rprBorderToolResources.btrWidth),
	    "Width" ),
    APP_RESOURCE( "formatToolRowBorderStyle",
	    offsetof(RowPageResources, rprBorderToolResources.btrStyle),
	    "Style" ),
    APP_RESOURCE( "formatToolRowBorderColor",
	    offsetof(RowPageResources, rprBorderToolResources.btrColor),
	    "Color" ),
    };

void tedFormatToolGetRowResourceTable(	EditApplication *		ea,
					RowPageResources *		rpr,
					InspectorSubjectResources *	isr )
    {
    static int	gotToolResources= 0;
    static int	gotSubjectResources= 0;

    if  ( ! gotToolResources )
	{
	appGuiGetResourceValues( &gotToolResources, ea, (void *)rpr,
				TED_TedRowToolResourceTable,
				sizeof(TED_TedRowToolResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    if  ( ! gotSubjectResources )
	{
	appGuiGetResourceValues( &gotSubjectResources, ea, (void *)isr,
				TED_TedRowSubjectResourceTable,
				sizeof(TED_TedRowSubjectResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    return;
    }
