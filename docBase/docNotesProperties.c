/************************************************************************/
/*									*/
/*  Buffer administration routines.					*/
/*									*/
/************************************************************************/

#   include	"docBaseConfig.h"

#   include	<stdio.h>

#   include	<utilPropMask.h>
#   include	<appDebugon.h>

#   include	"docNotesProperties.h"

/************************************************************************/

int docSetNotesProperty(		NotesProperties *	np,
					int			prop,
					int			val )
    {
    switch( prop )
	{
	case NOTESpropSTARTNR:
	    np->npStartNumber= val;
	    return 0;
	case NOTESpropJUSTIFICATION:
	    np->npJustification= val;
	    return 0;
	case NOTESpropPLACEMENT:
	    np->npPlacement= val;
	    return 0;
	case NOTESpropRESTART:
	    np->npRestart= val;
	    return 0;
	case NOTESpropSTYLE:
	    np->npNumberStyle= val;
	    return 0;
	default:
	    LDEB(prop); return -1;
	}
    }

int docGetNotesProperty(		const NotesProperties *	np,
					int			prop )
    {
    switch( prop )
	{
	case NOTESpropSTARTNR:
	    return np->npStartNumber;
	case NOTESpropJUSTIFICATION:
	    return np->npJustification;
	case NOTESpropPLACEMENT:
	    return np->npPlacement;
	case NOTESpropRESTART:
	    return np->npRestart;
	case NOTESpropSTYLE:
	    return np->npNumberStyle;
	default:
	    LDEB(prop); return -1;
	}
    }

/************************************************************************/
/*									*/
/*  Change note properties and tell what has been changed.		*/
/*									*/
/************************************************************************/

static int docUpdNotesProperties(
		PropertyMask *			pDoneMask,
		NotesProperties *		np,
		const PropertyMask *		setMask,
		const NotesProperties *		npSet,
		const int			propMap[NOTESprop_COUNT] )
    {
    int		prop;
    int		rval= 0;

    for ( prop= 0; prop < NOTESprop_COUNT; prop++ )
	{
	const int	pprop= propMap[prop];
	int		oval;
	int		nval;

	if  ( pprop < 0				||
	      ! PROPmaskISSET( setMask, pprop )	)
	    { continue;	}

	oval= docGetNotesProperty( np, prop );
	nval= docGetNotesProperty( npSet, prop );
	if  ( oval != nval )
	    {
	    if  ( docSetNotesProperty( np, prop, nval ) )
		{ LLLDEB(prop,pprop,nval); rval= -1;	}

	    PROPmaskADD( pDoneMask, pprop );
	    }
	}

    return rval;
    }

int docUpdFootEndNotesProperties(
		PropertyMask *			pDoneMask,
		FootEndNotesProperties *	fep,
		const PropertyMask *		setMask,
		const FootEndNotesProperties *	fepSet,
		const int			fepPropMap[FEPprop_COUNT] )
    {
    docUpdNotesProperties( pDoneMask, &(fep->fepFootnotesProps),
				setMask, &(fepSet->fepFootnotesProps),
				fepPropMap+ FEPpropFOOT_STARTNR );

    docUpdNotesProperties( pDoneMask, &(fep->fepEndnotesProps),
				setMask, &(fepSet->fepEndnotesProps),
				fepPropMap+ FEPpropEND_STARTNR );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Compare note properties.						*/
/*									*/
/************************************************************************/

static int docNotesPropertyDifference(
		PropertyMask *			pDifMask,
		const NotesProperties *		np1,
		const PropertyMask *		cmpMask,
		const NotesProperties *		np2,
		const int			propMap[NOTESprop_COUNT] )
    {
    int		prop;
    int		rval= 0;

    for ( prop= 0; prop < NOTESprop_COUNT; prop++ )
	{
	const int	pprop= propMap[prop];
	int		oval;
	int		nval;

	if  ( pprop < 0				||
	      ! PROPmaskISSET( cmpMask, pprop )	)
	    { continue;	}

	oval= docGetNotesProperty( np1, prop );
	nval= docGetNotesProperty( np2, prop );
	if  ( oval != nval )
	    { PROPmaskADD( pDifMask, pprop );	}
	}

    return rval;
    }

int docFootEndNotesPropertyDifference(
		PropertyMask *			pDifMask,
		const FootEndNotesProperties *	fep1,
		const PropertyMask *		cmpMask,
		const FootEndNotesProperties *	fep2,
		const int			fepPropMap[FEPprop_COUNT] )
    {
    docNotesPropertyDifference( pDifMask, &(fep1->fepFootnotesProps),
				cmpMask, &(fep2->fepFootnotesProps),
				fepPropMap+ FEPpropFOOT_STARTNR );
    docNotesPropertyDifference( pDifMask, &(fep1->fepEndnotesProps),
				cmpMask, &(fep2->fepEndnotesProps),
				fepPropMap+ FEPpropEND_STARTNR );
    return 0;
    }

void docInitNotesProperties(	NotesProperties *	np )
    {
    np->npStartNumber= 1;
    np->npJustification= FTNjustifyPAGE_BOTTOM;
    np->npPlacement= FTNplaceSECT_END;
    np->npRestart= FTN_RST_CONTINUOUS;
    np->npNumberStyle= FTNstyleNAR;

    return;
    }

void docDefaultFootNotesProperties(	NotesProperties *	np )
    {
    np->npStartNumber= 1;
    np->npJustification= FTNjustifyPAGE_BOTTOM;
    np->npPlacement= FTNplacePAGE_END;
    np->npRestart= FTN_RST_CONTINUOUS;
    np->npNumberStyle= FTNstyleNAR;

    return;
    }

void docDefaultEndNotesProperties(	NotesProperties *	np )
    {
    np->npStartNumber= 1;
    np->npJustification= FTNjustifyBELOW_TEXT;
    np->npPlacement= FTNplaceSECT_END;
    np->npRestart= FTN_RST_CONTINUOUS;
    np->npNumberStyle= FTNstyleNRLC;

    return;
    }

void docInitFootEndNotesProperties(	FootEndNotesProperties *	fep )
    {
    docInitNotesProperties( &(fep->fepFootnotesProps) );
    docInitNotesProperties( &(fep->fepEndnotesProps) );
    }

void docDefaultFootEndNotesProperties(	FootEndNotesProperties *	fep )
    {
    docDefaultFootNotesProperties( &(fep->fepFootnotesProps) );
    docDefaultEndNotesProperties( &(fep->fepEndnotesProps) );
    }

const char * docNotesJustificationStr( int pos )
    {
    static char scratch[20];

    switch( pos )
	{
	case FTNjustifyBELOW_TEXT:	return "BELOW_TEXT";
	case FTNjustifyPAGE_BOTTOM:	return "PAGE_BOTTOM";

	default:
	    sprintf( scratch, "%d", pos );
	    return scratch;
	}
    }

const char * docNotesPlacementStr( int pos )
    {
    static char scratch[20];

    switch( pos )
	{
	case FTNplaceSECT_END:	return "SECT_END";
	case FTNplaceDOC_END:	return "DOC_END";

	default:
	    sprintf( scratch, "%d", pos );
	    return scratch;
	}
    }
