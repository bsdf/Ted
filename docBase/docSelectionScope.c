/************************************************************************/
/*									*/
/*  Manage the scope of selections: In which sub tree if the document	*/
/*  are they rooted?							*/
/*									*/
/************************************************************************/

#   include	"docBaseConfig.h"

#   include	<appDebugon.h>

#   include	"docSelectionScope.h"
#   include	"docTreeType.h"

/************************************************************************/
/*									*/
/*  Initialise a SelectionScope.					*/
/*									*/
/************************************************************************/

void docInitSelectionScope(	SelectionScope *	ss )
    {
    ss->ssTreeType= DOCinUNKNOWN;
    ss->ssSectNr= -1;
    ss->ssOwnerSectNr= -1;
    ss->ssOwnerNumber= -1;
    }

/************************************************************************/
/*									*/
/*  Decide whether the selection moved to a different scope.		*/
/*									*/
/*  NOTE: Only use the scope fields of the old selection as deletions	*/
/*	or insertions of BufferItems may have invalidated the pointers.	*/
/*									*/
/************************************************************************/

int docSelectionSameScope(	const SelectionScope *	ssFrom,
				const SelectionScope *	ssTo )
    {
    if  ( ssTo->ssTreeType != ssFrom->ssTreeType )
	{ return 0;	}

    switch( ssTo->ssTreeType )
	{
	case DOCinBODY:

	case DOCinFTNSEP:
	case DOCinFTNSEPC:
	case DOCinFTNCN:
	case DOCinAFTNSEP:
	case DOCinAFTNSEPC:
	case DOCinAFTNCN:

	    return 1;

	case DOCinFIRST_HEADER:
	case DOCinLEFT_HEADER:
	case DOCinRIGHT_HEADER:

	case DOCinFIRST_FOOTER:
	case DOCinLEFT_FOOTER:
	case DOCinRIGHT_FOOTER:

	    if  ( ssTo->ssOwnerSectNr != ssFrom->ssOwnerSectNr )
		{ return 0;	}

	    return 1;

	case DOCinFOOTNOTE:
	case DOCinENDNOTE:

	    /*  Implied by ssOwnerNumber test:
	    if  ( ssTo->ssOwnerSectNr != ssFrom->ssOwnerSectNr )
		{ return 0;	}
	    */
	    if  ( ssTo->ssOwnerNumber != ssFrom->ssOwnerNumber )
		{ return 0;	}

	    return 1;

	case DOCinSHPTXT:
	    if  ( ssTo->ssOwnerNumber != ssFrom->ssOwnerNumber )
		{ return 0;	}
	    return 1;

	default:
	    LLDEB(ssTo->ssTreeType,ssFrom->ssTreeType);
	    return -1;
	}
    }

