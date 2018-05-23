/************************************************************************/
/*									*/
/*  Exchange text properties with an RTF file.				*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<stdio.h>
#   include	<ctype.h>

#   include	<appDebugon.h>

#   include	"docRtfReaderImpl.h"
#   include	"docRtfFlags.h"

/************************************************************************/
/*									*/
/*  Handle a text property when reading RTF.				*/
/*									*/
/************************************************************************/

static int docRtfRememberTextPropertyImpl(	TextAttribute *		ta,
						const RtfControlWord *	rcw,
						int			arg,
						RtfReader *		rrc )
    {
    switch( rcw->rcwID )
	{
	case TAprop_NONE:
	    docPlainTextAttribute( ta, rrc->rrDocument );
	    return 0;

	case TApropFONTSIZE:
	    if  ( arg < 1 || arg > 2000 )
		{ LSLDEB(rrc->rrCurrentLine,rcw->rcwWord,arg); arg= 20; }
	    break;

	case TApropBORDER:
	    arg= docRtfReadGetBorderNumber( rrc );
	    if  ( arg < 0 )
		{ LDEB(arg); return -1;	}
	    break;

	default:
	    break;
	}

    if  ( utilSetTextProperty( ta, rcw->rcwID, arg ) < 0 )
	{ SLDEB(rcw->rcwWord,arg); return -1;	}

    return 0;
    }

int docRtfRememberTextShadingProperty(	const RtfControlWord *	rcw,
					int			arg,
					RtfReader *	rrc )
    {
    RtfReadingState *	rrs= rrc->rrcState;

    if  ( docSetShadingProperty( &(rrs->rrsTextShading),
						    rcw->rcwID, arg ) < 0 )
	{ SLDEB(rcw->rcwWord,arg); return -1;	}

    rrs->rrsTextShadingChanged= 1;

    return 0;
    }

int docRtfRememberTextProperty(		const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rrc )
    {
    RtfReadingState *	rrs= rrc->rrcState;

    if  ( rcw->rcwID == TApropTEXT_STYLE )
	{
	const	DocumentStyleSheet *	dss= &(rrc->rrDocument->bdStyleSheet);

	rrc->rrcStyle.dsLevel= DOClevSPAN;

	if  ( arg >= 0 && arg < dss->dssStyleCount )
	    {
	    const DocumentStyle *	ds= dss->dssStyles[arg];

	    if  ( ds && ds->dsLevel == DOClevSPAN )
		{
		PropertyMask	doneMask;

		if  ( rrs->rrsTextShadingChanged )
		    { docRtfRefreshTextShading( rrc, rrs );	}

		utilUpdateTextAttribute( &doneMask, &(rrs->rrsTextAttribute),
				&(ds->dsTextMask), &(ds->dsTextAttribute) );
		}
	    }
	}

    /* Also used to start a font in the font list		*/
    if  ( rcw->rcwID == TApropFONT_NUMBER )
	{
	docRtfRestartFont( rrc );

	rrc->rrcCurrentEncodedFont.ecFileFontNumber= arg;

	if  ( ! ( rrc->rrReadFlags & RTFflagUNENCODED ) )
	    {
	    if  ( ! docRtfReadMapFont( rrc,
			    &(rrc->rrcCurrentEncodedFont.ecBufFontNumber),
			    &(rrc->rrcCurrentEncodedFont.ecCharset),
			    rrc->rrcCurrentEncodedFont.ecFileFontNumber ) )
		{
		rrc->rrcCurrentFont.dfDocFontNumber=
				    rrc->rrcCurrentEncodedFont.ecBufFontNumber;
		rrs->rrsTextCharset= rrc->rrcCurrentEncodedFont.ecCharset;

		arg= rrc->rrcCurrentEncodedFont.ecBufFontNumber;
		}
	    }
	else{
	    rrc->rrcCurrentEncodedFont.ecBufFontNumber=
				rrc->rrcCurrentEncodedFont.ecFileFontNumber;
	    }
	}

    if  ( rcw->rcwID == TAprop_NONE )
	{
	docInitItemShading( &(rrs->rrsTextShading) );
	rrs->rrsTextShadingChanged= 0;
	}

    if  ( docRtfRememberTextPropertyImpl( &(rrs->rrsTextAttribute),
							    rcw, arg, rrc ) )
	{ SDEB(rcw->rcwWord); return -1;	}

    if  ( rcw->rcwID != TAprop_NONE && rcw->rcwID < TAprop_COUNT )
	{ PROPmaskADD( &(rrc->rrcStyle.dsTextMask), rcw->rcwID ); }

    return 0;
    }

int docRtfRememberPntextProperty(	const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rrc )
    {
    /* Also used to start a font in the font list		*/
    if  ( rcw->rcwID == TApropFONT_NUMBER )
	{
	int		bufFontNum;
	int		charset;

	if  ( ! docRtfReadMapFont( rrc, &bufFontNum, &charset, arg ) )
	    { arg= bufFontNum;	}
	}

    if  ( rcw->rcwID == TAprop_NONE )
	{
	docInitItemShading( &(rrc->rrcParagraphNumberTextShading) );
	/*rrc->rrcParagraphNumberTextShadingChanged= 0;*/
	}

    if  ( docRtfRememberTextPropertyImpl(
		&(rrc->rrcParagraphNumber.pnTextAttribute), rcw, arg, rrc ) )
	{ SDEB(rcw->rcwWord); return -1;	}

    return 0;
    }

void docRtfResetTextAttribute(	RtfReadingState *	rrs,
				BufferDocument *	bd )
    {
    docPlainTextAttribute( &(rrs->rrsTextAttribute), bd );
    docInitItemShading( &(rrs->rrsTextShading) );
    rrs->rrsTextShadingChanged= 0;

    return;
    }

/************************************************************************/
/*									*/
/*  Translate the separate text shading properties that we have		*/
/*  collected to a shading number.					*/
/*									*/
/************************************************************************/

void docRtfRefreshTextShading(	RtfReader *		rrc,
				RtfReadingState *	rrs )
    {
    rrs->rrsTextAttribute.taShadingNumber= docItemShadingNumber(
				    rrc->rrDocument, &(rrs->rrsTextShading) );
    rrs->rrsTextShadingChanged= 0;

    return;
    }

