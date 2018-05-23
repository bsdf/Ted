/************************************************************************/
/*									*/
/*  Management of the list table of a document.				*/
/*									*/
/************************************************************************/

#   include	"docBaseConfig.h"

#   include	<stdlib.h>

#   include	<appDebugon.h>

#   include	"docListOverride.h"
#   include	"docListOverrideTable.h"

void docInitListOverrideTable(		ListOverrideTable *		lot )
    {
    lot->lotOverrides= (ListOverride *)0;
    lot->lotOverrideCount= 0;

    return;
    }

void docCleanListOverrideTable(		ListOverrideTable *		lot )
    {
    int		i;

    for ( i= 0; i < lot->lotOverrideCount; i++ )
	{ docCleanListOverride( &(lot->lotOverrides[i]) );	}

    if  ( lot->lotOverrides )
	{ free( (void *)lot->lotOverrides );	}

    return;
    }

int docCopyListOverrideTable(	ListOverrideTable *		to,
				const ListOverrideTable *	from )
    {
    int			i;

    if  ( to->lotOverrideCount > from->lotOverrideCount )
	{
	for ( i= from->lotOverrideCount; i < to->lotOverrideCount; i++ )
	    { docCleanListOverride( &(to->lotOverrides[i]) );	}

	to->lotOverrideCount= from->lotOverrideCount;
	}

    if  ( to->lotOverrideCount < from->lotOverrideCount )
	{
	ListOverride *	fresh;

	fresh= (ListOverride *)realloc( to->lotOverrides,
				from->lotOverrideCount* sizeof(ListOverride) );
	if  ( ! fresh )
	    { LXDEB(from->lotOverrideCount,fresh); return -1;	}

	to->lotOverrides= fresh;

	while( to->lotOverrideCount < from->lotOverrideCount )
	    {
	    docInitListOverride( &(to->lotOverrides[to->lotOverrideCount++]) );
	    }
	}

    for ( i= 0; i < from->lotOverrideCount; i++ )
	{
	if  ( docCopyListOverrideSameDocument( &(to->lotOverrides[i]),
						&(from->lotOverrides[i]) ) )
	    { LDEB(i); return -1;	}
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Add a list to the listtable.					*/
/*									*/
/************************************************************************/

int docListOverrideTableAddOverride(	ListOverride **		pLo,
					ListOverrideTable *	lot,
					int			ls,
					int			listId,
					int			listIndex )
    {
    if  ( ls < 0 )
	{ ls= lot->lotOverrideCount;	}
    if  ( ls < 1 )
	{ ls=  1;			}

    if  ( ls >= lot->lotOverrideCount )
	{
	ListOverride *	fresh;

	fresh= (ListOverride *)realloc( lot->lotOverrides,
				    (ls+ 1)* sizeof(ListOverride) );
	if  ( ! fresh )
	    { LXDEB(ls,fresh); return -1;	}

	lot->lotOverrides= fresh;

	fresh += lot->lotOverrideCount;
	while( lot->lotOverrideCount <= ls )
	    {
	    docInitListOverride( fresh );
	    lot->lotOverrideCount++; fresh++;
	    }
	}

    lot->lotOverrides[ls].loIndex= ls;
    lot->lotOverrides[ls].loListID= listId;
    lot->lotOverrides[ls].loListIndex= listIndex;

    if  ( pLo )
	{ *pLo= lot->lotOverrides+ ls;	}

    return ls;
    }

int docMergeListOverrideIntoTable(	ListOverrideTable *	lot,
					const ListOverride *	loSet,
					const int *		fontMap,
					const int *		colorMap,
					const int *		rulerMap )
    {
    int			ls;
    ListOverride *	lo;

    if  ( loSet->loIndex > 0						&&
	  loSet->loIndex < lot->lotOverrideCount			&&
	  lot->lotOverrides[loSet->loIndex].loIndex ==
						    loSet->loIndex	&&
	  lot->lotOverrides[loSet->loIndex].loListID ==
						    loSet->loListID	)
	{ return loSet->loIndex;	}

    ls= docListOverrideTableAddOverride( &lo, lot, -1,
					loSet->loListID, loSet->loListIndex );
    if  ( ls < 0 )
	{ LLDEB(ls,lot->lotOverrideCount); return -1;	}

    if  ( docCopyListOverride( lo, loSet, fontMap, colorMap, rulerMap ) )
	{ LDEB(ls); return -1;	}

    return ls;
    }

int docListOverrideTableSetOverride(
				ListOverrideTable *		lot,
				const ListOverride *		loSet,
				const int *			fontMap,
				const int *			colorMap,
				const int *			rulerMap )
    {
    int			ls= loSet->loIndex;
    ListOverride *	lo;

    if  ( ls >= lot->lotOverrideCount )
	{
	ls= docListOverrideTableAddOverride( &lo, lot,
				    ls, loSet->loListID, loSet->loListIndex );
	if  ( ls < 0 )
	    { LLDEB(ls,lot->lotOverrideCount); return -1;	}
	}
    else{
	lo= lot->lotOverrides+ ls;
	}

    if  ( docCopyListOverride( lo, loSet, fontMap, colorMap, rulerMap ) )
	{ LDEB(ls); return -1;	}

    return ls;
    }

/************************************************************************/
/*									*/
/*  delete a list from the table.					*/
/*									*/
/************************************************************************/

int docListOverrideTableDeleteOverride(	ListOverrideTable *	lot,
					int			ls )
    {
    if  ( ls < 0 || ls >= lot->lotOverrideCount )
	{ LLDEB(ls,lot->lotOverrideCount); return -1;	}

    docCleanListOverride( lot->lotOverrides+ ls );
    docInitListOverride( lot->lotOverrides+ ls );

    return 0;
    }

