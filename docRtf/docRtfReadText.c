/************************************************************************/
/*									*/
/*  Manage the actual string content while reading an RTF document.	*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	<appDebugon.h>

#   include	<uniUtf8.h>

#   include	"docRtfReaderImpl.h"
#   include	<docParaString.h>
#   include	<textConverter.h>
#   include	<textConverterImpl.h>
#   include	<docParaParticules.h>

/************************************************************************/
/*									*/
/*  Handle text..							*/
/*									*/
/*  1)  Ignore it.							*/
/*  2)  Refuse it.							*/
/*  3)  Save it for later use. (Convert it to UTF-8 on the fly).	*/
/*									*/
/************************************************************************/

/*  1  */
int docRtfIgnoreText(	RtfReader *		rr,
			const char *		text,
			int			len )
    { return 0; }

/*  2  */
int docRtfRefuseText(	RtfReader *		rr,
			const char *		text,
			int			len )
    { LDEB(1); return -1; }

/************************************************************************/
/*									*/
/*  Save text: It is not encoded.					*/
/*									*/
/************************************************************************/

int docRtfSaveRawBytes(		RtfReader *		rr,
				const char *		text,
				int			len )
    {
    RtfReadingState *	rrs= rr->rrcState;

    if  ( utilMemoryBufferAppendBytes( &(rrs->rrsSavedText),
					(const unsigned char *)text, len ) )
	{ LDEB(len); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Convert text from some encoding to UTF-8.				*/
/*									*/
/************************************************************************/

static int docRtfSaveBytes(	void *		vmb,
				int		offset,
				const char *	bytes,
				int		count )
    {
    MemoryBuffer *	mb= (MemoryBuffer *)vmb;

    if  ( utilMemoryBufferAppendBytes( mb,
				    (const unsigned char *)bytes, count ) )
	{ LDEB(mb->mbSize); return -1;	}

    return count;
    }

/************************************************************************/
/*									*/
/*  Save text: It is in the document encoding.				*/
/*									*/
/************************************************************************/

int docRtfSaveDocEncodedText(	RtfReader *		rr,
				const char *		text,
				int			len )
    {
    RtfReadingState *	rrs= rr->rrcState;
    int			upto;
    int			consumed= 0;

    upto= textConverterConvertToUtf8( rr->rrRtfTextConverter,
				    (void *)&(rrs->rrsSavedText),
				    &consumed,
				    rrs->rrsSavedText.mbSize, text, len );
    if  ( upto < 0 )
	{ LDEB(upto); return -1;	}

    return 0;
    }

/************************************************************************/

void docRtfReadSetupTextConverters(	RtfReader *	rr )
    {
    textConverterSetNativeEncodingName( rr->rrRtfTextConverter,
						    DOC_RTF_AnsiCharsetName );

    textConverterSetProduce( rr->rrRtfTextConverter, docRtfSaveBytes );

    docParaSetupTextConverter( rr->rrTextTextConverter );
    }

/************************************************************************/
/*									*/
/*  Store the text bytes that we collected from the rtf file in some	*/
/*  location. Use realloc() to rezize the target location and flush	*/
/*  the collected text bytes.						*/
/*									*/
/************************************************************************/

int docRtfStoreSavedText(	char **		pTarget,
				int *		pSize,
				RtfReader *	rr,
				int		removeSemicolon )
    {
    RtfReadingState *	rrs= rr->rrcState;

    char *	fresh;
    int		size;

    if  ( utilMemoryBufferIsEmpty( &(rrs->rrsSavedText) ) )
	{ *pSize= 0; return 0;	}

    size= rrs->rrsSavedText.mbSize;
    fresh= (char *)realloc( *pTarget, size+ 1 );
    if  ( ! fresh )
	{ LXDEB(size,fresh); return -1;	}

    memcpy( fresh, rrs->rrsSavedText.mbBytes, size );
    fresh[size]= '\0';

    if  ( removeSemicolon		&&
	  size > 0			&&
	  fresh[size- 1] == ';'		)
	{ fresh[--size]= '\0';	}

    utilEmptyMemoryBuffer( &(rrs->rrsSavedText) );

    *pTarget= fresh;
    *pSize= size;
    return 0;
    }

int docRtfMemoryBufferSetText(	MemoryBuffer *		mb,
				RtfReader *		rr,
				int			removeSemicolon )
    {
    char *	text= (char *)0;
    int		size;

    if  ( docRtfStoreSavedText( &text, &size, rr, removeSemicolon ) )
	{ LDEB(1); return -1;	}

    if  ( utilMemoryBufferSetBytes( mb, (const unsigned char *)text, size ) )
	{ LDEB(1); return -1;	}

    if  ( text )
	{ free( text );		}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Append saved text to a memory buffer.				*/
/*									*/
/************************************************************************/

int docRtfMemoryBufferAppendText(	MemoryBuffer *		mb,
					RtfReader *		rr )
    {
    const int	removeSemicolon= 0;
    char *	text= (char *)0;
    int		size;

    if  ( docRtfStoreSavedText( &text, &size, rr, removeSemicolon ) )
	{ LDEB(1); return -1;	}

    if  ( utilMemoryBufferAppendBytes( mb, (const unsigned char *)text, size ) )
	{ LDEB(1); return -1;	}

    if  ( text )
	{ free( text );		}

    return 0;
    }

/************************************************************************/

static int docRtfReadAdaptToFontEncoding(
				RtfReader *			rr,
				RtfReadingState *		rrs )
    {
    const char *		encodingName= (const char *)0;

    if  ( rrs->rrsTextCharset >= 0 )
	{
	encodingName= docGetEncodingName( rr->rrDocument,
			    &(rrs->rrsTextAttribute), rrs->rrsTextCharset );
	}

    if  ( ! encodingName )
	{ encodingName= rr->rrRtfTextConverter->tcNativeEncodingName;	}

    textConverterSetNativeEncodingName( rr->rrTextTextConverter, encodingName );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Insert particules from the input in the document.			*/
/*									*/
/************************************************************************/

int docRtfTextParticule(	RtfReader *		rr,
				const char *		text,
				int			len )
    {
    RtfReadingState *		rrs= rr->rrcState;
    BufferDocument *		bd= rr->rrDocument;
    BufferItem *		paraNode;

    if  ( rr->rrcInIgnoredGroup )
	{ return 0;	}

    paraNode= docRtfGetParaNode( rr );
    if  ( ! paraNode )
	{ XDEB(paraNode); return -1; }

    if  ( docParaStrlen( paraNode ) == 0	||
	  rr->rrAfterParaHeadField		)
	{
	if  ( docRtfAdaptToParaProperties( paraNode, bd, rrs,
					    rr->rrParagraphBreakOverride ) )
	    { LDEB(1); return -1;	}
	}

    if  ( docRtfReadAdaptToFontEncoding( rr, rrs ) )
	{ LDEB(1);	}

    if  ( rrs->rrsTextShadingChanged )
	{ docRtfRefreshTextShading( rr, rrs );	}

    if  ( docParaAppendText( bd, paraNode, &(rrs->rrsTextAttribute),
					rr->rrTextTextConverter, text, len ) )
	{ LDEB(1); return -1;	}

    rr->rrcAfterNoteref= 0;
    rr->rrAfterParaHeadField= 0;
    
    return 0;
    }

/************************************************************************/
/*									*/
/*  Handle an explicit unicode.						*/
/*  Special characters.							*/
/*									*/
/************************************************************************/

static int docRtfTextUnicodeValue(	const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr )
    {
    RtfReadingState *	rrs= rr->rrcState;

    char		bytes[7];
    int			count;

    if  ( arg < 0 )
	{ arg += 65536;	}

    /* Dirty HACK: Only use low byte of characters in the unicode private range */
    if  ( arg >= 0xE000 && arg <= 0xF8FF )
	{
	bytes[0]= arg & 0xff;
	bytes[1]= '\0';

	return docRtfSaveDocEncodedText( rr, (char *)bytes, 1 );
	}

    count= uniPutUtf8( bytes, arg );
    if  ( count < 1 )
	{ LDEB(count); return 0;	}

    if  ( rr->rrcAddParticule == docRtfSaveRawBytes )
	{ XXDEB(rr->rrcAddParticule,docRtfSaveRawBytes); return 0; }

    if  ( rr->rrcAddParticule == docRtfTextParticule )
	{
	int			stroffShift= 0;
	int			stroff;
	BufferItem *		paraNode= docRtfGetParaNode( rr );
	BufferDocument *	bd= rr->rrDocument;
	int			textAttributeNumber;

	if  ( ! paraNode )
	    { SXDEB(rcw->rcwWord,paraNode); return -1; }

	if  ( rrs->rrsTextShadingChanged )
	    { docRtfRefreshTextShading( rr, rrs );	}

	textAttributeNumber= docTextAttributeNumber( bd,
						&(rrs->rrsTextAttribute) );
	if  ( textAttributeNumber < 0 )
	    { LDEB(textAttributeNumber); return -1;	}

	stroff= docParaStrlen( paraNode );

	if  ( docParaStringReplace( &stroffShift, paraNode, stroff, stroff,
						    (char *)bytes, count ) )
	    { LDEB(docParaStrlen(paraNode)); return -1;	}

	if  ( docParaDivideAppendedText( paraNode, textAttributeNumber,
						    stroff, stroff+ count ) )
	    { LLDEB(count,paraNode->biParaParticuleCount); return -1; }
	}
    else{
	if  ( utilMemoryBufferAppendBytes( &(rrs->rrsSavedText),
					    (unsigned char *)bytes, count ) )
	    { LDEB(count); return -1;	}
	}

    return 0;
    }

int docRtfTextUnicode(		const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr )
    {
    RtfReadingState *	rrs= rr->rrcState;

    if  ( docRtfTextUnicodeValue( rcw, arg, rr ) )
	{ SXDEB(rcw->rcwWord,arg); return -1;	}

    rrs->rrsUnicodeBytesToSkip= rrs->rrsBytesPerUnicode;
    return 0;
    }

int docRtfTextSpecialChar(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr )
    {
    /* docRtfTextParticule() adjusts level */

    if  ( docRtfTextUnicodeValue( rcw, rcw->rcwID, rr ) )
	{ SXDEB(rcw->rcwWord,arg); return -1;	}

    return 0;
    }

int docRtfTextSpecialParticule(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr )
    {
    RtfReadingState *	rrs= rr->rrcState;
    BufferItem *	paraNode;

    if  ( rr->rrcInIgnoredGroup > 0 )
	{ return 0;	}

    if  ( rrs->rrsTextShadingChanged )
	{ docRtfRefreshTextShading( rr, rrs );	}

    paraNode= docRtfGetParaNode( rr );
    if  ( ! paraNode )
	{ SXDEB(rcw->rcwWord,paraNode); return -1; }

    switch( rcw->rcwID )
	{
	case DOCkindTAB:
	case DOCkindLINEBREAK:
	case DOCkindCHFTNSEP:
	case DOCkindCHFTNSEPC:
	case DOCkindOPT_HYPH:
	case DOCkindLTR_MARK:
	case DOCkindRTL_MARK:
	    if  ( docSaveSpecialParticule( rr->rrDocument, paraNode,
				    &(rrs->rrsTextAttribute), rcw->rcwID ) )
		{ LDEB(1); return -1;	}

	    rr->rrcAfterNoteref= 0;
	    rr->rrAfterParaHeadField= 0;

	    break;

	case DOCkindPAGEBREAK:
	case DOCkindCOLUMNBREAK:
	    {
	    int				done= 0;

	    if  ( rr->rrParagraphBreakOverride == -1		&&
		  ( docParaStrlen(paraNode) == 0	||
		    rr->rrAfterParaHeadField		)	)
		{
		if  ( rcw->rcwID == DOCkindPAGEBREAK )
		    { rr->rrParagraphBreakOverride= DOCibkPAGE; done= 1; }

		if  ( rcw->rcwID == DOCkindCOLUMNBREAK )
		    { rr->rrParagraphBreakOverride= DOCibkCOL; done= 1; }
		}

	    if  ( ! done						&&
		  docSaveSpecialParticule( rr->rrDocument, paraNode,
				&(rrs->rrsTextAttribute), rcw->rcwID ) )
		{ LDEB(1); return -1;	}

	    rr->rrcAfterNoteref= 0;
	    rr->rrAfterParaHeadField= 0;

	    break;
	    }

	default:
	    SLDEB(rcw->rcwWord,rcw->rcwID);
	    break;
	}

    return 0;
    }

int docRtfTextBidiMark(	const RtfControlWord *	rcw,
			int			arg,
			RtfReader *		rr )
    {
/*SDEB(rcw->rcwWord);*/
    return 0;
    }
