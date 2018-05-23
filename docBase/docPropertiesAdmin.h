/************************************************************************/
/*									*/
/*  Manage the lists of properties in a document.			*/
/*									*/
/************************************************************************/

#   ifndef	DOC_PROPERTY_ADMIN_H
#   define	DOC_PROPERTY_ADMIN_H

#   include	<utilNumberedPropertiesAdmin.h>
#   include	<utilDocFontList.h>
#   include	<utilPalette.h>

#   include	"docListAdmin.h"

typedef struct DocumentPropertyLists
    {
    NumberedPropertiesList	dplTextAttributeList;
    NumberedPropertiesList	dplBorderPropertyList;
    NumberedPropertiesList	dplItemShadingList;
    NumberedPropertiesList	dplFramePropertyList;
    NumberedPropertiesList	dplTabStopListList;
    NumberedPropertiesList	dplCellPropertyList;

    DocumentFontList		dplFontList;
    ListAdmin			dplListAdmin;
    ColorPalette		dplColorPalette;
    } DocumentPropertyLists;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern DocumentPropertyLists * docMakeDocumentPropertyLists( void );

extern void docFreeDocumentPropertyLists( DocumentPropertyLists * dpl );

#   endif	/*  DOC_PROPERTY_ADMIN_H	*/
