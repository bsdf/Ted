#   ifndef	UTIL_AFFINE_TRANSFORM_H
#   define	UTIL_AFFINE_TRANSFORM_H

/************************************************************************/
/*									*/
/*  Affine transformation 2 dimensional.				*/
/*									*/
/*     | Axx Ayx |     | Tx |      | x |     | AT2_X( x, y ) |		*/
/*  A= |         |  T= |    |  AT( |   | ) = |               |		*/
/*     | Axy Ayy |     | Ty |      | y |     | AT2_Y( x, y ) |		*/
/*									*/
/************************************************************************/

typedef struct AffineTransform2D
    {
    double		at2Axx;
    double		at2Axy;
    double		at2Ayx;
    double		at2Ayy;
    double		at2Tx;
    double		at2Ty;
    } AffineTransform2D;

#   define	AT2_X( x, y, at ) \
			( (at)->at2Axx* (x)+ (at)->at2Ayx* (y)+ (at)->at2Tx )
#   define	AT2_Y( x, y, at ) \
			( (at)->at2Axy* (x)+ (at)->at2Ayy* (y)+ (at)->at2Ty )

#   define	AT2_DET( at ) \
		( (at)->at2Axx* (at)->at2Ayy - (at)->at2Ayx* (at)->at2Axy )

#   define AFF2DEB(af) appDebug( "%s(%3d) %s= [%g,%g,%g,%g + %g,%g]\n",	\
			    __FILE__, __LINE__, #af,			\
			    (af)->at2Axx, (af)->at2Ayx,			\
			    (af)->at2Axy, (af)->at2Ayy,			\
			    (af)->at2Tx,  (af)->at2Ty )

/************************************************************************/
/*									*/
/*  Affine transformation 3 dimensional.				*/
/*									*/
/*     | Axx Ayx Azx |     | Tx |      | x |     | AT3_X( x, y, z ) |	*/
/*  A= | Axy Ayy Azy |  T= | Ty |  AT( | y | ) = | AT3_Y( x, y, z ) |	*/
/*     | Axz Ayz Azz |     | Tz |      | z |     | AT3_Z( x, y, z ) |	*/
/*									*/
/************************************************************************/

typedef struct AffineTransform3D
    {
    double	at3Axx;
    double	at3Axy;
    double	at3Axz;

    double	at3Ayx;
    double	at3Ayy;
    double	at3Ayz;

    double	at3Azx;
    double	at3Azy;
    double	at3Azz;

    double	at3Tx;
    double	at3Ty;
    double	at3Tz;
    } AffineTransform3D;

#   define AT3_X( x, y, z, at ) \
	( (at)->at3Axx* (x)+ (at)->at3Ayx* (y)+ (at)->at3Azx* (z)+ (at)->at3Tx )
#   define AT3_Y( x, y, z, at ) \
	( (at)->at3Axy* (x)+ (at)->at3Ayy* (y)+ (at)->at3Azy* (z)+ (at)->at3Ty )
#   define AT3_Z( x, y, z, at ) \
	( (at)->at3Axz* (x)+ (at)->at3Ayz* (y)+ (at)->at3Azz* (z)+ (at)->at3Tz )

/************************************************************************/
/*									*/
/*  Routine Declarations.						*/
/*									*/
/************************************************************************/

extern int geoAffineTransformForTriangles(	AffineTransform2D *	atRes,
						double			x_1,
						double			x_2,
						double			y_1,
						double			y_2,
						double			z_1,
						double			z_2,
						double			p_1,
						double			p_2,
						double			q_1,
						double			q_2,
						double			r_1,
						double			r_2 );

extern void geoInitAffineTransform2D(		AffineTransform2D *	at2 );
extern void geoIdentityAffineTransform2D(	AffineTransform2D *	at2 );
extern void geoRotationAffineTransform2D(	AffineTransform2D *	at2,
						double			a );
extern void geoRotationAffineTransform2DAtan(	AffineTransform2D *	at2,
						double			y,
						double			x );
extern void geoTranslationAffineTransform2D(	AffineTransform2D *	at2,
						double			x,
						double			y );
extern void geoScaleAffineTransform2D(		AffineTransform2D *	at2,
						double			xs,
						double			ys );

extern void geoAffineTransform2DProduct(
					AffineTransform2D *		ba,
					const AffineTransform2D *	b,
					const AffineTransform2D *	a );
extern int geoInvertAffineTransform2D(	AffineTransform2D *		atR,
					const AffineTransform2D *	atF );

extern double geoAffineTransformDeterminant2D(
					const AffineTransform2D *	at2 );

extern void geoInitAffineTransform3D(		AffineTransform3D *	at3 );
extern void geoIdentityAffineTransform3D(	AffineTransform3D *	at3 );
extern void geoXYRotationAffineTransform3D(	AffineTransform3D *	at3,
						double			a );
extern void geoXZRotationAffineTransform3D(	AffineTransform3D *	at3,
						double			a );
extern void geoYZRotationAffineTransform3D(	AffineTransform3D *	at3,
						double			a );

extern void geoAffineTransform3DProduct(
					AffineTransform3D *		ba,
					const AffineTransform3D *	b,
					const AffineTransform3D *	a );

extern double geoAffineTransformDeterminant3D(
					const AffineTransform3D *	at3 );

extern void geoLineAffineTransform2D(	AffineTransform2D *	at,
					double			a,
					double			b,
					double			c );

extern int geoLineConstants(		double *	pA,
					double *	pB,
					double *	pC,
					double		x_x0,
					double		x_y0,
					double		x_x1,
					double		x_y1 );

#   endif	/*  UTIL_AFFINE_TRANSFORM_H	*/
