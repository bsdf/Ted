/************************************************************************/
/*									*/
/*  A list of shapes.							*/
/*									*/
/************************************************************************/

#   ifndef DOC_DOCUMENT_SHAPE_LIST_H
#   define DOC_DOCUMENT_SHAPE_LIST_H

#   include	<utilPagedList.h>

typedef struct DrawingShapeList
    {
    PagedList		dslPagedList;
    } DrawingShapeList;

extern struct DrawingShape * docGetShapeByNumber(
					const DrawingShapeList *	dsl,
					int				n );

extern void docDeleteShapeFromList(	DrawingShapeList *		dsl,
					struct DrawingShape *		ds );

extern void docCleanShapeList(		DrawingShapeList *	dsl );
extern void docInitShapeList(		DrawingShapeList *	dsl );

extern struct DrawingShape * docClaimDrawingShape(
					DrawingShapeList *	dsl );

extern struct DrawingShape * docClaimShapeInParent(
					DrawingShapeList *	dsl,
					struct DrawingShape *	parent,
					int			n,
					int			kind );

#   endif /*  DOC_DOCUMENT_SHAPE_LIST_H  */
