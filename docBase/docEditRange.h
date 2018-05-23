/************************************************************************/
/*									*/
/*  A range in a document.						*/
/*									*/
/************************************************************************/

#   ifndef		DOC_EDIT_RANGE_H
#   define		DOC_EDIT_RANGE_H

#   include		"docEditPosition.h"

typedef struct EditRange
    {
    EditPosition	erHead;
    EditPosition	erTail;
    } EditRange;

#   define	docEditRangeIsIBar(er) \
		    ( (er)->erHead.epParaNr == (er)->erTail.epParaNr && \
		      (er)->erHead.epStroff == (er)->erTail.epStroff )

/************************************************************************/
/*									*/
/*  Routines.								*/
/*									*/
/************************************************************************/

extern void docInitEditRange(		EditRange *	er );

extern void docShiftEditRange(	EditRange  *		er,
				int			paraFrom,
				int			stroffFrom,
				int			paraShift,
				int			stroffShift );

extern void docUnionEditRanges(	EditRange *		erTo,
				const EditRange *	er1,
				const EditRange *	er2 );

#   endif	/*	DOC_EDIT_RANGE_H	*/
