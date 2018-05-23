/************************************************************************/
/*									*/
/*  Spell tool and spelling checking definitions.			*/
/*									*/
/************************************************************************/

#   ifndef		APP_SPELL_TOOL_H
#   define		APP_SPELL_TOOL_H

#   include	<indSpellChecker.h>

#   include	"appFrame.h"
#   include	"appInspector.h"

typedef struct LocaleName
    {
    const char *	lnLocale;
    const char *	lnLocaleName;
    } LocaleName;

typedef struct SpellToolResources
    {
    char *		astrDictionary;
    char *		astrLearn;
    char *		astrForget;
    char *		astrGuesses;
    char *		astrIgnore;
    char *		astrFindNext;
    char *		astrGuess;
    char *		astrCorrect;
    char *		astrPrivateDictionaries;
    char *		astrSystemDictionaries;
    char *		astrNoDicts;

    char *		astrErrors[SCerr_COUNT];

    LocaleName *	astrLocaleNames;
    int			astrLocaleNameCount;
    } SpellToolResources;

/************************************************************************/
/*									*/
/*  Represents a spell tool.						*/
/*									*/
/************************************************************************/

typedef struct SpellTool
    {
    EditApplication *		astApplication;
    AppInspector *		astInspector;

    const SpellToolResources *	astResources;

    APP_WIDGET			astDictionaryFrame;
    AppOptionmenu		astDictionaryOptionmenu;

    APP_WIDGET			astLearnButton;
    APP_WIDGET			astForgetButton;
    APP_WIDGET			astGuessButton;
    APP_WIDGET			astIgnoreButton;
    APP_WIDGET			astFindNextButton;

    APP_WIDGET			astCorrectButton;
    APP_WIDGET			astGuessList;
    APP_WIDGET			astWordText;

    SpellChecker *		astSpellChecker; /* Not owned by the tool */
    int				astCurrentDictionary;
    int				astReadOnly;
    int				astGotAlternative;
    } SpellTool;

/************************************************************************/
/*									*/
/*  Subroutine declarations.						*/
/*									*/
/************************************************************************/

extern void appEnableSpellTool(		void *		voidast,
					int		enabled );

extern void appFillSpellTool(	SpellChecker *			sc,
				SpellTool *			ast,
				const SpellToolResources *	astr,
				AppInspector *			ai,
				int				subjectPage,
				InspectorSubject *		is,
				APP_WIDGET			pageWidget,
				const InspectorSubjectResources * isr );

extern void appSpellToolGetResourceTable(
					EditApplication *		ea,
					SpellToolResources *		astr,
					InspectorSubjectResources *	isr );

extern void appSpellToolSetReadOnly(	SpellTool *		ast,
					int			readonly );

extern void appInitSpellTool(		SpellTool *		ast );
extern void appCleanSpellTool(		SpellTool *		ast );

extern void appSpellToolFillChoosers(	SpellTool *			ast,
					const SpellToolResources *	astr );

extern void appFinishSpellTool(		SpellTool *			ast,
					const SpellToolResources *	astr );

#   endif	/*	APP_SPELL_TOOL_H	*/
