/************************************************************************/
/*									*/
/*  Headers/Footers tool:						*/
/*									*/
/************************************************************************/

#   ifndef	TED_HEADER_FOOTER_TOOL_H
#   define	TED_HEADER_FOOTER_TOOL_H

#   include	<docBuf.h>
#   include	<appFrame.h>
#   include	<appInspector.h>
#   include	<docSelectionDescription.h>
#   include	<docSelectionGeometry.h>

/************************************************************************/
/*									*/
/*  A Header/Footer Tool. I.E. The header footer page on the 'Format	*/
/*  Tool'								*/
/*									*/
/************************************************************************/

typedef struct HeaderFooterPageResources
    {
    const char *	hfprSectionHeaderText;
    const char *	hfprSectionNumberText;
    const char *	hfprTitlepgText;

    const char *	hfprDocumentHeaderText;
    const char *	hfprFacingpText;
    const char *	hfprChangeDocText;
    const char *	hfprRevertDocText;

    const char *	hfprPagesHeaderText;
    const char *	hfprPagesOptionTexts[PAGES__COUNT];

    const char *	hfprDeleteHeaderText;
    const char *	hfprEditHeaderText;
    const char *	hfprDeleteFooterText;
    const char *	hfprEditFooterText;
    } HeaderFooterPageResources;

typedef struct HeaderFooterTool
    {
    EditApplication *			hftApplication;
    AppInspector *			hftInspector;
    const HeaderFooterPageResources *	hftPageResources;

    int					hftSectionNumber;

    unsigned char			hftCanInsertHeader;
    unsigned char			hftCanDeleteHeader;
    unsigned char			hftCanInsertFooter;
    unsigned char			hftCanDeleteFooter;

    DocumentProperties			hftDocPropertiesChosen;
    DocumentProperties			hftDocPropertiesSet;

    SectionProperties			hftSectPropertiesChosen;
    SectionProperties			hftSectPropertiesSet;
    int					hftPagesChosen;
    int					hftInTreeType;
    unsigned char			hftPagesOptionsEnabled[PAGES__COUNT];
    unsigned char			hftPagesHeaderExists[PAGES__COUNT];
    unsigned char			hftPagesFooterExists[PAGES__COUNT];
    unsigned char			hftPagesHeaderApplies[PAGES__COUNT];
    unsigned char			hftPagesFooterApplies[PAGES__COUNT];

    APP_WIDGET				hftSectionFrame;
    APP_WIDGET				hftSectionPaned;
    APP_WIDGET				hftSectionNumberLabel;
    APP_WIDGET				hftSectionNumberText;
    APP_WIDGET				hftTitlepgToggle;

    APP_WIDGET				hftDocumentFrame;
    APP_WIDGET				hftDocumentPaned;
    APP_WIDGET				hftFacingpToggle;
    APP_WIDGET				hftRevertDocButton;
    APP_WIDGET				hftChangeDocButton;

    APP_WIDGET				hftPagesFrame;
    APP_WIDGET				hftPagesPaned;
    AppOptionmenu			hftPagesOptionmenu;
    APP_WIDGET				hftPagesItems[PAGES__COUNT];

    APP_WIDGET				hftDeleteHeaderButton;
    APP_WIDGET				hftEditHeaderButton;
    APP_WIDGET				hftDeleteFooterButton;
    APP_WIDGET				hftEditFooterButton;
    } HeaderFooterTool;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void tedRefreshHeaderFooterTool(
				HeaderFooterTool *		hft,
				int *				pEnabled,
				int *				pPref,
				InspectorSubject *		is,
				const DocumentSelection *	ds,
				const SelectionDescription *	sd,
				const SelectionGeometry *	sg,
				BufferDocument *		bd,
				const unsigned char *		cmdEnabled );

extern void tedFormatToolGetHeaderFooterResourceTable( EditApplication * ea,
					HeaderFooterPageResources *	hfpr,
					InspectorSubjectResources *	isr );

extern void tedFormatCleanHeaderFooterTool(	HeaderFooterTool *	hft );

extern void tedFormatFillHeaderFooterChoosers( HeaderFooterTool *	hft,
				    const HeaderFooterPageResources *	hfpr );

extern void tedFormatFinishHeaderFooterPage( HeaderFooterTool *		hft,
				    const HeaderFooterPageResources *	hfpr );

extern void tedFormatFillHeaderFooterPage(
			HeaderFooterTool *			hft,
			const HeaderFooterPageResources *	hfpr,
			InspectorSubject *			is,
			APP_WIDGET				pageWidget,
			const InspectorSubjectResources *	isr );

#   endif	/*  TED_HEADER_FOOTER_TOOL_H */
