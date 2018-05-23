#   include	"bitmapConfig.h"

#   include	<geoAffineTransform.h>
#   include	<geo2DInteger.h>

#   include	"bmintern.h"

#   include	<string.h>
#   include	<stdlib.h>

#   define	y0	math_y0
#   define	y1	math_y1
#   include	<math.h>
#   undef	y0
#   undef	y1

#   include	<appDebugon.h>

#   ifndef	M_PI
#	define	M_PI	3.14159265358979323846
#   endif

/************************************************************************/
/*									*/
/*  Flip a scan line consisting of 1 bit pixels				*/
/*									*/
/************************************************************************/

static unsigned char	bmFlipLeft[256];
static unsigned char	bmFlipRight[256];

static void bmFlip1Init( int n )
    {
    int		i;
    int		rshift= n % 8;
    int		lshift= 8- rshift;

    for ( i= 0; i < 256; i++ )
	{
	unsigned char value=	( i & 0x01 ) << 7	|
				( i & 0x02 ) << 5	|
				( i & 0x04 ) << 3	|
				( i & 0x08 ) << 1	|
				( i & 0x10 ) >> 1	|
				( i & 0x20 ) >> 3	|
				( i & 0x40 ) >> 5	|
				( i & 0x80 ) >> 7	;

	bmFlipLeft[i]=  value << lshift;
	bmFlipRight[i]= value >> rshift;
	}
    }

static void bmFlip2Init( int n )
    {
    int		i;
    int		rshift= 2* ( n % 4 );
    int		lshift= 8- rshift;

    for ( i= 0; i < 256; i++ )
	{
	unsigned char value=	( i & 0x03 ) << 6	|
				( i & 0x0c ) << 2	|
				( i & 0x30 ) >> 2	|
				( i & 0xc0 ) >> 6	;

	bmFlipLeft[i]=  value << lshift;
	bmFlipRight[i]= value >> rshift;
	}
    }

static void bmFlip4Init( int n )
    {
    int		i;
    int		rshift= 4* ( n % 2 );
    int		lshift= 8- rshift;

    for ( i= 0; i < 256; i++ )
	{
	unsigned char value=	( i & 0x0f ) << 4	|
				( i & 0xf0 ) >> 4	;

	bmFlipLeft[i]=  value << lshift;
	bmFlipRight[i]= value >> rshift;
	}
    }

/************************************************************************/
/*									*/
/*  Flip the bytes in a scan line.					*/
/*									*/
/************************************************************************/

int bmFlipBytes(		unsigned char *	buffer,
				int		byteCount,
				int		bitsPerPixel )
    {
    switch( bitsPerPixel )
	{
	case  1:
	case  2:
	case  4:
	    bmFlip1Init( 8/bitsPerPixel );
	    break;

	case  8:
	    return 0;

	case 24:
	default:
	    LDEB(bitsPerPixel); return -1;
	}

    while( byteCount-- > 0 )
	{ *buffer= bmFlipRight[*buffer]; buffer++;	}

    return 0;
    }


/************************************************************************/
/*									*/
/*  Flip a scan line consisting of 1 bit pixels				*/
/*									*/
/************************************************************************/

static void bmFlipBits(	unsigned char *		to,
			const unsigned char *	from,
			int			pixelsWide,
			int			pixelsPerByte )
    {
    int			i;
    int			count;

    count= pixelsWide/ pixelsPerByte;
    to += count;

    if  ( pixelsWide % pixelsPerByte )
	{
	*(to)= bmFlipLeft[*from];
	to--;

	for ( i= count; i; i-- )
	    {
	    *to=       bmFlipRight[*(from++)];
	    *(to--) |= bmFlipLeft [* from   ];
	    }
	}
    else{
	for ( i= count; i; i-- )
	    { *(--to)= bmFlipRight[*(from++)];	}
	}
    }

/************************************************************************/
/*									*/
/*  Flip a scan line consisting of 8 bit pixels				*/
/*									*/
/************************************************************************/

static void bmFlip8Bits(	unsigned char *		to,
				const unsigned char *	from,
				int			pixelsWide,
				int			ignored	)
    {
    int			i;

    to += pixelsWide- 1;

    for ( i= pixelsWide; i; i-- )
	{ *(to--)= *(from++); }
    }

/************************************************************************/
/*									*/
/*  Flip a scan line consisting of 24 bit pixels			*/
/*									*/
/************************************************************************/

static void bmFlip24Bits(	unsigned char *		to,
				const unsigned char *	from,
				int			pixelsWide,
				int			ignored	)
    {
    int			i;

    to += 3*pixelsWide- 3;

    for ( i= pixelsWide; i; i-- )
	{
	to[0]= from[0]; to[1]= from[1]; to[2]= from[2];
	to -= 3; from += 3;
	}
    }

/************************************************************************/
/*									*/
/*  Mirror bitmap images: Flip over horizontal axis.			*/
/*									*/
/************************************************************************/

int bmVerticalFlip(	RasterImage *			riOut,
			const RasterImage *		riIn,
			int				ignoredInt )
    {
    int				rval= 0;
    const BitmapDescription *	bdIn= &(riIn->riDescription);

    unsigned int		row;

    RasterImage			ri;

    bmInitRasterImage( &ri );

    if  ( bmCopyDescription( &(ri.riDescription), bdIn ) )
	{ LDEB(1); rval= -1; goto ready;	}

    ri.riBytes= (unsigned char *)malloc( ri.riDescription.bdBufferLength );
    if  ( ! ri.riBytes )
	{ LLDEB(ri.riDescription.bdBufferLength,ri.riBytes); rval= -1; goto ready; }

    for ( row= 0; row < bdIn->bdPixelsHigh; row++ )
	{
	const unsigned char *	from;
	unsigned char *		to;

	from= riIn->riBytes+ row* bdIn->bdBytesPerRow;
	to= ri.riBytes+ ( bdIn->bdPixelsHigh- row -1 )* bdIn->bdBytesPerRow;

	memcpy( to, from, bdIn->bdBytesPerRow );
	}

    /* steal */
    *riOut= ri; bmInitRasterImage( &ri );

  ready:

    bmCleanRasterImage( &ri );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Mirror bitmap images: Rotate 180 degrees.				*/
/*									*/
/************************************************************************/

int bmRotate180(	RasterImage *			riOut,
			const RasterImage *		riIn,
			int				ignoredInt )
    {
    int				rval= 0;
    const BitmapDescription *	bdIn= &(riIn->riDescription);

    unsigned int		row;
    int				pixelsPerByte= 8/ bdIn->bdBitsPerPixel;
    void			(*flip)( unsigned char *,
					const unsigned char *, int, int );

    RasterImage			ri;

    bmInitRasterImage( &ri );

    if  ( bmCopyDescription( &(ri.riDescription), bdIn ) )
	{ LDEB(1); rval= -1; goto ready;	}

    ri.riBytes= (unsigned char *)malloc( ri.riDescription.bdBufferLength );
    if  ( ! ri.riBytes )
	{ LLDEB(ri.riDescription.bdBufferLength,ri.riBytes); rval= -1; goto ready; }

    switch( bdIn->bdBitsPerPixel )
	{
	case  1:
	    flip= bmFlipBits;	bmFlip1Init( bdIn->bdPixelsWide ); break;
	case  2:
	    flip= bmFlipBits;	bmFlip2Init( bdIn->bdPixelsWide ); break;
	case  4:
	    flip= bmFlipBits;	bmFlip4Init( bdIn->bdPixelsWide ); break;
	case  8:
	    flip= bmFlip8Bits;	break;
	case 24:
	    flip= bmFlip24Bits;	break;
	default:
	    LDEB(bdIn->bdBitsPerPixel); rval= -1; goto ready;
	}

    for ( row= 0; row < bdIn->bdPixelsHigh; row++ )
	{
	const unsigned char *	from;
	unsigned char *		to;

	from= riIn->riBytes+ row* bdIn->bdBytesPerRow;
	to= ri.riBytes+ ( bdIn->bdPixelsHigh- row -1 )* bdIn->bdBytesPerRow;

	(*flip)( to, from, bdIn->bdPixelsWide, pixelsPerByte );
	}

    /* steal */
    *riOut= ri; bmInitRasterImage( &ri );

  ready:

    bmCleanRasterImage( &ri );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Mirror bitmap images: Flip over vertical axis.			*/
/*									*/
/************************************************************************/

int bmHorizontalFlip(	RasterImage *			riOut,
			const RasterImage *		riIn,
			int				ignoredInt )
    {
    int				rval= 0;
    const BitmapDescription *	bdIn= &(riIn->riDescription);

    unsigned int		row;
    int				pixelsPerByte= 8/ bdIn->bdBitsPerPixel;
    void			(*flip)( unsigned char *,
					const unsigned char *, int, int );

    RasterImage			ri;

    bmInitRasterImage( &ri );

    if  ( bmCopyDescription( &(ri.riDescription), bdIn ) )
	{ LDEB(1); rval= -1; goto ready;	}

    ri.riBytes= (unsigned char *)malloc( ri.riDescription.bdBufferLength );
    if  ( ! ri.riBytes )
	{ LLDEB(ri.riDescription.bdBufferLength,ri.riBytes); rval= -1; goto ready; }

    switch( bdIn->bdBitsPerPixel )
	{
	case  1:
	    flip= bmFlipBits;	bmFlip1Init( bdIn->bdPixelsWide ); break;
	case  2:
	    flip= bmFlipBits;	bmFlip2Init( bdIn->bdPixelsWide ); break;
	case  4:
	    flip= bmFlipBits;	bmFlip4Init( bdIn->bdPixelsWide ); break;
	case  8:
	    flip= bmFlip8Bits;	break;
	case 24:
	    flip= bmFlip24Bits;	break;
	default:
	    LDEB(bdIn->bdBitsPerPixel); rval= -1; goto ready;
	}

    for ( row= 0; row < bdIn->bdPixelsHigh; row++ )
	{
	const unsigned char *	from;
	unsigned char *		to;

	from= riIn->riBytes+ row* bdIn->bdBytesPerRow;
	to= ri.riBytes+ row* bdIn->bdBytesPerRow;

	(*flip)( to, from, bdIn->bdPixelsWide, pixelsPerByte );
	}

    /* steal */
    *riOut= ri; bmInitRasterImage( &ri );

  ready:

    bmCleanRasterImage( &ri );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Rotate a bitmap by 0,90,180,270 degrees.				*/
/*									*/
/************************************************************************/

int bmRotate90(		RasterImage *			riOut,
			const RasterImage *		riIn,
			int				angle )
    {
    int				rval= 0;
    const BitmapDescription *	bdIn= &(riIn->riDescription);

    const unsigned char *	from;
    unsigned char *		to;
    unsigned int		row, col;

    int				s;
    int				shift;
    unsigned char		m= 0;
    unsigned char		mr;
    int				step;

    RasterImage			ri;

    bmInitRasterImage( &ri );

    switch( angle )
	{
	case 0:
	    if  ( bmCopyRasterImage( riOut, riIn ) )
		{ LDEB(1); rval= -1; goto ready;	}
	    goto ready;

	case 180:
	    if  ( bmRotate180( riOut, riIn, angle ) )
		{ LDEB(180); rval= -1; goto ready;	}
	    goto ready;

	case 270:
	case 90:
	    break;

	default:
	    LDEB(angle); rval= -1; goto ready;
	}


    if  ( bmCopyDescription( &(ri.riDescription), bdIn ) )
	{ LDEB(1); rval= -1; goto ready;	}

    ri.riDescription.bdPixelsWide= bdIn->bdPixelsHigh;
    ri.riDescription.bdPixelsHigh= bdIn->bdPixelsWide;

    if  ( bmCalculateSizes( &(ri.riDescription) ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( bmAllocateBuffer( &ri ) )
	{ LLDEB(ri.riDescription.bdBufferLength,ri.riBytes); rval= -1; goto ready; }

    switch( bdIn->bdBitsPerPixel )
	{
	case  4:
	    m |= 0x30;
	    /*FALLTHROUGH*/
	case  2:
	    m |= 0x40;
	    /*FALLTHROUGH*/
	case  1:
	    m |= 0x80;

	    memset( ri.riBytes, 0, ri.riDescription.bdBufferLength );
	    step= 8/ bdIn->bdBitsPerPixel;

	    if  ( angle == 90 )
		{
		for ( row= 0; row < ri.riDescription.bdPixelsHigh; row++ )
		    {
		    int		toRow= ri.riDescription.bdPixelsHigh- row- 1;

		    mr= m >> bdIn->bdBitsPerPixel* ( row % step );

		    for ( s= 0; s < row % step; s++ )
			{
			to= ri.riBytes+ toRow* ri.riDescription.bdBytesPerRow;
			from= riIn->riBytes+ row/step+ s* bdIn->bdBytesPerRow;

			shift= row % step- s;
			shift *= bdIn->bdBitsPerPixel;

			for ( col= s; col < ri.riDescription.bdPixelsWide; col += step )
			    {
			    *(to++) |= ( *from & mr ) << shift;
			    from += step* bdIn->bdBytesPerRow;
			    }
			}

		    for ( s= row % step; s < step; s++ )
			{
			to= ri.riBytes+ toRow* ri.riDescription.bdBytesPerRow;
			from= riIn->riBytes+ row/step+ s* bdIn->bdBytesPerRow;

			shift= s- row % step;
			shift *= bdIn->bdBitsPerPixel;

			for ( col= s; col < ri.riDescription.bdPixelsWide; col += step )
			    {
			    *(to++) |= ( *from & mr ) >> shift;
			    from += step* bdIn->bdBytesPerRow;
			    }
			}
		    }
		}
	    else{
		/*270*/
		for ( row= 0; row < ri.riDescription.bdPixelsHigh; row++ )
		    {
		    mr= m >> bdIn->bdBitsPerPixel* ( row % step );

		    for ( s= 0; s < row % step; s++ )
			{
			int r= bdIn->bdPixelsHigh- 1;

			to= ri.riBytes+ row* ri.riDescription.bdBytesPerRow;
			from= riIn->riBytes+ ( bdIn->bdPixelsHigh- 1 )*
							bdIn->bdBytesPerRow;
			from -= s* bdIn->bdBytesPerRow;
			r -= s;
			from += row/ step;

			shift= row % step- s;
			shift *= bdIn->bdBitsPerPixel;

			for ( col= 0; col < ri.riDescription.bdPixelsWide; col += step )
			    {
			    if  ( r < 0 )
				{ break;	}

			    *(to++) |= ( *from & mr ) << shift;
			    from -= step* bdIn->bdBytesPerRow;
			    r -= step;
			    }
			}

		    for ( s= row % step; s < step; s++ )
			{
			int r= bdIn->bdPixelsHigh- 1;

			to= ri.riBytes+ row* ri.riDescription.bdBytesPerRow;
			from= riIn->riBytes+ ( bdIn->bdPixelsHigh- 1 )*
							bdIn->bdBytesPerRow;
			from -= s* bdIn->bdBytesPerRow;
			r -= s;
			from += row/ step;

			shift= s- row % step;
			shift *= bdIn->bdBitsPerPixel;

			for ( col= 0; col < ri.riDescription.bdPixelsWide; col += step )
			    {
			    if  ( r < 0 )
				{ break;	}

			    *(to++) |= ( *from & mr ) >> shift;
			    from -= step* bdIn->bdBytesPerRow;
			    r -= step;
			    }
			}
		    }
		}

	    break;
	case  8:
	    if  ( angle == 90 )
		{
		for ( row= 0; row < ri.riDescription.bdPixelsHigh; row++ )
		    {
		    from= riIn->riBytes+ row;
		    to= ri.riBytes+ ( ri.riDescription.bdPixelsHigh- row- 1 )* ri.riDescription.bdBytesPerRow;

		    for ( col= 0; col < ri.riDescription.bdPixelsWide; col++ )
			{ *(to++)= *from; from += bdIn->bdBytesPerRow; }
		    }
		}
	    else{
		/*270*/
		for ( row= 0; row < ri.riDescription.bdPixelsHigh; row++ )
		    {
		    from= riIn->riBytes+ row;
		    to= ri.riBytes+ ( row+ 1 )* ri.riDescription.bdBytesPerRow- 1;

		    for ( col= 0; col < ri.riDescription.bdPixelsWide; col++ )
			{ *(to--)= *from; from += bdIn->bdBytesPerRow; }
		    }
		}
	    break;

	case 24:
	    if  ( angle == 90 )
		{
		for ( row= 0; row < ri.riDescription.bdPixelsHigh; row++ )
		    {
		    from= riIn->riBytes+ 3* row;
		    to= ri.riBytes+ ( ri.riDescription.bdPixelsHigh- row- 1 )* ri.riDescription.bdBytesPerRow;

		    for ( col= 0; col < ri.riDescription.bdPixelsWide; col++ )
			{
			*(to++)= from[0]; *(to++)= from[1]; *(to++)= from[2];
			from += bdIn->bdBytesPerRow;
			}
		    }
		}
	    else{
		for ( row= 0; row < ri.riDescription.bdPixelsHigh; row++ )
		    {
		    from= riIn->riBytes+ 3* row;
		    to= ri.riBytes+ ( row+ 1 )* ri.riDescription.bdBytesPerRow- 3;

		    for ( col= 0; col < ri.riDescription.bdPixelsWide; col++ )
			{
			to[0]= from[0]; to[1]= from[1]; to[2]= from[2];
			to -= 3; from += bdIn->bdBytesPerRow;
			}
		    }
		}
	    break;
	default:
	    LDEB(bdIn->bdBitsPerPixel); rval= -1; goto ready;
	}

    /* steal */
    *riOut= ri; bmInitRasterImage( &ri );

  ready:

    bmCleanRasterImage( &ri );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Shear operations.							*/
/*									*/
/************************************************************************/

typedef void (*ShearRow)(	unsigned char *			rowBufOut,
				const unsigned char *		rowBufIn,
				const BitmapDescription *	bdIn,
				const BitmapDescription *	bdOut,
				int				colShift,
				int				fl );

typedef void (*ShearCol)(	unsigned char *			imgBufOut,
				const unsigned char *		imgBufIn,
				const BitmapDescription *	bdIn,
				const BitmapDescription *	bdOut,
				int				col,
				int				rowShift,
				int				fl );

static void bmShearRow8Bit(	unsigned char *			rowBufOut,
				const unsigned char *		rowBufIn,
				const BitmapDescription *	bdIn,
				const BitmapDescription *	bdOut,
				int				colShift,
				int				fl )
    {
    int		fr= 255- fl;
    int		spp= bdIn->bdSamplesPerPixel;
    int		i;

    int		colIn0;
    int		colIn1;
    int		colIn;

    int		prev[5];

    /*  1  */
    colIn0= 0;
    colIn1= bdIn->bdPixelsWide- 1;

    if  ( colShift < 0 )
	{ colIn0= -colShift; }

    if  ( colIn1+ colShift >= bdOut->bdPixelsWide )
	{ colIn1= bdOut->bdPixelsWide- colShift- 1; }

    rowBufOut += spp* colShift;

    for ( i= 0; i < spp; i++ )
	{ prev[i]= rowBufIn[i]; }

    for ( colIn= colIn0; colIn <= colIn1; colIn++ )
	{
	const unsigned char *	f= rowBufIn;
	unsigned char *		t= rowBufOut;

	for ( i= 0; i < spp; i++ )
	    {
	    t[0]= ( ( fl* prev[i] )+ ( fr* f[0] ) )/ 255;
	    prev[i]= f[0];

	    f++; t++;
	    }

	rowBufOut += spp; rowBufIn += spp;
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Shear one column in a one byte per sample image.			*/
/*									*/
/*  1)  For all rows of the input image.				*/
/*									*/
/************************************************************************/

static void bmShearCol8Bit(	unsigned char *			imgBufOut,
				const unsigned char *		imgBufIn,
				const BitmapDescription *	bdIn,
				const BitmapDescription *	bdOut,
				int				col,
				int				rowShift,
				int				fl )
    {
    int				fr= 255- fl;
    int				spp= bdIn->bdSamplesPerPixel;
    int				i;

    int				rowIn0;
    int				rowIn1;
    int				rowIn;
    int				rowTo0;
    int				prev[5];

    const unsigned char *	from;
    unsigned char *		to;

    for ( i= 0; i < spp; i++ )
	{ prev[i]= 0; }

    /*  1  */
    rowIn0= 0;
    rowIn1= bdIn->bdPixelsHigh- 1;

    if  ( rowShift < 0 )
	{ rowIn0= -rowShift; }
    rowTo0= rowIn0+ rowShift;

    if  ( rowIn1+ rowShift >= bdOut->bdPixelsHigh )
	{ rowIn1= bdOut->bdPixelsHigh- rowShift- 1; }

    from= imgBufIn+ rowIn0* bdIn->bdBytesPerRow+ spp* col;
    to= imgBufOut+ rowTo0* bdOut->bdBytesPerRow+ spp* col;

    /*  1  */
    for ( rowIn= rowIn0; rowIn <= rowIn1; rowIn++ )
	{
	const unsigned char *	f= from;
	unsigned char *		t= to;

	for ( i= 0; i < spp; i++ )
	    {
	    t[0]= ( ( fl* prev[i] )+ ( fr* f[0] ) )/ 255;
	    prev[i]= f[0];

	    f++; t++;
	    }

	from += bdIn->bdBytesPerRow;
	to += bdOut->bdBytesPerRow;
	}

    return;
    }

static int bmShearRows(		unsigned char *			bufOut,
				const BitmapDescription *	bdOut,
				int				shear,
				const DocumentRectangle *	drOut,
				const RasterImage *		riIn,
				ShearRow			shearRow )
    {
    const BitmapDescription *	bdIn= &(riIn->riDescription);
    int				row;

    int				fl;
    int				colShift;

    if  ( bdOut->bdPixelsHigh != bdIn->bdPixelsHigh )
	{ LLDEB(bdOut->bdPixelsHigh,bdIn->bdPixelsHigh); return -1; }

    for ( row= 0; row < bdIn->bdPixelsHigh; row++ )
	{
	unsigned char *		rowBufOut;
	const unsigned char *	rowBufIn;

	rowBufOut= bufOut+ row* bdOut->bdBytesPerRow;
	rowBufIn= riIn->riBytes+ row* bdIn->bdBytesPerRow;

	fl= ( 255* shear* row )/ (int)bdIn->bdPixelsHigh;
	colShift= ( shear* row )/ (int)bdIn->bdPixelsHigh;

	if  ( fl >= 0 )
	    { fl -= 255* colShift;		}
	else{ fl -= 255* colShift; fl += 255;	}

	colShift -= drOut->drX0;

	(*shearRow)( rowBufOut, rowBufIn, bdIn, bdOut, colShift, fl );
	}

    return 0;
    }

static int bmShearCols(		unsigned char *			bufOut,
				const BitmapDescription *	bdOut,
				int				shear,
				const DocumentRectangle *	drOut,
				const RasterImage *		riIn,
				ShearCol			shearCol )
    {
    const BitmapDescription *	bdIn= &(riIn->riDescription);
    int				col;

    if  ( bdOut->bdPixelsWide != bdIn->bdPixelsWide )
	{ LLDEB(bdOut->bdPixelsWide,bdIn->bdPixelsWide); return -1; }

    for ( col= 0; col < bdIn->bdPixelsWide; col++ )
	{
	int			fl;
	int			rowShift;

	fl= ( 255* shear* col )/ (int)bdIn->bdPixelsWide;
	rowShift= ( shear* col )/ (int)bdIn->bdPixelsWide;

	if  ( fl >= 0 )
	    { fl -= 255* rowShift;		}
	else{ fl -= 255* rowShift; fl += 255;	}

	rowShift -= drOut->drY0;

	(*shearCol)( bufOut, riIn->riBytes, bdIn, bdOut, col, rowShift, fl );
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Apply an affine transform to the rectangle from the original image.	*/
/*									*/
/*  The routine used to determine the smallest rectangle that encloses	*/
/*  the result of an intermediate result in the rotation of an image.	*/
/*									*/
/************************************************************************/

static void bmTransformRectangle(
				BitmapDescription *		bd,
				Point2DI			to[4],
				DocumentRectangle *		drTo,
				const AffineTransform2D *	at,
				const Point2DI			from[4],
				const DocumentRectangle *	drFrom )
    {
    int		i;

    for ( i= 0; i < 4; i++ )
	{
	to[i].x= AT2_X( from[i].x- drFrom->drX0,
					    from[i].y- drFrom->drY0, at );
	to[i].y= AT2_Y( from[i].x- drFrom->drX0,
					    from[i].y- drFrom->drY0, at );
	}

    drTo->drX0= drTo->drX1= to[0].x;
    drTo->drY0= drTo->drY1= to[0].y;

    for ( i= 1; i < 4; i++ )
	{
	if  ( drTo->drX0 > to[i].x )
	    { drTo->drX0=  to[i].x;	}
	if  ( drTo->drY0 > to[i].y )
	    { drTo->drY0=  to[i].y;	}

	if  ( drTo->drX1 < to[i].x )
	    { drTo->drX1=  to[i].x;	}
	if  ( drTo->drY1 < to[i].y )
	    { drTo->drY1=  to[i].y;	}
	}

    bd->bdPixelsWide= drTo->drX1- drTo->drX0+ 1;
    bd->bdPixelsHigh= drTo->drY1- drTo->drY0+ 1;

    bmCalculateSizes( bd );

    return;
    }

/************************************************************************/
/*									*/
/*  Rotate a pixmap image over an arbitrary angle.			*/
/*									*/
/*  Use the algorithm outlined in:					*/
/*  PAETH, Alan: "A Fast Algorithm for General Raster Rotation" pp 179-	*/
/*  195 in: GLASSNER, Andrew S: "Graphics Gems", Academic Press,	*/
/*  Boston, 1990.							*/
/*									*/
/*  The two main advantages of the algorithm are fast and easy anti-	*/
/*  aliassing and the simple continuous loops that prevent holes in the	*/
/*  rendering.								*/
/*									*/
/*  Counterclockwise rotation of (x,y) onto (x',y') by an angle theta	*/
/*  is a multiplication of the vector my the matrix:			*/
/*	[  cos(theta) -sin(theta) ]					*/
/*	[  sin(theta)  cos(theta) ]					*/
/*  The matrix is orhogonal: the inverse is the transpose:		*/
/*	[  cos(theta)  sin(theta) ]					*/
/*	[ -sin(theta)  cos(theta) ]					*/
/*  It can be written as the product of three shears:			*/
/*	[ 1 a ] [ 1 0 ] [ 1 c ]   [ cos(theta) -sin(theta) ]		*/
/*	[ 0 1 ] [ b 1 ] [ 0 1 ] = [ sin(theta)  cos(theta) ]		*/
/*  Where:								*/
/*      a= -tan( theta/ 2 ).						*/
/*      b= sin( theta ).						*/
/*      c= -tan( theta/ 2 ).						*/
/*									*/
/*  1)  Find a theta <= 45 degrees.					*/
/*  2)  Initial X Shear.						*/
/*  3)  Subsequent Y Shear.						*/
/*  4)  Final X Shear.							*/
/*									*/
/************************************************************************/

int bmRotate(	RasterImage *			riOut,
		const RasterImage *		riIn,
		double				theta )
    {
    const BitmapDescription *	bdIn= &(riIn->riDescription);
    int				rval= 0;

    int				rot90;

    double			a;
    double			b;
    double			c;

    AffineTransform2D		C;
    AffineTransform2D		B;
    AffineTransform2D		A;

    Point2DI			rectIn[4];
    Point2DI			rectC[4];
    Point2DI			rectB[4];
    Point2DI			rectA[4];

    DocumentRectangle		drIn;
    DocumentRectangle		drC;
    DocumentRectangle		drB;
    DocumentRectangle		drA;

    int				shearC;
    int				shearB;
    int				shearA;

    RasterImage			riC;
    RasterImage			riB;
    RasterImage			riA;

    ShearRow			shearRow= (ShearRow)0;
    ShearCol			shearCol= (ShearCol)0;

    bmInitRasterImage( &riC );
    bmInitRasterImage( &riB );
    bmInitRasterImage( &riA );

    geoIdentityAffineTransform2D( &C );
    geoIdentityAffineTransform2D( &B );
    geoIdentityAffineTransform2D( &A );

    switch( bdIn->bdColorEncoding )
	{
	case BMcoBLACKWHITE:
	case BMcoWHITEBLACK:
	    switch( bdIn->bdBitsPerPixel )
		{
		case 8:
		    shearRow= bmShearRow8Bit;
		    shearCol= bmShearCol8Bit;
		    break;

		default:
		    LLDEB(bdIn->bdColorEncoding,bdIn->bdBitsPerPixel);
		    rval= -1; goto ready;
		}
	    break;

	case BMcoRGB:
	    switch( bdIn->bdBitsPerSample )
		{
		case 8:
		    shearRow= bmShearRow8Bit;
		    shearCol= bmShearCol8Bit;
		    break;

		default:
		    LLDEB(bdIn->bdColorEncoding,bdIn->bdBitsPerPixel);
		    rval= -1; goto ready;
		}
	    break;

	case BMcoRGB8PALETTE:
	    LDEB(bdIn->bdColorEncoding);
	    rval= -1; goto ready;
	    break;

	default:
	    LDEB(bdIn->bdColorEncoding);
	    rval= -1; goto ready;
	}

    /*********/
    /*  0 1  */
    /*  3 2  */
    /*********/
    drIn.drX0= 0; drIn.drX1= bdIn->bdPixelsWide- 1;
    drIn.drY0= 0; drIn.drY1= bdIn->bdPixelsHigh- 1;

    rectIn[0].x= 0;		rectIn[0].y= 0;
    rectIn[1].x= drIn.drX1;	rectIn[1].y= 0;
    rectIn[2].x= drIn.drX1;	rectIn[2].y= drIn.drY1;
    rectIn[3].x= 0;		rectIn[3].y= drIn.drY1;

    /*  1  */
    while( theta < -M_PI/4 )
	{ theta += 2* M_PI;	}
    for ( rot90= 0; theta > M_PI/4; rot90++ )
	{ theta -= M_PI/2;	}
    rot90= rot90 % 4;

    c= -tan( theta/ 2.0 );
    b= sin( theta );
    a= -tan( theta/ 2.0 );

    C.at2Ayx= c;
    B.at2Axy= b;
    A.at2Ayx= a;

    /*  2  */
    if  ( bmCopyDescription( &(riC.riDescription), bdIn ) )
	{ LDEB(1); rval= -1; goto ready;	}

    bmTransformRectangle( &(riC.riDescription), rectC, &drC, &C,
							rectIn, &drIn );

    shearC= c* bdIn->bdPixelsHigh;

    riC.riBytes= (unsigned char *)malloc( riC.riDescription.bdBufferLength );
    if  ( ! riC.riBytes )
	{ LLDEB(riC.riDescription.bdBufferLength,riC.riBytes); rval= -1; goto ready;	}
    memset( riC.riBytes, 0, riC.riDescription.bdBufferLength );

    if  ( bmShearRows( riC.riBytes, &(riC.riDescription), shearC, &drC, riIn, shearRow ) )
	{ LDEB(1); rval= -1; goto ready; }

#   define	RET_C	0
#   if		RET_C
    bmCopyDescription( bdOut, &bdC );
    *pBufOut= bufC;
    bufC= (unsigned char *)0;
    goto ready;
#   endif
#   define	NO_C	0
#   if		NO_C
    bmCopyDescription(&bdC,bdIn);
    memcpy( bufC, bufIn, bdIn->bdBufferLength );
#   endif

    /*  3  */
    if  ( bmCopyDescription( &(riB.riDescription), &(riC.riDescription) ) )
	{ LDEB(1); rval= -1; goto ready;	}
    bmTransformRectangle( &(riB.riDescription), rectB, &drB, &B, rectC, &drC );

    shearB= b* riC.riDescription.bdPixelsWide;

    riB.riBytes= (unsigned char *)malloc( riB.riDescription.bdBufferLength );
    if  ( ! riB.riBytes )
	{ LLDEB(riB.riDescription.bdBufferLength,riB.riBytes); rval= -1; goto ready;	}
    memset( riB.riBytes, 0, riB.riDescription.bdBufferLength );

    if  ( bmShearCols( riB.riBytes, &(riB.riDescription), shearB, &drB,
							    &riC, shearCol ) )
	{ LDEB(1); rval= -1; goto ready; }

#   define	RET_B	0
#   if		RET_B
    bmCopyDescription( bdOut, &bdB );
    *pBufOut= bufB;
    bufB= (unsigned char *)0;
    goto ready;
#   endif
#   define	NO_B	0
#   if		NO_B
    bmCopyDescription(&bdB,&bdC);
    memcpy( bufB, bufC, bdC.bdBufferLength );
#   endif

    /*  4  */
    if  ( bmCopyDescription( &(riA.riDescription), &(riB.riDescription) ) )
	{ LDEB(1); rval= -1; goto ready;	}
    bmTransformRectangle( &(riA.riDescription), rectA, &drA, &A, rectB, &drB );

    shearA= a* riB.riDescription.bdPixelsHigh;

    riA.riBytes= (unsigned char *)malloc( riA.riDescription.bdBufferLength );
    if  ( ! riA.riBytes )
	{ LLDEB(riA.riDescription.bdBufferLength,riA.riBytes); rval= -1; goto ready;	}
    memset( riA.riBytes, 0, riA.riDescription.bdBufferLength );

    if  ( bmShearRows( riA.riBytes, &(riA.riDescription), shearA, &drA, &riB, shearRow ) )
	{ LDEB(1); rval= -1; goto ready; }

    if  ( rot90 != 0 )
	{
	if  ( bmRotate90( riOut, &riA, 90* rot90 ) )
	    { LDEB(rot90); rval= -1; goto ready; }
	}
    else{
	/*  steal */
	*riOut= riA; bmInitRasterImage( &riA );
	}

  ready:

    bmCleanRasterImage( &riC );
    bmCleanRasterImage( &riB );
    bmCleanRasterImage( &riA );

    return rval;
    }
