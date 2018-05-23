/************************************************************************/
/*									*/
/*  Bookkeeping for BOOKMARK fields.					*/
/*									*/
/************************************************************************/

#   ifndef DOC_BOOKMARK_FIELD_H
#   define DOC_BOOKMARK_FIELD_H

#   define	DOCmaxBOOKMARK	40

#   include	<utilMemoryBuffer.h>

struct DocumentField;
struct MemoryBuffer;
struct FieldInstructions;

/************************************************************************/
/*									*/
/*  Administrative routines.						*/
/*									*/
/************************************************************************/

extern int docFieldGetBookmark(	const MemoryBuffer **		markName,
				const struct DocumentField *	df );

extern int docIsTocBookmark(	long *				pId,
				const struct DocumentField *	df );

extern int docFieldMatchesBookmark(	const struct DocumentField *	df,
					const MemoryBuffer *	markName );

extern int docSetBookmarkField(		struct FieldInstructions *	fi,
					const struct MemoryBuffer *	mb );

extern int docAdaptBookmarkName(	struct MemoryBuffer *	markName );

extern int docBookmarkFromText(		struct MemoryBuffer *	markName,
					const char *		text,
					int			len );

extern int docBookmarkSuffixIndex(
				const struct MemoryBuffer *	markName,
				int				wanted );

#   endif /*  DOC_BOOKMARK_FIELD_H  */
