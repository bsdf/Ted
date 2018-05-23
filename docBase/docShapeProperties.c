/************************************************************************/
/*									*/
/*  Shapes.. Administration.						*/
/*									*/
/************************************************************************/

#   include	"docBaseConfig.h"

#   include	"docPropVal.h"
#   include	"docShapeProperties.h"

#   include	<appDebugon.h>

void docInitShapeProperties(	ShapeProperties *	sp )
    {
    geoInitRectangle( &(sp->spRect) );

    sp->spXReference= FXrefCOLUMN;
    sp->spYReference= FYrefPARA;
    sp->spWrapStyle= SHPswtTOPBOTTOM;
    sp->spWrapSide= SHPswsBOTH;
    sp->spLockAnchor= 0;

    sp->spShapeBelowText= 0;
    sp->spInHeader= 0;
    sp->spLockAnchor= 0;

    return;
    }

int docSetShapeProperty(	ShapeProperties *	sp,
				int			prop,
				int			value )
    {
    switch( prop )
	{
	case SHPpropLEFT:
	    sp->spRect.drX0= value;
	    return 0;
	case SHPpropRIGHT:
	    sp->spRect.drX1= value;
	    return 0;
	case SHPpropTOP:
	    sp->spRect.drY0= value;
	    return 0;
	case SHPpropBOTTOM:
	    sp->spRect.drY1= value;
	    return 0;

	case SHPpropLID:
	    sp->spID= value;
	    return 0;
	case SHPpropZ:
	    sp->spID= value;
	    return 0;

	case SHPpropFHDR:
	    sp->spInHeader= value != 0;
	    return 0;

	case SHPpropBX:
	    sp->spXReference= value;
	    return 0;

	case SHPpropBY:
	    sp->spYReference= value;
	    return 0;

	case SHPpropWR:
	    sp->spWrapStyle= value;
	    return 0;
	case SHPpropWRK:
	    sp->spWrapSide= value;
	    return 0;

	case SHPpropFBLWTXT:
	    sp->spShapeBelowText= ( value != 0 );
	    return 0;
	case SHPpropLOCKANCHOR:
	    sp->spLockAnchor= ( value != 0 );
	    return 0;

	default:
	    LLDEB(prop,value); return -1;
	}
    }

/************************************************************************/
/*									*/
/*  Given the origin.. Get the rectangle for a shape.			*/
/*									*/
/************************************************************************/

void docPlaceRootShapeRect(		DocumentRectangle *	drTwips,
					const ShapeProperties *	sp,
					int			x0,
					int			y0 )
    {
    drTwips->drX0= x0;
    drTwips->drX1= drTwips->drX0+ sp->spRect.drX1- sp->spRect.drX0;

    drTwips->drY0= y0;
    drTwips->drY1= drTwips->drY0+ sp->spRect.drY1- sp->spRect.drY0;
    }

