#   include	"indConfig.h"

#   include	<stdlib.h>

#   include	"ind.h"
#   include	<uniUtf8.h>
#   include	"indlocal.h"
#   include	<appDebugon.h>

/************************************************************************/

typedef struct DumpThrough
    {
    char *	dtOutbuf;
    int		dtLevel;
    } DumpThrough;

static int indDumpAcceptingLink(	void *		vdt,
					int		dir,
					int		tnFrom,
					int		tnTo,
					unsigned short	key,
					int		toAccepts )
    {
    DumpThrough *	dt= (DumpThrough *)vdt;
    int			step;

    step= uniPutUtf8( dt->dtOutbuf+ dt->dtLevel, key );
    if  ( step < 1 )
	{ LDEB(step); return- 1;	}

    if  ( dir > 0 && toAccepts )
	{
	dt->dtOutbuf[dt->dtLevel+ step]= '\0';
	appDebug( "> %s\n", (char *)dt->dtOutbuf );
	}

    dt->dtLevel += dir* step;

    return 0;
    }

void indDump(	void *		voidind )
    {
    char		outbuf[400];
    IND *		ind= (IND *)voidind;
    DumpThrough		dt;

    dt.dtOutbuf= outbuf;
    dt.dtLevel= 0;

    if  ( ind->ind_magic != INDMAGIC )
	{ LDEB(ind->ind_magic); return;	}

    indINDforall( ind, ind->ind_start, (void *)&dt, indDumpAcceptingLink );
    }

/************************************************************************/
/*									*/
/*  Make an empty finite automaton.					*/
/*									*/
/************************************************************************/

void *	indMake( void )
    { return (void *)indINDmake( 0 );	}

/************************************************************************/
/*									*/
/*  Read a finite automaton from file. If the readOnly flag is set,	*/
/*  the file is memory mapped. This han an advantage on some operating	*/
/*  systems.								*/
/*									*/
/************************************************************************/

void *	indRead(	const char *	filename,
			int		readOnly )
    { return (void *)indINDread(  filename, readOnly );	}

/************************************************************************/
/*  Make automaton voidint accept word key. The number of the accepting	*/
/*  state is returned, or -1 on failure.				*/
/************************************************************************/

int indPutUtf8(	void *		voidind,
		const char *	key )
    {
    IND *	ind= (IND *)voidind;
    int		tnStart;

    if  ( ind->ind_magic != INDMAGIC )
	{ LDEB(ind->ind_magic); return -1;	}

    if  ( ( tnStart= ind->ind_start ) < 0 )
	{
	tnStart= ind->ind_start= indTNmake( ind );
	if  ( tnStart < 0 )
	    { LDEB(tnStart); return -1;	}
	}

    return indINDputUtf8( ind, tnStart, key );
    }

int indPutUtf16(	void *			voidind,
			const unsigned short *	key )
    {
    IND *	ind= (IND *)voidind;
    int		tnStart;

    if  ( ind->ind_magic != INDMAGIC )
	{ LDEB(ind->ind_magic); return -1;	}

    if  ( ( tnStart= ind->ind_start ) < 0 )
	{
	tnStart= ind->ind_start= indTNmake( ind );
	if  ( tnStart < 0 )
	    { LDEB(tnStart); return -1;	}
	}

    return indINDputUtf16( ind, tnStart, key );
    }

int indPutSuffixUtf16(	void *			voidind,
			const unsigned short *	key )
    {
    IND *	ind= (IND *)voidind;
    int		tnStart;

    if  ( ind->ind_magic != INDMAGIC )
	{ LDEB(ind->ind_magic); return -1;	}

    tnStart= ind->ind_start= indTNmake( ind );
    if  ( tnStart < 0 )
	{ LDEB(tnStart); return -1;	}

    if  ( indINDputUtf16( ind, tnStart, key ) < 0 )
	{ LDEB(tnStart); return -1;	}

    return tnStart;
    }

/************************************************************************/
/*									*/
/*  Tell automaton voidind to no longer accept the word key. This is	*/
/*  done by making the state that accepted key a non final state.	*/
/*									*/
/************************************************************************/

int indForget(	void *		voidind,
		const char *	key )
    {
    IND *	ind= (IND *)voidind;

    if  ( ind->ind_magic != INDMAGIC )
	{ LDEB(ind->ind_magic); return -1;	}

    return indINDforget( ind, key );
    }

/************************************************************************/
/*  Find the state in automaton voidind to which key is a path.		*/
/*  returns the number of the state or -1 if no such state exists.	*/
/*  *paccpt is set to 0 if it is a non final state, to other values in	*/
/*  final states.							*/
/************************************************************************/

int indGetUtf8(	int *		paccept,
		void *		voidind,
		const char *	key )
    {
    IND *	ind= (IND *)voidind;

    if  ( ind->ind_magic != INDMAGIC )
	{ LDEB(ind->ind_magic); return -1;	}

    if  ( ind->ind_start < 0 )
	{ return -1;	}

    return indINDgetUtf8( paccept, ind, ind->ind_start, key );
    }

int indGetUtf16(	int *			paccept,
			void *			voidind,
			const unsigned short *	key )
    {
    IND *	ind= (IND *)voidind;

    if  ( ind->ind_magic != INDMAGIC )
	{ LDEB(ind->ind_magic); return -1;	}

    if  ( ind->ind_start < 0 )
	{ return -1;	}

    return indINDgetUtf16( paccept, ind, ind->ind_start, key );
    }

int indAddSuffixUtf16(	void *			voidind,
			const unsigned short *	prefix,
			int			tnSuf )
    {
    IND *	ind= (IND *)voidind;

    int		accepts= 0;
    int		tnTo;

SDEB("Does not work"); return -1;
    if  ( ind->ind_magic != INDMAGIC )
	{ LDEB(ind->ind_magic); return -1;	}

    tnTo= indINDgetUtf16( &accepts, ind, ind->ind_start, prefix );
    if  ( tnTo < 0 )
	{ LDEB(tnTo); return -1;	}

    return indINDaddSuffix( ind, tnTo, tnSuf );
    }

/************************************************************************/
/*  See whether automaton voidind accepts word key.			*/
/*  Certain case mapping are tried if the word is not accepted as is.	*/
/*  The way case is changed is reported in *phow.			*/
/*  The value of *phow is set to one of the INDhSOMETHING values from	*/
/*  ind.h.								*/
/************************************************************************/

int	indGetWord(	int *			pWhatWasShifted,
			void *			voidind,
			const char *		word,
			int			asPrefix )
    {
    IND *	ind= (IND *)voidind;

    if  ( ind->ind_magic != INDMAGIC )
	{ LDEB(ind->ind_magic); return -1;	}

    return indWRDget( ind, pWhatWasShifted, word, asPrefix );
    }

/************************************************************************/
/*  free a finite automaton.						*/
/************************************************************************/

void indFree( void *	voidind )
    {
    IND *	ind= (IND *)voidind;

    if  ( ind->ind_magic != INDMAGIC )
	{ return;	}

    indINDfree( ind );
    }

/************************************************************************/
/*  Write an automaton to file.						*/
/************************************************************************/

int	indWrite(	void *		voidind,
			const char *	filename )
    {
    IND *	ind= (IND *)voidind;

    if  ( ind->ind_magic != INDMAGIC )
	{ LDEB(ind->ind_magic); return -1;	}

    return indINDwrite( ind, filename );
    }

/************************************************************************/
/*  Minimise an automaton.						*/
/************************************************************************/

void *	indMini(	void *	voidind )
    {
    IND *	ind= (IND *)voidind;

    if  ( ind->ind_magic != INDMAGIC )
	{ return (void *)0;	}

    return (void *)indINDmini( ind );
    }

/************************************************************************/
/*  Return an automaton that has the same properties as voidind.	*/
/*  Node numbers and transition numbers are given using a 'depth first'	*/
/*  scan of 'voidind'.							*/
/************************************************************************/

void *	indRenumber(	void *	voidind )
    {
    IND *	ind= (IND *)voidind;

    if  ( ind->ind_magic != INDMAGIC )
	{ return (void *)0;	}

    return (void *)indINDrenumber( ind );
    }

/************************************************************************/
/*  Make guesses for a certain word.					*/
/*  For every guess, (*fun)( something, <guess>, how ) is called.	*/
/************************************************************************/

int indGuess(	void *				voidind,
		const char *			word,
		SpellGuessContext *		sgc,
		int				how )
    {
    int			rval= 0;
    unsigned short *	ucods= (unsigned short *)0;
    int			ulen= 0;
    IND *		ind= (IND *)voidind;

    if  ( ind->ind_magic != INDMAGIC )
	{ LDEB(ind->ind_magic); rval= -1; goto ready;	}

    ucods= uniUtf8ToUnicodes( &ulen, word );
    if  ( ! ucods )
	{ XDEB(ucods); rval= -1; goto ready;		}

    rval= indINDguess( ind, ucods, ulen, sgc, how );

  ready:
    if  ( ucods )
	{ free( ucods );	}

    return rval;
    }

/************************************************************************/
/*  Make guesses for a certain word.					*/
/*  If the word starts with a capital, of consists of capitals only,	*/
/*  the guesses are based on the word or the tail of the word mapped	*/
/*  to lower case. The how argument to fun is used to report what was	*/
/*  mapped.								*/
/*  For every guess, (*fun)( something, <guess>, how ) is called.	*/
/************************************************************************/

int indGuessWord(	void *				voidind,
			const char *			word,
			SpellGuessContext *		sgc )
    {
    int			rval= 0;
    unsigned short *	source= (unsigned short *)0;
    int			sourceLen= 0;
    IND *		ind= (IND *)voidind;

    if  ( ind->ind_magic != INDMAGIC )
	{ LDEB(ind->ind_magic); rval= -1; goto ready;	}

    source= uniUtf8ToUnicodes( &sourceLen, word );
    if  ( ! source )
	{ XDEB(source); rval= -1; goto ready;		}

    rval= indWRDguess( ind, source, sourceLen, sgc );

  ready:
    if  ( source )
	{ free( source );	}

    return rval;
    }

