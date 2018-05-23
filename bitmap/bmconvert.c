#   include	"bitmapConfig.h"

#   include	"bmintern.h"
#   include	"bmgetrow.h"
#   include	"bmputrow.h"
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Scaling, Dithering, Antialiassing.					*/
/*									*/
/************************************************************************/

static int bmBuildImage(	FillJob *			fj,
				unsigned char *			bufferOut,
				const BitmapDescription *	bdOut,
				const RasterImage *		riIn,
				const DocumentRectangle *	drSel,
				GetSourceRow			getSource,
				PutScreenRow			putRow )
    {
    int				rval= 0;

    int				rowOut;
    int				rowIn;

    const unsigned char *	from;
    unsigned char *		to;

    int				toWide= bdOut->bdPixelsWide;
    int				toHigh= bdOut->bdPixelsHigh;
    int				frWide;
    int				frHigh;

    int				e;
    int				d2;
    int				e2;

    DocumentRectangle		drAll;
    RasterImage			riSel;

    bmInitRasterImage( &riSel );

    drAll.drX0= 0;
    drAll.drY0= 0;
    drAll.drX1= riIn->riDescription.bdPixelsWide- 1;
    drAll.drY1= riIn->riDescription.bdPixelsHigh- 1;

    if  ( ! drSel )
	{ drSel= &drAll;	}
    else{
	if  ( drSel->drX0 != drAll.drX0	||
	      drSel->drY0 != drAll.drY0	||
	      drSel->drX1 != drAll.drX1	||
	      drSel->drY1 != drAll.drY1	)
	    {
	    /*  Could be done MUCH more efficiently as part of the	*/
	    /*  regular approach.					*/

	    if  ( bmSelect( &riSel, riIn, drSel ) )
		{ RECTDEB(drSel); rval= -1; goto ready;	}

	    riIn= &riSel;
	    }
	}

    frWide= riIn->riDescription.bdPixelsWide;
    frHigh= riIn->riDescription.bdPixelsHigh;

    if  ( toHigh <= frHigh )
	{
	e2= 2* frHigh;
	d2= 2* toHigh;
	e= e2- toHigh;

	bmInitColorRow( fj->fjNextCor+ 1, toWide );

	rowIn= 0;
	for ( rowOut= 0; rowOut < toHigh; rowOut++ )
	    {
	    bmInitColorRow( fj->fjThisRow+ 1, frWide );
	    to= bufferOut+ rowOut* bdOut->bdBytesPerRow;

	    while( e >= 0 )
		{
		from= riIn->riBytes+ rowIn* riIn->riDescription.bdBytesPerRow;

		(*getSource)( fj->fjThisRow+ 1, 0, from, 0,
					    riIn->riDescription.bdPixelsWide,
					    &(riIn->riDescription) );

		rowIn++; e -= d2;
		}

	    e += e2;

	    if  ( (*putRow)( to, fj, fj->fjThisRow+ 1 ) )
		{ LDEB(rowOut); rval= -1; goto ready;	}
	    }
	}
    else{
	e2= 2* toHigh;
	d2= 2* frHigh;
	e= e2- frHigh;

	rowOut= 0;
	for ( rowIn= 0; rowIn < frHigh; rowIn++ )
	    {
	    from= riIn->riBytes+ rowIn* riIn->riDescription.bdBytesPerRow;

	    bmInitColorRow( fj->fjThisRow+ 1, frWide );
	    (*getSource)( fj->fjThisRow+ 1, 0, from, 0,
					    riIn->riDescription.bdPixelsWide,
					    &(riIn->riDescription) );

	    while( e >= 0 )
		{
		to= bufferOut+ rowOut* bdOut->bdBytesPerRow;

		if  ( (*putRow)( to, fj, fj->fjThisRow+ 1 ) )
		    { LDEB(rowOut); rval= -1; goto ready; }

		e -= d2; rowOut++;
		}

	    e += e2;
	    }
	}

  ready:

    bmCleanRasterImage( &riSel );

    return rval;
    }

int bmFillImage(	ColorAllocator *		ca,
			int				swapBitmapUnit,
			int				swapBitmapBytes,
			int				swapBitmapBits,
			int				dither,
			unsigned char *			bufferOut,
			const BitmapDescription *	bdOut,
			const RasterImage *		riIn,
			const DocumentRectangle *	drSel )
    {
    const BitmapDescription *	bdIn= &(riIn->riDescription);
    int				rval= 0;

    PutScreenRow		putRow= (PutScreenRow)0;
    GetSourceRow		getRow= (GetSourceRow)0;
    int				scratchSize= 0;
    int				pixelsWideIn= bdIn->bdPixelsWide;

    FillJob			fj;

    bmInitFillJob( &fj );

    if  ( drSel )
	{ pixelsWideIn= drSel->drX1- drSel->drX0+ 1;	}

    /*  1  */
    if  ( bmGetPutRow( &putRow, &scratchSize, ca,
					    swapBitmapUnit, swapBitmapBytes,
					    swapBitmapBits, bdOut ) )
	{ LDEB(1); rval= -1; goto ready; }

    if  ( bmGetGetRow( &getRow, bdIn ) )
	{ LDEB(1); return -1; }

    if  ( bmSetFillJob( &fj, ca,
			    pixelsWideIn, bdOut->bdPixelsWide,
			    scratchSize, dither ) )
	{ LDEB(scratchSize); rval= -1; goto ready;	}

    if  ( bmBuildImage( &fj, bufferOut, bdOut, riIn, drSel, getRow, putRow ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    bmCleanFillJob( &fj );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Make a color allocator for a certain kind of image.			*/
/*									*/
/************************************************************************/

int bmSetColorAllocatorForImage(	ColorAllocator *		ca,
					const BitmapDescription *	bd )
    {
    switch( bd->bdColorEncoding )
	{
	case BMcoRGB8PALETTE:
	    if  ( bmSetColorAllocatorForPaletteImage( ca, bd ) )
		{ LDEB(1); return -1;	}
	    return 0;

	case BMcoBLACKWHITE:
	    if  ( bmSetColorAllocatorForBWImage( ca, bd ) )
		{ LDEB(1); return -1;	}
	    return 0;

	case BMcoWHITEBLACK:
	    if  ( bmSetColorAllocatorForWBImage( ca, bd ) )
		{ LDEB(1); return -1;	}
	    return 0;

	case BMcoRGB:
	    switch( bd->bdBitsPerSample )
		{
		case 8:
		    bmSetCalculatedShifts( ca, 0xff0000, 0x00ff00, 0x0000ff );
		    ca->caAllocationType= CA_CALCULATED;
		    return 0;

		default:
		    LLDEB(bd->bdColorEncoding,bd->bdBitsPerSample);
		    return -1;
		}

	default:
	    LDEB(bd->bdColorEncoding); return -1;
	}
    }

