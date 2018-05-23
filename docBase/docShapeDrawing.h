/************************************************************************/
/*									*/
/*  Declarations for shapes.						*/
/*									*/
/*  Distances are in EMU's.						*/
/*  12700 EMU/Point.							*/
/*  635 EMU/Twip.							*/
/*  914400 EMU/Inch.							*/
/*  360000 EMU/cm.							*/
/*									*/
/*  With the exception of 'shplid', the ShapeProperties that come from	*/
/*  regular rtf only apply for the top level 'shp' or shpgrp'.		*/
/*									*/
/************************************************************************/

#   ifndef	DOC_SHAPE_DRAWING_H
#   define	DOC_SHAPE_DRAWING_H

#   include	<utilColor.h>
#   include	<geo2DInteger.h>

#   define	EMUtoTWIPS(e)	((e+317)/635)
#   define	TWIPStoEMU(t)	(635*(t))

typedef struct ShapePath
    {
    const char *	spName;
    short int		spXSize;
    short int		spYSize;
    short int		spClosed;
    short int		spVertexCount;
    Point2DI *	spVertices;
    } ShapePath;

/************************************************************************/
/*									*/
/*  A shape arrow description as it appears in the shape description.	*/
/*									*/
/************************************************************************/

typedef struct ShapeArrow
    {
    unsigned char	saArrowHead;
#				define	DSarrowNONE			0
#				define	DSarrowARROW			1
#				define	DSarrowSTEALTH_ARROW		2
#				define	DSarrowDIAMOND			3
#				define	DSarrowOVAL			4
#				define	DSarrowOPEN_ARROW		5
#				define	DSarrowCHEVRON_ARROW		7
#				define	DSarrow2CHEVRON_ARROW		8
    unsigned char	saArrowWidth;
#				define	DSarrowNARROW			0
#				define	DSarrowMEDIUM			1
#				define	DSarrowWIDE			2
    unsigned char	saArrowLength;
#				define	DSarrowSHORT			0
/*				define	DSarrowMEDIUM			1 */
#				define	DSarrowLONG			2
    } ShapeArrow;

typedef struct ShapeDrawing
    {
			/************************************************/
			/*  Position.					*/
			/************************************************/
    unsigned char	sdXPosition;
    unsigned char	sdXReference;
    unsigned char	sdYPosition;
    unsigned char	sdYReference;

    unsigned char	sd_fUseShapeAnchor;
    unsigned char	sd_fLayoutInCell;
    unsigned char	sd_fAllowOverlap;
    unsigned char	sd_fChangePage;

			/************************************************/
			/*  Object Type					*/
			/************************************************/
    int			sdShapeType;
    int			sdRotation;

    int			sdWrapDistLeftEmu;
    int			sdWrapDistTopEmu;
    int			sdWrapDistRightEmu;
    int			sdWrapDistBottomEmu;

    long		sdMasterShape;
    long		sdNextShape;

    int			sdXLimo;
    int			sdYLimo;

    unsigned int	sd_fIsBullet:1;
    unsigned int	sd_fFlipH:1;
    unsigned int	sd_fFlipV:1;
    unsigned int	sd_fBehindDocument:1;
    unsigned int	sd_fIsButton:1;
    unsigned int	sd_fHidden:1;
    unsigned int	sd_fReallyHidden:1;
    unsigned int	sd_fArrowheadsOK:1;
    unsigned int	sd_fBackground:1;
    unsigned int	sd_fDeleteAttachedObject:1;
    unsigned int	sd_fEditedWrap:1;
    unsigned int	sd_fHitTestFill:1;
    unsigned int	sd_fHitTestLine:1;
    unsigned int	sd_fInitiator:1;
    unsigned int	sd_fNoFillHitTest:1;
    unsigned int	sd_fNoHitTestPicture:1;
    unsigned int	sd_fNoLineDrawDash:1;
    unsigned int	sd_fOleIcon:1;
    unsigned int	sd_fOnDblClickNotify:1;
    unsigned int	sd_fOneD:1;
    unsigned int	sd_fPreferRelativeSize:1;
    unsigned int	sd_fPrint:1;

    unsigned int	sd_fPseudoInline:1;

			/************************************************/
			/*  Text box.					*/
			/************************************************/
    int			sd_dxTextLeft;
    int			sd_dyTextTop;
    int			sd_dxTextRight;
    int			sd_dyTextBottom;

    int			sd_scaleText;
    int			sd_lTxid;

			/**
			 *  Text wrapping:
			 */
#			define	SHAPEtextWRAP_SQUARE		0
#			define	SHAPEtextWRAP_TIGHT		1
#			define	SHAPEtextWRAP_NONE		2
#			define	SHAPEtextWRAP_TOP_BOTTOM	3
#			define	SHAPEtextWRAP_THROUGH		4
    unsigned char	sd_WrapText;
    unsigned char	sd_anchorText;
    unsigned char	sd_txflTextFlow;
    unsigned char	sd_cdirFont;

    unsigned int	sd_fAutoTextMargin:1;
    unsigned int	sd_fRotateText:1;
    unsigned int	sd_fSelectText:1;
    unsigned int	sd_fFitShapeToText:1;
    unsigned int	sd_fFitTextToShape:1;

			/************************************************/
			/*  Lock.					*/
			/************************************************/
    unsigned int	sd_fLockRotation:1;
    unsigned int	sd_fLockAspectRatio:1;
    unsigned int	sd_fLockAgainstSelect:1;
    unsigned int	sd_fLockCropping:1;
    unsigned int	sd_fLockVerticies:1;
    unsigned int	sd_fLockText:1;
    unsigned int	sd_fLockAdjustHandles:1;
    unsigned int	sd_fLockAgainstGrouping:1;
    unsigned int	sd_fLockShapeType:1;

    unsigned int	sd_fLockPosition:1;

			/************************************************/
			/*  Grouped Shapes				*/
			/*						*/
			/*  Relative position of children.		*/
			/*						*/
			/*  Position of groups. Actually the positions	*/
			/*  are given by shpleft,shpright,		*/
			/*  shptop,shpbottom in a root shape.		*/
			/************************************************/
    DocumentRectangle	sdRelRect;

    int			sdRelRotation;
    long		sdRegroupID;

    DocumentRectangle	sdGroupRect;

    unsigned int	sd_fRelChangePage:1;
    unsigned int	sd_fRelFlipH:1;
    unsigned int	sd_fRelFlipV:1;

			    /********************************************/
			    /*  Geometry				*/
			    /********************************************/
    DocumentRectangle	sdGeoRect;
    long		sdAdjustValue;
    long		sdAdjust2Value;
    long		sdAdjust3Value;
    long		sdAdjust4Value;
    long		sdAdjust5Value;
    long		sdAdjust6Value;
    long		sdAdjust7Value;
    long		sdAdjust8Value;
    long		sdAdjust9Value;
    long		sdAdjust10Value;

    Point2DI *	sdVertices;
    int			sdVertexCount;
    Point2DI *	sdWrapPolygonVertices;
    int			sdWrapPolygonVertexCount;
    int *		sdSegmentInfos;
    int			sdSegmentInfoCount;

    /*
    DSHPprop_pFragments,
    DSHPprop_pGuides,
    DSHPprop_pInscribe,
    DSHPprop_pAdjustHandles,
    */

			/************************************************/
			/*  Fill.					*/
			/************************************************/
    unsigned char	sdFillType;
#				define	DSfillSOLID			0
#				define	DSfillPATTERN			1
#				define	DSfillTEXTURE			2
#				define	DSfillCENTERED_PICTURE		3
#				define	DSfillSHADE_START_TO_END	4
#				define	DSfillSHADE_BOUNDS_TO_END	5
#				define	DSfillSHADE_OUTLINE_TO_END	6
#				define	DSfillSHADE_ANGLE		7
    unsigned char	sdFillblipflags;
    unsigned char	sdFillFocus;
    unsigned char	sdFillDztype;
    unsigned char	sdFillShadeType;

    int			sdFillOpacity;
    int			sdFillBackOpacity;

    int			sdFillWidthEmu;
    int			sdFillHeightEmu;
    int			sdFillAngle;

    int			sdFillToLeft;
    int			sdFillToTop;
    int			sdFillToRight;
    int			sdFillToBottom;

    int			sdFillOriginX;
    int			sdFillOriginY;
    int			sdFillShapeOriginX;
    int			sdFillShapeOriginY;

    int			sdFillRectLeftEmu;
    int			sdFillRectTopEmu;
    int			sdFillRectRightEmu;
    int			sdFillRectBottomEmu;

    long		sdFillShadePreset;

    RGB8Color		sdFillColor;
    RGB8Color		sdFillBackColor;
    RGB8Color		sdFillCrModColor;

    unsigned int	sd_fFilled:1;
    unsigned int	sd_fillShape:1;
    unsigned int	sd_fillUseRect:1;
    unsigned int	sd_fFillOK:1;
    unsigned int	sd_fFillShadeShapeOK:1;

			    /********************************************/
			    /*  Number/Flag: Line.			*/
			    /********************************************/
    unsigned char	sdLineType;
#				define	DSlineSOLID			0
#				define	DSlinePATTERN			1
#				define	DSlineTEXTURE			2
#				define	DSlinePICTURE			3

    unsigned char	sdLineFillBlipFlags;

    int			sdLineFillWidthEmu;
    int			sdLineFillHeightEmu;
    int			sdLineWidthEmu;

    unsigned char	sdLineStyle;
    unsigned char	sdLineDashing;
#				define	DSdashSOLID			0
#				define	DSdashDASHED			1
#				define	DSdashDOT			2
#				define	DSdashDASHDOT			3
#				define	DSdashDASHDOTDOT		4
#				define	DSdashDOT_X			6
#				define	DSdashDASHED_X			7
#				define	DSdashDASHED_L			8
#				define	DSdashDASHDOT_X			9
#				define	DSdashDASHDOT_L			10
#				define	DSdashDASHDOTDOT_L		11
#				define	DSdashHOLLOW			12/*do*/
    ShapeArrow		sdLineHeadArrow;
    ShapeArrow		sdLineTailArrow;
    unsigned char	sdLineEndCapStyle;
    unsigned char	sdLineFillDztype;
    unsigned char	sdLineJoinStyle;

    int			sdLineMiterLimit;

    RGB8Color		sdLineColor;
    RGB8Color		sdLineBackColor;
    RGB8Color		sdLineCrModColor;

    unsigned int	sd_fLine:1;
    unsigned int	sd_fLineOK:1;

    unsigned int	sd_fLineUseShapeAnchor:1;
    unsigned int	sd_fColumnLineOK:1;
    unsigned int	sd_fColumnLine:1;
    unsigned int	sd_fLeftLine:1;
    unsigned int	sd_fTopLine:1;
    unsigned int	sd_fRightLine:1;
    unsigned int	sd_fBottomLine:1;

    unsigned int	sd_fColumnHitTestLine:1;
    unsigned int	sd_fLeftHitTestLine:1;
    unsigned int	sd_fTopHitTestLine:1;
    unsigned int	sd_fRightHitTestLine:1;
    unsigned int	sd_fBottomHitTestLine:1;

    unsigned int	sd_lineFillShape:1;
    unsigned int	sd_lineColumnFillShape:1;
    unsigned int	sd_lineLeftFillShape:1;
    unsigned int	sd_lineTopFillShape:1;
    unsigned int	sd_lineRightFillShape:1;
    unsigned int	sd_lineBottomFillShape:1;

    unsigned int	sd_fInsetPen:1;
    unsigned int	sd_fLeftInsetPen:1;
    unsigned int	sd_fTopInsetPen:1;
    unsigned int	sd_fRightInsetPen:1;
    unsigned int	sd_fBottomInsetPen:1;

			/************************************************/
			/*  Shadow.					*/
			/************************************************/
    unsigned char	sdShadowType;
    int			sdShadowOpacity;
    int			sdShadowOffsetXEmu;
    int			sdShadowOffsetYEmu;
    int			sdShadowOffset2XEmu;
    int			sdShadowOffset2YEmu;
    int			sdShadowScaleXToX;
    int			sdShadowScaleYToX;
    int			sdShadowScaleXToY;
    int			sdShadowScaleYToY;
    int			sdShadowPerspectiveX;
    int			sdShadowPerspectiveY;
    int			sdShadowWeight;
    int			sdShadowOriginX;
    int			sdShadowOriginY;

    unsigned int	sd_fShadow:1;
    unsigned int	sd_fshadowObscured:1;
    unsigned int	sd_fshadowOK:1;

    RGB8Color		sdShadowColor;
    RGB8Color		sdShadowHighlightColor;
    RGB8Color		sdShadowCrModColor;
    RGB8Color		sdShadowc3DExtrusionColor;
    RGB8Color		sdShadowc3DCrModColor;
			/************************************************/
			/*  Connectors.					*/
			/************************************************/
    unsigned char	sdConnectionSite;
    unsigned char	sdConnectionStyle;

				/****************************************/

    int			sd_shapePath;

    int			sd_cropFromLeft;
    int			sd_cropFromRight;
    int			sd_cropFromTop;
    int			sd_cropFromBottom;
    } ShapeDrawing;

/************************************************************************/
/*									*/
/*  Kinds of shape.							*/
/*									*/
/************************************************************************/

typedef enum ShapeType
    {
    SHPtyFREE= -1,

    SHPtyFREEFORM_OR_NON_AUTOSHAPE= 0,
    SHPtyRECTANGLE= 1,
    SHPtyROUND_RECTANGLE= 2,
    SHPtyELLIPSE= 3,
    SHPtyDIAMOND= 4,
    SHPtyISOSCELES_TRIANGLE= 5,
    SHPtyRIGHT_TRIANGLE= 6,
    SHPtyPARALLELOGRAM= 7,
    SHPtyTRAPEZOID= 8,
    SHPtyHEXAGON= 9,
    SHPtyOCTAGON= 10,
    SHPtyPLUS_SIGN= 11,
    SHPtySTAR= 12,
    SHPtyARROW= 13,
    SHPtyTHICK_ARROW= 14,
    SHPtyHOME_PLATE= 15,
    SHPtyCUBE= 16,
    SHPtyBALLOON= 17,
    SHPtySEAL= 18,
    SHPtyARC= 19,
    SHPtyLINE= 20,
    SHPtyPLAQUE= 21,
    SHPtyCAN= 22,
    SHPtyDONUT= 23,
    SHPtyTEXT_SIMPLE= 24,
    SHPtyTEXT_OCTAGON= 25,
    SHPtyTEXT_HEXAGON= 26,
    SHPtyTEXT_CURVE= 27,
    SHPtyTEXT_WAVE= 28,
    SHPtyTEXT_RING= 29,
    SHPtyTEXT_ON_CURVE= 30,
    SHPtyTEXT_ON_RING= 31,
    SHPtyCALLOUT_1= 41,
    SHPtyCALLOUT_2= 42,
    SHPtyCALLOUT_3= 43,
    SHPtyACCENT_CALLOUT_1= 44,
    SHPtyACCENT_CALLOUT_2= 45,
    SHPtyACCENT_CALLOUT_3= 46,
    SHPtyBORDER_CALLOUT_1= 47,
    SHPtyBORDER_CALLOUT_2= 48,
    SHPtyBORDER_CALLOUT_3= 49,
    SHPtyACCENT_BORDER_CALLOUT_1= 50,
    SHPtyACCENT_BORDER_CALLOUT_2= 51,
    SHPtyACCENT_BORDER_CALLOUT_3= 52,
    SHPtyRIBBON= 53,
    SHPtyRIBBON2= 54,
    SHPtyCHEVRON= 55,
    SHPtyPENTAGON= 56,
    SHPtyNO_SMOKING= 57,
    SHPtySEAL8= 58,
    SHPtySEAL16= 59,
    SHPtySEAL32= 60,
    SHPtyWEDGE_RECT_CALLOUT= 61,
    SHPtyWEDGE_RRECT_CALLOUT= 62,
    SHPtyWEDGE_ELLIPSE_CALLOUT= 63,
    SHPtyWAVE= 64,
    SHPtyFOLDED_CORNER= 65,
    SHPtyLEFT_ARROW= 66,
    SHPtyDOWN_ARROW= 67,
    SHPtyUP_ARROW= 68,
    SHPtyLEFT_RIGHT_ARROW= 69,
    SHPtyUP_DOWN_ARROW= 70,
    SHPtyIRREGULARSEAL1= 71,
    SHPtyIRREGULARSEAL2= 72,
    SHPtyLIGHTNING_BOLT= 73,
    SHPtyHEART= 74,
    SHPtyPICTURE_FRAME= 75,
    SHPtyQUAD_ARROW= 76,
    SHPtyLEFT_ARROW_CALLOUT= 77,
    SHPtyRIGHT_ARROW_CALLOUT= 78,
    SHPtyUP_ARROW_CALLOUT= 79,
    SHPtyDOWN_ARROW_CALLOUT= 80,
    SHPtyLEFT_RIGHT_ARROW_CALLOUT= 81,
    SHPtyUP_DOWN_ARROW_CALLOUT= 82,
    SHPtyQUAD_ARROW_CALLOUT= 83,
    SHPtyBEVEL= 84,
    SHPtyLEFT_BRACKET= 85,
    SHPtyRIGHT_BRACKET= 86,
    SHPtyLEFT_BRACE= 87,
    SHPtyRIGHT_BRACE= 88,
    SHPtyLEFT_UP_ARROW= 89,
    SHPtyBENT_UP_ARROW= 90,
    SHPtyBENT_ARROW= 91,
    SHPtySEAL24= 92,
    SHPtySTRIPED_RIGHT_ARROW= 93,
    SHPtyNOTCHED_RIGHT_ARROW= 94,
    SHPtyBLOCK_ARC= 95,
    SHPtySMILEY_FACE= 96,
    SHPtyVERTICAL_SCROLL= 97,
    SHPtyHORIZONTAL_SCROLL= 98,
    SHPtyCIRCULAR_ARROW= 99,
    SHPtyNOTCHED_CIRCULAR_ARROW= 100,
    SHPtyUTURN_ARROW= 101,
    SHPtyCURVED_RIGHT_ARROW= 102,
    SHPtyCURVED_LEFT_ARROW= 103,
    SHPtyCURVED_UP_ARROW= 104,
    SHPtyCURVED_DOWN_ARROW= 105,
    SHPtyCLOUD_CALLOUT= 106,
    SHPtyELLIPSE_RIBBON= 107,
    SHPtyELLIPSE_RIBBON_2= 108,
    SHPtyFLOW_CHART_PROCESS= 109,
    SHPtyFLOW_CHART_DECISION= 110,
    SHPtyFLOW_CHART_INPUT_OUTPUT= 111,
    SHPtyFLOW_CHART_PREDEFINED_PROCESS= 112,
    SHPtyFLOW_CHART_INTERNAL_STORAGE= 113,
    SHPtyFLOW_CHART_DOCUMENT= 114,
    SHPtyFLOW_CHART_MULTIDOCUMENT= 115,
    SHPtyFLOW_CHART_TERMINATOR= 116,
    SHPtyFLOW_CHART_PREPARATION= 117,
    SHPtyFLOW_CHART_MANUAL_INPUT= 118,
    SHPtyFLOW_CHART_MANUAL_OPERATION= 119,
    SHPtyFLOW_CHART_CONNECTOR= 120,
    SHPtyFLOW_CHART_PUNCHED_CARD= 121,
    SHPtyFLOW_CHART_PUNCHED_TAPE= 122,
    SHPtyFLOW_CHART_SUMMING_JUNCTION= 123,
    SHPtyFLOW_CHART_OR= 124,
    SHPtyFLOW_CHART_COLLATE= 125,
    SHPtyFLOW_CHART_SORT= 126,
    SHPtyFLOW_CHART_EXTRACT= 127,
    SHPtyFLOW_CHART_MERGE= 128,
    SHPtyFLOW_CHART_OFFLINE_STORAGE= 129,
    SHPtyFLOW_CHART_ONLINE_STORAGE= 130,
    SHPtyFLOW_CHART_MAGNETIC_TAPE= 131,
    SHPtyFLOW_CHART_MAGNETIC_DISK= 132,
    SHPtyFLOW_CHART_MAGNETIC_DRUM= 133,
    SHPtyFLOW_CHART_DISPLAY= 134,
    SHPtyFLOW_CHART_DELAY= 135,
    SHPtyTEXT_PLAIN_TEXT= 136,
    SHPtyTEXT_STOP= 137,
    SHPtyTEXT_TRIANGLE= 138,
    SHPtyTEXT_TRIANGLE_INVERTED= 139,
    SHPtyTEXT_CHEVRON= 140,
    SHPtyTEXT_CHEVRON_INVERTED= 141,
    SHPtyTEXT_RING_INSIDE= 142,
    SHPtyTEXT_RING_OUTSIDE= 143,
    SHPtyTEXT_ARCH_UP_CURVE= 144,
    SHPtyTEXT_ARCH_DOWN_CURVE= 145,
    SHPtyTEXT_CIRCLE_CURVE= 146,
    SHPtyTEXT_BUTTON_CURVE= 147,
    SHPtyTEXT_ARCH_UP_POUR= 148,
    SHPtyTEXT_ARCH_DOWN_POUR= 149,
    SHPtyTEXT_CIRCLE_POUR= 150,
    SHPtyTEXT_BUTTON_POUR= 151,
    SHPtyTEXT_CURVE_UP= 152,
    SHPtyTEXT_CURVE_DOWN= 153,
    SHPtyTEXT_CASCADE_UP= 154,
    SHPtyTEXT_CASCADE_DOWN= 155,
    SHPtyTEXT_WAVE1= 156,
    SHPtyTEXT_WAVE2= 157,
    SHPtyTEXT_WAVE3= 158,
    SHPtyTEXT_WAVE4= 159,
    SHPtyTEXT_INFLATE= 160,
    SHPtyTEXT_DEFLATE= 161,
    SHPtyTEXT_INFLATE_BOTTOM= 162,
    SHPtyTEXT_DEFLATE_BOTTOM= 163,
    SHPtyTEXT_INFLATE_TOP= 164,
    SHPtyTEXT_DEFLATE_TOP= 165,
    SHPtyTEXT_DEFLATE_INFLATE= 166,
    SHPtyTEXT_DEFLATE_INFLATE_DEFLATE= 167,
    SHPtyTEXT_FADE_RIGHT= 168,
    SHPtyTEXT_FADE_LEFT= 169,
    SHPtyTEXT_FADE_UP= 170,
    SHPtyTEXT_FADE_DOWN= 171,
    SHPtyTEXT_SLANT_UP= 172,
    SHPtyTEXT_SLANT_DOWN= 173,
    SHPtyTEXT_CAN_UP= 174,
    SHPtyTEXT_CAN_DOWN= 175,
    SHPtyFLOW_CHART_ALTERNATE_PROCESS= 176,
    SHPtyFLOW_CHART_OFF_PAGE_CONNECTOR= 177,
    SHPtyCALLOUT_90= 178,
    SHPtyACCENT_CALLOUT_90= 179,
    SHPtyBORDER_CALLOUT_90= 180,
    SHPtyACCENT_BORDER_CALLOUT_90= 181,
    SHPtyLEFT_RIGHT_UP_ARROW= 182,
    SHPtySUN= 183,
    SHPtyMOON= 184,
    SHPtyBRACKET_PAIR= 185,
    SHPtyBRACE_PAIR= 186,
    SHPtySEAL4= 187,
    SHPtyDOUBLE_WAVE= 188,
    SHPtyHOST_CONTROL= 201,
    SHPtyTEXT_BOX= 202,

    SHPtyUNKNOWN,
    SHPtyGROUP,
    SHPtyPICPROP,

    SHPty_COUNT
    } ShapeType;

/************************************************************************/
/*									*/
/*  Descriptions of shapes.						*/
/*									*/
/************************************************************************/

extern const ShapePath	SP_33;
extern const ShapePath	SP_34;
extern const ShapePath	SP_LINE;
extern const ShapePath	SP_RECTANGLE;
extern const ShapePath	SP_DIAMOND;
extern const ShapePath	SP_ISOSCELES_TRIANGLE;
extern const ShapePath	SP_RIGHT_TRIANGLE;
extern const ShapePath	SP_PARALLELOGRAM;
extern const ShapePath	SP_TRAPEZOID;
extern const ShapePath	SP_HEXAGON;
extern const ShapePath	SP_OCTAGON;
extern const ShapePath	SP_PLUS_SIGN;
extern const ShapePath	SP_ARROW;
extern const ShapePath	SP_NOTCHED_RIGHT_ARROW;
extern const ShapePath	SP_HOME_PLATE;
extern const ShapePath	SP_CHEVRON;
extern const ShapePath	SP_LEFT_ARROW;
extern const ShapePath	SP_RIGHT_ARROW_CALLOUT;
extern const ShapePath	SP_LEFT_ARROW_CALLOUT;
extern const ShapePath	SP_UP_ARROW_CALLOUT;
extern const ShapePath	SP_LEFT_RIGHT_ARROW_CALLOUT;
extern const ShapePath	SP_UP_DOWN_ARROW_CALLOUT;
extern const ShapePath	SP_DOWN_ARROW_CALLOUT;
extern const ShapePath	SP_QUAD_ARROW_CALLOUT;
extern const ShapePath	SP_LEFT_RIGHT_ARROW;
extern const ShapePath	SP_UP_ARROW;
extern const ShapePath	SP_DOWN_ARROW;
extern const ShapePath	SP_UP_DOWN_ARROW;
extern const ShapePath	SP_QUAD_ARROW;
extern const ShapePath	SP_LEFT_RIGHT_UP_ARROW;
extern const ShapePath	SP_LEFT_UP_ARROW;
extern const ShapePath	SP_BENT_UP_ARROW;
extern const ShapePath	SP_PENTAGON;
extern const ShapePath	SP_FLOW_CHART_PUNCHED_CARD;
extern const ShapePath	SP_FLOW_CHART_MANUAL_INPUT;
extern const ShapePath	SP_FLOW_CHART_OFF_PAGE_CONNECTOR;
extern const ShapePath	SP_WEDGE_RECT_CALLOUT;
extern const ShapePath	SP_STAR;
extern const ShapePath	SP_SEAL4;
extern const ShapePath	SP_FLOW_CHART_MERGE;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docInitShapeArrow(		ShapeArrow *		sa );

extern void docInitShapeDrawing(	ShapeDrawing *		sd );

extern const char * docShapeTypeString(	int	shapeType );

extern void docInitShapeDrawingAllocated(	ShapeDrawing *	sd );

extern int docCopyShapeDrawing(	ShapeDrawing *			to,
				const ShapeDrawing *		from );

extern void docCleanShapeDrawing(	ShapeDrawing *		sd );

extern int docSetShapeDrawingProperty(	ShapeDrawing *		sd,
					int			prop,
					long			arg );

#   endif	/*  DOC_SHAPE_DRAWING_H	*/
