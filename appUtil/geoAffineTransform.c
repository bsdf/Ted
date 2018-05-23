/************************************************************************/
/*									*/
/*  Find the affine transform that maps one triangle to another.	*/
/*									*/
/************************************************************************/

#   define	y0	math_y0
#   define	y1	math_y1
#   include	<math.h>
#   undef	y0
#   undef	y1

#   include	"geoAffineTransform.h"
#   include	<appDebugon.h>

#   if 0

=========================================================

Affine transform T: T(x)= Ax+ t, maps triangle XYZ to PQR.
XYZ nor PQR is collinear.

Derivation...

=========================================================

a_11* x_1+ a_12* x_2+ t_1= p_1
a_11* y_1+ a_12* y_2+ t_1= q_1
a_11* z_1+ a_12* z_2+ t_1= r_1

a_21* x_1+ a_22* x_2+ t_2= p_2
a_21* y_1+ a_22* y_2+ t_2= q_2
a_21* z_1+ a_22* z_2+ t_2= r_2

Eliminate t_s:
--------------

a_11* ( x_1- y_1 )+ a_12* ( x_2- y_2 )= ( p_1- q_1 )
a_11* ( x_1- z_1 )+ a_12* ( x_2- z_2 )= ( p_1- r_1 )

a_21* ( x_1- y_1 )+ a_22* ( x_2- y_2 )= ( p_2- q_2 )
a_21* ( x_1- z_1 )+ a_22* ( x_2- z_2 )= ( p_2- r_2 )

Divide by factors of a_11, a_21:
--------------------------------

a_11+ [ ( x_2- y_2 )* a_12 ]/ ( x_1- y_1 )= ( p_1- q_1 ) / ( x_1- y_1 )
a_11+ [ ( x_2- z_2 )* a_12 ]/ ( x_1- z_1 )= ( p_1- r_1 ) / ( x_1- z_1 )

a_21+ [ ( x_2- y_2 )* a_22 ]/ ( x_1- y_1 )= ( p_2- q_2 ) / ( x_1- y_1 )
a_21+ [ ( x_2- z_2 )* a_22 ]/ ( x_1- z_1 )= ( p_2- r_2 ) / ( x_1- z_1 )

If x_1 = y_1
a_12* ( x_2- y_2 )= ( p_1- q_1 ) -> a_12= ( p_1- q_1 ) / ( x_2- y_2 ) or
a_12* ( x_2- z_2 )= ( p_1- r_1 ) -> a_12= ( p_1- r_1 ) / ( x_2- z_2 )

If x_1 = z_1
a_22* ( x_2- y_2 )= ( p_2- q_2 ) -> a_22= ( p_2- q_2 ) / ( x_2- y_2 ) or
a_22* ( x_2- z_2 )= ( p_2- r_2 ) -> a_22= ( p_2- r_2 ) / ( x_2- z_2 )

Subtract:
---------

a_12* { ( x_2- y_2 ) / ( x_1- y_1 ) - ( x_2- z_2 ) / ( x_1- z_1 ) } =
      { ( p_1- q_1 ) / ( x_1- y_1 ) - ( p_1- r_1 ) / ( x_1- z_1 ) } ;

a_22* { ( x_2- y_2 ) / ( x_1- y_1 ) - ( x_2- z_2 ) / ( x_1- z_1 ) } =
      { ( p_2- q_2 ) / ( x_1- y_1 ) - ( p_2- r_2 ) / ( x_1- z_1 ) } ;

Finally divide:
---------------

a_12=
    { ( p_1- q_1 ) / ( x_1- y_1 ) - ( p_1- r_1 ) / ( x_1- z_1 ) } /
    { ( x_2- y_2 ) / ( x_1- y_1 ) - ( x_2- z_2 ) / ( x_1- z_1 ) } ;

a_22=
    { ( p_2- q_2 ) / ( x_1- y_1 ) - ( p_2- r_2 ) / ( x_1- z_1 ) } /
    { ( x_2- y_2 ) / ( x_1- y_1 ) - ( x_2- z_2 ) / ( x_1- z_1 ) } =

Similarly: Divide by factors of a_12, a_22:
-------------------------------------------

[ a_11* ( x_1- y_1 ) ] / ( x_2- y_2 ) + a_12 = ( p_1- q_1 ) / ( x_2- y_2 )
[ a_11* ( x_1- z_1 ) ] / ( x_2- z_2 ) + a_12 = ( p_1- r_1 ) / ( x_2- z_2 )

[ a_21* ( x_1- y_1 ) ] / ( x_2- y_2 ) + a_22 = ( p_2- q_2 ) / ( x_2- y_2 )
[ a_21* ( x_1- z_1 ) ] / ( x_2- z_2 ) + a_22 = ( p_2- r_2 ) / ( x_2- z_2 )

If x_2 = y_2
a_11* ( x_1- y_1 )= ( p_1- q_1 ) -> a_11= ( p_1- q_1 ) / ( x_1- y_1 ) or
a_11* ( x_1- z_1 )= ( p_1- r_1 ) -> a_11= ( p_1- r_1 ) / ( x_1- z_1 )

If x_2 = z_2
a_21* ( x_1- y_1 )= ( p_2- q_2 ) -> a_21= ( p_2- q_2 ) / ( x_1- y_1 ) or
a_21* ( x_1- z_1 )= ( p_2- r_2 ) -> a_21= ( p_2- r_2 ) / ( x_1- z_1 )

Subtract:
---------

a_11* { ( x_1- y_1 ) / ( x_2- y_2 ) - ( x_1- z_1 ) / ( x_2- z_2 ) } =
      { ( p_1- q_1 ) / ( x_2- y_2 ) - ( p_1- r_1 ) / ( x_2- z_2 ) } ;

a_21* { ( x_1- y_1 ) / ( x_2- y_2 ) - ( x_1- z_1 ) / ( x_2- z_2 ) } =
      { ( p_2- q_2 ) / ( x_2- y_2 ) - ( p_2- r_2 ) / ( x_2- z_2 ) } ;

Finally divide:
---------------

a_11=
    { ( p_1- q_1 ) / ( x_2- y_2 ) - ( p_1- r_1 ) / ( x_2- z_2 ) } /
    { ( x_1- y_1 ) / ( x_2- y_2 ) - ( x_1- z_1 ) / ( x_2- z_2 ) } ;

a_21=
    { ( p_2- q_2 ) / ( x_2- y_2 ) - ( p_2- r_2 ) / ( x_2- z_2 ) } /
    { ( x_1- y_1 ) / ( x_2- y_2 ) - ( x_1- z_1 ) / ( x_2- z_2 ) } ;

#   endif

#   define	PARANOIA 	0

int geoAffineTransformForTriangles(	AffineTransform2D *	atRes,
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
					double			r_2 )
    {
    AffineTransform2D	at;

    double		xy_1= x_1- y_1;
    double		xy_2= x_2- y_2;

    double		xz_1= x_1- z_1;
    double		xz_2= x_2- z_2;

    double		pq_1= p_1- q_1;
    double		pq_2= p_2- q_2;

    double		pr_1= p_1- r_1;
    double		pr_2= p_2- r_2;

    geoInitAffineTransform2D( &at );

    if  ( x_2 == y_2 || x_2 == z_2 )
	{
	if  ( x_2 == y_2 && x_2 == z_2 )
	    { FFFDEB(x_2,y_2,z_2); return -1;	}

	if  ( x_2 == y_2 )
	    {
	    if  ( x_1 == y_1 )
		{ FFFFDEB(x_1,y_1,x_2,y_2); return -1; }

	    at.at2Axx= pq_1 / xy_1;
	    at.at2Axy= pq_2 / xy_1;
	    }

	if  ( x_2 == z_2 )
	    {
	    if  ( x_1 == z_1 )
		{ FFFFDEB(x_1,z_1,x_2,z_2); return -1; }

	    at.at2Axx= pr_1 / xz_1;
	    at.at2Axy= pr_2 / xz_1;
	    }
	}
    else{
	if  ( xy_1/xy_2 == xz_1/xz_2 )
	    { FFFFDEB(xy_1,xy_2,xz_1,xz_2); return -1; }

	at.at2Axx= ( pq_1/xy_2 - pr_1/xz_2 ) / ( xy_1/xy_2 - xz_1/xz_2 );
	at.at2Axy= ( pq_2/xy_2 - pr_2/xz_2 ) / ( xy_1/xy_2 - xz_1/xz_2 );
	}

    if  ( x_1 == y_1 || x_1 == z_1 )
	{
	if  ( x_1 == y_1 && x_1 == z_1 )
	    { FFFDEB(x_1,y_1,z_1); return -1;	}

	if  ( x_1 == y_1 )
	    {
	    if  ( x_2 == y_2 )
		{ FFFFDEB(x_1,y_1,x_2,y_2); return -1; }

	    at.at2Ayx= ( p_1- q_1 )/ ( x_2- y_2 );
	    at.at2Ayy= ( p_2- q_2 )/ ( x_2- y_2 );
	    }

	if  ( x_1 == z_1 )
	    {
	    if  ( x_2 == z_2 )
		{ FFFFDEB(x_1,z_1,x_2,z_2); return -1; }

	    at.at2Ayx= ( p_1- r_1 )/ ( x_2- z_2 );
	    at.at2Ayy= ( p_2- r_2 )/ ( x_2- z_2 );
	    }
	}
    else{
	if  ( xy_2/xy_1 == xz_2/xz_1 )
	    { FFFFDEB(xy_1,xy_2,xz_1,xz_2); return -1; }

	at.at2Ayx= ( pq_1/xy_1 - pr_1/xz_1 ) / ( xy_2/xy_1 - xz_2/xz_1 );
	at.at2Ayy= ( pq_2/xy_1 - pr_2/xz_1 ) / ( xy_2/xy_1 - xz_2/xz_1 );
	}

    at.at2Tx= 0.0;
    at.at2Ty= 0.0;

    at.at2Tx= p_1- AT2_X( x_1, x_2, &at );
    at.at2Ty= p_2- AT2_Y( x_1, x_2, &at );

#   if		PARANOIA

    if  ( geoAffineTransformTest( &at, x_1, x_2, p_1, p_2 ) )
	{ FFFFDEB(x_1,x_2,p_1,p_2); return -1;	}
    if  ( geoAffineTransformTest( &at, y_1, y_2, q_1, q_2 ) )
	{ FFFFDEB(y_1,y_2,q_1,q_2); return -1;	}
    if  ( geoAffineTransformTest( &at, z_1, z_2, r_1, r_2 ) )
	{ FFFFDEB(z_1,z_2,r_1,r_2); return -1;	}

#   endif

    *atRes= at;

    return 0;
    }

#   if		PARANOIA

static int geoAffineTransformTest(	const AffineTransform *	at,
					double			x_1,
					double			x_2,
					double			p_1,
					double			p_2 )
    {
    double	d_1;
    double	d_2;

    d_1= AT_X( x_1, x_2, at )- p_1;
    d_2= AT_Y( x_1, x_2, at )- p_2;

    if  ( d_1 < -0.00009 || d_1 > 0.00009 ||
	  d_2 < -0.00009 || d_2 > 0.00009 )
	{
	FFFFDEB(x_1,x_2,p_1,p_2); FFDEB(d_1,d_2);
	return -1;
	}

    return 0;
    }

void zzz( void )
    {
    double			x_1;
    double			x_2;
    double			y_1;
    double			y_2;
    double			z_1;
    double			z_2;
    double			p_1;
    double			p_2;
    double			q_1;
    double			q_2;
    double			r_1;
    double			r_2;


    for ( x_1= 0; x_1 <= 400; x_1 += 100 )
    for ( x_2= 0; x_2 <= 400; x_2 += 100 )
	{
	for ( y_1= 0; y_1 <= 400; y_1 += 100 )
	for ( y_2= 0; y_2 <= 400; y_2 += 100 )
	    {
	    double	xy_1= x_1- y_1;
	    double	xy_2= x_2- y_2;

	    if  ( y_1 == x_1 && y_2 == x_2 )
		{ continue;	}

	    for ( z_1= 0; z_1 <= 400; z_1 += 100 )
	    for ( z_2= 0; z_2 <= 400; z_2 += 100 )
		{
		double	xz_1= x_1- z_1;
		double	xz_2= x_2- z_2;

		if  ( z_1 == x_1 && z_2 == x_2 )
		    { continue;	}
		if  ( z_1 == y_1 && z_2 == y_2 )
		    { continue;	}
		if  ( z_1 == y_1 && z_1 == x_1 )
		    { continue;	}
		if  ( z_2 == y_2 && z_2 == x_2 )
		    { continue;	}

		if  ( xy_1/xy_2 == xz_1/xz_2 )
		    { continue;	}

		for ( p_1= 0; p_1 <= 400; p_1 += 100 )
		for ( p_2= 0; p_2 <= 400; p_2 += 100 )
		    {
		    for ( q_1= 0; q_1 <= 400; q_1 += 100 )
		    for ( q_2= 0; q_2 <= 400; q_2 += 100 )
			{
			if  ( q_1 == p_1 && q_2 == p_2 )
			    { continue;	}

			for ( r_1= 0; r_1 <= 400; r_1 += 100 )
			for ( r_2= 0; r_2 <= 400; r_2 += 100 )
			    {
			    AffineTransform at;

			    if  ( r_1 == p_1 && r_2 == p_2 )
				{ continue;	}
			    if  ( r_1 == q_1 && r_2 == q_2 )
				{ continue;	}
			    if  ( r_1 == q_1 && r_1 == p_1 )
				{ continue;	}
			    if  ( r_2 == q_2 && r_2 == p_2 )
				{ continue;	}

			    geoAffineTransformForTriangles( &at,
								x_1, x_2,
								y_1, y_2,
								z_1, z_2,
								p_1, p_2,
								q_1, q_2,
								r_1, r_2 );
			    }
			}
		    }
		}
	    }
	}

    return;
    }

#   endif

/************************************************************************/
/*									*/
/*  Initialise affine transforms (2D)					*/
/*									*/
/************************************************************************/

void geoInitAffineTransform2D(		AffineTransform2D *	at2 )
    {
    at2->at2Axx= 0.0;
    at2->at2Axy= 0.0;
    at2->at2Ayx= 0.0;
    at2->at2Ayy= 0.0;
    at2->at2Tx=  0.0;
    at2->at2Ty=  0.0;

    return;
    }

void geoIdentityAffineTransform2D(	AffineTransform2D *	at2 )
    {
    geoInitAffineTransform2D( at2 );

    at2->at2Axx= 1.0;
    at2->at2Ayy= 1.0;

    return;
    }

void geoRotationAffineTransform2D(	AffineTransform2D *	at2,
					double			a )
    {
    double	cosa= cos( a );
    double	sina= sin( a );

    geoInitAffineTransform2D( at2 );

    at2->at2Axx= +cosa;
    at2->at2Axy= +sina;
    at2->at2Ayx= -sina;
    at2->at2Ayy= +cosa;

    return;
    }

void geoTranslationAffineTransform2D(	AffineTransform2D *	at2,
					double			x,
					double			y )
    {
    geoInitAffineTransform2D( at2 );

    at2->at2Axx= 1.0;
    at2->at2Ayy= 1.0;
    at2->at2Tx= x;
    at2->at2Ty= y;

    return;
    }

void geoScaleAffineTransform2D(	AffineTransform2D *	at2,
					double			xs,
					double			ys )
    {
    geoInitAffineTransform2D( at2 );

    at2->at2Axx= xs;
    at2->at2Ayy= ys;

    return;
    }

/************************************************************************/
/*									*/
/*  Invert an affine transform.						*/
/*									*/
/*  1)  The linear part follows the college algebra text book.		*/
/*  2)  Analogous to x'= a*x+ t => x= x'/a- t/a.			*/
/*									*/
/************************************************************************/

# if 0
void xxx( void );
void xxx()
    {
    AffineTransform2D	at;
    AffineTransform2D	atRev;
    double		x, y, xx, yy;


    geoRotationAffineTransform2DAtan( &at, 3, 4 );
    at.at2Tx= 4;
    at.at2Ty= 3;

    x= AT2_X( 5, 0, &at );
    y= AT2_Y( 5, 0, &at );
    FFDEB(x,y); /* 8, 6 */

    geoInvertAffineTransform2D( &atRev, &at );
    xx= AT2_X( x, y, &atRev );
    yy= AT2_Y( x, y, &atRev );
    FFDEB(xx,yy); /* 5, 0 */

    }
# endif

int geoInvertAffineTransform2D(	AffineTransform2D *		atR,
				const AffineTransform2D *	atF )
    {
    AffineTransform2D	at;
    double		det;

    double		xx;
    double		yy;

    geoInitAffineTransform2D( &at );

    /*  1  */
    det= geoAffineTransformDeterminant2D( atF );
    if  ( det == 0 )
	{ /*FDEB(det);*/ return -1;	}

    at.at2Axx= atF->at2Ayy/ det;
    at.at2Axy= atF->at2Ayx/ det;
    at.at2Ayx= atF->at2Axy/ det;
    at.at2Ayy= atF->at2Axx/ det;

    /*  2  */
    xx= AT2_X( atF->at2Tx, atF->at2Ty, &at );
    yy= AT2_Y( atF->at2Tx, atF->at2Ty, &at );

    at.at2Tx= -xx;
    at.at2Ty= -yy;

    *atR= at;
    return 0;
    }

void geoRotationAffineTransform2DAtan(	AffineTransform2D *	at2,
					double			y,
					double			x )
    {
    double	a= atan2( y, x );
    double	cosa= cos( a );
    double	sina= sin( a );

    geoInitAffineTransform2D( at2 );

    at2->at2Axx= +cosa;
    at2->at2Axy= +sina;
    at2->at2Ayx= -sina;
    at2->at2Ayy= +cosa;

    return;
    }

double geoAffineTransformDeterminant2D(
				    const AffineTransform2D *	at2 )
    {
    return	at2->at2Axx* at2->at2Ayy-
		at2->at2Axy* at2->at2Ayx;
    }

double geoAffineTransformDeterminant3D(
				    const AffineTransform3D *	at3 )
    {
    return	at3->at3Axx* at3->at3Ayy* at3->at3Azz+
		at3->at3Axy* at3->at3Azy* at3->at3Axz+
		at3->at3Azx* at3->at3Axy* at3->at3Ayz-
		at3->at3Axz* at3->at3Ayy* at3->at3Azx-
		at3->at3Ayz* at3->at3Azy* at3->at3Axx-
		at3->at3Azz* at3->at3Axy* at3->at3Ayx;
    }

/************************************************************************/
/*									*/
/*  Find the affine transform A: A(x,y)= (fx,fy) where			*/
/*	fx is the distance along the line. The origin is chosen in such	*/
/*	    a way that fx= 0 for the point on the line closest to the	*/
/*	    origin of the plane.					*/
/*	fy is the distance from the line: Positive values are to the	*/
/*	    left of the line						*/
/*									*/
/************************************************************************/

# if 0

    Ortogonal line through origin:
	(1)  bx- ay= 0 => y= bx/ a.
    And this line:
	(2)  ax+ by+ c= 0;
    Substitute (1) in (2)
	(2)  ax+ (b^2*x)/a+ c= 0;
	(2) [ a+ (b^2)/a ]* x+ c= 0
	(2) [ a+ (b^2)/a ]* x= -c
	(2) x=  -c/ [ a+ (b^2)/a ];
	(1) y= -bc/ [ a+ (b^2)/a ];

# endif

void geoLineAffineTransform2D(		AffineTransform2D *	at,
					double			a,
					double			b,
					double			c )
    {
    double	div;
    double	x0;
    double	y0;

    at->at2Axx=  b;
    at->at2Axy=  a;
    at->at2Ayx= -a;
    at->at2Ayy=  b;

    if  ( a == 0 || b == 0 )
	{
	at->at2Tx= 0;
	at->at2Ty= c;
	return;
	}

    div= ( a+ (b*b)/ a );
    x0= -c     /  div;
    y0= (-b*c) / (div*a);

    /*FFFFDEB(x0,y0,a*x0+b*y0,sqrt(x0*x0+y0*y0));*/

    at->at2Tx= 0;
    at->at2Tx= -AT2_X( x0, y0, at );
    at->at2Ty= c;

    return;
    }

/************************************************************************/
/*									*/
/*  Determine the constants A,B,C to give the line equation		*/
/*  A*x+ B*y+ C= 0 for the carrier of the line segment through (x0,y0)	*/
/*  and (x1,y1).							*/
/*									*/
/*  A and B are chosen such that A*A+ B*B= 1.				*/
/*									*/
/************************************************************************/

int geoLineConstants(		double *	pA,
				double *	pB,
				double *	pC,
				double		x0,
				double		y0,
				double		x1,
				double		y1 )
    {
    if  ( x1 == x0 && y1 == y0 )
	{ FFFFDEB(x0,y0,x1,y1); return -1;	}

    if  ( x1 == x0 )
	{
	if  ( y1 > y0 )
	    { *pA= -1; *pB= 0;	}
	else{ *pA=  1; *pB= 0;	}
	}
    else{
	if  ( y1 == y0 )
	    {
	    if  ( x1 > x0 )
		{ *pA= 0; *pB=  1;	}
	    else{ *pA= 0; *pB= -1;	}
	    }
	else{
	    double		dx= x1- x0;
	    double		dy= y1- y0;
	    double		r= sqrt( dx*dx+ dy*dy );

	    *pA= -dy/r;
	    *pB=  dx/r;
	    }
	}

    *pC= -( *pA* x0+ *pB* y0 );
    return 0;
    }

# if 0

# define LL(x0,y0,x1,y1) \
appDebug( "(%8.3f, %8.3f) .. (%8.3f, %8.3f) #####\n", \
    (double)x0, (double)y0, (double)x1, (double)y1 ); \
    geoLineConstants( &a, &b, &c, x0, y0, x1, y1 ); \
    geoLineAffineTransform2D( &at, a, b, c )

static int about( double d1, double d2 )
    {
    double	d;

    if  ( d1 == d2 )
	{ return 1;	}

    if  ( d2+ d1 > -0.001 && d2+ d1 < 0.001 )
	{
	d= ( d2- d1 );
	}
    else{
	d= ( d2- d1 )/ ( d2+ d1 );
	}

    if  ( d >= -0.001 && d <= 0.001 )
	{ return 1;	}

    return 0;
    }

# define TT(x,y,ex,ey) \
appDebug( "(%8.3f, %8.3f) -> (%8.3f, %8.3f)\n                 Expect (%8.3f, %8.3f) %s\n\n", \
    (double)x, (double)y, \
    AT2_X( x, y, &at ), AT2_Y( x, y, &at ), \
    (double)ex, (double)ey, \
    (about( AT2_X( x, y, &at ), ex ) && \
     about( AT2_Y( x, y, &at ), ey ) )?"":" !!!" )

extern void xxx( void );
void xxx()
    {
    double		a, b, c;
    AffineTransform2D	at;

    LL(0,0, 4,3 );
    TT(0,0, 0,0);
    TT(4,3, 5,0);
    TT(0.8,0.6, 1,0);
    TT(-0.6,0.8, 0,1);

    LL(0,0, -4,3 );
    TT(0,0, 0,0);
    TT(-4,3, 5,0);

    LL(0,0, 4,-3 );
    TT(0,0, 0,0);
    TT(4,-3, 5,0);

    LL(0,0, -4,-3 );
    TT(0,0, 0,0);
    TT(-4,-3, 5,0);

    LL(0,2, 4,5 );
    TT(0,2, 1.2,0);
    TT(4,5, 6.2,0);

    /****/
    LL(0,1, 4,4 );
    TT(0,1, 0.6,0);
    TT(4,4, 5.6,0);

    LL(0,1, -4,4 );
    TT(0,1, 0.6,0);
    TT(-4,4, 5.6,0);

    LL(0,-1, 4,-4 );
    TT(0,-1, 0.6,0);
    TT(4,-4, 5.6,0);

    LL(0,-1, -4,-4 );
    TT(0,-1, 0.6,0);
    TT(-4,-4, 5.6,0);

    /****/

    LL(0,0, 1,0 );
    TT(0,0, 0,0);
    TT(1,0, 1,0);
    TT(2,0, 2,0);

    LL(0,0, -1,0 );
    TT(0,0, 0,0);
    TT(-1,0, 1,0);
    TT(-2,0, 2,0);

    LL(0,1, 1,1 );
    TT(0,1, 0,0);
    TT(1,1, 1,0);

    LL(0,1, -1,1 );
    TT(0,1, 0,0);
    TT(-1,1, 1,0);

    /****/
    LL(0,0, 0,1 );
    TT(0,0, 0,0);
    TT(0,1, 1,0);
    TT(0,2, 2,0);

    LL(0,0, 0,-1 );
    TT(0,0, 0,0);
    TT(0,-1, 1,0);

    LL(1,0, 1,1 );
    TT(1,0, 0,0);
    TT(1,1, 1,0);

    LL(1,0, 1,-1 );
    TT(1,0, 0,0);
    TT(1,-1, 1,0);

    }
# endif

/************************************************************************/
/*									*/
/*  Initialise affine transforms (3D)					*/
/*									*/
/************************************************************************/

void geoInitAffineTransform3D(		AffineTransform3D *	at3 )
    {
    at3->at3Axx= 0.0;
    at3->at3Axy= 0.0;
    at3->at3Axz= 0.0;

    at3->at3Ayx= 0.0;
    at3->at3Ayy= 0.0;
    at3->at3Ayz= 0.0;

    at3->at3Azx= 0.0;
    at3->at3Azy= 0.0;
    at3->at3Azz= 0.0;

    at3->at3Tx=  0.0;
    at3->at3Ty=  0.0;
    at3->at3Tz=  0.0;

    return;
    }

void geoIdentityAffineTransform3D(	AffineTransform3D *	at3 )
    {
    geoInitAffineTransform3D( at3 );

    at3->at3Axx= 1.0;
    at3->at3Ayy= 1.0;
    at3->at3Azz= 1.0;

    return;
    }

void geoXYRotationAffineTransform3D(	AffineTransform3D *	at3,
					double			a )
    {
    double	cosa= cos( a );
    double	sina= sin( a );

    geoInitAffineTransform3D( at3 );

    at3->at3Axx= +cosa;
    at3->at3Axy= +sina;
    at3->at3Ayx= -sina;
    at3->at3Ayy= +cosa;

    at3->at3Azz= 1.0;

    return;
    }

void geoXZRotationAffineTransform3D(	AffineTransform3D *	at3,
					double			a )
    {
    double	cosa= cos( a );
    double	sina= sin( a );

    geoInitAffineTransform3D( at3 );

    at3->at3Axx= +cosa;
    at3->at3Axz= +sina;
    at3->at3Azx= -sina;
    at3->at3Azz= +cosa;

    at3->at3Ayy= 1.0;

    return;
    }

void geoYZRotationAffineTransform3D(	AffineTransform3D *	at3,
					double			a )
    {
    double	cosa= cos( a );
    double	sina= sin( a );

    geoInitAffineTransform3D( at3 );

    at3->at3Ayy= +cosa;
    at3->at3Ayz= +sina;
    at3->at3Azy= -sina;
    at3->at3Azz= +cosa;

    at3->at3Axx= 1.0;

    return;
    }

/************************************************************************/
/*									*/
/*  Products.								*/
/*									*/
/************************************************************************/

void geoAffineTransform2DProduct(	AffineTransform2D *		ba,
					const AffineTransform2D *	b,
					const AffineTransform2D *	a )
    {
    AffineTransform2D	c;

    /**/
    c.at2Axx= b->at2Axx* a->at2Axx+ b->at2Ayx* a->at2Axy;
    c.at2Axy= b->at2Axy* a->at2Axx+ b->at2Ayy* a->at2Axy;

    /**/
    c.at2Ayx= b->at2Axx* a->at2Ayx+ b->at2Ayx* a->at2Ayy;
    c.at2Ayy= b->at2Axy* a->at2Ayx+ b->at2Ayy* a->at2Ayy;

    /**/
    c.at2Tx= b->at2Axx* a->at2Tx+ b->at2Ayx* a->at2Ty+ b->at2Tx;
    c.at2Ty= b->at2Axy* a->at2Tx+ b->at2Ayy* a->at2Ty+ b->at2Ty;

    /**/

    *ba= c;
    }

void geoAffineTransform3DProduct(	AffineTransform3D *		ba,
					const AffineTransform3D *	b,
					const AffineTransform3D *	a )
    {
    AffineTransform3D	c;

    /**/
    c.at3Axx= b->at3Axx* a->at3Axx+ b->at3Ayx* a->at3Axy+ b->at3Azx* a->at3Axz;
    c.at3Axy= b->at3Axy* a->at3Axx+ b->at3Ayy* a->at3Axy+ b->at3Azy* a->at3Axz;
    c.at3Axz= b->at3Axz* a->at3Axx+ b->at3Ayz* a->at3Axy+ b->at3Azz* a->at3Axz;

    /**/
    c.at3Ayx= b->at3Axx* a->at3Ayx+ b->at3Ayx* a->at3Ayy+ b->at3Azx* a->at3Ayz;
    c.at3Ayy= b->at3Axy* a->at3Ayx+ b->at3Ayy* a->at3Ayy+ b->at3Azy* a->at3Ayz;
    c.at3Ayz= b->at3Axz* a->at3Ayx+ b->at3Ayz* a->at3Ayy+ b->at3Azz* a->at3Ayz;

    /**/
    c.at3Azx= b->at3Axx* a->at3Azx+ b->at3Ayx* a->at3Azy+ b->at3Azx* a->at3Azz;
    c.at3Azy= b->at3Axy* a->at3Azx+ b->at3Ayy* a->at3Azy+ b->at3Azy* a->at3Azz;
    c.at3Azz= b->at3Axz* a->at3Azx+ b->at3Ayz* a->at3Azy+ b->at3Azz* a->at3Azz;

    /**/
    c.at3Tx= b->at3Axx* a->at3Tx+ b->at3Ayx* a->at3Ty+ b->at3Azx* a->at3Tz+
								    b->at3Tx;
    c.at3Ty= b->at3Axy* a->at3Tx+ b->at3Ayy* a->at3Ty+ b->at3Azy* a->at3Tz+
								    b->at3Ty;
    c.at3Tz= b->at3Axz* a->at3Tx+ b->at3Ayz* a->at3Ty+ b->at3Azz* a->at3Tz+
								    b->at3Tz;

    /**/

    *ba= c;
    }

