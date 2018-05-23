/************************************************************************/
/*									*/
/*  An array of bytes and its (maximum) size.				*/
/*									*/
/************************************************************************/

#   ifndef	UTIL_MEMORY_H
#   define	UTIL_MEMORY_H

typedef struct MemoryBuffer
    {
    int			mbSize;
    unsigned char *	mbBytes;
    } MemoryBuffer;

# define utilMemoryBufferIsEmpty( mb ) ( (mb)->mbSize == 0 )

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void utilInitMemoryBuffer(	MemoryBuffer *	mb );
extern void utilEmptyMemoryBuffer(	MemoryBuffer *	mb );
extern void utilCleanMemoryBuffer(	MemoryBuffer *	mb );

extern int utilCopyMemoryBuffer(	MemoryBuffer *		mbTo,
					const MemoryBuffer *	mbFrom );

extern int utilMemoryBufferSetSize(	MemoryBuffer *	mb,
					int		size );

extern int utilMemoryBufferSetString(	MemoryBuffer *		mb,
					const char *		s ) ;

extern int utilMemoryBufferSetBytes(	MemoryBuffer *		mb,
					const unsigned char *	bytes,
					int			size );

extern const unsigned char * utilMemoryBufferGetBytes(
					int *			pSize,
					const MemoryBuffer *	mbFrom );

extern const char * utilMemoryBufferGetString(
					const MemoryBuffer *	mbFrom );

extern int utilMemoryCompareBuffers(	const MemoryBuffer *	mb1,
					const MemoryBuffer *	mb2 );

# define utilEqualMemoryBuffer( mb1, mb2 ) \
			    ( (mb1)->mbSize == (mb2)->mbSize	&& \
			      utilMemoryCompareBuffers( (mb1), (mb2) ) == 0 )

extern int utilMemoryAppendBuffer(	MemoryBuffer *		to,
					const MemoryBuffer *	from );
extern int utilMemoryBufferAppendString(	MemoryBuffer *		to,
					const char *		s );
extern int utilMemoryBufferAppendBytes(	MemoryBuffer *		mb,
					const unsigned char *	bytes,
					int			size );

extern int utilMemoryBufferReplaceBytes( MemoryBuffer *		mb,
					int			offHead,
					int			offPast,
					const unsigned char *	addedBytes,
					int			addedCount );

extern int utilMemoryBufferGetRange(	MemoryBuffer *		mbTo,
					const MemoryBuffer *	mbFrom,
					int			offFrom,
					int			lenFrom );

extern char * utilMemoryStrdup(	const MemoryBuffer *		mbFrom );

extern int utilMemoryBufferEqualsString(	const MemoryBuffer *	mb,
						const char *		s );

extern int utilMemoryBufferLastIndexOf(	const MemoryBuffer *	mb,
					int			cal );

extern int utilMemoryBufferIndexOf(	const MemoryBuffer *	mb,
					int			cal );

#   endif	/*  UTIL_MEMORY_H	*/
