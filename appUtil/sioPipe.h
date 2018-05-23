/************************************************************************/
/*									*/
/*  sioPipe.[ch]:	sio equivalents to popen/pclose.		*/
/*									*/
/************************************************************************/

#   ifndef	SIO_PIPE_H
#   define	SIO_PIPE_H

#   include     "sioGeneral.h"
#   include     "utilMemoryBuffer.h"

extern SimpleInputStream * sioInPipeOpen(
				const MemoryBuffer *	command );

extern SimpleOutputStream * sioOutPipeOpen(
				const MemoryBuffer *	command );

#   endif
