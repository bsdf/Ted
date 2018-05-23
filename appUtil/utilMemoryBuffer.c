/************************************************************************/
/*									*/
/*  Manage an array of data bytes and its size.				*/
/*									*/
/************************************************************************/

#   include	"appUtilConfig.h"

#   include	<stdlib.h>
#   include	<string.h>

#   include	"utilMemoryBuffer.h"
#   include	<appDebugon.h>

void utilInitMemoryBuffer(	MemoryBuffer *	mb )
    {
    mb->mbSize= 0;
    mb->mbBytes= (unsigned char *)0;
    return;
    }

void utilCleanMemoryBuffer(	MemoryBuffer *	mb )
    {
    if  ( mb->mbBytes )
	{ free( mb->mbBytes );	}

    mb->mbSize= 0;
    mb->mbBytes= (unsigned char *)0;

    return;
    }

void utilEmptyMemoryBuffer(	MemoryBuffer *	mb )
    {
    mb->mbSize= 0;
    if  ( mb->mbBytes )
	{ mb->mbBytes[0]= '\0';	}

    return;
    }

/************************************************************************/
/*									*/
/*  Resize a memory buffer. Guarantee that there is a terminated buffer	*/
/*									*/
/************************************************************************/

int utilMemoryBufferSetSize(	MemoryBuffer *	mb,
				int		size )
    {
    if  ( utilMemoryBufferIsEmpty( mb ) || size > mb->mbSize )
	{
	unsigned char *	fresh;

	fresh= (unsigned char *)realloc( mb->mbBytes, size+ 1 );
	if  ( ! fresh )
	    { LXDEB(size,fresh); return -1;	}

	if  ( ! mb->mbBytes )
	    { fresh[0]= '\0';	}

	mb->mbBytes= fresh;
	mb->mbSize= size;
	fresh[mb->mbSize]= '\0';
	}

    if  ( size < mb->mbSize )
	{
	mb->mbSize= size;
	mb->mbBytes[mb->mbSize]= '\0';
	}

    return 0;
    }

int utilMemoryBufferSetBytes(	MemoryBuffer *		mb,
				const unsigned char *	bytes,
				int			size )
    {
    unsigned char *	fresh;

    fresh= (unsigned char *)realloc( mb->mbBytes, size+ 1 );
    if  ( ! fresh )
	{ LXDEB(size,fresh); return -1;	}

    memcpy( fresh, bytes, size );

    mb->mbBytes= fresh;
    mb->mbSize= size;
    fresh[mb->mbSize]= '\0';

    return 0;
    }

int utilMemoryBufferSetString( MemoryBuffer *			mb,
				const char *			s )
    {
    if  ( ! s )
	{ XDEB(s); return -1;	}

    return utilMemoryBufferSetBytes( mb, (const unsigned char *)s, strlen( s ) );
    }

int utilMemoryBufferAppendBytes(	MemoryBuffer *		mb,
					const unsigned char *	bytes,
					int			size )
    {
    unsigned char *	fresh;

    fresh= (unsigned char *)realloc( mb->mbBytes, mb->mbSize+ size+ 1 );
    if  ( ! fresh )
	{ LXDEB(size,fresh); return -1;	}

    memcpy( fresh+ mb->mbSize, bytes, size );

    mb->mbBytes= fresh;
    mb->mbSize= mb->mbSize+ size;
    fresh[mb->mbSize]= '\0';

    return 0;
    }

int utilMemoryBufferAppendString(	MemoryBuffer *		mb,
					const char *		s )
    {
    if  ( ! s )
	{ XDEB(s); return -1;	}

    return utilMemoryBufferAppendBytes( mb,
				    (const unsigned char *)s, strlen( s ) );
    }

int utilMemoryAppendBuffer(		MemoryBuffer *		to,
					const MemoryBuffer *	from )
    {
    return utilMemoryBufferAppendBytes( to, from->mbBytes, from->mbSize );
    }

int utilCopyMemoryBuffer(	MemoryBuffer *		mbTo,
				const MemoryBuffer *	mbFrom )
    {
    if  ( utilMemoryBufferSetBytes( mbTo, mbFrom->mbBytes, mbFrom->mbSize ) )
	{ LDEB(mbFrom->mbSize); return -1;	}

    return 0;
    }

const unsigned char * utilMemoryBufferGetBytes(	int *			pSize,
						const MemoryBuffer *	from )
    {
    *pSize= from->mbSize;
    return from->mbBytes?from->mbBytes:(const unsigned char *)"";
    }

const char * utilMemoryBufferGetString(	const MemoryBuffer *	from )
    {
    return from->mbBytes?(const char *)from->mbBytes:"";
    }

int utilMemoryCompareBuffers(	const MemoryBuffer *	mb1,
				const MemoryBuffer *	mb2 )
    {
    int		len;
    int		cmp;

    len= mb1->mbSize;
    if  ( len > mb2->mbSize )
	{ len=  mb2->mbSize;	}

    cmp= memcmp( mb1->mbBytes, mb2->mbBytes, len );
    if  ( cmp > 0 )
	{ cmp=  1;	}
    if  ( cmp < 0 )
	{ cmp= -1;	}

    if  ( cmp == 0 )
	{
	if  ( mb2->mbSize > mb1->mbSize )
	    { cmp=  1;	}
	if  ( mb2->mbSize < mb1->mbSize )
	    { cmp= -1;	}
	}

    return cmp;
    }

/************************************************************************/
/*									*/
/*  Replace bytes in a memory buffer.					*/
/*									*/
/*  1)	Refuse silly parameters. We do not want to create holes in the	*/
/*	buffer.								*/
/*									*/
/************************************************************************/

int utilMemoryBufferReplaceBytes(	MemoryBuffer *		mb,
					int			offHead,
					int			offPast,
					const unsigned char *	addedBytes,
					int			addedCount )
    {
    int		sizeShift;
    int		oldSize= mb->mbSize;

    sizeShift= addedCount- offPast+ offHead;

    /*  1  */
    if  ( offHead < 0 || offHead > mb->mbSize )
	{ LLDEB(offHead,mb->mbSize); return -1;	}

    if  ( sizeShift > 0						&&
	  utilMemoryBufferSetSize( mb, mb->mbSize+ sizeShift )	)
	{ LLDEB(mb->mbSize,sizeShift); return -1;	}

    if  ( oldSize > 0 )
	{
	int		n= oldSize- offPast+ 1;

	if  ( n < 0 )
	    { LDEB(n); return -1;	}
	else{
	    memmove( mb->mbBytes+ offPast+ sizeShift, mb->mbBytes+ offPast, n );
	    }
	}

    memcpy( mb->mbBytes+ offHead, addedBytes, addedCount );

    if  ( sizeShift < 0 )
	{
	mb->mbSize += sizeShift;
	mb->mbBytes[mb->mbSize]= '\0';
	}

    return 0;
    }


int utilMemoryBufferGetRange(	MemoryBuffer *		mbTo,
				const MemoryBuffer *	mbFrom,
				int			offFrom,
				int			lenFrom )
    {
    return utilMemoryBufferReplaceBytes( mbTo, 0, mbTo->mbSize,
					    mbFrom->mbBytes+ offFrom, lenFrom );
    }

/************************************************************************/
/*									*/
/*  Save the contents of a memory buffer to an allocated string.	*/
/*									*/
/************************************************************************/

char * utilMemoryStrdup(	const MemoryBuffer *	mbFrom )
    {
    if  ( utilMemoryBufferIsEmpty( mbFrom ) )
	{
	char * empty= malloc(1);
	if  ( empty )
	    { *empty= '\0';	}

	return empty;
	}

    return strdup( (const char *)mbFrom->mbBytes );
    }

/************************************************************************/
/*									*/
/*  Compare the contents to a string.					*/
/*									*/
/************************************************************************/

int utilMemoryBufferEqualsString(		const MemoryBuffer *	mb,
						const char *		s )
    {
    MemoryBuffer	mbLoc;

    mbLoc.mbBytes= (unsigned char *)s;
    mbLoc.mbSize= strlen( s );

    return utilMemoryCompareBuffers( &mbLoc, mb ) == 0;
    }

/************************************************************************/
/*									*/
/*  Find bytes in a memory buffer.					*/
/*									*/
/************************************************************************/

int utilMemoryBufferLastIndexOf(	const MemoryBuffer *	mb,
					int			chr )
    {
    const char *	p;

    if  ( utilMemoryBufferIsEmpty( mb ) )
	{ return -1;	}

    p= strrchr( (const char *)mb->mbBytes, chr );
    if  ( p )
	{ return p- (const char *)mb->mbBytes;	}
    else{ return -1;				}
    }

int utilMemoryBufferIndexOf(		const MemoryBuffer *	mb,
					int			chr )
    {
    const char *	p;

    if  ( utilMemoryBufferIsEmpty( mb ) )
	{ return -1;	}

    p= strchr( (const char *)mb->mbBytes, chr );
    if  ( p )
	{ return p- (const char *)mb->mbBytes;	}
    else{ return -1;				}
    }
