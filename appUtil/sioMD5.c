/************************************************************************/
/*									*/
/*  Read/Write data and calculate an MD5 on the fly.			*/
/*									*/
/************************************************************************/

#   include	"appUtilConfig.h"

#   include	<stdlib.h>

#   include	"sioMD5.h"
#   include	"utilMD5.h"
#   include	<appDebugon.h>


/************************************************************************/
/*									*/
/*  Exchange of data over the pushed stream.				*/
/*									*/
/************************************************************************/

typedef struct MD5InputStream
    {
    SimpleInputStream *		fisSisIn;
    int				fisExhausted;
    MD5Context			fisMD5Context;
    unsigned char *		fisDigest;
    } MD5InputStream;

static int sioInMD5Close( void *	voidfis )
    {
    MD5InputStream *	fis= (MD5InputStream *)voidfis;

    utilMD5Final( fis->fisDigest, &(fis->fisMD5Context) );

    free( voidfis );

    return 0;
    }

static int sioInMD5ReadBytes(	void *		voidfis,
				unsigned char *	buffer,
				unsigned int	count )
    {
    MD5InputStream *	fis= (MD5InputStream *)voidfis;
    int			done= 0;

    if  ( fis->fisExhausted )
	{ /*LDEB(fis->fisExhausted);*/ return -1;	}

    while( done < count )
	{
	int		got;

	got= sioInReadBytes( fis->fisSisIn, buffer, count- done );
	if  ( got <= 0 )
	    { fis->fisExhausted= 1; break;	}

	utilMD5Update( &(fis->fisMD5Context), buffer, got );

	done += got;
	buffer += got;
	}

    return done;
    }

SimpleInputStream * sioInMD5Open(	unsigned char *		digest,
					SimpleInputStream *	sisIn )
    {
    SimpleInputStream *		sis;
    MD5InputStream *		fis;

    fis= (MD5InputStream *)malloc( sizeof(MD5InputStream) );
    if  ( ! fis )
	{ XDEB(fis); return (SimpleInputStream *)0;	}

    fis->fisSisIn= sisIn;
    fis->fisExhausted= 0;
    fis->fisDigest= digest;

    utilMD5Init( &(fis->fisMD5Context) );

    sis= sioInOpen( (void *)fis, sioInMD5ReadBytes, sioInMD5Close );
    if  ( ! sis )
	{ XDEB(sis); free( fis ); return (SimpleInputStream *)0; }

    return sis;
    }

/************************************************************************/
/*									*/
/************************************************************************/

typedef struct MD5OutputStream
    {
    SimpleOutputStream *	fosSosOut;
    MD5Context			fosMD5Context;
    unsigned char *		fosDigest;
    } MD5OutputStream;

static int sioOutMD5Close( void *	voidfos )
    {
    MD5OutputStream *	fos= (MD5OutputStream *)voidfos;

    utilMD5Final( fos->fosDigest, &(fos->fosMD5Context) );

    free( voidfos );

    return 0;
    }

static int sioOutMD5WriteBytes(	void *			voidfos,
				const unsigned char *	buffer,
				int			count )
    {
    MD5OutputStream *	fos= (MD5OutputStream *)voidfos;

    utilMD5Update( &(fos->fosMD5Context), buffer, count );

    return sioOutWriteBytes( fos->fosSosOut, buffer, count );
    }

SimpleOutputStream * sioOutMD5Open(	unsigned char *		digest,
					SimpleOutputStream *	sosOut )
    {
    SimpleOutputStream *	sos;
    MD5OutputStream *		fos;

    fos= (MD5OutputStream *)malloc( sizeof(MD5OutputStream) );
    if  ( ! fos )
	{ XDEB(fos); return (SimpleOutputStream *)0;	}

    fos->fosSosOut= sosOut;
    fos->fosDigest= digest;

    utilMD5Init( &(fos->fosMD5Context) );

    sos= sioOutOpen( (void *)fos, sioOutMD5WriteBytes, sioOutMD5Close );
    if  ( ! sos )
	{ XDEB(sos); free( fos ); return (SimpleOutputStream *)0; }

    return sos;
    }
