/************************************************************************/
/*									*/
/*  Read/Write RTF files.						*/
/*									*/
/************************************************************************/

#   ifndef	DOC_FONTS_DOCUMENTS_H
#   define	DOC_FONTS_DOCUMENTS_H

#   include	<docBuf.h>

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int docFontsDocuments(	const PostScriptFontList *	psfl,
				const MemoryBuffer *		templDir,
				const MemoryBuffer *		outputDir );

#    endif	/*  DOC_FONTS_DOCUMENTS_H	*/
