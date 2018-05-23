/************************************************************************/
/*									*/
/*  Positioned Objects and Frames					*/
/*									*/
/************************************************************************/

#   include	"docBaseConfig.h"

#   include	<appDebugon.h>

#   include	"docFrameProperties.h"

/************************************************************************/
/*									*/
/*  Manage text frame properties.					*/
/*									*/
/************************************************************************/

void docInitFrameProperties(	FrameProperties *	fp )
    {
    fp->fpWideTwips= 0;
    fp->fpHighTwips= 0;

    fp->fpXTwips= 0;
    fp->fpYTwips= 0;

    fp->fpDistanceFromTextTwipsLeft= 0;
    fp->fpDistanceFromTextTwipsRight= 0;
    fp->fpDistanceFromTextTwipsTop= 0;
    fp->fpDistanceFromTextTwipsBottom= 0;
    fp->fpDistanceFromTextSet= 0;

    fp->fpIsSet= 0;

    fp->fpXReference= FXrefCOLUMN;
    fp->fpXPosition= FXposXL;

    fp->fpYReference= FYrefMARGIN;
    fp->fpYPosition= FYposYT;
    fp->fpLockAnchor= 0;

    fp->fpFrameWrapStyle= FWSwrapWRAP;
    fp->fpNoOverlap= 0;
    fp->fpLockAnchor= 0;

    fp->fpTextFlowDirection= TXflowTXLRTB;

    return;
    }

const unsigned char docFrameYAllowed[FYpos_COUNT][FYref_COUNT]=
{
/*			FYrefMARGIN	FYrefPAGE	FYrefPARA	*/
/* FYposYGIVEN	*/ {	1,		1,		1,		},
/* FYposYT	*/ {	1,		1,		0,		},
/* FYposYC	*/ {	1,		1,		0,		},
/* FYposYB	*/ {	1,		1,		0,		},
/* FYposYIN	*/ {	1,		1,		0,		},
/* FYposYOUT	*/ {	1,		1,		0,		},
};

int docSetFrameProperty(	FrameProperties *	fp,
				int			prop,
				int			arg )
    {
    switch( prop )
	{
	case TFPpropABSW:
	    fp->fpWideTwips= arg;
	    fp->fpIsSet= 1;
	    break;

	case TFPpropABSH:
	    fp->fpHighTwips= arg;
	    fp->fpIsSet= 1;
	    break;

	case TFPpropHORIZONTAL_REF:
	    fp->fpXReference= arg;
	    fp->fpIsSet= 1;
	    break;

	case TFPpropHORIZONTAL_POS:
	    fp->fpXTwips= arg;
	    fp->fpXPosition= FXposXGIVEN;
	    fp->fpIsSet= 1;
	    break;

	case TFPpropHORIZONTAL_ALIGN:
	    fp->fpXPosition= arg;
	    fp->fpIsSet= 1;
	    break;

	case TFPpropVERTICAL_REF:
	    fp->fpYReference= arg;
	    fp->fpIsSet= 1;
	    break;

	case TFPpropVERTICAL_POS:
	    fp->fpYTwips= arg;
	    fp->fpYPosition= FYposYGIVEN;
	    fp->fpIsSet= 1;
	    break;

	case TFPpropVERTICAL_ALIGN:
	    fp->fpYPosition= arg;
	    fp->fpIsSet= 1;
	    break;

	case TFPpropLOCK_ANCHOR:
	    fp->fpLockAnchor= arg != 0;
	    fp->fpIsSet= 1;
	    break;

	case TFPpropWRAP_STYLE:
	    fp->fpFrameWrapStyle= arg;
	    fp->fpIsSet= 1;
	    break;

	case TFPpropNO_OVERLAP:
	    fp->fpNoOverlap= arg != 0;
	    fp->fpIsSet= 1;
	    break;

	case TFPprop_DFRMTXT:
	    fp->fpDistanceFromTextTwipsLeft= arg;
	    fp->fpDistanceFromTextTwipsRight= arg;
	    fp->fpDistanceFromTextTwipsTop= arg;
	    fp->fpDistanceFromTextTwipsBottom= arg;
	    fp->fpDistanceFromTextSet |=
			(DISTsetLEFT|DISTsetRIGHT|DISTsetTOP|DISTsetBOTTOM);
	    break;

	case TFPprop_DFRMTXTX:
	    fp->fpDistanceFromTextTwipsLeft= arg;
	    fp->fpDistanceFromTextTwipsRight= arg;
	    fp->fpDistanceFromTextSet |= (DISTsetLEFT|DISTsetRIGHT);
	    break;

	case TFPprop_DFRMTXTY:
	    fp->fpDistanceFromTextTwipsTop= arg;
	    fp->fpDistanceFromTextTwipsBottom= arg;
	    fp->fpDistanceFromTextSet |= (DISTsetTOP|DISTsetBOTTOM);
	    break;

	case TFPpropDFRMTXTL:
	    fp->fpDistanceFromTextTwipsLeft= arg;
	    fp->fpDistanceFromTextSet |= DISTsetLEFT;
	    break;

	case TFPpropDFRMTXTR:
	    fp->fpDistanceFromTextTwipsRight= arg;
	    fp->fpDistanceFromTextSet |= DISTsetRIGHT;
	    break;

	case TFPpropDFRMTXTT:
	    fp->fpDistanceFromTextTwipsTop= arg;
	    fp->fpDistanceFromTextSet |= DISTsetTOP;
	    break;

	case TFPpropDFRMTXTB:
	    fp->fpDistanceFromTextTwipsBottom= arg;
	    fp->fpDistanceFromTextSet |= DISTsetBOTTOM;
	    break;

	case TFPpropTEXT_FLOW:
	    fp->fpTextFlowDirection= arg;
	    break;

	default:
	    LDEB(prop); return -1;
	}

    return 0;
    }

int docGetFrameProperty(	const FrameProperties *	fp,
				int			prop )
    {
    switch( prop )
	{
	case TFPpropISSET:
	    return fp->fpIsSet;

	case TFPpropABSW:
	    return fp->fpWideTwips;

	case TFPpropABSH:
	    return fp->fpHighTwips;

	case TFPpropHORIZONTAL_REF:
	    return fp->fpXReference;

	case TFPpropHORIZONTAL_POS:
	    return fp->fpXTwips;

	case TFPpropHORIZONTAL_ALIGN:
	    return fp->fpXPosition;

	case TFPpropVERTICAL_REF:
	    return fp->fpYReference;

	case TFPpropVERTICAL_POS:
	    return fp->fpYTwips;

	case TFPpropVERTICAL_ALIGN:
	    return fp->fpYPosition;

	case TFPpropLOCK_ANCHOR:
	    return fp->fpLockAnchor;

	case TFPpropWRAP_STYLE:
	    return fp->fpFrameWrapStyle;

	case TFPpropNO_OVERLAP:
	    return fp->fpNoOverlap;

	case TFPpropDFRMTXTL:
	    return fp->fpDistanceFromTextTwipsLeft;

	case TFPpropDFRMTXTR:
	    return fp->fpDistanceFromTextTwipsRight;

	case TFPpropDFRMTXTT:
	    return fp->fpDistanceFromTextTwipsTop;

	case TFPpropDFRMTXTB:
	    return fp->fpDistanceFromTextTwipsBottom;

	case TFPpropTEXT_FLOW:
	    return fp->fpTextFlowDirection;

	default:
	    LDEB(prop); return -1;
	}
    }
