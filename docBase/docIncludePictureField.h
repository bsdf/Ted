/************************************************************************/
/*									*/
/*  Bookkeeping for INCLUDEPICTURE fields.				*/
/*									*/
/************************************************************************/

#   ifndef DOC_INCLUDEPICTURE_FIELD_H
#   define DOC_INCLUDEPICTURE_FIELD_H

#   include	<utilMemoryBuffer.h>

struct DocumentField;

typedef struct IncludePictureField
    {
    MemoryBuffer	ipfFilename;
    MemoryBuffer	ipfConverter;		/* \c	*/
    unsigned char	ipfDoNotSaveResult;	/* \d	*/
    } IncludePictureField;

/************************************************************************/
/*									*/
/*  Administrative routines.						*/
/*									*/
/************************************************************************/

extern void docInitIncludePictureField(		IncludePictureField *	ipf );
extern void docCleanIncludePictureField(	IncludePictureField *	ipf );

extern int docGetIncludePictureField(	IncludePictureField *		ipf,
					const struct DocumentField *	df );

#   endif /*  DOC_INCLUDEPICTURE_FIELD_H  */
