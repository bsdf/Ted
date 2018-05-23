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
#   include	<appSystem.h>
#   include	<sioFileio.h>

#   include	"indSpellChecker.h"
#   include	"ind.h"

#   define FILEL 600
static const char * const	INDSPELL_DfaExtension= "dfa";

/************************************************************************/
/*									*/
/*  Obtain the list of dictionaries.					*/
/*									*/
/************************************************************************/

static int indSpellAddDictionary(	const MemoryBuffer *	filename,
					void *			voidsc )
    {
    int				rval= 0;
    SpellChecker *		sc= (SpellChecker *)voidsc;
    SpellCheckContext *		scc;
    SpellDictionary *		fresh;

    MemoryBuffer		relative;
    int				i;

    utilInitMemoryBuffer( &relative );

    for ( i= 0; i < sc->scDictionaryCount; i++ )
	{
	if  ( utilEqualMemoryBuffer( &(sc->scDictionaries[i].sdFileName),
								filename ) )
	    { goto ready;	}
	}

    if  ( appFileGetRelativeName( &relative, filename ) < 0 )
	{ LDEB(1); rval= -1; goto ready;	}

    fresh= (SpellDictionary *)realloc( sc->scDictionaries,
			(sc->scDictionaryCount+ 1)*sizeof(SpellDictionary) );
    if  ( ! fresh )
	{ XDEB(fresh); rval= -1; goto ready;	}
    sc->scDictionaries= fresh;

    fresh += sc->scDictionaryCount;

    indInitSpellDictionary( fresh );
    fresh->sdLocale= utilMemoryStrdup( &relative );
    fresh->sdLocaleLabel= utilMemoryStrdup( &relative );
    if  ( utilCopyMemoryBuffer( &(fresh->sdFileName), filename ) )
	{ LDEB(1);	}

    {
    int	rl= relative.mbSize;

    fresh->sdLocale[rl- strlen(INDSPELL_DfaExtension)-1]='\0';
    fresh->sdLocaleLabel[rl- strlen(INDSPELL_DfaExtension)-1]='\0';
    }

    fresh->sdSpellCheckContext= scc= (SpellCheckContext *)malloc(
						sizeof(SpellCheckContext) );
    indInitSpellCheckContext( scc );
    if  ( ! scc )
	{ XDEB(scc); rval= -1; goto ready;	}

    sc->scDictionaryCount++;

  ready:

    utilCleanMemoryBuffer( &relative );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Build the name of a private dictionary.				*/
/*									*/
/************************************************************************/

static int indSpellPrivateDictionaryName(
				    MemoryBuffer *		dictName,
				    const char *		locale,
				    const MemoryBuffer *	privateDicts,
				    int				readOnly,
				    SpellComplain		complain,
				    void *			through )
    {
    int			rval= 0;
    const char *	suffix= ".changes";

    MemoryBuffer	relative;
    MemoryBuffer	home;
    MemoryBuffer	dicts;

    utilInitMemoryBuffer( &relative );
    utilInitMemoryBuffer( &home );
    utilInitMemoryBuffer( &dicts );

    if  ( appHomeDirectory( &home ) < 0 )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( appAbsoluteName( &dicts, privateDicts, 0, &home ) < 0 )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( appTestDirectory( &dicts ) )
	{
	if  ( readOnly )
	    { rval= 1; goto ready;	}

	if  ( appMakeDirectory( &dicts ) )
	    {
	    (*complain)( SCerrPRIVATE_DIR_NOT_MADE,
				utilMemoryBufferGetString( &dicts ), through );
	    rval= 1; goto ready;
	    }
	}

    if  ( utilMemoryBufferSetString( &relative, locale ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( utilMemoryBufferAppendBytes( &relative,
		    (const unsigned char *)suffix, strlen( suffix ) ) )
	{ LDEB(1); return -1;	}

    if  ( appAbsoluteName( dictName, &relative, 0, &dicts ) < 0 )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    utilCleanMemoryBuffer( &relative );
    utilCleanMemoryBuffer( &home );
    utilCleanMemoryBuffer( &dicts );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Open index files for a certain dictionary.				*/
/*									*/
/************************************************************************/

static int indSpellOpenDictionary(	SpellDictionary *	sd,
					SpellComplain		complain,
					void *			through,
					const SpellChecker *	sc )
    {
    int			rval= 0;
    int			ret;
    SpellCheckContext *	scc= (SpellCheckContext *)sd->sdSpellCheckContext;
    SimpleInputStream *	sis= (SimpleInputStream *)0;
    const char *	sdf;

    const int		readOnly= 1;

    MemoryBuffer	dictName;

    utilInitMemoryBuffer( &dictName );

    if  ( ! scc )
	{ XDEB(scc); rval= -1; goto ready;	}
    if  ( scc->sccStaticInd )
	{ goto ready;	}

    sdf= utilMemoryBufferGetString( &(sd->sdFileName) );
    scc->sccStaticInd= indRead( sdf, 1 );
    if  ( ! scc->sccStaticInd )
	{
	(*complain)( SCerrDICT_NO_ACCESS, sdf, through );
	rval= -1; goto ready;
	}

    ret= indSpellPrivateDictionaryName( &dictName, sd->sdLocale,
		    &(sc->scPrivateDirectory), readOnly, complain, through );
    if  ( ret < 0 )
	{ LDEB(ret); rval= -1; goto ready;	}
    if  ( ret > 0 )
	{ goto ready;				}

    sis= sioInFileioOpen( &dictName );
    if  ( sis )
	{
	if  ( indReadPrivateDictionary( sis,
				&(scc->sccLearntInd), &(scc->sccForgotInd) ) )
	    {
	    (*complain)( SCerrPRIVATE_DICT_WRONG_FORMAT,
						    sd->sdLocale, through );
	    }
	}

  ready:

    if  ( sis )
	{ sioInClose( sis );	}

    utilCleanMemoryBuffer( &dictName );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Learn/Forget/Ignore a word.						*/
/*									*/
/************************************************************************/

static int indSpellLearnWord(	SpellDictionary *	sd,
				SpellComplain		complain,
				void *			through,
				const SpellChecker *	sc,
				const char *		word )
    {
    int			rval= 0;
    int			ret;
    SpellCheckContext *	scc= (SpellCheckContext *)sd->sdSpellCheckContext;
    SimpleOutputStream * sos= (SimpleOutputStream *)0;

    const int		readOnly= 0;

    MemoryBuffer	dictName;

    utilInitMemoryBuffer( &dictName );

    if  ( ! scc )
	{ XDEB(scc); goto ready;	}

    if  ( ! scc->sccStaticInd					&&
	  indSpellOpenDictionary( sd, complain, through, sc )	)
	{ rval= -1; goto ready;	}

    if  ( ! scc->sccLearntInd )
	{
	scc->sccLearntInd= indMake();
	if  ( ! scc->sccLearntInd )
	    { XDEB(scc->sccLearntInd); rval= -1; goto ready;	}
	}

    ret= indSpellPrivateDictionaryName( &dictName, sd->sdLocale,
		    &(sc->scPrivateDirectory), readOnly, complain, through );
    if  ( ret < 0 )
	{ LDEB(ret); rval= -1; goto ready;	}
    if  ( ret > 0 )
	{ goto ready;				}

    sos= sioOutFileioOpenForAppend( &dictName );
    if  ( ! sos )
	{ rval= -1; goto ready;	}

    if  ( indLearnWord( sos, scc->sccLearntInd, scc->sccForgotInd, word ) )
	{ SDEB((char *)word); rval= -1; goto ready;	}

  ready:

    if  ( sos )
	{ sioOutClose( sos );	}

    utilCleanMemoryBuffer( &dictName );

    return rval;
    }

static int indSpellForgetWord(	SpellDictionary *	sd,
				SpellComplain		complain,
				void *			through,
				const SpellChecker *	sc,
				const char *		word )
    {
    int			rval= 0;
    int			ret;
    SpellCheckContext *	scc= (SpellCheckContext *)sd->sdSpellCheckContext;
    SimpleOutputStream * sos= (SimpleOutputStream *)0;

    const int		readOnly= 0;

    MemoryBuffer	dictName;

    utilInitMemoryBuffer( &dictName );

    if  ( ! scc )
	{ XDEB(scc); goto ready;	}

    if  ( ! scc->sccStaticInd					&&
	  indSpellOpenDictionary( sd, complain, through, sc )	)
	{ rval= -1; goto ready;	}

    if  ( ! scc->sccForgotInd )
	{
	scc->sccForgotInd= indMake();
	if  ( ! scc->sccForgotInd )
	    { XDEB(scc->sccForgotInd); rval= -1; goto ready;	}
	}

    ret= indSpellPrivateDictionaryName( &dictName, sd->sdLocale,
		    &(sc->scPrivateDirectory), readOnly, complain, through );
    if  ( ret < 0 )
	{ LDEB(ret); rval= -1; goto ready;	}
    if  ( ret > 0 )
	{ goto ready;				}

    sos= sioOutFileioOpen( &dictName );
    if  ( ! sos )
	{ rval= -1; goto ready;	}

    if  ( indForgetWord( sos, scc->sccLearntInd, scc->sccForgotInd, word ) )
	{ SDEB((char *)word);	}

  ready:

    if  ( sos )
	{ sioOutClose( sos );	}

    utilCleanMemoryBuffer( &dictName );

    return rval;
    }

static void indSpellIgnoreWord(	SpellDictionary *	sd,
				SpellComplain		complain,
				void *			through,
				const SpellChecker *	sc,
				const char *		word )
    {
    SpellCheckContext *	scc= (SpellCheckContext *)sd->sdSpellCheckContext;

    if  ( ! scc )
	{ XDEB(scc); return;	}

    if  ( ! scc->sccLearntInd )
	{
	scc->sccLearntInd= indMake();
	if  ( ! scc->sccLearntInd )
	    { XDEB(scc->sccLearntInd); return;	}
	}

    if  ( indMoveWord( scc->sccForgotInd, scc->sccLearntInd, word ) )
	{ SDEB((char *)word);	}

    return;
    }

/************************************************************************/
/*									*/
/*  Make guesses.							*/
/*									*/
/************************************************************************/

static int indSpellGuess(	IndGuessList *		igl,
				SpellDictionary *	sd,
				SpellComplain		complain,
				void *			through,
				const SpellChecker * 	sc,
				const char *		word )
    {
    int			count= 0;
    int			limit;
    SpellCheckContext *	scc= (SpellCheckContext *)sd->sdSpellCheckContext;

    if  ( ! scc )
	{ XDEB(scc); return -1;	}

    if  ( ! scc->sccStaticInd					&&
	  indSpellOpenDictionary( sd, complain, through, sc )	)
	{ return -1;	}

    limit= indCollectGuesses( igl, scc, word );

    for ( count= 0; count < igl->iglGuessCount; count++ )
	{
	if  ( igl->iglGuesses[count].igsScore < limit )
	    { break;	}
	}

    return count;
    }

/************************************************************************/
/*									*/
/*  Clean a SpellChecker						*/
/*									*/
/************************************************************************/

static void indSpellCleanDict(	SpellDictionary *	sd )
    {
    SpellCheckContext *	scc= (SpellCheckContext *)sd->sdSpellCheckContext;

    if  ( scc )
	{
	indCleanSpellCheckContext( scc );
	free( scc );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Configure a spell checker.						*/
/*									*/
/************************************************************************/

int indSpellSetup(		SpellChecker *	sc,
				SpellComplain	complain,
				void *		through )
    {
    if  ( appForAllFiles( &(sc->scSystemDirectory), INDSPELL_DfaExtension,
					(void *)sc, indSpellAddDictionary ) )
	{ LDEB(1); return -1;	}

    if  ( sc->scDictionaryCount == 0 )
	{
	const char * dn= utilMemoryBufferGetString( &(sc->scSystemDirectory) );

	if  ( appTestDirectory( &(sc->scSystemDirectory) ) )
	    { (*complain)( SCerrDIR_DOES_NOT_EXIST, dn, through );	}
	else{
	    if  ( appTestFileReadable( &(sc->scSystemDirectory) ) )
		{ (*complain)( SCerrDIR_NO_ACCESS, dn, through );	}
	    else{ (*complain)( SCerrDIR_HAS_NO_DICTS, dn, through );	}
	    }

	return 1;
	}

    sc->scOpenDict= indSpellOpenDictionary;
    sc->scCleanDict= indSpellCleanDict;
    sc->scLearnWord= indSpellLearnWord;
    sc->scForgetWord= indSpellForgetWord;
    sc->scIgnoreWord= indSpellIgnoreWord;
    sc->scGuessWord= indSpellGuess;

    return 0;
    }

