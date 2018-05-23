/************************************************************************/
/*									*/
/*  Border properties.							*/
/*									*/
/************************************************************************/

#   ifndef	DOC_BORDER_PROP_H
#   define	DOC_BORDER_PROP_H

#   include	<utilPropMask.h>
#   include	<utilColor.h>
#   include	<utilPalette.h>

/************************************************************************/
/*									*/
/*  Border Styles: How they are drawn.					*/
/*									*/
/************************************************************************/

typedef enum BorderStyle
    {
    DOCbsNONE= 0,
    DOCbsS,
    DOCbsTH,
    DOCbsSH,

    DOCbsDB,
    DOCbsHAIR,
    DOCbsDOT,
    DOCbsDASH,
    DOCbsDASHD,
    DOCbsDASHDD,
    DOCbsTRIPLE,

    DOCbsTNTHSG,
    DOCbsTHTNSG,
    DOCbsTNTHTNSG,
    DOCbsTNTHMG,
    DOCbsTHTNMG,
    DOCbsTNTHTNMG,
    DOCbsTNTHLG,
    DOCbsTHTNLG,
    DOCbsTNTHTNLG,

    DOCbsWAVY,
    DOCbsWAVYDB,
    DOCbsDASHSM,
    DOCbsDASHDOTSTR,
    DOCbsEMBOSS,
    DOCbsENGRAVE,
    DOCbsFRAME,
    DOCbsOUTSET,

    DOCbsTBL,
    DOCbsNIL,

    DOCbs_COUNT
    } BorderStyle;

#   define	DOCisBORDER(bp) ( \
		    (bp)->bpStyle != DOCbsNONE && \
		    (bp)->bpStyle != DOCbsTBL && \
		    (bp)->bpStyle != DOCbsNIL )

#   define BRDRW_MAX	75		/*  Maximum width		*/
#   define BRDRW_MAX2	(2*BRDRW_MAX)	/*  Maximum double width	*/

typedef struct BorderProperties
    {
    unsigned char	bpColor;
    unsigned char	bpPenWideTwips;
    unsigned char	bpStyle;		/*  (enum)		*/
    unsigned short	bpSpacingTwips;		/*  Only used for	*/
						/*  paragraph borders	*/
    unsigned char	bpArt;

    } BorderProperties;

typedef struct ExpandedBorderProperties
    {
    int			ebpColorExplicit;
    RGB8Color		ebpColor;

    int			ebpPenWideTwips;
    int			ebpStyle;
    int			ebpSpacingTwips;
    int			ebpArt;
    } ExpandedBorderProperties;

typedef enum BorderProperty
    {
    BRDRprop_NONE= -1,

    BRDRpropCOLOR= 0,
    BRDRpropSPACING,
    BRDRpropPEN_WIDE,
    BRDRpropSTYLE,
    BRDRpropART,

    BRDRprop_COUNT
    } BorderProperty;

#   define	docCleanBorderProperties( bp ) /*void*/

/************************************************************************/
/*									*/
/*  Routine Declarations.						*/
/*									*/
/************************************************************************/

extern void docInitBorderProperties(	BorderProperties *	bp );

extern void docUpdateBorderProperties(
				    PropertyMask * 		pBpDoneMask,
				    BorderProperties *		bpTo,
				    const PropertyMask * 	bpSetMask,
				    const BorderProperties *	bpFrom );

extern void docCopyBorderProperties(BorderProperties *		bpTo,
				    const BorderProperties *	bpFrom,
				    const int *			colorMap );

extern int docBordersDiffer(	const BorderProperties *	bp1,
				const BorderProperties *	bp2,
				const int *			colorMap );

extern void docExpandBorderProperties(
				ExpandedBorderProperties *	ebp,
				const BorderProperties *	bp,
				const ColorPalette *		cp );

extern int docIndirectBorderProperties(
			PropertyMask *				pBpDoneMask,
			BorderProperties *			bp,
			const PropertyMask * 			bpSetMask,
			const ExpandedBorderProperties *	ebp,
			ColorPalette *				cp );

extern void docInitExpandedBorderProperties(
				ExpandedBorderProperties *	ebp );

extern int docBorderThick(	int *				pWide,
				const BorderProperties *	bp );

extern void docStretchInsetForBorder(	int *				pInset,
					const BorderProperties *	bp );

extern void docAddBorderToInset(	int *				pInset,
					const BorderProperties *	bp );

extern int docSetBorderProperty(	BorderProperties *	bp,
					int			prop,
					int			arg );

extern int docGetBorderProperty(	const BorderProperties * bp,
					int			prop );

#   endif	/*  DOC_BORDER_PROP_H  */
