/************************************************************************/
/*									*/
/*  Cell Properties.							*/
/*									*/
/************************************************************************/

#   ifndef	DOC_CELL_PROPS_H
#   define	DOC_CELL_PROPS_H

#  include	"docItemShading.h"
#   include	"docDocumentAttributeMap.h"

/************************************************************************/
/*									*/
/*  Cell Properties.							*/
/*									*/
/************************************************************************/

typedef enum CellMergePos
    {
    CELLmergeNONE= 0,
    CELLmergeHEAD,
    CELLmergeFOLLOW,

    CELLmerge_COUNT
    } CellMergePos;

typedef struct CellProperties
    {
    int			cpRightBoundaryTwips;

    /**/
    int			cpTopBorderNumber;
    int			cpLeftBorderNumber;
    int			cpRightBorderNumber;
    int			cpBottomBorderNumber;

    int			cpShadingNumber;

    int			cpPreferredWidth;

    short int		cpTopPadding;
    short int		cpLeftPadding;
    short int		cpRightPadding;
    short int		cpBottomPadding;

			/* TableAutoFormatUnit */
    unsigned char	cpPreferredWidthUnit;

    unsigned char	cpTopPaddingUnit;
    unsigned char	cpBottomPaddingUnit;
    unsigned char	cpLeftPaddingUnit;
    unsigned char	cpRightPaddingUnit;

    unsigned char	cpHorizontalMerge;
    unsigned char	cpVerticalMerge;
    unsigned char	cpNoWrap;

    unsigned char	cpNoShading; /* clshdrawnil */

    unsigned char	cpValign;			/*  (enum)	*/
    unsigned char	cpTextFlow;			/*  (enum)	*/
    } CellProperties;

#   define CELL_MERGED( cp ) \
		( (cp)->cpHorizontalMerge == CELLmergeFOLLOW || \
		  (cp)->cpVerticalMerge == CELLmergeFOLLOW )

#   define	docCleanCellProperties( cp ) ( ( *(cp)= *(cp) ), 0 )

typedef enum CellProperty
    {
    CLprop_NONE= -1,

    CLpropCELLX= 0,

    CLpropTOP_BORDER,
    CLpropLEFT_BORDER,
    CLpropRIGHT_BORDER,
    CLpropBOTTOM_BORDER,

    CLpropSHADING,
    CLpropCLW_WIDTH,
    CLpropCLFTS_WIDTH,

    CLpropHOR_MERGE,
    CLpropVERT_MERGE,
    CLpropNO_WRAP,

    CLpropVALIGN,
    CLpropTEXT_FLOW,

    CLpropTOP_PADDING,
    CLpropBOTTOM_PADDING,
    CLpropLEFT_PADDING,
    CLpropRIGHT_PADDING,

    CLpropTOP_PADDING_UNIT,
    CLpropBOTTOM_PADDING_UNIT,
    CLpropLEFT_PADDING_UNIT,
    CLpropRIGHT_PADDING_UNIT,

    CLpropNO_SHADING,

    CLprop_COUNT
    } CellProperty;

/************************************************************************/
/*									*/
/*  Application of table styles.					*/
/*									*/
/************************************************************************/

typedef enum CellInstanceProperty
    {
    CIpropYTS= 0,

    CIpropFIRST_ROW,
    CIpropLAST_ROW,

    CIpropFIRST_COLUMN,
    CIpropLAST_COLUMN,

    CIpropODD_ROW_BAND,
    CIpropEVEN_ROW_BAND,

    CIpropODD_COLUMN_BAND,
    CIpropEVEN_COLUMN_BAND,

    CIpropNW_CELL,
    CIpropNE_CELL,
    CIpropSW_CELL,
    CIpropSE_CELL,

    CIprop_COUNT
    } CellInstanceProperty;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docInitCellProperties(	CellProperties *	cp );

extern int docUpdCellProperties(PropertyMask *			pCpDonePask,
				CellProperties *		cp,
				const PropertyMask *		cpSetMask,
				const CellProperties *		cpSet,
				const DocumentAttributeMap *	dam );

extern void docCellPropertyDifference(	
				PropertyMask *			pDifMask,
				const CellProperties *		cp1,
				const PropertyMask *		cpCmpMask,
				const CellProperties *		cp2,
				const DocumentAttributeMap *	dam );

extern int docCopyCellProperties(	
				CellProperties *		cpTo,
				const CellProperties *		cpFrom,
				const DocumentAttributeMap *	dam );

extern int docSetCellProperty(		CellProperties *	cp,
					int			prop,
					int			arg );

extern int docGetCellProperty(		const CellProperties *	cp,
					int			prop );

#   endif	/*  DOC_CELL_PROPS_H  */
