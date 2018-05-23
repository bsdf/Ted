/************************************************************************/
/*									*/
/*  Bookkeeping for DATE fields.					*/
/*									*/
/************************************************************************/

#   ifndef DOC_DATE_FIELD_H
#   define DOC_DATE_FIELD_H

#   include	<utilMemoryBuffer.h>

struct DocumentField;

typedef enum FieldCalendar
    {
    FIELDcalendarDEFAULT= 0,	/*  Use locale, usually gregorian	*/
    FIELDcalendarLUNAR,		/* \h					*/
    FIELDcalendarSAKA,		/* \s					*/

    FIELDcalendar_COUNT
    } FieldCalendar;

typedef struct DateField
    {
    MemoryBuffer	dtfPicture;		/* \@			*/
    unsigned char	dtfCalendar;		/* \h, \s not for TIME	*/
    unsigned char	dtfUseLast;		/* \l DATE only.	*/
    } DateField;

/************************************************************************/
/*									*/
/*  Administrative routines.						*/
/*									*/
/************************************************************************/

extern void docInitDateField(		DateField *	dtf );
extern void docCleanDateField(		DateField *	dtf );

extern int docGetDateField(	DateField *			dtf,
				const struct DocumentField *	df );

#   endif /*  DOC_DATE_FIELD_H  */
