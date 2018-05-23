#   include	"bitmapConfig.h"

#   include	"bmintern.h"
#   include	<stdlib.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Binary morphology operations.					*/
/*									*/
/************************************************************************/

typedef struct DilateJob
    {
    BitmapDescription		djBdOut;
    unsigned char *		djBufOut;
				    /************************************/
				    /*  Output destination.		*/
				    /************************************/

    const RasterImage *		djRiSel;
    unsigned char		djInvertMaskSel;
				    /************************************/
				    /* Structuring element.		*/
				    /************************************/
    int				djRowSelOrig;
    int				djColSelOrig;
				    /************************************/
				    /* Origin of structuring element.	*/
				    /************************************/

    const RasterImage *		djRiIn;
    unsigned char		djInvertMaskIn;
				    /************************************/
				    /* Input image.			*/
				    /************************************/
    } DilateJob;

/************************************************************************/
/*									*/
/*  Draw the input buffer for a foreground pixel in the structuring	*/
/*  element.								*/
/*									*/
/*  As the input image is supposed to be much bigger than the		*/
/*  structuring element, it is more efficient to repeatedly draw the	*/
/*  image than repeatedly draw the element: The tight loops that copy	*/
/*  the rows of the image are more efficient than the coordinating code	*/
/*  around them.							*/
/*									*/
/*  NOTE: The efficiency assumption could be wrong for a big black	*/
/*  structuring element.						*/
/*									*/
/************************************************************************/

static int bmMorphoDilateElementPixel(	void *			voiddj,
					int			rowSel,
					int			colSel )
    
    {
    DilateJob *	dj= (DilateJob *)voiddj;

    bmDraw1BitImage( &(dj->djBdOut), dj->djBufOut,
					dj->djRiIn,
					dj->djInvertMaskIn,
					rowSel- dj->djRowSelOrig,
					colSel- dj->djColSelOrig );

    /* test code:
    bmCopyArea( colIn- dj->djColSelOrig, rowIn- dj->djRowSelOrig,
		    dj->djBufOut, dj->djBufIn, &(dj->djBdOut), dj->djBdIn );
    */

    return 0;
    }

/************************************************************************/
/*									*/
/*  Check the input of a morphology operation.				*/
/*									*/
/*  1)  Check the format of the input image.				*/
/*  2)  Check the format of the structuring element.			*/
/*									*/
/************************************************************************/

static int bmMorphoCheckInputFormats(	const BitmapDescription *	bdIn,
					const BitmapDescription *	bdSel )
    {
    /*  1  */
    switch( bdIn->bdColorEncoding )
	{
	case BMcoBLACKWHITE:
	case BMcoWHITEBLACK:
	    if  ( bdIn->bdBitsPerPixel != 1 )
		{
		LLDEB(bdIn->bdColorEncoding,bdIn->bdBitsPerPixel);
		return -1;
		}
	    break;

	default:
	    LLDEB(bdIn->bdColorEncoding,bdIn->bdBitsPerPixel);
	    return -1;
	}

    /*  2  */
    if  ( bdSel->bdColorEncoding != bdIn->bdColorEncoding )
	{
	LLDEB(bdSel->bdColorEncoding,bdIn->bdColorEncoding); 
	return -1;
	}

    if  ( bdSel->bdBitsPerPixel != 1 )
	{
	LLDEB(bdSel->bdColorEncoding,bdSel->bdBitsPerPixel);
	return -1;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Remember what to do.						*/
/*									*/
/************************************************************************/

static void bmMorphoStartDilateJob(
				DilateJob *			dj,
				const RasterImage *		riIn,
				const RasterImage *		riSel,
				int				rowSelOrig,
				int				colSelOrig )
    {
    bmInitDescription( &(dj->djBdOut) );

    dj->djBufOut= (unsigned char *)0;
    dj->djRiIn= riIn;
    dj->djRiSel= riSel;
    dj->djRowSelOrig= rowSelOrig;
    dj->djColSelOrig= colSelOrig;

    if  ( riIn->riDescription.bdColorEncoding == BMcoWHITEBLACK )
	{ dj->djInvertMaskIn= 0xff;	}
    else{ dj->djInvertMaskIn= 0x00;	}

    if  ( riSel->riDescription.bdColorEncoding == BMcoWHITEBLACK )
	{ dj->djInvertMaskSel= 0xff;	}
    else{ dj->djInvertMaskSel= 0x00;	}

    return;
    }

/************************************************************************/
/*									*/
/*  Dilate an image.							*/
/*									*/
/*  As a completely arbitrary decision, the '1' value in both the	*/
/*  input and the output have been chosen as the 'black' value of the	*/
/*  morphology books. In the most common WHITEBLACK case of bitmaps,	*/
/*  this means that the colors are swapped.				*/
/*									*/
/*  1)  Allocate output resources.					*/
/*  2)  Draw input image for all pixels in the structuring element.	*/
/*  3)  Return output.							*/
/*  4)  Cleanup.							*/
/*									*/
/************************************************************************/

static int bmMorphoDilateLow(	RasterImage *			riOut,
				DilateJob *			dj,
				int				reverse )
    {
    int			rval= 0;

    /*  1  */
    if  ( bmCopyDescription( &(dj->djBdOut), &(dj->djRiIn->riDescription) ) )
	{ LDEB(1); rval= -1; goto ready;	}

    /*
    dj->djBdOut.bdPixelsWide += dj->djBdSel->bdPixelsWide- 1;
    dj->djBdOut.bdPixelsHigh += dj->djBdSel->bdPixelsHigh- 1;
    */

    bmCalculateSizes( &(dj->djBdOut) );

    if  ( reverse )
	{
	dj->djBufOut= bmForegroundBuffer( dj->djBdOut.bdBufferLength,
						dj->djBdOut.bdColorEncoding );
	}
    else{
	dj->djBufOut= bmBackgroundBuffer( dj->djBdOut.bdBufferLength,
						dj->djBdOut.bdColorEncoding );
	}
    if  ( ! dj->djBufOut )
	{
	LXDEB(dj->djBdOut.bdBufferLength,dj->djBufOut);
	rval= -1; goto ready;
	}

#   if 0
    {
    const char *	name= "/tmp/background.png";
    int			format;

    format=  bmSuggestFormat( name, -1, &(dj->djBdOut) );
    bmWrite( name, dj->djBufOut, &(dj->djBdOut), format, 1.0 );
    }
#   endif

    /*  2  */
    bmForAll1Pixels( dj->djRiSel,
	    dj->djInvertMaskSel, (void *)dj, bmMorphoDilateElementPixel );

#   if 0
    {
    const char *	name= "/tmp/dilated.png";
    int			format;

    format=  bmSuggestFormat( name, -1, &(dj->djBdOut) );
    bmWrite( name, dj->djBufOut, &(dj->djBdOut), format, 1.0 );
    }
#   endif

    /*  3  */
    if  ( bmCopyDescription( &(riOut->riDescription), &(dj->djBdOut) ) )
	{ LDEB(1); rval= -1; goto ready;	}

    /*  steal  */
    riOut->riBytes= dj->djBufOut;
    dj->djBufOut= (unsigned char *)0;

  ready:

    /*  4  */
    if  ( dj->djBufOut )
	{ free( dj->djBufOut );	}

    bmCleanDescription( &(dj->djBdOut) );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Dilate an image.							*/
/*									*/
/************************************************************************/

int bmMorphoDilate(	RasterImage *			riOut,
			const RasterImage *		riIn,
			const RasterImage *		riSe,
			int				rowSelOrig,
			int				selColOrig )
    {
    DilateJob		dj;
    int			reverse= 0;

    if  ( bmMorphoCheckInputFormats( &(riIn->riDescription),
						&(riSe->riDescription) ) )
	{ LDEB(1); return -1;	}

    bmMorphoStartDilateJob( &dj, riIn, riSe, rowSelOrig, selColOrig );

    return bmMorphoDilateLow( riOut, &dj, reverse );
    }

/************************************************************************/
/*									*/
/*  Erode an image.							*/
/*									*/
/*  Use the fact that the erosion is the complement of the dilation of	*/
/*  the complement of the input with the mirrored structuring element.	*/
/*									*/
/*  1)  Initialization and verification of the input parameters.	*/
/*  2)  Mirror structuring element.					*/
/*  3)  Set dilate parameters. Realize that these are dilate parameters	*/
/*	and that we want to erode.					*/
/*  4)  To get the complement of the input image, swap the invert mask	*/
/*	for the input image.						*/
/*  5)  Invert the output image.					*/
/*									*/
/************************************************************************/

int bmMorphoErode(		RasterImage *			riOut,
				const RasterImage *		riIn,
				const RasterImage *		riSe,
				int				rowSelOrig,
				int				selColOrig )
    {
    int			rval= 0;
    DilateJob		dj;

    RasterImage		riRot;

    int			reverse= 1;

    /*  1  */
    bmInitRasterImage( &riRot );

    if  ( bmMorphoCheckInputFormats( &(riIn->riDescription),
						&(riSe->riDescription) ) )
	{ LDEB(1); return -1;	}

    /*  2  */
    if  ( bmRotate180( &riRot, riSe, 180 ) )
	{ LDEB(180); rval= -1; goto ready;	}

    /*  3  */
    bmMorphoStartDilateJob( &dj, riIn, &riRot, rowSelOrig, selColOrig );

    /*  4,5  */
    dj.djInvertMaskIn= ~dj.djInvertMaskIn;
    reverse= 1;

    if  ( bmMorphoDilateLow( riOut, &dj, reverse ) )
	{ XDEB(dj.djInvertMaskIn); rval= -1; goto ready;	}

  ready:

    bmCleanRasterImage( &riRot );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Return the 'Simple' structuring element.				*/
/*									*/
/************************************************************************/

int bmMorphoSetSimpleSe(	RasterImage *		riOut )
    {
    const int		size= 3;
    const int		wide= size;
    const int		high= size;

    riOut->riDescription.bdColorEncoding= BMcoWHITEBLACK;
    riOut->riDescription.bdPixelsWide= wide;
    riOut->riDescription.bdPixelsHigh= high;
    riOut->riDescription.bdBitsPerSample= 1;
    riOut->riDescription.bdSamplesPerPixel= 1;
    riOut->riDescription.bdBitsPerPixel= 1;
    riOut->riDescription.bdXResolution= 1;
    riOut->riDescription.bdYResolution= 1;
    riOut->riDescription.bdUnit= BMunPIXEL;

    if  ( bmCalculateSizes( &(riOut->riDescription) ) )
	{ LDEB(1); return -1;	}

    riOut->riBytes= bmBackgroundBuffer(
			    riOut->riDescription.bdBufferLength,
			    riOut->riDescription.bdColorEncoding );
    if  ( ! riOut->riBytes )
	{ LXDEB(riOut->riDescription.bdBufferLength,riOut->riBytes); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Dilate/Erode an image using the 'Simple' structuring element.	*/
/*									*/
/*  For Erosion, use the approach of bmMorphoErode(), but with some	*/
/*  shortcuts to avoid double work.					*/
/*									*/
/************************************************************************/

int bmMorphoDilateSimple(	RasterImage *			riOut,
				const RasterImage *		riIn,
				int				ignoredInt )
    {
    int			rval= 0;

    RasterImage		riSel;

    const int		rowSelOrig= 1;
    const int		selColOrig= 1;

    bmInitRasterImage( &riSel );

    if  ( bmMorphoSetSimpleSe( &riSel ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( bmMorphoDilate( riOut, riIn, &riSel, rowSelOrig, selColOrig ) )
	{ LLDEB(rowSelOrig,selColOrig); rval= -1; goto ready;	}

  ready:

    bmCleanRasterImage( &riSel );

    return rval;
    }

int bmMorphoErodeSimple(	RasterImage *			riOut,
				const RasterImage *		riIn,
				int				ignoredInt )
    {
    int			rval= 0;
    const unsigned char	invertMaskIn= 0xff;
    DilateJob		dj;

    RasterImage		riSel;

    const int		rowSelOrig= 1;
    const int		selColOrig= 1;
    int			reverse= 1;

    bmInitRasterImage( &riSel );

    if  ( bmMorphoSetSimpleSe( &riSel ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( bmMorphoCheckInputFormats( &(riIn->riDescription),
						&(riSel.riDescription) ) )
	{ LDEB(1); return -1;	}

    bmMorphoStartDilateJob( &dj, riIn, &riSel, rowSelOrig, selColOrig );

    /*  4  */
    dj.djInvertMaskIn= ~dj.djInvertMaskIn;
    reverse= 1;

    if  ( bmMorphoDilateLow( riOut, &dj, reverse ) )
	{ XDEB(invertMaskIn); rval= -1; goto ready;	}

  ready:

    bmCleanRasterImage( &riSel );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Make an element to recognize lines.					*/
/*									*/
/************************************************************************/

int bmMorphoLineElement(	RasterImage *			riOut,
				int				wide,
				int				high,
				int				x0,
				int				y0,
				int				x1,
				int				y1 )
    {
    if  ( x0 < 0 || x0 >= wide )
	{ LLDEB(x0,wide); return -1;	}
    if  ( x1 < 0 || x1 >= wide )
	{ LLDEB(x1,wide); return -1;	}

    if  ( y0 < 0 || y0 >= high )
	{ LLDEB(y0,high); return -1;	}
    if  ( y1 < 0 || y1 >= high )
	{ LLDEB(y1,high); return -1;	}

    riOut->riDescription.bdColorEncoding= BMcoWHITEBLACK;
    riOut->riDescription.bdPixelsWide= wide;
    riOut->riDescription.bdPixelsHigh= high;
    riOut->riDescription.bdBitsPerSample= 1;
    riOut->riDescription.bdSamplesPerPixel= 1;
    riOut->riDescription.bdBitsPerPixel= 1;
    riOut->riDescription.bdXResolution= 1;
    riOut->riDescription.bdYResolution= 1;
    riOut->riDescription.bdUnit= BMunPIXEL;

    if  ( bmCalculateSizes( &(riOut->riDescription) ) )
	{ LDEB(1); return -1;	}

    riOut->riBytes= bmBackgroundBuffer(
			    riOut->riDescription.bdBufferLength,
			    riOut->riDescription.bdColorEncoding );
    if  ( ! riOut->riBytes )
	{ LXDEB(riOut->riDescription.bdBufferLength,riOut->riBytes); return -1;	}

    bmDrawLine( riOut->riBytes, &(riOut->riDescription), x0, y0, x1, y1, 1 );

    return 0;
    }

