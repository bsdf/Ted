/************************************************************************/
/*									*/
/*  Map Unicode values to glyph numbers.				*/
/*									*/
/************************************************************************/

#   include	"appUtilConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>

#   include	"utilIndexMapping.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Bookkeeping.							*/
/*									*/
/************************************************************************/

void utilInitIndexMapping(	IndexMapping *	im )
    {
    im->imPages= (int **)0;
    im->imPageCount= 0;
    }

void utilCleanIndexMapping(	IndexMapping *	im )
    {
    int		i;

    for ( i= 0; i < im->imPageCount; i++ )
	{
	if  ( im->imPages[i] )
	    { free( im->imPages[i] );	}
	}

    if  ( im->imPages )
	{ free( im->imPages );	}
    }

void utilClearIndexMapping(	IndexMapping *	im )
    {
    utilCleanIndexMapping( im );
    utilInitIndexMapping( im );
    }

/************************************************************************/
/*									*/
/*  Map a unicode value to a glyph index.				*/
/*									*/
/************************************************************************/

int  utilIndexMappingPut(	IndexMapping *		im,
				int			from,
				int			to )
    {
    int		page;
    int		idx;

    if  ( from < 0 || to < 0 || from >= 256* 256 || to >= 256* 256 )
	{ XLDEB(from,to); return -1;	}

    page= from/ TGM_PSZ;
    idx=  from% TGM_PSZ;

    if  ( page >= im->imPageCount )
	{
	int **		pages;

	pages= (int **)realloc( im->imPages, (page+ 1)* sizeof(int *) );
	if  ( ! pages )
	    { LXDEB(page,pages); return -1;	}
	im->imPages= pages;

	while( im->imPageCount <= page )
	    { im->imPages[im->imPageCount++]= (int *)0; }
	}

    if  ( ! im->imPages[page] )
	{
	int		i;
	int *		fresh;

	fresh= (int *)malloc( TGM_PSZ* sizeof(int) );
	if  ( ! fresh )
	    { XDEB(fresh); return -1;	}

	for ( i= 0; i < TGM_PSZ; i++ )
	    { fresh[i]= -1;	}

	im->imPages[page]= fresh;
	}

    im->imPages[page][idx]= to;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Add a mapping at the end of the mapping.				*/
/*									*/
/************************************************************************/

int utilIndexMappingAppend(	IndexMapping *		im,
				int *			pFrom,
				int			to )
    {
    int		from= TGM_PSZ* im->imPageCount;
    int		page;

    for ( page= im->imPageCount- 1; page >= 0; page-- )
	{
	const int *	ints= im->imPages[page];
	int		idx= 0;

	if  ( ! ints )
	    { from= TGM_PSZ* page; continue;	}

	for ( idx= TGM_PSZ- 1; idx >= 0; idx-- )
	    {
	    if  ( ints[idx] >= 0 )
		{ break;	}

	    from= TGM_PSZ* page+ idx;
	    }

	if  ( idx >= 0 )
	    { break;	}
	}

    if  ( utilIndexMappingPut( im, from, to ) )
	{ LLDEB(from,to); return -1;	}

    if  ( pFrom )
	{ *pFrom= from;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Construct a backward mapping for an array of unicode code points.	*/
/*									*/
/************************************************************************/

int utilIndexMappingBuildBackward(	IndexMapping *		im,
					const int *		forward,
					int			forwardCount )
    {
    int		g;

    for ( g= 0; g < forwardCount; g++ )
	{
	if  ( forward[g] >= 0				&&
	      utilIndexMappingPut( im, forward[g], g )	)
	    { LLDEB(g,forward[g]);	}
	}

    return 0;
    }

int utilIndexMappingForAll(	const IndexMapping *	im,
				IndexMappingForOne	forOne,
				void *			through )
    {
    int		page;

    for ( page= 0; page < im->imPageCount; page++ )
	{
	const int *	ints= im->imPages[page];
	int		idx;

	if  ( ! ints )
	    { continue;	}

	for ( idx= 0; idx < TGM_PSZ; ints++, idx++ )
	    {
	    if  ( *ints >= 0 && (*forOne)( TGM_PSZ*page+ idx, *ints, through ) )
		{ return -1;	}
	    }
	}

    return 0;
    }
