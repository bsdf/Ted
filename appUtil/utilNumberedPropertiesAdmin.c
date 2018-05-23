/************************************************************************/
/*									*/
/*  Keep a collection of objects with integer properties by number.	*/
/*									*/
/************************************************************************/

#   include	"appUtilConfig.h"

#   include	<string.h>

#   include	<appDebugon.h>

#   include	"utilNumberedPropertiesAdmin.h"

/************************************************************************/
/*									*/
/*  Initialize/Clean property administration.				*/
/*									*/
/************************************************************************/

void utilInitNumberedPropertiesList(	NumberedPropertiesList *	npl )
    {
    utilInitPagedList( &(npl->nplPagedList) );
    utilInitIntegerValueNode( &(npl->nplValueNodes) );

    npl->nplPropCount= 0;
    npl->nplGetProperty= (NumberedPropertiesGetProperty)0;
    }

void utilCleanNumberedPropertiesList(	NumberedPropertiesList *	npl )
    {
    utilCleanPagedList( &(npl->nplPagedList) );
    utilCleanIntegerValueNode( &(npl->nplValueNodes) );

    return;
    }

/************************************************************************/
/*									*/
/*  Initialise an administration.					*/
/*									*/
/************************************************************************/

void utilStartNumberedPropertyList(
				NumberedPropertiesList * 	npl,
				int				propCount,
				NumberedPropertiesGetProperty	getProp,
				int				sizeofItem,
				InitPagedListItem		initItem,
				CleanPagedListItem	 	cleanItem )
    {
    npl->nplPropCount= propCount;
    npl->nplGetProperty= getProp;

    utilStartPagedList( &(npl->nplPagedList), sizeofItem, initItem, cleanItem );
    }

/************************************************************************/
/*									*/
/*  Call a function for all properties in the list.			*/
/*									*/
/************************************************************************/

void utilForAllNumberedProperties(
				const NumberedPropertiesList *	npl,
				NumberedPropertiesFunction	f,
				void *				through )
    {
    int			n;

    for ( n= 0; n < npl->nplPagedList.plItemCount; n++ )
	{
	void *      vta= utilPagedListGetItemByNumber(
						&(npl->nplPagedList), n );

	if  ( ! vta )
	    { continue;	}

	(*f)( vta, n, through );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Translate an instance to its number.				*/
/*									*/
/************************************************************************/

int utilGetPropertyNumber(	NumberedPropertiesList *	npl,
				int				make,
				const void *			vob )
    {
    int			prop;
    IntegerValueNode *	ivn= &(npl->nplValueNodes);
    void *		vta;

    if  ( npl->nplPropCount < 1 )
	{ LDEB(npl->nplPropCount); return -1;	}
    if  ( ! npl->nplGetProperty )
	{ XDEB(npl->nplGetProperty); return -1;	}

    for ( prop= 0; prop < npl->nplPropCount; prop++ )
	{
	int	propval= (*npl->nplGetProperty)( vob, prop );

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

    memcpy( vta, vob, npl->nplPagedList.plSizeofItem );
    ivn->ivnIsLeaf= 1;

    return ivn->ivnReference;
    }

