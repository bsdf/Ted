/************************************************************************/
/*									*/
/*  Paragraph borders and shading tool:					*/
/*									*/
/************************************************************************/

#   ifndef	TED_PARA_ORNAMENTS_TOOL_H
#   define	TED_PARA_ORNAMENTS_TOOL_H

#   include	"tedBorderTool.h"
#   include	"tedShadingTool.h"
#   include	<docSelectionDescription.h>
#   include	<docSelectionGeometry.h>

/************************************************************************/
/*									*/
/*  Resources for the 'Paragraph Borders and Shading' page.		*/
/*									*/
/************************************************************************/

typedef struct ParagraphOrnamentsPageResources
    {
    char *			poprShading;
    ShadingToolResources	poprShadingResources;
    BorderToolResources		poprBorderToolResources;

    char *			poprBorders;
    char *			poprTopBorder;
    char *			poprBottomBorder;
    char *			poprLeftBorder;
    char *			poprRightBorder;
    } ParagraphOrnamentsPageResources;

/************************************************************************/
/*									*/
/*  A paragraph tool, i.e. the 'Paragraph' page of the format tool.	*/
/*									*/
/************************************************************************/

typedef struct ParagraphOrnamentsTool
    {
    EditApplication *				potApplication;
    AppInspector *				potInspector;
    const ParagraphOrnamentsPageResources *	potPageResources;

    ParagraphProperties				potPropertiesSet;
    ParagraphProperties				potPropertiesChosen;

    unsigned char				potCanChange;

    APP_WIDGET					potBordersFrame;
    APP_WIDGET					potBordersPaned;
    BorderTool					potTopBorderTool;
    BorderTool					potBottomBorderTool;
    BorderTool					potLeftBorderTool;
    BorderTool					potRightBorderTool;

    ShadingTool					potShadingTool;
    } ParagraphOrnamentsTool;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void tedFormatToolGetParaOrnamentsResourceTable(
				    EditApplication *			ea,
				    ParagraphOrnamentsPageResources *	plpr,
				    InspectorSubjectResources *		isr );

extern void tedParaOrnamentsToolFillChoosers(
				ParagraphOrnamentsTool *		pot,
				const ParagraphOrnamentsPageResources *	popr );

extern void tedFormatFillParagraphOrnamentsPage(
			ParagraphOrnamentsTool *		pot,
			const ParagraphOrnamentsPageResources *	popr,
			AppInspector *				ai,
			int					subjectPage,
			InspectorSubject *			is,
			APP_WIDGET				pgW,
			const InspectorSubjectResources *	isr );

extern void tedRefreshParaOrnamentsTool(
				ParagraphOrnamentsTool *	pot,
				int *				pEnabled,
				int *				pPref,
				InspectorSubject *		is,
				const DocumentSelection *	ds,
				const SelectionGeometry *	sg,
				const SelectionDescription *	sd,
				const unsigned char *		cmdEnabled );

extern void tedInitParaOrnamentsTool(
				ParagraphOrnamentsTool *	pot );
extern void tedCleanParaOrnamentsTool(
				ParagraphOrnamentsTool *	pot );

extern void tedFormatFinishParaOrnamentsPage(
				ParagraphOrnamentsTool *		pot,
				const ParagraphOrnamentsPageResources *	popr );

#   endif	/*  TED_PARA_ORNAMENTS_TOOL_H */
