#   include	"drawMetaConfig.h"

#   include	<stddef.h>
#   include	<string.h>

#   include	"drawMetafileImpl.h"
#   include	"drawMetafileSvg.h"
#   include	"drawWinMetaImpl.h"
#   include	<psFontMetrics.h>
#   include	<utilMatchFont.h>
#   include	<utilDocFont.h>
#   include	<bmWmfIo.h>
#   include	<bmEmfIo.h>
#   include	"drawMacPictImpl.h"
#   include	"drawImageSvg.h"
#   include	<psShading.h>

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
/*  Find out whether we must use a hash.				*/
/*									*/
/************************************************************************/

static int appMetaSelectBrushObjectSvg( 	DeviceContext *		dc,
						void *			through,
						LogicalBrush *		lb )
    {
    SvgWriter *			sw= (SvgWriter *)through;

    WMFDEB(appDebug( "SelectObject() " ));
    WMFDEB(appDebug( "BRUSH\n" ));

    appMetaSelectBrushObjectImpl( dc, through, lb );

    if  ( dc->dcFillHatched && lb->lbTileNumber < 0 )
	{
	lb->lbTileNumber= sw->swPatternCount++;

	switch( lb->lbHatch )
	    {
	    case HS_HORIZONTAL:
		svgDefineShading( sw, lb->lbTileNumber, PSshdHORIZ,
							    &(lb->lbColor) );
		break;
	    case HS_VERTICAL:
		svgDefineShading( sw, lb->lbTileNumber, PSshdVERT,
							    &(lb->lbColor) );
		break;
	    case HS_FDIAGONAL:
		svgDefineShading( sw, lb->lbTileNumber, PSshdFDIAG,
							    &(lb->lbColor) );
		break;
	    case HS_BDIAGONAL:
		svgDefineShading( sw, lb->lbTileNumber, PSshdBDIAG,
							    &(lb->lbColor) );
		break;
	    case HS_CROSS:
		svgDefineShading( sw, lb->lbTileNumber, PSshdCROSS,
							    &(lb->lbColor) );
		break;
	    case HS_DIAGCROSS:
		svgDefineShading( sw, lb->lbTileNumber, PSshdDCROSS,
							    &(lb->lbColor) );
		break;
	    default:
		LDEB(dc->dcBrush.lbHatch);
	    }
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Play a windows metafile to SVG.					*/
/*									*/
/************************************************************************/

static int appMetaSvgEmitFill(	XmlWriter *		xw,
				int			fillInsides,
				const DeviceContext *	dc )
    {
    if  ( ! fillInsides )
	{
	xmlWriteStringAttribute( xw, "fill", "none" );
	return 0;
	}

    if  ( dc->dcFillPattern )
	{
	char			url[20];
	const MetaFileObject *	mfo;
	const PatternBrush *	pb;

	mfo= &(dc->dcObjects[dc->dcCurrentBrushObject]);
	pb= &(mfo->mfoPatternBrush);

	sprintf( url, "url(#pat%d)", pb->pbTileNumber );
	xmlWriteStringAttribute( xw, "fill", url );
	return 0;
	}

    if  ( dc->dcFillHatched )
	{
	char			url[20];
	const MetaFileObject *	mfo;
	const LogicalBrush *	lb;

	mfo= &(dc->dcObjects[dc->dcCurrentBrushObject]);
	lb= &(mfo->mfoLogicalBrush);

	sprintf( url, "url(#pat%d)", lb->lbTileNumber );
	xmlWriteStringAttribute( xw, "fill", url );
	return 0;
	}

    xmlWriteRgb8Attribute( xw, "fill", &(dc->dcBrush.lbColor) );
    return 0;
    }

static int appMetaSvgEmitStroke(XmlWriter *		xw,
				int			drawBorders,
				DeviceContext *		dc )
    {
    int				widthTwips;
    unsigned char		dots[6];
    int				dotCount;

    if  ( ! drawBorders )
	{ return 0;	}

    widthTwips= appMetaGetPenWidth( dc, &(dc->dcPen) );

    xmlWriteRgb8Attribute( xw, "stroke", &(dc->dcPen.lpColor) );
    xmlWriteIntAttribute( xw, "stroke-width", widthTwips );

    dotCount= appMetaGetPenDots( dc, dots, &(dc->dcPen) );
    if  ( dotCount > 0 )
	{
	char		dasharray[100];
	char *		to= dasharray;
	int		i;
	const char *	before= "";

	for ( i= 0; i < dotCount; i++ )
	    {
	    sprintf( to, "%s%d", before, dots[i] );
	    to += strlen( to );
	    before= ",";
	    }

	xmlWriteStringAttribute( xw, "stroke-dasharray", dasharray );
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Insert a bitmap image in the SVG output for a metafile.	*/
/*									*/
/************************************************************************/

static int appDrawMetaBitmapImageSvg(
				DeviceContext *			dc,
				void *				through,
				const RasterImage *		abiSrc,
				const DocumentRectangle *	drSrc,
				const DocumentRectangle *	drDest )
    {
    SvgWriter *			sw= (SvgWriter *)through;

    return drawRasterImageSvg( sw, abiSrc, drSrc, drDest );
    }

static int appCleanObjectSvg(		DeviceContext *		dc,
					void *			through,
					MetaFileObject *	mfo )
    { return 0;	}

static int appMeta_PolyPolygonSvg(	DeviceContext *		dc,
					void *			through,
					int			polyCount,
					const int *		pointCounts,
					const Point2DI *	points,
					int			fillInsides,
					int			drawBorders,
					int			closePath )
    {
    SvgWriter *			sw= (SvgWriter *)through;
    XmlWriter *			xw= &(sw->swXmlWriter);

    int				poly;
    const char *		sep= "";

    xmlPutString( "<path d=\"", xw );

    for ( poly= 0; poly < polyCount; poly++ )
	{
	int		x0= 0;
	int		y0= 0;
	const char *	command= "M";

	int		point;

	for ( point= 0; point < pointCounts[poly]; point++ )
	    {
	    sioOutPrintf( xw->xwSos, "%s%s %d,%d", sep,
			    command, points->x- x0, points->y- y0 );

	    sep= "\n\t";
	    command= "l";
	    x0= points->x;
	    y0= points->y;
	    points++;
	    }

	if  ( closePath )
	    { xmlPutString( "Z", xw ); }
	}

    xmlPutString( "\"", xw );
    xmlNewLine( xw );

    appMetaSvgEmitStroke( xw, drawBorders, dc );
    appMetaSvgEmitFill( xw, fillInsides, dc );

    xmlPutString( "/>", xw );
    xmlNewLine( xw );

    return 0;
    }

static int appMeta_PolyBezierSvg(	DeviceContext *		dc,
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

static int appMetaDrawStringSvg(	DeviceContext *		dc,
					void *			through,
					int			x0,
					int			y0,
					const MemoryBuffer *	text )
    {
    SvgWriter *		sw= (SvgWriter *)through;
    XmlWriter *		xw= &(sw->swXmlWriter);

    const AfmFontInfo *	afi= dc->dcAfi;
    LogicalFont *	lf= &(dc->dcFont);

    int			wide;
    DocumentRectangle	drText;
    const int		withKerning= 0;
    const int		vswap= 1;
    int			fontSizeTwipsGeo;
    int			fontSizeTwipsText;

    const char *	s= utilMemoryBufferGetString( text );
    int			len= text->mbSize;

    char		transform[100];

    if  ( ! afi )
	{ XDEB(afi); return -1;	}

    fontSizeTwipsGeo= appWinMetaOutputSize( dc, lf->lfHeight );
    fontSizeTwipsText= appWinMetaOutputSize( dc, ( 85* lf->lfHeight ) / 100 );

    wide= psCalculateStringExtents( &drText, s, len,
				    fontSizeTwipsGeo, withKerning, vswap, afi );
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
	    drText.drX0 += xshift;
	    drText.drX1 += xshift;
	    break;

	case TA_CENTER:
	    xshift= -wide/ 2;
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
	    psFontBBox( &drFontBBox, &drFontAscDesc, fontSizeTwipsGeo,
								vswap, afi );
	    yshift= -drFontAscDesc.drY0;
	    y0 += yshift;
	    drText.drY0 += yshift;
	    drText.drY1 += yshift;
	    break;

	case TA_BOTTOM:
	    psFontBBox( &drFontBBox, &drFontAscDesc, fontSizeTwipsGeo,
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

    if  ( dc->dcFont.lfOrientation != 0 )
	{
	sprintf( transform, "translate(%d,%d)", x0, y0 );

	xmlPutString( "<g ", xw );
	xmlWriteStringAttribute( xw, "transform", transform );
	xmlPutString( ">", xw );

	drText.drX0 -= x0;
	drText.drX1 -= x0;
	drText.drY0 -= y0;
	drText.drY1 -= y0;

	x0= y0= 0;

	sprintf( transform, "rotate(%g)", -0.1* dc->dcFont.lfOrientation );
	}

    if  ( dc->dcBkMode == OPAQUE )
	{
	xmlPutString( "<rect ", xw );

	if  ( dc->dcFont.lfOrientation != 0 )
	    { xmlWriteStringAttribute( xw, "transform", transform );	}
	svgWriteRectangleAttributes( sw, &drText );

	xmlWriteRgb8Attribute( xw, "fill", &(dc->dcBkColor) );

	xmlPutString( "/>", xw );
	xmlNewLine( xw );
	}

    xmlPutString( "<text ", xw );

    xmlWriteStringAttribute( xw, "font-family", dc->dcFont.lfFaceNameUtf8 );
    xmlWriteIntAttribute( xw, "font-size", fontSizeTwipsText );
    xmlWriteRgb8Attribute( xw, "fill", &(dc->dcTextColor) );

    if  ( dc->dcFont.lfTextAttribute.taFontIsSlanted )
	{ xmlWriteStringAttribute( xw, "font-style", "italic" );	}
    if  ( dc->dcFont.lfTextAttribute.taFontIsBold )
	{ xmlWriteStringAttribute( xw, "font-weight", "bold" );		}
    if  ( dc->dcFont.lfTextAttribute.taTextIsUnderlined )
	{
	/* Does not work with Firefox */
	xmlWriteRgb8Attribute( xw, "stroke", &(dc->dcTextColor) );
	xmlWriteStringAttribute( xw, "text-decoration", "underline" );
	}

    if  ( dc->dcFont.lfOrientation != 0 )
	{ xmlWriteStringAttribute( xw, "transform", transform );	}
    if  ( x0 != 0 )
	{ xmlWriteIntAttribute( xw, "x", x0 );	}
    if  ( y0 != 0 )
	{ xmlWriteIntAttribute( xw, "y", y0 );	}

    switch( dc->dcTextAlignment & 0x06 )
	{
	case TA_LEFT:
	    /* xmlWriteStringAttribute( xw, "text-anchor", "start" ); */
	    break;
	case TA_RIGHT:
	    xmlWriteStringAttribute( xw, "text-anchor", "end" );
	    break;
	case TA_CENTER:
	    xmlWriteStringAttribute( xw, "text-anchor", "middle" );
	    break;
	default:
	    XDEB(dc->dcTextAlignment & 0x06);
	    break;
	}

    xmlPutString( ">", xw );
    xmlNewLine( xw );

    xmlEscapeCharacters( xw, s, len );

    xmlPutString( "</text>", xw );
    xmlNewLine( xw );

    if  ( dc->dcFont.lfOrientation != 0 )
	{
	xmlPutString( "</g>", xw );
	xmlNewLine( xw );
	}

    return 0;
    }

static int appMetaPatBltSvg(	DeviceContext *			dc,
				void *				through,
				long				rop,
				const DocumentRectangle *	drOutput )
    {
    SvgWriter *		sw= (SvgWriter *)through;
    XmlWriter *		xw= &(sw->swXmlWriter);

    if  ( dc->dcFillInsides )
	{
	DocumentRectangle	drNorm;

	geoNormalizeRectangle( &drNorm, drOutput );

	xmlPutString( "<rect ", xw );

	svgWriteRectangleAttributes( sw, &drNorm );
	appMetaSvgEmitFill( xw, dc->dcFillInsides, dc );

	xmlPutString( "/>", xw );
	xmlNewLine( xw );
	}

    return 0;
    }

static int appMetaDrawArcSvg(	DeviceContext *			dc,
				void *				through,
				const DocumentRectangle *	dr,
				int				xs,
				int				ys,
				int				xe,
				int				ye,
				int				fillInside,
				int				drawBorder )
    {
    SvgWriter *		sw= (SvgWriter *)through;
    XmlWriter *		xw= &(sw->swXmlWriter);

    int			rx=  ( dr->drX1- dr->drX0+ 1 )/ 2;
    int			ry=  ( dr->drY1- dr->drY0+ 1 )/ 2;

    const int		xAxisRotation= 0;
    const int		largeArcFlag= 0;
    const int		sweepFlag= 0;

    xmlPutString( "<path d=\"", xw );

    sioOutPrintf( xw->xwSos, "M %d,%d A %d,%d "
		    "%d %d %d "
		    "%d,%d",
		    xs, ys, rx, ry,
		    xAxisRotation, largeArcFlag, sweepFlag,
		    xe, ye );

    xmlPutString( "\"", xw );
    xmlNewLine( xw );

    appMetaSvgEmitStroke( xw, drawBorder, dc );
    appMetaSvgEmitFill( xw, fillInside, dc );

    xmlPutString( "/>", xw );
    xmlNewLine( xw );

    return 0;
    }

static int appMetaDrawPieSvg(	DeviceContext *			dc,
				void *				through,
				const DocumentRectangle *	dr,
				int				xs,
				int				ys,
				int				xe,
				int				ye,
				int				fillInside,
				int				drawBorder )
    {
    SvgWriter *		sw= (SvgWriter *)through;
    XmlWriter *		xw= &(sw->swXmlWriter);

    int			cx=  ( dr->drX1+ dr->drX0+ 1 )/ 2;
    int			cy=  ( dr->drY1+ dr->drY0+ 1 )/ 2;
    int			rx=  ( dr->drX1- dr->drX0+ 1 )/ 2;
    int			ry=  ( dr->drY1- dr->drY0+ 1 )/ 2;

    const int		xAxisRotation= 0;
    const int		largeArcFlag= 0;
    const int		sweepFlag= 0;

    xmlPutString( "<path d=\"", xw );

    sioOutPrintf( xw->xwSos, "M %d,%d A %d,%d "
		    "%d %d %d "
		    "%d,%d "
		    "l %d,%d "
		    "l %d,%d",
		    xs, ys, rx, ry,
		    xAxisRotation, largeArcFlag, sweepFlag,
		    xe, ye,
		    cx- xe, cy- ye,
		    xs- cx, ys- ys );

    xmlPutString( "\"", xw );
    xmlNewLine( xw );

    appMetaSvgEmitStroke( xw, drawBorder, dc );
    appMetaSvgEmitFill( xw, fillInside, dc );

    xmlPutString( "/>", xw );
    xmlNewLine( xw );

    return 0;
    }

static int appMetaDrawChordSvg(	DeviceContext *			dc,
				void *				through,
				const DocumentRectangle *	dr,
				int				xs,
				int				ys,
				int				xe,
				int				ye,
				int				fillInside,
				int				drawBorder )
    {
    SvgWriter *		sw= (SvgWriter *)through;
    XmlWriter *		xw= &(sw->swXmlWriter);

    int			rx=  ( dr->drX1- dr->drX0+ 1 )/ 2;
    int			ry=  ( dr->drY1- dr->drY0+ 1 )/ 2;

    const int		xAxisRotation= 0;
    const int		largeArcFlag= 0;
    const int		sweepFlag= 0;

    xmlPutString( "<path d=\"", xw );

    sioOutPrintf( xw->xwSos, "M %d,%d A %d,%d "
		    "%d %d %d "
		    "%d,%d "
		    "l %d,%d",
		    xs, ys, rx, ry,
		    xAxisRotation, largeArcFlag, sweepFlag,
		    xe, ye,
		    xs- xe, ys- ye );

    xmlPutString( "\"", xw );
    xmlNewLine( xw );

    appMetaSvgEmitStroke( xw, drawBorder, dc );
    appMetaSvgEmitFill( xw, fillInside, dc );

    xmlPutString( "/>", xw );
    xmlNewLine( xw );

    return 0;
    }

static int appMetaDrawEllipseSvg(DeviceContext *		dc,
				void *				through,
				const DocumentRectangle *	dr,
				int				fillInside,
				int				drawBorder )
    {
    SvgWriter *			sw= (SvgWriter *)through;
    XmlWriter *			xw= &(sw->swXmlWriter);

    xmlPutString( "<ellipse ", xw );

    xmlWriteIntAttribute( xw, "cx", ( dr->drX0+ dr->drX1 )/ 2 );
    xmlWriteIntAttribute( xw, "cy", ( dr->drY0+ dr->drY1 )/ 2 );
    xmlWriteIntAttribute( xw, "rx", ( dr->drX1- dr->drX0+ 1 )/ 2 );
    xmlWriteIntAttribute( xw, "ry", ( dr->drY1- dr->drY0+ 1 )/ 2 );

    appMetaSvgEmitStroke( xw, drawBorder, dc );
    appMetaSvgEmitFill( xw, fillInside, dc );

    xmlPutString( "/>", xw );
    xmlNewLine( xw );
    return 0;
    }

static int appMetaDrawRoundedRectangleSvg(
				DeviceContext *			dc,
				void *				through,
				const DocumentRectangle *	dr,
				int				wide,
				int				high,
				int				fillInside,
				int				drawBorder )
    {
    SvgWriter *			sw= (SvgWriter *)through;
    XmlWriter *			xw= &(sw->swXmlWriter);

    xmlPutString( "<rect ", xw );

    svgWriteRectangleAttributes( sw, dr );
    xmlWriteIntAttribute( xw, "rx", wide );
    xmlWriteIntAttribute( xw, "ry", high );

    appMetaSvgEmitStroke( xw, drawBorder, dc );
    appMetaSvgEmitFill( xw, fillInside, dc );

    xmlPutString( "/>", xw );
    xmlNewLine( xw );
    return 0;
    }

static int appMetaIgnoreColorSvg(	DeviceContext *		dc,
					void *			through,
					int			r,
					int			g,
					int			b )
    { return 0;	}

static int appMetaSetPolyFillModeSvg(	DeviceContext *		dc,
					void *			through,
					int			mode )
    { return 0;	}

/************************************************************************/
/*									*/
/*  Start a pattern fill.						*/
/*									*/
/************************************************************************/

static int appMetaSelectPatternBrushObjectSvg(	DeviceContext *		dc,
						void *			through,
						PatternBrush *		pb )
    {
    SvgWriter *			sw= (SvgWriter *)through;
    XmlWriter *			xw= &(sw->swXmlWriter);
    char			id[20];
    char			viewBox[20];

    if  ( pb->pbTileNumber < 0 )
	{
	const RasterImage *	abi= pb->pbAbi;
	DocumentRectangle	drSrc;
	DocumentRectangle	drDest;

	drSrc.drX0= 0;
	drSrc.drY0= 0;
	drSrc.drX1= abi->riDescription.bdPixelsWide- 1;
	drSrc.drY1= abi->riDescription.bdPixelsHigh- 1;

	drDest.drX0= 0;
	drDest.drY0= 0;
	drDest.drX1= 20* abi->riDescription.bdPixelsWide- 1;
	drDest.drY1= 20* abi->riDescription.bdPixelsHigh- 1;

	pb->pbTileNumber= sw->swPatternCount++;

	sprintf( id, "pat%d", pb->pbTileNumber );
	sprintf( viewBox, "0 0 %d %d",
				drSrc.drX1- drSrc.drX0+ 1,
				drSrc.drX1- drSrc.drX0+ 1 );

	xmlPutString( "<pattern ", xw );
	xmlWriteStringAttribute( xw, "id", id );
	xmlWriteStringAttribute( xw, "patternUnits", "userSpaceOnUse" );
	xmlWriteIntAttribute( xw, "width",  drDest.drX1- drDest.drX0+ 1 );
	xmlWriteIntAttribute( xw, "height", drDest.drY1- drDest.drY0+ 1 );
	xmlWriteStringAttribute( xw, "viewBox", viewBox );
	xmlPutString( ">", xw );
	xmlNewLine( xw );

	if  ( appDrawMetaBitmapImageSvg( dc, through, abi, &drSrc, &drSrc ) )
	    { LDEB(1);	}

	xmlPutString( "</pattern>", xw );
	xmlNewLine( xw );
	}

    dc->dcFillHatched= 0;
    dc->dcFillPattern= 1;
    dc->dcFillInsides= 1;

    return 0;
    }

static int appMetaStartSvg(	DeviceContext *			dc,
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

    dc->dcCleanObject= appCleanObjectSvg;
    dc->dcDrawRasterImage= appDrawMetaBitmapImageSvg;
    dc->dcSelectPenObject= appMetaSelectPenObjectImpl;
    dc->dcSelectBrushObject= appMetaSelectBrushObjectSvg;
    dc->dcSelectFontObject= appMetaSelectFontObjectImpl;
    dc->dcSelectPatternBrushObject= appMetaSelectPatternBrushObjectSvg;
    dc->dcDrawPolyPolygon= appMeta_PolyPolygonSvg;
    dc->dcDrawPolyBezier= appMeta_PolyBezierSvg;
    dc->dcDrawString= appMetaDrawStringSvg;
    dc->dcPatBlt= appMetaPatBltSvg;
    dc->dcDrawPie= appMetaDrawPieSvg;
    dc->dcDrawArc= appMetaDrawArcSvg;
    dc->dcDrawChord= appMetaDrawChordSvg;
    dc->dcDrawEllipse= appMetaDrawEllipseSvg;
    dc->dcDrawRoundedRectangle= appMetaDrawRoundedRectangleSvg;
    dc->dcSetTextColor= appMetaIgnoreColorSvg;
    dc->dcSetBkColor= appMetaIgnoreColorSvg;
    dc->dcSetPolyFillMode= appMetaSetPolyFillModeSvg;

    return 0;
    }

int appMetaPlayWmfSvg(	SvgWriter *			sw,
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

    if  ( sioInGetByte( player->mpInputStream ) == EOF )
	{ return 0;	}
    sioInUngetLastRead( player->mpInputStream );

    if  ( bmMetaReadWmfHeader( &wh, player->mpInputStream ) )
	{ LDEB(1); return -1;		}

    if  ( appMetaStartSvg( &dc, wh.wh_objectCount, player, &drLogical,
							player->mpMapMode ) )
	{ LDEB(wh.wh_objectCount); return -1;	}

    WMFDEB(appDebug( "SVG WMF LOGICAL: [%d x %d] TWIPS: [%d x %d]\n",
				player->mpXWinExt, player->mpYWinExt,
				player->mpTwipsWide, player->mpTwipsHigh ));

    if  ( appMetaPlayWmf( &dc, (void *)sw ) )
	{ rval= -1; goto ready;	}

  ready:

    appMetaCleanDeviceContext( &dc, (void *)sw );

    return rval;
    }

int appMetaPlayEmfSvg(	SvgWriter *			sw,
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

    if  ( appMetaStartSvg( &dc, eh.eh_numOfHandles, player, &(eh.ehBounds),
							    MM_ANISOTROPIC ) )
	{ LDEB(eh.eh_numOfHandles); return -1;	}

    WMFDEB(appDebug( "SVG EMF LOGICAL: [%d..%d x %d..%d] TWIPS: [%d x %d]\n",
				    eh.ehBounds.drX0, eh.ehBounds.drX1,
				    eh.ehBounds.drY0, eh.ehBounds.drY1,
				    player->mpTwipsWide, player->mpTwipsHigh ));

    if  ( appMetaPlayEmf( &dc, (void *)sw ) )
	{ rval= -1; goto ready;	}

  ready:

    appMetaCleanDeviceContext( &dc, (void *)sw );

    return rval;
    }

int appMacPictPlayFileSvg(	SvgWriter *			sw,
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

    if  ( appMetaStartSvg( &dc, handleCount, player,
					&(mph.mphFrame), MM_ANISOTROPIC ) )
	{ LDEB(handleCount); return -1;	}

    dc.dcMacPictVersion= mph.mphVersion;

    WMFDEB(appDebug( "SVG PICT LOGICAL: [%d..%d x %d..%d] TWIPS: [%d x %d]\n",
				mph.mphFrame.drX0, mph.mphFrame.drX1,
				mph.mphFrame.drY0, mph.mphFrame.drY1,
				player->mpTwipsWide, player->mpTwipsHigh ));

    if  ( appMacPictPlayPict( &dc, (void *)sw ) )
	{ rval= -1; goto ready;	}

  ready:

    appMetaCleanDeviceContext( &dc, (void *)sw );

    return rval;
    }
