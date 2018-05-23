/************************************************************************/
/*									*/
/*  Ted: Link tool.							*/
/*									*/
/************************************************************************/

#   ifndef	TED_LINK_TOOL_H
#   define	TED_LINK_TOOL_H

#   include	<appFrame.h>
#   include	<appInspector.h>
#   include	<docBuf.h>
#   include	<docSelectionDescription.h>
#   include	<docHyperlinkField.h>
#   include	"tedBookmarkList.h"

/************************************************************************/
/*									*/
/*  User data for a link dialog.					*/
/*									*/
/************************************************************************/

typedef struct LinkToolResources
    {
    char *		lprFileText;
    char *		lprMarkText;
    } LinkToolResources;

typedef struct LinkTool
    {
    EditApplication *		ltApplication;
    AppInspector *		ltInspector;
    const LinkToolResources *	ltPageResources;

    APP_WIDGET			ltFileTextWidget;

    BookmarkList		ltBookmarkList;

    APP_WIDGET			ltSetLinkButton;
    APP_WIDGET			ltRevertButton;
    APP_WIDGET			ltFollowLinkButton;
    APP_WIDGET			ltRemoveLinkButton;

    unsigned int		ltCurrentDocumentId;
    int				ltCanChange;
    int				ltInProgrammaticChange;
    HyperlinkField		ltLinkSet;
    HyperlinkField		ltLinkChosen;
    } LinkTool;

/************************************************************************/
/*									*/
/*  Routine Declarations.						*/
/*									*/
/************************************************************************/

extern void tedFillLinkTool(	LinkTool *			lt,
				const LinkToolResources *	lpr,
				InspectorSubject *		is,
				APP_WIDGET			pageWidget,
				const InspectorSubjectResources * isr );

extern void tedInitLinkTool(	LinkTool *		lt );
extern void tedFormatCleanLinkTool(	LinkTool *		lt );

extern void tedLinkToolGetResourceTable(
				EditApplication *		ea,
				LinkToolResources *		lpr,
				InspectorSubjectResources *	isr );

extern void tedLinkToolFillChoosers(	LinkTool *			lt );
extern void tedFinishLinkTool(		LinkTool *			lt );

extern void tedRefreshLinkTool(	LinkTool *			lt,
				int *				pEnabled,
				int *				pPref,
				InspectorSubject *		is,
				const DocumentSelection *	ds,
				const SelectionDescription *	sd,
				BufferDocument *		bd,
				const unsigned char *		cmdEnabled );

#   endif	/*  TED_LINK_TOOL_H  */
