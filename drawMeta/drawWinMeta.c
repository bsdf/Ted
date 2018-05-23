#   include	"drawMetaConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>
#   include	<string.h>
#   include	<math.h>

#   include	"drawWinMetaImpl.h"
#   include	<sioEndian.h>
#   include	<psDocumentFontStyle.h>
#   include	<geoUnits.h>
#   include	<bmio.h>
#   include	<bmWmfIo.h>
#   include	<textConverter.h>

#   include	<appDebugon.h>

# if 0
#    define	WMFDEB(x)	(x)
# else
#    define	WMFDEB(x)	/*nothing*/
# endif

static void appCleanPatternBrush(	PatternBrush *	pb )
    {
    if  ( pb->pbAbi )
	{
	bmCleanRasterImage( pb->pbAbi );
	free( pb->pbAbi );
	pb->pbAbi= (RasterImage *)0;
	}
    }

/************************************************************************/
/*									*/
/*  Calculate the device to output transform based on the map mode.	*/
/*									*/
/************************************************************************/

static void appWinMetaRefreshTransform(	DeviceContext *		dc )
    {
    DeviceCoordinateTransform *	t= &(dc->dcTransform);
    AffineTransform2D		logicalToDevice;
    int				twipsWide;
    int				twipsHigh;
    double			high;
    double			wide;

    twipsWide= dc->dcTwipsRect.drX1- dc->dcTwipsRect.drX0+ 1;
    twipsHigh= dc->dcTwipsRect.drY1- dc->dcTwipsRect.drY0+ 1;

    switch( dc->dcMapMode )
	{
	case MM_TEXT:
	    /* Unit pixel, Y Down */
	    LDEB(dc->dcMapMode); return;

	case MM_LOMETRIC:
	    /* Unit 0.1mm, Y Up */
	    wide= ( 10000.0* twipsWide )/ TWIPS_PER_M;
	    high= ( 10000.0* twipsHigh )/ TWIPS_PER_M;
	    t->dctDeviceRect.drX0= 0;
	    t->dctDeviceRect.drY0= 0;
	    t->dctDeviceRect.drX1= t->dctDeviceRect.drX0+ wide- 1;
	    t->dctDeviceRect.drY1= t->dctDeviceRect.drY0+ high- 1;
	    t->dctLogicalRect.drX1= t->dctLogicalRect.drX0+ wide- 1;
	    t->dctLogicalRect.drY1= t->dctLogicalRect.drY0+ high- 1;
	    break;

	case MM_HIMETRIC:
	    /* Unit 0.01mm, Y Up */
	    wide= ( 100000.0* twipsWide )/ TWIPS_PER_M;
	    high= ( 100000.0* twipsHigh )/ TWIPS_PER_M;
	    t->dctDeviceRect.drX0= 0;
	    t->dctDeviceRect.drY0= 0;
	    t->dctDeviceRect.drX1= t->dctDeviceRect.drX0+ wide- 1;
	    t->dctDeviceRect.drY1= t->dctDeviceRect.drY0+ high- 1;
	    t->dctLogicalRect.drX1= t->dctLogicalRect.drX0+ wide- 1;
	    t->dctLogicalRect.drY1= t->dctLogicalRect.drY0+ high- 1;
	    break;

	case MM_LOENGLISH:
	    /* Unit 0.01inch, Y Up */
	    wide= ( 100000.0* twipsWide )/ ( 20* 72 );
	    high= ( 100000.0* twipsHigh )/ ( 20* 72 );
	    t->dctDeviceRect.drX0= 0;
	    t->dctDeviceRect.drY0= 0;
	    t->dctDeviceRect.drX1= t->dctDeviceRect.drX0+ wide- 1;
	    t->dctDeviceRect.drY1= t->dctDeviceRect.drY0+ high- 1;
	    t->dctLogicalRect.drX1= t->dctLogicalRect.drX0+ wide- 1;
	    t->dctLogicalRect.drY1= t->dctLogicalRect.drY0+ high- 1;
	    break;

	case MM_HIENGLISH:
	    /* Unit 0.001inch, Y Up */
	    wide= ( 1000000.0* twipsWide )/ ( 20* 72 );
	    high= ( 1000000.0* twipsHigh )/ ( 20* 72 );
	    t->dctDeviceRect.drX0= 0;
	    t->dctDeviceRect.drY0= 0;
	    t->dctDeviceRect.drX1= t->dctDeviceRect.drX0+ wide- 1;
	    t->dctDeviceRect.drY1= t->dctDeviceRect.drY0+ high- 1;
	    t->dctLogicalRect.drX1= t->dctLogicalRect.drX0+ wide- 1;
	    t->dctLogicalRect.drY1= t->dctLogicalRect.drY0+ high- 1;
	    break;

	case MM_TWIPS:
	    /* Unit twips, Y Up */
	    t->dctDeviceRect.drX0= 0;
	    t->dctDeviceRect.drY0= 0;
	    t->dctDeviceRect.drX1= t->dctDeviceRect.drX0+ twipsWide- 1;
	    t->dctDeviceRect.drY1= t->dctDeviceRect.drY0+ twipsHigh- 1;
	    t->dctLogicalRect.drX1= t->dctLogicalRect.drX0+ twipsWide- 1;
	    t->dctLogicalRect.drY1= t->dctLogicalRect.drY0+ twipsHigh- 1;
	    break;

	case MM_ISOTROPIC:
	case MM_ANISOTROPIC:
	    break;

	default:
	    LDEB(dc->dcMapMode); return;
	}

    if  ( geoAffineTransformForTriangles( &logicalToDevice,
	    t->dctLogicalRect.drX0,	t->dctLogicalRect.drY0,
	    t->dctLogicalRect.drX1,	t->dctLogicalRect.drY0,
	    t->dctLogicalRect.drX0,	t->dctLogicalRect.drY1,

	    t->dctDeviceRect.drX0,	t->dctDeviceRect.drY0,
	    t->dctDeviceRect.drX1,	t->dctDeviceRect.drY0,
	    t->dctDeviceRect.drX0,	t->dctDeviceRect.drY1 ) )
	{ LDEB(1);	}

    if  ( geoAffineTransformForTriangles( &(t->dctDeviceToOutput),
	    t->dctDeviceRect.drX0,	t->dctDeviceRect.drY0,
	    t->dctDeviceRect.drX1,	t->dctDeviceRect.drY0,
	    t->dctDeviceRect.drX0,	t->dctDeviceRect.drY1,

	    dc->dcOutputRect.drX0,	dc->dcOutputRect.drY0,
	    dc->dcOutputRect.drX1,	dc->dcOutputRect.drY0,
	    dc->dcOutputRect.drX0,	dc->dcOutputRect.drY1 ) )
	{ LDEB(1);	}

    geoAffineTransform2DProduct( &(dc->dcTransform.dctLogicalToOutput),
					&(dc->dcTransform.dctDeviceToOutput),
					&logicalToDevice );
    }

int appMetaInitDeviceContext(	DeviceContext *			dc,
				const MetafilePlayer *		player,
				int				objectCount,
				const DocumentRectangle *	drLogical,
				int				mapMode,
				int				outputWide,
				int				outputHigh,
				int				twipsWide,
				int				twipsHigh )
    {
    dc->dcDrawBorders= 1;
    dc->dcFillInsides= 1;

    dc->dcFillHatched= 0;
    dc->dcFillPattern= 0;

    docInitFontList( &(dc->dcFontList) );
    dc->dcPlayer= player;
    textInitTextConverter( &(dc->dcTextConverter) );
    utilInitMemoryBuffer( &(dc->dcCollectedText) );

    dc->dcPen.lpStyle= PS_SOLID;
    dc->dcPen.lpWidth= 1;
    utilRGB8SolidBlack( &(dc->dcPen.lpColor) );

    dc->dcBrush.lbStyle= BS_SOLID;
    utilRGB8SolidWhite( &(dc->dcBrush.lbColor) );

    utilRGB8SolidWhite( &(dc->dcBkColor) );
    utilRGB8SolidBlack( &(dc->dcTextColor) );

    utilRGB8SolidBlack( &(dc->dcMacPictForeColor) );
    utilRGB8SolidWhite( &(dc->dcMacPictBackColor) );

    utilRGB8SolidBlack( &(dc->dcMacPictPenSolid) );
    utilRGB8SolidBlack( &(dc->dcMacPictFillSolid) );
    utilRGB8SolidWhite( &(dc->dcMacPictBackSolid) );

    dc->dcCurrentBrushObject= -1;

    dc->dcMacPictPenIsSolid= 1;
    dc->dcMacPictFillIsSolid= 1;
    dc->dcMacPictBackIsSolid= 1;

    memset( dc->dcMacPictPenPattern, 0xff, 8 );
    memset( dc->dcMacPictFillPattern, 0xff, 8 );
    memset( dc->dcMacPictBackPattern, 0xff, 8 );

    dc->dcBkMode= TRANSPARENT;
    dc->dcPolyFillMode= WINDING;

    dc->dcExtraTextSpacing= 0;
    dc->dcJustificationAmount= 0;
    dc->dcJustificationSpaces= 0;
    dc->dcTextAlignment= TA_LEFT|TA_TOP;

    dc->dcTwipsRect.drX0= 0;
    dc->dcTwipsRect.drY0= 0;
    dc->dcTwipsRect.drX1= twipsWide- 1;
    dc->dcTwipsRect.drY1= twipsHigh- 1;

    dc->dcOutputRect.drX0= 0;
    dc->dcOutputRect.drY0= 0;
    dc->dcOutputRect.drX1= outputWide- 1;
    dc->dcOutputRect.drY1= outputHigh- 1;

    dc->dcTransform.dctLogicalRect= *drLogical;
    /* A good initial guess: */
    dc->dcTransform.dctDeviceRect= *drLogical;

    dc->dcMapMode= mapMode;
    appWinMetaRefreshTransform( dc );
    appWinMetaUpdateTransform( dc );

    dc->dcFont.lfHeight= 0;
    dc->dcFont.lfWidth= 0;
    dc->dcFont.lfEscapement= 0;
    dc->dcFont.lfOrientation= 0;
    dc->dcFont.lfWeight= 0;
    dc->dcFont.lfItalic= 0;
    dc->dcFont.lfUnderline= 0;
    dc->dcFont.lfStrikeOut= 0;
    dc->dcFont.lfCharSet= 0;
    dc->dcFont.lfOutPrecision= 0;
    dc->dcFont.lfClipPrecision= 0;
    dc->dcFont.lfQuality= 0;
    dc->dcFont.lfPitchAndFamily= 0;
    dc->dcFont.lfFaceNameUtf8[0]= '\0';
    utilInitTextAttribute( &(dc->dcFont.lfTextAttribute) );

    /* A good initial guess: */
    dc->dcTransform.dctDeviceRect= dc->dcTransform.dctLogicalRect;
    dc->dcTransform.dctDeviceToOutput= dc->dcTransform.dctLogicalToOutput;

    dc->dcTransform.dctPushed= (DeviceCoordinateTransform *)0;

    dc->dcClipRect.drX0= 0;
    dc->dcClipRect.drY0= 0;
    dc->dcClipRect.drX1= 0;
    dc->dcClipRect.drY1= 0;

    dc->dcObjects= (MetaFileObject *)0;
    dc->dcObjectCount= 0;

    if  ( objectCount > 0 )
	{
	int		ob;

	dc->dcObjects= (MetaFileObject *)
			    malloc( objectCount* sizeof(MetaFileObject) );
	if  ( ! dc->dcObjects )
	    { XDEB(dc->dcObjects); return -1;	}

	for ( ob= 0; ob < objectCount; ob++ )
	    { dc->dcObjects[ob].mfoType= MFtypeFREE;	}

	dc->dcObjectCount= objectCount;
	}

    dc->dcPoints= (Point2DI *)0;
    dc->dcCounts= (int *)0;
    dc->dcAfi= (AfmFontInfo *)0;
    dc->dcFontEncoding= -1;

    utilInitRGB8Color( &(dc->dcColorSet) );
    dc->dcPenSet.lpStyle= -1;
    dc->dcX= dc->dcTransform.dctLogicalRect.drX0;
    dc->dcY= dc->dcTransform.dctLogicalRect.drY0;

    utilInitTextAttribute( &(dc->dcTextAttributeSet) );

    return 0;
    }

void appMetaCleanObject( MetaFileObject *	mfo )
    {
    switch( mfo->mfoType )
	{
	case MFtypeFREE:
	    return;
	case MFtypeBRUSH:
	    return;
	case MFtypePEN:
	    return;
	case MFtypeFONT:
	    return;
	case MFtypePALETTE:
	    return;
	case MFtypeREGION:
	    return;
	case MFtypePATTERNBRUSH:
	    appCleanPatternBrush( &(mfo->mfoPatternBrush) );
	    return;

	default:
	    LDEB(mfo->mfoType);
	    return;
	}
    }

void appMetaCleanDeviceContext(	DeviceContext *		dc,
				void *			through )
    {
    int		ob;

    docCleanFontList( &(dc->dcFontList) );
    textCleanTextConverter( &(dc->dcTextConverter) );
    utilCleanMemoryBuffer( &(dc->dcCollectedText) );

    for ( ob= 0; ob < dc->dcObjectCount; ob++ )
	{
	if  ( dc->dcObjects[ob].mfoType != MFtypeFREE )
	    {
	    (*dc->dcCleanObject)( dc, through, dc->dcObjects+ ob );
	    appMetaCleanObject( dc->dcObjects+ ob );
	    }
	}

    if  ( dc->dcObjects )
	{ free( dc->dcObjects );	}

    if  ( dc->dcPoints )
	{ free( dc->dcPoints );	}
    if  ( dc->dcCounts )
	{ free( dc->dcCounts );	}

    while( dc->dcTransform.dctPushed )
	{
	DeviceCoordinateTransform *	toFree= dc->dcTransform.dctPushed;

	dc->dcTransform= *dc->dcTransform.dctPushed;

	free( toFree );
	}

    return;
    }

int appWinMetaRememberFontInList(	DeviceContext *		dc,
					TextAttribute *		ta,
					const LogicalFont *	lf )
    {
    int				rval= 0;

    int				familyStyle= DFstyleFNIL;

    int				fontNum;
    DocumentFont		dfNew;
    DocumentFontList *		dfl= &(dc->dcFontList);
    const MetafilePlayer *	player= dc->dcPlayer;

    const IndexSet *		unicodesWanted;

    docInitDocumentFont( &dfNew );

    utilInitTextAttribute( ta );

    ta->taFontNumber= 0;
    if  ( lf->lfHeight < 0 )
	{ ta->taFontSizeHalfPoints= -lf->lfHeight; }	/* Value not used */
    else{ ta->taFontSizeHalfPoints=  lf->lfHeight; }	/* Value not used */
    ta->taFontIsBold= lf->lfWeight > 500;
    ta->taFontIsSlanted= lf->lfItalic != 0;
    ta->taTextIsUnderlined= lf->lfUnderline != 0;
    ta->taHasStrikethrough= lf->lfStrikeOut != 0;

    switch( lf->lfPitchAndFamily & 0xf0 )
	{
	case FF_ROMAN:		familyStyle= DFstyleFROMAN;	break;
	case FF_SWISS:		familyStyle= DFstyleFSWISS;	break;
	case FF_MODERN:		familyStyle= DFstyleFMODERN;	break;
	case FF_SCRIPT:		familyStyle= DFstyleFSCRIPT;	break;
	case FF_DECORATIVE:	familyStyle= DFstyleFDECOR;	break;
	case 0x60:		familyStyle= DFstyleFTECH;	break;

	case FF_DONTCARE:
	default:
	    break;
	}

    if  ( docFontSetFamilyName( &dfNew, lf->lfFaceNameUtf8 ) )
	{ LDEB(1); rval= -1; goto ready;	}
    if  ( docFontSetFamilyStyle( &dfNew, familyStyle ) )
	{ LDEB(1); rval= -1; goto ready;	}

    fontNum= docMergeFontIntoFontlist( dfl, &dfNew );
    if  ( fontNum < 0 )
	{ SLDEB(lf->lfFaceNameUtf8,fontNum); goto ready;	}

    ta->taFontNumber= fontNum;

    if  ( ! (*player->mpGetFontForAttribute)( &unicodesWanted, ta, dfl,
					    player->mpPostScriptFontList ) )
	{ SLDEB(lf->lfFaceNameUtf8,fontNum); goto ready;	}

    WMFDEB(appDebug("CreateFontIndirect(h=%d,w=%d,..\"%s\")\n",
			    lf->lfHeight,
			    lf->lfWidth,
			    lf->lfFaceNameUtf8 ));

  ready:

    docCleanDocumentFont( &dfNew );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Save the current coordinate transform.				*/
/*									*/
/************************************************************************/

int appMetaSaveDC(		DeviceContext *		dc )
    {
    DeviceCoordinateTransform *	dct;

    WMFDEB(appDebug("SaveDC()\n" ));

    dct= (DeviceCoordinateTransform *)malloc( sizeof(DeviceCoordinateTransform) );
    if  ( ! dct )
	{ XDEB(dct); return -1;	}

    *dct= dc->dcTransform;
    dc->dcTransform.dctPushed= dct;

    return 0;
    }

int appMetaRestoreDC(		DeviceContext *		dc )
    {
    DeviceCoordinateTransform *	prv= dc->dcTransform.dctPushed;

    if  ( ! prv )
	{ XDEB(prv); return -1;	}

    dc->dcTransform= *prv;
    free( prv );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Collect a series of points.						*/
/*									*/
/************************************************************************/

int appWinMetaGetPoints16(	DeviceContext *		dc,
				int			count,
				SimpleInputStream *	sis )
    {
    Point2DI *		xp;
    int			done= 0;

    int			p;

    xp= (Point2DI *)realloc( dc->dcPoints, (count+ 1)* sizeof(Point2DI) );
    if  ( ! xp )
	{ LXDEB(count,xp); return -1;	}
    dc->dcPoints= xp;

    for ( p= 0; p < count; xp++, p++ )
	{
	int	x= sioEndianGetLeInt16( sis );
	int	y= sioEndianGetLeInt16( sis );

	done += 4;

	xp->x= DC_xViewport( x, y, dc );
	xp->y= DC_yViewport( x, y, dc );

#	if 0
	WMFDEB(appDebug("@%d: (%d,%d) :: (%d,%d)\n", p, x,y, xp->x,xp->y ));
#	endif
	}

    *xp= dc->dcPoints[0];

    return done;
    }

int appWinMetaGetPoints32(	DeviceContext *		dc,
				int			count,
				SimpleInputStream *	sis )
    {
    Point2DI *		xp;
    int			done= 0;

    int			p;

    xp= (Point2DI *)realloc( dc->dcPoints, (count+ 1)* sizeof(Point2DI) );
    if  ( ! xp )
	{ LXDEB(count,xp); return -1;	}
    dc->dcPoints= xp;

    for ( p= 0; p < count; xp++, p++ )
	{
	int	x= sioEndianGetLeInt32( sis );
	int	y= sioEndianGetLeInt32( sis );

	done += 8;

	xp->x= DC_xViewport( x, y, dc );
	xp->y= DC_yViewport( x, y, dc );
	}

    *xp= dc->dcPoints[0];

    return done;
    }

/************************************************************************/
/*									*/
/*  Handle Clipping by ignoring it.					*/
/*									*/
/************************************************************************/

int appMetaExcludeClipRect(	DeviceContext *		dc,
				int			recordSize,
				SimpleInputStream *	sis )
    {
    DocumentRectangle	dr;

    bmWmfReadRectangle( &dr, sis );

    WMFDEB(appDebug("ExcludeClipRect(%d..%d,%d..%d)\n", x0,x1,y0,y1));

    return 0;
    }

int appMetaIntersectClipRect(	DeviceContext *		dc,
				int			recordSize,
				SimpleInputStream *	sis )
    {
    DocumentRectangle	dr;

    bmWmfReadRectangle( &dr, sis );

    WMFDEB(appDebug("IntersectClipRect(%d..%d,%d..%d)\n", x0,x1,y0,y1));

    return 0;
    }

/************************************************************************/
/*									*/
/*  Remember text alighnment.						*/
/*									*/
/************************************************************************/

int appMetaSetTextAlign(	DeviceContext *		dc,
				int			arg )
    {
    dc->dcTextAlignment= arg;

    WMFDEB(appDebug("SetTextAlign( 0x%x: ", arg ));

    switch( dc->dcTextAlignment & 0x01 )
	{
	case TA_NOUPDATECP:	WMFDEB(appDebug(" NOUPDATECP" )); break;
	case TA_UPDATECP:	WMFDEB(appDebug(" UPDATECP" )); break;
	default:
	    WMFDEB(appDebug(" 0x%x", dc->dcTextAlignment & 0x01 ));
	    break;
	}

    switch( dc->dcTextAlignment & 0x07 )
	{
	case TA_LEFT:	WMFDEB(appDebug(" LEFT" )); break;
	case TA_RIGHT:	WMFDEB(appDebug(" RIGHT" )); break;
	case TA_CENTER:	WMFDEB(appDebug(" CENTER" )); break;
	default:
	    WMFDEB(appDebug(" 0x%x", dc->dcTextAlignment & 0x07 ));
	    break;
	}

    switch( dc->dcTextAlignment & 0x18 )
	{
	case TA_TOP:		WMFDEB(appDebug(" TOP" )); break;
	case TA_BOTTOM:		WMFDEB(appDebug(" BOTTOM" )); break;
	case TA_BASELINE:	WMFDEB(appDebug(" BASELINE" )); break;
	default:
	    WMFDEB(appDebug(" 0x%x", dc->dcTextAlignment & 0x18 ));
	    break;
	}

    WMFDEB(appDebug(" )\n" ));

    return 0;
    }

/************************************************************************/
/*									*/
/*  Extract a bitmap from the metafile stream.				*/
/*									*/
/************************************************************************/

int appMetaReadBitmapImage(	int *			pSkip,
				RasterImage **		pRi,
				SimpleInputStream *	sis,
				int			expectBytes )
    {
    int			rval= 0;
    RasterImage *	ri= (RasterImage *)0;
    int			done= 0;

    if  ( expectBytes < 40 )
	{
	/* LDEB(expectBytes); */

	while( done < expectBytes )
	    {
	    if  ( sioInGetByte( sis ) < 0 )
		{ LDEB(1); rval= -1; goto ready;	}
	    done++;
	    }

	*pSkip= 1;
	*pRi= ri;
	goto ready;
	}

    ri= (RasterImage *)malloc( sizeof(RasterImage) );
    if  ( ! ri )
	{ XDEB(ri); rval= -1; goto ready;	}
    bmInitRasterImage( ri );

    done= bmBmpReadDib( &(ri->riDescription), &(ri->riBytes), sis );
    if  ( done < 0 || done > expectBytes )
	{ LLDEB(done,expectBytes); rval= -1; goto ready;	}

    while( done < expectBytes )
	{
	if  ( sioInGetByte( sis ) < 0 )
	    { LDEB(1); rval= -1; goto ready;	}

	done++;
	}

    *pSkip= 0;
    *pRi= ri; ri= (RasterImage *)0;

  ready:
    if  ( ri )
	{
	bmCleanRasterImage( ri );
	free( ri );
	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Handle a raster image in the metafile input.			*/
/*									*/
/************************************************************************/

int appMetaDrawRasterImage(	SimpleInputStream *		sis,
				void *				through,
				int				expectBytes,
				DeviceContext *			dc,
				const DocumentRectangle *	drSrc,
				const DocumentRectangle *	drDest )
    {
    int				rval= 0;
    int				skip;

    RasterImage *		abi= (RasterImage *)0;
    BitmapDescription *		bd;

    int				adjusted= 0;
    DocumentRectangle		drSrcAdjusted= *drSrc;
    DocumentRectangle		drOutput;

    if  ( appMetaReadBitmapImage( &skip, &abi, sis, expectBytes ) )
	{ LDEB(expectBytes); rval= -1; goto ready;	}

    if  ( skip )
	{
	if  ( expectBytes > 2 )
	    { LLDEB(expectBytes,skip);	}

	goto ready;
	}

    bd= &(abi->riDescription);

    appWinMetaTransformRectangle( &drOutput, dc, drDest );

    if  ( drSrcAdjusted.drX0 < 0 )
	{ drSrcAdjusted.drX0=  0; adjusted= 1;	}
    if  ( drSrcAdjusted.drY0 < 0 )
	{ drSrcAdjusted.drY0=  0; adjusted= 1;	}

    if  ( drSrcAdjusted.drX1 > bd->bdPixelsWide- 1 )
	{ drSrcAdjusted.drX1=  bd->bdPixelsWide- 1; adjusted= 1;	}
    if  ( drSrcAdjusted.drY1 > bd->bdPixelsHigh- 1 )
	{ drSrcAdjusted.drY1=  bd->bdPixelsHigh- 1; adjusted= 1;	}

    if  ( adjusted )
	{
	LLDEB(bd->bdPixelsWide,bd->bdPixelsHigh);
	RECTDEB(drSrc);
	}

    if  ( drSrcAdjusted.drX0 > drSrcAdjusted.drX1 )
	{ LLDEB( drSrcAdjusted.drX0,drSrcAdjusted.drX1); rval= -1; goto ready; }
    if  ( drSrcAdjusted.drY0 > drSrcAdjusted.drY1 )
	{ LLDEB( drSrcAdjusted.drY0,drSrcAdjusted.drY1); rval= -1; goto ready; }

    rval= (*dc->dcDrawRasterImage)( dc, through, abi,
					    &drSrcAdjusted, &drOutput );
    if  ( rval )
	{ LDEB(rval);	}

  ready:

    if  ( abi )
	{ bmCleanRasterImage( abi ); free( abi );	}

    return rval;
    }

int appWinMetaSelectObject(	DeviceContext *		dc,
				void *			through,
				int			ob )
    {
    MetaFileObject *		mfo= &(dc->dcObjects[ob]);

    switch( mfo->mfoType )
	{
	case MFtypePEN:
	    {
	    LogicalPen *	lp= &(mfo->mfoLogicalPen);

	    WMFDEB(appDebug(
		    "SelectObject( ob= %d ) PEN width=%d, rgba=%d:%d:%d:%d\n",
		    ob, lp->lpWidth,
		    lp->lpColor.rgb8Red, lp->lpColor.rgb8Green,
		    lp->lpColor.rgb8Blue, lp->lpColor.rgb8Alpha ));

	    if  ( (*dc->dcSelectPenObject)( dc, through, lp ) )
		{ LDEB(ob); return -1;	}

	    }
	    break;

	case MFtypeBRUSH:
	    WMFDEB(appDebug( "SelectObject( ob= %d ) BRUSH\n", ob ));
	    if  ( (*dc->dcSelectBrushObject)( dc, through, &(mfo->mfoLogicalBrush) ) )
		{ LDEB(ob); return -1;	}
	    dc->dcCurrentBrushObject= ob;
	    break;

	case MFtypePATTERNBRUSH:
	    WMFDEB(appDebug( "SelectObject( ob= %d ) PATTERNBRUSH\n", ob ));
	    if  ( (*dc->dcSelectPatternBrushObject)( dc, through, &(mfo->mfoPatternBrush) ) )
		{ LDEB(ob); return -1;	}
	    dc->dcCurrentBrushObject= ob;
	    break;

	case MFtypeFONT:
	    WMFDEB(appDebug( "SelectObject( ob= %d ) FONT\n", ob ));
	    if  ( (*dc->dcSelectFontObject)( dc, through, &(mfo->mfoLogicalFont) ) )
		{ LDEB(ob); return -1;	}
	    dc->dcFont= mfo->mfoLogicalFont;
	    break;

	default:
	    WMFDEB(appDebug( "SelectObject( ob= %d ) type= %d\n", ob, mfo->mfoType));
	    break;
	}

    return 0;
    }

void appWinMetaTransformRectangle(
				DocumentRectangle *		drOutput,
				DeviceContext *			dc,
				const DocumentRectangle *	drLogical )
    {
    DocumentRectangle	dr;

    dr.drX0= DC_xViewport( drLogical->drX0, drLogical->drY0, dc );
    dr.drY0= DC_yViewport( drLogical->drX0, drLogical->drY0, dc );
    dr.drX1= DC_xViewport( drLogical->drX1, drLogical->drY1, dc );
    dr.drY1= DC_yViewport( drLogical->drX1, drLogical->drY1, dc );

    geoNormalizeRectangle( drOutput, &dr );
    }

int appWinMetaDrawRectangle(	DeviceContext *			dc,
				void *				through,
				const DocumentRectangle *	drLogical,
				int				fillInside,
				int				drawBorder )
    {
    DocumentRectangle		drOutput;
    Point2DI			points[4];
    int				pointCount= 4;

    appWinMetaTransformRectangle( &drOutput, dc, drLogical );

    points[0].x= drOutput.drX0;
    points[0].y= drOutput.drY0;

    points[1].x= drOutput.drX0;
    points[1].y= drOutput.drY1;

    points[2].x= drOutput.drX1;
    points[2].y= drOutput.drY1;

    points[3].x= drOutput.drX1;
    points[3].y= drOutput.drY0;

    if  ( (*dc->dcDrawPolyPolygon)( dc, through, 1, &pointCount, points,
						fillInside, drawBorder, 1 ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

int appWinMetaDrawEllipse(	DeviceContext *			dc,
				void *				through,
				const DocumentRectangle *	drLogical,
				int				fillInside,
				int				drawBorder )
    {
    DocumentRectangle		drOutput;

    appWinMetaTransformRectangle( &drOutput, dc, drLogical );

    if  ( (*dc->dcDrawEllipse)( dc, through, &drOutput,
						    fillInside, drawBorder ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

void appWinMetaUpdateTransform(	DeviceContext *			dc )
    {
    DeviceCoordinateTransform *	t= &(dc->dcTransform);

    if  ( geoAffineTransformForTriangles( &(t->dctLogicalToOutput),
		    t->dctLogicalRect.drX0,	t->dctLogicalRect.drY0,
		    t->dctLogicalRect.drX1,	t->dctLogicalRect.drY0,
		    t->dctLogicalRect.drX0,	t->dctLogicalRect.drY1,

		    dc->dcOutputRect.drX0,	dc->dcOutputRect.drY0,
		    dc->dcOutputRect.drX1,	dc->dcOutputRect.drY0,
		    dc->dcOutputRect.drX0,	dc->dcOutputRect.drY1 ) )
	{ 
	RECTDEB(&(t->dctLogicalRect));
	RECTDEB(&(dc->dcOutputRect));
	}

    return;
    }

int appWinMetaOutputSize(	DeviceContext *		dc,
				int			logicalSize )
    {
    DocumentRectangle	drLogical;
    DocumentRectangle	drOutput;
    int			w, h;

    drLogical.drX0= 0;
    drLogical.drY0= 0;
    drLogical.drX1= logicalSize;
    drLogical.drY1= logicalSize;

    drOutput.drX0= DC_xViewport( drLogical.drX0, drLogical.drY0, dc );
    drOutput.drY0= DC_yViewport( drLogical.drX0, drLogical.drY0, dc );
    drOutput.drX1= DC_xViewport( drLogical.drX1, drLogical.drY1, dc );
    drOutput.drY1= DC_yViewport( drLogical.drX1, drLogical.drY1, dc );

    w= drOutput.drX1- drOutput.drX0;
    h= drOutput.drY1- drOutput.drY0;

    return M_SQRT1_2* sqrt( w*w+ h*h )+ 0.4999;
    }
