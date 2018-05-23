/************************************************************************/
/*									*/
/*  Buffer administration: manage embedded objects.			*/
/*									*/
/************************************************************************/

#   include	"docBaseConfig.h"

#   include	<appDebugon.h>

#   include	"docInsertedObjectList.h"
#   include	"docObject.h"

InsertedObject * docClaimInsertedObject(	int *			pNr,
						InsertedObjectList *	iol )
    {
    InsertedObject *	io;
    int			nr;

    io= (InsertedObject *)utilPagedListClaimNewItem( &nr,
							&(iol->iolPagedList) );
    if  ( ! io )
	{ PDEB(io); return io;	}

    *pNr= nr; return io;
    }

InsertedObject * docGetObjectByNumber(
					const InsertedObjectList *	iol,
					int				n )
    {
    InsertedObject *	io;

    io= (InsertedObject *)utilPagedListGetItemByNumber(
						    &(iol->iolPagedList), n );
    if  ( ! io )
	{ LPDEB(n,io);	}

    return io;
    }

void docInitObjectList(			InsertedObjectList *		iol )
    {
    utilInitPagedList( &(iol->iolPagedList) );

    utilStartPagedList( &(iol->iolPagedList),
			    sizeof(InsertedObject),
			    (InitPagedListItem)docInitInsertedObject,
			    (CleanPagedListItem)docCleanInsertedObject );
    }

void docCleanObjectList(	InsertedObjectList *		iol )
    {
    utilCleanPagedList( &(iol->iolPagedList) );
    }
