/************************************************************************/
/*									*/
/*  Simple io streams using the C stdio.				*/
/*									*/
/************************************************************************/

#   include	"appUtilConfig.h"

#   include	<stdlib.h>
#   include	<string.h>

#   include	"sioMemory.h"
#   include	<appDebugon.h>

typedef struct SioMemoryPrivate
    {
    MemoryBuffer *	smpBuffer;
    int			smpPosition;
    } SioMemoryPrivate;

static int sioMemoryClose(	void *	voidsmp )
    { free( voidsmp ); return 0;	}

static int sioInMemoryReadBytes(	void *		voidsmp,
					unsigned char *	buffer,
					unsigned int	count )
    {
    SioMemoryPrivate *	smp= (SioMemoryPrivate *)voidsmp;
    MemoryBuffer *	mb= smp->smpBuffer;

    if  ( smp->smpPosition >= mb->mbSize )
	{ return -1;	}

    if  ( smp->smpPosition+ count > mb->mbSize )
	{ count= mb->mbSize- smp->smpPosition;	}

    memcpy( buffer, mb->mbBytes+ smp->smpPosition, count );
    smp->smpPosition += count;

    return count;
    }

SimpleInputStream * sioInMemoryOpenFrom(	const MemoryBuffer *	mb,
						int			from )
    {
    SimpleInputStream *	sis;
    SioMemoryPrivate *	smp;

    smp= (SioMemoryPrivate *)malloc( sizeof(SioMemoryPrivate) );
    if  ( ! smp )
	{ XDEB(smp); return (SimpleInputStream *)0;	}

    smp->smpBuffer= (MemoryBuffer *)mb;
    smp->smpPosition= from;

    sis= sioInOpen( (void *)smp, sioInMemoryReadBytes, sioMemoryClose );

    if  ( ! sis )
	{ XDEB(sis); free( smp ); return (SimpleInputStream *)0; }

    return sis;
    }

SimpleInputStream * sioInMemoryOpen(	const MemoryBuffer *	mb )
    {
    return sioInMemoryOpenFrom( mb, 0 );
    }

static int sioOutMemoryWriteBytes(	void *			voidsmp,
					const unsigned char *	buffer,
					int			count )
    {
    SioMemoryPrivate *	smp= (SioMemoryPrivate *)voidsmp;
    MemoryBuffer *	mb= smp->smpBuffer;

    if  ( smp->smpPosition+ count > mb->mbSize )
	{
	unsigned char *	fresh;

	fresh= (unsigned char *)realloc( mb->mbBytes, smp->smpPosition+ count+ 1 );
	if  ( ! fresh )
	    { LXDEB(smp->smpPosition+ count,fresh); return -1;	}

	mb->mbBytes= fresh;
	}

    memcpy( mb->mbBytes+ smp->smpPosition, buffer, count );

    smp->smpPosition += count;
    if  ( mb->mbSize < smp->smpPosition )
	{
	mb->mbSize=  smp->smpPosition;
	mb->mbBytes[mb->mbSize]= '\0';
	}

    return count;
    }

SimpleOutputStream * sioOutMemoryOpenTo(	MemoryBuffer *	mb,
						int		to )
    {
    SimpleOutputStream *	sos;
    SioMemoryPrivate *		smp;

    if  ( mb->mbSize < to )
	{ LLDEB(mb->mbSize,to); return (SimpleOutputStream *)0;	}

    smp= (SioMemoryPrivate *)malloc( sizeof(SioMemoryPrivate) );
    if  ( ! smp )
	{ XDEB(smp); return (SimpleOutputStream *)0;	}

    smp->smpBuffer= mb;
    smp->smpPosition= to;

    sos= sioOutOpen( (void *)smp, sioOutMemoryWriteBytes, sioMemoryClose );
    if  ( ! sos )
	{ XDEB(sos); free( smp ); return (SimpleOutputStream *)0; }

    mb->mbSize= to;

    return sos;
    }

SimpleOutputStream * sioOutMemoryOpen(	MemoryBuffer *	mb )
    { return sioOutMemoryOpenTo( mb, 0 );	}
