/************************************************************************/
/*									*/
/*  Find out how to draw a shape arrow.					*/
/*									*/
/************************************************************************/

#   include	"docBaseConfig.h"

#   include	<geo2DDouble.h>
#   include	<geoAffineTransform.h>

#   include	"docDrawShapeArrow.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Return sizes for arrows in twips.					*/
/*									*/
/************************************************************************/

void docShapeArrowSizesTwips(	DrawShapeArrow *	dsa,
				int			lineWidth,
				double			xfac,
				const Point2DD *	shaft,
				const ShapeArrow *	sa )
    {
    double		width;
    double		fac= 15.0* xfac;

    AffineTransform2D	at;
    AffineTransform2D	att;

    dsa->dsaArrow= *sa;
    dsa->dsaShaft[0]= shaft[0];
    dsa->dsaShaft[1]= shaft[1];

    switch( sa->saArrowLength )
	{
	case DSarrowSHORT:
	    dsa->dsaLength= fac* 6;
	    dsa->dsaLength2= fac* 9;
	    break;
	case DSarrowMEDIUM:
	    dsa->dsaLength= fac* 8;
	    dsa->dsaLength2= fac* 12;
	    break;
	case DSarrowLONG:
	    dsa->dsaLength= fac* 10;
	    dsa->dsaLength2= fac* 15;
	    break;
	default:
	    LDEB(sa->saArrowLength);
	    dsa->dsaLength= fac* 9;
	    dsa->dsaLength2= fac* 13;
	}

    switch( sa->saArrowWidth )
	{
	case DSarrowNARROW:
	    dsa->dsaWidth= fac* 4;
	    break;
	case DSarrowMEDIUM:
	    dsa->dsaWidth= fac* 6;
	    break;
	case DSarrowWIDE:
	    dsa->dsaWidth= fac* 8;
	    break;
	default:
	    LDEB(sa->saArrowWidth);
	    dsa->dsaWidth= fac* 6;
	}

    width= dsa->dsaWidth+ 0.5* xfac* lineWidth;

    geoRotationAffineTransform2DAtan( &at,
			    shaft[1].y- shaft[0].y,
			    shaft[1].x- shaft[0].x );
    geoTranslationAffineTransform2D( &att, shaft[1].x, shaft[1].y );
    geoAffineTransform2DProduct( &at, &att, &at );

    dsa->dsaPath[0].x= AT2_X( 0, 0, &at );
    dsa->dsaPath[0].y= AT2_Y( 0, 0, &at );

    switch( sa->saArrowHead )
	{
	case DSarrowNONE:
	    break;

	case DSarrowARROW:
	    dsa->dsaPath[1].x= AT2_X( -dsa->dsaLength,  width, &at );
	    dsa->dsaPath[1].y= AT2_Y( -dsa->dsaLength,  width, &at );
	    dsa->dsaPath[2].x= AT2_X( -dsa->dsaLength, -width, &at );
	    dsa->dsaPath[2].y= AT2_Y( -dsa->dsaLength, -width, &at );
	    dsa->dsaPath[3]= dsa->dsaPath[0];
	    dsa->dsaPathLength= 3;

	    dsa->dsaShaft[1].x= AT2_X( -0.7* dsa->dsaLength,  0, &at );
	    dsa->dsaShaft[1].y= AT2_Y( -0.7* dsa->dsaLength,  0, &at );
	    break;

	case DSarrowSTEALTH_ARROW:
	    dsa->dsaPath[1].x= AT2_X( -dsa->dsaLength2,  width, &at );
	    dsa->dsaPath[1].y= AT2_Y( -dsa->dsaLength2,  width, &at );
	    dsa->dsaPath[2].x= AT2_X( -dsa->dsaLength,  0, &at );
	    dsa->dsaPath[2].y= AT2_Y( -dsa->dsaLength,  0, &at );
	    dsa->dsaPath[3].x= AT2_X( -dsa->dsaLength2, -width, &at );
	    dsa->dsaPath[3].y= AT2_Y( -dsa->dsaLength2, -width, &at );
	    dsa->dsaPath[4]= dsa->dsaPath[0];
	    dsa->dsaPathLength= 4;

	    dsa->dsaShaft[1].x= AT2_X( -0.8* dsa->dsaLength,  0, &at );
	    dsa->dsaShaft[1].y= AT2_Y( -0.8* dsa->dsaLength,  0, &at );
	    break;

	case DSarrowDIAMOND:
	    dsa->dsaPath[1].x= AT2_X( -dsa->dsaLength/ 2,  width, &at );
	    dsa->dsaPath[1].y= AT2_Y( -dsa->dsaLength/ 2,  width, &at );
	    dsa->dsaPath[2].x= AT2_X( -dsa->dsaLength,  0, &at );
	    dsa->dsaPath[2].y= AT2_Y( -dsa->dsaLength,  0, &at );
	    dsa->dsaPath[3].x= AT2_X( -dsa->dsaLength/ 2, -width, &at );
	    dsa->dsaPath[3].y= AT2_Y( -dsa->dsaLength/ 2, -width, &at );
	    dsa->dsaPath[4]= dsa->dsaPath[0];
	    dsa->dsaPathLength= 4;
	    break;

	case DSarrowOPEN_ARROW:
	    dsa->dsaPath[0].x= AT2_X( -dsa->dsaLength,  width, &at );
	    dsa->dsaPath[0].y= AT2_Y( -dsa->dsaLength,  width, &at );
	    dsa->dsaPath[1].x= AT2_X( 0, 0, &at );
	    dsa->dsaPath[1].y= AT2_Y( 0, 0, &at );
	    dsa->dsaPath[2].x= AT2_X( -dsa->dsaLength, -width, &at );
	    dsa->dsaPath[2].y= AT2_Y( -dsa->dsaLength, -width, &at );
	    dsa->dsaPathLength= 3;
	    break;

	case DSarrowOVAL:
	    dsa->dsaArrowRectangle.x0=
			    AT2_X( -dsa->dsaLength/ 2,  width, &at );
	    dsa->dsaArrowRectangle.y0=
			    AT2_Y( -dsa->dsaLength/ 2,  width, &at );
	    dsa->dsaArrowRectangle.x1=
			    AT2_X(  dsa->dsaLength/ 2, -width, &at );
	    dsa->dsaArrowRectangle.y1=
			    AT2_Y(  dsa->dsaLength/ 2, -width, &at );
	    break;

	case DSarrowCHEVRON_ARROW:
	case DSarrow2CHEVRON_ARROW:
	default:
	    LDEB(sa->saArrowHead); break;
	}
    return;
    }

