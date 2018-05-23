/************************************************************************/
/*									*/
/*  Picture Properties.							*/
/*									*/
/************************************************************************/

#   ifndef	DOC_PICTURE_PROPERTIES_H
#   define	DOC_PICTURE_PROPERTIES_H

#   include	<utilPropMask.h>

/************************************************************************/
/*									*/
/*  Properties of a picture.						*/
/*									*/
/************************************************************************/

typedef struct PictureProperties
    {
    unsigned char	pipType;
    signed char		pipMapMode;

    int			pip_xWinExt;	/*  Of metafile picture.	*/
    int			pip_yWinExt;	/*  Of metafile picture.	*/

					/********************************/
    int			pipTwipsWide;	/*  Width of object.		*/
    int			pipTwipsHigh;	/*  Height of object.		*/
					/*  NOTE: this is a property of	*/
					/*  the image. Not of the way	*/
					/*  in which it is included.	*/
					/*  That is controlled by the	*/
					/*  scale.			*/
					/********************************/

    int			pipScaleXSet;
    int			pipScaleYSet;
    int			pipScaleXUsed;
    int			pipScaleYUsed;

    int			pipTopCropTwips;
    int			pipBottomCropTwips;
    int			pipLeftCropTwips;
    int			pipRightCropTwips;

    unsigned long	pipBliptag;

    int			pipMetafileBitmapBpp;
    int			pipBmUnitsPerInch;
    int			pipBmBitsPerPixel;
    int			pipBmPlanes;
    int			pipBmBytesPerRow;

    unsigned int	pipPictureIsWordArt:1;
    unsigned int	pipMetafileIsBitmap:1;
    unsigned int	pipPictIsScaled:1;
    } PictureProperties;

#   define docPictureIsCropped(pip) ( \
		(pip)->pipLeftCropTwips > 0	|| \
		(pip)->pipRightCropTwips > 0	|| \
		(pip)->pipTopCropTwips > 0	|| \
		(pip)->pipBottomCropTwips > 0	)

/************************************************************************/
/*									*/
/*  Numbers of picture properies.					*/
/*									*/
/************************************************************************/

typedef enum PictureProperty
    {
    PIPpropTYPE= 0,

    PIPpropPICX_WIN_EXT,
    PIPpropPICY_WIN_EXT,

    PIPpropPICTWIPS_WIDE,
    PIPpropPICTWIPS_HIGH,

    PIPpropPICSCALE_X,
    PIPpropPICSCALE_Y,

    PIPpropPICCROP_TOP,
    PIPpropPICCROP_BOTTOM,
    PIPpropPICCROP_LEFT,
    PIPpropPICCROP_RIGHT,

    PIPpropBLIPTAG,

    PIPpropPICBPP,
    PIPpropBLIPUPI,
    PIPpropWBMBITSPIXEL,
    PIPpropWBMPLANES,
    PIPpropWBMWIDTHBYTES,

    PIPpropDEFSHP,
    PIPpropPICBMP,
    PIPpropPICSCALED,

    PIPprop_COUNT
    } PictureProperty;

/************************************************************************/

# define PICshpSHPPICT		0
# define PICshpNONSHPPICT	1
# define PICshpPICT		2

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docInitPictureProperties(	PictureProperties *	pip );

extern int docUpdPictureProperties(
				PropertyMask *			pDoneMask,
				PictureProperties *		pipTo,
				const PropertyMask *		pipUpdMask,
				const PictureProperties *	pipFrom );

extern void docPicturePropertyDifference(
				PropertyMask *			pDifMask,
				const PictureProperties *	pip1,
				const PropertyMask *		cmpMask,
				const PictureProperties *	pip2 );

extern int docSetPictureProperty(	PictureProperties *	pip,
					int			prop,
					int			val );

extern int docGetPictureProperty(	const PictureProperties *	pip,
					int				prop );

#   endif	/*  DOC_PICTURE_PROPERTIES_H	*/
