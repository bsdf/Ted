#   include	"indConfig.h"

#   include	<ucdGeneralCategory.h>
#   include	"indScanStream.h"
#   include	<sioGeneral.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Scan an input stream for unknown words. Optionally make guesses for	*/
/*  the unknown words.							*/
/*									*/
/*  1)  Skip inital white space and diacritical signs.			*/
/*  2)  End of stream.. ready.						*/
/*  3)  Collect the first character for the list of possibilities.	*/
/*  4)  Collect subsequent characters upto a space or diacritical.	*/
/*  5)  Count the possibilities in the list.				*/
/*  6)  If no possibilities are left.. it must be a misspelling.	*/
/*									*/
/************************************************************************/

int indScanStream(	SimpleInputStream *	sis,
			SpellCheckContext *	scc,
			void *			through )
    {
    int				rval= 0;

    SpellScanJob		ssj;
    SpellGuessContext		sgc;
    IndGuessList		igl;

    int				position= -1;
    int				acceptedPos= position;

    indInitSpellScanJob( &ssj );
    indInitGuessList( &igl );
    indInitSpellGuessContext( &sgc, &igl, scc );

    for (;;)
	{
	int		c;
	int		count;

	/*  1  */
	while( ( c= sioInGetByte( sis ) ) != EOF )
	    {
	    position++;
	    if  ( ucdIsL( c ) )
		{ break;	}
	    }

	/*  2  */
	if  ( c == EOF )
	    { goto ready;	}

	/*  3  */

	if  ( indNewPossibility( &ssj, position ) )
	    { LDEB(position); rval= -1; goto ready;	}
	indAddCharacterToPossibilities( &ssj, c );

	/*  4  */
	while( ( c= sioInGetByte( sis ) ) != EOF )
	    {
	    position++;
	    if  ( ! ucdIsL( c ) )
		{ break;	}

	    indAddCharacterToPossibilities( &ssj, c );
	    }

	/*  5  */
	count= indCountPossibilities( &ssj, scc, position, c == EOF );

	/*  6  */
	if  ( count == 0 )
	    {
	    PossibleWord *	maxpw;

	    maxpw= indMaximalPossibility( &ssj );

	    if  ( ! maxpw )
		{ XDEB(maxpw); rval= -1; goto ready;	}
	    }

	indRejectPossibilities( &acceptedPos, acceptedPos, &ssj );
	indAddCharacterToPossibilities( &ssj, c );
	}

  ready:

    indCleanGuessList( &igl );
    indCleanSpellGuessContext( &sgc );
    indCleanSpellScanJob( &ssj );

    return rval;
    }

