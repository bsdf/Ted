/************************************************************************/
/*									*/
/*  Keep a collection TextAttributes by number.				*/
/*									*/
/************************************************************************/

#   include	"docBaseConfig.h"

#   include	<stdlib.h>

#   include	<appDebugon.h>

#   include	"docBorderPropertyAdmin.h"

/************************************************************************/
/*									*/
/*  Initialize/Clean border administration.				*/
/*									*/
/************************************************************************/

void docInitBorderPropertyList(	NumberedPropertiesList *	bpl )
    {
    int			num;
    BorderProperties	bp;

    utilInitNumberedPropertiesList( bpl );

    utilStartNumberedPropertyList( bpl,

		    BRDRprop_COUNT,
		    (NumberedPropertiesGetProperty)docGetBorderProperty,

		    sizeof(BorderProperties),
		    (InitPagedListItem)docInitBorderProperties,
		    (CleanPagedListItem)0 );

    docInitBorderProperties( &bp );

    num= docBorderPropertiesNumberImpl( bpl, &bp );
    if  ( num != 0 )
	{ LDEB(num);	}

    return;
    }

/************************************************************************/
/*									*/
/*  Translate a border properties number to a struct value.		*/
/*									*/
/************************************************************************/

void docGetBorderPropertiesByNumberImpl( BorderProperties *		bp,
					const NumberedPropertiesList *	bpl,
					int				n )
    {
    void *	vbp= utilPagedListGetItemByNumber( &(bpl->nplPagedList), n );

    if  ( ! vbp )
	{ LXDEB(n,vbp); docInitBorderProperties( bp ); return; }

    *bp= *((BorderProperties *)vbp);
    return;
    }

/************************************************************************/
/*									*/
/*  Is this number really a border?					*/
/*									*/
/************************************************************************/

int docBorderNumberIsBorderImpl(	const NumberedPropertiesList *	bpl,
					int				n )
    {
    BorderProperties	bp;

    if  ( n < 0 )
	{ return 0;	}

    docGetBorderPropertiesByNumberImpl( &bp, bpl, n );

    return DOCisBORDER( &bp );
    }

/************************************************************************/
/*									*/
/*  Call a function for all BorderProperties in the list.		*/
/*									*/
/************************************************************************/

void docForAllBorderProperties(	const NumberedPropertiesList *	bpl,
				BorderPropertiesFunction	f,
				void *				through )
    {
    int			n;
    const PagedList *	pl= &(bpl->nplPagedList);

    for ( n= 0; n < pl->plItemCount; n++ )
	{
	void *      vbp= utilPagedListGetItemByNumber( pl, n );

	(*f)( (BorderProperties *)vbp, n, through );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Translate border properties to a number.				*/
/*									*/
/************************************************************************/

int docBorderPropertiesNumberImpl( NumberedPropertiesList *		bpl,
				const BorderProperties *		bp )
    {
    const int	make= 1;

    return utilGetPropertyNumber( bpl, make, (void *)bp );
    }

int docMergeBorderPropertiesLists(
				int **				pBorderMap,
				const int *			colorMap,
				NumberedPropertiesList *	bplTo,
				const NumberedPropertiesList *	bplFrom )
    {
    int		fromCount= bplFrom->nplPagedList.plItemCount;

    if  ( fromCount > 0 )
	{
	int		n;
	int *		borderMap= (int *)malloc( fromCount* sizeof(int) );

	if  ( ! borderMap )
	    { LXDEB(fromCount,borderMap); return -1; }

	for ( n= 0; n < fromCount; n++ )
	    { borderMap[n]= -1;	}

	for ( n= 0; n < fromCount; n++ )
	    {
	    int			to;
	    void *      	vbp;
	    BorderProperties	bp;

	    vbp= utilPagedListGetItemByNumber( &(bplFrom->nplPagedList), n );
	    if  ( ! vbp )
		{ continue;	}

	    bp= *((BorderProperties *)vbp);
	    if  ( bp.bpColor > 0 && colorMap )
		{ bp.bpColor= colorMap[bp.bpColor];	}

	    to= docBorderPropertiesNumberImpl( bplTo, &bp );
	    if  ( to < 0 )
		{ LDEB(to); free( borderMap ); return -1;	}
	    borderMap[n]= to;
	    }

	*pBorderMap= borderMap;
	}

    return 0;
    }

