/************************************************************************/
/*									*/
/*  Positions in a document. To prevent crashes, all references are	*/
/*  to the paragraph number, rather than the paragraph object.		*/
/*									*/
/************************************************************************/

#   include	"docBaseConfig.h"

#   include	<appDebugon.h>

#   include	"docEditRange.h"

/************************************************************************/
/*									*/
/*  Initialize an EditRange.						*/
/*									*/
/************************************************************************/

void docInitEditRange(	EditRange *	er )
    {
    docInitEditPosition( &(er->erHead) );
    docInitEditPosition( &(er->erTail) );
    }

/************************************************************************/
/*									*/
/*  Shift references in various administrations.			*/
/*									*/
/************************************************************************/

void docShiftEditRange(	EditRange  *		er,
			int			paraFrom,
			int			stroffFrom,
			int			paraShift,
			int			stroffShift )
    {
    docAdjustEditPositionB( &(er->erHead),
				paraFrom, stroffFrom, paraShift, stroffShift );
    docAdjustEditPositionE( &(er->erTail),
				paraFrom, stroffFrom, paraShift, stroffShift );

    return;
    }

/************************************************************************/
/*									*/
/*  Union two ranges.							*/
/*									*/
/*  The code is written in such a way that the arguments can point to	*/
/*  the same address.							*/
/*									*/
/************************************************************************/

void docUnionEditRanges(	EditRange *		erTo,
				const EditRange *	er1,
				const EditRange *	er2 )
    {
    EditRange	er= *er1;

    if  ( docCompareEditPositions( &(er.erHead), &(er2->erHead) ) > 0 )
	{ er.erHead= er2->erHead;	}
    if  ( docCompareEditPositions( &(er.erTail), &(er2->erTail) ) < 0 )
	{ er.erTail= er2->erTail;	}

    *erTo= er;
    }
