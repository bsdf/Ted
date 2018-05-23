/************************************************************************/
/*									*/
/*  Paragraph properties.						*/
/*									*/
/************************************************************************/

#   ifndef	DOC_PARA_PROP_H
#   define	DOC_PARA_PROP_H

#   include	<utilPropMask.h>
#   include	"docDocumentAttributeMap.h"

/************************************************************************/
/*									*/
/*  Paragraph properties.						*/
/*									*/
/*  Unless explicitly stated, all properties except numbers in pixels	*/
/*  are properties of the paragraph.					*/
/*									*/
/*  1)  Tab stops.							*/
/*  2)  Indentations.							*/
/*  3)  White space above/after the paragraph.				*/
/*  4)  Style, List Override ID.					*/
/*  5)  Borders.							*/
/*  6)  Shading.							*/
/*  7)  Frame.							*/
/*									*/
/************************************************************************/

typedef struct ParagraphProperties
    {
    /*  1  */
    short int		ppTabStopListNumber;

    /*  2  */
    short int		ppFirstIndentTwips;
    short int		ppLeftIndentTwips;
    short int		ppRightIndentTwips;

    /*  3  */
    short int		ppSpaceBeforeTwips;
    short int		ppSpaceAfterTwips;
    short int		ppLineSpacingTwips;

    /*  4  */
    short int		ppStyle;
    short int		ppListOverride;

    /*  5  */
    int			ppTopBorderNumber;
    int			ppBottomBorderNumber;
    int			ppLeftBorderNumber;
    int			ppRightBorderNumber;
    int			ppBetweenBorderNumber;
    int			ppBarNumber;

    /*  6  */
    int			ppShadingNumber;

    /*  7  */
    int			ppFrameNumber;
    unsigned char	ppTableNesting;			/*  itap	*/

    unsigned char	ppBreakKind;

    unsigned char	ppOutlineLevel;			/*  0 .. 9	*/
							/*  9= body.	*/
    unsigned char	ppListLevel; 			/*  ilvl	*/

    unsigned char	ppAlignment;			/*  (enum)	*/

    unsigned char	ppLineSpacingIsMultiple;	/*  boolean	*/

    unsigned char	ppKeepOnPage;			/*  boolean	*/
    unsigned char	ppKeepWithNext;			/*  boolean	*/
    unsigned char	ppWidowControl;			/*  boolean	*/
    unsigned char	ppHyphenateParagraph;		/*  boolean	*/

			/**
			 * 0: Text in this paragraph will display with
			 *	left-to-right precedence (the default)
			 * 1: Text in this paragraph will display with
			 *	right-to-left precedence
			 */
    unsigned char	ppRToL;
    } ParagraphProperties;

/************************************************************************/
/*									*/
/*  Numbers of paragraph properies.					*/
/*									*/
/************************************************************************/

typedef enum ParagraphProperty
    {
    PPprop_NONE= -1,

    PPpropSTYLE= 0,
    PPpropLISTOVERRIDE,
    PPpropTABLE_NESTING,
    PPpropLEFT_INDENT,
    PPpropFIRST_INDENT,
    PPpropRIGHT_INDENT,
    PPpropALIGNMENT,
    PPpropTAB_STOPS,
    PPpropBREAK_KIND,
    PPpropWIDCTLPAR,
    PPpropKEEP,
    PPpropKEEPN,
    PPpropSPACE_BEFORE,
    PPpropSPACE_AFTER,
    PPpropLINE_SPACING_DIST,
    PPpropLINE_SPACING_MULT,

    PPpropTOP_BORDER,
    PPpropBOTTOM_BORDER,
    PPpropLEFT_BORDER,
    PPpropRIGHT_BORDER,
    PPpropBETWEEN_BORDER,
    PPpropBAR_BORDER,

    PPpropSHADING,
    PPpropFRAME,

    PPpropOUTLINELEVEL,
    PPpropLISTLEVEL,
    PPpropHYPHPAR,
    PPpropRTOL,

    PPprop_COUNT,

    PPprop_IN_TABLE,			/* For read/write/tools only	*/

    PPprop_IGNORED,

	/**
	 *  Not real properties. But used for managing the ruler of a
	 *  document window.
	 */
    PPprop_COLUMNS,
    PPprop_TAB_KIND_BUTTON,

	/**
	 *  Not a real property. But used to support the obsolete \\box tag.
	 *
	 */
    PPprop_BOX_BORDER,			/*  Not a real property.	*/
					/*  But used to support the	*/
					/*  obsolete \\box tag.		*/

    PPprop_LISB,			/*  ignored.			*/
    PPprop_LISA,			/*  ignored.			*/

    PPprop_FULL_COUNT
    } ParagraphProperty;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int docUpdParaProperties( PropertyMask *			pPpDonePask,
				ParagraphProperties *		pp,
				const PropertyMask *		ppSetMask,
				const ParagraphProperties *	ppSet,
				const DocumentAttributeMap *	dam );

extern void docParaPropertyDifference(
				PropertyMask *			pDifMask,
				const ParagraphProperties *	pp1,
				const PropertyMask *		cmpMask,
				const ParagraphProperties *	pp2 );

extern void docCleanParagraphProperties(	ParagraphProperties *	pp );

extern void docInitParagraphProperties(		ParagraphProperties *	pp );

extern int docCopyParagraphProperties(
				ParagraphProperties *		ppTo,
				const ParagraphProperties *	ppFrom );

extern int docSetParaProperty(		ParagraphProperties *	pp,
					int			prop,
					int			arg );

extern int docGetParaProperty(		const ParagraphProperties *	pp,
					int				prop );

#   endif	/*  DOC_PARA_PROP_H	*/
