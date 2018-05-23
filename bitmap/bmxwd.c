#   include	"bitmapConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>
#   include	"bmintern.h"
#   include	<sioFileio.h>
#   include	<sioEndian.h>
#   include	<appDebugon.h>

#   include	"XWDFile.h"

/************************************************************************/
/*  Implementing routines.						*/
/************************************************************************/
static int bmReadX11wd(	SimpleInputStream *	sis,
			XWDFileHeader *		xh,
			unsigned char **	pBuf,
			BitmapDescription *	bd );

/************************************************************************/
/*  Read an XWD file.							*/
/************************************************************************/

int bmReadXwdFile(	const MemoryBuffer *	filename,
			unsigned char **	pBuf,
			BitmapDescription *	bd,
			int *			pPrivateFormat )
    {
    SimpleInputStream *		sis= (SimpleInputStream *)0;
    int				rval= 0;
    int				privateFormat= -1;
    int				done= 0;

    XWDFileHeader		x11Header;

    sis= sioInFileioOpen( filename );
    if  ( ! sis )
	{ XDEB(sis); rval= -1; goto ready;	}

    /***************/
    x11Header.header_size= sioEndianGetBeInt32( sis ); done += 4;
    x11Header.file_version= sioEndianGetBeInt32( sis ); done += 4;
    x11Header.pixmap_format= sioEndianGetBeInt32( sis ); done += 4;
    x11Header.pixmap_depth= sioEndianGetBeInt32( sis ); done += 4;
    x11Header.pixmap_width= sioEndianGetBeInt32( sis ); done += 4;
    x11Header.pixmap_height= sioEndianGetBeInt32( sis ); done += 4;
    x11Header.xoffset= sioEndianGetBeInt32( sis ); done += 4;
    x11Header.byte_order= sioEndianGetBeInt32( sis ); done += 4;
    x11Header.bitmap_unit= sioEndianGetBeInt32( sis ); done += 4;
    x11Header.bitmap_bit_order= sioEndianGetBeInt32( sis ); done += 4;
    x11Header.bitmap_pad= sioEndianGetBeInt32( sis ); done += 4;
    x11Header.bits_per_pixel= sioEndianGetBeInt32( sis ); done += 4;
    x11Header.bytes_per_line= sioEndianGetBeInt32( sis ); done += 4;
    x11Header.visual_class= sioEndianGetBeInt32( sis ); done += 4;
    x11Header.red_mask= sioEndianGetBeInt32( sis ); done += 4;
    x11Header.green_mask= sioEndianGetBeInt32( sis ); done += 4;
    x11Header.blue_mask= sioEndianGetBeInt32( sis ); done += 4;
    x11Header.bits_per_rgb= sioEndianGetBeInt32( sis ); done += 4;
    x11Header.colormap_entries= sioEndianGetBeInt32( sis ); done += 4;
    x11Header.ncolors= sioEndianGetBeInt32( sis ); done += 4;
    x11Header.window_width= sioEndianGetBeInt32( sis ); done += 4;
    x11Header.window_height= sioEndianGetBeInt32( sis ); done += 4;
    x11Header.window_x= sioEndianGetBeInt32( sis ); done += 4;
    x11Header.window_y= sioEndianGetBeInt32( sis ); done += 4;
    x11Header.window_bdrwidth= sioEndianGetBeInt32( sis ); done += 4;
    /***************/

    /*  skip name */
    while( done < x11Header.header_size )
	{
	if  ( sioInGetByte( sis ) == EOF )
	    { LDEB(EOF); rval= -1; goto ready;	}

	done++;
	}

    switch( x11Header.file_version )
	{
	case XWD_FILE_VERSION:
	    privateFormat= 11;
	    rval= bmReadX11wd( sis, &x11Header, pBuf, bd );
	    break;
	case XWD_FILE_VERSION << 24:
	    XDEB(x11Header.file_version); sioInClose( sis ); return -1;
	default:
	    XDEB(x11Header.file_version); sioInClose( sis ); return -1;
	}

  ready:

    if  ( sis )
	{ sioInClose( sis );	}

    if  ( ! rval )
	{ *pPrivateFormat= privateFormat;	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Read an X11 window dump file.					*/
/*									*/
/************************************************************************/

static int bmReadX11wd(	SimpleInputStream *	sis,
			XWDFileHeader *		xh,
			unsigned char **	pBuf,
			BitmapDescription *	bd )
    {
    int			rval= 0;

    int			row;
    int			col;

    unsigned char *	buf= (unsigned char *)0;
    unsigned char *	to;

    unsigned long	rm= 0, gm= 0, bm= 0;
    int			rs= 0, gs= 0, bs= 0;
    XWDColor *		colors= (XWDColor *)0;

    if  ( xh->ncolors > 0 )
	{
	unsigned int	cp;

	colors= (XWDColor *)malloc( xh->ncolors* sizeof(XWDColor) );
	if  ( ! colors )
	    { LXDEB(xh->ncolors,colors); rval= -1; goto ready;	}

	for ( cp= 0; cp < xh->ncolors; cp++ )
	    {
	    colors[cp].pixel= sioEndianGetBeInt32( sis );
	    colors[cp].red= sioEndianGetBeInt16( sis );
	    colors[cp].green= sioEndianGetBeInt16( sis );
	    colors[cp].blue= sioEndianGetBeInt16( sis );
	    colors[cp].flags= sioInGetByte( sis );
	    colors[cp].pad= sioInGetByte( sis );
	    }
	}

    bd->bdPixelsWide= xh->pixmap_width;
    bd->bdPixelsHigh= xh->pixmap_height;

    bd->bdUnit= BMunPIXEL;
    bd->bdXResolution= 1;
    bd->bdYResolution= 1;

    switch( xh->visual_class )
	{
	int	cp;

	case StaticGray:
	case GrayScale:
	    if  ( xh->bits_per_pixel == 1 )
		{ bd->bdColorEncoding= BMcoBLACKWHITE;	}
	    else{ bd->bdColorEncoding= BMcoWHITEBLACK;	}

	    bd->bdBitsPerSample= xh->bits_per_pixel;
	    bd->bdSamplesPerPixel= 1;

	    bd->bdUnit= BMunPIXEL;
	    bd->bdXResolution= 1;
	    bd->bdYResolution= 1;

	    break;

	case StaticColor:
	case PseudoColor:
	    bd->bdColorEncoding= BMcoRGB8PALETTE;
	    bd->bdBitsPerSample= 8;
	    bd->bdSamplesPerPixel= 3;

	    if  ( utilPaletteSetCount( &(bd->bdPalette), xh->ncolors ) )
		{ LDEB(xh->ncolors); rval= -1; goto ready;	}

	    if  ( colors )
		{
		for ( cp= 0; cp < xh->ncolors; cp++ )
		    {
		    bd->bdPalette.cpColors[cp].rgb8Red= colors[cp].red/256;
		    bd->bdPalette.cpColors[cp].rgb8Green= colors[cp].green/256;
		    bd->bdPalette.cpColors[cp].rgb8Blue= colors[cp].blue/256;
		    bd->bdPalette.cpColors[cp].rgb8Alpha= 255;
		    }
		}

	    break;

	case TrueColor:
	case DirectColor:
	    bd->bdColorEncoding= BMcoRGB;
	    bd->bdBitsPerSample= xh->bits_per_rgb;
	    bd->bdSamplesPerPixel= 3;

	    rm= xh->red_mask;
	    if  ( rm == 0 )
		{ LDEB(rm); rval= -1; goto ready;	}
	    else{
		while( ! ( rm & 1 ) )
		    { rm >>= 1; rs++; }
		}

	    gm= xh->green_mask;
	    if  ( gm == 0 )
		{ LDEB(gm); rval= -1; goto ready;	}
	    else{
		while( ! ( gm & 1 ) )
		    { gm >>= 1; gs++; }
		}

	    bm= xh->blue_mask;
	    if  ( bm == 0 )
		{ LDEB(bm); rval= -1; goto ready;	}
	    else{
		while( ! ( bm & 1 ) )
		    { bm >>= 1; bs++; }
		}

	    if  ( xh->ncolors > 0			&&
	    	  ( xh->ncolors < rm+ 1		||
		    xh->ncolors < gm+ 1		||
		    xh->ncolors < bm+ 1		)	)
		{ LLLLDEB(xh->ncolors,rm,gm,bm); rval= -1; goto ready;	}
	    break;
	}

    switch( xh->bits_per_pixel )
	{
	case 1:
	    if  ( bd->bdColorEncoding != BMcoBLACKWHITE )
		{ LDEB(bd->bdColorEncoding); rval= -1; goto ready;	}

	    bd->bdBufferLength= xh->bytes_per_line* xh->pixmap_height;
	    bd->bdBitsPerSample= 1;
	    bd->bdBytesPerRow= xh->bytes_per_line;
	    bd->bdBitsPerPixel= xh->bits_per_pixel;

	    buf= (unsigned char *)malloc( bd->bdBufferLength );
	    if  ( ! buf )
		{ LLDEB(bd->bdBufferLength,buf); rval= -1; goto ready; }

	    if  ( sioInReadBytes( sis, buf, bd->bdBufferLength ) !=
							bd->bdBufferLength )
		{ LDEB(bd->bdBufferLength); rval= -1; goto ready; }
	    break;

	case	8:
	    if  ( bd->bdColorEncoding != BMcoRGB8PALETTE	&&
		  bd->bdColorEncoding != BMcoWHITEBLACK		)
		{ LDEB(bd->bdColorEncoding); rval= -1; goto ready;	}

	    bd->bdBufferLength= xh->bytes_per_line* xh->pixmap_height;
	    bd->bdBytesPerRow= xh->bytes_per_line;
	    bd->bdBitsPerPixel= xh->bits_per_pixel;

	    buf= (unsigned char *)malloc( bd->bdBufferLength );
	    if  ( ! buf )
		{ LLDEB(bd->bdBufferLength,buf); rval= -1; }
	    else{
		if  ( sioInReadBytes( sis, buf, bd->bdBufferLength ) !=
							bd->bdBufferLength )
		    { LDEB(bd->bdBufferLength); rval= -1; goto ready; }
		}

	    if  ( bd->bdColorEncoding != BMcoRGB8PALETTE )
		{
		if  ( xh->ncolors > 0 && colors )
		    {
		    if  ( xh->ncolors != 256 )
			{ LDEB(xh->ncolors); rval= -1; goto ready;	}

		    to= buf;
		    for ( col= 0; col < bd->bdBufferLength; col++ )
			{ *to= colors[*to].green/256; to++;	}
		    }
		}
	    break;

	case	16:
	    if  ( bd->bdColorEncoding != BMcoRGB )
		{ LDEB(bd->bdColorEncoding); rval= -1; goto ready;	}
	    if  ( rm == 0 || gm == 0 || bm == 0 )
		{ LLLDEB(rm,gm,bm); rval= -1; goto ready;	}


	    bd->bdBitsPerSample= 8;
	    bd->bdSamplesPerPixel= 3;

	    bmCalculateSizes( bd );

	    to= buf= (unsigned char *)malloc( bd->bdBufferLength );
	    if  ( ! to )
		{ LLDEB(bd->bdBufferLength,to); rval= -1; }

	    for ( row= 0; row < bd->bdPixelsHigh; row++ )
		{
		int	got= 0;

		for ( col= 0; col < xh->xoffset; col++ )
		    { sioInGetByte( sis );	}
		got += xh->xoffset;

		for ( col= 0; col < bd->bdPixelsWide; col++ )
		    {
		    unsigned long	pix;
		    int			r, g, b;

		    if  ( xh->byte_order == LSBFirst )
			{ pix= sioEndianGetLeInt16( sis );	}
		    else{ pix= sioEndianGetBeInt16( sis );	}

		    r= ( pix >> rs ) & rm;
		    g= ( pix >> gs ) & gm;
		    b= ( pix >> bs ) & bm;

		    if  ( xh->ncolors > 0 && colors )
			{
			*(to++)= colors[r].red/ 256;
			*(to++)= colors[g].green/ 256;
			*(to++)= colors[b].blue/ 256;
			}
		    else{
			*(to++)= ( 256* r )/ rm;
			*(to++)= ( 256* g )/ gm;
			*(to++)= ( 256* b )/ bm;
			}

		    got += 2;
		    }

		while( got < xh->bytes_per_line )
		    { sioInGetByte( sis ); got++;	}
		}

	    break;

	case	32:
	    if  ( bd->bdColorEncoding != BMcoRGB )
		{ LDEB(bd->bdColorEncoding); rval= -1; goto ready;	}
	    if  ( rm == 0 || gm == 0 || bm == 0 )
		{ LLLDEB(rm,gm,bm); rval= -1; goto ready;	}

	    bd->bdBitsPerSample= 8;
	    bd->bdSamplesPerPixel= 3;

	    bmCalculateSizes( bd );

	    to= buf= (unsigned char *)malloc( bd->bdBufferLength );
	    if  ( ! to )
		{ LLDEB(bd->bdBufferLength,to); rval= -1; }

	    for ( row= 0; row < bd->bdPixelsHigh; row++ )
		{
		int	got= 0;

		for ( col= 0; col < xh->xoffset; col++ )
		    { sioInGetByte( sis );	}
		got += xh->xoffset;

		for ( col= 0; col < bd->bdPixelsWide; col++ )
		    {
		    unsigned long	pix;
		    int			r, g, b;

		    if  ( xh->byte_order == LSBFirst )
			{ pix= sioEndianGetLeInt32( sis );	}
		    else{ pix= sioEndianGetBeInt32( sis );	}

		    r= ( pix >> rs ) & rm;
		    g= ( pix >> gs ) & gm;
		    b= ( pix >> bs ) & bm;

		    if  ( xh->ncolors > 0 && colors )
			{
			*(to++)= colors[r].red/ 256;
			*(to++)= colors[g].green/ 256;
			*(to++)= colors[b].blue/ 256;
			}
		    else{
			*(to++)= ( 256* r )/ rm;
			*(to++)= ( 256* g )/ gm;
			*(to++)= ( 256* b )/ bm;
			}

		    got += 4;
		    }

		while( got < xh->bytes_per_line )
		    { sioInGetByte( sis ); got++;	}
		}

	    break;

	default:
	    LLLDEB(xh->bits_per_pixel,xh->bitmap_unit,xh->bits_per_rgb);
	    rval= -1; goto ready;
	}

    *pBuf= buf; buf= (unsigned char *)0; /* steal */

  ready:

    if  ( buf )
	{ free( buf );	}
    if  ( colors )
	{ free( colors );	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Write an XWD file.							*/
/*									*/
/************************************************************************/

int bmWriteXwdFile(	const MemoryBuffer *		filename,
			const unsigned char *		buffer,
			const BitmapDescription *	bd,
			int				privateFormat )
    { LDEB(-1); return -1; }
