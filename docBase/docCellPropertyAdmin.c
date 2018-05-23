/************************************************************************/
/*									*/
/*  Keep a collection TextAttributes by number.				*/
/*									*/
/************************************************************************/

#   include	"docBaseConfig.h"

#   include	<stdlib.h>

#   include	<appDebugon.h>

#   include	"docCellPropertyAdmin.h"

/************************************************************************/
/*									*/
/*  Initialize/Clean border administration.				*/
/*									*/
/************************************************************************/

void docInitCellPropertyList(	NumberedPropertiesList *	cpl )
    {
    int			num;
    CellProperties	cp;

    utilInitNumberedPropertiesList( cpl );

    utilStartNumberedPropertyList( cpl,

		    CLprop_COUNT,
		    (NumberedPropertiesGetProperty)docGetCellProperty,

		    sizeof(CellProperties),
		    (InitPagedListItem)docInitCellProperties,
		    (CleanPagedListItem)0 );

    docInitCellProperties( &cp );

    num= docCellPropertiesNumber( cpl, &cp );
    if  ( num != 0 )
	{ LDEB(num);	}

    return;
    }

/************************************************************************/
/*									*/
/*  Translate a border properties number to a struct value.		*/
/*									*/
/************************************************************************/

void docGetCellPropertiesByNumber(	CellProperties *		cp,
					const NumberedPropertiesList *	cpl,
					int				n )
    {
    void *	vcp= utilPagedListGetItemByNumber( &(cpl->nplPagedList), n );

    if  ( ! vcp )
	{ LXDEB(n,vcp); docInitCellProperties( cp ); return; }

    *cp= *((CellProperties *)vcp);
    return;
    }

/************************************************************************/
/*									*/
/*  Call a function for all CellProperties in the list.		*/
/*									*/
/************************************************************************/

void docForAllCellProperties(	const NumberedPropertiesList *	cpl,
				CellPropertiesFunction	f,
				void *				through )
    {
    int			n;
    const PagedList *	pl= &(cpl->nplPagedList);

    for ( n= 0; n < pl->plItemCount; n++ )
	{
	void *      vcp= utilPagedListGetItemByNumber( pl, n );

	(*f)( (CellProperties *)vcp, n, through );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Translate border properties to a number.				*/
/*									*/
/************************************************************************/

int docCellPropertiesNumber(	NumberedPropertiesList *		cpl,
				const CellProperties *		cp )
    {
    const int	make= 1;

    return utilGetPropertyNumber( cpl, make, (void *)cp );
    }

int docMergeCellPropertiesLists(
				int **				pCellMap,
				const int *			borderMap,
				const int *			shadingMap,
				NumberedPropertiesList *	cplTo,
				const NumberedPropertiesList *	cplFrom )
    {
    int		fromCount= cplFrom->nplPagedList.plItemCount;

    if  ( fromCount > 0 )
	{
	int		n;
	int *		cellMmap= (int *)malloc( fromCount* sizeof(int) );

	if  ( ! cellMmap )
	    { LXDEB(fromCount,cellMmap); return -1; }

	for ( n= 0; n < fromCount; n++ )
	    { cellMmap[n]= -1;	}

	for ( n= 0; n < fromCount; n++ )
	    {
	    int			to;
	    void *      	vcp;
	    CellProperties	cp;

	    vcp= utilPagedListGetItemByNumber( &(cplFrom->nplPagedList), n );
	    if  ( ! vcp )
		{ continue;	}

	    cp= *((CellProperties *)vcp);
	    if  ( cp.cpTopBorderNumber > 0 && borderMap )
		{ cp.cpTopBorderNumber= borderMap[cp.cpTopBorderNumber]; }
	    if  ( cp.cpLeftBorderNumber > 0 && borderMap )
		{ cp.cpLeftBorderNumber= borderMap[cp.cpLeftBorderNumber]; }
	    if  ( cp.cpRightBorderNumber > 0 && borderMap )
		{ cp.cpRightBorderNumber= borderMap[cp.cpRightBorderNumber]; }
	    if  ( cp.cpBottomBorderNumber > 0 && borderMap )
		{ cp.cpBottomBorderNumber= borderMap[cp.cpBottomBorderNumber]; }

	    if  ( cp.cpShadingNumber > 0 && shadingMap )
		{ cp.cpShadingNumber= shadingMap[cp.cpShadingNumber]; }

	    to= docCellPropertiesNumber( cplTo, &cp );
	    if  ( to < 0 )
		{ LDEB(to); free( cellMmap ); return -1;	}
	    cellMmap[n]= to;
	    }

	*pCellMap= cellMmap;
	}

    return 0;
    }

