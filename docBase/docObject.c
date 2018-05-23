/************************************************************************/
/*									*/
/*  Buffer administration: manage embedded objects.			*/
/*									*/
/************************************************************************/

#   include	"docBaseConfig.h"

#   include	<stdlib.h>
#   include	<string.h>

#   include	<geoGrid.h>
#   include	<appDebugon.h>

#   include	"docObject.h"
#   include	"docObjectProperties.h"

/************************************************************************/
/*									*/
/*  Manage inserted objects.						*/
/*									*/
/************************************************************************/

void docCleanInsertedObject( InsertedObject * io )
    {
    utilCleanMemoryBuffer( &io->ioObjectData );
    utilCleanMemoryBuffer( &io->ioResultData );

    if  ( io->ioObjectName )
	{ free( io->ioObjectName );	}
    if  ( io->ioObjectClass )
	{ free( io->ioObjectClass );	}
    
    bmCleanRasterImage( &(io->ioRasterImage) );
    }

void docInitInsertedObject(	InsertedObject *	io )
    {
    io->ioKind= DOCokUNKNOWN;
    io->ioResultKind= DOCokUNKNOWN;
    io->ioRtfResultKind= RESULTkindUNKNOWN;
    io->ioRtfEmbedKind= EMBEDkindOBJEMB;

    docInitPictureProperties( &(io->ioPictureProperties) );

    io->ioInline= 1;

    io->ioTwipsWide= 0;
    io->ioTwipsHigh= 0;

    io->ioPixelsWide= 0;
    io->ioPixelsHigh= 0;
    io->ioScaleXSet= 100;
    io->ioScaleYSet= 100;
    io->ioScaleXUsed= 100;
    io->ioScaleYUsed= 100;

    io->ioX0Twips= 0;
    docInitLayoutPosition( &(io->ioY0Position) );

    io->ioTopCropTwips= 0;
    io->ioBottomCropTwips= 0;
    io->ioLeftCropTwips= 0;
    io->ioRightCropTwips= 0;

    utilInitMemoryBuffer( &io->ioObjectData );
    utilInitMemoryBuffer( &io->ioResultData );
    
    io->ioObjectName= (char *)0;
    io->ioObjectClass= (char *)0;

    io->ioDrawingShape= (struct DrawingShape *)0;

    io->ioDrawingSurface= (struct DrawingSurface *)0;
    bmInitRasterImage( &(io->ioRasterImage) );

    return;
    }

void docObjectAdaptToPictureGeometry(	InsertedObject *		io,
					const PictureProperties *	pip )
    {
    io->ioTwipsWide= pip->pipTwipsWide;
    io->ioTwipsHigh= pip->pipTwipsHigh;

    io->ioScaleXSet= pip->pipScaleXSet;
    io->ioScaleYSet= pip->pipScaleYSet;
    io->ioScaleXUsed= pip->pipScaleXUsed;
    io->ioScaleYUsed= pip->pipScaleYUsed;

    io->ioTopCropTwips= pip->pipTopCropTwips;
    io->ioBottomCropTwips= pip->pipBottomCropTwips;
    io->ioLeftCropTwips= pip->pipLeftCropTwips;
    io->ioRightCropTwips= pip->pipRightCropTwips;

    return;
    }

int docObjectSetData(	InsertedObject *	io,
			const char *		bytes,
			int			size )
    {
    return utilMemoryBufferSetBytes( &(io->ioObjectData),
				    (const unsigned char *)bytes, size );
    }

int docSetResultData(	InsertedObject *	io,
			const char *		bytes,
			int			size )
    {
    return utilMemoryBufferSetBytes( &(io->ioResultData),
				    (const unsigned char *)bytes, size );
    }

int docSetObjectName(	InsertedObject *	io,
			const char *		name,
			int			len )
    {
    char *	fresh= (char *)malloc( len+ 1 );

    if  ( ! fresh )
	{ LXDEB(len,fresh); return -1;	}

    io->ioObjectName= fresh;
    strncpy( fresh, name, len ); fresh[len]= '\0';

    return 0;
    }

int docSetObjectClass(	InsertedObject *	io,
			const char *		name,
			int			len )
    {
    char *	fresh= (char *)malloc( len+ 1 );

    if  ( ! fresh )
	{ LXDEB(len,fresh); return -1;	}

    io->ioObjectClass= fresh;
    strncpy( fresh, name, len ); fresh[len]= '\0';

    return 0;
    }

/************************************************************************/
/*									*/
/*  Find the pixel rectangle inside the cropping margins of an image.	*/
/*									*/
/************************************************************************/

void docObjectGetCropRect(	DocumentRectangle *		drSrc,
				const PictureProperties *	pip,
				const BitmapDescription *	bd )
    {
    int		c1Twips;

    drSrc->drX0= 0;
    drSrc->drY0= 0;
    drSrc->drX1= bd->bdPixelsWide- 1;
    drSrc->drY1= bd->bdPixelsHigh- 1;

    if  ( pip->pipLeftCropTwips+ pip->pipRightCropTwips > 0 )
	{
	drSrc->drX0= ( bd->bdPixelsWide* pip->pipLeftCropTwips ) /
							    pip->pipTwipsWide;
	c1Twips= pip->pipTwipsWide- pip->pipRightCropTwips;
	drSrc->drX1= ( bd->bdPixelsWide* c1Twips )/ pip->pipTwipsWide;

	if  ( drSrc->drX0 < 0 )
	    { drSrc->drX0=  0;	}
	if  ( drSrc->drX1 > bd->bdPixelsWide- 1 )
	    { drSrc->drX1=  bd->bdPixelsWide- 1;	}
	}

    if  ( pip->pipTopCropTwips+ pip->pipBottomCropTwips > 0 )
	{
	drSrc->drY0= ( bd->bdPixelsHigh* pip->pipTopCropTwips ) /
							    pip->pipTwipsHigh;
	c1Twips= pip->pipTwipsHigh- pip->pipBottomCropTwips;
	drSrc->drY1= ( bd->bdPixelsHigh* c1Twips )/ pip->pipTwipsHigh;

	if  ( drSrc->drY0 < 0 )
	    { drSrc->drY0=  0;	}
	if  ( drSrc->drY1 > bd->bdPixelsHigh- 1 )
	    { drSrc->drY1=  bd->bdPixelsHigh- 1;	}
	}

    return;
    }

void docObjectSetPixelSize(	InsertedObject *		io,
				double				pixelsPerTwip )
    {
    io->ioPixelsWide= COORDtoGRID( pixelsPerTwip,
				( io->ioScaleXUsed* io->ioTwipsWide )/ 100 );
    io->ioPixelsHigh= COORDtoGRID( pixelsPerTwip,
				( io->ioScaleYUsed* io->ioTwipsHigh )/ 100 );

    if  ( io->ioPixelsWide < 1 )
	{ io->ioPixelsWide=  1;	}
    if  ( io->ioPixelsHigh < 1 )
	{ io->ioPixelsHigh=  1;	}

    return;
    }

