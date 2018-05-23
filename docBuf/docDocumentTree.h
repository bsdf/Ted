/************************************************************************/
/*									*/
/*  Pieces of text, physically implemented as a 'Section' thay are	*/
/*  outside the normal flow of text.					*/
/*									*/
/************************************************************************/

#   ifndef	DOC_EXTERNAL_ITEM_H
#   define	DOC_EXTERNAL_ITEM_H

#   include	<docDocumentField.h>
#   include	<docListNumberTree.h>

/************************************************************************/
/*									*/
/*  Forward declarations of structs.					*/
/*									*/
/************************************************************************/

struct BufferDocument;
struct BufferItem;
struct DocumentField;

/************************************************************************/
/*									*/
/*  Headers and/or footers appear on different pages, depending on	*/
/*  whether the section has a title page and/or the document has facing	*/
/*  pages. These are the options as they exist on the tool. In my	*/
/*  opinion these are also the possibilities from the users perspective.*/
/*									*/
/*  The arrays are a mapping from the options to the header/footer data	*/
/*  structure that is used in the implementation.			*/
/*									*/
/************************************************************************/

typedef enum WhatPages
    {
    PAGES_FIRST_PAGE= 0,
    PAGES_SUBSEQUENT_PAGES,
    PAGES_ALL_PAGES,
    PAGES_ODD_PAGES,
    PAGES_EVEN_PAGES,
    PAGES__COUNT
    } WhatPages;

extern const int DOC_HeaderScopes[PAGES__COUNT];
extern const int DOC_FooterScopes[PAGES__COUNT];

/************************************************************************/
/*									*/
/*  A structure describing an external item.				*/
/*									*/
/************************************************************************/

typedef struct DocumentTree
    {
    struct BufferItem *		dtRoot;
    int				dtPageFormattedFor;
    int				dtColumnFormattedFor;
    int				dtY0UsedTwips;
    int				dtY1UsedTwips;
				/**
				 *  The top position that is reserved for the 
				 *  tree. For the page footers, this also 
				 *  determines the page bottom for the body 
				 *  text and the notes.
				 */
    int				dtY0ReservedTwips;
    int				dtY1ReservedTwips;

    int				dtPageSelectedUpon;
    int				dtColumnSelectedIn;

    ListNumberTrees		dtListNumberTrees;
    ListNumberTreeNode		dtOutlineTree;

    ChildFields			dtRootFields;
    } DocumentTree;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docInitDocumentTree(	DocumentTree *		dt );

extern void docCleanDocumentTree(	struct BufferDocument *	bd,
					DocumentTree *		dt );

extern void docInvalidateTreeLayout(	DocumentTree *		dt );

extern int docAddRootFieldToTree(	DocumentTree *		dt,
					DocumentField *		dfCh );

#   endif
