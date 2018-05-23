/************************************************************************/
/*									*/
/*  Bookkeeping for MERGE fields.					*/
/*									*/
/************************************************************************/

#   ifndef DOC_MERGE_FIELD_H
#   define DOC_MERGE_FIELD_H

#   include	<utilMemoryBuffer.h>

struct DocumentField;

typedef struct MergeField
    {
    MemoryBuffer	mfFieldName;
    MemoryBuffer	mfTextBefore;		/* \b	*/
    MemoryBuffer	mfTextAfter;		/* \f	*/
    unsigned char	mfIsMapped;		/* \m	*/
    unsigned char	mfEnableVertival;	/* \v	*/
    unsigned char	mfCaseShift;		/* \*	*/
    } MergeField;

/************************************************************************/
/*									*/
/*  Administrative routines.						*/
/*									*/
/************************************************************************/

extern void docInitMergeField(		MergeField *	mf );
extern void docCleanMergeField(		MergeField *	mf );

extern int docGetMergeField(	MergeField *			mf,
				const struct DocumentField *	df );

#   endif /*  DOC_MERGE_FIELD_H  */
