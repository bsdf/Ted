/************************************************************************/
/*									*/
/*  Description of an arrow in a shape as it must be drawn.		*/
/*									*/
/************************************************************************/

#   ifndef	DOC_DRAW_SHAPE_ARROW_H
#   define	DOC_DRAW_SHAPE_ARROW_H

#   include	"docShapeDrawing.h"
#   include	<geo2DDouble.h>

typedef struct DrawShapeArrow
    {
			/**
			 *  The line in the direction of the arrow (points 
			 *  to the head. It is truncated to reserve space for 
			 *  the head.
			 */
    Point2DD		dsaShaft[2];
			/**
			 *  The arrow from the shape
			 */
    ShapeArrow		dsaArrow;
			/**
			 *  The actual path of the arrow head
			 */
    Point2DD		dsaPath[7];
			/**
			 *  The length of the actual path of the arrow head
			 */
    int			dsaPathLength;

    Rectangle2DD	dsaArrowRectangle;

    double		dsaLength;
    double		dsaLength2;
    double		dsaWidth;
    } DrawShapeArrow;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docShapeArrowSizesTwips(	DrawShapeArrow *	dsa,
					int			lineWidth,
					double			xfac,
					const Point2DD *	shaft,
					const ShapeArrow *	sa );

#   endif	/*  DOC_DRAW_SHAPE_ARROW_H	*/
