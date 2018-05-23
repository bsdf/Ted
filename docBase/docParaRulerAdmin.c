/************************************************************************/
/*									*/
/*  Manage a lookup to keep track of the different possible rulers for	*/
/*  a paragraph								*/
/*									*/
/************************************************************************/

#   include	"docBaseConfig.h"

#   include	<stdlib.h>

#   include	<appDebugon.h>

#   include	"docParaRulerAdmin.h"

/************************************************************************/

static int docGetTabStopListProperty(	const TabStopList *	tsl,
					int			prop )
    {
    if  ( prop < 0 )
	{ LDEB(prop); return -1;	}
    if  ( prop >= 1+ tsl->tslTabStopCount* TABprop_COUNT )
	{
	LLLDEB(prop,tsl->tslTabStopCount,tsl->tslTabStopCount* TABprop_COUNT);
	return 0;
	}

    if  ( prop == 0 )
	{ return tsl->tslTabStopCount;	}

    prop--;

    return docTabStopGetProperty( tsl->tslTabStops+ (prop / TABprop_COUNT),
						     prop % TABprop_COUNT );
    }

/************************************************************************/
/*									*/
/*  Initialize/Clean attribute administration.				*/
/*									*/
/************************************************************************/

void docInitTabStopListList(	NumberedPropertiesList *	tsll )
    {
    int			num;
    TabStopList		tsl;

    utilInitNumberedPropertiesList( tsll );

    utilStartNumberedPropertyList( tsll,

		    1, /* An arbitrary value */
		    (NumberedPropertiesGetProperty)docGetTabStopListProperty,

		    sizeof(TabStopList),
		    (InitPagedListItem)docInitTabStopList,
		    (CleanPagedListItem)docCleanTabStopList );

    docInitTabStopList( &tsl );

    num= docTabStopListNumberImpl( tsll, &tsl );
    if  ( num != 0 )
	{ LDEB(num);	}

    docCleanTabStopList( &tsl );

    return;
    }

/************************************************************************/
/*									*/
/*  Translate a ruler number to a value.				*/
/*									*/
/************************************************************************/

void docGetTabStopListByNumberImpl(	TabStopList *			tsl,
					const NumberedPropertiesList *	tsll,
					int				n )
    {
    void *	vtsl= utilPagedListGetItemByNumber( &(tsll->nplPagedList), n );

    if  ( ! vtsl )
	{ LXDEB(n,vtsl); docInitTabStopList( tsl ); return; }

    *tsl= *((TabStopList *)vtsl);
    return;
    }

/************************************************************************/
/*									*/
/*  Translate a text attribute to its number.				*/
/*									*/
/************************************************************************/

int docTabStopListNumberImpl(	NumberedPropertiesList *	npl,
				const TabStopList *		tsl )
    {
    const int		make= 1;

    int			prop;
    IntegerValueNode *	ivn= &(npl->nplValueNodes);
    void *		vta;

    int			propCount= 1+ tsl->tslTabStopCount* TABprop_COUNT;

    if  ( propCount < 1 )
	{ LDEB(propCount); return -1;	}
    if  ( ! npl->nplGetProperty )
	{ XDEB(npl->nplGetProperty); return -1;	}

    for ( prop= 0; prop < propCount; prop++ )
	{
	int	propval= (*npl->nplGetProperty)( tsl, prop );

	ivn= utilChildIntegerValueNode( ivn, make, propval );
	if  ( ! ivn )
	    {
	    if  ( make )
		{ LLXDEB(propval,make,ivn);	}
	    return -1;
	    }
	}

    if  ( ivn->ivnIsLeaf )
	{ return ivn->ivnReference;	}

    vta= utilPagedListClaimItemAtEnd( &(ivn->ivnReference),
						    &(npl->nplPagedList) );
    if  ( ! vta )
	{ XDEB(vta); return -1;	}

    if  ( docCopyTabStopList( (TabStopList *)vta, tsl ) )
	{ LDEB(tsl->tslTabStopCount); return -1;	}
    ivn->ivnIsLeaf= 1;

    return ivn->ivnReference;
    }

/************************************************************************/
/*									*/
/*  Merge ruler lists and create a mapping.				*/
/*									*/
/************************************************************************/

int docMergeTabstopListLists(	int **				pRulerMap,
				NumberedPropertiesList *	tsllTo,
				const NumberedPropertiesList *	tsllFrom )
    {
    int		fromCount= tsllFrom->nplPagedList.plItemCount;

    if  ( fromCount > 0 )
	{
	int		fr;
	int *		rmap= (int *)malloc( fromCount* sizeof(int) );

	if  ( ! rmap )
	    { LXDEB(fromCount,rmap); return -1; }

	for ( fr= 0; fr < fromCount; fr++ )
	    { rmap[fr]= -1;	}

	for ( fr= 0; fr < fromCount; fr++ )
	    {
	    int				to;
	    void *			vtsl;
	    TabStopList			tsl;

	    vtsl= utilPagedListGetItemByNumber( &(tsllFrom->nplPagedList), fr );
	    if  ( ! vtsl )
		{ continue;	}

	    tsl= *((TabStopList *)vtsl);

	    to= docTabStopListNumberImpl( tsllTo, &tsl );
	    if  ( to < 0 )
		{ LDEB(to); free( rmap ); return -1;	}
	    rmap[fr]= to;
	    }

	if  ( pRulerMap )
	    { *pRulerMap= rmap;		}
	else{ free( rmap );		}
	}

    return 0;
    }

