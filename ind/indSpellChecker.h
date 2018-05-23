/************************************************************************/
/*									*/
/*  Spell checking interface.						*/
/*									*/
/************************************************************************/

#   ifndef	IND_SPELL_CHECHER_H
#   define	IND_SPELL_CHECHER_H

#   include	"indGuessList.h"
#   include	<utilMemoryBuffer.h>

typedef struct SpellDictionary
    {
    char *		sdLocale;
    char *		sdLocaleLabel;
    MemoryBuffer	sdFileName;
    void *		sdSpellCheckContext;
    } SpellDictionary;

struct SpellChecker;

typedef void (*SpellComplain)(		int		error,
					const char *	subject,
					void *		through );

typedef int (*SpellCheckerSetup)(	struct SpellChecker * 	sc,
					SpellComplain		complain,
					void *			through );

typedef int (*SpellCheckerFindNext)(	void *			target,
					MemoryBuffer *		mbFound,
					const struct SpellChecker * 	sc,
					SpellDictionary *	sd );

typedef void (*SpellCheckerCorrect)(	void *			target,
					const char * 		guess );

typedef int (*SpellCheckerOpenDict)(	SpellDictionary *	sd,
					SpellComplain		complain,
					void *			through,
					const struct SpellChecker * sc );

typedef void (*SpellCheckerCleanDict)(	SpellDictionary *	sd );

typedef int (*SpellCheckerLearnWord)(	SpellDictionary *	sd,
					SpellComplain		complain,
					void *			through,
					const struct SpellChecker * sc,
					const char *		word );

typedef void (*SpellCheckerIgnoreWord)(	SpellDictionary *	sd,
					SpellComplain		complain,
					void *			through,
					const struct SpellChecker * sc,
					const char *		word );

typedef int (*SpellCheckerForgetWord)(	SpellDictionary *	sd,
					SpellComplain		complain,
					void *			through,
					const struct SpellChecker * sc,
					const char *		word );

typedef int (*SpellCheckerGuessWord)(	IndGuessList *		igl,
					SpellDictionary *	sd,
					SpellComplain		complain,
					void *			through,
					const struct SpellChecker * sc,
					const char *		word );

typedef struct SpellChecker
    {
    SpellCheckerSetup		scSetup;
    SpellCheckerOpenDict	scOpenDict;
    SpellCheckerCleanDict	scCleanDict;
    SpellCheckerFindNext	scFindNext;
    SpellCheckerCorrect		scCorrect;
    SpellCheckerLearnWord	scLearnWord;
    SpellCheckerForgetWord	scForgetWord;
    SpellCheckerIgnoreWord	scIgnoreWord;
    SpellCheckerGuessWord	scGuessWord;

    MemoryBuffer		scPrivateDirectory;
    MemoryBuffer		scSystemDirectory;

    void *			scTarget;

    SpellDictionary *		scDictionaries;
    int				scDictionaryCount;
    } SpellChecker;

typedef enum SpellCheckError
    {
    SCerrNO_ERROR= 0,
    SCerrDIR_NO_ACCESS,
    SCerrDICT_NO_ACCESS,
    SCerrDIR_DOES_NOT_EXIST,
    SCerrDIR_HAS_NO_DICTS,
    SCerrPRIVATE_DIR_NOT_MADE,
    SCerrPRIVATE_DICT_NO_ACCESS,
    SCerrPRIVATE_DICT_WRONG_FORMAT,

    SCerr_COUNT
    } SpellCheckError;

/************************************************************************/
/*									*/
/*  Subroutine declarations.						*/
/*									*/
/************************************************************************/

extern void indInitSpellDictionary(	SpellDictionary *	sd );
extern void indInitSpellChecker(	SpellChecker *		sc );
extern void indCleanSpellChecker(	SpellChecker *		sc );

extern int indSpellDictionarySetLocaleLabel(
					SpellDictionary *	sd,
					const char *		localeLabel );

#   endif	/* IND_SPELL_CHECHER_H */
