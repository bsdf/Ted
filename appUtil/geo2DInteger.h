/************************************************************************/
/*									*/
/*  Various definitions relating to document geometry.			*/
/*									*/
/************************************************************************/

#   ifndef		GEO_2D_INTEGER_H
#   define		GEO_2D_INTEGER_H

typedef struct DocumentRectangle
    {
    int		drX0;
    int		drY0;
    int		drX1;
    int		drY1;
    } DocumentRectangle;

#   define RECTDEB(dr) appDebug(					\
			    "%s(%3d) %s= [%d..%d x %d..%d]\n",		\
			    __FILE__, __LINE__, #dr,			\
			    (dr)->drX0, (dr)->drX1,			\
			    (dr)->drY0, (dr)->drY1 )

typedef struct Point2DI
    {
    int		x;
    int		y;
    } Point2DI;

/**
  * A two dimensional arc
  */
typedef struct Arc2DI
    {
			/**
			  * The rectangle that holds the full ellipse
			  */
    DocumentRectangle	a2diRect;
			/**
			 * The start angle relative to the 3-o-clock angle 
			 * in 64th of a degree counter clock wise.
			 */
    int			a2diAngleFrom;
			/**
			 * The change of angle relative to a2diAngleFrom 
			 * Also counter clock wise and also in 64th of a degree.
			 */
    int			a2diAngleStep;
    } Arc2DI;

typedef struct LineSegment2DI
    {
    int		ls2diX0;
    int		ls2diY0;
    int		ls2diX1;
    int		ls2diY1;
    } LineSegment2DI;

# define geo2DIXYInBox( x, y, b ) \
		    ( (x) >= (b)->drX0 && \
		      (x) <= (b)->drX1 && \
		      (y) >= (b)->drY0 && \
		      (y) <= (b)->drY1 )

# define geo2DIPointInBox( p, b ) geo2DIXYInBox( (p)->x, (p)->y, (b) )

# define geo2DIBoxAroundXY( x, y, b ) \
    { \
    if  ( (b)->drX0 > (x) ) { (b)->drX0=  (x);	} \
    if  ( (b)->drY0 > (y) ) { (b)->drY0=  (y);	} \
    if  ( (b)->drX1 < (x) ) { (b)->drX1=  (x);	} \
    if  ( (b)->drY1 < (y) ) { (b)->drY1=  (y);	} \
    }

# define geoShiftRectangle( dr, ox, oy ) \
    { \
    (dr)->drX0 += (ox); \
    (dr)->drX1 += (ox); \
    (dr)->drY0 += (oy); \
    (dr)->drY1 += (oy); \
    }

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void geoInitRectangle(		DocumentRectangle *		dr );
extern void geoInvalidateRectangle(	DocumentRectangle *		dr );

extern void geoUnionRectangle(	DocumentRectangle *		dr,
				const DocumentRectangle *	dr1,
				const DocumentRectangle *	dr2	);

extern void geoNormalizeRectangle(
				DocumentRectangle *		drTo,
				const DocumentRectangle *	drFrom );

extern int geoIntersectRectangle(	DocumentRectangle *		dr,
					const DocumentRectangle *	dr1,
					const DocumentRectangle *	dr2 );

extern int geo2DIIntersectSegments(	Point2DI *		p1,
					Point2DI *		p2,
					double *		pXab1,
					double *		pXab2,
					double *		pXcd1,
					double *		pXcd2,
					const Point2DI *	ab,
					const Point2DI *	cd );

extern double geo2DIDistanceToLine(	const Point2DI *	ab,
					const Point2DI *	c );

extern double geo2DIProjectionOnLine(	const Point2DI *	ab,
					const Point2DI *	c );

extern int geo2DIPointInPolygon(	const Point2DI *	p,
					const Point2DI *	points,
					int			pointCount );

extern int geo2DIClipSegmentToRectangle(
				Point2DI			vpChanged[2],
				double *			pXp0,
				double *			pXp1,
				const Point2DI			vp[2],
				const DocumentRectangle *	dr );

extern int geo2DISurface(	Point2DI *			points,
				int				n );

#   endif	/*	GEO_2D_INTEGER_H */
