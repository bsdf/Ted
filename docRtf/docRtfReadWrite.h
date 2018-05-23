/************************************************************************/
/*									*/
/*  Read/Write RTF files.						*/
/*									*/
/************************************************************************/

#   ifndef	DOC_RTF_RW_H
#   define	DOC_RTF_RW_H

#   include	<stdio.h>
#   include	<time.h>

#   include	<docBuf.h>
#   include	<sioGeneral.h>

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int docWriteRtfMail(	SimpleOutputStream *		sos,
				const char *			mimeBoundary,
				BufferDocument *		bd );

extern int docRtfSaveDocument(	SimpleOutputStream *		sos,
				BufferDocument *		bd,
				const DocumentSelection *	bs,
				int				flags );

extern BufferDocument *	docRtfReadFile(
				SimpleInputStream *		sis,
				unsigned int			flags );

#    endif	/*  DOC_RTF_RW_H	*/
