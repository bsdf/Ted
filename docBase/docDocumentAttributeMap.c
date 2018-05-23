/************************************************************************/
/*									*/
/*  Map numbered attributes in one document to those in anaother one.	*/
/*									*/
/************************************************************************/

#   include	"docBaseConfig.h"

#   include	<stdlib.h>

#   include	<appDebugon.h>

#   include	"docDocumentAttributeMap.h"

void docInitDocumentAttributeMap(	DocumentAttributeMap *	dam )
    {
    dam->damFontMap= (int *)0;
    dam->damColorMap= (int *)0;
    dam->damRulerMap= (int *)0;
    dam->damBorderMap= (int *)0;
    dam->damShadingMap= (int *)0;
    dam->damCellMap= (int *)0;
    dam->damFrameMap= (int *)0;
    dam->damListStyleMap= (int *)0;

    return;
    }

void docCleanDocumentAttributeMap(	DocumentAttributeMap *	dam )
    {
    if  ( dam->damFontMap )
	{ free( dam->damFontMap );	}

    if  ( dam->damColorMap )
	{ free( dam->damColorMap );	}

    if  ( dam->damRulerMap )
	{ free( dam->damRulerMap );	}

    if  ( dam->damBorderMap )
	{ free( dam->damBorderMap );	}

    if  ( dam->damShadingMap )
	{ free( dam->damShadingMap );	}

    if  ( dam->damCellMap )
	{ free( dam->damCellMap );	}

    if  ( dam->damFrameMap )
	{ free( dam->damFrameMap );	}

    if  ( dam->damListStyleMap )
	{ free( dam->damListStyleMap );	}

    return;
    }

