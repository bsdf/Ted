/************************************************************************/
/*									*/
/*  Save the images in a BufferDocument and include references in the	*/
/*  HTML file.								*/
/*									*/
/************************************************************************/

#   include	"docHtmlConfig.h"

#   include	<string.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	<bitmap.h>
#   include	<bmio.h>

#   include	<drawMetafileSvg.h>
#   include	<docMetafileObject.h>
#   include	<docLayoutObject.h>
#   include	"docSvgDrawImpl.h"

#   include	<sioGeneral.h>
#   include	<sioMemory.h>
#   include	<sioHex.h>

#   include	<docBuf.h>
#   include	<docPageGrid.h>
#   include	<docObjectRect.h>
#   include	<docShape.h>
#   include	"docHtmlWriteImpl.h"

#   include	<appDebugon.h>
#   include	<docObjectProperties.h>

/************************************************************************/

int docHtmlObjectSaveHow(	int *			pType,
				const char **		pMimeType,
				const char **		pExt,
				const MemoryBuffer **	pObjectData,
				const InsertedObject *	io )
    {
    const RasterImage *		abi= &(io->ioRasterImage);

    if  ( io->ioKind == DOCokPICTWMETAFILE	||
	  io->ioKind == DOCokPICTEMFBLIP	||
	  io->ioKind == DOCokMACPICT		)
	{
	*pType= io->ioKind;
	*pMimeType= "image/svg+xml";
	*pExt= "svg";
	*pObjectData= &(io->ioObjectData);
	return 0;
	}

    if  ( io->ioKind == DOCokPICTJPEGBLIP )
	{
	*pType= io->ioKind;
	*pMimeType= "image/jpeg";
	*pExt= "jpg";
	*pObjectData= &(io->ioObjectData);
	return 0;
	}

    if  ( io->ioKind == DOCokPICTPNGBLIP )
	{
	*pType= io->ioKind;
	*pMimeType= "image/png";
	*pExt= "png";
	*pObjectData= &(io->ioObjectData);
	return 0;
	}

    if  ( io->ioKind == DOCokOLEOBJECT )
	{
	if  ( io->ioResultKind == DOCokPICTWMETAFILE	||
	      io->ioResultKind == DOCokPICTEMFBLIP	||
	      io->ioResultKind == DOCokMACPICT		)
	    {
	    *pType= io->ioResultKind;
	    *pMimeType= "image/svg+xml";
	    *pExt= "svg";
	    *pObjectData= &(io->ioResultData);
	    return 0;
	    }

	if  ( io->ioResultKind == DOCokPICTJPEGBLIP )
	    {
	    *pType= io->ioResultKind;
	    *pMimeType= "image/jpeg";
	    *pExt= "jpg";
	    *pObjectData= &(io->ioResultData);
	    return 0;
	    }

	if  ( io->ioResultKind == DOCokPICTPNGBLIP )
	    {
	    *pType= io->ioResultKind;
	    *pMimeType= "image/png";
	    *pExt= "png";
	    *pObjectData= &(io->ioResultData);
	    return 0;
	    }
	}

    if  ( io->ioKind == DOCokDRAWING_SHAPE )
	{
	*pType= io->ioKind;
	*pMimeType= "image/svg+xml";
	*pExt= "svg";
	*pObjectData= (const MemoryBuffer *)0;
	return 0;
	}

    if  ( ! io->ioRasterImage.riBytes )
	{ return 1;	}

#   if  USE_PNG
    if  ( abi->riDescription.bdColorEncoding == BMcoRGB8PALETTE )
	{
	*pType= DOCokPICTPNGBLIP;
	*pMimeType= "image/png";
	*pExt= "png";
	*pObjectData= (const MemoryBuffer *)0;
	return 0;
	}
#   endif

#   if  USE_GIF
    if  ( ! bmCanWriteGifFile( &(abi->riDescription), 89 ) )
	{
	*pType= DOCokGIF_FILE;
	*pMimeType= "image/gif";
	*pExt= "gif";
	*pObjectData= (const MemoryBuffer *)0;
	return 0;
	}
#   endif

    if  ( ! bmCanWriteJpegFile( &(abi->riDescription), 0 ) )
	{
	*pType= DOCokPICTJPEGBLIP;
	*pMimeType= "image/jpeg";
	*pExt= "jpg";
	*pObjectData= (const MemoryBuffer *)0;
	return 0;
	}

    return 1;
    }

/************************************************************************/

static void docHtmlSetShapeLeft(	char *			style )
    {
    strcat( style, "float: left;" );
    strcat( style, "clear: left;" );
    strcat( style, "margin: 5px 5px 5px 0;" );
    }

static void docHtmlSetShapeRight(	char *			style )
    {
    strcat( style, "float: right;" );
    strcat( style, "clear: right;" );
    strcat( style, "margin: 5px 0 5px 5px;" );
    }

static void docHtmlSetShapeStyle(	char *				style,
					HtmlWritingContext *		hwc,
					const struct BufferItem *	paraNode,
					const InsertedObject *		io )
    {
    const DrawingShape *	ds= io->ioDrawingShape;
    const ShapeProperties *	sp= &(ds->dsShapeProperties);

    switch( sp->spWrapStyle )
	{
	case SHPswtTOPBOTTOM:
	    strcat( style, "clear: both;" );
	    break;

	case SHPswtAROUND:
	case SHPswtTIGHTLY:
	    switch( sp->spWrapSide )
		{
		case SHPswsBOTH:
		case SHPswsLARGEST:
		    {
		    int	px0;
		    int	px1;

		    docParagraphFrameTwips( &(hwc->hwcParagraphFrame),
					&(hwc->hwcBlockFrame), paraNode );

		    px0= hwc->hwcParagraphFrame.pfParaContentRect.drX0;
		    px1= hwc->hwcParagraphFrame.pfParaContentRect.drX1;

		    if  ( sp->spRect.drX0- px0 >= px1- sp->spRect.drX1 )
			{ docHtmlSetShapeRight( style );	}
		    else{ docHtmlSetShapeLeft( style );		}
		    }

		    break;

		case SHPswsLEFT:
		    docHtmlSetShapeRight( style );
		    break;

		case SHPswsRIGHT:
		    docHtmlSetShapeLeft( style );
		    break;

		default:
		    LLDEB(sp->spWrapStyle,sp->spWrapSide);
		    break;
		}
	    break;

	case SHPswtNONE:
	case SHPswtTHROUGH:
	    LDEB(sp->spWrapStyle);
	    break;

	default:
	    LDEB(sp->spWrapStyle);
	    break;
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Start an 'object' in the sense of the HTML specification.		*/
/*									*/
/************************************************************************/

static void docHtmlStartObject(	HtmlWritingContext *		hwc,
				const struct BufferItem *	paraNode,
				const InsertedObject *		io,
				int				wide,
				int				high,
				const char *			mimeType,
				const MemoryBuffer *		src )
    {
    wide= TWIPS_TO_PIXELS( ( io->ioScaleXSet* wide )/100 );
    high= TWIPS_TO_PIXELS( ( io->ioScaleYSet* high )/100 );

    docHtmlPutString( "<object", hwc );

    docHtmlWriteStringAttribute( hwc, "type", mimeType );
    docHtmlWriteStringAttribute( hwc, "data", utilMemoryBufferGetString( src ) );

    docHtmlWriteIntAttribute( hwc, "width", wide );
    docHtmlWriteIntAttribute( hwc, "height", high );

    if  ( io->ioKind == DOCokDRAWING_SHAPE )
	{
	char	style[200+1];

	style[0]= '\0';

	docHtmlSetShapeStyle( style, hwc, paraNode, io );

	if  ( style[0] )
	    { docHtmlWriteStringAttribute( hwc, "style", style );	}
	}

    docHtmlPutString( ">", hwc );
    docHtmlNewLine( hwc );
    }

/************************************************************************/
/*									*/
/*  Emit the 'img' element for an image.				*/
/*									*/
/************************************************************************/

static void docHtmlEmitImgElement(	HtmlWritingContext *		hwc,
					const struct BufferItem *	paraNode,
					const InsertedObject *		io,
					int				wide,
					int				high,
					const MemoryBuffer *		src )
    {
    docHtmlPutString( "<img", hwc );

    docHtmlWriteStringAttribute( hwc, "src", utilMemoryBufferGetString( src ) );

    docHtmlWriteIntAttribute( hwc, "width", wide );
    docHtmlWriteIntAttribute( hwc, "height", high );

    if  ( io->ioKind == DOCokDRAWING_SHAPE )
	{
	char	style[200+1];

	style[0]= '\0';

	docHtmlSetShapeStyle( style, hwc, paraNode, io );

	if  ( style[0] )
	    { docHtmlWriteStringAttribute( hwc, "style", style );	}
	}

    docHtmlWriteStringAttribute( hwc, "alt", "<IMG>" );
    docHtmlPutString( "/>", hwc );
    docHtmlNewLine( hwc );

    return;
    }

/************************************************************************/
/*									*/
/*  Determine the size of a raster image thta is included in the html	*/
/*  file.								*/
/*									*/
/************************************************************************/

static int docHtmlRasterImageSize(	int *			pWide,
					int *			pHigh,
					InsertedObject *	io )
    {
    const BitmapDescription *		bd;

    int					wide;
    int					high;
    int					d;

    wide= TWIPS_TO_PIXELS( ( io->ioScaleXSet* io->ioTwipsWide )/100 );
    high= TWIPS_TO_PIXELS( ( io->ioScaleYSet* io->ioTwipsHigh )/100 );

    if  ( ! io->ioRasterImage.riBytes )
	{
	if  ( docGetBitmapForObject( io ) )
	    { XDEB(io->ioRasterImage.riBytes); return 1;	}
	}

    if  ( ! io->ioRasterImage.riBytes )
	{ return 1;	}
    bd= &(io->ioRasterImage.riDescription);

    if  ( wide < 1 ) { wide= 1;	}
    if  ( high < 1 ) { high= 1;	}

    d= ( 100* bd->bdPixelsWide- 100* wide )/ bd->bdPixelsWide;
    if  ( d < 0 )
	{ d= -d;	}
    if  ( d <= 15 )
	{ wide= bd->bdPixelsWide;	}

    d= ( 100* bd->bdPixelsHigh- 100* high )/ bd->bdPixelsHigh;
    if  ( d < 0 )
	{ d= -d;	}
    if  ( d <= 15 )
	{ high= bd->bdPixelsHigh;	}

    *pWide= wide; *pHigh= high; return 0;
    }

/************************************************************************/
/*									*/
/*  Same the reference to an image.					*/
/*									*/
/*  Usually, this is an 'img' element. For images that are better	*/
/*  rendered in SVG, we use an SVG object.				*/
/*									*/
/************************************************************************/

int docHtmlSaveImgElement(	int *				pDone,
				HtmlWritingContext *		hwc,
				int				n,
				const struct BufferItem *	paraNode,
				InsertedObject *		io )
    {
    int				rval= 0;
    int				wide;
    int				high;

    const char *		mimeType;
    const char *		ext;
    const MemoryBuffer *	mb= (const MemoryBuffer *)0;
    int				type;
    int				asObject= 0;

    int				fixed= 0;

    MemoryBuffer		src;

    utilInitMemoryBuffer( &src );

    if  ( ! hwc->hwcGetImageSrc )
	{ XDEB(hwc->hwcGetImageSrc); rval= -1; goto ready;	}

    if  ( docHtmlObjectSaveHow( &type, &mimeType, &ext, &mb, io ) )
	{ *pDone= 0; rval= 1; goto ready;	}

    if  ( (*hwc->hwcGetImageSrc)( &src, hwc, n, io, ext ) < 0 )
	{ LSDEB(n,ext); rval= -1; goto ready;	}

    if  ( docCheckObjectLayout(	&fixed, io ) )
	{ LDEB(1);	}

    switch( type )
	{
	case DOCokDRAWING_SHAPE:
	    {
	    DocumentRectangle	drTwips;

	    docPlaceRootShapeRect( &drTwips, &(io->ioDrawingShape->dsShapeProperties), 0, 0 );

	    wide= drTwips.drX1- drTwips.drX0+ 1;
	    high= drTwips.drY1- drTwips.drY0+ 1;

	    docHtmlStartObject( hwc, paraNode, io, wide, high, mimeType, &src );
	    asObject= 1;
	    }
	    break;

    	case DOCokPICTWMETAFILE:
	case DOCokPICTEMFBLIP:
	case DOCokMACPICT:
	    wide= ( io->ioScaleXSet* io->ioTwipsWide )/100;
	    high= ( io->ioScaleYSet* io->ioTwipsHigh )/100;

	    docHtmlStartObject( hwc, paraNode, io, wide, high, mimeType, &src );
	    asObject= 1;
	    break;

	case DOCokPICTPNGBLIP:
	case DOCokPICTJPEGBLIP:
	case DOCokGIF_FILE:
	    if  ( docHtmlRasterImageSize( &wide, &high, io ) )
		{ LDEB(1); *pDone= 0; rval= 1; goto ready;	}
	    break;

	default:
	    LDEB(type); *pDone= 0; rval= 1; goto ready;
	}

    docHtmlEmitImgElement( hwc, paraNode, io, wide, high, &src );

    if  ( asObject )
	{
	docHtmlPutString( "</object>", hwc );
	docHtmlNewLine( hwc );
	}

    hwc->hwcImageCount++;
    *pDone= 1;

  ready:

    utilCleanMemoryBuffer( &src );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Save drawing shape to SVG.						*/
/*									*/
/************************************************************************/

static int docHtmlSaveDrawingShape(	HtmlWritingContext *	hwc,
					int			n,
					InsertedObject *	io,
					const char *		mimeType,
					const char *		ext )
    {
    int				rval= 0;

    DocumentRectangle		drDest;
    int				pixelsWide;
    int				pixelsHigh;
    int				page;

    SimpleOutputStream *	sosImage= (SimpleOutputStream *)0;

    DocumentTree *		ei= (DocumentTree *)0;
    struct BufferItem *		bodySectBi= (struct BufferItem *)0;

    DrawingShape *		ds= io->ioDrawingShape;

    docObjectGetPageRect( &drDest, io, 0, 0 );
    pixelsWide= TWIPS_TO_PIXELS(drDest.drX1- drDest.drX0+ 1);
    pixelsHigh= TWIPS_TO_PIXELS(drDest.drY1- drDest.drY0+ 1);
    page= io->ioY0Position.lpPage;

    if  ( docGetRootOfSelectionScope( &ei, &bodySectBi,
					hwc->hwcLayoutContext->lcDocument,
					&(ds->dsSelectionScope) ) )
	{ LDEB(1);	}

    if  ( ! hwc->hwcOpenImageStream )
	{ XDEB(hwc->hwcOpenImageStream); rval= -1; goto ready;	}

    sosImage= (*hwc->hwcOpenImageStream)( hwc, n, io, mimeType, ext );
    if  ( ! sosImage )
	{ XDEB(sosImage); rval= -1; goto ready; }

    if  ( docSvgSaveShapeObject( sosImage,
				    page, pixelsWide, pixelsHigh,
				    io, bodySectBi, hwc->hwcLayoutContext ) )
	{ LDEB(1); return -1;	}

  ready:

    if  ( sosImage )
	{ sioOutClose( sosImage );	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Save metafile to SVG.						*/
/*									*/
/************************************************************************/

static int docHtmlPlayMetafileToSvg(	HtmlWritingContext *	hwc,
					int			n,
					InsertedObject *	io,
					const MemoryBuffer *	mb,
					MetafileWriteSvg	writeSvg,
					const char *		mimeType,
					const char *		ext )
    {
    int				rval= 0;

    const PictureProperties *	pip= &(io->ioPictureProperties);

    SimpleOutputStream *	sosImage= (SimpleOutputStream *)0;
    SimpleInputStream *		sisMem= (SimpleInputStream *)0;
    SimpleInputStream *		sisHex= (SimpleInputStream *)0;

    MetafilePlayer		mp;
    SvgWriter			sw;

    DocumentRectangle		drDest;

    svgInitSvgWriter( &sw );

    if  ( ! hwc->hwcOpenImageStream )
	{ XDEB(hwc->hwcOpenImageStream); rval= -1; goto ready;	}

    sosImage= (*hwc->hwcOpenImageStream)( hwc, n, io, mimeType, ext );
    if  ( ! sosImage )
	{ XDEB(sosImage); rval= -1; goto ready; }

    sisMem= sioInMemoryOpen( mb );
    if  ( ! sisMem )
	{ XDEB(sisMem); rval= -1; goto ready;	}
    sisHex= sioInHexOpen( sisMem );
    if  ( ! sisHex )
	{ XDEB(sisHex); rval= -1; goto ready;	}

    sw.swXmlWriter.xwSos= sosImage;

    docObjectGetPageRect( &drDest, io, 0, 0 );
    docObjectGetSourceRect( &(sw.swViewBox), pip );
    sw.swWide= TWIPS_TO_PIXELS(drDest.drX1- drDest.drX0+ 1);
    sw.swHigh= TWIPS_TO_PIXELS(drDest.drY1- drDest.drY0+ 1);

    docSetMetafilePlayer( &mp, sisHex, hwc->hwcLayoutContext, pip,
						    sw.swWide, sw.swHigh );

    svgStartDocument( &sw );

    if  ( (*writeSvg)( &sw, &mp ) )
	{ LSDEB(n,mimeType); rval= -1; goto ready;	}

    svgFinishDocument( &sw );

  ready:

    if  ( sisHex )
	{ sioInClose( sisHex );	}
    if  ( sisMem )
	{ sioInClose( sisMem );	}

    if  ( sosImage )
	{ sioOutClose( sosImage );	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Save from object data.						*/
/*									*/
/************************************************************************/

static int docHtmlSaveObjectData(	HtmlWritingContext *	hwc,
					int			n,
					InsertedObject *	io,
					const MemoryBuffer *	mb,
					const char *		mimeType,
					const char *		ext )
    {
    int				rval= 0;

    SimpleOutputStream *	sosImage= (SimpleOutputStream *)0;
    SimpleInputStream *		sisMem= (SimpleInputStream *)0;
    SimpleInputStream *		sisHex= (SimpleInputStream *)0;

    int			done;
    unsigned char	buf[1024];

    if  ( ! hwc->hwcOpenImageStream )
	{ XDEB(hwc->hwcOpenImageStream); rval= -1; goto ready;	}

    sosImage= (*hwc->hwcOpenImageStream)( hwc, n, io, mimeType, ext );
    if  ( ! sosImage )
	{ XDEB(sosImage); rval= -1; goto ready; }

    sisMem= sioInMemoryOpen( mb );
    if  ( ! sisMem )
	{ XDEB(sisMem); rval= -1; goto ready;	}
    sisHex= sioInHexOpen( sisMem );
    if  ( ! sisHex )
	{ XDEB(sisHex); rval= -1; goto ready;	}

    while( ( done= sioInReadBytes( sisHex, buf, sizeof(buf) ) ) > 0 )
	{
	if  ( sioOutWriteBytes( sosImage, buf, done ) != done )
	    { LDEB(done);	}
	}

  ready:

    if  ( sisHex )
	{ sioInClose( sisHex );	}
    if  ( sisMem )
	{ sioInClose( sisMem );	}

    if  ( sosImage )
	{ sioOutClose( sosImage );	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Save the images in the document.					*/
/*									*/
/************************************************************************/

static int docHtmlSaveImageBytes(	HtmlWritingContext *	hwc,
					bmWriteBitmap		writeBitmap,
					int			n,
					InsertedObject *	io,
					const char *		mimeType,
					const char *		ext )
    {
    int				rval= 0;

    SimpleOutputStream *	sosImage= (SimpleOutputStream *)0;

    if  ( ! hwc->hwcOpenImageStream )
	{ XDEB(hwc->hwcOpenImageStream); rval= -1; goto ready;	}

    sosImage= (*hwc->hwcOpenImageStream)( hwc, n, io, mimeType, ext );
    if  ( ! sosImage )
	{ XDEB(sosImage); rval= -1; goto ready; }

    if  ( (*writeBitmap)( &(io->ioRasterImage.riDescription),
					io->ioRasterImage.riBytes, sosImage ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    if  ( sosImage )
	{ sioOutClose( sosImage );	}

    return rval;
    }

static int docHtmlSaveImage( int n, void * vio, void * vhwc )
    {
    HtmlWritingContext *	hwc= (HtmlWritingContext *)vhwc;
    InsertedObject *		io= (InsertedObject *)vio;

    int				type;
    const char *		mimeType;
    const char *		ext;

    int				fixed= 0;

    const MemoryBuffer *	mb= (const MemoryBuffer *)0;

    if  ( docHtmlObjectSaveHow( &type, &mimeType, &ext, &mb, io ) )
	{ return 0;	}

    if  ( docCheckObjectLayout(	&fixed, io ) )
	{ LDEB(1);	}

    switch( type )
	{
	case DOCokDRAWING_SHAPE:
	    if  ( docHtmlSaveDrawingShape( hwc, n, io, mimeType, ext ) )
		{ SDEB(mimeType); return -1;	}

	    return 0;

	case DOCokPICTWMETAFILE:
	    if  ( docHtmlPlayMetafileToSvg( hwc, n, io, mb,
				    appMetaPlayWmfSvg, mimeType, ext ) )
		{ SDEB(mimeType); return -1;	}

	    return 0;

	case DOCokPICTEMFBLIP:
	    if  ( docHtmlPlayMetafileToSvg( hwc, n, io, mb,
				    appMetaPlayEmfSvg, mimeType, ext ) )
		{ SDEB(mimeType); return -1;	}

	    return 0;

	case DOCokMACPICT:
	    if  ( docHtmlPlayMetafileToSvg( hwc, n, io, mb,
				    appMacPictPlayFileSvg, mimeType, ext ) )
		{ SDEB(mimeType); return -1;	}

	    return 0;

	case DOCokPICTPNGBLIP:
	case DOCokPICTJPEGBLIP:
	    if  ( mb )
		{
		if  ( docHtmlSaveObjectData( hwc, n, io, mb, mimeType, ext ) )
		    { SDEB(mimeType); return -1;	}

		return 0;
		}
	    break;

	default:
	    break;
	}

    if  ( ! io->ioRasterImage.riBytes )
	{ XDEB(io->ioRasterImage.riBytes); return 0;	}

    switch( type )
	{
	case DOCokPICTPNGBLIP:
	    if  ( docHtmlSaveImageBytes( hwc, bmPngWritePng,
						    n, io, mimeType, ext ) )
		{ SDEB(mimeType); return -1;	}

	    return 0;

	case DOCokPICTJPEGBLIP:
	    if  ( docHtmlSaveImageBytes( hwc, bmJpegWriteJfif,
						    n, io, mimeType, ext ) )
		{ SDEB(mimeType); return -1;	}

	    return 0;

	case DOCokGIF_FILE:
	    if  ( docHtmlSaveImageBytes( hwc, bmGifWriteGif,
						    n, io, mimeType, ext ) )
		{ SDEB(mimeType); return -1;	}

	    return 0;

	default:
	    LLDEB(n,type); return 0;
	}
    }

int docHtmlSaveImages(	HtmlWritingContext *		hwc )
    {
    BufferDocument *	bd= hwc->hwcDocument;

    utilPagedListForAll( &(bd->bdObjectList.iolPagedList),
					    docHtmlSaveImage, (void *)hwc );

    return 0;
    }

