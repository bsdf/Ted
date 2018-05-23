/************************************************************************/
/*									*/
/*  Cell tool:								*/
/*									*/
/************************************************************************/

#   ifndef	TED_CELL_TOOL_H
#   define	TED_CELL_TOOL_H

#   include	"tedBorderTool.h"
#   include	"tedShadingTool.h"
#   include	<docSelectionDescription.h>

/************************************************************************/
/*									*/
/*  Resources for the 'Cell' page.					*/
/*									*/
/************************************************************************/

typedef struct CellPageResources
    {
    char *			cprRowColumn;

    char *			cprMerged;

    const char *		cprValignItemTexts[DOCtva_COUNT];

    char *			cprWidth;
    char *			cprBorders;

    char *			cprLeftBorder;
    char *			cprRightBorder;
    char *			cprTopBorder;
    char *			cprBottomBorder;

    char *			cprShadingPattern;
    char *			cprValign;

    char *			cprShading;

    char *			cprNextRow;
    char *			cprPrevRow;

    ShadingToolResources	cprShadingResources;
    BorderToolResources		cprBorderToolResources;
    } CellPageResources;

/************************************************************************/
/*									*/
/*  A Cell tool, i.e. the 'Cell' page of the format tool.		*/
/*									*/
/************************************************************************/

typedef struct CellTool
    {
    EditApplication *		ctApplication;
    AppInspector *		ctInspector;
    const CellPageResources *	ctPageResources;

    TableRectangle		ctTableRectangle;
    unsigned char		ctCanMerge;
    unsigned char		ctHorMerge;
    unsigned char		ctVerMerge;
    unsigned char		ctCanChange;

    APP_WIDGET			ctRowColumnRow;
    APP_WIDGET			ctRowColumnLabel;
    APP_WIDGET			ctRowColumnText;

    APP_WIDGET			ctValignRow;
    AppOptionmenu		ctValignMenu;
    APP_WIDGET			ctValignItems[DOCtva_COUNT];

    APP_WIDGET			ctMergedRow;
    APP_WIDGET			ctMergedLabel; /* to fill the place */
    APP_WIDGET			ctMergedToggle;

    CellProperties		ctPropertiesSet;
    CellProperties		ctPropertiesChosen;

    APP_WIDGET			ctBordersFrame;
    APP_WIDGET			ctBordersPaned;
    BorderTool			ctTopBorderTool;
    BorderTool			ctBottomBorderTool;
    BorderTool			ctLeftBorderTool;
    BorderTool			ctRightBorderTool;

    ShadingTool			ctShadingTool;

    APP_WIDGET			ctPrevRowButton;
    APP_WIDGET			ctNextRowButton;
    } CellTool;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void tedRefreshCellTool(
				CellTool *			ct,
				int *				pEnabled,
				int *				pPref,
				InspectorSubject *		is,
				const DocumentSelection *	ds,
				const SelectionDescription *	sd,
				const BufferDocument *		bd,
				const unsigned char *		cmdEnabled );

extern void tedCellToolFillChoosers(	CellTool *			ct,
					const CellPageResources *	cpr );

extern void tedFormatFillCellPage( CellTool *			ct,
				const CellPageResources *	cpr,
				AppInspector *			ai,
				int				subjectPage,
				InspectorSubject *		is,
				APP_WIDGET			pageWidget,
				const InspectorSubjectResources * isr );

extern void tedFormatToolGetCellResourceTable(
					EditApplication *		ea,
					CellPageResources *		cpr,
					InspectorSubjectResources *	isr );

extern void tedInitCellTool(	CellTool *			ct );
extern void tedCleanCellTool(	CellTool *			ct );

extern void tedFormatFinishCellPage(	CellTool *			ct,
					const CellPageResources *	cpr );

#   endif	/*  TED_CELL_TOOL_H */
