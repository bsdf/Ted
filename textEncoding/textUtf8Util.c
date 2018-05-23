/************************************************************************/
/*									*/
/*  Utf8 string manipulation.						*/
/*									*/
/************************************************************************/

#   include	"textEncodingConfig.h"

#   include	<string.h>

#   include	"uniUtf8.h"
#   include	"textUtf8Util.h"
#   include	"ucd.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Mirror a string.							*/
/*									*/
/*  We have verified in the UCD that the mirror characters take as many	*/
/*  bytes as the original. Verify here also!				*/
/*									*/
/************************************************************************/

int textMirrorUtf8String(	char *			to,
				const char *		from,
				int			len )
    {
    int		done= 0;

    to += len;

    while( done < len )
	{
	unsigned short		symbol;
	unsigned short		mirror;
	int			step;

	step= uniGetUtf8( &symbol, from );
	if  ( step < 1 )
	    { LDEB(step); return -1;	}

	to -= step;

	mirror= ucdToMirror( symbol );
	if  ( mirror != symbol )
	    {
	    char	buf[10];
	    int		step2;

	    step2= uniPutUtf8( buf, mirror );
	    if  ( step2 != step )
		{ LLDEB(step2,step); return -1;	}

	    memcpy( to, buf, step2 );
	    }
	else{
	    memcpy( to, from, step );
	    }

	done += step;
	from += step;
	}

    return done;
    }

