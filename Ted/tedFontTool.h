/************************************************************************/
/*									*/
/*  Font tool:								*/
/*									*/
/************************************************************************/

#   ifndef	TED_FONT_TOOL_H
#   define	TED_FONT_TOOL_H

#   include	<appFontTool.h>
#   include	<appFrame.h>
#   include	<docBuf.h>
#   include	<docSelectionDescription.h>

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void tedRefreshFontTool(	AppFontChooser *		afc,
				int *				pEnabled,
				int *				pPref,
				InspectorSubject *		is,
				const DocumentSelection *	ds,
				const SelectionDescription *	sd,
				const BufferDocument *		bd,
				int				documentId,
				const unsigned char *		cmdEnabled );

extern void tedFontToolGetResourceTable(
				EditApplication *		ea,
				AppFontToolResources *		aftr,
				InspectorSubjectResources *	isr );

#   endif	/*  TED_FONT_TOOL_H */
