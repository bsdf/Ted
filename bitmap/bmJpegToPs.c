/************************************************************************/
/*									*/
/*  Directly embed jpeg images in PostScript.				*/
/*									*/
/*  Code borrowed from Thomas Merz jpeg2ps: http://www.pdflib.com	*/
/*  And from Matthias Wandel jhead.					*/
/*									*/
/************************************************************************/

#   include	"bitmapConfig.h"

#   include	<stdio.h>

#   include	<sioGeneral.h>
#   include	<sioBase85.h>
#   include	<sioEndian.h>

#   include	"bmio.h"

#   include	<appDebugon.h>

/************************************************************************/

/* The following enum is stolen from the IJG JPEG library
 * Comments added by Thomas Merz
 * This table contains far too many names since jpeg2ps
 * is rather simple-minded about markers
 */

typedef enum {		/* JPEG marker codes			*/
  M_SOF0  = 0xc0,	/* baseline DCT				*/
  M_SOF1  = 0xc1,	/* extended sequential DCT		*/
  M_SOF2  = 0xc2,	/* progressive DCT			*/
  M_SOF3  = 0xc3,	/* lossless (sequential)		*/
  
  M_SOF5  = 0xc5,	/* differential sequential DCT		*/
  M_SOF6  = 0xc6,	/* differential progressive DCT		*/
  M_SOF7  = 0xc7,	/* differential lossless		*/
  
  M_JPG   = 0xc8,	/* JPEG extensions			*/
  M_SOF9  = 0xc9,	/* extended sequential DCT		*/
  M_SOF10 = 0xca,	/* progressive DCT			*/
  M_SOF11 = 0xcb,	/* lossless (sequential)		*/
  
  M_SOF13 = 0xcd,	/* differential sequential DCT		*/
  M_SOF14 = 0xce,	/* differential progressive DCT		*/
  M_SOF15 = 0xcf,	/* differential lossless		*/
  
  M_DHT   = 0xc4,	/* define Huffman tables		*/
  
  M_DAC   = 0xcc,	/* define arithmetic conditioning table	*/
  
  M_RST0  = 0xd0,	/* restart				*/
  M_RST1  = 0xd1,	/* restart				*/
  M_RST2  = 0xd2,	/* restart				*/
  M_RST3  = 0xd3,	/* restart				*/
  M_RST4  = 0xd4,	/* restart				*/
  M_RST5  = 0xd5,	/* restart				*/
  M_RST6  = 0xd6,	/* restart				*/
  M_RST7  = 0xd7,	/* restart				*/
  
  M_SOI   = 0xd8,	/* start of image			*/
  M_EOI   = 0xd9,	/* end of image				*/
  M_SOS   = 0xda,	/* start of scan			*/
  M_DQT   = 0xdb,	/* define quantization tables		*/
  M_DNL   = 0xdc,	/* define number of lines		*/
  M_DRI   = 0xdd,	/* define restart interval		*/
  M_DHP   = 0xde,	/* define hierarchical progression	*/
  M_EXP   = 0xdf,	/* expand reference image(s)		*/
  
  M_APP0  = 0xe0,	/* application marker, used for JFIF	*/
  M_APP1  = 0xe1,	/* application marker			*/
  M_APP2  = 0xe2,	/* application marker			*/
  M_APP3  = 0xe3,	/* application marker			*/
  M_APP4  = 0xe4,	/* application marker			*/
  M_APP5  = 0xe5,	/* application marker			*/
  M_APP6  = 0xe6,	/* application marker			*/
  M_APP7  = 0xe7,	/* application marker			*/
  M_APP8  = 0xe8,	/* application marker			*/
  M_APP9  = 0xe9,	/* application marker			*/
  M_APP10 = 0xea,	/* application marker			*/
  M_APP11 = 0xeb,	/* application marker			*/
  M_APP12 = 0xec,	/* application marker			*/
  M_APP13 = 0xed,	/* application marker			*/
  M_APP14 = 0xee,	/* application marker, used by Adobe	*/
  M_APP15 = 0xef,	/* application marker			*/
  
  M_JPG0  = 0xf0,	/* reserved for JPEG extensions		*/
  M_JPG13 = 0xfd,	/* reserved for JPEG extensions		*/
  M_COM   = 0xfe,	/* comment				*/
  
  M_TEM   = 0x01,	/* temporary use			*/

  M_ERROR = 0x100	/* dummy marker, internal use only	*/
} JPEG_MARKER;


/************************************************************************/
/*									*/
/*  Find out whether a JPEG file can be directly embedded in PostScript	*/
/*  output.								*/
/*									*/
/************************************************************************/

int bmEpsTestJpegEmbeddable(
			unsigned int *			pPixelsWide,
			unsigned int *			pPixelsHigh,
			unsigned int *			pComponentCount,
			unsigned int *			pBitsPerCompunent,
			SimpleInputStream *		sis )
    {
    int			rval= 0;
    int			c;
    int			marker;

    int			gotSOF = 0;

    unsigned int	bitsPerComponent= 0;
    unsigned int	componentCount= 0;
    unsigned int	pixelsWide= 0;
    unsigned int	pixelsHigh= 0;

    /*  Check first marker.  */
    c= sioInGetByte( sis );
    if  ( c == EOF || c != 0xff )
	{ CDEB(c); return -1;	}
    marker= sioInGetByte( sis );
    if  ( marker == EOF || marker != M_SOI )
	{ XXDEB(marker,M_SOI); return -1;	}

    for (;;)
	{
	int	segmentSize= 0;
	int	segmentUsed= 0;

	switch( marker )
	    {
	    case M_ERROR:
		LDEB(marker); return -1;

	    case M_SOF2:
	    case M_SOF3:
	    case M_SOF5:
	    case M_SOF6:
	    case M_SOF7:
	    case M_SOF9:
	    case M_SOF10:
	    case M_SOF11:
	    case M_SOF13:
	    case M_SOF14:
	    case M_SOF15:
		segmentSize= sioEndianGetBeUint16( sis );
		segmentUsed= 2;

		bitsPerComponent= sioInGetByte( sis );
		pixelsHigh= sioEndianGetBeUint16( sis );
		pixelsWide= sioEndianGetBeUint16( sis );
		componentCount= sioInGetByte( sis );
		segmentUsed += 6;
		rval= 1;  /* Not generally supported */
		gotSOF= 1;
		break;

	    case M_SOF0:
	    case M_SOF1:
		segmentSize= sioEndianGetBeUint16( sis );
		segmentUsed= 2;

		bitsPerComponent= sioInGetByte( sis );
		pixelsHigh= sioEndianGetBeUint16( sis );
		pixelsWide= sioEndianGetBeUint16( sis );
		componentCount= sioInGetByte( sis );
		segmentUsed += 6;
		gotSOF= 1;
		break;

	    case M_APP0:
	    case M_APP14:
		segmentSize= sioEndianGetBeUint16( sis );
		segmentUsed= 2;
		break;

	    case M_SOI:
	    case M_TEM:
	    case M_RST0:
	    case M_RST1:
	    case M_RST2:
	    case M_RST3:
	    case M_RST4:
	    case M_RST5:
	    case M_RST6:
	    case M_RST7:

	    case M_EOI:
	    case M_SOS:
		break;

	    default:
		segmentSize= sioEndianGetBeUint16( sis );
		segmentUsed= 2;
		break;
	    }

	if  ( marker == M_EOI || marker == M_SOS )
	    { break;	}

	while( segmentUsed < segmentSize )
	    {
	    c= sioInGetByte( sis );
	    if  ( c == EOF )
		{ XDEB(c); return -1;	}

	    segmentUsed++;
	    }

	c= sioInGetByte( sis );
	if  ( c == EOF || c != 0xff )
	    { CDEB(c); return -1;	}
	marker= sioInGetByte( sis );
	if  ( marker == EOF )
	    { XDEB(marker); return -1;	}
	}

    if  ( ! gotSOF )
	{ LDEB(gotSOF); return -1;	}

    if  ( bitsPerComponent != 8 )
	{ rval= 1;	}
    if  ( componentCount != 1	&&
	  componentCount != 3	&&
	  componentCount != 4	)
	{ rval= 1;	}

    *pPixelsWide= pixelsWide;
    *pPixelsHigh= pixelsHigh;
    *pComponentCount= componentCount;
    *pBitsPerCompunent= bitsPerComponent;

    return rval;
    }

/************************************************************************/
/*									*/
/*  Print an image from a JPEG source.					*/
/*									*/
/************************************************************************/

int bmPsPrintJpegImage(	SimpleOutputStream *		sos,
			double				xscale,
			double				yscale,
			int				componentCount,
			int				ox,
			int				oy,
			int				pixelsWide,
			int				pixelsHigh,
			int				bitsPerComponent,
			SimpleInputStream *		sis )
    {
    int				rval= 0;
    SimpleOutputStream *	sos85= (SimpleOutputStream *)0;

    sioOutPrintf( sos, "gsave 10 dict begin %% {> bitmap image\n" );

    if  ( ox != 0 || oy != 0 || xscale != 1.0 || yscale != 1.0 )
	{
	if  ( ox != 0 || oy != 0 )
	    { sioOutPrintf( sos, "%d %d translate ", ox, oy );	}
	if  ( xscale != 1.0 || yscale != 1.0 )
	    { sioOutPrintf( sos, "%f %f scale ", xscale, yscale ); }

	sioOutPrintf( sos, "\n" );
	}

    switch( componentCount )
	{
	case 1: /*  Non-standard grayscale file */
	    sioOutPrintf( sos, "/DeviceGray setcolorspace\n" );
	    break;
	case 3:
	    sioOutPrintf( sos, "/DeviceRGB setcolorspace\n" );
	    break;
	case 4: /*  Non-standard inverted CMYK file from photoshop.	*/
	    sioOutPrintf( sos, "/DeviceCMYK setcolorspace\n" );
	    break;
	default:
	    LDEB(componentCount); return -1;
	}

    sioOutPrintf( sos, "{ <<\n" );
    sioOutPrintf( sos, "  /ImageType 1\n" );
    sioOutPrintf( sos, "  /Width %u\n", pixelsWide );
    sioOutPrintf( sos, "  /Height %u\n", pixelsHigh );
    sioOutPrintf( sos, "  /ImageMatrix [ %d 0 0 %d 0 %d]\n",
				    pixelsWide, -pixelsHigh, pixelsHigh );
    sioOutPrintf( sos, "  /BitsPerComponent %u\n", bitsPerComponent );

    switch( componentCount )
	{
	case 1: /*  Non-standard grayscale file */
	    sioOutPrintf( sos, "  /Decode [0 1]\n" );
	    break;
	case 3:
	    sioOutPrintf( sos, "  /Decode [0 1 0 1 0 1]\n" );
	    break;
	case 4: /*  Non-standard inverted CMYK file from photoshop.	*/
	    sioOutPrintf( sos, "  /Decode [1 0 1 0 1 0 1 0]\n" );
	    break;
	default:
	    LDEB(componentCount); return -1;
	}

    sioOutPrintf( sos, "  /DataSource currentfile /ASCII85Decode filter << >> /DCTDecode filter\n" );
    sioOutPrintf( sos, ">> image } exec\n" );

    sos85= sioOutBase85Open( sos );
    if  ( ! sos85 )
	{ XDEB(sos85); rval= -1; goto ready;	}

    {
    unsigned char	buf[1000];
    int			done;

    while( ( done= sioInReadBytes( sis, buf, sizeof(buf) ) ) > 0 )
	{ sioOutWriteBytes( sos85, buf, done );	}
    }

    sioOutClose( sos85 ); sos85= (SimpleOutputStream *)0;

    sioOutPrintf( sos, "end grestore %% }< bitmap image\n");

  ready:
    if  ( sos85 )
	{ sioOutClose( sos85 );	}

    return rval;
    }
