/************************************************************************/
/*									*/
/*  Buffer administration routines.					*/
/*									*/
/************************************************************************/

#   include	"docBaseConfig.h"

#   include	<appDebugon.h>

#   include	"docCellProperties.h"
#   include	"docPropVal.h"

/************************************************************************/
/*									*/
/*  Initialise Cell properties.						*/
/*									*/
/************************************************************************/

void docInitCellProperties(	CellProperties *	cp )
    {
    cp->cpRightBoundaryTwips= 0;

    cp->cpTopBorderNumber= 0;
    cp->cpLeftBorderNumber= 0;
    cp->cpRightBorderNumber= 0;
    cp->cpBottomBorderNumber= 0;

    cp->cpShadingNumber= 0;

    /**/
    cp->cpPreferredWidth= 0;
    cp->cpPreferredWidthUnit= TRautoNONE;

    cp->cpTopPadding= 0;
    cp->cpBottomPadding= 0;
    cp->cpLeftPadding= 0;
    cp->cpRightPadding= 0;

    cp->cpTopPaddingUnit= TRautoNONE;
    cp->cpBottomPaddingUnit= TRautoNONE;
    cp->cpLeftPaddingUnit= TRautoNONE;
    cp->cpRightPaddingUnit= TRautoNONE;

    cp->cpHorizontalMerge= CELLmergeNONE;
    cp->cpVerticalMerge= CELLmergeNONE;
    cp->cpNoWrap= 0;
    cp->cpNoShading= 0;

    cp->cpValign= DOCtvaTOP;
    cp->cpTextFlow= TXflowTXLRTB;
    }

/************************************************************************/
/*									*/
/*  Change cell properties and tell what has been changed.		*/
/*									*/
/************************************************************************/

int docUpdCellProperties(	PropertyMask *			pCpDonePask,
				CellProperties *		cpTo,
				const PropertyMask *		cpSetMask,
				const CellProperties *		cpFrom,
				const DocumentAttributeMap *	dam )
    {
    PropertyMask		cpDoneMask;

    utilPropMaskClear( &cpDoneMask );

    if  ( PROPmaskISSET( cpSetMask, CLpropCELLX ) )
	{
	if  ( cpTo->cpRightBoundaryTwips != cpFrom->cpRightBoundaryTwips )
	    {
	    cpTo->cpRightBoundaryTwips= cpFrom->cpRightBoundaryTwips;
	    PROPmaskADD( &cpDoneMask, CLpropCELLX );
	    }
	}

    /**/
    if  ( PROPmaskISSET( cpSetMask, CLpropTOP_BORDER ) )
	{
	int	fromNumber= cpFrom->cpTopBorderNumber;

	if  ( fromNumber >= 0 && dam && dam->damBorderMap )
	    { fromNumber= dam->damBorderMap[fromNumber];	}

	if  ( cpTo->cpTopBorderNumber != fromNumber )
	    {
	    cpTo->cpTopBorderNumber= fromNumber;
	    PROPmaskADD( &cpDoneMask, CLpropTOP_BORDER );
	    }
	}

    if  ( PROPmaskISSET( cpSetMask, CLpropBOTTOM_BORDER ) )
	{
	int	fromNumber= cpFrom->cpBottomBorderNumber;

	if  ( fromNumber >= 0 && dam && dam->damBorderMap )
	    { fromNumber= dam->damBorderMap[fromNumber];	}

	if  ( cpTo->cpBottomBorderNumber != fromNumber )
	    {
	    cpTo->cpBottomBorderNumber= fromNumber;
	    PROPmaskADD( &cpDoneMask, CLpropBOTTOM_BORDER );
	    }
	}

    if  ( PROPmaskISSET( cpSetMask, CLpropLEFT_BORDER ) )
	{
	int	fromNumber= cpFrom->cpLeftBorderNumber;

	if  ( fromNumber >= 0 && dam && dam->damBorderMap )
	    { fromNumber= dam->damBorderMap[fromNumber];	}

	if  ( cpTo->cpLeftBorderNumber != fromNumber )
	    {
	    cpTo->cpLeftBorderNumber= fromNumber;
	    PROPmaskADD( &cpDoneMask, CLpropLEFT_BORDER );
	    }
	}

    if  ( PROPmaskISSET( cpSetMask, CLpropRIGHT_BORDER ) )
	{
	int	fromNumber= cpFrom->cpRightBorderNumber;

	if  ( fromNumber >= 0 && dam && dam->damBorderMap )
	    { fromNumber= dam->damBorderMap[fromNumber];	}

	if  ( cpTo->cpRightBorderNumber != fromNumber )
	    {
	    cpTo->cpRightBorderNumber= fromNumber;
	    PROPmaskADD( &cpDoneMask, CLpropRIGHT_BORDER );
	    }
	}

    if  ( PROPmaskISSET( cpSetMask, CLpropSHADING ) )
	{
	int	fromNumber= cpFrom->cpShadingNumber;

	if  ( fromNumber >= 0 && dam && dam->damShadingMap )
	    { fromNumber= dam->damShadingMap[fromNumber];	}

	if  ( cpTo->cpShadingNumber != fromNumber )
	    {
	    cpTo->cpShadingNumber= fromNumber;
	    PROPmaskADD( &cpDoneMask, CLpropSHADING );
	    }
	}

    /**/
    if  ( PROPmaskISSET( cpSetMask, CLpropCLW_WIDTH ) )
	{
	if  ( cpTo->cpPreferredWidth != cpFrom->cpPreferredWidth )
	    {
	    cpTo->cpPreferredWidth= cpFrom->cpPreferredWidth;
	    PROPmaskADD( &cpDoneMask, CLpropCLW_WIDTH );
	    }
	}

    if  ( PROPmaskISSET( cpSetMask, CLpropCLFTS_WIDTH ) )
	{
	if  ( cpTo->cpPreferredWidthUnit != cpFrom->cpPreferredWidthUnit )
	    {
	    cpTo->cpPreferredWidthUnit= cpFrom->cpPreferredWidthUnit;
	    PROPmaskADD( &cpDoneMask, CLpropCLFTS_WIDTH );
	    }
	}

    /**/
    if  ( PROPmaskISSET( cpSetMask, CLpropHOR_MERGE ) )
	{
	if  ( cpTo->cpHorizontalMerge != cpFrom->cpHorizontalMerge )
	    {
	    cpTo->cpHorizontalMerge= cpFrom->cpHorizontalMerge;
	    PROPmaskADD( &cpDoneMask, CLpropHOR_MERGE );
	    }
	}

    if  ( PROPmaskISSET( cpSetMask, CLpropVERT_MERGE ) )
	{
	if  ( cpTo->cpVerticalMerge != cpFrom->cpVerticalMerge )
	    {
	    cpTo->cpVerticalMerge= cpFrom->cpVerticalMerge;
	    PROPmaskADD( &cpDoneMask, CLpropVERT_MERGE );
	    }
	}

    /**/
    if  ( PROPmaskISSET( cpSetMask, CLpropVALIGN ) )
	{
	if  ( cpTo->cpValign != cpFrom->cpValign )
	    {
	    cpTo->cpValign= cpFrom->cpValign;
	    PROPmaskADD( &cpDoneMask, CLpropVALIGN );
	    }
	}

    if  ( PROPmaskISSET( cpSetMask, CLpropTEXT_FLOW ) )
	{
	if  ( cpTo->cpTextFlow != cpFrom->cpTextFlow )
	    {
	    cpTo->cpTextFlow= cpFrom->cpTextFlow;
	    PROPmaskADD( &cpDoneMask, CLpropTEXT_FLOW );
	    }
	}

    if  ( PROPmaskISSET( cpSetMask, CLpropTOP_PADDING ) )
	{
	if  ( cpTo->cpTopPadding != cpFrom->cpTopPadding )
	    {
	    cpTo->cpTopPadding= cpFrom->cpTopPadding;
	    PROPmaskADD( &cpDoneMask, CLpropTOP_PADDING );
	    }
	}

    if  ( PROPmaskISSET( cpSetMask, CLpropBOTTOM_PADDING ) )
	{
	if  ( cpTo->cpBottomPadding != cpFrom->cpBottomPadding )
	    {
	    cpTo->cpBottomPadding= cpFrom->cpBottomPadding;
	    PROPmaskADD( &cpDoneMask, CLpropBOTTOM_PADDING );
	    }
	}

    if  ( PROPmaskISSET( cpSetMask, CLpropLEFT_PADDING ) )
	{
	if  ( cpTo->cpLeftPadding != cpFrom->cpLeftPadding )
	    {
	    cpTo->cpLeftPadding= cpFrom->cpLeftPadding;
	    PROPmaskADD( &cpDoneMask, CLpropLEFT_PADDING );
	    }
	}

    if  ( PROPmaskISSET( cpSetMask, CLpropRIGHT_PADDING ) )
	{
	if  ( cpTo->cpRightPadding != cpFrom->cpRightPadding )
	    {
	    cpTo->cpRightPadding= cpFrom->cpRightPadding;
	    PROPmaskADD( &cpDoneMask, CLpropRIGHT_PADDING );
	    }
	}

    if  ( PROPmaskISSET( cpSetMask, CLpropTOP_PADDING_UNIT ) )
	{
	if  ( cpTo->cpTopPaddingUnit != cpFrom->cpTopPaddingUnit )
	    {
	    cpTo->cpTopPaddingUnit= cpFrom->cpTopPaddingUnit;
	    PROPmaskADD( &cpDoneMask, CLpropTOP_PADDING_UNIT );
	    }
	}

    if  ( PROPmaskISSET( cpSetMask, CLpropBOTTOM_PADDING_UNIT ) )
	{
	if  ( cpTo->cpBottomPaddingUnit != cpFrom->cpBottomPaddingUnit )
	    {
	    cpTo->cpBottomPaddingUnit= cpFrom->cpBottomPaddingUnit;
	    PROPmaskADD( &cpDoneMask, CLpropBOTTOM_PADDING_UNIT );
	    }
	}

    if  ( PROPmaskISSET( cpSetMask, CLpropLEFT_PADDING_UNIT ) )
	{
	if  ( cpTo->cpLeftPaddingUnit != cpFrom->cpLeftPaddingUnit )
	    {
	    cpTo->cpLeftPaddingUnit= cpFrom->cpLeftPaddingUnit;
	    PROPmaskADD( &cpDoneMask, CLpropLEFT_PADDING_UNIT );
	    }
	}

    if  ( PROPmaskISSET( cpSetMask, CLpropRIGHT_PADDING_UNIT ) )
	{
	if  ( cpTo->cpRightPaddingUnit != cpFrom->cpRightPaddingUnit )
	    {
	    cpTo->cpRightPaddingUnit= cpFrom->cpRightPaddingUnit;
	    PROPmaskADD( &cpDoneMask, CLpropRIGHT_PADDING_UNIT );
	    }
	}

    if  ( pCpDonePask )
	{ utilPropMaskOr( pCpDonePask, pCpDonePask, &cpDoneMask );	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Compare cell properties.						*/
/*									*/
/************************************************************************/

void docCellPropertyDifference(	PropertyMask *			pCpDifPask,
				const CellProperties *		cpTo,
				const PropertyMask *		cpCmpMask,
				const CellProperties *		cpFrom,
				const DocumentAttributeMap *	dam )
    {
    PropertyMask		cpDifMask;

    utilPropMaskClear( &cpDifMask );

    if  ( PROPmaskISSET( cpCmpMask, CLpropCELLX ) )
	{
	if  ( cpTo->cpRightBoundaryTwips != cpFrom->cpRightBoundaryTwips )
	    { PROPmaskADD( &cpDifMask, CLpropCELLX ); }
	}

    /**/
    if  ( PROPmaskISSET( cpCmpMask, CLpropTOP_BORDER ) )
	{
	int	fromNumber= cpFrom->cpTopBorderNumber;

	if  ( fromNumber >= 0 && dam && dam->damBorderMap )
	    { fromNumber= dam->damBorderMap[fromNumber];	}

	if  ( cpTo->cpTopBorderNumber != fromNumber )
	    { PROPmaskADD( &cpDifMask, CLpropTOP_BORDER ); }
	}

    if  ( PROPmaskISSET( cpCmpMask, CLpropBOTTOM_BORDER ) )
	{
	int	fromNumber= cpFrom->cpBottomBorderNumber;

	if  ( fromNumber >= 0 && dam && dam->damBorderMap )
	    { fromNumber= dam->damBorderMap[fromNumber];	}

	if  ( cpTo->cpBottomBorderNumber != fromNumber )
	    { PROPmaskADD( &cpDifMask, CLpropBOTTOM_BORDER ); }
	}

    if  ( PROPmaskISSET( cpCmpMask, CLpropLEFT_BORDER ) )
	{
	int	fromNumber= cpFrom->cpLeftBorderNumber;

	if  ( fromNumber >= 0 && dam && dam->damBorderMap )
	    { fromNumber= dam->damBorderMap[fromNumber];	}

	if  ( cpTo->cpLeftBorderNumber != fromNumber )
	    { PROPmaskADD( &cpDifMask, CLpropLEFT_BORDER ); }
	}

    if  ( PROPmaskISSET( cpCmpMask, CLpropRIGHT_BORDER ) )
	{
	int	fromNumber= cpFrom->cpRightBorderNumber;

	if  ( fromNumber >= 0 && dam && dam->damBorderMap )
	    { fromNumber= dam->damBorderMap[fromNumber];	}

	if  ( cpTo->cpRightBorderNumber != fromNumber )
	    { PROPmaskADD( &cpDifMask, CLpropRIGHT_BORDER ); }
	}

    /**/
    if  ( PROPmaskISSET( cpCmpMask, CLpropSHADING ) )
	{
	int		fromNumber= cpFrom->cpShadingNumber;

	if  ( fromNumber >= 0 && dam && dam->damShadingMap )
	    { fromNumber= dam->damShadingMap[fromNumber];	}

	if  ( cpTo->cpShadingNumber != fromNumber )
	    { PROPmaskADD( &cpDifMask, CLpropSHADING );	}
	}

    /**/
    if  ( PROPmaskISSET( cpCmpMask, CLpropCLW_WIDTH ) )
	{
	if  ( cpTo->cpPreferredWidth != cpFrom->cpPreferredWidth )
	    { PROPmaskADD( &cpDifMask, CLpropCLW_WIDTH );	}
	}

    if  ( PROPmaskISSET( cpCmpMask, CLpropCLFTS_WIDTH ) )
	{
	if  ( cpTo->cpPreferredWidthUnit != cpFrom->cpPreferredWidthUnit )
	    { PROPmaskADD( &cpDifMask, CLpropCLFTS_WIDTH );	}
	}

    if  ( PROPmaskISSET( cpCmpMask, CLpropHOR_MERGE ) )
	{
	if  ( cpTo->cpHorizontalMerge != cpFrom->cpHorizontalMerge )
	    { PROPmaskADD( &cpDifMask, CLpropHOR_MERGE ); }
	}

    /**/
    if  ( PROPmaskISSET( cpCmpMask, CLpropVERT_MERGE ) )
	{
	if  ( cpTo->cpVerticalMerge != cpFrom->cpVerticalMerge )
	    { PROPmaskADD( &cpDifMask, CLpropVERT_MERGE ); }
	}

    /**/
    if  ( PROPmaskISSET( cpCmpMask, CLpropVALIGN ) )
	{
	if  ( cpTo->cpValign != cpFrom->cpValign )
	    { PROPmaskADD( &cpDifMask, CLpropVALIGN ); }
	}

    if  ( PROPmaskISSET( cpCmpMask, CLpropTEXT_FLOW ) )
	{
	if  ( cpTo->cpTextFlow != cpFrom->cpTextFlow )
	    { PROPmaskADD( &cpDifMask, CLpropTEXT_FLOW ); }
	}

    if  ( PROPmaskISSET( cpCmpMask, CLpropTOP_PADDING ) )
	{
	if  ( cpTo->cpTopPadding != cpFrom->cpTopPadding )
	    { PROPmaskADD( &cpDifMask, CLpropTOP_PADDING ); }
	}

    if  ( PROPmaskISSET( cpCmpMask, CLpropBOTTOM_PADDING ) )
	{
	if  ( cpTo->cpBottomPadding != cpFrom->cpBottomPadding )
	    { PROPmaskADD( &cpDifMask, CLpropBOTTOM_PADDING ); }
	}

    if  ( PROPmaskISSET( cpCmpMask, CLpropLEFT_PADDING ) )
	{
	if  ( cpTo->cpLeftPadding != cpFrom->cpLeftPadding )
	    { PROPmaskADD( &cpDifMask, CLpropLEFT_PADDING ); }
	}

    if  ( PROPmaskISSET( cpCmpMask, CLpropRIGHT_PADDING ) )
	{
	if  ( cpTo->cpRightPadding != cpFrom->cpRightPadding )
	    { PROPmaskADD( &cpDifMask, CLpropRIGHT_PADDING ); }
	}

    if  ( PROPmaskISSET( cpCmpMask, CLpropTOP_PADDING_UNIT ) )
	{
	if  ( cpTo->cpTopPaddingUnit != cpFrom->cpTopPaddingUnit )
	    { PROPmaskADD( &cpDifMask, CLpropTOP_PADDING_UNIT ); }
	}

    if  ( PROPmaskISSET( cpCmpMask, CLpropBOTTOM_PADDING_UNIT ) )
	{
	if  ( cpTo->cpBottomPaddingUnit != cpFrom->cpBottomPaddingUnit )
	    { PROPmaskADD( &cpDifMask, CLpropBOTTOM_PADDING_UNIT ); }
	}

    if  ( PROPmaskISSET( cpCmpMask, CLpropLEFT_PADDING_UNIT ) )
	{
	if  ( cpTo->cpLeftPaddingUnit != cpFrom->cpLeftPaddingUnit )
	    { PROPmaskADD( &cpDifMask, CLpropLEFT_PADDING_UNIT ); }
	}

    if  ( PROPmaskISSET( cpCmpMask, CLpropRIGHT_PADDING_UNIT ) )
	{
	if  ( cpTo->cpRightPaddingUnit != cpFrom->cpRightPaddingUnit )
	    { PROPmaskADD( &cpDifMask, CLpropRIGHT_PADDING_UNIT ); }
	}

    if  ( pCpDifPask )
	{ utilPropMaskOr( pCpDifPask, pCpDifPask, &cpDifMask );	}

    return;
    }

int docCopyCellProperties(	CellProperties *		cpTo,
				const CellProperties *		cpFrom,
				const DocumentAttributeMap *	dam )
    {
    PropertyMask		cpDoneMask;
    PropertyMask		cpSetMask;

    utilPropMaskClear( &cpDoneMask );
    utilPropMaskClear( &cpSetMask );

    utilPropMaskFill( &cpSetMask, CLprop_COUNT );

    docUpdCellProperties( &cpDoneMask, cpTo, &cpSetMask, cpFrom, dam );

    return 0;
    }

int docSetCellProperty(		CellProperties *	cp,
				int			prop,
				int			arg )
    {
    switch( prop )
	{
	case CLpropCELLX:
	    cp->cpRightBoundaryTwips= arg;
	    break;

	case CLpropTOP_BORDER:
	    cp->cpTopBorderNumber= arg;
	    break;
	case CLpropBOTTOM_BORDER:
	    cp->cpBottomBorderNumber= arg;
	    break;
	case CLpropLEFT_BORDER:
	    cp->cpLeftBorderNumber= arg;
	    break;
	case CLpropRIGHT_BORDER:
	    cp->cpRightBorderNumber= arg;
	    break;

	case CLpropSHADING:
	    cp->cpShadingNumber= arg;
	    break;
	case CLpropCLW_WIDTH:
	    cp->cpPreferredWidth= arg;
	    break;
	case CLpropCLFTS_WIDTH:
	    cp->cpPreferredWidthUnit= arg;
	    break;

	case CLpropHOR_MERGE:
	    cp->cpHorizontalMerge= arg;
	    break;
	case CLpropVERT_MERGE:
	    cp->cpVerticalMerge= arg;
	    break;
	case CLpropNO_WRAP:
	    cp->cpNoWrap= ( arg != 0 );
	    break;

	case CLpropVALIGN:
	    cp->cpValign= arg;
	    break;
	case CLpropTEXT_FLOW:
	    cp->cpTextFlow= arg;
	    break;

	case CLpropTOP_PADDING:
	    cp->cpTopPadding= arg;
	    break;
	case CLpropBOTTOM_PADDING:
	    cp->cpBottomPadding= arg;
	    break;
	case CLpropLEFT_PADDING:
	    cp->cpLeftPadding= arg;
	    break;
	case CLpropRIGHT_PADDING:
	    cp->cpRightPadding= arg;
	    break;

	case CLpropTOP_PADDING_UNIT:
	    cp->cpTopPaddingUnit= arg;
	    break;
	case CLpropBOTTOM_PADDING_UNIT:
	    cp->cpBottomPaddingUnit= arg;
	    break;
	case CLpropLEFT_PADDING_UNIT:
	    cp->cpLeftPaddingUnit= arg;
	    break;
	case CLpropRIGHT_PADDING_UNIT:
	    cp->cpRightPaddingUnit= arg;
	    break;

	case CLpropNO_SHADING:
	    cp->cpNoShading= ( arg != 0 );
	    break;

	default:
	    LDEB(prop); return -1;
	}

    return 0;
    }

int docGetCellProperty(		const CellProperties *	cp,
				int			prop )
    {
    switch( prop )
	{
	case CLpropCELLX:
	    return cp->cpRightBoundaryTwips;
	    break;

	case CLpropTOP_BORDER:
	    return cp->cpTopBorderNumber;
	case CLpropBOTTOM_BORDER:
	    return cp->cpBottomBorderNumber;
	case CLpropLEFT_BORDER:
	    return cp->cpLeftBorderNumber;
	case CLpropRIGHT_BORDER:
	    return cp->cpRightBorderNumber;

	case CLpropSHADING:
	    return cp->cpShadingNumber;
	case CLpropCLW_WIDTH:
	    return cp->cpPreferredWidth;
	case CLpropCLFTS_WIDTH:
	    return cp->cpPreferredWidthUnit;

	case CLpropHOR_MERGE:
	    return cp->cpHorizontalMerge;
	case CLpropVERT_MERGE:
	    return cp->cpVerticalMerge;
	case CLpropNO_WRAP:
	    return cp->cpNoWrap;

	case CLpropVALIGN:
	    return cp->cpValign;
	case CLpropTEXT_FLOW:
	    return cp->cpTextFlow;

	case CLpropTOP_PADDING:
	    return cp->cpTopPadding;
	case CLpropBOTTOM_PADDING:
	    return cp->cpBottomPadding;
	case CLpropLEFT_PADDING:
	    return cp->cpLeftPadding;
	case CLpropRIGHT_PADDING:
	    return cp->cpRightPadding;

	case CLpropTOP_PADDING_UNIT:
	    return cp->cpTopPaddingUnit;
	case CLpropBOTTOM_PADDING_UNIT:
	    return cp->cpBottomPaddingUnit;
	case CLpropLEFT_PADDING_UNIT:
	    return cp->cpLeftPaddingUnit;
	case CLpropRIGHT_PADDING_UNIT:
	    return cp->cpRightPaddingUnit;

	case CLpropNO_SHADING:
	    return cp->cpNoShading;

	default:
	    LDEB(prop); return -1;
	}
    }
