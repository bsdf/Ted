/************************************************************************/
/*									*/
/*  Implementation of copy/paste related functionality.			*/
/*									*/
/************************************************************************/

#   ifndef	TED_COPY_PASTE_IMPL_H
#   define	TED_COPY_PASTE_IMPL_H

#   include	<docSelectionDescription.h>
#   include	<docBuf.h>

struct EditDocument;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void tedSaveSelectionTxtToFile(
				BufferDocument *		bd,
				const char *			filename );

extern void tedSaveSelectionToFile(
				BufferDocument *		bd,
				const DocumentSelection *	ds,
				int				rtfFlags,
				const char *			filename );

extern int tedDocSaveSelectionRtf(	MemoryBuffer *		mb,
					DocumentSelection *	ds,
					SelectionDescription *	sd,
					struct EditDocument *	ed );

extern int tedDocCopySelection(		struct EditDocument *	ed );

extern int tedApplyPastedRuler(		struct EditDocument *	ed,
					BufferDocument *	bdFrom,
					int			traced );

extern int tedApplyPastedFont(		struct EditDocument *	ed,
					BufferDocument *	bdFrom,
					int			traced );

extern void tedCopyFont( 		struct EditDocument *	ed );

extern void tedCopyRuler( 		struct EditDocument *	ed );

#   endif	/*  TED_COPY_PASTE_IMPL_H	*/
