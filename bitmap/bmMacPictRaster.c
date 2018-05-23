#   include	"bitmapConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>

#   include	<sioEndian.h>
#   include	"bmio.h"

#   include	<appDebugon.h>

/************************************************************************/

static int bmMacPictGetRect(		DocumentRectangle *	dr,
					SimpleInputStream *	sis )
    {
    int		done= 0;

    dr->drY0= sioEndianGetBeInt16( sis ); done += 2;
    dr->drX0= sioEndianGetBeInt16( sis ); done += 2;
    dr->drY1= sioEndianGetBeInt16( sis ); done += 2;
    dr->drX1= sioEndianGetBeInt16( sis ); done += 2;

    return done;
    }

/************************************************************************/
/*									*/
/*  Read the colors in a mac pict palette.				*/
/*									*/
/************************************************************************/

static int bmMacPictReadPaletteColors(	BitmapDescription *	bd,
					int			colorCount,
					int			flags,
					SimpleInputStream *	sis )
    {
    int		col;

    RGB8Color *	rgb8;

    int		bytesRead= 0;

    if  ( utilPaletteSetCount( &(bd->bdPalette), colorCount ) )
	{ LDEB(colorCount); return -1;	}

    rgb8= bd->bdPalette.cpColors;
    for ( col= 0; col < colorCount; rgb8++, col++ )
	{
	int	idx;

	idx= sioEndianGetBeInt16( sis ); bytesRead += 2;
	if  ( idx < 0 )
	    { LDEB(idx); idx= 0;			}
	if  ( idx >= colorCount )
	    { LLDEB(idx,colorCount); idx= colorCount- 1; }

	if  ( flags & 0x8000 )
	    { idx= col;	}

	rgb8->rgb8Red= sioEndianGetBeUint16( sis )/ 256; bytesRead += 2;
	rgb8->rgb8Green= sioEndianGetBeUint16( sis )/ 256; bytesRead += 2;
	rgb8->rgb8Blue= sioEndianGetBeUint16( sis )/ 256; bytesRead += 2;
	}

    return bytesRead;
    }

/************************************************************************/
/*									*/
/*  Read one row of PackBits data.					*/
/*									*/
/************************************************************************/

static int bmMacPictReadPacBitsRow(
				unsigned char *			to,
				int				bytesPerRow,
				const BitmapDescription *	bd,
				SimpleInputStream *		sis )
    {
    int			byteCount;
    int			done= 0;
    int			bytesRead= 0;

    if  ( bytesPerRow > 250 )
	{ byteCount= sioEndianGetBeInt16( sis ); bytesRead += 2;	}
    else{ byteCount= sioInGetByte( sis ); bytesRead++;		}

    /*
    if  ( byteCount > bd->bdBytesPerRow )
	{ LLDEB(byteCount,bd->bdBytesPerRow); return -1;	}
    */

    bytesRead += byteCount;

    while( byteCount > 0 )
	{
	int	c0= sioInGetByte( sis );
	int	len;
	int	use;

	if  ( c0 == EOF )
	    { LDEB(c0); return -1;	}

	if  ( c0 == 0x80 )
	    { XDEB(c0); byteCount--; continue;	}

	if  ( c0 & 0x80 )
	    {
	    int		c1= sioInGetByte( sis );

	    len= 257- c0; use= len;

	    if  ( done+ len > bd->bdBytesPerRow )
		{
		if  ( ! ( bd->bdBytesPerRow % 2 )	||
		      done+ len > bd->bdBytesPerRow+ 1	)
		    { LLLDEB(done,len,bd->bdBytesPerRow);	}

		use= bd->bdBytesPerRow- done;
		}

	    byteCount -= 2; done += use;

	    while( use > 0 )
		{ *(to++)= c1; use--; }
	    }
	else{
	    len= c0+ 1; use= len;

	    if  ( done+ len > bd->bdBytesPerRow )
		{
		if  ( ! ( bd->bdBytesPerRow % 2 )	||
		      done+ len > bd->bdBytesPerRow+ 1	)
		    { LLLDEB(done,len,bd->bdBytesPerRow);	}

		use= bd->bdBytesPerRow- done;
		}

	    byteCount -= 1+ len; done += use;

	    while( use > 0 )
		{ *(to++)= sioInGetByte( sis ); use--; len--; }
	    while( len > 0 )
		{ (void) sioInGetByte( sis ); len--; }
	    }
	}

    return bytesRead;
    }

/************************************************************************/

static int bmMacPictReadPacBitsImage(
				int				bytesPerRow,
				RasterImage *		abi,
				SimpleInputStream *		sis )
    {
    int				rval= -1;
    int				bytesRead= 0;
    int				row;
    const BitmapDescription *	bd= &(abi->riDescription);

    for ( row= 0; row < bd->bdPixelsHigh; row++ )
	{
	int		step;
	unsigned char *	to;

	to= abi->riBytes+ row* bd->bdBytesPerRow;

	step= bmMacPictReadPacBitsRow( to, bytesPerRow, bd, sis );
	if  ( step < 0 )
	    { LLDEB(row,step); goto ready; }
	bytesRead += step;
	}

    rval= bytesRead;

  ready:

    return rval;
    }

static int bmMacPictReadPlanarPacBitsImage(
				int				bytesPerRow,
				RasterImage *		abi,
				SimpleInputStream *		sis )
    {
    int				rval= -1;
    int				bytesRead= 0;
    int				row;
    const BitmapDescription *	bd= &(abi->riDescription);
    unsigned char *		scratch= (unsigned char *)0;

    scratch= (unsigned char *)malloc( bd->bdBytesPerRow );
    if  ( ! scratch )
	{ LPDEB(bd->bdBytesPerRow,scratch); goto ready;	}

    for ( row= 0; row < bd->bdPixelsHigh; row++ )
	{
	int			step;
	unsigned char *		to;

	const unsigned char *	r;
	const unsigned char *	g;
	const unsigned char *	b;
	int			i;

	step= bmMacPictReadPacBitsRow( scratch, bytesPerRow, bd, sis );
	if  ( step < 0 )
	    { LLDEB(row,step); return -1; }
	bytesRead += step;

	r= scratch;
	g= r+ bd->bdPixelsWide;
	b= g+ bd->bdPixelsWide;

	to= abi->riBytes+ row* bd->bdBytesPerRow;

	for ( i= 0; i < bd->bdPixelsWide; i++ )
	    {
	    *(to++)= *(r++);
	    *(to++)= *(g++);
	    *(to++)= *(b++);
	    }
	}

    rval= bytesRead;

  ready:

    if  ( scratch )
	{ free( scratch );	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  MACPICT_PackBitsRect (And similar images)				*/
/*									*/
/*  2)  Get bytes per row and palette flag.				*/
/*  3)  Get image frame.						*/
/*  4)  Default is a black and white image.				*/
/*  5)  For color images, get additional information.			*/
/*  6)  Retrieve palette.						*/
/*  7)  Finish bitmap administration and allocate buffer.		*/
/*  8)  Read source and destination rectangle.				*/
/*  9)  Read raster operation. (Assume copy anyway)			*/
/*  10) Read bitmap data.						*/
/*  11) Later versions of mac pict have an even number of bytes per	*/
/*	operation.							*/
/*									*/
/************************************************************************/

int bmMacPictGetImage(		RasterImage *	abi,
				DocumentRectangle *	drSource,
				DocumentRectangle *	drDest,
				SimpleInputStream *	sis,
				int			packed,
				int			direct,
				int			clipRegion )
    {
    unsigned int	bytesPerRow;
    int			hasPalette= 0;
    int			step;

    int			rop;
    DocumentRectangle	drImage;

    BitmapDescription *	bd= &(abi->riDescription);

    int			bytesRead= 0;
    int			rowsArePlanar= 0;

    /*  2  */
    if  ( direct )
	{
	bytesPerRow= 0;

	(void) sioEndianGetBeUint16( sis ); bytesRead += 2;
	(void) sioEndianGetBeUint16( sis ); bytesRead += 2;
	(void) sioEndianGetBeUint16( sis ); bytesRead += 2;
	}
    else{
	bytesPerRow= sioEndianGetBeUint16( sis ); bytesRead += 2;
	if  ( bytesPerRow & 0x8000 )
	    {
	    hasPalette= 1;
	    bytesPerRow &= ~0x8000;
	    }
	}

    /*  3  */
    step= bmMacPictGetRect( &drImage, sis );
    if  ( step < 0 )
	{ LDEB(step); return -1;	}
    bytesRead += step;

    bd->bdPixelsWide= drImage.drX1- drImage.drX0;
    bd->bdPixelsHigh= drImage.drY1- drImage.drY0;

    bd->bdBytesPerRow= bytesPerRow;

    /*  4  */
    bd->bdBitsPerPixel= 1;
    bd->bdSamplesPerPixel= 1;
    bd->bdBitsPerSample= 1;
    bd->bdColorEncoding= BMcoBLACKWHITE;
    bd->bdUnit= BMunPIXEL;

    /*  5  */
    if  ( direct || hasPalette )
	{
	int		version;
	int		packType;
	long		packSize;
	int		xRes;
	int		yRes;
	int		pixelType;
	int		planeBytes;
	int		table;
	int		reserved;

	version= sioEndianGetBeInt16( sis ); bytesRead += 2;
	packType= sioEndianGetBeInt16( sis ); bytesRead += 2;
	if  ( packType == 4 )
	    { packed= 1;	}
	packSize= sioEndianGetBeInt32( sis ); bytesRead += 4;
	xRes= sioEndianGetBeInt32( sis ); bytesRead += 4;
	yRes= sioEndianGetBeInt32( sis ); bytesRead += 4;
	pixelType= sioEndianGetBeInt16( sis ); bytesRead += 2;
	bd->bdBitsPerPixel= sioEndianGetBeInt16( sis ); bytesRead += 2;
	bd->bdSamplesPerPixel= sioEndianGetBeInt16( sis ); bytesRead += 2;
	bd->bdBitsPerSample= sioEndianGetBeInt16( sis ); bytesRead += 2;
	planeBytes= sioEndianGetBeInt32( sis ); bytesRead += 4;
	table= sioEndianGetBeInt32( sis ); bytesRead += 4;
	reserved= sioEndianGetBeInt32( sis ); bytesRead += 4;

	if  ( bd->bdSamplesPerPixel == 4 )
	    { bd->bdHasAlpha= 1;	}

	if  ( direct )
	    {
	    bytesPerRow=
		( bd->bdBitsPerPixel* bd->bdPixelsWide+ 7 )/ 8;
	    }

	if  ( direct && bd->bdSamplesPerPixel == 3 )
	    {
	    bd->bdColorEncoding= BMcoRGB;
	    if  ( bd->bdSamplesPerPixel == 3 && bd->bdBitsPerSample == 8 )
		{ rowsArePlanar= 1;	}
	    else{
LDEB(bd->bdColorEncoding);
LDEB(bd->bdBitsPerPixel);
LDEB(bd->bdBitsPerSample);
LDEB(bytesPerRow);
LDEB(bd->bdPixelsWide);
		}

	    bd->bdBitsPerSample= 8;
	    bd->bdBitsPerPixel= 24;
	    }

	version= version;
	packType= packType;
	packSize= packSize;
	xRes= xRes;
	yRes= yRes;
	pixelType= pixelType;
	planeBytes= planeBytes;
	table= table;
	reserved= reserved;
	}

    /*  6  */
    if  ( hasPalette )
	{
	int		flags;
	int		colorCount;
	int		step;

	/*seed=*/ (void) sioEndianGetBeInt32( sis ); bytesRead += 4;
	flags= sioEndianGetBeUint16( sis ); bytesRead += 2;
	colorCount= sioEndianGetBeInt16( sis )+ 1; bytesRead += 2;

	bd->bdColorEncoding= BMcoRGB8PALETTE;

	step= bmMacPictReadPaletteColors( bd, colorCount, flags, sis );
	if  ( step < 0 )
	    { LLDEB(hasPalette,step); return -1;	}
	bytesRead += step;
	}

    /*  7  */
    bmCalculateSizes( bd );

    abi->riBytes= (unsigned char *)malloc( bd->bdBufferLength );
    if  ( ! abi->riBytes )
	{ LXDEB(bd->bdBufferLength,abi->riBytes); return -1; }

    /*  8  */
    step= bmMacPictGetRect( drSource, sis );
    if  ( step < 0 )
	{ LDEB(step); return -1;	}
    bytesRead += step;

    /*  Cope with the fact the the image rectangle is one pixel	*/
    /*  big, and also with the fact that it may be shifted	*/
    drSource->drX0 -= drImage.drX0;
    drSource->drX1 -= drImage.drX0;
    drSource->drY0 -= drImage.drY0;
    drSource->drY1 -= drImage.drY0;
    drImage.drX1 -= drImage.drX0+ 1; drImage.drX0= 0;
    drImage.drY1 -= drImage.drY0+ 1; drImage.drY0= 0;

    if  ( ! geoIntersectRectangle( drSource, drSource, &drImage ) )
	{ RECTDEB(&drImage); return -1;	}

    step= bmMacPictGetRect( drDest, sis );
    if  ( step < 0 )
	{ LDEB(step); return -1;	}
    bytesRead += step;


    /*  9  */
    rop= sioEndianGetBeInt16( sis ); bytesRead += 2;
    if  ( rop != 0 )
	{ LDEB(rop);	}

    if  ( clipRegion )
	{
	int	i;
	int	count= sioEndianGetBeInt16( sis ); bytesRead += 2;

	for ( i= 2; i < count; i++ )
	    { (void) sioInGetByte( sis ); bytesRead++; }
	}

    /*  10  */
    if  ( ! packed || bytesPerRow < 8 )
	{
	int		row;

	if  ( bd->bdColorEncoding == BMcoRGB )
	    {
LLDEB(bytesPerRow,bd->bdBytesPerRow);
if(bytesPerRow%2)
LDEB(bytesPerRow);
	    for ( row= 0; row < bd->bdPixelsHigh; row++ )
		{
		unsigned char *	to;
		int		i;

		to= abi->riBytes+ row* bd->bdBytesPerRow;
		for ( i= 0; i < bytesPerRow; i += 2 )
		    {
		    unsigned char	b0= sioInGetByte( sis );
		    unsigned char	b1= sioInGetByte( sis );
		    int			b= 256* b1+ b0;

		    to[0]= ( 255* ( ( b >> 10 ) & 0x1f  ) )/ 31;
		    to[1]= ( 255* ( ( b >>  5 ) & 0x1f  ) )/ 31;
		    to[2]= ( 255* ( ( b >>  0 ) & 0x1f  ) )/ 31;

		    to += 3; bytesRead += 2;
		    }
		}
	    }
	else{
	    for ( row= 0; row < bd->bdPixelsHigh; row++ )
		{
		unsigned char *	to;
		int		i;

		to= abi->riBytes+ row* bd->bdBytesPerRow;

		for ( i= 0; i < bd->bdBytesPerRow && i < bytesPerRow; i++ )
		    { *(to++)= sioInGetByte( sis ); bytesRead++;	}
		for ( ; i < bytesPerRow; i++ )
		    { (void)sioInGetByte( sis ); bytesRead++;	}
		}
	    }
	}
    else{
	if  ( rowsArePlanar )
	    {
	    step= bmMacPictReadPlanarPacBitsImage( bytesPerRow, abi, sis );
	    if  ( step < 0 )
		{ LDEB(step); return -1;	}
	    bytesRead += step;
	    }
	else{
	    step= bmMacPictReadPacBitsImage( bytesPerRow, abi, sis );
	    if  ( step < 0 )
		{ LDEB(step); return -1;	}
	    bytesRead += step;
	    }
	}

#   if 0
    {
    static int	N= 1;
    char	scratch[20];
    int		format;

    sprintf( scratch, "/tmp/macpict-%d.png", N++ );
    format= bmSuggestFormat( scratch, -1, bd );
    bmWrite( scratch, abi->riBytes, bd, format );
    }
#   endif

    return bytesRead;
    }

