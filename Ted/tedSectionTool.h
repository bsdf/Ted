/************************************************************************/
/*									*/
/*  Format tool:							*/
/*									*/
/************************************************************************/

#   ifndef	TED_SECTION_TOOL_H
#   define	TED_SECTION_TOOL_H

#   include	<docBuf.h>
#   include	<appFrame.h>
#   include	<appInspector.h>
#   include	<docSelectionDescription.h>

/************************************************************************/
/*									*/
/*  Resources for the 'Section; page.					*/
/*									*/
/************************************************************************/

typedef struct SectionPageResources
    {
    const char *	sprUnitType;

    const char *	sprNumberingRestarts;

    const char *	sprPageNumberStyle;
    const char *	sprNumberStyleMenuTexts[DOCpgn_COUNT];

    const char *	sprBreakKind;
    const char *	sprBreakKindMenuTexts[DOCibk_COUNT];

    const char *	sprColumns;
    const char *	sprColumnCount;
    const char *	sprColumn;
    const char *	sprColumnWidth;
    const char *	sprColumnSpacing;
    const char *	sprLineBetweenColumns;
    const char *	sprColumnsEqualWidth;
    } SectionPageResources;

/************************************************************************/
/*									*/
/*  A section tool, i.e. the 'Section' page of the format tool.		*/
/*									*/
/************************************************************************/

# define SECT_MAX_COLUMNS	4

typedef struct SectionTool
    {
    EditApplication *			stApplication;
    AppInspector *			stInspector;
    const SectionPageResources *	stPageResources;

    int					stUnitInt;

    SectionProperties			stPropertiesSet;
    SectionProperties			stPropertiesChosen;

    int					stSectionNumber;
    int					stCanChange;

    APP_WIDGET				stNumberRow;
    APP_WIDGET				stNumberLabel;
    APP_WIDGET				stNumberText;

    APP_WIDGET				stPageRestartRow;
    APP_WIDGET				stPageRestartToggle;
    APP_WIDGET				stPageRestartText;

    APP_WIDGET				stNumberStyleRow;
    AppOptionmenu			stNumberStyleOptionmenu;
    APP_WIDGET				stNumberStyleItems[DOCpgn_COUNT];

    APP_WIDGET				stBreakKindRow;
    AppOptionmenu			stBreakKindOptionmenu;
    APP_WIDGET				stBreakKindItems[DOCibk_COUNT];

    APP_WIDGET				stColumnsFrame;
    APP_WIDGET				stColumnsPaned;

    AppOptionmenu			stColumnCountMenu;
    APP_WIDGET				stColumnCountItems[SECT_MAX_COLUMNS+1];

    APP_WIDGET				stLineBetweenColumnsToggle;
    APP_WIDGET				stEqualWidthToggle;

    APP_WIDGET				stColumnRows[SECT_MAX_COLUMNS];
    APP_WIDGET				stColnoLabels[SECT_MAX_COLUMNS];
    APP_WIDGET				stColwTexts[SECT_MAX_COLUMNS];
    APP_WIDGET				stColsrTexts[SECT_MAX_COLUMNS];
    } SectionTool;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void tedFormatToolGetSectResourceTable(	EditApplication *	ea,
					SectionPageResources *		spr,
					InspectorSubjectResources *	isr );

extern void tedRefreshSectionTool(
				SectionTool *			st,
				int *				pEnabled,
				int *				pPref,
				InspectorSubject *		is,
				const DocumentSelection *	ds,
				const SelectionDescription *	sd,
				const BufferDocument *		bd,
				const unsigned char *		cmdEnabled );

extern void tedFormatCleanSectionTool(		SectionTool *		st );

extern void tedFormatFillSectionPage(	SectionTool *		st,
				const SectionPageResources *	spr,
				InspectorSubject *		is,
				APP_WIDGET			pageWidget,
				const InspectorSubjectResources * isr );

extern void tedFormatFillSectionChoosers( SectionTool *			st,
					const SectionPageResources *	spr );

extern void tedFormatFinishSectionPage(	SectionTool *			st,
					const SectionPageResources *	spr );

#   endif	/*  TED_SECTION_TOOL_H */
