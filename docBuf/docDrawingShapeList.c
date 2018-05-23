/************************************************************************/
/*									*/
/*  Manage a list of shapes.						*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	<stdlib.h>

#   include	<appDebugon.h>

#   include	"docDrawingShapeList.h"
#   include	"docShape.h"

struct DrawingShape * docGetShapeByNumber(
					const DrawingShapeList *	dsl,
					int				n )
    {
    void *	vdf= utilPagedListGetItemByNumber( &(dsl->dslPagedList), n );

    return (struct DrawingShape *)vdf;
    }

void docDeleteShapeFromList(	DrawingShapeList *		dsl,
				struct DrawingShape *		ds )
    {
    int		n= ds->dsShapeNumber;

    if  ( docGetShapeByNumber( dsl, n ) != ds )
	{ LDEB(n);	}

    utilPagedListDeleteItemByNumber( &(dsl->dslPagedList), ds->dsShapeNumber );

    return;
    }

struct DrawingShape * docClaimDrawingShape(	DrawingShapeList *	dsl )
    {
    void *			vdf;
    int				n;
    struct DrawingShape *	ds;

    vdf= utilPagedListClaimNewItem( &n, &(dsl->dslPagedList) );

    if  ( ! vdf )
	{ PDEB(vdf); return (struct DrawingShape *)0;	}

    ds= (struct DrawingShape *)vdf;
    ds->dsShapeNumber= n;

    return ds;
    }

void docInitShapeList(	DrawingShapeList *	dsl )
    {
    utilInitPagedList( &(dsl->dslPagedList) );
    utilStartPagedList( &(dsl->dslPagedList), sizeof(DrawingShape),
			    (InitPagedListItem)docInitDrawingShape,
			    (CleanPagedListItem)docCleanDrawingShape );
    return;
    }

void docCleanShapeList(	DrawingShapeList *	dsl )
    { utilCleanPagedList( &(dsl->dslPagedList) );	}

/************************************************************************/
/*									*/
/*  Add a new child to a parent.					*/
/*									*/
/************************************************************************/

DrawingShape * docClaimShapeInParent(	DrawingShapeList *	dsl,
					DrawingShape *		parent,
					int			n,
					int			kind )
    {
    int			i;

    int			newSize;

    DrawingShape **	freshChildren;
    DrawingShape *	ds;

    if  ( ! parent )
	{
	ds= docClaimDrawingShape( dsl );
	if  ( ! ds )
	    { XDEB(ds); return ds;	}

	ds->dsDrawing.sdShapeType= kind;

	return ds;
	}

    if  ( n == -1 )
	{ n= parent->dsChildCount;	}

    newSize= parent->dsChildCount+ 1;
    newSize *= sizeof(DrawingShape *);

    freshChildren= (DrawingShape **)realloc( parent->dsChildren, newSize );
    if  ( ! freshChildren )
	{ LXDEB(newSize,freshChildren); return (DrawingShape *)0; }
    parent->dsChildren= freshChildren;

    ds= docClaimDrawingShape( dsl );
    if  ( ! ds )
	{ XDEB(ds); return ds;	}

    if  ( n == -1 )
	{ n= parent->dsChildCount; }

    ds->dsDrawing.sdShapeType= kind;

    for ( i= parent->dsChildCount; i > n; i-- )
	{ freshChildren[i]= freshChildren[i-1];	}

    freshChildren[n]= ds;
    parent->dsChildCount++;

    ds->dsIsChildShape= 1;

    return ds;
    }

