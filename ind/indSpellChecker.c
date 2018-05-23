/************************************************************************/
/*									*/
/*  Obtain the list of dictionaries.					*/
/*									*/
/*  The initialistation of the character classification should be	*/
/*  based on some kind of knowledge, NOT on the name of the language.	*/
/*									*/
/************************************************************************/

#   include	<string.h>
#   include	<stdlib.h>

#   include	<appDebugon.h>

#   include	"indSpellChecker.h"

/************************************************************************/

void indInitSpellDictionary(	SpellDictionary *	sd )
    {
    sd->sdLocale= (char *)0;
    sd->sdLocaleLabel= (char *)0;
    utilInitMemoryBuffer( &(sd->sdFileName) );

    sd->sdSpellCheckContext= (void *)0;
    }

void indInitSpellChecker(	SpellChecker *		sc )
    {
    sc->scSetup= (SpellCheckerSetup)0;
    sc->scOpenDict= (SpellCheckerOpenDict)0;
    sc->scCleanDict= (SpellCheckerCleanDict)0;
    sc->scFindNext= (SpellCheckerFindNext)0;
    sc->scCorrect= (SpellCheckerCorrect)0;
    sc->scLearnWord= (SpellCheckerLearnWord)0;
    sc->scForgetWord= (SpellCheckerForgetWord)0;
    sc->scIgnoreWord= (SpellCheckerIgnoreWord)0;
    sc->scGuessWord= (SpellCheckerGuessWord)0;

    utilInitMemoryBuffer( &(sc->scPrivateDirectory) );
    utilInitMemoryBuffer( &(sc->scSystemDirectory) );

    sc->scTarget= (void *)0;

    sc->scDictionaries= (SpellDictionary *)0;
    sc->scDictionaryCount= 0;
    }

void indCleanSpellChecker(	SpellChecker *		sc )
    {
    int			i;
    SpellDictionary *	sd;

    sd= sc->scDictionaries;
    for ( i= 0; i < sc->scDictionaryCount; sd++, i++ )
	{
	if  ( sd->sdSpellCheckContext && sc->scCleanDict )
	    { (*sc->scCleanDict)( sd );	}

	if  ( sd->sdLocale )
	    { free( sd->sdLocale );	}
	if  ( sd->sdLocaleLabel )
	    { free( sd->sdLocaleLabel );	}
	utilCleanMemoryBuffer( &(sd->sdFileName) );
	}

    if  ( sc->scDictionaries )
	{ free( sc->scDictionaries );	}

    utilCleanMemoryBuffer( &(sc->scPrivateDirectory) );
    utilCleanMemoryBuffer( &(sc->scSystemDirectory) );

    return;
    }

int indSpellDictionarySetLocaleLabel(	SpellDictionary *	sd,
					const char *		localeLabel )
    {
    char * ll= (char *)0;

    if  ( localeLabel )
	{
	ll= strdup( localeLabel );
	if  ( ! ll )
	    { XDEB(ll); return -1;	}
	}

    if  ( sd->sdLocaleLabel )
	{ free( sd->sdLocaleLabel );	}

    sd->sdLocaleLabel= ll;

    return 0;
    }
