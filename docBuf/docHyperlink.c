/************************************************************************/
/*									*/
/*  Calculate 'Hyperlink' fields.					*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	<appDebugon.h>

#   include	"docBuf.h"
#   include	"docField.h"

/************************************************************************/
/*									*/
/*  Determine the attribute change to set a hyperlink.			*/
/*									*/
/************************************************************************/

int docSetHyperlinkAttribute(	TextAttribute *		taSet,
				PropertyMask *		taSetMask,
				BufferDocument *	bd )
    {
    DocumentProperties *		dp= &(bd->bdProperties);
    int					blue;
    RGB8Color				rgb8Blue;

    docPlainTextAttribute( taSet, bd );
    utilPropMaskClear( taSetMask );

    utilInitRGB8Color( &rgb8Blue );
    rgb8Blue.rgb8Red= 0;
    rgb8Blue.rgb8Green= 0;
    rgb8Blue.rgb8Blue= 255;

    blue= docAllocateDocumentColor( dp, &rgb8Blue );
    if  ( blue < 0 )
	{ LDEB(blue); return -1;	}

    taSet->taTextColorNumber= blue;
    taSet->taTextIsUnderlined= 1;
    PROPmaskADD( taSetMask, TApropTEXT_COLOR );
    PROPmaskADD( taSetMask, TApropTEXTUNDERLINED );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Determine the attribute change to remove a hyperlink.		*/
/*									*/
/************************************************************************/

int docRemoveHyperlinkAttribute(	TextAttribute *		taSet,
					PropertyMask *		taSetMask,
					BufferDocument *	bd )
    {
    docPlainTextAttribute( taSet, bd );
    utilPropMaskClear( taSetMask );

    taSet->taTextColorNumber= 0;
    taSet->taTextIsUnderlined= 0;

    PROPmaskADD( taSetMask, TApropTEXT_COLOR );
    PROPmaskADD( taSetMask, TApropTEXTUNDERLINED );

    return 0;
    }

