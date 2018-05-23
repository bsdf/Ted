/************************************************************************/
/*									*/
/*  Simple io streams, Push input or output.. Makes it possible to	*/
/*  to send use an unchanged stream as if it were a different one.	*/
/*  One of the advantages is that you can simply close the pushed clone	*/
/*  without any administration to keep track of the fact that output	*/
/*  goes to the same destination.					*/
/*									*/
/************************************************************************/

#   include	"appUtilConfig.h"

#   include	<stdlib.h>

#   include	"sioPushed.h"
#   include	<appDebugon.h>


/************************************************************************/
/*									*/
/*  Exchange of data over the pushed stream.				*/
/*									*/
/************************************************************************/

static int sioPushedClose( void *	voids )
    {
    if  ( voids )
	{ free( voids );	}

    return 0;
    }

typedef struct PushedInputStream
    {
    SimpleInputStream *		fisSisIn;
    int				fisExhausted;
    } PushedInputStream;

static int sioInPushedReadBytes(	void *		voidfis,
					unsigned char *	buffer,
					unsigned int	count )
    {
    PushedInputStream *	fis= (PushedInputStream *)voidfis;
    int			done= 0;

    if  ( fis->fisExhausted )
	{ LDEB(fis->fisExhausted); return -1;	}

    while( done < count )
	{
	int		got;

	got= sioInReadBytes( fis->fisSisIn, buffer, count- done );
	if  ( got <= 0 )
	    { fis->fisExhausted= 1; break;	}

	done += got;
	buffer += got;
	}

    return done;
    }


SimpleInputStream * sioInPushedOpen(	SimpleInputStream *	sisIn )
    {
    SimpleInputStream *		sis;
    PushedInputStream *		fis;

    fis= (PushedInputStream *)malloc( sizeof(PushedInputStream) );
    if  ( ! fis )
	{ XDEB(fis); return (SimpleInputStream *)0;	}

    fis->fisSisIn= sisIn;
    fis->fisExhausted= 0;

    sis= sioInOpen( (void *)fis, sioInPushedReadBytes, sioPushedClose );

    if  ( ! sis )
	{ XDEB(sis); free( fis ); return (SimpleInputStream *)0; }

    return sis;
    }

/************************************************************************/
/*									*/
/************************************************************************/

typedef struct PushedOutputStream
    {
    SimpleOutputStream *	fosSosOut;
    } PushedOutputStream;

static int sioOutPushedWriteBytes(	void *			voidfos,
					const unsigned char *	buffer,
					int			count )
    {
    PushedOutputStream *	fos= (PushedOutputStream *)voidfos;

    return sioOutWriteBytes( fos->fosSosOut, buffer, count );
    }

SimpleOutputStream * sioOutPushedOpen(	SimpleOutputStream *	sosOut )
    {
    SimpleOutputStream *	sos;
    PushedOutputStream *	fos;

    fos= (PushedOutputStream *)malloc( sizeof(PushedOutputStream) );
    if  ( ! fos )
	{ XDEB(fos); return (SimpleOutputStream *)0;	}

    fos->fosSosOut= sosOut;

    sos= sioOutOpen( (void *)fos, sioOutPushedWriteBytes, sioPushedClose );

    if  ( ! sos )
	{ XDEB(sos); free( fos ); return (SimpleOutputStream *)0; }

    return sos;
    }
