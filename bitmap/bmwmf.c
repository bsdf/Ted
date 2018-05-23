#   include	"bitmapConfig.h"

#   include	"bmintern.h"
#   include	"bmio.h"
#   include	<appDebugon.h>
#   include	<sioFileio.h>
#   include	<sioEndian.h>

#   include	"bmbmp.h"
#   include	"bmWmf.h"

/************************************************************************/
/*									*/
/*  Write a bitmap to a wmf stream.					*/
/*									*/
/************************************************************************/

static int bmWmfWriteSpecialHeader(
				const BitmapDescription *	bd,
				SimpleOutputStream *		sos )
    {
    unsigned int	checksum= 0;
    unsigned int	val;

    int			twipsWide;
    int			twipsHigh;

    const unsigned long	key= 0x9ac6cdd7;

    bmImageSizeTwips( &twipsWide, &twipsHigh, bd );

    sioEndianPutLeInt32( key, sos );
    checksum ^= ( key & 0x0000ffff );
    checksum ^= ( key & 0xffff0000 ) >> 16;

    /*  handle  */
    val= 0;
    sioEndianPutLeUint16( val, sos );
    checksum ^= val;

    /*  left,top  */
    val= 0;
    sioEndianPutLeUint16( val, sos );
    checksum ^= val;
    sioEndianPutLeUint16( val, sos );
    checksum ^= val;

    /*  right,bottom  */
    sioEndianPutLeUint16( twipsWide, sos );
    checksum ^= twipsWide;
    sioEndianPutLeUint16( twipsHigh, sos );
    checksum ^= twipsHigh;

    /*  inch  */
    val= 1440;
    sioEndianPutLeUint16( val, sos );
    checksum ^= val;

    /*  reserved  */
    val= 0;
    sioEndianPutLeUint32( val, sos );
    checksum ^= ( val & 0x0000ffff );
    checksum ^= ( val & 0xffff0000 ) >> 16;

    /*  checksum  */
    sioEndianPutLeUint16( checksum, sos );

    return 22;
    }

static int bmWmfWriteWmfIntern(	const BitmapDescription *	bd,
				const unsigned char *		buffer,
				SimpleOutputStream *		sos,
				int				specialHeader )
    {
    int			rval= 0;

    int			done;
    int			bytesWritten= 0;

    int			size;
    const unsigned char *	bytes;

    int			headSize= 0;
    long		fileSize= 0L;
    long		recordSize;
    const int		privateFormat= 40;
    const int		fileHeader= 0;

    BmpImageHeader	bih;
    RGB8Color *		palette= (RGB8Color *)0;
    MemoryBuffer	bmpBytes;

    int			dibBytes;
    int			dibRecordSize;

    utilInitMemoryBuffer( &bmpBytes );

    dibBytes= bmCollectDib( bd, buffer,
			&bih, &bmpBytes, &palette, privateFormat, fileHeader );
    if  ( dibBytes < 0 )
	{ LLDEB(privateFormat,dibBytes); rval= -1; goto ready;	}

    dibRecordSize= 2+ 1+ 2+ ( 1+ 1+ 1+ 1 )+ ( 1+ 1+ 1+ 1 )+ dibBytes/ 2;

    if  ( specialHeader )
	{
	done= bmWmfWriteSpecialHeader( bd, sos );

	if  ( done < 0 )
	    { LDEB(done); rval= -1; goto ready;	}

	headSize= done;
	bytesWritten += done;
	}

    fileSize= 92+ dibBytes;

    sioEndianPutLeInt16( 1, sos );		/*  fileType		*/
    sioEndianPutLeInt16( 9, sos );		/*  headerSize		*/
    sioEndianPutLeInt16( 768, sos );		/*  windowsVersion	*/
    sioEndianPutLeInt32( fileSize, sos );	/*  fileSize		*/
    sioEndianPutLeInt16( 0, sos );		/*  objectCount		*/
    sioEndianPutLeInt32( dibRecordSize, sos );	/*  maxRecordSize	*/
    sioEndianPutLeInt16( 0, sos );		/*  parameterCount	*/

    bytesWritten += 2+ 2+ 2+ 4+ 2+ 4+ 2;

    recordSize= 2+ 1;
    sioEndianPutLeInt32( recordSize, sos );
    sioEndianPutLeInt16( META_SAVEDC, sos );

    /* recordSize= 3 */
    bytesWritten += 2* recordSize;

    recordSize= 2+ 1+ 1;
    sioEndianPutLeInt32( recordSize, sos );
    sioEndianPutLeInt16( META_SETMAPMODE, sos );
    sioEndianPutLeInt16( MM_ANISOTROPIC, sos );

    /* recordSize= 4 */
    bytesWritten += 2* recordSize;

    recordSize= 2+ 1+ 1+ 1;
    sioEndianPutLeInt32( recordSize, sos );
    sioEndianPutLeInt16( META_SETWINDOWORG, sos );
    sioEndianPutLeInt16( 0, sos );
    sioEndianPutLeInt16( 0, sos );

    /* recordSize= 5 */
    bytesWritten += 2* recordSize;

    recordSize= 2+ 1+ 1+ 1;
    sioEndianPutLeInt32( recordSize, sos );
    sioEndianPutLeInt16( META_SETWINDOWEXT, sos );
    sioEndianPutLeInt16( bd->bdPixelsHigh, sos );
    sioEndianPutLeInt16( bd->bdPixelsWide, sos );

    /* recordSize= 5 */
    bytesWritten += 2* recordSize;

    recordSize= 2+ 1+ 2+ ( 1+ 1+ 1+ 1 )+ ( 1+ 1+ 1+ 1 );
    sioEndianPutLeInt32( dibRecordSize, sos );
    sioEndianPutLeInt16( META_STRETCHBLT, sos );
    sioEndianPutLeInt32( 0xcc0020, sos );
    sioEndianPutLeInt16( bd->bdPixelsHigh, sos );
    sioEndianPutLeInt16( bd->bdPixelsWide, sos );
    sioEndianPutLeInt16( 0, sos ); /* y */
    sioEndianPutLeInt16( 0, sos ); /* x */
    sioEndianPutLeInt16( bd->bdPixelsHigh, sos );
    sioEndianPutLeInt16( bd->bdPixelsWide, sos );
    sioEndianPutLeInt16( 0, sos ); /* y */
    sioEndianPutLeInt16( 0, sos ); /* x */

    done= bmpWriteImageHeader( &bih, sos, palette );
    if  ( done < 0 )
	{ LDEB(1); rval= -1; goto ready; }
    recordSize += done/ 2;

    bytes= utilMemoryBufferGetBytes( &size, &bmpBytes );
    done= sioOutWriteBytes( sos, bytes, size );
    if  ( done != size )
	{ LLDEB(done,size); rval= -1; goto ready;	}

    recordSize += done/ 2;
    if  ( recordSize != dibRecordSize )
	{ LLDEB(recordSize,dibRecordSize); rval= -1; goto ready;	}

    bytesWritten += 2* recordSize;

    recordSize= 2+ 1+ 1;
    sioEndianPutLeInt32( recordSize, sos );
    sioEndianPutLeInt16( META_RESTOREDC, sos );
    sioEndianPutLeInt16( -1, sos );

    /* recordSize= 4 */
    bytesWritten += 2* recordSize;

    recordSize= 2+ 1;
    sioEndianPutLeInt32( recordSize, sos );
    sioEndianPutLeInt16( META_EOF, sos );

    /* recordSize= 3 */
    bytesWritten += 2* recordSize;

    if  ( bytesWritten- headSize != fileSize )
	{ LLDEB(bytesWritten-headSize,fileSize); rval= -1; goto ready;	}

  ready:
    utilCleanMemoryBuffer( &bmpBytes );

    return rval;
    }

int bmWmfWriteWmf(		const BitmapDescription *	bd,
				const unsigned char *		buffer,
				SimpleOutputStream *		sos )
    {
    const int			specialHeader= 0;

    return bmWmfWriteWmfIntern( bd, buffer, sos, specialHeader );
    }

/************************************************************************/
/*									*/
/*  Can write a bitmap to a WMF file?					*/
/*									*/
/************************************************************************/

int bmCanWriteWmfFile( const BitmapDescription *	bd,
			int				privateFormat )
    { return bmCanWriteBmpFile( bd, privateFormat ); }

int bmWriteWmfFile(	const MemoryBuffer *		filename,
			const unsigned char *		buffer,
			const BitmapDescription *	bd,
			int				privateFormat )
    {
    SimpleOutputStream *	sos;
    const int			specialHeader= 1;

    sos= sioOutFileioOpen( filename );
    if  ( ! sos )
	{ LDEB(1); return -1;	}

    if  ( bmWmfWriteWmfIntern( bd, buffer, sos, specialHeader ) )
	{ LDEB(1); sioOutClose( sos ); return -1; }

    sioOutClose( sos );

    return 0;
    }

