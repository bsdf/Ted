/************************************************************************/
/*									*/
/*  Printf to memory buffers.						*/
/*									*/
/************************************************************************/

#   include	"appUtilConfig.h"

#   include	<stdarg.h>

#   include	"sioGeneral.h"
#   include	"sioVPrintf.h"
#   include	"sioMemory.h"
#   include	"utilMemoryBuffer.h"
#   include	"utilMemoryBufferPrintf.h"

#   include	<appDebugon.h>

extern int utilMemoryBufferPrintf( MemoryBuffer *	mb,
				const char *		format,
				... )
    {
    va_list			ap;
    int				ret;
    SimpleOutputStream *	sos= (SimpleOutputStream *)0;

    sos= sioOutMemoryOpen( mb );
    if  ( ! sos )
	{ XDEB(sos); return -1;	}

    va_start( ap, format );

    ret= sioOutVPrintf( sos, format, ap );

    va_end( ap );

    if  ( sioOutClose( sos ) )
	{ LDEB(1); return -1;	}

    return ret;
    }

