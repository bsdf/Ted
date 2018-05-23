/************************************************************************/
/*									*/
/*  The Column page of the format tool.					*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdio.h>
#   include	<stddef.h>

#   include	<appUnit.h>

#   include	"tedAppFront.h"
#   include	"tedColumnTool.h"
#   include	"tedToolUtil.h"
#   include	"tedDocFront.h"
#   include	<guiToolUtil.h>
#   include	<guiTextUtil.h>
#   include	<docTreeNode.h>
#   include	<docNodeTree.h>
#   include	<appFrame.h>
#   include	<docEditCommand.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Refresh the 'Column' page of the format tool.			*/
/*									*/
/************************************************************************/

static void tedFormatToolRefreshColumnPage(	ColumnTool *	ct )
    {
    CellProperties *		cp= &(ct->ctPropertiesChosen);
    const TableRectangle *	tr= &(ct->ctTableRectangle);

    EditDocument *		ed;
    int				traced;
    BufferDocument *		bd= tedFormatCurDoc( &ed, &traced, ct->ctApplication );

    if  ( ! bd )
	{ XDEB(bd); return;	}

    appIntervalToTextWidget( ct->ctColumnText,
				    tr->trCol0+ 1,
				    tr->trCol1+ 1 );

    appLengthToTextWidget( ct->ctWidthText, ct->ctWidthChosen, UNITtyPOINTS );

    guiEnableText( ct->ctWidthText,
			    ct->ctCanChange && tr->trCol0 == tr->trCol1 );

    guiEnableWidget( ct->ctBordersFrame, ct->ctCanChange );
    tedEnableBorderTool( &(ct->ctLeftBorderTool), ct->ctCanChange );
    tedEnableBorderTool( &(ct->ctRightBorderTool), ct->ctCanChange );

    tedBorderToolSetPropertiesByNumber( &(ct->ctLeftBorderTool), bd,
						    cp->cpLeftBorderNumber );
    tedBorderToolSetPropertiesByNumber( &(ct->ctRightBorderTool), bd,
						    cp->cpRightBorderNumber );
    }

void tedRefreshColumnTool(	ColumnTool *			ct,
				int *				pEnabled,
				int *				pPref,
				InspectorSubject *		is,
				const DocumentSelection *	ds,
				const SelectionGeometry *	sg,
				const SelectionDescription *	sd,
				const unsigned char *		cmdEnabled )
    {
    const BufferItem *		rowBi;
    const RowProperties *	rp;

    const DocumentAttributeMap * const dam0= (const DocumentAttributeMap *)0;

    if  ( ! sd->sdInOneTable )
	{
	docInitTableRectangle( &(ct->ctTableRectangle) );
	*pEnabled= 0; return;
	}
    ct->ctTableRectangle= sd->sdTableRectangle;

    rowBi= docGetRowNode( ds->dsHead.dpNode );
    if  ( ! rowBi )
	{ XDEB(rowBi); *pEnabled= 0; return;	}
    rp= &(rowBi->biRowProperties);

    ct->ctBlockFrameWide= sg->sgHead.pgBlockFrameX1-
						sg->sgHead.pgBlockFrameX0;

    docSetCellRectangleProperties( &(ct->ctPropertiesSet), rowBi,
					    &(ct->ctTableRectangle), dam0 );

    if  ( docCopyCellProperties( &(ct->ctPropertiesChosen),
					&(ct->ctPropertiesSet), dam0 ) )
	{ LDEB(1); return;	}

    ct->ctWidthSet= docColumnWidth( rp, ct->ctTableRectangle.trCol0 );
    ct->ctWidthChosen= ct->ctWidthSet;

    ct->ctHalfGapWidthTwips= rp->rpHalfGapWidthTwips;

    guiEnableWidget( is->isPrevButton, ct->ctTableRectangle.trCol0 > 0 );
    guiEnableWidget( is->isNextButton,
		ct->ctTableRectangle.trCol1 < ct->ctTableRectangle.trCol11 );

    ct->ctCanChange= cmdEnabled[EDITcmdUPD_TABLE_PROPS];

    tedFormatToolRefreshColumnPage( ct );

    guiEnableWidget( ct->ctColumnRow, ct->ctCanChange );
    guiEnableWidget( ct->ctWidthRow, ct->ctCanChange );

    guiEnableWidget( is->isDeleteButton, cmdEnabled[EDITcmdDELETE_COLUMN] );

    guiEnableWidget( is->isInsertButton, cmdEnabled[EDITcmdINSERT_COLUMN] );
    guiEnableWidget( is->isAppendButton, cmdEnabled[EDITcmdAPPEND_COLUMN] );

    guiEnableWidget( is->isRevertButton, ct->ctCanChange );
    guiEnableWidget( is->isApplyButton, ct->ctCanChange );

    *pEnabled= 1;
    return;
    }

/************************************************************************/
/*									*/
/*  Retrieve the width of the column(s) from the width widget.		*/
/*									*/
/************************************************************************/

static int tedFormatToolGetColumnWidth(	ColumnTool *		ct )
    {
    CellProperties *		cpChosen= &(ct->ctPropertiesChosen);

    int				width= ct->ctWidthChosen;
    int				changed;

    const int			minValue= 20* 5+ 2* ct->ctHalfGapWidthTwips;
    const int			adaptToMin= 0;
    const int			maxValue= ct->ctBlockFrameWide;
    const int			adaptToMax= 1;

    /*  2  */
    if  ( appGetLengthFromTextWidget( ct->ctWidthText,
				&width, &changed, UNITtyPOINTS,
				minValue, adaptToMin, maxValue, adaptToMax ) )
	{ return -1; }

    if  ( changed )
	{
	int	left= ct->ctPropertiesSet.cpRightBoundaryTwips- ct->ctWidthSet;

	ct->ctWidthChosen= width;

	cpChosen->cpRightBoundaryTwips= left+ ct->ctWidthChosen;
	}

    return 0;
    }

/************************************************************************/

static int tedColumnToolGetChosen(	ColumnTool *		ct,
					BufferDocument *	bd )
    {
    CellProperties *		cpChosen= &(ct->ctPropertiesChosen);
    int				res;

    res= tedFormatToolGetColumnWidth( ct );
    if  ( res != 0 )
	{ return -1;	}

    if  ( tedBorderToolGetNumber( &(cpChosen->cpLeftBorderNumber), (int *)0,
					    &(ct->ctLeftBorderTool), bd ) )
	{ return -1;	}
    if  ( tedBorderToolGetNumber( &(cpChosen->cpRightBorderNumber), (int *)0,
					    &(ct->ctRightBorderTool), bd ) )
	{ return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Set/Revert buttons have been pushed.				*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedTableChangeColumnPushed, w, voidct )
    {
    ColumnTool *		ct= (ColumnTool *)voidct;

    CellProperties *		cpChosen= &(ct->ctPropertiesChosen);

    PropertyMask		cpCmpMask;
    PropertyMask		cpDifMask;

    const int			wholeRow= 0;
    const int			wholeColumn= 1;

    const DocumentAttributeMap * const dam0= (const DocumentAttributeMap *)0;

    /**/
    EditDocument *		ed;
    int				traced;
    BufferDocument *		bd= tedFormatCurDoc( &ed, &traced, ct->ctApplication );

    if  ( ! bd )
	{ XDEB(bd); return;	}

    if  ( tedColumnToolGetChosen( ct, bd ) )
	{ goto ready;	}

    utilPropMaskClear( &cpCmpMask );
    utilPropMaskClear( &cpDifMask );
    utilPropMaskFill( &cpCmpMask, CLprop_COUNT );

    docCellPropertyDifference( &cpDifMask, &(ct->ctPropertiesSet),
						&cpCmpMask, cpChosen, dam0 );

    tedDocSetTableProperties( ed, wholeRow, wholeColumn,
			&cpDifMask, cpChosen,
			(const PropertyMask *)0, (const RowProperties *)0,
			traced );

  ready:

    docCleanBorderProperties( &bp );

    return;
    }

static APP_BUTTON_CALLBACK_H( tedFormatRevertColumnPushed, w, voidct )
    {
    ColumnTool *		ct= (ColumnTool *)voidct;

    const DocumentAttributeMap * const dam0= (const DocumentAttributeMap *)0;

    docCopyCellProperties( &(ct->ctPropertiesChosen), &(ct->ctPropertiesSet),
									dam0 );

    tedFormatToolRefreshColumnPage( ct );

    return;
    }

/************************************************************************/
/*									*/
/*  Check a number as typed by the user.				*/
/*									*/
/************************************************************************/

static APP_TXACTIVATE_CALLBACK_H( tedColumnWidthChanged, w, voidct )
    {
    ColumnTool *		ct= (ColumnTool *)voidct;

    if  ( ! tedFormatToolGetColumnWidth( ct ) )
	{
	appLengthToTextWidget( ct->ctWidthText,
					ct->ctWidthChosen, UNITtyPOINTS );
	}
    }

/************************************************************************/
/*									*/
/*  Previous/Next Column/Row buttons have been pushed.			*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedColumnPreviousColumn, w, voidct )
    {
    ColumnTool *	ct= (ColumnTool *)voidct;
    EditApplication *	ea= ct->ctApplication;

    const int		direction= -1;
    const int		allRows= 1;

    tedAppSelectWholeCell( ea, direction, allRows );

    return;
    }

static APP_BUTTON_CALLBACK_H( tedTableSelectColumn, w, voidct )
    {
    ColumnTool *	ct= (ColumnTool *)voidct;
    EditApplication *	ea= ct->ctApplication;

    const int		direction= 0;
    const int		allRows= 1;

    tedAppSelectWholeCell( ea, direction, allRows );

    return;
    }

static APP_BUTTON_CALLBACK_H( tedColumnNextColumn, w, voidct )
    {
    ColumnTool *	ct= (ColumnTool *)voidct;
    EditApplication *	ea= ct->ctApplication;

    const int		direction= 1;
    const int		allRows= 1;

    tedAppSelectWholeCell( ea, direction, allRows );

    return;
    }

/************************************************************************/
/*									*/
/*  Delete Column/Row/Table buttons have been pushed.			*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedTableDeleteColumn, w, voidct )
    {
    ColumnTool *		ct= (ColumnTool *)voidct;
    EditApplication *		ea= ct->ctApplication;

    tedAppDeleteColumn( ea );

    return;
    }

/************************************************************************/
/*									*/
/*  Insert/Append Column/Row buttons have been pushed.			*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedTableInsertColumn, w, voidct )
    {
    ColumnTool *	ct= (ColumnTool *)voidct;
    EditApplication *	ea= ct->ctApplication;

    const int		after= 0;

    tedAppAddColumnToTable( ea, after );
    }

static APP_BUTTON_CALLBACK_H( tedTableAppendColumn, w, voidct )
    {
    ColumnTool *	ct= (ColumnTool *)voidct;
    EditApplication *	ea= ct->ctApplication;

    const int		after= 1;

    tedAppAddColumnToTable( ea, after );
    }

/************************************************************************/
/*									*/
/*  Callback from the general color chooser: a color was chosen.	*/
/*									*/
/************************************************************************/

static void tedColumnToolGotColor(	void *			voidct,
					int 			which,
					const RGB8Color *	rgb8 )
    {
    ColumnTool *			ct= (ColumnTool *)voidct;

    switch( which )
	{
	case CLpropLEFT_BORDER:
	    tedBorderSetExplicitColorChoice( &(ct->ctLeftBorderTool), rgb8 );
	    break;

	case CLpropRIGHT_BORDER:
	    tedBorderSetExplicitColorChoice( &(ct->ctRightBorderTool), rgb8 );
	    break;

	default:
	    LDEB(which); return;
	}
    }

/************************************************************************/

static void tedColumnToolMakeBorderFrame(
				ColumnTool *			ct,
				const ColumnPageResources *	cpr,
				AppInspector *			ai,
				int				subjectPage,
				APP_WIDGET			pageWidget )
    {
    appMakeColumnFrameInColumn( &(ct->ctBordersFrame), &(ct->ctBordersPaned),
						pageWidget, cpr->cprBorders );

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
/*  Make a column tool, I.E. the 'Column' page of the format tool.	*/
/*									*/
/************************************************************************/

void tedFormatFillColumnPage(	ColumnTool *			ct,
				const ColumnPageResources *	cpr,
				AppInspector *			ai,
				int				subjectPage,
				InspectorSubject *		is,
				APP_WIDGET			pageWidget,
				const InspectorSubjectResources * isr )
    {
    const int	textColumns= 10;

    /**/
    ct->ctPageResources= cpr;

    ct->ctCanChange= 1;

    is->isPrivate= ct;
    is->isGotColor= tedColumnToolGotColor;

    /**/
    guiToolMakeLabelAndTextRow( &(ct->ctColumnRow),
			    &(ct->ctColumnLabel), &(ct->ctColumnText),
			    pageWidget, isr->isrSubjectName, textColumns, 0 );

    guiToolMakeLabelAndTextRow( &(ct->ctWidthRow),
			    &(ct->ctWidthLabel), &(ct->ctWidthText),
			    pageWidget, cpr->cprWidth, textColumns, 1 );

    appGuiSetGotValueCallbackForText( ct->ctWidthText,
					tedColumnWidthChanged, (void *)ct );

    tedColumnToolMakeBorderFrame( ct, cpr, ai, subjectPage, pageWidget );

    guiToolMake2BottonRow( &(is->isNextPrevRow), pageWidget,
		    &(is->isPrevButton), &(is->isNextButton),
		    isr->isrPrevButtonText, isr->isrNextButtonText,
		    tedColumnPreviousColumn, tedColumnNextColumn, ct );

    {
    APP_WIDGET	row;

    guiToolMake2BottonRow( &row, pageWidget,
		    &(is->isSelectButton), &(is->isDeleteButton),
		    isr->isrSelectButtonText, isr->isrDeleteButtonText,
		    tedTableSelectColumn, tedTableDeleteColumn, ct );

    guiToolMake2BottonRow( &row, pageWidget,
		    &(is->isInsertButton), &(is->isAppendButton),
		    isr->isrInsertButtonText, isr->isrAppendButtonText,
		    tedTableInsertColumn, tedTableAppendColumn, ct );
    }

    guiToolMake2BottonRow( &(is->isApplyRow), pageWidget,
		    &(is->isRevertButton), &(is->isApplyButton),
		    isr->isrRevert, isr->isrApplyToSubject,
		    tedFormatRevertColumnPushed, tedTableChangeColumnPushed,
		    (void *)ct );

    return;
    }

void tedColumnToolFillChoosers(	ColumnTool *			ct,
					const ColumnPageResources *	cpr )
    {
    return;
    }

void tedFormatFinishColumnPage(		ColumnTool *			ct,
					const ColumnPageResources *	cpr )
    {
    const PostScriptFontList *	psfl;

    psfl= &(ct->ctApplication->eaPostScriptFontList);

    tedFinishBorderTool( &(ct->ctLeftBorderTool), psfl );
    tedFinishBorderTool( &(ct->ctRightBorderTool), psfl );
    }

/************************************************************************/
/*									*/
/*  Clean a column tool							*/
/*									*/
/************************************************************************/

void tedInitColumnTool(	ColumnTool *	ct )
    {
    ct->ctApplication= (EditApplication *)0;
    ct->ctInspector= (AppInspector *)0;
    ct->ctPageResources= (const ColumnPageResources *)0;

    docInitTableRectangle( &(ct->ctTableRectangle) );

    ct->ctBlockFrameWide= 0;

    docInitCellProperties( &(ct->ctPropertiesSet) );
    docInitCellProperties( &(ct->ctPropertiesChosen) );

    ct->ctWidthSet= 0;
    ct->ctWidthChosen= 0;
    ct->ctHalfGapWidthTwips= 0;

    ct->ctCanChange= 0;

    ct->ctColumnText= (APP_WIDGET)0;
    ct->ctWidthText= (APP_WIDGET)0;

    tedInitBorderTool( &(ct->ctLeftBorderTool) );
    tedInitBorderTool( &(ct->ctRightBorderTool) );

    return;
    }

void tedCleanColumnTool(	ColumnTool *	ct )
    {
    docCleanCellProperties( &(ct->ctPropertiesSet) );
    docCleanCellProperties( &(ct->ctPropertiesChosen) );

    tedCleanBorderTool( &(ct->ctLeftBorderTool) );
    tedCleanBorderTool( &(ct->ctRightBorderTool) );

    return;
    }

/************************************************************************/
/*									*/
/*  Get column tool resources.						*/
/*									*/
/************************************************************************/

static AppConfigurableResource TED_TedColumnSubjectResourceTable[]=
    {
    APP_RESOURCE( "tableToolColumn",
		offsetof(InspectorSubjectResources,isrSubjectName),
		"Column" ),
    APP_RESOURCE( "tableToolChangeColumn",
		offsetof(InspectorSubjectResources,isrApplyToSubject),
		"Apply to Column" ),
    APP_RESOURCE( "tableToolRevert",
		offsetof(InspectorSubjectResources,isrRevert),
		"Revert" ),
    APP_RESOURCE( "tableToolNextColumn",
		offsetof(InspectorSubjectResources,isrNextButtonText),
		"Next Column" ),
    APP_RESOURCE( "tableToolPreviousColumn",
		offsetof(InspectorSubjectResources,isrPrevButtonText),
		"Previous Column" ),
    APP_RESOURCE( "tableToolSelectColumn",
		offsetof(InspectorSubjectResources,isrSelectButtonText),
		"Select Column" ),
    APP_RESOURCE( "tableToolDeleteColumn",
		offsetof(InspectorSubjectResources,isrDeleteButtonText),
		"Delete Column" ),
    APP_RESOURCE( "tableToolInsertColumn",
		offsetof(InspectorSubjectResources,isrInsertButtonText),
		"Insert Column" ),
    APP_RESOURCE( "tableToolAppendColumn",
		offsetof(InspectorSubjectResources,isrAppendButtonText),
		"Append Column" ),
    };

static AppConfigurableResource TED_TedColumnToolResourceTable[]=
    {
    APP_RESOURCE( "tableToolWidth",
		offsetof(ColumnPageResources,cprWidth),
		"Width" ),

    /**/
    APP_RESOURCE( "tableToolColumnBorders",
		offsetof(ColumnPageResources,cprBorders),
		"Borders" ),
    APP_RESOURCE( "tableToolColumnBorderLeft",
		offsetof(ColumnPageResources,cprLeftBorder),
		"Left" ),
    APP_RESOURCE( "tableToolColumnBorderRight",
		offsetof(ColumnPageResources,cprRightBorder),
		"Right" ),

    /**/
    APP_RESOURCE( "formatToolColumnBorderWidth",
	    offsetof(ColumnPageResources, cprBorderToolResources.btrWidth),
	    "Width" ),
    APP_RESOURCE( "formatToolColumnBorderStyle",
	    offsetof(ColumnPageResources, cprBorderToolResources.btrStyle),
	    "Style" ),
    APP_RESOURCE( "formatToolColumnBorderColor",
	    offsetof(ColumnPageResources, cprBorderToolResources.btrColor),
	    "Color" ),
    APP_RESOURCE( "formatToolColumnBorderColorChooserAutomatic",
	    offsetof(ColumnPageResources,cprBorderToolResources.
			    btrColorChooserResources.ccrAutomaticColor),
	    "Automatic" ),
    APP_RESOURCE( "formatToolColumnBorderColorChooserMoreColors",
	    offsetof(ColumnPageResources,cprBorderToolResources.
			    btrColorChooserResources.ccrMoreColors),
	    "More Colors..." ),
    };

void tedFormatToolGetColumnResourceTable(EditApplication *		ea,
					ColumnPageResources *		cpr,
					InspectorSubjectResources *	isr )
    {
    static int	gotToolResources= 0;
    static int	gotSubjectResources= 0;

    if  ( ! gotToolResources )
	{
	appGuiGetResourceValues( &gotToolResources, ea, (void *)cpr,
				TED_TedColumnToolResourceTable,
				sizeof(TED_TedColumnToolResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    if  ( ! gotSubjectResources )
	{
	appGuiGetResourceValues( &gotSubjectResources, ea, (void *)isr,
				TED_TedColumnSubjectResourceTable,
				sizeof(TED_TedColumnSubjectResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    return;
    }
