#   include	"indConfig.h"

#   include	<stdlib.h>

#   include	"ind.h"
#   include	<uniUtf8.h>
#   include	<ucdGeneralCategory.h>
#   include	<ucd.h>
#   include	"indlocal.h"
#   include	<appDebugon.h>

/************************************************************************/

static int indGetCapModes(	int *			pTshift,
				int *			pHshift,
				int *			pFshift,
				int *			pTail,
				unsigned short *	ucods,
				int			ulen )
    {
    int		tshift= INDhASIS;
    int		hshift= INDhALLUP;
    int		fshift= INDhALLUP;
    int		tail= 0;
    int		i;

    if  ( ! ucdIsLu( ucods[0] ) )
	{
	*pTshift= INDhASIS;
	*pHshift= INDhASIS;
	*pFshift= INDhASIS;
	*pTail= ulen;
	return 0;
	}

    if  ( ulen > 1 && ucods[0] == 'I' && ucods[1] == 'J' )
	{
	tail= 2;
	tshift= INDhIJTAILUP; hshift= INDhIJUP;
	/* IJMUIDEN, IJsco */
	}
    else{
	tail= 1;
	tshift= INDhTAILUP; hshift= INDhFIRSTUP;
	/* EDAM, Knudde */
	}

    /************************************************************/
    /*  Inspect tail to determine shift kinds.			*/
    /*  If it is in upper case, shift it down.			*/
    /************************************************************/
    for ( i= tail; i < ulen; i++ )
	{
	if  ( ucdIsL( ucods[i] ) )
	    {
	    if  ( ! ucdIsLu( ucods[i] ) )
		{
		tshift= INDhASIS;
		fshift= INDhASIS;
		if  ( ! ucdIsLl( ucods[i] ) )
		    { hshift= INDhASIS;	}
		}
	    }
	}

    if  ( tshift != INDhASIS )
	{
	for ( i= tail; i < ulen; i++ )
	    {
	    if  ( ucdIsLu( ucods[i] ) )
		{ ucods[i]= ucdToLower( ucods[i] );	}
	    }
	}

    *pTshift= tshift;
    *pHshift= hshift;
    *pFshift= fshift;
    *pTail= tail;
    return 1;
    }

/************************************************************************/
/*									*/
/*  See whether a word is accepted.					*/
/*									*/
/*  This is a utility routine for spelling checking. Some variations	*/
/*  in capitalization are tried.					*/
/*									*/
/************************************************************************/

int indWRDget(	IND *		ind,
		int *		pWhatWasShifted,
		const char *	word,
		int		asPrefix )
    {
    int				rval= -1;

    unsigned short *		ucods= (unsigned short *)0;
    char *			shifted= (char *)0;

    int				ulen;
    int				accepted;
    int				tn;

    int				tshift;
    int				hshift;
    int				fshift;
    int				tail;

    if  ( ind->ind_start < 0 )
	{ LDEB(ind->ind_start); rval= -1; goto ready;	}

    tn= indINDgetUtf8( &accepted, ind, ind->ind_start, word );
    if  ( tn >= 0 && ( accepted || asPrefix ) )
	{ *pWhatWasShifted= INDhASIS; rval= 0; goto ready;	}

    ucods= uniUtf8ToUnicodes( &ulen, word );
    if  ( ! ucods )
	{ XDEB(ucods); rval= -1; goto ready;	}

    if  ( ulen < 1 )
	{ LDEB(ulen); rval= -1; goto ready;	}

    if  ( indGetCapModes( &tshift, &hshift, &fshift, &tail, ucods, ulen ) )
	{
	int		i;

	shifted= (char *)malloc( 4* ulen+ 1 );
	if  ( ! shifted )
	    { LXDEB(ulen,shifted); rval= -1; goto ready;	}

	/****************************************************************/
	/*  Try tail shifted down.					*/
	/*  NOTE: that the head is in upper case anyway.		*/
	/****************************************************************/
	if  ( tshift != INDhASIS )
	    {
	    indShiftWord( shifted, ucods, ulen, INDhASIS );

	    if  ( indINDgetUtf8( &accepted, ind, ind->ind_start,
					    shifted ) >= 0	&&
		  ( accepted || asPrefix )			)
		{ *pWhatWasShifted= tshift; rval= 0; goto ready;	}
	    }

	/****************************************************************/
	/*  Try head shifted down.					*/
	/*  This is only activated if the tail is in lower case		*/
	/****************************************************************/
	if  ( hshift != INDhASIS )
	    {
	    for ( i= 0; i < tail; i++ )
		{ ucods[i]= ucdToLower( ucods[i] );	}

	    indShiftWord( shifted, ucods, ulen, INDhASIS );

	    if  ( indINDgetUtf8( &accepted, ind, ind->ind_start,
					    shifted ) >= 0	&&
		  ( accepted || asPrefix )			)
		{ *pWhatWasShifted= hshift; rval= 0; goto ready;	}
	    }

	/****************************************************************/
	/*  Try fully shifted down.					*/
	/*  This is only activated if head and tail are in lower case	*/
	/****************************************************************/
	if  ( fshift != INDhASIS )
	    {
	    /* already shifted */

	    if  ( indINDgetUtf8( &accepted, ind, ind->ind_start,
					    shifted ) >= 0	&&
		  ( accepted || asPrefix )			)
		{ *pWhatWasShifted= fshift; rval= 0; goto ready;	}
	    }
	}

  ready:
    if  ( ucods )
	{ free( ucods );	}
    if  ( shifted )
	{ free( shifted );	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Generate guesses for a word.					*/
/*									*/
/************************************************************************/

int indWRDguess(	IND *				ind,
			unsigned short *		ucods,
			int				ulen,
			SpellGuessContext *		sgc )
    {
    int				rval= 0;

    int				tshift;
    int				hshift;
    int				fshift;
    int				tail;

    if  ( indGetCapModes( &tshift, &hshift, &fshift, &tail, ucods, ulen ) )
	{
	int		i;

	if  ( tshift != INDhASIS )
	    {
	    indINDguess( ind, ucods, ulen, sgc, tshift );
	    }

	if  ( hshift != INDhASIS )
	    {
	    for ( i= 0; i < tail; i++ )
		{ ucods[i]= ucdToLower( ucods[i] );	}

	    indINDguess( ind, ucods, ulen, sgc, hshift );
	    }

	if  ( fshift != INDhASIS )
	    {
	    indINDguess( ind, ucods, ulen, sgc, fshift );
	    }
	}
    else{
	indINDguess( ind, ucods, ulen, sgc, INDhASIS );
	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Shift an array unicodes to the correct case pattern.		*/
/*  Convert to UTF-8 in the mean time.					*/
/*									*/
/************************************************************************/

int indShiftWord(	char *				target,
			const unsigned short *		ucods,
			int				ulen,
			int				how )
    {
    int			from= 0;
    int			i;
    char *		to= target;

    switch( how )
	{
	case	INDhASIS:
	    break;

	case	INDhFIRSTUP:
	    if  ( ucdIsLl( ucods[0] ) )
		{
		int	step= uniPutUtf8( to, ucdToUpper( ucods[0] ) );

		if  ( step < 1 )
		    { LLDEB(ucods[0],step); return -1;	}

		to += step;
		from++;
		}
	    break;

	case	INDhIJUP:
	    if  ( ulen > 1 && ucods[0] == 'i' && ucods[1] == 'j' )
		{
		*(to++)= 'I'; *(to++)= 'J';
		from= 2;
		}
	    break;

	case	INDhALLUP:
	case	INDhTAILUP:
	case	INDhIJTAILUP:
	    for ( i= 0; i < ulen; i++ )
		{
		int	step= uniPutUtf8( to, ucdToUpper( ucods[i] ) );

		if  ( step < 1 )
		    { LLDEB(ucods[i],step); return -1;	}
		to += step;
		}
	    *to= '\0';
	    return 0;

	default:
	    LDEB(how); return -1;
	}

    for ( i= from; i < ulen; i++ )
	{
	int	step= uniPutUtf8( to, ucods[i] );

	if  ( step < 1 )
	    { LLDEB(ucods[i],step); return -1;	}
	to += step;
	}
    *to= '\0';

    return 0;
    }
