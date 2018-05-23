/************************************************************************/
/*									*/
/*  Paper sizes.							*/
/*									*/
/************************************************************************/

#   include	"appUtilConfig.h"

#   if USE_LIBPAPER
#	include		<paper.h>
#   endif

#   include	<string.h>
#   include	<stdio.h>
#   include	<stdlib.h>

#   include	"appSystem.h"
#   include	"appPaper.h"
#   include	"geoString.h"
#   include	<sioFileio.h>
#   include	<appDebugon.h>

#   if USE_LIBPAPER

    static int CalledPaperInit= 0;

    static const struct paper ** UtilPapers;
    static int UtilPaperCount = 0;

    static int utilPaperCollectPapers( void )
	{
	int			n= 0;
	const struct paper *	sp;

	if  ( UtilPaperCount > 0 )
	    { LDEB(UtilPaperCount); return -1;	}

	sp= paperfirst();
	if  ( ! sp )
	    { return -1;	}
	n++;

	for (;;)
	    {
	    sp= papernext( sp );
	    if  ( ! sp )
		{ break;	}
	    n++;
	    }

	UtilPapers= (const struct paper **)malloc(
				    (n+1)* sizeof(const struct paper *) );
	if  ( ! UtilPapers )
	    { LXDEB(n,UtilPapers); return -1;	}

	sp= paperfirst(); n--;
	if  ( ! sp )
	    { LXDEB(n+1,sp);			}
	else{ UtilPapers[UtilPaperCount++]= sp;	}

	while( n > 0 )
	    {
	    sp= papernext( sp ); n--;
	    if  ( ! sp )
		{ LXDEB(n+1,sp); break;			}
	    else{ UtilPapers[UtilPaperCount++]= sp;	}
	    }

	return 0;
	}

#   else

    typedef struct PaperSize
	{
	const char *	psMnemonic;
	const char *	psLabel;
	int		psWideTwips;
	int		psHighTwips;
	} PaperSize;

    static PaperSize UtilPaperSizes[]=
    {
	{ "a4",		"A4: 210 x 297 mm",		11909,	16834,	},
	{ "a5",		"A5: 149 x 210 mm",		8417,	11909,	},
	{ "a6",		"A6: 105 x 149 mm",		5954,	8417,	},
	{ "letter",	"Letter: 8 1/2 x 11 \"",	12240,	15840,	},
	{ "legal",	"Legal: 8 1/2 x 14 \"",		12240,	20720,	},
	{ "executive",	"Executive: 7 1/4 x 10 1/2 \"",	10440,	15120,	},
	{ "a3",		"A3: 297 x 420 mm",		16834,	23818,	},
    };

    static const char *	utilDefaultPaperSize= (const char *)0;

#   endif

/************************************************************************/
/*									*/
/*  Get information on a certain paper size.				*/
/*									*/
/************************************************************************/

int utilPaperGetInfoByNumber(	int		n,
				int *		pWide,
				int *		pHigh,
				const char **	pLabel )
    {
#   if	USE_LIBPAPER

    if  ( ! CalledPaperInit )
	{ paperinit(); CalledPaperInit= 1;	}
    if  ( ! UtilPapers && utilPaperCollectPapers() )
	{ LDEB(1); return -1;	}
    if  ( ! UtilPapers )
	{ XDEB(UtilPapers); return -1;	}

    if  ( n < 0 || n >= UtilPaperCount )
	{ return -1;	}

    if  ( pWide )
	{ *pWide= (int) ( ( paperpswidth( UtilPapers[n] )*  20.0 ) +0.499 ); }
    if  ( pHigh )
	{ *pHigh= (int) ( ( paperpsheight( UtilPapers[n] )* 20.0 ) +0.499 ); }
    if  ( pLabel )
	{ *pLabel= papername( UtilPapers[n] );				}

    return 0;
#   else

    PaperSize *	ps;

    if  ( n < 0								||
	  n >= (int)( sizeof(UtilPaperSizes)/sizeof(PaperSize) )	)
	{ return -1;	}

    ps= UtilPaperSizes+ n;

    if  ( pWide )
	{ *pWide= ps->psWideTwips;	}
    if  ( pHigh )
	{ *pHigh= ps->psHighTwips;	}
    if  ( pLabel )
	{ *pLabel= ps->psLabel;		}

    return 0;
#   endif
    }

/************************************************************************/
/*									*/
/*  Try to find a paper size by its size.				*/
/*									*/
/************************************************************************/

int utilPaperGetNumberBySize(	int		wide,
				int		high )
    {
    int		gw= wide/200;
    int		gh= high/200;

    int		i;

#   if	USE_LIBPAPER
    const struct paper *	sp;

    if  ( ! CalledPaperInit )
	{ paperinit(); CalledPaperInit= 1;	}
    if  ( ! UtilPapers && utilPaperCollectPapers() )
	{ LDEB(1); return -1;	}
    if  ( ! UtilPapers )
	{ XDEB(UtilPapers); return -1;	}

    sp= paperwithsize( ( wide+ 10 )/ 20.0, ( high+ 10 )/ 20.0 );
    if  ( sp )
	{
	for ( i= 0; i < UtilPaperCount; i++ )
	    {
	    if  ( sp == UtilPapers[i] )
		{ return i;	}
	    }

	LLSDEB(wide/20,high/20,papername(sp));
	FFDEB(paperpswidth(sp),paperpsheight(sp));
	return -1;
	}

    for ( i= 0; i < UtilPaperCount; i++ )
	{
	int		dw= paperpswidth( UtilPapers[i] )* 20- wide;
	int		dh= paperpsheight( UtilPapers[i] )* 20- high;

	if  ( dw < 0 )
	    { dw= -dw;	}
	if  ( dh < 0 )
	    { dh= -dh;	}

	if  ( dw < gw && dh < gh )
	    { return i; }
	}

    return -1;

#   else

    for ( i= 0; i < sizeof(UtilPaperSizes)/sizeof(PaperSize); i++ )
	{
	int		dw= UtilPaperSizes[i].psWideTwips- wide;
	int		dh= UtilPaperSizes[i].psHighTwips- high;

	if  ( dw < 0 )
	    { dw= -dw;	}
	if  ( dh < 0 )
	    { dh= -dh;	}

	if  ( dw < gw && dh < gh )
	    { return i; }
	}

    return -1;

#   endif
    }

/************************************************************************/
/*									*/
/*  Try to translate a string to a paper size.				*/
/*									*/
/*  1)  (If applicable:) Give libpaper a chance.			*/
/*  2)  Try standard sizes.						*/
/*  3)  Try capitalized standard sizes.					*/
/*									*/
/************************************************************************/

int utilPaperSizeFromString(	int *		pFormat,
				int *		pWide,
				int *		pHigh,
				int		unitType,
				const char *	paperString )
    {
    int		wide;
    int		high;
    int		i;

    /*  1  */
#   if	USE_LIBPAPER
    const struct paper *	sp;

    if  ( ! CalledPaperInit )
	{ paperinit(); CalledPaperInit= 1;	}
    if  ( ! UtilPapers && utilPaperCollectPapers() )
	{ LDEB(1); return -1;	}
    if  ( ! UtilPapers )
	{ XDEB(UtilPapers); return -1;	}

    sp= paperinfo( paperString );
    if  ( sp )
	{
	for ( i= 0; i < UtilPaperCount; i++ )
	    {
	    if  ( sp == UtilPapers[i] )
		{
		if  ( pWide )
		    { *pWide= (int) ( ( paperpswidth( sp )*  20.0 ) +0.499 ); }
		if  ( pHigh )
		    { *pHigh= (int) ( ( paperpsheight( sp )* 20.0 ) +0.499 ); }
		if  ( pFormat )
		    { *pFormat= i;	}
		return 0;
		}
	    }
	SXDEB(paperString,sp);
	}

#   else

    /*  2  */
    for ( i= 0; i < sizeof(UtilPaperSizes)/sizeof(PaperSize); i++ )
	{
	if  ( ! strcmp( UtilPaperSizes[i].psMnemonic, paperString ) )
	    {
	    utilPaperGetInfoByNumber( i, pWide, pHigh, (const char **)0 );
	    if  ( pFormat )
		{ *pFormat= i;	}
	    return i;
	    }
	}

    /*  3  */
    for ( i= 0; i < sizeof(UtilPaperSizes)/sizeof(PaperSize); i++ )
	{
	if  ( tolower( paperString[0] ) ==
			tolower( UtilPaperSizes[i].psMnemonic[0] )	&&
	      ! strcmp( UtilPaperSizes[i].psMnemonic+ 1, paperString+ 1 ) )
	    {
	    utilPaperGetInfoByNumber( i, pWide, pHigh, (const char **)0 );
	    if  ( pFormat )
		{ *pFormat= i;	}
	    return i;
	    }
	}

#   endif

    if  ( geoRectangleFromString( paperString, unitType, &wide, &high ) )
	{ SDEB(paperString); return -1;	}

    i= utilPaperGetNumberBySize( wide, high );
    if  ( i >= 0 )
	{
	if  ( pWide )
	    { *pWide= wide;	}
	if  ( pHigh )
	    { *pHigh= high;	}
	if  ( pFormat )
	    { *pFormat= i;	}

	return 0;
	}

    return -1;
    }

/************************************************************************/
/*									*/
/*  Return the default paper size.					*/
/*									*/
/************************************************************************/

const char * utilPaperDefaultSize( void )
    {
#   if	USE_LIBPAPER

    if  ( ! CalledPaperInit )
	{ paperinit(); CalledPaperInit= 1;	}

    return systempapername();

#   else

    const char *	rval= (const char *)0;
    SimpleInputStream *	sis= (SimpleInputStream *)0;
    MemoryBuffer	sizeFile;

    utilInitMemoryBuffer( &sizeFile );
    if  ( utilMemoryBufferSetString( &sizeFile, "/etc/papersize" ) )
	{ LDEB(1); goto ready;	}

    if  ( utilDefaultPaperSize )
	{
	if  ( utilDefaultPaperSize[0] )
	    { rval= utilDefaultPaperSize;	}

	goto ready;
	}

    if  ( ! appTestFileExists( &sizeFile ) )
	{
	sis= sioInFileioOpen( &sizeFile );
	if  ( ! sis )
	    { XDEB(sis); goto ready;	}
	else{
	    char	buf[150];

	    while( sioInGetString( buf, sizeof(buf)-1, sis ) )
		{
		int		l;

		buf[sizeof(buf)-1]= '\0';
		l= strlen( buf );
		if  ( l > 0 && buf[l-1] == '\n' )
		    { buf[l-1]= '\0'; l--; }

		if  ( l == 0 || buf[0] == '#' )
		    { continue;	}

		utilDefaultPaperSize= strdup( buf );
		goto ready;
		}
	    }
	}

    utilDefaultPaperSize= strdup( "" );

  ready:

    if  ( sis )
	{ sioInClose( sis );	}

    utilCleanMemoryBuffer( &sizeFile );

    return rval;
#   endif
    }

