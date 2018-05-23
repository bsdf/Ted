/************************************************************************/
/*									*/
/*  A collection of objects that is to be accessed by number.		*/
/*									*/
/************************************************************************/

#   ifndef	PAGED_LIST_H
#   define	PAGED_LIST_H

#   include	"utilIndexSet.h"

typedef void (*InitPagedListItem)(	void *	vit );
typedef void (*CleanPagedListItem)(	void *	vit );

typedef int (*PagedListForAllFun)(	int	n,
					void *	vit,
					void *	through );

typedef struct PagedList
    {
    int			plItemCount;
    void **		plItemPages;
    int			plItemPageCount;
    int			plSizeofItem;

    IndexSet		plItemUsed;

    InitPagedListItem	plInitItem;
    CleanPagedListItem	plCleanItem;
    } PagedList;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void * utilPagedListGetItemByNumber(
					const PagedList *	pl,
					int			n );

extern void utilPagedListDeleteItemByNumber(
					PagedList *		pl,
					int			n );

extern void * utilPagedListClaimItem(	PagedList *		pl,
					int			n );

extern int utilPagedListSetSize(	PagedList *		pl,
					int			n );

extern void utilInitPagedList(		PagedList *		pl );
extern void utilCleanPagedList(		PagedList *		pl );

extern void utilStartPagedList(		PagedList *		pl,
					int			sizeofItem,
					InitPagedListItem	initItem,
					CleanPagedListItem 	cleanItem );

extern void * utilPagedListClaimItemAtEnd(	int *			pN,
						PagedList *		pl );

extern void * utilPagedListClaimNewItem(	int *			pN,
						PagedList *		pl );

extern int utilPagedListForAll(		const PagedList *	pl,
					PagedListForAllFun	fun,
					void *			through );

extern void * utilPagedListGetNext(	int *			pN,
					const PagedList *	pl,
					int			n );

# define utilPagedListGetFirst( pN, is ) utilPagedListGetNext( (pN), (is), -1 )

#   endif	/*  PAGED_LIST_H	*/
