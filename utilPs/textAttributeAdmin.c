/************************************************************************/
/*									*/
/*  Keep a collection TextAttributes by number.				*/
/*									*/
/************************************************************************/

#   include	"utilPsConfig.h"

#   include	<appDebugon.h>

#   include	"textAttributeAdmin.h"

/************************************************************************/
/*									*/
/*  Initialize/Clean attribute administration.				*/
/*									*/
/************************************************************************/

void utilInitTextAttributeList(	NumberedPropertiesList *	tal )
    {
    int			num;
    TextAttribute	ta;

    utilInitNumberedPropertiesList( tal );

    utilStartNumberedPropertyList( tal,

		    TAprop_COUNT,
		    (NumberedPropertiesGetProperty)utilGetTextProperty,

		    sizeof(TextAttribute),
		    (InitPagedListItem)utilInitTextAttribute,
		    (CleanPagedListItem)0 );

    utilInitTextAttribute( &ta );
    ta.taFontNumber= 0;

    num= utilTextAttributeNumber( tal, &ta );
    if  ( num != 0 )
	{ LDEB(num);	}

    return;
    }

/************************************************************************/
/*									*/
/*  Translate a text attribute number to a struct value.		*/
/*									*/
/************************************************************************/

void utilGetTextAttributeByNumber(	TextAttribute *			ta,
					const NumberedPropertiesList *	tal,
					int				n )
    {
    void *	vta= utilPagedListGetItemByNumber( &(tal->nplPagedList), n );

    if  ( ! vta )
	{ LXDEB(n,vta); utilInitTextAttribute( ta ); return; }

    *ta= *((TextAttribute *)vta);
    return;
    }

/************************************************************************/
/*									*/
/*  Call a function for all TextAttributes in the list.			*/
/*									*/
/************************************************************************/

int textForAllAttributesInList(
				const NumberedPropertiesList *	tal,
				const IndexSet *		filter,
				TextAttributeFunction		f,
				void *				through )
    {
    int			n;
    const PagedList *	pl= &(tal->nplPagedList);

    for ( n= 0; n < pl->plItemCount; n++ )
	{
	void *      vta;

	if  ( ! utilIndexSetContains( &(pl->plItemUsed), n ) )
	    { continue;	}
	if  ( filter && ! utilIndexSetContains( filter, n ) )
	    { continue;	}

	vta= utilPagedListGetItemByNumber( &(tal->nplPagedList), n );

	if  ( (*f)( n, (TextAttribute *)vta, through ) )
	    { LDEB(n); return -1;	}
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Translate a text attribute to its number.				*/
/*									*/
/************************************************************************/

int utilTextAttributeNumber(	NumberedPropertiesList *	tal,
				const TextAttribute *		ta )
    {
    const int	make= 1;

    return utilGetPropertyNumber( tal, make, (void *)ta );
    }
