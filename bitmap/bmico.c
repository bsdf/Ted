#   include	"bitmapConfig.h"

#   include	<stdio.h>
#   include	<stdlib.h>
#   include	"bmintern.h"
#   include	<sioEndian.h>
#   include	<sioFileio.h>
#   include	<utilMemoryBuffer.h>

#   include	"bmbmp.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Read a Microsoft Ico file.						*/
/*									*/
/*  1)	Read and check header.						*/
/*  2)  Translate header to a BitmapDescription.			*/
/*  2a) Only use XOR part of image.					*/
/*  3)  Do not seek to data. It immediately follows.			*/
/*  4)  Read image.							*/
/*  5)  Combine with transparency mask.					*/
/*									*/
/************************************************************************/

static int bmpReadIcoDirectoryEntry(	IcoDirectoryEntry *	ide,
					SimpleInputStream *	sis )
    {
    ide->ideWidth= sioInGetByte( sis );
    ide->ideHeight= sioInGetByte( sis );
    ide->ideColorCount= sioInGetByte( sis );
    ide->ideReserved= sioInGetByte( sis );

    ide->idePlanes= sioEndianGetLeInt16( sis );
    ide->ideBitCount= sioEndianGetLeInt16( sis );
    ide->ideBytesInRes= sioEndianGetLeInt32( sis );
    ide->ideImageOffset= sioEndianGetLeInt32( sis );

    return 4* 1+ 2+ 2+ 4+ 4;
    }

# if 0
static void bmpLogIcoDirectoryEntry(	const IcoDirectoryEntry *	ide )
    {
    LDEB(ide->ideWidth);
    LDEB(ide->ideHeight);
    LDEB(ide->ideColorCount);
    LDEB(ide->ideReserved);
    LDEB(ide->idePlanes);
    LDEB(ide->ideBitCount);
    LDEB(ide->ideBytesInRes);
    LDEB(ide->ideImageOffset);
    }
# endif

int bmReadIcoFile(	const MemoryBuffer *	filename,
			unsigned char **	pBuffer,
			BitmapDescription *	bd,
			int *			pPrivateFormat )
    {
    int				rval= 0;
    SimpleInputStream *		sis= (SimpleInputStream *)0;
    unsigned char *		buffer= (unsigned char *)0;
    RGB8Color *			palette= (RGB8Color *)0;

    BmpImageHeader		bih;
    IcoFileHeader		ifh;
    IcoDirectoryEntry		ide;

    BitmapDescription		bdImage;
    BitmapDescription		bdAlpha;
    unsigned char *		bufImage= (unsigned char *)0;
    unsigned char *		bufAlpha= (unsigned char *)0;
    int				byprImage;
    int				byprAlpha;

    int				done;
    int				bytesRead= 0;
    int				i;

    bmInitDescription( &bdAlpha );
    bmInitDescription( &bdImage );

    sis= sioInFileioOpen( filename );
    if  ( ! sis )
	{ XDEB(sis); rval= -1; goto ready;	}

    ifh.ifhReserved= sioEndianGetLeInt16( sis );
    if  ( ifh.ifhReserved != 0 )
	{ LDEB(ifh.ifhReserved); rval= -1; goto ready;	}

    ifh.ifhType= sioEndianGetLeInt16( sis );
    if  ( ifh.ifhType != 1 )
	{ LDEB(ifh.ifhType); rval= -1; goto ready;	}

    ifh.ifhCount= sioEndianGetLeInt16( sis );
    if  ( ifh.ifhCount != 1 )
	{ LDEB(ifh.ifhCount);	}

    done= bmpReadIcoDirectoryEntry( &ide, sis );
    if  ( done < 0 )
	{ LDEB(done); rval= -1; goto ready;	}
    if  ( ide.ideReserved != 0 )
	{ LDEB(ide.ideReserved); rval= -1; goto ready;	}

    bytesRead= 2+ 2+ 2+ done;

    while( bytesRead < ide.ideImageOffset )
	{
	if  ( sioInGetByte( sis ) < 0 )
	    { LDEB(ide.ideImageOffset); rval= -1; goto ready;	}

	bytesRead++;
	}

    /*  1  */
    done= bmpReadImageHeader( &bih, sis, &palette );
    if  ( done < 0 )
	{ LDEB(done); rval= -1; goto ready;	}

    /*  2  */
    if  ( bmpHeaderToDescription( &bdImage, &byprImage, &bih, palette, 0 ) )
	{ LDEB(1); rval= -1; goto ready;	}
    /*  2a  */
    bdImage.bdPixelsHigh /= 2;
    if  ( bmCalculateSizes( &bdImage ) )
	{ LDEB(1); rval= -1; goto ready;	}

    bdAlpha.bdPixelsWide= bdImage.bdPixelsWide;
    bdAlpha.bdPixelsHigh= bdImage.bdPixelsHigh;
    bdAlpha.bdBitsPerSample= 1;
    bdAlpha.bdSamplesPerPixel= 1;
    bdAlpha.bdBitsPerPixel= 1;
    bdAlpha.bdXResolution= bdImage.bdXResolution;
    bdAlpha.bdYResolution= bdImage.bdYResolution;
    bdAlpha.bdUnit= bdImage.bdUnit;
    bdAlpha.bdColorEncoding= BMcoBLACKWHITE;
    bdAlpha.bdHasAlpha= 0;

    if  ( bmCalculateSizes( &bdAlpha ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( bmCopyDescription( bd, &bdImage ) )
	{ LDEB(1); rval= -1; goto ready;	}

    /*  3  LDEB(bih.bihBitmapDataOffset);  */

    /*  4  */
    done= bmBmpReadImageBytes( &bufImage, sis, bih.bihCompression,
				    byprImage, bih.bihBitsPerPixel, &bdImage );
    if  ( done < 0 )
	{ LDEB(1); rval= -1; goto ready;	}

    byprAlpha= ( bdAlpha.bdPixelsWide+ 7 )/ 8;
    done= bmBmpReadImageBytes( &bufAlpha, sis, bih.bihCompression,
				    byprAlpha, 1, &bdAlpha );
    if  ( done < 0 )
	{ LDEB(1); rval= -1; goto ready;	}
    /* xor */
    for ( i= 0; i < bdAlpha.bdBufferLength; i++ )
	{ bufAlpha[i]= ~bufAlpha[i];	}

    if  ( bmSetAlphaMask( bd, &bdImage, &bdAlpha,
					&buffer, bufImage, bufAlpha ) )
	{ LDEB(1); rval= -1; goto ready;	}

    *pBuffer= buffer; buffer= (unsigned char *)0; /* steal */
    *pPrivateFormat= bih.bihSizeOfRestOfHeader;

    if  ( bih.bihColorCount > 0 )
	{ bmMakeMonochrome( bd, buffer );	}

  ready:

    if  ( buffer )
	{ free( buffer );	}
    if  ( sis )
	{ sioInClose( sis );	}

    if  ( bufAlpha )
	{ free( bufAlpha );	}
    if  ( bufImage )
	{ free( bufImage );	}
    if  ( palette )
	{ free( palette );	}

    bmCleanDescription( &bdAlpha );
    bmCleanDescription( &bdImage );

    return rval;
    }

/************************************************************************/
/*  Write a Microsoft ICO file.						*/
/************************************************************************/

static int bmbmpSizeIcoDirectory(	const IcoDirectoryEntry *	ide )
    { return 16;	}

static int bmbmpWriteIcoDirectory(	const IcoDirectoryEntry *	ide,
					SimpleOutputStream *		sos )
    {
    if  ( sioOutPutByte( ide->ideWidth, sos ) < 0 )
	{ return -1;	}
    if  ( sioOutPutByte( ide->ideHeight, sos ) < 0 )
	{ return -1;	}
    if  ( sioOutPutByte( ide->ideColorCount, sos ) < 0 )
	{ return -1;	}
    if  ( sioOutPutByte( ide->ideReserved, sos ) < 0 )
	{ return -1;	}
    if  ( sioEndianPutLeInt16( ide->idePlanes, sos ) < 0 )
	{ return -1;	}
    if  ( sioEndianPutLeInt16( ide->ideBitCount, sos ) < 0 )
	{ return -1;	}
    if  ( sioEndianPutLeInt32( ide->ideBytesInRes, sos ) < 0 )
	{ return -1;	}
    if  ( sioEndianPutLeInt32( ide->ideImageOffset, sos ) < 0 )
	{ return -1;	}

    return 16;
    }

static int bmIcoCollectResources( IcoFileHeader *		ifh,
				IcoDirectoryEntry *		ide,
				BmpImageHeader *		bih,
				RGB8Color **			pPalette,
				MemoryBuffer *			mbImage,
				MemoryBuffer *			mbAlpha,
				const BitmapDescription *	bd,
				const unsigned char *		buffer )
    {
    int			rval= 0;

    int			bytesNeeded;
    int			size;

    RGB8Color *		palette= (RGB8Color *)0;

    RasterImage		riAlpha;
    int			alphaBytes;

    RasterImage		riImage;
    int			imageBytes;
    int			headerBytes;

    bmInitRasterImage( &riAlpha );
    bmInitRasterImage( &riImage );

    ifh->ifhReserved= 0;
    ifh->ifhType= 1;
    ifh->ifhCount= 1;

    bytesNeeded= 2+ 2+ 2;

    ide->ideWidth= bd->bdPixelsWide;
    ide->ideHeight= bd->bdPixelsHigh;
    ide->ideColorCount= bd->bdPalette.cpColorCount;
    ide->ideReserved= 0;
    ide->idePlanes= 0;
    ide->ideBitCount= 0;
    ide->ideBytesInRes= 0;
    ide->ideImageOffset= 0;

    size= bmbmpSizeIcoDirectory( ide );
    if  ( size < 0 )
	{ LDEB(size); rval= -1; goto ready;	}
    bytesNeeded += size;

    ide->ideImageOffset= ( ( bytesNeeded + 1 )/ 2 )* 2;

    while( bytesNeeded < ide->ideImageOffset )
	{ bytesNeeded++;	}

    if  ( bmbmpDescriptionToHeader( bih, &palette, 40, bd->bdHasAlpha, bd ) )
	{ LDEB(40); rval= -1; goto ready;	}

    bih->bihPixelsHigh *= 2;
    bih->bihCompression= 0;

    /**/ bih->bihXResolution= 0;
    /**/ bih->bihYResolution= 0;

    headerBytes= bmpSizeImageHeader( bih, palette );
    if  ( headerBytes < 0 )
	{ LDEB(headerBytes); rval= -1; goto ready;	}

    bytesNeeded += headerBytes;

#if 0
    /**/ bih->bihColorCount= 0;
#endif
    bih->bihBitmapDataOffset= bytesNeeded;

    if  ( bd->bdHasAlpha )
	{
	RasterImage	riTmp;
	const int	ignored= 0;

	riTmp.riDescription= *bd;
	riTmp.riBytes= (unsigned char *)buffer;

	if  ( bmRemoveAlpha( &riImage, &riTmp, ignored ) )
	    { LDEB(1); rval= -1; goto ready;	}
	if  ( bmGetAlphaMask( &riAlpha, &riTmp, ignored ) )
	    { LDEB(1); rval= -1; goto ready;	}

	imageBytes= bmBmpSaveImageBytes( mbImage, &(riImage.riDescription),
					    riImage.riBytes, COMPRESS_NONE );
	if  ( imageBytes < 0 )
	    { LDEB(imageBytes); rval= -1; goto ready;	}
	alphaBytes= bmBmpSaveImageBytes( mbAlpha, &(riAlpha.riDescription),
					    riAlpha.riBytes, COMPRESS_NONE );
	if  ( alphaBytes < 0 )
	    { LDEB(alphaBytes); rval= -1; goto ready;	}

	bytesNeeded += imageBytes+ alphaBytes;
	}
    else{
	riAlpha.riDescription.bdPixelsWide= bd->bdPixelsWide;
	riAlpha.riDescription.bdPixelsHigh= bd->bdPixelsHigh;
	riAlpha.riDescription.bdBitsPerSample= 1;
	riAlpha.riDescription.bdSamplesPerPixel= 1;
	riAlpha.riDescription.bdXResolution= bd->bdXResolution;
	riAlpha.riDescription.bdYResolution= bd->bdYResolution;
	riAlpha.riDescription.bdUnit= bd->bdUnit;
	riAlpha.riDescription.bdColorEncoding= BMcoBLACKWHITE;

	if  ( bmCalculateSizes( &(riAlpha.riDescription) ) )
	    { LDEB(1); rval= -1; goto ready;	}

	if  ( bmAllocateBuffer( &riAlpha ) )
	    { LXDEB(riAlpha.riDescription.bdBufferLength,riAlpha.riBytes); rval= -1; goto ready; }
	bmSetSolidBlack( &(riAlpha) );

	imageBytes= bmBmpSaveImageBytes( mbImage, bd, buffer, COMPRESS_NONE );
	if  ( imageBytes < 0 )
	    { LDEB(imageBytes); rval= -1; goto ready;	}
	alphaBytes= bmBmpSaveImageBytes( mbAlpha, &riAlpha.riDescription, riAlpha.riBytes,
								COMPRESS_NONE );
	if  ( alphaBytes < 0 )
	    { LDEB(alphaBytes); rval= -1; goto ready;	}

	bytesNeeded += imageBytes+ alphaBytes;
	}

    bih->bihFileSize= headerBytes+ imageBytes+ alphaBytes;
    bih->bihBufferLength= bih->bihFileSize- bih->bihBitmapDataOffset;
    ide->ideBytesInRes= bih->bihFileSize- ide->ideImageOffset;

    *pPalette= palette;
    rval= bytesNeeded;

  ready:

    bmCleanRasterImage( &riAlpha );
    bmCleanRasterImage( &riImage );

    return rval;
    }

static int bmIcoWriteResources( SimpleOutputStream *		sos,
				const IcoFileHeader *		ifh,
				const IcoDirectoryEntry *	ide,
				const BmpImageHeader *		bih,
				const RGB8Color *		palette,
				MemoryBuffer *			mbImage,
				MemoryBuffer *			mbAlpha )
    {
    int				bytesWritten;
    int				done;
    int				size;
    int				i;
    const unsigned char *	bytes= (unsigned char *)0;

    /*  Write Image File Header */
    sioEndianPutLeInt16( ifh->ifhReserved, sos );
    sioEndianPutLeInt16( ifh->ifhType, sos );
    sioEndianPutLeInt16( ifh->ifhCount, sos );

    bytesWritten= 2+ 2+ 2;

    done= bmbmpWriteIcoDirectory( ide, sos );
    if  ( done < 0 )
	{ LDEB(done); return -1;	}
    bytesWritten += done;

    while( bytesWritten < ide->ideImageOffset )
	{
	if  ( sioOutPutByte( '\0', sos ) < 0 )
	    { LDEB(bytesWritten); return -1;	}
	bytesWritten++;
	}

    done= bmpWriteImageHeader( bih, sos, palette );
    if  ( done < 0 )
	{ LDEB(done); return -1;	}
    bytesWritten += done;

    if  ( bih->bihBitmapDataOffset != bytesWritten )
	{ LLDEB(bih->bihBitmapDataOffset,bytesWritten); return -1;	}

    bytes= utilMemoryBufferGetBytes( &size, mbImage );
    done= sioOutWriteBytes( sos, bytes, size );
    if  ( done != size )
	{ LLDEB(done,size); return -1;	}
    bytesWritten += done;

    bytes= utilMemoryBufferGetBytes( &size, mbAlpha );
    /* xor */
    for ( i= 0; i < size; i++ )
	{
	if  ( sioOutPutByte( ~bytes[i], sos ) )
	    { LDEB(1); return -1;	}
	}
    bytesWritten += size;

    return bytesWritten;
    }

int bmWriteIcoFile(	const MemoryBuffer *		filename,
			const unsigned char *		buffer,
			const BitmapDescription *	bd,
			int				privateFormat )
    {
    int				rval= 0;
    SimpleOutputStream *	sos= (SimpleOutputStream *)0;
    BmpImageHeader		bih;
    IcoFileHeader		ifh;
    IcoDirectoryEntry		ide;
    RGB8Color *			palette= (RGB8Color *)0;
    int				bytesNeeded;
    int				bytesWritten;

    MemoryBuffer		mbAlpha;
    MemoryBuffer		mbImage;

    utilInitMemoryBuffer( &mbAlpha );
    utilInitMemoryBuffer( &mbImage );

    sos= sioOutFileioOpen( filename );
    if  ( ! sos )
	{ XDEB(sos); rval= -1; goto ready;	}

    bytesNeeded= bmIcoCollectResources( &ifh, &ide, &bih, &palette,
					&mbAlpha, &mbImage, bd, buffer );

    bytesWritten= bmIcoWriteResources( sos, &ifh, &ide, &bih, palette,
							&mbAlpha, &mbImage );

    if  ( bytesWritten != bytesNeeded )
	{ LLDEB(bytesNeeded,bytesWritten); rval= -1; goto ready;	}

  ready:
    if  ( sos )
	{ sioOutClose( sos );	}

    utilCleanMemoryBuffer( &mbAlpha );
    utilCleanMemoryBuffer( &mbImage );

    return rval;
    }

int bmCanWriteIcoFile( const BitmapDescription *	bd,
			int				privateFormat )
    {
    switch( bd->bdColorEncoding )
	{
	case BMcoRGB8PALETTE:
	    if  ( bd->bdPalette.cpColorCount > 256 )
		{ return -1;	}
	    return 0;
	case BMcoBLACKWHITE:
	case BMcoWHITEBLACK:
	    if  ( bd->bdBitsPerSample > 8 )
		{ return -1;	}
	    return 0;
	default:
	    return -1;
	}
    }
