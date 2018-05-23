/************************************************************************/
/*									*/
/*  Save a BufferDocument into an RTF file.				*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<stdio.h>
#   include	<ctype.h>

#   include	<appDebugon.h>

#   include	<docObjectProperties.h>
#   include	<geoUnits.h>
#   include	"docRtfWriterImpl.h"

/************************************************************************/
/*									*/
/*  Write the tags that describe the geometry of a picture object.	*/
/*									*/
/************************************************************************/

int docRtfSavePictureTags(	RtfWriter *			rwc,
				const PropertyMask *		pipSetMask,
				const PictureProperties *	pipSet )
    {
    if  ( PROPmaskISSET( pipSetMask, PIPpropTYPE ) )
	{
	switch( pipSet->pipType )
	    {
	    case DOCokPICTWMETAFILE:
		docRtfWriteArgTag( rwc, "wmetafile", pipSet->pipMapMode );

		if  ( pipSet->pipMetafileIsBitmap )
		    {
		    if  ( pipSet->pipMetafileBitmapBpp > 0 )
			{
			docRtfWriteArgTag( rwc, "picbpp",
					    pipSet->pipMetafileBitmapBpp );
			}
		    }
		break;

	    case DOCokMACPICT:
		docRtfWriteTag( rwc, "macpict" );
		break;
	    case DOCokPICTPNGBLIP:
		docRtfWriteTag( rwc, "pngblip" );
		break;
	    case DOCokPICTJPEGBLIP:
		docRtfWriteTag( rwc, "jpegblip" );
		break;
	    case DOCokPICTEMFBLIP:
		docRtfWriteTag( rwc, "emfblip" );
		break;
	    case DOCokDIBITMAP:
		docRtfWriteTag( rwc, "dibitmap" );
		break;
	    case DOCokWBITMAP:
		docRtfWriteTag( rwc, "wbitmap" );
		break;
	    case DOCokPMMETAFILE:
		docRtfWriteTag( rwc, "pmmetafile" );
		break;

	    default:
		LDEB(pipSet->pipType);
		break;
	    }
	}

    if  ( PROPmaskISSET( pipSetMask, PIPpropPICX_WIN_EXT ) )
	{
	int	xExt= pipSet->pip_xWinExt;

	if  ( xExt == 0 )
	    {
	    xExt= (int)( 100000.0* pipSet->pipTwipsWide )/
						    ( 20* POINTS_PER_M );
	    }

	docRtfWriteArgTag( rwc, "picw", xExt );
	}

    if  ( PROPmaskISSET( pipSetMask, PIPpropPICY_WIN_EXT ) )
	{
	int	yExt= pipSet->pip_yWinExt;

	if  ( yExt == 0 )
	    {
	    yExt= (int)( 100000.0* pipSet->pipTwipsHigh )/
						    ( 20* POINTS_PER_M );
	    }

	docRtfWriteArgTag( rwc, "pich", yExt );
	}

    if  ( PROPmaskISSET( pipSetMask, PIPpropPICSCALE_X ) )
	{ docRtfWriteArgTag( rwc, "picscalex", pipSet->pipScaleXSet ); }
    if  ( PROPmaskISSET( pipSetMask, PIPpropPICSCALE_Y ) )
	{ docRtfWriteArgTag( rwc, "picscaley", pipSet->pipScaleYSet ); }

    if  ( PROPmaskISSET( pipSetMask, PIPpropPICTWIPS_WIDE ) )
	{ docRtfWriteArgTag( rwc, "picwgoal", pipSet->pipTwipsWide );	}
    if  ( PROPmaskISSET( pipSetMask, PIPpropPICTWIPS_HIGH ) )
	{ docRtfWriteArgTag( rwc, "pichgoal", pipSet->pipTwipsHigh );	}

    if  ( PROPmaskISSET( pipSetMask, PIPpropPICCROP_TOP ) )
	{ docRtfWriteArgTag( rwc, "piccropt", pipSet->pipTopCropTwips ); }
    if  ( PROPmaskISSET( pipSetMask, PIPpropPICCROP_BOTTOM ) )
	{ docRtfWriteArgTag( rwc, "piccropb", pipSet->pipBottomCropTwips ); }
    if  ( PROPmaskISSET( pipSetMask, PIPpropPICCROP_LEFT ) )
	{ docRtfWriteArgTag( rwc, "piccropl", pipSet->pipLeftCropTwips ); }
    if  ( PROPmaskISSET( pipSetMask, PIPpropPICCROP_RIGHT ) )
	{ docRtfWriteArgTag( rwc, "piccropr", pipSet->pipRightCropTwips ); }

    if  ( PROPmaskISSET( pipSetMask, PIPpropBLIPTAG ) )
	{ docRtfWriteArgTag( rwc, "bliptag", pipSet->pipBliptag ); }

    if  ( PROPmaskISSET( pipSetMask, PIPpropWBMBITSPIXEL ) )
	{ docRtfWriteArgTag( rwc, "wbmbitspixel", pipSet->pipBmBitsPerPixel ); }
    if  ( PROPmaskISSET( pipSetMask, PIPpropWBMPLANES ) )
	{ docRtfWriteArgTag( rwc, "wbmplanes", pipSet->pipBmPlanes );	}
    if  ( PROPmaskISSET( pipSetMask, PIPpropWBMWIDTHBYTES ) )
	{ docRtfWriteArgTag( rwc, "wbmwidthbytes", pipSet->pipBmBytesPerRow ); }

    if  ( PROPmaskISSET( pipSetMask, PIPpropDEFSHP ) )
	{ docRtfWriteFlagTag( rwc, "defshp", pipSet->pipPictureIsWordArt ); }
    if  ( PROPmaskISSET( pipSetMask, PIPpropPICSCALED ) )
	{ docRtfWriteFlagTag( rwc, "picscaled", pipSet->pipPictIsScaled ); }
    if  ( PROPmaskISSET( pipSetMask, PIPpropPICBMP ) )
	{ docRtfWriteFlagTag( rwc, "picbmp", pipSet->pipMetafileIsBitmap ); }

    if  ( PROPmaskISSET( pipSetMask, PIPpropBLIPUPI ) )
	{ docRtfWriteArgTag( rwc, "blipupi", pipSet->pipBmUnitsPerInch ); }

    return 0;
    }

int docRtfPicturePropertyMask(	PropertyMask *			pipSetMask,
				const PictureProperties *	pipSet )
    {
    int			rval= 0;
    PictureProperties	pipDef;
    PropertyMask	pipAllMask;

    utilPropMaskClear( pipSetMask );
    utilPropMaskClear( &pipAllMask );
    utilPropMaskFill( &pipAllMask, PIPprop_COUNT );

    docInitPictureProperties( &pipDef );

    if  ( docUpdPictureProperties( pipSetMask, &pipDef, &pipAllMask, pipSet ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    /*docCleanPictureProperties( &pipDef );*/

    return rval;
    }

