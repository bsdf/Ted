/************************************************************************/
/*									*/
/*  Buffer administration: Properties of embedded images.		*/
/*									*/
/************************************************************************/

#   include	"docBaseConfig.h"

#   include	<utilPropMask.h>

#   include	"docObjectProperties.h"
#   include	"docPictureProperties.h"

#   include	<appDebugon.h>

void docInitPictureProperties(	PictureProperties *	pip )
    {
    pip->pipType= DOCokUNKNOWN;
    pip->pipMapMode= -1;

    pip->pip_xWinExt= 0;
    pip->pip_yWinExt= 0;

    pip->pipTwipsWide= 0;
    pip->pipTwipsHigh= 0;

    pip->pipScaleXSet= 100;
    pip->pipScaleYSet= 100;
    pip->pipScaleXUsed= 100;
    pip->pipScaleYUsed= 100;

    pip->pipTopCropTwips= 0;
    pip->pipBottomCropTwips= 0;
    pip->pipLeftCropTwips= 0;
    pip->pipRightCropTwips= 0;

    pip->pipBliptag= 0;

    pip->pipMetafileBitmapBpp= 0;
    pip->pipBmUnitsPerInch= 0;
    pip->pipBmBitsPerPixel= 0;
    pip->pipBmPlanes= 0;
    pip->pipBmBytesPerRow= 0;

    pip->pipPictureIsWordArt= 0;
    pip->pipMetafileIsBitmap= 0;
    pip->pipPictIsScaled= 0;

    return;
    }

int docUpdPictureProperties(	PropertyMask *			pDoneMask,
				PictureProperties *		pipTo,
				const PropertyMask *		pipSetMask,
				const PictureProperties *	pipFrom )
    {
    PropertyMask		doneMask;
    int				prop;

    utilPropMaskClear( &doneMask );

    for ( prop= 0; prop < PIPprop_COUNT; prop++ )
	{
	int	valFrom;
	int	valTo;

	if  ( ! PROPmaskISSET( pipSetMask, prop ) )
	    { continue;	}

	valFrom= docGetPictureProperty( pipFrom, prop );
	valTo= docGetPictureProperty( pipTo, prop );

	if  ( valTo != valFrom )
	    {
	    if  ( docSetPictureProperty( pipTo, prop, valFrom ) )
		{ LLDEB(prop,valFrom); return -1;	}

	    PROPmaskADD( &doneMask, prop );
	    }

	if  ( prop == PIPpropTYPE )
	    {
	    if  ( pipTo->pipMapMode != pipFrom->pipMapMode )
		{
		pipTo->pipMapMode= pipFrom->pipMapMode;
		PROPmaskADD( &doneMask, prop );
		}
	    }
	}

    if  ( pDoneMask )
	{ utilPropMaskOr( pDoneMask, pDoneMask, &doneMask ); }

    return 0;
    }

void docPicturePropertyDifference(
				PropertyMask *			pDifMask,
				const PictureProperties *	pip1,
				const PropertyMask *		cmpMask,
				const PictureProperties *	pip2 )
    {
    PropertyMask		difMask;
    int				prop;

    utilPropMaskClear( &difMask );

    for ( prop= 0; prop < PIPprop_COUNT; prop++ )
	{
	int	val1;
	int	val2;

	if  ( ! PROPmaskISSET( cmpMask, prop ) )
	    { continue;	}

	val1= docGetPictureProperty( pip1, prop );
	val2= docGetPictureProperty( pip2, prop );

	if  ( val2 != val1 )
	    { PROPmaskADD( &difMask, prop ); 	}

	if  ( prop == PIPpropTYPE )
	    {
	    if  ( pip2->pipMapMode != pip1->pipMapMode )
		{ PROPmaskADD( &difMask, prop );	}
	    }
	}

    if  ( pDifMask )
	{ utilPropMaskOr( pDifMask, pDifMask, &difMask ); }

    return;
    }

int docSetPictureProperty(	PictureProperties *	pip,
				int			prop,
				int			val )
    {
    switch( prop )
	{
	case PIPpropTYPE:
	    pip->pipType= val;
	    break;

	case PIPpropPICX_WIN_EXT:
	    pip->pip_xWinExt= val;
	    break;
	case PIPpropPICY_WIN_EXT:
	    pip->pip_yWinExt= val;
	    break;

	case PIPpropPICTWIPS_WIDE:
	    pip->pipTwipsWide= val;
	    break;
	case PIPpropPICTWIPS_HIGH:
	    pip->pipTwipsHigh= val;
	    break;

	case PIPpropPICSCALE_X:
	    pip->pipScaleXSet= val;
	    break;
	case PIPpropPICSCALE_Y:
	    pip->pipScaleYSet= val;
	    break;

	case PIPpropPICCROP_TOP:
	    pip->pipTopCropTwips= val;
	    break;
	case PIPpropPICCROP_BOTTOM:
	    pip->pipBottomCropTwips= val;
	    break;
	case PIPpropPICCROP_LEFT:
	    pip->pipLeftCropTwips= val;
	    break;
	case PIPpropPICCROP_RIGHT:
	    pip->pipRightCropTwips= val;
	    break;

	case PIPpropBLIPTAG:
	    pip->pipBliptag= ( (unsigned)val & 0xffffffff ); 
	    break;

	case PIPpropPICBPP:
	    pip->pipMetafileBitmapBpp= val;
	    break;
	case PIPpropBLIPUPI:
	    pip->pipBmUnitsPerInch= val;
	    break;
	case PIPpropWBMBITSPIXEL:
	    pip->pipBmBitsPerPixel= val;
	    break;
	case PIPpropWBMPLANES:
	    pip->pipBmPlanes= val;
	    break;
	case PIPpropWBMWIDTHBYTES:
	    pip->pipBmBytesPerRow= val;
	    break;

	case PIPpropDEFSHP:
	    pip->pipPictureIsWordArt= val != 0;
	    break;
	case PIPpropPICBMP:
	    pip->pipMetafileIsBitmap= val != 0;
	    break;
	case PIPpropPICSCALED:
	    pip->pipPictIsScaled= val != 0;
	    break;

	default:
	    LDEB(prop); return -1;
	}

    return 0;
    }

int docGetPictureProperty(	const PictureProperties *	pip,
				int				prop )
    {
    switch( prop )
	{
	case PIPpropTYPE:
	    return pip->pipType;

	case PIPpropPICX_WIN_EXT:
	    return pip->pip_xWinExt;
	case PIPpropPICY_WIN_EXT:
	    return pip->pip_yWinExt;

	case PIPpropPICTWIPS_WIDE:
	    return pip->pipTwipsWide;
	case PIPpropPICTWIPS_HIGH:
	    return pip->pipTwipsHigh;

	case PIPpropPICSCALE_X:
	    return pip->pipScaleXSet;
	case PIPpropPICSCALE_Y:
	    return pip->pipScaleYSet;

	case PIPpropPICCROP_TOP:
	    return pip->pipTopCropTwips;
	case PIPpropPICCROP_BOTTOM:
	    return pip->pipBottomCropTwips;
	case PIPpropPICCROP_LEFT:
	    return pip->pipLeftCropTwips;
	case PIPpropPICCROP_RIGHT:
	    return pip->pipRightCropTwips;

	case PIPpropBLIPTAG:
	    return pip->pipBliptag;

	case PIPpropPICBPP:
	    return pip->pipMetafileBitmapBpp;
	case PIPpropBLIPUPI:
	    return pip->pipBmUnitsPerInch;
	case PIPpropWBMBITSPIXEL:
	    return pip->pipBmBitsPerPixel;
	case PIPpropWBMPLANES:
	    return pip->pipBmPlanes;
	case PIPpropWBMWIDTHBYTES:
	    return pip->pipBmBytesPerRow;

	case PIPpropDEFSHP:
	    return pip->pipPictureIsWordArt;
	case PIPpropPICBMP:
	    return pip->pipMetafileIsBitmap;
	case PIPpropPICSCALED:
	    return pip->pipPictIsScaled;

	default:
	    LDEB(prop); return -1;
	}

    return 0;
    }
