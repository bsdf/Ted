/************************************************************************/
/*									*/
/*  The 'Paragraph' page of the format tool.				*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdio.h>
#   include	<stddef.h>

#   include	"tedParaOrnamentsTool.h"
#   include	"tedAppFront.h"
#   include	"tedDocFront.h"
#   include	"tedToolUtil.h"
#   include	<guiToolUtil.h>
#   include	<docTreeNode.h>
#   include	<docEditCommand.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Refresh the controls on the paragraph properties page.		*/
/*									*/
/************************************************************************/

static void tedFormatToolRefreshParaOrnamentsPage(
					ParagraphOrnamentsTool *	pot )
    {
    ParagraphProperties *	pp= &(pot->potPropertiesChosen);
    EditDocument *		ed;
    int				traced;
    BufferDocument *		bd= tedFormatCurDoc( &ed, &traced, pot->potApplication );

    if  ( ! bd )
	{ XDEB(bd); return;	}

    guiEnableWidget( pot->potBordersFrame, pot->potCanChange );
    tedEnableBorderTool( &(pot->potTopBorderTool), pot->potCanChange );
    tedEnableBorderTool( &(pot->potBottomBorderTool), pot->potCanChange );
    tedEnableBorderTool( &(pot->potLeftBorderTool), pot->potCanChange );
    tedEnableBorderTool( &(pot->potRightBorderTool), pot->potCanChange );

    tedBorderToolSetPropertiesByNumber( &(pot->potTopBorderTool), bd,
						pp->ppTopBorderNumber );
    tedBorderToolSetPropertiesByNumber( &(pot->potBottomBorderTool), bd,
						pp->ppBottomBorderNumber );
    tedBorderToolSetPropertiesByNumber( &(pot->potLeftBorderTool), bd,
						pp->ppLeftBorderNumber );
    tedBorderToolSetPropertiesByNumber( &(pot->potRightBorderTool), bd,
						pp->ppRightBorderNumber );

    tedEnableShadingTool( &(pot->potShadingTool), pot->potCanChange );
    tedSetShadingToolByNumber( &(pot->potShadingTool),
						bd, pp->ppShadingNumber );

    return;
    }


void tedRefreshParaOrnamentsTool(
				ParagraphOrnamentsTool *	pot,
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

    BufferItem *			bi= ds->dsHead.dpNode;

    pp= &(bi->biParaProperties);

    utilPropMaskClear( &ppSetMask );
    utilPropMaskFill( &ppSetMask, PPprop_FULL_COUNT );
    PROPmaskUNSET( &ppSetMask, PPpropTABLE_NESTING );
    PROPmaskUNSET( &ppSetMask, PPpropTAB_STOPS );

    if  ( docUpdParaProperties( (PropertyMask *)0, &(pot->potPropertiesChosen),
					    &ppSetMask, pp,
					    (const DocumentAttributeMap *)0 ) )
	{ LDEB(1); return ;	}

    if  ( docUpdParaProperties( (PropertyMask *)0, &(pot->potPropertiesSet),
					    &ppSetMask, pp,
					    (const DocumentAttributeMap *)0 ) )
	{ LDEB(1); return ;	}

    pot->potCanChange= cmdEnabled[EDITcmdUPD_PARA_PROPS];

    tedRefreshParaSubjectControls( is, ds, sg, sd, cmdEnabled );

    tedFormatToolRefreshParaOrnamentsPage( pot );

    *pEnabled= 1;
    return;
    }

/************************************************************************/
/*									*/
/*  The 'Revert' button on the paragraph tool has been pushed.		*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedFormatParaRevertPushed, w, voidpot )
    {
    ParagraphOrnamentsTool *	pot= (ParagraphOrnamentsTool *)voidpot;

    PropertyMask		ppSetMask;

    utilPropMaskClear( &ppSetMask );
    utilPropMaskFill( &ppSetMask, PPprop_FULL_COUNT );

    docUpdParaProperties( (PropertyMask *)0, &(pot->potPropertiesChosen),
				    &ppSetMask, &(pot->potPropertiesSet),
				    (const DocumentAttributeMap *)0 );

    tedFormatToolRefreshParaOrnamentsPage( pot );

    return;
    }

/************************************************************************/
/*									*/
/*  Previous/Next Paragraph buttons have been pushed.			*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedFormatPrevPara, w, voidpot )
    {
    ParagraphOrnamentsTool *	pot= (ParagraphOrnamentsTool *)voidpot;
    EditApplication *		ea= pot->potApplication;

    tedAppSelectWholeParagraph( ea, -1 );

    return;
    }

static APP_BUTTON_CALLBACK_H( tedFormatNextPara, w, voidpot )
    {
    ParagraphOrnamentsTool *	pot= (ParagraphOrnamentsTool *)voidpot;
    EditApplication *		ea= pot->potApplication;

    tedAppSelectWholeParagraph( ea, 1 );

    return;
    }

static APP_BUTTON_CALLBACK_H( tedFormatSelectPara, w, voidpot )
    {
    ParagraphOrnamentsTool *	pot= (ParagraphOrnamentsTool *)voidpot;
    EditApplication *		ea= pot->potApplication;

    tedAppSelectWholeParagraph( ea, 0 );

    return;
    }

static APP_BUTTON_CALLBACK_H( tedFormatDeletePara, w, voidpot )
    {
    ParagraphOrnamentsTool *	pot= (ParagraphOrnamentsTool *)voidpot;
    EditApplication *		ea= pot->potApplication;

    tedAppDeleteSelectedParagraphs( ea );
    }

static APP_BUTTON_CALLBACK_H( tedFormatInsertPara, w, voidpot )
    {
    ParagraphOrnamentsTool *	pot= (ParagraphOrnamentsTool *)voidpot;
    EditApplication *		ea= pot->potApplication;

    tedAppInsertParagraph( ea, 0 );

    return;
    }

static APP_BUTTON_CALLBACK_H( tedFormatAppendPara, w, voidpot )
    {
    ParagraphOrnamentsTool *	pot= (ParagraphOrnamentsTool *)voidpot;
    EditApplication *		ea= pot->potApplication;

    tedAppInsertParagraph( ea, 1 );

    return;
    }

/************************************************************************/
/*									*/
/*  The 'Change' button on the paragraph borders and shading page has	*/
/*  been pushed.							*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedFormatChangePara, w, voidpot )
    {
    ParagraphOrnamentsTool *	pot= (ParagraphOrnamentsTool *)voidpot;
    ParagraphProperties *	pp= &(pot->potPropertiesChosen);

    PropertyMask		ppSetMask;
    int				changed;

    /**/
    EditDocument *		ed;
    int				traced;
    BufferDocument *		bd= tedFormatCurDoc( &ed, &traced, pot->potApplication );

    if  ( ! bd )
	{ XDEB(bd); return;	}

    /**/
    utilPropMaskClear( &ppSetMask );
    PROPmaskADD( &ppSetMask, PPpropTOP_BORDER );
    PROPmaskADD( &ppSetMask, PPpropBOTTOM_BORDER );
    PROPmaskADD( &ppSetMask, PPpropLEFT_BORDER );
    PROPmaskADD( &ppSetMask, PPpropRIGHT_BORDER );

    PROPmaskADD( &ppSetMask, PPpropSHADING );

    /**/
    if  ( tedBorderToolGetNumber( &(pp->ppTopBorderNumber), &changed,
					    &(pot->potTopBorderTool), bd ) )
	{ LDEB(1); return;	}
    if  ( tedBorderToolGetNumber( &(pp->ppBottomBorderNumber), &changed,
					    &(pot->potBottomBorderTool), bd ) )
	{ LDEB(1); return;	}
    if  ( tedBorderToolGetNumber( &(pp->ppLeftBorderNumber), &changed,
					    &(pot->potLeftBorderTool), bd ) )
	{ LDEB(1); return;	}
    if  ( tedBorderToolGetNumber( &(pp->ppRightBorderNumber), &changed,
					    &(pot->potRightBorderTool), bd ) )
	{ LDEB(1); return;	}

    {
    PropertyMask		isSetMask;

    utilPropMaskClear( &isSetMask );

    if  ( tedShadingToolGetShadingNumber( &(pp->ppShadingNumber), &isSetMask,
						&(pot->potShadingTool), bd ) )
	{ return;	}

    if  ( ! utilPropMaskIsEmpty( &isSetMask ) )
	{ PROPmaskADD( &ppSetMask, PPpropSHADING );	}
    }

    /**/
    if  ( tedDocChangeParagraphProperties( ed, &ppSetMask, pp, traced ) )
	{ LDEB(1);	}

    return;
    }

/************************************************************************/
/*									*/
/*  Callback from the general color chooser: a color was chosen.	*/
/*									*/
/************************************************************************/

/* values are arbitrary but cannot be equal to real PPpropSOMETHING values */
# define PPprop_SHADE_FORE_COLOR PPprop_FULL_COUNT+ 1
# define PPprop_SHADE_BACK_COLOR PPprop_FULL_COUNT+ 2

static void tedParaOrnamentsGotColor(	void *			voidpot,
					int 			which,
					const RGB8Color *	rgb8 )
    {
    ParagraphOrnamentsTool *	pot= (ParagraphOrnamentsTool *)voidpot;

    switch( which )
	{
	case PPpropTOP_BORDER:
	    tedBorderSetExplicitColorChoice(
					&(pot->potTopBorderTool), rgb8 );
	    break;
	case PPpropBOTTOM_BORDER:
	    tedBorderSetExplicitColorChoice(
					&(pot->potBottomBorderTool), rgb8 );
	    break;
	case PPpropLEFT_BORDER:
	    tedBorderSetExplicitColorChoice(
					&(pot->potLeftBorderTool), rgb8 );
	    break;
	case PPpropRIGHT_BORDER:
	    tedBorderSetExplicitColorChoice(
					&(pot->potRightBorderTool), rgb8 );
	    break;

	case PPprop_BOX_BORDER:
	    LDEB(which); return;
	    break;
	case PPpropBETWEEN_BORDER:
	    LDEB(which); return;
	    break;
	case PPpropBAR_BORDER:
	    LDEB(which); return;
	    break;

	case PPprop_SHADE_BACK_COLOR:
	case PPprop_SHADE_FORE_COLOR:
	    tedShadeSetExplicitColorChoice( &(pot->potShadingTool),
							    which, rgb8 );
	    break;

	default:
	    LDEB(which); return;
	}
    }

/************************************************************************/

static void tedParaOrnamentsToolMakeBorderFrame(
			ParagraphOrnamentsTool *		pot,
			const ParagraphOrnamentsPageResources *	popr,
			AppInspector *				ai,
			int					subjectPage,
			APP_WIDGET				pageWidget )
    {
    appMakeColumnFrameInColumn( &(pot->potBordersFrame),
		    &(pot->potBordersPaned), pageWidget, popr->poprBorders );

    tedMakeBorderTool( &(pot->potTopBorderTool), ai, pot->potBordersPaned,
	    popr->poprTopBorder, &(popr->poprBorderToolResources),
	    subjectPage, PPpropTOP_BORDER );
    tedMakeBorderTool( &(pot->potBottomBorderTool), ai, pot->potBordersPaned,
	    popr->poprBottomBorder, &(popr->poprBorderToolResources),
	    subjectPage, PPpropBOTTOM_BORDER );
    tedMakeBorderTool( &(pot->potLeftBorderTool), ai, pot->potBordersPaned,
	    popr->poprLeftBorder, &(popr->poprBorderToolResources),
	    subjectPage, PPpropLEFT_BORDER );
    tedMakeBorderTool( &(pot->potRightBorderTool), ai, pot->potBordersPaned,
	    popr->poprRightBorder, &(popr->poprBorderToolResources),
	    subjectPage, PPpropRIGHT_BORDER );

    return;
    }

/************************************************************************/
/*									*/
/*  Make the paragraph layout page.					*/
/*									*/
/************************************************************************/

void tedFormatFillParagraphOrnamentsPage(
			ParagraphOrnamentsTool *		pot,
			const ParagraphOrnamentsPageResources *	popr,
			AppInspector *				ai,
			int					subjectPage,
			InspectorSubject *			is,
			APP_WIDGET				pageWidget,
			const InspectorSubjectResources *	isr )
    {
    APP_WIDGET		row;

    /**/
    pot->potPageResources= popr;
    is->isPrivate= pot;
    is->isGotColor= tedParaOrnamentsGotColor;

    /**/
    docInitParagraphProperties( &(pot->potPropertiesSet) );
    docInitParagraphProperties( &(pot->potPropertiesChosen) );

    /**/
    tedParaOrnamentsToolMakeBorderFrame( pot, popr,
					    ai, subjectPage, pageWidget );

    /**/
    tedFormatMakeShadingTool( &(pot->potShadingTool), ai, pageWidget,
		/* popr->poprShading, */ (const char *)0,
		&(popr->poprShadingResources),
		subjectPage, PPprop_SHADE_FORE_COLOR, PPprop_SHADE_BACK_COLOR,
		(TedShadingToolCallback)0, (void *)pot );


    /**/
    guiToolMake2BottonRow( &(is->isNextPrevRow), pageWidget,
			&(is->isPrevButton), &(is->isNextButton),
			isr->isrPrevButtonText, isr->isrNextButtonText,
			tedFormatPrevPara, tedFormatNextPara, pot );

    guiToolMake2BottonRow( &row, pageWidget,
			&(is->isSelectButton), &(is->isDeleteButton),
			isr->isrSelectButtonText, isr->isrDeleteButtonText,
			tedFormatSelectPara, tedFormatDeletePara, pot );

    guiToolMake2BottonRow( &row, pageWidget,
			&(is->isInsertButton), &(is->isAppendButton),
			isr->isrInsertButtonText, isr->isrAppendButtonText,
			tedFormatInsertPara, tedFormatAppendPara, pot );

    guiToolMake2BottonRow( &(is->isApplyRow), pageWidget,
			&(is->isRevertButton), &(is->isApplyButton),
			isr->isrRevert, isr->isrApplyToSubject,
			tedFormatParaRevertPushed, tedFormatChangePara, pot );

    return;
    }

/************************************************************************/
/*									*/
/*  Finish the paragraph page.						*/
/*									*/
/************************************************************************/

void tedParaOrnamentsToolFillChoosers(
			    ParagraphOrnamentsTool *			pot,
			    const ParagraphOrnamentsPageResources *	popr )
    {
    return;
    }

void tedFormatFinishParaOrnamentsPage(
			    ParagraphOrnamentsTool *			pot,
			    const ParagraphOrnamentsPageResources *	popr )
    {
    const PostScriptFontList *	psfl;

    psfl= &(pot->potApplication->eaPostScriptFontList);

    tedFinishBorderTool( &(pot->potTopBorderTool), psfl );
    tedFinishBorderTool( &(pot->potBottomBorderTool), psfl );
    tedFinishBorderTool( &(pot->potLeftBorderTool), psfl );
    tedFinishBorderTool( &(pot->potRightBorderTool), psfl );

    tedFinishShadingTool( &(pot->potShadingTool), psfl );

    return;
    }

/************************************************************************/
/*									*/
/*  Init/Clean the paragraph ornaments tool.				*/
/*									*/
/************************************************************************/

void tedInitParaOrnamentsTool(	ParagraphOrnamentsTool *	pot )
    {
    pot->potApplication= (EditApplication *)0;
    pot->potInspector= (AppInspector *)0;
    pot->potPageResources= (const ParagraphOrnamentsPageResources *)0;

    docInitParagraphProperties( &(pot->potPropertiesSet) );
    docInitParagraphProperties( &(pot->potPropertiesChosen) );

    tedInitBorderTool( &(pot->potTopBorderTool) );
    tedInitBorderTool( &(pot->potBottomBorderTool) );
    tedInitBorderTool( &(pot->potLeftBorderTool) );
    tedInitBorderTool( &(pot->potRightBorderTool) );
    
    tedInitShadingTool( &(pot->potShadingTool) );

    return;
    }

void tedCleanParaOrnamentsTool(	ParagraphOrnamentsTool *	pot )
    {
    docCleanParagraphProperties( &(pot->potPropertiesSet) );
    docCleanParagraphProperties( &(pot->potPropertiesChosen) );

    tedCleanBorderTool( &(pot->potTopBorderTool) );
    tedCleanBorderTool( &(pot->potBottomBorderTool) );
    tedCleanBorderTool( &(pot->potLeftBorderTool) );
    tedCleanBorderTool( &(pot->potRightBorderTool) );
    
    tedCleanShadingTool( &(pot->potShadingTool) );

    return;
    }

/************************************************************************/
/*									*/
/*  Get paragraph borders and shading tool resources.			*/
/*									*/
/************************************************************************/

static AppConfigurableResource TED_TedParagraphSubjectResourceTable[]=
    {
    APP_RESOURCE( "formatToolParaOrnaments",
	offsetof(InspectorSubjectResources,isrSubjectName),
	"Paragraph Borders and Shading" ),
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
    APP_RESOURCE( "formatToolParaBorders",
	offsetof(ParagraphOrnamentsPageResources,poprBorders),
	"Borders" ),
    APP_RESOURCE( "formatToolParaBorderTop",
	offsetof(ParagraphOrnamentsPageResources,poprTopBorder),
	"Top" ),
    APP_RESOURCE( "formatToolParaBorderBottom",
	offsetof(ParagraphOrnamentsPageResources,poprBottomBorder),
	"Bottom" ),
    APP_RESOURCE( "formatToolParaBorderLeft",
	offsetof(ParagraphOrnamentsPageResources,poprLeftBorder),
	"Left" ),
    APP_RESOURCE( "formatToolParaBorderRight",
	offsetof(ParagraphOrnamentsPageResources,poprRightBorder),
	"Right" ),

    /**/
    APP_RESOURCE( "formatToolParaBorderWidth",
	offsetof(ParagraphOrnamentsPageResources,
					poprBorderToolResources.btrWidth),
	"Width" ),
    APP_RESOURCE( "formatToolParaBorderStyle",
	offsetof(ParagraphOrnamentsPageResources,
					poprBorderToolResources.btrStyle),
	"Style" ),
    APP_RESOURCE( "formatToolParaBorderColor",
	offsetof(ParagraphOrnamentsPageResources,
					poprBorderToolResources.btrColor),
	"Color" ),
    APP_RESOURCE( "formatToolParaBorderColorChooserAutomatic",
	offsetof(ParagraphOrnamentsPageResources,poprBorderToolResources.
			    btrColorChooserResources.ccrAutomaticColor),
	"Automatic" ),
    APP_RESOURCE( "formatToolParaBorderColorChooserMoreColors",
	offsetof(ParagraphOrnamentsPageResources,poprBorderToolResources.
			    btrColorChooserResources.ccrMoreColors),
	"More Colors..." ),

    /**/
    APP_RESOURCE( "formatToolParaShading",
	offsetof(ParagraphOrnamentsPageResources,poprShading),
	"Shading" ),

    APP_RESOURCE( "formatToolParaShadingPattern",
	offsetof(ParagraphOrnamentsPageResources,
					poprShadingResources.strPattern),
	"Pattern" ),
    APP_RESOURCE( "formatToolParaShadingLevel",
	offsetof(ParagraphOrnamentsPageResources,
					poprShadingResources.strLevel),
	"Level" ),
    APP_RESOURCE( "formatToolParaShadingForeColor",
	offsetof(ParagraphOrnamentsPageResources,
					poprShadingResources.strForeColor),
	"Foreground" ),
    APP_RESOURCE( "formatToolParaShadingBackColor",
	offsetof(ParagraphOrnamentsPageResources,
					poprShadingResources.strBackColor),
	"Background" ),
    /**/
    APP_RESOURCE( "formatToolParaShadingBackColorChooserNoBackground",
	offsetof(ParagraphOrnamentsPageResources,poprShadingResources.
			    strBackColorChooserResources.ccrAutomaticColor),
	"No Background" ),
    APP_RESOURCE( "formatToolParaShadingBackColorChooserMoreColors",
	offsetof(ParagraphOrnamentsPageResources,poprShadingResources.
			    strBackColorChooserResources.ccrMoreColors),
	"More Colors..." ),

    APP_RESOURCE( "formatToolParaShadingForeColorChooserAutomatic",
	offsetof(ParagraphOrnamentsPageResources,poprShadingResources.
			    strForeColorChooserResources.ccrAutomaticColor),
	"Automatic" ),
    APP_RESOURCE( "formatToolParaShadingForeColorChooserMoreColors",
	offsetof(ParagraphOrnamentsPageResources,poprShadingResources.
			    strForeColorChooserResources.ccrMoreColors),
	"More Colors..." ),

    };

void tedFormatToolGetParaOrnamentsResourceTable(
			    EditApplication *			ea,
			    ParagraphOrnamentsPageResources *	popr,
			    InspectorSubjectResources *		isr )
    {
    static int	gotToolResources= 0;
    static int	gotSubjectResources= 0;

    if  ( ! gotToolResources )
	{
	appGuiGetResourceValues( &gotToolResources, ea, (void *)popr,
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
