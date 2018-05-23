#   include	"indConfig.h"

#   include	<stdlib.h>

#   include	"indlocal.h"
#   include	<ucd.h>

#   include	<appDebugoff.h>

# define LOG_GUESSES	0

typedef struct GuessContext
    {
				/*
				 * The finite automaton that holds the 
				 * acceptable words.
				 */
    IND *			gcInd;
				/*
				 * Receives the guesses.
				 */
    unsigned short *		gcTarget;
				/*
				 * The word to be approximated and its length.
				 */
    const unsigned short *	gcSource;
    int				gcSourceLen;

				/*
				 * Tells how to capitalize the guesses before 
				 * they are added to the list of candidates.
				 */
    int				gcShiftHow;
    SpellGuessContext *		gcSpellGuessContext;

				/*
				 * Penalty for deleting an arbitrary character
				 */
    int				gcDelCost;
				/*
				 * Penalty for deleting a character that is 
				 * identical to the previous one in the input
				 */
    int				gcDel2Cost;
				/*
				 * Penalty for inserting an arbitrary character
				 */
    int				gcInsCost;
				/*
				 * Penalty for inserting the same character 
				 * before a character.
				 */
    int				gcIns2Cost;
				/*
				 * Penalty for substituting an arbitrary
				 * character
				 */
    int				gcSubstCost;
				/*
				 * Penalty for changing case of a character.
				 */
    int				gcSubstCaseCost;
				/*
				 * Penalty for changing an accent on a 
				 * character.
				 */
    int				gcSubstAccentCost;
				/*
				 * Penalty for swapping two input characters.
				 */
    int				gcSwapCost;
    } GuessContext;

/************************************************************************/
/*  Guess a series of strings that are stored on an automaton from an	*/
/*  approximation.							*/
/************************************************************************/

static int indSpelRememberGuess(	const GuessContext *	gc,
					int			len,
					int			score )
    {
    SpellGuessContext *		sgc= gc->gcSpellGuessContext;
    IndGuessList *		igl= sgc->sgcGuessList;
    SpellCheckContext *		scc= sgc->sgcCheckContext;

    int				accepted;
    char			copy[100];

    if  ( indShiftWord( copy, gc->gcTarget, len, gc->gcShiftHow ) )
	{ LDEB(how); return 0;	}

    if  ( scc->sccForgotInd					&&
	  indGetUtf8( &accepted, scc->sccForgotInd, copy ) >= 0	&&
	  accepted						)
	{ return 0;	}

    if  ( indAddGuess( igl, copy, score ) )
	{ SLDEB(copy,score); return -1;	}

    return 0;
    }

/************************************************************************/

static int indGuessAcceptToEndCost(	const GuessContext *	gc,
					int			to,
					int			from )
    {
    int		cost;

    if  ( from == gc->gcSourceLen )
	{ return 0;	}

    if  ( from > 0 && from == gc->gcSourceLen- 1	&&
	  gc->gcSource[from] == gc->gcSource[from-1]	)
	{ cost= gc->gcDel2Cost;					}
    else{ cost= gc->gcDelCost* ( gc->gcSourceLen- from );	}

    return cost;
    }

static int indGuessCanSkip(		const GuessContext *	gc,
					int *			pN,
					int *			pCost,
					int			to,
					int			from )
    {
    int			cost;
    int			n= 1;

    while( from+ n < gc->gcSourceLen			&&
	   gc->gcSource[from+ n] != gc->gcTarget[to]	)
	{ n++;	}

    if  ( from+ n < gc->gcSourceLen )
	{
	if  ( from > 0 &&  gc->gcSource[from- 1] ==  gc->gcSource[from] )
	    { cost= gc->gcDel2Cost;	}
	else{ cost= gc->gcDelCost;	}

	cost += gc->gcDelCost* ( n- 1 );

	*pN= n; *pCost= cost;
	return 1;
	}

    return 0;
    }

static int indGuessInsertCost(		const GuessContext *	gc,
					int			to,
					int			from )
    {
    if  ( from < gc->gcSourceLen			&&
	  gc->gcSource[from] == gc->gcTarget[to]	)
	{ return gc->gcIns2Cost;	}
    else{ return gc->gcInsCost;		}
    }

static int indGuessCanReplace(		const GuessContext *	gc,
					int *			pCost,
					int			to,
					int			from )
    {
    if  ( from < gc->gcSourceLen )
	{
	*pCost= gc->gcSubstCost;

	if  ( gc->gcSource[from] == ucdToUpper( gc->gcTarget[to] )	||
	      gc->gcSource[from] == ucdToLower( gc->gcTarget[to] )	)
	    { *pCost= gc->gcSubstCaseCost;	}

	if  ( ucdBaseCharacter( gc->gcSource[from] ) ==
				    ucdBaseCharacter( gc->gcTarget[to] ) )
	    { *pCost= gc->gcSubstAccentCost;	}

	return 1;
	}

    return 0;
    }

static int indGuessCanSwap(		const GuessContext *	gc,
					int *			pNext,
					int *			pCost,
					int			next,
					int			to,
					int			from )
    {
    if  ( from+ 1 < gc->gcSourceLen			&&
	  gc->gcSource[from+1] != gc->gcSource[from]	&& /* TRUE! */
	  gc->gcSource[from+1] == gc->gcTarget[to]	)
	{
	int		ignored;

	next= indINDstep( &ignored, gc->gcInd, next, gc->gcSource[from] );
	if  ( next >= 0 )
	    { *pCost= gc->gcSwapCost; *pNext= next; return 1;	}
	}

    return 0;
    }

/************************************************************************/

# if LOG_GUESSES

static void indGuessLogChar(	const GuessContext *		gc,
				int				key )
    {
    unsigned short	ucods[2];
    char		scratch[100];

    ucods[0]= key;

    if  ( indShiftWord( scratch, ucods, 1, INDhASIS ) )
	{ LDEB(1); return;	}

    appDebug( "%s", scratch );
    }

static void indGuessLogTo(	const char *			label,
				const GuessContext *		gc,
				int				to )
    {
    char		scratch[100];

    if  ( indShiftWord( scratch, gc->gcTarget, to, INDhASIS ) )
	{ LDEB(1); return;	}

    appDebug( "%s %s", label, scratch );
    }

static void indGuessLogFrom(	const char *			label,
				const GuessContext *		gc,
				int				to )
    {
    char		scratch[100];

    if  ( indShiftWord( scratch, gc->gcSource, to, INDhASIS ) )
	{ LDEB(1); return;	}

    appDebug( "%s %s", label, scratch );
    }

# endif

/************************************************************************/

static void indIndGuessForNode(	const GuessContext *		gc,
				int				tn,
				int				to,
				int				from,
				const int			score )
    {
    if  ( tn < 0 )
	{ LDEB(tn); return;	}

    if  ( score < 0 )
	{ return;	}

    while( tn >= 0 )
	{
	int			j;
	int			cost;
	int			next;
	const TrieNode *	node= NODE( gc->gcInd, tn );

	tn= -1;

#	if LOG_GUESSES
	indGuessLogTo( ">", gc, to );
	appDebug( ": (%d)\n", score );
#	endif

	/****************************************************************/
	/*  0) 	If the cost of removing letters (if any) at the end	*/
	/*	of the source is not too high, accept the result.	*/
	/*  1)  Remove letters from inside the source.			*/
	/*  2)  insert one letter. Repeating the next in the tail is	*/
	/*      less expensive.						*/
	/*  3)  replace one letter. Changing case is inexpensive.	*/
	/*  4)  Transposition: Note that the next is different follows	*/
	/*	from that it is equal to the key.			*/
	/****************************************************************/

	/*  0  */
	if  ( node->tn_flags & TNfACCEPTS )
	    {
	    cost= indGuessAcceptToEndCost( gc, to, from );

#	    if LOG_GUESSES
	    indGuessLogTo( "-", gc, to );
	    appDebug( ": ACCEPT? (%d- %d)\n", score, cost );
#	    endif

	    if  ( score >= cost )
		{ indSpelRememberGuess( gc, to, score- cost );	}
	    }

	for ( j= 0; j < (int)node->tn_ntrans; j++ )
	    {
	    TrieLink *	link= LINK(gc->gcInd,node->tn_transitions+j);
	    int		n;

	    gc->gcTarget[to]= link->tl_key;

#	    if LOG_GUESSES
	    indGuessLogTo( ".", gc, to );
	    appDebug( "|" );
	    indGuessLogChar( gc, link->tl_key );
	    appDebug( "\n" );
#	    endif

	    /*  1  */
	    if  ( indGuessCanSkip( gc, &n, &cost, to, from )	&&
		  score >= cost					)
		{
		indIndGuessForNode( gc, link->tl_to, to+ 1,
						    from+ 1+ n, score- cost );
		}

	    /*  2  */
	    cost= indGuessInsertCost( gc, to, from );
	    if  ( score >= cost )
		{
		indIndGuessForNode( gc, link->tl_to, to+ 1, from, score- cost );
		}

	    /*  3  */
	    if  ( from < gc->gcSourceLen		&&
		  gc->gcSource[from] == link->tl_key	)
		{
		tn= link->tl_to;

#		if  LOG_GUESSES
		indGuessLogTo( "=", gc, to+ 1 );
		appDebug( "\n" );
#		endif
		}
	    else{
		if  ( indGuessCanReplace( gc, &cost, to, from )	&&
		      score >= cost				)
		    {
		    indIndGuessForNode( gc, link->tl_to, to+ 1, from+ 1,
								score- cost );
		    }

		/*  4  */
		if  ( indGuessCanSwap( gc, &next, &cost, link->tl_to,
							    to, from )	&&
		      score >= cost					)
		    {
		    gc->gcTarget[to+ 1]= gc->gcSource[from];
		    indIndGuessForNode( gc, next, to+ 2, from+ 2, score- cost );
		    }
		}
	    }

	if  ( tn >= 0 )
	    { gc->gcTarget[to++]= gc->gcSource[from++];	}
	}

    return;
    }

int indINDguess(	IND *				ind,
			const unsigned short *		source,
			int				sourceLen,
			SpellGuessContext *		sgc,
			int				how )
    {
    int			upto= 2* sourceLen+ 2;

    GuessContext	gc;

    gc.gcInd= ind;
    gc.gcSource= source;
    gc.gcSourceLen= sourceLen;
    gc.gcShiftHow= how;
    gc.gcSpellGuessContext= sgc;

    gc.gcDelCost= 12;
    gc.gcDel2Cost= 3;
    gc.gcInsCost= 12;
    gc.gcIns2Cost= 3;
    gc.gcSubstCost= 10;
    gc.gcSubstCaseCost= 2;
    gc.gcSubstAccentCost= 2;
    gc.gcSwapCost= 10;

    gc.gcTarget= (unsigned short *)malloc( upto* sizeof(unsigned short) );
    if  ( ! gc.gcTarget )
	{ LXDEB(upto,target); return -1;	}

    if  ( ind->ind_start >= 0 )
	{
#	if  LOG_GUESSES
	indGuessLogFrom( "#", &gc, gc.gcSourceLen );
	appDebug( "\n" );
#	endif

	indIndGuessForNode( &gc, ind->ind_start,
					0, 0, ( 3* sourceLen )/ 2+ 10 );
	}

    if  ( gc.gcTarget )
	{ free( gc.gcTarget );	}

    return 0;
    }

int indCollectGuesses(		IndGuessList *		igl,
				SpellCheckContext *	scc,
				const char *		word )
    {
    SpellGuessContext		sgc;

    int				limit;

    sgc.sgcGuessList= igl;
    sgc.sgcCheckContext= scc;

    indCleanGuessList( igl );
    indInitGuessList( igl );

    if  ( scc->sccStaticInd )
	{ indGuessWord( scc->sccStaticInd, word, &sgc );	}

    if  ( scc->sccLearntInd )
	{ indGuess( scc->sccLearntInd, word, &sgc, INDhASIS );	}

    indSortGuesses( igl );

    if  ( igl->iglGuessCount > 6 )
	{
	limit= ( igl->iglGuesses[0].igsScore+
		    2* igl->iglGuesses[igl->iglGuessCount-1].igsScore )/ 3;
	}
    else{ limit= 0;	}

    return limit;
    }

/*********************/
