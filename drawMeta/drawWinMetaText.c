#   include	"drawMetaConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>

#   include	"drawWinMetaImpl.h"
#   include	<sioEndian.h>
#   include	<uniUtf8.h>
#   include	<textConverter.h>

#   include	<appDebugon.h>

# if 0
#    define	WMFDEB(x)	(x)
# else
#    define	WMFDEB(x)	/*nothing*/
# endif

int appWinMetaReadUtf16Text(	DeviceContext *		dc,
				int			expectChars,
				SimpleInputStream *	sis )
    {
    int		nchars;
    int		nbytes;
    int		done= 0;

    utilEmptyMemoryBuffer( &(dc->dcCollectedText) );

    nbytes= 0;
    for ( nchars= 0; nchars < expectChars; nchars++ )
	{
	int		step;
	char		scratch[6+1];
	int		c= sioEndianGetLeUint16( sis ); done += 2;

	if  ( c == 0 )
	    { nchars++; break;	}

	step= uniPutUtf8( scratch, c );
	if  ( step < 1 || step > 3 )
	    { XLDEB(c,step); break;	}

	if  ( utilMemoryBufferAppendBytes( &(dc->dcCollectedText),
				    (const unsigned char *)scratch, step ) )
	    { LDEB(step); return -1;	}

	nbytes += step;
	}

    while( nchars < expectChars )
	{ sioEndianGetLeInt16( sis ); done += 2; nchars++;	}

    return done;
    }

static int appWinMetaProduceUtf8(	void *		voiddc,
					int		produced,
					const char *	bytes,
					int		count )
    {
    DeviceContext *	dc= (DeviceContext *)voiddc;

    if  ( utilMemoryBufferAppendBytes( &(dc->dcCollectedText), (unsigned char *)bytes, count ) )
	{ LDEB(count); return -1;	}

    return count;
    }

int appWinMetaReadLegacyText(	DeviceContext *		dc,
				int			count,
				int			expectBytes,
				SimpleInputStream *	sis )
    {
    int				rval= 0;
    int				nbytes;
    int				done= 0;
    const OfficeCharset *	oc;
    char *			buf= (char *)0;
    int				idx;

    utilEmptyMemoryBuffer( &(dc->dcCollectedText) );

    oc= utilGetOfficeCharsetByCharset( &idx, dc->dcFont.lfCharSet );

    if  ( ! oc || ! oc->ocEncodingName )
	{
	if  ( dc->dcFont.lfCharSet != FONTcharsetDEFAULT )
	    { LPDEB(dc->dcFont.lfCharSet,oc);	}

	for ( nbytes= 0; nbytes < count; nbytes++ )
	    {
	    int		step;
	    char	scratch[6+1];
	    int		c= sioInGetByte( sis ); done += 1;

	    if  ( c == 0 )
		{ nbytes++; break;	}

	    step= uniPutUtf8( scratch, c );
	    if  ( step < 1 || step > 3 )
		{ XLDEB(c,step); break;	}

	    if  ( utilMemoryBufferAppendBytes( &(dc->dcCollectedText),
				    (const unsigned char *)scratch, step ) )
		{ LDEB(step); rval= -1; goto ready;	}
	    }
	}
    else{
	int	consumed= 0;
	int	produced= 0;

	textConverterSetNativeEncodingName( &(dc->dcTextConverter),
							oc->ocEncodingName );
	/* Do at setup time! */
	dc->dcTextConverter.tcProduce= appWinMetaProduceUtf8;

	buf= (char *)malloc( count+ 1 );
	if  ( ! buf )
	    { LPDEB(count,buf); rval= -1; goto ready;	}

	if  ( sioInReadBytes( sis, (unsigned char *)buf, count ) != count )
	    { LDEB(count); rval= -1; goto ready;	}
	nbytes= count; done += count;
	buf[count]= '\0';

	textConverterConvertToUtf8( &(dc->dcTextConverter), (void *)dc,
					&consumed, produced, buf, count );
	}

    while( nbytes < expectBytes )
	{
	if  ( sioInGetByte( sis ) < 0 )
	    { LDEB(1); rval= -1; goto ready;	}

	done += 1; nbytes++;
	}

    rval= done;

  ready:

    if  ( buf )
	{ free( buf );	}

    return rval;
    }

