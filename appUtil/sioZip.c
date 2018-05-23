/************************************************************************/
/*									*/
/*  Simple io streams, Data on the stream travels in In/Deflate format	*/
/*  using zlib. The implementation is a VERY SIMPLE ONE: It assumes	*/
/*  that the compressed files fit in memory.				*/
/*									*/
/************************************************************************/

#   include	"appUtilConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<zlib.h>
#   include	<time.h>

#   include	"sioZip.h"
#   include	"sioEndian.h"
#   include	"utilMemoryBuffer.h"
#   include	"utilDosTime.h"

#   include	<appDebugon.h>

#   define	ZIPmethodSTORED			0
#   define	ZIPmethodDEFLATED		8

#   define	ZIPflagsSTORED			0
#   define	ZIPflagsDEFLATED		0

#   define	ZIPversionSTORED		0xa
#   define	ZIPversionDEFLATED		0xa

#   define	ZIPsignatureDIRECTORY_ENTRY	0x02014b50
#   define	ZIPsignatureDIRECTORY_END	0x06054b50
#   define	ZIPsignatureMEMBER		0x04034b50

#   define	ZIPattributesINTERNAL		0 /* binary */
#   define	ZIPattributesEXTERNAL		0 /* from stdin */

/************************************************************************/
/*									*/
/*  Actual Output stream object.					*/
/*									*/
/************************************************************************/

typedef struct ZipOutputStream
    {
    z_stream			zosZstream;
    unsigned char		zosHasZstream;
    unsigned char		zosOutputBuffer[SIOsizBUF];
    ZipOutput *			zosZipOutput;
    int				zosIndex;
    unsigned long		zosHeaderOffset;

    int				zosVersion;
    int				zosFlags;
    int				zosMethod;
    int				zosDosTime;
    int				zosDosDate;

    MemoryBuffer		zosCollectedBuffer;

    unsigned long		zosAdlerCrc;
    unsigned long		zosUncompressedSize;
    unsigned long		zosCollectedSize;

    char *			zosFileName;
    } ZipOutputStream;

static void sioZipInitStream(	ZipOutputStream *	zos )
    {
    zos->zosHasZstream= 0;
    zos->zosZipOutput= (ZipOutput *)0;
    zos->zosIndex= -1;
    zos->zosHeaderOffset= 0;

    zos->zosVersion= ZIPversionSTORED;
    zos->zosFlags= ZIPflagsSTORED;
    zos->zosMethod= ZIPmethodSTORED;
    zos->zosDosTime= 0;
    zos->zosDosDate= 0;

    utilInitMemoryBuffer( &(zos->zosCollectedBuffer) );

    zos->zosAdlerCrc= 0;
    zos->zosUncompressedSize= 0;
    zos->zosCollectedSize= 0;

    zos->zosFileName= (char *)0;
    }

static void sioZipCleanStream(	ZipOutputStream *	zos )
    {
    utilCleanMemoryBuffer( &(zos->zosCollectedBuffer) );

    if  ( zos->zosHasZstream )
	{
	int ret= deflateEnd( &(zos->zosZstream) );
	if  ( ret != Z_OK )
	    { LLDEB(zos->zosHasZstream,ret);	}
	}

    if  ( zos->zosFileName )
	{ free( zos->zosFileName );	}
    }

/************************************************************************/

void sioZipInitOutput(	ZipOutput *			zo )
    {
    zo->zoSosZip= (SimpleOutputStream *)0;
    zo->zoBytesWritten= 0;

    utilInitPagedList( &(zo->zoFileList) );
    utilStartPagedList( &(zo->zoFileList),
			    sizeof(ZipOutputStream),
			    (InitPagedListItem)sioZipInitStream,
			    (InitPagedListItem)sioZipCleanStream );

    zo->zoEntryCount= 0;
    zo->zoDirectoryOffset= 0;
    zo->zoDirectorySize= 0;

    return;
    }

void sioZipCleanOutput(	ZipOutput *			zo )
    {
    /*  zo->zoSosZip Is owned by the consumer */

    utilCleanPagedList( &(zo->zoFileList) );

    return;
    }

/************************************************************************/

static int sioZipWriteLocalFileHeader(	const ZipOutputStream *	zos )
    {
    SimpleOutputStream *	sosZip= zos->zosZipOutput->zoSosZip;

    const int			extraLength= 0;

    int				done= 0;
    int				fileNameLength= 0;

    if  ( zos->zosFileName )
	{ fileNameLength= strlen( zos->zosFileName );	}

    sioEndianPutLeUint32( ZIPsignatureMEMBER, sosZip ); done += 4;
    sioEndianPutLeUint16( zos->zosVersion, sosZip ); done += 2;
    sioEndianPutLeUint16( zos->zosFlags, sosZip ); done += 2;
    sioEndianPutLeUint16( zos->zosMethod, sosZip ); done += 2;
    sioEndianPutLeUint16( zos->zosDosTime, sosZip ); done += 2;
    sioEndianPutLeUint16( zos->zosDosDate, sosZip ); done += 2;
    sioEndianPutLeUint32( zos->zosAdlerCrc, sosZip ); done += 4;
    sioEndianPutLeUint32( zos->zosCollectedSize, sosZip ); done += 4;
    sioEndianPutLeUint32( zos->zosUncompressedSize, sosZip ); done += 4;
    sioEndianPutLeUint16( fileNameLength, sosZip ); done += 2;
    sioEndianPutLeUint16( extraLength, sosZip ); done += 2;

    if  ( fileNameLength > 0 )
	{
	sioOutPutString( zos->zosFileName, sosZip );
	done += fileNameLength;
	}

    /* no extra field */

    return done;
    }

static int sioZipWriteDirectoryEntry(	const ZipOutputStream *	zos )
    {
    SimpleOutputStream *	sosZip= zos->zosZipOutput->zoSosZip;

    const int			extraLength= 0;
    const int			commentLength= 0;
    const int			diskNumberStart= 0;

    int				done= 0;
    int				fileNameLength= 0;

    if  ( zos->zosFileName )
	{ fileNameLength= strlen( zos->zosFileName );	}

    sioEndianPutLeUint32( ZIPsignatureDIRECTORY_ENTRY, sosZip ); done += 4;
    sioEndianPutLeUint16( zos->zosVersion, sosZip ); done += 2;
    sioEndianPutLeUint16( zos->zosVersion, sosZip ); done += 2;
    sioEndianPutLeUint16( zos->zosFlags, sosZip ); done += 2;
    sioEndianPutLeUint16( zos->zosMethod, sosZip ); done += 2;
    sioEndianPutLeUint16( zos->zosDosTime, sosZip ); done += 2;
    sioEndianPutLeUint16( zos->zosDosDate, sosZip ); done += 2;
    sioEndianPutLeUint32( zos->zosAdlerCrc, sosZip ); done += 4;
    sioEndianPutLeUint32( zos->zosCollectedSize, sosZip ); done += 4;
    sioEndianPutLeUint32( zos->zosUncompressedSize, sosZip ); done += 4;
    sioEndianPutLeUint16( fileNameLength, sosZip ); done += 2;
    sioEndianPutLeUint16( extraLength, sosZip ); done += 2;

    sioEndianPutLeUint16( commentLength, sosZip ); done += 2;
    sioEndianPutLeUint16( diskNumberStart, sosZip ); done += 2;
    sioEndianPutLeUint16( ZIPattributesINTERNAL, sosZip ); done += 2;
    sioEndianPutLeUint32( ZIPattributesEXTERNAL, sosZip ); done += 4;

    sioEndianPutLeUint32( zos->zosHeaderOffset, sosZip ); done += 4;

    if  ( fileNameLength > 0 )
	{
	sioOutPutString( zos->zosFileName, sosZip );
	done += fileNameLength;
	}

    /* no extra field */
    /* no comment */

    return done;
    }

static int sioZipWriteDirectoryEnd(	const ZipOutput *	zo )
    {
    SimpleOutputStream *	sosZip= zo->zoSosZip;

    const int			diskNumber= 0;
    const int			commentLength= 0;

    int				done= 0;

    sioEndianPutLeUint32( ZIPsignatureDIRECTORY_END, sosZip ); done += 4;
    /*  number of this disk */
    sioEndianPutLeUint16( diskNumber, sosZip ); done += 2;
    /*  number of the disk where the central dir starts */
    sioEndianPutLeUint16( diskNumber, sosZip ); done += 2;
    /*  Entry count on this disk */
    sioEndianPutLeUint16( diskNumber, sosZip ); done += 2;
    /*  Total entry count */
    sioEndianPutLeUint16( zo->zoEntryCount, sosZip ); done += 2;
    sioEndianPutLeUint32( zo->zoDirectorySize, sosZip ); done += 4;
    sioEndianPutLeUint32( zo->zoDirectoryOffset, sosZip ); done += 4;
    sioEndianPutLeUint16( commentLength, sosZip ); done += 2;

    /* no comment */

    return done;
    }

/************************************************************************/

typedef struct ZipStats
    {
    int			zsEntryCount;
    unsigned long	zsDirectorySize;
    } ZipStats;

static int sioZipEmitDirectoryEntry(	int		n,
					void *		voidzos,
					void *		voidzs )
    {
    ZipOutputStream *		zos= (ZipOutputStream *)voidzos;
    ZipStats *			zs= (ZipStats *)voidzs;
    ZipOutput *			zo= zos->zosZipOutput;

    int				step;

    step= sioZipWriteDirectoryEntry( zos );
    if  ( step < 0 )
	{ LDEB(step); return -1;	}

    zs->zsEntryCount++;
    zs->zsDirectorySize += step;

    zo->zoBytesWritten += step;
    zo->zoDirectorySize += step;

    return 0;
    }

int sioZipFlushOutput(		ZipOutput *		zo )
    {
    ZipStats	zs;
    int		n;
    int		step;

    zs.zsEntryCount= 0;
    zs.zsDirectorySize= 0;

    zo->zoDirectoryOffset= zo->zoBytesWritten;

    n= utilPagedListForAll( &(zo->zoFileList),
				sioZipEmitDirectoryEntry, (void *)&zs );
    if  ( n < 0 )
	{ LDEB(n); return -1;	}
    if  ( n!= zs.zsEntryCount || n != zo->zoEntryCount )
	{ LLLDEB(n,zs.zsEntryCount,zo->zoEntryCount);	}

    step= sioZipWriteDirectoryEnd( zo );
    if  ( step < 0 )
	{ LDEB(step); return -1;	}
    zo->zoBytesWritten += step;

    return 0;
    }

/************************************************************************/
/*									*/
/*  The Zip compression routine:					*/
/*									*/
/************************************************************************/

static int sioOutZipFlushBytesDeflated(	ZipOutputStream *	zos,
					int			n )
    {
    z_stream *			c_stream= &(zos->zosZstream);
    const unsigned char *	b= zos->zosOutputBuffer;

    if  ( n > 0 && utilMemoryBufferAppendBytes( &(zos->zosCollectedBuffer), b, n ) )
	{ LDEB(n); return -1; }

    zos->zosCollectedSize += n;

    c_stream->next_out= zos->zosOutputBuffer;
    c_stream->avail_out= SIOsizBUF;

    return n;
    }

static int sioOutZipWriteBytesDeflated(	void *			voidzos,
					const unsigned char *	buffer,
					int			count )
    {
    ZipOutputStream *		zos= (ZipOutputStream *)voidzos;
    z_stream *			c_stream= &(zos->zosZstream);

    zos->zosUncompressedSize += count;
    zos->zosAdlerCrc= crc32( zos->zosAdlerCrc, buffer, count );

    c_stream->next_in= (unsigned char *)buffer;
    c_stream->avail_in= count;

    while( c_stream->avail_in > 0 )
	{
	int		ret;

	if  ( c_stream->avail_out == 0 )
	    {
	    if  ( sioOutZipFlushBytesDeflated( zos, SIOsizBUF ) != SIOsizBUF )
		{ LDEB(SIOsizBUF); return -1;	}
	    }

	ret= deflate( c_stream, Z_NO_FLUSH );
	if  ( ret != Z_OK )
	    { LDEB(ret); return -1;	}
	}

    return count;
    }

static int sioOutZipWriteBytesStored(	void *			voidzos,
					const unsigned char *	buffer,
					int			count )
    {
    ZipOutputStream *		zos= (ZipOutputStream *)voidzos;

    zos->zosUncompressedSize += count;
    zos->zosAdlerCrc= crc32( zos->zosAdlerCrc, buffer, count );

    if  ( utilMemoryBufferAppendBytes( &(zos->zosCollectedBuffer), buffer, count ) )
	{ LDEB(count); return -1; }

    zos->zosCollectedSize += count;

    return count;
    }

/************************************************************************/
/*									*/
/*  Close a Zip output stream.						*/
/*									*/
/************************************************************************/

static void sioOutZipFinishFile(	ZipOutputStream *	zos,
					int			rval )
    {
    ZipOutput *			zo= zos->zosZipOutput;

    if  ( zos->zosHasZstream )
	{
	int ret= deflateEnd( &(zos->zosZstream) );
	if  ( ret != Z_OK )
	    { LLDEB(zos->zosHasZstream,ret);	}

	zos->zosHasZstream= 0;
	}

    utilCleanMemoryBuffer( &(zos->zosCollectedBuffer) );
    utilInitMemoryBuffer( &(zos->zosCollectedBuffer) );

    if  ( rval )
	{
	if  ( zos->zosIndex >= 0 )
	    {
	    utilPagedListDeleteItemByNumber( &(zo->zoFileList), zos->zosIndex );
	    }
	}
    else{
	zo->zoEntryCount++;
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Emit a single local file 						*/
/*									*/
/************************************************************************/

static int sioZipEmitFile(		ZipOutputStream *	zos )
    {
    ZipOutput *			zo= zos->zosZipOutput;
    SimpleOutputStream *	sosZip= zo->zoSosZip;

    int				size;
    const unsigned char *	bytes;

    int				step;

    zos->zosHeaderOffset= zo->zoBytesWritten;

    step= sioZipWriteLocalFileHeader( zos );
    if  ( step < 0 )
	{ LDEB(step); return -1;	}
    zo->zoBytesWritten += step;

    bytes= utilMemoryBufferGetBytes( &size, &(zos->zosCollectedBuffer) );
    step= sioOutWriteBytes( sosZip, bytes, size );
    if  ( step != size )
	{ LLDEB(step,size); return -1;	}
    zo->zoBytesWritten += step;

    /*
    Only if the file header flags had bit 0xa and the fileds were 0 there

    sioEndianPutLeInt32( zos->zosAdlerCrc, zos->zosSosZip ); done += 4;
    sioEndianPutLeInt32( zos->zosCollectedSize, zos->zosSosZip ); done += 4;
    sioEndianPutLeInt32( zos->zosUncompressedSize, zos->zosSosZip ); done += 4;
    */

    return 0;
    }

static int sioOutZipCloseDeflated(	void *		voidzos )
    {
    int				rval= 0;
    int				ret;

    ZipOutputStream *		zos= (ZipOutputStream *)voidzos;
    z_stream *			c_stream= &(zos->zosZstream);

    c_stream->next_in= (unsigned char *)0;
    c_stream->avail_in= 0;

    for (;;)
	{
	if  ( c_stream->avail_out == 0 )
	    {
	    if  ( sioOutZipFlushBytesDeflated( zos, SIOsizBUF ) != SIOsizBUF )
		{ LDEB(SIOsizBUF); rval= -1; goto ready;	}
	    }

	ret= deflate( c_stream, Z_FINISH );
	if  ( ret == Z_STREAM_END )
	    { break;	}
	if  ( ret != Z_OK )
	    { LDEB(ret); rval= -1; goto ready;	}
	}

    if  ( c_stream->avail_out < SIOsizBUF )
	{
	int	n= SIOsizBUF- c_stream->avail_out;

	if  ( sioOutZipFlushBytesDeflated( zos, n ) != n )
	    { LDEB(n); return -1;	}
	}

    if  ( sioZipEmitFile( zos ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    sioOutZipFinishFile( zos, rval );

    return rval;
    }

static int sioOutZipCloseStored(	void *		voidzos )
    {
    int				rval= 0;

    ZipOutputStream *		zos= (ZipOutputStream *)voidzos;

    if  ( sioZipEmitFile( zos ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    sioOutZipFinishFile( zos, rval );

    return rval;
    }

static int sioZipStartDeflated(		ZipOutputStream *	zos )
    {
    int		ret;

    zos->zosZstream.zalloc = (alloc_func)0;
    zos->zosZstream.zfree = (free_func)0;
    zos->zosZstream.opaque = (voidpf)0;

    ret= deflateInit2( &(zos->zosZstream), Z_DEFAULT_COMPRESSION, Z_DEFLATED,
			    -MAX_WBITS, MAX_MEM_LEVEL, Z_DEFAULT_STRATEGY );
    if  ( ret != Z_OK )
	{ LDEB(ret); return -1;	}
    zos->zosHasZstream= 1;

    zos->zosZstream.next_in= (unsigned char *)0;
    zos->zosZstream.avail_in= 0;

    zos->zosZstream.next_out= zos->zosOutputBuffer;
    zos->zosZstream.avail_out= SIOsizBUF;

    zos->zosVersion= ZIPversionDEFLATED;
    zos->zosFlags= ZIPflagsDEFLATED;
    zos->zosMethod= ZIPmethodDEFLATED;

    return 0;
    }

static int sioZipStartStored(		ZipOutputStream *	zos )
    {
    zos->zosVersion= ZIPversionSTORED;
    zos->zosFlags= ZIPflagsSTORED;
    zos->zosMethod= ZIPmethodSTORED;

    return 0;
    }

SimpleOutputStream * sioOutZipOpen(	ZipOutput *		zo,
					const char *		fileName,
					int			deflated )
    {
    SimpleOutputStream *	sos= (SimpleOutputStream *)0;
    ZipOutputStream *		zos= (ZipOutputStream *)0;

    {
    int		n= -1;

    zos= (ZipOutputStream *)utilPagedListClaimNewItem( &n,
						&(zo->zoFileList) );
    if  ( ! zos )
	{ XDEB(zos); goto ready;	}

    zos->zosIndex= n;
    }

    zos->zosZipOutput= zo;
    utilDosTimeDate( &(zos->zosDosTime), &(zos->zosDosDate),
						    time( (time_t *)0 ) );
    if  ( fileName )
	{
	zos->zosFileName= strdup( fileName );
	if  ( ! zos->zosFileName )
	    { XDEB(zos->zosFileName); goto ready;	}
	}

    zos->zosAdlerCrc= crc32( 0L, Z_NULL, 0 );

    if  ( deflated )
	{
	if  ( sioZipStartDeflated( zos ) )
	    { LDEB(deflated); goto ready;	}

	sos= sioOutOpen( (void *)zos,
			sioOutZipWriteBytesDeflated, sioOutZipCloseDeflated );
	if  ( ! sos )
	    { LXDEB(deflated,sos); goto ready;	}
	zos= (ZipOutputStream *)0; /* steal */
	}
    else{
	if  ( sioZipStartStored( zos ) )
	    { LDEB(deflated); goto ready;	}

	sos= sioOutOpen( (void *)zos,
			sioOutZipWriteBytesStored, sioOutZipCloseStored );
	if  ( ! sos )
	    { LXDEB(deflated,sos); goto ready;	}
	zos= (ZipOutputStream *)0; /* steal */
	}

  ready:

    if  ( zos )
	{ utilPagedListDeleteItemByNumber( &(zo->zoFileList), zos->zosIndex ); }

    return sos;
    }

