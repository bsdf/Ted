/************************************************************************/
/*									*/
/*  Manage text attributes while saving an RTF file.			*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	<appDebugon.h>

#   include	<uniUtf8.h>
#   include	<utilMatchFont.h>
#   include	<psShading.h>
#   include	"docRtfWriterImpl.h"
#   include	"docRtfFlags.h"
#   include	"docRtfTags.h"
#   include	<textConverter.h>

/************************************************************************/
/*									*/
/*  Save a text attribute (change).					*/
/*									*/
/************************************************************************/

static void docRtfSaveTextAttributeImpl(	RtfWriter *		rw,
						const PropertyMask *	updMask,
						const TextAttribute *	ta )
    {
    if  ( PROPmaskISSET( updMask, TApropTEXT_STYLE ) )
	{ docRtfWriteArgTag( rw, "cs", ta->taTextStyleNumber ); }

    if  ( PROPmaskISSET( updMask, TApropFONT_NUMBER ) )
	{ docRtfWriteArgTag( rw, "f", ta->taFontNumber ); }

    if  ( PROPmaskISSET( updMask, TApropFONTSIZE ) )
	{ docRtfWriteArgTag( rw, "fs", ta->taFontSizeHalfPoints ); }

    if  ( PROPmaskISSET( updMask, TApropTEXT_COLOR ) )
	{ docRtfWriteArgTag( rw, "cf", ta->taTextColorNumber ); }

    if  ( PROPmaskISSET( updMask, TApropBORDER ) )
	{
	const int	anyway= 1;

	docRtfSaveBorderByNumber( rw, "chbrdr",
					    ta->taBorderNumber, anyway );
	}

    if  ( PROPmaskISSET( updMask, TApropSHADING ) )
	{
	const BufferDocument *		bd= rw->rwDocument;
	ItemShading			is;

	docGetItemShadingByNumber( &is, bd, ta->taShadingNumber );

	if  ( is.isBackColor > 0		&&
	      is.isPattern == DOCspSOLID	&&
	      is.isLevel == 0			)
	    { docRtfWriteArgTag( rw, "cb", is.isBackColor );	}

	docRtfSaveShadingByNumber( rw, ta->taShadingNumber,
		DOCrtf_TextShadingTags, DOCrtf_TextShadingTagCount,
		"chcfpat", "chcbpat", "chshdng" );
	}

    if  ( PROPmaskISSET( updMask, TApropFONTBOLD ) )
	{ docRtfWriteFlagTag( rw, "b", ta->taFontIsBold );	}

    if  ( PROPmaskISSET( updMask, TApropFONTSLANTED ) )
	{ docRtfWriteFlagTag( rw, "i", ta->taFontIsSlanted );	}

    if  ( PROPmaskISSET( updMask, TApropTEXTUNDERLINED ) )
	{ docRtfWriteFlagTag( rw, "ul", ta->taTextIsUnderlined );	}

    if  ( PROPmaskISSET( updMask, TApropSUPERSUB ) )
	{
	docRtfWriteEnumTag( rw, DOCrtf_SupersubTags, ta->taSuperSub,
				    DOCrtf_SupersubTagCount, TEXTva_COUNT );
	}

    if  ( PROPmaskISSET( updMask, TApropBASELINE_SHIFT ) )
	{
	if  ( ta->taBaselineShiftHalfPoints >= 0 )
	    { docRtfWriteArgTag( rw, "up",  ta->taBaselineShiftHalfPoints ); }
	else{ docRtfWriteArgTag( rw, "dn", -ta->taBaselineShiftHalfPoints ); }
	}

    if  ( PROPmaskISSET( updMask, TApropSMALLCAPS ) )
	{ docRtfWriteFlagTag( rw, "scaps", ta->taSmallCaps );	}

    if  ( PROPmaskISSET( updMask, TApropCAPITALS ) )
	{ docRtfWriteFlagTag( rw, "caps", ta->taCapitals );	}

    if  ( PROPmaskISSET( updMask, TApropSTRIKETHROUGH ) )
	{ docRtfWriteFlagTag( rw, "strike", ta->taHasStrikethrough );	}

    return;
    }

void docRtfSaveTextAttribute(		RtfWriter *		rw,
					const PropertyMask *	updMask,
					const TextAttribute *	taDoc )
    {
    TextAttribute	taFile= *taDoc;

    if  ( ! ( rw->rwSaveFlags & RTFflagUNENCODED )		&&
	  PROPmaskISSET( updMask, TApropFONT_NUMBER )	)
	{
	const DocumentProperties *	dp= &(rw->rwDocument->bdProperties);
	const DocumentFontList *	dfl= dp->dpFontList;
	const DocumentFont *		df;

	df= docFontListGetFontByNumber( dfl, taFile.taFontNumber );
	if  ( df )
	    {
	    int			fileFontNumber;
	    int			charset= FONTcharsetDEFAULT;
	    const int		symbol= ' ';

	    fileFontNumber= docRtfWriteGetCharset( rw, &charset, df, symbol );
	    if  ( fileFontNumber < 0 )
		{ SLXDEB(df->dfName,fileFontNumber,symbol);	}
	    else{ taFile.taFontNumber= fileFontNumber;		}
	    }
	}

    docRtfSaveTextAttributeImpl( rw, updMask, &taFile );
    }

/************************************************************************/
/*									*/
/*  Finish/Begin writing a destination.					*/
/*									*/
/************************************************************************/

int docRtfPopAttribute(		RtfWriter *	rw )
    {
    PushedAttribute *	pa;

    pa= rw->rwcPushedAttribute;
    if  ( ! pa )
	{ XDEB(pa); return -1;	}

    rw->rwTextAttribute= pa->paTextAttribute;
    rw->rwTextCharset= pa->paTextCharset;
    rw->rwcPushedAttribute= pa->paNext;

    free( pa );

    return 0;
    }

int docRtfPushAttribute(		RtfWriter *	rw )
    {
    PushedAttribute *		pa;

    pa= (PushedAttribute *)malloc( sizeof( PushedAttribute ) );
    if  ( ! pa )
	{ XDEB(pa); return- 1;	}
    pa->paNext= rw->rwcPushedAttribute;
    rw->rwcPushedAttribute= pa;
    pa->paTextAttribute= rw->rwTextAttribute;
    pa->paTextCharset= rw->rwTextCharset;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Reset the text attribute.						*/
/*									*/
/************************************************************************/

void docRtfWriteSwitchToPlain(		RtfWriter *	rw )
    {
    /*  1  */
    docPlainTextAttribute( &(rw->rwTextAttribute), rw->rwDocument );
    rw->rwTextCharset= FONTcharsetANSI;

    /*  2  */
    docRtfWriteTag( rw, RTFtag_plain );
    }

/************************************************************************/
/*									*/
/*  Switch text attributes.						*/
/*									*/
/************************************************************************/

void docRtfWriteSwitchTextAttributes(	RtfWriter *		rw,
					int			textAttrNr,
					const char *		first )
    {
    const BufferDocument *	bd= rw->rwDocument;
    TextAttribute		ta;

    int				symbol= ' ';

    if  ( first )
	{
	unsigned short	uni;
	int		step= uniGetUtf8( &uni, first );

	if  ( step > 1 )
	    { symbol= uni;	}
	}

    docGetTextAttributeByNumber( &ta, bd, textAttrNr );

    if  ( ta.taFontNumber >= 0 )
	{
	const DocumentProperties *	dp= &(bd->bdProperties);
	const DocumentFontList *	dfl= dp->dpFontList;

	PropertyMask			taSetMask;
	PropertyMask			doneMask;

	utilPropMaskClear( &taSetMask );
	utilPropMaskFill( &taSetMask, TAprop_COUNT );

	utilPropMaskClear( &doneMask );
	utilUpdateTextAttribute( &doneMask, &(rw->rwTextAttribute),
							    &taSetMask, &ta );
	if  ( PROPmaskISSET( &doneMask, TApropSHADING ) )
	    {
	    if  ( ! docShadingNumberIsShading( bd, ta.taShadingNumber ) )
		{
		docRtfWriteTag( rw, RTFtag_plain );
		utilInitTextAttribute( &(rw->rwTextAttribute) );
		utilPropMaskClear( &doneMask );
		utilUpdateTextAttribute( &doneMask, &(rw->rwTextAttribute),
							    &taSetMask, &ta );
		}
	    }

	if  ( ! ( rw->rwSaveFlags & RTFflagUNENCODED ) )
	    {
	    const DocumentFont *		df;

	    df= docFontListGetFontByNumber( dfl, ta.taFontNumber );
	    if  ( df )
		{
		int			fileFontNumber;
		int			charset= FONTcharsetDEFAULT;
		const char *		encodingName= (const char *)0;

		fileFontNumber= docRtfWriteGetCharset( rw,
						&charset, df, symbol );
		if  ( fileFontNumber < 0 )
		    { SLXDEB(df->dfName,fileFontNumber,symbol);	}
		else{ ta.taFontNumber= fileFontNumber;		}

		if  ( rw->rwTextCharset != charset )
		    {
		    rw->rwTextCharset= charset;
		    PROPmaskADD( &doneMask, TApropFONT_NUMBER );
		    }

		encodingName= utilGetEncodingName( df->dfName, charset );
		textConverterSetNativeEncodingName(
				    rw->rwTextTextConverter, encodingName );
		}
	    }

	if  ( ! utilPropMaskIsEmpty( &doneMask ) )
	    { docRtfSaveTextAttributeImpl( rw, &doneMask, &ta );	}
	}

    return;
    }

