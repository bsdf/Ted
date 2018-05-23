/************************************************************************/
/*									*/
/*  Row tool:								*/
/*									*/
/************************************************************************/

#   ifndef	TED_ROW_TOOL_H
#   define	TED_ROW_TOOL_H

#   include	"tedBorderTool.h"
#   include	"tedShadingTool.h"
#   include	"tedHeightTool.h"
#   include	"tedRowMarginsTool.h"
#   include	<docSelectionDescription.h>
#   include	<docSelectionGeometry.h>

/************************************************************************/
/*									*/
/*  Resources for the 'Row' page.					*/
/*									*/
/************************************************************************/

typedef struct RowPageResources
    {
    char *			rprHeightFree;
    char *			rprHeightAtLeast;
    char *			rprHeightExactly;

    char *			rprBorders;
    char *			rprTopBorder;
    char *			rprBottomBorder;

    RowMarginsToolResources	rprMarginsResources;

    char *			rprKeepOnPage;
    char *			rprIsTableHeader;

    char *			rprKeepWithNext;

    char *			rprShading;
    ShadingToolResources	rprShadingResources;
    BorderToolResources		rprBorderToolResources;
    } RowPageResources;

/************************************************************************/
/*									*/
/*  A row tool, i.e. the 'Row' page of the format tool.			*/
/*									*/
/************************************************************************/

typedef struct RowTool
    {
    EditApplication *		rtApplication;
    AppInspector *		rtInspector;
    const RowPageResources *	rtPageResources;

    RowProperties		rtRowPropertiesSet;
    RowProperties		rtRowPropertiesChosen;
    CellProperties		rtCellPropertiesSet;
    CellProperties		rtCellPropertiesChosen;

    TableRectangle		rtTableRectangle;

    unsigned char		rtCanChange;

    APP_WIDGET			rtRowRow;
    APP_WIDGET			rtRowLabel;
    APP_WIDGET			rtRowText;

    HeightChooser		rtHeightChooser;
    RowMarginsTool		rtMarginsTool;

    APP_WIDGET			rtBordersFrame;
    APP_WIDGET			rtBordersPaned;
    BorderTool			rtTopBorderTool;
    BorderTool			rtBottomBorderTool;

    ShadingTool			rtShadingTool;

    APP_WIDGET			rtKeepHeaderRow;
    APP_WIDGET			rtKeepOnPageToggle;
    APP_WIDGET			rtIsTableHeaderToggle;

    APP_WIDGET			rtKeepfollowRow;
    APP_WIDGET			rtKeepfollowToggle;
    APP_WIDGET			rtKeepfollowLabel;
    } RowTool;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void tedFormatFillRowPage( RowTool *			rt,
				const RowPageResources *	rpr,
				AppInspector *			ai,
				int				subjectPage,
				InspectorSubject *		is,
				APP_WIDGET			pageWidget,
				const InspectorSubjectResources * isr );

extern void tedRowToolFillChoosers(	RowTool *			rt,
					const RowPageResources *	rpr );

extern void tedFormatFinishRowPage( 	RowTool *			rt,
					const RowPageResources *	rpr );

extern void tedFormatToolGetRowResourceTable(
				EditApplication *		ea,
				RowPageResources *		spr,
				InspectorSubjectResources *	isr );

extern void tedRefreshRowTool(	RowTool *			rt,
				int *				pEnabled,
				int *				pPref,
				InspectorSubject *		is,
				const DocumentSelection *	ds,
				const SelectionGeometry *	sg,
				const SelectionDescription *	sd,
				BufferDocument *		bd,
				const unsigned char *		cmdEnabled );

extern void tedInitRowTool(	RowTool *	rt );
extern void tedCleanRowTool(	RowTool *	rt );

#   endif	/*  TED_ROW_TOOL_H */
