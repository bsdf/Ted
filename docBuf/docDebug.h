/************************************************************************/
/*									*/
/*  Text buffer: Diagnostic routines.					*/
/*									*/
/************************************************************************/

#   ifndef	DOC_DEBUG_H
#   define	DOC_DEBUG_H

#   include	"docBuf.h"

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docCheckFieldOffsets(	const BufferDocument *	bd,
					const struct BufferItem *	bi );

extern int docCheckNode(	const struct BufferItem *	bi,
				int			checkGeometry );
extern int docCheckRootNode(	const struct BufferItem *	bi,
				int			checkGeometry );

extern void docListFieldParticule(	int				indent,
					const char *			label,
					int				n,
					const struct BufferItem *		bi,
					const struct TextParticule *	tp,
					const DocumentField *		df );

extern void docListFieldTree(		const BufferDocument *	bd,
					const DocumentTree *	dt );

extern void docScanNotesOfDocument(	const BufferDocument *	bd );

extern void docListFields(		const ChildFields *	cf,
					const BufferDocument *	bd );

extern void docListFontList(		const DocumentFontList * dfl );

extern void docLogRectangle(	const char *			label,
				const DocumentRectangle *	dr );

extern void docLogRectangles(	const char *			label1,
				const DocumentRectangle *	dr1,
				const char *			label2,
				const DocumentRectangle *	dr2 );

extern void docListNode(	int			indent,
				const struct BufferItem *	bi,
				int			checkGeometry );

extern void docListRootNode(	int			indent,
				const struct BufferItem *	bi,
				int			checkGeometry );

extern void docListParticule(	int				indent,
				const char *			label,
				int				n,
				const struct BufferItem *		bi,
				const struct TextParticule *	tp );

extern void docListTextLine(	int			indent,
				const char *		label,
				int			n,
				const struct BufferItem *	bi,
				const struct TextLine *	tl );

extern void docListObjects(	BufferDocument * 	bd );

extern void docListNotes(	const BufferDocument *	bd );

extern void docListShapes(		BufferDocument *		bd );
extern void docListDocumentObjects(	BufferDocument *		bd );

extern void docListField(		int			indent,
					const DocumentField *	df );

#   endif
