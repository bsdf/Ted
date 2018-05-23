/************************************************************************/
/*									*/
/*  Save a BufferDocument into an HTML file.				*/
/*  Depending on the parameters, this is either an HTML file with	*/
/*  a directory for the images, or a MHTML (rfc2112,rfc2557) aggregate.	*/
/*  RFC 2557 was never validated.					*/
/*									*/
/************************************************************************/

#   include	"docHtmlConfig.h"

#   include	<stdio.h>
#   include	<ctype.h>

#   include	<appSystem.h>
#   include	<sioGeneral.h>
#   include	<sioBase64.h>
#   include	<sioPushed.h>
#   include	<utilMemoryBufferPrintf.h>

#   include	<docBuf.h>
#   include	"docHtmlWriteImpl.h"

#   include	<appDebugon.h>

#   define	IDlenTAIL	200

typedef struct EmlWriter
    {
    const char *	ewMimeBoundary;
    char		ewContentIdTail[1+IDlenTAIL+1];
    } EmlWriter;

static SimpleOutputStream * docEmlOpenImageStream(
					HtmlWritingContext *	hwc,
					int			n,
					const InsertedObject *	io,
					const char *		mimeType,
					const char *		ext );

static int docEmlGetImageSrc(		MemoryBuffer *		target,
					HtmlWritingContext *	hwc,
					int			n,
					const InsertedObject *	io,
					const char *		ext );

static SimpleOutputStream * docEmlOpenCssStream(
					HtmlWritingContext *	hwc );

static int docEmlGetCssName(		MemoryBuffer *		target,
					HtmlWritingContext *	hwc );

static int docEmlGetCssNameX(		MemoryBuffer *		target,
					int			relative,
					EmlWriter *		ew );

static void docInitEmlWriter(	EmlWriter *	ew )
    {
    ew->ewMimeBoundary= (const char *)0;
    ew->ewContentIdTail[0]= '\0';
    }

const char DocEmlDirectoryCssName[]= "document.css";
const int  DocEmlDirectoryCssNameLength= sizeof(DocEmlDirectoryCssName)- 1;

/************************************************************************/
/*									*/
/*  Save a document to EML: A RFC 2557 compliant mail message.		*/
/*									*/
/************************************************************************/

int docEmlSaveDocument(		SimpleOutputStream *	sos,
				BufferDocument *	bd,
				const char *		mimeBoundary,
				const LayoutContext *	lc )
    {
    int				rval= 0;
    HtmlWritingContext		hwc;
    EmlWriter			ew;

    const DocumentProperties *	dp= &(bd->bdProperties);
    SimpleOutputStream *	sosCss= (SimpleOutputStream *)0;

    docInitHtmlWritingContext( &hwc );
    docInitEmlWriter( &ew );

    hwc.hwcSupportsBullets= 0;
    hwc.hwcEmitBackground= 1;

    hwc.hwcXmlWriter.xwSos= sos;
    hwc.hwcXmlWriter.xwCrlf= 1;

    hwc.hwcLayoutContext= lc;
    hwc.hwcOpenImageStream= docEmlOpenImageStream;
    hwc.hwcGetImageSrc= docEmlGetImageSrc;
    hwc.hwcGetCssName= docEmlGetCssName;
    hwc.hwcPrivate= (void *)&ew;
    hwc.hwcDocument= bd;
    hwc.hwcInlineCss= 1;
    hwc.hwcInlineNotes= 0;

    ew.ewMimeBoundary= mimeBoundary;
    ew.ewContentIdTail[0]= '.';
    if  ( appMakeUniqueString( ew.ewContentIdTail+ 1, IDlenTAIL ) )
	{ LDEB(IDlenTAIL); rval= -1; goto ready;	}

    /* {WHOLE */
    if  ( ! utilMemoryBufferIsEmpty( &(dp->dpTitle) ) )
	{
	sioOutPutString( "Subject: ", sos );
	sioOutPutString( utilMemoryBufferGetString( &(dp->dpTitle) ), sos );
	sioOutPutString( "\r\n", sos );
	}

    sioOutPutString( "MIME-Version: 1.0", sos );
    sioOutPutString( "\r\n", sos );

    sioOutPutString( "Content-Transfer-Encoding: 8bit", sos );
    sioOutPutString( "\r\n", sos );

    sioOutPutString( "Content-Type: multipart/related; boundary=\"", sos );
    sioOutPutString( mimeBoundary, sos );
    sioOutPutString( "\"", sos );
    sioOutPutString( "\r\n", sos );

    sioOutPutString( "\r\n", sos );
    /* WHOLE} */

    sioOutPutString( "--", sos );
    sioOutPutString( ew.ewMimeBoundary, sos );
    sioOutPutString( "\r\n", sos );
    sioOutPutString( "Content-Type: text/html; charset=UTF-8", sos );
    sioOutPutString( "\r\n", sos );
    sioOutPutString( "Content-Transfer-Encoding: 8bit", sos );
    sioOutPutString( "\r\n", sos );

    sioOutPutString( "\r\n", sos );

    if  ( docHtmlStartDocument( &hwc ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( docHtmlSaveSelection( &hwc, &(bd->bdBody),
					(const DocumentSelection *)0 ) )
	{ LDEB(1); rval= -1; goto ready; }

    if  ( ! hwc.hwcInlineNotes )
	{
	if  ( hwc.hwcNoteRefCount > 0	&&
	      docHtmlSaveNotes( &hwc )	)
	    { LDEB(hwc.hwcNoteRefCount); rval= -1; goto ready;	}
	}

    if  ( docHtmlFinishDocument( &hwc ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( ! hwc.hwcInlineCss )
	{
	sosCss= docEmlOpenCssStream( &hwc );
	if  ( ! sosCss )
	    { XDEB(sosCss); rval= -1; goto ready;	}

	if  ( docHtmlSaveDocumentStyles( &hwc, sosCss ) )
	    { SDEB(DocEmlDirectoryCssName); rval= -1; goto ready; }

	sioOutClose( sosCss ); sosCss= (SimpleOutputStream *)0;
	}

    if  ( hwc.hwcImageCount > 0		&&
	  docHtmlSaveImages( &hwc )	)
	{ LDEB(hwc.hwcImageCount); rval= -1; goto ready;	}

    sioOutPutString( "--", sos );
    sioOutPutString( ew.ewMimeBoundary, sos );
    sioOutPutString( "--", sos );
    sioOutPutString( "\r\n", sos );

  ready:

    if  ( sosCss )
	{ sioOutClose( sosCss );	}

    docCleanHtmlWritingContext( &hwc );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Get the 'src' value for an embedded image.				*/
/*									*/
/************************************************************************/

static int docEmlGetImageSrcX(		MemoryBuffer *		target,
					EmlWriter *		ew,
					int			relative,
					int			n,
					const InsertedObject *	io,
					const char *		ext )
    {
    const PictureProperties *	pip= &(io->ioPictureProperties);

    const char *		prefix;
    const char *		suffix;

    switch( relative )
	{
	case 0:
	    prefix= "";
	    suffix= ew->ewContentIdTail;
	    break;

	case 1:
	    prefix= "cid:";
	    suffix= ew->ewContentIdTail;
	    break;

	case 2:
	    prefix= "";
	    suffix= "";
	    break;

	default:
	    LDEB(relative); return -1;
	}

    if  ( 1 || pip->pipBliptag == 0 )
	{
	utilMemoryBufferPrintf( target, "%si%d.%s%s",
				    prefix, n, ext, suffix );
	}
    else{
	utilMemoryBufferPrintf( target, "%sb%08lx.%s%s",
				    prefix, pip->pipBliptag, ext, suffix );
	}

    return 0;
    }

static int docEmlGetImageSrc(		MemoryBuffer *		target,
					HtmlWritingContext *	hwc,
					int			n,
					const InsertedObject *	io,
					const char *		ext )
    {
    EmlWriter *		ew= (EmlWriter *)hwc->hwcPrivate;
    const int		relative= 1;

    return docEmlGetImageSrcX( target, ew, relative, n, io, ext );
    }

/************************************************************************/
/*									*/
/*  Start a mime part.							*/
/*									*/
/************************************************************************/

static int docEmlStartMimePart(	SimpleOutputStream *	sos,
				const EmlWriter *	ew,
				const char *		mimeType,
				const MemoryBuffer *	contentID,
				const char *		contentTxEnc,
				const MemoryBuffer *	fileName )
    {
    sioOutPutString( "--", sos );
    sioOutPutString( ew->ewMimeBoundary, sos );
    sioOutPutString( "\r\n", sos );

    sioOutPutString( "Content-Type: ", sos );
    sioOutPutString( mimeType, sos );
    sioOutPutString( "\r\n", sos );

    sioOutPutString( "Content-Id: <", sos );
    sioOutPutString( utilMemoryBufferGetString( contentID ), sos );
    (void)sioOutPutByte( '>', sos );
    sioOutPutString( "\r\n", sos );

    sioOutPutString( "Content-Transfer-Encoding: ", sos );
    sioOutPutString( contentTxEnc, sos );
    sioOutPutString( "\r\n", sos );

    sioOutPutString( "Content-Disposition: inline;", sos );
    sioOutPutString( "\r\n", sos );
    sioOutPutString( " filename=\"", sos );
    sioOutPutString( utilMemoryBufferGetString( fileName ), sos );
    (void)sioOutPutByte( '"', sos );
    sioOutPutString( "\r\n", sos );

    sioOutPutString( "\r\n", sos );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Start a mime part for an image.					*/
/*									*/
/************************************************************************/

static SimpleOutputStream * docEmlOpenImageStream(
					HtmlWritingContext *	hwc,
					int			n,
					const InsertedObject *	io,
					const char *		mimeType,
					const char *		ext )
    {
    EmlWriter *			ew= (EmlWriter *)hwc->hwcPrivate;

    SimpleOutputStream *	sosImage= (SimpleOutputStream *)0;
    MemoryBuffer		contentID;
    MemoryBuffer		fileName;

    utilInitMemoryBuffer( &contentID );
    utilInitMemoryBuffer( &fileName );

    if  ( docEmlGetImageSrcX( &contentID, ew, /*r:*/ 0, n, io, ext ) < 0 )
	{ SDEB(ext); goto ready;	}

    if  ( docEmlGetImageSrcX( &fileName, ew, /*r:*/ 2, n, io, ext ) < 0 )
	{ SDEB(ext); goto ready;	}

    if  ( docEmlStartMimePart( hwc->hwcXmlWriter.xwSos, ew, mimeType,
					    &contentID, "base64", &fileName ) )
	{ SDEB(ext); goto ready;	}

    sosImage= sioOutBase64Open( hwc->hwcXmlWriter.xwSos );
    if  ( ! sosImage )
	{ XDEB(sosImage); goto ready; }

  ready:

    utilCleanMemoryBuffer( &contentID );
    utilCleanMemoryBuffer( &fileName );

    return sosImage;
    }

/************************************************************************/
/*									*/
/*  Return the name of the CSS file. An empty name means inline CSS.	*/
/*									*/
/************************************************************************/

static int docEmlGetCssNameX(		MemoryBuffer *		target,
					int			relative,
					EmlWriter *		ew )
    {
    const char *	prefix;
    const char *	suffix;

    switch( relative )
	{
	case 0:
	    prefix= "";
	    suffix= ew->ewContentIdTail;
	    break;

	case 1:
	    prefix= "cid:";
	    suffix= ew->ewContentIdTail;
	    break;

	case 2:
	    prefix= "";
	    suffix= "";
	    break;

	default:
	    LDEB(relative); return -1;
	}

    utilMemoryBufferPrintf( target,
			"%s%s%s", prefix, DocEmlDirectoryCssName, suffix );

    return 0;
    }

static int docEmlGetCssName(		MemoryBuffer *		target,
					HtmlWritingContext *	hwc )
    {
    EmlWriter *		ew= (EmlWriter *)hwc->hwcPrivate;
    const int		relative= 1;

    return docEmlGetCssNameX( target, relative, ew );
    }

/************************************************************************/
/*									*/
/*  Start a mime part for an image.					*/
/*									*/
/************************************************************************/

static SimpleOutputStream * docEmlOpenCssStream(
					HtmlWritingContext *	hwc )
    {
    EmlWriter *			ew= (EmlWriter *)hwc->hwcPrivate;

    SimpleOutputStream *	sosCss= (SimpleOutputStream *)0;
    MemoryBuffer		contentID;
    MemoryBuffer		fileName;

    utilInitMemoryBuffer( &contentID );
    utilInitMemoryBuffer( &fileName );

    if  ( docEmlGetCssNameX( &contentID, /*r:*/ 0, ew ) < 0 )
	{ LDEB(1); goto ready;	}

    if  ( docEmlGetCssNameX( &fileName, /*r:*/ 2, ew ) < 0 )
	{ LDEB(1); goto ready;	}

    if  ( docEmlStartMimePart( hwc->hwcXmlWriter.xwSos, ew,
					    "text/css; charset=UTF-8",
					    &contentID, "8bit", &fileName ) )
	{ LDEB(1); goto ready;	}

    sosCss= sioOutPushedOpen( hwc->hwcXmlWriter.xwSos );
    if  ( ! sosCss )
	{ XDEB(sosCss); return sosCss; }

  ready:

    utilCleanMemoryBuffer( &contentID );
    utilCleanMemoryBuffer( &fileName );

    return sosCss;
    }

