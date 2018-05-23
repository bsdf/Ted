#   ifndef	IND_H						/*  {{	*/
#   define	IND_H

#   include	<sioGeneral.h>
#   include	"indSpellChecker.h"

/************************************************************************/
/*									*/
/*  A possible 'word' that is collected during the scan of the input.	*/
/*									*/
/************************************************************************/
#   define	FORM_MAX	60

typedef struct PossibleWord
    {
    int				pwStartAt;
    int				pwInsertionPoint;
    int				pwRejectedAt;
    int				pwAcceptedAt;
    char			pwForm[FORM_MAX+2];

    struct PossibleWord *	pwNext;
    } PossibleWord;

typedef struct SpellScanJob
    {
    PossibleWord *	ssjPossibleWords;
    int			ssjPossibleWordCount;
    } SpellScanJob;

/************************************************************************/
/*									*/
/*  Operating environment for a checker.				*/
/*									*/
/************************************************************************/

typedef struct SpellCheckContext
    {
    const char *	sccDictionaryPrefix;
    void *		sccStaticInd;
    void *		sccForgotInd;
    void *		sccLearntInd;
    } SpellCheckContext;

typedef struct SpellGuessContext
    {
    IndGuessList *	sgcGuessList;
    SpellCheckContext *	sgcCheckContext;
    } SpellGuessContext;

/************************************************************************/

typedef int (*IndForAllFun)(	void *		through,
				int		dir,
				int		tnFrom,
				int		tnTo,
				unsigned short	key,
				int		toAccepts );

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void *	indMake( void );
extern void *	indRead( const char * filename, int readOnly );

extern int indPutUtf8(		void *			ind,
				const char *		key );

extern int indPutUtf16(		void *			ind,
				const unsigned short *	key );

extern int indPutSuffixUtf16(	void *			voidind,
				const unsigned short *	key );

extern int	indForget( void * ind, const char * key );
extern int	indGetUtf8( int *, void * ind, const char * key );
extern int	indGetUtf16( int *, void * ind, const unsigned short * key );
extern void	indFree( void * ind );
extern int	indWrite( void * ind, const char * filename );
extern void *	indMini( void * ind );
extern void *	indRenumber( void * ind );

extern int indGuess(	void *				voidind,
			const char *			word,
			SpellGuessContext *		sgc,
			int				how );

extern int	indGetWord(	int *			pWhatWasShifted,
				void *			voidind,
				const char *		word,
				int			asPrefix );

extern int indGuessWord(    void *				voidind,
			    const char *			word,
			    SpellGuessContext *			sgc );

extern void	indLogPossibilities(	SpellScanJob *	ssj	);

extern int	indNewPossibility(	SpellScanJob *	ssj,
					int		position );

extern void indAddCharacterToPossibilities(
					SpellScanJob *		ssj,
					int			c );

extern int indCountPossibilities(	SpellScanJob *		ssj,
					SpellCheckContext *	scc,
					int			position,
					int			rejectPrefices );

extern void indRejectPossibilities(	int *			pAcceptedPos,
					int			acceptedPos,
					SpellScanJob *		ssj	);

extern PossibleWord * indMaximalPossibility(	SpellScanJob *	ssj	);

extern int	indMoveWord(	void *			fromInd,
				void *			toInd,
				const char *		word );

extern int indReadPrivateDictionary(	SimpleInputStream *	sis,
					void **			pLearntInd,
					void **			pForgotInd );

extern int indLearnWord(	SimpleOutputStream *	sos,
				void *			learntInd,
				void *			forgotInd,
				const char *		word );

extern int indForgetWord(	SimpleOutputStream *	sos,
				void *			learntInd,
				void *			forgotInd,
				const char *		word );

extern void indInitSpellScanJob(	SpellScanJob *	ssj );
extern void indCleanSpellScanJob(	SpellScanJob *	ssj );

extern void indInitSpellGuessContext(	SpellGuessContext *	sgc,
					IndGuessList *		igl,
					SpellCheckContext *	scc );
extern void indCleanSpellGuessContext(	SpellGuessContext *	sgc );

extern void indInitSpellCheckContext(  SpellCheckContext *     scc );
extern void indCleanSpellCheckContext(  SpellCheckContext *     scc );

extern void indSpellIso1CharacterKinds(	SpellCheckContext *	scc );
extern void indSpellIso2CharacterKinds(	SpellCheckContext *	scc );
extern void indSpellIso7CharacterKinds(	SpellCheckContext *	scc );
extern void indSpellIso5CharacterKinds(	SpellCheckContext *	scc );
extern void indSpellKoi8rCharacterKinds( SpellCheckContext *	scc );

extern int indCollectGuesses(		IndGuessList *		igl,
					SpellCheckContext *	scc,
					const char *		word );


extern int indSpellSetup(		SpellChecker *		sc,
					SpellComplain		complain,
					void *			through );

extern void indDump(			void *			voidind );

extern int indAddSuffixUtf16(	void *			voidind,
				const unsigned short *	prefix,
				int			suffixNumber );

#   endif /*	IND_H						    }}	*/
