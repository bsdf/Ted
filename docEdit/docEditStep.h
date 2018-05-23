/************************************************************************/
/*									*/
/*  Keep a trace of modifications to a document.			*/
/*									*/
/************************************************************************/

#   ifndef	DOC_EDIT_STEP_H
#   define	DOC_EDIT_STEP_H

#   include	<docBuf.h>
#   include	<docDocumentList.h>

/************************************************************************/
/*									*/
/*  A step on the trace.						*/
/*									*/
/************************************************************************/

typedef struct EditStep
    {
			/**
			 *   What kind of action was this?
			 */
    int			esCommand;
			/**
			 *   Hierarchy level at which a property change is
			 *   applied.
			 */
    int			esPropLevel;
			/**
			 *   In what tree of the document did it start?
			 */
    SelectionScope	esOldSelectionScope;
			/**
			 *   The selection that we acted upon before the action.
			 *   Usually this is the selection that the user sees.
			 */
    EditRange		esOldEditRange;
    int			esOldCol0;
    int			esOldCol1;
			/**
			 *  Page and column for headers etc
			 */
    int			esOldPage;
    int			esOldColumn;
			/**
			 *   In what tree of the document did it finish?
			 */
    SelectionScope	esNewSelectionScope;
			/**
			 *   The selection that was the result of the action.
			 *   (This is not necessarily the selection visible 
			 *   to the user. In a replacement this selects the 
			 *   result of the replacement. The user sees the 
			 *   cursor at the end of the replacement.)
			 *   We need to know this for the undo. It is irrelevant
			 *   for repeat/redo.
			 */
    EditRange		esNewEditRange;
    int			esNewCol0;
    int			esNewCol1;
			/**
			 *  Page and column for headers etc
			 */
    int			esNewPage;
    int			esNewColumn;
			/**
			 *   Where did the selection end up after the 
			 *   edit action. We need to know to do the same 
			 *   after a redo/repeat;
			 */
    int			esSelectionPosition;
			/**
			 *   The contents that have been retrieved from the 
			 *   trace. NOTE that some of the fields of the 
			 *   document are shallow copies of the parent 
			 *   document.
			 */
    BufferDocument *	esSourceDocument;
    int			esSourceIsIntermediary;

			/**
			 *   Holds the kind of field that was deleted or
			 *   changed.
			 */
    int			esFieldKind;

			/**
			 *   Holds the property change.
			 */
    DocumentStyle	esNewStyle;
    PropertyMask	esNewDocPropMask;
    DocumentProperties	esNewDocProps;
			/**
			 *   Holds the picture property change.
			 */
    PropertyMask	esPicturePropMask;
			/**
			 *   Holds the note property change.
			 */
    PropertyMask	esNotePropMask;
			/**
			 *   Holds the old/new list description.
			 */
    DocumentList	esDocumentList;

			/**
			 *  The position in the trace where the step was 
			 *  found.
			 */
    int			esTraceIndex;
			/**
			 *  Number of steps in trace (0, 1 or 2)
			 */
    int			esStepCount;
    } EditStep;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docInitEditStep(	EditStep *		es );
extern void docCleanEditStep(	EditStep *		es );

extern BufferDocument * docEditStepMakeSourceDocument(
						EditStep *		es,
						const BufferDocument *	bdRef );

#    endif	/*  DOC_EDIT_STEP_H	*/
