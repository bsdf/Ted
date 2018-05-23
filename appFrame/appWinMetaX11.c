#   include	"appFrameConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>

#   include	<drawWinMetaImpl.h>
#   include	<drawMacPictImpl.h>
#   include	"appWinMetaX11.h"
#   include	<bmEmfIo.h>
#   include	<bmWmfIo.h>
#   include	"drawDrawingSurfaceImpl.h"
#   include	"drawUtilGtk.h"

#   include	<math.h>

#   include	<appDebugon.h>

typedef struct DeviceContextX11
    {
    DrawingSurface	dcxDrawingSurface;
    AppColors *		dcxColors;

    int			dcxPixelsWide;
    int			dcxPixelsHigh;

    APP_COLOR_RGB	dcxPenColor;
    APP_COLOR_RGB	dcxBrushColor;

    APP_COLOR_RGB	dcxBkColor;
    APP_COLOR_RGB	dcxTextColor;
    int			dcxFillTiled;
    int			dcxFillHatched;

    int			dcxScreenFont;
    } DeviceContextX11;

# if 0
#    define	WMFDEB(x)	(x)
# else
#    define	WMFDEB(x)	/*nothing*/
# endif

static int appMetaCleanObjectX11(	DeviceContext *		dc,
					void *			through,
					MetaFileObject *	mfo )
    {
    PatternBrush *	pb;
    LogicalBrush *	lb;

    switch( mfo->mfoType )
	{
	case MFtypePATTERNBRUSH:
	    pb= &(mfo->mfoPatternBrush);
	    if  ( pb->pbTilePixmap )
		{
		drawFreeDrawingSurface( pb->pbTilePixmap );
		pb->pbTilePixmap= (struct DrawingSurface *)0;
		}
	    break;

	case MFtypeBRUSH:
	    lb= &(mfo->mfoLogicalBrush);
	    if  ( lb->lbTilePixmap )
		{
		drawFreeDrawingSurface( lb->lbTilePixmap );
		lb->lbTilePixmap= (struct DrawingSurface *)0;
		}
	    break;

	default:
	    break;
	}

    return 0;
    }

static int appMetaInitDeviceContextX11(
				DeviceContextX11 *		dcx,
				DeviceContext *			dc,
				DrawingSurface			ds,
				const MetafilePlayer *		player,
				int				objectCount,
				const DocumentRectangle *	drLogical,
				int				mapMode,
				int				twipsWide,
				int				twipsHigh,
				int				pixelsWide,
				int				pixelsHigh )
    {
    if  ( appMetaInitDeviceContext( dc, player, objectCount, drLogical, mapMode,
						    pixelsWide, pixelsHigh,
						    twipsWide, twipsHigh ) )
	{ LDEB(objectCount); return -1;	}

    dcx->dcxDrawingSurface= ds;

    dcx->dcxScreenFont= -1;

    dcx->dcxPixelsWide= pixelsWide;
    dcx->dcxPixelsHigh= pixelsHigh;

    return 0;
    }

static void appMetaCleanDeviceContextX11(	DeviceContext *		dc,
						DeviceContextX11 *	dcx )
    {
    appMetaCleanDeviceContext( dc, (void *)dcx );
    }

static int appMetaSetDeviceDefaultsX11(	DeviceContextX11 *	dcx,
					DeviceContext *		dc )
    {
    DrawingSurface	ds= dcx->dcxDrawingSurface;

    drawSetForegroundColorBlack( ds );

    if  ( appColorRgb( &(dcx->dcxPenColor), ds->dsColors,
					    dc->dcPen.lpColor.rgb8Red,
					    dc->dcPen.lpColor.rgb8Green,
					    dc->dcPen.lpColor.rgb8Blue ) )
	{ LDEB(1); return -1;	}

    if  ( appColorRgb( &(dcx->dcxBrushColor), ds->dsColors,
					    dc->dcBrush.lbColor.rgb8Red,
					    dc->dcBrush.lbColor.rgb8Green,
					    dc->dcBrush.lbColor.rgb8Blue ) )
	{ LDEB(1); return -1;	}

    if  ( appColorRgb( &(dcx->dcxBkColor), ds->dsColors,
					    dc->dcBkColor.rgb8Red,
					    dc->dcBkColor.rgb8Green,
					    dc->dcBkColor.rgb8Blue ) )
	{ LDEB(1); return -1;	}

    if  ( appColorRgb( &(dcx->dcxTextColor), ds->dsColors,
					    dc->dcTextColor.rgb8Red,
					    dc->dcTextColor.rgb8Green,
					    dc->dcTextColor.rgb8Blue ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

static int appMetaSelectBrushObjectX11(	DeviceContext *		dc,
					void *			through,
					LogicalBrush *		lb )
    {
    DeviceContextX11 *	dcx= (DeviceContextX11 *)through;
    DrawingSurface	ds= dcx->dcxDrawingSurface;

    switch( lb->lbStyle )
	{
	case BS_SOLID:
	    dc->dcFillInsides= 1;

	    if  ( appColorRgb( &(dcx->dcxBrushColor), ds->dsColors,
					    lb->lbColor.rgb8Red,
					    lb->lbColor.rgb8Green,
					    lb->lbColor.rgb8Blue ) )
		{ LDEB(1);  return -1;	}

#	    ifdef USE_MOTIF
	    XSetFillStyle( ds->dsDisplay, ds->dsGc, FillSolid );
#	    endif
#	    ifdef USE_GTK
	    gdk_gc_set_fill( ds->dsGc, GDK_SOLID );
#	    endif

	    dcx->dcxFillTiled= 0;
	    dcx->dcxFillHatched= 0;
	    drawSetSystemColor( ds, &(dcx->dcxBrushColor) );
	    break;

	case BS_HOLLOW:
#	    ifdef USE_MOTIF
	    XSetFillStyle( ds->dsDisplay, ds->dsGc, FillSolid );
#	    endif
#	    ifdef USE_GTK
	    gdk_gc_set_fill( ds->dsGc, GDK_SOLID );
#	    endif

	    dcx->dcxFillTiled= 0;
	    dcx->dcxFillHatched= 0;
	    dc->dcFillInsides= 0;

	    break;

	case BS_HATCHED:
	case BS_PATTERN:
	    if  ( ! lb->lbTilePixmap )
		{
		const int		wide= 8;
		const int		high= 8;
		DocumentRectangle	drPixmap;

		drPixmap.drX0= 0;
		drPixmap.drY0= 0;
		drPixmap.drX1= wide- 1;
		drPixmap.drY1= high- 1;

		if  ( appColorRgb( &(dcx->dcxBrushColor), ds->dsColors,
						lb->lbColor.rgb8Red,
						lb->lbColor.rgb8Green,
						lb->lbColor.rgb8Blue ) )
		    { LDEB(1);  return -1;	}

		lb->lbTilePixmap= drawMakeDrawingSurfaceForParent( ds,
								wide, high );

		drawSetForegroundColorWhite( lb->lbTilePixmap );
		drawFillRectangle( lb->lbTilePixmap, &drPixmap );

		drawSetSystemColor( lb->lbTilePixmap, &(dcx->dcxBrushColor) );

		switch( lb->lbHatch )
		    {
		    case HS_HORIZONTAL:
			drawLine( lb->lbTilePixmap, 0, 1, wide- 1, 1 );
			break;

		    case HS_VERTICAL:
			drawLine( lb->lbTilePixmap, 1, 0, 1, high- 1 );
			break;

		    case HS_FDIAGONAL:
			drawLine( lb->lbTilePixmap, 0, 0, wide- 1, high- 1 );
			break;

		    case HS_BDIAGONAL:
			drawLine( lb->lbTilePixmap, 0, high- 1, wide- 1, 0 );
			break;

		    case HS_CROSS:
			drawLine( lb->lbTilePixmap, 0, 1, wide- 1, 1 );
			drawLine( lb->lbTilePixmap, 1, 0, 1, high- 1 );
			break;

		    case HS_DIAGCROSS:
			drawLine( lb->lbTilePixmap, 0, 0, wide- 1, high- 1 );
			drawLine( lb->lbTilePixmap, 0, high- 1, wide- 1, 0 );
			break;

		    default:
			LDEB(lb->lbHatch);
			break;
		    }
		}

#	    ifdef USE_MOTIF
	    XSetTile( ds->dsDisplay, ds->dsGc,
					lb->lbTilePixmap->dsDrawable );
#	    endif
#	    ifdef USE_GTK
	    gdk_gc_set_tile( ds->dsGc, lb->lbTilePixmap->dsDrawable );
#	    endif

	    dc->dcFillInsides= 1;
	    dcx->dcxFillTiled= 1;
	    dcx->dcxFillHatched= 1;
	    break;

	default:
	    LDEB(lb->lbStyle); 
	    dc->dcFillInsides= 0;
	    break;
	}

    dc->dcBrush= *lb;

    return 0;
    }

static int appMetaSelectPatternBrushObjectX11(	DeviceContext *		dc,
						void *			through,
						PatternBrush *		pb )
    {
    DeviceContextX11 *	dcx= (DeviceContextX11 *)through;
    DrawingSurface	ds= dcx->dcxDrawingSurface;

    if  ( ! pb->pbTilePixmap )
	{
	RasterImage *	abi= pb->pbAbi;
	const int		dstXExt= abi->riDescription.bdPixelsWide;
	const int		dstYExt= abi->riDescription.bdPixelsHigh;

	pb->pbTilePixmap= drawMakeDrawingSurfaceForImageAndParent( ds,
			    abi, (DocumentRectangle *)0, dstXExt, dstYExt );
	if  ( ! pb->pbTilePixmap )
	    { XDEB(pb->pbTilePixmap); return -1;	}
	}

#   ifdef USE_MOTIF
    XSetTile( ds->dsDisplay, ds->dsGc, pb->pbTilePixmap->dsDrawable );
#   endif
#   ifdef USE_GTK
    gdk_gc_set_tile( ds->dsGc, pb->pbTilePixmap->dsDrawable );
#   endif

    dc->dcFillInsides= 1;
    dcx->dcxFillTiled= 1;
    dcx->dcxFillHatched= 0;

    return 0;
    }

static int appMetaSelectFontObjectX11(	DeviceContext *		dc,
					void *			through,
					LogicalFont *		lf )
    {
    const MetafilePlayer *	player= dc->dcPlayer;
    DeviceContextX11 *		dcx= (DeviceContextX11 *)through;
    DrawingSurface		ds= dcx->dcxDrawingSurface;

    if  ( lf->lfPrivateFont < 0 )
	{
	int			pixelSize;
	const AfmFontInfo *	afi;
	const IndexSet *	unicodesWanted;

	afi= (*player->mpGetFontForAttribute)( &unicodesWanted,
				    &(lf->lfTextAttribute), &(dc->dcFontList),
				    player->mpPostScriptFontList );
	if  ( ! afi )
	    { SXDEB(lf->lfFaceNameUtf8,afi); return -1;	}

	pixelSize= appWinMetaOutputSize( dc, lf->lfHeight );

	lf->lfPrivateFont= drawOpenScreenFont( ds,
					    afi, pixelSize, unicodesWanted );
	if  ( lf->lfPrivateFont < 0 )
	    { SLDEB(lf->lfFaceNameUtf8,lf->lfPrivateFont); return 0;	}
	}

    if  ( lf->lfPrivateFont >= 0 )
	{ dcx->dcxScreenFont= lf->lfPrivateFont;	}
    else{ dcx->dcxScreenFont= -1;			}

    return 0;
    }

static void appMetaX11SetWindowsLineStyle(
					DrawingSurface		ds,
					int *			pDraw,
					int			ps,
					int			lpW,
					int			width )
    {
    int		drawBorders= 0;

    static unsigned char	dash[]= { 3, 2 };
    static unsigned char	dot[]= { 1, 2 };
    static unsigned char	dashdot[]= { 2, 2, 1, 2 };
    static unsigned char	dashdotdot[]= { 2, 2, 1, 2, 1, 2 };

    switch( ps & 0xff )
	{
	case PS_SOLID:
	  solid:
	    drawBorders= 1;

	    drawSetLineAttributes( ds,
			width, LineStyleSolid, LineCapButt, LineJoinMiter,
			(const unsigned char *)0, 0 );
	    break;

	case PS_INSIDEFRAME:
	    drawBorders= 1;

	    drawSetLineAttributes( ds,
			width, LineStyleSolid, LineCapButt, LineJoinMiter,
			(const unsigned char *)0, 0 );
	    break;

	case PS_DASH:
	    if  ( lpW > 1 )
		{ goto solid;	}

	    drawBorders= 1;

	    drawSetLineAttributes( ds,
			1, LineStyleSingleDash, LineCapButt, LineJoinMiter,
			dash, sizeof( dash ) );
	    break;

	case PS_DOT:
	    if  ( lpW > 1 )
		{ goto solid;	}

	    drawBorders= 1;

	    drawSetLineAttributes( ds,
			1, LineStyleSingleDash, LineCapButt, LineJoinMiter,
			dot, sizeof( dot ) );
	    break;

	case PS_DASHDOT:
	    if  ( lpW > 1 )
		{ goto solid;	}

	    drawBorders= 1;

	    drawSetLineAttributes( ds,
			1, LineStyleSingleDash, LineCapButt, LineJoinMiter,
			dashdot, sizeof( dashdot ) );
	    break;

	case PS_DASHDOTDOT:
	    if  ( lpW > 1 )
		{ goto solid;	}

	    drawBorders= 1;

	    drawSetLineAttributes( ds,
			1, LineStyleSingleDash, LineCapButt, LineJoinMiter,
			dashdotdot, sizeof( dashdotdot ) );
	    break;

	case PS_NULL:
	    drawBorders= 0;
	    break;

	default:
	    drawBorders= 0;
	    break;
	}


    *pDraw= drawBorders;
    return;
    }

static int appMetaSelectPenObjectX11(	DeviceContext *		dc,
					void *			through,
					LogicalPen *		lp )
    {
    DeviceContextX11 *		dcx= (DeviceContextX11 *)through;
    DrawingSurface		ds= dcx->dcxDrawingSurface;
    AppColors *			ac= dcx->dcxColors;
    int				outputWidth;

    outputWidth= appWinMetaOutputSize( dc, lp->lpWidth );

    appMetaX11SetWindowsLineStyle( ds, &(dc->dcDrawBorders),
			lp->lpStyle, lp->lpWidth, outputWidth );

    if  ( dc->dcDrawBorders )
	{
	if  ( appColorRgb( &dcx->dcxPenColor, ac,
					    lp->lpColor.rgb8Red,
					    lp->lpColor.rgb8Green,
					    lp->lpColor.rgb8Blue ) )
		{ LDEB(1);  return -1;	}

	drawSetSystemColor( ds, &(dcx->dcxPenColor) );
	}

    dc->dcPen= *lp;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Draw a raster image on the window.					*/
/*									*/
/************************************************************************/

static int appDrawMetaBitmapImageX11(	DeviceContext *			dc,
					void *				through,
					const RasterImage *		abi,
					const DocumentRectangle *	drSrc,
					const DocumentRectangle *	drDest )
    {
    DeviceContextX11 *	dcx= (DeviceContextX11 *)through;
    DrawingSurface	ds= dcx->dcxDrawingSurface;

    return drawRasterImage( ds, drDest, abi, drSrc );
    }

/************************************************************************/
/*									*/
/*  Draw a rounded rectangle.						*/
/*									*/
/************************************************************************/

static int appMetaDrawRoundedRectangleX11(
				DeviceContext *			dc,
				void *				through,
				const DocumentRectangle *	dr,
				int				wide,
				int				high,
				int				fillInside,
				int				drawBorder )
    {
    DeviceContextX11 *	dcx= (DeviceContextX11 *)through;
    DrawingSurface	ds= dcx->dcxDrawingSurface;

    if  ( fillInside )
	{
	drawSetSystemColor( ds, &(dcx->dcxBrushColor) );
	drawFillRoundedRect( ds, dr, wide, high );
	}

    if  ( drawBorder )
	{
	drawSetSystemColor( ds, &(dcx->dcxPenColor) );
	drawRoundedRect( ds, dr, wide, high );
	}

    return 0;
    }

static void appMetaGetArcX11(	DeviceContextX11 *		dcx,
				Arc2DI *			arc,
				const DocumentRectangle *	dr,
				int				xs,
				int				ys,
				int				xe,
				int				ye )

    {
    int		ym;
    int		xm;
    int		h;
    int		w;

    double	as;
    double	ae;

    ym= ( dr->drY1+ dr->drY0 )/2;
    xm= ( dr->drX1+ dr->drX0 )/2;
    arc->a2diRect= *dr;

    if  ( dr->drX1 < dr->drX0 )
	{
	xs= 2* xm- xs;
	xe= 2* xm- xe;
	arc->a2diRect.drX1= dr->drX0;
	arc->a2diRect.drX0= dr->drX1;
	}
    if  ( dr->drY1 < dr->drY0 )
	{
	ys= 2* ym- ys;
	ye= 2* ym- ye;
	arc->a2diRect.drY1= dr->drY0;
	arc->a2diRect.drY0= dr->drY1;
	}

    h= arc->a2diRect.drY1- arc->a2diRect.drY0;
    w= arc->a2diRect.drY1- arc->a2diRect.drY0;

    as= -atan2(	(double) w* ( ys- ym ), (double) h* ( xs- xm ) );
    ae= -atan2(	(double) w* ( ye- ym ), (double) h* ( xe- xm ) );

    as= ( 180* as )/ M_PI;
    ae= ( 180* ae )/ M_PI;

    if  ( ae < as )
	{ ae += 360;	}

    as= 64* as;
    ae= 64* ae;

    arc->a2diAngleFrom= (int)as;
    arc->a2diAngleStep= (int)ae- (int)as;

    return;
    }

static int appMetaDrawArcX11(	DeviceContext *			dc,
				void *				through,
				const DocumentRectangle *	dr,
				int				xs,
				int				ys,
				int				xe,
				int				ye,
				int				fillInside,
				int				drawBorder )
    {
    DeviceContextX11 *	dcx= (DeviceContextX11 *)through;
    DrawingSurface	ds= dcx->dcxDrawingSurface;

    Arc2DI		arc;

    appMetaGetArcX11( dcx, &arc, dr, xs, ys, xe, ye );

    if  ( drawBorder )
	{
	drawSetSystemColor( ds, &(dcx->dcxPenColor) );
	drawArc( ds, &arc );
	}

    return 0;
    }

static int appMetaDrawChordX11(	DeviceContext *			dc,
				void *				through,
				const DocumentRectangle *	dr,
				int				xs,
				int				ys,
				int				xe,
				int				ye,
				int				fillInside,
				int				drawBorder )
    {
    DeviceContextX11 *	dcx= (DeviceContextX11 *)through;
    DrawingSurface	ds= dcx->dcxDrawingSurface;

    Arc2DI		arc;

    appMetaGetArcX11( dcx, &arc, dr, xs, ys, xe, ye );

    if  ( drawBorder )
	{
	drawSetSystemColor( ds, &(dcx->dcxPenColor) );

	/* WRONG */
	drawArc( ds, &arc );
	}

    return 0;
    }

static int appMetaDrawPieX11(	DeviceContext *			dc,
				void *				through,
				const DocumentRectangle *	dr,
				int				xs,
				int				ys,
				int				xe,
				int				ye,
				int				fillInside,
				int				drawBorder )
    {
    DeviceContextX11 *	dcx= (DeviceContextX11 *)through;
    DrawingSurface	ds= dcx->dcxDrawingSurface;

    Arc2DI		arc;

    appMetaGetArcX11( dcx, &arc, dr, xs, ys, xe, ye );

    if  ( fillInside )
	{
	drawSetSystemColor( ds, &(dcx->dcxBrushColor) );
	drawFillArc( ds, &arc );
	}

    if  ( drawBorder )
	{
	drawSetSystemColor( ds, &(dcx->dcxPenColor) );
	drawArc( ds, &arc );
	}

    return 0;
    }

static int appMetaDrawEllipseX11(
				DeviceContext *			dc,
				void *				through,
				const DocumentRectangle *	dr,
				int				fillInside,
				int				drawBorder )
    {
    DeviceContextX11 *	dcx= (DeviceContextX11 *)through;
    DrawingSurface	ds= dcx->dcxDrawingSurface;

    Arc2DI		arc;

    arc.a2diRect= *dr;
    arc.a2diAngleFrom= 0;
    arc.a2diAngleStep= 64* 360;

    if  ( fillInside )
	{
	drawSetSystemColor( ds, &(dcx->dcxBrushColor) );
	drawFillArc( ds, &arc );
	}

    if  ( drawBorder )
	{
	drawSetSystemColor( ds, &(dcx->dcxPenColor) );
	drawArc( ds, &arc );
	}

    return 0;
    }

static void appMeta_SetFillX11(		DeviceContextX11 *	dcx,
					int			before )
    {
    DrawingSurface	ds= dcx->dcxDrawingSurface;

    if  ( before )
	{ drawSetSystemColor( ds, &(dcx->dcxBrushColor) );	}

    if  ( dcx->dcxFillTiled && before )
	{
#	ifdef USE_MOTIF
	XSetFillStyle( ds->dsDisplay, ds->dsGc, FillTiled );
#	endif
#	ifdef USE_GTK
	gdk_gc_set_fill( ds->dsGc, GDK_TILED );
#	endif
	}

    if  ( dcx->dcxFillTiled && ! before )
	{
#	ifdef USE_MOTIF
	XSetFillStyle( ds->dsDisplay, ds->dsGc, FillSolid );
#	endif
#	ifdef USE_GTK
	gdk_gc_set_fill( ds->dsGc, GDK_SOLID );
#	endif
	}

    return;
    }

static int appMeta_PolyPolygonX11(	DeviceContext *		dc,
					void *			through,
					int			polyCount,
					const int *		pointCounts,
					const Point2DI *	points,
					int			fillInsides,
					int			drawBorders,
					int			closePath )
    {
    DeviceContextX11 *	dcx= (DeviceContextX11 *)through;
    DrawingSurface	ds= dcx->dcxDrawingSurface;
    int			poly;

    for ( poly= 0; poly < polyCount; poly++ )
	{
	if  ( fillInsides )
	    {
	    appMeta_SetFillX11( dcx, 1 );

	    drawFillPolygon( ds, points, pointCounts[poly] );

	    appMeta_SetFillX11( dcx, 0 );
	    }

	if  ( drawBorders )
	    {
	    drawSetSystemColor( ds, &(dcx->dcxPenColor) );

	    drawLines( ds, points, pointCounts[poly], closePath );
	    }

	points += pointCounts[poly];
	}

    return 0;
    }

static int appMeta_PolyBezierX11(	DeviceContext *		dc,
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

static int appMetaDrawStringX11(	DeviceContext *		dc,
					void *			through,
					int			x0,
					int			y0,
					const MemoryBuffer *	text )
    {
    DeviceContextX11 *	dcx= (DeviceContextX11 *)through;
    DrawingSurface	ds= dcx->dcxDrawingSurface;

    DocumentRectangle	drText;
    int			wide;

    const char *	s= utilMemoryBufferGetString( text );
    int			len= text->mbSize;

    if  ( dcx->dcxScreenFont < 0 )
	{ XDEB(dcx->dcxScreenFont); return 0;	}

    drawGetTextExtents( &drText, ds, x0, y0, dcx->dcxScreenFont, s, len );
    wide= drText.drX1- drText.drX0;

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
	int	xshift;

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
	int	yshift;

	case TA_TOP:
	    yshift= y0- drText.drY0;
	    y0 += yshift;
	    drText.drY0 += yshift;
	    drText.drY1 += yshift;
	    break;

	case TA_BOTTOM:
	    yshift= y0- drText.drY1;
	    y0 += yshift;
	    drText.drY0 += yshift;
	    drText.drY1 += yshift;
	    break;

	case TA_BASELINE:
	    break;

	default:
	    break;
	}

    if  ( dc->dcBkMode == OPAQUE )
	{
	drawSetSystemColor( ds, &(dcx->dcxBkColor) );

	drawFillRectangle( ds, &drText );
	}

    drawSetSystemColor( ds, &(dcx->dcxTextColor) );
    drawString( ds, x0, y0, dcx->dcxScreenFont, s, len );

    if  ( dc->dcFont.lfTextAttribute.taTextIsUnderlined )
	{
	int			thick;
	int			y;
	DocumentRectangle	drUnderline= drText;

	drawGetUnderlineGeometry( &thick, &y, ds, dcx->dcxScreenFont, y0 );
	drUnderline.drY0= y;
	drUnderline.drY1= y+ thick- 1;

	drawFillRectangle( ds, &drUnderline );
	}

    switch( dc->dcTextAlignment & 0x01 )
	{
	case TA_NOUPDATECP:
	    break;

	case TA_UPDATECP:
	    /*
	    XDEB(dc->dcTextAlignment & 0x01 );
	    */
	    break;

	default:
	    XDEB(dc->dcTextAlignment & 0x01 );
	    break;
	}

    return 0;
    }

static int appMetaPatBltX11(	DeviceContext *			dc,
				void *				through,
				long				rop,
				const DocumentRectangle *	drOutput )
    {
    DeviceContextX11 *	dcx= (DeviceContextX11 *)through;
    DrawingSurface	ds= dcx->dcxDrawingSurface;

    DocumentRectangle	drHere;

    geoNormalizeRectangle( &drHere, drOutput );

    if  ( dc->dcFillInsides )
	{
	drawSetSystemColor( ds, &(dcx->dcxBrushColor) );
	drawFillRectangle( ds, &drHere );
	}

    return 0;
    }

static int appMetaSetPolyFillModeX11(	DeviceContext *		dc,
					void *			through,
					int			mode )
    {
    DeviceContextX11 *	dcx= (DeviceContextX11 *)through;
    DrawingSurface	ds= dcx->dcxDrawingSurface;

    switch( mode )
	{
	case ALTERNATE:
#	    ifdef USE_MOTIF
	    XSetFillRule( ds->dsDisplay, ds->dsGc, EvenOddRule );
#	    endif
#	    ifdef USE_GTK
	    appDrawGtkSetXFillRule( ds->dsGc, GDK_EVEN_ODD_RULE );
#	    endif
	    break;

	case WINDING:
#	    ifdef USE_MOTIF
	    XSetFillRule( ds->dsDisplay, ds->dsGc, WindingRule );
#	    endif
#	    ifdef USE_GTK
	    appDrawGtkSetXFillRule( ds->dsGc, GDK_WINDING_RULE );
#	    endif
	    break;

	default:
	    break;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Fill the background of the pixmap.					*/
/*									*/
/************************************************************************/

static void appMetaFillBackgroundX11(	DeviceContextX11 *	dcx )
    {
    DrawingSurface	ds= dcx->dcxDrawingSurface;

    DocumentRectangle	drAll;

    drAll.drX0= 0;
    drAll.drY0= 0;
    drAll.drX1= dcx->dcxPixelsWide- 1;
    drAll.drY1= dcx->dcxPixelsHigh- 1;

    drawSetForegroundColorWhite( ds );
    drawFillRectangle( ds, &drAll );

    return;
    }

static int appMetaSetTextColorX11(	DeviceContext *		dc,
					void *			through,
					int			r,
					int			g,
					int			b )
    {
    DeviceContextX11 *	dcx= (DeviceContextX11 *)through;
    AppColors *		ac= dcx->dcxColors;

    if  ( appColorRgb( &(dcx->dcxTextColor), ac, r, g, b ) )
	{ LDEB(1);  return -1;	}

    return 0;
    }

static int appMetaSetBkColorX11(	DeviceContext *		dc,
					void *			through,
					int			r,
					int			g,
					int			b )
    {
    DeviceContextX11 *	dcx= (DeviceContextX11 *)through;
    AppColors *		ac= dcx->dcxColors;

    if  ( appColorRgb( &(dcx->dcxBkColor), ac, r, g, b ) )
	{ LDEB(1);  return -1;	}

    return 0;
    }

/************************************************************************/

static int appMetaStartX11DeviceContext(
				DeviceContextX11 *		dcx,
				DeviceContext *			dc,
				DrawingSurface			ds,
				const MetafilePlayer * 		player,
				int				objectCount,
				const DocumentRectangle *	drLogical,
				int				mapMode )
    {
    appMetaInitDeviceContextX11( dcx, dc, ds, player,
			    objectCount, drLogical, mapMode,
			    player->mpTwipsWide, player->mpTwipsHigh,
			    player->mpPixelsWide, player->mpPixelsHigh );

    if  ( appMetaSetDeviceDefaultsX11( dcx, dc ) )
	{ LDEB(1); return -1;	}

    dcx->dcxFillTiled= 0;
    dcx->dcxFillHatched= 0;
    dcx->dcxColors= ds->dsColors;

    appMetaFillBackgroundX11( dcx );

    dc->dcCleanObject= appMetaCleanObjectX11;
    dc->dcDrawRasterImage= appDrawMetaBitmapImageX11;
    dc->dcSelectPenObject= appMetaSelectPenObjectX11;
    dc->dcSelectBrushObject= appMetaSelectBrushObjectX11;
    dc->dcSelectFontObject= appMetaSelectFontObjectX11;
    dc->dcSelectPatternBrushObject= appMetaSelectPatternBrushObjectX11;
    dc->dcDrawPolyPolygon= appMeta_PolyPolygonX11;
    dc->dcDrawPolyBezier= appMeta_PolyBezierX11;
    dc->dcDrawString= appMetaDrawStringX11;
    dc->dcPatBlt= appMetaPatBltX11;
    dc->dcDrawPie= appMetaDrawPieX11;
    dc->dcDrawArc= appMetaDrawArcX11;
    dc->dcDrawChord= appMetaDrawChordX11;
    dc->dcDrawEllipse= appMetaDrawEllipseX11;
    dc->dcDrawRoundedRectangle= appMetaDrawRoundedRectangleX11;
    dc->dcSetTextColor= appMetaSetTextColorX11;
    dc->dcSetBkColor= appMetaSetBkColorX11;
    dc->dcSetPolyFillMode= appMetaSetPolyFillModeX11;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Play the contents of a 'wmf' original windows metafile.		*/
/*									*/
/************************************************************************/

int appMetaPlayWmfX11(	void **			pPrivate,
			DrawingSurface		ds,
			const MetafilePlayer *	player )
    {
    WmfHeader		wh;

    DeviceContextX11	dcx;
    DeviceContext	dc;

    DocumentRectangle	drLogical;

    drLogical.drX0= 0;
    drLogical.drY0= 0;
    drLogical.drX1= player->mpXWinExt;
    drLogical.drY1= player->mpYWinExt;

    if  ( player->mpXWinExt == 0 || player->mpYWinExt == 0 )
	{
	/*
	LLDEB(player->mpXWinExt,player->mpYWinExt);
	LLDEB(player->mpPixelsWide,player->mpPixelsHigh);
	*/
	drLogical.drX1= player->mpPixelsWide;
	drLogical.drY1= player->mpPixelsHigh;
	}

    if  ( sioInGetByte( player->mpInputStream ) == EOF )
	{ return 0;	}
    sioInUngetLastRead( player->mpInputStream );

    if  ( bmMetaReadWmfHeader( &wh, player->mpInputStream ) )
	{ LDEB(1); return -1;		}

    WMFDEB(appDebug(
	    "X11 WMF LOGICAL: [%d x %d] PIXELS [%d x %d] TWIPS: [%d x %d]\n",
				    drLogical.drX1, drLogical.drY1,
				    player->mpPixelsWide, player->mpPixelsHigh,
				    player->mpTwipsWide, player->mpTwipsHigh ));

    if  ( appMetaStartX11DeviceContext( &dcx, &dc, ds, player,
		    wh.wh_objectCount, &drLogical, player->mpMapMode ) )
	{ LDEB(1); return -1;	}

    if  ( appMetaPlayWmf( &dc, (void *)&dcx ) )
	{ LDEB(1);	}

    appMetaCleanDeviceContextX11( &dc, &dcx );

    return 0;
    }

int appMetaPlayEmfX11(	void **			pPrivate,
			DrawingSurface		ds,
			const MetafilePlayer *	player )
    {
    EmfHeader		eh;

    int			done;
    DeviceContextX11	dcx;
    DeviceContext	dc;

    done= bmMetaReadEmfHeader( &eh, player->mpInputStream );
    if  ( done < 0 )
	{ LDEB(done); return -1;		}
    while( done < eh.eh_headerSize )
	{
	if  ( sioInGetByte( player->mpInputStream ) < 0 )
	    { LDEB(1); return -1;	}

	done++;
	}

    WMFDEB(appDebug( "X11 EMF LOGICAL: [%d..%d x %d..%d] PIXELS: [%d x %d] TWIPS: [%d x %d]\n",
				    eh.ehBounds.drX0, eh.ehBounds.drX1,
				    eh.ehBounds.drY0, eh.ehBounds.drY1,
				    player->mpPixelsWide, player->mpPixelsHigh,
				    player->mpTwipsWide, player->mpTwipsHigh ));

    if  ( appMetaStartX11DeviceContext( &dcx, &dc, ds, player,
			eh.eh_numOfHandles, &(eh.ehBounds), MM_ANISOTROPIC ) )
	{ LDEB(1); return -1;	}


    /*********/
    if  ( appMetaPlayEmf( &dc, (void *)&dcx ) )
	{ LDEB(1);	}

    appMetaCleanDeviceContextX11( &dc, &dcx );

    return 0;
    }


int appMacPictPlayFileX11(	void **			pPrivate,
				DrawingSurface		ds,
				const MetafilePlayer *	player )
    {
    MacPictHeader	mph;

    int			done;
    DeviceContextX11	dcx;
    DeviceContext	dc;
    const int		handleCount= MACPICThandle_COUNT;

    done= appMacPictGetDeviceHeader( &mph, player->mpInputStream );
    if  ( done < 0 )
	{ LDEB(done); return -1;		}

    WMFDEB(appDebug(
	    "X11 PICT LOGICAL: [%d x %d] PIXELS [%d x %d] TWIPS: [%d x %d]\n",
				    player->mpXWinExt, player->mpYWinExt,
				    player->mpPixelsWide, player->mpPixelsHigh,
				    player->mpTwipsWide, player->mpTwipsHigh ));

    if  ( appMetaStartX11DeviceContext( &dcx, &dc, ds, player, handleCount,
					    &(mph.mphFrame), MM_ANISOTROPIC ) )
	{ LDEB(1); return -1;	}

    dc.dcMacPictVersion= mph.mphVersion;

    /*********/
    if  ( appMacPictPlayPict( &dc, (void *)&dcx ) )
	{ LDEB(1);	}

    appMetaCleanDeviceContextX11( &dc, &dcx );

    return 0;
    }

