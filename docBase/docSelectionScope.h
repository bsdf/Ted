#   ifndef		DOC_SELECTION_SCOPE_H
#   define		DOC_SELECTION_SCOPE_H

/************************************************************************/
/*									*/
/*  Scope of a selection.						*/
/*									*/
/*  1)  Indication for the kind of root item for the selection.		*/
/*  2)  Geometry is derived from the position, NOT the other way round.	*/
/*									*/
/*  A)  Indication for the kind of root item for the selection.		*/
/*  B)  When the selection is not in the body of the document, the	*/
/*	number of the section that the external item belongs to.	*/
/*  D)  If the document tree is a footnote/endnote: The number of the	*/
/*	note field in the body that owns the note.			*/
/*  D)  If the document tree is a drawing shape: The number of the	*/
/*	shape in the documents shape administration.			*/
/*									*/
/************************************************************************/

typedef struct SelectionScope
    {
    int			ssTreeType;				/*  A  */
    int			ssSectNr;				/*  B  */
    int			ssOwnerSectNr;				/*  B  */
    int			ssOwnerNumber;				/*  D  */
    } SelectionScope;

typedef enum SelectionScopeProperty
    {
    SSpropTREE_TYPE= 0,
    SSpropSECT_NR,
    SSpropOWNER_SECT_NR,
    SSpropOWNER_NR,
    SSpropPAGE,
    SSpropCOLUMN,

    SSprop_COUNT
    } SelectionScopeProperty;

/************************************************************************/
/*									*/
/*  Routine Declarations.						*/
/*									*/
/************************************************************************/

extern void docInitSelectionScope(	SelectionScope *	ss );

extern int docSelectionSameScope(
				const SelectionScope *		ssFrom,
				const SelectionScope *		ssTo );

#   endif	/*	DOC_SELECTION_SCOPE_H	*/
