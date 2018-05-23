#   include	<stdio.h>
#   include	<stdlib.h>

#   include	<float.h>

#   define	y0	math_y0
#   define	y1	math_y1
#   include	<math.h>
#   undef	y0
#   undef	y1

#   include	"geo2DInteger.h"
#   include	<geoQuadTree.h>
#   include	<geoAffineTransform.h>
#   include	<geoLineFitter.h>
#   include	<geoClusterSegments.h>
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Find line segments and dump them.					*/
/*									*/
/************************************************************************/

static void bmInitSegmentCluster(	SegmentCluster *	sc )
    {
    sc->scSegments= (LineSegment2DI *)0;
    sc->scSegmentCount= 0;

    geoInvalidateRectangle( &(sc->scRectangle) );

    geoInitLineFitter( &(sc->scLineFitter) );

    sc->scX0= 0;
    sc->scY0= 0;
    sc->scX1= 0;
    sc->scY1= 0;
    sc->scLength= 0;

    sc->scA= 0;
    sc->scB= 0;
    sc->scC= 0;

    geoInitAffineTransform2D( &(sc->scTransform) );

    return;
    }

static void geoCleanSegmentCluster(	SegmentCluster *	sc )
    {
    if  ( sc->scSegments )
	{ free( sc->scSegments );	}

    return;
    }

static int geoFreeSegmentCluster(	int *			pDelete,
					int			x0,
					int			y0,
					void *			voidsc,
					void *			voidcs )
    {
    SegmentCluster *	sc= (SegmentCluster *)voidsc;

    geoCleanSegmentCluster( sc );
    free( sc );

    return 0;
    }

/************************************************************************/

int geoClusterSegmentsAddSegment(	ClusterSegments *	cs,
					int			x0,
					int			y0,
					int			x1,
					int			y1 )
    {
    int			rval= 0;
    SegmentCluster *	sc;
    double		dx, dy;

    sc= (SegmentCluster *)malloc( sizeof(SegmentCluster) );
    if  ( ! sc )
	{ XDEB(sc); return -1; }
    bmInitSegmentCluster( sc );

    sc->scSegments= (LineSegment2DI *)malloc( sizeof(LineSegment2DI) );
    if  ( ! sc->scSegments )
	{ XDEB(sc->scSegments); rval= -1; goto ready;	}
    sc->scSegmentCount= 1;

    sc->scX0= x0;
    sc->scY0= y0;
    sc->scX1= x1;
    sc->scY1= y1;

    dx= sc->scX1- sc->scX0;
    dy= sc->scY1- sc->scY0;
    sc->scLength= sqrt( dx*dx+ dy*dy );

    sc->scSegments->ls2diX0= x0;
    sc->scSegments->ls2diY0= y0;
    sc->scSegments->ls2diX1= x1;
    sc->scSegments->ls2diY1= y1;

    geo2DIBoxAroundXY( x0, y0, &(sc->scRectangle) );
    geo2DIBoxAroundXY( x1, y1, &(sc->scRectangle) );

    geoAddToLineFitter( &(sc->scLineFitter), x0, y0 );
    geoAddToLineFitter( &(sc->scLineFitter), x1, y1 );

    geoLineConstants( &(sc->scA), &(sc->scB), &(sc->scC), x0, y0, x1, y1 );
    geoLineAffineTransform2D( &(sc->scTransform), sc->scA, sc->scB, sc->scC );

    if  ( qtPut( cs->csClusterTree, x0, y0, sc )	)
	{ LDEB(1); rval= -1; goto ready;	}

    cs->csClusterCount++;
    cs->csSegmentCount++;

  ready:
    if  ( rval )
	{ geoFreeSegmentCluster( (int *)0, x0, y0, sc, (void *)0 ); }

    return rval;
    }

static void geoClusterSegmentsSetCurrentRectangle( ClusterSegments *	cs )
    {
    const DocumentRectangle *	lcDr= &(cs->csCurrentCluster->scRectangle);
    DocumentRectangle *		curDr= &(cs->csCurrentRectangle);

    const int		p0= -cs->csRectPaddingPercent;
    const int		p1= 100+ cs->csRectPaddingPercent;
    const int		p= ( p0+ p1 );
    const int		d= cs->csDistance;

    curDr->drX0= ( p1* lcDr->drX0+ p0* lcDr->drX1 )/ p- d;
    curDr->drY0= ( p1* lcDr->drY0+ p0* lcDr->drY1 )/ p- d;
    curDr->drX1= ( p0* lcDr->drX0+ p1* lcDr->drX1 )/ p+ d;
    curDr->drY1= ( p0* lcDr->drY0+ p1* lcDr->drY1 )/ p+ d;

    return;
    }

static void geoClusterSegmentsCalculateXY01(	SegmentCluster *	sc )
    {
    const LineSegment2DI *	ls= sc->scSegments;
    int				seg;

    double			l0= +DBL_MAX;
    double			l1= -DBL_MAX;
    double			sumc= 0;

    AffineTransform2D		atRev;

    double			dx, dy;

    if  ( sc->scSegmentCount < 1 )
	{ LDEB(sc->scSegmentCount); return;	}
    if  ( sc->scSegmentCount == 1 )
	{
	sc->scX0= ls->ls2diX0;
	sc->scY0= ls->ls2diY0;
	sc->scX1= ls->ls2diX1;
	sc->scY1= ls->ls2diY1;

	dx= sc->scX1- sc->scX0;
	dy= sc->scY1- sc->scY0;
	sc->scLength= sqrt( dx*dx+ dy*dy );

	return;
	}

    if  ( geoInvertAffineTransform2D( &atRev, &(sc->scTransform) ) )
	{ LDEB(1); return;	}

    ls= sc->scSegments;
    for ( seg= 0; seg < sc->scSegmentCount; ls++, seg++ )
	{
	double		l;
	double		c;

	l= AT2_X( ls->ls2diX0+ 0.499, ls->ls2diY0+ 0.499, &(sc->scTransform) );
	c= AT2_Y( ls->ls2diX0+ 0.499, ls->ls2diY0+ 0.499, &(sc->scTransform) );
	sumc += c;
	if  ( l0 > l )
	    { l0=  l; }
	if  ( l1 < l )
	    { l1=  l; }

	l= AT2_X( ls->ls2diX1+ 0.499, ls->ls2diY1+ 0.499, &(sc->scTransform) );
	c= AT2_Y( ls->ls2diX1+ 0.499, ls->ls2diY1+ 0.499, &(sc->scTransform) );
	sumc += c;
	if  ( l0 > l )
	    { l0=  l; }
	if  ( l1 < l )
	    { l1=  l; }
	}

    sumc /= 2* sc->scSegmentCount;

    sc->scX0= AT2_X( l0, sumc, &atRev )+ 0.499;
    sc->scY0= AT2_Y( l0, sumc, &atRev )+ 0.499;
    sc->scX1= AT2_X( l1, sumc, &atRev )+ 0.499;
    sc->scY1= AT2_Y( l1, sumc, &atRev )+ 0.499;

    dx= sc->scX1- sc->scX0;
    dy= sc->scY1- sc->scY0;
    sc->scLength= sqrt( dx*dx+ dy*dy );

    return;
    }

/************************************************************************/
/*									*/
/*  Check whether any candidates to be merged with the current can be	*/
/*  found in a certain rectangle					*/
/*									*/
/************************************************************************/

static int bmCurrentFilter(	const DocumentRectangle *	dr,
				void *				voidcs )
    {
    const ClusterSegments *	cs= (ClusterSegments *)voidcs;
    SegmentCluster *		curSc= cs->csCurrentCluster;
    double			c[QTquad_COUNT];
    int				i;
    double			c0;
    double			c1;

    if  ( ! geoIntersectRectangle( (DocumentRectangle *)0,
					&(cs->csCurrentRectangle), dr ) )
	{ return 0;	}

    c[QTquadNE]= AT2_Y( dr->drX1, dr->drY1, &(curSc->scTransform ) );
    c[QTquadNW]= AT2_Y( dr->drX0, dr->drY1, &(curSc->scTransform ) );
    c[QTquadSW]= AT2_Y( dr->drX0, dr->drY0, &(curSc->scTransform ) );
    c[QTquadSE]= AT2_Y( dr->drX1, dr->drY0, &(curSc->scTransform ) );

    c0= c1= c[QTquadNE];
    for ( i= 0; i < QTquad_COUNT; i++ )
	{
	if  ( c0 > c[i] )
	    { c0=  c[i];	}
	if  ( c1 < c[i] )
	    { c1=  c[i];	}
	}

    if  ( c0 < -cs->csDistance && c1 < -cs->csDistance )
	{ return 0; }


    if  ( c0 >  cs->csDistance && c1 >  cs->csDistance )
	{ return 0; }

    return 1;
    }

static int bmClusterCandidate(		int *			pDelete,
					int			x,
					int			y,
					void *			voidsc,
					void *			voidcs )
    {
    ClusterSegments *	cs= (ClusterSegments *)voidcs;
    SegmentCluster *	sc= (SegmentCluster *)voidsc;
    SegmentCluster *	curSc= cs->csCurrentCluster;
    LineSegment2DI *	fresh;

    LineFitter		lfMerged;
    int			n;

    double		a;
    double		b;
    double		c;

    double		msd;

    if  ( sc == curSc )
	{ return 0;	}

    geoAddLineFitters( &lfMerged,
			&(sc->scLineFitter), &(curSc->scLineFitter) );

    if  ( geoLineFitterFitLine( &a, &b, &c, &lfMerged ) )
	{ return 0;	}

    msd= geoLineFitterMeanSquareDistance( &lfMerged, a, b, c );

    if  ( msd > cs->csDistance )
	{ return 0;	}

    n= curSc->scSegmentCount+ sc->scSegmentCount;
    fresh= (LineSegment2DI *)realloc( curSc->scSegments,
						n* sizeof(LineSegment2DI) );
    if  ( ! fresh )
	{ LXDEB(n,fresh); return -1;	}
    curSc->scSegments= fresh;

    for ( n= 0; n < sc->scSegmentCount; n++ )
	{ fresh[curSc->scSegmentCount++]= sc->scSegments[n]; }

    geoUnionRectangle( &(curSc->scRectangle), &(curSc->scRectangle),
							&(sc->scRectangle) );
    curSc->scLineFitter= lfMerged;
    curSc->scA= a;
    curSc->scB= b;
    curSc->scC= c;

    geoClusterSegmentsSetCurrentRectangle( cs );

    geoLineAffineTransform2D( &(curSc->scTransform),
					curSc->scA, curSc->scB, curSc->scC );

    geoClusterSegmentsCalculateXY01( curSc );

    cs->csMerged= 1;

    geoFreeSegmentCluster( (int *)0, x, y, sc, voidcs );
    cs->csClusterCount--;
    *pDelete= 1;

    return 0;
    }

static int bmGrowCluster(		int *			pDelete,
					int			x,
					int			y,
					void *			voidsc,
					void *			voidcs )
    {
    SegmentCluster *	sc= (SegmentCluster *)voidsc;
    ClusterSegments *	cs= (ClusterSegments *)voidcs;

    if  ( sc->scSegmentCount < 1 )
	{ LDEB(sc->scSegmentCount); return -1;	}
    if  ( sc->scSegments[0].ls2diX0 != x )
	{ LLDEB(sc->scSegments[0].ls2diX0,x); return -1;	}
    if  ( sc->scSegments[0].ls2diY0 != y )
	{ LLDEB(sc->scSegments[0].ls2diY0,y); return -1;	}

    cs->csCurrentCluster= sc;
    geoClusterSegmentsSetCurrentRectangle( cs );

    qtForAll( cs->csClusterTree, bmCurrentFilter,
					bmClusterCandidate, voidcs );

    cs->csCurrentCluster= (SegmentCluster *)0;

    return 0;
    }

static int bmPrintAllSegments(		int *			pDelete,
					int			x,
					int			y,
					void *			voidsc,
					void *			voidcs )
    {
    ClusterSegments *		cs= (ClusterSegments *)voidcs;
    SegmentCluster *		sc= (SegmentCluster *)voidsc;
    const LineSegment2DI *	ls= sc->scSegments;
    int				seg;

    if  ( sc->scLength >= cs->csDiagonal/ 40 )
	{ cs->csLongClusterCount++;	}

    ls= sc->scSegments;
    for ( seg= 0; seg < sc->scSegmentCount; ls++, seg++ )
	{
	fprintf( cs->csFile, "%d %d %d %d ls\n",
					ls->ls2diX1- ls->ls2diX0,
					ls->ls2diY1- ls->ls2diY0,
					ls->ls2diX0, ls->ls2diY0 );
	}

    return 0;
    }

static int geoPrintClusterSegments(	int *			pDelete,
					int			x,
					int			y,
					void *			voidsc,
					void *			voidcs )
    {
    ClusterSegments *		cs= (ClusterSegments *)voidcs;
    SegmentCluster *		sc= (SegmentCluster *)voidsc;
    const LineSegment2DI *	ls= sc->scSegments;
    int				seg;

return 0;
    if  ( sc->scLength < cs->csDiagonal/ 40 )
	{ return 0;	}

    ls= sc->scSegments;
    for ( seg= 0; seg < sc->scSegmentCount; ls++, seg++ )
	{
	fprintf( cs->csFile, "%d %d %d %d ls\n",
					ls->ls2diX1- ls->ls2diX0,
					ls->ls2diY1- ls->ls2diY0,
					ls->ls2diX0, ls->ls2diY0 );
	}

    return 0;
    }

# if 0
static int geoPrintCluster(		int *			pDelete,
					int			x,
					int			y,
					void *			voidsc,
					void *			voidcs )
    {
    ClusterSegments *		cs= (ClusterSegments *)voidcs;
    SegmentCluster *		sc= (SegmentCluster *)voidsc;

    if  ( sc->scLength < cs->csDiagonal/ 40 )
	{ return 0;	}

    cs->csLongClusters[cs->csLongClusterCount++]= sc;

    fprintf( cs->csFile, "%d %d %d %d ls %%%% N=%d\n",
				sc->scX1- sc->scX0,
				sc->scY1- sc->scY0,
				sc->scX0, sc->scY0,
				sc->scSegmentCount );

    return 0;
    }
# endif

void geoInitClusterSegments(		ClusterSegments *	cs )
    {
    cs->csFile= (FILE *)0;
    cs->csClusterTree= (QuadTree *)0;
    cs->csCurrentCluster= (SegmentCluster *)0;
    geoInitRectangle( &(cs->csCurrentRectangle) );
    cs->csMerged= 0;
    cs->csDiagonal= 0;
    cs->csClusterCount= 0;
    cs->csSegmentCount= 0;
    cs->csLongClusterCount= 0;
    cs->csLongClusters= (SegmentCluster **)0;
    cs->csLongClusterTree= (QuadTree *)0;
    cs->csRectPaddingPercent= 20;
    cs->csDistance= 4;

    return;
    }

void geoCleanClusterSegments(		ClusterSegments *	cs )
    {
    if  ( cs->csLongClusterTree )
	{ qtFreeTree( cs->csLongClusterTree, (QuadForAllCall)0, cs ); }
    if  ( cs->csClusterTree )
	{ qtFreeTree( cs->csClusterTree, geoFreeSegmentCluster, cs ); }

    if  ( cs->csLongClusters )
	{ free( cs->csLongClusters );	}

    if  ( cs->csFile )
	{ fclose( cs->csFile );	}

    return;
    }

int geoClusterSegmentsAllocateTree(	ClusterSegments *		cs,
					const DocumentRectangle *	dr )
    {
    double		dx, dy;

    cs->csClusterTree= qtMakeTree( dr );
    if  ( ! cs->csClusterTree )
	{ XDEB(cs->csClusterTree); return -1;	}

    cs->csLongClusterTree= qtMakeTree( dr );
    if  ( ! cs->csLongClusterTree )
	{ XDEB(cs->csLongClusterTree); return -1;	}

    dx= dr->drX1- dr->drX0;
    dy= dr->drY1- dr->drY0;
    cs->csDiagonal= sqrt( dx*dx+ dy*dy );

    return 0;
    }

int geoClusterSegmentsSetFile(	ClusterSegments *		cs,
				const char *			fileName )
    {
    cs->csFile= fopen( fileName, "w" );
    if  ( ! cs->csFile )
	{ SXDEB(fileName,cs->csFile); return -1;	}

    return 0;
    }

static int geoFinishClusterSegments(	ClusterSegments *	cs )
    {
    int			i;

    cs->csLongClusterCount= 0;
    fprintf( cs->csFile, "0.6 setgray\n" );
    qtForAll( cs->csClusterTree, (QuadForAllFilter)0,
					bmPrintAllSegments, (void *)cs );

    if  ( cs->csLongClusterCount > 0 )
	{
	cs->csLongClusters= (SegmentCluster **)malloc( cs->csLongClusterCount*
						sizeof(SegmentCluster *) );
	if  ( ! cs->csLongClusters )
	    { LXDEB(cs->csLongClusterCount,cs->csLongClusters); return -1; }

	cs->csLongClusterCount= 0;
	}

    /*
    fprintf( cs->csFile, "1 0.5 0 setrgbcolor\n" );
    qtForAll( cs->csClusterTree, (QuadForAllFilter)0,
					geoPrintCluster, (void *)cs );
    */

    fprintf( cs->csFile, "1 0 0 setrgbcolor\n" );
    qtForAll( cs->csClusterTree, (QuadForAllFilter)0,
					geoPrintClusterSegments, (void *)cs );

    for ( i= 0; i < 100; i++ )
	{
	int				ni;
	int				j;
	const SegmentCluster *		sci;
	DocumentRectangle		dri;

	const DocumentRectangle *	drX;
	const int			p0= -5;
	const int			p1= 100+ 5;
	const int			p= ( p0+ p1 );
	const int			d= 8;

	if  ( i >= cs->csLongClusterCount )
	    { break;	}

	ni= ( cs->csLongClusterCount* rand() )/ ( RAND_MAX+ 1.0 );
	sci= cs->csLongClusters[ni];
	drX= &(sci->scRectangle);

	dri.drX0= ( p1* drX->drX0+ p0* drX->drX1 )/ p- d;
	dri.drY0= ( p1* drX->drY0+ p0* drX->drY1 )/ p- d;
	dri.drX1= ( p0* drX->drX0+ p1* drX->drX1 )/ p+ d;
	dri.drY1= ( p0* drX->drY0+ p1* drX->drY1 )/ p+ d;

	for ( j= 0; j < 100; j++ )
	    {
	    int				nj;
	    const SegmentCluster *	scj;
	    DocumentRectangle		drj;

	    if  ( j >= cs->csLongClusterCount )
		{ break;	}

	    nj= ( cs->csLongClusterCount* rand() )/ ( RAND_MAX+ 1.0 );
	    scj= cs->csLongClusters[nj];
	    drX= &(scj->scRectangle);

	    drj.drX0= ( p1* drX->drX0+ p0* drX->drX1 )/ p- d;
	    drj.drY0= ( p1* drX->drY0+ p0* drX->drY1 )/ p- d;
	    drj.drX1= ( p0* drX->drX0+ p1* drX->drX1 )/ p+ d;
	    drj.drY1= ( p0* drX->drY0+ p1* drX->drY1 )/ p+ d;

	    if  ( ! geoIntersectRectangle( (DocumentRectangle *)0,
							    &dri, &drj ) )
		{ continue;	}
	    }

	if  ( j >= 100 || j >= cs->csLongClusterCount )
	    { continue;	}
	}

    fprintf( cs->csFile, "showpage\n" );

    return 0;
    }

int geoRunClusterSegments(	ClusterSegments *		cs )
    {
    cs->csMerged= 1;
    while( cs->csMerged )
	{
	cs->csMerged= 0;

	qtForAll( cs->csClusterTree, (QuadForAllFilter)0,
					    bmGrowCluster, (void *)cs );
	}

    if  ( geoFinishClusterSegments( cs ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

