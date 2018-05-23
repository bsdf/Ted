#   include	<stdio.h>

#   include	<geoQuadTree.h>
#   include	<geoAffineTransform.h>
#   include	<geoLineFitter.h>

/************************************************************************/
/*									*/
/*  Find line segments and dump them.					*/
/*									*/
/************************************************************************/

typedef struct SegmentCluster
    {
    LineSegment2DI *	scSegments;
    int			scSegmentCount;
    DocumentRectangle	scRectangle;
    int			scX0;
    int			scY0;
    int			scX1;
    int			scY1;
    double		scLength;
    LineFitter		scLineFitter;
    double		scA;
    double		scB;
    double		scC;
    AffineTransform2D	scTransform;
    } SegmentCluster;

typedef struct ClusterSegments
    {
    FILE *		csFile;
    QuadTree *		csClusterTree;
    SegmentCluster *	csCurrentCluster;
    DocumentRectangle	csCurrentRectangle;
    int			csMerged;
    double		csDiagonal;
    int			csSegmentCount;
    int			csClusterCount;

    QuadTree *		csLongClusterTree;
    SegmentCluster **	csLongClusters;
    int			csLongClusterCount;

    int			csRectPaddingPercent;
    int			csDistance;
    } ClusterSegments;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/


extern void geoInitClusterSegments(		ClusterSegments *	cs );
extern void geoCleanClusterSegments(		ClusterSegments *	cs );

extern int geoClusterSegmentsAddSegment(ClusterSegments *	cs,
					int			x0,
					int			y0,
					int			x1,
					int			y1 );

extern int geoClusterSegmentsAllocateTree(
					ClusterSegments *		cs,
					const DocumentRectangle *	dr );

extern int geoClusterSegmentsSetFile(
				ClusterSegments *		cs,
				const char *			fileName );

extern int geoRunClusterSegments(	ClusterSegments *		cs );
