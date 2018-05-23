/************************************************************************/
/*									*/
/*  Manage paragraph properties.					*/
/*									*/
/************************************************************************/

#   include	"docBaseConfig.h"

#   include	<appDebugon.h>

#   include	"docParaProperties.h"
#   include	"docPropVal.h"
#   include	"docListDepth.h"

/************************************************************************/
/*									*/
/*  Manage paragraph properties.					*/
/*									*/
/************************************************************************/

void docCleanParagraphProperties(	ParagraphProperties *	pp )
    {
    }

void docInitParagraphProperties(	ParagraphProperties *	pp )
    {
    pp->ppTabStopListNumber= 0;

    pp->ppFirstIndentTwips= 0;
    pp->ppLeftIndentTwips= 0;
    pp->ppRightIndentTwips= 0;

    pp->ppSpaceBeforeTwips= 0;
    pp->ppSpaceAfterTwips= 0;
    pp->ppLineSpacingTwips= 0;

    pp->ppStyle= 0;
    pp->ppListOverride= 0;

    pp->ppTopBorderNumber= 0;
    pp->ppLeftBorderNumber= 0;
    pp->ppRightBorderNumber= 0;
    pp->ppBottomBorderNumber= 0;
    pp->ppBetweenBorderNumber= 0;
    pp->ppBarNumber= 0;

    pp->ppShadingNumber= 0;
    pp->ppFrameNumber= 0;

    pp->ppOutlineLevel= PPoutlineBODYTEXT;
    pp->ppListLevel= 0;

    pp->ppAlignment= DOCthaLEFT;

    pp->ppBreakKind= DOCibkNONE;
    pp->ppTableNesting= 0;
    pp->ppLineSpacingIsMultiple= 0;

    pp->ppKeepOnPage= 0;
    pp->ppKeepWithNext= 0;
    pp->ppWidowControl= 0;
    pp->ppHyphenateParagraph= 0;
    pp->ppRToL= 0;

    return;
    }

/************************************************************************/

static const int DocParaIntProps[]=
{
    PPpropSTYLE,
    PPpropTABLE_NESTING,
    PPpropFIRST_INDENT,
    PPpropLEFT_INDENT,
    PPpropRIGHT_INDENT,
    PPpropALIGNMENT,

    PPpropBREAK_KIND,
    PPpropWIDCTLPAR,
    PPpropKEEPN,
    PPpropKEEP,
    PPpropSPACE_BEFORE,
    PPpropSPACE_AFTER,
    PPpropLINE_SPACING_DIST,
    PPpropLINE_SPACING_MULT,

    PPpropOUTLINELEVEL,
    PPpropLISTLEVEL,
    PPpropHYPHPAR,
    PPpropRTOL,
};

static const int DocParaIntPropCount= sizeof(DocParaIntProps)/sizeof(int);

static const int DocParaBorderProps[]=
{
    PPpropTOP_BORDER,
    PPpropBOTTOM_BORDER,
    PPpropLEFT_BORDER,
    PPpropRIGHT_BORDER,
    PPpropBETWEEN_BORDER,
    PPpropBAR_BORDER,
};

static const int DocParaBorderPropCount= sizeof(DocParaBorderProps)/sizeof(int);

/************************************************************************/
/*									*/
/*  Change paragraph properties and tell what has been changed.		*/
/*									*/
/************************************************************************/

int docCopyParagraphProperties(	ParagraphProperties *		ppTo,
				const ParagraphProperties *	ppFrom )
    {
    PropertyMask	ppSetMask;

    utilPropMaskFill( &ppSetMask, PPprop_FULL_COUNT );

    return docUpdParaProperties( (PropertyMask *)0, ppTo, &ppSetMask, ppFrom,
					    (const DocumentAttributeMap *)0 );
    }

int docUpdParaProperties(	PropertyMask *			pDoneMask,
				ParagraphProperties *		ppTo,
				const PropertyMask *		ppSetMask,
				const ParagraphProperties *	ppFrom,
				const DocumentAttributeMap *	dam )
    {
    PropertyMask		ppDoneMask;
    int				p;

    utilPropMaskClear( &ppDoneMask );

    for ( p= 0; p < DocParaIntPropCount; p++ )
	{
	const int	prop= DocParaIntProps[p];

	if  ( PROPmaskISSET( ppSetMask, prop ) )
	    {
	    int		from= docGetParaProperty( ppFrom, prop );
	    int		to= docGetParaProperty( ppTo, prop );

	    if  ( to != from )
		{
		if  ( docSetParaProperty( ppTo, prop, from ) )
		    { LLDEB(prop,from);	}
		else{ PROPmaskADD( &ppDoneMask, prop );	}
		}
	    }
	}

    if  ( PROPmaskISSET( ppSetMask, PPpropLISTOVERRIDE ) )
	{
	int		listStyle= ppFrom->ppListOverride;

	if  ( dam && dam->damListStyleMap )
	    { listStyle= dam->damListStyleMap[listStyle];	}

	if  ( ppTo->ppListOverride != listStyle )
	    {
	    ppTo->ppListOverride= listStyle;
	    PROPmaskADD( &ppDoneMask, PPpropLISTOVERRIDE );
	    }
	}

    if  ( PROPmaskISSET( ppSetMask, PPpropTAB_STOPS ) )
	{
	int	fromNumber= ppFrom->ppTabStopListNumber;

	if  ( fromNumber >= 0 && dam && dam->damRulerMap )
	    { fromNumber= dam->damRulerMap[fromNumber];	}

	if  ( ppTo->ppTabStopListNumber != fromNumber )
	    {
	    ppTo->ppTabStopListNumber= fromNumber;
	    PROPmaskADD( &ppDoneMask, PPpropTAB_STOPS );
	    }
	}

    for ( p= 0; p < DocParaBorderPropCount; p++ )
	{
	const int	prop= DocParaBorderProps[p];

	if  ( PROPmaskISSET( ppSetMask, prop ) )
	    {
	    int		from= docGetParaProperty( ppFrom, prop );
	    int		to= docGetParaProperty( ppTo, prop );

	    if  ( from >= 0 && dam && dam->damBorderMap )
		{ from= dam->damBorderMap[from];	}

	    if  ( to != from )
		{
		if  ( docSetParaProperty( ppTo, prop, from ) )
		    { LLDEB(prop,from);	}
		else{ PROPmaskADD( &ppDoneMask, prop );	}
		}
	    }
	}

    if  ( PROPmaskISSET( ppSetMask, PPpropSHADING ) )
	{
	int	fromNumber= ppFrom->ppShadingNumber;

	if  ( fromNumber >= 0 && dam && dam->damShadingMap )
	    { fromNumber= dam->damShadingMap[fromNumber];	}

	if  ( ppTo->ppShadingNumber != fromNumber )
	    {
	    ppTo->ppShadingNumber= fromNumber;
	    PROPmaskADD( &ppDoneMask, PPpropSHADING );
	    }
	}

    if  ( PROPmaskISSET( ppSetMask, PPpropFRAME ) )
	{
	int	fromNumber= ppFrom->ppFrameNumber;

	if  ( fromNumber >= 0 && dam && dam->damFrameMap )
	    { fromNumber= dam->damFrameMap[fromNumber];	}

	if  ( ppTo->ppFrameNumber != fromNumber )
	    {
	    ppTo->ppFrameNumber= fromNumber;
	    PROPmaskADD( &ppDoneMask, PPpropFRAME );
	    }
	}

    if  ( pDoneMask )
	{ utilPropMaskOr( pDoneMask, pDoneMask, &ppDoneMask );	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Compare paragraph properties.					*/
/*									*/
/************************************************************************/

void docParaPropertyDifference(	PropertyMask *			pDifMask,
				const ParagraphProperties *	ppTo,
				const PropertyMask *		ppCmpMask,
				const ParagraphProperties *	ppFrom )
    {
    PropertyMask		ppDifMask;
    int				p;

    const DocumentAttributeMap *	dam= (const DocumentAttributeMap *)0;

    utilPropMaskClear( &ppDifMask );

    for ( p= 0; p < DocParaIntPropCount; p++ )
	{
	const int	prop= DocParaIntProps[p];

	if  ( PROPmaskISSET( ppCmpMask, prop ) )
	    {
	    int		from= docGetParaProperty( ppFrom, prop );
	    int		to= docGetParaProperty( ppTo, prop );

	    if  ( to != from )
		{ PROPmaskADD( &ppDifMask, prop );	}
	    }
	}

    if  ( PROPmaskISSET( ppCmpMask, PPpropLISTOVERRIDE ) )
	{
	int		listStyle= ppFrom->ppListOverride;

	if  ( dam && dam->damListStyleMap )
	    { listStyle= dam->damListStyleMap[listStyle];	}

	if  ( ppTo->ppListOverride != listStyle )
	    { PROPmaskADD( &ppDifMask, PPpropLISTOVERRIDE );	}
	}

    if  ( PROPmaskISSET( ppCmpMask, PPpropTAB_STOPS ) )
	{
	int	fromNumber= ppFrom->ppTabStopListNumber;

	if  ( fromNumber >= 0 && dam && dam->damRulerMap )
	    { fromNumber= dam->damRulerMap[fromNumber];	}

	if  ( ppTo->ppTabStopListNumber != fromNumber )
	    { PROPmaskADD( &ppDifMask, PPpropTAB_STOPS );	}
	}

    for ( p= 0; p < DocParaBorderPropCount; p++ )
	{
	const int	prop= DocParaBorderProps[p];

	if  ( PROPmaskISSET( ppCmpMask, prop ) )
	    {
	    int		from= docGetParaProperty( ppFrom, prop );
	    int		to= docGetParaProperty( ppTo, prop );

	    if  ( from >= 0 && dam && dam->damBorderMap )
		{ from= dam->damBorderMap[from];	}

	    if  ( to != from )
		{ PROPmaskADD( &ppDifMask, prop );	}
	    }
	}

    if  ( PROPmaskISSET( ppCmpMask, PPpropSHADING ) )
	{
	int	fromNumber= ppFrom->ppShadingNumber;

	if  ( fromNumber >= 0 && dam && dam->damShadingMap )
	    { fromNumber= dam->damShadingMap[fromNumber];	}

	if  ( ppTo->ppShadingNumber != fromNumber )
	    { PROPmaskADD( &ppDifMask, PPpropSHADING );	}
	}

    if  ( PROPmaskISSET( ppCmpMask, PPpropFRAME ) )
	{
	int	fromNumber= ppFrom->ppFrameNumber;

	if  ( fromNumber >= 0 && dam && dam->damFrameMap )
	    { fromNumber= dam->damFrameMap[fromNumber];	}

	if  ( ppTo->ppFrameNumber != fromNumber )
	    { PROPmaskADD( &ppDifMask, PPpropFRAME );	}
	}

    *pDifMask= ppDifMask;
    return;
    }

int docSetParaProperty(		ParagraphProperties *	pp,
				int			prop,
				int			arg )
    {
    switch( prop )
	{
	case PPpropSTYLE:
	    pp->ppStyle= arg;
	    break;

	case PPpropLISTOVERRIDE:
	    pp->ppListOverride= arg;
	    break;

	case PPprop_IN_TABLE:  /* intbl */
	    if  ( arg == 0 )
		{ pp->ppTableNesting= 0;	}
	    else{
		if  ( pp->ppTableNesting == 0 )
		    { pp->ppTableNesting= 1;	}
		}
	    break;

	case PPpropTABLE_NESTING:  /* itap */
	    pp->ppTableNesting= arg;
	    break;

	case PPpropLEFT_INDENT:
	    pp->ppLeftIndentTwips= arg;
	    break;

	case PPpropFIRST_INDENT:
	    pp->ppFirstIndentTwips= arg;
	    break;

	case PPpropRIGHT_INDENT:
	    pp->ppRightIndentTwips= arg;
	    break;

	case PPpropALIGNMENT:
	    pp->ppAlignment= arg;
	    break;

	case PPpropTAB_STOPS:
	    LDEB(prop);
	    return 0;

	case PPpropBREAK_KIND:
	    pp->ppBreakKind= arg;
	    break;

	case PPpropWIDCTLPAR:
	    pp->ppWidowControl= arg;
	    break;

	case PPpropKEEP:
	    pp->ppKeepOnPage= arg != 0;
	    break;

	case PPpropKEEPN:
	    pp->ppKeepWithNext= arg != 0;
	    break;

	case PPpropSPACE_BEFORE:
	    pp->ppSpaceBeforeTwips= arg;
	    break;

	case PPpropSPACE_AFTER:
	    pp->ppSpaceAfterTwips= arg;
	    break;

	case PPpropLINE_SPACING_DIST:
	    pp->ppLineSpacingTwips= arg;
	    break;

	case PPpropLINE_SPACING_MULT:
	    pp->ppLineSpacingIsMultiple= ( arg != 0 );
	    break;

	case PPpropTOP_BORDER:
	    pp->ppTopBorderNumber= arg;
	    break;

	case PPpropBOTTOM_BORDER:
	    pp->ppBottomBorderNumber= arg;
	    break;

	case PPpropLEFT_BORDER:
	    pp->ppLeftBorderNumber= arg;
	    break;

	case PPpropRIGHT_BORDER:
	    pp->ppRightBorderNumber= arg;
	    break;

	case PPprop_BOX_BORDER:
	    pp->ppTopBorderNumber= arg;
	    pp->ppBottomBorderNumber= arg;
	    pp->ppLeftBorderNumber= arg;
	    pp->ppRightBorderNumber= arg;
	    break;

	case PPpropBETWEEN_BORDER:
	    pp->ppBetweenBorderNumber= arg;
	    break;

	case PPpropBAR_BORDER:
	    pp->ppBarNumber= arg;
	    break;

	case PPpropOUTLINELEVEL:
	    if  ( arg >= 0 && arg <= PPoutlineBODYTEXT )
		{ pp->ppOutlineLevel= arg;			}
	    else{ pp->ppOutlineLevel= PPoutlineBODYTEXT;	}
	    break;

	case PPpropLISTLEVEL:
	    if  ( arg > 0 && arg < 10 )
		{ pp->ppListLevel= arg;	}
	    else{ pp->ppListLevel= 0;	}
	    break;

	case PPpropHYPHPAR:
	    pp->ppHyphenateParagraph= arg != 0;
	    break;

	case PPpropRTOL:
	    pp->ppRToL= arg != 0;
	    break;

	case PPprop_LISB:
	case PPprop_LISA:
	    break;

	case PPprop_IGNORED:
	    break;

	default:
	    LDEB(prop); return -1;
	}

    return 0;
    }

int docGetParaProperty(		const ParagraphProperties *	pp,
				int				prop )
    {
    switch( prop )
	{
	case PPpropSTYLE:
	    return pp->ppStyle;

	case PPpropLISTOVERRIDE:
	    return pp->ppListOverride;

	case PPprop_IN_TABLE:  /* intbl */
	    return ( pp->ppTableNesting > 0 );

	case PPpropTABLE_NESTING:  /* itap */
	    return pp->ppTableNesting;

	case PPpropLEFT_INDENT:
	    return pp->ppLeftIndentTwips;

	case PPpropFIRST_INDENT:
	    return pp->ppFirstIndentTwips;

	case PPpropRIGHT_INDENT:
	    return pp->ppRightIndentTwips;

	case PPpropALIGNMENT:
	    return pp->ppAlignment;

	case PPpropTAB_STOPS:
	    LDEB(prop); return -1;

	case PPpropBREAK_KIND:
	    return pp->ppBreakKind;

	case PPpropWIDCTLPAR:
	    return pp->ppWidowControl;

	case PPpropKEEP:
	    return pp->ppKeepOnPage;

	case PPpropKEEPN:
	    return pp->ppKeepWithNext;

	case PPpropSPACE_BEFORE:
	    return pp->ppSpaceBeforeTwips;

	case PPpropSPACE_AFTER:
	    return pp->ppSpaceAfterTwips;

	case PPpropLINE_SPACING_DIST:
	    return pp->ppLineSpacingTwips;

	case PPpropLINE_SPACING_MULT:
	    return pp->ppLineSpacingIsMultiple;

	case PPpropTOP_BORDER:
	    return pp->ppTopBorderNumber;

	case PPpropBOTTOM_BORDER:
	    return pp->ppBottomBorderNumber;

	case PPpropLEFT_BORDER:
	    return pp->ppLeftBorderNumber;

	case PPpropRIGHT_BORDER:
	    return pp->ppRightBorderNumber;

	case PPprop_BOX_BORDER:
	    LDEB(prop); return -1;

	case PPpropBETWEEN_BORDER:
	    return pp->ppBetweenBorderNumber;

	case PPpropBAR_BORDER:
	    return pp->ppBarNumber;

	case PPpropOUTLINELEVEL:
	    return pp->ppOutlineLevel;

	case PPpropLISTLEVEL:
	    return pp->ppListLevel;

	case PPpropHYPHPAR:
	    return pp->ppHyphenateParagraph;

	case PPpropRTOL:
	    return pp->ppRToL;

	case PPprop_LISB:
	case PPprop_LISA:
	case PPprop_IGNORED:
	    LDEB(prop); return -1;

	default:
	    LDEB(prop); return -1;
	}
    }

