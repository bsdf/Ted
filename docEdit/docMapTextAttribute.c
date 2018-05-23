/************************************************************************/
/*									*/
/*  Change the font of spans of text.					*/
/*									*/
/************************************************************************/

#   include	"docEditConfig.h"

#   include	<stdio.h>

#   include	<appDebugon.h>

#   include	<docExpandedTextAttribute.h>

#   include	<docBuf.h>
#   include	"docDocumentCopyJob.h"

/************************************************************************/
/*									*/
/*  Map text attribute numbers from one document to another document.	*/
/*									*/
/************************************************************************/

void docMapTextAttribute(	TextAttribute *		taTo,
				const TextAttribute *	taFrom,
				const DocumentCopyJob *	dcj )
    {
    const DocumentAttributeMap *	dam= &(dcj->dcjAttributeMap);

    *taTo= *taFrom;

    if  ( taFrom->taFontNumber >= 0 && dam->damFontMap )
	{
	taTo->taFontNumber= dam->damFontMap[taFrom->taFontNumber];
	}

    if  ( taFrom->taTextColorNumber > 0 && dam->damColorMap )
	{
	taTo->taTextColorNumber= dam->damColorMap[taFrom->taTextColorNumber];
	}

    taTo->taTextStyleNumber= 0;

    if  ( taFrom->taBorderNumber > 0 && dam->damBorderMap )
	{
	taTo->taBorderNumber= dam->damBorderMap[taFrom->taBorderNumber];
	}

    if  ( taFrom->taShadingNumber > 0 && dam->damShadingMap )
	{
	taTo->taShadingNumber= dam->damShadingMap[taFrom->taShadingNumber];
	}

    return;
    }

int docMapTextAttributeNumber(	DocumentCopyJob *	dcj,
				int			attributeNumberFrom )
    {
    EditOperation *		eo= dcj->dcjEditOperation;
    TextAttribute		taTo;
    TextAttribute		taFrom;

    int				attributeNumberTo;

    if  ( attributeNumberFrom < 0 )
	{ LDEB(attributeNumberFrom); return attributeNumberFrom;	}

    if  ( eo->eoDocument == dcj->dcjSourceDocument )
	{ return attributeNumberFrom;	}

    if  ( dcj->dcjForceAttributeTo >= 0 )
	{ return dcj->dcjForceAttributeTo;	}

    if  ( attributeNumberFrom == dcj->dcjCurrentTextAttributeNumberFrom )
	{ return dcj->dcjCurrentTextAttributeNumberTo;	}

    docGetTextAttributeByNumber( &taFrom, dcj->dcjSourceDocument,
							attributeNumberFrom );

    docMapTextAttribute( &taTo, &taFrom, dcj );

    attributeNumberTo= docTextAttributeNumber( eo->eoDocument, &taTo );

    if  ( attributeNumberTo < 0 )
	{ LDEB(attributeNumberTo);	}

    dcj->dcjCurrentTextAttributeNumberFrom= attributeNumberFrom;
    dcj->dcjCurrentTextAttributeNumberTo= attributeNumberTo;

    return attributeNumberTo;
    }

int docMapTextAttributeNumberFromTo(
				BufferDocument *	bdTo,
				const BufferDocument *	bdFrom,
				int			attributeNumberFrom )
    {
    int				rval= -1;

    TextAttribute		ta;
    ExpandedTextAttribute	eta;

    PropertyMask		doneMask;
    PropertyMask		setMask;

    docInitExpandedTextAttribute( &eta );

    docGetTextAttributeByNumber( &ta, bdFrom, attributeNumberFrom );

    utilPropMaskFill( &setMask, TAprop_COUNT );
    utilPropMaskClear( &doneMask );
    docExpandTextAttribute( &doneMask, &eta, &ta, &setMask,
				    bdFrom->bdProperties.dpFontList,
				    bdFrom->bdProperties.dpColorPalette );

    utilPropMaskFill( &setMask, TAprop_COUNT );
    utilPropMaskClear( &doneMask );

    docIndirectTextAttribute( &doneMask, &ta, &eta, &setMask,
				    bdTo->bdProperties.dpFontList,
				    bdTo->bdProperties.dpColorPalette );

    rval= docTextAttributeNumber( bdTo, &ta );

    docCleanExpandedTextAttribute( &eta );

    return rval;
    }
