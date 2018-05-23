/************************************************************************/
/*									*/
/*  VPrintf for sio.							*/
/*									*/
/************************************************************************/

#   ifndef	SIO_VPRINTF_H
#   define	SIO_VPRINTF_H

#   include	"sioGeneral.h"
#   include	<stdarg.h>

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int sioOutVPrintf(	SimpleOutputStream *	sos,
				const char *		format,
				va_list			ap );
#   endif
