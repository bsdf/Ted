#   include	"bitmapConfig.h"

#   include	"bmintern.h"
#   include	<stdlib.h>
#   include	<string.h>
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Expand a palette from one buffer to another one. Do it in such a	*/
/*  way ( reverse order ) that it works with from == to.		*/
/*									*/
/************************************************************************/

#   define	setBytes(t,p,f)	\
    { *(t--)= p[f].rgb8Blue; *(t--)= p[f].rgb8Green; *(t--)= p[f].rgb8Red; }
#   define	setAlpha(t,p,f)	\
    { *(t--)= (f)?0xff:0x00; }

int bmExpandRGB8Palette(	unsigned char *		to,
				const unsigned char *	from,
				int			pixelsWide,
				int			bitsPerPixel,
				const ColorPalette *	cp,
				int			hasAlpha )
    {
    unsigned char	f1, f2;

    if  ( hasAlpha )
	{ to += 4* pixelsWide- 1;	}
    else{ to += 3* pixelsWide- 1;	}

    from += ( bitsPerPixel* pixelsWide - 1 )/8;

    switch( bitsPerPixel )
	{
	case	16:
	    if  ( ! hasAlpha )
		{ LLDEB(bitsPerPixel,hasAlpha); return -1; }

	    while( pixelsWide > 0 )
		{
		*(to--)= *(from--);
		f1= *(from--);
		setBytes(to,cp->cpColors,f1);
		pixelsWide--;
		}
	    break;

	case	8:
	    if  ( hasAlpha )
		{
		while( pixelsWide > 0 )
		    {
		    f1= *(from--);
		    *(to--)= ( ( f1 & 0x0f ) << 4 ) | ( f1 & 0x0f );
		    f2= ( f1 >> 4 ) & 0x0f;	setBytes(to,cp->cpColors,f2);
		    pixelsWide--;
		    }
		}
	    else{
		while( pixelsWide > 0 )
		    {
		    f1= *(from--);
		    setBytes(to,cp->cpColors,f1);
		    pixelsWide--;
		    }
		}
	    break;

	case	4:
	    if  ( hasAlpha )
		{ LDEB(hasAlpha); return -1;	}
	    while( pixelsWide > 0 )
		{
		f1= *(from--);
		f2=   f1        & 0x0f;	setBytes(to,cp->cpColors,f2);
		if  ( pixelsWide == 1 )
		    { break;	}
		f2= ( f1 >> 4 ) & 0x0f;	setBytes(to,cp->cpColors,f2);
		pixelsWide -= 2;
		}
	    break;

	case	2:
	    if  ( hasAlpha )
		{
		while( pixelsWide > 0 )
		    {
		    f1= *(from--);
		    f2=   f1        & 0x01;	setAlpha(to,cp->cpColors,f2);
		    f2= ( f1 >> 1 ) & 0x01;	setBytes(to,cp->cpColors,f2);
		    f2= ( f1 >> 2 ) & 0x01;	setAlpha(to,cp->cpColors,f2);
		    f2= ( f1 >> 3 ) & 0x01;	setBytes(to,cp->cpColors,f2);
		    f2= ( f1 >> 4 ) & 0x01;	setAlpha(to,cp->cpColors,f2);
		    f2= ( f1 >> 5 ) & 0x01;	setBytes(to,cp->cpColors,f2);
		    f2= ( f1 >> 6 ) & 0x01;	setAlpha(to,cp->cpColors,f2);
		    f2= ( f1 >> 7 ) & 0x01;	setBytes(to,cp->cpColors,f2);
		    pixelsWide -= 4;
		    }
		}
	    else{
		while( pixelsWide > 0 )
		    {
		    f1= *(from--);
		    f2=   f1        & 0x03;	setBytes(to,cp->cpColors,f2);
		    f2= ( f1 >> 2 ) & 0x03;	setBytes(to,cp->cpColors,f2);
		    f2= ( f1 >> 4 ) & 0x03;	setBytes(to,cp->cpColors,f2);
		    f2= ( f1 >> 6 ) & 0x03;	setBytes(to,cp->cpColors,f2);
		    pixelsWide -= 4;
		    }
		}
	    break;

	case	1:
	    while( pixelsWide > 0 )
		{
		f1= *(from--);
		f2=   f1        & 0x01;	setBytes(to,cp->cpColors,f2);
		f2= ( f1 >> 1 ) & 0x01;	setBytes(to,cp->cpColors,f2);
		f2= ( f1 >> 2 ) & 0x01;	setBytes(to,cp->cpColors,f2);
		f2= ( f1 >> 3 ) & 0x01;	setBytes(to,cp->cpColors,f2);
		f2= ( f1 >> 4 ) & 0x01;	setBytes(to,cp->cpColors,f2);
		f2= ( f1 >> 5 ) & 0x01;	setBytes(to,cp->cpColors,f2);
		f2= ( f1 >> 6 ) & 0x01;	setBytes(to,cp->cpColors,f2);
		f2= ( f1 >> 7 ) & 0x01;	setBytes(to,cp->cpColors,f2);
		pixelsWide -= 8;
		}
	    break;

	default:
	    LDEB(bitsPerPixel); return -1;
	}

    return 0;
    }

int bmExpandPaletteImage(	RasterImage *			riOut,
				const RasterImage *		riIn,
				int				ignoredInt )
    {
    const BitmapDescription *	bdIn= &(riIn->riDescription);
    int				rval= 0;
    int				row;
    const unsigned char *	from;
    unsigned char *		to;

    RasterImage			ri;

    bmInitRasterImage( &ri );

    switch( bdIn->bdColorEncoding )
	{
	case BMcoBLACKWHITE:
	case BMcoWHITEBLACK:
	case BMcoRGB:
	    LDEB(bdIn->bdColorEncoding);
	    rval= -1; goto ready;

	case BMcoRGB8PALETTE:
	    break;
	}

    /************************************************************/
    /*  Derive properties of output bitmap from input.		*/
    /************************************************************/
    if  ( bmCopyDescription( &(ri.riDescription), bdIn ) )
	{ LDEB(1); rval= -1; goto ready;	}

    ri.riDescription.bdColorEncoding= BMcoRGB;
    ri.riDescription.bdBitsPerSample= 8;
    /*  Done by copy..
    ri.riDescription.bdHasAlpha= bdIn->bdHasAlpha;
    */

    if  ( bmCalculateSizes( &(ri.riDescription) ) )
	{ LDEB(1); rval= -1; goto ready;	}

    /************************************************************/
    /*  Allocate new buffer.					*/
    /************************************************************/
    if  ( bmAllocateBuffer( &ri ) )
	{
	LXDEB(ri.riDescription.bdBufferLength,ri.riBytes);
	rval= -1; goto ready;
	}

    from= riIn->riBytes; to= ri.riBytes;
    for ( row= 0; row < bdIn->bdPixelsHigh; row++ )
	{
	if  ( bmExpandRGB8Palette( to, from,
				    bdIn->bdPixelsWide, bdIn->bdBitsPerPixel,
				    &(bdIn->bdPalette), bdIn->bdHasAlpha ) )
	    { LDEB(1); rval= -1; goto ready;	}

	from += bdIn->bdBytesPerRow;
	to += ri.riDescription.bdBytesPerRow;
	}

    /* steal */
    *riOut= ri; bmInitRasterImage( &ri );

  ready:

    bmCleanRasterImage( &ri );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Make a palette for a grayscale raster image.			*/
/*									*/
/************************************************************************/

int bmMakeGrayPalette(	const BitmapDescription *	bd,
			int *				pColorCount,
			int *				pTransparentColor,
			RGB8Color *			palette,
			int				maxColors )
    {
    int		i;
    int		colorCount;
    int		transparentColor= -1;

    switch( bd->bdColorEncoding )
	{
	case BMcoRGB8PALETTE:
	    LDEB(bd->bdColorEncoding); return -1;

	case BMcoBLACKWHITE:
	    switch( bd->bdBitsPerSample )
		{
		case 1: case 2: case 4: case 8:
		    colorCount= 1 << bd->bdBitsPerSample;

		    if  ( colorCount+ bd->bdHasAlpha > maxColors )
			{
			LLLDEB(colorCount,bd->bdHasAlpha,maxColors);
			return -1;
			}

		    for ( i= 0; i < colorCount; i++ )
			{
			int n= colorCount- i- 1;
			int v= ( i* 255 )/ ( colorCount- 1 );

			palette[n].rgb8Red= v;
			palette[n].rgb8Green= v;
			palette[n].rgb8Blue= v;
			palette[n].rgb8Alpha= 255;
			}

		    break;

		default:
		    LLDEB(bd->bdColorEncoding,bd->bdBitsPerSample);
		    return -1;
		}
	    break;

	case BMcoWHITEBLACK:
	    switch( bd->bdBitsPerSample )
		{
		case 1: case 2: case 4: case 8:
		    colorCount= 1 << bd->bdBitsPerSample;

		    if  ( colorCount+ bd->bdHasAlpha > maxColors )
			{
			LLLDEB(colorCount,bd->bdHasAlpha,maxColors);
			return -1;
			}

		    for ( i= 0; i < colorCount; i++ )
			{
			int v= ( i* 255 )/ ( colorCount- 1 );

			palette[i].rgb8Red= v;
			palette[i].rgb8Green= v;
			palette[i].rgb8Blue= v;
			palette[colorCount- i- 1].rgb8Alpha= 255;
			}

		    break;

		default:
		    LLDEB(bd->bdColorEncoding,bd->bdBitsPerSample);
		    return -1;
		}
	    break;

	case BMcoRGB:
	default:
	    LDEB(bd->bdColorEncoding); return -1;
	}

    if  ( bd->bdHasAlpha )
	{
	if  ( colorCount >= maxColors )
	    { LLDEB(colorCount,maxColors); return -1;	}

	palette[colorCount].rgb8Red= 255;
	palette[colorCount].rgb8Green= 255;
	palette[colorCount].rgb8Blue= 255;
	palette[colorCount].rgb8Alpha= 0;

	transparentColor= colorCount++;
	}

    if  ( pColorCount )
	{ *pColorCount= colorCount;	}
    if  ( pTransparentColor )
	{ *pTransparentColor= transparentColor;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Inflate a scan line to 8 bits per pixel.				*/
/*									*/
/************************************************************************/

int bmInflateTo8bit(		unsigned char *			to,
				const unsigned char *		from,
				const BitmapDescription *	bd,
				const int			transpColor,
				int				removeAlpha )
    {
    int			col;
    unsigned char	b;

    switch( bd->bdBitsPerPixel )
	{
	case 1:
	    if  ( bd->bdHasAlpha && removeAlpha )
		{ LLDEB(bd->bdBitsPerPixel,bd->bdHasAlpha); return -1;	}
	    else{
		for ( col= 0; col < bd->bdPixelsWide; col += 8 )
		    {
		    b= *(from++);

		    *(to++)= ( b & 0x80 ) >> 7;
		    *(to++)= ( b & 0x40 ) >> 6;
		    *(to++)= ( b & 0x20 ) >> 5;
		    *(to++)= ( b & 0x10 ) >> 4;
		    *(to++)= ( b & 0x08 ) >> 3;
		    *(to++)= ( b & 0x04 ) >> 2;
		    *(to++)= ( b & 0x02 ) >> 1;
		    *(to++)= ( b & 0x01 )     ;
		    }
		}

	    return 0;

	case 2:
	    if  ( bd->bdHasAlpha && removeAlpha )
		{
		for ( col= 0; col < bd->bdPixelsWide; col += 4 )
		    {
		    b= *(from++);

		    if  ( ( b & 0x40 ) )
			{ *(to++)= ( b & 0x80 ) >> 7;	}
		    else{ *(to++)= transpColor;	}

		    if  ( ( b & 0x10 ) )
			{ *(to++)= ( b & 0x20 ) >> 5;	}
		    else{ *(to++)= transpColor;	}

		    if  ( ( b & 0x04 ) )
			{ *(to++)= ( b & 0x08 ) >> 3;	}
		    else{ *(to++)= transpColor;	}

		    if  ( ( b & 0x01 ) )
			{ *(to++)= ( b & 0x02 ) >> 1;	}
		    else{ *(to++)= transpColor;	}
		    }
		}
	    else{
		for ( col= 0; col < bd->bdPixelsWide; col += 4 )
		    {
		    b= *(from++);

		    *(to++)= ( b & 0xc0 ) >> 6;
		    *(to++)= ( b & 0x30 ) >> 4;
		    *(to++)= ( b & 0x0c ) >> 2;
		    *(to++)= ( b & 0x03 )     ;
		    }
		}

	    return 0;

	case 4:
	    if  ( bd->bdHasAlpha && removeAlpha )
		{
		for ( col= 0; col < bd->bdPixelsWide; col += 2 )
		    {
		    b= *(from++);

		    if  ( ( b & 0x30 ) )
			{ *(to++)= ( b & 0xc0 ) >> 6;	}
		    else{ *(to++)= transpColor;	}

		    if  ( ( b & 0x03 ) )
			{ *(to++)= ( b & 0x0c ) >> 2;	}
		    else{ *(to++)= transpColor;	}
		    }
		}
	    else{
		for ( col= 0; col < bd->bdPixelsWide; col += 2 )
		    {
		    b= *(from++);

		    *(to++)= ( b & 0xf0 ) >> 4;
		    *(to++)= ( b & 0x0f )     ;
		    }
		}

	    return 0;

	case 8:
	    if  ( bd->bdHasAlpha && removeAlpha )
		{
		b= *(from++);

		for ( col= 0; col < bd->bdPixelsWide; col += 1 )
		    {
		    if  ( ( b & 0x0f ) )
			{ *(to++)= ( b & 0xf0 ) >> 4;	}
		    else{ *(to++)= transpColor;	}
		    }
		}
	    else{ memcpy( to, from, bd->bdBytesPerRow );	}

	    return 0;

	case 16:
	    if  ( ! removeAlpha )
		{ LDEB(removeAlpha); return -1;	}

	    for ( col= 0; col < bd->bdPixelsWide; col += 1 )
		{
		b= *(from++);

		if  ( *from == 0 )
		    { b= transpColor;	}

		*(to++)= b; from++;
		}

	    return 0;

	default:
	    LDEB(bd->bdBitsPerPixel); return -1;
	}
    }

/************************************************************************/
/*									*/
/*  Inflate a scan line to 16 bits per pixel.				*/
/*									*/
/************************************************************************/

int bmInflateToInt(		unsigned int *			to,
				const unsigned char *		from,
				const BitmapDescription *	bd,
				const int			transpColor,
				int				removeAlpha )
    {
    int		col;
    int		b;

    switch( bd->bdBitsPerPixel )
	{
	case 1:
	    if  ( bd->bdHasAlpha && removeAlpha )
		{ LLDEB(bd->bdBitsPerPixel,bd->bdHasAlpha); return -1;	}
	    else{
		for ( col= 0; col < bd->bdPixelsWide; col += 8 )
		    {
		    b= *(from++);

		    *(to++)= ( b & 0x80 ) >> 7;
		    *(to++)= ( b & 0x40 ) >> 6;
		    *(to++)= ( b & 0x20 ) >> 5;
		    *(to++)= ( b & 0x10 ) >> 4;
		    *(to++)= ( b & 0x08 ) >> 3;
		    *(to++)= ( b & 0x04 ) >> 2;
		    *(to++)= ( b & 0x02 ) >> 1;
		    *(to++)= ( b & 0x01 )     ;
		    }
		}

	    return 0;

	case 2:
	    if  ( bd->bdHasAlpha && removeAlpha )
		{
		for ( col= 0; col < bd->bdPixelsWide; col += 4 )
		    {
		    b= *(from++);

		    if  ( ( b & 0x40 ) )
			{ *(to++)= ( b & 0x80 ) >> 7;	}
		    else{ *(to++)= transpColor;	}

		    if  ( ( b & 0x10 ) )
			{ *(to++)= ( b & 0x20 ) >> 5;	}
		    else{ *(to++)= transpColor;	}

		    if  ( ( b & 0x04 ) )
			{ *(to++)= ( b & 0x08 ) >> 3;	}
		    else{ *(to++)= transpColor;	}

		    if  ( ( b & 0x01 ) )
			{ *(to++)= ( b & 0x02 ) >> 1;	}
		    else{ *(to++)= transpColor;	}
		    }
		}
	    else{
		for ( col= 0; col < bd->bdPixelsWide; col += 4 )
		    {
		    b= *(from++);

		    *(to++)= ( b & 0xc0 ) >> 6;
		    *(to++)= ( b & 0x30 ) >> 4;
		    *(to++)= ( b & 0x0c ) >> 2;
		    *(to++)= ( b & 0x03 )     ;
		    }
		}

	    return 0;

	case 4:
	    if  ( bd->bdHasAlpha && removeAlpha )
		{
		for ( col= 0; col < bd->bdPixelsWide; col += 2 )
		    {
		    b= *(from++);

		    if  ( ( b & 0x30 ) )
			{ *(to++)= ( b & 0xc0 ) >> 6;	}
		    else{ *(to++)= transpColor;	}

		    if  ( ( b & 0x03 ) )
			{ *(to++)= ( b & 0x0c ) >> 2;	}
		    else{ *(to++)= transpColor;	}
		    }
		}
	    else{
		for ( col= 0; col < bd->bdPixelsWide; col += 2 )
		    {
		    b= *(from++);

		    *(to++)= ( b & 0xf0 ) >> 4;
		    *(to++)= ( b & 0x0f )     ;
		    }
		}

	    return 0;

	case 8:
	    if  ( bd->bdHasAlpha && removeAlpha )
		{
		b= *(from++);

		for ( col= 0; col < bd->bdPixelsWide; col += 1 )
		    {
		    if  ( ( b & 0x0f ) )
			{ *(to++)= ( b & 0xf0 ) >> 4;	}
		    else{ *(to++)= transpColor;	}
		    }
		}
	    else{ memcpy( to, from, bd->bdBytesPerRow );	}

	    return 0;

	case 16:
	    if  ( ! removeAlpha )
		{ LDEB(removeAlpha); return -1;	}

	    for ( col= 0; col < bd->bdPixelsWide; col += 1 )
		{
		b= *(from++);

		if  ( *from == 0 )
		    { b= transpColor;	}

		*(to++)= b; from++;
		}

	    return 0;

	default:
	    LDEB(bd->bdBitsPerPixel); return -1;
	}
    }

int bmInflateTo8bitGray(	unsigned char *			to,
				const unsigned char *		from,
				int				bitsPerPixel,
				int				pixelsWide )
    {
    int			col;
    unsigned char	b;

    switch( bitsPerPixel )
	{
	case 1:
	    for ( col= 0; col < pixelsWide; col += 8 )
		{
		b= *(from++);

		*(to++)= ( 255* ( ( b & 0x80 ) >> 7 ) );
		*(to++)= ( 255* ( ( b & 0x40 ) >> 6 ) );
		*(to++)= ( 255* ( ( b & 0x20 ) >> 5 ) );
		*(to++)= ( 255* ( ( b & 0x10 ) >> 4 ) );
		*(to++)= ( 255* ( ( b & 0x08 ) >> 3 ) );
		*(to++)= ( 255* ( ( b & 0x04 ) >> 2 ) );
		*(to++)= ( 255* ( ( b & 0x02 ) >> 1 ) );
		*(to++)= ( 255* ( ( b & 0x01 )      ) );
		}
	    return 0;

	case 2:
	    for ( col= 0; col < pixelsWide; col += 4 )
		{
		b= *(from++);

		*(to++)= ( 255* ( ( b & 0xc0 ) >> 6 ) )/ 3;
		*(to++)= ( 255* ( ( b & 0x30 ) >> 4 ) )/ 3;
		*(to++)= ( 255* ( ( b & 0x0c ) >> 2 ) )/ 3;
		*(to++)= ( 255* ( ( b & 0x03 )      ) )/ 3;
		}
	    return 0;

	case 4:
	    for ( col= 0; col < pixelsWide; col += 2 )
		{
		b= *(from++);

		*(to++)= ( 255* ( ( b & 0xf0 ) >> 4 ) )/ 15;
		*(to++)= ( 255* ( ( b & 0x0f )      ) )/ 15;
		}
	    return 0;

	default:
	    LDEB(bitsPerPixel); return -1;
	}
    }

/************************************************************************/
/*									*/
/*  Find/Allocate a color in the palette of a palette image. Only do so	*/
/*  if there is space in the palette.					*/
/*									*/
/************************************************************************/

int bmPaletteColor(	BitmapDescription *		bd,
			int				r,
			int				g,
			int				b,
			int				a )
    {
    return utilPaletteColorIndex( &(bd->bdPalette), 1<<bd->bdBitsPerPixel,
								r, g, b, a );
    }
