/************************************************************************/
/*									*/
/*  Simple io streams using the C stdio.				*/
/*									*/
/************************************************************************/

#   include	"textEncodingConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<iconv.h>
#   include	<errno.h>

#   include	"sioIconv.h"
#   include	<appDebugon.h>

#   if HAVE_ICONV	/*  {{	*/

typedef struct IconvInStream
    {
    SimpleInputStream *		iisSisIn;
    iconv_t			iisIconv;
    unsigned char		iisInBuf[SIOsizBUF];
    size_t			iisInLeft;
    } IconvInStream;

static int sioIconvInClose(	void *	voidiis )
    {
    IconvInStream *	iis= (IconvInStream *)voidiis;

    iconv_close( iis->iisIconv );

    free( iis );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Convert as many bytes as possible.					*/
/*									*/
/*  1)  While there is room in the output buffer..			*/
/*  2)  Get more input.							*/
/*  3)  And convert it.							*/
/*  4)  Why did conversion stop?					*/
/*  5)  We need more input... continue.					*/
/*									*/
/************************************************************************/

static int sioInIconvReadBytes(	void *		voidiis,
				unsigned char *	buffer,
				unsigned int	count )
    {
    IconvInStream *	iis= (IconvInStream *)voidiis;

    size_t		outbytesleft= count;
    char *		outbuf= (char *)buffer;

    int			tooBig= 0;

    /*  1  */
    while( ! tooBig )
	{
#	if defined(__GNUC__) && ! defined(iconv)
	char *		inbuf= (char *)iis->iisInBuf;
#	else
	const char *	inbuf= (const char *)iis->iisInBuf;
#	endif
	int		got;

	/*  2  */
	got= sioInReadBytes( iis->iisSisIn,
				iis->iisInBuf+ iis->iisInLeft,
				SIOsizBUF- iis->iisInLeft );
	if  ( got == 0 )
	    { break;	}
	if  ( got < 0 )
	    { LDEB(got); return -1;	}

	iis->iisInLeft += got;

	/*  3  */
	while( outbytesleft > 0 && iconv( iis->iisIconv,
					&inbuf, &(iis->iisInLeft),
					&outbuf, &outbytesleft ) > 0 ) {;}

	/*  4  */
	switch( errno )
	    {
	    case EINVAL:
		break;
	    case E2BIG:
		tooBig= 1;
		break;

	    case EILSEQ:
		LDEB(errno); return -1;
	    default:
		LDEB(errno); return -1;
	    }

	memcpy( iis->iisInBuf, inbuf, iis->iisInLeft );
	}

    return count- outbytesleft;
    }

SimpleInputStream * sioInIconvOpen(	SimpleInputStream *	sisIn,
					const char *		tocode,
					const char *		frcode )
    {
    SimpleInputStream *	sis;
    iconv_t		ict= iconv_open( tocode, frcode );
    IconvInStream *	iis= (IconvInStream *)0;

    if  ( ! ict )
	{ SSXDEB(tocode,frcode,ict); return (SimpleInputStream *)0;	}

    iis= (IconvInStream *)malloc( sizeof(IconvInStream) );
    if  ( ! iis )
	{ XDEB(iis); iconv_close( ict ); return (SimpleInputStream *)0;	}

    iis->iisSisIn= sisIn;
    iis->iisIconv= ict;
    iis->iisInLeft= 0;

    sis= sioInOpen( (void *)iis, sioInIconvReadBytes, sioIconvInClose );

    if  ( ! sis )
	{
	sioIconvInClose( (void *)iis );
	return (SimpleInputStream *)0;
	}

    return sis;
    }

typedef struct IconvOutStream
    {
    SimpleOutputStream *	iosSosOut;
    iconv_t			iosIconv;
    unsigned char		iosInBuf[SIOsizBUF];
    size_t			iosInLeft;
    } IconvOutStream;

static int sioIconvOutClose(	void *	voidios )
    {
    IconvOutStream *	ios= (IconvOutStream *)voidios;

    if  ( ios->iosInLeft > 0 )
	{ LDEB(ios->iosInLeft);	}

    iconv_close( ios->iosIconv );

    free( ios );

    return 0;
    }

static int sioOutIconvWriteBytes(	void *			voidios,
					const unsigned char *	buffer,
					int			count )
    {
    IconvOutStream *	ios= (IconvOutStream *)voidios;

    size_t		inbytesleft= count;
#   ifdef __GNUC__
    char *		inbuf= (char *)buffer;
#   else
    const char *	inbuf= (const char *)buffer;
#   endif

    unsigned char	out[2*SIOsizBUF];
    char *		outbuf= (char *)out;
    size_t		outbytesleft= sizeof(out);

    int			incomplete= 0;

    if  ( ios->iosInLeft > 0 )
	{
#	if defined(__GNUC__) && ! defined(iconv)
	char *		ib= (char *)ios->iosInBuf;
#	else
	const char *	ib= (const char *)ios->iosInBuf;
#	endif
	size_t		ibl= ios->iosInLeft;
	int		got= 0;
	int		done;

	while( got < count					&&
	       iconv( ios->iosIconv,
		    &ib, &ibl, &outbuf, &outbytesleft ) == -1	&&
	       errno == EINVAL					)
	    { ios->iosInBuf[ibl++]= buffer[got++];	}

	done= ibl- ios->iosInLeft;

	if  ( done == 0 && got == count )
	    { incomplete= 1;	}

	inbytesleft -= done;
	inbuf += done;
	}

    /*  1  */
    while( ! incomplete )
	{
	int		done;

	/*  3  */
	while( iconv( ios->iosIconv,
				&inbuf, &inbytesleft,
				&outbuf, &outbytesleft ) > 0 ) {;}

	done= outbuf- (char *)out;
	if  ( done > 0 )
	    {
	    if  ( sioOutWriteBytes( ios->iosSosOut, out, done ) != done )
		{ LDEB(done); return -1;	}

	    outbuf= (char *)out;
	    outbytesleft= sizeof(out);
	    }

	/*  4  */
	switch( errno )
	    {
	    case EINVAL:
		incomplete= 1;
		break;
	    case E2BIG:
		break;

	    case EILSEQ:
		LDEB(errno); return -1;
	    default:
		LDEB(errno); return -1;
	    }
	}

    if  ( inbytesleft > 0 )
	{
	memcpy( ios->iosInBuf+ ios->iosInLeft, inbuf, inbytesleft );
	ios->iosInLeft += inbytesleft;
	}

    return count;
    }

SimpleOutputStream * sioOutIconvOpen(
					SimpleOutputStream *	sosOut,
					const char *		tocode,
					const char *		frcode )
    {
    SimpleOutputStream *	sos;
    iconv_t			ict= iconv_open( tocode, frcode );
    IconvOutStream *		ios= (IconvOutStream *)0;

    if  ( ! ict )
	{ SSXDEB(tocode,frcode,ict); return (SimpleOutputStream *)0;	}

    ios= (IconvOutStream *)malloc( sizeof(IconvOutStream) );
    if  ( ! ios )
	{ XDEB(ios); iconv_close( ict ); return (SimpleOutputStream *)0; }

    ios->iosSosOut= sosOut;
    ios->iosIconv= ict;
    ios->iosInLeft= 0;

    sos= sioOutOpen( (void *)ios, sioOutIconvWriteBytes, sioIconvOutClose );

    if  ( ! sos )
	{
	sioIconvOutClose( (void *)ios );
	return (SimpleOutputStream *)0;
	}

    return sos;
    }

#   endif /*  HAVE_ICONV }}	*/
