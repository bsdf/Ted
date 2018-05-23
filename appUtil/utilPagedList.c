/************************************************************************/
/*									*/
/*   collection of objects that is to be accessed by number.		*/
/*									*/
/************************************************************************/

#   include	"appUtilConfig.h"

#   include	<stdlib.h>

#   include	<appDebugon.h>

#   include	"utilPagedList.h"

/************************************************************************/

#   define	PL_PGSZ		512

/************************************************************************/
/*									*/
/*  Initialise a Paged List.						*/
/*  Clean a Paged List.							*/
/*  Copy a Paged List.							*/
/*									*/
/************************************************************************/

void utilInitPagedList(	PagedList *	pl )
    {
    pl->plItemCount= 0;
    pl->plItemPages= (void **)0;
    pl->plItemPageCount= 0;
    pl->plSizeofItem= 0;

    utilInitIndexSet( &(pl->plItemUsed) );

    pl->plInitItem= (InitPagedListItem)0;
    pl->plCleanItem= (CleanPagedListItem)0;
    }

void utilStartPagedList(		PagedList *		pl,
					int			sizeofItem,
					InitPagedListItem	initItem,
					CleanPagedListItem 	cleanItem )
    {
    if  ( pl->plItemPageCount > 0 )
	{ LDEB(pl->plItemPageCount);	}

    pl->plSizeofItem= sizeofItem;
    pl->plInitItem= initItem;
    pl->plCleanItem= cleanItem;
    }

static int utilPagedListCleanItem(	int		n,
					void *		vpl )
    {
    PagedList *		pl= (PagedList *)vpl;
    void *		vit= utilPagedListGetItemByNumber( pl, n );

    if  ( vit && pl->plCleanItem )
	{ (*pl->plCleanItem)( vit );	}

    return 0;
    }

void utilCleanPagedList(	PagedList *	pl )
    {
    int		page;

    if  ( pl->plCleanItem )
	{
	utilIndexSetForAll( &(pl->plItemUsed),
					utilPagedListCleanItem, (void *)pl );
	}

    for ( page = 0; page < pl->plItemPageCount; page++ )
	{
	if  ( pl->plItemPages[page] )
	    { free( pl->plItemPages[page] );	}
	}

    if  ( pl->plItemPages )
	{ free( pl->plItemPages );	}

    utilCleanIndexSet( &(pl->plItemUsed) );

    return;
    }

typedef struct ForAllThrough
    {
    const PagedList *	fatPagedList;
    PagedListForAllFun	fatForAllFun;
    void *		fatThrough;
    } ForAllThrough;

static int utilPagedListForOne(	int		n,
				void *		vfat )
    {
    ForAllThrough *	fat= (ForAllThrough *)vfat;
    const PagedList *	pl= fat->fatPagedList;
    void *		vit= utilPagedListGetItemByNumber( pl, n );

    return fat->fatForAllFun( n, vit, fat->fatThrough );
    }

int utilPagedListForAll(	const PagedList *	pl,
				PagedListForAllFun	fun,
				void *			through )
    {
    ForAllThrough	fat;

    fat.fatPagedList= pl;
    fat.fatForAllFun= fun;
    fat.fatThrough= through;

    return utilIndexSetForAll( &(pl->plItemUsed),
					utilPagedListForOne, (void *)&fat );
    }

void * utilPagedListGetItemByNumber(	const PagedList *	pl,
					int			n )
    {
    int			page= n/ PL_PGSZ;
    unsigned char *	itemPage;

    if  ( n < 0 )
	{ LDEB(n); return (void *)0;	}

    if  ( ! utilIndexSetContains( &(pl->plItemUsed), n ) )
	{ return (void *)0;	}

    if  ( page >= pl->plItemPageCount )
	{ LLDEB(page,pl->plItemPageCount); return (void *)0;	}

    itemPage= (unsigned char *)pl->plItemPages[page];
    if  ( ! itemPage )
	{ LXDEB(page,itemPage); return (void *)0;	}

    return (void *)(itemPage+ (n % PL_PGSZ)*pl->plSizeofItem);
    }

void utilPagedListDeleteItemByNumber(	PagedList *		pl,
					int			n )
    {
    void *		vit= utilPagedListGetItemByNumber( pl, n );

    if  ( ! vit )
	{ LXDEB(n,vit);	}

    if  ( vit && pl->plCleanItem )
	{ (*pl->plCleanItem)( vit );	}

    utilIndexSetRemove( &(pl->plItemUsed), n );

    while( pl->plItemCount > 0 && pl->plItemCount == n+ 1 )
	{
	if  ( utilIndexSetContains( &(pl->plItemUsed), n ) )
	    { break;	}

	pl->plItemCount--; n--;
	}

    return;
    }

void * utilPagedListClaimItem(	PagedList *		pl,
				int			n )
    {
    int			page= n/ PL_PGSZ;
    unsigned char *	itemPage;
    void *		ret;

    if  ( n < 0 )
	{ LDEB(n); return (void *)0;	}

    if  ( page >= pl->plItemPageCount )
	{
	int	newPageCount= page+ 1;

	void **	fresh= (void **)realloc( pl->plItemPages,
				    newPageCount* sizeof(void *) );

	if  ( ! fresh )
	    { LXDEB(n,fresh); return (void *)0;	}
	pl->plItemPages= fresh;

	while( pl->plItemPageCount < newPageCount )
	    { fresh[pl->plItemPageCount++]= (void *)0;	}
	}

    itemPage= (unsigned char *)pl->plItemPages[page];
    if  ( ! itemPage )
	{
	pl->plItemPages[page]= itemPage=
			(unsigned char *)malloc( PL_PGSZ* pl->plSizeofItem );
	if  ( ! itemPage )
	    { XDEB(itemPage); return (void *)0;	}

	if  ( pl->plInitItem )
	    {
	    int		i;

	    for ( i= 0; i < PL_PGSZ; i++ )
		{ (*pl->plInitItem)( itemPage+ i* pl->plSizeofItem );	}
	    }
	}

    utilIndexSetAdd( &(pl->plItemUsed), n );

    if  ( pl->plItemCount <= n )
	{ pl->plItemCount= n+ 1;	}

    ret= (void *)(itemPage+ (n % PL_PGSZ)*pl->plSizeofItem);
    if  ( pl->plInitItem )
	{ (*pl->plInitItem)( ret );	}

    return ret;
    }

int utilPagedListSetSize(	PagedList *		pl,
				int			n )
    {
    while( pl->plItemCount < n )
	{
	if  ( ! utilPagedListClaimItem( pl, pl->plItemCount ) )
	    { LLDEB(pl->plItemCount,n); return -1;	}
	}

    while( pl->plItemCount > n )
	{
	utilPagedListDeleteItemByNumber( pl, pl->plItemCount- 1 );
	}

    return 0;
    }

void * utilPagedListClaimItemAtEnd(	int *			pN,
					PagedList *		pl )
    {
    int			n;
    void *		vit;

    n= pl->plItemCount;
    vit= utilPagedListClaimItem( pl, n );

    if  ( vit )
	{ *pN= n;	}

    return vit;
    }

void * utilPagedListClaimNewItem(	int *			pN,
					PagedList *		pl )
    {
    int			n;
    void *		vit;

    n= utilIndexSetGetNextHole( &(pl->plItemUsed), -1 );
    if  ( n < 0 )
	{ LDEB(n); return (void *)0;	}

    vit= utilPagedListClaimItem( pl, n );

    if  ( vit )
	{ *pN= n;	}

    return vit;
    }

void * utilPagedListGetNext(	int *			pN,
				const PagedList *	pl,
				int			n )
    {
    void *	v;
    int		nn= utilIndexSetGetNext( &(pl->plItemUsed), n );

    if  ( nn < 0 )
	{ return (void *)0;	}

    v= utilPagedListGetItemByNumber( pl, nn );
    if  ( ! v )
	{ XDEB(v);	}
    else{ *pN= nn;	}

    return v;
    }
