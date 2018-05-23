/************************************************************************/
/*									*/
/*  Intermediary Routines for the page layout page.			*/
/*									*/
/************************************************************************/

#   ifndef	TED_PAGE_LAYOUT_TOOL_H
#   define	TED_PAGE_LAYOUT_TOOL_H

#   include	<appPageLayoutTool.h>
#   include	<docBuf.h>
#   include	<docSelectionDescription.h>

/************************************************************************/
/*									*/
/*  Routine Declarations.						*/
/*									*/
/************************************************************************/

extern void tedRefreshPageLayoutTool(
				PageLayoutTool *		plt,
				int *				pEnabled,
				int *				pPref,
				InspectorSubject *		is,
				const DocumentSelection *	ds,
				const SelectionDescription *	sd,
				const BufferDocument *		bd,
				const unsigned char *		cmdEnabled );

extern void tedFormatCleanPageLayoutTool(	PageLayoutTool *	plt );

extern void tedFormatFinishPageLayoutPage(	PageLayoutTool *	plt,
					const PageLayoutPageResources *	plpr );

extern void tedFormatFillPageLayoutPage(
			PageLayoutTool *			plt,
			const PageLayoutPageResources *		plpr,
			InspectorSubject *			is,
			APP_WIDGET				pageWidget,
			const InspectorSubjectResources *	isr );

#   endif	/*  TED_PAGE_LAYOUT_TOOL_H */
