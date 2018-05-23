/************************************************************************/
/*									*/
/*  Manage note properties.						*/
/*									*/
/************************************************************************/

#   include	"docBaseConfig.h"

#   include	<appDebugon.h>

#   include	<utilPropMask.h>

#   include	"docTreeType.h"
#   include	"docNoteProperties.h"

void docInitNoteProperties(	NoteProperties *	np )
    {
    utilInitMemoryBuffer( &(np->npFixedText) );
    np->npTreeType= DOCinFOOTNOTE;
    np->npAutoNumber= 1;
    }

void docCleanNoteProperties(	NoteProperties *	np )
    {
    utilCleanMemoryBuffer( &(np->npFixedText) );
    }

int docCopyNoteProperties(	NoteProperties *	to,
				const NoteProperties *	from )
    {
    if  ( utilCopyMemoryBuffer( &(to->npFixedText), &(from->npFixedText) ) )
	{ LDEB(1); return -1;	}

    to->npTreeType= from->npTreeType;
    to->npAutoNumber= from->npAutoNumber;

    return 0;
    }

int docNotePropertyDifference(	PropertyMask *			pDifMask,
				const NoteProperties *		np1,
				const PropertyMask *		cmpMask,
				const NoteProperties *		np2 )
    {
    PropertyMask	difMask;

    utilPropMaskClear( &difMask );

    if  ( PROPmaskISSET( cmpMask, NOTEpropFIXED_TEXT ) )
	{
	if  ( ! utilEqualMemoryBuffer( &(np1->npFixedText),
						    &(np2->npFixedText) ) )
	    {
	    PROPmaskADD( &difMask, NOTEpropFIXED_TEXT );
	    }
	}

    if  ( PROPmaskISSET( cmpMask, NOTEpropTREE_TYPE ) )
	{
	if  ( np1->npTreeType != np2->npTreeType )
	    {
	    PROPmaskADD( &difMask, NOTEpropTREE_TYPE );
	    }
	}

    if  ( PROPmaskISSET( cmpMask, NOTEpropAUTO_NUMBER ) )
	{
	if  ( np1->npAutoNumber != np2->npAutoNumber )
	    {
	    PROPmaskADD( &difMask, NOTEpropAUTO_NUMBER );
	    }
	}

    *pDifMask= difMask;
    return 0;
    }

int docUpdNoteProperties(	PropertyMask *			npDoneMask,
				NoteProperties *		npTo,
				const PropertyMask *		npSetMask,
				const NoteProperties *		npSet )
    {
    PropertyMask	doneMask;

    utilPropMaskClear( &doneMask );

    if  ( PROPmaskISSET( npSetMask, NOTEpropFIXED_TEXT ) )
	{
	if  ( ! utilEqualMemoryBuffer( &(npTo->npFixedText),
						    &(npSet->npFixedText) ) )
	    {
	    if  ( utilCopyMemoryBuffer( &(npTo->npFixedText),
						    &(npSet->npFixedText) ) )
		{ LDEB(1); return -1;	}

	    PROPmaskADD( &doneMask, NOTEpropFIXED_TEXT );
	    }
	}

    if  ( PROPmaskISSET( npSetMask, NOTEpropTREE_TYPE ) )
	{
	if  ( npTo->npTreeType != npSet->npTreeType )
	    {
	    npTo->npTreeType= npSet->npTreeType;
	    PROPmaskADD( &doneMask, NOTEpropTREE_TYPE );
	    }
	}

    if  ( PROPmaskISSET( npSetMask, NOTEpropAUTO_NUMBER ) )
	{
	if  ( npTo->npAutoNumber != npSet->npAutoNumber )
	    {
	    npTo->npAutoNumber= npSet->npAutoNumber;
	    PROPmaskADD( &doneMask, NOTEpropAUTO_NUMBER );
	    }
	}

    if  ( npDoneMask )
	{ utilPropMaskOr( npDoneMask, npDoneMask, &doneMask); }

    return 0;
    }

