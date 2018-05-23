/************************************************************************/
/*									*/
/*  Bookkeeping for REF fields.						*/
/*									*/
/************************************************************************/

#   ifndef DOC_REF_FIELD_H
#   define DOC_REF_FIELD_H

#   include	<utilMemoryBuffer.h>

struct DocumentField;

typedef struct RefField
    {
    MemoryBuffer	rfBookmark;
    MemoryBuffer	rfSeparators;		/* \d	*/
    unsigned char	rfToNote;		/* \f	*/
    unsigned char	rfAsHyperlink;		/* \h	*/
    unsigned char	rfAsParagraphNumber;	/* \n	*/
    unsigned char	rfWithPosition;		/* \p	*/
    } RefField;

/************************************************************************/
/*									*/
/*  Administrative routines.						*/
/*									*/
/************************************************************************/

extern void docInitRefField(		RefField *		rf );
extern void docCleanRefField(		RefField *		rf );

extern int docGetRefField(		RefField *			rf,
					const struct DocumentField *	df );

extern int docSetRefField(		struct DocumentField *		df,
					const RefField *		rf );

#   endif /*  DOC_REF_FIELD_H  */
