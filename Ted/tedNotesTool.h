/************************************************************************/
/*									*/
/*  Notes tool:								*/
/*									*/
/************************************************************************/

#   ifndef	TED_NOTES_TOOL_H
#   define	TED_NOTES_TOOL_H

#   include	<appFrame.h>
#   include	<appInspector.h>
#   include	<docBuf.h>
#   include	<docSelectionDescription.h>

/************************************************************************/
/*									*/
/*  A notes tool, i.e. the 'Notes' page of the format tool.		*/
/*									*/
/************************************************************************/

typedef struct NotesPageResources
    {
    const char *	nprFootnotesText;
    const char *	nprEndnotesText;

    const char *	nprFirstNumberText;
    const char *	nprJustifyText;
    const char *	nprPositionText;
    const char *	nprRestartText;
    const char *	nprStyleText;

    const char *	nprJustifyMenuTexts[FTNjustify_COUNT];
    const char *	nprPlacementMenuTexts[FTNplace_COUNT];
    const char *	nprRestartMenuTexts[FTN_RST__COUNT];
    const char *	nprNumberStyleMenuTexts[FTNstyle_COUNT];

    const char *	nprRevertSelectionText;
    const char *	nprChangeSelectionText;
    } NotesPageResources;

typedef struct NotesTool
    {
    EditApplication *		ntApplication;
    AppInspector *		ntInspector;
    const NotesPageResources *	ntPageResources;

    unsigned int		ntNoteTreeType;

    unsigned char		ntCanChangeSelection;
    unsigned char		ntCanChangeDocument;
    int				ntInProgrammaticChange;

    FootEndNotesProperties	ntNotesPropertiesSetDocument;
    FootEndNotesProperties	ntNotesPropertiesSetSelection;
    FootEndNotesProperties	ntNotesPropertiesChosen;

    APP_WIDGET			ntRevertSelectionWidget;
    APP_WIDGET			ntChangeSelectionWidget;


    APP_WIDGET			ntJustifyRow;
    APP_WIDGET			ntJustifyLabel;
				/*  Not all positions used!	*/
    AppOptionmenu		ntJustifyOptionmenu;
    APP_WIDGET			ntJustifyOptions[FTNjustify_COUNT];

    APP_WIDGET			ntPlacementRow;
    APP_WIDGET			ntPlacementLabel;
				/*  Not all positions used!	*/
    AppOptionmenu		ntPlacementOptionmenu;
    APP_WIDGET			ntPlacementOptions[FTNplace_COUNT];

    APP_WIDGET			ntRestartRow;
    APP_WIDGET			ntRestartLabel;
				/*  Not all positions used!	*/
    AppOptionmenu		ntRestartOptionmenu;
    APP_WIDGET			ntRestartOptions[FTN_RST__COUNT];

    APP_WIDGET			ntStyleRow;
    APP_WIDGET			ntStyleLabel;
    AppOptionmenu		ntStyleOptionmenu;
    APP_WIDGET			ntStyleOptions[FTNstyle_COUNT];

    APP_WIDGET			ntStartNumberRow;
    APP_WIDGET			ntStartNumberLabel;
    APP_WIDGET			ntStartNumberText;
    } NotesTool;

/************************************************************************/
/*									*/
/*  Routine Declarations.						*/
/*									*/
/************************************************************************/

extern void tedFormatToolGetNotesResourceTable( EditApplication *	ea,
					NotesPageResources *		spr,
					InspectorSubjectResources *	isr );

extern void tedRefreshNotesTool( NotesTool *			nt,
				int *				pEnabled,
				int *				pPref,
				InspectorSubject *		is,
				const DocumentSelection *	ds,
				const SelectionDescription *	sd,
				BufferDocument *		bd,
				const unsigned char *		cmdEnabled );

extern void tedFormatCleanNotesTool(	NotesTool *		nt );

extern void tedFormatInitNotesTool(	NotesTool *		nt );

extern void tedFormatFillNotesChoosers(	NotesTool *			nt,
					const NotesPageResources *	npr );

extern void tedFormatFinishNotesPage(	NotesTool *			nt,
					const NotesPageResources *	npr );

extern void tedFormatFillNotesPage( NotesTool *			nt,
				const NotesPageResources *	npr,
				InspectorSubject *		is,
				APP_WIDGET			pageWidget,
				const InspectorSubjectResources * isr,
				int				treeType );

#   endif	/*  TED_NOTES_TOOL_H */
