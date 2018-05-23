/************************************************************************/
/*									*/
/*  Manage the lists of properties in a document.			*/
/*									*/
/************************************************************************/

#   include	"docBaseConfig.h"

#   include	"docPropertiesAdmin.h"

#   include	"docBorderPropertyAdmin.h"
#   include	"docItemShadingAdmin.h"
#   include	"docFramePropertiesAdmin.h"
#   include	"docParaRulerAdmin.h"
#   include	"docCellPropertyAdmin.h"

#   include	<textAttributeAdmin.h>

#   include	<stdlib.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Clean and free DocumentPropertyLists				*/
/*									*/
/************************************************************************/

void docFreeDocumentPropertyLists( DocumentPropertyLists * dpl )
    {
    utilCleanNumberedPropertiesList( &(dpl->dplTextAttributeList) );
    utilCleanNumberedPropertiesList( &(dpl->dplBorderPropertyList) );
    utilCleanNumberedPropertiesList( &(dpl->dplItemShadingList) );
    utilCleanNumberedPropertiesList( &(dpl->dplFramePropertyList) );
    utilCleanNumberedPropertiesList( &(dpl->dplTabStopListList) );
    utilCleanNumberedPropertiesList( &(dpl->dplCellPropertyList) );

    docCleanFontList( &(dpl->dplFontList) );
    docCleanListAdmin( &(dpl->dplListAdmin) );
    utilCleanColorPalette( &(dpl->dplColorPalette) );

    free( dpl );
    }

/************************************************************************/
/*									*/
/*  Allocate DocumentPropertyLists					*/
/*									*/
/************************************************************************/

DocumentPropertyLists * docMakeDocumentPropertyLists( void )
    {
    DocumentPropertyLists * dpl= malloc( sizeof(DocumentPropertyLists) );
    if  ( ! dpl )
	{ XDEB(dpl); return (DocumentPropertyLists *)0;	}

    utilInitTextAttributeList( &(dpl->dplTextAttributeList) );
    docInitBorderPropertyList( &(dpl->dplBorderPropertyList) );
    docInitItemShadingList( &(dpl->dplItemShadingList) );
    docInitFramePropertyList( &(dpl->dplFramePropertyList) );
    docInitTabStopListList( &(dpl->dplTabStopListList) );
    docInitCellPropertyList( &(dpl->dplCellPropertyList) );

    docInitFontList( &(dpl->dplFontList) );
    docInitListAdmin( &(dpl->dplListAdmin) );
    utilInitColorPalette( &(dpl->dplColorPalette) );

    return dpl;
    }

