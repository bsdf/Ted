/************************************************************************/
/*									*/
/*  Ted: Link tool.							*/
/*									*/
/************************************************************************/

#   ifndef	TED_BOOKMARK_TOOL_H
#   define	TED_BOOKMARK_TOOL_H

#   include	<appFrame.h>
#   include	<appInspector.h>
#   include	<docBuf.h>
#   include	<docSelectionDescription.h>
#   include	"tedBookmarkList.h"

/************************************************************************/
/*									*/
/*  User data for a bookmark dialog.					*/
/*									*/
/************************************************************************/

typedef struct BookmarkPageResources
    {
    char *		bprMarkText;
    char *		bprShowTocMarks;
    } BookmarkPageResources;

typedef struct BookmarkTool
    {
    EditApplication *			btApplication;
    AppInspector *			btInspector;
    const BookmarkPageResources *	btResources;

    BookmarkList			btBookmarkList;

    APP_WIDGET				btSetBookmarkButton;
    APP_WIDGET				btRevertButton;
    APP_WIDGET				btGoToBookmarkButton;
    APP_WIDGET				btRemoveBookmarkButton;

    unsigned int			btCurrentDocumentId;
    int					btCanChange;
    MemoryBuffer			btMarkSet;
    MemoryBuffer			btMarkChosen;

    int					btInProgrammaticChange;
    } BookmarkTool;

/************************************************************************/
/*									*/
/*  Routine Declarations.						*/
/*									*/
/************************************************************************/

extern void tedFillBookmarkTool(BookmarkTool *			bt,
				const BookmarkPageResources *	bpr,
				InspectorSubject *		is,
				APP_WIDGET			pageWidget,
				const InspectorSubjectResources * isr );

extern void tedInitBookmarkTool(	BookmarkTool *		bt );
extern void tedFormatCleanBookmarkTool(	BookmarkTool *		bt );

extern void tedBookmarkToolGetResourceTable(
				EditApplication *		ea,
				BookmarkPageResources *		bpr,
				InspectorSubjectResources *	isr );

extern void tedBookmarkToolFillChoosers(BookmarkTool *			bt );

extern void tedFinishBookmarkTool(	BookmarkTool *			bt );

extern void tedRefreshBookmarkTool(
				BookmarkTool *			bt,
				int *				pEnabled,
				int *				pPref,
				InspectorSubject *		is,
				const DocumentSelection *	ds,
				const SelectionDescription *	sd,
				DocumentTree *			ei,
				BufferDocument *		bd,
				const unsigned char *		cmdEnabled );

#   endif	/*  TED_BOOKMARK_TOOL_H  */
