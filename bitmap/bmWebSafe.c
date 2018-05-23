#   include	"bitmapConfig.h"

#   include	"bmRender.h"
#   include	<stdlib.h>
#   include	<appDebugon.h>

# define N(r,g,b) ( 6* 6* ( (r)/ 43 )+ 6* ( (g)/ 43 )+ ( (b) / 43 ) )

/************************************************************************/
/*									*/
/*  Map an image to the so called web safe palette.			*/
/*									*/
/************************************************************************/

static void bmWebSafeCleanupAllocator(	ColorAllocator *	ca )
    {
    return;
    }

/************************************************************************/
/*									*/
/*  Set resources in a grayscale allocator, depending on the number of	*/
/*  bits.								*/
/*									*/
/************************************************************************/

static int bmWebSafeSetAllocator(	ColorAllocator *	ca,
					int			bitsPerPixel,
					SystemAllocator		sysAllocator )
    {
    if  ( bitsPerPixel != 8 )
	{ LDEB(bitsPerPixel); return -1;	}

    ca->caSystemAllocator= sysAllocator;
    ca->caSystemCleanup= bmWebSafeCleanupAllocator;
    ca->caAllocationType= CA_ALLOCATOR;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Allocate a 'Web Safe' color.					*/
/*									*/
/************************************************************************/

static int bmToWebSafeAllocateColor(	AllocatorColor *	ac,
					ColorAllocator *	ca,
					unsigned int		r,
					unsigned int		g,
					unsigned int		b )
    {
    ac->acRed= 257* r;
    ac->acGreen= 257* g;
    ac->acBlue= 257* b;
    ac->acColorNumber= N( r, g, b );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Convert an image to the web safe palette.				*/
/*									*/
/*  1)  Check input format.						*/
/*  2)	Derive properties of output bitmap from input.			*/
/*  4)	Allocate output buffer.						*/
/*  5)	Fill it.							*/
/*  6)	Return outputs.							*/
/*  7)	Cleanup.							*/
/*									*/
/************************************************************************/

int bmToWebSafe(	RasterImage *			riOut,
			const RasterImage *		riIn,
			int				ignoredInt )
    {
    const BitmapDescription *	bdIn= &(riIn->riDescription);
    int				rval= 0;

    RasterImage			ri;

    ColorAllocator		ca;

    int				bitmapUnit= 0;
    int				swapBitmapBytes= 0;
    int				swapBitmapBits= 0;
    const int			dither= 0;

    bmInitRasterImage( &ri );
    bmInitColorAllocator( &ca );

    /*  1  */
    switch( bdIn->bdColorEncoding )
	{
	case BMcoRGB:
	case BMcoRGB8PALETTE:
	    break;

	case BMcoBLACKWHITE:
	case BMcoWHITEBLACK:
	default:
	    LDEB(bdIn->bdColorEncoding);
	    rval= -1; goto ready;
	}

    switch( bdIn->bdBitsPerSample )
	{
	case 8:
	    break;
	default:
	    LDEB(bdIn->bdBitsPerSample);
	    rval= -1; goto ready;
	}

    /*  2  */
    if  ( bmCopyDescription( &(ri.riDescription), bdIn ) )
	{ LDEB(1); rval= -1; goto ready;	}

    ri.riDescription.bdColorEncoding= BMcoRGB8PALETTE;
    ri.riDescription.bdBitsPerSample= 8;
    ri.riDescription.bdBitsPerPixel= 8;

    if  ( utilPaletteSetCount( &(ri.riDescription.bdPalette), 216 ) )
	{ LDEB(216); rval= -1; goto ready;	}

    {
    int		r, g, b;

    for ( r= 0; r < 256; r += 51 )
    for ( g= 0; g < 256; g += 51 )
    for ( b= 0; b < 256; b += 51 )
    	{
	int		n= N( r, g, b );
	RGB8Color *	rgb8= &(ri.riDescription.bdPalette.cpColors[n]);

	rgb8->rgb8Red= r;
	rgb8->rgb8Green= g;
	rgb8->rgb8Blue= b;
	rgb8->rgb8Alpha= 255;
	}
    }

    if  ( bmCalculateSizes( &(ri.riDescription) ) )
	{ LDEB(1); rval= -1; goto ready;	}

    /*  3  */
    if  ( bmWebSafeSetAllocator( &ca, ri.riDescription.bdBitsPerPixel,
						    bmToWebSafeAllocateColor ) )
	{ LDEB(ri.riDescription.bdBitsPerPixel); rval= -1; goto ready; }

    /*  4  */
    if  ( bmAllocateBuffer( &ri ) )
	{
	LLDEB(ri.riDescription.bdBufferLength,ri.riBytes);
	rval= -1; goto ready;
	}

    /*  5  */
    if  ( bmFillImage( &ca, bitmapUnit, swapBitmapBytes, swapBitmapBits,
			dither, ri.riBytes, &(ri.riDescription),
			riIn, (const DocumentRectangle *)0 ) )
	{ LDEB(1); rval= -1; goto ready;	}

    /* steal */
    *riOut= ri; bmInitRasterImage( &ri );

  ready:

    /*  7  */
    bmCleanRasterImage( &ri );
    bmCleanColorAllocator( &ca );

    return rval;
    }
