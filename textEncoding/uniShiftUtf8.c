/************************************************************************/
/*									*/
/*  Translation between unicode values and utf-8.			*/
/*									*/
/************************************************************************/

#   include	"textEncodingConfig.h"

#   include	<stdlib.h>

#   include	"ucd.h"
#   include	"uniUtf8.h"
#   include	"uniShiftUtf8.h"
#   include	"ucdGeneralCategory.h"

#   include	<appDebugon.h>

static int uniShiftUtf8StringUp( char *			to,
				int *			segments,
				int			segmentCount,
				const char *		from,
				int			len )
    {
    int		n;
    int		seg= 0;

    n= 0;
    while( n < len )
	{
	unsigned short		symbol;
	int			n0= 0;
	int			n1= 0;
	int			df;
	int			dt;

	if  ( segments && seg >= segmentCount )
	    { LLDEB(seg,segmentCount); return -1;	}

	while( n < len )
	    {
	    df= uniGetUtf8( &symbol, from );
	    if  ( df < 1 )
		{ LDEB(df); return -1;	}
	    if  (   ucdIsLl( symbol ) )
		{ break;	}

	    dt= uniPutUtf8( to, symbol );
	    n0 += dt; from += df; to += dt; n += df;
	    }

	while( n < len )
	    {
	    df= uniGetUtf8( &symbol, from );
	    if  ( df < 1 )
		{ LDEB(df); return -1;	}
	    if  ( ! ucdIsLl( symbol ) )
		{ break;	}

	    symbol= ucdToUpper( symbol );

	    dt= uniPutUtf8( to, symbol );
	    n1 += dt; from += df; to += dt; n += df;
	    }

	if  ( segments )
	    {
	    segments[2*seg+ 0]= n0;
	    segments[2*seg+ 1]= n1;
	    }

	seg++;
	}

    *to= '\0';
    return seg;
    }

/************************************************************************/
/*									*/
/*  Shift a string to different case.					*/
/*									*/
/*  If the caller asks for segments, the even segments receive the	*/
/*  (byte)lengths of the streches that already were in the desired	*/
/*  case. The odd segments recieve the (byte)lengths of the streches	*/
/*  that were not in the desired case.					*/
/*									*/
/************************************************************************/

int uniShiftUtf8String(	char **			pShifted,
			int **			pSegments,
			int *			pSegmentCount,
			int			how,
			const char *		sourceString,
			int			len )
    {
    int			rval= 0;

    char *		shiftedString= (char *)0;
    int	*		segments= (int *)0;
    int			segmentCount= 0;

    switch( how )
	{
	case SHIFT_ASIS:
	    LDEB(how); rval= -1; goto ready;

	case SHIFT_LOWER:
	    LDEB(how); rval= -1; goto ready;

	case SHIFT_FIRSTCAP:
	    LDEB(how); rval= -1; goto ready;

	case SHIFT_UPPER:
	    shiftedString= (char *)malloc( 2* len+ 1 );
	    if  ( ! shiftedString )
		{ XDEB(shiftedString); rval= -1; goto ready;	}

	    if  ( pSegments && pSegmentCount )
		{
		/*  3  */
		segments= (int *)malloc( (len+ 2)* sizeof(int) );
		if  ( ! segments )
		    { XDEB(segments); rval= -1; goto ready;	}
		}

	    segmentCount= uniShiftUtf8StringUp( shiftedString,
							segments, len+ 2,
							sourceString, len );
	    if  ( segmentCount < 1 )
		{ LDEB(segmentCount); rval= -1; goto ready;	}

	    *pShifted= shiftedString; shiftedString= (char *)0; /* steal */
	    if  ( pSegments )
		{ *pSegments= segments; segments= (int *)0; /* steal */	}
	    if  ( pSegmentCount )
		{ *pSegmentCount= segmentCount;				}
	    break;

	default:
	    LDEB(how); rval= -1; goto ready;
	}


  ready:

    if  ( shiftedString )
	{ free( shiftedString );	}
    if  ( segments )
	{ free( segments );	}

    return rval;
    }
