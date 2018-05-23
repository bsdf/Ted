/************************************************************************/
/*									*/
/*  Tabs tool:								*/
/*									*/
/*  The page on the format tool to edit tab stops.			*/
/*									*/
/************************************************************************/

#   ifndef	TED_TABS_TOOL_H
#   define	TED_TABS_TOOL_H

#   include	<appFrame.h>
#   include	<appInspector.h>
#   include	<docBuf.h>
#   include	<docTabStop.h>
#   include	<docTabStopList.h>
#   include	<docSelectionDescription.h>
#   include	<docSelectionGeometry.h>

/************************************************************************/
/*									*/
/*  Resources for the 'Tabs' page.					*/
/*									*/
/************************************************************************/

typedef struct TabsPageResources
    {
    const char *	tprDefaultTabStopsLabel;
    const char *	tprDefaultTabStopsEvery;
    const char *	tprDefaultTabStopsRevert;
    const char *	tprDefaultTabStopsApply;

    const char *	tprPositions;

    const char *	tprTabStop;

    const char *	tprPosition;

    const char *	tprAlignment;
    const char *	tprAlignmentOptionTexts[DOCta_COUNT];

    const char *	tprLeader;
    const char *	tprLeaderOptionTexts[DOCtl_COUNT];

    const char *	tprTooCloseMessage;
    } TabsPageResources;

/************************************************************************/
/*									*/
/*  A tabs tool, i.e. the 'Tabs' page of the format tool.		*/
/*									*/
/************************************************************************/

typedef struct TabsTool
    {
    EditApplication *		ttApplication;
    AppInspector *		ttInspector;
    const TabsPageResources *	ttPageResources;

    int				ttInProgrammaticChange;

    int				ttUnitInt;
    unsigned char		ttCanChange;

    DocumentProperties		ttDocPropertiesSet;
    DocumentProperties		ttDocPropertiesChosen;

    TabStopList			ttTabStopsSet;
    TabStopList			ttTabStopsChosen;

    TabStop			ttTabStopValue;
    int				ttTabStopNumber;

    /**/
    APP_WIDGET			ttTabDefaultFrame;
    APP_WIDGET			ttTabDefaultPaned;
    APP_WIDGET			ttTabDefaultText;
    APP_WIDGET			ttTabDefaultRevertButton;
    APP_WIDGET			ttTabDefaultApplyButton;

    /**/
    APP_WIDGET			ttTabPositionList;

    APP_WIDGET			ttTabPropertyFrame;
    APP_WIDGET			ttTabPropertyPaned;

    APP_WIDGET			ttTabPositionText;

    APP_WIDGET			ttDeleteTabButton;
    APP_WIDGET			ttInsertTabButton;
    APP_WIDGET			ttChangeTabButton;

    AppOptionmenu		ttAlignmentOptionmenu;
    APP_WIDGET			ttAlignmentItems[DOCta_COUNT];

    AppOptionmenu		ttLeaderOptionmenu;
    APP_WIDGET			ttLeaderItems[DOCtl_COUNT];
    } TabsTool;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void tedFormatToolGetTabsResourceTable(	EditApplication *	ea,
					TabsPageResources *		spr,
					InspectorSubjectResources *	isr );

extern void tedRefreshTabsTool(
				TabsTool *			tt,
				int *				pEnabled,
				int *				pPref,
				InspectorSubject *		is,
				const DocumentSelection *	bs,
				const SelectionGeometry *	sg,
				const SelectionDescription *	sd,
				const BufferDocument *		bd,
				const unsigned char *		cmdEnabled );

extern void tedFormatFillTabsPage( TabsTool *			tt,
				const TabsPageResources *	tpr,
				InspectorSubject *		is,
				APP_WIDGET			pageWidget,
				const InspectorSubjectResources *	isr );

extern void tedTabsToolFillChoosers(	TabsTool *			tt );

extern void tedFormatFinishTabsPage(	TabsTool *			tt );

extern void tedFormatCleanParaTabsTool(	TabsTool *			tt );

#   endif	/*  TED_TABS_TOOL_H */
