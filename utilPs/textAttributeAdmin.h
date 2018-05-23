/************************************************************************/
/*									*/
/*  Manage a lookup to keep track of text attributes.			*/
/*									*/
/************************************************************************/

#   ifndef	UTIL_TEXT_ATTRIBUTE_ADMIN_H
#   define	UTIL_TEXT_ATTRIBUTE_ADMIN_H

#   include	"textAttribute.h"
#   include	<utilNumberedPropertiesAdmin.h>

typedef int (*TextAttributeFunction)(	int			n,
					const TextAttribute *	ta,
					void *			through );

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void utilInitTextAttributeList(	NumberedPropertiesList *	tal );

extern int utilTextAttributeNumber(	NumberedPropertiesList *	tal,
					const TextAttribute *		ta );

extern void utilGetTextAttributeByNumber(
					TextAttribute *			ta,
					const NumberedPropertiesList *	tal,
					int				n );

extern int textForAllAttributesInList(
				const NumberedPropertiesList *	tal,
				const IndexSet *		filter,
				TextAttributeFunction		f,
				void *				through );

#   endif	/*  UTIL_TEXT_ATTRIBUTE_ADMIN_H	*/
