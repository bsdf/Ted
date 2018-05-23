/************************************************************************/
/*									*/
/*  Section Properties.							*/
/*									*/
/************************************************************************/

#   ifndef	DOC_SECT_PROP_H
#   define	DOC_SECT_PROP_H

#   include	<utilDocumentGeometry.h>
#   include	"docNotesProperties.h"

typedef enum PageNumberStyle
    {
    DOCpgnDEC= 0,
    DOCpgnUCRM,
    DOCpgnLCRM,
    DOCpgnUCLTR,
    DOCpgnLCLTR,

    DOCpgn_COUNT
    } PageNumberStyle;

typedef enum PageNumberHyphen
    {
    DOCpgnhPGNHNSH,
    DOCpgnhPGNHNSP,
    DOCpgnhPGNHNSC,
    DOCpgnhPGNHNSM,
    DOCpgnhPGNHNSN,

    DOCpgnh_COUNT
    } PageNumberHyphen;

typedef struct SectionColumn
    {
    int		scColumnWidthTwips;
    int		scSpaceToRightTwips;
    } SectionColumn;

typedef struct SectionProperties
    {
    DocumentGeometry		spDocumentGeometry;

    int				spStyle;

    int				spColumnSpacingTwips;
    unsigned char		spLineBetweenColumns;

    unsigned char		spHasTitlePage;
    unsigned char		spBreakKind;
    unsigned char		spPageNumberStyle;
    unsigned char		spPageNumberHyphen;
    unsigned char		spRestartPageNumbers;
				/**
				 * 0: This section will snake (newspaper style)
				 *	columns from left to right (the default)
				 * 1: This section will snake (newspaper style)
				 *	columns from right to left
				 */
    unsigned char		spRToL;

    int				spStartPageNumber;

    int				spColumnCount;
    SectionColumn *		spColumns;

    FootEndNotesProperties	spNotesProperties;
    } SectionProperties;

# define DOC_SECTpropsBELOW_PREVIOUS( sp ) \
				( (sp)->spBreakKind == DOCibkNONE )

# define docSectPropsFixedWidthColumns( sp ) \
    ( (sp)->spColumnCount < 2 || (sp)->spColumns[0].scColumnWidthTwips == 0 )

typedef enum SectionProperty
    {
    SPprop_NONE= -1,

    /* document geometry */

    SPpropSTYLE= DGprop_COUNT,
    SPpropTITLEPG,
    SPpropBREAK_KIND,
    SPpropNUMBER_STYLE,
    SPpropNUMBER_HYPHEN,
    SPpropPAGE_RESTART,
    SPpropRTOL,
    SPpropSTART_PAGE,
    SPpropCOLUMN_COUNT,
    SPpropCOLUMN_SPACING,
    SPpropLINEBETCOL,
    SPpropCOLUMNS,

    SPpropFOOTNOTE_STARTNR,
    SPpropFOOTNOTE_JUSTIFICATION,
    /* No SPpropFOOTNOTE_PLACEMENT, */
    SPpropFOOTNOTE_RESTART,
    SPpropFOOTNOTE_STYLE,

    SPpropENDNOTE_STARTNR,
    /* No SPpropENDNOTE_JUSTIFICATION, */
    /* No SPpropENDNOTE_PLACEMENT, */
    SPpropENDNOTE_RESTART,
    SPpropENDNOTE_STYLE,

    SPprop_COUNT,

		    /****************************************************/
		    /*  Used by RTF parser:				*/
		    /****************************************************/
    SPprop_COLUMN_NUMBER,
    SPprop_COLUMN_WIDTH,
    SPprop_COLUMN_RIGHT,

    SPprop_FULL_COUNT
    } SectionProperty;

extern const int DOCsectNOTE_PROP_MAP[FEPprop_COUNT];
extern const int * const DOCsectFOOTNOTE_PROP_MAP;
extern const int * const DOCsectENDNOTE_PROP_MAP;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docInitSectionProperties(	SectionProperties *	sp );
extern void docCleanSectionProperties(	SectionProperties *	sp );

extern int docCopySectionProperties(	SectionProperties *		to,
					const SectionProperties *	from );

extern int docSectionPropertiesSetColumnCount(
					SectionProperties *	sp,
					int			count );

extern void docSectPropertyDifference(
				PropertyMask *			pDiffMask,
				const SectionProperties *	sp1,
				const PropertyMask *		cmpMask,
				const SectionProperties *	sp2 );

extern int docUpdSectProperties( PropertyMask *			pSpDoneMask,
				SectionProperties *		spTo,
				const PropertyMask *		spSetMask,
				const SectionProperties *	spSet );

extern int docSectSetEqualColumnWidth(	SectionProperties *		sp );
extern int docSectSetExplicitColumnWidth( SectionProperties *		sp );

extern void docSectGetColumnX(	int *				pXLine,
				int *				pX0,
				int *				pX1,
				const SectionProperties *	sp,
				const DocumentGeometry *	dgPage,
				int				column );

extern int docSetSectionProperty(	SectionProperties *	sp,
					int			prop,
					int			arg );

extern int docGetSectionProperty(	const SectionProperties *	sp,
					int				prop );

extern void docFillSectNotesMask(	PropertyMask *		spMask );

extern int docSectGetColumnSpacing(
				int *				pMinValue,
				int *				pMaxValue,
				const SectionProperties *	sp,
				int				col );

extern int docSectSetColumnSpacing(	SectionProperties *	sp,
					int			col,
					int			value );

extern int docSectGetColumnWidth(
				int *				pMinValue,
				int *				pMaxValue,
				const SectionProperties *	sp,
				int				col );

extern int docSectSetColumnWidth(	SectionProperties *	sp,
					int			col,
					int			value );

#   endif	/*  DOC_SECT_PROP_H  */
