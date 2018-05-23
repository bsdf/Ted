/************************************************************************/
/*									*/
/*  Management of Border Properties.					*/
/*									*/
/************************************************************************/

#   include	"docBaseConfig.h"

#   include	<utilPalette.h>

#   include	<appDebugon.h>

#   include	"docBorderProperties.h"

/************************************************************************/
/*									*/
/*  Initialise Border Properties.					*/
/*									*/
/************************************************************************/

void docInitBorderProperties(	BorderProperties *	bp )
    {
    bp->bpColor= 0;
    bp->bpSpacingTwips= 0;
    bp->bpPenWideTwips= 15;
    bp->bpStyle= DOCbsNONE;
    bp->bpArt= 0;
    }

/************************************************************************/
/*									*/
/*  Get the thickness of a border.					*/
/*									*/
/************************************************************************/

int docBorderThick(		int *				pSpace,
				const BorderProperties *	bp )
    {
    int			space;
    int			thick;

    if  ( ! DOCisBORDER( bp ) )
	{
	thick= 0;
	space= 0;
	}
    else{
	thick= bp->bpPenWideTwips;

	if  ( bp->bpStyle == DOCbsTH )
	    { thick *= 2;	}

	/* This is the way MS-Word works: */
	if  ( thick > 0 && thick < 5 )
	    { thick= 5;	}

	space= bp->bpSpacingTwips;
	}

    *pSpace= space;
    return thick;
    }


/************************************************************************/
/*									*/
/*  1) Are the columns in two RowProperties 'the same' (Do they align?)	*/
/*  2) All column properties identical?					*/
/*									*/
/************************************************************************/

static void docBorderPropertyDifference(
				PropertyMask *			pBpDifMask,
				const BorderProperties *	bpTo,
				const PropertyMask *		bpCmpMask,
				const BorderProperties *	bpFrom,
				const int *			colorMap )
    {
    PropertyMask		bpDifMask;

    utilPropMaskClear( &bpDifMask );

    if  ( PROPmaskISSET( bpCmpMask, BRDRpropCOLOR ) )
	{
	int		fromColor= bpFrom->bpColor;

	if  ( fromColor > 0 && colorMap )
	    { fromColor= colorMap[fromColor];	}

	if  ( bpTo->bpColor != fromColor )
	    { PROPmaskADD( &bpDifMask, BRDRpropCOLOR ); }
	}

    if  ( PROPmaskISSET( bpCmpMask, BRDRpropSPACING ) )
	{
	if  ( bpTo->bpSpacingTwips != bpFrom->bpSpacingTwips )
	    { PROPmaskADD( &bpDifMask, BRDRpropSPACING ); }
	}

    if  ( PROPmaskISSET( bpCmpMask, BRDRpropPEN_WIDE ) )
	{
	if  ( bpTo->bpPenWideTwips != bpFrom->bpPenWideTwips )
	    { PROPmaskADD( &bpDifMask, BRDRpropPEN_WIDE ); }
	}

    if  ( PROPmaskISSET( bpCmpMask, BRDRpropSTYLE ) )
	{
	if  ( bpTo->bpStyle != bpFrom->bpStyle )
	    { PROPmaskADD( &bpDifMask, BRDRpropSTYLE ); }
	}

    if  ( PROPmaskISSET( bpCmpMask, BRDRpropART ) )
	{
	if  ( bpTo->bpArt != bpFrom->bpArt )
	    { PROPmaskADD( &bpDifMask, BRDRpropART ); }
	}

    *pBpDifMask= bpDifMask;
    return;
    }

void docUpdateBorderProperties(	PropertyMask *			pBpDoneMask,
				BorderProperties *		bpTo,
				const PropertyMask *		bpSetMask,
				const BorderProperties *	bpSet )
    {
    PropertyMask		bpDoneMask;

    utilPropMaskClear( &bpDoneMask );

    if  ( PROPmaskISSET( bpSetMask, BRDRpropCOLOR ) )
	{
	if  ( bpTo->bpColor != bpSet->bpColor )
	    {
	    bpTo->bpColor= bpSet->bpColor;
	    PROPmaskADD( &bpDoneMask, BRDRpropCOLOR );
	    }
	}

    if  ( PROPmaskISSET( bpSetMask, BRDRpropSPACING ) )
	{
	if  ( bpTo->bpSpacingTwips != bpSet->bpSpacingTwips )
	    {
	    bpTo->bpSpacingTwips= bpSet->bpSpacingTwips;
	    PROPmaskADD( &bpDoneMask, BRDRpropSPACING );
	    }
	}

    if  ( PROPmaskISSET( bpSetMask, BRDRpropPEN_WIDE ) )
	{
	if  ( bpTo->bpPenWideTwips != bpSet->bpPenWideTwips )
	    {
	    bpTo->bpPenWideTwips= bpSet->bpPenWideTwips;
	    PROPmaskADD( &bpDoneMask, BRDRpropPEN_WIDE );
	    }
	}

    if  ( PROPmaskISSET( bpSetMask, BRDRpropSTYLE ) )
	{
	if  ( bpTo->bpStyle != bpSet->bpStyle )
	    {
	    bpTo->bpStyle= bpSet->bpStyle;
	    PROPmaskADD( &bpDoneMask, BRDRpropSTYLE );
	    }
	}

    if  ( PROPmaskISSET( bpSetMask, BRDRpropART ) )
	{
	if  ( bpTo->bpArt != bpSet->bpArt )
	    {
	    bpTo->bpArt= bpSet->bpArt;
	    PROPmaskADD( &bpDoneMask, BRDRpropART );
	    }
	}

    *pBpDoneMask= bpDoneMask;
    return;
    }

int docBordersDiffer(		const BorderProperties *	bpTo,
				const BorderProperties *	bpFrom,
				const int *			colorMap )
    {
    PropertyMask	bpCmpMask;
    PropertyMask	bpDifMask;

    utilPropMaskClear( &bpDifMask );
    utilPropMaskClear( &bpCmpMask );
    utilPropMaskFill( &bpCmpMask, BRDRprop_COUNT );

    docBorderPropertyDifference( &bpDifMask, bpTo,
					    &bpCmpMask, bpFrom, colorMap );

    if  ( ! utilPropMaskIsEmpty( &bpDifMask ) )
	{ return 1;	}

    return 0;
    }

void docCopyBorderProperties(	BorderProperties *		bpTo,
				const BorderProperties *	bpFrom,
				const int *			colorMap )
    {
    *bpTo= *bpFrom;

    if  ( ! DOCisBORDER( bpTo ) )
	{ bpTo->bpColor= 0;	}
    else{
	if  ( bpTo->bpColor > 0 && colorMap )
	    { bpTo->bpColor= colorMap[bpTo->bpColor];	}
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Add the space occupied by a border to an inset.			*/
/*									*/
/************************************************************************/

void docAddBorderToInset(	int *				pInset,
				const BorderProperties *	bp )
    {
    if  ( ! DOCisBORDER( bp ) )
	{ return;	}

    (*pInset) += bp->bpSpacingTwips;
    (*pInset) += bp->bpPenWideTwips;

    return;
    }

void docStretchInsetForBorder(	int *				pInset,
				const BorderProperties *	bp )
    {
    int		inset= 0;

    if  ( ! DOCisBORDER( bp ) )
	{ return;	}

    inset += bp->bpSpacingTwips;
    inset += bp->bpPenWideTwips;

    if  ( (*pInset) < inset )
	{ (*pInset)=  inset;	}

    return;
    }

/************************************************************************/
/*									*/
/*  Expand the palette references in border properties.			*/
/*									*/
/************************************************************************/

void docExpandBorderProperties(	ExpandedBorderProperties *	ebp,
				const BorderProperties *	bp,
				const ColorPalette *		cp )
    {
    if  ( bp->bpColor == 0 )
	{
	ebp->ebpColorExplicit= 0;
	utilInitRGB8Color( &(ebp->ebpColor) );
	}
    else{
	if  ( bp->bpColor >= cp->cpColorCount	)
	    {
	    LLDEB(bp->bpColor,cp->cpColorCount);

	    ebp->ebpColorExplicit= 0;
	    utilInitRGB8Color( &(ebp->ebpColor) );
	    }
	else{
	    ebp->ebpColorExplicit= 1;
	    ebp->ebpColor= cp->cpColors[bp->bpColor];
	    }
	}

    ebp->ebpPenWideTwips= bp->bpPenWideTwips;
    ebp->ebpStyle= bp->bpStyle;
    ebp->ebpArt= bp->bpArt;
    ebp->ebpSpacingTwips= bp->bpSpacingTwips;

    return;
    }

/************************************************************************/
/*									*/
/*  Get the palette references in a border properties.			*/
/*									*/
/************************************************************************/

int docIndirectBorderProperties(
			PropertyMask *				pBpDoneMask,
			BorderProperties *			bpTo,
			const PropertyMask *			bpSetMask,
			const ExpandedBorderProperties *	ebpFrom,
			ColorPalette *				cp )
    {
    PropertyMask		bpDoneMask;

    utilPropMaskClear( &bpDoneMask );

    if  ( PROPmaskISSET( bpSetMask, BRDRpropCOLOR ) )
	{
	const int		avoidZero= 1;
	const int		maxColors= 256;
	int			color;

	if  ( ebpFrom->ebpColorExplicit )
	    {
	    color= utilPaletteInsertColor( cp,
				avoidZero, maxColors, &(ebpFrom->ebpColor) );
	    if  ( color < 0 )
		{ LDEB(color); return -1;	}

	    color= color;
	    }
	else{ color= 0;	}

	if  ( bpTo->bpColor != color )
	    {
	    bpTo->bpColor= color;
	    PROPmaskADD( &bpDoneMask, BRDRpropCOLOR );
	    }
	}

    if  ( PROPmaskISSET( bpSetMask, BRDRpropSPACING ) )
	{
	if  ( bpTo->bpSpacingTwips != ebpFrom->ebpSpacingTwips )
	    {
	    bpTo->bpSpacingTwips= ebpFrom->ebpSpacingTwips;
	    PROPmaskADD( &bpDoneMask, BRDRpropSPACING );
	    }
	}

    if  ( PROPmaskISSET( bpSetMask, BRDRpropPEN_WIDE ) )
	{
	if  ( bpTo->bpPenWideTwips != ebpFrom->ebpPenWideTwips )
	    {
	    bpTo->bpPenWideTwips= ebpFrom->ebpPenWideTwips;
	    PROPmaskADD( &bpDoneMask, BRDRpropPEN_WIDE );
	    }
	}

    if  ( PROPmaskISSET( bpSetMask, BRDRpropSTYLE ) )
	{
	if  ( bpTo->bpStyle != ebpFrom->ebpStyle )
	    {
	    bpTo->bpStyle= ebpFrom->ebpStyle;
	    PROPmaskADD( &bpDoneMask, BRDRpropSTYLE );
	    }
	}

    if  ( PROPmaskISSET( bpSetMask, BRDRpropART ) )
	{
	if  ( bpTo->bpArt != ebpFrom->ebpArt )
	    {
	    bpTo->bpArt= ebpFrom->ebpArt;
	    PROPmaskADD( &bpDoneMask, BRDRpropART );
	    }
	}

    *pBpDoneMask= bpDoneMask;
    return 0;
    }


/************************************************************************/
/*									*/
/*  Initialize expanded border properties.				*/
/*									*/
/************************************************************************/

void docInitExpandedBorderProperties(	ExpandedBorderProperties *	ebp )
    {
    ebp->ebpColorExplicit= 0;

    utilInitRGB8Color( &(ebp->ebpColor) );

    ebp->ebpSpacingTwips= 0;
    ebp->ebpPenWideTwips= 15;
    ebp->ebpStyle= DOCbsNONE;
    ebp->ebpArt= 0;
    }

/************************************************************************/
/*									*/
/*  Set a border property.						*/
/*									*/
/************************************************************************/

int docSetBorderProperty(	BorderProperties *	bp,
				int			prop,
				int			arg )
    {
    switch( prop )
	{
	case BRDRpropPEN_WIDE:
	    if  ( arg > 255 )
		{ LDEB(arg); arg= 255;	}

	    bp->bpPenWideTwips= arg;
	    break;

	case BRDRpropCOLOR:
	    if  ( arg > 255 )
		{ LDEB(arg); arg= 255;	}

	    bp->bpColor= arg;
	    break;

	case BRDRpropSPACING:
	    bp->bpSpacingTwips= arg;
	    break;

	case BRDRpropSTYLE:
	    bp->bpStyle= arg;
	    break;

	case BRDRpropART:
	    bp->bpArt= arg;
	    break;

	default:
	    LDEB(prop); return -1;
	}

    return 0;
    }


/************************************************************************/
/*									*/
/*  Get a border property.						*/
/*									*/
/************************************************************************/

int docGetBorderProperty(	const BorderProperties *	bp,
				int				prop )
    {
    switch( prop )
	{
	case BRDRpropPEN_WIDE:
	    return bp->bpPenWideTwips;

	case BRDRpropCOLOR:
	    return bp->bpColor;

	case BRDRpropSPACING:
	    return bp->bpSpacingTwips;

	case BRDRpropSTYLE:
	    return bp->bpStyle;

	case BRDRpropART:
	    return bp->bpArt;

	default:
	    LDEB(prop); return -1;
	}
    }

