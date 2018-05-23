/************************************************************************/
/*									*/
/*  Infrastructure for printing 'shapes'.				*/
/*									*/
/************************************************************************/

#   include	"docLayoutConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<math.h>

#   include	<sioGeneral.h>
#   include	<geo2DDouble.h>
#   include	<psPrintShape.h>
#   include	<docObjectProperties.h>
#   include	<docShape.h>
#   include	<docDrawShapeArrow.h>

#   include	"docDraw.h"
#   include	"docPsPrintImpl.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Include procedures to draw the paths that make up the simple shapes	*/
/*  in the PostScript printout.						*/
/*									*/
/************************************************************************/

static void docPsEmitVertices(		SimpleOutputStream *		sos,
					const AffineTransform2D *	at,
					double *			pX,
					double *			pY,
					const Point2DI *		sv,
					int				nv,
					double				x0,
					double				y0,
					double				xm,
					double				ym )
    {
    int			i;

    for ( i= 1; i < nv; i++ )
	{
	double		x;
	double		y;

	double		x1;
	double		y1;

	x= sv[i].x- xm;
	y= sv[i].y- ym;
	x1= AT2_X( x, y, at );
	y1= AT2_Y( x, y, at );

	sioOutPrintf( sos, "%g %g rl", x1- x0, y1- y0 );

	if  ( i % 8 == 7 )
	    { sioOutPrintf( sos, "\n " );	}
	else{ sioOutPrintf( sos, " " );		}

	x0= x1; y0= y1;
	}

    *pX= x0; *pY= y0;
    }

static void docPsEmitFirstVertex(	SimpleOutputStream *		sos,
					const AffineTransform2D *	at,
					double *			pX,
					double *			pY,
					const Point2DI *		sv,
					double				xm,
					double				ym )
    {
    double	x;
    double	y;
    double	x0;
    double	y0;

    x= sv[0].x- xm;
    y= sv[0].y- ym;
    x0= AT2_X( x, y, at );
    y0= AT2_Y( x, y, at );

    sioOutPrintf( sos, " %g %g bp ", x0, y0 );

    *pX= x0; *pY= y0;
    }

static void docPsPrintShapePathLow(	SimpleOutputStream *		sos,
					const AffineTransform2D *	at,
					const ShapePath *		sp )
    {
    double	xm= 0.5* sp->spXSize;
    double	ym= 0.5* sp->spYSize;

    double	x0;
    double	y0;

    docPsEmitFirstVertex( sos, at, &x0, &y0, sp->spVertices, xm, ym );
    docPsEmitVertices( sos, at, &x0, &y0, sp->spVertices, sp->spVertexCount,
							    x0, y0, xm, ym );

    if  ( sp->spClosed )
	{ sioOutPrintf( sos, " closepath " );	}

    return;
    }

static int docPsPrintArrowHead(	PrintingState *			ps,
				int				lineWidth,
				const DrawShapeArrow *		dsa )
    {
    const ShapeArrow *		sa= &(dsa->dsaArrow);

    SimpleOutputStream *	sos= ps->psSos;

    switch( sa->saArrowHead )
	{
	int	i;

	case DSarrowNONE:
	    break;

	case DSarrowARROW:
	case DSarrowSTEALTH_ARROW:
	case DSarrowDIAMOND:
	    sioOutPrintf( sos, "%g %g bp ",
				    dsa->dsaPath[0].x,
				    dsa->dsaPath[0].y );

	    for ( i= 1; i < dsa->dsaPathLength; i++ )
		{
		sioOutPrintf( sos, "%g %g rl ",
				    dsa->dsaPath[i].x- dsa->dsaPath[i-1].x,
				    dsa->dsaPath[i].y- dsa->dsaPath[i-1].y );
		}

	    sioOutPrintf( sos, "closepath fill %% ARROW\n" );
	    break;

	case DSarrowOPEN_ARROW:
	    sioOutPrintf( sos, "[] 0 setdash\n" );

	    sioOutPrintf( sos, "%g %g bp ",
				dsa->dsaPath[0].x,
				dsa->dsaPath[0].y );

	    for ( i= 1; i < dsa->dsaPathLength; i++ )
		{
		sioOutPrintf( sos, "%g %g rl ",
				dsa->dsaPath[i].x- dsa->dsaPath[i-1].x,
				dsa->dsaPath[i].y- dsa->dsaPath[i-1].y );
		}

	    sioOutPrintf( sos, "stroke\n" );
	    break;

	case DSarrowOVAL:
	case DSarrowCHEVRON_ARROW:
	case DSarrow2CHEVRON_ARROW:
	default:
	    LDEB(sa->saArrowHead);
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Draw a can.								*/
/*									*/
/************************************************************************/

static void docPsCanPath(	SimpleOutputStream *		sos,
				const DocumentRectangle *	drCan,
				const char *			oper )
    {
    int		w= drCan->drX1- drCan->drX0;
    int		h= drCan->drY1- drCan->drY0;
    int		y;

    sioOutPrintf( sos, "gsave %d %d translate ", drCan->drX0, drCan->drY0 );

    sioOutPrintf( sos, "1 %g scale ", h/ ( 4.0* w ) );
    y= w/ 2;
    sioOutPrintf( sos, "%d %d bp ", w, y );
    sioOutPrintf( sos, "%d %d %d 0 360 arc ", w/ 2, y, w/ 2 );
    sioOutPrintf( sos, "%d %d rl ", 0, 3* w );

    y= ( 7* w )/ 2;
    sioOutPrintf( sos, "%d %d %d 0 -180 arc ", w/ 2, y, w/ 2 );
    sioOutPrintf( sos, "%d %d rl ", 0, -3* w );
    sioOutPrintf( sos, "%s grestore\n", oper );

    return;
    }

/************************************************************************/
/*									*/
/*  Draw a cube.							*/
/*									*/
/************************************************************************/

static void docPsCubePath(	SimpleOutputStream *		sos,
				const DocumentRectangle *	drCube,
				const char *			oper )
    {
    int		w= drCube->drX1- drCube->drX0;
    int		h= drCube->drY1- drCube->drY0;

    sioOutPrintf( sos, "gsave %d %d translate ", drCube->drX0, drCube->drY0 );

    sioOutPrintf( sos, "%d %d bp ", ( 3* w )/4, h/ 4 );
    sioOutPrintf( sos, "%d %d rl ", ( -3* w )/4, 0 );
    sioOutPrintf( sos, "%d %d rl ", 0, ( 3* h )/4 );
    sioOutPrintf( sos, "%d %d rl ", ( 3* w )/4, 0 );
    sioOutPrintf( sos, "%d %d rl ", 0, ( -3* h )/4 );

    sioOutPrintf( sos, "%d %d rl ", w/ 4, -h/ 4 );
    sioOutPrintf( sos, "%d %d rl ", 0, ( 3* h )/4 );
    sioOutPrintf( sos, "%d %d rl ", -w/ 4, h/ 4 );
    sioOutPrintf( sos, "%d %d rl ", 0, ( -3* h )/4 );

    sioOutPrintf( sos, "%d %d rl ", w/ 4, -h/ 4 );
    sioOutPrintf( sos, "%d %d rl ", ( -3* w )/4, 0 );
    sioOutPrintf( sos, "%d %d rl ", -w/ 4, h/ 4 );
    sioOutPrintf( sos, "%d %d rl ", ( 3* w )/4, 0 );

    sioOutPrintf( sos, "%s grestore\n", oper );

    return;
    }

/************************************************************************/
/*									*/
/*  Line needed?							*/
/*									*/
/************************************************************************/

static void docPsPrintGetLine(	int *				pLine,
				int *				pLineWidth,
				const DrawingShape *		ds,
				DrawingContext *		dc,
				PrintingState *			ps,
				const AffineTransform2D *	at )
    {
    RGB8Color			rgb8Stroke;
    const ShapeDrawing *	sd= &(ds->dsDrawing);

    docDrawShapeGetLine( pLine, &rgb8Stroke, ds, dc, ps );

    if  ( *pLine )
	{
	SimpleOutputStream *	sos= ps->psSos;
	int			widthTwips;

	widthTwips= EMUtoTWIPS( sd->sdLineWidthEmu );

	sioOutPrintf( ps->psSos, "%d setlinewidth\n", widthTwips );

	switch( sd->sdLineDashing )
	    {
	    case DSdashSOLID:
		sioOutPrintf( sos, "[] 0 setdash\n" );
		break;

	    case DSdashDASHED:
	    case DSdashDASHED_X:
	    case DSdashDASHED_L:
		sioOutPrintf( sos, "[%d %d] 0 setdash\n",
						6* widthTwips,
						4* widthTwips );
		break;

	    case DSdashDOT:
	    case DSdashDOT_X:
		sioOutPrintf( sos, "[%d %d] 0 setdash\n",
						1* widthTwips,
						2* widthTwips );
		break;

	    case DSdashDASHDOT:
	    case DSdashDASHDOT_X:
	    case DSdashDASHDOT_L:
		sioOutPrintf( sos, "[%d %d %d %d] 0 setdash\n",
						6* widthTwips,
						2* widthTwips,
						1* widthTwips,
						2* widthTwips );
		break;

	    case DSdashDASHDOTDOT:
	    case DSdashDASHDOTDOT_L:
		sioOutPrintf( sos, "[%d %d %d %d %d %d] 0 setdash\n",
						6* widthTwips,
						2* widthTwips,
						1* widthTwips,
						2* widthTwips,
						1* widthTwips,
						2* widthTwips );
		break;

	    default:
		LDEB(sd->sdLineDashing);
		sioOutPrintf( sos, "[] 0 setdash\n" );
	    }

	if  ( pLineWidth )
	    { *pLineWidth= widthTwips;	}
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Draw the form given by the path for an individual shape.		*/
/*									*/
/************************************************************************/

static int docPsPrintShapePath(	const DrawingShape *		ds,
				const ShapePath *		sp,
				const DocumentRectangle *	dr,
				DrawingContext *		dc,
				PrintingState *			ps )
    {
    SimpleOutputStream *	sos= ps->psSos;
    const ShapeDrawing *	sd= &(ds->dsDrawing);

    int				rval= 0;
    AffineTransform2D		at;

    int				line= 0;
    int				lineWidth= 0;

    int				drawHeadArrow= 0;
    DrawShapeArrow		dsaHead;
    Point2DD			shaftHead[2];
    int				drawTailArrow= 0;
    DrawShapeArrow		dsaTail;
    Point2DD			shaftTail[2];

    const Point2DI *		sv= sp->spVertices;
    int				nv= sp->spVertexCount;
    double			xm= 0.5* sp->spXSize;
    double			ym= 0.5* sp->spYSize;

    int				from= 0;
    int				upto= sp->spVertexCount;

    double			x0;
    double			y0;

    docShapeStartShapeTransform( &at, ds, dr, sp->spXSize, sp->spYSize );

    docPsPrintGetLine( &line, &lineWidth, ds, dc, ps, (AffineTransform2D *)0 );
    if  ( line && ! sp->spClosed && nv >= 2 )
	{
	if  ( sd->sdLineHeadArrow.saArrowHead != DSarrowNONE )
	    {
	    drawHeadArrow= 1;
	    from++;

	    shaftHead[0].x= AT2_X(sv[1].x- xm,sv[1].y- ym, &at );
	    shaftHead[0].y= AT2_Y(sv[1].x- xm,sv[1].y- ym, &at );
	    shaftHead[1].x= AT2_X(sv[0].x- xm,sv[0].y- ym, &at );
	    shaftHead[1].y= AT2_Y(sv[0].x- xm,sv[0].y- ym, &at );

	    docShapeArrowSizesTwips( &dsaHead, lineWidth, 1.0,
					shaftHead, &(sd->sdLineHeadArrow) );
	    }

	if  ( sd->sdLineTailArrow.saArrowHead != DSarrowNONE )
	    {
	    drawTailArrow= 1;
	    upto--;

	    shaftTail[0].x= AT2_X(sv[nv-2].x- xm,sv[nv-2].y- ym, &at );
	    shaftTail[0].y= AT2_Y(sv[nv-2].x- xm,sv[nv-2].y- ym, &at );
	    shaftTail[1].x= AT2_X(sv[nv-1].x- xm,sv[nv-1].y- ym, &at );
	    shaftTail[1].y= AT2_Y(sv[nv-1].x- xm,sv[nv-1].y- ym, &at );

	    docShapeArrowSizesTwips( &dsaTail, lineWidth, 1.0,
					shaftTail, &(sd->sdLineTailArrow) );
	    }
	}

    if  ( drawHeadArrow )
	{
	x0= dsaHead.dsaShaft[1].x;
	y0= dsaHead.dsaShaft[1].y;

	sioOutPrintf( sos, " %g %g bp ", x0, y0 );
	}
    else{
	docPsEmitFirstVertex( sos, &at, &x0, &y0, sp->spVertices, xm, ym );
	}

    docPsEmitVertices( sos, &at, &x0, &y0, sp->spVertices, upto,
							    x0, y0, xm, ym );

    if  ( drawTailArrow )
	{
	sioOutPrintf( sos, "%g %g rl ",
				dsaTail.dsaShaft[1].x- x0,
				dsaTail.dsaShaft[1].y- y0 );
	}

    if  ( sp->spClosed )
	{
	int		fill= 0;
	RGB8Color	rgb8Fill;

	sioOutPrintf( sos, " closepath " );

	docDrawShapeGetFill( &fill, &rgb8Fill, ds, dc, (void *)ps );
	if  ( fill )
	    { sioOutPrintf( sos, " gsave fill grestore\n" ); }
	}

    if  ( line )
	{ sioOutPrintf( sos, "stroke\n" );	}

    if  ( drawHeadArrow )
	{
	docPsPrintGetLine( &line, &lineWidth,
				    ds, dc, ps, (AffineTransform2D *)0 );

	docPsPrintArrowHead( ps, lineWidth, &dsaHead );
	}

    if  ( drawTailArrow )
	{
	docPsPrintGetLine( &line, &lineWidth,
				    ds, dc, ps, (AffineTransform2D *)0 );

	docPsPrintArrowHead( ps, lineWidth, &dsaTail );
	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Draw an individual shape.						*/
/*									*/
/************************************************************************/

int docPsPrintDrawDrawingShape(	const DocumentRectangle *	drTwips,
				int				page,
				DrawingShape *			ds,
				DrawingContext *		dc,
				void *				vps )
    {
    const ShapeDrawing *	sd= &(ds->dsDrawing);
    PrintingState *		ps= (PrintingState *)vps;
    SimpleOutputStream *	sos= ps->psSos;

    int				rval= 0;

    int				fill= 0;
    int				line= 0;

    AffineTransform2D		at;
    RGB8Color			rgb8Fill;

    switch( sd->sdShapeType )
	{
	case SHPtyGROUP:
	    /* Done by docDrawDrawingShape(). */
	    LSDEB(sd->sdShapeType,docShapeTypeString(sd->sdShapeType));
	    break;

	case 33:
	    if  ( docPsPrintShapePath( ds, &SP_33, drTwips, dc, ps ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case 34:
	    if  ( docPsPrintShapePath( ds, &SP_34, drTwips, dc, ps ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyPICTURE_FRAME:
	    {
	    const ShapePath *	sp= &SP_RECTANGLE;

	    docShapeStartShapeTransform( &at, ds, drTwips,
						    sp->spXSize, sp->spYSize );

	    docDrawShapeGetFill( &fill, &rgb8Fill, ds, dc, ps );
	    if  ( fill )
		{
		docPsPrintShapePathLow( sos, &at, sp );
		sioOutPrintf( sos, "fill\n" );
		}

	    if  ( ds->dsPictureProperties.pipType != DOCokUNKNOWN )
		{ docPsPrintShapeImage( ps, dc, ds, drTwips, &at ); }

	    docPsPrintGetLine( &line, (int *)0, ds, dc, ps, &at );
	    if  ( line )
		{
		docPsPrintShapePathLow( sos, &at, sp );
		sioOutPrintf( sos, "stroke\n" );
		}
	    }
	    break;

	case SHPtyCALLOUT_1:
	case SHPtyCALLOUT_90:
	    {
	    const ShapePath *	sp= &SP_RECTANGLE;

	    docShapeStartShapeTransform( &at, ds, drTwips,
						    sp->spXSize, sp->spYSize );

	    docDrawShapeGetFill( &fill, &rgb8Fill, ds, dc, ps );
	    if  ( fill )
		{
		docPsPrintShapePathLow( sos, &at, sp );
		sioOutPrintf( sos, "fill\n" );
		}

	    docPsPrintGetLine( &line, (int *)0, ds, dc, ps, &at );
	    if  ( line )
		{
		const int	SC= 21600; /* adjust to geoLeft etc? */

		sioOutPrintf( sos, "newpath %g %g moveto %g %g lineto stroke\n",
				sd->sdAdjustValue/  ( 1.0* SC )- 0.5,
				sd->sdAdjust2Value/ ( 1.0* SC )- 0.5,
				sd->sdAdjust3Value/ ( 1.0* SC )- 0.5,
				sd->sdAdjust4Value/ ( 1.0* SC )- 0.5 );
		}
	    }
	    break;

	case SHPtyRECTANGLE:
	case SHPtyFLOW_CHART_PROCESS:
	case SHPtyACCENT_BORDER_CALLOUT_90:
	case SHPtyTEXT_BOX:
	    if  ( docPsPrintShapePath( ds, &SP_RECTANGLE, drTwips, dc, ps ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyDIAMOND:
	case SHPtyFLOW_CHART_DECISION:
	    if  ( docPsPrintShapePath( ds, &SP_DIAMOND, drTwips, dc, ps ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyISOSCELES_TRIANGLE:
	case SHPtyFLOW_CHART_EXTRACT:
	    if  ( docPsPrintShapePath( ds, &SP_ISOSCELES_TRIANGLE,
							drTwips, dc, ps ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyFLOW_CHART_MERGE:
	    if  ( docPsPrintShapePath( ds, &SP_FLOW_CHART_MERGE,
							drTwips, dc, ps ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyRIGHT_TRIANGLE:
	    if  ( docPsPrintShapePath( ds, &SP_RIGHT_TRIANGLE,
							drTwips, dc, ps ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyPARALLELOGRAM:
	case SHPtyFLOW_CHART_INPUT_OUTPUT:
	    if  ( docPsPrintShapePath( ds, &SP_PARALLELOGRAM,
							drTwips, dc, ps ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyTRAPEZOID:
	case SHPtyFLOW_CHART_MANUAL_OPERATION:
	    if  ( docPsPrintShapePath( ds, &SP_TRAPEZOID, drTwips, dc, ps ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyHEXAGON:
	case SHPtyFLOW_CHART_PREPARATION:
	    if  ( docPsPrintShapePath( ds, &SP_HEXAGON, drTwips, dc, ps ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyOCTAGON:
	    if  ( docPsPrintShapePath( ds, &SP_OCTAGON, drTwips, dc, ps ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyPLUS_SIGN:
	    if  ( docPsPrintShapePath( ds, &SP_PLUS_SIGN, drTwips, dc, ps ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyARROW:
	    if  ( docPsPrintShapePath( ds, &SP_ARROW, drTwips, dc, ps ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyNOTCHED_RIGHT_ARROW:
	    if  ( docPsPrintShapePath( ds, &SP_NOTCHED_RIGHT_ARROW,
							drTwips, dc, ps ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyHOME_PLATE:
	    if  ( docPsPrintShapePath( ds, &SP_HOME_PLATE, drTwips, dc, ps ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyCHEVRON:
	    if  ( docPsPrintShapePath( ds, &SP_CHEVRON, drTwips, dc, ps ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyLEFT_ARROW:
	    if  ( docPsPrintShapePath( ds, &SP_LEFT_ARROW, drTwips, dc, ps ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyRIGHT_ARROW_CALLOUT:
	    if  ( docPsPrintShapePath( ds, &SP_RIGHT_ARROW_CALLOUT,
							drTwips, dc, ps ) )
		{ LDEB(1); rval= -1;	}
	    break;
	case SHPtyLEFT_ARROW_CALLOUT:
	    if  ( docPsPrintShapePath( ds, &SP_LEFT_ARROW_CALLOUT,
							drTwips, dc, ps ) )
		{ LDEB(1); rval= -1;	}
	    break;
	case SHPtyUP_ARROW_CALLOUT:
	    if  ( docPsPrintShapePath( ds, &SP_UP_ARROW_CALLOUT,
							drTwips, dc, ps ) )
		{ LDEB(1); rval= -1;	}
	    break;
	case SHPtyDOWN_ARROW_CALLOUT:
	    if  ( docPsPrintShapePath( ds, &SP_DOWN_ARROW_CALLOUT,
							drTwips, dc, ps ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyLEFT_RIGHT_ARROW_CALLOUT:
	    if  ( docPsPrintShapePath( ds, &SP_LEFT_RIGHT_ARROW_CALLOUT,
							drTwips, dc, ps ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyUP_DOWN_ARROW_CALLOUT:
	    if  ( docPsPrintShapePath( ds, &SP_UP_DOWN_ARROW_CALLOUT,
							drTwips, dc, ps ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyQUAD_ARROW_CALLOUT:
	    if  ( docPsPrintShapePath( ds, &SP_QUAD_ARROW_CALLOUT,
							drTwips, dc, ps ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyLEFT_RIGHT_ARROW:
	    if  ( docPsPrintShapePath( ds, &SP_LEFT_RIGHT_ARROW,
							drTwips, dc, ps ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyUP_ARROW:
	    if  ( docPsPrintShapePath( ds, &SP_UP_ARROW, drTwips, dc, ps ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyDOWN_ARROW:
	    if  ( docPsPrintShapePath( ds, &SP_DOWN_ARROW, drTwips, dc, ps ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyUP_DOWN_ARROW:
	    if  ( docPsPrintShapePath( ds, &SP_UP_DOWN_ARROW,
							drTwips, dc, ps ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyQUAD_ARROW:
	    if  ( docPsPrintShapePath( ds, &SP_QUAD_ARROW, drTwips, dc, ps ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyLEFT_RIGHT_UP_ARROW:
	    if  ( docPsPrintShapePath( ds, &SP_LEFT_RIGHT_UP_ARROW,
							drTwips, dc, ps ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyLEFT_UP_ARROW:
	    if  ( docPsPrintShapePath( ds, &SP_LEFT_UP_ARROW,
							drTwips, dc, ps ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyBENT_UP_ARROW:
	    if  ( docPsPrintShapePath( ds, &SP_BENT_UP_ARROW,
							drTwips, dc, ps ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyPENTAGON:
	    if  ( docPsPrintShapePath( ds, &SP_PENTAGON, drTwips, dc, ps ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtySTAR:
	    if  ( docPsPrintShapePath( ds, &SP_STAR, drTwips, dc, ps ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtySEAL4:
	    if  ( docPsPrintShapePath( ds, &SP_SEAL4, drTwips, dc, ps ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case 32:
	case SHPtyLINE:
	    if  ( docPsPrintShapePath( ds, &SP_LINE, drTwips, dc, ps ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyFLOW_CHART_MANUAL_INPUT:
	    if  ( docPsPrintShapePath( ds, &SP_FLOW_CHART_MANUAL_INPUT,
							drTwips, dc, ps ) )
		{ LDEB(1); rval= -1;	}
	    break;
	case SHPtyFLOW_CHART_OFF_PAGE_CONNECTOR:
	    if  ( docPsPrintShapePath( ds, &SP_FLOW_CHART_OFF_PAGE_CONNECTOR,
							drTwips, dc, ps ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyFLOW_CHART_PUNCHED_CARD:
	    if  ( docPsPrintShapePath( ds, &SP_FLOW_CHART_PUNCHED_CARD,
							drTwips, dc, ps ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyWEDGE_RECT_CALLOUT:
	    if  ( docPsPrintShapePath( ds, &SP_WEDGE_RECT_CALLOUT,
							drTwips, dc, ps ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyELLIPSE:
	case SHPtyFLOW_CHART_CONNECTOR:
	    {
	    DocumentRectangle	drEllipse;
	    int			w;
	    int			h;
	    int			r;

	    docShapeStartShapeTransform( &at, ds, drTwips, 1, 1 );

	    drEllipse.drX0= AT2_X( -0.5, -0.5, &at );
	    drEllipse.drY0= AT2_Y( -0.5, -0.5, &at );
	    drEllipse.drX1= AT2_X(  0.5,  0.5, &at );
	    drEllipse.drY1= AT2_Y(  0.5,  0.5, &at );

	    w= ( drEllipse.drX1- drEllipse.drX0 );
	    h= ( drEllipse.drY1- drEllipse.drY0 );
	    r= w/ 2;

	    sioOutPrintf( sos, "gsave %d %d translate ",
				( drEllipse.drX0+ drEllipse.drX1 )/ 2,
				( drEllipse.drY0+ drEllipse.drY1 )/ 2 );
	    if  ( w > h )
		{
		sioOutPrintf( sos, "1 %g scale ", ( 1.0* h )/ w );
		r= w/ 2;
		}
	    else{
		if  ( h > w )
		    {
		    sioOutPrintf( sos, "%g 1 scale ", ( 1.0* w )/ h );
		    r= h/ 2;
		    }
		}

	    sioOutPrintf( sos, "newpath 0 0 %d 0 360 arc\n", r );

	    docDrawShapeGetFill( &fill, &rgb8Fill, ds, dc, ps );
	    if  ( fill )
		{ sioOutPrintf( sos, "gsave fill grestore " ); }
	    docPsPrintGetLine( &line, (int *)0, ds, dc, ps, &at );
	    if  ( line )
		{ sioOutPrintf( sos, "stroke " );	}
	    }
	    sioOutPrintf( sos, "grestore\n" );

	    break;

	case SHPtyARC:
	    docShapeStartShapeTransform( &at, ds, drTwips, 2, 2 );

	    sioOutPrintf( sos, "newpath -1 0 1 -90 0 arc\n" );

	    docDrawShapeGetFill( &fill, &rgb8Fill, ds, dc, ps );
	    if  ( fill )
		{ sioOutPrintf( sos, "gsave fill grestore\n" ); }
	    docPsPrintGetLine( &line, (int *)0, ds, dc, ps, &at );
	    if  ( line )
		{ sioOutPrintf( sos, "stroke\n" );	}

	    break;

	case SHPtyROUND_RECTANGLE:
	case SHPtyFLOW_CHART_ALTERNATE_PROCESS:
	    {
	    DocumentRectangle	drRRect;
	    int			w;
	    int			h;
	    int			r;

	    docShapeStartShapeTransform( &at, ds, drTwips, 1, 1 );

	    drRRect.drX0= AT2_X( -0.5, -0.5, &at );
	    drRRect.drY0= AT2_Y( -0.5, -0.5, &at );
	    drRRect.drX1= AT2_X(  0.5,  0.5, &at );
	    drRRect.drY1= AT2_Y(  0.5,  0.5, &at );

	    w= ( drRRect.drX1- drRRect.drX0 );
	    h= ( drRRect.drY1- drRRect.drY0 );
	    r= sqrt( w*w+ h*h )/ 8;

	    docDrawShapeGetFill( &fill, &rgb8Fill, ds, dc, (void *)ps );
	    if  ( fill )
		{ psDrawRoundRectPath( sos, &drRRect, r, "cp fill\n" ); }

	    docPsPrintGetLine( &line, (int *)0, ds, dc, ps, &at );
	    if  ( line )
		{ psDrawRoundRectPath( sos, &drRRect, r, "stroke\n" ); }
	    }
	    break;

	case SHPtyCAN:
	case SHPtyFLOW_CHART_MAGNETIC_DISK:
	    {
	    DocumentRectangle	drCan;

	    docShapeStartShapeTransform( &at, ds, drTwips, 1, 1 );

	    drCan.drX0= AT2_X( -0.5, -0.5, &at );
	    drCan.drY0= AT2_Y( -0.5, -0.5, &at );
	    drCan.drX1= AT2_X(  0.5,  0.5, &at );
	    drCan.drY1= AT2_Y(  0.5,  0.5, &at );

	    docDrawShapeGetFill( &fill, &rgb8Fill, ds, dc, (void *)ps );
	    if  ( fill )
		{ docPsCanPath( sos, &drCan, "cp fill" );	}
	    docPsPrintGetLine( &line, (int *)0, ds, dc, ps, &at );
	    if  ( line )
		{ docPsCanPath( sos, &drCan, "stroke" );	}
	    }
	    break;

	case SHPtyCUBE:
	    {
	    DocumentRectangle	drCube;

	    docShapeStartShapeTransform( &at, ds, drTwips, 1, 1 );

	    drCube.drX0= AT2_X( -0.5, -0.5, &at );
	    drCube.drY0= AT2_Y( -0.5, -0.5, &at );
	    drCube.drX1= AT2_X(  0.5,  0.5, &at );
	    drCube.drY1= AT2_Y(  0.5,  0.5, &at );

	    docDrawShapeGetFill( &fill, &rgb8Fill, ds, dc, (void *)ps );
	    if  ( fill )
		{ docPsCubePath( sos, &drCube, "cp fill" );	}
	    docPsPrintGetLine( &line, (int *)0, ds, dc, ps, &at );
	    if  ( line )
		{ docPsCubePath( sos, &drCube, "stroke" );	}
	    }
	    break;

	case SHPtyRIGHT_BRACE:
	    {
	    docShapeStartShapeTransform( &at, ds, drTwips, 2, 2 );

	    docPsPrintGetLine( &line, (int *)0, ds, dc, ps, &at );

	    sioOutPrintf( sos, "gsave 0 -0.75 translate 1 0.25 scale" );
	    sioOutPrintf( sos, " newpath -1 0 1 270 360 arc" );
	    sioOutPrintf( sos, " stroke grestore\n" );

	    sioOutPrintf( sos, "newpath 0 -0.75 moveto 0 0.5 rlineto stroke\n" );

	    sioOutPrintf( sos, "gsave 0 -0.25 translate 1 0.25 scale" );
	    sioOutPrintf( sos, " newpath  1 0 1  90 180 arc" );
	    sioOutPrintf( sos, " stroke grestore\n" );

	    sioOutPrintf( sos, "gsave 0  0.25 translate 1 0.25 scale" );
	    sioOutPrintf( sos, " newpath  1 0 1 180 270 arc" );
	    sioOutPrintf( sos, " stroke grestore\n" );

	    sioOutPrintf( sos, "newpath 0  0.25 moveto 0 0.5 rlineto stroke\n" );
	    sioOutPrintf( sos, "gsave 0  0.75 translate 1 0.25 scale" );
	    sioOutPrintf( sos, " newpath -1 0 1   0  90 arc" );
	    sioOutPrintf( sos, " stroke grestore\n" );
	    }
	    break;


	case SHPtyFREEFORM_OR_NON_AUTOSHAPE:
	    if  ( sd->sdVertexCount > 1 )
		{
		int		i;

		docShapeStartShapeTransform( &at, ds, drTwips,
					drTwips->drX1- drTwips->drX0,
					drTwips->drY1- drTwips->drY0 );

		sioOutPrintf( sos, "%d %d bp ",
			sd->sdVertices[0].x-
				( drTwips->drX1- drTwips->drX0 )/ 2,
			sd->sdVertices[0].y-
				( drTwips->drY1- drTwips->drY0 )/ 2 );

		for ( i= 1; i < sd->sdVertexCount; i++ )
		    {
		    sioOutPrintf( sos, "%d %d rl",
			    sd->sdVertices[i].x- sd->sdVertices[i- 1].x,
			    sd->sdVertices[i].y- sd->sdVertices[i- 1].y );

		    if  ( i % 8 == 7 || i == sd->sdVertexCount- 1 )
			{ sioOutPrintf( sos, "\n" );	}
		    else{ sioOutPrintf( sos, " " );	}
		    }

		docDrawShapeGetFill( &fill, &rgb8Fill, ds, dc, (void *)ps );
		if  ( fill )
		    { sioOutPrintf( sos, "gsave closepath fill grestore\n" ); }

		docPsPrintGetLine( &line, (int *)0, ds, dc, ps, &at );
		if  ( line )
		    { sioOutPrintf( sos, "stroke\n" );	}
		}
	    break;

	default:
	    LLSDEB(ds->dsShapeNumber,sd->sdShapeType,docShapeTypeString(sd->sdShapeType));
	    break;
	}

    /* Done by docDrawDrawingShape()....
    if  ( ds->dsDocumentTree.dtRoot )
	{}
    */

    ps->psLastPageMarked= ps->psPagesPrinted;
    return rval;
    }

