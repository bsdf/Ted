/************************************************************************/
/*									*/
/*  Simple output stream to a particular file descriptor.		*/
/*									*/
/************************************************************************/

#   include	"utilMemoryBuffer.h"
#   include	"sioGeneral.h"

/************************************************************************/
/*									*/
/*  Error return values.						*/
/*									*/
/************************************************************************/

#   define	FDerrPERM		(-1)	/* -EPERM		*/
#   define	FDerrNOENT		(-2)	/* -ENOENT		*/
#   define	FDerrEXIST		(-17)	/* -EXIST		*/
#   define	FDerrACCES		(-13)	/* -EACCES		*/
#   define	FDerrISDIR		(-21)	/* -EISDIR		*/
#   define	FDerrOTHER		(-9999)	/* Unclassified cause	*/

/************************************************************************/
/*									*/
/*  Routine dectarations.						*/
/*									*/
/************************************************************************/

extern SimpleOutputStream * sioOutFdOpen( int		fd );

extern SimpleInputStream * sioInFdOpen( int		fd );

extern int sioFdOpenFile(		const MemoryBuffer *	filename,
					int			read,
					int			write,
					int			append,
					int			create,
					int			exclusive );

extern int sioFdCloseFile(		int			fd );

extern SimpleOutputStream * sioOutFdOpenAt(
				    int				fd,
				    long			offset );

extern SimpleInputStream * sioInFdOpenAt(
				    int				fd,
				    long			offset );

