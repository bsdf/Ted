#   include	"bitmapConfig.h"

#   include	<stdlib.h>
#   include	<string.h>

#   include	"bmintern.h"
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Replace 'White' in a bitmap with 'Transparent'			*/
/*									*/
/************************************************************************/

static int bmSetAlphaBytes(	unsigned char *			b1,
				unsigned char *			b2,
				int				bitsPerPixel,
				unsigned int			t )
    {
    unsigned int	i;
    unsigned int	m;

    switch( bitsPerPixel )
	{
	case 1:
	    m= 0x01;

	    for ( i= 0; i < 256; i++ )
		{
		b1[i]= b2[i]= 0;

		b1[i] |=	( i & 0x80 )     	|
				( i & 0x40 ) >> 1	|
				( i & 0x20 ) >> 2	|
				( i & 0x10 ) >> 3	;

		b1[i] |=	( m* ( ( ( i & 0x80 ) >> 7 ) != t ) ) << 6 |
				( m* ( ( ( i & 0x40 ) >> 6 ) != t ) ) << 4 |
				( m* ( ( ( i & 0x20 ) >> 5 ) != t ) ) << 2 |
				( m* ( ( ( i & 0x10 ) >> 4 ) != t ) ) << 0 ;

		b2[i] |=	( i & 0x08 ) << 4	|
				( i & 0x04 ) << 3	|
				( i & 0x02 ) << 2	|
				( i & 0x01 ) << 1	;

		b2[i] |=	( m* ( ( ( i & 0x08 ) >> 3 ) != t ) ) << 6 |
				( m* ( ( ( i & 0x04 ) >> 2 ) != t ) ) << 4 |
				( m* ( ( ( i & 0x02 ) >> 1 ) != t ) ) << 2 |
				( m* ( ( ( i & 0x01 ) >> 0 ) != t ) ) << 0 ;
		}
	    break;

	case 2:
	    m= 0x03;

	    for ( i= 0; i < 256; i++ )
		{
		b1[i]= b2[i]= 0;

		b1[i] |=	( i & 0xc0 )     	|
				( i & 0x30 ) >> 2	;

		b1[i] |=	( m* ( ( ( i & 0xc0 ) >> 6 ) != t ) ) << 4 |
				( m* ( ( ( i & 0x60 ) >> 4 ) != t ) ) << 0 ;

		b2[i] |=	( i & 0x0c ) << 4	|
				( i & 0x03 ) << 2	;

		b2[i] |=	( m* ( ( ( i & 0x0c ) >> 2 ) != t ) ) << 4 |
				( m* ( ( ( i & 0x03 ) >> 0 ) != t ) ) << 0 ;
		}
	    break;

	case 4:
	    m= 0x0f;

	    for ( i= 0; i < 256; i++ )
		{
		b1[i]= b2[i]= 0;

		b1[i] |=	( i & 0xf0 )     	;

		b1[i] |=	( m* ( ( ( i & 0xf0 ) >> 4 ) != t ) ) << 0 ;

		b2[i] |=	( i & 0x0f ) << 4	;

		b2[i] |=	( m* ( ( ( i & 0x0f ) >> 0 ) != t ) ) << 0 ;
		}
	    break;

	case 8:
	    for ( i= 0; i < 256; i++ )
		{
		b1[i]=	i;
		b2[i]=	( i == t )? 0: 255;
		}
	    break;

	default:
	    LDEB(bitsPerPixel); return -1;
	    break;
	}

    return 0;
    }

int bmWhiteToTransparent(	RasterImage *			riOut,
				const RasterImage *		riIn,
				int				ignoredInt )
    {
    int				rval= 0;

    const BitmapDescription *	bdIn= &(riIn->riDescription);
    RasterImage			ri;
    int				row, col;
    const unsigned char *	from;
    unsigned char *		to;

    unsigned char		b1[256];
    unsigned char		b2[256];

    unsigned int		white;

    bmInitRasterImage( &ri );
    if  ( bmCopyDescription( &(ri.riDescription), bdIn ) )
	{ LDEB(1); rval= -1; goto ready;	}

    switch( bdIn->bdColorEncoding )
	{
	case BMcoBLACKWHITE:
	    if  ( bdIn->bdSamplesPerPixel != 1 )
		{ LDEB(bdIn->bdSamplesPerPixel); rval= -1; goto ready;	}
	    ri.riDescription.bdSamplesPerPixel= 2;
	    ri.riDescription.bdHasAlpha= 1;

	    white= 0;
	    if  ( bmSetAlphaBytes( b1, b2, bdIn->bdBitsPerSample, white ) )
		{ LDEB(bdIn->bdBitsPerSample); rval= -1; goto ready;	}

	    ri.riDescription.bdBitsPerPixel=
			ri.riDescription.bdSamplesPerPixel*
			ri.riDescription.bdBitsPerSample;
	    break;

	case BMcoWHITEBLACK:
	    if  ( bdIn->bdSamplesPerPixel != 1 )
		{ LDEB(bdIn->bdSamplesPerPixel); rval= -1; goto ready;	}
	    ri.riDescription.bdSamplesPerPixel= 2;
	    ri.riDescription.bdHasAlpha= 1;

	    white= ( 1 << bdIn->bdBitsPerPixel )- 1;
	    if  ( bmSetAlphaBytes( b1, b2, bdIn->bdBitsPerSample, white ) )
		{ LDEB(bdIn->bdBitsPerSample); rval= -1; goto ready;	}

	    ri.riDescription.bdBitsPerPixel=
			ri.riDescription.bdSamplesPerPixel*
			ri.riDescription.bdBitsPerSample;
	    break;

	case BMcoRGB8PALETTE:
	    if  ( bdIn->bdSamplesPerPixel != 3 )
		{ LDEB(bdIn->bdSamplesPerPixel); rval= -1; goto ready;	}
	    ri.riDescription.bdSamplesPerPixel= 4;
	    ri.riDescription.bdHasAlpha= 1;

	    for ( row= 0; row < ri.riDescription.bdPalette.cpColorCount; row++ )
		{
		if  ( ri.riDescription.bdPalette.cpColors[row].rgb8Red == 255		&&
		      ri.riDescription.bdPalette.cpColors[row].rgb8Green == 255	&&
		      ri.riDescription.bdPalette.cpColors[row].rgb8Blue == 255	)
		    {
		    ri.riDescription.bdPalette.cpColors[row].rgb8Alpha= 0;
		    break;
		    }
		}

	    white= row;
	    if  ( bmSetAlphaBytes( b1, b2, bdIn->bdBitsPerSample, white ) )
		{ LDEB(bdIn->bdBitsPerSample); rval= -1; goto ready;	}

	    ri.riDescription.bdBitsPerPixel= 2* bdIn->bdBitsPerPixel;
	    break;

	case BMcoRGB:
	    if  ( bdIn->bdSamplesPerPixel != 3 )
		{ LDEB(bdIn->bdSamplesPerPixel); rval= -1; goto ready;	}
	    ri.riDescription.bdSamplesPerPixel= 4;
	    ri.riDescription.bdHasAlpha= 1;
	    break;

	default:
	    LDEB(bdIn->bdColorEncoding); rval= -1; goto ready;
	}

    if  ( bmCalculateSizes( &(ri.riDescription) ) )
	{ LDEB(1); rval= -1; goto ready;	}
    if  ( bmAllocateBuffer( &ri ) )
	{ LLDEB(ri.riDescription.bdBufferLength,ri.riBytes); rval= -1; goto ready; }

    switch( bdIn->bdColorEncoding )
	{
	case BMcoBLACKWHITE:
	case BMcoWHITEBLACK:
	case BMcoRGB8PALETTE:
	    switch( bdIn->bdBitsPerSample )
		{
		case 8:
		    for ( row= 0; row < bdIn->bdPixelsHigh; row++ )
			{
			from= riIn->riBytes+ row* bdIn->bdBytesPerRow;
			to  = ri.riBytes+ row* ri.riDescription.bdBytesPerRow;

			for ( col= 0; col < bdIn->bdPixelsWide; col++ )
			    {
			    *(to++)= b1[*from];
			    *(to++)= b2[*from];
			    from++;
			    }
			}
		    break;

		case 4:
		    for ( row= 0; row < bdIn->bdPixelsHigh; row++ )
			{
			from= riIn->riBytes+ row* bdIn->bdBytesPerRow;
			to  = ri.riBytes+ row* ri.riDescription.bdBytesPerRow;

			for ( col= 0; col < bdIn->bdPixelsWide; col++ )
			    {
			    if  ( col % 2 )
				{
				*(to++)= b1[(*from&0xf0)] | b2[(*from&0xf0)];
				}
			    else{
				*(to++)= b1[(*from&0x0f)] | b2[(*from&0x0f)];
				to++;
				}
			    }
			}
		    break;

		default:
		    LDEB(bdIn->bdBitsPerSample); rval= -1; goto ready;
		}
	    break;

	case BMcoRGB:
	    LDEB(bdIn->bdColorEncoding); rval= -1; goto ready;
	    break;

	default:
	    LDEB(bdIn->bdColorEncoding); rval= -1; goto ready;
	}

    *riOut= ri; bmInitRasterImage( &ri ); /* steal */

  ready:

    bmCleanRasterImage( &ri );

    return rval;
    }

static int bmGetRowAlpha(	unsigned char *		to,
				const unsigned char *	from,
				int			pixelsWide,
				int			bpp,
				int			spp )
    {
    unsigned char	mask= 0x80;
    int			bit= 0;
    int			col;
    int			step;

    switch( bpp )
	{
	case 16:
	    bit= 0;
	    for ( col= 0; col < pixelsWide; from += spp, col++ )
		{
		if  ( from[spp- 1] )
		    { *to |= mask;	}

		bit++; mask >>= 1;

		if  ( bit == 8 )
		    { to++; mask= 0x80; bit= 0;	}
		}
	    break;

	case 8:
	    if  ( spp % 2 != 0 )
		{ LDEB(spp); return -1;	}
	    step= spp/ 2;

	    bit= 0;
	    for ( col= 0; col < pixelsWide; from += step, col++ )
		{
		if  ( from[step- 1] & 0x0f )
		    { *to |= mask;	}

		bit++; mask >>= 1;

		if  ( bit == 8 )
		    { to++; mask= 0x80; bit= 0;	}
		}
	    break;

	default:
	    LDEB(bpp); return -1;
	}

    return 0;
    }

int bmGetAlphaMask(	RasterImage *			riOut,
			const RasterImage *		riIn,
			int				ignoredInt )
    {
    const BitmapDescription *	bdIn= &(riIn->riDescription);
    int				rval= 0;
    int				row;

    RasterImage			ri;

    bmInitRasterImage( &ri );

    if  ( ! bdIn->bdHasAlpha )
	{ LDEB(bdIn->bdHasAlpha); rval= -1; goto ready;	}

    ri.riDescription.bdPixelsWide= bdIn->bdPixelsWide;
    ri.riDescription.bdPixelsHigh= bdIn->bdPixelsHigh;
    ri.riDescription.bdBitsPerSample= 1;
    ri.riDescription.bdSamplesPerPixel= 1;
    ri.riDescription.bdXResolution= bdIn->bdXResolution;
    ri.riDescription.bdYResolution= bdIn->bdYResolution;
    ri.riDescription.bdUnit= bdIn->bdUnit;
    ri.riDescription.bdColorEncoding= BMcoBLACKWHITE;
    ri.riDescription.bdHasAlpha= 0;

    if  ( bmCalculateSizes( &(ri.riDescription) ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( bmAllocateBuffer( &ri ) )
	{ LLDEB(ri.riDescription.bdBufferLength,ri.riBytes); rval= -1; goto ready; }
    memset( ri.riBytes, '\0', ri.riDescription.bdBufferLength );

    switch( bdIn->bdColorEncoding )
	{
	case BMcoBLACKWHITE:
	case BMcoWHITEBLACK:
	case BMcoRGB8PALETTE:
	    for ( row= 0; row < bdIn->bdPixelsHigh; row++ )
		{
		if  ( bmGetRowAlpha(
			ri.riBytes+ row* ri.riDescription.bdBytesPerRow,
			riIn->riBytes+ row* bdIn->bdBytesPerRow,
			bdIn->bdPixelsWide, bdIn->bdBitsPerPixel, 2 ) )
		    { LDEB(bdIn->bdBitsPerPixel); rval= -1; goto ready; }
		}
	    break;

	case BMcoRGB:
	    for ( row= 0; row < bdIn->bdPixelsHigh; row++ )
		{
		if  ( bmGetRowAlpha(
			ri.riBytes+ row* ri.riDescription.bdBytesPerRow,
			riIn->riBytes+ row* bdIn->bdBytesPerRow,
			bdIn->bdPixelsWide, bdIn->bdBitsPerPixel, 4 ) )
		    { LDEB(bdIn->bdBitsPerPixel); rval= -1; goto ready; }
		}
	    break;

	default:
	    LDEB(bdIn->bdColorEncoding); rval= -1; goto ready;
	}

    /* steal */
    *riOut= ri; bmInitRasterImage( &ri );

  ready:

    bmCleanRasterImage( &ri );

    return rval;
    }

static int bmRemoveRowAlpha(	unsigned char *		to,
				const unsigned char *	from,
				int			pixelsWide,
				int			bpp,
				int			spp,
				int			empty )
    {
    int			col;
    int			step;

    switch( bpp )
	{
	case 16:
	    step= spp- 1;
	    for ( col= 0; col < pixelsWide; col++ )
		{
		int	sample;

		if  ( from[step] )
		    {
		    for ( sample= 0; sample < step; sample++ )
			{ *(to++)= *(from++);	}
		    from++;
		    }
		else{
		    for ( sample= 0; sample < step; sample++ )
			{ *(to++)= empty;	}
		    from += step+ 1;
		    }
		}
	    break;

	case 8:
	    for ( col= 0; col < pixelsWide; col += 2 )
		{
		if  ( *from & 0x0f )
		    { *to= *from & 0xf0;	}
		else{ *to= empty << 4;		}
		from++;

		if  ( *from & 0x0f )
		    { *to |= *from >> 4;	}
		else{ *to |= empty & 0x0f;	}
		from++;

		to++;
		}
	    break;

	default:
	    LDEB(bpp); return -1;
	}

    return 0;
    }

int bmRemoveAlpha(	RasterImage *			riOut,
			const RasterImage *		riIn,
			int				ignoredInt )
    {
    const BitmapDescription *	bdIn= &(riIn->riDescription);
    int				rval= 0;
    int				row;
    int				white= -1;

    RasterImage			ri;

    bmInitRasterImage( &ri );

    if  ( ! bdIn->bdHasAlpha )
	{ LDEB(bdIn->bdHasAlpha); rval= -1; goto ready;	}

    if  ( bmCopyDescription( &(ri.riDescription), bdIn ) )
	{ LDEB(1); rval= -1; goto ready;	}

    ri.riDescription.bdHasAlpha= 0;
    if  ( bdIn->bdColorEncoding == BMcoRGB8PALETTE )
	{ ri.riDescription.bdBitsPerPixel /= 2;	}

    if  ( bmCalculateSizes( &(ri.riDescription) ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( bmAllocateBuffer( &ri ) )
	{ LLDEB(ri.riDescription.bdBufferLength,ri.riBytes); rval= -1; goto ready; }
    memset( ri.riBytes, '\0', ri.riDescription.bdBufferLength );

    switch( bdIn->bdColorEncoding )
	{
	case BMcoBLACKWHITE:
	    white= 0;
	    for ( row= 0; row < bdIn->bdPixelsHigh; row++ )
		{
		if  ( bmRemoveRowAlpha(
			ri.riBytes+ row* ri.riDescription.bdBytesPerRow,
			riIn->riBytes+ row* bdIn->bdBytesPerRow,
			bdIn->bdPixelsWide, bdIn->bdBitsPerPixel, 2, white ) )
		    { LDEB(bdIn->bdBitsPerPixel); rval= -1; goto ready; }
		}
	    break;

	case BMcoWHITEBLACK:
	    white= ( 1 >> bdIn->bdBitsPerSample ) -1;
	    for ( row= 0; row < bdIn->bdPixelsHigh; row++ )
		{
		if  ( bmRemoveRowAlpha(
			ri.riBytes+ row* ri.riDescription.bdBytesPerRow,
			riIn->riBytes+ row* bdIn->bdBytesPerRow,
			bdIn->bdPixelsWide, bdIn->bdBitsPerPixel, 2, white ) )
		    { LDEB(bdIn->bdBitsPerPixel); rval= -1; goto ready; }
		}
	    break;

	case BMcoRGB8PALETTE:
	    /* All transparent colors to opaque white */
	    white= 0;
	    for ( row= 0; row < ri.riDescription.bdPalette.cpColorCount; row++ )
		{
		if  ( ri.riDescription.bdPalette.cpColors[row].rgb8Alpha == 0 )
		    {
		    ri.riDescription.bdPalette.cpColors[row].rgb8Red= 255;
		    ri.riDescription.bdPalette.cpColors[row].rgb8Green= 255;
		    ri.riDescription.bdPalette.cpColors[row].rgb8Blue= 255;
		    ri.riDescription.bdPalette.cpColors[row].rgb8Alpha= 255;
		    white= row;
		    }
		}
	    for ( row= 0; row < bdIn->bdPixelsHigh; row++ )
		{
		if  ( bmRemoveRowAlpha(
			ri.riBytes+ row* ri.riDescription.bdBytesPerRow,
			riIn->riBytes+ row* bdIn->bdBytesPerRow,
			bdIn->bdPixelsWide, bdIn->bdBitsPerPixel, 2, white ) )
		    { LDEB(bdIn->bdBitsPerPixel); rval= -1; goto ready; }
		}
	    break;

	case BMcoRGB:
	    white= ( 1 >> bdIn->bdBitsPerSample ) -1;
	    for ( row= 0; row < bdIn->bdPixelsHigh; row++ )
		{
		if  ( bmRemoveRowAlpha(
			ri.riBytes+ row* ri.riDescription.bdBytesPerRow,
			riIn->riBytes+ row* bdIn->bdBytesPerRow,
			bdIn->bdPixelsWide, bdIn->bdBitsPerPixel, 2, white ) )
		    { LDEB(bdIn->bdBitsPerPixel); rval= -1; goto ready; }
		}

	    break;

	default:
	    LDEB(bdIn->bdColorEncoding); rval= -1; goto ready;
	}

    /* steal */
    *riOut= ri; bmInitRasterImage( &ri );

  ready:

    bmCleanRasterImage( &ri );

    return rval;
    }

int bmSetAlphaMask(	BitmapDescription *		bdOut,
			const BitmapDescription *	bdImage,
			const BitmapDescription *	bdAlpha,
			unsigned char **		pBufOut,
			const unsigned char *		bufImage,
			const unsigned char *		bufAlpha )
    {
    int			rval= 0;
    BitmapDescription	bd;
    unsigned char *	bufOut= (unsigned char *)0;
    int			row;

    bmInitDescription( &bd );

    if  ( bdImage->bdHasAlpha )
	{ LDEB(bdImage->bdHasAlpha); rval= -1; goto ready;	}
    if  ( bdAlpha->bdHasAlpha )
	{ LDEB(bdAlpha->bdHasAlpha); rval= -1; goto ready;	}
    if  ( bdAlpha->bdColorEncoding != BMcoBLACKWHITE )
	{ LDEB(bdAlpha->bdColorEncoding); rval= -1; goto ready;	}
    if  ( bdAlpha->bdBitsPerPixel != 1 )
	{ LDEB(bdAlpha->bdBitsPerPixel); rval= -1; goto ready;	}

    if  ( bmCopyDescription( &bd, bdImage ) )
	{ LDEB(1); rval= -1; goto ready;	}

    bd.bdHasAlpha= 1;
    if  ( bd.bdColorEncoding == BMcoRGB8PALETTE )
	{ bd.bdBitsPerPixel *= 2;	}

    bmCalculateSizes( &bd );

    bufOut= (unsigned char *)malloc( bd.bdBufferLength+ 7 );
    if  ( ! bufOut )
	{ LXDEB(bd.bdBufferLength,bufOut); rval= -1; goto ready;	}

    /*  5  */
    for ( row= 0; row < bd.bdPixelsHigh; row++ )
	{
	unsigned char *		to;
	const unsigned char *	fi;
	const unsigned char *	fa;

	int			i;

	to= bufOut+   row* bd.bdBytesPerRow;
	fi= bufImage+ row* bdImage->bdBytesPerRow;
	fa= bufAlpha+ row* bdAlpha->bdBytesPerRow;

	switch( bdImage->bdBitsPerPixel )
	    {
	    case 1:
		for ( i= 0; i < bdImage->bdPixelsWide; i += 8 )
		    {
		    *to= 0;
		    *to |= ( *fi & 0x80 )      | ( *fa & 0x80 ) >> 1;
		    *to |= ( *fi & 0x40 ) >> 1 | ( *fa & 0x40 ) >> 2;
		    *to |= ( *fi & 0x20 ) >> 2 | ( *fa & 0x20 ) >> 3;
		    *to |= ( *fi & 0x10 ) >> 3 | ( *fa & 0x10 ) >> 4;
		    *(++to)= 0;
		    *to |= ( *fi & 0x08 ) << 4 | ( *fa & 0x08 ) << 3;
		    *to |= ( *fi & 0x04 ) << 3 | ( *fa & 0x04 ) << 2;
		    *to |= ( *fi & 0x02 ) << 2 | ( *fa & 0x02 ) << 1;
		    *to |= ( *fi & 0x01 ) << 1 | ( *fa & 0x01 )     ;
		    to++; fi++; fa++;
		    }
		break;

	    case 4:
		for ( i= 0; i < bdImage->bdPixelsWide; i += 2 )
		    {
		    if  ( ( fa[i/8] << (i%8) ) & 0x80 )
			{ *(to++)= ( *fi & 0xf0 ) | 0x0f;	}
		    else{ *(to++)= ( *fi & 0xf0 );		}

		    if  ( ( fa[i/8] << (i%8) ) & 0x40 )
			{ *(to++)= ( *fi << 4 ) | 0x0f;		}
		    else{ *(to++)= ( *fi << 4 );		}

		    fi++;
		    }
		break;

	    case 8:
		for ( i= 0; i < bdImage->bdPixelsWide; i++ )
		    {
		    *(to++)= *(fi++);
		    if  ( ( fa[i/8] << (i%8) ) & 0x80 )
			{ *(to++)= 255;	}
		    else{ *(to++)= 0;	}
		    }
		break;

	    case 24:
		for ( i= 0; i < bdImage->bdPixelsWide; i++ )
		    {
		    *(to++)= *(fi++);
		    *(to++)= *(fi++);
		    *(to++)= *(fi++);
		    if  ( ( fa[i/8] << (i%8) ) & 0x80 )
			{ *(to++)= 255;	}
		    else{ *(to++)= 0;	}
		    }
		break;

	    default:
		LDEB(bdImage->bdBitsPerPixel); rval= -1; goto ready;
	    }
	}


    /* steal */
    *pBufOut= bufOut; bufOut= (unsigned char *)0;
    *bdOut= bd; bmInitDescription( &bd );

  ready:

    if  ( bufOut )
	{ free( bufOut );	}

    bmCleanDescription( &bd );

    return rval;
    }
