/************************************************************************/
/*									*/
/*  Find utilities.							*/
/*									*/
/************************************************************************/

#   ifndef	TED_FIND_H
#   define	TED_FIND_H

#   include	<ind.h>

struct DocumentSelection;
struct BufferItem;
struct BufferDocument;
struct DocumentPosition;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int tedFindToolSetPattern(	void *		voidea,
					const char *	pattern,
					int		useRegex );

extern int tedSpellFindNext(	void *			voidea,
				MemoryBuffer *		mbGuess,
				const SpellChecker *	spc,
				SpellDictionary *	spd );

extern int docSpellParaFindNext(
				struct DocumentSelection *	ds,
				struct BufferItem *		paraNode,
				struct BufferDocument *		bd,
				const struct DocumentPosition *	dpFrom,
				void *				through );

#   endif	/*  TED_FIND_H */
