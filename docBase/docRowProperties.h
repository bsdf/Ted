/************************************************************************/
/*									*/
/*  Row Properties.							*/
/*									*/
/************************************************************************/

#   ifndef	DOC_ROW_PROPS_H
#   define	DOC_ROW_PROPS_H

#   include	"docCellProperties.h"
#   include	"docDocumentAttributeMap.h"

/************************************************************************/
/*									*/
/*  Row Properties.							*/
/*									*/
/************************************************************************/

typedef struct RowProperties
    {
    int			rpCellCount;
    CellProperties *	rpCells;

    int			rpHalfGapWidthTwips;
    int			rpLeftIndentTwips;
    int			rpHeightTwips;

    int			rpRowNumber;
    int			rpRowBandNumber;
    int			rpRowStyle;

			/************************************************/
			/*  Row borders are not stored in the word	*/
			/*  binary format, and seem to be irrelevant	*/
			/************************************************/
    int			rpTopBorderNumber;
    int			rpLeftBorderNumber;
    int			rpRightBorderNumber;
    int			rpBottomBorderNumber;
    int			rpVerticalBorderNumber;
    int			rpHorizontalBorderNumber;

    int			rpShadingNumber;
    int			rpFrameNumber;

    unsigned char	rpAlignment;

    unsigned char	rpIsTableHeader;
    unsigned char	rpKeepOnOnePage;
    unsigned char	rp_Keepfollow;	/*  derived property	*/
    unsigned char	rpAutofit;
			/**
			 * 0: Cells in this table row will have left-to-right
			 *	precedence (the default)
			 * 1: Cells in this table row will have right-to-left
			 *	precedence
			 */
    unsigned char	rpRToL;

    int			rpPreferredWidth;

    int			rpTopDefaultCellSpacing;
    int			rpLeftDefaultCellSpacing;
    int			rpRightDefaultCellSpacing;
    int			rpBottomDefaultCellSpacing;

    int			rpTopCellPadding;
    int			rpLeftCellPadding;
    int			rpRightCellPadding;
    int			rpBottomCellPadding;

    int			rpCellWidthBefore;
    int			rpCellWidthAfter;

    int			rpAuthor;

			/* TableAutoFormatUnit */
    unsigned char	rpPreferredWidthUnit;

			/* TableAutoFormatUnit */
    unsigned char	rpTopDefaultCellSpacingUnit;
    unsigned char	rpLeftDefaultCellSpacingUnit;
    unsigned char	rpRightDefaultCellSpacingUnit;
    unsigned char	rpBottomDefaultCellSpacingUnit;

			/* TableAutoFormatUnit */
    unsigned char	rpTopCellPaddingUnit;
    unsigned char	rpLeftCellPaddingUnit;
    unsigned char	rpRightCellPaddingUnit;
    unsigned char	rpBottomCellPaddingUnit;

			/* TableAutoFormatUnit */
    unsigned char	rpCellWidthBeforeUnit;
    unsigned char	rpCellWidthAfterUnit;

    unsigned char	rpAutoformatBorders;
    unsigned char	rpAutoformatShading;
    unsigned char	rpAutoformatFont;
    unsigned char	rpAutoformatColor;
    unsigned char	rpAutoformatApplyBestFit;
    unsigned char	rpAutoformatFirstRow;
    unsigned char	rpAutoformatLastRow;
    unsigned char	rpAutoformatFirstColumn;
    unsigned char	rpAutoformatLastColumn;

    unsigned char	rpIsLastRow;
    } RowProperties;

typedef enum RowProperty
    {
    RPprop_NONE= -1,

		/**
		 *  The number of cells and their x position (cellx)
		 */
    RPpropCELL_LAYOUT= 0,
		/**
		 *  Other cell properties such as borders and shading.
		 */
    RPpropCELL_PROPS,

    RPpropGAP_WIDTH,
    RPpropLEFT_INDENT,
    RPpropHEIGHT,

    RPpropTOP_BORDER,
    RPpropLEFT_BORDER,
    RPpropRIGHT_BORDER,
    RPpropBOTTOM_BORDER,
    RPpropVERT_BORDER,
    RPpropHORIZ_BORDER,

    RPpropSHADING,
    RPpropFRAME,

    RPpropALIGNMENT,
    RPpropIS_TABLE_HEADER,
    RPpropKEEP_ON_ONE_PAGE,
    RPpropAUTOFIT,
    RPpropRTOL,

    /**/
    RPpropTRW_WIDTH,
    RPpropTRFTS_WIDTH,

    /**/
    RPpropTRSPDT,
    RPpropTRSPDL,
    RPpropTRSPDR,
    RPpropTRSPDB,

    RPpropTRPADDT,
    RPpropTRPADDL,
    RPpropTRPADDR,
    RPpropTRPADDB,

    /**/
    RPpropTRSPDFT,
    RPpropTRSPDFL,
    RPpropTRSPDFR,
    RPpropTRSPDFB,

    RPpropTRPADDFT,
    RPpropTRPADDFL,
    RPpropTRPADDFR,
    RPpropTRPADDFB,

    /**/
    RPpropTRW_WIDTHB,
    RPpropTRW_WIDTHA,

    RPpropTRFTS_WIDTHB,
    RPpropTRFTS_WIDTHA,

    /**/
    RPpropAUTOFORMAT_BORDERS,
    RPpropAUTOFORMAT_SHADING,
    RPpropAUTOFORMAT_FONT,
    RPpropAUTOFORMAT_COLOR,
    RPpropAUTOFORMAT_APPLY_BEST_FIT,
    RPpropAUTOFORMAT_FIRST_ROW,
    RPpropAUTOFORMAT_LAST_ROW,
    RPpropAUTOFORMAT_FIRST_COLUMN,
    RPpropAUTOFORMAT_LAST_COLUMN,

    RPpropROW_NUMBER,
    RPpropROW_BAND_NUMBER,
    RPpropROW_STYLE,

    RPpropIS_LAST_ROW,
    RPpropTRAUTH,

    RPprop_COUNT,

    RPprop_IGNORED,		/*  For read/write/tool only	*/
    RPprop_KEEPFOLLOW,		/*  For read/write/tool only	*/

    RPprop_FULL_COUNT
    } RowProperty;

# define RP_IS_ONE_PAGE( rp ) ( \
		    (rp)->rpKeepOnOnePage	|| \
		    (rp)->rp_Keepfollow		|| \
		    (rp)->rpHeightTwips != 0	|| \
		    (rp)->rpIsTableHeader	)

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docInitRowProperties(	RowProperties *		rp );
extern void docCleanRowProperties(	RowProperties *		rp );

extern int docCopyRowProperties(	RowProperties *			to,
					const RowProperties *		from,
					const DocumentAttributeMap *	dam );

extern int docInsertRowColumn(	RowProperties *			rp,
				int				n,
				int				shiftTail,
				const CellProperties *		cp,
				const DocumentAttributeMap *	dam );

extern int docApproximatelyAlignedColumns(
				const RowProperties *	rp1,
				const RowProperties *	rp2 );

extern int docUpdRowProperties(	PropertyMask *			pRpDonePask,
				RowProperties *			rp,
				const PropertyMask *		rpSetMask,
				const RowProperties *		rpSet,
				const DocumentAttributeMap *	dam );

extern int docDeleteColumnsFromRow(
				RowProperties *			rp,
				int				col0,
				int				count,
				int				shiftTail );

extern void docRowPropertiesSetWidth(	RowProperties *		rp,
					int			col0,
					int			col1,
					int			wide,
					int			victim,
					int			victimWide );

extern int docSetRowProperty(		RowProperties *		rp,
					int			prop,
					int			arg );

extern int docGetRowProperty(		const RowProperties *	rp,
					int			prop );

extern int docEqualWidthColumns(	RowProperties *		rp,
					int			columns,
					int			wide,
					int			fsHalfPoints );

extern int docColumnWidth(		const RowProperties *	rp,
					int			col );
extern int docColumnLeft(		const RowProperties *	rp,
					int			col );
extern int docColumnRight(		const RowProperties *	rp,
					int			col );

extern int docRowPropertiesMakeColWider( RowProperties *	rp,
					int			col,
					int			wider );

extern void docRowPropertyDifference(
				PropertyMask *			pRpDifPask,
				const RowProperties *		rp1,
				const PropertyMask *		rpCmpMask,
				const RowProperties *		rp2,
				const DocumentAttributeMap *	dam );

extern int docCellRight(	int *			pColspan,
				const RowProperties *	rp,
				int			col );

extern void docRowMaskToCellMask(	PropertyMask *		cellMask,
					const PropertyMask *	rowMask );

extern void docRowMaskApplyCellMask(	PropertyMask *		rowMask,
					const PropertyMask *	cellMask );

#   endif	/*  DOC_ROW_PROPS_H  */
