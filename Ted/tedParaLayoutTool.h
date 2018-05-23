/************************************************************************/
/*									*/
/*  Paragraph layout tool:						*/
/*									*/
/************************************************************************/

#   ifndef	TED_PARA_LAYOUT_TOOL_H
#   define	TED_PARA_LAYOUT_TOOL_H

#   include	<docPropVal.h>
#   include	<docBuf.h>
#   include	<docSelectionGeometry.h>
#   include	<docSelectionDescription.h>
#   include	<appFrame.h>
#   include	<appInspector.h>
#   include	"tedHeightTool.h"

/************************************************************************/
/*									*/
/*  Resources for the 'Paragraph Formatting' page.			*/
/*									*/
/************************************************************************/

typedef struct ParagraphLayoutPageResources
    {
    const char *	plprParaListLevel;
    const char *	plprParaFirstIndent;
    const char *	plprParaLeftIndent;
    const char *	plprParaRightIndent;

    const char *	pprParaSpaceAbove;
    const char *	pprParaSpaceBelow;

    const char *	pprOnNewPage;
    const char *	pprOnOnePage;

    const char *	pprWidctrl;
    const char *	pprKeepWithNext;

    const char *	pprParaLineDistFree;
    const char *	pprParaLineDistAtLeast;
    const char *	pprParaLineDistExactly;

    const char *	pprParaAlignment;
    const char *	pprParaAlignMenuTexts[DOCtha_COUNT];
    } ParagraphLayoutPageResources;

/************************************************************************/
/*									*/
/*  A paragraph tool, i.e. the 'Paragraph' page of the format tool.	*/
/*									*/
/************************************************************************/

typedef struct ParagraphLayoutTool
    {
    EditApplication *				ptApplication;
    AppInspector *				ptInspector;
    const ParagraphLayoutPageResources *	ptPageResources;

    unsigned char				pltCanChange;

    ParagraphProperties				ptPropertiesSet;
    ParagraphProperties				ptPropertiesChosen;

    APP_WIDGET					pltListLevelRow;
    APP_WIDGET					pltListLevelText;

    APP_WIDGET					ptFirstIndentRow;
    APP_WIDGET					ptFirstIndentLabel;
    APP_WIDGET					ptFirstIndentText;

    APP_WIDGET					ptLeftIndentRow;
    APP_WIDGET					ptLeftIndentLabel;
    APP_WIDGET					ptLeftIndentText;

    APP_WIDGET					ptRightIndentRow;
    APP_WIDGET					ptRightIndentLabel;
    APP_WIDGET					ptRightIndentText;

    APP_WIDGET					pltPageRow;
    APP_WIDGET					ptOnNewPageToggle;
    APP_WIDGET					ptKeepOnPageToggle;

    APP_WIDGET					pltKeepRow;
    APP_WIDGET					ptWidowControlToggle;
    APP_WIDGET					ptKeepWithNextToggle;

    APP_WIDGET					pltSpaceAboveRow;
    APP_WIDGET					pltSpaceAboveToggle;
    APP_WIDGET					pltSpaceAboveText;

    APP_WIDGET					pltSpaceBelowRow;
    APP_WIDGET					pltSpaceBelowToggle;
    APP_WIDGET					pltSpaceBelowText;

    HeightChooser				ptLineDistChooser;

    APP_WIDGET					pltAlignmentRow;
    APP_WIDGET					pltAlignmentLabel;
    AppOptionmenu				pltAlignOptionmenu;
    APP_WIDGET					pltAlignItems[DOCtha_COUNT];
    } ParagraphLayoutTool;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

void tedFormatToolGetParaLayoutResourceTable(
					EditApplication *		ea,
					ParagraphLayoutPageResources *	plpr,
					InspectorSubjectResources *	isr );

extern void tedParaLayoutToolFillChoosers(
				ParagraphLayoutTool *			plt,
				const ParagraphLayoutPageResources *	plpr );

extern void tedFormatFillParagraphLayoutPage(
				ParagraphLayoutTool *			plt,
				const ParagraphLayoutPageResources *	ppr,
				InspectorSubject *			is,
				APP_WIDGET				pgW,
				const InspectorSubjectResources *	isr );

extern void tedRefreshParaLayoutTool(
				ParagraphLayoutTool *		plt,
				int *				pEnabled,
				int *				pPref,
				InspectorSubject *		is,
				const DocumentSelection *	ds,
				const SelectionGeometry *	sg,
				const SelectionDescription *	sd,
				const unsigned char *		cmdEnabled );

extern void tedFormatCleanParaLayoutTool(
				ParagraphLayoutTool *		plt );

extern void tedFinishParaLayoutPage(
				ParagraphLayoutTool *			plt,
				const ParagraphLayoutPageResources *	plpr );

#   endif	/*  TED_PARA_LAYOUT_TOOL_H */
