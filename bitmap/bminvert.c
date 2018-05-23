#   include	"bitmapConfig.h"

#   include	"bmintern.h"

#   include	<stdlib.h>
#   include	<string.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Invert the individual components of an image.			*/
/*									*/
/************************************************************************/

#   define	MAX_COMPONENTS	4

int bmInvertImage(	RasterImage *			riOut,
			const RasterImage *		riIn,
			int				ignoredInt )
    {
    const BitmapDescription *	bdIn= &(riIn->riDescription);
    int				rval= 0;

    int				map[256*MAX_COMPONENTS];
    int *			mp;
    int				mxv;

    int				i;
    int				j;

    RasterImage			ri;

    bmInitRasterImage( &ri );

    /*  1  */
    if  ( bdIn->bdBitsPerSample > 8 )
	{ LDEB(bdIn->bdBitsPerSample); rval= -1; goto ready;	}

    /*  2  */
    mxv= 1;
    for ( i= 0; i < bdIn->bdBitsPerSample; i++ )
	{ mxv *= 2;	}
    mxv--;

    /*  3  */
    mp= map;
    for ( i= 0; i <= mxv; i++ )
	{
	for ( j= 0; j < bdIn->bdSamplesPerPixel; j++ )
	    { *(mp++)= mxv- i;	}
	}

    if  ( bmCopyDescription( &(ri.riDescription), bdIn ) )
	{ LDEB(1); rval= -1; goto ready;	}

    /*  4  */
    ri.riBytes= (unsigned char *)malloc( bdIn->bdBufferLength+ bdIn->bdSamplesPerPixel- 1 );
    if  ( ! ri.riBytes )
	{ LXDEB(bdIn->bdBufferLength,ri.riBytes); rval= -1; goto ready; }

    memcpy( ri.riBytes, riIn->riBytes, bdIn->bdBufferLength );

    /*  5  */
    if  ( bmMapImageColors( &(ri.riDescription), map, ri.riBytes ) )
	{ LDEB(1); rval= -1; goto ready;	}

    /*  6  */
    /* steal */
    *riOut= ri; bmInitRasterImage( &ri );

  ready:
    
    bmCleanRasterImage( &ri );

    return rval;
    }

