/************************************************************************/
/*									*/
/*  Document Editing.							*/
/*									*/
/************************************************************************/

#   ifndef		DOC_EDIT_OPERATION_H
#   define		DOC_EDIT_OPERATION_H

#   include		<docBuf.h>
#   include		<sioGeneral.h>

/************************************************************************/

typedef enum ReformatNeeded
    {
    REFORMAT_NOTHING= 0,
    REFORMAT_ADJUST_PARAGRAPH,
    REFORMAT_RANGE,
    REFORMAT_BODY_SECT,
    REFORMAT_DOCUMENT,

    REFORMAT__COUNT
    } ReformatNeeded;

/************************************************************************/
/*									*/
/*  Describes an edit operation. Part is only used for interactive edit	*/
/*  operations.								*/
/*									*/
/************************************************************************/

typedef struct EditOperation
    {
    SelectionScope		eoSelectionScope;
    struct BufferItem *		eoBodySectNode;
    DocumentField *		eoBottomField;
				    /************************************/
				    /*  Field (if any) that fully holds	*/
				    /*  the initial selection for this	*/
				    /*  edit operation. The assumption	*/
				    /*	is that it will not be deleted	*/
				    /*  and that it will be the parent	*/
				    /*  of new fields.			*/
				    /************************************/

    unsigned char		eoIBarSelectionOld;
    unsigned char		eoMultiParagraphSelectionOld;

    /**/
    int				eoParaAdjustParagraphNumber;
    int				eoParaAdjustStroffFrom;
    int				eoParaAdjustStroffShift;
    int				eoParaAdjustStroffUpto;

    /**/
				/**
				 *  Document range that is touched by the edit 
				 *  operation: It must be reformatted and 
				 *  redisplayed after editing.
				 */
    EditRange			eoReformatRange;
				/**
				 *  The selection at the beginning of the 
				 *  edit operation. If needed, it is updated 
				 *  along the way to make it possible to 
				 *  select the same content after  the 
				 *  operation.
				 */
    EditRange			eoSelectedRange;
				/**
				 *  The selection that is touched by the edit
				 *  operation. If needed, it is updated along
				 *  the way. If some cases, it may be different,
				 *  usually wider than the selected range.
				 */
    EditRange			eoAffectedRange;
				/**
				 *  In some cases, the edit operation begins
				 *  by deepening the split at the head of 
				 *  the edit operation and inserting new 
				 *  paragraphs there. In those cases, the 
				 *  head of the head of the affected range 
				 *  will not be shifted because paragraphs
				 *  are inserted.
				 */
    unsigned char		eoSplitAtHead;

    /**/
    IndexSet			eoNoteFieldsAdded;
    int				eoSectionsDeleted;
    int				eoSectionsAdded;
    int				eoParagraphsInserted;

    unsigned int		eoFieldUpdate;
				/**
				 *  A 'ReformatNeeded' enum value that tells 
				 *  what to reformat at the end of the 
				 *  operation.
				 */
    unsigned char		eoReformatNeeded;

    /**/
				/**
				  *  The document that the edit operation 
				  *  operates upon.
				  */
    BufferDocument *		eoDocument;
				/**
				  *  The tree of the document that the edit 
				  *  operation operates upon.
				  */
    DocumentTree *		eoTree;
    DOC_CLOSE_OBJECT		eoCloseObject;

				/**
				 *   The position at the head of the current 
				 *   step in the dit operation. It is at the
				 *   end of the region that will change in
				 *   the current step.
				 */
    DocumentPosition		eoHeadDp;
				/**
				 *   The position at the end of the current 
				 *   step in the dit operation. It is at the
				 *   end of the region that has been affected
				 *   by the current step.
				 */
    DocumentPosition		eoTailDp;
    DocumentPosition		eoLastDp;

				/**
				 *  To delimit a table rectangle selection.
				 *  Are reset to -1 after the first change 
				 *  to the selection.
				 */
    int				eoCol0;
    int				eoCol1;

    SimpleOutputStream *	eoTraceStream;
    struct RtfWriter *		eoTraceWriter;
    } EditOperation;

/************************************************************************/
/*									*/
/*  Routine Declarations.						*/
/*									*/
/************************************************************************/

extern void docInitEditOperation(	EditOperation *	eo );
extern void docCleanEditOperation(	EditOperation *	eo );

extern void docEditOperationGetSelection(	DocumentSelection *	dsNew,
						const EditOperation *	eo );

extern int docStartEditOperation(	EditOperation *			eo,
					const DocumentSelection *	ds,
					BufferDocument *		bd );

extern void docEditIncludeNodeInReformatRange(	EditOperation *		eo,
						struct BufferItem *	bi );

extern void docEditIncludeRowsInReformatRange(
					EditOperation *		eo,
					struct BufferItem *	sectBi,
					int			row0,
					int			row1 );

extern void docSetParagraphAdjust(	EditOperation *		eo,
					struct BufferItem *	paraBi,
					int			stroffShift,
					int			stroffUpto );

extern void docExtendParagraphAdjust(	EditOperation *		eo,
					struct BufferItem *	paraBi,
					int			stroffShift );

extern void docEditFinishStep(		EditOperation *		eo );

extern int docMoveEditOperationToBodySect(
					EditOperation *		eo,
					struct BufferItem *	bodySectNode );

extern DocumentField * docEditOperationGetSelectedNote(
					struct DocumentNote **	pDn,
					int *			pSelInNote,
					const EditOperation *	eo );

#   endif	/*	DOC_EDIT_OPERATION_H	*/
