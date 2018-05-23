#   include	"appUtilConfig.h"

#   include	"utilDocumentGeometry.h"
#   include	"utilPropMask.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Set default document geometry.					*/
/*									*/
/************************************************************************/

void utilInitDocumentGeometry(	DocumentGeometry *	dg )
    {
    dg->dgPageWideTwips= 12240;
    dg->dgPageHighTwips= 15840;

    dg->dgLeftMarginTwips= 1800;
    dg->dgTopMarginTwips= 1440;
    dg->dgRightMarginTwips= 1800;
    dg->dgBottomMarginTwips= 1440;

    dg->dgHeaderPositionTwips= 720;
    dg->dgFooterPositionTwips= 720;

    dg->dgGutterTwips= 0;
    dg->dgMirrorMargins= 0;
    }

/************************************************************************/
/*									*/
/*  Copy a document Geometry.						*/
/*									*/
/************************************************************************/

void utilUpdDocumentGeometry(	PropertyMask *			dpDoneMask,
				DocumentGeometry *		dgTo,
				const PropertyMask *		dgSetMask,
				const DocumentGeometry *	dgSet )
    {
    PropertyMask		doneMask;

    utilPropMaskClear( &doneMask );

    if  ( PROPmaskISSET( dgSetMask, DGpropPAGE_WIDTH ) )
	{
	if  ( dgTo->dgPageWideTwips != dgSet->dgPageWideTwips )
	    {
	    dgTo->dgPageWideTwips= dgSet->dgPageWideTwips;
	    PROPmaskADD( &doneMask, DGpropPAGE_WIDTH );
	    }
	}

    if  ( PROPmaskISSET( dgSetMask, DGpropPAGE_HEIGHT ) )
	{
	if  ( dgTo->dgPageHighTwips != dgSet->dgPageHighTwips )
	    {
	    dgTo->dgPageHighTwips= dgSet->dgPageHighTwips;
	    PROPmaskADD( &doneMask, DGpropPAGE_HEIGHT );
	    }
	}

    if  ( PROPmaskISSET( dgSetMask, DGpropLEFT_MARGIN ) )
	{
	if  ( dgTo->dgLeftMarginTwips != dgSet->dgLeftMarginTwips )
	    {
	    dgTo->dgLeftMarginTwips= dgSet->dgLeftMarginTwips;
	    PROPmaskADD( &doneMask, DGpropLEFT_MARGIN );
	    }
	}

    if  ( PROPmaskISSET( dgSetMask, DGpropRIGHT_MARGIN ) )
	{
	if  ( dgTo->dgRightMarginTwips != dgSet->dgRightMarginTwips )
	    {
	    dgTo->dgRightMarginTwips= dgSet->dgRightMarginTwips;
	    PROPmaskADD( &doneMask, DGpropRIGHT_MARGIN );
	    }
	}

    if  ( PROPmaskISSET( dgSetMask, DGpropTOP_MARGIN ) )
	{
	if  ( dgTo->dgTopMarginTwips != dgSet->dgTopMarginTwips )
	    {
	    dgTo->dgTopMarginTwips= dgSet->dgTopMarginTwips;
	    PROPmaskADD( &doneMask, DGpropTOP_MARGIN );
	    }
	}

    if  ( PROPmaskISSET( dgSetMask, DGpropBOTTOM_MARGIN ) )
	{
	if  ( dgTo->dgBottomMarginTwips != dgSet->dgBottomMarginTwips )
	    {
	    dgTo->dgBottomMarginTwips= dgSet->dgBottomMarginTwips;
	    PROPmaskADD( &doneMask, DGpropBOTTOM_MARGIN );
	    }
	}

    if  ( PROPmaskISSET( dgSetMask, DGpropHEADER_POSITION ) )
	{
	if  ( dgTo->dgHeaderPositionTwips != dgSet->dgHeaderPositionTwips )
	    {
	    dgTo->dgHeaderPositionTwips= dgSet->dgHeaderPositionTwips;
	    PROPmaskADD( &doneMask, DGpropHEADER_POSITION );
	    }
	}

    if  ( PROPmaskISSET( dgSetMask, DGpropFOOTER_POSITION ) )
	{
	if  ( dgTo->dgFooterPositionTwips != dgSet->dgFooterPositionTwips )
	    {
	    dgTo->dgFooterPositionTwips= dgSet->dgFooterPositionTwips;
	    PROPmaskADD( &doneMask, DGpropFOOTER_POSITION );
	    }
	}

    if  ( PROPmaskISSET( dgSetMask, DGpropGUTTER ) )
	{
	if  ( dgTo->dgGutterTwips != dgSet->dgGutterTwips )
	    {
	    dgTo->dgGutterTwips= dgSet->dgGutterTwips;
	    PROPmaskADD( &doneMask, DGpropGUTTER );
	    }
	}

    if  ( PROPmaskISSET( dgSetMask, DGpropMARGMIR ) )
	{
	if  ( dgTo->dgMirrorMargins != dgSet->dgMirrorMargins )
	    {
	    dgTo->dgMirrorMargins= dgSet->dgMirrorMargins;
	    PROPmaskADD( &doneMask, DGpropMARGMIR );
	    }
	}

    if  ( dpDoneMask )
	{ utilPropMaskOr( dpDoneMask, dpDoneMask, &doneMask );	}

    return;
    }

void utilDocumentGeometryGetBodyRect(
				DocumentRectangle *		dr,
				const DocumentGeometry *	dg )
    {
    dr->drX0= dg->dgLeftMarginTwips;
    dr->drX1= dg->dgPageWideTwips- dg->dgRightMarginTwips;
    dr->drY0= dg->dgTopMarginTwips;
    dr->drY1= dg->dgPageHighTwips- dg->dgBottomMarginTwips;
    }

void utilDocumentGeometryGetHeaderRect(
				DocumentRectangle *		dr,
				const DocumentGeometry *	dg )
    {
    dr->drX0= dg->dgLeftMarginTwips;
    dr->drX1= dg->dgPageWideTwips- dg->dgRightMarginTwips;
    dr->drY0= dg->dgHeaderPositionTwips;
    dr->drY1= dg->dgTopMarginTwips;
    }

void utilDocumentGeometryGetFooterRect(
				DocumentRectangle *		dr,
				const DocumentGeometry *	dg )
    {
    dr->drX0= dg->dgLeftMarginTwips;
    dr->drX1= dg->dgPageWideTwips- dg->dgRightMarginTwips;
    dr->drY0= dg->dgPageHighTwips- dg->dgBottomMarginTwips;
    dr->drY1= dg->dgPageHighTwips- dg->dgFooterPositionTwips;
    }

void utilDocumentGeometryGetPageBoundingBox(
				DocumentRectangle *		dr,
				const DocumentGeometry *	dg,
				int				hasHeader,
				int				hasFooter )
    {
    utilDocumentGeometryGetBodyRect( dr, dg );

    if  ( hasHeader )
	{
	DocumentRectangle	drHead;

	utilDocumentGeometryGetHeaderRect( &drHead, dg );
	geoUnionRectangle( dr, dr, &drHead );
	}

    if  ( hasFooter )
	{
	DocumentRectangle	drFoot;

	utilDocumentGeometryGetFooterRect( &drFoot, dg );
	geoUnionRectangle( dr, dr, &drFoot );
	}
    }

void utilOverridePaperSize(	DocumentGeometry *		dgTo,
				const DocumentGeometry *	dgFrom )
    {
    if  ( dgTo->dgPageWideTwips > dgTo->dgPageHighTwips		&&
	  dgFrom->dgPageHighTwips > dgFrom->dgPageWideTwips	)
	{
	dgTo->dgPageWideTwips= dgFrom->dgPageHighTwips;
	dgTo->dgPageHighTwips= dgFrom->dgPageWideTwips;
	}
    else{
	dgTo->dgPageWideTwips= dgFrom->dgPageWideTwips;
	dgTo->dgPageHighTwips= dgFrom->dgPageHighTwips;
	}
    }

