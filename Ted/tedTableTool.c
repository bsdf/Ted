/************************************************************************/
/*									*/
/*  Table page of the format tool.					*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdio.h>
#   include	<stddef.h>

#   include	"tedTableTool.h"
#   include	"tedAppFront.h"
#   include	"tedToolUtil.h"
#   include	<guiToolUtil.h>
#   include	<guiTextUtil.h>
#   include	<docTreeNode.h>
#   include	<docNodeTree.h>
#   include	<docEditCommand.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Refresh the 'Table' page of the format tool.			*/
/*									*/
/************************************************************************/

static void tedFormatToolRefreshTablePage(	TableTool *	tt )
    {
    RowProperties *	rp= &(tt->ttPropertiesChosen);

    tedRefreshRowMarginsTool( &(tt->ttMarginsTool), rp, tt->ttCanChange );
    }

void tedRefreshTableTool(	TableTool *			tt,
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

    if  ( ! sd->sdInOneTable )
	{ *pEnabled= 0; return;	}

    tt->ttTableRectangle= sd->sdTableRectangle;

    rowNode= docGetRowNode( ds->dsHead.dpNode );
    if  ( ! rowNode )
	{ XDEB(rowNode); *pEnabled= 0; return;	}

    tedRowMarginToolGetMargins( &(tt->ttMarginsTool), rowNode, bd );

    if  ( docCopyRowProperties( &(tt->ttPropertiesSet),
					&(rowNode->biRowProperties), dam0 ) )
	{ LDEB(1); return;	}
    if  ( docCopyRowProperties( &(tt->ttPropertiesChosen),
					&(tt->ttPropertiesSet), dam0 ) )
	{ LDEB(1); return;	}

    tt->ttCanChange= cmdEnabled[EDITcmdUPD_TABLE_PROPS];

    tedFormatToolRefreshTablePage( tt );

    guiEnableWidget( is->isDeleteButton, cmdEnabled[EDITcmdDELETE_TABLE] );

    guiEnableWidget( is->isRevertButton, tt->ttCanChange );
    guiEnableWidget( is->isApplyButton, tt->ttCanChange );

    *pEnabled= 1;
    return;
    }

/************************************************************************/

static int tedTableToolGetChosen(	TableTool *		tt )
    {
    RowProperties *		rpChosen= &(tt->ttPropertiesChosen);

    if  ( tedRowMarginToolGetValues( rpChosen, &(tt->ttMarginsTool) ) )
	{ return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Set/Revert buttons have been pushed.				*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedTableChangeTablePushed, w, voidtt )
    {
    TableTool *			tt= (TableTool *)voidtt;
    RowProperties *		rpChosen= &(tt->ttPropertiesChosen);

    PropertyMask		rpCmpMask;
    PropertyMask		rpDifMask;

    const int			wholeRow= 1;
    const int			wholeColumn= 1;

    const DocumentAttributeMap * const dam0= (const DocumentAttributeMap *)0;

    if  ( tedTableToolGetChosen( tt ) )
	{ return;	}

    utilPropMaskClear( &rpCmpMask );
    utilPropMaskClear( &rpDifMask );
    utilPropMaskFill( &rpCmpMask, RPprop_FULL_COUNT );

    docRowPropertyDifference( &rpDifMask, &(tt->ttPropertiesSet),
						&rpCmpMask, rpChosen, dam0 );

    tedAppSetTableProperties( tt->ttApplication, wholeRow, wholeColumn,
			(const PropertyMask *)0, (const CellProperties *)0,
			&rpDifMask, rpChosen );

    return;
    }

static APP_BUTTON_CALLBACK_H( tedFormatRevertTablePushed, w, voidtt )
    {
    TableTool *	tt= (TableTool *)voidtt;

    docCopyRowProperties( &(tt->ttPropertiesChosen), &(tt->ttPropertiesSet),
				(const DocumentAttributeMap *)0 );

    tedFormatToolRefreshTablePage( tt );

    return;
    }

/************************************************************************/
/*									*/
/*  Check the cell margin that was entered on the 'Table' page of the	*/
/*  format tool.							*/
/*									*/
/************************************************************************/

static APP_TXACTIVATE_CALLBACK_H( tedTableCellMarginChanged, w, voidtt )
    {
    TableTool *		tt= (TableTool *)voidtt;

    tedMarginToolCheckCellMargin( &(tt->ttMarginsTool),
					    &(tt->ttPropertiesChosen) );
    }

/************************************************************************/
/*									*/
/*  Check the left indent that was entered on the 'Table' page of the	*/
/*  format tool.							*/
/*									*/
/************************************************************************/

static APP_TXACTIVATE_CALLBACK_H( tedTableLeftIndentChanged, w, voidtt )
    {
    TableTool *		tt= (TableTool *)voidtt;

    tedMarginToolCheckLeftIndent( &(tt->ttMarginsTool),
						&(tt->ttPropertiesChosen) );
    }

/************************************************************************/
/*									*/
/*  Delete/Select Table buttons have been pushed.			*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedTableDeleteTable, w, voidtt )
    {
    TableTool *			tt= (TableTool *)voidtt;
    EditApplication *		ea= tt->ttApplication;

    tedAppDeleteTable( ea );

    return;
    }

static APP_BUTTON_CALLBACK_H( tedTableSelectTable, w, voidtt )
    {
    TableTool *			tt= (TableTool *)voidtt;
    EditApplication *		ea= tt->ttApplication;

    tedAppSelectTable( ea );
    }

/************************************************************************/
/*									*/
/*  Make the 'Table' page of the format tool.				*/
/*									*/
/************************************************************************/

void tedFormatFillTablePage(	TableTool *			tt,
				const TablePageResources *	tpr,
				InspectorSubject *		is,
				APP_WIDGET			pageWidget,
				const InspectorSubjectResources * isr )
    {
    APP_WIDGET	row= (APP_WIDGET )0;

    /**/
    tt->ttPageResources= tpr;
    tt->ttCanChange= 1;

    /**/
    docInitTableRectangle( &(tt->ttTableRectangle) );

    docInitRowProperties( &(tt->ttPropertiesSet) );
    docInitRowProperties( &(tt->ttPropertiesChosen) );

    /**************/
    tedFormatFillRowMarginsTool( &(tt->ttMarginsTool),
			&(tpr->tprMarginsResources),
			tedTableLeftIndentChanged, tedTableCellMarginChanged,
			(void *)tt, pageWidget );

    /**************/
    guiToolMake2BottonRow( &row, pageWidget,
		&(is->isSelectButton), &(is->isDeleteButton),
		isr->isrSelectButtonText, isr->isrDeleteButtonText,
		tedTableSelectTable, tedTableDeleteTable, (void *)tt );

    guiToolMake2BottonRow( &(is->isApplyRow), pageWidget,
		&(is->isRevertButton), &(is->isApplyButton),
		isr->isrRevert, isr->isrApplyToSubject,
		tedFormatRevertTablePushed, tedTableChangeTablePushed,
		(void *)tt );

    return;
    }

/************************************************************************/
/*									*/
/*  Clean the table tool.						*/
/*									*/
/************************************************************************/

void tedFormatCleanTableTool(	TableTool *	tt )
    {
    docCleanRowProperties( &(tt->ttPropertiesSet) );
    docCleanRowProperties( &(tt->ttPropertiesChosen) );

    return;
    }

/************************************************************************/
/*									*/
/*  Get Table tool resources.						*/
/*									*/
/************************************************************************/

static AppConfigurableResource TED_TedTableSubjectResourceTable[]=
    {
    APP_RESOURCE( "tableToolTable",
		offsetof(InspectorSubjectResources,isrSubjectName),
		"Table" ),
    APP_RESOURCE( "tableToolChangeTable",
		offsetof(InspectorSubjectResources,isrApplyToSubject),
		"Apply to Table" ),
    APP_RESOURCE( "tableToolRevert",
		offsetof(InspectorSubjectResources,isrRevert),
		"Revert" ),
    /*  no Next Table */
    /*  no Prev Table */
    APP_RESOURCE( "tableToolSelectTable",
		offsetof(InspectorSubjectResources,isrSelectButtonText),
		"Select Table" ),
    APP_RESOURCE( "tableToolDeleteTable",
		offsetof(InspectorSubjectResources,isrDeleteButtonText),
		"Delete Table" ),
    /*  no Insert Table */
    /*  no Append Table */
    };

static AppConfigurableResource TED_TedTableToolResourceTable[]=
    {
    /**/
    APP_RESOURCE( "tableToolCellMargin",
		offsetof(TablePageResources,tprMarginsResources.rmtrCellMargin),
		"Cell Margin" ),
    APP_RESOURCE( "tableToolLeftIndent",
		offsetof(TablePageResources,tprMarginsResources.rmtrLeftIndent),
		"Left Margin" ),
    };

void tedFormatToolGetTableResourceTable( EditApplication *		ea,
					TablePageResources *		tpr,
					InspectorSubjectResources *	isr )
    {
    static int	gotToolResources= 0;
    static int	gotSubjectResources= 0;

    if  ( ! gotToolResources )
	{
	appGuiGetResourceValues( &gotToolResources, ea, (void *)tpr,
				TED_TedTableToolResourceTable,
				sizeof(TED_TedTableToolResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    if  ( ! gotSubjectResources )
	{
	appGuiGetResourceValues( &gotSubjectResources, ea, (void *)isr,
				TED_TedTableSubjectResourceTable,
				sizeof(TED_TedTableSubjectResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    return;
    }
