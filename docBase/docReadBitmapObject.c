/************************************************************************/
/*									*/
/*  Obtain the objects embedded in a document.				*/
/*									*/
/************************************************************************/

#   include	"docBaseConfig.h"

#   include	<string.h>
#   include	<stdio.h>

#   include	<bmio.h>
#   include	<sioMemory.h>
#   include	<sioFileio.h>
#   include	<sioHex.h>

#   include	<appDebugon.h>

#   include	"docObject.h"
#   include	"docObjectProperties.h"

/************************************************************************/
/*									*/
/*  Read a bitmap/pixmap image file.					*/
/*									*/
/************************************************************************/

int docReadBitmapObject(	InsertedObject *	io,
				const MemoryBuffer *	filename )
    {
    int				rval= 0;
    int				res;
    RasterImage			ri;
    const char *		typeId= "?";
    int				copyAsFile= 0;
    int				includedAsRaster= 0;

    PictureProperties *		pip= &(io->ioPictureProperties);

    SimpleOutputStream *	sosMem= (SimpleOutputStream *)0;
    SimpleOutputStream *	sosHex= (SimpleOutputStream *)0;
    SimpleInputStream *		sisFile= (SimpleInputStream *)0;

    bmInitRasterImage( &ri );

    res= bmRead( filename,
		&(ri.riBytes), &(ri.riDescription), &(ri.riFormat) );
    if  ( res )
	{ LDEB(res); rval= -1; goto ready;	}

    if  ( ri.riFormat >= 0 )
	{ typeId= bmFileFormats[ri.riFormat].bffFileType->bftTypeId; }

    io->ioKind= DOCokPICTPNGBLIP;
    io->ioInline= 1;

    bmImageSizeTwips( &(io->ioTwipsWide), &(io->ioTwipsHigh),
							&(ri.riDescription) );

    pip->pipTwipsWide= io->ioTwipsWide;
    pip->pipTwipsHigh= io->ioTwipsHigh;

    io->ioResultData.mbSize= 0;
    sosMem= sioOutMemoryOpen( &(io->ioObjectData) );
    if  ( ! sosMem )
	{ XDEB(sosMem); rval= -1; goto ready;	}

    sosHex= sioOutHexOpen( sosMem );
    if  ( ! sosHex )
	{ XDEB(sosHex); rval= -1; goto ready;	}

    if  ( ri.riFormat >= 0				&&
	  ( ! strcmp( typeId, "jpgFile" )	||
	    ! strcmp( typeId, "jpegFile" )	)	)
	{
	io->ioKind= DOCokPICTJPEGBLIP;
	pip->pipType= DOCokPICTJPEGBLIP;
	copyAsFile= 1;
	}

    if  ( ri.riFormat >= 0			&&
	  ! strcmp( typeId, "pngFile" )		)
	{
	io->ioKind= DOCokPICTPNGBLIP;
	pip->pipType= DOCokPICTPNGBLIP;
	copyAsFile= 1;
	}

    if  ( copyAsFile )
	{
	unsigned char		buf[4096];
	int			got;

	sisFile= sioInFileioOpen( filename );
	if  ( ! sisFile )
	    { XDEB(sisFile); rval= -1; goto ready; }

	for (;;)
	    {
	    got= sioInReadBytes( sisFile, buf, sizeof(buf) );
	    if  ( got < 0 )
		{ LDEB(got); rval= -1; goto ready; }
	    if  ( got <= 0 )
		{ break; }

	    if  ( sioOutWriteBytes( sosHex, buf, got ) < 0 )
		{ LDEB(got); rval= -1; goto ready;	}
	    }

	includedAsRaster= 1;
	}

    if  ( ! includedAsRaster					&&
	  ! bmCanWritePngFile(  &(ri.riDescription), 1 )	)
	{
	if  ( bmPngWritePng( &(ri.riDescription), ri.riBytes, sosHex ) )
	    { LDEB(1); rval= -1; goto ready;	}

	io->ioKind= DOCokPICTPNGBLIP;
	pip->pipType= DOCokPICTPNGBLIP;
	includedAsRaster= 1;
	}

    if  ( ! includedAsRaster )
	{
	if  ( bmWmfWriteWmf( &(ri.riDescription), ri.riBytes, sosHex ) )
	    { LDEB(1); rval= -1; goto ready;	}

	pip->pipMetafileIsBitmap= 1;
	pip->pipMetafileBitmapBpp= ri.riDescription.bdBitsPerPixel;

	io->ioKind= DOCokPICTWMETAFILE;
	pip->pipType= DOCokPICTWMETAFILE;
	pip->pipMapMode= 8;
	}

  ready:

    bmCleanRasterImage( &ri );

    if  ( sisFile )
	{ sioInClose( sisFile );	}
    if  ( sosHex )
	{ sioOutClose( sosHex );	}
    if  ( sosMem )
	{ sioOutClose( sosMem );	}

    return rval;
    }
