#   include	"bitmapConfig.h"

#   include	"bmintern.h"
#   include	<stdlib.h>

#   include	<geoAffineTransform.h>
#   include	<geo2DInteger.h>

#   include	<appDebugon.h>

#   define	CHECK_SEGMENTS	1

/************************************************************************/
/*									*/
/*  Find line segments in an image.					*/
/*									*/
/************************************************************************/

typedef struct FindLineSegmentsJob
    {
    int				flsjPixelSize;

    int				flsjSelX0;
    int				flsjSelY0;
    int				flsjSelX1;
    int				flsjSelY1;

    RasterImage			flsjRiSel;
    RasterImage			flsjRiSegs;

    void *			flsjThrough;
    BM_SEG_FUN			flsjSegFun;

    int				flsjSegmentCount;
    int				flsjSegmentsAllocated;
    LineSegment2DI *		flsjSegments;

    double			flsjA;
    double			flsjB;
    double			flsjC;
    AffineTransform2D		flsjTransform;
    } FindLineSegmentsJob;

static int bmCollectLineSegment(	void *			voidflsj,
					int			row,
					int			col )
    
    {
    FindLineSegmentsJob *	flsj= (FindLineSegmentsJob *)voidflsj;
    LineSegment2DI *		fresh;

    if  ( flsj->flsjSegmentCount >= flsj->flsjSegmentsAllocated )
	{
	int		n= ( 3* flsj->flsjSegmentsAllocated+ 4 )/ 2;

	fresh= (LineSegment2DI *)realloc( flsj->flsjSegments, n* sizeof(LineSegment2DI) );
	if  ( ! fresh )
	    { LXDEB(n,fresh); return -1;	}

	flsj->flsjSegments= fresh;
	flsj->flsjSegmentsAllocated= n;
	}

    fresh= flsj->flsjSegments+ flsj->flsjSegmentCount++;
    fresh->ls2diX0= flsj->flsjSelX0+ col;
    fresh->ls2diX1= flsj->flsjSelX1+ col;
    fresh->ls2diY0= flsj->flsjSelY0+ row;
    fresh->ls2diY1= flsj->flsjSelY1+ row;

    return 0;
    }

static const AffineTransform2D * bmSortAt;

static int bmCompareLineSegments(	const void *	vls1,
					const void *	vls2 )
    {
    const LineSegment2DI *	ls1= (const LineSegment2DI *)vls1;
    const LineSegment2DI *	ls2= (const LineSegment2DI *)vls2;
    double		c1, c2;
    double		l1, l2;

    c1= AT2_Y( ls1->ls2diX0, ls1->ls2diY0, bmSortAt );
    c2= AT2_Y( ls2->ls2diX0, ls2->ls2diY0, bmSortAt );

    if  ( c1 < c2- 0.05 )
	{ return -1;	}
    if  ( c1 > c2+ 0.05 )
	{ return  1;	}

    l1= AT2_X( ls1->ls2diX0, ls1->ls2diY0, bmSortAt );
    l2= AT2_X( ls2->ls2diX0, ls2->ls2diY0, bmSortAt );

    if  ( l1 < l2 )
	{ return -1;	}
    if  ( l1 > l2 )
	{ return  1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Start a FindLineSegments job.					*/
/*									*/
/************************************************************************/

static void bmInitFindLineSegmentsJob(	FindLineSegmentsJob *	flsj )
    {
    flsj->flsjPixelSize= 0;

    flsj->flsjSelX0= 0;
    flsj->flsjSelY0= 0;
    flsj->flsjSelX1= 0;
    flsj->flsjSelY1= 0;

    bmInitRasterImage( &(flsj->flsjRiSel) );
    bmInitRasterImage( &(flsj->flsjRiSegs) );

    flsj->flsjThrough= (void *)0;
    flsj->flsjSegFun= (BM_SEG_FUN)0;

    flsj->flsjSegmentCount= 0;
    flsj->flsjSegmentsAllocated= 0;
    flsj->flsjSegments= (LineSegment2DI *)0;

    flsj->flsjA= 0;
    flsj->flsjB= 0;
    flsj->flsjC= 0;
    geoInitAffineTransform2D( &(flsj->flsjTransform) );

    return;
    }

static int bmStartFindLineSegmentsJob(	FindLineSegmentsJob *	flsj,
					void *			through,
					BM_SEG_FUN		segFun,
					int			size,
					int			x0,
					int			y0,
					int			x1,
					int			y1 )
    {
    geoLineConstants( &(flsj->flsjA),  &(flsj->flsjB), &(flsj->flsjC),
							    x0, y0, x1, y1 );
    geoLineAffineTransform2D( &(flsj->flsjTransform),
				flsj->flsjA,  flsj->flsjB, flsj->flsjC );

    if  ( bmMorphoLineElement( &(flsj->flsjRiSel),
					    size, size, x0, y0, x1, y1 ) )
	{ LDEB(size); return -1;	}

    flsj->flsjPixelSize= size;

    flsj->flsjSelX0= x0;
    flsj->flsjSelY0= y0;
    flsj->flsjSelX1= x1;
    flsj->flsjSelY1= y1;

    flsj->flsjThrough= through;
    flsj->flsjSegFun= segFun;

    return 0;
    }

static void bmCleanFindLineSegmentsJob(	FindLineSegmentsJob *	flsj )
    {
    /**/
    bmCleanRasterImage( &(flsj->flsjRiSel) );

    /**/
    bmCleanRasterImage( &(flsj->flsjRiSegs) );

    /**/
    if  ( flsj->flsjSegments )
	{ free( flsj->flsjSegments );	}

    return;
    }

/************************************************************************/

#   if CHECK_SEGMENTS

static void bmCheckLineSegOrientation(	const FindLineSegmentsJob *	flsj )
    {
    int				to;
    LineSegment2DI *		lsTo;
    const AffineTransform2D *	at= &(flsj->flsjTransform);

    lsTo= flsj->flsjSegments;
    for ( to= 0; to < flsj->flsjSegmentCount; lsTo++, to++ )
	{
	double	c0= AT2_Y( lsTo->ls2diX0, lsTo->ls2diY0, at );
	double	l0= AT2_X( lsTo->ls2diX0, lsTo->ls2diY0, at );
	double	c1= AT2_Y( lsTo->ls2diX1, lsTo->ls2diY1, at );
	double	l1= AT2_X( lsTo->ls2diX1, lsTo->ls2diY1, at );

	if  ( c1 < c0- 0.05 )
	    { LFFDEB(to,c0,c1); continue;	}
	if  ( c1 > c0+ 0.05 )
	    { continue;				}

	if  ( l1 < l0 )
	    { LFFDEB(to,l0,l1); continue;	}
	if  ( l1 > l0 )
	    { continue;				}
	}

    return;
    }

static void bmCheckLineSegSort(	const FindLineSegmentsJob *	flsj )
    {
    int				to;
    LineSegment2DI *		lsTo;
    const AffineTransform2D *	at= &(flsj->flsjTransform);

    lsTo= flsj->flsjSegments;
    for ( to= 0; to < flsj->flsjSegmentCount- 1; lsTo++, to++ )
	{
	double	c0= AT2_Y( lsTo[0].ls2diX0, lsTo[0].ls2diY0, at );
	double	c1= AT2_Y( lsTo[1].ls2diX0, lsTo[1].ls2diY0, at );
	double	l0;
	double	l1;

	if  ( c1 < c0- 0.05 )
	    { LFFDEB(to,c0,c1); continue;	}
	if  ( c1 > c0+ 0.05 )
	    { continue;				}

	l0= AT2_X( lsTo[0].ls2diX0, lsTo[0].ls2diY0, at );
	l1= AT2_X( lsTo[1].ls2diX0, lsTo[1].ls2diY0, at );
	if  ( l1 < l0 )
	    { LFFDEB(to,l0,l1); continue;	}
	if  ( l1 > l0 )
	    { continue;				}
	}

    return;
    }

static void bmCheckLineSegChain(	const FindLineSegmentsJob *	flsj )
    {
    int				to;
    LineSegment2DI *		lsTo;
    const AffineTransform2D *	at= &(flsj->flsjTransform);

    lsTo= flsj->flsjSegments;
    for ( to= 0; to < flsj->flsjSegmentCount- 1; lsTo++, to++ )
	{
	double	c01= AT2_Y( lsTo[0].ls2diX1, lsTo[0].ls2diY1, at );
	double	l01= AT2_X( lsTo[0].ls2diX1, lsTo[0].ls2diY1, at );
	double	c11= AT2_Y( lsTo[1].ls2diX1, lsTo[1].ls2diY1, at );
	double	l11= AT2_X( lsTo[1].ls2diX1, lsTo[1].ls2diY1, at );

	if  ( c11 < c01- 0.05 )
	    { LFFDEB(to,c11,c01); continue;	}
	if  ( c11 > c01+ 0.05 )
	    { continue;				}

	if  ( l11 < l01 )
	    { LFFDEB(to,l11,l01); continue;	}
	if  ( l11 > l01 )
	    { continue;				}
	}

    return;
    }

#   endif

/************************************************************************/

static void bmChainLineSegments(	FindLineSegmentsJob *     flsj )
    {
    int				fr;
    const LineSegment2DI *	lsFr;

    int				to;
    LineSegment2DI *		lsTo;

    double			cTo0;
    double			lTo1;

    const AffineTransform2D *	at= &(flsj->flsjTransform);

    lsTo= flsj->flsjSegments;
    for ( to= 0; to < flsj->flsjSegmentCount; lsTo++, to++ )
	{
	double	c0= AT2_Y( lsTo->ls2diX0, lsTo->ls2diY0, at );
	double	c1= AT2_Y( lsTo->ls2diX1, lsTo->ls2diY1, at );
	double	l0;
	double	l1;

	if  ( c1 > c0- 0.05 )
	    { continue;	}
	if  ( c1 < c0+ 0.05 )
	    {
	    int	c;

	    c= lsTo->ls2diX0; lsTo->ls2diX0= lsTo->ls2diX1; lsTo->ls2diX1= c;
	    c= lsTo->ls2diY0; lsTo->ls2diY0= lsTo->ls2diY1; lsTo->ls2diY1= c;
	    continue;
	    }

	l0= AT2_X( lsTo->ls2diX0, lsTo->ls2diY0, at );
	l1= AT2_X( lsTo->ls2diX1, lsTo->ls2diY1, at );
	if  ( l1 > l0 )
	    { continue;	}
	if  ( l1 < l0 )
	    {
	    int	c;

	    c= lsTo->ls2diX0; lsTo->ls2diX0= lsTo->ls2diX1; lsTo->ls2diX1= c;
	    c= lsTo->ls2diY0; lsTo->ls2diY0= lsTo->ls2diY1; lsTo->ls2diY1= c;
	    continue;
	    }
	}

#   if CHECK_SEGMENTS
    bmCheckLineSegOrientation( flsj );
#   endif

    bmSortAt= at;
    qsort( flsj->flsjSegments, flsj->flsjSegmentCount,
					sizeof(LineSegment2DI),
					bmCompareLineSegments );

#   if CHECK_SEGMENTS
    bmCheckLineSegSort( flsj );
    bmCheckLineSegChain( flsj );
#   endif

    to= 0; lsTo= flsj->flsjSegments;
    lsTo= flsj->flsjSegments;

    cTo0= AT2_Y( lsTo->ls2diX0, lsTo->ls2diY0, at );
    lTo1= AT2_X( lsTo->ls2diX1, lsTo->ls2diY1, at );

    fr= 0; lsFr= flsj->flsjSegments;

    lsFr= flsj->flsjSegments+ 1;
    for ( fr= 1; fr < flsj->flsjSegmentCount; lsFr++, fr++ )
	{
	double	cFr0= AT2_Y( lsFr->ls2diX0, lsFr->ls2diY0, at );
	double	lFr0= AT2_X( lsFr->ls2diX0, lsFr->ls2diY0, at );
	double	lFr1= AT2_X( lsFr->ls2diX1, lsFr->ls2diY1, at );

	if  ( cFr0 < cTo0- 0.05 )
	    { LLFFDEB(fr,to,cFr0,cTo0);	}
	if  ( cFr0 > cTo0+ 0.05 )
	    {
	    lsTo++; to++; lsTo[0]= lsFr[0];
	    cTo0= AT2_Y( lsTo->ls2diX0, lsTo->ls2diY0, at );
	    lTo1= AT2_X( lsTo->ls2diX1, lsTo->ls2diY1, at );

	    continue;
	    }

	if  ( lFr1 < lTo1- 0.05 )
	    { LLFFDEB(fr,to,lFr1,lTo1); }
	if  ( lFr0 > lTo1+ 0.05 )
	    {
	    lsTo++; to++; lsTo[0]= lsFr[0];
	    cTo0= AT2_Y( lsTo->ls2diX0, lsTo->ls2diY0, at );
	    lTo1= AT2_X( lsTo->ls2diX1, lsTo->ls2diY1, at );

	    continue;
	    }

	lsTo->ls2diX1= lsFr->ls2diX1;
	lsTo->ls2diY1= lsFr->ls2diY1;
	lTo1= AT2_X( lsTo->ls2diX1, lsTo->ls2diY1, at );
	}

    flsj->flsjSegmentCount= to+ 1;

    lsTo= flsj->flsjSegments;
    for ( to= 0; to < flsj->flsjSegmentCount; lsTo++, to++ )
	{
	if  ( lsTo->ls2diX1 > lsTo->ls2diX0 )
	    { continue;	}
	if  ( lsTo->ls2diX1 < lsTo->ls2diX0 )
	    {
	    int	c;

	    c= lsTo->ls2diX0; lsTo->ls2diX0= lsTo->ls2diX1; lsTo->ls2diX1= c;
	    c= lsTo->ls2diY0; lsTo->ls2diY0= lsTo->ls2diY1; lsTo->ls2diY1= c;
	    continue;
	    }

	if  ( lsTo->ls2diY1 > lsTo->ls2diY0 )
	    { continue;	}
	if  ( lsTo->ls2diY1 < lsTo->ls2diY0 )
	    {
	    int	c;

	    c= lsTo->ls2diX0; lsTo->ls2diX0= lsTo->ls2diX1; lsTo->ls2diX1= c;
	    c= lsTo->ls2diY0; lsTo->ls2diY0= lsTo->ls2diY1; lsTo->ls2diY1= c;
	    continue;
	    }
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Find the line segments of a certain fixed length in an image.	*/
/*									*/
/*  This is done by eroding the image with structuring elements that	*/
/*  correspond to the possible line segments through the center of	*/
/*  a square with the given size.					*/
/*									*/
/*  1)  Only operate on 2 color images.					*/
/*  2)  For all possible line sigments of this size see whether they	*/
/*	match.								*/
/*  2a) Vertical segments: from top to bottom.				*/
/*  2b) Horizontal segments: from left to right.			*/
/*									*/
/*  3)  For all black pixels.. draw a line: invert for BMcoWHITEBLACK.	*/
/*									*/
/************************************************************************/

static int bmRunFindLineSegmentsJob(	FindLineSegmentsJob *		flsj,
					const RasterImage *		riIn,
					void *				through,
					BM_SEG_FUN			segFun,
					int				size,
					int				x0,
					int				y0,
					int				x1,
					int				y1 )
    {
    int				fr;
    const LineSegment2DI *	lsFr;

    if  ( bmStartFindLineSegmentsJob( flsj, through, segFun,
						    size, x0, y0, x1, y1 ) )
	{ LDEB(size); return -1;	}

#   if 0
    {
    const char *	name= "/tmp/segment.png";
    int			format;

    format=  bmSuggestFormat( name, -1, &(flsj->flsjBdSel) );
    bmWrite( name, flsj->flsjBufSel, &(flsj->flsjBdSel), format, 1.0 );
    }
#   endif

    if  ( bmMorphoErode( &(flsj->flsjRiSegs), riIn,
			    &(flsj->flsjRiSel), size/ 2, size/ 2 ) )
	{ LDEB(1); return -1;	}

#   if 0
    {
    const char *	name= "/tmp/eroded.png";
    int			format;

    format=  bmSuggestFormat( name, -1, &(flsj->flsjBdSegs) );
    bmWrite( name, flsj->flsjBufSegs, &(flsj->flsjBdSegs), format, 1.0 );
    }
#   endif

    /*  3  */
    if  ( flsj->flsjRiSegs.riDescription.bdColorEncoding == BMcoWHITEBLACK )
	{
	bmForAll1Pixels( &(flsj->flsjRiSegs),
				    0xff, (void *)flsj, bmCollectLineSegment );
	}
    else{
	bmForAll1Pixels( &(flsj->flsjRiSegs),
				    0x00, (void *)flsj, bmCollectLineSegment );
	}

    if  ( flsj->flsjSegmentCount > 0 )
	{ bmChainLineSegments( flsj );	}

    lsFr= flsj->flsjSegments;
    for ( fr= 0; fr < flsj->flsjSegmentCount; lsFr++, fr++ )
	{
/*
if(lsFr->ls2diX1-lsFr->ls2diX0<=x1-x0&&
   lsFr->ls2diY1-lsFr->ls2diY0<=y1-y0)
   {continue;}
*/

	(*flsj->flsjSegFun)( flsj->flsjThrough,
					lsFr->ls2diX0, lsFr->ls2diY0,
					lsFr->ls2diX1, lsFr->ls2diY1 );
	}

    bmCleanFindLineSegmentsJob( flsj );
    bmInitFindLineSegmentsJob( flsj );

    return 0;
    }

int bmFindLineSegments(		const RasterImage *		riIn,
				int				size,
				void *				through,
				BM_SEG_FUN			segFun )
    {
    const BitmapDescription *	bdIn= &(riIn->riDescription);
    int				rval= 0;
    FindLineSegmentsJob		flsj;

    int				i;

    bmInitFindLineSegmentsJob( &flsj );

    /*  1  */
    if  ( bdIn->bdBitsPerPixel != 1 )
	{ LDEB(bdIn->bdBitsPerPixel); rval= -1; goto ready;	}

    /*  2a  */
    for ( i= 0; i < size; i++ )
	{
	if  ( bmRunFindLineSegmentsJob( &flsj, riIn, through,
					segFun, size,
					i, 0, size- i- 1, size- 1 ) )
	    { LDEB(i); rval= -1; goto ready;	}
	}

    /*  2b  */
    for ( i= 1; i < size- 1; i++ )
	{
	if  ( bmRunFindLineSegmentsJob( &flsj, riIn, through,
					segFun, size,
					0, i, size- 1, size- i- 1 ) )
	    { LDEB(i); rval= -1; goto ready;	}
	}

  ready:

    bmCleanFindLineSegmentsJob( &flsj );

    return rval;
    }

