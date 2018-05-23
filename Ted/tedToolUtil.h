/************************************************************************/
/*									*/
/*  Format tool shared pieced:						*/
/*									*/
/************************************************************************/

#   ifndef	TED_TOOL_UTIL_H
#   define	TED_TOOL_UTIL_H

#   include	<appFrame.h>
#   include	<docRowProperties.h>
#   include	<docBuf.h>
#   include	<appInspector.h>
#   include	<docSelectionGeometry.h>
#   include	<docSelectionDescription.h>

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int tedFormatValidateDimension(		int *		pNewValue,
						int *		pChanged,
						APP_WIDGET	w,
						int		oldValue );

extern BufferDocument * tedFormatCurDoc(	EditDocument **		pEd,
						int *			pTraced,
						EditApplication *	ea );

extern void tedRefreshParaSubjectControls(
				InspectorSubject *		is,
				const DocumentSelection *	ds,
				const SelectionGeometry *	sg,
				const SelectionDescription *	sd,
				const unsigned char *		cmdEnabled );

#   endif	/*  TED_TOOL_UTIL_H */
