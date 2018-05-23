/************************************************************************/
/*									*/
/*  Shapes.. Administration.						*/
/*									*/
/************************************************************************/

#   include	"docBaseConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>

#   include	"docPropVal.h"
#   include	"docShapeProp.h"
#   include	"docShapeDrawing.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Initialize/Clean a shape arrow.					*/
/*									*/
/************************************************************************/

void docInitShapeArrow(	ShapeArrow *	sa )
    {
    sa->saArrowHead= 0;
    sa->saArrowWidth= 1;
    sa->saArrowLength= 1;
    }

/************************************************************************/
/*									*/
/*  Initialize/Clean a shape.						*/
/*									*/
/************************************************************************/

void docCleanShapeDrawing(	ShapeDrawing *		sd )
    {
    if  ( sd->sdVertices )
	{ free( sd->sdVertices ); }
    if  ( sd->sdWrapPolygonVertices )
	{ free( sd->sdWrapPolygonVertices ); }
    if  ( sd->sdSegmentInfos )
	{ free( sd->sdSegmentInfos ); }

    return;
    }

void docInitShapeDrawingAllocated(	ShapeDrawing *	sd )
    {
    sd->sdVertexCount= 0;
    sd->sdVertices= (Point2DI *)0;

    sd->sdWrapPolygonVertexCount= 0;
    sd->sdWrapPolygonVertices= (Point2DI *)0;

    sd->sdSegmentInfoCount= 0;
    sd->sdSegmentInfos= (int *)0;

    return;
    }

void docInitShapeDrawing(	ShapeDrawing *	sd )
    {
    sd->sdXPosition= 0;
    sd->sdXReference= FXrefCOLUMN;
    sd->sdYPosition= 0;
    sd->sdYReference= FYrefPARA;

    sd->sd_fUseShapeAnchor= 1;
    sd->sd_fLayoutInCell= 0;
    sd->sd_fAllowOverlap= 1;
    sd->sd_fChangePage= 0;

			/************************************************/
			/*  Object Type.				*/
			/************************************************/
    sd->sdShapeType= SHPtyFREE;
    sd->sdRotation= 0;

    sd->sdWrapDistLeftEmu= 114305;
    sd->sdWrapDistTopEmu= 0;
    sd->sdWrapDistRightEmu= 114305;
    sd->sdWrapDistBottomEmu= 0;

    sd->sdMasterShape= 0;
    sd->sdNextShape= 0;

    sd->sdXLimo= 0;
    sd->sdYLimo= 0;

    sd->sd_fIsBullet= 0;
    sd->sd_fFlipH= 0;
    sd->sd_fFlipV= 0;
    sd->sd_fBehindDocument= 0;
    sd->sd_fIsButton= 0;
    sd->sd_fHidden= 0;
    sd->sd_fReallyHidden= 0;
    sd->sd_fArrowheadsOK= 0;
    sd->sd_fBackground= 0;
    sd->sd_fDeleteAttachedObject= 0;
    sd->sd_fEditedWrap= 0;
    sd->sd_fHitTestFill= 1;
    sd->sd_fHitTestLine= 1;
    sd->sd_fInitiator= 0;
    sd->sd_fNoFillHitTest= 0;
    sd->sd_fNoHitTestPicture= 0;
    sd->sd_fNoLineDrawDash= 0;
    sd->sd_fOleIcon= 0;
    sd->sd_fOnDblClickNotify= 0;
    sd->sd_fOneD= 0;
    sd->sd_fPreferRelativeSize= 0;
    sd->sd_fPrint= 1;

    sd->sd_fPseudoInline= 0;

				/****************************************/
				/*  Text box.				*/
				/****************************************/
    sd->sd_dxTextLeft= 91440;
    sd->sd_dyTextTop= 45720;
    sd->sd_dxTextRight= 91440;
    sd->sd_dyTextBottom= 45720;

    sd->sd_scaleText= 0;
    sd->sd_lTxid= 0;

    sd->sd_WrapText= SHAPEtextWRAP_SQUARE;
    sd->sd_anchorText= 0;
    sd->sd_txflTextFlow= 0;
    sd->sd_cdirFont= 0;

    sd->sd_fAutoTextMargin= 0;
    sd->sd_fRotateText= 0;
    sd->sd_fSelectText= 1;
    sd->sd_fFitShapeToText= 0;
    sd->sd_fFitTextToShape= 0;

			/************************************************/
			/*  Lock.					*/
			/************************************************/
    sd->sd_fLockRotation= 0;
    sd->sd_fLockAspectRatio= 0;
    sd->sd_fLockAgainstSelect= 0;
    sd->sd_fLockCropping= 0;
    sd->sd_fLockVerticies= 0;
    sd->sd_fLockText= 0;
    sd->sd_fLockAdjustHandles= 0;
    sd->sd_fLockAgainstGrouping= 0;
    sd->sd_fLockShapeType= 0;

    sd->sd_fLockPosition= 0;

			/************************************************/
			/*  Fill.					*/
			/************************************************/
    sd->sdFillType= 0;
    sd->sdFillblipflags= 0;
    sd->sdFillFocus= 0;
    sd->sdFillDztype= 0;
    sd->sdFillShadeType= 0;

    sd->sdFillOpacity= 65536;
    sd->sdFillBackOpacity= 65536;

    sd->sdFillWidthEmu= 0;
    sd->sdFillHeightEmu= 0;
    sd->sdFillAngle= 0;

    sd->sdFillToLeft= 0;
    sd->sdFillToTop= 0;
    sd->sdFillToRight= 0;
    sd->sdFillToBottom= 0;

    sd->sdFillOriginX= 0;
    sd->sdFillOriginY= 0;
    sd->sdFillShapeOriginX= 0;
    sd->sdFillShapeOriginY= 0;

    sd->sdFillRectLeftEmu= 0;
    sd->sdFillRectTopEmu= 0;
    sd->sdFillRectRightEmu= 0;
    sd->sdFillRectBottomEmu= 0;

    sd->sdFillShadePreset= 0;

    utilInitRGB8Color( &(sd->sdFillColor) );
    utilInitRGB8Color( &(sd->sdFillBackColor) );
    utilInitRGB8Color( &(sd->sdFillCrModColor) );

    sd->sdFillColor.rgb8Red= 255;
    sd->sdFillColor.rgb8Green= 255;
    sd->sdFillColor.rgb8Blue= 255;

    sd->sdFillBackColor.rgb8Red= 255;
    sd->sdFillBackColor.rgb8Green= 255;
    sd->sdFillBackColor.rgb8Blue= 255;

    sd->sd_fFilled= 1;
    sd->sd_fillShape= 1;
    sd->sd_fillUseRect= 0;
    sd->sd_fFillOK= 1;
    sd->sd_fFillShadeShapeOK= 0;

			    /********************************************/
			    /*  Number/Flag: Line.			*/
			    /********************************************/
    sd->sdLineType= 0;

    sd->sdLineFillBlipFlags= 0;

    sd->sdLineFillWidthEmu= 0;
    sd->sdLineFillHeightEmu= 0;
    sd->sdLineWidthEmu= 9525;

    sd->sdLineStyle= 0;
    sd->sdLineDashing= 0;
    docInitShapeArrow( &(sd->sdLineHeadArrow) );
    docInitShapeArrow( &(sd->sdLineTailArrow) );
    sd->sdLineEndCapStyle= 2;
    sd->sdLineFillDztype= 0;
    sd->sdLineJoinStyle= 2;

    sd->sdLineMiterLimit= 524288;

    utilInitRGB8Color( &(sd->sdLineColor) );
    utilInitRGB8Color( &(sd->sdLineBackColor) );
    utilInitRGB8Color( &(sd->sdLineCrModColor) );

    sd->sd_fLine= 1;
    sd->sd_fLineOK= 1;

    sd->sd_fLineUseShapeAnchor= 0;
    sd->sd_fColumnLineOK= 0;
    sd->sd_fColumnLine= 0;
    sd->sd_fLeftLine= 0;
    sd->sd_fTopLine= 0;
    sd->sd_fRightLine= 0;
    sd->sd_fBottomLine= 0;

    sd->sd_fColumnHitTestLine= 0;
    sd->sd_fLeftHitTestLine= 0;
    sd->sd_fTopHitTestLine= 0;
    sd->sd_fRightHitTestLine= 0;
    sd->sd_fBottomHitTestLine= 0;

    sd->sd_lineFillShape= 0;
    sd->sd_lineColumnFillShape= 0;
    sd->sd_lineLeftFillShape= 0;
    sd->sd_lineTopFillShape= 0;
    sd->sd_lineRightFillShape= 0;
    sd->sd_lineBottomFillShape= 0;

    sd->sd_fInsetPen= 0;
    sd->sd_fLeftInsetPen= 0;
    sd->sd_fTopInsetPen= 0;
    sd->sd_fRightInsetPen= 0;
    sd->sd_fBottomInsetPen= 0;

				/****************************************/
				/*  Text box.				*/
				/****************************************/
    sd->sdGeoRect.drX0= 0;
    sd->sdGeoRect.drY0= 0;
    sd->sdGeoRect.drX1= 21600;
    sd->sdGeoRect.drY1= 21600;
    sd->sdAdjustValue= 0;
    sd->sdAdjust2Value= 0;
    sd->sdAdjust3Value= 0;
    sd->sdAdjust4Value= 0;
    sd->sdAdjust5Value= 0;
    sd->sdAdjust6Value= 0;
    sd->sdAdjust7Value= 0;
    sd->sdAdjust8Value= 0;
    sd->sdAdjust9Value= 0;
    sd->sdAdjust10Value= 0;

				/****************************************/
				/*  Grouped Shapes			*/
				/****************************************/
    sd->sdRelRect.drX0= 0;
    sd->sdRelRect.drY0= 0;
    sd->sdRelRect.drX1= 1;
    sd->sdRelRect.drY1= 1;

    sd->sdRelRotation= 0;
    sd->sdRegroupID= 0;

    sd->sdGroupRect.drX0= 0;
    sd->sdGroupRect.drY0= 0;
    sd->sdGroupRect.drX1= 20000;
    sd->sdGroupRect.drY1= 20000;

    sd->sd_fRelChangePage= 0;
    sd->sd_fRelFlipH= 0;
    sd->sd_fRelFlipV= 0;

			/************************************************/
			/*  Shadow.					*/
			/************************************************/
    sd->sdShadowType= 0;

    sd->sdShadowOpacity= 65536;
    sd->sdShadowOffsetXEmu= 0;
    sd->sdShadowOffsetYEmu= 0;
    sd->sdShadowOffset2XEmu= 25400;
    sd->sdShadowOffset2YEmu= 25400;
    sd->sdShadowScaleXToX= 65536;
    sd->sdShadowScaleYToX= 0;
    sd->sdShadowScaleXToY= 0;
    sd->sdShadowScaleYToY= 65536;
    sd->sdShadowPerspectiveX= 0;
    sd->sdShadowPerspectiveY= 0;
    sd->sdShadowWeight= 32768;
    sd->sdShadowOriginX= 0;
    sd->sdShadowOriginY= 0;

    sd->sd_fShadow= 0;
    sd->sd_fshadowObscured= 0;
    sd->sd_fshadowOK= 1;

    utilInitRGB8Color( &(sd->sdShadowColor) );
    utilInitRGB8Color( &(sd->sdShadowHighlightColor) );
    utilInitRGB8Color( &(sd->sdShadowCrModColor) );
    utilInitRGB8Color( &(sd->sdShadowc3DExtrusionColor) );
    utilInitRGB8Color( &(sd->sdShadowc3DCrModColor) );

    sd->sdShadowColor.rgb8Red= 128;
    sd->sdShadowColor.rgb8Green= 128;
    sd->sdShadowColor.rgb8Blue= 128;

    sd->sdShadowHighlightColor.rgb8Red= 203;
    sd->sdShadowHighlightColor.rgb8Green= 203;
    sd->sdShadowHighlightColor.rgb8Blue= 203;

			/************************************************/
			/*  Connectors.					*/
			/************************************************/
    sd->sdConnectionSite= 1;
    sd->sdConnectionStyle= 3;

			/************************************************/

    sd->sd_cropFromLeft= 0;
    sd->sd_cropFromRight= 0;
    sd->sd_cropFromTop= 0;
    sd->sd_cropFromBottom= 0;

    return;
    }

/************************************************************************/
/*									*/
/*  Copy a shape.							*/
/*									*/
/*  2)  Vertices.							*/
/*  2)  Wrap polygon.							*/
/*  4)  Segment information						*/
/*  5)  Text box content.						*/
/*									*/
/************************************************************************/

int docCopyShapeDrawing(	ShapeDrawing *		to,
				const ShapeDrawing *	from )
    {
    int				rval= 0;

    *to= *from;
    docInitShapeDrawingAllocated( to );

    /*  2  */
    to->sdVertices= (Point2DI *)malloc( from->sdVertexCount* sizeof(Point2DI) );
    if  ( ! to->sdVertices )
	{
	LXDEB(from->sdVertexCount,to->sdVertices);
	rval= -1; goto ready;
	}

    /*  3  */
    to->sdWrapPolygonVertices= (Point2DI *)malloc(
		    from->sdWrapPolygonVertexCount* sizeof(Point2DI) );
    if  ( ! to->sdWrapPolygonVertices )
	{
	LXDEB(from->sdWrapPolygonVertexCount,to->sdWrapPolygonVertices);
	rval= -1; goto ready;
	}

    /*  4  */
    to->sdSegmentInfos= (int *)malloc( from->sdSegmentInfoCount* sizeof(int) );
    if  ( ! to->sdSegmentInfos )
	{
	LXDEB(from->sdSegmentInfoCount,to->sdSegmentInfos);
	rval= -1; goto ready;
	}

    /*  2  */
    while( to->sdVertexCount < from->sdVertexCount )
	{
	to->sdVertices[to->sdVertexCount]= from->sdVertices[to->sdVertexCount];
	to->sdVertexCount++;
	}

    /*  3  */
    while( to->sdWrapPolygonVertexCount < from->sdWrapPolygonVertexCount )
	{
	to->sdWrapPolygonVertices[to->sdWrapPolygonVertexCount]=
		    from->sdWrapPolygonVertices[to->sdWrapPolygonVertexCount];
	to->sdWrapPolygonVertexCount++;
	}

    /*  4  */
    while( to->sdSegmentInfoCount < from->sdSegmentInfoCount )
	{
	to->sdSegmentInfos[to->sdSegmentInfoCount]=
			    from->sdSegmentInfos[to->sdSegmentInfoCount];
	to->sdSegmentInfoCount++;
	}

  ready:

    if  ( rval )
	{
	docCleanShapeDrawing( to );
	docInitShapeDrawingAllocated( to );
	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Translate shape type to string.					*/
/*									*/
/************************************************************************/

const char * docShapeTypeString(	int	shapeType )
    {
    static char		scratch[12];

    switch( shapeType )
	{
	case SHPtyFREEFORM_OR_NON_AUTOSHAPE: return "FREEFORM_OR_NON_AUTOSHAPE";
	case SHPtyRECTANGLE: return "RECTANGLE";
	case SHPtyROUND_RECTANGLE: return "ROUND_RECTANGLE";
	case SHPtyELLIPSE: return "ELLIPSE";
	case SHPtyDIAMOND: return "DIAMOND";
	case SHPtyISOSCELES_TRIANGLE: return "ISOSCELES_TRIANGLE";
	case SHPtyRIGHT_TRIANGLE: return "RIGHT_TRIANGLE";
	case SHPtyPARALLELOGRAM: return "PARALLELOGRAM";
	case SHPtyTRAPEZOID: return "TRAPEZOID";
	case SHPtyHEXAGON: return "HEXAGON";
	case SHPtyOCTAGON: return "OCTAGON";
	case SHPtyPLUS_SIGN: return "PLUS_SIGN";
	case SHPtySTAR: return "STAR";
	case SHPtyARROW: return "ARROW";
	case SHPtyTHICK_ARROW: return "THICK_ARROW";
	case SHPtyHOME_PLATE: return "HOME_PLATE";
	case SHPtyCUBE: return "CUBE";
	case SHPtyBALLOON: return "BALLOON";
	case SHPtySEAL: return "SEAL";
	case SHPtyARC: return "ARC";
	case SHPtyLINE: return "LINE";
	case SHPtyPLAQUE: return "PLAQUE";
	case SHPtyCAN: return "CAN";
	case SHPtyDONUT: return "DONUT";
	case SHPtyTEXT_SIMPLE: return "TEXT_SIMPLE";
	case SHPtyTEXT_OCTAGON: return "TEXT_OCTAGON";
	case SHPtyTEXT_HEXAGON: return "TEXT_HEXAGON";
	case SHPtyTEXT_CURVE: return "TEXT_CURVE";
	case SHPtyTEXT_WAVE: return "TEXT_WAVE";
	case SHPtyTEXT_RING: return "TEXT_RING";
	case SHPtyTEXT_ON_CURVE: return "TEXT_ON_CURVE";
	case SHPtyTEXT_ON_RING: return "TEXT_ON_RING";
	case SHPtyCALLOUT_1: return "CALLOUT_1";
	case SHPtyCALLOUT_2: return "CALLOUT_2";
	case SHPtyCALLOUT_3: return "CALLOUT_3";
	case SHPtyACCENT_CALLOUT_1: return "ACCENT_CALLOUT_1";
	case SHPtyACCENT_CALLOUT_2: return "ACCENT_CALLOUT_2";
	case SHPtyACCENT_CALLOUT_3: return "ACCENT_CALLOUT_3";
	case SHPtyBORDER_CALLOUT_1: return "BORDER_CALLOUT_1";
	case SHPtyBORDER_CALLOUT_2: return "BORDER_CALLOUT_2";
	case SHPtyBORDER_CALLOUT_3: return "BORDER_CALLOUT_3";
	case SHPtyACCENT_BORDER_CALLOUT_1: return "ACCENT_BORDER_CALLOUT_1";
	case SHPtyACCENT_BORDER_CALLOUT_2: return "ACCENT_BORDER_CALLOUT_2";
	case SHPtyACCENT_BORDER_CALLOUT_3: return "ACCENT_BORDER_CALLOUT_3";
	case SHPtyRIBBON: return "RIBBON";
	case SHPtyRIBBON2: return "RIBBON2";
	case SHPtyCHEVRON: return "CHEVRON";
	case SHPtyPENTAGON: return "PENTAGON";
	case SHPtyNO_SMOKING: return "NO_SMOKING";
	case SHPtySEAL8: return "SEAL8";
	case SHPtySEAL16: return "SEAL16";
	case SHPtySEAL32: return "SEAL32";
	case SHPtyWEDGE_RECT_CALLOUT: return "WEDGE_RECT_CALLOUT";
	case SHPtyWEDGE_RRECT_CALLOUT: return "WEDGE_RRECT_CALLOUT";
	case SHPtyWEDGE_ELLIPSE_CALLOUT: return "WEDGE_ELLIPSE_CALLOUT";
	case SHPtyWAVE: return "WAVE";
	case SHPtyFOLDED_CORNER: return "FOLDED_CORNER";
	case SHPtyLEFT_ARROW: return "LEFT_ARROW";
	case SHPtyDOWN_ARROW: return "DOWN_ARROW";
	case SHPtyUP_ARROW: return "UP_ARROW";
	case SHPtyLEFT_RIGHT_ARROW: return "LEFT_RIGHT_ARROW";
	case SHPtyUP_DOWN_ARROW: return "UP_DOWN_ARROW";
	case SHPtyIRREGULARSEAL1: return "IRREGULARSEAL1";
	case SHPtyIRREGULARSEAL2: return "IRREGULARSEAL2";
	case SHPtyLIGHTNING_BOLT: return "LIGHTNING_BOLT";
	case SHPtyHEART: return "HEART";
	case SHPtyPICTURE_FRAME: return "PICTURE_FRAME";
	case SHPtyQUAD_ARROW: return "QUAD_ARROW";
	case SHPtyLEFT_ARROW_CALLOUT: return "LEFT_ARROW_CALLOUT";
	case SHPtyRIGHT_ARROW_CALLOUT: return "RIGHT_ARROW_CALLOUT";
	case SHPtyUP_ARROW_CALLOUT: return "UP_ARROW_CALLOUT";
	case SHPtyDOWN_ARROW_CALLOUT: return "DOWN_ARROW_CALLOUT";
	case SHPtyLEFT_RIGHT_ARROW_CALLOUT: return "LEFT_RIGHT_ARROW_CALLOUT";
	case SHPtyUP_DOWN_ARROW_CALLOUT: return "UP_DOWN_ARROW_CALLOUT";
	case SHPtyQUAD_ARROW_CALLOUT: return "QUAD_ARROW_CALLOUT";
	case SHPtyBEVEL: return "BEVEL";
	case SHPtyLEFT_BRACKET: return "LEFT_BRACKET";
	case SHPtyRIGHT_BRACKET: return "RIGHT_BRACKET";
	case SHPtyLEFT_BRACE: return "LEFT_BRACE";
	case SHPtyRIGHT_BRACE: return "RIGHT_BRACE";
	case SHPtyLEFT_UP_ARROW: return "LEFT_UP_ARROW";
	case SHPtyBENT_UP_ARROW: return "BENT_UP_ARROW";
	case SHPtyBENT_ARROW: return "BENT_ARROW";
	case SHPtySEAL24: return "SEAL24";
	case SHPtySTRIPED_RIGHT_ARROW: return "STRIPED_RIGHT_ARROW";
	case SHPtyNOTCHED_RIGHT_ARROW: return "NOTCHED_RIGHT_ARROW";
	case SHPtyBLOCK_ARC: return "BLOCK_ARC";
	case SHPtySMILEY_FACE: return "SMILEY_FACE";
	case SHPtyVERTICAL_SCROLL: return "VERTICAL_SCROLL";
	case SHPtyHORIZONTAL_SCROLL: return "HORIZONTAL_SCROLL";
	case SHPtyCIRCULAR_ARROW: return "CIRCULAR_ARROW";
	case SHPtyNOTCHED_CIRCULAR_ARROW: return "NOTCHED_CIRCULAR_ARROW";
	case SHPtyUTURN_ARROW: return "UTURN_ARROW";
	case SHPtyCURVED_RIGHT_ARROW: return "CURVED_RIGHT_ARROW";
	case SHPtyCURVED_LEFT_ARROW: return "CURVED_LEFT_ARROW";
	case SHPtyCURVED_UP_ARROW: return "CURVED_UP_ARROW";
	case SHPtyCURVED_DOWN_ARROW: return "CURVED_DOWN_ARROW";
	case SHPtyCLOUD_CALLOUT: return "CLOUD_CALLOUT";
	case SHPtyELLIPSE_RIBBON: return "ELLIPSE_RIBBON";
	case SHPtyELLIPSE_RIBBON_2: return "ELLIPSE_RIBBON_2";
	case SHPtyFLOW_CHART_PROCESS: return "FLOW_CHART_PROCESS";
	case SHPtyFLOW_CHART_DECISION: return "FLOW_CHART_DECISION";
	case SHPtyFLOW_CHART_INPUT_OUTPUT: return "FLOW_CHART_INPUT_OUTPUT";
	case SHPtyFLOW_CHART_PREDEFINED_PROCESS:
				    return "FLOW_CHART_PREDEFINED_PROCESS";
	case SHPtyFLOW_CHART_INTERNAL_STORAGE:
				    return "FLOW_CHART_INTERNAL_STORAGE";
	case SHPtyFLOW_CHART_DOCUMENT: return "FLOW_CHART_DOCUMENT";
	case SHPtyFLOW_CHART_MULTIDOCUMENT: return "FLOW_CHART_MULTIDOCUMENT";
	case SHPtyFLOW_CHART_TERMINATOR: return "FLOW_CHART_TERMINATOR";
	case SHPtyFLOW_CHART_PREPARATION: return "FLOW_CHART_PREPARATION";
	case SHPtyFLOW_CHART_MANUAL_INPUT: return "FLOW_CHART_MANUAL_INPUT";
	case SHPtyFLOW_CHART_MANUAL_OPERATION:
				    return "FLOW_CHART_MANUAL_OPERATION";
	case SHPtyFLOW_CHART_CONNECTOR: return "FLOW_CHART_CONNECTOR";
	case SHPtyFLOW_CHART_PUNCHED_CARD: return "FLOW_CHART_PUNCHED_CARD";
	case SHPtyFLOW_CHART_PUNCHED_TAPE: return "FLOW_CHART_PUNCHED_TAPE";
	case SHPtyFLOW_CHART_SUMMING_JUNCTION:
				    return "FLOW_CHART_SUMMING_JUNCTION";
	case SHPtyFLOW_CHART_OR: return "FLOW_CHART_OR";
	case SHPtyFLOW_CHART_COLLATE: return "FLOW_CHART_COLLATE";
	case SHPtyFLOW_CHART_SORT: return "FLOW_CHART_SORT";
	case SHPtyFLOW_CHART_EXTRACT: return "FLOW_CHART_EXTRACT";
	case SHPtyFLOW_CHART_MERGE: return "FLOW_CHART_MERGE";
	case SHPtyFLOW_CHART_OFFLINE_STORAGE:
					return "FLOW_CHART_OFFLINE_STORAGE";
	case SHPtyFLOW_CHART_ONLINE_STORAGE: return "FLOW_CHART_ONLINE_STORAGE";
	case SHPtyFLOW_CHART_MAGNETIC_TAPE: return "FLOW_CHART_MAGNETIC_TAPE";
	case SHPtyFLOW_CHART_MAGNETIC_DISK: return "FLOW_CHART_MAGNETIC_DISK";
	case SHPtyFLOW_CHART_MAGNETIC_DRUM: return "FLOW_CHART_MAGNETIC_DRUM";
	case SHPtyFLOW_CHART_DISPLAY: return "FLOW_CHART_DISPLAY";
	case SHPtyFLOW_CHART_DELAY: return "FLOW_CHART_DELAY";
	case SHPtyTEXT_PLAIN_TEXT: return "TEXT_PLAIN_TEXT";
	case SHPtyTEXT_STOP: return "TEXT_STOP";
	case SHPtyTEXT_TRIANGLE: return "TEXT_TRIANGLE";
	case SHPtyTEXT_TRIANGLE_INVERTED: return "TEXT_TRIANGLE_INVERTED";
	case SHPtyTEXT_CHEVRON: return "TEXT_CHEVRON";
	case SHPtyTEXT_CHEVRON_INVERTED: return "TEXT_CHEVRON_INVERTED";
	case SHPtyTEXT_RING_INSIDE: return "TEXT_RING_INSIDE";
	case SHPtyTEXT_RING_OUTSIDE: return "TEXT_RING_OUTSIDE";
	case SHPtyTEXT_ARCH_UP_CURVE: return "TEXT_ARCH_UP_CURVE";
	case SHPtyTEXT_ARCH_DOWN_CURVE: return "TEXT_ARCH_DOWN_CURVE";
	case SHPtyTEXT_CIRCLE_CURVE: return "TEXT_CIRCLE_CURVE";
	case SHPtyTEXT_BUTTON_CURVE: return "TEXT_BUTTON_CURVE";
	case SHPtyTEXT_ARCH_UP_POUR: return "TEXT_ARCH_UP_POUR";
	case SHPtyTEXT_ARCH_DOWN_POUR: return "TEXT_ARCH_DOWN_POUR";
	case SHPtyTEXT_CIRCLE_POUR: return "TEXT_CIRCLE_POUR";
	case SHPtyTEXT_BUTTON_POUR: return "TEXT_BUTTON_POUR";
	case SHPtyTEXT_CURVE_UP: return "TEXT_CURVE_UP";
	case SHPtyTEXT_CURVE_DOWN: return "TEXT_CURVE_DOWN";
	case SHPtyTEXT_CASCADE_UP: return "TEXT_CASCADE_UP";
	case SHPtyTEXT_CASCADE_DOWN: return "TEXT_CASCADE_DOWN";
	case SHPtyTEXT_WAVE1: return "TEXT_WAVE1";
	case SHPtyTEXT_WAVE2: return "TEXT_WAVE2";
	case SHPtyTEXT_WAVE3: return "TEXT_WAVE3";
	case SHPtyTEXT_WAVE4: return "TEXT_WAVE4";
	case SHPtyTEXT_INFLATE: return "TEXT_INFLATE";
	case SHPtyTEXT_DEFLATE: return "TEXT_DEFLATE";
	case SHPtyTEXT_INFLATE_BOTTOM: return "TEXT_INFLATE_BOTTOM";
	case SHPtyTEXT_DEFLATE_BOTTOM: return "TEXT_DEFLATE_BOTTOM";
	case SHPtyTEXT_INFLATE_TOP: return "TEXT_INFLATE_TOP";
	case SHPtyTEXT_DEFLATE_TOP: return "TEXT_DEFLATE_TOP";
	case SHPtyTEXT_DEFLATE_INFLATE: return "TEXT_DEFLATE_INFLATE";
	case SHPtyTEXT_DEFLATE_INFLATE_DEFLATE:
					return "TEXT_DEFLATE_INFLATE_DEFLATE";
	case SHPtyTEXT_FADE_RIGHT: return "TEXT_FADE_RIGHT";
	case SHPtyTEXT_FADE_LEFT: return "TEXT_FADE_LEFT";
	case SHPtyTEXT_FADE_UP: return "TEXT_FADE_UP";
	case SHPtyTEXT_FADE_DOWN: return "TEXT_FADE_DOWN";
	case SHPtyTEXT_SLANT_UP: return "TEXT_SLANT_UP";
	case SHPtyTEXT_SLANT_DOWN: return "TEXT_SLANT_DOWN";
	case SHPtyTEXT_CAN_UP: return "TEXT_CAN_UP";
	case SHPtyTEXT_CAN_DOWN: return "TEXT_CAN_DOWN";
	case SHPtyFLOW_CHART_ALTERNATE_PROCESS:
					return "FLOW_CHART_ALTERNATE_PROCESS";
	case SHPtyFLOW_CHART_OFF_PAGE_CONNECTOR:
					return "FLOW_CHART_OFF_PAGE_CONNECTOR";
	case SHPtyCALLOUT_90: return "CALLOUT_90";
	case SHPtyACCENT_CALLOUT_90: return "ACCENT_CALLOUT_90";
	case SHPtyBORDER_CALLOUT_90: return "BORDER_CALLOUT_90";
	case SHPtyACCENT_BORDER_CALLOUT_90: return "ACCENT_BORDER_CALLOUT_90";
	case SHPtyLEFT_RIGHT_UP_ARROW: return "LEFT_RIGHT_UP_ARROW";
	case SHPtySUN: return "SUN";
	case SHPtyMOON: return "MOON";
	case SHPtyBRACKET_PAIR: return "BRACKET_PAIR";
	case SHPtyBRACE_PAIR: return "BRACE_PAIR";
	case SHPtySEAL4: return "SEAL4";
	case SHPtyDOUBLE_WAVE: return "DOUBLE_WAVE";
	case SHPtyHOST_CONTROL: return "HOST_CONTROL";
	case SHPtyTEXT_BOX: return "TEXT_BOX";

	case SHPtyUNKNOWN: return "UNKNOWN";
	case SHPtyGROUP: return "GROUP";
	case SHPtyPICPROP: return "PICPROP";

	default:
	    sprintf( scratch, "SHPty%d", shapeType );
	    return scratch;
	}
    }

int docSetShapeDrawingProperty(	ShapeDrawing *		sd,
				int			prop,
				long			val )
    {
    switch( prop )
	{
	case DSHPprop_posh:
	    sd->sdXPosition= val;
	    break;
	case DSHPprop_posrelh:
	    sd->sdXReference= val;
	    break;
	case DSHPprop_posv:
	    sd->sdYPosition= val;
	    break;
	case DSHPprop_posrelv:
	    sd->sdYReference= val;
	    break;

	case DSHPprop_fUseShapeAnchor:
	    sd->sd_fUseShapeAnchor= val != 0;
	    break;
	case DSHPprop_fLayoutInCell:
	    sd->sd_fLayoutInCell= val != 0;
	    break;
	case DSHPprop_fAllowOverlap:
	    sd->sd_fAllowOverlap= val != 0;
	    break;
	case DSHPprop_fChangePage:
	    sd->sd_fChangePage= val != 0;
	    break;

	case DSHPprop_shapeType:
	    sd->sdShapeType= val;
	    break;

	case DSHPprop_rotation:
	    sd->sdRotation= val;
	    break;

	case DSHPprop_dxWrapDistLeft:
	    sd->sdWrapDistLeftEmu= val;
	    break;
	case DSHPprop_dyWrapDistTop:
	    sd->sdWrapDistTopEmu= val;
	    break;
	case DSHPprop_dxWrapDistRight:
	    sd->sdWrapDistRightEmu= val;
	    break;
	case DSHPprop_dyWrapDistBottom:
	    sd->sdWrapDistBottomEmu= val;
	    break;

	case DSHPprop_hspMaster:
	    sd->sdMasterShape= val;
	    break;
	case DSHPprop_hspNext:
	    sd->sdNextShape= val;
	    break;

	case DSHPprop_xLimo:
	    sd->sdXLimo= val;
	    break;
	case DSHPprop_yLimo:
	    sd->sdXLimo= val;
	    break;

	case DSHPprop_fIsBullet:
	    sd->sd_fIsBullet= val != 0;
	    break;
	case DSHPprop_fFlipV:
	    sd->sd_fFlipV= val != 0;
	    break;
	case DSHPprop_fFlipH:
	    sd->sd_fFlipH= val != 0;
	    break;
	case DSHPprop_fBehindDocument:
	    sd->sd_fBehindDocument= val != 0;
	    break;
	case DSHPprop_fIsButton:
	    sd->sd_fIsButton= val != 0;
	    break;
	case DSHPprop_fHidden:
	    sd->sd_fHidden= val != 0;
	    break;
	case DSHPprop_fReallyHidden:
	    sd->sd_fReallyHidden= val != 0;
	    break;
	case DSHPprop_fArrowheadsOK:
	    sd->sd_fArrowheadsOK= val != 0;
	    break;
	case DSHPprop_fBackground:
	    sd->sd_fBackground= val != 0;
	    break;
	case DSHPprop_fDeleteAttachedObject:
	    sd->sd_fDeleteAttachedObject= val != 0;
	    break;
	case DSHPprop_fEditedWrap:
	    sd->sd_fEditedWrap= val != 0;
	    break;
	case DSHPprop_fHitTestFill:
	    sd->sd_fHitTestFill= val != 0;
	    break;
	case DSHPprop_fHitTestLine:
	    sd->sd_fHitTestLine= val != 0;
	    break;
	case DSHPprop_fInitiator:
	    sd->sd_fInitiator= val != 0;
	    break;
	case DSHPprop_fNoFillHitTest:
	    sd->sd_fNoFillHitTest= val != 0;
	    break;
	case DSHPprop_fNoHitTestPicture:
	    sd->sd_fNoHitTestPicture= val != 0;
	    break;
	case DSHPprop_fNoLineDrawDash:
	    sd->sd_fNoLineDrawDash= val != 0;
	    break;
	case DSHPprop_fOleIcon:
	    sd->sd_fOleIcon= val != 0;
	    break;
	case DSHPprop_fOnDblClickNotify:
	    sd->sd_fOnDblClickNotify= val != 0;
	    break;
	case DSHPprop_fOneD:
	    sd->sd_fOneD= val != 0;
	    break;
	case DSHPprop_fPreferRelativeSize:
	    sd->sd_fPreferRelativeSize= val != 0;
	    break;
	case DSHPprop_fPrint:
	    sd->sd_fPrint= val != 0;
	    break;

	case DSHPprop_fPseudoInline:
	    sd->sd_fPseudoInline= val != 0;
	    break;

	case DSHPprop_fLockRotation:
	    sd->sd_fLockRotation= val != 0;
	    break;
	case DSHPprop_fLockAspectRatio:
	    sd->sd_fLockAspectRatio= val != 0;
	    break;
	case DSHPprop_fLockAgainstSelect:
	    sd->sd_fLockAgainstSelect= val != 0;
	    break;
	case DSHPprop_fLockCropping:
	    sd->sd_fLockCropping= val != 0;
	    break;
	case DSHPprop_fLockVerticies:
	    sd->sd_fLockVerticies= val != 0;
	    break;
	case DSHPprop_fLockText:
	    sd->sd_fLockText= val != 0;
	    break;
	case DSHPprop_fLockAdjustHandles:
	    sd->sd_fLockAdjustHandles= val != 0;
	    break;
	case DSHPprop_fLockAgainstGrouping:
	    sd->sd_fLockAgainstGrouping= val != 0;
	    break;
	case DSHPprop_fLockShapeType:
	    sd->sd_fLockShapeType= val != 0;
	    break;

	case DSHPprop_fLockPosition:
	    sd->sd_fLockPosition= val != 0;
	    break;

	case DSHPprop_fillType:
	    sd->sdFillType= val;
	    break;
	case DSHPprop_fillOpacity:
	    sd->sdFillOpacity= val;
	    break;
	case DSHPprop_fillBackOpacity:
	    sd->sdFillBackOpacity= val;
	    break;

	case DSHPprop_fillblipflags:
	    sd->sdFillblipflags= val;
	    break;

	case DSHPprop_fillWidth:
	    sd->sdFillWidthEmu= val;
	    break;
	case DSHPprop_fillHeight:
	    sd->sdFillHeightEmu= val;
	    break;
	case DSHPprop_fillAngle:
	    sd->sdFillAngle= val;
	    break;
	case DSHPprop_fillFocus:
	    sd->sdFillFocus= val;
	    break;

	case DSHPprop_fillToLeft:
	    sd->sdFillToLeft= val;
	    break;
	case DSHPprop_fillToTop:
	    sd->sdFillToTop= val;
	    break;
	case DSHPprop_fillToRight:
	    sd->sdFillToRight= val;
	    break;
	case DSHPprop_fillToBottom:
	    sd->sdFillToBottom= val;
	    break;

	case DSHPprop_fillOriginX:
	    sd->sdFillOriginX= val;
	    break;
	case DSHPprop_fillOriginY:
	    sd->sdFillOriginY= val;
	    break;
	case DSHPprop_fillShapeOriginX:
	    sd->sdFillShapeOriginX= val;
	    break;
	case DSHPprop_fillShapeOriginY:
	    sd->sdFillShapeOriginY= val;
	    break;

	case DSHPprop_fillDztype:
	    sd->sdFillDztype= val;
	    break;

	case DSHPprop_fillRectLeft:
	    sd->sdFillRectLeftEmu= val;
	    break;
	case DSHPprop_fillRectTop:
	    sd->sdFillRectTopEmu= val;
	    break;
	case DSHPprop_fillRectRight:
	    sd->sdFillRectRightEmu= val;
	    break;
	case DSHPprop_fillRectBottom:
	    sd->sdFillRectBottomEmu= val;
	    break;

	case DSHPprop_fillShadePreset:
	    sd->sdFillShadePreset= val;
	    break;
	case DSHPprop_fillShadeType:
	    sd->sdFillShadeType= val;
	    break;

	case DSHPprop_fFilled:
	    sd->sd_fFilled= val != 0;
	    break;
	case DSHPprop_fillShape:
	    sd->sd_fillShape= val != 0;
	    break;
	case DSHPprop_fillUseRect:
	    sd->sd_fillUseRect= val != 0;
	    break;
	case DSHPprop_fFillOK:
	    sd->sd_fFillOK= val != 0;
	    break;
	case DSHPprop_fFillShadeShapeOK:
	    sd->sd_fFillShadeShapeOK= val != 0;
	    break;

	case DSHPprop_lineType:
	    sd->sdLineType= val;
	    break;

	case DSHPprop_lineFillBlipFlags:
	    sd->sdLineFillBlipFlags= val;
	    break;

	case DSHPprop_lineFillWidth:
	    sd->sdLineFillWidthEmu= val;
	    break;
	case DSHPprop_lineFillHeight:
	    sd->sdLineFillHeightEmu= val;
	    break;
	case DSHPprop_lineWidth:
	    sd->sdLineWidthEmu= val;
	    break;

	case DSHPprop_lineStyle:
	    sd->sdLineStyle= val;
	    break;
	case DSHPprop_lineDashing:
	    sd->sdLineDashing= val;
	    break;
	case DSHPprop_lineStartArrowhead:
	    sd->sdLineHeadArrow.saArrowHead= val;
	    break;
	case DSHPprop_lineEndArrowhead:
	    sd->sdLineTailArrow.saArrowHead= val;
	    break;
	case DSHPprop_lineStartArrowWidth:
	    sd->sdLineHeadArrow.saArrowWidth= val;
	    break;
	case DSHPprop_lineStartArrowLength:
	    sd->sdLineHeadArrow.saArrowLength= val;
	    break;
	case DSHPprop_lineEndArrowWidth:
	    sd->sdLineTailArrow.saArrowWidth= val;
	    break;
	case DSHPprop_lineEndArrowLength:
	    sd->sdLineTailArrow.saArrowLength= val;
	    break;
	case DSHPprop_lineEndCapStyle:
	    sd->sdLineEndCapStyle= val;
	    break;
	case DSHPprop_lineFillDztype:
	    sd->sdLineFillDztype= val;
	    break;
	case DSHPprop_lineJoinStyle:
	    sd->sdLineJoinStyle= val;
	    break;

	case DSHPprop_lineMiterLimit:
	    sd->sdLineMiterLimit= val;
	    break;

	case DSHPprop_fLine:
	    sd->sd_fLine= val != 0;
	    break;
	case DSHPprop_fLineOK:
	    sd->sd_fLineOK= val != 0;
	    break;

	case DSHPprop_fLineUseShapeAnchor:
	    sd->sd_fLineUseShapeAnchor= val != 0;
	    break;
	case DSHPprop_fColumnLineOK:
	    sd->sd_fColumnLineOK= val != 0;
	    break;
	case DSHPprop_fColumnLine:
	    sd->sd_fColumnLine= val != 0;
	    break;
	case DSHPprop_fLeftLine:
	    sd->sd_fLeftLine= val != 0;
	    break;
	case DSHPprop_fTopLine:
	    sd->sd_fTopLine= val != 0;
	    break;
	case DSHPprop_fRightLine:
	    sd->sd_fRightLine= val != 0;
	    break;
	case DSHPprop_fBottomLine:
	    sd->sd_fBottomLine= val != 0;
	    break;

	case DSHPprop_fColumnHitTestLine:
	    sd->sd_fColumnHitTestLine= val != 0;
	    break;
	case DSHPprop_fLeftHitTestLine:
	    sd->sd_fLeftHitTestLine= val != 0;
	    break;
	case DSHPprop_fTopHitTestLine:
	    sd->sd_fTopHitTestLine= val != 0;
	    break;
	case DSHPprop_fRightHitTestLine:
	    sd->sd_fRightHitTestLine= val != 0;
	    break;
	case DSHPprop_fBottomHitTestLine:
	    sd->sd_fBottomHitTestLine= val != 0;
	    break;

	case DSHPprop_lineFillShape:
	    sd->sd_lineFillShape= val != 0;
	    break;
	case DSHPprop_lineColumnFillShape:
	    sd->sd_lineColumnFillShape= val != 0;
	    break;
	case DSHPprop_lineLeftFillShape:
	    sd->sd_lineLeftFillShape= val != 0;
	    break;
	case DSHPprop_lineTopFillShape:
	    sd->sd_lineTopFillShape= val != 0;
	    break;
	case DSHPprop_lineRightFillShape:
	    sd->sd_lineRightFillShape= val != 0;
	    break;
	case DSHPprop_lineBottomFillShape:
	    sd->sd_lineBottomFillShape= val != 0;
	    break;

	case DSHPprop_fInsetPen:
	    sd->sd_fInsetPen= val != 0;
	    break;
	case DSHPprop_fLeftInsetPen:
	    sd->sd_fLeftInsetPen= val != 0;
	    break;
	case DSHPprop_fTopInsetPen:
	    sd->sd_fTopInsetPen= val != 0;
	    break;
	case DSHPprop_fRightInsetPen:
	    sd->sd_fRightInsetPen= val != 0;
	    break;
	case DSHPprop_fBottomInsetPen:
	    sd->sd_fBottomInsetPen= val != 0;
	    break;

	case DSHPprop_relLeft:
	    sd->sdRelRect.drX0= val;
	    break;
	case DSHPprop_relTop:
	    sd->sdRelRect.drY0= val;
	    break;
	case DSHPprop_relRight:
	    sd->sdRelRect.drX1= val;
	    break;
	case DSHPprop_relBottom:
	    sd->sdRelRect.drY1= val;
	    break;

	case DSHPprop_relRotation:
	    sd->sdRelRotation= val;
	    break;
	case DSHPprop_lidRegroup:
	    sd->sdRegroupID= val;
	    break;

	case DSHPprop_groupLeft:
	    sd->sdGroupRect.drX0= val;
	    break;
	case DSHPprop_groupTop:
	    sd->sdGroupRect.drY0= val;
	    break;
	case DSHPprop_groupRight:
	    sd->sdGroupRect.drX1= val;
	    break;
	case DSHPprop_groupBottom:
	    sd->sdGroupRect.drY1= val;
	    break;

	case DSHPprop_fRelChangePage:
	    sd->sd_fRelChangePage= val != 0;
	    break;
	case DSHPprop_fRelFlipH:
	    sd->sd_fRelFlipH= val != 0;
	    break;
	case DSHPprop_fRelFlipV:
	    sd->sd_fRelFlipV= val != 0;
	    break;

	case DSHPprop_shadowType:
	    sd->sdShadowType= val;
	    break;
	case DSHPprop_shadowOpacity:
	    sd->sdShadowOpacity= val;
	    break;

	case DSHPprop_shadowOffsetX:
	    sd->sdShadowOffsetXEmu= val;
	    break;
	case DSHPprop_shadowOffsetY:
	    sd->sdShadowOffsetYEmu= val;
	    break;

	case DSHPprop_shadowSecondOffsetX:
	    sd->sdShadowOffset2XEmu= val;
	    break;
	case DSHPprop_shadowSecondOffsetY:
	    sd->sdShadowOffset2YEmu= val;
	    break;

	case DSHPprop_shadowScaleXToX:
	    sd->sdShadowScaleXToX= val;
	    break;
	case DSHPprop_shadowScaleYToX:
	    sd->sdShadowScaleYToX= val;
	    break;
	case DSHPprop_shadowScaleXToY:
	    sd->sdShadowScaleXToY= val;
	    break;
	case DSHPprop_shadowScaleYToY:
	    sd->sdShadowScaleYToY= val;
	    break;

	case DSHPprop_shadowPerspectiveX:
	    sd->sdShadowPerspectiveX= val;
	    break;
	case DSHPprop_shadowPerspectiveY:
	    sd->sdShadowPerspectiveY= val;
	    break;

	case DSHPprop_shadowWeight:
	    sd->sdShadowWeight= val;
	    break;

	case DSHPprop_shadowOriginX:
	    sd->sdShadowOriginX= val;
	    break;
	case DSHPprop_shadowOriginY:
	    sd->sdShadowOriginY= val;
	    break;

	case DSHPprop_fShadow:
	    sd->sd_fShadow= val != 0;
	    break;
	case DSHPprop_fshadowObscured:
	    sd->sd_fshadowObscured= val != 0;
	    break;
	case DSHPprop_fShadowOK:
	    sd->sd_fshadowOK= val != 0;
	    break;

	case DSHPprop_cxk:
	    sd->sdConnectionSite= val;
	    break;
	case DSHPprop_cxstyle:
	    sd->sdConnectionStyle= val;
	    break;

	case DSHPprop_geoLeft:
	    sd->sdGeoRect.drX0= val;
	    break;
	case DSHPprop_geoTop:
	    sd->sdGeoRect.drY0= val;
	    break;
	case DSHPprop_geoRight:
	    sd->sdGeoRect.drX1= val;
	    break;
	case DSHPprop_geoBottom:
	    sd->sdGeoRect.drY1= val;
	    break;

	case DSHPprop_adjustValue:
	    sd->sdAdjustValue= val;
	    break;
	case DSHPprop_adjust2Value:
	    sd->sdAdjust2Value= val;
	    break;
	case DSHPprop_adjust3Value:
	    sd->sdAdjust3Value= val;
	    break;
	case DSHPprop_adjust4Value:
	    sd->sdAdjust4Value= val;
	    break;
	case DSHPprop_adjust5Value:
	    sd->sdAdjust5Value= val;
	    break;
	case DSHPprop_adjust6Value:
	    sd->sdAdjust6Value= val;
	    break;
	case DSHPprop_adjust7Value:
	    sd->sdAdjust7Value= val;
	    break;
	case DSHPprop_adjust8Value:
	    sd->sdAdjust8Value= val;
	    break;
	case DSHPprop_adjust9Value:
	    sd->sdAdjust9Value= val;
	    break;
	case DSHPprop_adjust10Value:
	    sd->sdAdjust10Value= val;
	    break;

	case DSHPprop_dxTextLeft:
	    sd->sd_dxTextLeft= val;
	    break;
	case DSHPprop_dyTextTop:
	    sd->sd_dyTextTop= val;
	    break;
	case DSHPprop_dxTextRight:
	    sd->sd_dxTextRight= val;
	    break;
	case DSHPprop_dyTextBottom:
	    sd->sd_dyTextBottom= val;
	    break;

	case DSHPprop_scaleText:
	    sd->sd_scaleText= val;
	    break;
	case DSHPprop_lTxid:
	    sd->sd_lTxid= val;
	    break;

	case DSHPprop_WrapText:
	    sd->sd_WrapText= val;
	    break;
	case DSHPprop_anchorText:
	    sd->sd_anchorText= val;
	    break;
	case DSHPprop_txflTextFlow:
	    sd->sd_txflTextFlow= val;
	    break;
	case DSHPprop_cdirFont:
	    sd->sd_cdirFont= val;
	    break;

	case DSHPprop_fAutoTextMargin:
	    sd->sd_fAutoTextMargin= val != 0;
	    break;
	case DSHPprop_fRotateText:
	    sd->sd_fRotateText= val != 0;
	    break;
	case DSHPprop_fSelectText:
	    sd->sd_fSelectText= val != 0;
	    break;
	case DSHPprop_fFitShapeToText:
	    sd->sd_fFitShapeToText= val != 0;
	    break;
	case DSHPprop_fFitTextToShape:
	    sd->sd_fFitTextToShape= val != 0;
	    break;

	case DSHPprop_shapePath:
	    sd->sd_shapePath= val;
	    break;

	case DSHPprop_pictureActive:
	case DSHPprop_pictureId:
	case DSHPprop_pictureContrast:
	case DSHPprop_pictureBrightness:
	case DSHPprop_pictureDblCrMod:
	case DSHPprop_pictureFillCrMod:
	case DSHPprop_pictureLineCrMod:
	    break;

	case DSHPprop_f3D:
	case DSHPprop_pibFlags:
	case DSHPprop_dhgt:
	case DSHPprop_sizerelh:
	case DSHPprop_sizerelv:
	case DSHPprop_pctHoriz:
	case DSHPprop_pictureGray:
	case DSHPprop_pictureBiLevel:
	case DSHPprop_fPreferRelativeResize:
	case DSHPprop_fScriptAnchor:
	case DSHPprop_fFakeMaster:
	case DSHPprop_fCameFromImgDummy:
	    break;

	case DSHPprop_fRecolorFillAsPicture:
	    break;

	case DSHPprop_cropFromLeft:
	    sd->sd_cropFromLeft= val;
	    break;
	case DSHPprop_cropFromRight:
	    sd->sd_cropFromRight= val;
	    break;
	case DSHPprop_cropFromTop:
	    sd->sd_cropFromTop= val;
	    break;
	case DSHPprop_cropFromBottom:
	    sd->sd_cropFromBottom= val;
	    break;

	default:
	    LDEB(prop); return -1;
	}

    return 0;
    }
