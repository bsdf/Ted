#   include	"bitmapConfig.h"

#   include	"bmintern.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Call a function for all pixels that have value '1' in a bitmap	*/
/*  image. [Or '0'.. when invertMaskIn == 0xff.].			*/
/*  To do something for all black pixels in an image, pass 0x00 for	*/
/*  BMcoBLACKWHITE and 0xff for BMcoWHITEBLACK images.			*/
/*									*/
/*  Used inside morphology code and to extract features from the	*/
/*  result.								*/
/*									*/
/*  The behavior with ( invertMaskIn != 0xff && invertMaskIn != 0x00 )	*/
/*  is completely predictable, buf not what this routine was meant for.	*/
/*									*/
/*  1)  Determine the mask to ignore the trailing bits in the input	*/
/*	image.								*/
/*									*/
/************************************************************************/

int bmForAll1Pixels(		const RasterImage *		riIn,
				unsigned char			invertMaskIn,
				void *				through,
				BM_PIX_FUN			pixFun )
    {
    const BitmapDescription *	bdIn= &(riIn->riDescription);
    int				row;
    unsigned char		lastMaskIn;

    int			col;

    if  ( bdIn->bdBitsPerPixel != 1 )
	{ LLDEB(bdIn->bdBitsPerPixel,bdIn->bdBitsPerPixel!=1);	}

    /*  1  */
    lastMaskIn= 0xff;
    lastMaskIn >>= ( bdIn->bdPixelsWide % 8 );
    lastMaskIn= ~lastMaskIn;

    for ( row= 0; row < bdIn->bdPixelsHigh; row++ )
	{
	const unsigned char *	from= riIn->riBytes+ row* bdIn->bdBytesPerRow;

	for ( col= 0; col < bdIn->bdPixelsWide; from++, col += 8 )
	    {
	    unsigned char	val= ( from[0] ^ invertMaskIn );
	    unsigned char	mask= 0x80;
	    int			pix;

	    if  ( col+ 8 >= bdIn->bdPixelsWide )
		{ val &= lastMaskIn;	}
	    if  ( ! val )
		{ continue;	}

	    for ( pix= 0; pix < 8; mask >>= 1, pix++ )
		{
		int		colIn= col+ pix;

		if  ( ! ( val & mask ) )
		    { continue;	}

		if  ( (*pixFun)( through, row, colIn ) )
		    { LLDEB(row,colIn); return -1; }
		}
	    }
	}

    return 0;
    }

