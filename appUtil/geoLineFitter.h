/************************************************************************/
/*									*/
/*  Statistics for finding lines in the plain.				*/
/*									*/
/************************************************************************/

#   ifndef		GEO_LINE_FITTER_H
#   define		GEO_LINE_FITTER_H

typedef struct LineFitter
    {
    int		lfN;
    double	lfSx;
    double	lfSy;
    double	lfSxx;
    double	lfSyy;
    double	lfSxy;
    } LineFitter;

# define geoAddToLineFitter( lf, x, y ) \
    { \
    (lf)->lfN++; \
    (lf)->lfSx += (x); \
    (lf)->lfSy += (y); \
    (lf)->lfSxx += (x)*(x); \
    (lf)->lfSyy += (y)*(y); \
    (lf)->lfSxy += (x)*(y); \
    }

/************************************************************************/
/*									*/
/*  Routine Declarations						*/
/*									*/
/************************************************************************/

extern void geoInitLineFitter(	LineFitter *	lf );

extern int geoLineFitterFitLine(	double *		pA,
					double *		pB,
					double *		pC,
					const LineFitter *	lf );

extern double geoLineFitterMeanSquareDistance(
					const LineFitter *	lf,
					double			a,
					double			b,
					double			c );

extern void geoAddLineFitters(		LineFitter *		to,
					LineFitter *		fr1,
					LineFitter *		fr2 );

#   endif	/*	GEO_LINE_FITTER_H	*/
