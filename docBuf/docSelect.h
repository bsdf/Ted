#   ifndef		DOC_SELECT_H
#   define		DOC_SELECT_H

#   include		<docLayoutPosition.h>
#   include		<docSelectionScope.h>
#   include		<docObject.h>
#   include		<docEditPosition.h>
#   include		<docEditRange.h>
#   include		"docTableRectangle.h"

/************************************************************************/
/*									*/
/*  Forward declarations of structs.					*/
/*									*/
/************************************************************************/

struct BufferDocument;
struct BufferItem;
struct DocumentTree;
struct DocumentField;

/************************************************************************/
/*									*/
/*  A position in a document.						*/
/*									*/
/*  NOTE Geometry is derived from the position, NOT the other way round.*/
/*									*/
/************************************************************************/

typedef struct DocumentPosition
    {
    struct BufferItem *	dpNode;
    int			dpStroff;
    } DocumentPosition;

typedef struct DocumentSelection
    {
    SelectionScope	dsSelectionScope;

    DocumentPosition	dsHead;					/*  E  */
    DocumentPosition	dsTail;					/*  E  */
    DocumentPosition	dsAnchor;				/*  E  */

    int			dsCol0;
    int			dsCol1;
    signed char		dsDirection;
    } DocumentSelection;

#   define	docSamePosition(b,e) \
		((b)->dpNode == (e)->dpNode && (b)->dpStroff == (e)->dpStroff)

#   define	docPositionsInsideParagraph(b,e) \
		((b)->dpNode == (e)->dpNode)
#   define	docPositionsInsideCell(b,e) \
		((b)->dpNode->biParent == (e)->dpNode->biParent)
#   define	docPositionsInsideRow(b,e) \
		((b)->dpNode->biParent->biParent == (e)->dpNode->biParent->biParent)

#   define	docSelectionInsideParagraph(ds) \
		docPositionsInsideParagraph(&(ds)->dsHead,&(ds)->dsTail)
#   define	docSelectionInsideCell(ds) \
		docPositionsInsideCell(&(ds)->dsHead,&(ds)->dsTail)
#   define	docSelectionInsideRow(ds) \
		docPositionsInsideRow(&(ds)->dsHead,&(ds)->dsTail)

/************************************************************************/
/*									*/
/*  Routine Declarations.						*/
/*									*/
/************************************************************************/

extern void docInitDocumentPosition(	DocumentPosition *	dp );
extern void docInitDocumentSelection(	DocumentSelection *	ds );

extern void docSetIBarSelection(	DocumentSelection *		ds,
					const DocumentPosition *	dp );

extern int docIsIBarSelection( const DocumentSelection *		ds );
extern int docIsParaSelection( const DocumentSelection *		ds );

extern int docGetObjectSelection(
				const DocumentSelection *	ds,
				const struct BufferDocument *	bd,
				int *				pPart,
				DocumentPosition *		dpObject,
				InsertedObject **		pIo );

extern void docSetRangeSelection(
				DocumentSelection *		ds,
				const DocumentPosition *	dpHead,
				const DocumentPosition *	dpTail,
				int				direction );

extern int docLineHead(		DocumentPosition *		dp,
				int				posFlags );
extern int docLineTail(		DocumentPosition *		dp,
				int				posFlags );

extern int docSelectFrameOfPosition(	DocumentSelection *		ds,
					const DocumentPosition *	dp );

extern void docGetSelectionScope(	SelectionScope *		ss,
					const struct BufferItem *	node );

extern void docSetSelectionScope(	DocumentSelection *		ds );

extern int docSelectionSameInstance(
				const struct DocumentTree *	tree,
				int				page,
				int				column );

extern int docSelectionSameRoot(
				const DocumentSelection *	dsFrom,
				const struct BufferItem *	biTo );

extern void docSetEditPosition(	EditPosition *			ep,
				const DocumentPosition *	dp );

extern void docSetEditRange(	EditRange *			er,
				const DocumentSelection *	ds );

extern void docSetIBarRange(		EditRange *			er,
					const DocumentPosition *	dp );

extern int docGetTableRectangle(	TableRectangle *		tr,
					const DocumentSelection *	ds );

extern int docSetNodeSelection(		DocumentSelection *		ds,
					struct BufferItem *		node );

extern void docAvoidMergedTail(		DocumentSelection *		dsNew,
					EditRange *			er );

#   endif	/*	DOC_SELECT_H	*/
