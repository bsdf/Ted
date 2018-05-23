/************************************************************************/
/*									*/
/*  A list of objects that are inserted into the document.		*/
/*  Currently, this simply is a wrapper around a list. In the future,	*/
/*  we might want to include optimisations for efficient redraws.	*/
/*									*/
/************************************************************************/

#   ifndef DOC_INSERTED_OBJECT_LIST_H
#   define DOC_INSERTED_OBJECT_LIST_H

#   include	<utilPagedList.h>

struct InsertedObject;

typedef struct InsertedObjectList
    {
    PagedList		iolPagedList;
    } InsertedObjectList;

extern struct InsertedObject * docGetObjectByNumber(
					const InsertedObjectList *	iol,
					int				n );

extern void docCleanObjectList(		InsertedObjectList *		iol );
extern void docInitObjectList(		InsertedObjectList *		iol );

extern struct InsertedObject * docClaimInsertedObject(
					int *				pNr,
					InsertedObjectList *		iol );

#   endif /*  DOC_INSERTED_OBJECT_LIST_H  */
