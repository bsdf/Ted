/************************************************************************/
/*									*/
/*  Simple io streams using blocks of memory.				*/
/*									*/
/************************************************************************/

#   ifndef	SIO_MEMORY_H
#   define	SIO_MEMORY_H

#   include	"sioGeneral.h"
#   include	"utilMemoryBuffer.h"

extern SimpleInputStream * sioInMemoryOpen(	const MemoryBuffer *	mb );

extern SimpleInputStream * sioInMemoryOpenFrom(	const MemoryBuffer *	mb,
						int			from );

extern SimpleOutputStream * sioOutMemoryOpen(	MemoryBuffer *		mb );

extern SimpleOutputStream * sioOutMemoryOpenTo(	MemoryBuffer *		mb,
						int			to );

#   endif
