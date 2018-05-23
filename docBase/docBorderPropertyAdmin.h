/************************************************************************/
/*									*/
/*  Manage a lookup to keep track of border properties.			*/
/*									*/
/************************************************************************/

#   ifndef	DOC_BORDER_PROPERTY_ADMIN_H
#   define	DOC_BORDER_PROPERTY_ADMIN_H

#   include	"docBorderProperties.h"
#   include	<utilIntegerValueNode.h>
#   include	<utilNumberedPropertiesAdmin.h>

typedef void (*BorderPropertiesFunction)( const BorderProperties * bp,
					int			n,
					void *			through );

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docInitBorderPropertyList(	NumberedPropertiesList *	bpl );

extern int docBorderPropertiesNumberImpl( NumberedPropertiesList *	bpl,
					const BorderProperties *	bp );

extern void docForAllBorderProperties(
				const NumberedPropertiesList *	bpl,
				BorderPropertiesFunction	f,
				void *				through );

extern void docGetBorderPropertiesByNumberImpl(
					BorderProperties *		bp,
					const NumberedPropertiesList *	bpl,
					int				n );

extern int docBorderNumberIsBorderImpl(	const NumberedPropertiesList *	bpl,
					int				n );

extern int docMergeBorderPropertiesLists(
				int **				pBorderMap,
				const int *			colorMap,
				NumberedPropertiesList *	bplTo,
				const NumberedPropertiesList *	bplFrom );

#   endif	/*  DOC_BORDER_PROPERTY_ADMIN_H	*/
