/************************************************************************/
/*									*/
/*  Open the objects embedded in a document.				*/
/*									*/
/************************************************************************/

#   include	"docLayoutConfig.h"

#   include	<stdio.h>

#   include	<sioMemory.h>
#   include	<sioHex.h>
#   include	<drawMetafile.h>
#   include	"docLayoutObject.h"

#   include	<appDebugon.h>

#   include	<docBuf.h>
#   include	<docPageGrid.h>
#   include	<docTreeNode.h>
#   include	<geoGrid.h>
#   include	<geoUnits.h>
#   include	<docShape.h>
#   include	<docObjectProperties.h>
#   include	<bmio.h>

/************************************************************************/
/*									*/
/*  Make the bitmap for an image.					*/
/*  Make a name for an image.						*/
/*									*/
/************************************************************************/

static int docGetBitmap(	bmReadBitmap		readBitmap,
				RasterImage *		ri,
				const MemoryBuffer *	mb )
    {
    int			rval= 0;
    int			res;

    SimpleInputStream *	sisMem= (SimpleInputStream *)0;
    SimpleInputStream *	sisBitmap= (SimpleInputStream *)0;

    sisMem= sioInMemoryOpen( mb );
    if  ( ! sisMem )
	{ XDEB(sisMem); rval= -1; goto ready;	}

    sisBitmap= sioInHexOpen( sisMem );
    if  ( ! sisBitmap )
	{ XDEB(sisBitmap); rval= -1; goto ready;	}

    res= (*readBitmap)( &(ri->riDescription), &(ri->riBytes), sisBitmap );

    if  ( res < 0 )
	{ LDEB(res); rval= -1;	}

  ready:

    if  ( sisBitmap )
	{ sioInClose( sisBitmap );	}
    if  ( sisMem )
	{ sioInClose( sisMem );	}

    return rval;
    }

int docGetBitmapForObjectData(	int			kind,
				RasterImage *		ri,
				const MemoryBuffer *	mb )
    {
    switch( kind )
	{
	case DOCokPICTWMETAFILE:
	    if  ( docGetBitmap( appMetaPlayWmfImg, ri, mb )	)
		{ LDEB(1); return -1;	}

	    break;

	case DOCokPICTPNGBLIP:
	    if  ( docGetBitmap( bmPngReadPng, ri, mb )	)
		{ LDEB(1); return -1;	}
	    break;

	case DOCokPICTJPEGBLIP:
	    if  ( docGetBitmap( bmJpegReadJfif, ri, mb )	)
		{ LDEB(1); return -1;	}

	    break;

	case DOCokMACPICT:
	case DOCokPICTEMFBLIP:
	default:
	    LDEB(kind); return -1;
	}

    return 0;
    }

int docGetBitmapForObject(	InsertedObject *	io )
    {
    switch( io->ioKind )
	{
	case DOCokPICTWMETAFILE:
	    if  ( ! io->ioRasterImage.riBytes				&&
		  docGetBitmapForObjectData( io->ioKind,
			&(io->ioRasterImage), &(io->ioObjectData) )	)
		{ LDEB(1); return 0;	}

	    if  ( io->ioRasterImage.riBytes )
		{ io->ioPictureProperties.pipMetafileIsBitmap= 1;	}

	    break;

	case DOCokPICTPNGBLIP:
	case DOCokPICTJPEGBLIP:
	    if  ( ! io->ioRasterImage.riBytes				&&
		  docGetBitmapForObjectData( io->ioKind,
			&(io->ioRasterImage), &(io->ioObjectData) )	)
		{ LDEB(1); return 0;	}
	    break;

	case DOCokOLEOBJECT:
	    if  ( io->ioResultKind == DOCokPICTWMETAFILE )
		{
		if  ( ! io->ioRasterImage.riBytes			&&
		      docGetBitmapForObjectData( io->ioResultKind,
			    &(io->ioRasterImage), &(io->ioResultData) )	)
		    { LDEB(1); return 0;	}

		if  ( io->ioRasterImage.riBytes )
		    { io->ioPictureProperties.pipMetafileIsBitmap= 1;	}

		return 0;
		}

	    if  ( io->ioResultKind == DOCokPICTPNGBLIP	||
		  io->ioResultKind == DOCokPICTJPEGBLIP )
		{
		if  ( ! io->ioRasterImage.riBytes			&&
		      docGetBitmapForObjectData( io->ioResultKind,
			    &(io->ioRasterImage), &(io->ioResultData) )	)
		    { LDEB(1); return 0;	}

		return 0;
		}

	    return 0;

	case DOCokMACPICT:
	case DOCokPICTEMFBLIP:
	default:
	    LDEB(io->ioKind); return 0;
	}

    return 0;
    }

int docCheckObjectLayout(	int *			pFixed,
				InsertedObject *	io )
    {
    PictureProperties *	pip= &(io->ioPictureProperties);
    int			fixed= 0;

    if  ( io->ioKind == DOCokMACPICT )
	{
	if  ( pip->pipTwipsWide == 0 )
	    { pip->pipTwipsWide= 20* pip->pip_xWinExt; fixed= 1;	}
	if  ( pip->pipTwipsHigh == 0 )
	    { pip->pipTwipsHigh= 20* pip->pip_yWinExt; fixed= 1;	}
	}

    if  ( io->ioKind == DOCokDRAWING_SHAPE )
	{
	const DrawingShape *	ds= io->ioDrawingShape;

	if  ( ! ds )
	    { LXDEB(io->ioKind,io->ioDrawingShape);	}
	else{
	    DocumentRectangle	drTwips;

	    docPlaceRootShapeRect( &drTwips, &(io->ioDrawingShape->dsShapeProperties), 0, 0 );

	    if  ( io->ioTwipsWide == 0 )
		{
		io->ioTwipsWide= drTwips.drX1- drTwips.drX0+ 1;
		fixed= 1;
		}
	    if  ( io->ioTwipsHigh == 0 )
		{
		io->ioTwipsHigh= drTwips.drY1- drTwips.drY0+ 1;
		fixed= 1;
		}
	    }
	}

    if  ( ( pip->pipTwipsWide == 0		||
	    pip->pipTwipsHigh == 0		)	&&
	  ( io->ioKind == DOCokPICTPNGBLIP	||
	    io->ioKind == DOCokPICTJPEGBLIP	)	)
	{
	if  ( ! io->ioRasterImage.riBytes	&&
	      docGetBitmapForObject( io )	)
	    { LDEB(1);	}
	else{
	    bmRectangleSizeTwips( &(pip->pipTwipsWide), &(pip->pipTwipsHigh),
				&(io->ioRasterImage.riDescription),
				io->ioRasterImage.riDescription.bdPixelsWide,
				io->ioRasterImage.riDescription.bdPixelsHigh );
	    fixed= 1;
	    }
	}

    if  ( io->ioTwipsWide == 0 )
	{ io->ioTwipsWide= pip->pipTwipsWide; fixed= 1;	}
    if  ( io->ioTwipsHigh == 0 )
	{ io->ioTwipsHigh= pip->pipTwipsHigh; fixed= 1;	}

    *pFixed= fixed;
    return 0;
    }

/************************************************************************/
/*									*/
/*  Scale an inserted object to fit where we want to place it.		*/
/*									*/
/*  1)  If the object fits on the page at its current scale, nothing	*/
/*	needs to be done. Apparently this was the case before as well,	*/
/*	as otherwise the ioScale[XY]Used would not have been equal to	*/
/*	ioScale[XY]Set.							*/
/*  2)  Otherwise scale the object to fit on the page. X and Y scale	*/
/*	are the same, irrespective of the original scaling.		*/
/*									*/
/************************************************************************/

void docLayoutScaleObjectToFitParagraphFrame(
				int *				pChanged,
				InsertedObject *		io,
				int				pageHigh,
				const DocumentRectangle *	drParaContent )
    {
    int		textWideTwips= drParaContent->drX1- drParaContent->drX0;

    int		objectWideTwips= ( io->ioScaleXSet* io->ioTwipsWide )/ 100.0;
    int		objectHighTwips= ( io->ioScaleYSet* io->ioTwipsHigh )/ 100.0;

    double	scaleX;
    double	scaleY;
    int		scaleMax;

    int		prevXScaleUsed= io->ioScaleXUsed;
    int		prevYScaleUsed= io->ioScaleYUsed;

    PictureProperties *	pip= &(io->ioPictureProperties);

    /*  1  */
    if  ( io->ioScaleXUsed == io->ioScaleXSet	&&
	  io->ioScaleYUsed == io->ioScaleYSet	&&
	  objectWideTwips <= textWideTwips	&&
	  objectHighTwips <= pageHigh		)
	{ *pChanged= 0; return;	}

    /*  2  */
    scaleX= (double)textWideTwips/ (double)io->ioTwipsWide;
    scaleY= (double)pageHigh/ (double)io->ioTwipsHigh;

    if  ( scaleY > scaleX )
	{ scaleY=  scaleX;	}
    scaleMax= (int)( 99* scaleY );

    io->ioScaleXUsed= io->ioScaleXSet;
    io->ioScaleYUsed= io->ioScaleYSet;

    pip->pipScaleXUsed= io->ioScaleXUsed;
    pip->pipScaleYUsed= io->ioScaleYUsed;

    if  ( scaleMax < io->ioScaleXUsed	||
	  scaleMax < io->ioScaleYUsed	)
	{
	if  ( io->ioScaleXUsed > scaleMax )
	    { io->ioScaleXUsed= scaleMax;	}
	if  ( io->ioScaleYUsed > scaleMax )
	    { io->ioScaleYUsed= scaleMax;	}

	if  ( io->ioScaleXUsed < 1 )
	    { io->ioScaleXUsed=  1;	}
	if  ( io->ioScaleYUsed < 1 )
	    { io->ioScaleYUsed=  1;	}

	pip->pipScaleXUsed= io->ioScaleXUsed;
	pip->pipScaleYUsed= io->ioScaleYUsed;
	}

    if  ( io->ioScaleXUsed != prevXScaleUsed	||
	  io->ioScaleYUsed != prevYScaleUsed	)
	{ *pChanged= 1;	}
    else{ *pChanged= 0;	}

    return;
    }

void docScaleObjectToParagraph( BufferDocument *		bd,
				BufferItem *			paraBi,
				double				xfac,
				InsertedObject *		io )
    {
    ParagraphFrame		pf;

    PictureProperties *		pip= &(io->ioPictureProperties);

    BlockFrame			bf;
    int				changed= 0;

    int				pageHigh;

    docBlockFrameTwips( &bf, paraBi, bd,
				    paraBi->biTopPosition.lpPage,
				    paraBi->biTopPosition.lpColumn );

    docParagraphFrameTwips( &pf, &bf, paraBi );

    pageHigh= bf.bfPageGeometry.dgPageHighTwips- 
			    bf.bfPageGeometry.dgTopMarginTwips-
			    bf.bfPageGeometry.dgBottomMarginTwips;

    docLayoutScaleObjectToFitParagraphFrame( &changed,
				    io, pageHigh, &(pf.pfParaContentRect) );

    pip->pipScaleXUsed= io->ioScaleXUsed;
    pip->pipScaleYUsed= io->ioScaleYUsed;

    io->ioPixelsWide= COORDtoGRID( xfac,
				( io->ioScaleXUsed* pip->pipTwipsWide )/ 100 );
    io->ioPixelsHigh= COORDtoGRID( xfac,
				( io->ioScaleYUsed* pip->pipTwipsHigh )/ 100 );
    if  ( io->ioPixelsWide < 1 )
	{ io->ioPixelsWide=  1;	}
    if  ( io->ioPixelsHigh < 1 )
	{ io->ioPixelsHigh=  1;	}

    pip->pip_xWinExt= (int) ( ( 100000.0* pip->pipTwipsWide )/ TWIPS_PER_M );
    pip->pip_yWinExt= (int) ( ( 100000.0* pip->pipTwipsHigh )/ TWIPS_PER_M );

    return;
    }

