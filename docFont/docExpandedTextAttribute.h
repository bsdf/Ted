/************************************************************************/
/*									*/
/*  Text attributes without a reference to palettes or font lists.	*/
/*									*/
/************************************************************************/

#   ifndef	DOC_EXPANDED_TEXT_ATTRIBUTE_H
#   define	DOC_EXPANDED_TEXT_ATTRIBUTE_H

#   include	<textAttribute.h>
#   include	"utilDocFontList.h"
#   include	<utilColor.h>
#   include	<utilPalette.h>

typedef struct ExpandedTextAttribute
    {
    TextAttribute	etaTextAttribute;

    RGB8Color		etaTextColor;
    int			etaTextColorExplicit;

    char *		etaFontName;
    } ExpandedTextAttribute;

/************************************************************************/
/*									*/
/*  Routine declarations						*/
/*									*/
/************************************************************************/

extern void docInitExpandedTextAttribute(	ExpandedTextAttribute * eta );
extern void docCleanExpandedTextAttribute(	ExpandedTextAttribute * eta );

extern int docExpandedTextAttributeSetFontName(
				ExpandedTextAttribute *		etaTo,
				int *				pChanged,
				const char *			fontName );

extern int docUpdateExpandedTextAttribute(
				PropertyMask *			pDoneMask,
				ExpandedTextAttribute *		etaTo,
				const ExpandedTextAttribute *	etaFrom,
				const PropertyMask *		setMask );

extern int docCopyExpandedTextAttribute(
				ExpandedTextAttribute *		etaTo,
				const ExpandedTextAttribute *	etaFrom );

extern int docExpandTextAttribute(
				PropertyMask *			pDoneMask,
				ExpandedTextAttribute *		etaTo,
				const TextAttribute *		taFrom,
				const PropertyMask *		setMask,
				const DocumentFontList *	dfl,
				const ColorPalette *		cp );

extern void docIndirectTextAttribute(
				PropertyMask *			pDoneMask,
				TextAttribute *			taTo,
				const ExpandedTextAttribute *	etaFrom,
				const PropertyMask *		setMask,
				DocumentFontList *		dfl,
				ColorPalette *			cp );

#   endif	/*  DOC_EXPANDED_TEXT_ATTRIBUTE_H  */
