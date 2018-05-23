/************************************************************************/
/*									*/
/*  Simplification of stdio.						*/
/*									*/
/************************************************************************/

#   ifndef	SIO_GENERAL_H
#   define	SIO_GENERAL_H


		/*  for EOF:			*/
#   include	<stdio.h>

#   define	SIOsizBUF		1024

typedef int	(*SIOinREADBYTES)( void *, unsigned char *, unsigned int );
typedef int	(*SIOinCLOSE)( void * );

typedef struct SimpleInputStream
    {
    unsigned char	sisBuffer[SIOsizBUF];
    unsigned char *	sisP;
    int			sisN;
    void *		sisPrivate;

    long		sisBytesRead;

    SIOinREADBYTES	sisReadBytes;
    SIOinCLOSE		sisClose;
    } SimpleInputStream;

#   define	sioInGetByte(s)			\
		    ( (--((s)->sisN)) >= 0	?	\
		      *(((s)->sisP)++)		:	\
		      sioInFillBuffer(s)	)

typedef int	(*SIOoutWRITEBYTES)( void *, const unsigned char *, int );
typedef int	(*SIOoutCLOSE)( void * );

typedef struct SimpleOutputStream
    {
    unsigned char	sosBuffer[SIOsizBUF];
    unsigned char *	sosP;
    int			sosN;
    void *		sosPrivate;

    long		sosBytesWritten;

    SIOoutWRITEBYTES	sosWriteBytes;
    SIOoutCLOSE		sosClose;
    } SimpleOutputStream;

#   define	sioOutPutByte(c,s)		\
		( ( (s)->sosN >= SIOsizBUF && sioOutFlushBuffer(s) ) ? -1 : \
		  ( *(((s)->sosP)++)= (c), (s)->sosN++, 0 ) )

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

/*  in  */

extern int sioInFillBuffer(	SimpleInputStream *	sis );

extern int sioInUngetLastRead(	SimpleInputStream *	sis );

extern SimpleInputStream * sioInOpen(	void *			specific,
					SIOinREADBYTES		readBytes,
					SIOinCLOSE		closeIt );

extern long sioInGetBytesRead(		SimpleInputStream *	sis );

extern int sioInClose(	SimpleInputStream *		sis );

extern char * sioInGetString(	char *			s,
				int			size,
				SimpleInputStream *	sis );

extern int sioInReadBytes(	SimpleInputStream *	sis,
				unsigned char *		buf,
				int			count );

/*  out  */

extern int sioOutFlushBuffer(	SimpleOutputStream *	sos );

extern SimpleOutputStream * sioOutOpen(	void *			specific,
					SIOoutWRITEBYTES	writeBytes,
					SIOoutCLOSE		closeIt );

extern int sioOutClose(	SimpleOutputStream *		sos );

extern long sioOutGetBytesWritten(	SimpleOutputStream *	sos );

extern int sioOutPutString(	const char *		s,
				SimpleOutputStream *	sos );

extern int sioOutWriteBytes(	SimpleOutputStream *	sos,
				const unsigned char *	buf,
				int			count );

#ifdef	__GNUC__
extern int sioOutPrintf(	SimpleOutputStream *	sos,
				const char *		format,
				... )
				__attribute__ (( format ( printf, 2, 3 ) ));
#else
extern int sioOutPrintf(	SimpleOutputStream *	sos,
				const char *		format,
				... );
#endif

#   endif
