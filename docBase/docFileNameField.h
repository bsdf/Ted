/************************************************************************/
/*									*/
/*  Bookkeeping for FILENAME fields.					*/
/*									*/
/************************************************************************/

#   ifndef DOC_FILENAME_FIELD_H
#   define DOC_FILENAME_FIELD_H

struct DocumentField;

typedef struct FileNameField
    {
    unsigned char	fnfIncludePath;		/* \p	*/
    } FileNameField;

/************************************************************************/
/*									*/
/*  Administrative routines.						*/
/*									*/
/************************************************************************/

extern void docInitFileNameField(	FileNameField *	fnf );
extern void docCleanFileNameField(	FileNameField *	fnf );

extern int docGetFileNameField(		FileNameField *			fnf,
					const struct DocumentField *	df );

#   endif /*  DOC_FILENAME_FIELD_H  */
