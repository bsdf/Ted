#   include	"bitmapConfig.h"

#   include	"bmRender.h"
#   include	<stdlib.h>
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Map a color image to grayscale.					*/
/*									*/
/************************************************************************/

#   define	F_R	77
#   define	F_G	150
#   define	F_B	29

/************************************************************************/
/*									*/
/*  Free memory in a grayscale allocator.				*/
/*									*/
/************************************************************************/

static void bmGrayCleanupAllocator(	ColorAllocator *	ca )
    {
    if  ( ca->caSystemPrivate )
	{ free( ca->caSystemPrivate );	}

    return;
    }

/************************************************************************/
/*									*/
/*  Set resources in a grayscale allocator, depending on the number of	*/
/*  bits.								*/
/*									*/
/************************************************************************/

static int bmGraySetAllocator(		ColorAllocator *	ca,
					int			bitsPerPixel,
					SystemAllocator		sysAllocator )
    {
    unsigned int *	fresh;

    unsigned int	mask;
    unsigned int	divi;

    mask= ( 1 << bitsPerPixel )- 1;
    divi= ( 255* 256 )/mask;

    fresh= (unsigned int *)realloc( ca->caSystemPrivate, 2* sizeof(int) );
    if  ( ! fresh )
	{ XDEB(fresh); return -1;	}

    ca->caSystemPrivate= fresh;

    fresh[0]= divi;
    fresh[1]= mask;

    ca->caSystemAllocator= sysAllocator;
    ca->caSystemCleanup= bmGrayCleanupAllocator;
    ca->caAllocationType= CA_ALLOCATOR;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Allocate a grayscale color:						*/
/*									*/
/*  WB:	White on Black i.e. White is maximum.				*/
/*  BW:	Black on White i.e. Black is maximum.				*/
/*									*/
/************************************************************************/

static int bmToGrayAllocateWBColor(	AllocatorColor *	ac,
					ColorAllocator *	ca,
					unsigned int		r,
					unsigned int		g,
					unsigned int		b )
    {
    const unsigned int *	pars= (unsigned int *)ca->caSystemPrivate;
    unsigned int		divi= pars[0];
    long			val;

    val= ( F_R* r+ F_G* g+ F_B* b )/ divi;

    ac->acRed= 257* r;
    ac->acGreen= 257* g;
    ac->acBlue= 257* b;
    ac->acColorNumber= val;

    return 0;
    }

static int bmToGrayAllocateBWColor(	AllocatorColor *	ac,
					ColorAllocator *	ca,
					unsigned int		r,
					unsigned int		g,
					unsigned int		b )
    {
    const unsigned int *	pars= (unsigned int *)ca->caSystemPrivate;
    unsigned int		divi= pars[0];
    unsigned int		mask= pars[1];
    long			val;

    val= ( F_R* r+ F_G* g+ F_B* b )/ divi;

    ac->acRed= 257* r;
    ac->acGreen= 257* g;
    ac->acBlue= 257* b;
    ac->acColorNumber= mask- val;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Choose an allocator for colors in a grayscale image.		*/
/*									*/
/*  WB:	White on Black i.e. White is maximum.				*/
/*  BW:	Black on White i.e. Black is maximum.				*/
/*									*/
/************************************************************************/

int bmSetColorAllocatorForBWImage(	ColorAllocator *		ca,
					const BitmapDescription *	bd )
    {
    switch( bd->bdBitsPerPixel )
	{
	case 8:
	case 4:
	case 2:
	case 1:
	    if  ( bmGraySetAllocator( ca, bd->bdBitsPerPixel,
						    bmToGrayAllocateBWColor ) )
		{ LDEB(bd->bdBitsPerPixel); return -1;	}

	    return 0;

	default:
	    LDEB(bd->bdBitsPerPixel); return -1;
	}
    }

int bmSetColorAllocatorForWBImage(	ColorAllocator *		ca,
					const BitmapDescription *	bd )
    {
    switch( bd->bdBitsPerPixel )
	{
	case 8:
	case 4:
	case 2:
	case 1:
	    if  ( bmGraySetAllocator( ca, bd->bdBitsPerPixel,
						bmToGrayAllocateWBColor ) )
		{ LDEB(bd->bdBitsPerPixel); return -1;	}

	    return 0;

	default:
	    LDEB(bd->bdBitsPerPixel); return -1;
	}
    }

/************************************************************************/
/*									*/
/*  Convert an image to grayscale.					*/
/*									*/
/*  1)  Check input format.						*/
/*  2)	Derive properties of output bitmap from input.			*/
/*  3)	Choose grayscale color allocator.				*/
/*  4)	Allocate output buffer.						*/
/*  5)	Fill it.							*/
/*  6)	Return outputs.							*/
/*  7)	Cleanup.							*/
/*									*/
/************************************************************************/

int bmToGrayscale(	RasterImage *			riOut,
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
	    break;

	case BMcoBLACKWHITE:
	case BMcoWHITEBLACK:
	case BMcoRGB8PALETTE:
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

    ri.riDescription.bdColorEncoding= BMcoBLACKWHITE;
    ri.riDescription.bdBitsPerSample= 8;

    if  ( bmCalculateSizes( &(ri.riDescription) ) )
	{ LDEB(1); rval= -1; goto ready;	}

    /*  3  */
    if  ( bmGraySetAllocator( &ca, ri.riDescription.bdBitsPerPixel,
						    bmToGrayAllocateBWColor ) )
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
