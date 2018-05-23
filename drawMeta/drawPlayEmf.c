#   include	"drawMetaConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>
#   include	<string.h>

#   include	"drawWinMetaImpl.h"
#   include	<bmEmfIo.h>
#   include	<sioEndian.h>
#   include	<uniUtf8.h>

#   include	<appDebugon.h>

# if 0
#    define	WMFDEB(x)	(x)
# else
#    define	WMFDEB(x)	/*nothing*/
# endif

/************************************************************************/

typedef struct bmi
    {
    long	offBmi;
    long	cbBmi;
    long	offBits;
    long	cbBits;
    } bmi;

static int appEmfReadBmi(	bmi *			pbmi,
				SimpleInputStream *	sis )
    {
    int	done= 0;

    pbmi->offBmi= sioEndianGetLeInt32( sis ); done += 4;
    pbmi->cbBmi= sioEndianGetLeInt32( sis ); done += 4;
    pbmi->offBits= sioEndianGetLeInt32( sis ); done += 4;
    pbmi->cbBits= sioEndianGetLeInt32( sis ); done += 4;

    return done;
    }

/************************************************************************/
/*									*/
/*  Handle a poly-polygon. For simplicity, treat it as multiple polygons*/
/*									*/
/************************************************************************/

static int appEmfGetPolyPoly(	SimpleInputStream *	sis,
				const char *		recordTypeStr,
				int			recordSize,
				DocumentRectangle *	bounds,
				int *			pPolyCount,
				int *			pPointCount,
				int **			pCounts,
				DeviceContext *		dc )
    {
    int			polyCount;
    int			pointCount;
    int			pointCount2= 0;
    int			i;

    int *		fresh;
    int			done= 0;
    int			step;

    step= bmEmfReadRectangle( bounds, sis );
    if  ( step < 0 )
	{ LDEB(step); return -1;	}
    done += step;

    polyCount= sioEndianGetLeInt32( sis ); done += 4;
    pointCount= sioEndianGetLeInt32( sis ); done += 4;

    WMFDEB(appDebug(
	    "%s( polyCount=%d, bounds=[%d..%d x %d..%d] )\n",
					recordTypeStr,
					polyCount, bounds->drX0, bounds->drX1,
					bounds->drY0, bounds->drY1 ));

    fresh= (int *)realloc( dc->dcCounts, polyCount* sizeof(int) );
    if  ( ! fresh )
	{ LXDEB(polyCount,fresh); return -1;	}
    dc->dcCounts= fresh;

    for ( i= 0; i < polyCount; i++ )
	{
	dc->dcCounts[i]= sioEndianGetLeInt32( sis ); done += 4;
	pointCount2 += dc->dcCounts[i];
	}

    if  ( pointCount != pointCount2 )
	{ LLDEB(pointCount,pointCount2);	}

    *pPolyCount= polyCount;
    *pPointCount= pointCount;
    *pCounts= dc->dcCounts;
    return done;
    }

static int appEmfDrawPolyPolygonXX(
				SimpleInputStream *	sis,
				const char *		recordTypeStr,
				void *			through,
				int			recordSize,
				DocumentRectangle *	bounds,
				DeviceContext *		dc,
				MetaGetPoints		getPoints,
				int			fillInside,
				int			drawBorder,
				int			closePath )
    {
    int			polyCount;
    int			pointCount;
    int *		counts;

    int			step;
    int			done= 0;

    step= appEmfGetPolyPoly( sis, recordTypeStr, recordSize, bounds,
				    &polyCount, &pointCount, &counts, dc );
    if  ( step < 0 )
	{ LDEB(step); return -1;	}
    done += step;

    step= (*getPoints)( dc, pointCount, sis );
    if  ( step < 0 )
	{ LLDEB(pointCount,step); return -1;	}
    done += step;

    if  ( (*dc->dcDrawPolyPolygon)( dc, through,
					polyCount, counts, dc->dcPoints,
					fillInside, drawBorder, closePath ) )
	{ LDEB(polyCount); return -1;	}

    return done;
    }

static int appEmfReadPolyPoints32(	SimpleInputStream *	sis,
					DocumentRectangle *	bounds,
					int *			pPointCount,
					Point2DI **		pPoints,
					DeviceContext *		dc )
    {
    int		done= 0;
    int		step;
    int		pointCount;

    step= bmEmfReadRectangle( bounds, sis );
    if  ( step < 0 )
	{ LDEB(step); return -1;	}
    done += step;

    pointCount= sioEndianGetLeInt32( sis ); done += 4;
    step= appWinMetaGetPoints32( dc, pointCount, sis );
    if  ( step < 0 )
	{ LDEB(step); return -1;	}
    done += step;

    *pPointCount= pointCount;
    *pPoints= dc->dcPoints;
    return done;
    }

static int appEmfReadPolyPoints16(	SimpleInputStream *	sis,
					DocumentRectangle *	bounds,
					int *			pPointCount,
					Point2DI **		pPoints,
					DeviceContext *		dc )
    {
    int		done= 0;
    int		step;
    int		pointCount;

    step= bmEmfReadRectangle( bounds, sis );
    if  ( step < 0 )
	{ LDEB(step); return -1;	}
    done += step;

    pointCount= sioEndianGetLeInt32( sis ); done += 4;
    step= appWinMetaGetPoints16( dc, pointCount, sis );
    if  ( step < 0 )
	{ LDEB(step); return -1;	}
    done += step;

    *pPointCount= pointCount;
    *pPoints= dc->dcPoints;
    return done;
    }

/************************************************************************/

static int appEmfReadPen(	DeviceContext *		dc,
				int			ob,
				SimpleInputStream *	sis )
    {
    MetaFileObject *	mfo= dc->dcObjects+ ob;
    LogicalPen *	lp= &(mfo->mfoSpecific.mfsLogicalPen);
    int			x;
    int			y;
    int			step;
    int			done= 0;

    if  ( ob < 0 || ob >= dc->dcObjectCount )
	{ LLDEB(ob,dc->dcObjectCount); return -1;	}

    appMetaCleanObject( mfo );
    /*appMetaInitObject( mfo );*/

    mfo->mfoType= MFtypePEN;

    lp->lpStyle= sioEndianGetLeInt32( sis ); done += 4;
    x= sioEndianGetLeInt32( sis ); done += 4;
    y= sioEndianGetLeInt32( sis ); done += 4;
    lp->lpWidth= ( x+ y )/ 2;

    step= bmEmfReadRgb8Color( &(lp->lpColor), sis );
    if  ( step < 0 )
	{ LDEB(step); return -1;	}
    done += step;

    return done;
    }

# if 0
static int appEmfReadLogPen(	DeviceContext *		dc,
				LogicalPen *		lp,
				SimpleInputStream *	sis )
    {
    int		done= 0;

    int		x;
    int		y;
    int		step;

    lp->lpStyle= sioEndianGetLeInt32( sis ); done += 4;

    x= sioEndianGetLeInt32( sis ); done += 4;
    y= sioEndianGetLeInt32( sis ); done += 4;
    lp->lpWidth= x;

    step= bmEmfReadRgb8Color( &(lp->lpColor), sis );
    if  ( step < 0 )
	{ LDEB(step); return -1;	}
    done += step;

    return done;
    }
# endif

static int appEmfReadLogPenEx(	DeviceContext *		dc,
				LogicalPen *		lp,
				SimpleInputStream *	sis )
    {
    int		done= 0;

    long	NumStyleEntries;

    int		step;
    int		i;

    lp->lpStyle= sioEndianGetLeInt32( sis ); done += 4;
    lp->lpWidth= sioEndianGetLeInt32( sis ); done += 4;

    (void) /* BrushStyle= */ sioEndianGetLeInt32( sis ); done += 4;

    step= bmEmfReadRgb8Color( &(lp->lpColor), sis );
    if  ( step < 0 )
	{ LDEB(step); return -1;	}
    done += step;

    (void) /* BrushHatch= */ sioEndianGetLeInt32( sis ); done += 4;

    NumStyleEntries= sioEndianGetLeInt32( sis ); done += 4;
    for ( i= 0; i < NumStyleEntries; i++ )
	{
	sioEndianGetLeInt32( sis ); done += 4;
	}

    return done;
    }

static int appEmfExtCreatePen(	DeviceContext *		dc,
				int			ob,
				int			skipped,
				SimpleInputStream *	sis )
    {
    int			step;
    int			done= 0;

    bmi			bmi;

    MetaFileObject *	mfo= dc->dcObjects+ ob;
    LogicalPen *	lp= &(mfo->mfoSpecific.mfsLogicalPen);

    if  ( ob < 0 || ob >= dc->dcObjectCount )
	{ LLDEB(ob,dc->dcObjectCount); return -1;	}

    appMetaCleanObject( mfo );
    /*appMetaInitObject( mfo );*/

    mfo->mfoType= MFtypePEN;

    done += appEmfReadBmi( &bmi, sis );

    step= appEmfReadLogPenEx( dc, lp, sis );
    if  ( step < 0 )
	{ LDEB(step); return -1;	}
    done += step;

    while( skipped+ done < bmi.offBmi )
	{
	sioEndianGetLeInt32( sis ); done += 4;
	}

    return done;
    }

static int appEmfReadBrush(	DeviceContext *		dc,
				int			ob,
				SimpleInputStream *	sis )
    {
    MetaFileObject *	mfo= dc->dcObjects+ ob;
    LogicalBrush *	lb= &(mfo->mfoSpecific.mfsLogicalBrush);
    int			done= 0;

    const char *	style;
    const char *	hatch;

    if  ( ob < 0 || ob >= dc->dcObjectCount )
	{ LLDEB(ob,dc->dcObjectCount); return -1;	}

    appMetaCleanObject( mfo );
    /*appMetaInitObject( mfo );*/

    mfo->mfoType= MFtypeBRUSH;
    lb->lbTilePixmap= (struct DrawingSurface *)0;
    lb->lbTileNumber= -1;

    lb->lbStyle= sioEndianGetLeInt32( sis ); done += 4;

    switch( lb->lbStyle )
	{
	case BS_SOLID:
	    style= "SOLID";
	    bmEmfReadRgb8Color( &(lb->lbColor), sis ); done += 4;
	    lb->lbHatch= sioEndianGetLeInt32( sis ); done += 4;
	    break;
	case BS_HOLLOW:
	    style= "HOLLOW";
	    bmEmfReadRgb8Color( &(lb->lbColor), sis ); done += 4;
	    lb->lbHatch= sioEndianGetLeInt32( sis ); done += 4;
	    break;
	case BS_HATCHED:
	    style= "HATCHED";
	    bmEmfReadRgb8Color( &(lb->lbColor), sis ); done += 4;
	    lb->lbHatch= sioEndianGetLeInt32( sis ); done += 4;
	    break;
	case BS_PATTERN:
	    style= "PATTERN";
	    bmEmfReadRgb8Color( &(lb->lbColor), sis ); done += 4;
	    lb->lbHatch= sioEndianGetLeInt32( sis ); done += 4;
	    break;
	default:
	    style= "?-?";
	    LDEB(dc->dcObjects[ob].mfoLogicalBrush.lbStyle);
	}

    switch( lb->lbHatch )
	{
	case HS_HORIZONTAL:	hatch= "HORIZONTAL";	break;
	case HS_VERTICAL:	hatch= "VERTICAL";	break;
	case HS_FDIAGONAL:	hatch= "FDIAGONAL";	break;
	case HS_BDIAGONAL:	hatch= "BDIAGONAL";	break;
	case HS_CROSS:		hatch= "CROSS";		break;
	case HS_DIAGCROSS:	hatch= "DIAGCROSS";	break;
	default:
	    hatch= "?-?";
	    break;
	}

    WMFDEB(appDebug("CreateBrushIndirect(%s,%s) ob=%d\n",
						    style, hatch, ob ));

    style= style; hatch= hatch; /* use them */

    return done;
    }

static int appEmfReadPatternBrush(	DeviceContext *		dc,
					int			ob,
					SimpleInputStream *	sis,
					int			skipped,
					int			expectBytes )
    {
    MetaFileObject *	mfo= dc->dcObjects+ ob;
    PatternBrush *	pb= &(mfo->mfoPatternBrush);
    int			done= 0;

    bmi			bmi;

    int			skip= 0;

    pb->pbUsage= sioEndianGetLeInt32( sis ); done += 4;

    done += appEmfReadBmi( &bmi, sis );

    while( skipped+ done < bmi.offBmi )
	{
	skip= sioEndianGetLeInt32( sis ); done += 4;
	}

    mfo->mfoType= MFtypePATTERNBRUSH;
    pb->pbType= 0;
    pb->pbUsage= 0;
    pb->pbAbi= (RasterImage *)0;
    pb->pbTilePixmap= (struct DrawingSurface *)0;
    pb->pbTileNumber= -1;

    expectBytes -= done;

    skip= 0;
    if  ( appMetaReadBitmapImage( &skip, &(pb->pbAbi), sis, expectBytes ) )
	{ LDEB(expectBytes); return -1; }

    done += expectBytes;
    return done;
    }

static int appEmfReadUtf16String(	char *			to,
					int			expectChars,
					SimpleInputStream *	sis )
    {
    int		nchars;
    int		nbytes;
    int		done= 0;

    nbytes= 0;
    for ( nchars= 0; nchars < expectChars; nchars++ )
	{
	int		step;
	int		c= sioEndianGetLeUint16( sis ); done += 2;

	if  ( c == 0 )
	    { nchars++; break;	}

	step= uniPutUtf8( to, c );
	if  ( step < 1 || step > 3 )
	    { XLDEB(c,step); break;	}

	nbytes += step; to += step;
	}

    *to= '\0';

    while( nchars < expectChars )
	{ sioEndianGetLeInt16( sis ); done += 2; nchars++;	}

    return done;
    }

static int appEmfReadLogFont(		DeviceContext *		dc,
					int			ob,
					SimpleInputStream *	sis,
					int			expectBytes )
    {
    MetaFileObject *	mfo= dc->dcObjects+ ob;
    LogicalFont *	lf= &(mfo->mfoLogicalFont);

    int			done= 0;
    int			step= 0;
    int			togo;

    mfo->mfoType= MFtypeFONT;

    lf->lfHeight= sioEndianGetLeInt32( sis ); done += 4;
    lf->lfWidth= sioEndianGetLeInt32( sis ); done += 4;
    lf->lfEscapement= sioEndianGetLeInt32( sis ); done += 4;
    lf->lfOrientation= sioEndianGetLeInt32( sis ); done += 4;
    lf->lfWeight= sioEndianGetLeInt32( sis ); done += 4;

    lf->lfItalic= sioInGetByte( sis ); done += 1;
    lf->lfUnderline= sioInGetByte( sis ); done += 1;
    lf->lfStrikeOut= sioInGetByte( sis ); done += 1;
    lf->lfCharSet= sioInGetByte( sis ); done += 1;
    lf->lfOutPrecision= sioInGetByte( sis ); done += 1;
    lf->lfClipPrecision= sioInGetByte( sis ); done += 1;
    lf->lfQuality= sioInGetByte( sis ); done += 1;
    lf->lfPitchAndFamily= sioInGetByte( sis ); done += 1;

    togo= 32;
    if  ( togo > ( expectBytes- done )/ 2 )
	{ togo= ( expectBytes- done )/ 2;	}

    memset( lf->lfFaceNameUtf8, 0, sizeof(lf->lfFaceNameUtf8) );
    step= appEmfReadUtf16String( lf->lfFaceNameUtf8, togo, sis );
    if  ( step < 0 )
	{ LDEB(step); return -1;	}
    done += step;

    lf->lfPrivateFont= -1;

    if  ( appWinMetaRememberFontInList( dc, &(lf->lfTextAttribute), lf ) )
	{ SDEB(lf->lfFaceNameUtf8); return -1;	}

    return done;
    }

static int appEmfReadLogFontEx(		DeviceContext *		dc,
					int			ob,
					SimpleInputStream *	sis,
					int			expectBytes )
    {
    int		done= 0;
    int		step;

    char	fullName[3*64+1];
    char	style[3*32+1];
    char	script[3*32+1];

    step= appEmfReadLogFont( dc, ob, sis, expectBytes );
    if  ( step < 0 )
	{ LDEB(step); return -1;	}
    done += step;

    memset( fullName, 0, sizeof(fullName) );
    step= appEmfReadUtf16String( fullName, 64, sis );
    if  ( step < 0 )
	{ LDEB(step); return -1;	}
    done += step;

    memset( style, 0, sizeof(style) );
    step= appEmfReadUtf16String( style, 32, sis );
    if  ( step < 0 )
	{ LDEB(step); return -1;	}
    done += step;

    memset( script, 0, sizeof(script) );
    step= appEmfReadUtf16String( script, 32, sis );
    if  ( step < 0 )
	{ LDEB(step); return -1;	}
    done += step;

    return done;
    }

static int appEmfReadDesignVector(	DeviceContext *		dc,
					SimpleInputStream *	sis )
    {
    int			done= 0;
    unsigned long	signature;
    int			numAxes;
    int			i;

    signature= sioEndianGetLeInt32( sis ); done += 4;
    if  ( signature != 0x08007664 )
	{ XXDEB(signature,0x08007664); return -1;	}

    numAxes= sioEndianGetLeInt32( sis ); done += 4;
    if  ( numAxes < 0 || numAxes > 16 )
	{ LDEB(numAxes); return -1;	}

    for ( i= 0; i < numAxes; i++ )
	{ (void) /* axis= */ sioEndianGetLeInt32( sis ); done += 4;	}

    return done;
    }

static int appEmfReadLogFontExDv(	DeviceContext *		dc,
					int			ob,
					SimpleInputStream *	sis,
					int			expectBytes )
    {
    int		step;
    int		done= 0;

    step= appEmfReadLogFontEx( dc, ob, sis, expectBytes );
    if  ( step < 0 )
	{ LDEB(step); return -1;	}
    done += step;

    expectBytes -= done;

    step= appEmfReadDesignVector( dc, sis );
    if  ( step < 0 )
	{ LDEB(step); return -1;	}
    done += step;

    return done;
    }

static int appEmfReadLogFontPanose(	DeviceContext *		dc,
					int			ob,
					SimpleInputStream *	sis,
					int			expectBytes )
    {
    int		step;
    int		done= 0;

    step= appEmfReadLogFont( dc, ob, sis, expectBytes );
    if  ( step < 0 )
	{ LDEB(step); return -1;	}
    done += step;

    expectBytes -= done;

    if  ( done < expectBytes )
	{
	char	fullName[3*64+1];
	char	style[3*32+1];

	/*
	int	Version;
	int	StyleSize;
	int	Match;
	int	Reserved;
	int	VendirId;
	int	Culture;
	*/

	char	panose[10+1];

	int	i;

	memset( fullName, 0, sizeof(fullName) );
	step= appEmfReadUtf16String( fullName, 64, sis );
	if  ( step < 0 )
	    { LDEB(step); return -1;	}
	done += step;

	memset( style, 0, sizeof(style) );
	step= appEmfReadUtf16String( style, 32, sis );
	if  ( step < 0 )
	    { LDEB(step); return -1;	}
	done += step;

	(void) /* Version= */ sioEndianGetLeInt32( sis ); done += 4;
	(void) /* StyleSize= */ sioEndianGetLeInt32( sis ); done += 4;
	(void) /* Match= */ sioEndianGetLeInt32( sis ); done += 4;
	(void) /* Reserved= */ sioEndianGetLeInt32( sis ); done += 4;
	(void) /* VendirId= */ sioEndianGetLeInt32( sis ); done += 4;
	(void) /* Culture= */ sioEndianGetLeInt32( sis ); done += 4;

	memset( panose, 0, sizeof(panose) );
	for ( i= 0; i < 10; i++ )
	    { panose[i]= sioInGetByte( sis ); done += 1;	}
	(void)sioInGetByte( sis ); done += 1;
	(void)sioInGetByte( sis ); done += 1;
	}

    return done;
    }

static int appEmfSelectStockObject(	DeviceContext *	dc,
					void *		through,
					unsigned long	arg )
    {
    static LogicalBrush	lb;
    static LogicalPen	lp;
    static LogicalFont	lf;

    switch( arg )
	{
	case WHITE_BRUSH:
	    WMFDEB(appDebug("SelectStockObject(%lx):WHITE_BRUSH\n", arg ));
	    lb.lbStyle= BS_SOLID;
	    lb.lbTilePixmap= (struct DrawingSurface *)0;
	    lb.lbTileNumber= 0;
	    lp.lpColor.rgb8Alpha= 255;
	    lp.lpColor.rgb8Red= 255;
	    lp.lpColor.rgb8Green= 255;
	    lp.lpColor.rgb8Blue= 255;
	    if  ( (*dc->dcSelectBrushObject)( dc, through, &lb ) )
		{ XDEB(arg); return -1;		}
	    return 0;

	case LTGRAY_BRUSH:
	    WMFDEB(appDebug("SelectStockObject(%lx):LTGRAY_BRUSH\n", arg ));
	    lb.lbStyle= BS_SOLID;
	    lb.lbTilePixmap= (struct DrawingSurface *)0;
	    lb.lbTileNumber= -1;
	    lp.lpColor.rgb8Alpha= 255;
	    lp.lpColor.rgb8Red= 192;
	    lp.lpColor.rgb8Green= 192;
	    lp.lpColor.rgb8Blue= 192;
	    if  ( (*dc->dcSelectBrushObject)( dc, through, &lb ) )
		{ XDEB(arg); return -1;		}
	    return 0;

	case GRAY_BRUSH:
	    WMFDEB(appDebug("SelectStockObject(%lx):GRAY_BRUSH\n", arg ));
	    lb.lbStyle= BS_SOLID;
	    lb.lbTilePixmap= (struct DrawingSurface *)0;
	    lb.lbTileNumber= -1;
	    lp.lpColor.rgb8Alpha= 255;
	    lp.lpColor.rgb8Red= 128;
	    lp.lpColor.rgb8Green= 128;
	    lp.lpColor.rgb8Blue= 128;
	    if  ( (*dc->dcSelectBrushObject)( dc, through, &lb ) )
		{ XDEB(arg); return -1;		}
	    return 0;

	case DKGRAY_BRUSH:
	    WMFDEB(appDebug("SelectStockObject(%lx):DKGRAY_BRUSH\n", arg ));
	    lb.lbStyle= BS_SOLID;
	    lb.lbTilePixmap= (struct DrawingSurface *)0;
	    lb.lbTileNumber= -1;
	    lp.lpColor.rgb8Alpha= 255;
	    lp.lpColor.rgb8Red= 64;
	    lp.lpColor.rgb8Green= 64;
	    lp.lpColor.rgb8Blue= 64;
	    if  ( (*dc->dcSelectBrushObject)( dc, through, &lb ) )
		{ XDEB(arg); return -1;		}
	    return 0;

	case BLACK_BRUSH:
	    WMFDEB(appDebug("SelectStockObject(%lx):BLACK_BRUSH\n", arg ));
	    lb.lbStyle= BS_SOLID;
	    lb.lbTilePixmap= (struct DrawingSurface *)0;
	    lb.lbTileNumber= -1;
	    lp.lpColor.rgb8Alpha= 255;
	    lp.lpColor.rgb8Red= 0;
	    lp.lpColor.rgb8Green= 0;
	    lp.lpColor.rgb8Blue= 0;
	    if  ( (*dc->dcSelectBrushObject)( dc, through, &lb ) )
		{ XDEB(arg); return -1;		}
	    return 0;

	case NULL_BRUSH:
	    WMFDEB(appDebug("SelectStockObject(%lx):NULL_BRUSH\n", arg ));
	    lb.lbStyle= BS_NULL;
	    lb.lbTilePixmap= (struct DrawingSurface *)0;
	    lb.lbTileNumber= -1;
	    lp.lpColor.rgb8Alpha= 0;
	    lp.lpColor.rgb8Red= 255;
	    lp.lpColor.rgb8Green= 255;
	    lp.lpColor.rgb8Blue= 255;
	    if  ( (*dc->dcSelectBrushObject)( dc, through, &lb ) )
		{ XDEB(arg); return -1;		}
	    return 0;

	case WHITE_PEN:
	    WMFDEB(appDebug("SelectStockObject(%lx):WHITE_PEN\n", arg ));
	    lp.lpStyle= PS_SOLID;
	    lp.lpWidth= 1;
	    lp.lpColor.rgb8Alpha= 255;
	    lp.lpColor.rgb8Red= 255;
	    lp.lpColor.rgb8Green= 255;
	    lp.lpColor.rgb8Blue= 255;
	    if  ( (*dc->dcSelectPenObject)( dc, through, &lp ) )
		{ XDEB(arg); return -1;		}
	    return 0;

	case BLACK_PEN:
	    WMFDEB(appDebug("SelectStockObject(%lx):BLACK_PEN\n", arg ));
	    lp.lpStyle= PS_SOLID;
	    lp.lpWidth= 1;
	    lp.lpColor.rgb8Alpha= 255;
	    lp.lpColor.rgb8Red= 0;
	    lp.lpColor.rgb8Green= 0;
	    lp.lpColor.rgb8Blue= 0;
	    if  ( (*dc->dcSelectPenObject)( dc, through, &lp ) )
		{ XDEB(arg); return -1;		}
	    return 0;

	case NULL_PEN:
	    WMFDEB(appDebug("SelectStockObject(%lx):NULL_PEN\n", arg ));
	    lp.lpStyle= PS_NULL;
	    lp.lpWidth= 0;
	    lp.lpColor.rgb8Alpha= 0;
	    lp.lpColor.rgb8Red= 255;
	    lp.lpColor.rgb8Green= 255;
	    lp.lpColor.rgb8Blue= 255;
	    if  ( (*dc->dcSelectPenObject)( dc, through, &lp ) )
		{ XDEB(arg); return -1;		}
	    return 0;

	case OEM_FIXED_FONT:
	case ANSI_FIXED_FONT:
	case SYSTEM_FONT:
	case SYSTEM_FIXED_FONT:
	    WMFDEB(appDebug("SelectStockObject(%lx): COURIER_FONT\n", arg ));
	    lf.lfHeight= 240;
	    lf.lfWidth= 200;
	    lf.lfWeight= 500;
	    lf.lfPrivateFont= -1;
	    strcpy( lf.lfFaceNameUtf8, "Courier" );
	    if  ( docGetFontByName( &(dc->dcFontList), lf.lfFaceNameUtf8 ) < 0 )
		{ LDEB(1);	}
	    if  ( (*dc->dcSelectFontObject)( dc, through, &lf ) )
		{ XDEB(arg); return -1;		}
	    dc->dcFont= lf;
	    return 0;

	case DEVICE_DEFAULT_FONT:
	case ANSI_VAR_FONT:
	case DEFAULT_GUI_FONT:
	    WMFDEB(appDebug("SelectStockObject(%lx): HELVETICA_FONT\n", arg ));
	    lf.lfHeight= 240;
	    lf.lfWidth= 200;
	    lf.lfWeight= 500;
	    lf.lfPrivateFont= -1;
	    strcpy( lf.lfFaceNameUtf8, "Helvetica" );
	    if  ( docGetFontByName( &(dc->dcFontList), lf.lfFaceNameUtf8 ) < 0 )
		{ LDEB(1);	}
	    if  ( (*dc->dcSelectFontObject)( dc, through, &lf ) )
		{ XDEB(arg); return -1;		}
	    dc->dcFont= lf;
	    return 0;


	case DEFAULT_PALETTE:

	case DC_BRUSH:
	case DC_PEN:
	default:
	    WMFDEB(appDebug("SelectStockObject(%lx): ?-?\n", arg ));
	    XDEB(arg); return -1;
	}
    }

static int appEmfReadTextObject(	DeviceContext *		dc,
					int			skipped,
					SimpleInputStream *	sis,
					long *			pX,
					long *			pY )
    {
    int			step;
    int			done= 0;

    int			refX;
    int			refY;
    int			options;

    int			charCount;
    int			offString;
    int			offDx;

    DocumentRectangle	rect;

    int			i;

    refX= sioEndianGetLeInt32( sis ); done += 4;
    refY= sioEndianGetLeInt32( sis ); done += 4;

    charCount= sioEndianGetLeInt32( sis ); done += 4;
    offString= sioEndianGetLeInt32( sis ); done += 4;
    options= sioEndianGetLeInt32( sis ); done += 4;

    step= bmEmfReadRectangle( &rect, sis );
    if  ( step < 0 )
	{ LDEB(step); return -1;	}
    done += step;

    offDx= sioEndianGetLeInt32( sis ); done += 4;

    while( skipped+ done < offString )
	{ (void)sioInGetByte( sis ); done++; 	}

    step= appWinMetaReadUtf16Text( dc, charCount, sis );
    if  ( step < 0 )
	{ LDEB(step); return -1;	}
    while( step % 4 )
	{ (void)sioInGetByte( sis ); step++;	}
    done += step;

    while( skipped+ done < offDx )
	{ (void)sioInGetByte( sis ); done++; 	}

    if  ( offDx > 0 )
	{
	for ( i= 0; i < charCount; i++ )
	    {
	    sioEndianGetLeInt32( sis ); done += 4;
	    if  ( options & ETO_PDY )
		{
		sioEndianGetLeInt32( sis ); done += 4;
		}
	    }
	}

    *pX= refX;
    *pY= refY;

    return done;
    }

static int appWmfReadSmallTextObject(	DeviceContext *		dc,
					int			skipped,
					SimpleInputStream *	sis,
					long *			pX,
					long *			pY,
					int *			pByteCount )
    {
    int			step;
    int			done= 0;

    int			refX;
    int			refY;
    int			options;

    int			charCount;

    DocumentRectangle	rect;

    refX= sioEndianGetLeInt32( sis ); done += 4;
    refY= sioEndianGetLeInt32( sis ); done += 4;

    charCount= sioEndianGetLeInt32( sis ); done += 4;
    options= sioEndianGetLeInt32( sis ); done += 4;
    (void) /* iGraphicsMode= */ sioEndianGetLeInt32( sis ); done += 4;
    (void) /* exScale= */ sioEndianGetLeFloat( sis ); done += 4;
    (void) /* eyScale= */ sioEndianGetLeFloat( sis ); done += 4;

    if  ( ! ( options & ETO_NO_RECT ) )
	{
	step= bmEmfReadRectangle( &rect, sis );
	if  ( step < 0 )
	    { LDEB(step); return -1;	}
	done += step;
	}

    if  ( options & ETO_SMALL_CHARS )
	{
	step= appWinMetaReadLegacyText( dc, charCount, charCount, sis );
	if  ( step < 0 )
	    { LDEB(step); return -1;	}
	while( step % 4 )
	    { (void)sioInGetByte( sis ); step++;	}
	done += step;
	}
    else{
	step= appWinMetaReadUtf16Text( dc, charCount, sis );
	if  ( step < 0 )
	    { LDEB(step); return -1;	}
	while( step % 4 )
	    { (void)sioInGetByte( sis ); step++;	}
	done += step;
	}

    *pX= refX;
    *pY= refY;

    return done;
    }

static int appEmfStretchBlt(	DeviceContext *		dc,
				SimpleInputStream *	sis,
				void *			through,
				int			skipped,
				int			expectBytes )
    {
    int				step;
    int				done= 0;

    AffineTransform2D		transform;

    bmi				bmi;

    DocumentRectangle		drDest;
    DocumentRectangle		drSrc;
    int				w;
    int				h;

    step= bmEmfReadXYWHRectangle( &drDest, sis );
    if  ( step < 0 )
	{ LDEB(step); return -1;	}
    done += step;

    (void) /* BitBltRasterOperation= */ sioEndianGetLeInt32( sis ); done += 4;

    drSrc.drX0= sioEndianGetLeInt32( sis ); done += 4;
    drSrc.drY0= sioEndianGetLeInt32( sis ); done += 4;

    step= bmEmfReadAffineTransform2D( &transform, sis );
    if  ( step < 0 )
	{ LDEB(step); return -1;	}
    done += step;

    (void) /* BkColorSrc= */ sioEndianGetLeInt32( sis ); done += 4;

    /* What is this? */
    (void) /* skip= */ sioEndianGetLeInt32( sis ); done += 4;

    done += appEmfReadBmi( &bmi, sis );

    w= sioEndianGetLeInt32( sis ); done += 4;
    h= sioEndianGetLeInt32( sis ); done += 4;
    drSrc.drX1= drSrc.drX0+ w- 1;
    drSrc.drY1= drSrc.drY0+ h- 1;

    expectBytes= expectBytes- done;

    while( skipped+ done < bmi.offBmi )
	{
	(void) /* skip= */ sioEndianGetLeInt32( sis ); done += 4;
	}

    if  ( appMetaDrawRasterImage( sis, through, expectBytes, dc,
							&drSrc, &drDest ) )
	{ LDEB(expectBytes); return -1; }

    done += expectBytes;

    return done;
    }

/************************************************************************/

static void appWinMetaUpdateTransformEmf(	DeviceContext *	dc )
    {
    DeviceCoordinateTransform *	t= &(dc->dcTransform);
    AffineTransform2D		logicalToDevice;

    if  ( geoAffineTransformForTriangles( &logicalToDevice,
		    t->dctLogicalRect.drX0,	t->dctLogicalRect.drY0,
		    t->dctLogicalRect.drX1,	t->dctLogicalRect.drY0,
		    t->dctLogicalRect.drX0,	t->dctLogicalRect.drY1,

		    t->dctDeviceRect.drX0,	t->dctDeviceRect.drY0,
		    t->dctDeviceRect.drX1,	t->dctDeviceRect.drY0,
		    t->dctDeviceRect.drX0,	t->dctDeviceRect.drY1	) )
	{ LDEB(1);	}

    geoAffineTransform2DProduct( &(t->dctLogicalToOutput),
				&(t->dctDeviceToOutput), &logicalToDevice );

    return;
    }

/************************************************************************/
/*									*/
/*  Get an Arc/Pie/Chord.						*/
/*									*/
/************************************************************************/

static int appEmfGetArc(	DocumentRectangle *	bounds,
				Point2DI *		pS,
				Point2DI *		pE,
				DeviceContext *		dc,
				SimpleInputStream *	sis )
    {
    int		done= 0;

    Point2DI	s;
    Point2DI	e;

    done= bmEmfReadRectangle( bounds, sis );
    if  ( done < 0 )
	{ LDEB(done); return -1;	}

    s.y= sioEndianGetLeInt32( sis ); done += 4;
    s.x= sioEndianGetLeInt32( sis ); done += 4;
    e.y= sioEndianGetLeInt32( sis ); done += 4;
    e.x= sioEndianGetLeInt32( sis ); done += 4;

    appWinMetaTransformRectangle( bounds, dc, bounds );

    pS->y= DC_yViewport( s.x, s.y, dc );
    pS->x= DC_xViewport( s.x, s.y, dc );
    pE->y= DC_yViewport( e.x, e.y, dc );
    pE->x= DC_xViewport( e.x, e.y, dc );

    return done;
    }

/************************************************************************/
/*									*/
/*  Play a windows extended metafile and invoke callbacks.		*/
/*									*/
/************************************************************************/

int appMetaPlayEmf(	DeviceContext *			dc,
			void *				through )
    {
    SimpleInputStream *	sis= dc->dcPlayer->mpInputStream;

    WMFDEB(appDebug( "PLAY EMF METAFILE ...\n" ));

    for (;;)
	{
	long			x0;
	long			y0;
	long			x1;
	long			y1;
	long			w;
	long			h;
	unsigned long		arg;

	Point2DI		s;
	Point2DI		e;

	int			recordType= sioEndianGetLeInt32( sis );
	int			recordSize= sioEndianGetLeInt32( sis );
	int			done= 8;
	int			step;
	int			count;
	int			expectBytes;

	DocumentRectangle	bounds;
	AffineTransform2D	transform;

	if  ( recordSize < 1 || recordType < 1 )
	    { XLDEB(recordType,recordSize); return -1;	}

	switch( recordType )
	    {
	    case EMR_COMMENT:
		WMFDEB(appDebug("Comment()\n"));
		goto skipArguments;

	    case EMR_SETMAPMODE:
		arg= sioEndianGetLeUint32( sis ); done += 4;

		WMFDEB(appDebug("SetMapMode(mapMode=%ld)\n", arg ));
		dc->dcMapMode= arg;
		appWinMetaUpdateTransformEmf( dc );
		goto checkSize;

	    case EMR_SETVIEWPORTEXTEX:
		w= sioEndianGetLeInt32( sis ); done += 4;
		h= sioEndianGetLeInt32( sis ); done += 4;

		WMFDEB(appDebug(
		    "SetViewportExt(yViewExt=%ld,xViewExt=%ld)\n", h, w ));

		dc->dcTransform.dctDeviceRect.drX1=
				    dc->dcTransform.dctDeviceRect.drX0+ w;
		dc->dcTransform.dctDeviceRect.drY1=
				    dc->dcTransform.dctDeviceRect.drY0+ h;
		appWinMetaUpdateTransformEmf( dc );

		goto checkSize;

	    case EMR_SETWINDOWEXTEX:
		w= sioEndianGetLeInt32( sis ); done += 4;
		h= sioEndianGetLeInt32( sis ); done += 4;

		WMFDEB(appDebug("SetWindowExt(yWinExt=%ld,xWinExt=%ld)\n",
								    h, w ));
		dc->dcTransform.dctLogicalRect.drX1=
				    dc->dcTransform.dctLogicalRect.drX0+ w;
		dc->dcTransform.dctLogicalRect.drY1=
				    dc->dcTransform.dctLogicalRect.drY0+ h;
		appWinMetaUpdateTransformEmf( dc );

		goto checkSize;

	    case EMR_SETWINDOWORGEX:
		x0= sioEndianGetLeInt32( sis ); done += 4;
		y0= sioEndianGetLeInt32( sis ); done += 4;

		WMFDEB(appDebug("SetWindowOrg(yWinOrg=%ld,xWinOrg=%ld)\n",
								    y0,x0 ));

		w= x0- dc->dcTransform.dctLogicalRect.drX0;
		h= y0- dc->dcTransform.dctLogicalRect.drY0;
		dc->dcTransform.dctLogicalRect.drX0 += w;
		dc->dcTransform.dctLogicalRect.drY0 += h;
		dc->dcTransform.dctLogicalRect.drX1 += w;
		dc->dcTransform.dctLogicalRect.drY1 += h;
		appWinMetaUpdateTransformEmf( dc );

		goto checkSize;

	    case EMR_SETVIEWPORTORGEX:
		x0= sioEndianGetLeInt32( sis ); done += 4;
		y0= sioEndianGetLeInt32( sis ); done += 4;

		WMFDEB(appDebug("SetViewportOrg(yViewOrg=%ld,xViewOrg=%ld)\n",
								    y0,x0 ));

		w= x0- dc->dcTransform.dctDeviceRect.drX0;
		h= y0- dc->dcTransform.dctDeviceRect.drY0;
		dc->dcTransform.dctDeviceRect.drX0 += w;
		dc->dcTransform.dctDeviceRect.drY0 += h;
		dc->dcTransform.dctDeviceRect.drX1 += w;
		dc->dcTransform.dctDeviceRect.drY1 += h;
		appWinMetaUpdateTransformEmf( dc );
		goto checkSize;

	    case EMR_SAVEDC:
		WMFDEB(appDebug("SaveDC()\n"));

		if  ( appMetaSaveDC( dc ) )
		    { LDEB(1); return -1;	}
		goto checkSize;

	    case EMR_RESTOREDC:
		arg= sioEndianGetLeInt32( sis ); done += 4;

		WMFDEB(appDebug("RestoreDC(%ld)\n", arg ));

		if  ( appMetaRestoreDC( dc ) )
		    { LDEB(1); 	}

		goto checkSize;

	    case EMR_SELECTOBJECT:
		arg= sioEndianGetLeUint32( sis ); done += 4;

		if  ( arg & 0x80000000 )
		    {
		    if  ( appEmfSelectStockObject( dc, through, arg ) )
			{ LXDEB(recordSize,arg); return -1;	}
		    }
		else{
		    WMFDEB(appDebug("SelectObject(%ld)\n", arg ));

		    if  ( arg >= dc->dcObjectCount )
			{ LLDEB(arg,dc->dcObjectCount); return -1;	}

		    if  ( appWinMetaSelectObject( dc, through, arg ) )
			{ LLDEB(recordSize,arg); return -1;	}
		    }
		goto checkSize;

	    case EMR_DELETEOBJECT:
		arg= sioEndianGetLeUint32( sis ); done += 4;

		if  ( arg & 0x80000000 )
		    {
		    WMFDEB(appDebug("DeleteStockObject(%lx)\n", arg ));
		    }
		else{
		    WMFDEB(appDebug("DeleteObject(%ld)\n", arg ));

		    if  ( arg >= dc->dcObjectCount )
			{ LLDEB(arg,dc->dcObjectCount); return -1;	}

		    (*dc->dcCleanObject)( dc, through, &(dc->dcObjects[arg]) );
		    appMetaCleanObject( &(dc->dcObjects[arg]) );
		    dc->dcObjects[arg].mfoType= MFtypeFREE;
		    }

		goto checkSize;

	    case EMR_SETICMMODE:
		arg= sioEndianGetLeUint32( sis ); done += 4;
		WMFDEB(appDebug("SetIcmMode(%ld) size= %d\n", arg, recordSize ));
		goto checkSize;

	    case EMR_SETARCDIRECTION:
		arg= sioEndianGetLeUint32( sis ); done += 4;
		WMFDEB(appDebug("SetArcDirection(%ld) size= %d\n", arg, recordSize ));
		goto checkSize;

	    case EMR_SELECTCLIPPATH:
		arg= sioEndianGetLeUint32( sis ); done += 4;
		WMFDEB(appDebug("SelectClipPath(%ld) size= %d\n", arg, recordSize ));
		goto checkSize;

	    case EMR_SETMETARGN:
		WMFDEB(appDebug("SetMetaArgN()\n"));
		goto checkSize;

	    case EMR_SELECTPALETTE:
		arg= sioEndianGetLeUint16( sis ); done += 2;
		x0= sioEndianGetLeInt16( sis ); done += 2;
		WMFDEB(appDebug("SelectPalette(%ld) size= %d\n", arg, recordSize ));
		goto checkSize;

	    case EMR_SETLAYOUT:
		arg= sioEndianGetLeUint32( sis ); done += 4;
		WMFDEB(appDebug("SetLayout(%ld)\n", arg ));
		goto checkSize;

	    case EMR_SETCOLORSPACE:
		arg= sioEndianGetLeUint32( sis ); done += 4;
		WMFDEB(appDebug("SetColorSpace(%ld)\n", arg ));
		goto checkSize;

	    case EMR_SETBKMODE:
		dc->dcBkMode= sioEndianGetLeInt32( sis ); done += 4;
		WMFDEB(appDebug("SetBkMode(%d)\n", dc->dcBkMode ));
		goto checkSize;

	    case EMR_SETPOLYFILLMODE:
		arg= sioEndianGetLeUint32( sis ); done += 4;

		WMFDEB(appDebug("SetPolyFillMode(%ld)\n", arg ));

		if  ( (*dc->dcSetPolyFillMode)( dc, through, arg ) )
		    { LDEB(arg); return -1;	}

		dc->dcPolyFillMode= arg;

		goto checkSize;

	    case EMR_SETROP2:
		arg= sioEndianGetLeUint32( sis ); done += 4;
		WMFDEB(appDebug("SetRop2(%ld)\n", arg ));
		goto checkSize;

	    case EMR_SETMITERLIMIT:
		arg= sioEndianGetLeUint32( sis ); done += 4;
		WMFDEB(appDebug("SetMiterLimit(%ld)\n", arg ));
		goto checkSize;

	    case EMR_SETSTRETCHBLTMODE:
		dc->dcStretchBltMode= sioEndianGetLeInt32( sis ); done += 4;

		WMFDEB(appDebug("SetStretchBltMode(%ld)\n",
						    dc->dcStretchBltMode ));
		goto checkSize;

	    case EMR_SETTEXTALIGN:
		arg= sioEndianGetLeUint32( sis ); done += 4;

		WMFDEB(appDebug("SetTextAlign( 0x%x )\n", (unsigned)arg ));

		if  ( appMetaSetTextAlign( dc, arg ) )
		    { LLDEB(recordSize,arg); return -1;	}
		goto checkSize;

	    case EMR_SETTEXTJUSTIFICATION:
		dc->dcJustificationAmount= sioEndianGetLeInt32( sis ); done += 4;
		dc->dcJustificationSpaces= sioEndianGetLeInt32( sis ); done += 4;

		WMFDEB(appDebug("SetTextJustification(...)\n"));

		goto checkSize;

	    case EMR_EXTSELECTCLIPRGN:
		arg= sioEndianGetLeUint32( sis ); done += 4;
		arg= sioEndianGetLeUint32( sis ); done += 4;
		WMFDEB(appDebug("ExtSelectClipRgn()\n" ));
		/* variable data */
		goto skipArguments;

	    case EMR_CREATEPEN:
		arg= sioEndianGetLeUint32( sis ); done += 4;
		WMFDEB(appDebug("CreatePen(%ld)\n", arg ));
		step= appEmfReadPen( dc, arg, sis );
		if  ( step < 0 )
		    { LDEB(step); return -1;	}
		done += step;
		goto checkSize;

	    case EMR_EXTCREATEPEN:
		arg= sioEndianGetLeUint32( sis ); done += 4;
		WMFDEB(appDebug("ExtCreatePen(%ld)\n", arg ));
		step= appEmfExtCreatePen( dc, arg, done, sis );
		if  ( step < 0 )
		    { LDEB(step); return -1;	}
		done += step;
		goto checkSize;

	    case EMR_CREATEBRUSHINDIRECT:
		arg= sioEndianGetLeUint32( sis ); done += 4;
		step= appEmfReadBrush( dc, arg, sis );
		if  ( step < 0 )
		    { LDEB(step); return -1;	}
		done += step;
		goto checkSize;

	    case EMR_SETLINKEDUFIS:
		count= sioEndianGetLeInt32( sis ); done += 4;
		WMFDEB(appDebug("SetLinkedUfis( count=%d, ... )\n", count ));
		for ( x0= 0; x0 < count; x0++ )
		    {
		    arg= sioEndianGetLeUint32( sis ); done += 4;
		    }
		arg= sioEndianGetLeUint32( sis ); done += 4;
		arg= sioEndianGetLeUint32( sis ); done += 4;
		goto checkSize;

	    case EMR_INTERSECTCLIPRECT:
		step= bmEmfReadRectangle( &bounds, sis );
		if  ( step < 0 )
		    { LDEB(step); return -1;	}
		done += step;

		WMFDEB(appDebug("IntersectClipRect()\n"));
		goto checkSize;

	    case EMR_EXCLUDECLIPRECT:
		step= bmEmfReadRectangle( &bounds, sis );
		if  ( step < 0 )
		    { LDEB(step); return -1;	}
		done += step;

		WMFDEB(appDebug("ExcludeClipRect()\n"));
		goto checkSize;

	    case EMR_RECTANGLE:
		step= bmEmfReadRectangle( &bounds, sis );
		if  ( step < 0 )
		    { LDEB(step); return -1;	}
		done += step;

		WMFDEB(appDebug("Rectangle(%d..%d,%d..%d)\n",
						bounds.drX0,bounds.drX1,
						bounds.drY0,bounds.drY1));

		appWinMetaDrawRectangle( dc, through, &bounds,
					dc->dcFillInsides, dc->dcDrawBorders );

		goto checkSize;

	    case EMR_ELLIPSE:
		step= bmEmfReadRectangle( &bounds, sis );
		if  ( step < 0 )
		    { LDEB(step); return -1;	}
		done += step;

		WMFDEB(appDebug("Ellipse(%d..%d,%d..%d)\n",
						bounds.drX0,bounds.drX1,
						bounds.drY0,bounds.drY1));

		appWinMetaDrawEllipse( dc, through, &bounds,
					dc->dcFillInsides, dc->dcDrawBorders );

		goto checkSize;

	    case EMR_POLYBEZIER:
		{
		Point2DI *	points;

		step= appEmfReadPolyPoints32( sis, &bounds,
						    &count, &points, dc );
		if  ( step < 0 )
		    { LDEB(step); return -1;	}
		done += step;

		WMFDEB(appDebug("PolyBezier( count=%d, ... )\n", count ));

		if  ( (*dc->dcDrawPolyBezier)( dc, through,
							count, points, 1,
							dc->dcFillInsides,
							dc->dcDrawBorders,
							dc->dcFillInsides ) )
		    { LDEB(count); return -1;	}
		}
		goto checkSize;

	    case EMR_POLYBEZIERTO16:
		{
		Point2DI *	points;

		step= appEmfReadPolyPoints16( sis, &bounds,
						    &count, &points, dc );
		if  ( step < 0 )
		    { LDEB(step); return -1;	}
		done += step;

		WMFDEB(appDebug("PolyBezierTo16( count=%d, ... )\n", count ));

		if  ( (*dc->dcDrawPolyBezier)( dc, through, count, points, 0,
								    0, 1, 0 ) )
		    { LDEB(count); return -1;	}
		}
		goto checkSize;

	    case EMR_POLYLINE:
		{
		Point2DI *	points;

		step= appEmfReadPolyPoints32( sis, &bounds,
						    &count, &points, dc );
		if  ( step < 0 )
		    { LDEB(step); return -1;	}
		done += step;

		WMFDEB(appDebug("PolyLine( count=%d, ... )\n", count ));

		if  ( (*dc->dcDrawPolyPolygon)( dc, through, 1, &count, points,
								    0, 1, 0 ) )
		    { LDEB(count); return -1;	}
		}
		goto checkSize;

	    case EMR_POLYGON:
		{
		Point2DI *	points;

		step= appEmfReadPolyPoints32( sis, &bounds,
						    &count, &points, dc );
		if  ( step < 0 )
		    { LDEB(step); return -1;	}
		done += step;

		WMFDEB(appDebug("Polygon( count=%d, ... )\n", count ));

		if  ( (*dc->dcDrawPolyPolygon)( dc, through, 1, &count, points,
				    dc->dcFillInsides, dc->dcDrawBorders, 1 ) )
		    { LDEB(count); return -1;	}
		}
		goto checkSize;

	    case EMR_POLYLINETO:
		{
		Point2DI *	points;

		step= appEmfReadPolyPoints32( sis, &bounds,
						    &count, &points, dc );
		if  ( step < 0 )
		    { LDEB(step); return -1;	}
		done += step;

		WMFDEB(appDebug("PolyLineTo( count=%d, ... )\n", count ));

		if  ( (*dc->dcDrawPolyPolygon)( dc, through, 1, &count, points,
								    0, 1, 0 ) )
		    { LDEB(count); return -1;	}
		}
		goto checkSize;

	    case EMR_POLYLINETO16:
		{
		Point2DI *	points;

		step= appEmfReadPolyPoints16( sis, &bounds,
						    &count, &points, dc );
		if  ( step < 0 )
		    { LDEB(step); return -1;	}
		done += step;

		WMFDEB(appDebug("PolyLineTo16( count=%d, ... )\n", count ));

		if  ( (*dc->dcDrawPolyPolygon)( dc, through, 1, &count, points,
								    0, 1, 0 ) )
		    { LDEB(count); return -1;	}
		}
		goto checkSize;

	    case EMR_POLYBEZIERTO:
		{
		Point2DI *	points;

		step= appEmfReadPolyPoints32( sis, &bounds,
						    &count, &points, dc );
		if  ( step < 0 )
		    { LDEB(step); return -1;	}
		done += step;

		WMFDEB(appDebug("PolyBezierTo( count=%d, ... )\n", count ));

		if  ( (*dc->dcDrawPolyBezier)( dc, through,
							count, points, 0,
							dc->dcFillInsides,
							dc->dcDrawBorders,
							dc->dcFillInsides ) )
		    { LDEB(count); return -1;	}
		}
		goto checkSize;

	    case EMR_POLYLINE16:
		{
		Point2DI *	points;

		step= appEmfReadPolyPoints16( sis, &bounds,
						    &count, &points, dc );
		if  ( step < 0 )
		    { LDEB(step); return -1;	}
		done += step;

		WMFDEB(appDebug(
		    "PolyLine16( count=%d, bounds=[%d..%d x %d..%d] )\n",
					count, bounds.drX0, bounds.drX1,
					bounds.drY0, bounds.drY1 ));

		if  ( (*dc->dcDrawPolyPolygon)( dc, through, 1, &count, points,
								    0, 1, 0 ) )
		    { LDEB(count); return -1;	}
		}
		goto checkSize;

	    case EMR_POLYBEZIER16:
		{
		Point2DI *	points;

		step= appEmfReadPolyPoints16( sis, &bounds,
						    &count, &points, dc );
		if  ( step < 0 )
		    { LDEB(step); return -1;	}
		done += step;

		WMFDEB(appDebug("PolyBezier16( count=%d, ... )\n", count ));

		if  ( (*dc->dcDrawPolyBezier)( dc, through,
							count, points, 0,
							dc->dcFillInsides,
							dc->dcDrawBorders,
							dc->dcFillInsides ) )
		    { LDEB(count); return -1;	}
		}
		goto checkSize;

	    case EMR_POLYGON16:
		{
		Point2DI *	points;

		step= appEmfReadPolyPoints16( sis, &bounds,
						    &count, &points, dc );
		if  ( step < 0 )
		    { LDEB(step); return -1;	}
		done += step;

		WMFDEB(appDebug("Polygon16( count=%d, ... )\n", count ));

		if  ( (*dc->dcDrawPolyPolygon)( dc, through, 1, &count, points,
				    dc->dcFillInsides, dc->dcDrawBorders, 1 ) )
		    { LDEB(count); return -1;	}
		}
		goto checkSize;

	    case EMR_POLYPOLYGON:
		WMFDEB(appDebug("PolyPolygon( ... )\n" ));

		step= appEmfDrawPolyPolygonXX( sis, "PolyPolygon",
			through, recordSize,
			&bounds, dc, appWinMetaGetPoints32,
			dc->dcFillInsides, dc->dcDrawBorders, 1 );
		if  ( step < 0 )
		    { LDEB(step); return -1;	}
		done += step;

		goto checkSize;


	    case EMR_POLYPOLYGON16:
		WMFDEB(appDebug("PolyPolygon16( ... )\n" ));

		step= appEmfDrawPolyPolygonXX( sis, "PolyPolygon16",
			through, recordSize,
			&bounds, dc, appWinMetaGetPoints16,
			dc->dcFillInsides, dc->dcDrawBorders, 1 );
		if  ( step < 0 )
		    { LDEB(step); return -1;	}
		done += step;

		goto checkSize;

	    case EMR_POLYPOLYLINE:
		WMFDEB(appDebug("PolyPolyLine( ... )\n" ));

		step= appEmfDrawPolyPolygonXX( sis, "PolyPolyLine",
			through, recordSize,
			&bounds, dc, appWinMetaGetPoints32,
			0, 1, 0 );
		if  ( step < 0 )
		    { LDEB(step); return -1;	}
		done += step;

		goto checkSize;

	    case EMR_POLYPOLYLINE16:
		WMFDEB(appDebug("PolyPolyLine16( ... )\n" ));

		step= appEmfDrawPolyPolygonXX( sis, "PolyPolyLine16",
			through, recordSize,
			&bounds, dc, appWinMetaGetPoints16,
			0, 1, 0 );
		if  ( step < 0 )
		    { LDEB(step); return -1;	}
		done += step;

		goto checkSize;

	    case EMR_MOVETOEX:
		x0= sioEndianGetLeInt32( sis ); done += 4;
		y0= sioEndianGetLeInt32( sis ); done += 4;

		WMFDEB(appDebug("MoveTo( x=%ld, y=%ld )\n", x0, y0 ));

		dc->dcX= x0;
		dc->dcY= y0;

		goto checkSize;

	    case EMR_LINETO:
		x0= dc->dcX;
		y0= dc->dcY;
		x1= sioEndianGetLeInt32( sis ); done += 4;
		y1= sioEndianGetLeInt32( sis ); done += 4;

		WMFDEB(appDebug("LineTo( x:%ld-> %ld, y:%ld-> %ld )\n",
							x0, x1, y0, y1 ));

		dc->dcX= x1;
		dc->dcY= y1;

		{
		Point2DI	points[2];

		count= 2;
		points[0].x= DC_xViewport( x0, y0, dc );
		points[0].y= DC_yViewport( x0, y0, dc );
		points[1].x= DC_xViewport( x1, y1, dc );
		points[1].y= DC_yViewport( x1, y1, dc );

		if  ( (*dc->dcDrawPolyPolygon)( dc, through,
						1, &count, points, 0, 1, 0 ) )
		    { LDEB(count); return -1;	}
		}

		goto checkSize;

	    case EMR_SETBRUSHORGEX:
		x0= sioEndianGetLeInt32( sis ); done += 4;
		y0= sioEndianGetLeInt32( sis ); done += 4;

		WMFDEB(appDebug("SetBrushOrgEx(yBOrg=%ld,xBOrg=%ld)\n",
								    y0,x0 ));
		goto checkSize;

	    case EMR_SETTEXTCOLOR:
		bmEmfReadRgb8Color( &(dc->dcTextColor), sis ); done += 4;

		WMFDEB(appDebug("SetTextColor( r=%d, g=%d, b= %d )\n",
					dc->dcTextColor.rgb8Red,
					dc->dcTextColor.rgb8Green,
					dc->dcTextColor.rgb8Blue ));

		if  ( (*dc->dcSetTextColor)( dc, through,
					dc->dcTextColor.rgb8Red,
					dc->dcTextColor.rgb8Green,
					dc->dcTextColor.rgb8Blue ) )
		    { LDEB(1);  return -1;	}

		goto checkSize;

	    case EMR_SETBKCOLOR:
		bmEmfReadRgb8Color( &(dc->dcBkColor), sis ); done += 4;

		WMFDEB(appDebug("SetBkColor( r=%d, g=%d, b= %d )\n",
					    dc->dcBkColor.rgb8Red,
					    dc->dcBkColor.rgb8Green,
					    dc->dcBkColor.rgb8Blue ));

		if  ( (*dc->dcSetBkColor)( dc, through,
					    dc->dcBkColor.rgb8Red,
					    dc->dcBkColor.rgb8Green,
					    dc->dcBkColor.rgb8Blue ) )
		    { LDEB(1);  return -1;	}

		goto checkSize;

	    case EMR_SETWORLDTRANSFORM:
		step= bmEmfReadAffineTransform2D( &transform, sis );
		if  ( step < 0 )
		    { LDEB(step); return -1;	}
		done += step;

		WMFDEB(appDebug("SetWorldTransform([%g,%g,%g,%g + %g,%g])\n",
			    transform.at2Axx, transform.at2Ayx,
			    transform.at2Axy, transform.at2Ayy,
			    transform.at2Tx,  transform.at2Ty ));

		geoAffineTransform2DProduct(
				&(dc->dcTransform.dctLogicalToOutput),
				&(dc->dcTransform.dctDeviceToOutput),
				&transform );

		goto checkSize;

	    case EMR_MODIFYWORLDTRANSFORM:
		step= bmEmfReadAffineTransform2D( &transform, sis );
		if  ( step < 0 )
		    { LDEB(step); return -1;	}
		done += step;

		arg= sioEndianGetLeUint32( sis ); done += 4;

		switch( arg )
		    {
		    case MWT_IDENTITY:
			WMFDEB(appDebug(
			    "ModifyWorldTransform(MWT_IDENTITY)\n"));
			dc->dcTransform.dctLogicalToOutput=
					dc->dcTransform.dctDeviceToOutput;
			break;

		    case MWT_LEFTMULTIPLY:
			WMFDEB(appDebug(
			    "ModifyWorldTransform(MWT_LEFTMULTIPLY [%g,%g,%g,%g + %g,%g])\n",
			    transform.at2Axx, transform.at2Ayx,
			    transform.at2Axy, transform.at2Ayy,
			    transform.at2Tx,  transform.at2Ty ));

			/* or the other way round? */
			geoAffineTransform2DProduct(
					&(dc->dcTransform.dctLogicalToOutput),
					&(dc->dcTransform.dctLogicalToOutput),
					&transform );
			break;

		    case MWT_RIGHTMULTIPLY:
			WMFDEB(appDebug(
			    "ModifyWorldTransform(MWT_RIGHTMULTIPLY [%g,%g,%g,%g + %g,%g])\n",
			    transform.at2Axx, transform.at2Ayx,
			    transform.at2Axy, transform.at2Ayy,
			    transform.at2Tx,  transform.at2Ty ));
			/* or the other way round? */
			geoAffineTransform2DProduct(
					&(dc->dcTransform.dctLogicalToOutput),
					&transform,
					&(dc->dcTransform.dctLogicalToOutput) );
			break;

		    case MWT_SET:
			WMFDEB(appDebug(
			    "ModifyWorldTransform(MWT_SET [%g,%g,%g,%g + %g,%g])\n",
			    transform.at2Axx, transform.at2Ayx,
			    transform.at2Axy, transform.at2Ayy,
			    transform.at2Tx,  transform.at2Ty ));

			geoAffineTransform2DProduct(
				&(dc->dcTransform.dctLogicalToOutput),
				&(dc->dcTransform.dctDeviceToOutput),
				&transform );
			break;

		    default:
			WMFDEB(appDebug("ModifyWorldTransform(%ld)\n", arg ));
			LDEB(arg);
		    }

		goto checkSize;

	    case EMR_CREATEDIBPATTERNBRUSHPT:
		{
		arg= sioEndianGetLeUint32( sis ); done += 4;

		WMFDEB(appDebug("CreateDibPatternBrushPt(%ld)\n", arg ));

		expectBytes= recordSize- done;

		step= appEmfReadPatternBrush( dc, arg, sis, done, expectBytes );
		if  ( step < 0 )
		    { LDEB(step); return -1;	}
		done += step;
		}
		goto checkSize;

	    case EMR_EXTCREATEFONTINDIRECTW:
		arg= sioEndianGetLeUint32( sis ); done += 4;

		WMFDEB(appDebug("ExtCreateFontIndirectW(%ld)\n", arg ));

		if  ( recordSize- done <= 320 )
		    {
		    expectBytes= recordSize- done;
		    step= appEmfReadLogFontPanose( dc, arg, sis, expectBytes );
		    if  ( step < 0 )
			{ LDEB(step); return -1;	}
		    done += step;
		    }
		else{
		    expectBytes= recordSize- done;
		    step= appEmfReadLogFontExDv( dc, arg, sis, expectBytes );
		    if  ( step < 0 )
			{ LDEB(step); return -1;	}
		    done += step;
		    }

		goto checkSize;

	    case EMR_BITBLT:
		{
		DocumentRectangle	drSrc;
		DocumentRectangle	drDest;

		bmi			bmiSrc;

		step= bmEmfReadRectangle( &bounds, sis );
		if  ( step < 0 )
		    { LDEB(step); return -1;	}
		done += step;

		step= bmEmfReadXYWHRectangle( &drDest, sis );
		if  ( step < 0 )
		    { LDEB(step); return -1;	}
		done += step;

		(void) /* BitBltRasterOperation= */
				    sioEndianGetLeInt32( sis ); done += 4;

		drSrc.drX0= sioEndianGetLeInt32( sis ); done += 4;
		drSrc.drY0= sioEndianGetLeInt32( sis ); done += 4;
		/* Same Size */
		drSrc.drX1= drSrc.drX0+ ( drDest.drX1- drDest.drX0 );
		drSrc.drY1= drSrc.drY0+ ( drDest.drY1- drDest.drY0 );

		step= bmEmfReadAffineTransform2D( &transform, sis );
		if  ( step < 0 )
		    { LDEB(step); return -1;	}
		done += step;

		(void) /* BkColorSrc= */ sioEndianGetLeInt32( sis ); done += 4;
		(void) /* UsageSrc= */ sioEndianGetLeInt32( sis ); done += 4;

		done += appEmfReadBmi( &bmiSrc, sis );

		expectBytes= recordSize- done;

		if  ( appMetaDrawRasterImage( sis, through, expectBytes, dc,
							    &drSrc, &drDest ) )
		    { LDEB(1); return -1; }
		done= recordSize;
		}
		goto checkSize;

	    case EMR_STRETCHBLT:
		step= bmEmfReadRectangle( &bounds, sis );
		if  ( step < 0 )
		    { LDEB(step); return -1;	}
		done += step;

		WMFDEB(appDebug("StretchBlt()\n"));

		expectBytes= recordSize- done;

		step= appEmfStretchBlt( dc, sis, through, done, expectBytes );
		if  ( step < 0 )
		    { LDEB(step); return -1;	}
		done += step;
		goto checkSize;

	    case EMR_STRETCHDIBITS:
		{
		bmi			bmiSrc;

		DocumentRectangle	drSrc;
		DocumentRectangle	drDest;
		int			w;
		int			h;

		/* Device Coords */
		step= bmEmfReadRectangle( &bounds, sis );
		if  ( step < 0 )
		    { LDEB(step); return -1;	}
		done += step;

		drDest.drX0= sioEndianGetLeInt32( sis ); done += 4;
		drDest.drY0= sioEndianGetLeInt32( sis ); done += 4;

		step= bmEmfReadXYWHRectangle( &drSrc, sis );
		if  ( step < 0 )
		    { LDEB(step); return -1;	}
		done += step;

		done += appEmfReadBmi( &bmiSrc, sis );

		(void) /* UsageSrc= */ sioEndianGetLeInt32( sis ); done += 4;
		(void) /* BitBltRasterOperation= */
				    sioEndianGetLeInt32( sis ); done += 4;

		w= sioEndianGetLeInt32( sis ); done += 4;
		h= sioEndianGetLeInt32( sis ); done += 4;
		drDest.drX1= drDest.drX0+ w- 1;
		drDest.drY1= drDest.drY0+ h- 1;

		while ( bmiSrc.offBmi > done )
		    { (void) sioInGetByte( sis ); done++;	}
		expectBytes= recordSize- done;

		WMFDEB(appDebug("StretchDiBits()\n"));

		if  ( appMetaDrawRasterImage( sis, through, expectBytes, dc,
							    &drSrc, &drDest ) )
		    { LDEB(1); return -1; }
		done= recordSize;
		}
		goto checkSize;

	    case EMR_ARC:
		WMFDEB(appDebug("Arc()\n"));

		step= appEmfGetArc( &bounds, &s, &e, dc, sis );
		if  ( step < 0 )
		    { LDEB(step); return -1;	}
		done += step;

		if  ( (*dc->dcDrawArc)( dc, through, &bounds,
				    s.x, s.y, e.x, e.y,
				    dc->dcFillInsides, dc->dcDrawBorders ) )
		    { LDEB(1); return -1;	}

		continue;
		goto checkSize;

	    case EMR_PIE:
		WMFDEB(appDebug("Pie()\n"));

		step= appEmfGetArc( &bounds, &s, &e, dc, sis );
		if  ( step < 0 )
		    { LDEB(step); return -1;	}
		done += step;

		if  ( (*dc->dcDrawPie)( dc, through, &bounds,
				    s.x, s.y, e.x, e.y,
				    dc->dcFillInsides, dc->dcDrawBorders ) )
		    { LDEB(1); return -1;	}

		continue;
		goto checkSize;

	    case EMR_EXTTEXTOUTW:
		{
		int		iGraphicsMode;
		double		exScale;
		double		eyScale;

		WMFDEB(appDebug("ExtTextOutW(...)\n"));

		step= bmEmfReadRectangle( &bounds, sis );
		if  ( step < 0 )
		    { LDEB(step); return -1;	}
		done += step;

		iGraphicsMode= sioEndianGetLeInt32( sis ); done += 4;
		exScale= sioEndianGetLeFloat( sis ); done += 4;
		eyScale= sioEndianGetLeFloat( sis ); done += 4;

		step= appEmfReadTextObject( dc, done, sis, &x0, &y0 );
		if  ( step < 0 )
		    { LDEB(step); return -1;	}
		done += step;

		WMFDEB(appDebug("ExtTextOutW( %ld, %ld, \"%s\" )\n", x0, y0,
				    (char *)dc->dcCollectedText.mbBytes ));

		if  ( iGraphicsMode == GM_COMPATIBLE && appMetaSaveDC( dc ) )
		    { FFDEB(exScale,eyScale); return -1;	}

		/* Adapt transform and rescale current font; undo below */

		x0= DC_xViewport( x0, y0, dc );
		y0= DC_yViewport( x0, y0, dc );

		if  ( (*dc->dcDrawString)( dc, through, x0, y0,
						    &(dc->dcCollectedText) ) )
		    { LDEB(recordSize); return -1;	}

		if  ( iGraphicsMode == GM_COMPATIBLE && appMetaRestoreDC( dc ) )
		    { FFDEB(exScale,eyScale); return -1;	}
		}
		goto checkSize;

	    case EMR_SMALLTEXTOUT:
		{
		int		count;

		WMFDEB(appDebug("SmallTextOut(...)\n"));

		step= appWmfReadSmallTextObject( dc, done, sis,
						    &x0, &y0, &count );
		if  ( step < 0 )
		    { LDEB(step); return -1;	}
		done += step;

		x0= DC_xViewport( x0, y0, dc );
		y0= DC_yViewport( x0, y0, dc );

		if  ( (*dc->dcDrawString)( dc, through, x0, y0,
						    &(dc->dcCollectedText) ) )
		    { LDEB(recordSize); return -1;	}
		}
		goto checkSize;

	    case EMR_BEGINPATH:
		WMFDEB(appDebug("BeginPath()\n"));
		goto checkSize;

	    case EMR_CLOSEFIGURE:
		WMFDEB(appDebug("CloseFigure()\n"));
		goto checkSize;

	    case EMR_ENDPATH:
		WMFDEB(appDebug("EndPath()\n"));
		goto checkSize;

	    case EMR_FILLPATH:
		step= bmEmfReadRectangle( &bounds, sis );
		if  ( step < 0 )
		    { LDEB(step); return -1;	}
		done += step;

		WMFDEB(appDebug("FillPath()\n"));
		goto checkSize;

	    case EMR_STROKEPATH:
		step= bmEmfReadRectangle( &bounds, sis );
		if  ( step < 0 )
		    { LDEB(step); return -1;	}
		done += step;

		WMFDEB(appDebug("StrokePath()\n"));
		goto checkSize;

	    case EMR_STROKEANDFILLPATH:
		step= bmEmfReadRectangle( &bounds, sis );
		if  ( step < 0 )
		    { LDEB(step); return -1;	}
		done += step;

		WMFDEB(appDebug("StrokeAndFillPath()\n"));
		goto checkSize;

	    case EMR_SETPIXELV:
		{
		LogicalBrush	lb;

		bounds.drX0= sioEndianGetLeInt32( sis ); done += 4;
		bounds.drY0= sioEndianGetLeInt32( sis ); done += 4;
		bounds.drX1= bounds.drX0;
		bounds.drY1= bounds.drY0;

		lb.lbStyle= BS_SOLID;
		step= bmEmfReadRgb8Color( &(lb.lbColor), sis );
		if  ( step < 0 )
		    { LDEB(step); return -1;	}
		done += step;

		WMFDEB(appDebug("SetPixelV(%d,%d,rgba:%d,%d,%d,%d)\n",
		    bounds.drX0, bounds.drY0,
		    lb.lbColor.rgb8Red, lb.lbColor.rgb8Green,
		    lb.lbColor.rgb8Blue, lb.lbColor.rgb8Alpha));

		if  ( (*dc->dcSelectBrushObject)( dc, through, &lb ) )
		    { LDEB(1); return -1;	}

		appWinMetaDrawRectangle( dc, through, &bounds, 1, 0 );

		if  ( dc->dcCurrentBrushObject >= 0 )
		    {
		    if  ( appWinMetaSelectObject( dc, through,
						dc->dcCurrentBrushObject ) )
			{ LDEB(dc->dcCurrentBrushObject); return -1;	}
		    }
		}
		goto checkSize;

	    case EMR_EOF:
		break;

	    skipArguments:
		if  ( done > recordSize )
		    { XLLDEB(recordType,done,recordSize); return -1;	}
		while( done < recordSize )
		    { (void) sioInGetByte( sis ); done++; }
		continue;
	    checkSize:
# 		if 0
		if  ( done != recordSize )
		    { SXLLDEB("###",recordType,done,recordSize); abort(); }
# 		endif
		if  ( done != recordSize )
		    { SXLLDEB("###",recordType,done,recordSize);	}
		if  ( done > recordSize )
		    { XLLDEB(recordType,done,recordSize); return -1;	}
		while( done < recordSize )
		    { (void) sioInGetByte( sis ); done++; }
		continue;
	    default:
		XLDEB(recordType,recordSize);
#		if 1
		while( done < recordSize )
		    { (void) sioInGetByte( sis ); done++; }
		continue;
#		else
		return -1;
#		endif
	    }

	break;
	}

    return 0;
    }

