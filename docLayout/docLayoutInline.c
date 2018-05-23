/************************************************************************/
/*									*/
/*  Layout of a document. Do the layout of the particules inside the	*/
/*  line.								*/
/*									*/
/************************************************************************/

#   include	"docLayoutConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>

#   include	<psFontMetrics.h>
#   include	"docLayout.h"

#   include	<appDebugon.h>
#   include	<docObjectProperties.h>
#   include	<docTextLine.h>
#   include	<docTreeNode.h>
#   include	<docBorderPropertyAdmin.h>
#   include	<docShape.h>

/************************************************************************/
/*									*/
/*  Get font information.						*/
/*									*/
/************************************************************************/

const AfmFontInfo * docLayoutGetAfi(
			int *				pTextAttrNr,
			TextAttribute *			ta,
			unsigned char *			pLineFlags,
			const LayoutContext *		lc,
			const BufferItem *		paraNode,
			int				part )
    {
    BufferDocument *		bd= lc->lcDocument;

    DocumentProperties *	dp= &(bd->bdProperties);
    DocumentFontList *		dfl= dp->dpFontList;

    const AfmFontInfo *		afi;
    const IndexSet *		unicodesWanted;
    int				textAttrNr;

    textAttrNr= docGetEffectiveTextAttributes( ta, bd, paraNode, part );

    afi= (*lc->lcGetFontForAttribute)( &unicodesWanted,
					ta, dfl, lc->lcPostScriptFontList );
    if  ( ! afi )
	{ LDEB(textAttrNr); return (AfmFontInfo *)0; }

    if  ( docBorderNumberIsBorder( bd, ta->taBorderNumber ) )
	{ *pLineFlags |= TLflagBORDER;	}

    if  ( docShadingNumberIsShading( bd, ta->taShadingNumber ) )
	{ *pLineFlags |= TLflagSHADING;	}

    *pTextAttrNr= textAttrNr;
    return afi;
    }

/************************************************************************/
/*									*/
/*  Get descent/ascent for the font of a special particule.		*/
/*									*/
/************************************************************************/

int docLayoutFontAscDesc(	const BufferItem *		paraNode,
				TextAttribute *			ta,
				DocumentRectangle *		drAscDesc,
				int *				pBorder,
				const LayoutContext *		lc,
				int				part )
    {
    BufferDocument *		bd= lc->lcDocument;

    int				rval;
    DocumentRectangle		drBBox;

    const AfmFontInfo *		afi;
    BorderProperties		bp;

    int				sizeTwips;
    const int			vswap= 1;

    unsigned char		lineFlags= 0;
    int				textAttrNr;

    afi= docLayoutGetAfi( &textAttrNr, ta, &lineFlags, lc, paraNode, part );
    if  ( ! afi )
	{ XDEB(afi); return -1;	}

    sizeTwips= 10* ta->taFontSizeHalfPoints;

    rval= psFontBBox( &drBBox, drAscDesc, sizeTwips, vswap, afi );

    if  ( pBorder && ( lineFlags & TLflagBORDER ) )
	{
	int		thick= 0;
	int		space= 0;

	docGetBorderPropertiesByNumber( &bp, bd, ta->taBorderNumber );

	thick= docBorderThick( &space, &bp );

	*pBorder= thick+ space;
	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Calculate the layout of the next stretch of text upto the point	*/
/*  where it can be folded.						*/
/*									*/
/************************************************************************/

static int docLayoutSegmentedStringExtents(
				DocumentRectangle *	drRet,
				const char *		printString,
				const int *		segments,
				int			segmentCount,
				int			sizeTwips,
				const AfmFontInfo *	afi )
    {
    const char *		s= printString;
    int				seg;
    int				scapsSize= ( 8* sizeTwips )/ 10;

    int				x= 0;
    int				wide;

    DocumentRectangle		dr;
    int				done= 0;

    const int			withKerning= 0;
    const int			vswap= 1;

    for ( seg= 0; seg < segmentCount; seg++ )
	{
	if  ( segments[2* seg+ 0] > 0 )
	    {
	    wide= psCalculateStringExtents( &dr, s, segments[2* seg+ 0],
					sizeTwips, withKerning, vswap, afi );

	    if  ( done )
		{
		drRet->drX1= x+ dr.drX1;

		includeRectangleY( drRet, &dr );
		}
	    else{ *drRet= dr;	}

	    s += segments[2* seg+ 0];
	    x += wide;
	    done= 1;
	    }

	if  ( segments[2* seg+ 1] > 0 )
	    {
	    wide= psCalculateStringExtents( &dr, s, segments[2* seg+ 1],
					scapsSize, withKerning, vswap, afi );

	    if  ( done )
		{
		drRet->drX1= x+ dr.drX1;

		includeRectangleY( drRet, &dr );
		}
	    else{ *drRet= dr;	}

	    s += segments[2* seg+ 1];
	    x += wide;
	    done= 1;
	    }
	}

    return x;
    }

int docLayoutStringExtents(	int *			pWidth,
				int *			pDecWidth,
				DocumentRectangle *	dr,
				const TextAttribute *	ta,
				int			sizeTwips,
				const BufferDocument *	bd,
				const AfmFontInfo *	afi,
				const char *		printString,
				int			len )
    {
    char *			upperString= (char *)0;
    int *			segments= (int *)0;
    int				segmentCount= 0;

    int				rval= 0;
    const int			withKerning= 0;
    const int			vswap= 1;

    int				dec;
    int				d;

    int				width;
    int				fontHigh;
    int				decWidth;

    DocumentRectangle		drFontBBox;
    DocumentRectangle		drFontAscDesc;

    if  ( ! printString )
	{ printString= "";	}

    if  ( len > 0 && ( ta->taSmallCaps || ta->taCapitals ) )
	{
	if  ( docMakeCapsString( &upperString, &segments, &segmentCount,
						    ta, printString, len ) )
	    { LDEB(len); return -1;	}

	if  ( ! upperString )
	    { XDEB(upperString); return -1;	}

	printString= upperString;
	}

    if  ( len > 0 && ta->taSmallCaps && ! ta->taCapitals )
	{
	width= docLayoutSegmentedStringExtents( dr, printString,
				    segments, segmentCount, sizeTwips, afi );
	if  ( width < 0 )
	    { LLDEB(ta->taFontNumber,width);	}
	}
    else{
	width= psCalculateStringExtents( dr, printString, len,
					sizeTwips, withKerning, vswap, afi );
	if  ( width < 0 )
	    { LSLDEB(len,printString,width);	}
	}

    (void)psFontBBox( &drFontBBox, &drFontAscDesc, sizeTwips, vswap, afi );
    dr->drY0= drFontAscDesc.drY0;
    dr->drY1= drFontAscDesc.drY1;

    /*LINEDISTANCE: scale the position of the baseline based on the bbox */
    fontHigh= drFontBBox.drY1- drFontBBox.drY0;
    if  ( fontHigh < 2 )
	{ LLDEB(ta->taFontSizeHalfPoints,fontHigh); fontHigh= 2;	}
    dr->drY0= ( drFontBBox.drY0* sizeTwips )/ fontHigh;
    dr->drY1= ( drFontBBox.drY1* sizeTwips )/ fontHigh;

    dec= -1;
    for ( d= 0; d < len; d++ )
	{
	if  ( printString[d] == '.' || printString[d] == ','	)
	    { dec= d;	}
	}

    if  ( dec >= 0 )
	{
	DocumentRectangle		drDec;

	decWidth= psCalculateStringExtents( &drDec, printString, dec,
					sizeTwips, withKerning, vswap, afi );
	if  ( decWidth < 0 )
	    { LDEB(decWidth);	}
	}
    else{ decWidth= width;	}

    *pWidth= width;
    *pDecWidth= decWidth;

    if  ( upperString )
	{ free( upperString );	}
    if  ( segments )
	{ free( segments );	}

    return rval;
    }

/************************************************************************/

int docLayoutInlineObject(	TextAttribute *			ta,
				BufferDocument *		bd,
				const struct BufferItem *	paraNode,
				int				part,
				InsertedObject *		io,
				DocumentRectangle *		drWord,
				int *				pWordBorder,
				int *				pX1,
				int				x0 )
    {
    const int			accepted= 1;

    BorderProperties		bp;
    int				thick= 0;
    int				space= 0;

    int				width;

    if  ( io->ioKind == DOCokDRAWING_SHAPE )
	{
	DrawingShape *		ds= io->ioDrawingShape;
	const ShapeProperties *	sp= &(ds->dsShapeProperties);

	if  ( sp->spWrapStyle == SHPswtNONE	||
	      sp->spWrapStyle == SHPswtTHROUGH	)
	    {
	    drWord->drX0= x0;
	    drWord->drX1= x0;
	    drWord->drY0= 0;
	    drWord->drY1= 0;

	    *pWordBorder= 0;

	    *pX1= x0;
	    return accepted;
	    }
	}

    width= ( io->ioScaleXUsed* io->ioTwipsWide )/ 100.0;

    docGetEffectiveTextAttributes( ta, bd, paraNode, part );
    docGetBorderPropertiesByNumber( &bp, bd, ta->taBorderNumber );

    if  ( DOCisBORDER( &bp ) )
	{ thick= docBorderThick( &space, &bp );	}

    drWord->drX0= x0;
    drWord->drX1= x0+ width;
    drWord->drY0= -( io->ioScaleYUsed* io->ioTwipsHigh )/ 100.0;
    drWord->drY1= 0;

    *pWordBorder= thick+ space;

    *pX1= x0+ width;
    return accepted;
    }

