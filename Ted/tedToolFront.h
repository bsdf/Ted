/************************************************************************/
/*									*/
/*  Format tool:							*/
/*									*/
/************************************************************************/

#   ifndef	TED_TOOL_FRONT_H
#   define	TED_TOOL_FRONT_H

#   include	<appFrame.h>
#   include	<appInspector.h>
#   include	<docSelect.h>
#   include	<docDocumentTree.h>
#   include	<docSelectionGeometry.h>
#   include	<docSelectionDescription.h>

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void tedFormatToolAdaptToSelection( AppInspector *	ai,
				EditDocument *			ed,
				int				choosePage,
				DocumentTree *			ei,
				const DocumentSelection *	ds,
				const SelectionGeometry *	sg,
				const SelectionDescription *	sd,
				const unsigned char *		cmdEnabled );

extern void tedFormatShowPagePage( EditApplication *		ea );
extern void tedFormatShowFontPage( EditApplication *		ea );
extern void tedFormatShowSymbolPage( EditApplication *		ea );
extern void tedFormatShowLinkPage( EditApplication *		ea );
extern void tedFormatShowBookmarkPage( EditApplication *	ea );

extern void tedFormatShowParaLayoutPage( EditApplication *	ea );
extern void tedFormatShowParaTabsPage( EditApplication *	ea );
extern void tedFormatShowColumnPage(	EditApplication *	ea );
extern void tedFormatShowTablePage(	EditApplication *	ea );

extern void tedShowFormatTool(	APP_WIDGET		toolOption,
				EditApplication *	ea );

extern void tedFormatFieldListChanged(	EditApplication *	ea );

#   endif	/*  TED_TOOL_FRONT_H */
