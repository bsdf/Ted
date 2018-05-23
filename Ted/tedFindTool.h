/************************************************************************/
/*									*/
/*  Find Tool.								*/
/*									*/
/************************************************************************/

#   ifndef	TED_FIND_TOOL_H
#   define	TED_FIND_TOOL_H

#   include	<appFrame.h>
#   include	<docBuf.h>
#   include	<appFindTool.h>
#   include	<docSelectionDescription.h>

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void tedRefreshFindTool(	FindTool *			aft,
				int *				pEnabled,
				int *				pPref,
				InspectorSubject *		is,
				const DocumentSelection *	ds,
				const SelectionDescription *	sd,
				const BufferDocument *		bd,
				int				documentId,
				const unsigned char *		cmdEnabled );

extern void tedFillFindTool(	FindTool *			aft,
				const FindToolResources *	aftr,
				AppInspector *			ai,
				int				subjectPage,
				InspectorSubject *		is,
				APP_WIDGET			pageWidget,
				const InspectorSubjectResources * isr );

extern void tedFormatShowFindPage( EditApplication *		ea );

#   endif	/*  TED_FIND_TOOL_H */
