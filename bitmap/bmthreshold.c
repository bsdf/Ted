#   include	"bitmapConfig.h"

#   include	<stdlib.h>

#   include	"bmGrayHisto.h"

#   include	"bmRender.h"
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Thresholding implementations.					*/
/*									*/
/************************************************************************/

static int bmToThresholdAllocateColor(	AllocatorColor *	ac,
					ColorAllocator *	ca,
					unsigned int		r,
					unsigned int		g,
					unsigned int		b )
    {
    ThresholderHistogram *	th= (ThresholderHistogram *)ca->caSystemPrivate;
    long			i;

    if  ( r > th->thThreshold )
	{ i= 0;	}
    else{ i= 1;	}

    ac->acRed= 257* r;
    ac->acGreen= 257* g;
    ac->acBlue= 257* b;
    ac->acColorNumber= i;

    return 0;
    }

static void bmThresholdFillHistogram8(
				ThresholderHistogram *		th,
				const BitmapDescription *	bdIn,
				const unsigned char *		bufferIn )
    {
    int		row;
    int		col;

    for ( col= 0; col < 256; col++ )
	{ th->thHistogram[col]= 0;	}

    for ( row= 0; row < bdIn->bdPixelsHigh; row++ )
	{
	const unsigned char *	from= bufferIn+ row* bdIn->bdBytesPerRow;

	for ( col= 0; col < bdIn->bdPixelsWide; from++, col++ )
	    {
	    if  ( bdIn->bdColorEncoding == BMcoWHITEBLACK )
		{ th->thHistogram[     *from]++; }
	    else{ th->thHistogram[255- *from]++; }
	    }
	}

    th->thHistogramSize= 256;
    th->thPixelCount= bdIn->bdPixelsHigh* bdIn->bdPixelsWide;
    th->thThreshold= 0;

    return;
    }

/************************************************************************/
/*									*/
/*  Thresholding an image.						*/
/*									*/
/*  1)  Initialize data structures.					*/
/*  2)  Only operate on monochrome images.				*/
/*  3)  At this point histogram code only works for 8 bit/pixel images.	*/
/*  4)  Determine threshold value.					*/
/*  5)  Derive properties of output bitmap from input.			*/
/*  6)  Allocate buffer for output image.				*/
/*  7)  Actual thresholding implementaed as a bitmat format		*/
/*	modification.							*/
/*									*/
/************************************************************************/

int bmThreshold(	RasterImage *			riOut,
			const RasterImage *		riIn,
			int				ignoredInt )
    {
    const BitmapDescription *	bdIn= &(riIn->riDescription);
    int				rval= 0;

    RasterImage			ri;

    ColorAllocator		ca;
    ThresholderHistogram	th;

    int				bitmapUnit= 0;
    int				swapBitmapBytes= 0;
    int				swapBitmapBits= 0;
    const int			dither= 0;

    bmInitRasterImage( &ri );

    /*  1  */
    bmInitColorAllocator( &ca );
    ca.caSystemAllocator= bmToThresholdAllocateColor;
    ca.caSystemPrivate= &th;
    ca.caAllocationType= CA_ALLOCATOR;

    /*  2  */
    switch( bdIn->bdColorEncoding )
	{
	case BMcoBLACKWHITE:
	case BMcoWHITEBLACK:
	    break;

	case BMcoRGB8PALETTE:
	case BMcoRGB:
	default:
	    LDEB(bdIn->bdColorEncoding);
	    rval= -1; goto ready;
	}

    /*  3  */
    switch( bdIn->bdBitsPerSample )
	{
	case 8:
	    bmThresholdFillHistogram8( &th, bdIn, riIn->riBytes );
	    break;

	default:
	    LDEB(bdIn->bdBitsPerSample);
	    rval= -1; goto ready;
	}

    /*  4  */
    if  ( 0 )
	{
	bmThresholdMean( &th );
	bmThresholdQuantile( &th, 0.5 );
	bmThreshold2Peaks( &th );
	bmThresholdRidler( &th );
	bmThresholdVariance( &th );
	bmThresholdMinimumError( &th );
	bmThresholdKapur( &th );
	bmThresholdJohannsen( &th );
	}
    else{
	bmThresholdJohannsen( &th );
	}

    /*  5  */
    if  ( bmCopyDescription( &(ri.riDescription), bdIn ) )
	{ LDEB(1); rval= -1; goto ready;	}

    ri.riDescription.bdBitsPerSample= 1;
    ri.riDescription.bdSamplesPerPixel= 1;
    ri.riDescription.bdBitsPerPixel= 1;
    ri.riDescription.bdColorEncoding= BMcoBLACKWHITE;

    if  ( bmCalculateSizes( &(ri.riDescription) ) )
	{ LDEB(1); rval= -1; goto ready;	}

    /*  6  */
    if  ( bmAllocateBuffer( &ri ) )
	{ LLDEB(ri.riDescription.bdBufferLength,ri.riBytes); rval= -1; goto ready; }

    /*  7  */
    if  ( bmFillImage( &ca, bitmapUnit, swapBitmapBytes, swapBitmapBits,
			dither, ri.riBytes, &(ri.riDescription),
			riIn, (const DocumentRectangle *)0 ) )
	{ LDEB(1); rval= -1; goto ready;	}

    /* steal */
    *riOut= ri; bmInitRasterImage( &ri );

  ready:

    bmCleanRasterImage( &ri );
    bmCleanColorAllocator( &ca );

    return rval;
    }
