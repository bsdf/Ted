/************************************************************************/
/*									*/
/*  Simple io streams using raw file IO.				*/
/*									*/
/************************************************************************/

#   include	"utilMemoryBuffer.h"
#   include	"sioGeneral.h"

extern SimpleOutputStream * sioOutFileioOpen(
				    const MemoryBuffer *	filename );

extern SimpleOutputStream * sioOutFileioOpenForAppend(
				    const MemoryBuffer *	filename );

extern SimpleOutputStream * sioOutFileioOpenS(
				    const char *		filename );

extern SimpleOutputStream * sioOutFileioOpenTempFile(
				    MemoryBuffer *		filename,
				    const char *		template );

extern SimpleInputStream * sioInFileioOpen(
				    const MemoryBuffer *	filename );

extern SimpleInputStream * sioInFileioOpenS(
				    const char *		filename );
