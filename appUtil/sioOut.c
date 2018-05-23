/************************************************************************/
/*									*/
/*  Simplification of stdio: Output.					*/
/*									*/
/************************************************************************/

#   include	"appUtilConfig.h"

#   include	<stdlib.h>
#   include	<string.h>

#   include	"sioGeneral.h"
#   include	<appDebugon.h>

static int sioOutWriteBuffer(	SimpleOutputStream *	sos,
				const unsigned char *	b,
				int			n )
    {
    while( n > 0 )
	{
	int		done;

	done= (*sos->sosWriteBytes)( sos->sosPrivate, b, n );
	if  ( done <= 0 )
	    { LLDEB(n,done); return -1;	}

	b += done; n -= done;
	sos->sosBytesWritten += done;
	}

    return 0;
    }

int sioOutFlushBuffer(	SimpleOutputStream *	sos )
    {
    if  ( sioOutWriteBuffer( sos, sos->sosBuffer, sos->sosN ) )
	{ LDEB(sos->sosN); return -1;	}

    sos->sosN= 0;
    sos->sosP= sos->sosBuffer;

    return 0;
    }

SimpleOutputStream * sioOutOpen(	void *			specific,
					SIOoutWRITEBYTES	writeBytes,
					SIOoutCLOSE		closeIt )
    {
    SimpleOutputStream *	sos;

    sos= (SimpleOutputStream *)malloc( sizeof(SimpleOutputStream) );
    if  ( ! sos )
	{ XDEB(sos); return sos;	}

    sos->sosP= sos->sosBuffer;
    sos->sosN= 0;
    sos->sosPrivate= specific;

    sos->sosBytesWritten= 0;

    sos->sosWriteBytes= writeBytes;
    sos->sosClose= closeIt;

    return sos;
    }

int sioOutClose(	SimpleOutputStream *		sos )
    {
    int		rval= 0;

    if  ( sioOutFlushBuffer( sos ) )
	{ LDEB(sos->sosN); rval= -1;	}

    if  ( (*sos->sosClose)( sos->sosPrivate ) )
	{ XDEB(sos->sosPrivate); rval= -1;	}

    free( sos );

    return rval;
    }

long sioOutGetBytesWritten(			SimpleOutputStream *	sos )
    { return sos->sosBytesWritten+ sos->sosN;	}

int sioOutPutString(	const char *		s,
			SimpleOutputStream *	sos )
    {
    while( *s )
	{
	if  ( sioOutPutByte( *(s++), sos ) )
	    { return -1;	}
	}

    return 0;
    }

int sioOutWriteBytes(	SimpleOutputStream *	sos,
			const unsigned char *	buf,
			int			count )
    {
    int		done;

    done= count;
    if  ( done > SIOsizBUF- sos->sosN )
	{ done= SIOsizBUF- sos->sosN;	}

    memcpy( sos->sosP, buf, done );

    count -= done; buf += done;  sos->sosN += done; sos->sosP += done;

    if  ( count > 0 )
	{
	if  ( sioOutFlushBuffer( sos ) )
	    { LDEB(sos->sosN); return -1;	}

	while( count >= SIOsizBUF )
	    {
	    if  ( sioOutWriteBuffer( sos, buf, SIOsizBUF ) )
		{ LDEB(sos->sosN); return -1;	}

	    count -= SIOsizBUF; buf += SIOsizBUF; done += SIOsizBUF;
	    }

	if  ( count > 0 )
	    {
	    memcpy( sos->sosBuffer, buf, count );
	    done += count; sos->sosN= count; sos->sosP= sos->sosBuffer+ count;
	    }
	}

    return done;
    }

