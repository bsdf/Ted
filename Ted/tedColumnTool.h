/************************************************************************/
/*									*/
/*  Column Tool.							*/
/*									*/
/************************************************************************/

#   ifndef	TED_COLUMN_TOOL_H
#   define	TED_COLUMN_TOOL_H

#   include	"tedBorderTool.h"
#   include	<docSelectionDescription.h>
#   include	<docSelectionGeometry.h>

/************************************************************************/
/*									*/
/*  Resources for the 'Column' page.					*/
/*									*/
/************************************************************************/

typedef struct ColumnPageResources
    {
    char *		cprColumn;

    char *		cprWidth;
    char *		cprBorders;
    char *		cprLeftBorder;
    char *		cprRightBorder;

    BorderToolResources	cprBorderToolResources;
    } ColumnPageResources;

/************************************************************************/
/*									*/
/*  A Column tool, i.e. the 'Column' page of the format tool.		*/
/*									*/
/************************************************************************/

typedef struct ColumnTool
    {
    EditApplication *		ctApplication;
    AppInspector *		ctInspector;
    const ColumnPageResources *	ctPageResources;

    TableRectangle		ctTableRectangle;

    int				ctBlockFrameWide;

    CellProperties		ctPropertiesSet;
    CellProperties		ctPropertiesChosen;
    int				ctWidthSet;
    int				ctWidthChosen;
    int				ctHalfGapWidthTwips;
    unsigned char		ctCanChange;

    APP_WIDGET			ctColumnRow;
    APP_WIDGET			ctColumnLabel;
    APP_WIDGET			ctColumnText;

    APP_WIDGET			ctWidthRow;
    APP_WIDGET			ctWidthLabel;
    APP_WIDGET			ctWidthText;

    APP_WIDGET			ctBordersFrame;
    APP_WIDGET			ctBordersPaned;
    BorderTool			ctLeftBorderTool;
    BorderTool			ctRightBorderTool;
    } ColumnTool;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void tedFormatFillColumnPage(	ColumnTool *		ct,
				const ColumnPageResources *	cpr,
				AppInspector *			ai,
				int				subjectPage,
				InspectorSubject *		is,
				APP_WIDGET			pageWidget,
				const InspectorSubjectResources * isr );

extern void tedRefreshColumnTool(
				ColumnTool *			ct,
				int *				pEnabled,
				int *				pPref,
				InspectorSubject *		is,
				const DocumentSelection *	ds,
				const SelectionGeometry *	sg,
				const SelectionDescription *	sd,
				const unsigned char *		cmdEnabled );

extern void tedInitColumnTool(	ColumnTool *	ct );
extern void tedCleanColumnTool(	ColumnTool *	ct );

extern void tedFormatFinishColumnPage(	ColumnTool *			ct,
					const ColumnPageResources *	cpr );

extern void tedColumnToolFillChoosers( ColumnTool *			ct,
					const ColumnPageResources *	cpr );

extern void tedFormatToolGetColumnResourceTable(
					EditApplication *		ea,
					ColumnPageResources *		cpr,
					InspectorSubjectResources *	isr );

#   endif	/*  TED_COLUMN_TOOL_H */
