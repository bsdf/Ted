/************************************************************************/
/*									*/
/*  Save a BufferDocument into an EPUB file.				*/
/*									*/
/*  Documents can be validated against:					*/
/*  -  http://threepress.org/document/epub-validate.			*/
/*									*/
/************************************************************************/

#   include	"docHtmlConfig.h"

#   include	<string.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	<sioGeneral.h>
#   include	<sioZip.h>
#   include	<utilWindowsLanguageCode.h>
#   include	<utilMemoryBufferPrintf.h>

#   include	<docBuf.h>
#   include	<docCalculateToc.h>
#   include	"docHtmlWriteImpl.h"
#   include	<docEvalField.h>
#   include	<docTreeType.h>

#   include	<appDebugon.h>

/************************************************************************/

typedef struct EpubWriter
    {
    CalculateToc	ewCalculateToc;
    ZipOutput		ewZipOutput;
    } EpubWriter;

static int docEpubGetImageSrcX(		MemoryBuffer *		target,
					int			relative,
					int			n,
					const InsertedObject *	io,
					const char *		ext );

static SimpleOutputStream * docEpubOpenImageStream(
					HtmlWritingContext *	hwc,
					int			n,
					const InsertedObject *	io,
					const char *		mimeType,
					const char *		ext );

static int docEpubGetImageSrc(		MemoryBuffer *		target,
					HtmlWritingContext *	hwc,
					int			n,
					const InsertedObject *	io,
					const char *		ext );

static int docEpubGetCssName(		MemoryBuffer *		target,
					HtmlWritingContext *	hwc );

static void docInitEpubWriter(	EpubWriter *	ew )
    {
    docInitCalculateToc( &(ew->ewCalculateToc) );
    sioZipInitOutput( &(ew->ewZipOutput) );
    }

static void docCleanEpubWriter(	EpubWriter *	ew )
    {
    sioZipCleanOutput( &(ew->ewZipOutput) );
    docCleanCalculateToc( &(ew->ewCalculateToc) );
    }

/************************************************************************/

# define DIR_CONTENT	"document"
# define DIR_MEDIA	"media"
# define NAME_CSS	"document.css"
# define NAME_DOC	"document.html"

static const char	DocEpubIdNcx[]= "ncx";
static const char	DocEpubIdCss[]= "css";
static const char	DocEpubIdDoc[]= "doc";

static const char	DocEpubNameMimetype[]= "mimetype";
static const char	DocEpubNameContainer[]= "META-INF/container.xml";
static const char	DocEpubNameOpf[]= "content.opf";
static const char	DocEpubNameNcx[]= "toc.ncx";
static const char	DocEpubNameContent[]= DIR_CONTENT;
static const char	DocEpubNameDocAbs[]= DIR_CONTENT "/" NAME_DOC;
static const char	DocEpubNameDocRel[]= NAME_DOC;
static const char	DocEpubNameMediaAbs[]= DIR_CONTENT "/" DIR_MEDIA;
static const char	DocEpubNameMediaRel[]= DIR_MEDIA;

static const char	DocEpubNameCssAbs[]= DIR_CONTENT "/" 
					     DIR_MEDIA "/" NAME_CSS;
static const char	DocEpubNameCssRel[]= DIR_MEDIA "/" NAME_CSS;

static const char	DocEpubMediaNcx[]= "application/x-dtbncx+xml";
static const char	DocEpubMediaCss[]= "text/css";
static const char	DocEpubMediaDoc[]= "application/xhtml+xml";
static const char	DocEpubMediaEpub[]= "application/epub+zip";

static const char	DocEpubNameContentLen= sizeof(DocEpubNameContent)- 1;
static const int	DocEpubNameMediaLenAbs= sizeof(DocEpubNameMediaAbs)- 1;
static const int	DocEpubNameMediaLenRel= sizeof(DocEpubNameMediaRel) -1;
static const int	DocEpubNameCssLenAbs= sizeof(DocEpubNameCssAbs) -1;
static const int	DocEpubNameCssLenRel= sizeof(DocEpubNameCssRel) -1;

/************************************************************************/

static const char	DocEpubXmlDeclaration[]=
			    "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";

/************************************************************************/

static const char	DocEpubIdentifierName[]= "dcidentifier";

/************************************************************************/

static const char *	DocEpubOpfMetaHead[]=
{
  "  <metadata xmlns:dc=\"http://purl.org/dc/elements/1.1/\"\n",
  "    xmlns:dcterms=\"http://purl.org/dc/terms/\"\n",
  "    xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\n",
  "    xmlns:opf=\"http://www.idpf.org/2007/opf\">\n",
};

static const char	DocEpubOpfMetaTail[]= "  </metadata>\n";

/************************************************************************/
/*									*/
/*  Insert a boilerplate container.xml.					*/
/*									*/
/************************************************************************/

static const char *	DocEpubContainerXml[]=
{
  DocEpubXmlDeclaration,
  "<container version=\"1.0\" xmlns=\"urn:oasis:names:tc:opendocument:xmlns:container\">\n",
  "  <rootfiles>\n",
  "    <rootfile full-path=\"", DocEpubNameOpf, "\" media-type=\"application/oebps-package+xml\"/>\n",
  "  </rootfiles>\n",
  "</container>\n",
};

static int docEpubEmitContainerXml(		ZipOutput *	zo )
    {
    int				rval= 0;

    const char * const		nameMember= DocEpubNameContainer;
    SimpleOutputStream *	sosMember= (SimpleOutputStream *)0;
    const int			compressed= 1;

    int				line;

    sosMember= sioOutZipOpen( zo, nameMember, compressed );
    if  ( ! sosMember )
	{ SXDEB(nameMember,sosMember); rval= -1; goto ready;	}

    for ( line= 0; line < sizeof(DocEpubContainerXml)/sizeof(char *); line++ )
	{ sioOutPutString( DocEpubContainerXml[line], sosMember ); }

  ready:

    if  ( sosMember && sioOutClose( sosMember ) )
	{ SDEB(nameMember); rval= -1;	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Emit a meta information element for the opf package file.		*/
/*									*/
/************************************************************************/

static void docEpubEmitOpfMetaElement(	XmlWriter *		xw,
					const char *		id,
					const char *		name,
					const MemoryBuffer *	value )
    {
    sioOutPutString( "    <", xw->xwSos );
    sioOutPutString( name, xw->xwSos );
    if  ( id )
	{ xmlWriteStringAttribute( xw, "id", id );	}
    sioOutPutString( ">", xw->xwSos );

    xmlEscapeCharacters( xw, (const char *)value->mbBytes, value->mbSize );

    sioOutPutString( "</", xw->xwSos );
    sioOutPutString( name, xw->xwSos );
    sioOutPutString( ">", xw->xwSos );
    xmlNewLine( xw );
    }

static void docEpubEmitOpfMetaElementIfSet(
					XmlWriter *		xw,
					const char *		id,
					const char *		name,
					const MemoryBuffer *	value )
    {
    if  ( ! utilMemoryBufferIsEmpty( value ) )
	{ docEpubEmitOpfMetaElement( xw, id, name, value );	}

    return;
    }

/************************************************************************/
/*									*/
/*  Emit the matadata in an OPF package file. I.E:			*/
/*									*/
/*  Translate document information to Dublin Core Metadata.		*/
/*  See http://dublincore.org/documents/usageguide/elements.shtml	*/
/*									*/
/*  Dublin Core		RTF						*/
/*  ------ ----		---						*/
/*  Title		title						*/
/*  Creator		author						*/
/*  Subject		subject+ keywords				*/
/*  Description		doccom						*/
/*  Publisher		company (Do not repeat the autor value)		*/
/*  Contributor		-- (Do not repeat the autor value)		*/
/*  Date		revtim or creatim				*/
/*  Type		-- Not available				*/
/*  Format		-- Irrelevant					*/
/*  Identifier		-- Must be made up.				*/
/*  Source		-- Not available				*/
/*  Language		deflang						*/
/*  Relation		-- Not available				*/
/*  Coverage		-- Not available				*/
/*  Rights		-- Not available				*/
/*									*/
/************************************************************************/

static int docEpubEmitMetaData(		XmlWriter *		xw,
					const MemoryBuffer *	title,
					const MemoryBuffer *	identifier,
					const BufferDocument *	bd )
    {
    int				rval= 0;
    const DocumentProperties *	dp= &(bd->bdProperties);
    const char *		lang= "en_US";

    MemoryBuffer		mbLang;

    int				l;

    utilInitMemoryBuffer( &mbLang );
    if  ( utilMemoryBufferSetString( &mbLang, lang ) )
	{ LDEB(1); rval= -1; goto ready;	}

    for ( l= 0; l < sizeof(DocEpubOpfMetaHead)/sizeof(char *); l++ )
	{ sioOutPutString( DocEpubOpfMetaHead[l], xw->xwSos ); }

    docEpubEmitOpfMetaElement( xw, (char *)0, "dc:title", title );

    lang= utilLangForLanguageCode( dp->dpDefaultLanguage );
    if  ( lang )
	{
	if  ( utilMemoryBufferSetString( &mbLang, lang ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}

    docEpubEmitOpfMetaElement( xw, (char *)0, "dc:language", &mbLang );
    docEpubEmitOpfMetaElement( xw, DocEpubIdentifierName,
					"dc:identifier", identifier );

    docEpubEmitOpfMetaElementIfSet( xw, (char *)0,
					"dc:creator", &(dp->dpAuthor) );

    docEpubEmitOpfMetaElementIfSet( xw, (char *)0,
					"dc:subject", &(dp->dpSubject) );
    docEpubEmitOpfMetaElementIfSet( xw, (char *)0,
					"dc:subject", &(dp->dpKeywords) );

    docEpubEmitOpfMetaElementIfSet( xw, (char *)0,
					"dc:description", &(dp->dpDoccomm) );
    docEpubEmitOpfMetaElementIfSet( xw, (char *)0,
					"dc:publisher", &(dp->dpCompany) );

    sioOutPutString( DocEpubOpfMetaTail, xw->xwSos );

  ready:

    utilCleanMemoryBuffer( &mbLang );

    return rval;
    }

/************************************************************************/

static int docEpubEmitManifestItem(	XmlWriter *		xw,
					const char *		id,
					const char *		href,
					const char *		mediaType )
    {
    sioOutPutString( "    <item", xw->xwSos );

    xmlWriteStringAttribute( xw, "id", id );
    xmlWriteStringAttribute( xw, "href", href );
    xmlWriteStringAttribute( xw, "media-type", mediaType );

    sioOutPutString( "/>\n", xw->xwSos );

    return 0;
    }

static int docEpubEmitSpineItem(	XmlWriter *		xw,
					const char *		id )
    {
    sioOutPutString( "    <itemref", xw->xwSos );

    xmlWriteStringAttribute( xw, "idref", id );

    sioOutPutString( "/>\n", xw->xwSos );

    return 0;
    }

/************************************************************************/

static int docEpubAddImageToOpf(	int			n,
					void *			vio,
					void *			vxw )
    {
    int				rval= 0;

    const InsertedObject *	io= (InsertedObject *)vio;
    const PictureProperties *	pip= &(io->ioPictureProperties);
    XmlWriter *			xw= (XmlWriter *)vxw;

    const char *		mimeType;
    const char *		ext;
    const MemoryBuffer *	mb= (const MemoryBuffer *)0;
    int				type;

    MemoryBuffer		href;
    char			id[20+1];
    const int			relative= 0;

    utilInitMemoryBuffer( &href );

    if  ( docHtmlObjectSaveHow( &type, &mimeType, &ext, &mb, io ) )
	{ goto ready;	}

    if  ( docEpubGetImageSrcX( &href, relative, n, io, ext )
									< 0 )
	{ LDEB(n); rval= -1; goto ready;	}

    if  ( 1 || pip->pipBliptag == 0 )
	{ sprintf( id, "i%d", n );			}
    else{ sprintf( id, "b%08lx", pip->pipBliptag );	}

    if  ( docEpubEmitManifestItem( xw, id, utilMemoryBufferGetString( &href ), mimeType ) )
	{ SDEB(id); rval= -1; goto ready;	}

  ready:

    utilCleanMemoryBuffer( &href );

    return rval;
    }

static int docEpubAddImagesToOpf(	const BufferDocument *	bd,
					XmlWriter *		xw )
    {
    utilPagedListForAll( &(bd->bdObjectList.iolPagedList),
					docEpubAddImageToOpf, (void *)xw );

    return 0;
    }

/************************************************************************/

static const char	DocEpubOpfManifestHead[]= "<manifest>\n";
static const char	DocEpubOpfManifestTail[]= "</manifest>\n";

static const char *	DocEpubSpineHead[]=
{
  "<spine toc=\"", DocEpubIdNcx, "\">\n",
};
static const char	DocEpubSpineTail[]= "</spine>\n";


static const char *	DocEpubOpfHead[]=
{
  DocEpubXmlDeclaration,
  "<package xmlns=\"http://www.idpf.org/2007/opf\" version=\"2.0\" unique-identifier=\"", DocEpubIdentifierName, "\">\n",
};
static const char	DocEpubOpfTail[]= "</package>\n";

static int docEpubStartOpf(	ZipOutput *		zo,
				XmlWriter *		xw,
				const MemoryBuffer *	title,
				const MemoryBuffer *	identifier,
				const BufferDocument *	bd )
    {
    const char * const		nameMember= DocEpubNameOpf;
    const int			compressed= 1;

    int				l;

    xw->xwSos= sioOutZipOpen( zo, nameMember, compressed );
    if  ( ! xw->xwSos )
	{ SXDEB(nameMember,xw->xwSos); return -1;	}

    for ( l= 0; l < sizeof(DocEpubOpfHead)/sizeof(char *); l++ )
	{ sioOutPutString( DocEpubOpfHead[l], xw->xwSos ); }

    if  ( docEpubEmitMetaData( xw, title, identifier, bd ) )
	{ SDEB(nameMember); return -1;	}

    return 0;
    }

static int docEpubEmitSimpleOpf(	ZipOutput *		zo,
					const MemoryBuffer *	title,
					const MemoryBuffer *	identifier,
					const BufferDocument *	bd )
    {
    int				rval= 0;
    int				l;

    XmlWriter			xw;

    xmlInitXmlWriter( &xw );

    if  ( docEpubStartOpf( zo, &xw, title, identifier, bd ) )
	{ LDEB(1); rval= -1; goto ready;	}

    sioOutPutString( DocEpubOpfManifestHead, xw.xwSos );

    docEpubEmitManifestItem( &xw,
			    DocEpubIdNcx, DocEpubNameNcx, DocEpubMediaNcx );
    docEpubEmitManifestItem( &xw,
			    DocEpubIdCss, DocEpubNameCssAbs, DocEpubMediaCss );
    docEpubEmitManifestItem( &xw,
			    DocEpubIdDoc, DocEpubNameDocAbs, DocEpubMediaDoc );

    docEpubAddImagesToOpf( bd, &xw );

    sioOutPutString( DocEpubOpfManifestTail, xw.xwSos );

    for ( l= 0; l < sizeof(DocEpubSpineHead)/sizeof(char *); l++ )
	{ sioOutPutString( DocEpubSpineHead[l], xw.xwSos ); }

    docEpubEmitSpineItem( &xw, DocEpubIdDoc );

    sioOutPutString( DocEpubSpineTail, xw.xwSos );

    sioOutPutString( DocEpubOpfTail, xw.xwSos );

  ready:

    if  ( xw.xwSos && sioOutClose( xw.xwSos ) )
	{ LDEB(1); rval= -1;	}

    return rval;
    }

/************************************************************************/

static int docEpubEmitMimeType(		ZipOutput *	zo )
    {
    int				rval= 0;

    SimpleOutputStream *	sosMember= (SimpleOutputStream *)0;
    const char *		nameMember= DocEpubNameMimetype;
    const int			compressed= 0;

    sosMember= sioOutZipOpen( zo, nameMember, compressed );
    if  ( ! sosMember )
	{ SXDEB(nameMember,sosMember); rval= -1; goto ready;	}

    sioOutPutString( DocEpubMediaEpub, sosMember );
    sioOutPutString( "\n", sosMember );

  ready:

    if  ( sosMember && sioOutClose( sosMember ) )
	{ SDEB(nameMember); rval= -1;	}

    return rval;
    }

/************************************************************************/

static void docEpubEmitNcxMetaElement(	XmlWriter *		xw,
					const char *		name,
					const char *		value )
    {
    sioOutPutString( "    <meta", xw->xwSos );

    xmlWriteStringAttribute( xw, "name", name );
    xmlWriteStringAttribute( xw, "content", value );

    sioOutPutString( "/>\n", xw->xwSos );
    }

static int docEpubEmitNcxHead(		XmlWriter *		xw,
					const MemoryBuffer *	title,
					const MemoryBuffer *	identifier,
					int			depth )
    {
    char		scratch[10];

    sprintf( scratch, "%d", depth );

    sioOutPutString( "    <head>\n", xw->xwSos );

    docEpubEmitNcxMetaElement( xw, "dtb:uid",
				    utilMemoryBufferGetString( identifier ) );
    docEpubEmitNcxMetaElement( xw, "dtb:depth", scratch );
    docEpubEmitNcxMetaElement( xw, "dtb:totalPageCount", "0" );
    docEpubEmitNcxMetaElement( xw, "dtb:maxPageNumber", "0" );

    sioOutPutString( "    </head>\n", xw->xwSos );

    sioOutPutString( "    <docTitle><text>", xw->xwSos );
    xmlEscapeCharacters( xw, (const char *)title->mbBytes, title->mbSize );
    sioOutPutString( "</text></docTitle>\n", xw->xwSos );

    return 0;
    }

/************************************************************************/

static int docEpubCloseNavPoint(	XmlWriter *		xw,
					int			level )
    {
    sioOutPrintf( xw->xwSos, "%*s</navPoint>\n", 4+ 4* level, "" );
    return 0;
    }

static int docEpubStartNavPoint(	XmlWriter *		xw,
					int			level,
					const char *		id,
					int			playOrder,
					const MemoryBuffer *	label,
					const char *		src )
    {
    sioOutPrintf( xw->xwSos, "%*s<navPoint", 4+ 4* level, "" );
    xmlWriteStringAttribute( xw, "id", id );
    xmlWriteIntAttribute( xw, "playOrder", playOrder );
    sioOutPutString( ">\n", xw->xwSos );

    sioOutPrintf( xw->xwSos, "%*s<navLabel><text>", 8+ 4* level, "" );
    xmlEscapeCharacters( xw, (const char *)label->mbBytes, label->mbSize );
    sioOutPutString( "</text></navLabel>\n", xw->xwSos );

    sioOutPrintf( xw->xwSos, "%*s<content", 8+ 4* level, "" );
    xmlWriteStringAttribute( xw, "src", src );
    sioOutPutString( "/>\n", xw->xwSos );

    return 0;
    }

/************************************************************************/

static const char *	DocEpubNcxHead[]=
{
  DocEpubXmlDeclaration,
  "<!DOCTYPE ncx PUBLIC \"-//NISO//DTD ncx 2005-1//EN\"\n",
  "  \"http://www.daisy.org/z3986/2005/ncx-2005-1.dtd\">\n",
  "<ncx xmlns=\"http://www.daisy.org/z3986/2005/ncx/\" version=\"2005-1\">\n",
};
static const char	DocEpubNcxTail[]= "</ncx>\n";

static int docEpubStartNcx(	ZipOutput *		zo,
				XmlWriter *		xw,
				const MemoryBuffer *	title,
				const MemoryBuffer *	identifier,
				int			depth,
				const BufferDocument *	bd )
    {
    const char * const		nameMember= DocEpubNameNcx;
    const int			compressed= 1;

    int				l;

    xw->xwSos= sioOutZipOpen( zo, nameMember, compressed );
    if  ( ! xw->xwSos )
	{ SXDEB(nameMember,xw->xwSos); return -1;	}

    for ( l= 0; l < sizeof(DocEpubNcxHead)/sizeof(char *); l++ )
	{ sioOutPutString( DocEpubNcxHead[l], xw->xwSos ); }

    if  ( docEpubEmitNcxHead( xw, title, identifier, depth ) )
	{ SDEB(nameMember); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Emit a Contents NCX that only holds the main document.		*/
/*									*/
/************************************************************************/

static int docEpubEmitSimpleNcx(	ZipOutput *		zo,
					const MemoryBuffer *	title,
					const MemoryBuffer *	identifier,
					const BufferDocument *	bd )
    {
    int				rval= 0;
    const int			depth= 1;

    int				playOrder= 1;

    XmlWriter			xw;

    xmlInitXmlWriter( &xw );

    if  ( docEpubStartNcx( zo, &xw, title, identifier, depth, bd ) )
	{ LDEB(1); rval= -1; goto ready;	}

    sioOutPutString( "    <navMap>\n", xw.xwSos );

    docEpubStartNavPoint( &xw, 0, DocEpubIdDoc, playOrder++,
						    title, DocEpubNameDocAbs );
    docEpubCloseNavPoint( &xw, 0 );

    sioOutPutString( "    </navMap>\n", xw.xwSos );

    sioOutPutString( DocEpubNcxTail, xw.xwSos );

  ready:

    if  ( xw.xwSos && sioOutClose( xw.xwSos ) )
	{ LDEB(1); rval= -1;	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Emit a Contents NCX that follows the structure of a table of	*/
/*  contents.								*/
/*									*/
/*  Note the toilsome mapping between the level numbers in the TOC and	*/
/*  those in the NCX. This is to cope with situations where the TOC	*/
/*  skips levels.							*/
/*									*/
/************************************************************************/

static int docEpubEmitCompositeNcx(	EpubWriter *		ew,
					const MemoryBuffer *	title,
					const MemoryBuffer *	identifier,
					const BufferDocument *	bd )
    {
    int				rval= 0;

    int				playOrder= 1;
    const TocEntry *		te;
    int				l;

    char			id[20+1];
    char			src[150+1];

    MemoryBuffer		mbLabel;
    XmlWriter			xw;

    int				ncxLevel;
    int				m[PPoutline_COUNT];

    utilInitMemoryBuffer( &mbLabel );
    xmlInitXmlWriter( &xw );

    if  ( docEpubStartNcx( &(ew->ewZipOutput), &xw,
			title, identifier, ew->ewCalculateToc.ctDepth, bd ) )
	{ LDEB(1); rval= -1; goto ready;	}

    sioOutPutString( "    <navMap>\n", xw.xwSos );

    for ( ncxLevel= 0; ncxLevel < PPoutline_COUNT; ncxLevel++ )
	{ m[ncxLevel]= 0;	}
    ncxLevel= 0;

    te= ew->ewCalculateToc.ctEntries;
    for ( l= 0; l < ew->ewCalculateToc.ctEntryCount; te++, l++ )
	{
#	if 0
	sioOutPrintf( xw.xwSos, "%*s<--  m[%d]=%d teLevel= %d -->\n",
			    4+ 4* ncxLevel, "",
			    ncxLevel, m[ncxLevel], te->teLevel );
#	endif

	while( m[ncxLevel] > te->teLevel )
	    { docEpubCloseNavPoint( &xw, --ncxLevel );	}

	sprintf( src, "%s#%.*s", DocEpubNameDocAbs,
					    te->teMarkName->mbSize,
					    te->teMarkName->mbBytes );

	utilEmptyMemoryBuffer( &mbLabel );
	if  ( docCalculateRefFieldValue( &mbLabel, te->teMarkName, bd ) )
	    { LDEB(1); rval= -1; goto ready;	}

	sprintf( id, "np-%d", playOrder );
	docEpubStartNavPoint( &xw, ncxLevel, id, playOrder++, &mbLabel, src );
	m[++ncxLevel]= te->teLevel+ 1;

#	if 0
	sioOutPrintf( xw.xwSos, "%*s<--  m[%d]:=%d teLevel= %d -->\n",
			    4+ 4* ncxLevel, "",
			    ncxLevel, m[ncxLevel], te->teLevel );
#	endif
	}

    while( ncxLevel > 0 )
	{ docEpubCloseNavPoint( &xw, --ncxLevel );	}

    sioOutPutString( "    </navMap>\n", xw.xwSos );

    sioOutPutString( DocEpubNcxTail, xw.xwSos );

  ready:

    utilCleanMemoryBuffer( &mbLabel );

    if  ( xw.xwSos && sioOutClose( xw.xwSos ) )
	{ LDEB(1); rval= -1;	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Return the name of the CSS file.					*/
/*									*/
/************************************************************************/

static int docEpubGetCssNameX(		MemoryBuffer *		target,
					int			relative )
    {
    if  ( relative )
	{
	if  ( utilMemoryBufferSetString( target,DocEpubNameCssRel ) )
	    { LDEB(1); return -1;	}

	return 0;
	}
    else{
	if  ( utilMemoryBufferSetString( target,DocEpubNameCssAbs ) )
	    { LDEB(1); return -1;	}

	return 0;
	}
    }

static int docEpubGetCssName(		MemoryBuffer *		target,
					HtmlWritingContext *	hwc )
    {
    const int		relative= 1;

    return docEpubGetCssNameX( target, relative );
    }

/************************************************************************/
/*									*/
/*  Emit the stylesheet							*/
/*									*/
/************************************************************************/

static int docEpubWriteCss(		HtmlWritingContext *	hwc )
    {
    int				rval= 0;

    EpubWriter *		ew= (EpubWriter *)hwc->hwcPrivate;
    const int			relative= 0;

    MemoryBuffer		nameMember;
    SimpleOutputStream *	sosCss= (SimpleOutputStream *)0;
    const int			compressed= 1;

    utilInitMemoryBuffer( &nameMember );

    if  ( docEpubGetCssNameX( &nameMember, relative ) < 0 )
	{ LDEB(1); rval= -1; goto ready;	}

    sosCss= sioOutZipOpen( &(ew->ewZipOutput),
			utilMemoryBufferGetString( &nameMember ), compressed );
    if  ( ! sosCss )
	{ XDEB(sosCss); rval= -1; goto ready;	}

    if  ( docHtmlSaveDocumentStyles( hwc, sosCss ) )
	{ LDEB(1); rval= -1; goto ready; }

  ready:

    utilCleanMemoryBuffer( &nameMember );

    if  ( sosCss )
	{ sioOutClose( sosCss );	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Emit an actual document.						*/
/*									*/
/************************************************************************/

static int docEpubEmitDocument(	HtmlWritingContext *		hwc,
				BufferDocument *		bd,
				const char *			relativeName,
				const DocumentSelection *	ds )
    {
    int				rval= 0;
    EpubWriter *		ew= (EpubWriter *)hwc->hwcPrivate;

    char			absoluteName[200+1];
    SimpleOutputStream *	sosDoc= (SimpleOutputStream *)0;
    const int			compressed= 1;

    if  ( DocEpubNameContentLen+ 1+ strlen(relativeName)+ 1 >=
							sizeof(absoluteName) )
	{ SDEB(relativeName); rval= -1; goto ready;	}

    sprintf( absoluteName, "%s/%s", DocEpubNameContent, relativeName );

    sosDoc= sioOutZipOpen( &(ew->ewZipOutput), absoluteName, compressed );
    if  ( ! sosDoc )
	{ SXDEB(absoluteName,sosDoc); rval= -1; goto ready;	}

    hwc->hwcXmlWriter.xwSos= sosDoc;
    hwc->hwcXmlWriter.xwColumn= 0;

    if  ( docHtmlStartDocument( hwc ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( docHtmlSaveSelection( hwc, &(bd->bdBody), ds ) )
	{ XDEB(ds); rval= -1; goto ready;	}

    if  ( ! hwc->hwcInlineNotes )
	{
	if  ( hwc->hwcNoteRefCount > 0	&&
	      docHtmlSaveNotes( hwc )	)
	    { LDEB(hwc->hwcNoteRefCount); rval= -1; goto ready;	}
	}

    if  ( docHtmlFinishDocument( hwc ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    hwc->hwcXmlWriter.xwSos= (SimpleOutputStream *)0; /* Only for this doc! */

    if  ( sosDoc )
	{ sioOutClose( sosDoc );	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Find the table of contents of the document.				*/
/*									*/
/************************************************************************/

static DocumentField * docHtmlGetTocForEpub(	TocField *		tf,
						BufferDocument *	bd )
    {
    const DocumentFieldList *	dfl= &(bd->bdFieldList);
    const int			fieldCount= dfl->dflPagedList.plItemCount;
    int				fieldNr;

    DocumentField *		rval= (DocumentField *)0;

    for ( fieldNr= 0; fieldNr < fieldCount; fieldNr++ )
	{
	DocumentField *		df= docGetFieldByNumber( dfl, fieldNr );

	if  ( ! df						||
	      df->dfKind != DOCfkTOC				||
	      df->dfSelectionScope.ssTreeType != DOCinBODY	)
	    { continue;	}

	docFieldGetToc( tf, df );
	if  ( tf->tfType == TOCtypeTOC )
	    { rval= df; break;	}
	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Save a document to EPUB.						*/
/*									*/
/************************************************************************/

int docEpubSaveDocument(	SimpleOutputStream *	sos,
				BufferDocument *	bd,
				const LayoutContext *	lc )
    {
    HtmlWritingContext		hwc;
    int				rval= 0;
    EpubWriter			ew;

    const DocumentProperties *	dp= &(bd->bdProperties);
    DocumentField *		dfToc;

    MemoryBuffer		identifier;
    MemoryBuffer		title;

    const char * const		identifier_X= "Identifier";
    const char * 		title_X= "UnknownTitle";

    utilInitMemoryBuffer( &identifier );
    utilInitMemoryBuffer( &title );

    docInitHtmlWritingContext( &hwc );
    docInitEpubWriter( &ew );

    if  ( utilMemoryBufferSetString( &identifier, identifier_X ) )
	{ LDEB(1); rval= -1; goto ready;	}
    if  ( utilMemoryBufferSetString( &identifier, title_X ) )
	{ LDEB(1); rval= -1; goto ready;	}

    hwc.hwcSupportsBullets= 0;
    hwc.hwcEmitBackground= 0;

    ew.ewZipOutput.zoSosZip= sos;

    ew.ewCalculateToc.ctBdDoc= bd;

    hwc.hwcXmlWriter.xwSos= (SimpleOutputStream *)0; /* sos is not for HTML */
    hwc.hwcXmlWriter.xwCrlf= 0;

    hwc.hwcLayoutContext= lc;
    hwc.hwcOpenImageStream= docEpubOpenImageStream;
    hwc.hwcGetImageSrc= docEpubGetImageSrc;
    hwc.hwcGetCssName= docEpubGetCssName;
    hwc.hwcPrivate= (void *)&ew;
    hwc.hwcDocument= bd;
    hwc.hwcInlineCss= 0;
    hwc.hwcInlineNotes= 1;

    if  ( ! utilMemoryBufferIsEmpty( &(dp->dpTitle) ) )
	{
	if  ( utilCopyMemoryBuffer( &title, &(dp->dpTitle) ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}

    if  ( docEpubEmitMimeType( &(ew.ewZipOutput) ) )
	{ LDEB(1); rval= -1; goto ready;	}
    if  ( docEpubEmitContainerXml( &(ew.ewZipOutput) ) )
	{ LDEB(1); rval= -1; goto ready;	}

    dfToc= docHtmlGetTocForEpub( &(ew.ewCalculateToc.ctTocField), bd );
    if  ( dfToc )
	{
	if  ( docCollectTocInput( &(ew.ewCalculateToc) ) )
	    { LDEB(1); rval= -1; goto ready;	}

	if  ( docEpubEmitSimpleOpf( &(ew.ewZipOutput),
						&title, &identifier, bd ) )
	    { LDEB(1); rval= -1; goto ready;	}
	if  ( docEpubEmitCompositeNcx( &ew, &title, &identifier, bd ) )
	    { LDEB(1); rval= -1; goto ready;	}
	if  ( docEpubEmitDocument( &hwc, bd, DocEpubNameDocRel,
					    (const DocumentSelection *)0 ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}
    else{
	if  ( docEpubEmitSimpleOpf( &(ew.ewZipOutput),
						&title, &identifier, bd ) )
	    { LDEB(1); rval= -1; goto ready;	}
	if  ( docEpubEmitSimpleNcx( &(ew.ewZipOutput),
						&title, &identifier, bd ) )
	    { LDEB(1); rval= -1; goto ready;	}
	if  ( docEpubEmitDocument( &hwc, bd, DocEpubNameDocRel,
					    (const DocumentSelection *)0 ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}

    if  ( ! hwc.hwcInlineCss && docEpubWriteCss( &hwc ) )
	{ LDEB(1); rval= -1; return -1;	}

    if  ( docHtmlSaveImages( &hwc ) )
	{ LDEB(hwc.hwcImageCount); rval= -1; goto ready;	}

    if  ( sioZipFlushOutput( &(ew.ewZipOutput) ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    utilCleanMemoryBuffer( &identifier );
    utilCleanMemoryBuffer( &title );

    docCleanEpubWriter( &ew );
    docCleanHtmlWritingContext( &hwc );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Get the 'src' value for an embedded image.				*/
/*									*/
/************************************************************************/

static int docEpubGetImageSrcX(		MemoryBuffer *		target,
					int			relative,
					int			n,
					const InsertedObject *	io,
					const char *		ext )
    {
    const PictureProperties *	pip= &(io->ioPictureProperties);

    const char *		dir;

    if  ( relative )
	{
	dir= DocEpubNameMediaRel;
	/*dirlen= DocEpubNameMediaLenRel;*/
	}
    else{
	dir= DocEpubNameMediaAbs;
	/*dirlen= DocEpubNameMediaLenAbs;*/
	}

    if  ( 1 || pip->pipBliptag == 0 )
	{ utilMemoryBufferPrintf( target, "%s/i%d.%s", dir, n, ext );			}
    else{ utilMemoryBufferPrintf( target, "%s/b%08lx.%s", dir, pip->pipBliptag, ext );	}

    return 0;
    }

static int docEpubGetImageSrc(		MemoryBuffer *		target,
					HtmlWritingContext *	hwc,
					int			n,
					const InsertedObject *	io,
					const char *		ext )
    {
    const int		relative= 1;

    return docEpubGetImageSrcX( target, relative, n, io, ext );
    }

/************************************************************************/
/*									*/
/*  Open the file to write an image.					*/
/*									*/
/************************************************************************/

static SimpleOutputStream * docEpubOpenImageStream(
					HtmlWritingContext *	hwc,
					int			n,
					const InsertedObject *	io,
					const char *		mimeType,
					const char *		ext )
    {
    EpubWriter *		ew= (EpubWriter *)hwc->hwcPrivate;
    SimpleOutputStream *	sosImage= (SimpleOutputStream *)0;
    const int			relative= 0;
    const int			compressed= 1;

    MemoryBuffer		src;

    utilInitMemoryBuffer( &src );

    if  ( docEpubGetImageSrcX( &src, relative, n, io, ext ) < 0 )
	{ LDEB(1); goto ready;	}

    sosImage= sioOutZipOpen( &(ew->ewZipOutput),
				utilMemoryBufferGetString( &src ), compressed );
    if  ( ! sosImage )
	{ XDEB(sosImage); goto ready; }

  ready:

    utilCleanMemoryBuffer( &src );

    return sosImage;
    }
