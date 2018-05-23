/************************************************************************/
/*									*/
/*  List tool:								*/
/*									*/
/*  The page on the format tool to edit list properties.		*/
/*									*/
/************************************************************************/

#   ifndef	TED_LIST_TOOL_H
#   define	TED_LIST_TOOL_H

#   include	<docBuf.h>
#   include	<appFrame.h>
#   include	<appInspector.h>
#   include	<docSelectionDescription.h>
#   include	<docListLevel.h>
#   include	<docDocumentList.h>
#   include	<docListOverride.h>

# define LISTTOOLcountSTYLES	8

/************************************************************************/
/*									*/
/*  Resources for the 'Bullets and Numbers' page.			*/
/*									*/
/************************************************************************/

typedef struct ListsPageResources
    {
    const char *	lprListLevel;
    const char *	lprNumberStyle;
    const char *	lprNumberFormat;

    const char *	lprStartAt;
    const char *	lprNoRestart;
    const char *	lprJustify;
    const char *	lprFollowedBy;
    const char *	lprStyleOptionTexts[LISTTOOLcountSTYLES];
    const char *	lprJustifyOptionTexts[DOClla_COUNT];
    const char *	lprFollowOptionTexts[DOCllf_COUNT];

    const char *	lprLevelFirstIndent;
    const char *	lprLevelLeftIndent;

    const char *	lprSelection;
    const char *	lprSetList;
    const char *	lprNewList;
    const char *	lprSetLevel;
    const char *	lprRemoveFromList;

    const char *	lprDeleteLevelNumber;
    const char *	lprInsertLevelNumber;
    const char *	lprEditLevelText;
    } ListsPageResources;

/************************************************************************/
/*									*/
/*  A lists tool, i.e. the 'Bullets and Numbers' page of the format	*/
/*  tool.								*/
/*									*/
/************************************************************************/

typedef struct ListTool
    {
    EditApplication *		ltApplication;
    AppInspector *		ltInspector;
    const ListsPageResources *	ltPageResources;

    int				ltUnitInt;

    int				ltInProgrammaticChange;

    DocumentList		ltListPropertiesSet;
    DocumentList		ltListPropertiesChosen;
    ListOverride		ltListOverrideSet;
    ListOverride		ltListOverrideChosen;
    int				ltCurrentLevel;
    int				ltCurrentParagraphNumber;
    int				ltTabIntervalTwips;

    int				ltHerePath[DLmaxLEVELS];
    int				ltPrevPath[DLmaxLEVELS];
    int				ltCurrPath[DLmaxLEVELS];
    int				ltFormatPath[DLmaxLEVELS];
    int				ltStartPath[DLmaxLEVELS];

    int				ltHereLevel;
    int				ltPrevLevel;
    int				ltFormatIndex0;
    int				ltFormatIndex1;
				/****************************************/
				/*  Possible values range from 0 to	*/
				/*  2* ll->dllNumberSize. Even		*/
				/*  numbers refer to the text. Odd	*/
				/*  numbers to the number. The last	*/
				/*  place is for the text after the	*/
				/*  last number. I.E: these are		*/
				/*  subsctipts in the offsets array.	*/
				/****************************************/
    int				ltFormatOffset0;
    int				ltFormatOffset1;
    int				ltNumberFormatOffsets[2* DLmaxLEVELS+ 2];

    unsigned char		ltCanUpdateSelection;
    unsigned char		ltCanUpdateList;

    /**/
    APP_WIDGET			ltListLevelList;

    APP_WIDGET			ltSelectionFrame;
    APP_WIDGET			ltSelectionPaned;

    APP_WIDGET			ltListLevelFrame;
    APP_WIDGET			ltListLevelPaned;

    AppOptionmenu		ltNumberStyleOptionmenu;
    APP_WIDGET			ltNumberStyleItems[LISTTOOLcountSTYLES];

    APP_WIDGET			ltFormatLabel;
    APP_WIDGET			ltNumberFormatText;
    APP_WIDGET			ltNumberFormatMenu;
    int				ltFormatEditable;

    APP_WIDGET			ltFirstIndentText;
    APP_WIDGET			ltLeftIndentText;
    APP_WIDGET			ltStartAtText;
    APP_WIDGET			ltNoRestartToggle;

    AppOptionmenu		ltJustifyOptionmenu;
    APP_WIDGET			ltJustifyItems[DOClla_COUNT];

    AppOptionmenu		ltFollowOptionmenu;
    APP_WIDGET			ltFollowItems[DOCllf_COUNT];

    APP_WIDGET			ltSetListButton;
    APP_WIDGET			ltNewListButton;
    APP_WIDGET			ltSetLevelButton;
    APP_WIDGET			ltRemoveFromListButton;
    } ListTool;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void tedFormatToolGetListsResourceTable(
					EditApplication *		ea,
					ListsPageResources *		spr,
					InspectorSubjectResources *	isr );

extern void tedRefreshListTool(
				ListTool *			lt,
				int *				pEnabled,
				int *				pPref,
				InspectorSubject *		is,
				const DocumentSelection *	ds,
				const SelectionDescription *	sd,
				BufferDocument *		bd,
				const unsigned char *		cmdEnabled );

extern void tedFormatFillListsPage( ListTool *			lt,
				const ListsPageResources *	lpr,
				InspectorSubject *		is,
				APP_WIDGET			pageWidget,
				const InspectorSubjectResources *	isr );

extern void tedFormatFillListChoosers(	ListTool *			lt );

extern void tedFormatFinishListPage(	ListTool *			lt );

extern void tedFormatCleanListsTool(	ListTool *			lt );

#   endif	/*  TED_LIST_TOOL_H */
