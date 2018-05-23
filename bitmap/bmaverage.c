#   include	"bitmapConfig.h"

#   include	<stdlib.h>

#   include	"bitmap.h"

#   include	<appDebugon.h>

static void bmAverageMono8Row(	const BitmapDescription *	bdOut,
				const BitmapDescription *	bdIn,
				unsigned char *			to,
				const unsigned char *		rf,
				int				xpixels,
				int				ypixels )
    {
    int		col;

    int		r;
    int		c;

    for ( col= 0; col < bdIn->bdPixelsWide/ xpixels; rf += xpixels, col++ )
	{
	int				v= 0;
	const int			n= xpixels* ypixels;
	const unsigned char *		cf= rf;

	for ( r= 0; r < ypixels; cf += bdIn->bdBytesPerRow, r++ )
	    {
	    const unsigned char *	f= cf;

	    for ( c= 0; c < xpixels; c++ )
		{ v += *(f++);	}
	    }

	*(to++)= ( v+ n/2 )/ n;
	}

    xpixels= bdIn->bdPixelsWide % xpixels;

    if  ( xpixels > 0 )
	{
	int				v= 0;
	const int			n= ypixels* xpixels;
	const unsigned char *		cf= rf;

	for ( r= 0; r < ypixels; cf += bdIn->bdBytesPerRow, r++ )
	    {
	    const unsigned char *	f= cf;

	    for ( c= 0; c < xpixels; c++ )
		{ v += *(f++);	}
	    }

	*(to++)= ( v+ n/2 )/ n;
	}

    return;
    }

static void bmAverageMono8Image(
				const BitmapDescription *	bdOut,
				const BitmapDescription *	bdIn,
				unsigned char *			buffer,
				const unsigned char *		bufIn,
				int				xpixels,
				int				ypixels )
    {
    int			row;
    unsigned char *	to= buffer;
    int			ytail= bdIn->bdPixelsHigh % ypixels;

    for ( row= 0; row < bdIn->bdPixelsHigh/ ypixels; row++ )
	{
	const unsigned char *	rf= bufIn+ ypixels* row* bdIn->bdBytesPerRow;

	bmAverageMono8Row( bdOut, bdIn, to, rf, xpixels, ypixels );

	to += bdOut->bdBytesPerRow;
	}

    if  ( ytail > 0 )
	{
	const unsigned char *	rf= bufIn+ ypixels* row* bdIn->bdBytesPerRow;

	bmAverageMono8Row( bdOut, bdIn, to, rf, xpixels, ytail );
	}

    return;
    }

static void bmAverageRgb24Row(	const BitmapDescription *	bdOut,
				const BitmapDescription *	bdIn,
				unsigned char *			to,
				const unsigned char *		rf,
				int				xpixels,
				int				ypixels )
    {
    int		col;

    int		r;
    int		c;

    for ( col= 0; col < bdIn->bdPixelsWide/ xpixels; rf += 3* xpixels, col++ )
	{
	int				rv= 0;
	int				gv= 0;
	int				bv= 0;
	const int			n= xpixels* ypixels;
	const unsigned char *		cf= rf;

	for ( r= 0; r < ypixels; cf += bdIn->bdBytesPerRow, r++ )
	    {
	    const unsigned char *	f= cf;

	    for ( c= 0; c < xpixels; c++ )
		{ rv += *(f++); gv += *(f++); bv += *(f++);	}
	    }

	*(to++)= ( rv+ n/2 )/ n;
	*(to++)= ( gv+ n/2 )/ n;
	*(to++)= ( bv+ n/2 )/ n;
	}

    xpixels= bdIn->bdPixelsWide % xpixels;

    if  ( xpixels > 0 )
	{
	int				rv= 0;
	int				gv= 0;
	int				bv= 0;
	const int			n= ypixels* xpixels;
	const unsigned char *		cf= rf;

	for ( r= 0; r < ypixels; cf += bdIn->bdBytesPerRow, r++ )
	    {
	    const unsigned char *	f= cf;

	    for ( c= 0; c < xpixels; c++ )
		{ rv += *(f++); gv += *(f++); bv += *(f++);	}
	    }

	*(to++)= ( rv+ n/2 )/ n;
	*(to++)= ( gv+ n/2 )/ n;
	*(to++)= ( bv+ n/2 )/ n;
	}

    return;
    }

static void bmAverageRgb24Image(
				const BitmapDescription *	bdOut,
				const BitmapDescription *	bdIn,
				unsigned char *			buffer,
				const unsigned char *		bufIn,
				int				xpixels,
				int				ypixels )
    {
    int			row;
    unsigned char *	to= buffer;
    int			ytail= bdIn->bdPixelsHigh % ypixels;

    for ( row= 0; row < bdIn->bdPixelsHigh/ ypixels; row++ )
	{
	const unsigned char *	rf= bufIn+ ypixels* row* bdIn->bdBytesPerRow;

	bmAverageRgb24Row( bdOut, bdIn, to, rf, xpixels, ypixels );

	to += bdOut->bdBytesPerRow;
	}

    if  ( ytail > 0 )
	{
	const unsigned char *	rf= bufIn+ ypixels* row* bdIn->bdBytesPerRow;

	bmAverageRgb24Row( bdOut, bdIn, to, rf, xpixels, ytail );
	}

    return;
    }

int bmAverage(		RasterImage *			riOut,
			const RasterImage *		riIn,
			int				xpixels,
			int				ypixels )
    {
    const BitmapDescription *	bdIn= &(riIn->riDescription);
    int				rval= 0;

    RasterImage		ri;

    bmInitRasterImage( &ri );

    switch( bdIn->bdColorEncoding )
	{
	case BMcoBLACKWHITE:
	case BMcoWHITEBLACK:
	case BMcoRGB:
	    break;
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
	    LDEB(bdIn->bdBitsPerSample); rval= -1; goto ready;
	}

    /************************************************************/
    /*  Derive properties of output bitmap from input.		*/
    /************************************************************/
    if  ( bmCopyDescription( &(ri.riDescription), bdIn ) )
	{ LDEB(1); rval= -1; goto ready;	}

    ri.riDescription.bdPixelsWide= ( bdIn->bdPixelsWide+ xpixels- 1 )/xpixels;
    ri.riDescription.bdPixelsHigh= ( bdIn->bdPixelsHigh+ ypixels- 1 )/ypixels;

    if  ( bmCalculateSizes( &(ri.riDescription) ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( bmAllocateBuffer( &ri ) )
	{  LLDEB(ri.riDescription.bdBufferLength,ri.riBytes); rval= -1; goto ready; }

    /************************************************************/
    /*  Average pixel values.					*/
    /************************************************************/
    switch( ri.riDescription.bdColorEncoding )
	{
	case BMcoBLACKWHITE:
	case BMcoWHITEBLACK:
	    bmAverageMono8Image( &(ri.riDescription), bdIn,
				ri.riBytes, riIn->riBytes, xpixels, ypixels );
	    break;

	case BMcoRGB:
	    bmAverageRgb24Image( &(ri.riDescription), bdIn,
				ri.riBytes, riIn->riBytes, xpixels, ypixels );
	    break;

	case BMcoRGB8PALETTE:
	default:
	    LDEB(ri.riDescription.bdColorEncoding);
	    return -1;
	}

    /* steal */
    *riOut= ri; bmInitRasterImage( &ri );

  ready:

    bmCleanRasterImage( &ri );

    return rval;
    }

