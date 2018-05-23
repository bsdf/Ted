#   include	"bitmapConfig.h"

#   include	<stdlib.h>
#   include	<string.h>

#   include	"bmintern.h"
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Make images with standard content.					*/
/*									*/
/************************************************************************/

/************************************************************************/
/*									*/
/*  Make a completely transparent image					*/
/*									*/
/************************************************************************/

int bmTransparentImage(		BitmapDescription *		bdOut,
				unsigned char **		pBufOut,
				int				colorEncoding,
				int				wide,
				int				high )
    {
    int				rval= 0;
    BitmapDescription		bd;
    unsigned char *		buffer= (unsigned char *)0;

    bmInitDescription( &bd );

    switch( colorEncoding )
	{
	case BMcoRGB8PALETTE:
	    bd.bdPixelsWide= wide;
	    bd.bdPixelsHigh= high;
	    bd.bdHasAlpha= 1;
	    bd.bdUnit= BMunINCH;
	    bd.bdXResolution= 72;
	    bd.bdYResolution= 72;

	    bd.bdBitsPerSample= 8;
	    bd.bdSamplesPerPixel= 3;

	    if  ( bd.bdHasAlpha )
		{ bd.bdBitsPerPixel= 8;	}
	    else{ bd.bdBitsPerPixel= 4;	}

	    bd.bdColorEncoding= BMcoRGB8PALETTE;
	    if  ( utilPaletteSetCount( &(bd.bdPalette), 3 ) )
		{ LDEB(3); rval= -1; goto ready;	}

	    if  ( bmCalculateSizes( &bd ) )
		{ LDEB(1); rval= -1; goto ready;	}

	    buffer= (unsigned char *)malloc( bd.bdBufferLength );
	    if  ( ! buffer )
		{ LXDEB(bd.bdBufferLength,buffer); rval= -1; goto ready; }

	    /*  transparent */
	    bd.bdPalette.cpColors[0].rgb8Red= 255;
	    bd.bdPalette.cpColors[0].rgb8Green= 255;
	    bd.bdPalette.cpColors[0].rgb8Blue= 255;
	    bd.bdPalette.cpColors[0].rgb8Alpha= 0;

	    /*  white */
	    bd.bdPalette.cpColors[1].rgb8Red= 255;
	    bd.bdPalette.cpColors[1].rgb8Green= 255;
	    bd.bdPalette.cpColors[1].rgb8Blue= 255;
	    bd.bdPalette.cpColors[1].rgb8Alpha= 255;

	    /*  black */
	    bd.bdPalette.cpColors[2].rgb8Red= 0;
	    bd.bdPalette.cpColors[2].rgb8Green= 0;
	    bd.bdPalette.cpColors[2].rgb8Blue= 0;
	    bd.bdPalette.cpColors[2].rgb8Alpha= 255;

	    memset( buffer, 0, bd.bdBufferLength );

	    break;

	default:
	    LDEB(colorEncoding); rval= -1; goto ready;
	}

    /* steal */
    *bdOut= bd;
    *pBufOut= buffer;

    buffer= (unsigned char *)0;
    bmInitDescription( &bd );

  ready:

    bmCleanDescription( &bd );
    if  ( buffer )
	{ free( buffer );	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Make an image containing just one RGB color.			*/
/*									*/
/************************************************************************/

int bmRGBImage(			BitmapDescription *		bdOut,
				unsigned char **		pBufOut,
				int				colorEncoding,
				int				r,
				int				g,
				int				b,
				int				wide,
				int				high )
    {
    int				rval= 0;
    BitmapDescription		bd;
    unsigned char *		buffer= (unsigned char *)0;

    bmInitDescription( &bd );

    switch( colorEncoding )
	{
	case BMcoRGB8PALETTE:

	    if  ( r < 0 || r > 255 )
		{ LDEB(r); rval= -1; goto ready;	}
	    if  ( g < 0 || g > 255 )
		{ LDEB(g); rval= -1; goto ready;	}
	    if  ( b < 0 || b > 255 )
		{ LDEB(b); rval= -1; goto ready;	}

	    bd.bdPixelsWide= wide;
	    bd.bdPixelsHigh= high;
	    bd.bdHasAlpha= 0;
	    bd.bdUnit= BMunINCH;
	    bd.bdXResolution= 72;
	    bd.bdYResolution= 72;

	    bd.bdBitsPerSample= 8;
	    bd.bdSamplesPerPixel= 3;

	    if  ( bd.bdHasAlpha )
		{ bd.bdBitsPerPixel= 8;	}
	    else{ bd.bdBitsPerPixel= 4;	}

	    bd.bdColorEncoding= BMcoRGB8PALETTE;
	    if  ( utilPaletteSetCount( &(bd.bdPalette), 3 ) )
		{ LDEB(3); rval= -1; goto ready;	}

	    if  ( bmCalculateSizes( &bd ) )
		{ LDEB(1); rval= -1; goto ready;	}

	    buffer= (unsigned char *)malloc( bd.bdBufferLength );
	    if  ( ! buffer )
		{ LXDEB(bd.bdBufferLength,buffer); rval= -1; goto ready; }

	    /*  color */
	    bd.bdPalette.cpColors[0].rgb8Red= r;
	    bd.bdPalette.cpColors[0].rgb8Green= g;
	    bd.bdPalette.cpColors[0].rgb8Blue= b;
	    bd.bdPalette.cpColors[0].rgb8Alpha= 255;

	    /*  white */
	    bd.bdPalette.cpColors[1].rgb8Red= 255;
	    bd.bdPalette.cpColors[1].rgb8Green= 255;
	    bd.bdPalette.cpColors[1].rgb8Blue= 255;
	    bd.bdPalette.cpColors[1].rgb8Alpha= 255;

	    /*  black */
	    bd.bdPalette.cpColors[2].rgb8Red= 0;
	    bd.bdPalette.cpColors[2].rgb8Green= 0;
	    bd.bdPalette.cpColors[2].rgb8Blue= 0;
	    bd.bdPalette.cpColors[2].rgb8Alpha= 255;

	    memset( buffer, 0, bd.bdBufferLength );

	    break;

	default:
	    LDEB(colorEncoding); rval= -1; goto ready;
	}

    /* steal */
    *bdOut= bd;
    *pBufOut= buffer;

    buffer= (unsigned char *)0;
    bmInitDescription( &bd );

  ready:

    bmCleanDescription( &bd );
    if  ( buffer )
	{ free( buffer );	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Make a solid white/black image.					*/
/*									*/
/************************************************************************/

int bmSetSolidWhite(		RasterImage *	ri )
    {
    int			col;
    BitmapDescription *	bd= &(ri->riDescription);

    switch( bd->bdColorEncoding )
	{
	case BMcoBLACKWHITE:
	    if  ( bd->bdHasAlpha )
		{
		switch( bd->bdBitsPerPixel )
		    {
		    case 2:
			memset( ri->riBytes, 0x55, bd->bdBufferLength );
			break;
		    case 4:
			memset( ri->riBytes, 0x33, bd->bdBufferLength );
			break;
		    case 8:
			memset( ri->riBytes, 0x0f, bd->bdBufferLength );
			break;
		    default:
			LLDEB(bd->bdBitsPerPixel,bd->bdHasAlpha);
			return -1;
		    }
		}
	    else{
		memset( ri->riBytes, 0x00, bd->bdBufferLength );
		}
	    break;

	case BMcoWHITEBLACK:
	case BMcoRGB:
	    memset( ri->riBytes, 0xff, bd->bdBufferLength );
	    break;

	case BMcoRGB8PALETTE:
	    if  ( bd->bdHasAlpha )
		{ LDEB(bd->bdHasAlpha); return -1;	}

	    col= bmPaletteColor( bd, 255, 255, 255, 255 );
	    if  ( col < 0 )
		{ LLDEB(bd->bdPalette.cpColorCount,col); return -1;	}

	    if  ( ! bd->bdHasAlpha && bd->bdBitsPerPixel == 8 )
		{
		memset( ri->riBytes, col, bd->bdBufferLength );
		return 0;
		}
	    if  ( ! bd->bdHasAlpha && bd->bdBitsPerPixel == 4 )
		{
		memset( ri->riBytes, col << 4 | col, bd->bdBufferLength );
		return 0;
		}

	    LDEB(bd->bdColorEncoding); return -1;
	    break;

	default:
	    LDEB(bd->bdColorEncoding); return -1;
	}

    return 0;
    }

int bmSetSolidBlack(		RasterImage *	ri )
    {
    int			col;
    BitmapDescription *	bd= &(ri->riDescription);

    if  ( bd->bdHasAlpha )
	{ LDEB(bd->bdHasAlpha); return -1;	}

    switch( bd->bdColorEncoding )
	{
	case BMcoBLACKWHITE:
	    memset( ri->riBytes, 0xff, bd->bdBufferLength );
	    break;

	case BMcoWHITEBLACK:
	case BMcoRGB:
	    memset( ri->riBytes, 0x00, bd->bdBufferLength );
	    break;

	case BMcoRGB8PALETTE:
	    col= bmPaletteColor( bd, 0, 0, 0, 255 );
	    if  ( col < 0 )
		{ LLDEB(bd->bdPalette.cpColorCount,col); return -1;	}

	    if  ( ! bd->bdHasAlpha && bd->bdBitsPerPixel == 8 )
		{
		memset( ri->riBytes, col, bd->bdBufferLength );
		return 0;
		}
	    if  ( ! bd->bdHasAlpha && bd->bdBitsPerPixel == 4 )
		{
		memset( ri->riBytes, col << 4 | col, bd->bdBufferLength );
		return 0;
		}

	    LDEB(bd->bdColorEncoding); return -1;
	    break;

	default:
	    LDEB(bd->bdColorEncoding); return -1;
	}

    return 0;
    }
