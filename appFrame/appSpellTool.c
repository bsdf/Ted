/************************************************************************/
/*									*/
/*  A spelling tool.							*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<stddef.h>
#   include	<string.h>
#   include	<appDebugon.h>

#   include	"appSpellTool.h"
#   include	"appFrame.h"
#   include	"appQuestion.h"
#   include	"guiToolUtil.h"

/************************************************************************/
/*									*/
/*  Resources for the spell tool.					*/
/*									*/
/************************************************************************/

/* From ../tedPackage/languages.txt */
static LocaleName AppSpellToolLocaleNames[]=
{
    { "en_US", "American English",		},

    { "nl_NL", "Dutch",				},
    { "en_GB", "British English",		},
    { "de_DE", "German",			},
    { "es_ES", "Spanish",			},
    { "es_AR", "Argentinian Spanish",		},
    { "pt_PT", "Portuguese",			},
    { "pt_BR", "Brazilian Portuguese",		},
    { "fr_FR", "French",			},
    { "it_IT", "Italian",			},
    { "cs_CZ", "Czech",				},
    { "da_DK", "Danish",			},
    { "sv_SE", "Swedish",			},
    { "no_NO", "Norwegian",			},
    { "pl_PL", "Polish",			},
    { "ru_RU", "Russian",			},
    { "sk_SK", "Slovak",			},
    { "sl_SI", "Slovene",			},
    { "hu_HU", "Hungarian",			},
    { "mg_MG", "Malagasi",			},
    { "oc_FR", "Occitan",			},
    { "eu_FR", "Basque",			},
    { "bg_BG", "Bulgarian",			},
    { "hr_HR", "Croatian",			},
    { "et_EE", "Estonian",			},
    { "fi_FI", "Finnish",			},
};

/************************************************************************/
/*									*/
/*  Adapt buttons to different situations..				*/
/*									*/
/************************************************************************/

static void appSpellToolSomethingFound(	SpellTool *	ast,
					int		yes_no )
    {
    guiEnableWidget( ast->astLearnButton, yes_no );
    guiEnableWidget( ast->astIgnoreButton, yes_no );

    return;
    }

static void appSpellToolGotAlternative(	SpellTool *	ast,
					int		gotAlternative )
    {
    ast->astGotAlternative= gotAlternative;

    guiEnableWidget( ast->astForgetButton, ast->astGotAlternative );
    guiEnableWidget( ast->astCorrectButton,
			    ast->astGotAlternative && ! ast->astReadOnly );
    guiEnableWidget( ast->astGuessButton, ast->astGotAlternative );

    return;
    }

/************************************************************************/
/*									*/
/*  Complain								*/
/*									*/
/************************************************************************/

static void appSpellToolComplain(	int			error,
					const char *		subject,
					void *			voidast )
    {
    SpellTool *			ast= (SpellTool *)voidast;
    const char *		message= "?";
    const SpellToolResources *	astr= ast->astResources;
    APP_WIDGET			topWidget= ast->astInspector->aiTopWidget;

    if  ( error > 0 && error < SCerr_COUNT )
	{ message= astr->astrErrors[error]; }

    if  ( subject )
	{
	appQuestionRunErrorDialog( ast->astApplication,
				topWidget, (APP_WIDGET)0, message );
	}
    else{
	appQuestionRunSubjectErrorDialog( ast->astApplication,
				topWidget, (APP_WIDGET)0, subject, message );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Guess what was intended for word.					*/
/*									*/
/************************************************************************/

static void appSpellMakeGuesses(	void *		voidast,
					const char *	word )
    {
    SpellTool *			ast= (SpellTool *)voidast;
    SpellChecker *		sc;
    SpellDictionary *		sd;

    int				count;
    int				i;
    static IndGuessList		igl;

    if  ( ast->astCurrentDictionary < 0 )
	{ LDEB(ast->astCurrentDictionary); return;	}
    if  ( ! ( sc= ast->astSpellChecker ) )
	{ XDEB(ast->astSpellChecker); return;	}
    if  ( ! sc->scGuessWord )
	{ XDEB(sc->scGuessWord); return;		}

    sd= sc->scDictionaries+ ast->astCurrentDictionary;

    count= (*sc->scGuessWord)( &igl, sd,
				appSpellToolComplain, (void *)ast, sc, word );
    if  ( count < 0 )
	{ LDEB(count); return;	}

    appGuiEmptyListWidget( ast->astGuessList );
    appStringToTextWidget( ast->astWordText, (const char *)word );

    for ( i= 0; i < count; i++ )
	{
	appGuiAddValueToListWidget( ast->astGuessList, -1,
					(char *)igl.iglGuesses[i].igsWord );
	}

    appSpellToolGotAlternative( ast, 0 );
    }

void appSpellToolSetReadOnly(	SpellTool *		ast,
				int			readOnly )
    {
    ast->astReadOnly= readOnly;

    guiEnableWidget( ast->astCorrectButton,
				ast->astGotAlternative && ! ast->astReadOnly );

    /* Not needed, but not doing this confuses: */
    guiEnableWidget( ast->astGuessList, ! ast->astReadOnly );
    guiEnableText( ast->astWordText, ! ast->astReadOnly );

    return;
    }

/************************************************************************/
/*									*/
/*  Look for the next unknown word.					*/
/*									*/
/************************************************************************/

static void appSpellToolFindNext(	SpellTool *	ast )
    {
    SpellChecker *		sc;
    SpellDictionary *		sd;
    MemoryBuffer		mbGuess;

    utilInitMemoryBuffer( &mbGuess );

    if  ( ! ( sc= ast->astSpellChecker ) )
	{ XDEB(ast->astSpellChecker); return;	}

    if  ( ast->astCurrentDictionary < 0				||
	  ast->astCurrentDictionary >= sc->scDictionaryCount	)
	{ LLDEB(ast->astCurrentDictionary,sc->scDictionaryCount); return; }

    sd= sc->scDictionaries+ ast->astCurrentDictionary;

    appStringToTextWidget( ast->astWordText, "" );
    appGuiEmptyListWidget( ast->astGuessList );

    if  ( (*sc->scOpenDict)( sd, appSpellToolComplain, (void *)ast, sc ) )
	{ return;	}

    if  ( ! (*sc->scFindNext)( ast->astApplication, &mbGuess, sc, sd ) )
	{
	appSpellToolSomethingFound( ast, 1 );
	appSpellMakeGuesses( ast, utilMemoryBufferGetString( &mbGuess ) );
	}
    else{ appSpellToolSomethingFound( ast, 0 );	}

    appSpellToolGotAlternative( ast, 0 );

    utilCleanMemoryBuffer( &mbGuess );
    }

/************************************************************************/
/*									*/
/*  'Learn/Forget/Ignore' button has been pushed.			*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( appSpellToolLearnPushed, w, voidast )
    {
    SpellTool *		ast= (SpellTool *)voidast;
    SpellChecker *		sc;
    SpellDictionary *		sd;
    char *			word;

    if  ( ast->astCurrentDictionary < 0 )
	{ LDEB(ast->astCurrentDictionary); return;	}
    if  ( ! ( sc= ast->astSpellChecker ) )
	{ XDEB(ast->astSpellChecker); return;	}
    if  ( ! sc->scLearnWord )
	{ XDEB(sc->scLearnWord); return;		}

    sd= sc->scDictionaries+ ast->astCurrentDictionary;

    word= appGetStringFromTextWidget( ast->astWordText );

    (*sc->scLearnWord)( sd, appSpellToolComplain, (void *)ast, sc, word );

    appFreeStringFromTextWidget( word );

    appSpellToolFindNext( ast );

    return;
    }

static APP_BUTTON_CALLBACK_H( appSpellToolForgetPushed, w, voidast )
    {
    SpellTool *		ast= (SpellTool *)voidast;
    SpellChecker *		sc;
    SpellDictionary *		sd;
    char *			word;

    if  ( ast->astCurrentDictionary < 0 )
	{ LDEB(ast->astCurrentDictionary); return;	}
    if  ( ! ( sc= ast->astSpellChecker ) )
	{ XDEB(ast->astSpellChecker); return;	}
    if  ( ! sc->scForgetWord )
	{ XDEB(sc->scForgetWord); return;		}

    sd= sc->scDictionaries+ ast->astCurrentDictionary;

    word= appGetStringFromTextWidget( ast->astWordText );

    (*sc->scForgetWord)( sd, appSpellToolComplain, (void *)ast, sc, word );

    appFreeStringFromTextWidget( word );

    guiEnableWidget( ast->astForgetButton, 0 );
    guiEnableWidget( ast->astLearnButton, 1 );

    return;
    }

static APP_BUTTON_CALLBACK_H( appSpellToolIgnorePushed, w, voidast )
    {
    SpellTool *		ast= (SpellTool *)voidast;
    SpellChecker *		sc;
    SpellDictionary *		sd;
    char *			word;

    if  ( ast->astCurrentDictionary < 0 )
	{ LDEB(ast->astCurrentDictionary); return;	}
    if  ( ! ( sc= ast->astSpellChecker ) )
	{ XDEB(ast->astSpellChecker); return;	}
    if  ( ! sc->scIgnoreWord )
	{ XDEB(sc->scIgnoreWord); return;		}

    sd= sc->scDictionaries+ ast->astCurrentDictionary;

    word= appGetStringFromTextWidget( ast->astWordText );

    (*sc->scIgnoreWord)( sd, appSpellToolComplain, (void *)ast, sc, word );

    appFreeStringFromTextWidget( word );

    appSpellToolFindNext( ast );

    return;
    }

/************************************************************************/
/*									*/
/*  'Find Next' button has been pushed.					*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( appSpellToolFindNextPushed, w, voidast )
    {
    SpellTool *		ast= (SpellTool *)voidast;

    appSpellToolFindNext( ast );

    return;
    }

/************************************************************************/
/*									*/
/*  The user typed something in the Coreection, Turn on the the		*/
/*  'Correct' button.							*/
/*									*/
/************************************************************************/

static APP_TXTYPING_CALLBACK_H( appSpellCorrectionTyped, w, voidast )
    {
    SpellTool *	ast= (SpellTool *)voidast;

    appSpellToolGotAlternative( ast, 1 );

    return;
    }

/************************************************************************/
/*									*/
/*  'Correct' button has been pushed.					*/
/*  Or a double click on the listbox with guesses.			*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( appSpellToolCorrect, w, voidast )
    {
    SpellTool *	ast= (SpellTool *)voidast;
    SpellChecker *	sc;
    char *		guess;

    if  ( ast->astReadOnly )
	{ return;	}
    if  ( ! ( sc= ast->astSpellChecker ) )
	{ XDEB(ast->astSpellChecker); return;	}
    if  ( ! sc->scCorrect || ast->astReadOnly )
	{ XLDEB(sc->scCorrect,ast->astReadOnly); return;	}

    guess= appGetStringFromTextWidget( ast->astWordText );

    (*sc->scCorrect)( ast->astApplication, guess );

    appFreeStringFromTextWidget( guess );

    appSpellToolFindNext( ast );

    return;
    }

/************************************************************************/
/*									*/
/*  The 'Guess' button has been pushed.					*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( appSpellGuessButtonPushed, w, voidast )
    {
    SpellTool *	ast= (SpellTool *)voidast;
    char *		word;

    word= appGetStringFromTextWidget( ast->astWordText );

    appSpellMakeGuesses( voidast, word );

    appFreeStringFromTextWidget( word );

    return;
    }

/************************************************************************/
/*  A guess in the list has been selected.				*/
/************************************************************************/

static APP_LIST_CALLBACK_H( appSpellGuessChosen, w, voidast, voidlcs )
    {
    SpellTool *		ast= (SpellTool *)voidast;

    char *			text;

    text= appGuiGetStringFromListCallback( w, voidlcs );
    if  ( text )
	{
	appStringToTextWidget( ast->astWordText, text );
	appFreeStringFromListCallback( text );
	}

    appSpellToolGotAlternative( ast, 1 );

    return;
    }

/************************************************************************/
/*									*/
/*  A dictionary has been selected.					*/
/*									*/
/************************************************************************/

static void appSpellDictionaryChosen(	int		dict,
					void *		voidast )
    {
    SpellTool *		ast= (SpellTool *)voidast;
    SpellChecker *	sc;

    if  ( ! ( sc= ast->astSpellChecker ) )
	{ XDEB(ast->astSpellChecker); return;	}

    if  ( dict < 0 || dict >= sc->scDictionaryCount )
	{ LLDEB(dict,sc->scDictionaryCount); return;	}

    ast->astCurrentDictionary= dict;

    return;
    }

/************************************************************************/
/*									*/
/*  Make the dictionary part of the spelling tool.			*/
/*									*/
/************************************************************************/

static APP_WIDGET appSpellMakeDictionaryFrame(
					APP_WIDGET			parent,
					const SpellToolResources *	astr,
					SpellTool *			ast )
    {
    APP_WIDGET		frame;
    APP_WIDGET		paned;

    APP_WIDGET		buttonRow;

    appMakeColumnFrameInColumn( &frame, &paned,
					    parent, astr->astrDictionary );

    appMakeOptionmenuInColumn( &(ast->astDictionaryOptionmenu), paned,
				    appSpellDictionaryChosen, (void *)ast );

    guiToolMake2BottonRow( &buttonRow, paned,
		&(ast->astLearnButton), &(ast->astForgetButton),
		astr->astrLearn, astr->astrForget,
		appSpellToolLearnPushed, appSpellToolForgetPushed,
		(void *)ast );

    return frame;
    }

/************************************************************************/
/*									*/
/*  Fill the list of dictionaries.					*/
/*									*/
/************************************************************************/

static void appSpellFillDictionaryMenu(	SpellTool *			ast,
					const SpellToolResources *	astr )
    {
    SpellChecker *	sc;
    SpellDictionary *	sd;

    int			dict;
    int			defaultDict= -1;

    if  ( ! ( sc= ast->astSpellChecker ) )
	{ XDEB(ast->astSpellChecker); return;	}

    appEmptyOptionmenu( &(ast->astDictionaryOptionmenu) );

    sd= sc->scDictionaries;
    for ( dict= 0; dict < sc->scDictionaryCount; sd++, dict++ )
	{
	appAddItemToOptionmenu(
		    &(ast->astDictionaryOptionmenu), sd->sdLocaleLabel );

	if  ( ast->astApplication					&&
	      ast->astApplication->eaLocaleName				&&
	      ! strcmp( sd->sdLocale, ast->astApplication->eaLocaleName ) )
	    { defaultDict= dict;	}
	}

    if  ( sc->scDictionaryCount == 0 )
	{
	(void) appAddItemToOptionmenu(
		    &(ast->astDictionaryOptionmenu), astr->astrNoDicts );
	}

    if  ( defaultDict < 0 )
	{ defaultDict= 0;	}

    appSetOptionmenu( &(ast->astDictionaryOptionmenu), defaultDict );
    ast->astCurrentDictionary= defaultDict;

    appOptionmenuRefreshWidth( &(ast->astDictionaryOptionmenu) );
    }

/************************************************************************/
/*									*/
/*  Make the listbox with guesses.					*/
/*									*/
/************************************************************************/

static APP_WIDGET appSpellGuessList(	APP_WIDGET			parent,
					const SpellToolResources *	astr,
					SpellTool *			ast )
    {
    APP_WIDGET		label;
    APP_WIDGET		list;

    const int		visibleItems= 6;

    appMakeLabelInColumn( &label, parent, astr->astrGuesses );

    appGuiMakeListInColumn( &list, parent, visibleItems,
		    appSpellGuessChosen, appSpellToolCorrect, (void *)ast );

    return list;
    }

/************************************************************************/
/*									*/
/*  Make the button part of the spelling Tool.				*/
/*									*/
/************************************************************************/

static void appSpellMakeButtonRows(
				APP_WIDGET			spellForm,
				const SpellToolResources * 	astr,
				SpellTool *			ast )
    {
    APP_WIDGET		buttonRow;

    guiToolMake2BottonRow( &buttonRow, spellForm,
		&(ast->astIgnoreButton), &(ast->astFindNextButton),
		astr->astrIgnore, astr->astrFindNext,
		appSpellToolIgnorePushed, appSpellToolFindNextPushed,
		(void *)ast );

    guiToolMake2BottonRow( &buttonRow, spellForm,
		&(ast->astGuessButton), &(ast->astCorrectButton),
		astr->astrGuess, astr->astrCorrect,
		appSpellGuessButtonPushed, appSpellToolCorrect,
		(void *)ast );

    return;
    }

/************************************************************************/
/*									*/
/*  Resolve locale names.						*/
/*									*/
/************************************************************************/

static int appSpellGetLocaleNames(		EditApplication *	ea,
						SpellChecker *		sc )
    {
    static int				gotResources;

    static LocaleName *			localeNames= (LocaleName *)0;
    static int				localeNameCount;
    static AppConfigurableResource *	localeRes= (AppConfigurableResource *)0;

    int					i;

    if  ( ! gotResources )
	{
	localeNames= malloc( sc->scDictionaryCount* sizeof(LocaleName) );
	if  ( ! localeNames )
	    { LXDEB(sc->scDictionaryCount,localeNames); return -1; }
	localeRes= malloc( sc->scDictionaryCount*
					    sizeof(AppConfigurableResource) );
	if  ( ! localeRes )
	    { LXDEB(sc->scDictionaryCount,localeRes); return -1; }

	for ( i= 0; i < sc->scDictionaryCount; i++ )
	    {
	    int			j;
	    char *		name;
	    const char *	def;

	    localeNames[i].lnLocale= strdup( sc->scDictionaries[i].sdLocale );
	    if  ( ! localeNames[i].lnLocale )
		{ XDEB(localeNames[i].lnLocale); return -1;	}

	    name= malloc( 5+ strlen(sc->scDictionaries[i].sdLocale)+ 1 );
	    if  ( ! name )
		{ XDEB(name); return -1;	}

	    sprintf( name, "NAME_%s", sc->scDictionaries[i].sdLocale );
	    def=  sc->scDictionaries[i].sdLocale;

	    for ( j= 0;
		  j < sizeof(AppSpellToolLocaleNames)/sizeof(LocaleName);
		  j++ )
		{
		if  ( ! strcmp( AppSpellToolLocaleNames[j].lnLocale,
					    sc->scDictionaries[i].sdLocale ) )
		    {
		    def= AppSpellToolLocaleNames[j].lnLocaleName;
		    break;
		    }
		}

	    APP_SET_RESOURCE( localeRes+ i, name,
		    i* sizeof(LocaleName)+ offsetof(LocaleName,lnLocaleName),
		    def );
	    }

	appGuiGetResourceValues( &gotResources, ea, (void *)localeNames,
					    localeRes, sc->scDictionaryCount );
	localeNameCount= sc->scDictionaryCount;
	gotResources= 1;
	}

    for ( i= 0; i < sc->scDictionaryCount; i++ )
	{
	int		j;

	for ( j= 0; j < localeNameCount; j++ )
	    {
	    int		n= ( i+ j ) % localeNameCount;

	    if  ( ! strcmp( localeNames[n].lnLocale,
					sc->scDictionaries[i].sdLocale ) )
		{
		indSpellDictionarySetLocaleLabel( &(sc->scDictionaries[i]),
						localeNames[n].lnLocaleName );
		break;
		}
	    }
	}

    return 0;
    }

/************************************************************************/

void appInitSpellTool(	SpellTool *	ast )
    {
    ast->astApplication= (EditApplication *)0;
    ast->astInspector= (AppInspector *)0;

    ast->astResources= (SpellToolResources *)0;

    appInitOptionmenu( &(ast->astDictionaryOptionmenu) );

    ast->astLearnButton= (APP_WIDGET)0;
    ast->astForgetButton= (APP_WIDGET)0;
    ast->astGuessButton= (APP_WIDGET)0;
    ast->astIgnoreButton= (APP_WIDGET)0;
    ast->astFindNextButton= (APP_WIDGET)0;

    ast->astCorrectButton= (APP_WIDGET)0;
    ast->astGuessList= (APP_WIDGET)0;
    ast->astWordText= (APP_WIDGET)0;

    ast->astSpellChecker= (SpellChecker *)0;
    ast->astCurrentDictionary= -1;

    ast->astReadOnly= 0;
    ast->astGotAlternative= 0;
    }

void appCleanSpellTool(	SpellTool *	ast )
    {
    /* ast->astSpellChecker is not owned bt the tool */
    return;
    }

/************************************************************************/
/*									*/
/*  make a spell tool.							*/
/*									*/
/************************************************************************/

void appFillSpellTool(	SpellChecker *			sc,
			SpellTool *			ast,
			const SpellToolResources *	astr,
			AppInspector *			ai,
			int				subjectPage,
			InspectorSubject *		is,
			APP_WIDGET			pageWidget,
			const InspectorSubjectResources * isr )
    {
    ast->astResources= astr;

    ast->astSpellChecker= sc;
    ast->astCurrentDictionary= -1;
    if  ( utilMemoryBufferSetString( &(sc->scPrivateDirectory),
					    astr->astrPrivateDictionaries ) )
	{ LDEB(1);	}
    if  ( utilMemoryBufferSetString( &(sc->scSystemDirectory),
					    astr->astrSystemDictionaries ) )
	{ LDEB(1);	}

    appInitOptionmenu( &(ast->astDictionaryOptionmenu) );

    (*sc->scSetup)( sc, appSpellToolComplain, ast );

    ast->astDictionaryFrame=
		    appSpellMakeDictionaryFrame( pageWidget, astr, ast );
    ast->astGuessList= appSpellGuessList( pageWidget, astr, ast );

    appMakeTextInColumn( &(ast->astWordText), pageWidget, 0, 1 );

    appGuiSetTypingCallbackForText( ast->astWordText,
				    appSpellCorrectionTyped, (void *)ast );

    appSpellMakeButtonRows( pageWidget, astr, ast );

    appSpellToolSomethingFound( ast, 0 );
    appSpellToolGotAlternative( ast, 0 );

    return;
    }

void appSpellToolFillChoosers(	SpellTool *			ast,
				const SpellToolResources *	astr )
    {
    if  ( appSpellGetLocaleNames( ast->astApplication, ast->astSpellChecker ) )
	{ LDEB(1);	}

    appSpellFillDictionaryMenu( ast, astr );

    return;
    }

void appFinishSpellTool(		SpellTool *			ast,
					const SpellToolResources *	astr )
    {
    SpellChecker *		sc;
    int				hasDict= 1;

    appOptionmenuRefreshWidth( &(ast->astDictionaryOptionmenu) );

    if  ( ! ( sc= ast->astSpellChecker ) )
	{ hasDict= 0;	}
    else{
	if  ( ast->astCurrentDictionary < 0				||
	      ast->astCurrentDictionary >= sc->scDictionaryCount	)
	    { hasDict= 0;	}
	}

    guiEnableWidget( ast->astFindNextButton, hasDict );
    appSpellToolGotAlternative( ast, 0 );
    }

static AppConfigurableResource APP_SpellToolResourceTable[]=
    {
    /************************/
    /*  Spell Tool.		*/
    /************************/
    APP_RESOURCE( "spellToolDictTitle",
		offsetof(SpellToolResources,astrDictionary),
		"Dictionary" ),
    APP_RESOURCE( "spellToolLearn",
		offsetof(SpellToolResources,astrLearn),
		"Learn" ),
    APP_RESOURCE( "spellToolForget",
		offsetof(SpellToolResources,astrForget),
		"Forget" ),
    APP_RESOURCE( "spellToolGuesses",
		offsetof(SpellToolResources,astrGuesses),
		"Guesses" ),
    APP_RESOURCE( "spellToolIgnore",
		offsetof(SpellToolResources,astrIgnore),
		"Ignore" ),
    APP_RESOURCE( "spellToolFindNext",
		offsetof(SpellToolResources,astrFindNext),
		"Find Next" ),
    APP_RESOURCE( "spellToolGuess",
		offsetof(SpellToolResources,astrGuess),
		"Guess" ),
    APP_RESOURCE( "spellToolCorrect",
		offsetof(SpellToolResources,astrCorrect),
		"Correct" ),
    APP_RESOURCE( "spellToolNoDicts",
		offsetof(SpellToolResources,astrNoDicts),
		"None" ),

#   ifdef __VMS
    APP_RESOURCE( "spellToolPrivateDicts",
		offsetof(SpellToolResources,astrPrivateDictionaries),
		"_Dictionaries" ),
#   else
    APP_RESOURCE( "spellToolPrivateDicts",
		offsetof(SpellToolResources,astrPrivateDictionaries),
		".Dictionaries" ),
#   endif
    APP_RESOURCE( "spellToolSystemDicts",
		offsetof(SpellToolResources,astrSystemDictionaries),
		INDDIR ),

    APP_RESOURCE( "spellToolDirNoAccess",
	    offsetof(SpellToolResources,astrErrors[SCerrDIR_NO_ACCESS]),
	    "Could not access directory for dictionaries." ),
    APP_RESOURCE( "spellToolDirNoSuchDir",
	    offsetof(SpellToolResources,astrErrors[SCerrDIR_DOES_NOT_EXIST]),
	    "This directory could not be found." ),
    APP_RESOURCE( "spellToolDirNoDicts",
	    offsetof(SpellToolResources,astrErrors[SCerrDIR_HAS_NO_DICTS]),
	    "No dictionaries were found in this directory." ),
    APP_RESOURCE( "spellToolSysDictNoAccess",
	    offsetof(SpellToolResources,astrErrors[SCerrDICT_NO_ACCESS]),
	    "Could not read system dictionary." ),
    APP_RESOURCE( "spellToolPrivDictDirNotMade",
	    offsetof(SpellToolResources,astrErrors[SCerrPRIVATE_DIR_NOT_MADE]),
	    "Could not make private directory." ),
    APP_RESOURCE( "spellToolPrivDictNoAccess",
	    offsetof(SpellToolResources,astrErrors[SCerrPRIVATE_DICT_NO_ACCESS]),
	    "Could not read private dictionary." ),
    APP_RESOURCE( "spellToolPrivDictWrongFormat",
	    offsetof(SpellToolResources,astrErrors[SCerrPRIVATE_DICT_WRONG_FORMAT]),
	    "Private dictionary has an illegal format." ),
    };

static AppConfigurableResource APP_SpellToolSubjectResourceTable[]=
    {
    APP_RESOURCE( "docToolMenuSpellText",
		offsetof(InspectorSubjectResources,isrSubjectName),
		"Spelling" ),
    };

void appSpellToolGetResourceTable(	EditApplication *		ea,
					SpellToolResources *		astr,
					InspectorSubjectResources *	isr )
    {
    static int	gotToolResources= 0;
    static int	gotSubjectResources= 0;

    if  ( ! gotToolResources )
	{
	appGuiGetResourceValues( &gotToolResources, ea, (void *)astr,
					APP_SpellToolResourceTable,
					sizeof(APP_SpellToolResourceTable)/
					sizeof(AppConfigurableResource) );
	}

    if  ( ! gotSubjectResources )
	{
	appGuiGetResourceValues( &gotSubjectResources, ea, (void *)isr,
				APP_SpellToolSubjectResourceTable,
				sizeof(APP_SpellToolSubjectResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    return;
    }
