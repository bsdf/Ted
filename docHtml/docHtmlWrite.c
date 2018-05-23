/************************************************************************/
/*									*/
/*  Save a BufferDocument into an HTML file.				*/
/*  Depending on the parameters, this is either an HTML file with	*/
/*  a directory for the images, or a MHTML (rfc2112,rfc2557) aggregate.	*/
/*  RFC 2557 was never validated.					*/
/*									*/
/************************************************************************/

#   include	"docHtmlConfig.h"

#   include	<string.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	<utilWindowsLanguageCode.h>
#   include	<sioGeneral.h>

#   include	<docBuf.h>
#   include	<psShading.h>
#   include	"docHtmlWriteImpl.h"
#   include	<docHyperlinkField.h>
#   include	<docBookmarkField.h>
#   include	<docTreeNode.h>
#   include	<docNotes.h>
#   include	<docDocumentNote.h>

#   include	<appDebugon.h>

void docInitHtmlWritingContext(	HtmlWritingContext *	hwc )
    {
    xmlInitXmlWriter( &(hwc->hwcXmlWriter) );

    hwc->hwcLayoutContext= (const LayoutContext *)0;
    docLayoutInitBlockFrame( &(hwc->hwcBlockFrame) );
    docInitParagraphFrame( &(hwc->hwcParagraphFrame) );

    hwc->hwcOpenImageStream= (HtmlOpenImageStream)0;
    hwc->hwcGetImageSrc= (HtmlGetImageSrc)0;
    hwc->hwcGetCssName= (HtmlGetCssName)0;
    hwc->hwcPrivate= (void *)0;
    hwc->hwcDocument= (BufferDocument *)0;

    utilInitIndexMapping( &(hwc->hwcDeferredNotes) );

    hwc->hwcInlineCss= 0;
    hwc->hwcInlineNotes= 0;

    hwc->hwcInHyperlink= 0;
    hwc->hwcInBookmark= 0;
    hwc->hwcInPageref= 0;
    hwc->hwcBytesInLink= 0;

    docPlainTextAttribute( &(hwc->hwcDefaultAttribute), hwc->hwcDocument );
    hwc->hwcDefaultAttribute.taFontSizeHalfPoints= 24;
    hwc->hwcDefaultAttribute.taFontNumber= -1;

    docInitParagraphProperties( &(hwc->hwcParagraphProperties) );

    hwc->hwcImageCount= 0;
    hwc->hwcNoteRefCount= 0;
    hwc->hwcNoteDefCount= 0;

    hwc->hwcTableNesting= 0;

    hwc->hwcCurrentAttributeNumber= -1;

    hwc->hwcSupportsBullets= 0;
    hwc->hwcEmitBackground= 0;
    hwc->hwcEmitBackground= 0;
    return;
    }

void docCleanHtmlWritingContext(	HtmlWritingContext *	hwc )
    {
    utilCleanIndexMapping( &(hwc->hwcDeferredNotes) );

    docCleanParagraphProperties( &(hwc->hwcParagraphProperties) );

    docLayoutCleanBlockFrame( &(hwc->hwcBlockFrame) );
    /*docCleanParagraphFrame( &(hwc->hwcParagraphFrame) );*/

    return;
    }

/************************************************************************/
/*									*/
/*  Save a tag with an argument.					*/
/*									*/
/************************************************************************/

void docHtmlPutString(		const char *		s,
				HtmlWritingContext *	hwc )
    { xmlPutString( s, &(hwc->hwcXmlWriter) );	}

void docHtmlNewLine(	HtmlWritingContext *	hwc )
    {
    xmlNewLine(  &(hwc->hwcXmlWriter) );
    }

void docHtmlWriteStringAttribute(	HtmlWritingContext *	hwc,
					const char *		name,
					const char *		value )
    {
    xmlWriteStringAttribute( &(hwc->hwcXmlWriter), name, value );
    }

void docHtmlWriteIntAttribute(		HtmlWritingContext *	hwc,
					const char *		name,
					int			value )
    {
    xmlWriteIntAttribute( &(hwc->hwcXmlWriter), name, value );
    }

void docHtmlEscapeCharacters(	HtmlWritingContext *	hwc,
				const char *		ss,
				int			len )
    {
    xmlEscapeCharacters( &(hwc->hwcXmlWriter), ss, len );
    }

static void docHtmlEscapeString(	const char *		s,
					HtmlWritingContext *	hwc )
    {
    xmlEscapeCharacters( &(hwc->hwcXmlWriter), s, strlen( s ) );

    return;
    }

/************************************************************************/
/*									*/
/*  Change attributes.							*/
/*									*/
/************************************************************************/

void docHtmlChangeAttributes(	HtmlWritingContext *		hwc,
				int				taNr )
    {
    if  ( taNr == hwc->hwcCurrentAttributeNumber )
	{ return;	}

    if  ( hwc->hwcCurrentAttributeNumber >= 0 )
	{
	docHtmlPutString( "</span>", hwc );
	}

    if  ( taNr >= 0 )
	{
	char	scratch[20];

	sprintf( scratch, "t%d", taNr );

	docHtmlPutString( "<span", hwc );
	docHtmlWriteStringAttribute( hwc, "class", scratch );
	docHtmlPutString( ">", hwc );
	}

    hwc->hwcCurrentAttributeNumber= taNr;
    }

int docHtmlStartAnchor(	HtmlWritingContext *		hwc,
			int				isNote,
			const MemoryBuffer *		fileName,
			const MemoryBuffer *		markName,
			const MemoryBuffer *		refName,
			const char *			title,
			int				titleSize )
    {
    XmlWriter *			xw= &(hwc->hwcXmlWriter);
    SimpleOutputStream *	sos= xw->xwSos;

    int				afterSpace;
    int				needed= 0;

    if  ( fileName && utilMemoryBufferIsEmpty( fileName ) )
	{ fileName= (const MemoryBuffer *)0;	}
    if  ( markName && utilMemoryBufferIsEmpty( markName ) )
	{ markName= (const MemoryBuffer *)0;	}
    if  ( refName && utilMemoryBufferIsEmpty( refName ) )
	{ refName= (const MemoryBuffer *)0;	}

    docHtmlPutString( "<a", hwc );

    if  ( isNote )
	{
	docHtmlWriteStringAttribute( hwc, "style", "text-decoration:none" );
	}
    afterSpace= 0;

    if  ( fileName || markName )
	{
	needed += 1+ 6;
	if  ( fileName )
	    { needed += fileName->mbSize;	}
	if  ( markName )
	    { needed += 1+ markName->mbSize;	}
	needed += 1;
	}

    if  ( refName )
	{ needed += 1+ 6+ refName->mbSize+ 1;	}

    if  ( xw->xwColumn > 5		&&
	  xw->xwColumn+ needed > 76	)
	{ docHtmlNewLine( hwc ); afterSpace= 1;		}

    if  ( fileName || markName )
	{
	if  ( ! afterSpace )
	    { docHtmlPutString( " ", hwc ); }

	docHtmlPutString( "href=\"", hwc );

	if  ( fileName )
	    { xmlEscapeBuffer( xw, fileName );	}

	if  ( markName )
	    {
	    (void)sioOutPutByte( '#', sos );
	    xmlEscapeBuffer( xw, markName );
	    }

	docHtmlPutString( "\"", hwc );
	afterSpace= 0;
	}

    if  ( refName )
	{
	if  ( ! afterSpace )
	    { docHtmlPutString( " ", hwc ); }

	docHtmlPutString( "id=\"", hwc );
	xmlEscapeBuffer( xw, refName );

	docHtmlPutString( "\"", hwc );
	afterSpace= 0;
	}

    if  ( titleSize > 0 )
	{
	if  ( ! afterSpace )
	    { docHtmlPutString( " ", hwc ); }

	docHtmlPutString( "title=\"", hwc );
	xmlEscapeCharacters( xw, title, titleSize );

	docHtmlPutString( "\"", hwc );
	afterSpace= 0;
	}

    docHtmlPutString( ">", hwc );

    return 0;
    }

int docHtmlStartField(	const DocumentField *		df,
			HtmlWritingContext *		hwc,
			const BufferItem *		bi,
			int				attNr )
    {
    int			rval= 0;

    const MemoryBuffer *	fileName= (const MemoryBuffer *)0;
    const MemoryBuffer *	markName= (const MemoryBuffer *)0;

    const char *	refName= (const char *)0;
    int			refSize= 0;
    const char *	title= (const char *)0;
    int			titleSize= 0;

    HyperlinkField		hf;
    const MemoryBuffer *	mbBookmark= (const MemoryBuffer *)0;

    docInitHyperlinkField( &hf );

    switch( df->dfKind )
	{
	case DOCfkCHFTN:
	    hwc->hwcInHyperlink++;
	    if  ( ! hwc->hwcInBookmark && hwc->hwcInHyperlink == 1 )
		{ docHtmlStartNote( df, hwc, bi, attNr );	}
	    break;

	case DOCfkHYPERLINK:
	    hwc->hwcInHyperlink++;
	    if  ( ! hwc->hwcInBookmark && hwc->hwcInHyperlink == 1 )
		{
		if  ( ! docGetHyperlinkField( &hf, df ) )
		    {
		    const int	isNote= 0;

		    docHtmlChangeAttributes( hwc, -1 );

		    docHtmlStartAnchor( hwc, isNote,
					    &(hf.hfFile), &(hf.hfBookmark),
					    (const MemoryBuffer *)0,
					    title, titleSize );

		    docHtmlChangeAttributes( hwc, attNr );

		    hwc->hwcBytesInLink= 0;
		    }
		}
	    break;

	case DOCfkBOOKMARK:
	    hwc->hwcInBookmark++;
	    if  ( ! docFieldGetBookmark( &mbBookmark, df ) )
		{
		if  ( df->dfHeadPosition.epParaNr ==
					df->dfTailPosition.epParaNr	&&
		      ! hwc->hwcInHyperlink				&&
		      hwc->hwcInBookmark == 1				)
		    {
		    const int	isNote= 0;

		    docHtmlChangeAttributes( hwc, -1 );

		    docHtmlStartAnchor( hwc, isNote,
					    fileName, markName,
					    mbBookmark,
					    title, titleSize );

		    docHtmlChangeAttributes( hwc, attNr );
		    }
		else{
		    docHtmlPutString( "<span id=\"", hwc );
		    docHtmlEscapeCharacters( hwc, refName, refSize );
		    docHtmlPutString( "\"/>", hwc );
		    }
		}
	    break;

	case DOCfkPAGEREF:
	    hwc->hwcInPageref++;
	    break;

	default:
	    break;
	}

    docCleanHyperlinkField( &hf );

    return rval;
    }

int docHtmlFinishField(	const DocumentField *		df,
			HtmlWritingContext *		hwc )
    {
    switch( df->dfKind )
	{
	case DOCfkCHFTN:
	case DOCfkHYPERLINK:
	    if  ( ! hwc->hwcInBookmark && hwc->hwcInHyperlink == 1 )
		{
		docHtmlChangeAttributes( hwc, -1 );

		docHtmlPutString( "</a>", hwc );
		}
	    hwc->hwcInHyperlink--;
	    if  ( hwc->hwcInHyperlink < 0 )
		{ hwc->hwcInHyperlink= 0;	}
	    break;

	case DOCfkBOOKMARK:
	    if  ( df->dfHeadPosition.epParaNr ==
					df->dfTailPosition.epParaNr	&&
		  ! hwc->hwcInHyperlink					&&
		  hwc->hwcInBookmark == 1				)
		{
		docHtmlChangeAttributes( hwc, -1 );
		docHtmlPutString( "</a>", hwc );
		}

	    hwc->hwcInBookmark--;
	    if  ( hwc->hwcInBookmark < 0 )
		{ hwc->hwcInBookmark= 0;	}
	    break;

	case DOCfkPAGEREF:
	    hwc->hwcInPageref--;
	    break;

	default:
	    break;
	}
    
    return 0;
    }

void docHtmlEmitBackgroundProperty(
				const ItemShading *		is,
				HtmlWritingContext *		hwc )
    {
    const BufferDocument *	bd= hwc->hwcDocument;

    int				isFilled= 0;
    RGB8Color			rgb8;

    char			scratch[39];

    if  ( is->isPattern == DOCspSOLID )
	{
	if  ( docGetSolidRgbShadeOfItem( &isFilled, &rgb8, bd, is ) )
	    { LDEB(1);	}

	if  ( isFilled )
	    {
	    sprintf( scratch, "\"#%02x%02x%02x\"",
			    rgb8.rgb8Red, rgb8.rgb8Green, rgb8.rgb8Blue );

	    docHtmlWriteStringAttribute( hwc, "BGCOLOR", scratch );
	    }
	}

    return;
    }

/************************************************************************/

static void docHtmlSaveMetaElement(	HtmlWritingContext *	hwc,
					const char *		name,
					const MemoryBuffer *	content )
    {
    if  ( ! utilMemoryBufferIsEmpty( content ) )
	{
	docHtmlPutString( "<meta name=\"", hwc );
	docHtmlEscapeString( name, hwc );
	docHtmlPutString( "\" content=\"", hwc );
	docHtmlEscapeString( utilMemoryBufferGetString( content ), hwc );
	docHtmlPutString( "\"/>", hwc );
	docHtmlNewLine( hwc );
	}

    return;
    }

/************************************************************************/

int docHtmlStartDocument(	HtmlWritingContext *	hwc )
    {
    int				rval= 0;
    const DocumentProperties *	dp= &(hwc->hwcDocument->bdProperties);
    const char *		lang;

    MemoryBuffer		stylesheet;

    utilInitMemoryBuffer( &stylesheet );

    lang= utilLangForLanguageCode( dp->dpDefaultLanguage );

#   if 0
    docHtmlPutString( "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\">\r\n",
									hwc );
#   else
    docHtmlPutString( "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\r\n", hwc );
    docHtmlPutString( "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.1//EN\" ",
									hwc );
    docHtmlPutString( "\"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd\">\r\n",
									hwc );
#   endif

    docHtmlNewLine( hwc );
    docHtmlPutString( "<html", hwc );
    docHtmlWriteStringAttribute( hwc,
				"xmlns", "http://www.w3.org/1999/xhtml" );
    if  ( lang )
	{ docHtmlWriteStringAttribute( hwc, "xml:lang", lang );	}
    docHtmlPutString( ">", hwc );
    docHtmlNewLine( hwc );

    docHtmlPutString( "<head>", hwc );
    docHtmlNewLine( hwc );
    docHtmlPutString( "<meta ", hwc );
    docHtmlWriteStringAttribute( hwc, "http-equiv", "Content-Type" );
    docHtmlWriteStringAttribute( hwc, "content", "text/html; charset=UTF-8" );
    docHtmlPutString( "/>", hwc );
    docHtmlNewLine( hwc );

    if  ( ! utilMemoryBufferIsEmpty( &(dp->dpTitle) ) )
	{
	docHtmlPutString( "<title>", hwc );
	docHtmlEscapeString( utilMemoryBufferGetString( &(dp->dpTitle) ), hwc );
	docHtmlPutString( "</title>", hwc );
	docHtmlNewLine( hwc );
	}

    if  ( ! hwc->hwcInlineCss )
	{
	if  ( ! hwc->hwcGetCssName )
	    { XDEB(hwc->hwcInlineCss); rval= -1; goto ready;	}

	if  ( (*hwc->hwcGetCssName)( &stylesheet, hwc ) < 0 )
	    { LDEB(1); return -1;	}

	docHtmlPutString( "<link ", hwc );
	docHtmlWriteStringAttribute( hwc, "rel", "stylesheet" );
	docHtmlWriteStringAttribute( hwc, "type", "text/css; charset=UTF-8" );
	docHtmlWriteStringAttribute( hwc, "href",
				    utilMemoryBufferGetString( &stylesheet ) );
	docHtmlPutString( "/>", hwc );
	docHtmlNewLine( hwc );
	}

    docHtmlSaveMetaElement( hwc, "description", &(dp->dpSubject) );
    docHtmlSaveMetaElement( hwc, "keywords", &(dp->dpKeywords) );
    docHtmlSaveMetaElement( hwc, "comment", &(dp->dpDoccomm) );
    docHtmlSaveMetaElement( hwc, "author", &(dp->dpAuthor) );

    if  ( ! utilMemoryBufferIsEmpty( &(dp->dpHlinkbase) ) )
	{
	docHtmlPutString( "<base href=\"", hwc );
	docHtmlEscapeString( utilMemoryBufferGetString( &(dp->dpHlinkbase) ), hwc );
	docHtmlPutString( "\"/>", hwc );
	docHtmlNewLine( hwc );
	}

    if  ( hwc->hwcInlineCss )
	{
	docHtmlPutString( "<style type=\"text/css\">", hwc );
	docHtmlNewLine( hwc );
	docHtmlNewLine( hwc );

	if  ( docHtmlSaveDocumentStyles( hwc, hwc->hwcXmlWriter.xwSos ) )
	    { LDEB(1); rval= -1; goto ready;	}

	docHtmlPutString( "</style>", hwc );
	docHtmlNewLine( hwc );
	}

    docHtmlPutString( "</head>", hwc );
    docHtmlNewLine( hwc );

    docHtmlPutString( "<body", hwc );
    docHtmlWriteStringAttribute( hwc, "class", "ted" );
    /*
    if  ( lang )
	{ docHtmlWriteStringAttribute( hwc, "lang", lang );	}
    */
    docHtmlPutString( ">", hwc );
    docHtmlNewLine( hwc );

  ready:

    utilCleanMemoryBuffer( &stylesheet );

    return rval;
    }

int docHtmlFinishDocument(	HtmlWritingContext *	hwc )
    {
    docHtmlPutString( "</body></html>", hwc );
    docHtmlNewLine( hwc );

    return 0;
    }

/************************************************************************/

int docHtmlSaveNotes(	HtmlWritingContext *	hwc )
    {
    DocumentField *	dfNote;
    DocumentNote *	dn;

    docHtmlPutString( "<hr/>", hwc );

    for ( dfNote= docGetFirstNoteOfDocument( &dn, hwc->hwcDocument, -1 );
	  dfNote;
	  dfNote= docGetNextNoteInDocument( &dn, hwc->hwcDocument, dfNote, -1 ) )
	{
	DocumentTree *	ei;

	ei= &(dn->dnDocumentTree);
	if  ( ! ei->dtRoot )
	    { XDEB(ei->dtRoot); continue;	}

	if  ( docHtmlSaveSelection( hwc, ei, (const DocumentSelection *)0 ) )
	    { XDEB(ei->dtRoot); return -1; }
	}

    if  ( hwc->hwcNoteDefCount != hwc->hwcNoteRefCount )
	{ LLDEB(hwc->hwcNoteDefCount,hwc->hwcNoteRefCount);	}

    return 0;
    }

