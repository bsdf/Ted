/************************************************************************/
/*									*/
/*  Ted: Interface to the generic spell tool.				*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	"tedAppFront.h"
#   include	"tedFind.h"
#   include	"tedSpellTool.h"
#   include	"tedAppResources.h"
#   include	<guiToolUtil.h>
#   include	<docEditCommand.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Adapt find tool to the current document.				*/
/*									*/
/************************************************************************/

void tedRefreshSpellTool(	SpellTool *			ast,
				int *				pEnabled,
				int *				pPref,
				InspectorSubject *		is,
				const DocumentSelection *	ds,
				const SelectionDescription *	sd,
				const BufferDocument *		bd,
				int				documentId,
				const unsigned char *		cmdEnabled )
    {
    if  ( ! ast->astSpellChecker			||
	  ast->astSpellChecker->scDictionaryCount == 0	)
	{ *pEnabled= 0; return;		}

    appSpellToolSetReadOnly( ast, ! cmdEnabled[EDITcmdREPLACE] );

    *pEnabled= 1;
    return;
    }

/************************************************************************/
/*									*/
/*  Build a find tool and connect it to Ted.				*/
/*									*/
/************************************************************************/

static SpellChecker TED_SpellChecker;
static int TED_SpellCheckerInitialized;

void tedFillSpellTool(		SpellTool *			ast,
				const SpellToolResources *	astr,
				AppInspector *			ai,
				int				subjectPage,
				InspectorSubject *		is,
				APP_WIDGET			pageWidget,
				const InspectorSubjectResources * isr )
    {
    if  ( ! TED_SpellCheckerInitialized )
	{
	indInitSpellChecker( &TED_SpellChecker );

	TED_SpellChecker.scSetup= indSpellSetup;
	TED_SpellChecker.scFindNext= tedSpellFindNext;
	TED_SpellChecker.scCorrect= tedAppReplace;

	TED_SpellCheckerInitialized= 1;
	}

    appFillSpellTool( &TED_SpellChecker,
			    ast, astr, ai, subjectPage, is, pageWidget, isr );

    return;
    }
