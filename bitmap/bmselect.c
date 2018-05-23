#   include	"bitmapConfig.h"

#   include	<string.h>
#   include	<stdlib.h>
#   include	"bitmap.h"

#   include	<appDebugon.h>

int bmSelect(	RasterImage *			riOut,
		const RasterImage *		riIn,
		const DocumentRectangle *	drSel )
    {
    const BitmapDescription *	bdIn= &(riIn->riDescription);
    int				rval= 0;

    RasterImage			ri;

    int				originBit;
    int				originByte;
    int				row;

    bmInitRasterImage( &ri );

    if  ( drSel->drX0 < 0			||
	  drSel->drY0 < 0			||
	  drSel->drX1 >= bdIn->bdPixelsWide	||
	  drSel->drY1 >= bdIn->bdPixelsHigh	||
	  drSel->drX0 >= drSel->drX1		||
	  drSel->drY0 >= drSel->drY1		)
	{
	RECTDEB(drSel);
	LLDEB(bdIn->bdPixelsWide,bdIn->bdPixelsHigh);
	rval= -1; goto ready;
	}

    /************************************************************/
    /*  Derive properties of output bitmap from input.		*/
    /************************************************************/
    bmCopyDescription( &(ri.riDescription), bdIn );
    ri.riDescription.bdPixelsWide= drSel->drX1- drSel->drX0+ 1;
    ri.riDescription.bdPixelsHigh= drSel->drY1- drSel->drY0+ 1;

    bmCalculateSizes( &(ri.riDescription) );

    /************************************************************/
    /*  How many bits to shift?					*/
    /************************************************************/
    originBit=  ( drSel->drX0* bdIn->bdBitsPerPixel )% 8;
    originByte= ( drSel->drX0* bdIn->bdBitsPerPixel )/ 8;

    /************************************************************/
    /*  Allocate new buffer.					*/
    /************************************************************/
    if  ( bmAllocateBuffer( &ri ) )
	{ LLDEB(ri.riDescription.bdBufferLength,ri.riBytes); rval= -1; goto ready;	}

    if  ( originBit == 0 )
	{
	for ( row= 0; row < ri.riDescription.bdPixelsHigh; row++ )
	    {
	    memcpy( ri.riBytes+ row* ri.riDescription.bdBytesPerRow,
		riIn->riBytes+ ( row+ drSel->drY0 )* bdIn->bdBytesPerRow+ originByte,
		ri.riDescription.bdBytesPerRow );
	    }
	}
    else{
	int	rightShift= 8- originBit;

	for ( row= 0; row < ri.riDescription.bdPixelsHigh; row++ )
	    {
	    int				byte;
	    unsigned char *		to;
	    const unsigned char *	from;

	    to= ri.riBytes+ row* ri.riDescription.bdBytesPerRow;
	    from= riIn->riBytes+ ( row+ drSel->drY0 )* bdIn->bdBytesPerRow+ originByte;

	    *to=   *from << originBit; from++;

	    for ( byte= 1; byte < ri.riDescription.bdBytesPerRow; byte++ )
		{
		*to |= *from >> rightShift; to++;
		*to=   *from << originBit; from++;
		}

	    /*
	    *to |= *from >> rightShift;
	    */
	    }
	}

    /* steal */
    *riOut= ri; bmInitRasterImage( &ri );

  ready:

    bmCleanRasterImage( &ri );

    return rval;
    }

# if 0

/* To be run through valgrind */
extern void bmSelectTest( void );

void bmSelectTest( void )
    {
    BitmapDescription	bdIn;
    unsigned char *	bufIn= (unsigned char *)0;
    const int		IN_SZ= 25;
    const int		OUT_SZ= 8;
    int			i;

    int			format;
    char		fileName[30];

    bmInitDescription( &bdIn );

    bdIn.bdPixelsWide= IN_SZ;
    bdIn.bdPixelsHigh= IN_SZ;
    bdIn.bdColorEncoding= BMcoBLACKWHITE;
    bdIn.bdBitsPerPixel= 1;
    bdIn.bdBitsPerSample= 1;
    bdIn.bdUnit= BMunPIXEL;
    bdIn.bdXResolution= 1;
    bdIn.bdYResolution= 1;
    bdIn.bdHasAlpha= 0;

    bmCalculateSizes( &bdIn );
    bufIn= malloc( bdIn.bdBufferLength );
    memset( bufIn, 0x55, bdIn.bdBufferLength );

    sprintf( fileName, "/tmp/bsin.png" );
    format= bmSuggestFormat( fileName, -1, &bdIn );
    SDEB(fileName);
    bmWrite( fileName, bufIn, &bdIn, format );

    for ( i= IN_SZ- OUT_SZ- 6; i <= IN_SZ- OUT_SZ; i++ )
	{
	BitmapDescription	bdOut;
	unsigned char *		bufOut= (unsigned char *)0;
	DocumentRectangle	drSel;

	drSel.drX0= i;
	drSel.drY0= i;
	drSel.drX1= i+ OUT_SZ- 1;
	drSel.drY1= i+ OUT_SZ- 1;

	LDEB(i);
	bmInitDescription( &bdOut );
	bmSelect( &bdOut, &bufOut, &bdIn, bufIn, &drSel );

	sprintf( fileName, "/tmp/bsin-%d.png", i );
	format= bmSuggestFormat( fileName, -1, &bdOut );
	SDEB(fileName);
	bmWrite( fileName, bufOut, &bdOut, format );

	bmCleanDescription( &bdOut );
	free( bufOut );
	}

    bmCleanDescription( &bdIn );
    free( bufIn );
    }
# endif

/************************************************************************/
/*									*/
/*  Copy parts of an image into another one.				*/
/*									*/
/************************************************************************/

int bmCopyArea(			int				x0,
				int				y0,
				RasterImage *			riTo,
				const RasterImage *		riFrom )
    {
    int			rval= 0;
    int *		colorMap= (int *)0;

    int			x1;
    int			y1;

    int			y;

    int			targetBit;
    int			targetByte;
    int			copyBits;
    int			copyBytes;
    int			tailBits;

    BitmapDescription *		bdTo= &(riTo->riDescription);
    const BitmapDescription *	bdFrom= &(riFrom->riDescription);

    /************************************************************/
    /*  Same kind of image?					*/
    /************************************************************/
    if  ( bdTo->bdBitsPerSample != bdFrom->bdBitsPerSample )
	{
	LLDEB(bdTo->bdBitsPerSample,bdFrom->bdBitsPerSample);
	rval= -1; goto ready;
	}
    if  ( bdTo->bdSamplesPerPixel != bdFrom->bdSamplesPerPixel )
	{
	LLDEB(bdTo->bdSamplesPerPixel,bdFrom->bdSamplesPerPixel);
	rval= -1; goto ready;
	}
    if  ( bdTo->bdBitsPerPixel != bdFrom->bdBitsPerPixel )
	{
	LLDEB(bdTo->bdBitsPerPixel,bdFrom->bdBitsPerPixel);
	rval= -1; goto ready;
	}
    if  ( bdTo->bdColorEncoding != bdFrom->bdColorEncoding )
	{
	LLDEB(bdTo->bdColorEncoding,bdFrom->bdColorEncoding);
	rval= -1; goto ready;
	}

    if  ( bdTo->bdColorEncoding == BMcoRGB8PALETTE )
	{
	const int	avoidZero= 0;
	const int	maxColors= 1 << bdTo->bdBitsPerPixel;

	if  ( utilMergeColorPalettes( &colorMap,
				    &(bdTo->bdPalette), &(bdFrom->bdPalette),
				    avoidZero, maxColors ) )
	    { LDEB(maxColors); rval= -1; goto ready;	}
	}

    /************************************************************/
    /*  Origin inside target?					*/
    /************************************************************/
    if  ( x0 < 0 || y0 < 0 )
	{ LLDEB(x0,y0); rval= -1; goto ready;	}
    if  ( x0 >= bdTo->bdPixelsWide || y0 >= bdTo->bdPixelsHigh )
	{
	LLLLDEB(x0,y0,bdTo->bdPixelsWide,bdTo->bdPixelsHigh);
	rval= -1; goto ready;
	}

    x1= x0+ bdFrom->bdPixelsWide;
    if  ( x1 > bdTo->bdPixelsWide )
	{ x1=  bdTo->bdPixelsWide;	}

    y1= y0+ bdFrom->bdPixelsHigh;
    if  ( y1 > bdTo->bdPixelsHigh )
	{ y1=  bdTo->bdPixelsHigh;	}

    /************************************************************/
    /*  How many bits to copy/shift?				*/
    /************************************************************/
    targetBit=  ( x0* bdTo->bdBitsPerPixel )% 8;
    targetByte= ( x0* bdTo->bdBitsPerPixel )/ 8;

    copyBits= ( x1- x0 )* bdTo->bdBitsPerPixel;
    if  ( targetBit != 0 )
	{ copyBits -= 8- targetBit;	}
    copyBytes= copyBits/ 8;
    tailBits=  copyBits% 8;

    if  ( targetBit == 0 )
	{
	unsigned char	t1Mask= 0xff >> tailBits;
	unsigned char	t2Mask= 0xff << ( 8- tailBits );

	for ( y= y0; y < y1; y++ )
	    {
	    unsigned char *		r1;
	    const unsigned char *	r2;

	    r1= riTo->riBytes+   ( y-  0 )* bdTo->bdBytesPerRow;
	    r2= riFrom->riBytes+ ( y- y0 )* bdFrom->bdBytesPerRow;

	    r1 += targetByte;

	    if  ( colorMap && bdTo->bdBitsPerPixel == 8 )
		{
		int	i;

		for ( i= 0; i < copyBytes; i++ )
		    { r1[i]= colorMap[r2[i]];	}
		}
	    else{
		memcpy( r1, r2, copyBytes );
		}

	    if  ( tailBits > 0 )
		{
		r1 += copyBytes;
		r2 += copyBytes;

		*r1 &= t1Mask;
		*r1 |= *r2 & t2Mask;
		}
	    }
	}
    else{
	unsigned char	h1Mask= ~( 0xff >> targetBit );

	unsigned char	t1Mask= 0xff >> tailBits;
	unsigned char	t2Mask= 0xff << ( 8- tailBits );

	for ( y= y0; y < y1; y++ )
	    {
	    unsigned char *		r1;
	    const unsigned char *	r2;
	    int				i;
	    unsigned char		c;


	    r1= riTo->riBytes+   ( y-  0 )* bdTo->bdBytesPerRow;
	    r2= riFrom->riBytes+ ( y- y0 )* bdFrom->bdBytesPerRow;

	    r1 += targetByte;

	    c= ( *r1 & h1Mask ) | ( *r2 >> targetBit );
	    *(r1++)= c;

	    for ( i= 0; i < copyBytes; i++ )
		{
		c= *(r2++) << ( 8- targetBit );
		c |= *r2 >> targetBit;
		*(r1++)= c;
		}

	    if  ( tailBits > 0 )
		{
		c= *(r2++) << ( 8- targetBit );
		c |= *r2 >> targetBit;

		*r1 &= t1Mask;
		*r1 |= c & t2Mask;
		}
	    }
	}

  ready:

    if  ( colorMap )
	{ free( colorMap );	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Transfer parts of an image into another one: Only locations that	*/
/*  are not transparent are affected.					*/
/*									*/
/************************************************************************/

static int bmPaintArea8(	int				x0,
				int				x1,
				int				y0,
				int				y1,
				unsigned char *			buffer1,
				const unsigned char *		buffer2,
				const BitmapDescription *	bdTo,
				const BitmapDescription *	bdFrom )
    {
    int		yf;
    int		yt;
    const int	spp1= bdTo->bdSamplesPerPixel;
    const int	spp2= bdFrom->bdSamplesPerPixel;

    yf= 0;
    for ( yt= y0; yt < y1; yf++, yt++ )
	{
	int			x;
	unsigned char *		to= buffer1+ yt* bdTo->bdBytesPerRow+ x0* spp1;
	const unsigned char *	fr= buffer2+ yf* bdFrom->bdBytesPerRow;

	for ( x= x0; x < x1; x++ )
	    {
	    if  ( fr[spp2- 1] )
		{ memcpy( to, fr, spp1 );	}

	    to += spp1; fr += spp2;
	    }
	}

    return 0;
    }

int bmPaintArea(		int				x0,
				int				y0,
				RasterImage *			riTo,
				const RasterImage *		riFrom )
    {
    int				x1;
    int				y1;

    const BitmapDescription *	bdTo= &(riTo->riDescription);
    const BitmapDescription *	bdFrom= &(riFrom->riDescription);

    if  ( ! bdFrom->bdHasAlpha )
	{ return bmCopyArea( x0, y0, riTo, riFrom );	}

    /************************************************************/
    /*  Same kind of image?					*/
    /************************************************************/
    if  ( bdTo->bdBitsPerSample != bdFrom->bdBitsPerSample )
	{ LLDEB(bdTo->bdBitsPerSample,bdFrom->bdBitsPerSample); return -1;	}
    if  ( bdTo->bdColorEncoding != bdFrom->bdColorEncoding )
	{ LLDEB(bdTo->bdColorEncoding,bdFrom->bdColorEncoding); return -1; }

    if  ( bdTo->bdSamplesPerPixel+ !bdTo->bdHasAlpha != bdFrom->bdSamplesPerPixel )
	{
	LLLDEB(bdTo->bdSamplesPerPixel,bdTo->bdHasAlpha,bdFrom->bdSamplesPerPixel);
	return -1;
	}

    /************************************************************/
    /*  Origin inside target?					*/
    /************************************************************/
    if  ( x0 < 0 || y0 < 0 )
	{ LLDEB(x0,y0); return -1;	}
    if  ( x0 >= bdTo->bdPixelsWide || y0 >= bdTo->bdPixelsHigh )
	{ LLLLDEB(x0,y0,bdTo->bdPixelsWide,bdTo->bdPixelsHigh); return -1; }

    x1= x0+ bdFrom->bdPixelsWide;
    if  ( x1 > bdTo->bdPixelsWide )
	{ x1=  bdTo->bdPixelsWide;	}

    y1= y0+ bdFrom->bdPixelsHigh;
    if  ( y1 > bdTo->bdPixelsHigh )
	{ y1=  bdTo->bdPixelsHigh;	}

    switch( bdTo->bdBitsPerSample )
	{
	case 8:
	    return bmPaintArea8( x0, x1, y0, y1,
				riTo->riBytes, riFrom->riBytes, bdTo, bdFrom );

	default:
	    LDEB(bdTo->bdBitsPerSample); return -1;
	}
    }

