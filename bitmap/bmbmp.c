#   include	"bitmapConfig.h"

#   include	<stdio.h>
#   include	<stdlib.h>
#   include	<string.h>
#   include	"bmintern.h"
#   include	<sioEndian.h>
#   include	<sioFileio.h>
#   include	<utilMemoryBuffer.h>
#   include	<sioMemory.h>
#   include	<geoUnits.h>

#   include	"bmbmp.h"
#   include	"bmio.h"

#   include	<appDebugon.h>

# if 0
static void bmpLogImageHeader(	const BmpImageHeader *	bih )
    {
    LDEB(bih->bihMagic);
    LDEB(bih->bihFileSize);
    LDEB(bih->bihReserved1);
    LDEB(bih->bihReserved2);
    LDEB(bih->bihBitmapDataOffset);
    LDEB(bih->bihSizeOfRestOfHeader);
    LDEB(bih->bihPixelsWide);
    LDEB(bih->bihPixelsHigh);
    LDEB(bih->bihPlanes);
    LDEB(bih->bihBitsPerPixel);
    LDEB(bih->bihCompression);
    LDEB(bih->bihBufferLength);
    LDEB(bih->bihXResolution);
    LDEB(bih->bihYResolution);
    LDEB(bih->bihColorCount);
    LDEB(bih->bihImportantColors);
    }
# endif

/************************************************************************/
/*									*/
/*  Read a 3 or 4 byte color table.					*/
/*									*/
/************************************************************************/

static int	bmBmpRead3ColorTable(	int			colorCount,
					SimpleInputStream *	sis,
					RGB8Color *		palette	)
    {
    int		i;

    for ( i= 0; i < colorCount; i++ )
	{
	/* or in the other order ? */
	palette[i].rgb8Blue=	sioInGetByte( sis );
	palette[i].rgb8Green=	sioInGetByte( sis );
	palette[i].rgb8Red=	sioInGetByte( sis );
	palette[i].rgb8Alpha=	255;
	}

    return 0;
    }

static int	bmBmpWrite3ColorTable(	int			colorCount,
					SimpleOutputStream *	sos,
					const RGB8Color *	palette	)
    {
    int		i;

    for ( i= 0; i < colorCount; i++ )
	{
	/* or in the other order ? */
	if  ( sioOutPutByte( palette[i].rgb8Blue, sos ) < 0 )
	    { return -1;	}
	if  ( sioOutPutByte( palette[i].rgb8Green, sos ) < 0 )
	    { return -1;	}
	if  ( sioOutPutByte( palette[i].rgb8Red, sos ) < 0 )
	    { return -1;	}
	}

    return 0;
    }

static int	bmBmpRead4ColorTable(	int			colorCount,
					SimpleInputStream *	sis,
					RGB8Color *		palette	)
    {
    int		i;

    for ( i= 0; i < colorCount; i++ )
	{
	palette[i].rgb8Blue=	sioInGetByte( sis );
	palette[i].rgb8Green=	sioInGetByte( sis );
	palette[i].rgb8Red=	sioInGetByte( sis );
	(void) sioInGetByte( sis );
	palette[i].rgb8Alpha=	255;
	}

    return 0;
    }

static int	bmBmpWrite4ColorTable(	int			colorCount,
					SimpleOutputStream *	sos,
					const RGB8Color *	palette	)
    {
    int		i;

    for ( i= 0; i < colorCount; i++ )
	{
	if  ( sioOutPutByte( palette[i].rgb8Blue, sos ) < 0 )
	    { return -1;	}
	if  ( sioOutPutByte( palette[i].rgb8Green, sos ) < 0 )
	    { return -1;	}
	if  ( sioOutPutByte( palette[i].rgb8Red, sos ) < 0 )
	    { return -1;	}
	if  ( sioOutPutByte( '\0', sos ) < 0 )
	    { return -1;	}
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Read a Microsoft BMP header.					*/
/*  1)  Read fields common to all formats.				*/
/*  2)  Set all distinguishing properties to nonsense, then dixede on	*/
/*	the file type.							*/
/*  3)  If it is not a direct color image, read palette.		*/
/*  4)  Assume reasonable values for missing fields.			*/
/*									*/
/************************************************************************/

int bmpReadImageHeader(	BmpImageHeader *	bih,
			SimpleInputStream *	sis,
			RGB8Color **		pPalette )
    {
    int			colorCount;
    RGB8Color *		palette= (RGB8Color *)0;
    int			paletteBytes= 0;

    /*  1  */
    bih->bihSizeOfRestOfHeader= sioEndianGetLeInt32( sis );

    switch( bih->bihSizeOfRestOfHeader )
	{
	case 12:		/*  OS/2 1.*	*/
	    bih->bihPixelsWide   = sioEndianGetLeInt16( sis );
	    bih->bihPixelsHigh   = sioEndianGetLeInt16( sis );
	    bih->bihPlanes       = sioEndianGetLeInt16( sis );
	    bih->bihBitsPerPixel = sioEndianGetLeInt16( sis );

	    if  ( bih->bihBitsPerPixel != 24 && bih->bihBitsPerPixel != 16 )
		{
		colorCount=
		    (1U << (bih->bihPlanes * bih->bihBitsPerPixel ));
		}
	    else{
		colorCount= 0;
		}

	    /*  4  */
	    bih->bihCompression= 0;
	    bih->bihBufferLength= 0;
	    bih->bihXResolution= POINTS_PER_M;
	    bih->bihYResolution= POINTS_PER_M;
	    bih->bihColorCount= colorCount;
	    bih->bihImportantColors= colorCount;

	    /*  3  */
	    if  ( colorCount > 0 )
		{
		palette= (RGB8Color *)malloc( colorCount * sizeof(RGB8Color) );

		if  ( ! palette )
		    { XDEB(palette); return -1;	}

		if  ( bmBmpRead3ColorTable( colorCount, sis, palette ) )
		    { XDEB(sis); free( palette ); return -1; }

		paletteBytes= 3* colorCount;
		}
	    break;
	case 40:		/*  Windows 3.*	*/
	    bih->bihPixelsWide		= sioEndianGetLeInt32( sis );
	    bih->bihPixelsHigh		= sioEndianGetLeInt32( sis );
	    bih->bihPlanes		= sioEndianGetLeInt16( sis );
	    bih->bihBitsPerPixel	= sioEndianGetLeInt16( sis );
	    bih->bihCompression		= sioEndianGetLeInt32( sis );
	    bih->bihBufferLength	= sioEndianGetLeInt32( sis );
	    bih->bihXResolution		= sioEndianGetLeInt32( sis );
	    bih->bihYResolution		= sioEndianGetLeInt32( sis );
	    bih->bihColorCount		= sioEndianGetLeInt32( sis );
	    bih->bihImportantColors	= sioEndianGetLeInt32( sis );

	    /*  3  */
	    switch( bih->bihBitsPerPixel )
		{
		case 32:
		case 24:
		case 16:
		    if  ( bih->bihColorCount != 0 )
			{ goto palette40;	}
		    break;

		palette40:
		case 8:
		case 4:
		case 2:
		case 1:
		    if  ( bih->bihColorCount == 0 )
			{
			colorCount=
			    ( 1U << (bih->bihPlanes * bih->bihBitsPerPixel ));
			}
		    else{ colorCount= bih->bihColorCount; }

		    palette= (RGB8Color *)malloc(
					colorCount * sizeof(RGB8Color) );

		    if  ( ! palette )
			{ XDEB(palette); return -1;	}

		    if  ( bmBmpRead4ColorTable( colorCount, sis, palette ) )
			{ XDEB(sis); free( palette ); return -1; }

		    paletteBytes= 4* colorCount;
		    break;

		default:
		    LDEB(bih->bihBitsPerPixel); return -1;
		}
	    break;

	case 64:		/*  OS/2 2.0 */
	    bih->bihPixelsWide		= sioEndianGetLeInt32( sis );
	    bih->bihPixelsHigh		= sioEndianGetLeInt32( sis );
	    bih->bihPlanes		= sioEndianGetLeInt16( sis );
	    bih->bihBitsPerPixel	= sioEndianGetLeInt16( sis );
	    bih->bihCompression		= sioEndianGetLeInt32( sis );
	    bih->bihBufferLength	= sioEndianGetLeInt32( sis );
	    bih->bihXResolution		= sioEndianGetLeInt32( sis );
	    bih->bihYResolution		= sioEndianGetLeInt32( sis );
	    bih->bihColorCount		= sioEndianGetLeInt32( sis );
	    bih->bihImportantColors	= sioEndianGetLeInt32( sis );
	    sioEndianGetLeInt16( sis );
	    sioEndianGetLeInt16( sis );
	    sioEndianGetLeInt16( sis );
	    sioEndianGetLeInt16( sis );
	    sioEndianGetLeInt32( sis );
	    sioEndianGetLeInt32( sis );
	    sioEndianGetLeInt32( sis );
	    sioEndianGetLeInt32( sis );
      
	    /*  3  */
	    switch( bih->bihBitsPerPixel )
		{
		case 24:
		case 16:
		    if  ( bih->bihColorCount != 0 )
			{ goto palette64;	}
		    break;

		palette64:
		case 8:
		case 4:
		case 2:
		case 1:
		    if  ( bih->bihColorCount == 0 )
			{
			colorCount=
			    ( 1U << (bih->bihPlanes * bih->bihBitsPerPixel ));
			}
		    else{ colorCount= bih->bihColorCount; }

		    palette= (RGB8Color *)malloc(
					colorCount * sizeof(RGB8Color) );

		    if  ( ! palette )
			{ XDEB(palette); return -1;	}

		    if  ( bmBmpRead4ColorTable( colorCount, sis, palette ) )
			{ XDEB(sis); free( palette ); return -1; }

		    paletteBytes= 4* colorCount;
		    break;

		default:
		    LDEB(bih->bihBitsPerPixel); return -1;
		}
	    break;

	default:
	    LDEB(bih->bihSizeOfRestOfHeader); return -1;
	}

    *pPalette= palette;

    return bih->bihSizeOfRestOfHeader+ paletteBytes;
    }

/************************************************************************/
/*									*/
/*  Read the file header as it appears in a BMP file.			*/
/*									*/
/************************************************************************/

static int bmpReadFileHeader(	BmpImageHeader *	bih,
				SimpleInputStream *	sis,
				RGB8Color **		pPalette )
    {
    int			done;

    bih->bihMagic = sioEndianGetLeInt16( sis );

    if  ( bih->bihMagic != BMP_ID )
	{ LLDEB(bih->bihMagic,BMP_ID); return -1; }

    bih->bihFileSize= sioEndianGetLeInt32( sis );
    bih->bihReserved1= sioEndianGetLeInt16( sis );
    bih->bihReserved2= sioEndianGetLeInt16( sis );
    bih->bihBitmapDataOffset= sioEndianGetLeInt32( sis );

    done= bmpReadImageHeader( bih, sis, pPalette );
    if  ( done < 0 )
	{ LDEB(done); return -1;	}

    return 2+ 4+ 2+ 2+ 4+ done;
    }

/************************************************************************/
/*									*/
/*  Read an uncompresse image from a BMP file.				*/
/*									*/
/************************************************************************/

static int bmBmpReadUncompressed(
				SimpleInputStream *	sis,
				unsigned char *		buffer,
				int			pixelsHigh,
				int			bytesPerRowB,
				int			bytesPerRowF,
				int			bitsPerPixelF )
    {
    int		row;
    int		col;
    int		done= 0;

    switch( bitsPerPixelF )
	{
	case 1: case 2: case 4: case 8:
	    if  ( bytesPerRowB != bytesPerRowF )
		{ LLDEB(bytesPerRowB,bytesPerRowF); return -1;	}

	    for ( row= 0; row < pixelsHigh; row++ )
		{
		unsigned char *	to;

		to= buffer+ ( pixelsHigh- row- 1 )* bytesPerRowB;

		for ( col= 0; col < bytesPerRowF; col++ )
		    { (*to++)= sioInGetByte( sis ); done++; }

		while( col % 4 )
		    { (void)sioInGetByte( sis ); done++; col++;	}
		}
	    return done;

	case 32:
	    if  ( bytesPerRowB != bytesPerRowF )
		{ LLDEB(bytesPerRowB,bytesPerRowF); return -1;	}

	    for ( row= 0; row < pixelsHigh; row++ )
		{
		unsigned char *	to;

		to= buffer+ ( pixelsHigh- row- 1 )* bytesPerRowB;

		for ( col= 0; col < bytesPerRowF; to += 4, done += 4, col += 4 )
		    {
		    to[2]= sioInGetByte( sis );
		    to[1]= sioInGetByte( sis );
		    to[0]= sioInGetByte( sis );
		    to[3]= sioInGetByte( sis );
		    }
		}
	    return done;

	case 24:
	    if  ( bytesPerRowB != bytesPerRowF )
		{ LLDEB(bytesPerRowB,bytesPerRowF); return -1;	}

	    for ( row= 0; row < pixelsHigh; row++ )
		{
		unsigned char *	to;

		to= buffer+ ( pixelsHigh- row- 1 )* bytesPerRowB;

		for ( col= 0; col < bytesPerRowF; to += 3, done += 3, col += 3 )
		    {
		    to[2]= sioInGetByte( sis );
		    to[1]= sioInGetByte( sis );
		    to[0]= sioInGetByte( sis );
		    }

		while( col % 4 )
		    { (void)sioInGetByte( sis ); done++; col++;	}
		}
	    return done;

	case 16:
	    for ( row= 0; row < pixelsHigh; row++ )
		{
		unsigned char *	to;

		to= buffer+ ( pixelsHigh- row- 1 )* bytesPerRowB;

		for ( col= 0; col < bytesPerRowF; to += 3, done += 2, col += 2 )
		    {
		    unsigned char	b0= sioInGetByte( sis );
		    unsigned char	b1= sioInGetByte( sis );
		    int			b= 256* b1+ b0;

		    to[0]= ( 255* ( ( b >> 10 ) & 0x1f  ) )/ 31;
		    to[1]= ( 255* ( ( b >>  5 ) & 0x1f  ) )/ 31;
		    to[2]= ( 255* ( ( b >>  0 ) & 0x1f  ) )/ 31;
		    }

		while( col % 4 )
		    { (void)sioInGetByte( sis ); done++; col++;	}
		}
	    return done;

	default:
	    LDEB(bitsPerPixelF); return -1;
	}
    }

/************************************************************************/
/*									*/
/*  Translate a microsoft header to our bitmap descriptors.		*/
/*									*/
/*  1)  Boolean to value.						*/
/*									*/
/************************************************************************/

int bmpHeaderToDescription(	BitmapDescription *	bd,
				int *			pBytesPerRow,
				const BmpImageHeader *	bih,
				const RGB8Color *	palette,
				int			assumeAlpha )
    {
    /*  1  */
    if  ( assumeAlpha )
	{ assumeAlpha= 1; }

    bd->bdPixelsWide= bih->bihPixelsWide;
    bd->bdPixelsHigh= bih->bihPixelsHigh;

    bd->bdBitsPerSample= 8;
    bd->bdSamplesPerPixel= 3+ assumeAlpha;

    switch( bih->bihBitsPerPixel )
	{
	case 32:
	    if  ( bih->bihColorCount != 0 )
		{ goto palette;	}

	    bd->bdColorEncoding= BMcoRGB;
	    assumeAlpha= 1;
	    break;

	case 24:
	    if  ( bih->bihColorCount != 0 )
		{ goto palette;	}

	    bd->bdColorEncoding= BMcoRGB;
	    break;

	case 16:
	    if  ( bih->bihColorCount != 0 )
		{ goto palette;	}

	    bd->bdColorEncoding= BMcoRGB;
	    break;

	palette:
	case 8:
	case 4:
	case 2:
	case 1:
	    {
	    int		colorCount;

	    if  ( bih->bihColorCount == 0 )
		{
		colorCount=
		    ( 1U << (bih->bihPlanes * bih->bihBitsPerPixel ));
		}
	    else{ colorCount= bih->bihColorCount; }

	    bd->bdColorEncoding= BMcoRGB8PALETTE;
	    bd->bdBitsPerPixel= ( 1+ assumeAlpha )* bih->bihBitsPerPixel;

	    if  ( utilPaletteSetCount( &(bd->bdPalette), colorCount ) )
		{ LDEB(colorCount); return -1;	}

	    memcpy( bd->bdPalette.cpColors, palette,
			    bd->bdPalette.cpColorCount* sizeof( RGB8Color ) );
	    break;
	    }

	default:
	    LDEB(bih->bihBitsPerPixel); return -1;
	}

    bd->bdHasAlpha= assumeAlpha;

    if  ( bmCalculateSizes( bd ) )
	{ LDEB(1); return -1;	}

    *pBytesPerRow= ( bih->bihBitsPerPixel* bd->bdPixelsWide + 7 )/ 8;

    bd->bdUnit= BMunM;
    bd->bdXResolution= bih->bihXResolution;
    bd->bdYResolution= bih->bihYResolution;

    /********************************************************************/
    /*  Check people that even run microsoft systems in their brain.	*/
    /********************************************************************/
    if  ( bd->bdXResolution < 500 || bd->bdXResolution < 500 )
	{ bd->bdYResolution= bd->bdXResolution= 1; bd->bdUnit= BMunPIXEL; }

    return 0;
    }

/************************************************************************/
/*									*/
/*  Convert a bitmap description to a Bitmap header.			*/
/*									*/
/************************************************************************/

int bmbmpDescriptionToHeader(	BmpImageHeader *		bih,
				RGB8Color **			pPalette,
				int				privateFormat,
				int				hasAlpha,
				const BitmapDescription *	bd )
    {
    RGB8Color			BWPalette[257];
    RGB8Color *			palette= (RGB8Color *)0;

    bih->bihMagic= BMP_ID;
    bih->bihFileSize= 0;		/*  Set below.	*/
    bih->bihReserved1= 0;
    bih->bihReserved2= 0;
    bih->bihBitmapDataOffset= 0;	/*  Set below.	*/
    bih->bihSizeOfRestOfHeader= privateFormat;

    bih->bihPixelsWide= bd->bdPixelsWide;
    bih->bihPixelsHigh= bd->bdPixelsHigh;

    bih->bihPlanes= 1;
    bih->bihBitsPerPixel= 0;	/*  Set Below.	*/
    bih->bihCompression= 0;	/*  Set Below.	*/

    bih->bihBufferLength= 0;	/*  Set Below.	*/

    switch( bd->bdUnit )
	{
	case BMunM:
	    bih->bihXResolution= bd->bdXResolution;
	    bih->bihYResolution= bd->bdYResolution;
	    break;

	case BMunINCH:
	    bih->bihXResolution= 40* bd->bdXResolution;
	    bih->bihYResolution= 40* bd->bdYResolution;
	    break;

	case BMunPOINT:
	    bih->bihXResolution= POINTS_PER_M* bd->bdXResolution;
	    bih->bihYResolution= POINTS_PER_M* bd->bdYResolution;
	    break;

	case BMunPIXEL:
	    bih->bihXResolution= 0;
	    bih->bihYResolution= 0;
	    break;

	default:
	    LDEB(bd->bdUnit); return -1;
	}

    bih->bihColorCount= 0;	/*  Set Below.	*/
    bih->bihImportantColors= 0;	/*  Set Below.	*/

    switch( bd->bdColorEncoding )
	{
	int	colorCount;
	int	transparentColor;

	case BMcoBLACKWHITE:
	case BMcoWHITEBLACK:
	    if  ( bmMakeGrayPalette( bd, &colorCount,
					&transparentColor, BWPalette, 257 ) )
		{ LDEB(bd->bdBitsPerPixel); return -1;	}
			
	    switch( bd->bdBitsPerPixel )
		{
		case 1:
		    bih->bihBitsPerPixel= bd->bdBitsPerPixel;
		    bih->bihColorCount= colorCount;
		    bih->bihCompression= 0;
		    palette= BWPalette;
		    break;

		case 2:
		case 4:
		    bih->bihBitsPerPixel= 4;
		    bih->bihColorCount= colorCount;
		    bih->bihCompression= 2;
		    palette= BWPalette;
		    break;

		case 8:
		    bih->bihBitsPerPixel= bd->bdBitsPerPixel;
		    bih->bihColorCount= colorCount;
		    bih->bihCompression= 1;
		    palette= BWPalette;
		    break;

		default:
		    LDEB(bd->bdBitsPerPixel); return -1;
		}
	    break;

	case BMcoRGB:
	    if  ( bd->bdSamplesPerPixel != 3	||
		  bd->bdBitsPerSample != 8	||
		  bd->bdBitsPerPixel != 24	)
		{
		LLDEB(bd->bdSamplesPerPixel,bd->bdBitsPerSample != 8);
		return -1;
		}
	    bih->bihBitsPerPixel= 24;
	    bih->bihColorCount= 0;
	    bih->bihCompression= 0;
	    break;

	case BMcoRGB8PALETTE:
	    switch( bd->bdBitsPerPixel )
		{
		int		i;

		case 16:
		    if  ( hasAlpha )
			{
			bih->bihBitsPerPixel= 8;
			bih->bihCompression= 1;
			}
		    else{
			LLDEB(bd->bdColorEncoding,bd->bdBitsPerPixel);
			return -1;
			}
		    bih->bihColorCount= bd->bdPalette.cpColorCount;
		    palette= bd->bdPalette.cpColors;
		    break;

		case 8:
		    if  ( hasAlpha )
			{ bih->bihBitsPerPixel= 4; }
		    else{ bih->bihBitsPerPixel= 8; }
		    bih->bihCompression= 1;
		    bih->bihColorCount= bd->bdPalette.cpColorCount;
		    palette= bd->bdPalette.cpColors;
		    break;

		case 4:
		    if  ( hasAlpha )
			{
			LLDEB(bd->bdColorEncoding,bd->bdBitsPerPixel);
			return -1;
			}
		    else{ bih->bihBitsPerPixel= 4; }
		    bih->bihCompression= 2;
		    bih->bihColorCount= bd->bdPalette.cpColorCount;
		    palette= bd->bdPalette.cpColors;
		    break;

		case 2:
		    LDEB(bd->bdPalette.cpColorCount);
		    for ( i= 0; i < bd->bdPalette.cpColorCount; i++ )
			{ RGB8DEB(bd->bdPalette.cpColors+i);	}
		default:
		    LDEB(hasAlpha);
		    LLDEB(bd->bdColorEncoding,bd->bdBitsPerPixel);
		    return -1;
		}
		break;

	default:
	    LDEB(bd->bdColorEncoding); return -1;
	}

    *pPalette= palette; return 0;
    }

/************************************************************************/
/*									*/
/*  Read the data bytes from a Microsoft BMP image.			*/
/*									*/
/************************************************************************/

int bmBmpReadImageBytes(	unsigned char **		pBuffer,
				SimpleInputStream *		sis,
				int				compression,
				int				bytesPerRowF,
				int				bitsPerPixelF,
				const BitmapDescription *	bd )
    {
    int			rval= -1;

    int			done;
    int			bRead= 0;

    int			x;
    int			y;
    int			bytesPerRowB= bd->bdBytesPerRow;
    int			high= bd->bdPixelsHigh;

    int			c;
    int			n;
    unsigned char	v;

    unsigned char *	to;

    unsigned char *	buffer= (unsigned char *)0;

    buffer= (unsigned char *)malloc( bd->bdBufferLength+ 3 );
    if  ( ! buffer )
	{ LLDEB(bd->bdBufferLength,buffer); rval= -1; goto ready;	}

    switch( compression )
	{
	case COMPRESS_NONE:
	    done= bmBmpReadUncompressed( sis, buffer,
					    high, bytesPerRowB, bytesPerRowF,
					    bitsPerPixelF );

	    if  ( done < 0 )
		{ LDEB(done); rval= -1; goto ready;	}

	    bRead += done;
	    break;
	case COMPRESS_RLE8:
	    memset( buffer, '\0', bd->bdBufferLength );

	    x= y= 0;
	    to= buffer+ x+ ( high- y- 1 )* bytesPerRowB;

	    while( y < high )
		{
		c= sioInGetByte( sis ); bRead++;

		if  ( c > 0 )
		    {
		    v= sioInGetByte( sis ); bRead++;
		    while( c > 0 )
			{ *(to++)= v; x++; c--;	}

		    continue;
		    }

		c= sioInGetByte( sis ); bRead++;

		switch( c )
		    {
		    case 0x00:
			x= 0; y++;
			to= buffer+ x+ ( high- y- 1 )* bytesPerRowB;
			continue;
		    case 0x01:
			break;
		    case 0x02:
			c= sioInGetByte( sis ); bRead++; x += c;
			c= sioInGetByte( sis ); bRead++; y += c;
			to= buffer+ x+ ( high- y- 1 )* bytesPerRowB;
			continue;
		    default:
			while( c > 1 )
			    {
			    *(to++)= sioInGetByte( sis ); bRead++; x++;
			    *(to++)= sioInGetByte( sis ); bRead++; x++;
			    c -= 2;
			    }
			if  ( c > 0 )
			    {
			    *(to++)= sioInGetByte( sis ); bRead++; x++;
			    (void)sioInGetByte( sis ); bRead++;
			    }
			continue;
		    }

		break;
		}

	    break;
	case COMPRESS_RLE4:
	    memset( buffer, '\0', bd->bdBufferLength );
	    x= y= 0;  
	    to= buffer + x/2 + ( high- y- 1 )* bytesPerRowB;

	    while( y < high )
		{
		c= sioInGetByte( sis ); bRead++;
		if  ( c == EOF )
		    { LDEB(c); break;	}

		if  ( c > 0 )
		    {
		    v= sioInGetByte( sis ); bRead++;

		    if  ( x+ c > bd->bdPixelsWide )
			{
			LLLDEB(x,c,bd->bdPixelsWide);
			c= bd->bdPixelsWide- x;
			}

		    if  ( x % 2 )
			{
			unsigned char	vv;

			*(to++) |= ( v >> 4 ); c--;
			vv= ( v >> 4 ) | ( v << 4 );

			while( c > 1 )
			    { *(to++)= vv; x += 2; c -= 2;	}

			if  ( c > 0 )
			    { *(to)= v; x++; }
			}
		    else{
			while( c > 1 )
			    { *(to++)= v; x += 2; c -= 2;	}

			if  ( c > 0 )
			    { *(to)= v; x++; }
			}
		    continue;
		    }

		n= c= sioInGetByte( sis ); bRead++;
		switch( c )
		    {
		    case 0x00:
			x= 0; y++;
			to= buffer + x/2 + ( high- y- 1 )* bytesPerRowB;
			continue;
		    case 0x01:
			break;
		    case 0x02:
			c= sioInGetByte( sis ); bRead++;
			if  ( c > 127 )
			    { c= 256- c;	}
			x += c;
			if  ( x < 0 || x >= bd->bdPixelsWide )
			    {
			    LLLDEB(x,c,bd->bdPixelsWide);
			    x= ( x+ 256* bd->bdPixelsWide ) % bd->bdPixelsWide;
			    }

			c= sioInGetByte( sis ); bRead++;
			if  ( c > 127 )
			    { c= 256- c;	}
			y += c;
			if  ( y < 0 || y >= bd->bdPixelsHigh )
			    {
			    LLLDEB(x,c,bd->bdPixelsHigh);
			    x= ( x+ 256* bd->bdPixelsHigh ) % bd->bdPixelsHigh;
			    }

			to= buffer + x/2 + ( high- y- 1 )* bytesPerRowB;
			continue;

		    default:
			if  ( x % 2 )
			    {
			    v= sioInGetByte( sis ); bRead++;
			    *(to++) |= ( v >> 4 ); c--; x++;

			    while( c > 1 )
				{
				*(to)= ( v << 4 );
				v= sioInGetByte( sis ); bRead++;
				*(to++) |= ( v >> 4 );
				c -= 2; x += 2;
				}
			    if  ( c )
				{ (void)sioInGetByte( sis ); bRead++;	}
			    }
			else{
			    while( c > 1 )
				{
				v= sioInGetByte( sis ); bRead++;
				*(to++)= v; c -= 2; x += 2;
				}
			    if  ( c )
				{ (void)sioInGetByte( sis ); bRead++;	}
			    }

			if  ( ( n & 3 ) == 1 || ( n & 3 ) == 2 )
			    { (void)sioInGetByte( sis ); bRead++; }
			continue;
		    }

		break;
		}
	    break;

	default:
	    LDEB(compression); rval= -1; goto ready;
	}

    *pBuffer= buffer; buffer= (unsigned char *)0;
    rval= bRead;

  ready:

    if  ( buffer )
	{ free( buffer );	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Read a Microsoft BMP file.						*/
/*									*/
/*  1)	Read and check header.						*/
/*  2)  Translate header to a BitmapDescription.			*/
/*  3)  Seek to data. ( We are supposed to be there ).			*/
/*									*/
/************************************************************************/

int bmBmpReadDibX(	BitmapDescription *	pBd,
			unsigned char **	pBuffer,
			BmpImageHeader *	bih,
			const RGB8Color *	palette,
			SimpleInputStream *	sis )
    {
    int			rval= -1;
    int			bytesPerRowF;

    unsigned char *	buffer= (unsigned char *)0;
    BitmapDescription	bd;

    int			done;
    int			bytesRead= 0;

    bmInitDescription( &bd );

    if  ( bmpHeaderToDescription( &bd, &bytesPerRowF, bih, palette, 0 ) )
	{ LDEB(1); goto ready;	}

    done= bmBmpReadImageBytes( &buffer, sis,
				bih->bihCompression, bytesPerRowF,
				bih->bihBitsPerPixel, &bd );
    if  ( done < 0 )
	{ LDEB(bd.bdBufferLength); goto ready; }

    bytesRead += done;

    /* Try to make monochrome */
    if  ( bih->bihColorCount > 0 )
	{ bmMakeMonochrome( &bd, buffer );	}

    *pBd= bd; *pBuffer= buffer;

    bmInitDescription( &bd ); /* steal */
    buffer= (unsigned char *)0;
    rval= bytesRead;

  ready:

    bmCleanDescription( &bd );
    if  ( buffer )
	{ free( buffer );	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Extract an image from data, e.g. from a windows  metafile in a	*/
/*  word processing document.						*/
/*									*/
/************************************************************************/

int bmBmpReadDib(	BitmapDescription *	bd,
			unsigned char **	pBuffer,
			SimpleInputStream *	sis )
    {
    int			rval= 0;
    BmpImageHeader	bih;
    RGB8Color *		palette= (RGB8Color *)0;

    int			done;
    int			bytesRead= 0;

    /*  1  */
    done= bmpReadImageHeader( &bih, sis, &palette );
    if  ( done < 0 )
	{ LDEB(1); rval= -1; goto ready;	}
    bytesRead += done;

    done= bmBmpReadDibX( bd, pBuffer, &bih, palette, sis );
    if  ( done < 0 )
	{ LDEB(1); rval= -1; goto ready;	}
    bytesRead += done;

    rval= bytesRead;

  ready:

    if  ( palette )
	{ free( palette );	}

    return rval;
    }

int bmReadBmpFile(	const MemoryBuffer *	filename,
			unsigned char **	pBuffer,
			BitmapDescription *	bd,
			int *			pPrivateFormat )
    {
    int			rval= 0;
    SimpleInputStream *	sis= (SimpleInputStream *)0;
    BmpImageHeader	bih;
    RGB8Color *		palette= (RGB8Color *)0;

    int			done;

    sis= sioInFileioOpen( filename );
    if  ( ! sis )
	{ XDEB(sis); rval= -1; goto ready;	}

    /*  1  */
    done= bmpReadFileHeader( &bih, sis, &palette );
    if  ( done < 0 )
	{ LDEB(1); rval= -1; goto ready;	}

    done= bmBmpReadDibX( bd, pBuffer, &bih, palette, sis );
    if  ( done < 0 )
	{ LDEB(1); rval= -1; goto ready;	}

    *pPrivateFormat= bih.bihSizeOfRestOfHeader;

  ready:

    if  ( sis )
	{ sioInClose( sis );	}
    if  ( palette )
	{ free( palette );	}

    return rval;
    }

/************************************************************************/
/*  Write a Microsoft BMP header.					*/
/*  1)  Write fields common to all formats.				*/
/*  2)  If it is not a direct color image, write palette.		*/
/************************************************************************/

int bmpWriteImageHeader(	const BmpImageHeader *	bih,
				SimpleOutputStream *	sos,
				const RGB8Color *	palette )
    {
    int		paletteBytes= 0;

    sioEndianPutLeInt32( bih->bihSizeOfRestOfHeader, sos );

    switch( bih->bihSizeOfRestOfHeader )
	{
	case 12:		/*  OS/2 1.*	*/
	    sioEndianPutLeInt16( bih->bihPixelsWide, sos );
	    sioEndianPutLeInt16( bih->bihPixelsHigh, sos );
	    sioEndianPutLeInt16( bih->bihPlanes, sos );
	    sioEndianPutLeInt16( bih->bihBitsPerPixel, sos );

	    /*  2  */
	    if  ( bih->bihColorCount > 0	&&
		  palette			)
		{
		if  ( bmBmpWrite3ColorTable( bih->bihColorCount,
							    sos, palette ) )
		    { XDEB(sos); return -1; }

		paletteBytes= 3* bih->bihColorCount;
		}
	    break;
	case 40:		/*  Windows 3.*	*/
	    sioEndianPutLeInt32( bih->bihPixelsWide, sos );
	    sioEndianPutLeInt32( bih->bihPixelsHigh, sos );
	    sioEndianPutLeInt16( bih->bihPlanes, sos );
	    sioEndianPutLeInt16( bih->bihBitsPerPixel, sos );
	    sioEndianPutLeInt32( bih->bihCompression, sos );
	    sioEndianPutLeInt32( bih->bihBufferLength, sos );
	    sioEndianPutLeInt32( bih->bihXResolution, sos );
	    sioEndianPutLeInt32( bih->bihYResolution, sos );
	    sioEndianPutLeInt32( bih->bihColorCount, sos );
	    sioEndianPutLeInt32( bih->bihImportantColors, sos );

	    /*  2  */
	    if  ( bih->bihColorCount > 0	&&
		  palette			)
		{
		if  ( bmBmpWrite4ColorTable( bih->bihColorCount,
							    sos, palette ) )
		    { XDEB(sos); return -1; }

		paletteBytes= 4* bih->bihColorCount;
		}
	    break;
	case 64:		/*  OS/2 2.0 */
	    sioEndianPutLeInt32( bih->bihPixelsWide, sos );
	    sioEndianPutLeInt32( bih->bihPixelsHigh, sos );
	    sioEndianPutLeInt16( bih->bihPlanes, sos );
	    sioEndianPutLeInt16( bih->bihBitsPerPixel, sos );
	    sioEndianPutLeInt32( bih->bihCompression, sos );
	    sioEndianPutLeInt32( bih->bihBufferLength, sos );
	    sioEndianPutLeInt32( bih->bihXResolution, sos );
	    sioEndianPutLeInt32( bih->bihYResolution, sos );
	    sioEndianPutLeInt32( bih->bihColorCount, sos );
	    sioEndianPutLeInt32( bih->bihImportantColors, sos );
	    sioEndianPutLeInt16( 0, sos );
	    sioEndianPutLeInt16( 0, sos );
	    sioEndianPutLeInt16( 0, sos );
	    sioEndianPutLeInt16( 0, sos );
	    sioEndianPutLeInt32( 0L, sos );
	    sioEndianPutLeInt32( 0L, sos );
	    sioEndianPutLeInt32( 0L, sos );
	    sioEndianPutLeInt32( 0L, sos );
      
	    /*  2  */
	    if  ( bih->bihColorCount > 0	&&
		  palette			)
		{
		if  ( bmBmpWrite4ColorTable( bih->bihColorCount,
							    sos, palette ) )
		    { XDEB(sos); return -1; }

		paletteBytes= 4* bih->bihColorCount;
		}
	    break;
	default:
	    LDEB(bih->bihSizeOfRestOfHeader); return -1;
	}

    return bih->bihSizeOfRestOfHeader+ paletteBytes;
    }

static int bmpWriteFileHeader(	const BmpImageHeader *	bih,
				SimpleOutputStream *	sos,
				const RGB8Color *	palette )
    {
    int		bytesWritten;

    /*  1  */
    sioEndianPutLeInt16( bih->bihMagic, sos );

    sioEndianPutLeInt32( bih->bihFileSize, sos );
    sioEndianPutLeInt16( bih->bihReserved1, sos );
    sioEndianPutLeInt16( bih->bihReserved2, sos );
    sioEndianPutLeInt32( bih->bihBitmapDataOffset, sos );

    bytesWritten= bmpWriteImageHeader( bih, sos, palette );

    if  ( bytesWritten < 0 )
	{ LDEB(bytesWritten); return -1;	}

    return 2+ 4+ 2+ 2+ 4+ bytesWritten;
    }

int bmpSizeImageHeader(	const BmpImageHeader *	bih,
			RGB8Color *		palette )
    {
    int		paletteBytes= 0;

    switch( bih->bihSizeOfRestOfHeader )
	{
	case 12:		/*  OS/2 1.*	*/
	    /*  2  */
	    if  ( bih->bihColorCount > 0	&&
		  palette			)
		{ paletteBytes= 3* bih->bihColorCount;	}
	    break;
	case 40:		/*  Windows 3.*	*/
	    /*  2  */
	    if  ( bih->bihColorCount > 0	&&
		  palette			)
		{ paletteBytes= 4* bih->bihColorCount;	}
	    break;
	case 64:		/*  OS/2 2.0 */
	    /*  2  */
	    if  ( bih->bihColorCount > 0	&&
		  palette			)
		{ paletteBytes= 4* bih->bihColorCount;	}
	    break;
	default:
	    LDEB(bih->bihSizeOfRestOfHeader); return -1;
	}

    return bih->bihSizeOfRestOfHeader+ paletteBytes;
    }

static int bmpSizeFileHeader(	const BmpImageHeader *	bih,
				RGB8Color *		palette )
    {
    int		bytesNeeded;

    bytesNeeded= bmpSizeImageHeader( bih, palette );

    if  ( bytesNeeded < 0 )
	{ LDEB(bytesNeeded); return -1;	}

    return 2+ 4+ 2+ 2+ 4+ bytesNeeded;
    }

/************************************************************************/
/*									*/
/*  Write different kinds of scanlines to a Microsoft BMP file.		*/
/*									*/
/*  1)	Count number of identical bytes.				*/
/*  2)	Repeating run.							*/
/*  3)	Count literal run.						*/
/*  3)	Literal run. (Pixel count must be at least 3)			*/
/*  2a)	If possible extend the literal to even length.			*/
/*  2b)	Literals should have even lenghth, pad if necessary.		*/
/*  3)	Repeating run.							*/
/*									*/
/************************************************************************/

static int bmBmpWriteScanLine(	SimpleOutputStream *	sos,
				int			pixelsPerByte,
				int			maxByteCount,
				const unsigned char *	buffer,
				int			npixels,
				int			nbytes	)
    {
    int			done= 0;
    int			byteCount;
    int			pixelCount;
    unsigned char	byteValue;

    while( npixels > 0 )
	{
	byteValue= buffer[0];
	byteCount= 1; pixelCount= pixelsPerByte;

	/*  1  */
	while( byteCount < nbytes		&&
	       byteCount < maxByteCount		&&
	       buffer[byteCount] == byteValue	)
	    { byteCount++; pixelCount += pixelsPerByte;	}

	if  ( pixelsPerByte == 2					&&
	      pixelCount == npixels -1					&&
	      ( buffer[byteCount] & 0xf0 ) == ( byteValue & 0xf0 )	)
	    { byteCount++; pixelCount++; }

	/*  2  */
	if  ( byteCount > 1 )
	    {
	    if  ( pixelCount > npixels )
		{ pixelCount=  npixels;	}

	    if  ( sioOutPutByte( pixelCount, sos ) )
		{ return -1;	}
	    if  ( sioOutPutByte( byteValue, sos ) )
		{ return -1;	}
	    buffer += byteCount;
	    nbytes -= byteCount; npixels -= pixelCount;
	    done += 2; continue;
	    }

	/*  3  */
	while( byteCount+ 1 < nbytes			&&
	       byteCount+ 1 < maxByteCount		&&
	       buffer[byteCount+1] != buffer[byteCount]	)
	    { byteCount++; pixelCount += pixelsPerByte;	}

	if  ( byteCount+ 1 == nbytes			&&
	      byteCount+ 1 < maxByteCount		)
	    { byteCount++; pixelCount += pixelsPerByte; }

	if  ( pixelCount > npixels )
	    { pixelCount=  npixels;	}

	switch( pixelCount )
	    {
	    int		i;

	    default:
		if  ( sioOutPutByte( 0, sos ) < 0 )
		    { return -1;	}
		if  ( sioOutPutByte( pixelCount, sos ) < 0 )
		    { return -1;	}
		for ( i= 0; i < byteCount; i++ )
		    {
		    if  ( sioOutPutByte( *(buffer++), sos ) < 0 )
			{ return -1;	}

		    done++;
		    }
		if  ( byteCount % 2 )
		    {
		    if  ( sioOutPutByte( 0, sos ) < 0 )
			{ return -1;	}
		    done++;
		    }
		done += 2;
		nbytes -= byteCount; npixels -= pixelCount;
		continue;
	    case 1:
		if  ( sioOutPutByte( pixelCount, sos ) < 0 )
		    { return -1;	}
		if  ( sioOutPutByte( *(buffer++), sos ) < 0 )
		    { return -1;	}
		done += 2;
		nbytes -= byteCount; npixels -= pixelCount;
		continue;
	    case 2:
		if  ( pixelsPerByte == 2 )
		    {
		    if  ( sioOutPutByte( pixelCount, sos ) < 0 )
			{ return -1;	}
		    if  ( sioOutPutByte( *(buffer++), sos ) < 0 )
			{ return -1;	}
		    done += 2;
		    nbytes -= byteCount; npixels -= pixelCount;
		    continue;
		    }
		if  ( pixelsPerByte == 1 )
		    {
		    if  ( sioOutPutByte( 1, sos ) < 0 )
			{ return -1;	}
		    if  ( sioOutPutByte( *(buffer++), sos ) < 0 )
			{ return -1;	}
		    if  ( sioOutPutByte( 1, sos ) < 0 )
			{ return -1;	}
		    if  ( sioOutPutByte( *(buffer++), sos ) < 0 )
			{ return -1;	}
		    done += 4;
		    nbytes -= byteCount; npixels -= pixelCount;
		    continue;
		    }
		LLDEB(pixelCount,pixelsPerByte); return -1;
	    case 0:
		LLDEB(pixelCount,pixelsPerByte); return -1;
	    }
	}

    if  ( sioOutPutByte( 0, sos ) < 0 )
	{ return -1;	}
    if  ( sioOutPutByte( 0, sos ) < 0 )
	{ return -1;	}
    done += 2;

    return done;
    }

/************************************************************************/
/*									*/
/*  Write the image bytes of a BMP file.				*/
/*									*/
/************************************************************************/

int bmBmpSaveImageBytes(	MemoryBuffer *			bmpBytes,
				const BitmapDescription *	bd,
				const unsigned char *		buffer,
				int				compression )
    {
    int				done;
    int				bytesWritten= 0;

    int				row;
    int				i;
    const unsigned char *	from;

    int				maxByteCount;
    int				bytesPerRow;
    unsigned char *		inflated= (unsigned char *)0;

    int				white= -1;

    SimpleOutputStream *	sos= sioOutMemoryOpen( bmpBytes );

    if  ( ! sos )
	{ XDEB(sos); bytesWritten= -1; goto ready;	}

    switch( compression )
	{
	case COMPRESS_NONE:
	    for ( row= 0; row < bd->bdPixelsHigh; row++ )
		{
		from= buffer+ ( bd->bdPixelsHigh- row- 1 )* bd->bdBytesPerRow;

		if  ( bd->bdBitsPerPixel == 24 )
		    {
		    int		col;

		    for ( col= 0; col < bd->bdBytesPerRow; from += 3, col += 3 )
			{
			if  ( sioOutPutByte( from[2], sos ) < 0 )
			    { bytesWritten= -1; goto ready;	}
			if  ( sioOutPutByte( from[1], sos ) < 0 )
			    { bytesWritten= -1; goto ready;	}
			if  ( sioOutPutByte( from[0], sos ) < 0 )
			    { bytesWritten= -1; goto ready;	}
			}
		    }
		else{
		    if  ( sioOutWriteBytes( sos, from, bd->bdBytesPerRow ) !=
							    bd->bdBytesPerRow )
			{ LDEB(bd->bdBytesPerRow); bytesWritten= -1; goto ready; }
		    }

		bytesWritten += bd->bdBytesPerRow;

		for ( i = bd->bdBytesPerRow; i % 4; i++ )
		    {
		    if  ( sioOutPutByte( '\0', sos ) < 0 )
			{ bytesWritten= -1; goto ready;	}
		    bytesWritten++;
		    }
		}
	    break;
	case COMPRESS_RLE8:
	case COMPRESS_RLE4:
	    bytesPerRow= bd->bdBytesPerRow;

	    if  ( compression == 2		&&
		  bd->bdBitsPerPixel == 2	)
		{
		inflated= (unsigned char *)malloc( 2* bd->bdBytesPerRow );
		if  ( ! inflated )
		    { LXDEB(bd->bdBytesPerRow,inflated); bytesWritten= -1; goto ready;	}

		bytesPerRow *= 2;
		}

	    if  ( compression == 1		&&
		  bd->bdHasAlpha		&&
		  bd->bdBitsPerPixel == 16	)
		{
		bytesPerRow= bytesPerRow/ 2;

		inflated= (unsigned char *)malloc( bytesPerRow );
		if  ( ! inflated )
		    { LXDEB(bd->bdBytesPerRow,inflated); bytesWritten= -1; goto ready;	}

		for ( i= 0; i < bd->bdPalette.cpColorCount; i++ )
		    {
		    if  ( bd->bdPalette.cpColors[i].rgb8Red == 255	&&
			  bd->bdPalette.cpColors[i].rgb8Green == 255	&&
			  bd->bdPalette.cpColors[i].rgb8Blue == 255	)
			{ white= i; break;	}
		    }

		if  ( white < 0 )
		    { LDEB(white); white= 0;	}
		}

	    maxByteCount= 255/ compression;
	    for ( row= 0; row < bd->bdPixelsHigh; row++ )
		{
		unsigned char *	to;

		from= buffer+ ( bd->bdPixelsHigh- row- 1 )* bd->bdBytesPerRow;

		if  ( compression == 2		&&
		      bd->bdBitsPerPixel == 2	)
		    {
		    to= inflated;

		    for ( i= 0; i < bd->bdBytesPerRow; to += 2, from++, i++ )
			{
			to[0]= ( ( from[0] & 0x30 ) >> 4 )	|
			       ( ( from[0] & 0xc0 ) >> 2 )	;
			to[1]= ( ( from[0] & 0x03 ) << 0 )	|
			       ( ( from[0] & 0x0c ) << 2 )	;
			}

		    from= inflated;
		    }

		if  ( compression == 1		&&
		      bd->bdHasAlpha		&&
		      bd->bdBitsPerPixel == 16	)
		    {
		    to= inflated;

		    for ( i= 0; i < bytesPerRow; from += 2, to++, i++ )
			{
			if  ( from[1] )
			    { *to= *from;	}
			else{ *to= white;	}
			}

		    from= inflated;
		    }

		done= bmBmpWriteScanLine( sos, compression, maxByteCount,
				from, bd->bdPixelsWide, bytesPerRow );

		if  ( done < 0 )
		    { LDEB(row); bytesWritten= -1; goto ready; }

		bytesWritten += done;
		}

	    if  ( sioOutPutByte( 0, sos ) < 0 )
		{ bytesWritten= -1; goto ready;	}
	    if  ( sioOutPutByte( 1, sos ) < 0 )
		{ bytesWritten= -1; goto ready;	}
	    bytesWritten += 2;

	    break;

	default:
	    LDEB(compression); bytesWritten= -1; goto ready;
	}

  ready:
    if  ( inflated )
	{ free( inflated );	}
    if  ( sos )
	{ sioOutClose( sos );	}

    return bytesWritten;
    }

/************************************************************************/
/*									*/
/*  Write a Microsoft BMP file.						*/
/*									*/
/************************************************************************/

int bmCollectDib(		const BitmapDescription *	bd,
				const unsigned char *		buffer,
				BmpImageHeader *		bih,
				MemoryBuffer *			bmpBytes,
				RGB8Color **			pPalette,
				int				privateFormat,
				int				fileHeader )
    {
    RGB8Color *			palette= (RGB8Color *)0;

    int				done;
    int				bytesNeeded= 0;

    if  ( bmbmpDescriptionToHeader( bih, &palette,
					privateFormat, bd->bdHasAlpha, bd ) )
	{ LDEB(privateFormat); return -1;	}

    if  ( fileHeader )
	{
	done= bmpSizeFileHeader( bih, palette );
	if  ( done < 0 )
	    { LDEB(bytesNeeded); return -1; }
	}
    else{
	done= bmpSizeImageHeader( bih, palette );
	if  ( done < 0 )
	    { LDEB(bytesNeeded); return -1; }
	}

    bytesNeeded += done;
    bih->bihBitmapDataOffset= bytesNeeded;

    done= bmBmpSaveImageBytes( bmpBytes, bd, buffer, bih->bihCompression );
    if  ( done < 0 )
	{ LDEB(done); return -1;	}

    bytesNeeded += done;

    bih->bihFileSize= bytesNeeded;
    bih->bihBufferLength= done;

    *pPalette= palette;
    return bytesNeeded;
    }

int bmWriteBmpFile(	const MemoryBuffer *		filename,
			const unsigned char *		buffer,
			const BitmapDescription *	bd,
			int				privateFormat )
    {
    int				rval= 0;

    SimpleOutputStream *	sosFile= (SimpleOutputStream *)0;
    int				done;
    int				needed;

    int				size;
    const unsigned char *	bytes;

    BmpImageHeader		bih;
    RGB8Color *			palette= (RGB8Color *)0;
    MemoryBuffer		bmpBytes;

    const int			fileHeader= 1;

    utilInitMemoryBuffer( &bmpBytes );

    needed= bmCollectDib( bd, buffer, &bih, &bmpBytes, &palette,
						privateFormat, fileHeader );
    if  ( needed < 0 )
	{ LLDEB(privateFormat,needed); rval= -1; goto ready;	}

    sosFile= sioOutFileioOpen( filename );
    if  ( ! sosFile )
	{ XDEB(sosFile);  rval= -1; goto ready;	}

    if  ( bmpWriteFileHeader( &bih, sosFile, palette ) < 0 )
	{ LDEB(1); return -1; }

    bytes= utilMemoryBufferGetBytes( &size, &bmpBytes );
    done= sioOutWriteBytes( sosFile, bytes, size );
    if  ( done != size )
	{ LLDEB(done,size); return -1;	}

  ready:
    utilCleanMemoryBuffer( &bmpBytes );
    if  ( sosFile )
	{ sioOutClose( sosFile ); 	}

    return rval;
    }

int bmCanWriteBmpFile( const BitmapDescription *	bd,
			int				privateFormat )
    {
    if  ( bd->bdHasAlpha )
	{ return -1;	}

    return 0;
    }

