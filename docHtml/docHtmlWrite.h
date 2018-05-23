/************************************************************************/
/*									*/
/*  Save a BufferDocument into an HTML file.				*/
/*  Depending on the parameters, this is either an HTML file with	*/
/*  a directory for the images, or a MHTML (rfc2112,rfc2557) aggregate.	*/
/*  RFC 2557 was never validated.					*/
/*									*/
/*  Additionally, the HTML writing code is used to save documents in	*/
/*  EPUB format. See http://www.idpf.org/specs.htm.			*/
/*									*/
/************************************************************************/

#   ifndef		DOC_HTML_WRITE_H
#   define		DOC_HTML_WRITE_H

#   include		<sioGeneral.h>
#   include		<docBuf.h>
#   include		<layoutContext.h>

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int docHtmlSaveDocument(	SimpleOutputStream *	sos,
				BufferDocument *	bd,
				const MemoryBuffer *	filename,
				const LayoutContext *	lc );

extern int docEmlSaveDocument(	SimpleOutputStream *	sos,
				BufferDocument *	bd,
				const char *		mimeBoundary,
				const LayoutContext *	lc );

extern int docEpubSaveDocument(	SimpleOutputStream *	sos,
				BufferDocument *	bd,
				const LayoutContext *	lc );

#   endif	/*	DOC_HTML_WRITE_H	*/

