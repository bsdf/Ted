#   include	"bitmapConfig.h"

#   include	"bmintern.h"
#   include	"bmio.h"
#   include	<appDebugon.h>
#   include	<sioFileio.h>
#   include	<sioHex.h>
#   include	<geoUnits.h>

/************************************************************************/
/*									*/
/*  Write a bitmap to an rtf stream.					*/
/*									*/
/************************************************************************/

static int bmRtfStartRtf(	const BitmapDescription *	bd,
				const char *			type,
				SimpleOutputStream *		sos )
    {
    sioOutPrintf( sos, "{\\rtf1\\ansi\r\n" );
    sioOutPrintf( sos, "\\deff1\r\n" );
    sioOutPrintf( sos, "{\\fonttbl{\\f1\\fswiss Helvetica;}}\r\n" );
    sioOutPrintf( sos, "\\sectd\\f1\\fs20\r\n" );

    sioOutPrintf( sos, "{\\pict%s", type );

    return 0;
    }

static int bmRtfImageSize(	const BitmapDescription *	bd,
				SimpleOutputStream *		sos )
    {
    int			picwgoal;
    int			pichgoal;
    int			picw;
    int			pich;

    bmImageSizeTwips( &picwgoal, &pichgoal, bd );
    picw= (int)( 100000.0* picwgoal )/ ( 20* POINTS_PER_M );
    pich= (int)( 100000.0* pichgoal )/ ( 20* POINTS_PER_M );

    sioOutPrintf( sos, "\\picw%d\\pich%d\\picwgoal%d\\pichgoal%d\r\n",
					    picw, pich, picwgoal, pichgoal );

    return 0;
    }

int bmRtfWritePngRtf(		const BitmapDescription *	bd,
				const unsigned char *		buffer,
				SimpleOutputStream *		sos )
    {
    int				rval= 0;
    SimpleOutputStream *	sosHex= (SimpleOutputStream *)0;
    const int			lastNl= 1;

    bmRtfStartRtf( bd, "\\pngblip", sos );
    bmRtfImageSize( bd, sos );

    sosHex= sioOutHexOpenFolded( sos, 72, lastNl );
    if  ( ! sosHex )
	{ XDEB(sosHex); return -1;	}

    rval= bmPngWritePng( bd, buffer, sosHex );

    sioOutClose( sosHex );
    sioOutPrintf( sos, "}\\par}\r\n" );

    return rval;
    }

int bmRtfWriteJfifRtf(		const BitmapDescription *	bd,
				const unsigned char *		buffer,
				SimpleOutputStream *		sos )
    {
    int				rval= 0;
    SimpleOutputStream *	sosHex= (SimpleOutputStream *)0;
    const int			lastNl= 1;

    bmRtfStartRtf( bd, "\\jpegblip", sos );
    bmRtfImageSize( bd, sos );

    sosHex= sioOutHexOpenFolded( sos, 72, lastNl );
    if  ( ! sosHex )
	{ XDEB(sosHex); return -1;	}

    rval= bmJpegWriteJfif( bd, buffer, sosHex );

    sioOutClose( sosHex );
    sioOutPrintf( sos, "}\\par}\r\n" );

    return rval;
    }

int bmRtfWriteWmfRtf(		const BitmapDescription *	bd,
				const unsigned char *		buffer,
				SimpleOutputStream *		sos )
    {
    int				rval= 0;
    SimpleOutputStream *	sosHex= (SimpleOutputStream *)0;
    const int			lastNl= 1;

    sosHex= sioOutHexOpenFolded( sos, 72, lastNl );
    if  ( ! sosHex )
	{ XDEB(sosHex); return -1;	}

    bmRtfStartRtf( bd, "\\wmetafile8\\picbmp", sos );
    sioOutPrintf( sos, "\\picbpp%d", bd->bdBitsPerPixel );
    bmRtfImageSize( bd, sos );

    rval= bmWmfWriteWmf( bd, buffer, sosHex );

    sioOutClose( sosHex );
    sioOutPrintf( sos, "}\\par}\r\n" );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Write a bitmap to an rtf stream.					*/
/*									*/
/************************************************************************/

int bmRtfWriteRtf(		const BitmapDescription *	bd,
				const unsigned char *		buffer,
				SimpleOutputStream *		sos )
    { return bmRtfWritePngRtf( bd, buffer, sos ); }

/************************************************************************/
/*									*/
/*  Can write a bitmap to an RTF file?					*/
/*									*/
/************************************************************************/

int bmCanWriteRtfFile(	const BitmapDescription *	bd,
			int				privateFormat )
    {
    switch( privateFormat )
	{
	case 0:
	    return bmCanWritePngFile( bd, 1 );
	case 1:
	    return bmCanWriteJpegFile( bd, 1 );
	case 2:
	    return bmCanWriteWmfFile( bd, 1 );
	default:
	    LDEB(privateFormat); return -1;
	}
    }

int bmWriteRtfFile(	const MemoryBuffer *		filename,
			const unsigned char *		buffer,
			const BitmapDescription *	bd,
			int				privateFormat )
    {
    int				rval= 0;
    SimpleOutputStream *	sos;

    sos= sioOutFileioOpen( filename );
    if  ( ! sos )
	{ XDEB(sos); return -1;	}

    switch( privateFormat )
	{
	case 0:
	    rval= bmRtfWritePngRtf( bd, buffer, sos );
	    break;
	case 1:
	    rval= bmRtfWriteJfifRtf( bd, buffer, sos );
	    break;
	case 2:
	    rval= bmRtfWriteWmfRtf( bd, buffer, sos );
	    break;
	default:
	    LDEB(privateFormat); rval= -1;
	}

    sioOutClose( sos );

    return rval;
    }

