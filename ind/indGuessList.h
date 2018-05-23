#   ifndef	IND_GUESSS_LIST_H						/*  {{	*/
#   define	IND_GUESSS_LIST_H

/************************************************************************/
/*									*/
/*  A list of Guesses.							*/
/*									*/
/************************************************************************/

typedef struct IndGuessScore
    {
    unsigned char *	igsWord;
    int			igsScore;
    } IndGuessScore;

typedef struct IndGuessList
    {
    int			iglGuessCount;
    IndGuessScore *	iglGuesses;
    } IndGuessList;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int indAddGuess(		IndGuessList *		igl,
				const char *		word,
				int			score	);

extern void 	indCleanGuessList( IndGuessList *		igl );
extern void 	indEmptyGuessList( IndGuessList *		igl );
extern void 	indInitGuessList( IndGuessList *		igl );
extern void 	indSortGuesses(	IndGuessList *			igl );

#   endif /*	IND_GUESSS_LIST_H	    }}	*/
