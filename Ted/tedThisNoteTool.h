/************************************************************************/
/*									*/
/*  Notes tool:								*/
/*									*/
/************************************************************************/

#   ifndef	TED_THIS_NOTE_TOOL_H
#   define	TED_THIS_NOTE_TOOL_H

#   include	<appFrame.h>
#   include	<appInspector.h>
#   include	<docBuf.h>
#   include	<docNoteProperties.h>
#   include	<docSelectionDescription.h>

/************************************************************************/
/*									*/
/*  A note tool, i.e. the 'This Note' page of the format tool.		*/
/*									*/
/************************************************************************/

typedef struct ThisNotePageResources
    {
    const char *	nprCurrentNoteText;
    const char *	nprFootnoteText;
    const char *	nprEndnoteText;
    const char *	nprFixedTextText;
    const char *	nprToNoteRefText;
    const char *	nprToNoteText;
    } ThisNotePageResources;

typedef struct ThisNoteTool
    {
    EditApplication *			tntApplication;
    AppInspector *			tntInspector;
    const ThisNotePageResources *	tntPageResources;

    FootEndNotesProperties		tntNotesPropertiesSet;

    int					tntNoteNumber;
    NoteProperties			tntNotePropertiesSet;
    NoteProperties			tntNotePropertiesChosen;
    unsigned char			tntCanChange;
    unsigned char			tntInsideNote;
    int					tntInProgrammaticChange;

    /****/
    APP_WIDGET				tntNoteTypeRow;
    APP_WIDGET				tntFootnoteToggle;
    APP_WIDGET				tntEndnoteToggle;
    APP_WIDGET				tntFixedTextRow;
    APP_WIDGET				tntFixedTextToggle;
    APP_WIDGET				tntFixedTextText;

    APP_WIDGET				tntToOtherRow;
    APP_WIDGET				tntToNoteRefButton;
    APP_WIDGET				tntToNoteButton;
    } ThisNoteTool;

/************************************************************************/
/*									*/
/*  Routine Declarations.						*/
/*									*/
/************************************************************************/

extern void tedRefreshThisNoteTool( ThisNoteTool *		tnt,
				int *				pEnabled,
				int *				pPref,
				InspectorSubject *		is,
				const DocumentSelection *	ds,
				const SelectionDescription *	sd,
				BufferDocument *		bd,
				const unsigned char *		cmdEnabled );

extern void tedFormatCleanThisNoteTool(	ThisNoteTool *		tnt );

extern void tedFormatInitThisNoteTool(	ThisNoteTool *		tnt );

extern void tedFormatFillThisNotePage(
				ThisNoteTool *			tnt,
				const ThisNotePageResources *	npr,
				InspectorSubject *		is,
				APP_WIDGET			pageWidget,
				const InspectorSubjectResources * isr );

extern void tedFormatToolGetThisNoteResourceTable(
				EditApplication *		ea,
				ThisNotePageResources *		npr,
				InspectorSubjectResources *	isr );

#   endif	/*  TED_THIS_NOTE_TOOL_H */
