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

#   include	<appSystem.h>
#   include	<sioGeneral.h>
#   include	<sioFileio.h>

#   include	<docBuf.h>
#   include	"docHtmlWriteImpl.h"

#   include	<appDebugon.h>

typedef struct HtmlWriter
    {
			/**
			 * The file name if any
			 */
    const MemoryBuffer *	hwFilename;
    MemoryBuffer		hwRelativeName;
    MemoryBuffer		hwFilesDirectoryAbs;
    MemoryBuffer		hwFilesDirectoryRel;
    } HtmlWriter;

static SimpleOutputStream * docHtmlOpenImageStream(
					HtmlWritingContext *	hwc,
					int			n,
					const InsertedObject *	io,
					const char *		mimeType,
					const char *		ext );

static int docHtmlGetImageSrc(		MemoryBuffer *		target,
					HtmlWritingContext *	hwc,
					int			n,
					const InsertedObject *	io,
					const char *		ext );

static int docHtmlGetCssName(		MemoryBuffer *		target,
					HtmlWritingContext *	hwc );

static int docDocHtmlWriteCss(		HtmlWritingContext *	hwc );

static void docInitHtmlWriter(	HtmlWriter *	hw )
    {
    hw->hwFilename= (const MemoryBuffer *)0;
    utilInitMemoryBuffer( &(hw->hwRelativeName) );
    utilInitMemoryBuffer( &(hw->hwFilesDirectoryAbs) );
    utilInitMemoryBuffer( &(hw->hwFilesDirectoryRel) );
    }

static void docCleanHtmlWriter(	HtmlWriter *	hw )
    {
    utilCleanMemoryBuffer( &(hw->hwRelativeName) );
    utilCleanMemoryBuffer( &(hw->hwFilesDirectoryAbs) );
    utilCleanMemoryBuffer( &(hw->hwFilesDirectoryRel) );
    }

/**
 *  Added to the file name at hwBaseLength to make up the name
 *  of the media directory.
 */
const char DocHtmlDirectorySuffix[]= "_files";
const int  DocHtmlDirectorySuffixLength= sizeof(DocHtmlDirectorySuffix)- 1;

/************************************************************************/
/*									*/
/*  Save a document to HTML.						*/
/*									*/
/************************************************************************/

int docHtmlSaveDocument(	SimpleOutputStream *	sos,
				BufferDocument *	bd,
				const MemoryBuffer *	filename,
				const LayoutContext *	lc )
    {
    int				rval= 0;
    HtmlWritingContext		hwc;
    HtmlWriter			hw;

    if  ( filename && utilMemoryBufferIsEmpty( filename ) )
	{ filename= (const MemoryBuffer *)0;	}

    docInitHtmlWritingContext( &hwc );
    docInitHtmlWriter( &hw );

    hwc.hwcSupportsBullets= 1;
    hwc.hwcEmitBackground= 1;

    hwc.hwcXmlWriter.xwSos= sos;
    hwc.hwcXmlWriter.xwCrlf= 0;

    hwc.hwcLayoutContext= lc;
    hwc.hwcOpenImageStream= docHtmlOpenImageStream;
    hwc.hwcGetImageSrc= docHtmlGetImageSrc;
    hwc.hwcGetCssName= docHtmlGetCssName;
    hwc.hwcPrivate= (void *)&hw;
    hwc.hwcDocument= bd;
    hwc.hwcInlineCss= ( bd->bdObjectList.iolPagedList.plItemCount == 0 || ! filename );
    hwc.hwcInlineNotes= 0;

    hw.hwFilename= filename;

    if  ( hw.hwFilename )
	{
	if  ( appFileGetRelativeName( &(hw.hwRelativeName), hw.hwFilename ) )
	    { LDEB(1); rval= -1; goto ready;	}

	if  ( utilCopyMemoryBuffer( &(hw.hwFilesDirectoryAbs),
						    hw.hwFilename ) )
	    { LDEB(1); rval= -1; goto ready;	}
	if  ( utilCopyMemoryBuffer( &(hw.hwFilesDirectoryRel),
						    &(hw.hwRelativeName) ) )
	    { LDEB(1); rval= -1; goto ready;	}

	if  ( appFileSetExtension( &(hw.hwFilesDirectoryAbs),
						    (const char *)0 ) )
	    { LDEB(1); rval= -1; goto ready;	}
	if  ( appFileSetExtension( &(hw.hwFilesDirectoryRel),
						    (const char *)0 ) )
	    { LDEB(1); rval= -1; goto ready;	}

	if  ( utilMemoryBufferAppendBytes( &(hw.hwFilesDirectoryAbs),
				(const unsigned char *)DocHtmlDirectorySuffix,
				DocHtmlDirectorySuffixLength ) )
	    { LDEB(1); rval= -1; goto ready;	}
	if  ( utilMemoryBufferAppendBytes( &(hw.hwFilesDirectoryRel),
				(const unsigned char *)DocHtmlDirectorySuffix,
				DocHtmlDirectorySuffixLength ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}

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

    if  ( hwc.hwcImageCount > 0	|| ! hwc.hwcInlineCss )
	{
	if  ( appTestDirectory( &(hw.hwFilesDirectoryAbs) )	&&
	      appMakeDirectory( &(hw.hwFilesDirectoryAbs) )	)
	    { LDEB(1); rval= -1; goto ready;	}
	}

    if  ( ! hwc.hwcInlineCss && docDocHtmlWriteCss( &hwc ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( docHtmlSaveImages( &hwc ) )
	{ LDEB(hwc.hwcImageCount); rval= -1; goto ready;	}

  ready:

    docCleanHtmlWritingContext( &hwc );
    docCleanHtmlWriter( &hw );

    return rval;
    }

/************************************************************************/

static int docHtmlGetNameX(		MemoryBuffer *		target,
					const HtmlWriter *	hw,
					int			relative,
					const char *		slashName )
    {
    if  ( relative )
	{
	if  ( utilCopyMemoryBuffer( target, &(hw->hwFilesDirectoryRel) ) )
	    { LDEB(1); return -1;	}
	}
    else{
	if  ( utilCopyMemoryBuffer( target, &(hw->hwFilesDirectoryAbs) ) )
	    { LDEB(1); return -1;	}
	}

    if  ( utilMemoryBufferAppendBytes( target,
		(const unsigned char *)slashName, strlen( slashName ) ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Get the 'src' value for an embedded image.				*/
/*									*/
/************************************************************************/

static int docHtmlGetImageSrcX(		MemoryBuffer *		target,
					HtmlWriter *		hw,
					int			relative,
					int			n,
					const InsertedObject *	io,
					const char *		ext )
    {
    const PictureProperties *	pip= &(io->ioPictureProperties);
    char			scratch[50];

    if  ( 1 || pip->pipBliptag == 0 )
	{ sprintf( scratch, "/i%d.%s", n, ext );			}
    else{ sprintf( scratch, "/b%08lx.%s", pip->pipBliptag, ext );	}

    if  ( docHtmlGetNameX( target, hw, relative, scratch ) )
	{ SDEB(scratch); return -1;	}

    return 0;
    }

static int docHtmlGetImageSrc(		MemoryBuffer *		target,
					HtmlWritingContext *	hwc,
					int			n,
					const InsertedObject *	io,
					const char *		ext )
    {
    HtmlWriter *	hw= (HtmlWriter *)hwc->hwcPrivate;
    const int		relative= 1;

    return docHtmlGetImageSrcX( target, hw, relative, n, io, ext );
    }

/************************************************************************/
/*									*/
/*  Open the file to write an image.					*/
/*									*/
/************************************************************************/

static SimpleOutputStream * docHtmlOpenImageStream(
					HtmlWritingContext *	hwc,
					int			n,
					const InsertedObject *	io,
					const char *		mimeType,
					const char *		ext )
    {
    HtmlWriter *		hw= (HtmlWriter *)hwc->hwcPrivate;
    SimpleOutputStream *	sosImage= (SimpleOutputStream *)0;
    const int			relative= 0;

    MemoryBuffer		src;

    utilInitMemoryBuffer( &src );

    if  ( docHtmlGetImageSrcX( &src, hw, relative, n, io, ext ) < 0 )
	{ LDEB(1); goto ready;	}

    sosImage= sioOutFileioOpen( &src );
    if  ( ! sosImage )
	{ XDEB(sosImage); return sosImage; }

  ready:

    utilCleanMemoryBuffer( &src );

    return sosImage;
    }

/************************************************************************/
/*									*/
/*  Return the name of the CSS file.					*/
/*									*/
/************************************************************************/

static int docHtmlGetCssNameX(		MemoryBuffer *		target,
					int			relative,
					HtmlWriter *		hw )
    {
    const char DocHtmlCssName[]= "/document.css";

    if  ( docHtmlGetNameX( target, hw, relative, DocHtmlCssName ) )
	{ SDEB(DocHtmlCssName); return -1;	}

    return 0;
    }

static int docHtmlGetCssName(		MemoryBuffer *		target,
					HtmlWritingContext *	hwc )
    {
    HtmlWriter *	hw= (HtmlWriter *)hwc->hwcPrivate;
    const int		relative= 1;

    return docHtmlGetCssNameX( target, relative, hw );
    }

static int docDocHtmlWriteCss(		HtmlWritingContext *	hwc )
    {
    int				rval= 0;

    HtmlWriter *		hw= (HtmlWriter *)hwc->hwcPrivate;
    const int			relative= 0;

    MemoryBuffer		nameCss;
    SimpleOutputStream *	sosCss= (SimpleOutputStream *)0;

    utilInitMemoryBuffer( &nameCss );

    if  ( docHtmlGetCssNameX( &nameCss, relative, hw ) < 0 )
	{ LDEB(1); rval= -1; goto ready;	}

    sosCss= sioOutFileioOpen( &nameCss );
    if  ( ! sosCss )
	{ XDEB(sosCss); rval= -1; goto ready;	}

    if  ( docHtmlSaveDocumentStyles( hwc, sosCss ) )
	{ LDEB(1); rval= -1; goto ready; }

  ready:

    utilCleanMemoryBuffer( &nameCss );

    if  ( sosCss )
	{ sioOutClose( sosCss );	}

    return rval;
    }
