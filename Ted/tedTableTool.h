/************************************************************************/
/*									*/
/*  'Table' page of the format tool.					*/
/*									*/
/************************************************************************/

#   ifndef	TED_TABLE_TOOL_H
#   define	TED_TABLE_TOOL_H

#   include	<appFrame.h>
#   include	<appInspector.h>
#   include	<docBuf.h>
#   include	"tedRowMarginsTool.h"
#   include	<docSelectionDescription.h>
#   include	<docSelectionGeometry.h>

/************************************************************************/
/*									*/
/*  Resources for the 'Table' page.					*/
/*									*/
/************************************************************************/

typedef struct TablePageResources
    {
    RowMarginsToolResources	tprMarginsResources;
    } TablePageResources;

/************************************************************************/
/*									*/
/*  A table tool, i.e. the 'Table' page of the format tool.		*/
/*									*/
/************************************************************************/

typedef struct TableTool
    {
    EditApplication *		ttApplication;
    AppInspector *		ttInspector;
    const TablePageResources *	ttPageResources;

    RowProperties		ttPropertiesSet;
    RowProperties		ttPropertiesChosen;

    TableRectangle		ttTableRectangle;

    unsigned char		ttCanChange;

    RowMarginsTool		ttMarginsTool;
    } TableTool;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void tedFormatFillTablePage( TableTool *			tt,
				const TablePageResources *	tpr,
				InspectorSubject *		is,
				APP_WIDGET			pageWidget,
				const InspectorSubjectResources * isr );

extern void tedFormatToolGetTableResourceTable(
					EditApplication *		ea,
					TablePageResources *		tpr,
					InspectorSubjectResources *	isr );

extern void tedRefreshTableTool(
				TableTool *			tt,
				int *				pEnabled,
				int *				pPref,
				InspectorSubject *		is,
				const DocumentSelection *	ds,
				const SelectionGeometry *	sg,
				const SelectionDescription *	sd,
				BufferDocument *		bd,
				const unsigned char *		cmdEnabled );

extern void tedFormatCleanTableTool(	TableTool *		tt );

#   endif	/*  TED_TABLE_TOOL_H */
