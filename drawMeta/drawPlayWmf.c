#   include	"drawMetaConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>
#   include	<string.h>

#   include	"drawWinMetaImpl.h"
#   include	<sioEndian.h>
#   include	<bmWmfIo.h>
#   include	<bmio.h>
#   include	<uniUtf8.h>

#   include	<appDebugon.h>

# if 0
#    define	WMFDEB(x)	(x)
# else
#    define	WMFDEB(x)	/*nothing*/
# endif

/************************************************************************/

static int appWmfGetObjectSlot(	DeviceContext *	dc )
    {
    int		ob;

    for ( ob= 0; ob < dc->dcObjectCount; ob++ )
	{
	if  ( dc->dcObjects[ob].mfoType == MFtypeFREE )
	    { break;	}
	}
    
    return ob;
    }

/************************************************************************/
/*									*/
/*  Handle a poly-polygon. For simplicity, treat it as multiple polygons*/
/*									*/
/************************************************************************/

static int appWmfGetCounts(	SimpleInputStream *	sis,
				int			recordSize,
				int *			pPolyCount,
				int *			pPointCount,
				int **			pCounts,
				DeviceContext *		dc )
    {
    int			pointCount= 0;
    int			polyCount;
    int			i;

    int *		fresh;

    polyCount= sioEndianGetLeInt16( sis );

    fresh= (int *)realloc( dc->dcCounts, polyCount* sizeof(int) );
    if  ( ! fresh )
	{ LXDEB(polyCount,fresh); return -1;	}
    dc->dcCounts= fresh;

    for ( i= 0; i < polyCount; i++ )
	{
	dc->dcCounts[i]= sioEndianGetLeInt16( sis );
	pointCount += dc->dcCounts[i];
	}

    *pPolyCount= polyCount;
    *pPointCount= pointCount;
    *pCounts= dc->dcCounts;
    return 0;
    }

static int appWmfDrawPolyPolygon(	SimpleInputStream *	sis,
					void *			through,
					int			recordSize,
					DeviceContext *		dc )
    {
    int			polyCount;
    int			pointCount;
    int *		pointCounts;

    if  ( appWmfGetCounts( sis, recordSize,
				&polyCount, &pointCount, &pointCounts, dc ) )
	{ LDEB(1); return -1;	}

    if  ( appWinMetaGetPoints16( dc, pointCount, sis ) < 0 )
	{ LDEB(pointCount); return -1;	}

    WMFDEB(appDebug("PolyPolygon( polyCount=%d, ... )\n", polyCount ));

    if  ( (*dc->dcDrawPolyPolygon)( dc, through,
				polyCount, pointCounts, dc->dcPoints,
				dc->dcFillInsides, dc->dcDrawBorders, 1 ) )
	{ LDEB(polyCount); return -1;	}

    return 0;
    }

static int appWmfReadCountAndPoints(	DeviceContext *		dc,
					int *			pCount,
					SimpleInputStream *	sis,
					int			recordSize )
    {
    int				step;
    int				done= 0;
    int				count;

    count= sioEndianGetLeInt16( sis ); done += 2;

    if  ( 2* count != recordSize- 4 )
	{ LLDEB(2*count,recordSize-4); return -1;	}

    step= appWinMetaGetPoints16( dc, count, sis );
    if  ( step < 0 )
	{ LLDEB(count,step); return -1;	}
    done += step;

    *pCount= count; return done;
    }

static int appWmfCreatePenIndirect(	DeviceContext *		dc,
					int			recordSize,
					SimpleInputStream *	sis )
    {
    int			ob;
    int			done;

    int			x;
    int			y;

    for ( ob= 0; ob < dc->dcObjectCount; ob++ )
	{
	if  ( dc->dcObjects[ob].mfoType == MFtypeFREE )
	    { break;	}
	}
    dc->dcObjects[ob].mfoType= MFtypePEN;

    dc->dcObjects[ob].mfoLogicalPen.lpStyle= sioEndianGetLeInt16( sis );
    x= sioEndianGetLeInt16( sis );
    y= sioEndianGetLeInt16( sis );
    dc->dcObjects[ob].mfoLogicalPen.lpWidth= ( x+ y )/ 2;
    bmWmfGetColor( sis, &dc->dcObjects[ob].mfoLogicalPen.lpColor );

    WMFDEB(appDebug("CreatePenIndirect( style=%d,x,y=%d,%d) ob=%d\n",
				dc->dcObjects[ob].mfoLogicalPen.lpStyle,
				x,y, ob ));

    for ( done= 8; done < recordSize; done++ )
	{ (void) /* ignore= */ sioEndianGetLeInt16( sis ); }

    return 0;
    }

static int appWmfCreateBrushIndirect(	DeviceContext *		dc,
					int			recordSize,
					SimpleInputStream *	sis )
    {
    int			ob;
    LogicalBrush *	lb;
    const char *	hatch="";
    const char *	style="";

    for ( ob= 0; ob < dc->dcObjectCount; ob++ )
	{
	if  ( dc->dcObjects[ob].mfoType == MFtypeFREE )
	    { break;	}
	}

    lb= &(dc->dcObjects[ob].mfoLogicalBrush);
    dc->dcObjects[ob].mfoType= MFtypeBRUSH;
    lb->lbTilePixmap= (struct DrawingSurface *)0;
    lb->lbTileNumber= -1;

    lb->lbStyle= sioEndianGetLeInt16( sis );
    
    switch( lb->lbStyle )
	{
	case BS_SOLID:
	    style= "SOLID";
	    bmWmfGetColor( sis, &(lb->lbColor) );
	    lb->lbHatch= sioEndianGetLeInt16( sis );
	    break;
	case BS_HOLLOW:
	    style= "HOLLOW";
	    bmWmfGetColor( sis, &(lb->lbColor) );
	    lb->lbHatch= sioEndianGetLeInt16( sis );
	    break;
	case BS_HATCHED:
	    style= "HATCHED";
	    bmWmfGetColor( sis, &(lb->lbColor) );
	    lb->lbHatch= sioEndianGetLeInt16( sis );
	    break;
	case BS_PATTERN:
	    style= "PATTERN";
	    bmWmfGetColor( sis, &(lb->lbColor) );
	    lb->lbHatch= sioEndianGetLeInt16( sis );
	    break;
	default:
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
	    break;
	}

    WMFDEB(appDebug("CreateBrushIndirect(%s,%s) ob=%d\n",
						    style, hatch, ob ));

    style= style; hatch= hatch; /* use them */

    return 0;
    }

static int appWmfCreateFontIndirect(	DeviceContext *		dc,
					int			bytesLeft,
					SimpleInputStream *	sis )
    {
    int				ob;
    LogicalFont *		lf;

    int				count;
    int				i;
    int				expectBytes;
    int				bytesDone= 0;

    ob= appWmfGetObjectSlot( dc );
    dc->dcObjects[ob].mfoType= MFtypeFONT;

    lf= &(dc->dcObjects[ob].mfoLogicalFont);

    lf->lfHeight= sioEndianGetLeInt16( sis ); bytesDone += 2;
    lf->lfWidth= sioEndianGetLeInt16( sis ); bytesDone += 2;
    lf->lfEscapement= sioEndianGetLeInt16( sis ); bytesDone += 2;
    lf->lfOrientation= sioEndianGetLeInt16( sis ); bytesDone += 2;
    lf->lfWeight= sioEndianGetLeInt16( sis ); bytesDone += 2;

    lf->lfItalic= sioInGetByte( sis ); bytesDone++;
    lf->lfUnderline= sioInGetByte( sis ); bytesDone++;
    lf->lfStrikeOut= sioInGetByte( sis ); bytesDone++;
    lf->lfCharSet= sioInGetByte( sis ); bytesDone++;
    lf->lfOutPrecision= sioInGetByte( sis ); bytesDone++;
    lf->lfClipPrecision= sioInGetByte( sis ); bytesDone++;
    lf->lfQuality= sioInGetByte( sis ); bytesDone++;
    lf->lfPitchAndFamily= sioInGetByte( sis ); bytesDone++;

    memset( lf->lfFaceNameUtf8, 0, sizeof(lf->lfFaceNameUtf8) );
    expectBytes= bytesLeft- bytesDone;
    count= 0;
    for ( i= 0; i < expectBytes; i++ )
	{
	int	step;
	int	c= sioInGetByte( sis ); bytesDone++;

	if  ( c == 0 )
	    { break; 	}

	step= uniPutUtf8( lf->lfFaceNameUtf8+ count, c );
	if  ( step < 1 )
	    { XLDEB(c,step); break;	}
	count += step;
	}

    lf->lfFaceNameUtf8[count]= '\0';

    while( bytesDone < bytesLeft )
	{
	if  ( sioInGetByte( sis ) < 0 )
	    { LDEB(1); return -1;	}
	bytesDone++;
	}

    lf->lfPrivateFont= -1;

    if  ( appWinMetaRememberFontInList( dc, &(lf->lfTextAttribute), lf ) )
	{ SDEB(lf->lfFaceNameUtf8); return -1;	}

    WMFDEB(appDebug("CreateFontIndirect() -> %d\n", ob ));
    return bytesDone;
    }

static int appWmfReadTextObject(SimpleInputStream *	sis,
				int			recordSize,
				int *			pX0,
				int *			pY0,
				int *			pStyle,
				DocumentRectangle *	bgRect,
				DeviceContext *		dc )
    {
    int			x0;
    int			y0;
    int			count;
    int			style;

    int			shortsDone;
    int			done= 0;
    int			step;

    y0= sioEndianGetLeInt16( sis ); done += 2;
    x0= sioEndianGetLeInt16( sis ); done += 2;
    count= sioEndianGetLeInt16( sis ); done += 2;
    style= sioEndianGetLeInt16( sis ); done += 2;

    shortsDone= 7;

    if  ( style )
	{
	step= bmWmfReadHWYXRectangle( bgRect, sis );
	if  ( step < 0 )
	    { LDEB(step); return -1;	}
	done += step;
	shortsDone += step/ 2;
	}
    else{ geoInitRectangle( bgRect );	}

    step= appWinMetaReadLegacyText( dc, count, 2* (( count+ 1 )/ 2), sis );
    if  ( step < 0 || step % 2 )
	{ LDEB(step); return -1;	}
    done += step;
    shortsDone += step/ 2;

    for ( ; shortsDone < recordSize; shortsDone++ )
	{ (void) /* ignore= */ sioEndianGetLeInt16( sis ); done += 2; }

    WMFDEB(appDebug( "ExtTextOut( x=%d, y= %d, opts= %d, ", x0, y0, style ));
    if  ( style )
	{
	WMFDEB(appDebug( "rect=[%d..%d,%d..%d], ",
					bgRect->drX0, bgRect->drX1,
					bgRect->drY0, bgRect->drY1 ));
	}

    WMFDEB(appDebug( "\"%.*s\", .... )\n",
				    dc->dcCollectedText.mbSize,
				    (char *)dc->dcCollectedText.mbBytes ));

    *pX0= x0;
    *pY0= y0;
    *pStyle= style;

    return done;
    }

static int appWmfTextOut(	SimpleInputStream *	sis,
				int			recordSize,
				int *			pX0,
				int *			pY0,
				DeviceContext *		dc )
    {
    int			x0;
    int			y0;
    int			step;
    int			count;
    int			done= 0;

    count= sioEndianGetLeInt16( sis ); done += 2;

    step= appWinMetaReadLegacyText( dc, count, count, sis );
    if  ( step < 0 )
	{ LDEB(step); return -1;	}
    if  ( step % 2 )
	{
	if  ( sioInGetByte( sis ) < 0 )
	    { LDEB(step); return -1;	}

	step++;
	}
    done += step;

    y0= sioEndianGetLeInt16( sis ); done += 2;
    x0= sioEndianGetLeInt16( sis ); done += 2;

    WMFDEB(appDebug("TextOut( \"%.*s\", x=%d, y= %d )\n",
				    dc->dcCollectedText.mbSize,
				    (char *)dc->dcCollectedText.mbBytes,
				    x0, y0 ));

    *pX0= x0;
    *pY0= y0;

    return done;
    }

static int appWmfCreatePatternBrush(	DeviceContext *		dc,
					int			recordSize,
					SimpleInputStream *	sis )
    {
    int			ob;
    int			got;
    int			done= 3;

    PatternBrush *	pb;

    ob= appWmfGetObjectSlot( dc );
    dc->dcObjects[ob].mfoType= MFtypePATTERNBRUSH;

    pb= &(dc->dcObjects[ob].mfoPatternBrush);

    pb->pbType= 0;
    pb->pbUsage= 0;
    pb->pbAbi= (RasterImage *)0;
    pb->pbTilePixmap= (struct DrawingSurface *)0;
    pb->pbTileNumber= -1;

    pb->pbType= sioEndianGetLeInt16( sis ); done++;
    pb->pbUsage= sioEndianGetLeInt16( sis ); done++;

    if  ( pb->pbUsage != 0 )
	{
	LLDEB(pb->pbType,pb->pbUsage);
	for ( done= done; done < recordSize; done++ )
	    { (void) sioEndianGetLeInt16( sis ); }
	WMFDEB(appDebug("CreatePatternBrush(...) ob=%d? recordSize=%d\n",
							    ob, recordSize ));
	return 0;
	}

    switch( pb->pbType )
	{
	case 3:
	case 5:
	    pb->pbAbi= (RasterImage *)malloc( sizeof(RasterImage) );
	    if  ( ! pb->pbAbi )
		{ PDEB(pb->pbAbi); return -1;	}
	    bmInitRasterImage( pb->pbAbi );
	    got= bmBmpReadDib( &(pb->pbAbi->riDescription),
					    &(pb->pbAbi->riBytes), sis );
	    if  ( got < 0 || got > 2* ( recordSize- done ) )
		{
		LLDEB(got,recordSize-done );

		bmCleanRasterImage( pb->pbAbi );
		free( pb->pbAbi );
		pb->pbAbi= (RasterImage *)0;
		return -1;
		}
	    if  ( got % 2 )
		{
		if  ( sioInGetByte( sis ) < 0 )
		    { LDEB(got); return -1;	}
		got++;
		}
	    done += got/ 2;
	    break;
	default:
	    LLDEB(pb->pbType,pb->pbUsage);
	    for ( done= done; done < recordSize; done++ )
		{ (void) sioEndianGetLeInt16( sis ); }
	    WMFDEB(appDebug("CreatePatternBrush(...) ob=%d? recordSize=%d\n",
							    ob, recordSize ));
	    return 0;
	}

    while( done < recordSize )
	{ (void) sioEndianGetLeInt16( sis ); done++;	}

    WMFDEB(appDebug("CreatePatternBrush(...) ob=%d recordSize=%d\n",
							    ob, recordSize ));

    return 0;
    }

/************************************************************************/
/*									*/
/*  Manage the current coordinate transform.				*/
/*									*/
/************************************************************************/

static int appMetaScaleViewportExt(	DeviceContext *		dc,
					int			recordSize,
					SimpleInputStream *	sis )
    {
    int				yd;
    int				yn;
    int				xd;
    int				xn;

    DeviceCoordinateTransform *	dct= &(dc->dcTransform);
    int				w;
    int				h;

    w= dct->dctDeviceRect.drX1- dct->dctDeviceRect.drX0;
    h= dct->dctDeviceRect.drY1- dct->dctDeviceRect.drY0;

    yd= sioEndianGetLeInt16( sis );
    yn= sioEndianGetLeInt16( sis );
    xd= sioEndianGetLeInt16( sis );
    xn= sioEndianGetLeInt16( sis );

    WMFDEB(appDebug("SetScaleViewportExt(xe*=%d/%d,ye*=%d/%d)\n",
							xn, xd, yn, yd ));

    dct->dctDeviceRect.drX1= dct->dctDeviceRect.drX0+ ( xn* w )/ xd;
    dct->dctDeviceRect.drY1= dct->dctDeviceRect.drY0+ ( yn* h )/ yd;
    appWinMetaUpdateTransform( dc );

    return 0;
    }

static int appMetaScaleWindowExt(	DeviceContext *		dc,
					int			recordSize,
					SimpleInputStream *	sis )
    {
    int				yd;
    int				yn;
    int				xd;
    int				xn;

    DeviceCoordinateTransform *	dct= &(dc->dcTransform);
    int				w;
    int				h;

    w= dct->dctLogicalRect.drX1- dct->dctLogicalRect.drX0;
    h= dct->dctLogicalRect.drY1- dct->dctLogicalRect.drY0;

    yd= sioEndianGetLeInt16( sis );
    yn= sioEndianGetLeInt16( sis );
    xd= sioEndianGetLeInt16( sis );
    xn= sioEndianGetLeInt16( sis );

    WMFDEB(appDebug("SetScaleWindowExt(xe*=%d/%d,ye*=%d/%d)\n",
							xn, xd, yn, yd ));

    dct->dctLogicalRect.drX1= dct->dctLogicalRect.drX0+ ( xn* w )/ xd;
    dct->dctLogicalRect.drY1= dct->dctLogicalRect.drY0+ ( yn* h )/ yd;
    appWinMetaUpdateTransform( dc );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Create a palette.							*/
/*									*/
/************************************************************************/

static int appWmfCreatePalette(		DeviceContext *		dc,
					int			recordSize,
					SimpleInputStream *	sis )
    {
    int			ob;
    int			done;

    ob= appWmfGetObjectSlot( dc );
    dc->dcObjects[ob].mfoType= MFtypePALETTE;

    (void) sioEndianGetLeInt16( sis );
    (void) /* count= */ sioEndianGetLeInt16( sis );
    /*
    if  ( 2* count != recordSize- 5 )
	{ LLDEB(count,recordSize);	}
    */
    for ( done= 5; done < recordSize; done++ )
	{ (void) sioEndianGetLeInt16( sis ); }

    WMFDEB(appDebug("CreatePalette(...) ob=%d\n", ob ));

    return 0;
    }

/************************************************************************/
/*									*/
/*  Play a windows metafile and invoke callbacks.			*/
/*									*/
/************************************************************************/

int appMetaPlayWmf(	DeviceContext *			dc,
			void *				through )
    {
    SimpleInputStream *	sis= dc->dcPlayer->mpInputStream;

    WMFDEB(appDebug( "PLAY WMF ...\n" ));

    for (;;)
	{
	long			rop;

	DocumentRectangle	bounds;
	int			x0;
	int			y0;
	int			w0;
	int			h0;

	int			x1;
	int			y1;

	int			xs;
	int			ys;
	int			xe;
	int			ye;

	int			step;
	int			bytesDone;
	int			ignore;

	long			recordSize;
	int			function;
	int			arg;
	int			style;
	int			count;

	recordSize= sioEndianGetLeInt32( sis );
	function= sioEndianGetLeInt16( sis );
	bytesDone= 4+ 2;

	if  ( recordSize < 0 || function < 0 )
	    { LLDEB(recordSize,function); return -1;	}

	switch( function )
	    {
	    case META_OFFSETWINDOWORG:
		y0= sioEndianGetLeInt16( sis ); bytesDone += 2;
		x0= sioEndianGetLeInt16( sis ); bytesDone += 2;

		WMFDEB(appDebug("OffsetWindowOrg(yWinOrg+=%d,xWinOrg+=%d)\n",
								    y0,x0 ));

		dc->dcTransform.dctLogicalRect.drX0 += x0;
		dc->dcTransform.dctLogicalRect.drY0 += y0;
		dc->dcTransform.dctLogicalRect.drX1 += x0;
		dc->dcTransform.dctLogicalRect.drY1 += y0;
		appWinMetaUpdateTransform( dc );

		goto checkSize;

	    case META_SETWINDOWORG:
		y0= sioEndianGetLeInt16( sis ); bytesDone += 2;
		x0= sioEndianGetLeInt16( sis ); bytesDone += 2;

		WMFDEB(appDebug("SetWindowOrg(yWinOrg=%d,xWinOrg=%d)\n",
								    y0,x0 ));

		w0= x0- dc->dcTransform.dctLogicalRect.drX0;
		h0= y0- dc->dcTransform.dctLogicalRect.drY0;
		dc->dcTransform.dctLogicalRect.drX0 += w0;
		dc->dcTransform.dctLogicalRect.drY0 += h0;
		dc->dcTransform.dctLogicalRect.drX1 += w0;
		dc->dcTransform.dctLogicalRect.drY1 += h0;
		appWinMetaUpdateTransform( dc );

		goto checkSize;

	    case META_SETWINDOWEXT:
		h0= sioEndianGetLeInt16( sis ); bytesDone += 2;
		w0= sioEndianGetLeInt16( sis ); bytesDone += 2;

		WMFDEB(appDebug("SetWindowExt(yWinExt=%d,xWinExt=%d)\n",
								    h0, w0 ));
		dc->dcTransform.dctLogicalRect.drX1=
				    dc->dcTransform.dctLogicalRect.drX0+ w0;
		dc->dcTransform.dctLogicalRect.drY1=
				    dc->dcTransform.dctLogicalRect.drY0+ h0;
		appWinMetaUpdateTransform( dc );

		goto checkSize;

	    case META_SETVIEWPORTORG:
		y0= sioEndianGetLeInt16( sis ); bytesDone += 2;
		x0= sioEndianGetLeInt16( sis ); bytesDone += 2;

		WMFDEB(appDebug("SetViewportOrg(yViewOrg=%d,xViewOrg=%d)\n", y0,x0 ));

		w0= x0- dc->dcTransform.dctDeviceRect.drX0;
		h0= y0- dc->dcTransform.dctDeviceRect.drY0;
		dc->dcTransform.dctDeviceRect.drX0 += w0;
		dc->dcTransform.dctDeviceRect.drY0 += h0;
		dc->dcTransform.dctDeviceRect.drX1 += w0;
		dc->dcTransform.dctDeviceRect.drY1 += h0;
		appWinMetaUpdateTransform( dc );
		goto checkSize;

	    case META_SETVIEWPORTEXT:
		h0= sioEndianGetLeInt16( sis ); bytesDone += 2;
		w0= sioEndianGetLeInt16( sis ); bytesDone += 2;

		WMFDEB(appDebug("SetViewportExt(yViewExt=%d,xViewExt=%d)\n",
								    h0, w0 ));

		dc->dcTransform.dctDeviceRect.drX1=
				    dc->dcTransform.dctDeviceRect.drX0+ w0;
		dc->dcTransform.dctDeviceRect.drY1=
				    dc->dcTransform.dctDeviceRect.drY0+ h0;
		appWinMetaUpdateTransform( dc );

		goto checkSize;

	    case META_SAVEDC:
		WMFDEB(appDebug("SaveDC()\n"));

		if  ( appMetaSaveDC( dc ) )
		    { LDEB(1); return -1;	}
		goto checkSize;

	    case META_RESTOREDC:
		ignore= sioEndianGetLeInt16( sis ); bytesDone += 2;

		WMFDEB(appDebug("RestoreDC(%d)\n", ignore ));

		if  ( ignore >= 0 )
		    { LDEB(ignore);	}

		if  ( appMetaRestoreDC( dc ) )
		    { LDEB(1); 	}

		goto checkSize;

	    case META_OFFSETCLIPRGN:
		y0= sioEndianGetLeInt16( sis ); bytesDone += 2;
		x0= sioEndianGetLeInt16( sis ); bytesDone += 2;

		WMFDEB(appDebug("OffsetClipRgn(yClpOrg+=%d,xClpOrg+=%d)\n",
								    y0,x0 ));

		dc->dcClipRect.drY0 += y0;
		dc->dcClipRect.drX0 += x0;
		dc->dcClipRect.drY1 += y0;
		dc->dcClipRect.drX1 += x0;
		goto checkSize;

	    case META_SELECTCLIPREGION:
		switch( recordSize )
		    {
		    case 4:
			arg= sioEndianGetLeInt16( sis ); bytesDone += 2;
			break;
		    case 5:
			arg= sioEndianGetLeInt32( sis ); bytesDone += 4;
			break;
		    default:
			LDEB(recordSize); return -1;
		    }

		WMFDEB(appDebug("SelectClipRgn(%d)\n", arg));

		goto checkSize;

	    case META_EXCLUDECLIPRECT:
		if  ( appMetaExcludeClipRect( dc, recordSize, sis ) )
		    { LDEB(1); return -1;	}
		continue;

	    case META_INTERSECTCLIPRECT:
		if  ( appMetaIntersectClipRect( dc, recordSize, sis ) )
		    { LDEB(1); return -1;	}
		continue;

	    case META_SETMAPPERFLAGS:
		rop= sioEndianGetLeInt32( sis ); bytesDone += 4;
		WMFDEB(appDebug("SetMapperFlags(0x%lx)\n",rop));
		goto checkSize;

	    case META_DIBBITBLT:
		{
		DocumentRectangle	drSrc;
		DocumentRectangle	drDest;

		rop= sioEndianGetLeInt32( sis ); bytesDone += 4;
		/*  source	*/
		drSrc.drX0= sioEndianGetLeInt16( sis ); bytesDone += 2;
		drSrc.drY0= sioEndianGetLeInt16( sis ); bytesDone += 2;
		/*  destination	*/
		step= bmWmfReadHWYXRectangle( &drDest, sis );
		if  ( step < 0 )
		    { LDEB(step); return -1;	}
		bytesDone += step;

		drSrc.drX1= drSrc.drX0+ ( drDest.drX1- drDest.drX0 );
		drSrc.drY1= drSrc.drY0+ ( drDest.drY1- drDest.drY0 );

		WMFDEB(appDebug("BitBlt([%d..%d,%d..%d]->[%d..%d,%d..%d],..)\n",
			drSrc.drX0, drSrc.drX1, drSrc.drY0, drSrc.drY1, 
			drDest.drX0, drDest.drX1, drDest.drY0, drDest.drY1 ));

		if  ( appMetaDrawRasterImage( sis, through,
				2*(recordSize-3-2-6*1), dc, &drSrc, &drDest ) )
		    { LDEB(1); return -1; }

		}
		continue;

	    case META_DIBSTRETCHBLT:
		{
		DocumentRectangle	drSrc;
		DocumentRectangle	drDest;

		rop= sioEndianGetLeInt32( sis ); bytesDone += 4;
		/*  source	*/
		step= bmWmfReadHWYXRectangle( &drSrc, sis );
		if  ( step < 0 )
		    { LDEB(step); return -1;	}
		bytesDone += step;

		/*  destination	*/
		step= bmWmfReadHWYXRectangle( &drDest, sis );
		if  ( step < 0 )
		    { LDEB(step); return -1;	}
		bytesDone += step;

		WMFDEB(appDebug(
			"StretchBlt([%d..%d,%d..%d]->[%d..%d,%d..%d],..)\n",
			drSrc.drX0, drSrc.drX1, drSrc.drY0, drSrc.drY1, 
			drDest.drX0, drDest.drX1, drDest.drY0, drDest.drY1 ));

		if  ( appMetaDrawRasterImage( sis, through,
				2*(recordSize-3-2-8*1), dc, &drSrc, &drDest ) )
		    { LDEB(1); return -1; }

		continue;
		}

	    case META_STRETCHDIB:
		{
		DocumentRectangle	drSrc;
		DocumentRectangle	drDest;

		rop= sioEndianGetLeInt32( sis ); bytesDone += 4;
		ignore= sioEndianGetLeInt16( sis ); bytesDone += 2;
		/*  source	*/
		step= bmWmfReadHWYXRectangle( &drSrc, sis );
		if  ( step < 0 )
		    { LDEB(step); return -1;	}
		bytesDone += step;

		/*  destination	*/
		step= bmWmfReadHWYXRectangle( &drDest, sis );
		if  ( step < 0 )
		    { LDEB(step); return -1;	}
		bytesDone += step;

		WMFDEB(appDebug(
			"StretchDIBits([%d..%d,%d..%d]->[%d..%d,%d..%d],..)\n",
			drSrc.drX0, drSrc.drX1, drSrc.drY0, drSrc.drY1, 
			drDest.drX0, drDest.drX1, drDest.drY0, drDest.drY1 ));

		if  ( appMetaDrawRasterImage( sis, through,
				2*(recordSize-3-2-9*1), dc, &drSrc, &drDest ) )
		    { LDEB(1); return -1; }
		continue;
		}

	    case META_SELECTOBJECT:
		arg= sioEndianGetLeInt16( sis ); bytesDone += 2;

		if  ( arg < 0 || arg >= dc->dcObjectCount )
		    { LLDEB(arg,dc->dcObjectCount); return -1;	}

		if  ( appWinMetaSelectObject( dc, through, arg ) )
		    { LLDEB(recordSize,arg); return -1;	}
		goto checkSize;

	    case META_CREATEBRUSHINDIRECT:
		if  ( appWmfCreateBrushIndirect( dc, recordSize, sis ) )
		    { LDEB(recordSize); return -1;	}
		continue;

	    case META_CREATEPENINDIRECT:
		if  ( appWmfCreatePenIndirect( dc, recordSize, sis ) )
		    { LDEB(recordSize); return -1;	}
		continue;

	    case META_DIBCREATEPATTERNBRUSH:
		if  ( appWmfCreatePatternBrush( dc, recordSize, sis ) )
		    { LDEB(recordSize); return -1;	}
		continue;

	    case META_CREATEPALETTE:
		if  ( appWmfCreatePalette( dc, recordSize, sis ) )
		    { LDEB(recordSize); return -1;	}
		continue;

	    case META_CREATEFONTINDIRECT:
		WMFDEB(appDebug("CreateFontIndirect() recordSize=%ld\n",
								recordSize));
		step= appWmfCreateFontIndirect( dc, 2* recordSize- bytesDone, sis );
		if  ( step < 0 )
		    { LDEB(step); return -1;	}
		bytesDone += step;
		goto checkSize;

	    case META_DELETEOBJECT:

		arg= sioEndianGetLeInt16( sis ); bytesDone += 2;

		if  ( arg < 0 || arg >= dc->dcObjectCount )
		    { LLDEB(arg,dc->dcObjectCount); return -1;	}

		WMFDEB(appDebug("DeleteObject(%d)\n", arg));
		(*dc->dcCleanObject)( dc, through, &(dc->dcObjects[arg]) );
		appMetaCleanObject( &(dc->dcObjects[arg]) );
		dc->dcObjects[arg].mfoType= MFtypeFREE;

		goto checkSize;

	    case META_SETBKCOLOR:
		bmWmfGetColor( sis, &(dc->dcBkColor) ); bytesDone += 4;

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

	    case META_SETTEXTCOLOR:
		bmWmfGetColor( sis, &(dc->dcTextColor) ); bytesDone += 4;

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

	    case META_SETBKMODE:
		if  ( recordSize == 4 )
		    {
		    dc->dcBkMode= sioEndianGetLeInt16( sis ); bytesDone += 2;
		    WMFDEB(appDebug("SetBkMode(%d)\n",dc->dcBkMode));
		    goto checkSize;
		    }
		if  ( recordSize == 5 )
		    {
		    dc->dcBkMode= sioEndianGetLeInt32( sis ); bytesDone += 4;
		    WMFDEB(appDebug("SetBkMode(%d)\n",dc->dcBkMode));
		    goto checkSize;
		    }
		WMFDEB(appDebug("SetBkMode(...)\n"));
		XLDEB(function,recordSize); return -1;

	    case META_SETTEXTCHAREXTRA:
		WMFDEB(appDebug("SetTextCharExtra(...)\n"));
		dc->dcExtraTextSpacing= sioEndianGetLeInt16( sis ); bytesDone += 2;
		goto checkSize;

	    case META_SETTEXTALIGN:
		switch( recordSize )
		    {
		    case 4:
			arg= sioEndianGetLeInt16( sis ); bytesDone += 2;
			break;
		    case 5:
			arg= sioEndianGetLeInt32( sis ); bytesDone += 4;
			break;
		    default:
			LDEB(recordSize); return -1;
		    }

		WMFDEB(appDebug("SetTextAlign( 0x%x )\n", arg ));

		if  ( appMetaSetTextAlign( dc, arg ) )
		    { LDEB(recordSize); return -1;	}
		goto checkSize;

	    case META_SETTEXTJUSTIFICATION:
		dc->dcJustificationAmount= sioEndianGetLeInt16( sis ); bytesDone += 2;
		dc->dcJustificationSpaces= sioEndianGetLeInt16( sis ); bytesDone += 2;

		WMFDEB(appDebug("SetTextJustification(...)\n"));

		goto checkSize;

	    case META_SETSTRETCHBLTMODE:
		switch( recordSize )
		    {
		    case 4:
			arg= sioEndianGetLeInt16( sis ); bytesDone += 2;
			break;
		    case 5:
			arg= sioEndianGetLeInt32( sis ); bytesDone += 4;
			break;
		    default:
			LDEB(recordSize); return -1;
		    }

		dc->dcStretchBltMode= arg;
		WMFDEB(appDebug("SetStretchBltMode(%ld)\n",
						    dc->dcStretchBltMode ));
		goto checkSize;

	    case META_POLYLINE:
		step= appWmfReadCountAndPoints( dc, &count, sis, recordSize );
		if  ( step < 0 )
		    { LLDEB(recordSize,step); return -1;	}
		bytesDone += step;

		WMFDEB(appDebug("PolyLine( count=%d, ... )\n", count ));

		if  ( (*dc->dcDrawPolyPolygon)( dc, through,
					1, &count, dc->dcPoints, 0, 1, 0 ) )
		    { LDEB(count); return -1;	}
		goto checkSize;

	    case META_POLYGON:
		step= appWmfReadCountAndPoints( dc, &count, sis, recordSize );
		if  ( step < 0 )
		    { LLDEB(recordSize,step); return -1;	}
		bytesDone += step;

		WMFDEB(appDebug("Polygon( count=%d, ... )\n", count ));

		if  ( (*dc->dcDrawPolyPolygon)( dc, through,
				    1, &count, dc->dcPoints,
				    dc->dcFillInsides, dc->dcDrawBorders, 1 ) )
		    { LDEB(count); return -1;	}
		goto checkSize;

	    case META_RECTANGLE:
		step= bmWmfReadRectangle( &bounds, sis );
		if  ( step < 0 )
		    { LDEB(step); return -1;	}
		bytesDone += step;

		WMFDEB(appDebug("Rectangle(%d..%d,%d..%d)\n",
						bounds.drX0,bounds.drX1,
						bounds.drY0,bounds.drY1));

		if  ( appWinMetaDrawRectangle( dc, through, &bounds,
				    dc->dcFillInsides, dc->dcDrawBorders ) )
		    { LDEB(1); return -1;	}
		goto checkSize;

	    case META_LINETO:
		x0= dc->dcX;
		y0= dc->dcY;
		y1= sioEndianGetLeInt16( sis ); bytesDone += 2;
		x1= sioEndianGetLeInt16( sis ); bytesDone += 2;

		WMFDEB(appDebug("LineTo( x:%d-> %d, y:%d-> %d )\n",
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

	    case META_MOVETO:
		y0= sioEndianGetLeInt16( sis ); bytesDone += 2;
		x0= sioEndianGetLeInt16( sis ); bytesDone += 2;

		WMFDEB(appDebug("MoveTo( x=%d, y=%d )\n", x0, y0 ));

		dc->dcX= x0;
		dc->dcY= y0;

		goto checkSize;

	    case META_EXTTEXTOUT:
		{
		DocumentRectangle	bgRect;

		step= appWmfReadTextObject( sis, recordSize, &x0, &y0, &style,
								&bgRect, dc );
		if  ( step < 0 )
		    { LDEB(step); return -1;		}
		bytesDone += step;

		x0= DC_xViewport( x0, y0, dc );
		y0= DC_yViewport( x0, y0, dc );

		if  ( (*dc->dcDrawString)( dc, through, x0, y0,
						    &(dc->dcCollectedText) ) )
		    { LDEB(recordSize); return -1;	}
		}
		goto checkSize;

	    case META_TEXTOUT:
		step= appWmfTextOut( sis, recordSize, &x0, &y0, dc );
		if  ( step < 0 )
		    { LDEB(step); return -1;		}
		bytesDone += step;

		x0= DC_xViewport( x0, y0, dc );
		y0= DC_yViewport( x0, y0, dc );

		if  ( (*dc->dcDrawString)( dc, through, x0, y0,
						    &(dc->dcCollectedText) ) )
		    { LDEB(recordSize); return -1;	}
		goto checkSize;

	    case META_ESCAPE:
		WMFDEB(appDebug("Escape(...)\n"));
		goto skipArguments;

	    case META_PATBLT:
		rop= sioEndianGetLeInt32( sis ); bytesDone += 4;
		step= bmWmfReadHWYXRectangle( &bounds, sis );
		if  ( step < 0 )
		    { LDEB(step); return -1;	}
		bytesDone += step;

		WMFDEB(appDebug("PatBlt([%d..%d,%d..%d], rop=0x%lx )\n",
					    bounds.drX0, bounds.drX1,
					    bounds.drY0, bounds.drY1, rop ));

		appWinMetaTransformRectangle( &bounds, dc, &bounds );

		if  ( (*dc->dcPatBlt)( dc, through, rop, &bounds ) )
		    { LDEB(recordSize); return -1;	}
		goto checkSize;

	    case META_SETPOLYFILLMODE:
		if  ( recordSize != 4 && recordSize != 5 )
		    { LDEB(recordSize); return -1;	}

		if  ( recordSize == 5 )
		    { arg= sioEndianGetLeInt32( sis ); bytesDone += 4;	}
		else{ arg= sioEndianGetLeInt16( sis ); bytesDone += 2;	}

		WMFDEB(appDebug("SetPolyFillMode(%d)\n", arg ));

		if  ( (*dc->dcSetPolyFillMode)( dc, through, arg ) )
		    { LDEB(arg); return -1;	}

		dc->dcPolyFillMode= arg;

		goto checkSize;

	    case META_PIE:
		ye= sioEndianGetLeInt16( sis ); bytesDone += 2;
		xe= sioEndianGetLeInt16( sis ); bytesDone += 2;
		ys= sioEndianGetLeInt16( sis ); bytesDone += 2;
		xs= sioEndianGetLeInt16( sis ); bytesDone += 2;

		step= bmWmfReadRectangle( &bounds, sis );
		if  ( step < 0 )
		    { LDEB(step); return -1;	}
		bytesDone += step;

		WMFDEB(appDebug("Pie(%d..%d,%d..%d, (%d,%d), (%d,%d)\n",
						    bounds.drX0,bounds.drX1,
						    bounds.drY0,bounds.drY1,
						    xs,ys,xe,ye));

		appWinMetaTransformRectangle( &bounds, dc, &bounds );

		ys= DC_yViewport( xs, ys, dc );
		xs= DC_xViewport( xs, ys, dc );
		ye= DC_yViewport( xe, ye, dc );
		xe= DC_xViewport( xe, ye, dc );

		if  ( (*dc->dcDrawPie)( dc, through, &bounds, xs, ys, xe, ye,
					dc->dcFillInsides, dc->dcDrawBorders ) )
		    { LDEB(1); return -1;	}
		goto checkSize;

	    case META_ARC:
		ye= sioEndianGetLeInt16( sis ); bytesDone += 2;
		xe= sioEndianGetLeInt16( sis ); bytesDone += 2;
		ys= sioEndianGetLeInt16( sis ); bytesDone += 2;
		xs= sioEndianGetLeInt16( sis ); bytesDone += 2;

		step= bmWmfReadRectangle( &bounds, sis );
		if  ( step < 0 )
		    { LDEB(step); return -1;	}
		bytesDone += step;

		WMFDEB(appDebug("Arc(%d..%d,%d..%d, (%d,%d), (%d,%d)\n",
						    bounds.drX0,bounds.drX1,
						    bounds.drY0,bounds.drY1,
						    xs,ys,xe,ye));

		appWinMetaTransformRectangle( &bounds, dc, &bounds );

		ys= DC_yViewport( xs, ys, dc );
		xs= DC_xViewport( xs, ys, dc );
		ye= DC_yViewport( xe, ye, dc );
		xe= DC_xViewport( xe, ye, dc );

		if  ( (*dc->dcDrawArc)( dc, through, &bounds, xs, ys, xe, ye,
					dc->dcFillInsides, dc->dcDrawBorders ) )
		    { LDEB(1); return -1;	}
		goto checkSize;

	    case META_CHORD:
		ye= sioEndianGetLeInt16( sis ); bytesDone += 2;
		xe= sioEndianGetLeInt16( sis ); bytesDone += 2;
		ys= sioEndianGetLeInt16( sis ); bytesDone += 2;
		xs= sioEndianGetLeInt16( sis ); bytesDone += 2;

		step= bmWmfReadRectangle( &bounds, sis );
		if  ( step < 0 )
		    { LDEB(step); return -1;	}
		bytesDone += step;

		WMFDEB(appDebug("Chord(%d..%d,%d..%d, (%d,%d), (%d,%d)\n",
						    bounds.drX0,bounds.drX1,
						    bounds.drY0,bounds.drY1,
						    xs,ys,xe,ye));

		appWinMetaTransformRectangle( &bounds, dc, &bounds );

		ys= DC_yViewport( xs, ys, dc );
		xs= DC_xViewport( xs, ys, dc );
		ye= DC_yViewport( xe, ye, dc );
		xe= DC_xViewport( xe, ye, dc );

		if  ( (*dc->dcDrawChord)( dc, through, &bounds, xs, ys, xe, ye,
					dc->dcFillInsides, dc->dcDrawBorders ) )
		    { LDEB(1); return -1;	}
		goto checkSize;

	    case META_ELLIPSE:
		step= bmWmfReadRectangle( &bounds, sis );
		if  ( step < 0 )
		    { LDEB(step); return -1;	}
		bytesDone += step;

		WMFDEB(appDebug("Ellipse(%d..%d,%d..%d)\n",
						bounds.drX0,bounds.drX1,
						bounds.drY0,bounds.drY1));

		if  ( appWinMetaDrawEllipse( dc, through, &bounds,
				    dc->dcFillInsides, dc->dcDrawBorders ) )
		    { LDEB(1); return -1;	}
		goto checkSize;

	    case META_ROUNDRECT:
		h0= sioEndianGetLeInt16( sis ); bytesDone += 2;
		w0= sioEndianGetLeInt16( sis ); bytesDone += 2;

		step= bmWmfReadRectangle( &bounds, sis );
		if  ( step < 0 )
		    { LDEB(step); return -1;	}
		bytesDone += step;

		WMFDEB(appDebug("RoundRect(%d..%d,%d..%d, w= %d,h= %d\n",
						bounds.drX0,bounds.drX1,
						bounds.drY0,bounds.drY1,
						w0, h0 ));

		appWinMetaTransformRectangle( &bounds, dc, &bounds );

		h0= DC_hViewport( h0, dc );
		w0= DC_wViewport( w0, dc );

		if  ( (*dc->dcDrawRoundedRectangle)( dc, through,
				    &bounds, w0, h0,
				    dc->dcFillInsides, dc->dcDrawBorders ) )
		    { LDEB(1); return -1;	}
		goto checkSize;

	    case META_SETMAPMODE:
		arg= sioEndianGetLeInt16( sis ); bytesDone += 2;

		WMFDEB(appDebug("SetMapMode(mapMode=%d)\n", arg ));

		dc->dcMapMode= arg;
		appWinMetaUpdateTransform( dc );
		goto checkSize;

	    case META_SETROP2:
		if  ( recordSize == 5 )
		    {
		    dc->dcROP2= sioEndianGetLeInt32( sis ); bytesDone += 4;
		    WMFDEB(appDebug("SetROP2(%ld)\n", (long)dc->dcROP2 ));
		    goto checkSize;
		    }
		if  ( recordSize == 4 )
		    {
		    dc->dcROP2= sioEndianGetLeInt16( sis ); bytesDone += 2;
		    WMFDEB(appDebug("SetROP2(%ld)\n", (long)dc->dcROP2 ));
		    goto checkSize;
		    }
		XLDEB(function,recordSize); return -1;

	    case META_SELECTPALETTE:
		arg= sioEndianGetLeInt16( sis ); bytesDone += 2;
		WMFDEB(appDebug("SelectPalette(%d)\n", arg ));
		goto checkSize;

	    case META_REALIZEPALETTE:
		WMFDEB(appDebug("RealizePalette()\n"));
		goto checkSize;

	    case META_CREATEREGION:
		arg= appWmfGetObjectSlot( dc );
		dc->dcObjects[arg].mfoType= MFtypeREGION;
		WMFDEB(appDebug("CreateRegion() -> %d\n", arg));
		goto skipArguments;
		continue;

	    case META_SETPIXEL:
		{
		LogicalBrush	lb;

		lb.lbStyle= BS_SOLID;
		step= bmWmfGetColor( sis, &(lb.lbColor) );
		if  ( step < 0 )
		    { LDEB(step); return -1;	}
		bytesDone += step;

		bounds.drY0= sioEndianGetLeInt16( sis ); bytesDone += 2;
		bounds.drX0= sioEndianGetLeInt16( sis ); bytesDone += 2;
		bounds.drX1= bounds.drX0;
		bounds.drY1= bounds.drY0;

		WMFDEB(appDebug("SetPixel(%d,%d,rgba:%d,%d,%d,%d)\n",
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

	    case META_POLYPOLYGON:
		if  ( appWmfDrawPolyPolygon( sis, through, recordSize, dc ) )
		    { LDEB(1); return -1;	}
		continue;

	    case META_SCALEVIEWPORTEXT:
		appMetaScaleViewportExt( dc, recordSize, sis );
		continue;

	    case META_SCALEWINDOWEXT:
		appMetaScaleWindowExt( dc, recordSize, sis );
		continue;

	    case META_SETRELABS: /* MUST be ignored */
		goto skipArguments;

	    case META_EOF:
		WMFDEB(appDebug("0:EOF\n"));
		if  ( recordSize != 3 )
		    { XLDEB(function,recordSize); return -1;	}
		break;
	    skipArguments:
		{
		int		done;

		for ( done= 3; done < recordSize; done++ )
		    { ignore= sioEndianGetLeInt16( sis ); }
		continue;
		}
	    checkSize:
# 		if 0
		if  ( bytesDone != 2* recordSize )
		    { SXLLDEB("###",recordType,bytesDone,2*recordSize); abort(); }
# 		endif
		if  ( bytesDone != 2* recordSize )
		    { SXLLDEB("###",function,bytesDone,2*recordSize); }
		if  ( bytesDone > 2* recordSize )
		    { SXLLDEB("###",function,bytesDone,2*recordSize); return -1;	}
		while( bytesDone < 2* recordSize )
		    { (void) sioInGetByte( sis ); bytesDone++; }
		continue;
	    default:
		XLDEB(function,recordSize);
		/*
		for ( done= 3; done < recordSize; done++ )
		    { ignore= sioEndianGetLeInt16( sis ); }
		continue;
		*/
		return -1;
	    }

	break;
	}

    return 0;
    }
