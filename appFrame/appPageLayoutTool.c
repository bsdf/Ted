/************************************************************************/
/*									*/
/*  Page Layout page on an inspector:					*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   include	<stdio.h>
#   include	<stddef.h>
#   include	<limits.h>

#   include	<appUnit.h>

#   include	"appFrame.h"
#   include	"appPageLayoutTool.h"
#   include	"guiWidgetDrawingSurface.h"
#   include	"guiDrawingWidget.h"
#   include	"guiDrawPage.h"
#   include	"guiToolUtil.h"
#   include	"guiTextUtil.h"

#   include	<appDebugon.h>

#   define	DRH_MM		32

/************************************************************************/
/*									*/
/*  Refresh the section page.						*/
/*									*/
/************************************************************************/

static void appPageLayoutRefreshPageLayoutPage(	PageLayoutTool *	plt )
    {
    EditApplication *		ea= plt->pltApplication;
    DocumentGeometry *		dg= &(plt->pltGeometryChosen);

    appPaperChooserAdaptToGeometry( &(plt->pltPaperChooser), dg );

    appMarginToolShowMargins( &(plt->pltMarginTool), plt->pltUnitType, dg );

    guiExposeDrawingWidget( plt->pltPageDrawing );

    if  ( plt->pltHeaderPositionText )
	{
	appLengthToTextWidget( plt->pltHeaderPositionText,
				dg->dgHeaderPositionTwips, ea->eaUnitInt );
	}

    if  ( plt->pltFooterPositionText )
	{
	appLengthToTextWidget( plt->pltFooterPositionText,
				dg->dgFooterPositionTwips, ea->eaUnitInt );
	}

    if  ( plt->pltHeaderFooterFrame )
	{
	guiEnableWidget( plt->pltHeaderFooterFrame,
		    plt->pltCanChangeSelection || plt->pltCanChangeDocument );

	guiEnableText( plt->pltHeaderPositionText,
		    plt->pltCanChangeSelection || plt->pltCanChangeDocument );
	guiEnableText( plt->pltFooterPositionText,
		    plt->pltCanChangeSelection || plt->pltCanChangeDocument );
	}

    appEnablePaperChooser( &(plt->pltPaperChooser),
		    plt->pltCanChangeSelection || plt->pltCanChangeDocument );
    appEnableMarginTool( &(plt->pltMarginTool),
		    plt->pltCanChangeSelection || plt->pltCanChangeDocument );

    if  ( plt->pltChangeSelectionWidget )
	{
	guiEnableWidget( plt->pltChangeSelectionWidget,
						plt->pltCanChangeSelection );
	}
    if  ( plt->pltRevertSelectionWidget )
	{
	guiEnableWidget( plt->pltRevertSelectionWidget,
						plt->pltCanChangeSelection );
	}
    if  ( plt->pltChangeDocumentWidget )
	{
	guiEnableWidget( plt->pltChangeDocumentWidget,
						plt->pltCanChangeDocument );
	}
    if  ( plt->pltRevertDocumentWidget )
	{
	guiEnableWidget( plt->pltRevertDocumentWidget,
						plt->pltCanChangeDocument );
	}

    return;
    }

void appPageLayoutPageRefresh(	PageLayoutTool *		plt,
				InspectorSubject *		is,
				const DocumentGeometry *	dgSect,
				const DocumentGeometry *	dgDoc )
    {
    if  ( plt->pltManageSelection && dgSect )
	{
	plt->pltGeometryChosen= *dgSect;
	plt->pltGeometrySetSelection= *dgSect;
	plt->pltGeometrySetDocument= *dgDoc;
	}
    else{
	plt->pltGeometryChosen= *dgDoc;
	plt->pltGeometrySetSelection= *dgDoc;
	plt->pltGeometrySetDocument= *dgDoc;
	}

    appPageLayoutRefreshPageLayoutPage( plt );

    return;
    }

/************************************************************************/
/*									*/
/*  The 'Change' button of the row tool has been pushed.		*/
/*									*/
/************************************************************************/

static void appPageLayoutChangeLayout(	PageLayoutTool *	plt,
					int			wholeDocument )
    {
    EditApplication *		ea= plt->pltApplication;
    EditDocument *		ed= ea->eaCurrentDocument;

    int				changed;

    PropertyMask		sizeUpdMask;
    PropertyMask		margUpdMask;
    PropertyMask		margChgMask;
    PropertyMask		setMask;

    DocumentGeometry		dgNew;

    const int			minValue= 1;
    const int			adaptToMin= 0;
    const int			maxValue= INT_MAX;
    const int			adaptToMax= 0;

    if  ( ! ed )
	{ XDEB(ed); return;	}

    utilPropMaskClear( &sizeUpdMask );
    utilPropMaskClear( &margUpdMask );

    utilPropMaskClear( &setMask );
    utilPropMaskFill( &setMask, DGprop_COUNT );

    utilInitDocumentGeometry( &dgNew );

    dgNew= plt->pltGeometryChosen;

    if  ( appPaperChooserGetSize( &sizeUpdMask, &(plt->pltPaperChooser),
								    &dgNew ) )
	{ LDEB(1); return;	}

    utilPropMaskClear( &margChgMask );
    utilPropMaskFill( &margChgMask, DGprop_COUNT );

    if  ( appMarginToolGetMargins( &margUpdMask, &margChgMask, plt->pltUnitType,
					     &(plt->pltMarginTool), &dgNew ) )
	{ return;	}

    if  ( appMarginToolCheckMargins( &dgNew ) )
	{ return;	}

    if  ( plt->pltManageHeadersFooters )
	{
	if  ( appGetLengthFromTextWidget( plt->pltHeaderPositionText,
			    &(dgNew.dgHeaderPositionTwips),
			    &changed, ea->eaUnitInt,
			    minValue, adaptToMin, maxValue, adaptToMax ) )
	    { return;	}

	if  ( appGetLengthFromTextWidget( plt->pltFooterPositionText,
			    &(dgNew.dgFooterPositionTwips),
			    &changed, ea->eaUnitInt,
			    minValue, adaptToMin, maxValue, adaptToMax ) )
	    { return;	}
	}
    else{
	PROPmaskUNSET( &setMask, DGpropHEADER_POSITION );
	PROPmaskUNSET( &setMask, DGpropFOOTER_POSITION );
	}

    (*ea->eaSetPageLayout)( ed, &setMask, &dgNew, wholeDocument );

    return;
    }

static APP_BUTTON_CALLBACK_H( appPageLayoutChangeSelectionPushed, w, voidplt )
    {
    PageLayoutTool *	plt= (PageLayoutTool *)voidplt;
    const int		wholeDocument= 0;

    appPageLayoutChangeLayout( plt, wholeDocument );
    }

static APP_BUTTON_CALLBACK_H( appPageLayoutChangeDocumentPushed, w, voidplt )
    {
    PageLayoutTool *	plt= (PageLayoutTool *)voidplt;
    const int		wholeDocument= 1;

    appPageLayoutChangeLayout( plt, wholeDocument );
    }

static void appSectHeaderFooterPosChanged(	PageLayoutTool *	plt,
						APP_WIDGET		w )
    {
    EditApplication *		ea= plt->pltApplication;
    DocumentGeometry *		dgSect= &(plt->pltGeometryChosen);

    int				width;
    int				changed;

    const int			minValue= 1;
    const int			adaptToMin= 0;
    const int			maxValue= INT_MAX;
    const int			adaptToMax= 0;

    if  ( w == plt->pltHeaderPositionText )
	{ width= dgSect->dgHeaderPositionTwips;	}
    if  ( w == plt->pltFooterPositionText )
	{ width= dgSect->dgFooterPositionTwips;	}

    if  ( ! appGetLengthFromTextWidget( w, &width, &changed, ea->eaUnitInt,
				minValue, adaptToMin, maxValue, adaptToMax ) )
	{
	appLengthToTextWidget( w, width, ea->eaUnitInt );
	}
    }

static APP_TXTYPING_CALLBACK_H( appSectHeaderPositionChanged, w, voidplt )
    {
    PageLayoutTool *		plt= (PageLayoutTool *)voidplt;

    appSectHeaderFooterPosChanged( plt, plt->pltHeaderPositionText );

    return;
    }

static APP_TXTYPING_CALLBACK_H( appSectFooterPositionChanged, w, voidplt )
    {
    PageLayoutTool *		plt= (PageLayoutTool *)voidplt;

    appSectHeaderFooterPosChanged( plt, plt->pltFooterPositionText );

    return;
    }

/************************************************************************/
/*									*/
/*  One of the margins has been changed by the user.			*/
/*									*/
/************************************************************************/

static APP_TXACTIVATE_CALLBACK_H( appPaperMarginChanged, w, voidplt )
    {
    PageLayoutTool *		plt= (PageLayoutTool *)voidplt;
    DocumentGeometry *		dgSect= &(plt->pltGeometryChosen);

    PropertyMask		dgUpdMask;
    PropertyMask		dgChgMask;

    DocumentGeometry		dg;

    utilPropMaskClear( &dgUpdMask );
    utilPropMaskClear( &dgChgMask );

    dg= *dgSect;

    utilPropMaskFill( &dgChgMask, DGprop_COUNT );

    if  ( appMarginToolGetMargins( &dgUpdMask, &dgChgMask,
			    plt->pltUnitType, &(plt->pltMarginTool), &dg ) )
	{ return;	}

    if  ( utilPropMaskIsEmpty( &dgUpdMask ) )
	{ return;	}

    if  ( appMarginToolCheckMargins( &dg ) )
	{ return;	}

    dgSect->dgLeftMarginTwips= dg.dgLeftMarginTwips;
    dgSect->dgTopMarginTwips= dg.dgTopMarginTwips;
    dgSect->dgRightMarginTwips= dg.dgRightMarginTwips;
    dgSect->dgBottomMarginTwips= dg.dgBottomMarginTwips;

    guiExposeDrawingWidget( plt->pltPageDrawing );

    return;
    }

/************************************************************************/
/*									*/
/*  The 'Revert' button of the row tool has been pushed.		*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( appPageLayoutRevertSelectionPushed, w, voidplt )
    {
    PageLayoutTool *	plt= (PageLayoutTool *)voidplt;

    plt->pltGeometryChosen= plt->pltGeometrySetSelection;

    appPageLayoutRefreshPageLayoutPage( plt );

    return;
    }

static APP_BUTTON_CALLBACK_H( appPageLayoutRevertDocumentPushed, w, voidplt )
    {
    PageLayoutTool *	plt= (PageLayoutTool *)voidplt;

    plt->pltGeometryChosen= plt->pltGeometrySetDocument;

    appPageLayoutRefreshPageLayoutPage( plt );

    return;
    }

/************************************************************************/
/*									*/
/*  Draw a schematic view of the page.					*/
/*									*/
/************************************************************************/

static APP_EVENT_HANDLER_H( appPageLayoutToolDrawPage, w, voidplt, exposeEvent )
    {
    PageLayoutTool *		plt= (PageLayoutTool *)voidplt;
    DrawingSurface		ds= plt->pltDrawingSurface;
    DocumentGeometry *		dg= &(plt->pltGeometryChosen);

    appDrawPageDiagram( w, ds, &(plt->pltBackgroundColor),
						    plt->pltPixelsPerTwip, dg );
    }

/************************************************************************/
/*									*/
/*  A paper size was chosen.						*/
/*									*/
/************************************************************************/

static void appPageLayoutToolPaperRectChanged(
				PaperChooser *			pc,
				void *				voidplt,
				const DocumentGeometry *	dg )
    {
    PageLayoutTool *		plt= (PageLayoutTool *)voidplt;

    plt->pltGeometryChosen.dgPageWideTwips= dg->dgPageWideTwips;
    plt->pltGeometryChosen.dgPageHighTwips= dg->dgPageHighTwips;

    guiExposeDrawingWidget( plt->pltPageDrawing );

    return;
    }

/************************************************************************/
/*									*/
/*  Make the frame for the header and footer positions.			*/
/*									*/
/************************************************************************/

static void appPageLayoutMakeHeaderFooterWidgets(
				APP_WIDGET			parent,
				const PageLayoutPageResources *	plpr,
				PageLayoutTool *		plt )
    {
    APP_WIDGET	label;
    APP_WIDGET	row;

    const int	colspan= 1;

    appMakeRowFrameInColumn( &(plt->pltHeaderFooterFrame),
					&(plt->pltHeaderFooterInside),
					parent, 2, plpr->plprPositionsText );

    appMakeColumnInRow( &(plt->pltHeaderFooterLeftColumn),
				    plt->pltHeaderFooterInside, 0, colspan );

    appMakeColumnInRow( &(plt->pltHeaderFooterRightColumn),
				    plt->pltHeaderFooterInside, 1, colspan );

    /**********************/
#   define	TW	7

    guiToolMakeLabelAndTextRow( &row, &label, &(plt->pltHeaderPositionText),
				    plt->pltHeaderFooterLeftColumn,
				    plpr->plprHeaderPositionText, TW, 1 );

    guiToolMakeLabelAndTextRow( &row, &label, &(plt->pltFooterPositionText),
				    plt->pltHeaderFooterRightColumn,
				    plpr->plprFooterPositionText, TW, 1 );

    appGuiSetGotValueCallbackForText( plt->pltHeaderPositionText,
				appSectHeaderPositionChanged, (void *)plt );

    appGuiSetGotValueCallbackForText( plt->pltFooterPositionText,
				appSectFooterPositionChanged, (void *)plt );

    return;
    }

/************************************************************************/
/*									*/
/*  Make a page layout page in an inspector tool.			*/
/*									*/
/************************************************************************/

void appPageLayoutPageFillPage(	PageLayoutTool *		plt,
				int				doHdFt,
				int				doSelection,
				const PageLayoutPageResources *	plpr,
				InspectorSubject *		is,
				APP_WIDGET			pageWidget,
				const InspectorSubjectResources * isr )
    {
    EditApplication *	ea= plt->pltApplication;
    APP_WIDGET		row;

    /**/

    plt->pltUnitType= appUnitTypeInt( plpr->plprUnitType );
    if  ( plt->pltUnitType < 0 )
	{ SDEB(plpr->plprUnitType); plt->pltUnitType= UNITtyINCH;	}

    plt->pltHeaderPositionText= (APP_WIDGET)0;
    plt->pltFooterPositionText= (APP_WIDGET)0;
    plt->pltRevertSelectionWidget= (APP_WIDGET)0;
    plt->pltChangeSelectionWidget= (APP_WIDGET)0;

    /**/
    plt->pltPageResources= plpr;
    plt->pltManageHeadersFooters= doHdFt;
    plt->pltManageSelection= doSelection;

    plt->pltCanChangeSelection= 1;
    plt->pltCanChangeDocument= 1;

    plt->pltHeaderFooterFrame= (APP_WIDGET)0;
    plt->pltHeaderFooterLeftColumn= (APP_WIDGET)0;
    plt->pltHeaderFooterRightColumn= (APP_WIDGET)0;
    plt->pltHeaderPositionText= (APP_WIDGET)0;
    plt->pltFooterPositionText= (APP_WIDGET)0;

    /**/

    plt->pltDrawingSurface= (DrawingSurface)0;

    utilInitDocumentGeometry( &(plt->pltGeometrySetDocument) );
    utilInitDocumentGeometry( &(plt->pltGeometrySetSelection) );
    utilInitDocumentGeometry( &(plt->pltGeometryChosen) );

    plt->pltGeometrySetDocument.dgPageWideTwips= 0;
    plt->pltGeometrySetSelection.dgPageWideTwips= 0;
    plt->pltGeometryChosen.dgPageWideTwips= 0;

    appInitPaperChooser( &(plt->pltPaperChooser) );

    /**************/
    plt->pltPageHighMm= DRH_MM;
    plt->pltPixelsPerTwip= ( plt->pltPageHighMm* ea->eaPixelsPerTwip )/
								A3_MM_HIGH;

    plt->pltPageDrawing= appMakePageDrawing( pageWidget,
				ea->eaPixelsPerTwip, plt->pltPageHighMm,
				appPageLayoutToolDrawPage, (void *)plt );

    appMakePaperChooserWidgets( pageWidget, plpr->plprPage,
					    plt->pltUnitType,
					    &(plt->pltPaperChooser),
					    appPageLayoutToolPaperRectChanged,
					    (void *)plt );

    appPaperChooserAddOrientationToggles( &(plt->pltPaperChooser),
					    plpr->plprPortraitText,
					    plpr->plprLandscapeText );

    /**************/

    appMakeMarginToolWidgets( pageWidget, &(plpr->plprMarginToolResources),
					    &(plt->pltMarginTool),
					    appPaperMarginChanged,
					    (void *)plt );


    /**************/

    if  ( plt->pltManageHeadersFooters )
	{
	appPageLayoutMakeHeaderFooterWidgets( pageWidget, plpr, plt );
	}

    if  ( plt->pltManageSelection )
	{
	guiToolMake2BottonRow( &row, pageWidget,
				    &(plt->pltRevertSelectionWidget),
				    &(plt->pltChangeSelectionWidget),
				    plpr->plprRevertSelectionText,
				    plpr->plprChangeSelectionText,
				    appPageLayoutRevertSelectionPushed,
				    appPageLayoutChangeSelectionPushed,
				    (void *)plt );
	}

    guiToolMake2BottonRow( &(is->isApplyRow), pageWidget,
				&(is->isRevertButton), &(is->isApplyButton),
				isr->isrRevert, isr->isrApplyToSubject,
				appPageLayoutRevertDocumentPushed,
				appPageLayoutChangeDocumentPushed,
				(void *)plt );

    plt->pltChangeDocumentWidget= is->isApplyButton;
    plt->pltRevertDocumentWidget= is->isRevertButton;

    return;
    }

/************************************************************************/
/*									*/
/*  Finish the page layout page.					*/
/*									*/
/************************************************************************/

void appPageLayoutPageFillChoosers(	PageLayoutTool *		plt,
					const PageLayoutPageResources *	plpr )
    {
    appPaperChooserFillMenu( &(plt->pltPaperChooser),
					    plpr->plprCustomPageSizeText );

    return;
    }

void appPageLayoutPageFinishPage(	PageLayoutTool *		plt,
					const PageLayoutPageResources *	plpr )
    {
    appPaperChooserRefreshMenuWidth( &(plt->pltPaperChooser) );

    plt->pltDrawingSurface= guiDrawingSurfaceForNativeWidget(
		plt->pltPageDrawing,
		plt->pltApplication->eaPostScriptFontList.psflAvoidFontconfig );

    guiGetBackgroundColor( &(plt->pltBackgroundColor), plt->pltPageDrawing );

    return;
    }

/************************************************************************/
/*									*/
/*  Clean the page layout tool.						*/
/*									*/
/************************************************************************/

void appCleanPageLayoutTool(	PageLayoutTool *	plt )
    {
    appCleanPaperChooser( &(plt->pltPaperChooser) );

    if  ( plt->pltDrawingSurface )
	{ drawFreeDrawingSurface( plt->pltDrawingSurface );	}

    return;
    }

/************************************************************************/
/*									*/
/*  Get page layout tool resources.					*/
/*									*/
/************************************************************************/

static AppConfigurableResource APP_PageSubjectResourceTable[]=
    {
    APP_RESOURCE( "tableToolPageLayout",
		offsetof(InspectorSubjectResources,isrSubjectName),
		"Page Layout" ),
    APP_RESOURCE( "tableToolChangeDocumentPageLayout",
		offsetof(InspectorSubjectResources,isrApplyToSubject),
		"Apply to Document" ),
    APP_RESOURCE( "tableToolRevertDocumentPageLayout",
		offsetof(InspectorSubjectResources,isrRevert),
		"Revert to Document" ),
    };

static AppConfigurableResource APP_PageToolResourceTable[]=
    {
    APP_RESOURCE( "tableToolRevertSection",
		offsetof(PageLayoutPageResources,plprRevertSelectionText),
		"Revert to Selection" ),
    APP_RESOURCE( "tableToolChangeSelection",
		offsetof(PageLayoutPageResources,plprChangeSelectionText),
		"Apply to Selection" ),
    /**/
    APP_RESOURCE( "unit",
		offsetof(PageLayoutPageResources,plprUnitType),
		"inch" ),
    APP_RESOURCE( "formatToolPageLayoutPage",
		offsetof(PageLayoutPageResources,plprPage),
		"Page" ),
    APP_RESOURCE( "pageToolCustom",
		offsetof(PageLayoutPageResources,plprCustomPageSizeText),
		"Custom" ),
    APP_RESOURCE( "pageToolPortrait",
		offsetof(PageLayoutPageResources,plprPortraitText),
		"Portrait" ),
    APP_RESOURCE( "pageToolLandscape",
		offsetof(PageLayoutPageResources,plprLandscapeText),
		"Landscape" ),
    /**/
    APP_RESOURCE( "pageToolMargins",
		offsetof(PageLayoutPageResources,
				plprMarginToolResources.mtrTitleText),
		"Margins" ),
    APP_RESOURCE( "pageToolLeftMargin",
		offsetof(PageLayoutPageResources,
				plprMarginToolResources.mtrLeftMarginText),
		"Left" ),
    APP_RESOURCE( "pageToolTopMargin",
		offsetof(PageLayoutPageResources,
				plprMarginToolResources.mtrTopMarginText),
		"Top" ),
    APP_RESOURCE( "pageToolRightMargin",
		offsetof(PageLayoutPageResources,
				plprMarginToolResources.mtrRightMarginText),
		"Right" ),
    APP_RESOURCE( "pageToolBottomMargin",
		offsetof(PageLayoutPageResources,
				plprMarginToolResources.mtrBottomMarginText),
		"Bottom" ),
    /**/
    APP_RESOURCE( "formatToolSectHeaderFooterPositions",
		offsetof(PageLayoutPageResources,plprPositionsText),
		"Positions" ),
    APP_RESOURCE( "formatToolSectHeaderPosition",
		offsetof(PageLayoutPageResources,plprHeaderPositionText),
		"Header" ),
    APP_RESOURCE( "formatToolSectFooterPosition",
		offsetof(PageLayoutPageResources,plprFooterPositionText),
		"Footer" ),
    };

void appPageLayoutPageGetResourceTable(	EditApplication *		ea,
					PageLayoutPageResources *	plpr,
					InspectorSubjectResources *	isr )
    {
    static int	gotToolResources;
    static int	gotSubjectResources;

    if  ( ! gotToolResources )
	{
	appGuiGetResourceValues( &gotToolResources, ea, (void *)plpr,
				APP_PageToolResourceTable,
				sizeof(APP_PageToolResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    if  ( ! gotSubjectResources )
	{
	appGuiGetResourceValues( &gotSubjectResources, ea, (void *)isr,
				APP_PageSubjectResourceTable,
				sizeof(APP_PageSubjectResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    return;
    }
