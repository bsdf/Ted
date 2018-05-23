/************************************************************************/
/*									*/
/*  A tool to manage the headers and footers of a Document/Section.	*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdio.h>
#   include	<stddef.h>

#   include	"tedHeaderFooterTool.h"
#   include	"tedAppFront.h"
#   include	<guiToolUtil.h>
#   include	<guiTextUtil.h>
#   include	<docTreeNode.h>
#   include	<docNodeTree.h>
#   include	<docEditCommand.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Set all the controls on the Header/Footer tool, to reflect the	*/
/*  section properties currently chosen.				*/
/*									*/
/************************************************************************/

static void tedFormatToolRefreshHeadFootPage(	HeaderFooterTool *	hft )
    {
    const SectionProperties *	sp= &(hft->hftSectPropertiesChosen);
    const DocumentProperties *	dp= &(hft->hftDocPropertiesChosen);

    int				i;

    appIntegerToTextWidget( hft->hftSectionNumberText,
						hft->hftSectionNumber+ 1 );

    appGuiSetToggleState( hft->hftTitlepgToggle, sp->spHasTitlePage );
    appGuiSetToggleState( hft->hftFacingpToggle, dp->dpHasFacingPages );

    for ( i= 0; i < PAGES__COUNT; i++ )
	{
	guiEnableWidget( hft->hftPagesItems[i],
					hft->hftPagesOptionsEnabled[i] );
	}

    if  ( hft->hftPagesChosen < 0				||
	  hft->hftPagesChosen >= PAGES__COUNT			||
	  ! hft->hftPagesOptionsEnabled[hft->hftPagesChosen]	)
	{
	for ( i= 0; i < PAGES__COUNT; i++ )
	    {
	    if  ( hft->hftPagesOptionsEnabled[i] )
		{
		appSetOptionmenu( &(hft->hftPagesOptionmenu), i );
		hft->hftPagesChosen= i;
		break;
		}
	    }
	}

    if  ( hft->hftPagesChosen >= 0 )
	{
	int headerType= DOC_HeaderScopes[hft->hftPagesChosen];
	int footerType= DOC_FooterScopes[hft->hftPagesChosen];

	guiEnableWidget( hft->hftDeleteHeaderButton,
			    hft->hftCanDeleteHeader &&
			    hft->hftPagesHeaderExists[hft->hftPagesChosen] );
	guiEnableWidget( hft->hftDeleteFooterButton,
			    hft->hftCanDeleteFooter &&
			    hft->hftPagesFooterExists[hft->hftPagesChosen] );

	guiEnableWidget( hft->hftEditHeaderButton,
			hft->hftInTreeType != headerType &&
			(  hft->hftCanInsertHeader ||
			   hft->hftPagesHeaderExists[hft->hftPagesChosen] ) );
	guiEnableWidget( hft->hftEditFooterButton,
			hft->hftInTreeType != footerType &&
			(  hft->hftCanInsertFooter ||
			   hft->hftPagesFooterExists[hft->hftPagesChosen] ) );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Adapt the Header/Footer tool to the current selection in the text.	*/
/*									*/
/************************************************************************/

void tedRefreshHeaderFooterTool(
				HeaderFooterTool *		hft,
				int *				pEnabled,
				int *				pPref,
				InspectorSubject *		is,
				const DocumentSelection *	ds,
				const SelectionDescription *	sd,
				const SelectionGeometry *	sg,
				BufferDocument *		bd,
				const unsigned char *		cmdEnabled )
    {
    const DocumentProperties *	dpSet= &(bd->bdProperties);
    PropertyMask		dpSetMask;

    BufferItem *		selSectBi;
    const BufferItem *		bodySectNode;

    const SectionProperties *	spSet;
    PropertyMask		spSetMask;

    int				whatHeaderFooter;
    int				i;

    const DocumentAttributeMap * const	dam0= (const DocumentAttributeMap *)0;

    selSectBi= docGetSectNode( ds->dsHead.dpNode );
    if  ( ! selSectBi )
	{ XDEB(selSectBi); return;	}
    bodySectNode= docGetBodySectNode( selSectBi, bd );
    if  ( ! bodySectNode )
	{ XDEB(bodySectNode); *pEnabled= 0; return;	}

    whatHeaderFooter= hft->hftInTreeType= selSectBi->biTreeType;
    if  ( sd->sdInHeaderFooter )
	{
	whatHeaderFooter= selSectBi->biTreeType;
	hft->hftSectionNumber= bodySectNode->biNumberInParent;
	spSet= &(bodySectNode->biSectProperties);

	(*pPref) += 2;
	}
    else{
	if  ( ! sd->sdInDocumentBody )
	    { *pEnabled= 0; return;	}

	whatHeaderFooter= sd->sdHeaderTypeForSelection;
	hft->hftSectionNumber= selSectBi->biNumberInParent;
	spSet= &(selSectBi->biSectProperties);
	}

    /**********/

    utilPropMaskClear( &dpSetMask );
    PROPmaskADD( &dpSetMask, DPpropFACING_PAGES );

    if  ( docUpdDocumentProperties( (PropertyMask *)0,
		    &(hft->hftDocPropertiesChosen), &dpSetMask, dpSet, dam0 ) )
	{ LDEB(1); return;	}
    if  ( docUpdDocumentProperties( (PropertyMask *)0,
		    &(hft->hftDocPropertiesSet), &dpSetMask, dpSet, dam0 ) )
	{ LDEB(1); return;	}

    utilPropMaskClear( &spSetMask );
    PROPmaskADD( &spSetMask, SPpropTITLEPG );

    if  ( docUpdSectProperties( (PropertyMask *)0,
		    &(hft->hftSectPropertiesChosen), &spSetMask, spSet ) )
	{ LDEB(1); return;	}
    if  ( docUpdSectProperties( (PropertyMask *)0,
		    &(hft->hftSectPropertiesSet), &spSetMask, spSet ) )
	{ LDEB(1); return;	}

    /**********/

    for ( i= 0; i < PAGES__COUNT; i++ )
	{
	hft->hftPagesHeaderExists[i]= 0;
	hft->hftPagesFooterExists[i]= 0;

	if  ( docSectionHasHeaderFooter( bodySectNode,
					&(hft->hftPagesHeaderApplies[i]),
					&(hft->hftDocPropertiesSet),
					DOC_HeaderScopes[i] ) )
	    { hft->hftPagesHeaderExists[i]= 1;	}
	if  ( docSectionHasHeaderFooter( bodySectNode,
					&(hft->hftPagesFooterApplies[i]),
					&(hft->hftDocPropertiesSet),
					DOC_FooterScopes[i] ) )
	    { hft->hftPagesFooterExists[i]= 1;	}
	}

    if  ( sd->sdInHeaderFooter )
	{
	for ( i= 0; i < PAGES__COUNT; i++ )
	    { hft->hftPagesOptionsEnabled[i]= 0; }

	guiEnableWidget( hft->hftSectionFrame, 0 );
	guiEnableWidget( hft->hftDocumentFrame, 0 );
	}
    else{
	hft->hftPagesOptionsEnabled[PAGES_FIRST_PAGE]=
					    spSet->spHasTitlePage;
	hft->hftPagesOptionsEnabled[PAGES_SUBSEQUENT_PAGES]=
					    spSet->spHasTitlePage	&&
					    ! dpSet->dpHasFacingPages;
	hft->hftPagesOptionsEnabled[PAGES_ALL_PAGES]=
					    ! spSet->spHasTitlePage	&&
					    ! dpSet->dpHasFacingPages;
	hft->hftPagesOptionsEnabled[PAGES_ODD_PAGES]=
					    dpSet->dpHasFacingPages;
	hft->hftPagesOptionsEnabled[PAGES_EVEN_PAGES]=
					    dpSet->dpHasFacingPages;

	guiEnableWidget( hft->hftSectionFrame,
					cmdEnabled[EDITcmdUPD_SECT_PROPS] );
	guiEnableWidget( hft->hftDocumentFrame,
					cmdEnabled[EDITcmdUPD_DOC_PROPS] );
	}

    if  ( sd->sdInHeaderFooter )
	{
	int	pages;

	pages= docWhatPagesForHeaderFooter( dpSet, spSet, whatHeaderFooter );
	if  ( pages < 0 )
	    { LLDEB(whatHeaderFooter,pages); *pEnabled= 0; return;	}
	hft->hftPagesChosen= pages;
	hft->hftPagesOptionsEnabled[hft->hftPagesChosen]= 1;

	appSetOptionmenu( &(hft->hftPagesOptionmenu), hft->hftPagesChosen );
	}

    hft->hftCanInsertHeader= cmdEnabled[EDITcmdINSERT_HEADER];
    hft->hftCanDeleteHeader= cmdEnabled[EDITcmdDELETE_HEADER];
    hft->hftCanInsertFooter= cmdEnabled[EDITcmdINSERT_FOOTER];
    hft->hftCanDeleteFooter= cmdEnabled[EDITcmdDELETE_FOOTER];

    tedFormatToolRefreshHeadFootPage( hft );

    *pEnabled= 1;
    return;
    }

/************************************************************************/
/*									*/
/*  Set/Revert buttons have been pushed.				*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedHeadFootChangeDocPushed, w, voidhft )
    {
    HeaderFooterTool *		hft= (HeaderFooterTool *)voidhft;
    DocumentProperties *	dpSet= &(hft->hftDocPropertiesChosen);

    PropertyMask		dpSetMask;

    utilPropMaskClear( &dpSetMask );
    PROPmaskADD( &dpSetMask, DPpropFACING_PAGES );

    tedAppSetDocumentProperties( hft->hftApplication, &dpSetMask, dpSet );

    return;
    }

static APP_BUTTON_CALLBACK_H( tedHeadFootChangeSectPushed, w, voidhft )
    {
    HeaderFooterTool *		hft= (HeaderFooterTool *)voidhft;
    SectionProperties *		sp= &(hft->hftSectPropertiesChosen);

    PropertyMask		update;

    utilPropMaskClear( &update );
    PROPmaskADD( &update, SPpropTITLEPG );

    if  ( tedAppChangeSectionProperties( hft->hftApplication, &update, sp ) )
	{ LDEB(1);	}

    return;
    }

static APP_BUTTON_CALLBACK_H( tedHeadFootRevertDocPushed, w, voidhft )
    {
    HeaderFooterTool *		hft= (HeaderFooterTool *)voidhft;
    DocumentProperties *	dp= &(hft->hftDocPropertiesChosen);

    PropertyMask		dpSetMask;

    const DocumentAttributeMap * const	dam0= (const DocumentAttributeMap *)0;

    utilPropMaskClear( &dpSetMask );
    PROPmaskADD( &dpSetMask, DPpropFACING_PAGES );

    if  ( docUpdDocumentProperties( (PropertyMask *)0, dp,
			    &dpSetMask, &(hft->hftDocPropertiesSet), dam0 ) )
	{ LDEB(1); return;	}

    tedFormatToolRefreshHeadFootPage( hft );

    return;
    }

static APP_BUTTON_CALLBACK_H( tedHeadFootRevertSectPushed, w, voidhft )
    {
    HeaderFooterTool *		hft= (HeaderFooterTool *)voidhft;

    PropertyMask		spSetMask;

    utilPropMaskClear( &spSetMask );
    PROPmaskADD( &spSetMask, SPpropTITLEPG );

    if  ( docUpdSectProperties(
			    (PropertyMask *)0, &(hft->hftSectPropertiesChosen),
			    &spSetMask, &(hft->hftSectPropertiesSet) ) )
	{ LDEB(1); return;	}

    tedFormatToolRefreshHeadFootPage( hft );

    return;
    }

/************************************************************************/
/*									*/
/*  The 'Titlepg' toggle was pushed.					*/
/*  The 'Facingp' toggle was pushed.					*/
/*									*/
/************************************************************************/

static APP_TOGGLE_CALLBACK_H( tedHeaderFooterToggleTitlepg, w, voidhft, voidtbcs )
    {
    HeaderFooterTool *		hft= (HeaderFooterTool *)voidhft;
    SectionProperties *		sp= &(hft->hftSectPropertiesChosen);
    int				set;

    set= appGuiGetToggleStateFromCallback( w, voidtbcs );

    sp->spHasTitlePage= ( set != 0 );

    return;
    }

static APP_TOGGLE_CALLBACK_H( tedHeaderFooterToggleFacingp, w, voidhft, voidtbcs )
    {
    HeaderFooterTool *		hft= (HeaderFooterTool *)voidhft;
    DocumentProperties *	dp= &(hft->hftDocPropertiesChosen);
    int				set;

    set= appGuiGetToggleStateFromCallback( w, voidtbcs );

    dp->dpHasFacingPages= ( set != 0 );

    return;
    }

/************************************************************************/
/*									*/
/*  A kind of pages was chosen in the optionmenu.			*/
/*									*/
/************************************************************************/

static void tedHeadFootPagesChosen(	int		pages,
					void *		voidhft )
    {
    HeaderFooterTool *		hft= (HeaderFooterTool *)voidhft;

    if  ( pages < 0 || pages >= PAGES__COUNT )
	{ LLDEB(pages,PAGES__COUNT); return;		}

    hft->hftPagesChosen= pages;

    return;
    }


/************************************************************************/
/*									*/
/*  Delete/Edit Headers/Footers.					*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedHeadFootDeleteHeaderPushed, w, voidhft )
    {
    HeaderFooterTool *		hft= (HeaderFooterTool *)voidhft;

    if  ( hft->hftPagesChosen < 0		||
	  hft->hftPagesChosen >= PAGES__COUNT	)
	{ LLDEB(hft->hftPagesChosen,PAGES__COUNT); return;	}

    tedAppDeleteHeaderFooter( hft->hftApplication,
				    DOC_HeaderScopes[hft->hftPagesChosen] );
    return;
    }

static APP_BUTTON_CALLBACK_H( tedHeadFootEditHeaderPushed, w, voidhft )
    {
    HeaderFooterTool *		hft= (HeaderFooterTool *)voidhft;
    APP_WIDGET			relative;

    if  ( hft->hftPagesChosen < 0		||
	  hft->hftPagesChosen >= PAGES__COUNT	)
	{ LLDEB(hft->hftPagesChosen,PAGES__COUNT); return;	}

    relative= hft->hftInspector->aiTopWidget;

    tedAppEditHeaderFooter( hft->hftApplication, relative,
				    hft->hftEditHeaderButton,
				    DOC_HeaderScopes[hft->hftPagesChosen] );
    return;
    }

static APP_BUTTON_CALLBACK_H( tedHeadFootDeleteFooterPushed, w, voidhft )
    {
    HeaderFooterTool *		hft= (HeaderFooterTool *)voidhft;

    if  ( hft->hftPagesChosen < 0		||
	  hft->hftPagesChosen >= PAGES__COUNT	)
	{ LLDEB(hft->hftPagesChosen,PAGES__COUNT); return;	}

    tedAppDeleteHeaderFooter( hft->hftApplication,
				    DOC_FooterScopes[hft->hftPagesChosen] );
    return;
    }

static APP_BUTTON_CALLBACK_H( tedHeadFootEditFooterPushed, w, voidhft )
    {
    HeaderFooterTool *		hft= (HeaderFooterTool *)voidhft;
    APP_WIDGET			relative;

    if  ( hft->hftPagesChosen < 0		||
	  hft->hftPagesChosen >= PAGES__COUNT	)
	{ LLDEB(hft->hftPagesChosen,PAGES__COUNT); return;	}

    relative= hft->hftInspector->aiTopWidget;

    tedAppEditHeaderFooter( hft->hftApplication, relative,
				    hft->hftEditFooterButton,
				    DOC_FooterScopes[hft->hftPagesChosen] );
    return;
    }

/************************************************************************/
/*									*/
/*  Make the 'HeaderFooter' page of the format tool.			*/
/*									*/
/************************************************************************/

void tedFormatFillHeaderFooterPage( HeaderFooterTool *		hft,
			    const HeaderFooterPageResources *	hfpr,
			    InspectorSubject *			is,
			    APP_WIDGET				pageWidget,
			    const InspectorSubjectResources *	isr )
    {
    APP_WIDGET	row= (APP_WIDGET )0;

    const int	heightResizable= 0;
    const int	textColumns= 5;

    /**/
    hft->hftPageResources= hfpr;

    /**/
    docInitDocumentProperties( &(hft->hftDocPropertiesSet) );
    docInitDocumentProperties( &(hft->hftDocPropertiesChosen) );
    docInitSectionProperties( &(hft->hftSectPropertiesSet) );
    docInitSectionProperties( &(hft->hftSectPropertiesChosen) );

    /**/

    appMakeColumnFrameInColumn( &(hft->hftSectionFrame),
			    &(hft->hftSectionPaned),
			    pageWidget, hfpr->hfprSectionHeaderText );

    guiToolMakeLabelAndTextRow( &row, &(hft->hftSectionNumberLabel),
			    &(hft->hftSectionNumberText),
			    hft->hftSectionPaned, hfpr->hfprSectionNumberText,
			    textColumns, 0 );

    row= appMakeRowInColumn( hft->hftSectionPaned, 1, heightResizable );
    hft->hftTitlepgToggle= appMakeToggleInRow( row, hfpr->hfprTitlepgText,
			    tedHeaderFooterToggleTitlepg, (void *)hft, 0, 1 );

    guiToolMake2BottonRow( &(is->isApplyRow), hft->hftSectionPaned,
		&(is->isRevertButton), &(is->isApplyButton),
		isr->isrRevert, isr->isrApplyToSubject,
		tedHeadFootRevertSectPushed, tedHeadFootChangeSectPushed,
		(void *)hft );

    /**************/

    appMakeColumnFrameInColumn( &(hft->hftDocumentFrame),
			    &(hft->hftDocumentPaned),
			    pageWidget, hfpr->hfprDocumentHeaderText );

    row= appMakeRowInColumn( hft->hftDocumentPaned, 1, heightResizable );
    hft->hftFacingpToggle= appMakeToggleInRow( row, hfpr->hfprFacingpText,
				tedHeaderFooterToggleFacingp, (void *)hft, 0, 1 );

    guiToolMake2BottonRow( &row, hft->hftDocumentPaned,
		&(hft->hftRevertDocButton), &(hft->hftChangeDocButton),
		hfpr->hfprRevertDocText, hfpr->hfprChangeDocText,
		tedHeadFootRevertDocPushed, tedHeadFootChangeDocPushed,
		(void *)hft );

    /**************/

    appMakeColumnFrameInColumn( &(hft->hftPagesFrame),
			    &(hft->hftPagesPaned),
			    pageWidget, hfpr->hfprPagesHeaderText );

    appMakeOptionmenuInColumn( &(hft->hftPagesOptionmenu), hft->hftPagesPaned,
					tedHeadFootPagesChosen, (void *)hft );

    guiToolMake2BottonRow( &row, hft->hftPagesPaned,
		&(hft->hftDeleteHeaderButton), &(hft->hftEditHeaderButton),
		hfpr->hfprDeleteHeaderText, hfpr->hfprEditHeaderText,
		tedHeadFootDeleteHeaderPushed, tedHeadFootEditHeaderPushed,
		(void *)hft );

    guiToolMake2BottonRow( &row, hft->hftPagesPaned,
		&(hft->hftDeleteFooterButton), &(hft->hftEditFooterButton),
		hfpr->hfprDeleteFooterText, hfpr->hfprEditFooterText,
		tedHeadFootDeleteFooterPushed, tedHeadFootEditFooterPushed,
		(void *)hft );

    /**************/

    return;
    }

/************************************************************************/
/*									*/
/*  Finish the Header/Footer page.					*/
/*									*/
/************************************************************************/

void tedFormatFillHeaderFooterChoosers( HeaderFooterTool *		hft,
				    const HeaderFooterPageResources *	hfpr )
    {
    appFillInspectorMenu( PAGES__COUNT, PAGES_ALL_PAGES,
			hft->hftPagesItems, hfpr->hfprPagesOptionTexts,
			&(hft->hftPagesOptionmenu) );

    hft->hftPagesChosen= PAGES_ALL_PAGES;

    return;
    }

void tedFormatFinishHeaderFooterPage( HeaderFooterTool *		hft,
				    const HeaderFooterPageResources *	hfpr )
    {
    appOptionmenuRefreshWidth( &(hft->hftPagesOptionmenu) );

    return;
    }

/************************************************************************/
/*									*/
/*  Clean the header footer tool.					*/
/*									*/
/************************************************************************/

void tedFormatCleanHeaderFooterTool(	HeaderFooterTool *		hft )
    {
    docCleanDocumentProperties( &(hft->hftDocPropertiesChosen) );
    docCleanDocumentProperties( &(hft->hftDocPropertiesSet) );
    docCleanSectionProperties( &(hft->hftSectPropertiesChosen) );
    docCleanSectionProperties( &(hft->hftSectPropertiesSet) );

    return;
    }

/************************************************************************/
/*									*/
/*  Get Header/Footer tool resources.					*/
/*									*/
/************************************************************************/

static AppConfigurableResource TED_TedHeaderFooterSubjectResourceTable[]=
    {
    APP_RESOURCE( "formatToolHeaderFooter",
		offsetof(InspectorSubjectResources,isrSubjectName),
		"Headers and Footers" ),

    APP_RESOURCE( "formatToolHeadFootChangeSection",
		offsetof(InspectorSubjectResources,isrApplyToSubject),
		"Apply to Section" ),
    APP_RESOURCE( "formatToolHeadFootRevertSection",
		offsetof(InspectorSubjectResources,isrRevert),
		"Revert" ),
    };

static AppConfigurableResource TED_TedHeaderFooterToolResourceTable[]=
    {
    APP_RESOURCE( "formatToolHeadFootSectionHeader",
		offsetof(HeaderFooterPageResources,hfprSectionHeaderText),
		"Section Properties" ),
    APP_RESOURCE( "formatToolHeadFootSectionNumber",
		offsetof(HeaderFooterPageResources,hfprSectionNumberText),
		"Section" ),
    APP_RESOURCE( "formatToolHeadFootTitlepgText",
		offsetof(HeaderFooterPageResources,hfprTitlepgText),
		"Different first Page" ),

    APP_RESOURCE( "formatToolHeadFootDocumentHeader",
		offsetof(HeaderFooterPageResources,hfprDocumentHeaderText),
		"Document Properties" ),
    APP_RESOURCE( "formatToolHeadFootFacingpText",
		offsetof(HeaderFooterPageResources,hfprFacingpText),
		"Different odd and even Pages" ),
    APP_RESOURCE( "formatToolHeadFootChangeDocument",
		offsetof(HeaderFooterPageResources,hfprChangeDocText),
		"Apply to Document" ),
    APP_RESOURCE( "formatToolHeadFootRevertDocument",
		offsetof(HeaderFooterPageResources,hfprRevertDocText),
		"Revert" ),

    APP_RESOURCE( "formatToolHeadFootPagesHeader",
		offsetof(HeaderFooterPageResources,hfprPagesHeaderText),
		"Pages in Section" ),

    APP_RESOURCE( "formatToolHeadFootFirstPage",
		offsetof(HeaderFooterPageResources,
				hfprPagesOptionTexts[PAGES_FIRST_PAGE]),
		"First Page" ),
    APP_RESOURCE( "formatToolHeadFootSubsequentPages",
		offsetof(HeaderFooterPageResources,
				hfprPagesOptionTexts[PAGES_SUBSEQUENT_PAGES]),
		"Subsequent Pages" ),
    APP_RESOURCE( "formatToolHeadFootAllPages",
		offsetof(HeaderFooterPageResources,
				hfprPagesOptionTexts[PAGES_ALL_PAGES]),
		"All Pages" ),
    APP_RESOURCE( "formatToolHeadFootOddPages",
		offsetof(HeaderFooterPageResources,
				hfprPagesOptionTexts[PAGES_ODD_PAGES]),
		"Odd Pages" ),
    APP_RESOURCE( "formatToolHeadFootEvenPages",
		offsetof(HeaderFooterPageResources,
				hfprPagesOptionTexts[PAGES_EVEN_PAGES]),
		"Even Pages" ),

    APP_RESOURCE( "formatToolHeadFootDeleteHeader",
		offsetof(HeaderFooterPageResources,hfprDeleteHeaderText),
		"Delete Header" ),
    APP_RESOURCE( "formatToolHeadFootEditHeader",
		offsetof(HeaderFooterPageResources,hfprEditHeaderText),
		"Edit Header" ),
    APP_RESOURCE( "formatToolHeadFootDeleteFooter",
		offsetof(HeaderFooterPageResources,hfprDeleteFooterText),
		"Delete Footer" ),
    APP_RESOURCE( "formatToolHeadFootEditFooter",
		offsetof(HeaderFooterPageResources,hfprEditFooterText),
		"Edit Footer" ),
    };

void tedFormatToolGetHeaderFooterResourceTable( EditApplication *	ea,
					HeaderFooterPageResources *	hfpr,
					InspectorSubjectResources *	isr )
    {
    static int	gotToolResources= 0;
    static int	gotSubjectResources= 0;

    if  ( ! gotToolResources )
	{
	appGuiGetResourceValues( &gotToolResources, ea, (void *)hfpr,
				TED_TedHeaderFooterToolResourceTable,
				sizeof(TED_TedHeaderFooterToolResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    if  ( ! gotSubjectResources )
	{
	appGuiGetResourceValues( &gotSubjectResources, ea, (void *)isr,
				TED_TedHeaderFooterSubjectResourceTable,
				sizeof(TED_TedHeaderFooterSubjectResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    return;
    }
