/************************************************************************/
/*									*/
/*  Manage a lookup to keep track of the different possible rulers for	*/
/*  a paragraph								*/
/*									*/
/************************************************************************/

#   ifndef	DOC_PARA_RULER_ADMIN_H
#   define	DOC_PARA_RULER_ADMIN_H

#   include	"docTabStop.h"
#   include	"docTabStopList.h"
#   include	<utilIntegerValueNode.h>
#   include	<utilNumberedPropertiesAdmin.h>

typedef void (*TabStopListFunction)( 	const TabStopList * 	tsl,
					int			n,
					void *			through );

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docInitTabStopListList(	NumberedPropertiesList *	tsll );

extern int docTabStopListNumberImpl(	NumberedPropertiesList *	tsll,
					const TabStopList *		tsl );

extern void docGetTabStopListByNumberImpl( TabStopList *		tsl,
					const NumberedPropertiesList *	tsll,
					int				n );

extern int docMergeTabstopListLists(
				int **				pRulerMap,
				NumberedPropertiesList *	tsllTo,
				const NumberedPropertiesList *	tsllFrom );

#   endif	/*  DOC_PARA_RULER_ADMIN_H	*/
