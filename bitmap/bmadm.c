#   include	"bitmapConfig.h"

#   include	"bmintern.h"
#   include	<string.h>
#   include	<stdlib.h>
#   include	<appDebugon.h>
#   include	<geoUnits.h>

/************************************************************************/
/*  Translation back and forth between color encoding and string.	*/
/************************************************************************/

const char *	bmcoStrings[]=
    {
    "black on white",
    "white on black",
    "RGB",
    "RGB palette",

    (const char *)0
    };

const char *	bmcoIntToString( int colorEncodingInt )
    {
    if  ( colorEncodingInt >= 0						&&
	  colorEncodingInt < sizeof( bmcoStrings )/sizeof(char *) -1	)
	{ return bmcoStrings[colorEncodingInt]; }

    LDEB(colorEncodingInt); return (char *)0;
    }

int		bmcoStringToInt( const char * colorEncodingString )
    {
    int	i;

    for ( i= 0; i < sizeof( bmcoStrings )/sizeof(char *) -1; i++ )
	{
	if  ( ! strcmp( colorEncodingString, bmcoStrings[i] ) )
	    { return i;	}
	}

    SDEB(colorEncodingString); return -1;
    }

/************************************************************************/
/*  Translation back and forth between resolution unit and string.	*/
/************************************************************************/

const char *	bmunStrings[]=
    {
    "m",
    "inch",
    "point",
    "pixel",

    (const char *)0
    };

const char * bmunIntToString( int unitInt )
    {
    if  ( unitInt >= 0						&&
	  unitInt < sizeof( bmcoStrings )/sizeof(char *) -1	)
	{ return bmunStrings[unitInt]; }

    LDEB(unitInt); return (char *)0;
    }

int bmunStringToInt( const char * unitString )
    {
    int	i;

    for ( i= 0; i < sizeof( bmunStrings )/sizeof(char *) -1; i++ )
	{
	if  ( ! strcmp( unitString, bmunStrings[i] ) )
	    { return i;	}
	}

    SDEB(unitString); return -1;
    }

/************************************************************************/
/*									*/
/*  Initialise a BitmapDescription.					*/
/*									*/
/************************************************************************/

void bmInitDescription	( BitmapDescription *	bd )
    {
    bd->bdBufferLength= 0;
    bd->bdBytesPerRow= 0;
    bd->bdPixelsWide= 0;
    bd->bdPixelsHigh= 0;
    bd->bdBitsPerSample= 0;
    bd->bdSamplesPerPixel= 0;
    bd->bdBitsPerPixel= 0;
    bd->bdXResolution= 0;
    bd->bdYResolution= 0;
    bd->bdUnit= BMunILLEGALVALUE;
    bd->bdColorEncoding= BMcoILLEGALVALUE;

    bd->bdHasAlpha= 0;

    utilInitColorPalette( &(bd->bdPalette) );
    }

/************************************************************************/
/*									*/
/*  Clean a BitmapDescription.						*/
/*									*/
/************************************************************************/

void bmCleanDescription	( BitmapDescription *	bd )
    {
    utilCleanColorPalette( &(bd->bdPalette) );
    }

/************************************************************************/
/*									*/
/*  Copy a BitmapDescription.						*/
/*									*/
/************************************************************************/

int bmCopyDescription	(	BitmapDescription *		to,
				const BitmapDescription *	from )
    {
    utilCleanColorPalette( &(to->bdPalette) );

    *to= *from;

    utilInitColorPalette( &(to->bdPalette) );

    if  ( utilCopyColorPalette( &(to->bdPalette), &(from->bdPalette) ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Byte masks								*/
/*									*/
/************************************************************************/

unsigned char	Bmc1Masks[8]=
    { 0x80, 0x40, 0x20, 0x10, 0x8, 0x4, 0x2, 0x1 };

unsigned char	Bmc7Masks[8]=
    { 0x7f, 0xbf, 0xdf, 0xef, 0xf7, 0xfb, 0xfd, 0xfe };

/************************************************************************/
/*									*/
/*  Geometry.								*/
/*									*/
/************************************************************************/

void bmRectangleSizeTwips(
			int *				pRectangleWideTwips,
			int *				pRectangleHighTwips,
			const BitmapDescription *	bd,
			int				pixelsWide,
			int				pixelsHigh )
    {
    int		rectangleWideTwips;
    int		rectangleHighTwips;

    switch( bd->bdUnit )
	{
	case BMunM:
	    rectangleWideTwips= (int) ( ( TWIPS_PER_M* pixelsWide )/
							bd->bdXResolution );
	    rectangleHighTwips= (int) ( ( TWIPS_PER_M* pixelsHigh )/
							bd->bdYResolution );
	    break;

	case BMunINCH:
	    rectangleWideTwips= (int)( ( 72* 20* pixelsWide )/
							bd->bdXResolution );
	    rectangleHighTwips= (int)( ( 72* 20* pixelsHigh )/
							bd->bdYResolution );
	    break;

	case BMunPOINT:
	    rectangleWideTwips= (int)( ( 20* pixelsWide )/
							bd->bdXResolution );
	    rectangleHighTwips= (int)( ( 20* pixelsHigh )/
							bd->bdYResolution );
	    break;

	case BMunPIXEL:
	    rectangleWideTwips= 20* pixelsWide;
	    rectangleHighTwips= 20* pixelsHigh;
	    break;

	default:
	    LDEB(bd->bdUnit);
	    rectangleWideTwips= 20* pixelsWide;
	    rectangleHighTwips= 20* pixelsHigh;
	    break;
	}

    *pRectangleWideTwips= rectangleWideTwips;
    *pRectangleHighTwips= rectangleHighTwips;

    return;
    }

void bmImageSizeTwips(	int *				pImageWideTwips,
			int *				pImageHighTwips,
			const BitmapDescription *	bd )
    {
    bmRectangleSizeTwips( pImageWideTwips, pImageHighTwips, bd,
				bd->bdPixelsWide, bd->bdPixelsHigh );
    }

/************************************************************************/
/*									*/
/*  Derive the sizes of a description from the other fields.		*/
/*									*/
/************************************************************************/

int bmCalculateSizes(	BitmapDescription *	bd )
    {
    switch( bd->bdColorEncoding )
	{
	case BMcoBLACKWHITE:
	case BMcoWHITEBLACK:
	    if  ( bd->bdHasAlpha )
		{ bd->bdSamplesPerPixel= 2;	}
	    else{ bd->bdSamplesPerPixel= 1;	}

	    bd->bdBitsPerPixel= bd->bdSamplesPerPixel* bd->bdBitsPerSample;
	    break;

	case BMcoRGB:
	    if  ( bd->bdHasAlpha )
		{ bd->bdSamplesPerPixel= 4;	}
	    else{ bd->bdSamplesPerPixel= 3;	}

	    bd->bdBitsPerPixel= bd->bdSamplesPerPixel* bd->bdBitsPerSample;
	    break;

	case BMcoRGB8PALETTE:
	    if  ( bd->bdHasAlpha )
		{ bd->bdSamplesPerPixel= 4;	}
	    else{ bd->bdSamplesPerPixel= 3;	}

	    break;

	default:
	    LDEB(bd->bdColorEncoding);
	    return -1;
	}

    bd->bdBytesPerRow= ( bd->bdBitsPerPixel* bd->bdPixelsWide+ 7 )/ 8;
    bd->bdBufferLength= bd->bdPixelsHigh* bd->bdBytesPerRow;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Allocate buffer for an image.					*/
/*									*/
/************************************************************************/

int bmAllocateBuffer(		RasterImage *		ri )
    {
    unsigned char *	fresh;

    fresh= (unsigned char *)realloc( ri->riBytes,
					    ri->riDescription.bdBufferLength );
    if  ( ! fresh )
	{ LXDEB(ri->riDescription.bdBufferLength,fresh); return -1; }

    ri->riBytes= fresh;
    return 0;
    }

