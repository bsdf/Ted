#   include	<math.h>
#   include	<geoLineFitter.h>
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Fit lines of the form a*x+ b*y+ c to collections of points and	*/
/*  calculate properties of lines represented in this way.		*/
/*									*/
/************************************************************************/

/************************************************************************/
/*									*/
/*  Initialise structure.						*/
/*									*/
/************************************************************************/

void geoInitLineFitter(	LineFitter *	lf )
    {
    lf->lfN= 0;
    lf->lfSx= 0;
    lf->lfSy= 0;
    lf->lfSxx= 0;
    lf->lfSyy= 0;
    lf->lfSxy= 0;

    return;
    }


void geoAddLineFitters(		LineFitter *		to,
				LineFitter *		fr1,
				LineFitter *		fr2 )
    {
    to->lfN= fr1->lfN+ fr2->lfN;
    to->lfSx= fr1->lfSx+ fr2->lfSx;
    to->lfSy= fr1->lfSy+ fr2->lfSy;
    to->lfSxx= fr1->lfSxx+ fr2->lfSxx;
    to->lfSyy= fr1->lfSyy+ fr2->lfSyy;
    to->lfSxy= fr1->lfSxy+ fr2->lfSxy;

    return;
    }

/************************************************************************/
/*									*/
/*  Fit a line trough the collection of point that have been collected	*/
/*  in this lene fitter.						*/
/*									*/
/*  Return: -1 for an absurd call.					*/
/*  Return:  1 If no line can be found: The points form a circular	*/
/*	cloud.								*/
/*  Return:  0 For a successful call: a,b,c are returned.		*/
/*									*/
/************************************************************************/

# if 0

Find the line that minimizes the sum of squared distances from 
the line for a given set of points.

A line is identified by its distance equation: ax+ by+ c= 0
Require that a*a+ b*b= 1. c is the negative distance of the 
line to the origin.

The distance of an arbitrary point (x,y) to the line is 
ax+ by+ c. Remember that a^2+ b^2= 1.  This can be rewritten as 
sin( A )* x+ cos( A )* y+ c;

So the square of the distance is:
( ax+ by+ c )* ( ax+ by+ c )= a*a*x*x+	a*b*x*y+	a*c*x+
			      a*b*x*y+	b*b*y*y+	b*c*y+
			      a*c*x+	c*b*y+		c*c=

Cumulated and expanding to cosA,sinA:
    sinA*sinA* Sxx+
    cosA*cosA* Syy+
    2*sinA*cosA* Sxy+
    2*sinA*c* Sx+
    2*cosA*c* Sy+
    N* c*c.

(1) Derivative to A; 0 in extremes:
    2*  cosA*sinA* Sxx+
    2* -sinA*cosA* Syy+
    2*  cosA* cosA* Sxy+ -2* sinA* sinA* Sxy+
    2*  cosA* c* Sx+
    2* -sinA* c* Sy= 0
(2) Derivative to c; 0 in extremes:
    2* sinA* Sx+ 2* cosA* Sy+ 2* N* c= 0;

(1) ( Sxx- Syy )* cosAsinA+
    Sxy* cosAcosA+
    -Sxy* sinAsinA+
     c*Sx* cosA+
    -c*Sy* sinA= 0;
(2) Sx* sinA+ Sy* cosA+ N* c= 0; -> c= -Sx/N* sinA -Sy/N* cosA.

Substitute (2) into (1):
(1) ( Sxx- Syy )* cosAsinA+
    Sxy* cosAcosA+
    -Sxy* sinAsinA+
     (-Sx/N* sinA -Sy/N* cosA)*Sx* cosA+
    -(-Sx/N* sinA -Sy/N* cosA)*Sy* sinA= 0;

(1) ( Sxx- Syy )* cosAsinA+
    Sxy* cosAcosA+
    -Sxy* sinAsinA+
    -SxSx/N* cosAsinA -SxSy/N* cosAcosA+
     SxSy/N* sinAsinA +SySy/N* sinAcosA= 0;

(1) ( Sxx- Syy- SxSx/N+ SySy/N )* cosAsinA+
    (  Sxy -SxSy/N )* cosAcosA+
    ( -Sxy+ SxSy/N )* sinAsinA= 0;

Divide by N:

(1) ( Sxx/N- Syy/N- SxSx/NN+ SySy/NN )* cosAsinA+
    (  Sxy/N -SxSy/NN )* cosAcosA+
    ( -Sxy/N+ SxSy/NN )* sinAsinA= 0;

CovXY= Sxy/N- SxSy/(N*N)
VarX=  Sxx/N- SxSx/(N*N)
VarY=  Syy/N- SySy/(N*N)

(1) ( VarX- VarY )* cosAsinA+ CovXY* cosAcosA+ -CovXY* sinAsinA= 0;

(1) ( VarX- VarY )* sin( 2* A )/2+ CovXY* cos( 2* A )= 0;

(1) [( VarX- VarY )/2]* sin( 2* A )= -CovXY* cos( 2* A );

(1) sin( 2* A )/cos( 2* A )= -CovXY/[( VarX- VarY )/2];

A= atan( -CovXY/[( VarX- VarY )/2] )/2

# endif

int geoLineFitterFitLine(	double *		pA,
				double *		pB,
				double *		pC,
				const LineFitter *	lf )
    {
    double	NN;
    double	CovXY;
    double	VarX;
    double	VarY;
    double	A;

    if  ( lf->lfN < 2 )
	{ LDEB(lf->lfN); return -1;	}

    NN= lf->lfN* lf->lfN;

    CovXY= lf->lfSxy/lf->lfN- ( lf->lfSx* lf->lfSy )/ NN;
    VarX=  lf->lfSxx/lf->lfN- ( lf->lfSx* lf->lfSx )/ NN;
    VarY=  lf->lfSyy/lf->lfN- ( lf->lfSy* lf->lfSy )/ NN;

    if  ( CovXY == 0 && VarX == 0 && VarY == 0 )
	{ FFFDEB(VarX,VarY,CovXY); return -1;	}

    if  ( CovXY == 0 && VarX != 0 && VarY != 0 )
	{ return  1;	}

    A= atan2( -CovXY, ( VarX- VarY )/2 )/2;

    *pA= sin( A );
    *pB= cos( A );
    *pC= -( lf->lfSx* *pA+ lf->lfSy* *pB )/lf->lfN;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Calculate the mean square distance from the line. This is a measure	*/
/*  of the quality of the fit.						*/
/*									*/
/************************************************************************/

double geoLineFitterMeanSquareDistance(	const LineFitter *	lf,
					double			a,
					double			b,
					double			c )
    {
    double	d;

    d=  a* a* lf->lfSxx+
	2* a* b* lf->lfSxy+
	2* a* c* lf->lfSx+
	b* b* lf->lfSyy+
	2* b* c* lf->lfSy+
	c* c* lf->lfN;

    return d/ lf->lfN;
    }

# if 0
typedef struct LineSegment
    {
    double	lsX0;
    double	lsY0;
    double	lsX1;
    double	lsY1;
    } LineSegment;

# define SCORE() (((d+0.1)*(l1-l0+1))/lf.lfN)

extern void xxx( void );
void xxx()
    {
    double		a, b, c;
    double		d;
    LineFitter		lf;
    double		l0;
    double		l1;
    double		l;

    AffineTransform2D	at;

    LineSegment		ls1;
    LineSegment		ls2;
int dx;

    double		s1= 0;
    double		s2= 0;
    double		s= 0;

    ls1.lsX0= 0; ls1.lsY0= 0;
    ls1.lsX1= 8; ls1.lsY1= 2;


for ( dx= 0; dx <= 8; dx ++ )
{
    ls2= ls1;
    ls2.lsY0 += 2;
    ls2.lsY1 += 2;

    ls2.lsX0 += dx;
    ls2.lsX1 += dx;

    geoInitLineFitter( &lf );
    l0= DBL_MAX; l1= DBL_MIN;

    geoAddToLineFitter( &lf,  ls1.lsX0,  ls1.lsY0 );
    geoAddToLineFitter( &lf,  ls1.lsX1,  ls1.lsY1 );

    if  ( ! geoLineFitterFitLine( &a, &b, &c, &lf ) )
	{
	d= geoLineFitterMeanSquareDistance( &lf, a, b, c );
	/*
	appDebug( "%g* x+ %g* y+ %g= 0; D=%g\n", a, b, c, d );
	*/

	geoLineAffineTransform2D( &at, a, b, c );

	l= AT2_X( ls1.lsX0, ls1.lsY0, &at );
	if ( l0 > l ) { l0= l;	}
	if ( l1 < l ) { l1= l;	}
	l= AT2_X( ls1.lsX1, ls1.lsY1, &at );
	if ( l0 > l ) { l0= l;	}
	if ( l1 < l ) { l1= l;	}

	s1= SCORE();
	if(dx==0)LFDEB(dx,s1);
	}

    geoInitLineFitter( &lf );
    l0= DBL_MAX; l1= DBL_MIN;

    geoAddToLineFitter( &lf,  ls2.lsX0,  ls2.lsY0 );
    geoAddToLineFitter( &lf,  ls2.lsX1,  ls2.lsY1 );

    if  ( ! geoLineFitterFitLine( &a, &b, &c, &lf ) )
	{
	d= geoLineFitterMeanSquareDistance( &lf, a, b, c );
	/*
	appDebug( "%g* x+ %g* y+ %g= 0; D=%g\n", a, b, c, d );
	*/

	geoLineAffineTransform2D( &at, a, b, c );

	l= AT2_X( ls2.lsX0, ls2.lsY0, &at );
	if ( l0 > l ) { l0= l;	}
	if ( l1 < l ) { l1= l;	}
	l= AT2_X( ls2.lsX1, ls2.lsY1, &at );
	if ( l0 > l ) { l0= l;	}
	if ( l1 < l ) { l1= l;	}

	s2= SCORE();
	if(dx==0)LFDEB(dx,s2);
	}

    geoInitLineFitter( &lf );
    l0= DBL_MAX; l1= DBL_MIN;

    geoAddToLineFitter( &lf,  ls1.lsX0,  ls1.lsY0 );
    geoAddToLineFitter( &lf,  ls1.lsX1,  ls1.lsY1 );
    geoAddToLineFitter( &lf,  ls2.lsX0,  ls2.lsY0 );
    geoAddToLineFitter( &lf,  ls2.lsX1,  ls2.lsY1 );

    if  ( ! geoLineFitterFitLine( &a, &b, &c, &lf ) )
	{
	d= geoLineFitterMeanSquareDistance( &lf, a, b, c );
	/*
	appDebug( "%g* x+ %g* y+ %g= 0; D=%g\n", a, b, c, d );
	*/

	geoLineAffineTransform2D( &at, a, b, c );

	l= AT2_X( ls1.lsX0, ls1.lsY0, &at );
	if ( l0 > l ) { l0= l;	}
	if ( l1 < l ) { l1= l;	}
	l= AT2_X( ls1.lsX1, ls1.lsY1, &at );
	if ( l0 > l ) { l0= l;	}
	if ( l1 < l ) { l1= l;	}

	l= AT2_X( ls2.lsX0, ls2.lsY0, &at );
	if ( l0 > l ) { l0= l;	}
	if ( l1 < l ) { l1= l;	}
	l= AT2_X( ls2.lsX1, ls2.lsY1, &at );
	if ( l0 > l ) { l0= l;	}
	if ( l1 < l ) { l1= l;	}

	s= SCORE();
	LFFDEB(dx,s,s/s1);
	}
}

    return;
    }
# endif
