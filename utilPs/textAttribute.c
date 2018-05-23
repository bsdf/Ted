/************************************************************************/
/*									*/
/*  Font administration for a document.					*/
/*									*/
/************************************************************************/

#   include	"utilPsConfig.h"

#   include	<appDebugon.h>

#   include	<utilPropMask.h>
#   include	"textAttribute.h"

/************************************************************************/
/*									*/
/*  Set text attributes to a default value.				*/
/*									*/
/************************************************************************/

void utilInitTextAttribute(	TextAttribute *	ta	)
    {
    ta->taFontNumber= -1;
    ta->taFontSizeHalfPoints= 0;
    ta->taTextColorNumber= 0;
    ta->taBorderNumber= 0;
    ta->taShadingNumber= 0;
    ta->taTextStyleNumber= 0;
    ta->taFontIsBold= 0;
    ta->taFontIsSlanted= 0;
    ta->taTextIsUnderlined= 0;
    ta->taSuperSub= TEXTvaREGULAR;
    ta->taSmallCaps= 0;
    ta->taHasStrikethrough= 0;
    ta->taCapitals= 0;
    ta->taBaselineShiftHalfPoints= 0;
    }

/************************************************************************/
/*									*/
/*  Set a text attribute property.					*/
/*									*/
/************************************************************************/

int utilSetTextProperty(		TextAttribute *		ta,
					int			prop,
					int			arg )
    {
    switch( prop )
	{
	case TApropFONT_NUMBER:
	    ta->taFontNumber= arg;
	    break;

	case TApropFONTSIZE:
	    ta->taFontSizeHalfPoints= arg;
	    break;

	case TApropFONTBOLD:
	    ta->taFontIsBold= arg != 0;
	    break;

	case TApropFONTSLANTED:
	    ta->taFontIsSlanted= arg != 0;
	    break;

	case TApropTEXTUNDERLINED:
	    ta->taTextIsUnderlined= arg != 0;
	    break;

	case TApropSUPERSUB:
	    ta->taSuperSub= arg;
	    break;

	case TApropSMALLCAPS:
	    ta->taSmallCaps= arg != 0;
	    break;

	case TApropCAPITALS:
	    ta->taCapitals= arg != 0;
	    break;

	case TApropSTRIKETHROUGH:
	    ta->taHasStrikethrough= arg != 0;
	    break;

	case TApropTEXT_COLOR:
	    ta->taTextColorNumber= arg;
	    break;

	case TApropTEXT_STYLE:
	    ta->taTextStyleNumber= arg;
	    break;

	case TApropBORDER:
	    ta->taBorderNumber= arg;
	    break;

	case TApropSHADING:
	    ta->taShadingNumber= arg;
	    break;

	case TApropBASELINE_SHIFT:
	    ta->taBaselineShiftHalfPoints= arg;
	    break;

	case TAprop_BASELINE_SHIFT_DOWN:
	    ta->taBaselineShiftHalfPoints= -arg;
	    break;

	default:
	    LDEB(prop); return -1;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Return a text attribute property.					*/
/*									*/
/************************************************************************/

int utilGetTextProperty(		const TextAttribute *	ta,
					int			prop )
    {
    switch( prop )
	{
	case TApropFONT_NUMBER:
	    return ta->taFontNumber;

	case TApropFONTSIZE:
	    return ta->taFontSizeHalfPoints;

	case TApropFONTBOLD:
	    return ta->taFontIsBold;

	case TApropFONTSLANTED:
	    return ta->taFontIsSlanted;

	case TApropTEXTUNDERLINED:
	    return ta->taTextIsUnderlined;

	case TApropSUPERSUB:
	    return ta->taSuperSub;

	case TApropSMALLCAPS:
	    return ta->taSmallCaps;

	case TApropCAPITALS:
	    return ta->taCapitals;

	case TApropSTRIKETHROUGH:
	    return ta->taHasStrikethrough;

	case TApropTEXT_COLOR:
	    return ta->taTextColorNumber;

	case TApropTEXT_STYLE:
	    return ta->taTextStyleNumber;

	case TApropBORDER:
	    return ta->taBorderNumber;

	case TApropSHADING:
	    return ta->taShadingNumber;

	case TApropBASELINE_SHIFT:
	    return ta->taBaselineShiftHalfPoints;
	    break;

	case TAprop_BASELINE_SHIFT_DOWN:
	    return -ta->taBaselineShiftHalfPoints;
	    break;

	default:
	    LDEB(prop); return -1;
	}
    }

/************************************************************************/
/*									*/
/*  Get text attribute difference mask.					*/
/*									*/
/************************************************************************/

void utilAttributeDifference(		PropertyMask *		pDifMask,
					const TextAttribute *	ta1,
					const PropertyMask *	cmpMask,
					const TextAttribute *	ta2 )
    {
    PropertyMask	difMask;
    int			prop;

    utilPropMaskClear( &difMask );

    for ( prop= 0; prop < TAprop_COUNT; prop++ )
	{
	if  ( ! cmpMask || PROPmaskISSET( cmpMask, prop ) )
	    {
	    int	val1= utilGetTextProperty( ta1, prop );
	    int	val2= utilGetTextProperty( ta2, prop );

	    if  ( val1 != val2 )
		{ PROPmaskADD( &difMask, prop ); }
	    }
	}

    if  ( pDifMask )
	{ *pDifMask= difMask;	}

    return;
    }

/************************************************************************/
/*									*/
/*  Set text attribute and remember difference mask.			*/
/*									*/
/************************************************************************/

void utilUpdateTextAttribute(		PropertyMask *		pDoneMask,
					TextAttribute *		ta,
					const PropertyMask *	taSetMask,
					const TextAttribute *	taSet )
    {
    PropertyMask	doneMask;
    int			prop;

    utilPropMaskClear( &doneMask );

    for ( prop= 0; prop < TAprop_COUNT; prop++ )
	{
	if  ( ! taSetMask || PROPmaskISSET( taSetMask, prop ) )
	    {
	    int	to= utilGetTextProperty( ta, prop );
	    int	from= utilGetTextProperty( taSet, prop );

	    if  ( to != from )
		{
		utilSetTextProperty( ta, prop, from );
		PROPmaskADD( &doneMask, prop );
		}
	    }
	}

    if  ( pDoneMask )
	{ utilPropMaskOr( pDoneMask, pDoneMask, &doneMask );	}

    return;
    }
