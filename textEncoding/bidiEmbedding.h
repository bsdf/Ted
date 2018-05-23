/************************************************************************/
/*									*/
/*  Embedding of paragraphs and directional text runs.			*/
/*									*/
/*  Embeddings relationg to the Unicode Bidirectional algorithm.	*/
/*  See: http://unicode.org/reports/tr9/				*/
/*									*/
/************************************************************************/

#   ifndef		BIDI_EMBEDDING_H
#   define		BIDI_EMBEDDING_H

typedef int (*BidiGotLevel)(		void *		through,
					int		embedding,
					int		level,
					int		from,
					int		upto );

/************************************************************************/
/*									*/
/*  Embeddings from X2,X3: RLE,LRE					*/
/*  Overrides from X4,X5: RLO,LRO					*/
/*  And a value to indicate that the embedding was resolved.		*/
/*									*/
/*  And finally, a value that indicates that the position was removed	*/
/*  as a consequence of rule X9.					*/
/*									*/
/************************************************************************/

#   define	BIDIembedLRE		'['
#   define	BIDIembedRLE		']'
#   define	BIDIembedLRO		'{'
#   define	BIDIembedRLO		'}'
#   define	BIDIembedPDF		'^'

#   define	BIDIembedEMBEDDING	'e'
#   define	BIDIembedIMPLICIT	'i'

#   define	BIDIembedREMOVED	'x'
#   define	BIDIembedREJECTED	'!'

/************************************************************************/
/*									*/
/*  Routine declarations						*/
/*									*/
/************************************************************************/

extern const char * bidiEmbeddingStr(	int	embedding );

#   endif	/*	BIDI_EMBEDDING_H	*/
