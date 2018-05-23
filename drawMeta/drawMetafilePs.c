#   include	"drawMetaConfig.h"

#   include	<stddef.h>

#   include	"drawMetafileImpl.h"
#   include	"drawMetafilePs.h"
#   include	"drawWinMetaImpl.h"
#   include	<psFontMetrics.h>
#   include	<utilMatchFont.h>
#   include	<psPrint.h>
#   include	<psPrintShape.h>
#   include	<psShading.h>
#   include	<bmWmfIo.h>
#   include	<bmEmfIo.h>
#   include	<bmBitmapPrinter.h>
#   include	"drawMacPictImpl.h"

#   include	<math.h>

#   include	<appDebugon.h>

# if 0
#    define	WMFDEB(x)	(x)
# else
#    define	WMFDEB(x)	/*nothing*/
# endif

# if 0
#    define	WMFLOG(x)	(x)
# else
#    define	WMFLOG(x)	/*nothing*/
# endif

/************************************************************************/
/*									*/
/*  Play a windows metafile to PostScript.				*/
/*									*/
/************************************************************************/

static void appMetaSetColorPs(		PrintingState *		ps,
					DeviceContext *		dc,
					const RGB8Color *	rgb8 )
    {
    if  ( dc->dcColorSet.rgb8Red == rgb8->rgb8Red	&&
	  dc->dcColorSet.rgb8Green == rgb8->rgb8Green	&&
	  dc->dcColorSet.rgb8Blue == rgb8->rgb8Blue	)
	{ return;	}

    psSetRgb8Color( ps, rgb8 );

    dc->dcColorSet= *rgb8;

    return;
    }

static void appMetafileRectPathPs(	PrintingState *			ps,
					const DocumentRectangle *	dr )
    {
    int		dx= dr->drX1- dr->drX0;
    int		dy= dr->drY1- dr->drY0;

    sioOutPrintf( ps->psSos, "%d %d bp ",  dr->drX0,  dr->drY0 );
    sioOutPrintf( ps->psSos, "%d %d rl ",  dx,   0 );
    sioOutPrintf( ps->psSos, "%d %d rl ",   0,  dy );
    sioOutPrintf( ps->psSos, "%d %d rl ", -dx,   0 );

    sioOutPrintf( ps->psSos, "closepath\n" );

    return;
    }

static void appMetaSetFontPs(		PrintingState *		ps,
					DeviceContext *		dc,
					TextAttribute		ta,
					int			fontSizeTwips )
    {
    ta.taFontSizeHalfPoints= fontSizeTwips/ 10;

    if  ( docEqualTextAttributes( &ta, &(dc->dcTextAttributeSet) ) )
	{ return;	}

    dc->dcTextAttributeSet= ta;
    psSetFont( ps, dc->dcAfi, &(dc->dcTextAttributeSet) );
    }

static void appMetaIssueDotsPs(		SimpleOutputStream *	sos,
					const unsigned char *	dots,
					int			dotCount )
    {
    const char *	before= "";

    sioOutPrintf( sos, "[" );
    while( dotCount-- > 0 )
	{ sioOutPrintf( sos, "%s%d", before, *(dots++) ); before= " ";	}
    sioOutPrintf( sos, "] 0 setdash\n" );

    return;
    }

static void appMetaSetPenPs(		DeviceContext *		dc,
					void *			through,
					const LogicalPen *	lp )
    {
    PrintingState *		ps= (PrintingState *)through;
    SimpleOutputStream *	sos= ps->psSos;
    int				widthTwips;

    unsigned char		dots[6];
    int				dotCount;

    if  ( dc->dcPenSet.lpStyle == lp->lpStyle		&&
	  dc->dcPenSet.lpWidth == lp->lpWidth		)
	{ return;	}

    widthTwips= appMetaGetPenWidth( dc, lp );
    dotCount= appMetaGetPenDots( dc, dots, lp );

    if  ( widthTwips > 0 )
	{
	sioOutPrintf( sos, "%d setlinewidth ", widthTwips );
	appMetaIssueDotsPs( sos, dots, dotCount );
	}

    dc->dcPenSet= *lp;

    return;
    }

/************************************************************************/

typedef struct PushedGraphics
    {
    RGB8Color		pgColor;
    } PushedGraphics;

static void appWinMetaPushGraphics(	PushedGraphics *	pg,
					const DeviceContext *	dc,
					PrintingState *		ps )
    {
    sioOutPrintf( ps->psSos, "gsave " );
    pg->pgColor= dc->dcColorSet;
    }

static void appWinMetaPushGraphicsForDraw(
					PushedGraphics *	pg,
					DeviceContext *		dc,
					PrintingState *		ps,
					int			fillInsides,
					int			drawBorders )
    {
    if  ( fillInsides )
	{
	if  ( ! dc->dcFillPattern )
	    {
	    appMetaSetColorPs( ps, dc, &(dc->dcBrush.lbColor) );
	    goto ready;
	    }
	}

    if  ( drawBorders )
	{
	appMetaSetColorPs( ps, dc, &(dc->dcPen.lpColor) );
	goto ready;
	}

  ready:
    appWinMetaPushGraphics( pg, dc, ps );
    return;
    }

static void appWinMetaPopGraphics(	const PushedGraphics *	pg,
					DeviceContext *		dc,
					PrintingState *		ps )
    {
    sioOutPrintf( ps->psSos, "grestore\n" );
    dc->dcColorSet= pg->pgColor;
    }

/************************************************************************/
/*									*/
/*  Insert a bitmap image in the PostScript output for a metafile.	*/
/*									*/
/************************************************************************/

static int appDrawMetaBitmapImagePs(
				DeviceContext *			dc,
				void *				through,
				const RasterImage *		abi,
				const DocumentRectangle *	drSrc,
				const DocumentRectangle *	drDest )
    {
    PrintingState *		ps= (PrintingState *)through;
    SimpleOutputStream *	sos= ps->psSos;
    const BitmapDescription *	bd= &(abi->riDescription);
    const int			onWhite= 1;

    int				twipsWide= drDest->drX1- drDest->drX0+ 1;
    int				twipsHigh= drDest->drY1- drDest->drY0+ 1;

    sioOutPrintf( sos, "gsave 1 setgray\n" );
    sioOutPrintf( sos, "%d %d %d %d rectfill\n", drDest->drX0, drDest->drY0,
							twipsWide, twipsHigh );
    sioOutPrintf( sos, "grestore\n" );

    if  ( bmPsPrintBitmapImage( sos, 1,
			    twipsWide, -twipsHigh,
			    drDest->drX0, ( drDest->drY0+ twipsHigh ),
			    drSrc, onWhite,
			    ps->psUsePostScriptFilters,
			    ps->psUsePostScriptIndexedImages,
			    bd, abi->riBytes ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

static int appCleanObjectPs(		DeviceContext *		dc,
					void *			through,
					MetaFileObject *	mfo )
    { return 0;	}

static int appWinMeta_FillPathPs(	PrintingState *		ps,
					DeviceContext *		dc )
    {
    SimpleOutputStream *	sos= ps->psSos;

    if  ( dc->dcFillPattern )
	{
	sioOutPrintf( sos, "fill-pattern\n" );
	return 0;
	}

    appMetaSetColorPs( ps, dc, &(dc->dcBrush.lbColor) );

    if  ( dc->dcFillHatched )
	{
	double	det;

	det= geoAffineTransformDeterminant2D( &(dc->dcTransform.dctLogicalToOutput) );
	switch( dc->dcBrush.lbHatch )
	    {
	    case HS_HORIZONTAL:
		sioOutPrintf( sos, "%s\n", PsShadingNames[PSshdHORIZ] );
		break;
	    case HS_VERTICAL:
		sioOutPrintf( sos, "%s\n", PsShadingNames[PSshdVERT] );
		break;
	    case HS_FDIAGONAL:
		if  ( det >= 0 )
		    { sioOutPrintf( sos, "%s\n", PsShadingNames[PSshdBDIAG] ); }
		else{ sioOutPrintf( sos, "%s\n", PsShadingNames[PSshdFDIAG] ); }
		break;
	    case HS_BDIAGONAL:
		if  ( det >= 0 )
		    { sioOutPrintf( sos, "%s\n", PsShadingNames[PSshdFDIAG] );	}
		else{ sioOutPrintf( sos, "%s\n", PsShadingNames[PSshdBDIAG] );	}
		break;
	    case HS_CROSS:
		sioOutPrintf( sos, "%s\n", PsShadingNames[PSshdCROSS] );
		break;
	    case HS_DIAGCROSS:
		sioOutPrintf( sos, "%s\n", PsShadingNames[PSshdDCROSS] );
		break;
	    default:
		LDEB(dc->dcBrush.lbHatch);
		sioOutPrintf( sos, "%% HATCH %d !!!!!!!!!!!\n",
						    dc->dcBrush.lbHatch );
	    }
	}
    else{
	if  ( dc->dcPolyFillMode == WINDING )
	    { sioOutPrintf( sos, "eofill\n" );	}
	else{ sioOutPrintf( sos, "fill\n" );	}
	}

    return 0;
    }

static int appMetafilePolygonPathPs(	PrintingState *		ps,
					const Point2DI *	points,
					int			count,
					const char *		command,
					const char *		psOperator,
					int			closePath )
    {
    SimpleOutputStream *	sos= ps->psSos;

    int				done;
    int				x0;
    int				y0;


    x0= 0; y0= 0;

    for ( done= 0; done < count; done++ )
	{
	int	sep;

	if  ( done % 8 == 7 )
	    { sep= '\n';	}
	else{ sep= ' ';		}

	sioOutPrintf( sos, "%d %d %s%c",
			    points[done].x- x0,
			    points[done].y- y0,
			    command, sep );

	command= "rl"; x0= points[done].x; y0= points[done].y;
	}

    if  ( closePath )
	{
	int	sep;

	if  ( done % 8 == 7 )
	    { sep= '\n';	}
	else{ sep= ' ';		}

	sioOutPrintf( sos, "%d %d %s%c",
			    points[0].x- x0,
			    points[0].y- y0,
			    command, sep );
	}

    if  ( psOperator )
	{ sioOutPrintf( sos, "%s", psOperator );	}

    return 0;
    }

static int appMeta_PolyPolygonPs( DeviceContext *		dc,
				void *			through,
				int			polyCount,
				const int *		pointCounts,
				const Point2DI *	points,
				int			fillInsides,
				int			drawBorders,
				int			closePath )
    {
    PrintingState *		ps= (PrintingState *)through;
    int				poly;

    const char *		sep;

    if  ( polyCount == 1 )
	{ sep= " ";	}
    else{ sep= "\n";	}

    if  ( fillInsides )
	{
	const char *	command= "bp";
	int		done= 0;

	for ( poly= 0; poly < polyCount; poly++ )
	    {
	    appMetafilePolygonPathPs( ps, points+ done, pointCounts[poly],
						    command, sep, closePath );
	    command= "mt";
	    done += pointCounts[poly];
	    }

	appWinMeta_FillPathPs( ps, dc );
	}

    if  ( drawBorders )
	{
	const char *	command= "bp";
	int		done= 0;

	appMetaSetColorPs( ps, dc, &(dc->dcPen.lpColor) );
	appMetaSetPenPs( dc, through, &(dc->dcPen) );

	for ( poly= 0; poly < polyCount; poly++ )
	    {
	    appMetafilePolygonPathPs( ps, points+ done, pointCounts[poly],
						    command, sep, closePath );
	    command= "mt";
	    done += pointCounts[poly];
	    }

	if  ( polyCount == 1 && closePath )
	    { sioOutPrintf( ps->psSos, "cp stroke\n" );	}
	else{ sioOutPrintf( ps->psSos, "stroke\n" );	}
	}

    return 0;
    }

static int appMeta_PolyBezierPs( DeviceContext *	dc,
				void *			through,
				int			count,
				const Point2DI *	points,
				int			startPath,
				int			fillInsides,
				int			drawBorders,
				int			closePath )
    {
return 0;
    }

static int appMetaDrawStringPs(		DeviceContext *		dc,
					void *			through,
					int			x0,
					int			y0,
					const MemoryBuffer *	text )
    {
    PrintingState *	ps= (PrintingState *)through;
    const AfmFontInfo *	afi= dc->dcAfi;
    LogicalFont *	lf= &(dc->dcFont);

    int			wide;
    DocumentRectangle	drText;
    const int		withKerning= 0;
    const int		vswap= 1;
    int			fontSizeTwips;

    const char *	s= utilMemoryBufferGetString( text );
    int			len= text->mbSize;

    PushedGraphics	pg;

    if  ( ! afi )
	{ XDEB(afi); return -1;	}

    fontSizeTwips= appWinMetaOutputSize( dc, lf->lfHeight );

    wide= psCalculateStringExtents( &drText, s, len,
				    fontSizeTwips, withKerning, vswap, afi );
    drText.drX0 += x0;
    drText.drX1 += x0;
    drText.drY0 += y0;
    drText.drY1 += y0;

    switch( dc->dcTextAlignment & 0x01 )
	{
	case TA_NOUPDATECP:
	    break;

	case TA_UPDATECP:
	    x0= DC_xViewport( dc->dcX, dc->dcY, dc );
	    y0= DC_yViewport( dc->dcX, dc->dcY, dc );
	    break;

	default:
	    XDEB(dc->dcTextAlignment & 0x01 );
	    break;
	}

    switch( dc->dcTextAlignment & 0x06 )
	{
	int		xshift;

	case TA_LEFT:
	    break;

	case TA_RIGHT:
	    xshift= -wide;
	    x0 += xshift;
	    drText.drX0 += xshift;
	    drText.drX1 += xshift;
	    break;

	case TA_CENTER:
	    xshift= -wide/ 2;
	    x0 += xshift;
	    drText.drX0 += xshift;
	    drText.drX1 += xshift;
	    break;

	default:
	    XDEB(dc->dcTextAlignment & 0x06);
	    break;
	}

    switch( dc->dcTextAlignment & 0x18 )
	{
	int			yshift;
	DocumentRectangle	drFontBBox;
	DocumentRectangle	drFontAscDesc;

	case TA_TOP:
	    psFontBBox( &drFontBBox, &drFontAscDesc, fontSizeTwips,
								vswap, afi );
	    yshift= -drFontAscDesc.drY0;
	    y0 += yshift;
	    drText.drY0 += yshift;
	    drText.drY1 += yshift;
	    break;

	case TA_BOTTOM:
	    psFontBBox( &drFontBBox, &drFontAscDesc, fontSizeTwips,
								vswap, afi );
	    yshift= -drFontAscDesc.drY1;
	    y0 += yshift;
	    drText.drY0 += yshift;
	    drText.drY1 += yshift;
	    break;

	case TA_BASELINE:
	    break;

	default:
	    WMFDEB(appDebug(" 0x%x", dc->dcTextAlignment & 0x18 ));
	    break;
	}

    if  ( dc->dcBkMode == OPAQUE )
	{
	appMetaSetColorPs( ps, dc, &(dc->dcBkColor) );

	psFillRectangle( ps, drText.drX0, drText.drY0,
				    drText.drX1- drText.drX0+ 1,
				    drText.drY1- drText.drY0+ 1 );
	}

    appMetaSetFontPs( ps, dc, dc->dcFont.lfTextAttribute, fontSizeTwips );
    appMetaSetColorPs( ps, dc, &(dc->dcTextColor) );

    if  ( dc->dcFont.lfOrientation != 0 )
	{
	appWinMetaPushGraphics( &pg, dc, ps );
	sioOutPrintf( ps->psSos, "%d %d translate %g rotate\n",
				x0, y0, -0.1* dc->dcFont.lfOrientation );

	x0= y0= 0;
	}

    psMoveShowString( ps, (unsigned char *)s, len, x0, y0 );

    if  ( dc->dcFont.lfTextAttribute.taTextIsUnderlined )
	{
	int		h;
	int		pos;

	psUnderlineGeometry( &pos, &h, y0, fontSizeTwips, afi );

	psFillRectangle( ps, x0, y0, wide, h );
	}

    if  ( dc->dcFont.lfOrientation != 0 )
	{
	appWinMetaPopGraphics( &pg, dc, ps );
	}

    return 0;
    }

static int appMetaPatBltPs(	DeviceContext *			dc,
				void *				through,
				long				rop,
				const DocumentRectangle *	drOutput )
    {
    PrintingState *	ps= (PrintingState *)through;

    if  ( dc->dcFillInsides )
	{
	appMetafileRectPathPs( ps, drOutput );
	appWinMeta_FillPathPs( ps, dc );
	}

    return 0;
    }

static void appMetaGetArcPs(	DeviceContext *			dc,
				int *				pW,
				int *				pH,
				int *				pXm,
				int *				pYm,
				int *				pXs,
				int *				pYs,
				double *			pas,
				double *			pae,
				const DocumentRectangle *	dr,
				int				xs,
				int				ys,
				int				xe,
				int				ye )

    {
    int			ym;
    int			xm;
    int			h;
    int			w;

    double		as;
    double		ae;

    DocumentRectangle	drFixed;

    ym= ( dr->drY1+ dr->drY0 )/2;
    xm= ( dr->drX1+ dr->drX0 )/2;
    drFixed= *dr;

    if  ( dr->drX1 < dr->drX0 )
	{
	xs= 2* xm- xs;
	xe= 2* xm- xe;
	drFixed.drX0= dr->drX1;
	drFixed.drX1= dr->drX0;
	}
    if  ( dr->drY1 < dr->drY0 )
	{
	ys= 2* ym- ys;
	ye= 2* ym- ye;
	drFixed.drY0= dr->drY1;
	drFixed.drY1= dr->drY0;
	}

    h= drFixed.drY1- drFixed.drY0;
    w= drFixed.drX1- drFixed.drX0;

    as= -atan2(	(double) w* ( ys- ym ), (double) h* ( xs- xm ) );
    ae= -atan2(	(double) w* ( ye- ym ), (double) h* ( xe- xm ) );

    xs= (int)( ( ( drFixed.drX1- drFixed.drX0 )/ 2 )* cos( as )+ xm );
    ys= (int)( ( ( drFixed.drY1- drFixed.drY0 )/ 2 )* sin( as ) );

    as= ( 180* as )/ M_PI;
    ae= ( 180* ae )/ M_PI;

    *pW= w;
    *pH= h;
    *pXm= xm;
    *pYm= ym;

    *pYs= ys;
    *pXs= xs;
    *pas= as;
    *pae= ae;
*pas= -as;
*pae= 360- ae;

    return;
    }

static int appMetaDrawArcPs(	DeviceContext *			dc,
				void *				through,
				const DocumentRectangle *	dr,
				int				xs,
				int				ys,
				int				xe,
				int				ye,
				int				fillInside,
				int				drawBorder )
    {
    PrintingState *	ps= (PrintingState *)through;
    double		as;
    double		ae;
    int			xm;
    int			ym;
    int			w;
    int			h;

    appMetaGetArcPs( dc, &w, &h, &xm, &ym, &xs, &ys, &as, &ae,
							dr, xs, ys, xe, ye );

    if  ( drawBorder )
	{
	appMetaSetColorPs( ps, dc, &(dc->dcPen.lpColor) );
	appMetaSetPenPs( dc, through, &(dc->dcPen) );

	sioOutPrintf( ps->psSos, "gsave [1 0 0 %g 0 %d] concat\n",
						    (double)h/(double)w, ym );

	sioOutPrintf( ps->psSos, "%d %d bp ", xs, ys );

	sioOutPrintf( ps->psSos, "%d 0 %d %f %f arc stroke\n",
							    xm, w/2, as, ae );

	sioOutPrintf( ps->psSos, "grestore\n" );
	}

    return 0;
    }

static int appMetaDrawPiePs(	DeviceContext *			dc,
				void *				through,
				const DocumentRectangle *	dr,
				int				xs,
				int				ys,
				int				xe,
				int				ye,
				int				fillInside,
				int				drawBorder )
    {
    PrintingState *	ps= (PrintingState *)through;
    double		as;
    double		ae;
    int			xm;
    int			ym;
    int			w;
    int			h;

    PushedGraphics	pg;

    appMetaGetArcPs( dc, &w, &h, &xm, &ym, &xs, &ys, &as, &ae,
							dr, xs, ys, xe, ye );
    appWinMetaPushGraphicsForDraw( &pg, dc, ps, fillInside, drawBorder );
    sioOutPrintf( ps->psSos, "[1 0 0 %g 0 %d] concat\n",
						    (double)h/(double)w, ym );

    if  ( fillInside )
	{
	sioOutPrintf( ps->psSos, "%d %d bp ", xs, ys );

	sioOutPrintf( ps->psSos, "%d 0 %d %f %f arc\n", xm, w/2, as, ae );
	sioOutPrintf( ps->psSos, "%d  0 lineto ", xm );
	sioOutPrintf( ps->psSos, "%d %d lineto closepath\n", xs, ys );

	appWinMeta_FillPathPs( ps, dc );
	}

    if  ( drawBorder )
	{
	appMetaSetColorPs( ps, dc, &(dc->dcPen.lpColor) );
	appMetaSetPenPs( dc, through, &(dc->dcPen) );

	sioOutPrintf( ps->psSos, "%d %d bp ", xs, ys );

	sioOutPrintf( ps->psSos, "%d 0 %d %f %f arc ", xm, w/2, as, ae );
	sioOutPrintf( ps->psSos, "%d  0 lineto ", xm );
	sioOutPrintf( ps->psSos, "%d %d lineto stroke\n", xs, ys );
	}

    appWinMetaPopGraphics( &pg, dc, ps );

    return 0;
    }

static int appMetaDrawChordPs(	DeviceContext *			dc,
				void *				through,
				const DocumentRectangle *	dr,
				int				xs,
				int				ys,
				int				xe,
				int				ye,
				int				fillInside,
				int				drawBorder )
    {
    PrintingState *	ps= (PrintingState *)through;
    double		as;
    double		ae;
    int			xm;
    int			ym;
    int			w;
    int			h;

    PushedGraphics	pg;

    appMetaGetArcPs( dc, &w, &h, &xm, &ym, &xs, &ys, &as, &ae,
							dr, xs, ys, xe, ye );

    appWinMetaPushGraphicsForDraw( &pg, dc, ps, fillInside, drawBorder );
    sioOutPrintf( ps->psSos, "[1 0 0 %g 0 %d] concat\n",
						    (double)h/(double)w, ym );

    if  ( drawBorder )
	{
	appMetaSetColorPs( ps, dc, &(dc->dcPen.lpColor) );
	appMetaSetPenPs( dc, through, &(dc->dcPen) );

	sioOutPrintf( ps->psSos, "%d %d bp ", xs, ys );

	sioOutPrintf( ps->psSos, "%d 0 %d %f %f arc %d %d lineto stroke\n",
						    xm, w/2, as, ae, xs, ys );
	}

    appWinMetaPopGraphics( &pg, dc, ps );

    return 0;
    }

static int appMetaDrawEllipsePs(DeviceContext *			dc,
				void *				through,
				const DocumentRectangle *	dr,
				int				fillInside,
				int				drawBorder )
    {
    PrintingState *	ps= (PrintingState *)through;
    int			w= dr->drX1- dr->drX0;
    int			h= dr->drY1- dr->drY0;
    int			xm= ( dr->drX0+ dr->drX1 )/ 2;
    int			ym= ( dr->drY0+ dr->drY1 )/ 2;

    PushedGraphics	pg;

    appWinMetaPushGraphicsForDraw( &pg, dc, ps, fillInside, drawBorder );
    sioOutPrintf( ps->psSos, "[1 0 0 %g 0 %d] concat\n",
						(double)h/(double)w, ym );

    if  ( fillInside )
	{
	sioOutPrintf( ps->psSos, "%d 0 bp ", dr->drX1 );
	sioOutPrintf( ps->psSos, "%d 0 %d 0 360 arc closepath\n", xm, w/2 );

	appWinMeta_FillPathPs( ps, dc );
	}

    if  ( drawBorder )
	{
	appMetaSetColorPs( ps, dc, &(dc->dcPen.lpColor) );
	appMetaSetPenPs( dc, through, &(dc->dcPen) );

	sioOutPrintf( ps->psSos, "%d 0 bp ", dr->drX1 );
	sioOutPrintf( ps->psSos, "%d 0 %d 0 360 arc stroke\n", xm, w/2 );
	}

    appWinMetaPopGraphics( &pg, dc, ps );

    return 0;
    }

static int appMetaDrawRoundedRectanglePs(
				DeviceContext *			dc,
				void *				through,
				const DocumentRectangle *	dr,
				int				wide,
				int				high,
				int				fillInside,
				int				drawBorder )
    {
    PrintingState *		ps= (PrintingState *)through;
    SimpleOutputStream *	sos= ps->psSos;

    DocumentRectangle		drHere;
    PushedGraphics		pg;
    int				r;

    geoNormalizeRectangle( &drHere, dr );

    if  ( high < 0 )
	{ high= -high;	}
    if  ( wide < 0 )
	{ wide= -wide;	}

    r= sqrt( wide*wide+ high*high )/ 2;

    appWinMetaPushGraphicsForDraw( &pg, dc, ps, fillInside, drawBorder );

    if  ( fillInside )
	{
	psDrawRoundRectPath( sos, &drHere, r, "closepath\n" );

	appWinMeta_FillPathPs( ps, dc );
	}

    if  ( drawBorder )
	{
	appMetaSetColorPs( ps, dc, &(dc->dcPen.lpColor) );
	appMetaSetPenPs( dc, through, &(dc->dcPen) );

	psDrawRoundRectPath( sos, &drHere, r, "stroke\n" );
	}

    appWinMetaPopGraphics( &pg, dc, ps );

    return 0;
    }

static int appMetaIgnoreColorPs(	DeviceContext *		dc,
					void *			through,
					int			r,
					int			g,
					int			b )
    { return 0;	}

static int appMetaSetPolyFillModePs(	DeviceContext *		dc,
					void *			through,
					int			mode )
    { return 0;	}

/************************************************************************/
/*									*/
/*  Start a pattern fill.						*/
/*									*/
/************************************************************************/

static int appMetaSelectPatternBrushObjectPs(	DeviceContext *		dc,
						void *			through,
						PatternBrush *		pb )
    {
    PrintingState *		ps= (PrintingState *)through;
    const RasterImage *	abi= pb->pbAbi;

    SimpleOutputStream *	sos= ps->psSos;

    int				bytesPerRow;

    const int			useFilters= 0;
    const int			indexedImages= 0;
    BitmapPrinter		bp;
    const int			onWhite= 1;

    bytesPerRow= bmPsRowStringSize( &(abi->riDescription),
						abi->riDescription.bdPixelsWide,
						indexedImages );
    if  ( bytesPerRow < 1 )
	{ LDEB(bytesPerRow); return -1;	}

    sioOutPrintf( sos, "currentfile %d string readhexstring\n",
				    abi->riDescription.bdPixelsHigh* bytesPerRow );

    bmPsOpenBitmapPrinter( &bp, sos, &(abi->riDescription),
						useFilters, indexedImages );

    {
    DocumentRectangle	drSel;

    drSel.drX0= 0;
    drSel.drY0= 0;
    drSel.drX1= abi->riDescription.bdPixelsWide- 1;
    drSel.drY1= abi->riDescription.bdPixelsHigh- 1;
    if  ( bmPsWriteBitmapData( &bp, &drSel,
				    &(abi->riDescription), abi->riBytes ) )
	{ LDEB(1); return -1;	}
    }

    bmCloseBitmapPrinter( &bp );

    sioOutPrintf( sos, "%%\n" );
    sioOutPrintf( sos, "pop /fill-data exch store\n" );

    sioOutPrintf( sos, "/fill-wide %d store\n", abi->riDescription.bdPixelsWide );
    sioOutPrintf( sos, "/fill-high %d store\n", abi->riDescription.bdPixelsHigh );

    sioOutPrintf( sos, "/fill-cell " );
    sioOutPrintf( sos, "{ " );
    sioOutPrintf( sos, "gsave " );
    sioOutPrintf( sos, "%d %d scale\n" ,
			    abi->riDescription.bdPixelsWide,
			    abi->riDescription.bdPixelsHigh );

    sioOutPrintf( sos, "1 setgray 0 0 1 1 rectfill\n" );

    bmPsWriteImageInstructions( sos,
			    &(abi->riDescription), onWhite,
			    abi->riDescription.bdPixelsWide,
			    abi->riDescription.bdPixelsHigh,
			    "{ /fill-data load } bind",
			    indexedImages );

    sioOutPrintf( sos, "grestore " );
    sioOutPrintf( sos, "} bind def\n" );

    dc->dcFillHatched= 0;
    dc->dcFillPattern= 1;
    dc->dcFillInsides= 1;

    return 0;
    }

static int appMetaStartPs(	DeviceContext *			dc,
				int				objectCount,
				const MetafilePlayer *		player,
				const DocumentRectangle *	drLogical,
				int				mapMode )
    {
    if  ( appMetaInitDeviceContext( dc, player, objectCount,
				    drLogical, mapMode,
				    player->mpTwipsWide, player->mpTwipsHigh,
				    player->mpTwipsWide, player->mpTwipsHigh ) )
	{ LDEB(objectCount); return -1;	}

    dc->dcCleanObject= appCleanObjectPs;
    dc->dcDrawRasterImage= appDrawMetaBitmapImagePs;
    dc->dcSelectPenObject= appMetaSelectPenObjectImpl;
    dc->dcSelectBrushObject= appMetaSelectBrushObjectImpl;
    dc->dcSelectFontObject= appMetaSelectFontObjectImpl;
    dc->dcSelectPatternBrushObject= appMetaSelectPatternBrushObjectPs;
    dc->dcDrawPolyPolygon= appMeta_PolyPolygonPs;
    dc->dcDrawPolyBezier= appMeta_PolyBezierPs;
    dc->dcDrawString= appMetaDrawStringPs;
    dc->dcPatBlt= appMetaPatBltPs;
    dc->dcDrawPie= appMetaDrawPiePs;
    dc->dcDrawArc= appMetaDrawArcPs;
    dc->dcDrawChord= appMetaDrawChordPs;
    dc->dcDrawEllipse= appMetaDrawEllipsePs;
    dc->dcDrawRoundedRectangle= appMetaDrawRoundedRectanglePs;
    dc->dcSetTextColor= appMetaIgnoreColorPs;
    dc->dcSetBkColor= appMetaIgnoreColorPs;
    dc->dcSetPolyFillMode= appMetaSetPolyFillModePs;

    return 0;
    }

int appMetaPlayWmfPs(	PrintingState *			ps,
			const MetafilePlayer *		player )
    {
    int			rval= 0;

    WmfHeader		wh;
    DeviceContext	dc;

    DocumentRectangle	drLogical;

    drLogical.drX0= 0;
    drLogical.drY0= 0;
    drLogical.drX1= player->mpXWinExt;
    drLogical.drY1= player->mpYWinExt;

    utilRGB8SolidBlack( &(dc.dcColorSet) );
    psSetRgb8Color( ps, &(dc.dcColorSet) );

    if  ( sioInGetByte( player->mpInputStream ) == EOF )
	{ return 0;	}
    sioInUngetLastRead( player->mpInputStream );

    if  ( bmMetaReadWmfHeader( &wh, player->mpInputStream ) )
	{ LDEB(1); return -1;		}

    if  ( appMetaStartPs( &dc, wh.wh_objectCount, player, &drLogical,
							player->mpMapMode ) )
	{ LDEB(wh.wh_objectCount); return -1;	}

    WMFDEB(appDebug( "PS WMF LOGICAL: [%d x %d] TWIPS: [%d x %d]\n",
				player->mpXWinExt, player->mpYWinExt,
				player->mpTwipsWide, player->mpTwipsHigh ));

    if  ( appMetaPlayWmf( &dc, (void *)ps ) )
	{ rval= -1;	}

    appMetaCleanDeviceContext( &dc, (void *)ps );

    return rval;
    }

int appMetaPlayEmfPs(	PrintingState *			ps,
			const MetafilePlayer *		player )
    {
    int			rval= 0;

    int			done;
    EmfHeader		eh;
    DeviceContext	dc;

    done= bmMetaReadEmfHeader( &eh, player->mpInputStream );
    if  ( done < 0 )
	{ LDEB(done); return -1;		}
    while( done < eh.eh_headerSize )
	{ (void)sioInGetByte( player->mpInputStream ); done++; }

    if  ( appMetaStartPs( &dc, eh.eh_numOfHandles, player, &(eh.ehBounds),
							    MM_ANISOTROPIC ) )
	{ LDEB(eh.eh_numOfHandles); return -1;	}

    WMFDEB(appDebug( "PS EMF LOGICAL: [%d..%d x %d..%d] TWIPS: [%d x %d]\n",
				    eh.ehBounds.drX0, eh.ehBounds.drX1,
				    eh.ehBounds.drY0, eh.ehBounds.drY1,
				    player->mpTwipsWide, player->mpTwipsHigh ));

    if  ( appMetaPlayEmf( &dc, (void *)ps ) )
	{ rval= -1;	}

    appMetaCleanDeviceContext( &dc, (void *)ps );

    return rval;
    }

int appMacPictPlayFilePs(	PrintingState *			ps,
				const MetafilePlayer *		player )
    {
    MacPictHeader	mph;
    int			rval= 0;

    int			done;
    DeviceContext	dc;
    const int		handleCount= MACPICThandle_COUNT;

    done= appMacPictGetDeviceHeader( &mph,  player->mpInputStream );
    if  ( done < 0 )
	{ LDEB(done); return -1;		}

    if  ( appMetaStartPs( &dc, handleCount, player,
					&(mph.mphFrame), MM_ANISOTROPIC ) )
	{ LDEB(handleCount); return -1;	}

    dc.dcMacPictVersion= mph.mphVersion;

    WMFDEB(appDebug( "PS PICT LOGICAL: [%d..%d x %d..%d] TWIPS: [%d x %d]\n",
				mph.mphFrame.drX0, mph.mphFrame.drX1,
				mph.mphFrame.drY0, mph.mphFrame.drY1,
				player->mpTwipsWide, player->mpTwipsHigh ));

    if  ( appMacPictPlayPict( &dc, (void *)ps ) )
	{ rval= -1;	}

    appMetaCleanDeviceContext( &dc, (void *)ps );

    return rval;
    }
