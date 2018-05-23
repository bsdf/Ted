/************************************************************************/
/*									*/
/*  Manage bookmarks related to TOC fields.				*/
/*									*/
/************************************************************************/

#   ifndef	DOC_TOC_BOOKMARKS_H
#   define	DOC_TOC_BOOKMARKS_H

struct BufferDocument;
struct DocumentTree;
struct BufferItem;
struct DocumentField;
struct DocumentSelection;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docRemoveUnbalancedTocBookmarks(
					struct BufferDocument *	bdDoc );

extern int docSetTocBookmarks(		struct BufferDocument *	bd );

extern const struct DocumentField * docGetParaTocBookmark(
					struct BufferDocument *	bd,
					struct DocumentTree *	dt,
					struct BufferItem *	paraNode );

extern int docTocSetOutlineBookmarks(	struct BufferDocument *	bd );

#   endif /*	DOC_TOC_BOOKMARKS_H	*/
