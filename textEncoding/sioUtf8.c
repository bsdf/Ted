/************************************************************************/
/*									*/
/*  Read/Write UTF-8 sequences at a time.				*/
/*									*/
/************************************************************************/

#   include	"textEncodingConfig.h"

#   include	"sioUtf8.h"
#   include	"uniUtf8.h"
#   include	"uniUtf8Ranges.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Input.								*/
/*									*/
/*  Duplicate the unixGetUtf8() functionality for optimal performance.	*/
/*									*/
/************************************************************************/

int sioInGetUtf8(	SimpleInputStream *	sis )
    {
    unsigned char	buf[10];
    int			c;

    unsigned int	symbol;
    int			step;

    c= sioInGetByte( sis );
    if  ( c == EOF || c < Utf8Ranges[1].urFirstPast )
	{ return c;	}
    buf[0]= c;

    symbol= 0; step= 1;
    while( step < Utf8RangeCount )
	{
	if  ( buf[0] < Utf8Ranges[step].urFirstPast )
	    { break;	}

	c= sioInGetByte( sis );
	if  ( c == EOF )
	    { XDEB(c); return -1;	}
	buf[step]= c;
	if  ( ! UTF_IS_F(buf[step]) )
	    {
	    LXXDEB(step,buf[0],buf[step]);
	    sioInUngetLastRead( sis );
	    return -1;
	    }

	symbol= ( symbol << 6 ) + ( buf[step] & UTF_FM );

	step++;
	}

    if  ( step >= Utf8RangeCount- 1 )
	{ XLLDEB(buf[0],step,Utf8RangeCount); return -1;	}

    if  ( step == 1 )
	{ return buf[0];	}

    symbol= ( ( buf[0] & Utf8Ranges[step].urFirstMask ) << ( 6* ( step -1 ) ) ) + symbol;

    return symbol;
    }

/************************************************************************/
/*									*/
/*  Output.								*/
/*									*/
/************************************************************************/

int sioOutPutUtf8(	int			symbol,
			SimpleOutputStream *	sos )
    {
    char		buf[10];
    int			step;
    int			done;

    step= uniPutUtf8( buf, symbol );
    if  ( step < 1 )
	{ LDEB(step); return -1;	}

    for ( done= 0; done < step; done++ )
	{
	if  ( sioOutPutByte( buf[done], sos ) )
	    { LDEB(done); return -1;	}
	}

    return 0;
    }

