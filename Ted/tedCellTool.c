/************************************************************************/
/*									*/
/*  The 'Cell' page of the 'Ted' format tool.				*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdio.h>
#   include	<stddef.h>

#   include	"tedAppFront.h"
#   include	"tedCellTool.h"
#   include	"tedDocFront.h"
#   include	"tedToolUtil.h"
#   include	<guiToolUtil.h>
#   include	<guiTextUtil.h>
#   include	<docTreeNode.h>
#   include	<docNodeTree.h>
#   include	<docEditCommand.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Refresh the 'Cell' page of the format tool.				*/
/*									*/
/************************************************************************/

static void tedFormatToolRefreshCellPage(	CellTool *	ct )
    {
    const TableRectangle *	tr= &(ct->ctTableRectangle);
    const CellProperties *	cp= &(ct->ctPropertiesChosen);

    EditDocument *		ed;
    int				traced;
    BufferDocument *		bd= tedFormatCurDoc( &ed, &traced, ct->ctApplication );

    int				row0= tr->trRow0;
    int				row1= tr->trRow1;
    int				col0= tr->trCol0;
    int				col1= tr->trCol1;

    if  ( ! bd )
	{ XDEB(bd); return;	}

    appSetOptionmenu( &(ct->ctValignMenu), cp->cpValign );

    if  ( tr->trIsSingleCell )
	{
	appGuiSetToggleState( ct->ctMergedToggle,
				    tr->trCellColspan > 1 ||
				    tr->trCellRowspan > 1 );

	col1= col0+ tr->trCellColspan- 1;
	row1= row0+ tr->trCellRowspan- 1;
	}
    else{
	appGuiSetToggleState( ct->ctMergedToggle, 0 );
	}

    guiEnableWidget( ct->ctMergedToggle,
				ct->ctCanChange && ct->ctCanMerge );

    appRectangleToTextWidget( ct->ctRowColumnText,
				    row0- tr->trRow00+ 1,
				    row1- tr->trRow00+ 1,
				    col0+ 1, col1+ 1 );

    guiEnableWidget( ct->ctBordersFrame, ct->ctCanChange );
    tedEnableBorderTool( &(ct->ctTopBorderTool), ct->ctCanChange );
    tedEnableBorderTool( &(ct->ctBottomBorderTool), ct->ctCanChange );
    tedEnableBorderTool( &(ct->ctLeftBorderTool), ct->ctCanChange );
    tedEnableBorderTool( &(ct->ctRightBorderTool), ct->ctCanChange );

    tedBorderToolSetPropertiesByNumber( &(ct->ctTopBorderTool), bd,
						cp->cpTopBorderNumber );
    tedBorderToolSetPropertiesByNumber( &(ct->ctBottomBorderTool), bd,
						cp->cpBottomBorderNumber );
    tedBorderToolSetPropertiesByNumber( &(ct->ctLeftBorderTool), bd,
						cp->cpLeftBorderNumber );
    tedBorderToolSetPropertiesByNumber( &(ct->ctRightBorderTool), bd,
						cp->cpRightBorderNumber );

    tedEnableShadingTool( &(ct->ctShadingTool), ct->ctCanChange );
    tedSetShadingToolByNumber( &(ct->ctShadingTool), bd, cp->cpShadingNumber );
    }

void tedRefreshCellTool(	CellTool *			ct,
				int *				pEnabled,
				int *				pPref,
				InspectorSubject *		is,
				const DocumentSelection *	ds,
				const SelectionDescription *	sd,
				const BufferDocument *		bd,
				const unsigned char *		cmdEnabled )
    {
    TableRectangle *		tr= &(ct->ctTableRectangle);

    int				row0;
    int				row1;
    int				col0;
    int				col1;

    const BufferItem *		rowNode;

    const DocumentAttributeMap * const dam0= (const DocumentAttributeMap *)0;

    if  ( docGetTableRectangle( tr, ds ) )
	{ docInitTableRectangle( tr ); *pEnabled= 0; return; }

    row0= tr->trRow0;
    row1= tr->trRow1;
    col0= tr->trCol0;
    col1= tr->trCol1;

    if  ( tr->trIsSingleCell )
	{
	col1= col0+ tr->trCellColspan- 1;
	row1= row0+ tr->trCellRowspan- 1;
	}

    ct->ctCanMerge= ( col1 > col0 || row1 > row0 ) &&
			    ! ( row1 > row0 && tr->trInTableHeader );
    ct->ctHorMerge= CELLmergeNONE;
    ct->ctVerMerge= CELLmergeNONE;

    if  ( ct->ctCanMerge )
	{
	if  ( col1 > col0 )
	    { ct->ctHorMerge= CELLmergeHEAD;	}
	if  ( row1 > row0 )
	    { ct->ctVerMerge= CELLmergeHEAD;	}
	}

    rowNode= docGetRowNode( ds->dsHead.dpNode );
    if  ( ! rowNode )
	{ XDEB(rowNode); *pEnabled= 0; return;	}

    docSetCellRectangleProperties( &(ct->ctPropertiesSet), rowNode, tr, dam0 );

    if  ( docCopyCellProperties( &(ct->ctPropertiesChosen),
					&(ct->ctPropertiesSet), dam0 ) )
	{ LDEB(1); return;	}

    guiEnableWidget( is->isPrevButton, tr->trCol0 > 0 );
    guiEnableWidget( is->isNextButton, tr->trCol1 < tr->trCol11 );

    guiEnableWidget( ct->ctPrevRowButton, tr->trRow0 > tr->trRow00 );
    guiEnableWidget( ct->ctNextRowButton, tr->trRow1 < tr->trRow11 );

    ct->ctCanChange= cmdEnabled[EDITcmdUPD_TABLE_PROPS];

    tedFormatToolRefreshCellPage( ct );

    guiEnableWidget( ct->ctRowColumnRow, ct->ctCanChange );

    guiEnableWidget( is->isRevertButton, ct->ctCanChange );
    guiEnableWidget( is->isApplyButton, ct->ctCanChange );
    guiEnableWidget( ct->ctValignRow, ct->ctCanChange );

    *pEnabled= 1;
    return;
    }


/************************************************************************/

static int tedCellToolGetChosen(	CellTool *		ct,
					BufferDocument *	bd )
    {
    CellProperties *		cpChosen= &(ct->ctPropertiesChosen);

    if  ( tedBorderToolGetNumber( &(cpChosen->cpTopBorderNumber), (int *)0,
					    &(ct->ctTopBorderTool), bd ) )
	{ return -1;	}
    if  ( tedBorderToolGetNumber( &(cpChosen->cpBottomBorderNumber), (int *)0,
					    &(ct->ctBottomBorderTool), bd ) )
	{ return -1;	}
    if  ( tedBorderToolGetNumber( &(cpChosen->cpLeftBorderNumber), (int *)0,
					    &(ct->ctLeftBorderTool), bd ) )
	{ return -1;	}
    if  ( tedBorderToolGetNumber( &(cpChosen->cpRightBorderNumber), (int *)0,
					    &(ct->ctRightBorderTool), bd ) )
	{ return -1;	}

    if  ( tedFormatToolGetCellShading( cpChosen, bd, &(ct->ctShadingTool) ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Set/Revert buttons have been pushed.				*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedTableChangeCellPushed, w, voidct )
    {
    CellTool *			ct= (CellTool *)voidct;
    CellProperties *		cpChosen= &(ct->ctPropertiesChosen);

    PropertyMask		cpDifMask;
    PropertyMask		cpCmpMask;

    const int			wholeRow= 0;
    const int			wholeColumn= 0;

    const DocumentAttributeMap * const dam0= (const DocumentAttributeMap *)0;

    /**/
    EditDocument *		ed;
    int				traced;
    BufferDocument *		bd= tedFormatCurDoc( &ed, &traced, ct->ctApplication );

    if  ( ! bd )
	{ XDEB(bd); goto ready;	}

    if  ( tedCellToolGetChosen( ct, bd ) )
	{ goto ready;	}

    utilPropMaskClear( &cpCmpMask );
    utilPropMaskFill( &cpCmpMask, CLprop_COUNT );
    utilPropMaskClear( &cpDifMask );

    docCellPropertyDifference( &cpDifMask, &(ct->ctPropertiesSet),
						&cpCmpMask, cpChosen, dam0 );

    tedDocSetTableProperties( ed, wholeRow, wholeColumn,
			&cpDifMask, cpChosen,
			(const PropertyMask *)0, (const RowProperties *)0,
			traced );

  ready:

    return;
    }

static void tedCellValignChosen(	int		valign,
					void *		voidct )
    {
    CellTool *			ct= (CellTool *)voidct;
    CellProperties *		cp= &(ct->ctPropertiesChosen);

    if  ( valign < 0 || valign >= DOCtva_COUNT )
	{ LLDEB(valign,DOCtva_COUNT); return;	}

    cp->cpValign= valign;

    return;
    }


static APP_BUTTON_CALLBACK_H( tedFormatRevertCellPushed, w, voidct )
    {
    CellTool *			ct= (CellTool *)voidct;

    const DocumentAttributeMap * const dam0= (const DocumentAttributeMap *)0;

    docCopyCellProperties( &(ct->ctPropertiesChosen),
						&(ct->ctPropertiesSet), dam0 );

    tedFormatToolRefreshCellPage( ct );

    return;
    }

static APP_TOGGLE_CALLBACK_H( tedCellToggleMerged, w, voidct, voidtbcs )
    {
    CellTool *			ct= (CellTool *)voidct;
    CellProperties *		cp= &(ct->ctPropertiesChosen);
    int				set;

    set= appGuiGetToggleStateFromCallback( w, voidtbcs );

    if  ( set )
	{
	cp->cpHorizontalMerge= ct->ctHorMerge;
	cp->cpVerticalMerge= ct->ctVerMerge;
	}
    else{
	cp->cpHorizontalMerge= CELLmergeNONE;
	cp->cpVerticalMerge= CELLmergeNONE;
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Previous/Next Column/Row buttons have been pushed.			*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedCellPreviousColumn, w, voidct )
    {
    CellTool *		ct= (CellTool *)voidct;
    EditApplication *	ea= ct->ctApplication;

    const int		direction= -1;
    const int		allRows= 0;

    tedAppSelectWholeCell( ea, direction, allRows );
    }

# if 0
static APP_BUTTON_CALLBACK_H( tedTableSelectCell, w, voidct )
    {
    CellTool *		ct= (CellTool *)voidct;
    EditApplication *	ea= ct->ctApplication;
    EditDocument *	ed= ea->eaCurrentDocument;

    const int		direction= 0;
    const int		allRows= 0;

    tedAppSelectWholeCell( ea, direction, allRows );
    }
# endif

static APP_BUTTON_CALLBACK_H( tedCellNextColumn, w, voidct )
    {
    CellTool *		ct= (CellTool *)voidct;
    EditApplication *	ea= ct->ctApplication;

    const int		direction= 1;
    const int		allRows= 0;

    tedAppSelectWholeCell( ea, direction, allRows );
    }

static APP_BUTTON_CALLBACK_H( tedCellPreviousRow, w, voidct )
    {
    CellTool *		ct= (CellTool *)voidct;
    EditApplication *	ea= ct->ctApplication;

    const int		direction= -1;
    const int		allColumns= 0;

    tedAppSelectRow( ea, direction, allColumns );
    }

static APP_BUTTON_CALLBACK_H( tedCellNextRow, w, voidct )
    {
    CellTool *		ct= (CellTool *)voidct;
    EditApplication *	ea= ct->ctApplication;

    const int		direction= 1;
    const int		allColumns= 0;

    tedAppSelectRow( ea, direction, allColumns );
    }

/************************************************************************/
/*									*/
/*  Callback from the general color chooser: a color was chosen.	*/
/*									*/
/************************************************************************/

/* values are arbitrary but cannot be equal to real CLpropSOMETHING values */
# define CLprop_SHADE_BACK_COLOR CLprop_COUNT+ 1
# define CLprop_SHADE_FORE_COLOR CLprop_COUNT+ 2

static void tedCellToolGotColor(	void *			voidct,
					int 			which,
					const RGB8Color *	rgb8 )
    {
    CellTool *			ct= (CellTool *)voidct;

    switch( which )
	{
	case CLpropTOP_BORDER:
	    tedBorderSetExplicitColorChoice( &(ct->ctTopBorderTool), rgb8 );
	    break;

	case CLpropBOTTOM_BORDER:
	    tedBorderSetExplicitColorChoice( &(ct->ctBottomBorderTool), rgb8 );
	    break;

	case CLpropLEFT_BORDER:
	    tedBorderSetExplicitColorChoice( &(ct->ctLeftBorderTool), rgb8 );
	    break;

	case CLpropRIGHT_BORDER:
	    tedBorderSetExplicitColorChoice( &(ct->ctRightBorderTool), rgb8 );
	    break;

	case CLprop_SHADE_BACK_COLOR:
	case CLprop_SHADE_FORE_COLOR:
	    tedShadeSetExplicitColorChoice( &(ct->ctShadingTool),
							    which, rgb8 );
	    break;

	default:
	    LDEB(which); return;
	}
    }

/************************************************************************/

static void tedCellToolMakeBorderFrame(
				CellTool *			ct,
				const CellPageResources *	cpr,
				AppInspector *			ai,
				int				subjectPage,
				APP_WIDGET			pageWidget )
    {
    appMakeColumnFrameInColumn( &(ct->ctBordersFrame), &(ct->ctBordersPaned),
						pageWidget, cpr->cprBorders );

    tedMakeBorderTool( &(ct->ctTopBorderTool), ai, ct->ctBordersPaned,
		cpr->cprTopBorder, &(cpr->cprBorderToolResources),
		subjectPage, CLpropTOP_BORDER );

    tedMakeBorderTool( &(ct->ctBottomBorderTool), ai, ct->ctBordersPaned,
		cpr->cprBottomBorder, &(cpr->cprBorderToolResources),
		subjectPage, CLpropBOTTOM_BORDER );

    tedMakeBorderTool( &(ct->ctLeftBorderTool), ai, ct->ctBordersPaned,
		cpr->cprLeftBorder, &(cpr->cprBorderToolResources),
		subjectPage, CLpropLEFT_BORDER );

    tedMakeBorderTool( &(ct->ctRightBorderTool), ai, ct->ctBordersPaned,
		cpr->cprRightBorder, &(cpr->cprBorderToolResources),
		subjectPage, CLpropRIGHT_BORDER );

    return;
    }

/************************************************************************/
/*									*/
/*  Make a Cell tool, I.E. the 'Cell' page of the format tool.		*/
/*									*/
/************************************************************************/

void tedFormatFillCellPage(	CellTool *			ct,
				const CellPageResources *	cpr,
				AppInspector *			ai,
				int				subjectPage,
				InspectorSubject *		is,
				APP_WIDGET			pageWidget,
				const InspectorSubjectResources * isr )
    {
    APP_WIDGET	label;

    APP_WIDGET	row= (APP_WIDGET)0;

    const int	textColumns= 10;

    /**/
    ct->ctPageResources= cpr;
    ct->ctCanChange= 1;

    is->isPrivate= ct;
    is->isGotColor= tedCellToolGotColor;

    /**/

    docInitTableRectangle( &(ct->ctTableRectangle) );

    docInitCellProperties( &(ct->ctPropertiesSet) );
    docInitCellProperties( &(ct->ctPropertiesChosen) );

    /**/

    guiToolMakeLabelAndTextRow( &(ct->ctRowColumnRow),
			    &(ct->ctRowColumnLabel), &(ct->ctRowColumnText),
			    pageWidget, cpr->cprRowColumn, textColumns, 0 );

    guiToolMakeToggleAndLabelRow( &(ct->ctMergedRow), pageWidget,
			    &(ct->ctMergedToggle), &(ct->ctMergedLabel),
			    cpr->cprMerged, "",
			    tedCellToggleMerged, (void *)ct );
    /**/
    guiToolMakeLabelAndMenuRow( &(ct->ctValignRow), &(ct->ctValignMenu),
				&label, pageWidget, cpr->cprValign,
				tedCellValignChosen, (void *)ct );

    /**/
    tedCellToolMakeBorderFrame( ct, cpr, ai, subjectPage, pageWidget );

    /**/
    tedFormatMakeShadingTool( &(ct->ctShadingTool), ai, pageWidget,
		/* cpr->cprShading, */ (const char *)0,
		&(cpr->cprShadingResources),
		subjectPage, CLprop_SHADE_FORE_COLOR, CLprop_SHADE_BACK_COLOR,
		(TedShadingToolCallback)0, (void *)ct );

    /**/
    guiToolMake2BottonRow( &(is->isNextPrevRow), pageWidget,
		    &(is->isPrevButton), &(is->isNextButton),
		    isr->isrPrevButtonText, isr->isrNextButtonText,
		    tedCellPreviousColumn, tedCellNextColumn, ct );

    /**/
    guiToolMake2BottonRow( &row, pageWidget,
		    &(ct->ctPrevRowButton), &(ct->ctNextRowButton),
		    cpr->cprPrevRow, cpr->cprNextRow,
		    tedCellPreviousRow, tedCellNextRow, ct );

    /* ?
    guiToolMake2BottonRow( &row, pageWidget,
		    &(is->isSelectButton), &(is->isDeleteButton),
		    isr->isrSelectButtonText, isr->isrDeleteButtonText,
		    tedTableSelectColumn, tedTableDeleteColumn, ct );
    */

    guiToolMake2BottonRow( &(is->isApplyRow), pageWidget,
		    &(is->isRevertButton), &(is->isApplyButton),
		    isr->isrRevert, isr->isrApplyToSubject,
		    tedFormatRevertCellPushed, tedTableChangeCellPushed,
		    (void *)ct );

    return;
    }

void tedCellToolFillChoosers(		CellTool *			ct,
					const CellPageResources *	cpr )
    {
    appFillInspectorMenu( DOCtva_COUNT, DOCtvaTOP,
					    ct->ctValignItems,
					    cpr->cprValignItemTexts,
					    &(ct->ctValignMenu) );
    return;
    }

void tedFormatFinishCellPage(		CellTool *			ct,
					const CellPageResources *	cpr )
    {
    const PostScriptFontList *	psfl;

    psfl= &(ct->ctApplication->eaPostScriptFontList);

    appOptionmenuRefreshWidth( &(ct->ctValignMenu) );

    tedFinishBorderTool( &(ct->ctTopBorderTool), psfl );
    tedFinishBorderTool( &(ct->ctBottomBorderTool), psfl );
    tedFinishBorderTool( &(ct->ctLeftBorderTool), psfl );
    tedFinishBorderTool( &(ct->ctRightBorderTool), psfl );

    tedFinishShadingTool( &(ct->ctShadingTool), psfl );
    }

/************************************************************************/
/*									*/
/*  Initialize/Clean a cell Tool.					*/
/*									*/
/************************************************************************/

void tedInitCellTool(	CellTool *	ct )
    {
    ct->ctApplication= (EditApplication *)0;
    ct->ctInspector= (AppInspector *)0;
    ct->ctPageResources= (const CellPageResources *)0;

    docInitTableRectangle( &(ct->ctTableRectangle) );
    ct->ctCanMerge= 0;
    ct->ctHorMerge= CELLmergeNONE;
    ct->ctVerMerge= CELLmergeNONE;
    ct->ctCanChange= 0;

    ct->ctRowColumnRow= (APP_WIDGET)0;
    ct->ctRowColumnLabel= (APP_WIDGET)0;
    ct->ctRowColumnText= (APP_WIDGET)0;

    ct->ctMergedRow= (APP_WIDGET)0;
    ct->ctMergedToggle= (APP_WIDGET)0;
    ct->ctMergedLabel= (APP_WIDGET)0;

    appInitOptionmenu( &(ct->ctValignMenu) );

    docInitCellProperties( &(ct->ctPropertiesSet) );
    docInitCellProperties( &(ct->ctPropertiesChosen) );

    tedInitBorderTool( &(ct->ctTopBorderTool) );
    tedInitBorderTool( &(ct->ctBottomBorderTool) );
    tedInitBorderTool( &(ct->ctLeftBorderTool) );
    tedInitBorderTool( &(ct->ctRightBorderTool) );

    tedInitShadingTool( &(ct->ctShadingTool) );

    return;
    }

void tedCleanCellTool(	CellTool *	ct )
    {
    docCleanCellProperties( &(ct->ctPropertiesSet) );
    docCleanCellProperties( &(ct->ctPropertiesChosen) );

    tedCleanBorderTool( &(ct->ctTopBorderTool) );
    tedCleanBorderTool( &(ct->ctBottomBorderTool) );
    tedCleanBorderTool( &(ct->ctLeftBorderTool) );
    tedCleanBorderTool( &(ct->ctRightBorderTool) );

    tedCleanShadingTool( &(ct->ctShadingTool) );

    return;
    }

/************************************************************************/
/*									*/
/*  Get row tool resources.						*/
/*									*/
/************************************************************************/

static AppConfigurableResource TED_TedCellSubjectResourceTable[]=
    {
    APP_RESOURCE( "tableToolCell",
		offsetof(InspectorSubjectResources,isrSubjectName),
		"Cell" ),
    APP_RESOURCE( "tableToolChangeCell",
		offsetof(InspectorSubjectResources,isrApplyToSubject),
		"Apply to Cell" ),
    APP_RESOURCE( "tableToolRevert",
		offsetof(InspectorSubjectResources,isrRevert),
		"Revert" ),
    APP_RESOURCE( "tableToolCellNextColumn",
		offsetof(InspectorSubjectResources,isrNextButtonText),
		"Next Column" ),
    APP_RESOURCE( "tableToolCellPreviousColumn",
		offsetof(InspectorSubjectResources,isrPrevButtonText),
		"Previous Column" ),
    APP_RESOURCE( "tableToolSelectCell",
		offsetof(InspectorSubjectResources,isrSelectButtonText),
		"Select Cell" ),
    };

static AppConfigurableResource TED_TedCellToolResourceTable[]=
    {
    APP_RESOURCE( "tableToolCellRowColumn",
	    offsetof(CellPageResources,cprRowColumn),
	    "Row, Column" ),

    /**/
    APP_RESOURCE( "tableToolCellBorders",
	    offsetof(CellPageResources,cprBorders),
	    "Borders" ),
    APP_RESOURCE( "tableToolCellBorderTop",
	    offsetof(CellPageResources,cprTopBorder),
	    "Top" ),
    APP_RESOURCE( "tableToolCellBorderBottom",
	    offsetof(CellPageResources,cprBottomBorder),
	    "Bottom" ),
    APP_RESOURCE( "tableToolCellBorderLeft",
	    offsetof(CellPageResources,cprLeftBorder),
	    "Left" ),
    APP_RESOURCE( "tableToolCellBorderRight",
	    offsetof(CellPageResources,cprRightBorder),
	    "Right" ),

    APP_RESOURCE( "tableToolCellMerged",
	    offsetof(CellPageResources,cprMerged),
	    "Merged" ),

    /**/
    APP_RESOURCE( "formatToolCellBorderWidth",
	    offsetof(CellPageResources, cprBorderToolResources.btrWidth),
	    "Width" ),
    APP_RESOURCE( "formatToolCellBorderStyle",
	    offsetof(CellPageResources, cprBorderToolResources.btrStyle),
	    "Style" ),
    APP_RESOURCE( "formatToolCellBorderColor",
	    offsetof(CellPageResources, cprBorderToolResources.btrColor),
	    "Color" ),
    APP_RESOURCE( "formatToolCellBorderColorChooserAutomatic",
	    offsetof(CellPageResources,cprBorderToolResources.
			    btrColorChooserResources.ccrAutomaticColor),
	    "Automatic" ),
    APP_RESOURCE( "formatToolCellBorderColorChooserMoreColors",
	    offsetof(CellPageResources,cprBorderToolResources.
			    btrColorChooserResources.ccrMoreColors),
	    "More Colors..." ),

    /**/
    APP_RESOURCE( "formatToolCellShading",
	    offsetof(CellPageResources,cprShading),
	    "Shading" ),

    APP_RESOURCE( "formatToolCellShadingPattern",
	    offsetof(CellPageResources,cprShadingResources.strPattern),
	    "Pattern" ),
    APP_RESOURCE( "formatToolCellShadingLevel",
	    offsetof(CellPageResources,cprShadingResources.strLevel),
	    "Level" ),
    APP_RESOURCE( "formatToolCellShadingForeColor",
	    offsetof(CellPageResources,cprShadingResources.strForeColor),
	    "Foreground" ),
    APP_RESOURCE( "formatToolCellShadingBackColor",
	    offsetof(CellPageResources,cprShadingResources.strBackColor),
	    "Background" ),

    /**/
    APP_RESOURCE( "formatToolCellShadingBackColorChooserNoBackground",
	    offsetof(CellPageResources,cprShadingResources.
			    strBackColorChooserResources.ccrAutomaticColor),
	    "No Background" ),
    APP_RESOURCE( "formatToolCellShadingBackColorChooserMoreColors",
	    offsetof(CellPageResources,cprShadingResources.
			    strBackColorChooserResources.ccrMoreColors),
	    "More Colors..." ),

    APP_RESOURCE( "formatToolCellShadingForeColorChooserAutomatic",
	    offsetof(CellPageResources,cprShadingResources.
			    strForeColorChooserResources.ccrAutomaticColor),
	    "Automatic" ),
    APP_RESOURCE( "formatToolCellShadingForeColorChooserMoreColors",
	    offsetof(CellPageResources,cprShadingResources.
			    strForeColorChooserResources.ccrMoreColors),
	    "More Colors..." ),

    /**/
    APP_RESOURCE( "tableToolCellVerticalAlignment",
	    offsetof(CellPageResources,cprValign),
	    "Vertical Alignment" ),
    APP_RESOURCE( "tableToolCellValignTop",
		offsetof(CellPageResources,cprValignItemTexts[DOCtvaTOP]),
		"Top" ),
    APP_RESOURCE( "tableToolCellValignCenter",
		offsetof(CellPageResources,cprValignItemTexts[DOCtvaCENTERED]),
		"Center" ),
    APP_RESOURCE( "tableToolCellValignBottom",
		offsetof(CellPageResources,cprValignItemTexts[DOCtvaBOTTOM]),
		"Bottom" ),

    APP_RESOURCE( "cellToolNextRow",
		offsetof(CellPageResources,cprNextRow),
		"Next Row" ),
    APP_RESOURCE( "cellToolPreviousRow",
		offsetof(CellPageResources,cprPrevRow),
		"Previous Row" ),
    };

void tedFormatToolGetCellResourceTable(	EditApplication *		ea,
					CellPageResources *		cpr,
					InspectorSubjectResources *	isr )
    {
    static int	gotToolResources= 0;
    static int	gotSubjectResources= 0;

    if  ( ! gotToolResources )
	{
	appGuiGetResourceValues( &gotToolResources, ea, (void *)cpr,
				TED_TedCellToolResourceTable,
				sizeof(TED_TedCellToolResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    if  ( ! gotSubjectResources )
	{
	appGuiGetResourceValues( &gotSubjectResources, ea, (void *)isr,
				TED_TedCellSubjectResourceTable,
				sizeof(TED_TedCellSubjectResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    return;
    }
