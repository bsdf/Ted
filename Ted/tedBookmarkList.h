/************************************************************************/
/*									*/
/*  Ted: The list of bookmarks on the Bookmark and Hyperlink tools.	*/
/*									*/
/************************************************************************/

#   ifndef	TED_BOOKMARK_LIST_H
#   define	TED_BOOKMARK_LIST_H

#   include	<appFrame.h>
#   include	<docBuf.h>

typedef struct BookmarkList
    {
    APP_WIDGET				blMarkLabelWidget;
    APP_WIDGET				blMarkTextWidget;

    APP_WIDGET				blMarkListWidget;

    unsigned char			blIsLocal;
    unsigned char			blMarkChosenExists;
    } BookmarkList;

/************************************************************************/
/*									*/
/*  Routine Declarations.						*/
/*									*/
/************************************************************************/

extern void tedInitBookmarkList(	BookmarkList *		bl );

extern void tedFillBookmarkList(	BookmarkList *		bl,
					int			includeTocMarks,
					const BufferDocument *	bd );

extern void tedBookmarkFindChosen(	BookmarkList *		bl,
					EditApplication *	ea,
					const MemoryBuffer *	mbChosen );

extern void tedBookmarkUpdateSelectionInList(
					BookmarkList *		bl,
					const MemoryBuffer *	mbChosen );

extern void tedBookmarkListToText(	BookmarkList *		bl,
					MemoryBuffer *		mbChosen,
					void *			voidlcs,
					APP_WIDGET		w );

#   endif	/*  TED_BOOKMARK_LIST_H  */
