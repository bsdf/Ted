/************************************************************************/
/*									*/
/*  Translation between unicode values and utf-8.			*/
/*									*/
/************************************************************************/

#   include	"textEncodingConfig.h"

#   include	<stdlib.h>
#   include	<string.h>

#   include	"uniUtf8.h"
#   include	"uniUtf8Ranges.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  UTF-8 Calculations.							*/
/*									*/
/************************************************************************/

const Utf8Range Utf8Ranges[]=
{
    { 0x00, 0x00, 0x00,	         0x1 },	/*  0	*/
    { 0x80, 0x7f, 0x00,	        0x80 },	/*  1	*/
    { 0xe0, 0x1f, 0xc0,	       0x800 },	/*  2	*/
    { 0xf0, 0x0f, 0xe0,	     0x10000 },	/*  3	*/
    { 0xf8, 0x07, 0xf0,	    0x200000 },	/*  4	*/
    { 0xfc, 0x03, 0xf8,	   0x4000000 },	/*  5	*/
    { 0xfe, 0x01, 0xfc,	  0x80000000 },	/*  6	*/

#   if 0
    { 0xff, 0x00, 0xfe,	0x1000000000 },	/*  7	*/
#   endif

    { 0xffffffff, 0xffffffff, 0xffffffff }
};

const int Utf8RangeCount= sizeof(Utf8Ranges)/sizeof(Utf8Range);

/************************************************************************/
/*									*/
/*  Retrieve one character from a string.				*/
/*									*/
/************************************************************************/

int unixGetUtf8(	unsigned int *		pSymbol,
			const unsigned char *	buf )
    {
    unsigned int	symbol;
    int			step;

    if  ( ! buf[0] )
	{ return 0;	}
    if  ( buf[0] < Utf8Ranges[1].urFirstPast )
	{ *pSymbol= buf[0]; return 1;	}

    symbol= 0; step= 1;
    while( step < Utf8RangeCount )
	{
	if  ( buf[0] < Utf8Ranges[step].urFirstPast )
	    { break;	}
	if  ( ! UTF_IS_F(buf[step]) )
	    { LXXDEB(step,buf[0],buf[step]); return 0;	}

	symbol= ( symbol << 6 ) + ( buf[step] & UTF_FM );

	step++;
	}

    if  ( step >= Utf8RangeCount- 1 )
	{ XLLDEB(buf[0],step,Utf8RangeCount); return 0;	}

    if  ( step == 1 )
	{ *pSymbol= buf[0]; return 1;	}

    symbol= ( ( buf[0] & Utf8Ranges[step].urFirstMask ) << ( 6* ( step -1 ) ) ) + symbol;

    *pSymbol= symbol;
    return step;
    }

int uni_GetUtf8(	unsigned short *	pSymbol,
			const char *		buf )
    {
    unsigned int	symbol;
    int			step;

    step= unixGetUtf8( &symbol, (const unsigned char *)buf );
    if  ( step < 1 )
	{ LDEB(step); return step;	}

    if  ( symbol > 0xffff )
	{ XLLDEB(buf[0],step,symbol); return -1;	}

    *pSymbol= symbol;
    return step;
    }

/************************************************************************/
/*									*/
/*  Emit one character to a string.					*/
/*									*/
/************************************************************************/

int uni_PutUtf8(	char *		buf,
			int		symbol )
    {
    int			step;
    int			done;

    if  ( symbol < 0 )
	{ LDEB(symbol); return 0;	}
    if  ( symbol < 1 )
	{ return 0;	}

    step= 1;
    while( step < Utf8RangeCount )
	{
	if  ( symbol < Utf8Ranges[step].urValuePast )
	    { break;	}

	step++;
	}

    if  ( step >= Utf8RangeCount- 1 )
	{ LLLDEB(symbol,step,Utf8RangeCount); return 0;	}

    if  ( step == 1 )
	{ buf[0]= symbol; return 1;	}

    for ( done= step-1; done > 0; done-- )
	{
	buf[done]= ( symbol & UTF_FM ) | UTF_FH;
	symbol >>= 6;
	}

    buf[0] = Utf8Ranges[step].urFirstHead |
				    ( symbol & Utf8Ranges[step].urFirstMask );

    return step;
    }

# if 0
extern void xxxx( void );
void xxxx( void )
    {
    int		r;

    for ( r= 2; r < Utf8RangeCount; r++ )
	{
	unsigned int	fp= ( 0xff << ( 7- r ) ) & 0xff;
	unsigned int	fh= ( 0xff << ( 8- r ) ) & 0xff;
	unsigned int	fm= ( 0xff >> ( r+ 1 ) ) & 0xff;

	unsigned long	vp= fm;

	int		i;

	for ( i= 1; i < r; i++ )
	    { vp= ( vp << 6 )+ UTF_FM;	}
	vp++;

	appDebug( "{ 0x%02x, 0x%02x, 0x%02x, 0x%lx }, /* %d */\n",
						    fp, fm, fh, vp, r );
	}

    for ( r= 1; r < Utf8RangeCount- 1; r++ )
	{
	unsigned int		symbol1;

	LLDEB(r,Utf8Ranges[r- 1].urValuePast);

	for ( symbol1=  Utf8Ranges[r- 1].urValuePast;
	      symbol1 < Utf8Ranges[r   ].urValuePast;
	      symbol1++ )
	    {
	    unsigned char	buf[10];
	    int			step1, step2;
	    unsigned int	symbol2;

	    if  ( symbol1 % 1000000 == 0 )
		{ LDEB(symbol1);	}

	    step1= uniPutUtf8( buf, symbol1 );
	    if  ( step1 < 1 )
		{ LLDEB(symbol1,step1); abort();	}
	    if  ( step1 != r )
		{ LLLDEB(symbol1,step1,r); abort();	}

	    step2= unixGetUtf8( &symbol2, buf );
	    if  ( step2 != step1 )
		{ LLLDEB(symbol1,step1,step2); abort();	}
	    if  ( symbol2 != symbol1 )
		{ LLLDEB(step1,symbol1,symbol2); abort();	}
	    }
	}
    }

# endif

/************************************************************************/
/*									*/
/*  Translate an utf8 string to 16 bit unicode values.			*/
/*									*/
/************************************************************************/

unsigned short * uniUtf8ToUnicodes(	int *		pUlen,
					const char *	word )
    {
    int			l= strlen( (char *)word );
    unsigned short *	ucods= (unsigned short *)malloc( ( l+ 1 )* sizeof(unsigned short) );
    int			fr;
    int			ulen;

    if  ( ! ucods )
	{ LXDEB(l,ucods); return (unsigned short *)0;	}

    fr= 0; ulen= 0;
    while( fr < l )
	{
	int	step= uniGetUtf8( ucods+ ulen, word+ fr );

	if  ( step < 1 )
	    { break;	}

	ulen++; fr += step;
	}

    *pUlen= ulen;
    return ucods;
    }

/************************************************************************/
/*									*/
/*  Translate string of 16 bit unicode values to UTF-8.			*/
/*									*/
/************************************************************************/

char * uniUnicodesToUtf8(	int *			pMBlen,
				const unsigned short *	codes )
    {
    int		ulen;
    char *	utf8;
    int		fr;
    int		mblen;

    ulen= 0;
    while( codes[ulen] )
	{ ulen++;	}

    utf8= (char *)malloc( 4* ulen+ 1 );
    if  ( ! utf8 )
	{ LXDEB(ulen,utf8); return (char *)0;	}

    fr= 0; mblen= 0;
    while( fr < ulen )
	{
	int	step= uniPutUtf8( utf8+ mblen, codes[fr] );

	if  ( step < 1 )
	    { break;	}

	fr++; mblen += step;
	}

    *pMBlen= mblen;
    return utf8;
    }

