/************************************************************************/
/*									*/
/*  Shape Properties.							*/
/*									*/
/************************************************************************/

#   ifndef	DOC_SHAPE_PROPERTIES_H
#   define	DOC_SHAPE_PROPERTIES_H

#   include	<geo2DInteger.h>

/************************************************************************/
/*									*/
/*  Primitives for shapes.						*/
/*									*/
/************************************************************************/

typedef enum ShapeWrapType
    {
	    /**
	      *  Wrap around top and bottom of shape. (No text beside shape)
	      */
    SHPswtTOPBOTTOM= 1,

	    /**
	      *  Wrap around shape. (Side is relevant)
	      */
    SHPswtAROUND,

	    /**
	      *  Wrap as if shape is not present.
	      */
    SHPswtNONE,

	    /**
	      *  Wrap tightly around shape.
	      */
    SHPswtTIGHTLY,

	    /**
	      *  Wrap text through shape. (Side is relevant)
	      */
    SHPswtTHROUGH,

    SHPswt_COUNT
    } ShapeWrapType;

typedef enum ShapeWrapSide
    {
	    /**
	      *  Wrap around both sides of the shape.
	      */
    SHPswsBOTH= 0,

	    /**
	      *  Only place text left of the shape.
	      */
    SHPswsLEFT,

	    /**
	      *  Only place text right of the shape.
	      */
    SHPswsRIGHT,

	    /**
	      *  Only place text on the side on the side where most 
	      *  space is left. (Left or right)
	      */
    SHPswsLARGEST,

    SHPsws_COUNT
    } ShapeWrapSide;

typedef struct ShapeProperties
    {
    DocumentRectangle	spRect;
    long		spID;
    int			spZ;

    unsigned char	spXReference;		/*  (enum)	*/
    unsigned char	spYReference;		/*  (enum)	*/
			/**
			  *  shpwrN tag in RTF: A ShapeWrapStyle 
			  *  value.
			  */
    unsigned char	spWrapStyle;		/*  (enum)	*/
			/**
			  *  shpwrkN tag in RTF: A ShapeWrapSide 
			  *  value. Only relevant if 
			  *	spWrapStyle == SHPswsAROUND or
			  *	spWrapStyle == SHPswsTIGHTLY
			  */
    unsigned char	spWrapSide;		/*  (enum)	*/

    unsigned char	spShapeBelowText;
    unsigned char	spInHeader;
    unsigned char	spLockAnchor;
    } ShapeProperties;

/************************************************************************/
/*									*/
/*  Numbers of shape properies.						*/
/*									*/
/************************************************************************/

typedef enum ShapeProperty
    {
    SHPpropLEFT= 0,
    SHPpropTOP,
    SHPpropBOTTOM,
    SHPpropRIGHT,
    SHPpropLID,
    SHPpropZ,
    SHPpropFHDR,
    SHPpropBX,
    SHPpropBY,
    SHPpropWR,
    SHPpropWRK,
    SHPpropFBLWTXT,
    SHPpropLOCKANCHOR,

    SHPprop_COUNT
    } ShapeProperty;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docInitShapeProperties(	ShapeProperties *	sp );

extern int docSetShapeProperty(		ShapeProperties *	sp,
					int			prop,
					int			value );

extern void docPlaceRootShapeRect(	DocumentRectangle *	drTwips,
					const ShapeProperties *	sp,
					int			x0,
					int			y0 );

#   endif	/*  DOC_SHAPE_PROPERTIES_H	*/
