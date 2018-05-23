/************************************************************************/
/*									*/
/*  Print images to PostScript.						*/
/*									*/
/************************************************************************/

#   include	"docLayoutConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>

#   include	<sioMemory.h>
#   include	<sioHex.h>

#   include	"docDraw.h"
#   include	"docPsPrintImpl.h"
#   include	<drawMetafilePs.h>
#   include	<docShape.h>
#   include	<docObjectProperties.h>
#   include	<bmBitmapPrinter.h>
#   include	"docMetafileObject.h"
#   include	"docLayoutObject.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Print a metafile.							*/
/*									*/
/************************************************************************/

static int docPsPrintMetafile(	PrintingState *			ps,
				const PictureProperties *	pip,
				const MemoryBuffer *		mb,
				int				objectKind,
				const LayoutContext *		lc,
				int				x0,
				int				baseline )
    {
    int				rval= 0;
    int				scaleX= pip->pipScaleXUsed;
    int				scaleY= pip->pipScaleYUsed;

    SimpleInputStream *		sisMem= (SimpleInputStream *)0;
    SimpleInputStream *		sisMeta= (SimpleInputStream *)0;

    PostScriptTypeList		pstl;

    int				y0;

    MetafilePlayer		mp;
    MetafileWritePs		playMetafile;

    psInitPostScriptFaceList( &pstl );

    if  ( docPsListImageFonts( &pstl, pip, mb, lc, "pf" ) )
	{ LDEB(1); rval= -1; goto ready;	}

    switch( objectKind )
	{
	case DOCokPICTWMETAFILE:
	    playMetafile= appMetaPlayWmfPs;
	    break;

	case DOCokPICTEMFBLIP:
	    playMetafile= appMetaPlayEmfPs;
	    break;

	case DOCokMACPICT:
	    playMetafile= appMacPictPlayFilePs;
	    break;

	case DOCokPICTJPEGBLIP:
	case DOCokPICTPNGBLIP:
	default:
	    LDEB(pip->pipType); goto ready;
	}

    sisMem= sioInMemoryOpen( mb );
    if  ( ! sisMem )
	{ XDEB(sisMem); rval= -1; goto ready;	}

    sisMeta= sioInHexOpen( sisMem );
    if  ( ! sisMeta )
	{ XDEB(sisMeta); rval= -1; goto ready;	}

    docSetMetafilePlayer( &mp, sisMeta, lc, pip, 0, 0 );

    y0= baseline- ( ( scaleY/100.0 )* mp.mpTwipsHigh );

    sioOutPrintf( ps->psSos, "100 dict begin\n" );

# if 0
    /* First fix definition of fonts in images */
    psSelectFontProcedures( ps->psSos, &pstl, /*allFonts=*/ 1 );
# endif

    sioOutPrintf( ps->psSos, "gsave %d %d translate %%{IMG\n", x0, y0 );

    if  ( scaleX != 100 || scaleY != 100 )
	{
	sioOutPrintf( ps->psSos, "%f %f scale\n", scaleX/100.0, scaleY/100.0 );
	}

    sioOutPrintf( ps->psSos,
		    "0 0 bp %d 0 rl 0 %d rl %d 0 rl 0 %d rl closepath clip\n",
		    mp.mpTwipsWide, mp.mpTwipsHigh,
		    -mp.mpTwipsWide, -mp.mpTwipsHigh );

    if  ( (*playMetafile)( ps, &mp ) )
	{ LDEB(1);	}

    sioOutPrintf( ps->psSos, "grestore end %%}IMG\n" );

    ps->psLastPageMarked= ps->psPagesPrinted;

  ready:
    if  ( sisMeta )
	{ sioInClose( sisMeta );	}
    if  ( sisMem )
	{ sioInClose( sisMem );	}

    psCleanPostScriptFaceList( &pstl );

    return rval;
    }

static int psPrintIncludeEpsObject(	PrintingState *		ps,
					InsertedObject *	io,
					int			x0,
					int			baseLine )
    {
    int				rval= 0;
    SimpleInputStream *		sisMem= (SimpleInputStream *)0;
    SimpleInputStream *		sisHex= (SimpleInputStream *)0;

    DocumentRectangle		drTo;
    DocumentRectangle		drBBox;

    sisMem= sioInMemoryOpen( &(io->ioResultData) );
    if  ( ! sisMem )
	{ XDEB(sisMem); rval= -1; goto ready;	}

    sisHex= sioInHexOpen( sisMem );
    if  ( ! sisHex )
	{ XDEB(sisHex); rval= -1; goto ready;	}

    drBBox.drX0= 0;
    drBBox.drY0= 0;
    drBBox.drX1= io->ioTwipsWide;
    drBBox.drY1= io->ioTwipsHigh;

    drTo.drX0= x0;
    drTo.drY0= baseLine;
    drTo.drX1= drTo.drX0+ ( io->ioScaleXUsed/ 100.0 )* 20* io->ioTwipsWide;
    drTo.drY1= drTo.drY0- ( io->ioScaleYUsed/ 100.0 )* 20* io->ioTwipsHigh;

    psBeginEpsObject( ps->psSos, &drTo, &drBBox,
			utilMemoryBufferGetString( &(io->ioObjectData) ) );

    if  ( psIncludeEpsFile( ps->psSos, sisHex ) )
	{ LDEB(1); rval= -1;	}

    psEndEpsObject( ps->psSos );

    ps->psLastPageMarked= ps->psPagesPrinted;

  ready:

    if  ( sisHex )
	{ sioInClose( sisHex );	}
    if  ( sisMem )
	{ sioInClose( sisMem );	}

    return rval;
    }

static void psPrintObjectBox(	DrawingContext *	dc,
				PrintingState *		ps,
				const InsertedObject *	io,
				int			x0,
				int			y0 )
    {
    int		high;
    int		wide;

    RGB8Color	rgb8;

    utilRGB8SolidBlack( &rgb8 );

    docDrawSetColorRgb( dc, (void *)ps, &rgb8 );

    high= ( io->ioScaleYUsed* io->ioTwipsHigh )/ 100;
    wide= ( io->ioScaleXUsed* io->ioTwipsWide )/ 100;

    sioOutPrintf( ps->psSos, "%d %d bp ", x0, y0- high );
    sioOutPrintf( ps->psSos, "%d 0 rl ", wide );
    sioOutPrintf( ps->psSos, "0 %d rl ", high );
    sioOutPrintf( ps->psSos, "%d 0 rl ", -wide );
    sioOutPrintf( ps->psSos, "closepath stroke\n" );

    ps->psLastPageMarked= ps->psPagesPrinted;
    return;
    }

/************************************************************************/
/*									*/
/*  Print a bitmap image included in the document.			*/
/*									*/
/************************************************************************/

static int docPsPrintBitmapObject( PrintingState *		ps,
				int				x0,
				int				baseLine,
				const RasterImage *		abi,
				const PictureProperties *	pip )
    {
    const BitmapDescription *	bd;

    double		scaleX= pip->pipScaleXUsed/ 100.0;
    double		scaleY= pip->pipScaleYUsed/ 100.0;

    int			imageWideTwips;
    int			imageHighTwips;

    DocumentRectangle	drSel;

    bd= &(abi->riDescription);

    bmImageSizeTwips( &imageWideTwips, &imageHighTwips, bd );

    if  ( imageWideTwips > 20 )
	{ scaleX= ( scaleX* pip->pipTwipsWide )/ imageWideTwips;	}
    if  ( imageHighTwips > 20 )
	{ scaleY= ( scaleY* pip->pipTwipsHigh )/ imageHighTwips;	}

    docObjectGetCropRect( &drSel, pip, bd );

    if  ( bmPsPrintBitmap( ps->psSos, 1,
			    20.0* scaleX, -20.0* scaleY,
			    x0, baseLine, (const DocumentRectangle *)0,
			    ps->psUsePostScriptFilters,
			    ps->psUsePostScriptIndexedImages,
			    bd, abi->riBytes ) )
	{ LDEB(1); return -1; }

    ps->psLastPageMarked= ps->psPagesPrinted;
    return 0;
    }

static int docPsPrintShapeBitmap(	int				kind,
					PrintingState *			ps,
					DrawingShape *			ds,
					const DocumentRectangle *	drTwips,
					const AffineTransform2D *	at )
    {
    const PictureProperties *	pip= &(ds->dsPictureProperties);
    int				x0= 0;
    int				y0= 0;

    if  ( ! ds->dsRasterImage.riBytes )
	{
	if  ( docGetBitmapForObjectData( kind,
				&(ds->dsRasterImage), &(ds->dsPictureData) ) )
	    { XDEB(ds->dsRasterImage.riBytes);	}
	}

    if  ( ds->dsRasterImage.riBytes )
	{
	AffineTransform2D		atLocal;

	DocumentRectangle		drSel;

	double				xs;
	double				ys;
	const int			onWhite= 0;

	/* Center image in frame */
	geoTranslationAffineTransform2D( &atLocal, -0.5, 0.5 );
	geoAffineTransform2DProduct( &atLocal, at, &atLocal );

	x0= AT2_X( 0, 0, &atLocal );
	y0= AT2_Y( 0, 0, &atLocal );

	docObjectGetCropRect( &drSel, pip, &(ds->dsRasterImage.riDescription) );

	xs= drTwips->drX1- drTwips->drX0+ 1;
	ys= drTwips->drY1- drTwips->drY0+ 1;

	if  ( bmPsPrintBitmapImage( ps->psSos, 1, xs, -ys, x0, y0, &drSel,
					onWhite,
					ps->psUsePostScriptFilters,
					ps->psUsePostScriptIndexedImages,
					&(ds->dsRasterImage.riDescription),
					ds->dsRasterImage.riBytes ) )
	    { LDEB(1); return -1; }
	}

    ps->psLastPageMarked= ps->psPagesPrinted;
    return 0;
    }

int docPsPrintShapeImage(	PrintingState *			ps,
				DrawingContext *		dc,
				DrawingShape *			ds,
				const DocumentRectangle *	drTwips,
				const AffineTransform2D *	at )
    {
    const PictureProperties *	pip= &(ds->dsPictureProperties);
    const LayoutContext *	lc= &(dc->dcLayoutContext);

    const int			x0= 0;
    const int			y0= 0;

    switch( pip->pipType )
	{
	case DOCokPICTWMETAFILE:
	case DOCokMACPICT:
	case DOCokPICTEMFBLIP:
	    if  ( docPsPrintMetafile( ps, pip, &(ds->dsPictureData),
						pip->pipType, lc, x0, y0 ) )
		{ LDEB(1); break;	}

	    dc->dcCurrentTextAttributeSet= 0;
	    dc->dcCurrentColorSet= 0;
	    return 0;

	case DOCokPICTPNGBLIP:
	case DOCokPICTJPEGBLIP:
	    if  ( docPsPrintShapeBitmap( pip->pipType, ps, ds, drTwips, at ) )
		{ LDEB(1); return -1;	}
	    break;

	default:
	    LDEB(pip->pipType); return 0;
	}

    return 0;
    }

static int docPsPrintJpegImage( PrintingState *			ps,
				int				x0,
				int				baseLine,
				InsertedObject *		io,
				MemoryBuffer *			mb )
    {
    int				rval= 0;

    unsigned int		bitsPerComponent;
    unsigned int		componentCount;
    unsigned int		pixelsWide;
    unsigned int		pixelsHigh;

    const PictureProperties *	pip= &(io->ioPictureProperties);

    SimpleInputStream *	sisMem= (SimpleInputStream *)0;
    SimpleInputStream *	sisBitmap= (SimpleInputStream *)0;

    double		scaleX= pip->pipScaleXUsed/ 100.0;
    double		scaleY= pip->pipScaleYUsed/ 100.0;

    sisMem= sioInMemoryOpen( mb );
    if  ( ! sisMem )
	{ XDEB(sisMem); rval= -1; goto ready;	}

    sisBitmap= sioInHexOpen( sisMem );
    if  ( ! sisBitmap )
	{ XDEB(sisBitmap); rval= -1; goto ready;	}

    if  ( bmEpsTestJpegEmbeddable( &pixelsWide, &pixelsHigh,
			    &componentCount, &bitsPerComponent, sisBitmap ) )
	{ rval= 1; goto ready;	}

    sioInClose( sisBitmap ); sisBitmap= (SimpleInputStream *)0;
    sioInClose( sisMem ); sisMem= (SimpleInputStream *)0;

    sisMem= sioInMemoryOpen( mb );
    if  ( ! sisMem )
	{ XDEB(sisMem); rval= -1; goto ready;	}

    sisBitmap= sioInHexOpen( sisMem );
    if  ( ! sisBitmap )
	{ XDEB(sisBitmap); rval= -1; goto ready;	}

    if  ( bmPsPrintJpegImage( ps->psSos,
			pip->pipTwipsWide* scaleX, -pip->pipTwipsHigh* scaleY,
			componentCount, x0, baseLine,
			pixelsWide, pixelsHigh, bitsPerComponent, sisBitmap ) )
	{ LDEB(1); rval= -1; goto ready;	}

    ps->psLastPageMarked= ps->psPagesPrinted;

  ready:

    if  ( sisBitmap )
	{ sioInClose( sisBitmap ); }
    if  ( sisMem )
	{ sioInClose( sisMem ); }

    return rval;
    }

static int docPsPrintBitmapImage(	PrintingState *		ps,
					DrawingContext *	dc,
					InsertedObject *	io,
					int			objectKind,
					MemoryBuffer *		mb,
					const int		x0Twips,
					int			baseLine )
    {
    if  ( ! io->ioRasterImage.riBytes )
	{
	if  ( docGetBitmapForObject( io ) )
	    { XDEB(io->ioRasterImage.riBytes);	}
	}

    if  ( io->ioRasterImage.riBytes )
	{
	if  ( ps->psUsePostScriptFilters	&&
	      objectKind == DOCokPICTJPEGBLIP	)
	    {
	    if  ( ! docPsPrintJpegImage( ps, x0Twips, baseLine, io, mb ) )
		{
		ps->psLinkParticulesDone++;
		return 1;
		}
	    }

	if  ( docPsPrintBitmapObject( ps, x0Twips, baseLine,
			&(io->ioRasterImage), &(io->ioPictureProperties) ) )
	    { LDEB(1); return -1;	}

	ps->psLinkParticulesDone++;
	return 1;
	}

    return 0;
    }

int docPsPrintObject(		const DrawTextLine *		dtl,
				int				part,
				InsertedObject *		io,
				const int			x0Twips,
				const int			x1Twips,
				const LayoutPosition *		baseLine )
    {
    PrintingState *		ps= (PrintingState *)dtl->dtlThrough;
    DrawingContext *		dc= dtl->dtlDrawingContext;
    const LayoutContext *	lc= &(dc->dcLayoutContext);
    const PictureProperties *	pip= &(io->ioPictureProperties);

    switch( io->ioKind )
	{
	int		done;

	case DOCokPICTWMETAFILE:
	case DOCokPICTEMFBLIP:
	case DOCokMACPICT:

	    if  ( docPsPrintMetafile( ps, pip, &(io->ioObjectData),
			    io->ioKind, lc, x0Twips, baseLine->lpPageYTwips ) )
		{ LDEB(1); break;	}

	    dc->dcCurrentTextAttributeSet= 0;
	    dc->dcCurrentColorSet= 0;
	    ps->psLinkParticulesDone++;
	    return 1;

	case DOCokPICTJPEGBLIP:
	case DOCokPICTPNGBLIP:

	    done= docPsPrintBitmapImage( ps, dc, io,
					io->ioKind, &(io->ioObjectData),
					x0Twips, baseLine->lpPageYTwips );
	    return done;
	    break;

	case DOCokOLEOBJECT:
	    if  ( io->ioResultKind == DOCokPICTWMETAFILE	||
		  io->ioResultKind == DOCokPICTEMFBLIP		||
		  io->ioResultKind == DOCokMACPICT		)
		{
		if  ( docPsPrintMetafile( ps, pip, &(io->ioResultData),
					io->ioResultKind,
					lc, x0Twips, baseLine->lpPageYTwips ) )
		    { LDEB(1); break;	}

		dc->dcCurrentTextAttributeSet= 0;
		dc->dcCurrentColorSet= 0;
		ps->psLinkParticulesDone++;
		return 1;
		}

	    if  ( io->ioResultKind == DOCokPICTJPEGBLIP	||
	    	  io->ioResultKind == DOCokPICTPNGBLIP	)
		{
		done= docPsPrintBitmapImage( ps, dc, io,
					io->ioResultKind, &(io->ioResultData),
					x0Twips, baseLine->lpPageYTwips );
		return done;
		}

	    break;

	case DOCokEPS_FILE:
	    if  ( psPrintIncludeEpsObject( ps, io,
					x0Twips, baseLine->lpPageYTwips ) )
		{ LDEB(1); break;	}

	    dc->dcCurrentTextAttributeSet= 0;
	    dc->dcCurrentColorSet= 0;
	    ps->psLinkParticulesDone++;
	    return 1;

	case DOCokDRAWING_SHAPE:
	    /*  Done in a separate loop from generic drawing code */
	    return 1;

	default:
	    LDEB(io->ioKind); return 0;
	}

    psPrintObjectBox( dc, ps, io, x0Twips, baseLine->lpPageYTwips );
    ps->psLinkParticulesDone++;
    return 1;
    }


