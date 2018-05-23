/************************************************************************/
/*									*/
/*  Bookkeeping for SEQ fields.						*/
/*									*/
/************************************************************************/

#   ifndef DOC_SEQ_FIELD_H
#   define DOC_SEQ_FIELD_H

#   include <utilMemoryBuffer.h>

struct DocumentField;

typedef struct SeqField
    {
    unsigned char	sfIncrement;	/*  \\c, \\n	*/
    unsigned char	sfHidden;	/*  \\h		*/
    unsigned char	sfNumberFormat;	/*  \\*		*/

    int			sfResetTo;	/*  \\r n	*/
    int			sfStyle;	/*  \\s n (Do not understand) */

    MemoryBuffer	sfIdentifier;
    MemoryBuffer	sfBookmark;
    } SeqField;

/************************************************************************/
/*									*/
/*  Administrative routines.						*/
/*									*/
/************************************************************************/

extern void docInitSeqField(		SeqField *			sf );
extern void docCleanSeqField(		SeqField *			sf );

extern int docGetSeqField(		SeqField *			sf,
					const struct DocumentField *	df );

#   endif /*  DOC_SEQ_FIELD_H  */
