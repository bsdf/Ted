#   include	"docLayoutConfig.h"

#   include	"docDraw.h"

#   include	<appDebugon.h>
#   include	<docObjectProperties.h>
#   include	<docShape.h>

/************************************************************************/

typedef struct DrawPageShapes
    {
    DrawingContext *	dpsDrawingContext;
    void *		dpsThrough;
    int			dpsPage;
    int			dpsBelowText;
    } DrawPageShapes;

/************************************************************************/
/*									*/
/*  Draw the shape that belongs to an object if it is on the current	*/
/*  page.								*/
/*									*/
/************************************************************************/

static int docDrawShapesVisitObject(	int		n,
					void *		vio,
					void *		vdps )
    {
    InsertedObject *		io= (InsertedObject *)vio;
    const DrawPageShapes *	dps= (const DrawPageShapes *)vdps;
    DrawingContext *		dc= dps->dpsDrawingContext;
    const LayoutContext *	lc= &(dc->dcLayoutContext);

    DrawingShape *		ds;
    const ShapeProperties *	sp;

    struct BufferItem *		bodySectNode;

    if  ( io->ioKind != DOCokDRAWING_SHAPE		||
	  io->ioY0Position.lpPage != dps->dpsPage	)
	{ return 0;	}

    ds= io->ioDrawingShape;
    sp= &(ds->dsShapeProperties);

    if  ( sp->spShapeBelowText != dps->dpsBelowText )
	{ return 0;	}

    bodySectNode= docGetBodySectNodeOfScope( &(ds->dsSelectionScope),
							    lc->lcDocument );
    if  ( ! bodySectNode )
	{ XDEB(bodySectNode); return -1;	}

    if  ( docDrawShape( dc, dps->dpsThrough, bodySectNode, io ) )
	{ LDEB(n);	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Draw the shapes for this page					*/
/*									*/
/************************************************************************/

int docDrawShapesForPage(		void *			through,
					DrawingContext *	dc,
					int			belowText,
					int			page )
    {
    const LayoutContext *	lc= &(dc->dcLayoutContext);
    BufferDocument *		bd= lc->lcDocument;

    DrawPageShapes		dps;

    dps.dpsDrawingContext= dc;
    dps.dpsThrough= through;
    dps.dpsPage= page;
    dps.dpsBelowText= belowText;

    if  ( utilPagedListForAll( &(bd->bdObjectList.iolPagedList),
					docDrawShapesVisitObject, &dps ) < 0 )
	{ LDEB(page); return -1;	}

    return 0;
    }

