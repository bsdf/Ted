/************************************************************************/
/*									*/
/*  Open/Close objects inserted in the document.			*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>

#   include	<sioMemory.h>
#   include	<sioHex.h>
#   include	<bmio.h>

#   include	"tedApp.h"
#   include	"docDraw.h"
#   include	"tedDraw.h"
#   include	"tedLayout.h"
#   include	<docScreenLayout.h>
#   include	<drawMetafile.h>
#   include	<appWinMetaX11.h>
#   include	<docMetafileObject.h>
#   include	<docObjectProperties.h>
#   include	<docTreeNode.h>
#   include	<docTextParticule.h>
#   include	<docShape.h>

#   include	<docLayout.h>
#   include	<geoGrid.h>

#   include	<appDebugon.h>

#   define	DRAW_EPSF	0

#   if DRAW_EPSF
#	include	<X11/Xlib.h>
#   endif

/************************************************************************/
/*									*/
/*  Clip and draw the visible part of a pixmap.				*/
/*									*/
/*  X11 can get in serious trouble if we draw to negative coordinates	*/
/*  so really avoid this. To hide the similarity to MS-Windows we say	*/
/*  this is an optimization.						*/
/*									*/
/*  1)  Destination rectangle.						*/
/*  2)  Clip if needed.							*/
/*  3)  Shift by scrollbars.						*/
/*  4)  Do not draw what protrudes on the top and left sides of the	*/
/*	destination. (Clipping usually takes care of right/bottom).	*/
/*  5)  Draw if something left. The source coordinates reflect what	*/
/*	has been cut off on the top and left by clipping.		*/
/*									*/
/************************************************************************/

static void tedDrawObjectPixmap( const InsertedObject *		io,
				int				x0,
				int				y0,
				const DocumentRectangle *	drClip,
				const LayoutContext *		lc )
    {
    DocumentRectangle	drSrc;
    int			destX0;
    int			destY0;

    /*  1  */
    drSrc.drX0= 0;
    drSrc.drY0= 0;
    drSrc.drX1= io->ioPixelsWide- 1;
    drSrc.drY1= io->ioPixelsHigh- 1;

    /*  2  */
    if  ( drClip )
	{
	DocumentRectangle	drClipSrc= *drClip;

	geoShiftRectangle( &drClipSrc, -x0, -y0 );

	if  ( ! geoIntersectRectangle( &drSrc, &drSrc, &drClipSrc ) )
	    { return;	}
	}

    destX0= x0+ drSrc.drX0- lc->lcOx;
    destY0= y0+ drSrc.drY0- lc->lcOy;

    /*  5  */
    drawChildSurface( lc->lcDrawingSurface, io->ioDrawingSurface,
						destX0, destY0, &drSrc );
    }

/************************************************************************/
/*									*/
/*  Draw an object.							*/
/*									*/
/************************************************************************/

int tedDrawObject(	const DrawTextLine *		dtl,
			int				part,
			InsertedObject *		io,
			int				x0Twips,
			int				x1Twips,
			const LayoutPosition *		baseLine )
    {
    DrawingContext *		dc= dtl->dtlDrawingContext;
    const DocumentRectangle *	drClip= dc->dcClipRect;
    const LayoutContext *	lc= &(dc->dcLayoutContext);
    int				x0;
    int				y0;
    int				baselinePixels;
    int				x0FrameShifted;
    const BufferItem *		paraBi= dtl->dtlParaNode;
    const TextParticule *	tp= paraBi->biParaParticules+ part;

    DocumentRectangle		drObject;

    baselinePixels= docLayoutYPixels( lc, baseLine );
    x0FrameShifted= docLayoutXPixels( lc,
	    dtl->dtlParagraphFrame->pfParaContentRect.drX0+ dtl->dtlXShift );

    x0= x0FrameShifted+ tp->tpXContentXPixels;
    y0= baselinePixels- io->ioPixelsHigh;

    if  ( ! io->ioInline )
	{
	x0= docLayoutXPixels( lc, io->ioX0Twips );
	y0= docLayoutYPixels( lc, &(io->ioY0Position) );
	}

    if  ( io->ioKind == DOCokDRAWING_SHAPE )
	{ return 0;	}

    if  ( io->ioDrawingSurface				&&
	  ( io->ioKind == DOCokPICTWMETAFILE	||
	    io->ioKind == DOCokPICTEMFBLIP	||
	    io->ioKind == DOCokMACPICT		||
	    io->ioKind == DOCokPICTPNGBLIP	||
	    io->ioKind == DOCokPICTJPEGBLIP	)	)
	{
	tedDrawObjectPixmap( io, x0, y0, drClip, lc );
	return 0;
	}

    if  ( io->ioKind == DOCokOLEOBJECT				&&
	  io->ioDrawingSurface					&&
	  ( io->ioResultKind == DOCokPICTWMETAFILE	||
	    io->ioResultKind == DOCokPICTEMFBLIP	||
	    io->ioResultKind == DOCokMACPICT		||
	    io->ioResultKind == DOCokPICTPNGBLIP	||
	    io->ioResultKind == DOCokPICTJPEGBLIP	)	)
	{
	tedDrawObjectPixmap( io, x0, y0, drClip, lc );
	return 0;
	}

    if  ( io->ioKind == DOCokEPS_FILE				&&
	  io->ioDrawingSurface					)
	{
	tedDrawObjectPixmap( io, x0, y0, drClip, lc );
	return 0;
	}

    drObject.drX0= x0;
    drObject.drY0= baselinePixels- io->ioPixelsHigh;
    drObject.drX1= drObject.drX0+ io->ioPixelsWide- 1;
    drObject.drY1= drObject.drY0+ io->ioPixelsHigh- 1;

    drawSetLineAttributes( lc->lcDrawingSurface,
			    1, LineStyleSolid, LineCapButt, LineJoinMiter,
			    (const unsigned char *)0, 0 );

    geoShiftRectangle( &drObject, -lc->lcOx, -lc->lcOy );
    drawRectangle( lc->lcDrawingSurface, &drObject );

    return 0;
    }

static int docOpenMetafileObject(
				DrawingSurface *		pDs,
				RasterImage *			ri,
				const PictureProperties *	pip,
				int				pixelsWide,
				int				pixelsHigh,
				MetafilePlayX11			playMetaX11,
				const LayoutContext *		lc,
				const MemoryBuffer *		mb )
    {
    int			rval= 0;
    MetafilePlayer	mp;

    SimpleInputStream *	sisMem= (SimpleInputStream *)0;
    SimpleInputStream *	sisMeta= (SimpleInputStream *)0;

    void *		ownData= (void *)0;
    DrawingSurface	ds= (DrawingSurface)0;

    sisMem= sioInMemoryOpen( mb );
    if  ( ! sisMem )
	{ XDEB(sisMem); rval= -1; goto ready;	}

    sisMeta= sioInHexOpen( sisMem );
    if  ( ! sisMeta )
	{ XDEB(sisMeta); rval= -1; goto ready;	}

    docSetMetafilePlayer( &mp, sisMeta, lc, pip, pixelsWide, pixelsHigh );

    ds= drawMakeDrawingSurfaceForParent( lc->lcDrawingSurface,
						    pixelsWide, pixelsHigh );
    if  ( ! ds )
	{ XDEB(ds); rval= -1; goto ready;	}

    if  ( (*playMetaX11)( &ownData, ds, &mp ) )
	{ LDEB(1);	}

    if  ( ownData )
	{
XDEB(ownData); SDEB("IS PLAATJE");
	}
    *pDs= ds;

  ready:

    if  ( sisMeta )
	{ sioInClose( sisMeta );	}
    if  ( sisMem )
	{ sioInClose( sisMem );		}

    return rval;
    }

static int docOpenBitmapObject( DrawingSurface *		pSp,
				RasterImage *			ri,
				int				pixelsWide,
				int				pixelsHigh,
				bmReadBitmap			readBitmap,
				const LayoutContext *		lc,
				const MemoryBuffer *		mb )
    {
    int			rval= 0;
    SimpleInputStream *	sisMem= (SimpleInputStream *)0;
    SimpleInputStream *	sisBitmap= (SimpleInputStream *)0;

    DrawingSurface	ds;

    if  ( ! ri->riBytes )
	{
	sisMem= sioInMemoryOpen( mb );
	if  ( ! sisMem )
	    { XDEB(sisMem); rval= -1; goto ready;	}

	sisBitmap= sioInHexOpen( sisMem );
	if  ( ! sisBitmap )
	    { XDEB(sisBitmap); rval= -1; goto ready;	}

	if  ( (*readBitmap)( &(ri->riDescription), &(ri->riBytes), sisBitmap ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}

    ds= drawMakeDrawingSurfaceForImageAndParent( lc->lcDrawingSurface,
		ri, (const DocumentRectangle *)0, pixelsWide, pixelsHigh );
    if  ( ! ds )
	{ LLPDEB(pixelsWide,pixelsHigh,ds); rval= -1; goto ready; }

    *pSp= ds;

  ready:

    if  ( sisBitmap )
	{ sioInClose( sisBitmap );	}
    if  ( sisMem )
	{ sioInClose( sisMem );	}

    return rval;
    }

static int tedOpenPixmap(	DrawingSurface *		pSp,
				RasterImage *			ri,
				const PictureProperties *	pip,
				int				kind,
				int				pixelsWide,
				int				pixelsHigh,
				const LayoutContext *		lc,
				const MemoryBuffer *		mb )
    {
    switch( kind )
	{
	case DOCokPICTWMETAFILE:
	    if  ( docOpenMetafileObject( pSp, ri, pip,
					pixelsWide, pixelsHigh,
					appMetaPlayWmfX11, lc, mb ) )
		{ LDEB(1); return 0;	}

	    return 0;

	case DOCokPICTEMFBLIP:
	    if  ( docOpenMetafileObject( pSp, ri, pip,
					pixelsWide, pixelsHigh,
					appMetaPlayEmfX11, lc, mb ) )
		{ LDEB(1); return 0;	}
	    return 0;

	case DOCokMACPICT:
	    if  ( docOpenMetafileObject( pSp, ri, pip,
					pixelsWide, pixelsHigh,
					appMacPictPlayFileX11, lc, mb ) )
		{ LDEB(1); return 0;	}
	    return 0;

	case DOCokPICTPNGBLIP:
	    if  ( docOpenBitmapObject( pSp, ri,
					pixelsWide, pixelsHigh,
					bmPngReadPng, lc, mb )	)
		{ LDEB(1); return 0;	}
	    return 0;

	case DOCokPICTJPEGBLIP:
	    if  ( docOpenBitmapObject( pSp, ri,
					pixelsWide, pixelsHigh,
					bmJpegReadJfif, lc, mb ) )
		{ LDEB(1); return 0;	}
	    return 0;

	default:
	    LDEB(kind); return 0;
	}
    }

static int tedOpenEpsfObject(		const LayoutContext *	lc,
					InsertedObject *	io,
					const MemoryBuffer *	mb )
    {
#   if DRAW_EPSF
    SimpleInputStream *		sisMem;
    SimpleInputStream *		sisEps;
    SimpleOutputStream *	sosGs;

    char			command[1000];

    static Atom			XA_GHOSTVIEW= None;

    if  ( XA_GHOSTVIEW == None )
	{
	XA_GHOSTVIEW= XInternAtom( add->addDisplay, "GHOSTVIEW", False );
	if  ( XA_GHOSTVIEW == None )
	    { XDEB(XA_GHOSTVIEW); return -1;	}
	}

    io->ioDrawingSurface= appMakePixmap( add,
				    io->ioPixelsWide, io->ioPixelsHigh );
    if  ( ! io->ioDrawingSurface )
	{ XDEB(io->ioDrawingSurface); return -1;	}

    sprintf( command, "0 0 %d %d %d %d %f %f",
				/* llx	*/ 0,
				/* lly	*/ 0,
				/* urx	*/ io->ioTwipsWide/ 20,
				/* ury	*/ io->ioTwipsHigh/ 20,
				/* xdpi	*/ 72.0,
				/* ydpi	*/ 72.0 );

    XChangeProperty( add->addDisplay, add->addDrawable,
			XA_GHOSTVIEW, XA_STRING, 8,
			PropModeReplace,
			(unsigned char *)command,
			strlen(command) );

    XSync( add->addDisplay, False );

    sprintf( command,
	    "GHOSTVIEW='%ld %ld' gs -dQUIET -sDEVICE=x11 -dNOPAUSE -c quit -",
	    add->addDrawable, io->ioDrawingSurface );
    sosGs= sioOutPipeOpen( command );

    sisMem= sioInMemoryOpen( mb );
    if  ( ! sisMem )
	{ XDEB(sisMem); return -1;	}

    sisEps= sioInHexOpen( sisMem );
    if  ( ! sisEps )
	{ XDEB(sisEps); return -1;	}

    for (;;)
	{
	int		got;

	got= sioInReadBytes( sisEps,
				(unsigned char *)command, sizeof( command ) );
	if  ( got < 1 )
	    { break;	}
	if  ( sioOutWriteBytes( sosGs, (unsigned char *)command, got ) != got )
	    { LDEB(got); break;	}
	}

    sioInClose( sisEps );
    sioInClose( sisMem );

    sioOutPrintf( sosGs, " showpage\n" );
    sioOutClose( sosGs );

#   endif
    return 0;
    }

static int tedOpenShapePixmaps(	DrawingShape *			ds,
				const DocumentRectangle *	drTwips,
				const LayoutContext *		lc )
    {
    double			xfac= lc->lcPixelsPerTwip;

    int				rval= 0;
    const PictureProperties *	pip= &(ds->dsPictureProperties);
    int				pixelsWide;
    int				pixelsHigh;

    pixelsWide= docLayoutXPixels( lc, drTwips->drX1 )-
		docLayoutXPixels( lc, drTwips->drX0 )+ 1;
    pixelsHigh= COORDtoGRID( xfac, drTwips->drY1 )-
		COORDtoGRID( xfac, drTwips->drY0 )+ 1;

    if  ( ds->dsChildCount > 0 )
	{
	int			child;

	DocumentRectangle	drHere;
	DocumentRectangle	drNorm;

	docShapeGetRects( &drHere, &drNorm, drTwips, ds );

	for ( child= 0; child < ds->dsChildCount; child++ )
	    {
	    DrawingShape *	dsChild= ds->dsChildren[child];
	    DocumentRectangle	drChild;

	    docShapeGetChildRect( &drChild, dsChild, &drHere, ds );

	    if  ( tedOpenShapePixmaps( dsChild, &drChild, lc ) )
		{ LDEB(child); rval= -1;	}
	    }
	}

    switch( pip->pipType )
	{
	case DOCokUNKNOWN:
	    break;

	case DOCokPICTWMETAFILE:
	case DOCokPICTEMFBLIP:
	case DOCokMACPICT:
	case DOCokPICTPNGBLIP:
	case DOCokPICTJPEGBLIP:
	    if  ( pixelsWide == 0 || pixelsHigh == 0 )
		{ LLDEB(pixelsWide,pixelsHigh); break;	}

	    if  ( ! ds->dsDrawingSurface				&&
		  tedOpenPixmap( &(ds->dsDrawingSurface), &(ds->dsRasterImage),
				    pip, pip->pipType, pixelsWide, pixelsHigh,
				    lc, &(ds->dsPictureData) )	)
		{ LDEB(1); rval= -1; break;	}
	    break;

	default:
	    SDEB(docObjectKindStr(pip->pipType)); rval= -1; break;
	}

    if  ( ds->dsDocumentTree.dtRoot )
	{
	if  ( tedOpenTreeObjects( &(ds->dsDocumentTree), lc ) )
	    { LDEB(1); rval= -1;	}
	}

    return rval;
    }

int tedOpenObject(	InsertedObject *	io,
			const LayoutContext *	lc )
    {
    PictureProperties *	pip= &(io->ioPictureProperties);

    if  ( io->ioKind != DOCokDRAWING_SHAPE )
	{
	if  ( io->ioTwipsWide < 1 || io->ioTwipsHigh < 1 )
	    { LLLDEB(io->ioKind,io->ioTwipsWide,io->ioTwipsHigh); return -1; }
	}

    if  ( io->ioPixelsWide == 0 || io->ioPixelsHigh == 0 )
	{ LLDEB(io->ioPixelsWide,io->ioPixelsHigh); return 0;	}

    switch( io->ioKind )
	{
	case DOCokPICTWMETAFILE:
	case DOCokPICTEMFBLIP:
	    if  ( ! io->ioDrawingSurface				&&
		  tedOpenPixmap( &(io->ioDrawingSurface),
			    &(io->ioRasterImage), pip, io->ioKind,
			    io->ioPixelsWide, io->ioPixelsHigh,
			    lc, &(io->ioObjectData) )	)
		{ LDEB(1); return 0;	}

	    if  ( io->ioRasterImage.riBytes )
		{ io->ioPictureProperties.pipMetafileIsBitmap= 1;	}

	    return 0;

	case DOCokMACPICT:
	case DOCokPICTPNGBLIP:
	case DOCokPICTJPEGBLIP:
	    if  ( ! io->ioDrawingSurface				&&
		  tedOpenPixmap( &(io->ioDrawingSurface),
			    &(io->ioRasterImage), pip, io->ioKind,
			    io->ioPixelsWide, io->ioPixelsHigh,
			    lc, &(io->ioObjectData) )	)
		{ LDEB(1); return 0;	}
	    return 0;

	case DOCokOLEOBJECT:
	    if  ( io->ioResultKind == DOCokPICTWMETAFILE	||
		  io->ioResultKind == DOCokPICTEMFBLIP		)
		{
		if  ( ! io->ioDrawingSurface				&&
		      tedOpenPixmap( &(io->ioDrawingSurface),
					&(io->ioRasterImage),
					pip, io->ioResultKind,
					io->ioPixelsWide, io->ioPixelsHigh,
					lc, &(io->ioResultData) )	)
		    { LDEB(1); return 0;	}

		if  ( io->ioRasterImage.riBytes )
		    { io->ioPictureProperties.pipMetafileIsBitmap= 1;	}

		return 0;
		}

	    if  ( io->ioResultKind == DOCokMACPICT	||
		  io->ioResultKind == DOCokPICTPNGBLIP	||
		  io->ioResultKind == DOCokPICTJPEGBLIP	)
		{
		if  ( ! io->ioDrawingSurface				&&
		      tedOpenPixmap( &(io->ioDrawingSurface),
					&(io->ioRasterImage),
					pip, io->ioResultKind,
					io->ioPixelsWide, io->ioPixelsHigh,
					lc, &(io->ioResultData) )	)
		    { LDEB(1); return 0;	}
		}

	    return 0;

	case DOCokEPS_FILE:
	    if  ( tedOpenEpsfObject( lc, io, &io->ioResultData ) )
		{ LDEB(1); return 0;	}

	    return 0;

	case DOCokDRAWING_SHAPE:
	    {
	    DrawingShape *		ds= io->ioDrawingShape;

	    if  ( ! ds )
		{ XDEB(ds); return 0;	}

	    if  ( tedOpenShapePixmaps( ds,
				&(ds->dsShapeProperties.spRect), lc ) )
		{ LDEB(1); return 0;	}
	    }
	    return 0;

	default:
	    SDEB(docObjectKindStr(io->ioKind)); return 0;
	}
    }

static int tedOpenParaObjects(	BufferItem *		bi,
				const LayoutContext *	lc )
    {
    int			part;
    TextParticule *	tp;

    tp= bi->biParaParticules;
    for ( part= 0; part < bi->biParaParticuleCount; tp++, part++ )
	{
	InsertedObject *	io;

	if  ( tp->tpKind != DOCkindOBJECT )
	    { continue;	}

	io= docGetObject( lc->lcDocument, tp->tpObjectNumber );
	if  ( ! io )
	    { LPDEB(tp->tpObjectNumber,io); continue;	}

	if  ( tedOpenObject( io, lc )	)
	    { LDEB(part); continue;	}
	}

    return 0;
    }

int tedOpenTreeObjects(	DocumentTree *		ei,
			const LayoutContext *	lc )
    {
    if  ( ei->dtRoot				&&
	  tedOpenNodeObjects( ei->dtRoot, lc )	)
	{ LDEB(1); return -1;	}

    return 0;
    }

int tedOpenNodeObjects(	BufferItem *		bi,
			const LayoutContext *	lc )
    {
    int			i;
    BufferDocument *	bd= lc->lcDocument;

    switch( bi->biLevel )
	{
	case DOClevBODY:

	    if  ( tedOpenTreeObjects( &(bd->bdEiFtnsep), lc ) )
		{ LDEB(1); return -1;	}
	    if  ( tedOpenTreeObjects( &(bd->bdEiFtnsepc), lc ) )
		{ LDEB(1); return -1;	}
	    if  ( tedOpenTreeObjects( &(bd->bdEiFtncn), lc ) )
		{ LDEB(1); return -1;	}

	    if  ( tedOpenTreeObjects( &(bd->bdEiAftnsep), lc ) )
		{ LDEB(1); return -1;	}
	    if  ( tedOpenTreeObjects( &(bd->bdEiAftnsepc), lc ) )
		{ LDEB(1); return -1;	}
	    if  ( tedOpenTreeObjects( &(bd->bdEiAftncn), lc ) )
		{ LDEB(1); return -1;	}

	    break;

	case DOClevSECT:
	    {
	    SectHeadersFooters *	shf= bi->biSectHeadersFooters;

	    if  ( shf )
		{
		if  ( tedOpenTreeObjects( &(shf->shfFirstPageHeader), lc ) )
		    { LDEB(1); return -1;	}
		if  ( tedOpenTreeObjects( &(shf->shfLeftPageHeader), lc ) )
		    { LDEB(1); return -1;	}
		if  ( tedOpenTreeObjects( &(shf->shfRightPageHeader), lc ) )
		    { LDEB(1); return -1;	}

		if  ( tedOpenTreeObjects( &(shf->shfFirstPageFooter), lc ) )
		    { LDEB(1); return -1;	}
		if  ( tedOpenTreeObjects( &(shf->shfLeftPageFooter), lc ) )
		    { LDEB(1); return -1;	}
		if  ( tedOpenTreeObjects( &(shf->shfRightPageFooter), lc ) )
		    { LDEB(1); return -1;	}
		}
	    }

	    break;

	case DOClevROW:
	case DOClevCELL:
	    break;

	case DOClevPARA:
	    if  ( tedOpenParaObjects( bi, lc ) )
		{ LDEB(0); return -1;	}
	    break;

	default:
	    LDEB(bi->biLevel); return -1;
	}

    for ( i= 0; i < bi->biChildCount; i++ )
	{
	if  ( tedOpenNodeObjects( bi->biChildren[i], lc ) )
	    { LDEB(i); return -1;	}
	}

    return 0;
    }

static void tedCloseDrawingShape(	DrawingShape *		ds )
    {
    int		child;

    for ( child= 0; child < ds->dsChildCount; child++ )
	{
	DrawingShape *	dsChild= ds->dsChildren[child];

	tedCloseDrawingShape( dsChild );
	}

    if  ( ds->dsDrawingSurface )
	{
	drawFreeDrawingSurface( ds->dsDrawingSurface );
	ds->dsDrawingSurface= (DrawingSurface)0;
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Called to resize an inserted object.				*/
/*									*/
/************************************************************************/

int tedReopenObject(	TextParticule *		tp,
			const LayoutContext *	lc )
    {
    InsertedObject *		io;

    io= docGetObject( lc->lcDocument, tp->tpObjectNumber );
    if  ( ! io )
	{ LPDEB(tp->tpObjectNumber,io); return -1;	}

    if  ( lc->lcCloseObject )
	{ (*lc->lcCloseObject)( lc->lcDocument, tp );	}

    if  ( tedOpenObject( io, lc ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Save the picture of an object for Copy/Paste.			*/
/*									*/
/************************************************************************/

int tedSaveObjectPicture(	RasterImage *		abiTo,
				const InsertedObject *	io )
    {
    if  ( ! io->ioRasterImage.riBytes )
	{ LXDEB(io->ioKind,io->ioRasterImage.riBytes); return -1;	}

    if  ( bmCopyRasterImage( abiTo, &(io->ioRasterImage) ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

void docScreenCloseObject(	const BufferDocument *	bd,
				const TextParticule *	tp )
    {
    InsertedObject *	io;

    if  ( tp->tpKind != DOCkindOBJECT )
	{ return;	}

    io= docGetObject( bd, tp->tpObjectNumber );
    if  ( ! io )
	{ LPDEB(tp->tpObjectNumber,io); return;	}

    switch( io->ioKind )
	{
	case DOCokPICTWMETAFILE:
	case DOCokPICTEMFBLIP:
	case DOCokPICTPNGBLIP:
	case DOCokPICTJPEGBLIP:
	case DOCokOLEOBJECT:
	case DOCokMACPICT:
	case DOCokEPS_FILE:
	    if  ( io->ioDrawingSurface )
		{
		drawFreeDrawingSurface( io->ioDrawingSurface );
		io->ioDrawingSurface= (DrawingSurface)0;
		}
	    break;

	case DOCokDRAWING_SHAPE:
	    tedCloseDrawingShape( io->ioDrawingShape );
	    break;

	default:
	    LDEB(io->ioKind); break;
	}

    return;
    }

