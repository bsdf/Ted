/************************************************************************/
/*									*/
/*  Find a screen font with a font.					*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>

#   include	<appMatchFont.h>

#   include	<docLayout.h>
#   include	<docScreenLayout.h>
#   include	<textAttributeUtil.h>
#   include	<drawDrawingSurface.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Find/Open the screen font belonging to an attribute number.		*/
/*									*/
/************************************************************************/

int docOpenScreenFont(	const LayoutContext *	lc,
			int			textAttrNr )
    {
    int		screenFont;

    screenFont= utilIndexMappingGet( lc->lcAttributeToScreenFont,
							    textAttrNr );
    if  ( screenFont < 0 )
	{
	BufferDocument *	bd= lc->lcDocument;
	DocumentFontList *	dfl= bd->bdProperties.dpFontList;
	const AfmFontInfo *	afi;
	const IndexSet *	unicodesWanted;
	TextAttribute		ta;
	int			pixelSize;

	docGetTextAttributeByNumber( &ta, bd, textAttrNr );

	afi= appGetFontInfoForAttribute( &unicodesWanted, &ta, dfl,
						    lc->lcPostScriptFontList );
	if  ( ! afi )
	    { LXDEB(ta.taFontNumber,afi); return -1;	}

	pixelSize= textGetPixelSize( lc->lcPixelsPerTwip, &ta );
	screenFont= drawOpenScreenFont( lc->lcDrawingSurface, afi,
						pixelSize, unicodesWanted );
	if  ( screenFont < 0 )
	    { LLDEB(textAttrNr,screenFont); return -1;	}

	if  ( utilIndexMappingPut( lc->lcAttributeToScreenFont,
					    textAttrNr, screenFont ) < 0 )
	    { LLDEB(textAttrNr,screenFont); return -1;	}
	}

    return screenFont;
    }

