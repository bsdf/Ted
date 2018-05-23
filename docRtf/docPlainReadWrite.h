/************************************************************************/
/*									*/
/*  Read/Write plain text documents and selections (For Copy/Paste)	*/
/*									*/
/************************************************************************/

#   ifndef		DOC_PLAIN_RW_H
#   define		DOC_PLAIN_RW_H

#   include	<docBuf.h>
#   include	<sioGeneral.h>

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern BufferDocument *	docPlainReadFile( SimpleInputStream *	sis,
				    int *			pMxL,
				    const DocumentGeometry *	dg );

extern int docPlainSaveDocument(SimpleOutputStream *		sos,
				BufferDocument *		bd,
				const DocumentSelection *	ds,
				int				fold );

#   endif	/*	DOC_PLAIN_RW_H	*/
