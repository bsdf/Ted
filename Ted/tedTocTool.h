/************************************************************************/
/*									*/
/*  'Table of Contents' page of the format tool.			*/
/*									*/
/************************************************************************/

#   ifndef	TED_TOC_TOOL_H
#   define	TED_TOC_TOOL_H

#   include	<appFrame.h>
#   include	<appInspector.h>
#   include	<docBuf.h>
#   include	<docTocField.h>
#   include	<docSelectionDescription.h>
#   include	<docListDepth.h>

/************************************************************************/
/*									*/
/*  Resources for the 'Table of Contents' page.				*/
/*									*/
/************************************************************************/

typedef struct TocPageResources
    {
    const char *	tprType;
    const char *	tprTypeItems[TOCtype_COUNT];
    const char *	tprLevels;

    const char *	tprFrom;
    const char *	tprFromStyles;
    const char *	tprFromOutline;
    const char *	tprFromTcEntries;

    const char *	tprSequence;

    const char *	tprMakeHyperlinks;
    const char *	tprIncludePageNumbers;

    const char *	tprOutlineLevel;
    const char *	tprOutlineLevels[PPoutline_COUNT+1];
    const char *	tprRevertParaLevel;
    const char *	tprApplyParaLevel;
    } TocPageResources;

/************************************************************************/
/*									*/
/*  A toc tool, i.e. the 'Table of Contents' page of the format tool.	*/
/*									*/
/************************************************************************/

typedef struct TocTool
    {
    EditApplication *		ttApplication;
    AppInspector *		ttInspector;
    const TocPageResources *	ttPageResources;

    /*  TOC  */
    TocField			ttPropertiesSet;
    TocField			ttPropertiesChosen;
    unsigned char		ttHaveToc;

    APP_WIDGET			ttTocFrame;
    APP_WIDGET			ttTocPaned;
    AppOptionmenu		ttTocTypeMenu;
    APP_WIDGET			ttTocTypeItems[TOCtype_COUNT+1];
    AppOptionmenu		ttTocLevelsMenu;
    APP_WIDGET			ttTocLevelsItems[PPoutline_COUNT];

    APP_WIDGET			ttTocEntryNameText;	/* \f		*/
    APP_WIDGET			ttTocFromTcToggle;	/* \f		*/
    APP_WIDGET			ttTocFromStylesToggle;	/* \o		*/
    APP_WIDGET			ttTocFromOutlineToggle;	/* \u		*/

    AppOptionmenu		ttTocSeqOptionMenu;	/* \c \a	*/

    APP_WIDGET			ttTocHyperlinksToggle;	/* \h		*/
    APP_WIDGET			ttTocPageNumbersToggle;	/* \n		*/

    const char **		ttSequences;
    int				ttSequenceCount;

    /*  OUTLINE  */
    int				ttOutlineLevelSet;
    int				ttOutlineLevelChosen;

    APP_WIDGET			ttOutlineFrame;
    APP_WIDGET			ttOutlinePaned;
    AppOptionmenu		ttOulineLevelMenu;
    APP_WIDGET			ttOulineLevelItems[PPoutline_COUNT+1];
    APP_WIDGET			ttRevertParaButton;
    APP_WIDGET			ttApplyParaButton;

    APP_WIDGET			ttApplyButton;
    APP_WIDGET			ttInsertButton;
    APP_WIDGET			ttDeleteButton;
    } TocTool;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void tedFormatFillTocPage( TocTool *			tt,
				const TocPageResources *	tpr,
				AppInspector *			ai,
				int				subjectPage,
				InspectorSubject *		is,
				APP_WIDGET			pageWidget,
				const InspectorSubjectResources * isr );

extern void tedTocToolGetResourceTable(
					EditApplication *		ea,
					TocPageResources *		tpr,
					InspectorSubjectResources *	isr );

extern void tedRefreshTocTool(	TocTool *			tt,
				int *				pEnabled,
				int *				pPref,
				InspectorSubject *		is,
				const DocumentSelection *	ds,
				const SelectionDescription *	sd,
				BufferDocument *		bd,
				const unsigned char *		cmdEnabled );

extern void tedTocToolFillChoosers(	TocTool *			tt,
					const TocPageResources *	fpr );

extern void tedInitTocTool(		TocTool *	tt );
extern void tedCleanTocTool(		TocTool *	tt );

extern void tedFinishTocTool( 		TocTool *			tt,
					const TocPageResources *	fpr );

#   endif	/*  TED_TOC_TOOL_H */
