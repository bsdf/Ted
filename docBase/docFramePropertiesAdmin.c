/************************************************************************/
/*									*/
/*  Keep a collection TextAttributes by number.				*/
/*									*/
/************************************************************************/

#   include	"docBaseConfig.h"

#   include	<stdlib.h>

#   include	<appDebugon.h>

#   include	"docFramePropertiesAdmin.h"

/************************************************************************/
/*									*/
/*  Initialize/Clean attribute administration.				*/
/*									*/
/************************************************************************/

void docInitFramePropertyList(	NumberedPropertiesList *	fpl )
    {
    int			num;
    FrameProperties	fp;

    utilInitNumberedPropertiesList( fpl );

    utilStartNumberedPropertyList( fpl,

		    TFPprop_COUNT,
		    (NumberedPropertiesGetProperty)docGetFrameProperty,

		    sizeof(FrameProperties),
		    (InitPagedListItem)docInitFrameProperties,
		    (CleanPagedListItem)0 );

    docInitFrameProperties( &fp );

    num= docFramePropertiesNumberImpl( fpl, &fp );
    if  ( num != 0 )
	{ LDEB(num);	}

    return;
    }

/************************************************************************/
/*									*/
/*  Translate a frame properties number to a struct value.		*/
/*									*/
/************************************************************************/

void docGetFramePropertiesByNumberImpl(	FrameProperties *		fp,
					const NumberedPropertiesList *	fpl,
					int				n )
    {
    void *	vfp= utilPagedListGetItemByNumber( &(fpl->nplPagedList), n );

    if  ( ! vfp )
	{ LXDEB(n,vfp); docInitFrameProperties( fp ); return; }

    *fp= *((FrameProperties *)vfp);
    return;
    }

/************************************************************************/
/*									*/
/*  Is this number really a border?					*/
/*									*/
/************************************************************************/

int docFrameNumberIsFrameImpl(		const NumberedPropertiesList *	fpl,
					int				n )
    {
    FrameProperties	fp;

    if  ( n < 0 )
	{ return 0;	}

    docGetFramePropertiesByNumberImpl( &fp, fpl, n );

    return DOCisFRAME( &fp );
    }

/************************************************************************/
/*									*/
/*  Call a function for all FrameProperties in the list.		*/
/*									*/
/************************************************************************/

void docForAllFrameProperties(	const NumberedPropertiesList *	fpl,
				FramePropertiesFunction		f,
				void *				through )
    {
    int			n;

    for ( n= 0; n < fpl->nplPagedList.plItemCount; n++ )
	{
	void *      vbp= utilPagedListGetItemByNumber(
						&(fpl->nplPagedList), n );

	(*f)( (FrameProperties *)vbp, n, through );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Translate a text attribute to its number.				*/
/*									*/
/************************************************************************/

int docFramePropertiesNumberImpl(	NumberedPropertiesList *	fpl,
					const FrameProperties *		fp )
    {
    const int	make= 1;

    return utilGetPropertyNumber( fpl, make, (void *)fp );
    }

int docMergeFramePropertyLists(	int **				pFrameMap,
				NumberedPropertiesList *	fplTo,
				const NumberedPropertiesList *	fplFrom )
    {
    int		fromCount= fplFrom->nplPagedList.plItemCount;

    if  ( fromCount > 0 )
	{
	int		n;
	int *		frameMmap= (int *)malloc( fromCount* sizeof(int) );

	if  ( ! frameMmap )
	    { LXDEB(fromCount,frameMmap); return -1; }

	for ( n= 0; n < fromCount; n++ )
	    { frameMmap[n]= -1;	}

	for ( n= 0; n < fromCount; n++ )
	    {
	    int				to;
	    void *			vfp;
	    FrameProperties		fp;

	    vfp= utilPagedListGetItemByNumber( &(fplFrom->nplPagedList), n );
	    if  ( ! vfp )
		{ continue;	}

	    fp= *((FrameProperties *)vfp);

	    to= docFramePropertiesNumberImpl( fplTo, &fp );
	    if  ( to < 0 )
		{ LDEB(to); free( frameMmap ); return -1;	}
	    frameMmap[n]= to;
	    }

	*pFrameMap= frameMmap;
	}

    return 0;
    }

