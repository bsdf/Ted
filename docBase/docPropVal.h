/************************************************************************/
/*									*/
/*  Values and sub structures used in various document property		*/
/*  structs.								*/
/*									*/
/************************************************************************/

#   ifndef	DOC_PROP_VAL_H
#   define	DOC_PROP_VAL_H

/************************************************************************/
/*									*/
/*  Levels of document item tree nesting.				*/
/*									*/
/************************************************************************/

typedef enum ItemLevel
    {
			/************************************************/
			/*  Ignore; Garbage values.			*/
			/************************************************/
    DOClevANY,
    DOClevOUT,
			/************************************************/
			/*  Different kinds of BufferItems.		*/
			/************************************************/
    DOClevBODY,
    DOClevSECT,
    DOClevROW,
    DOClevCELL,
    DOClevPARA,
			/************************************************/
			/*  Not really a level: Plain text.		*/
			/************************************************/
    DOClevSPAN,
			/************************************************/
			/*  Not really a level: To be investigated	*/
			/*  Values currently only used in the RTF	*/
			/*  parser for tags that it ignores or handles	*/
			/*  in a special way.				*/
			/************************************************/
    DOClevNESTCELL,
    DOClevNESTROW,
			/************************************************/
    DOClevNONESTTABLES,
			/************************************************/
    DOClevCOLUMN,
    DOClevTABLE,

    DOClev_COUNT
    } ItemLevel;

/************************************************************************/
/*									*/
/*  Breaks before sections. DOCibkNONE, DOCibkCOL and DOCibkPAGE are	*/
/*  also used for paragraphs.						*/
/*									*/
/************************************************************************/

typedef enum ItemBreak
    {
    DOCibkNONE= 0,
    DOCibkCOL,
    DOCibkPAGE,
#   define DOCibkpara_COUNT DOCibkEVEN
    DOCibkEVEN,
    DOCibkODD,

    DOCibk_COUNT
    } ItemBreak;

#   define	DOCibksect_COUNT DOCibk_COUNT

/************************************************************************/
/*									*/
/*  Horizontal alilignment of paragraphs and cells.			*/
/*									*/
/************************************************************************/

typedef enum TextHorizontalAlignment
    {
    DOCthaLEFT= 0,
    DOCthaRIGHT,
    DOCthaCENTERED,
    DOCthaJUSTIFIED,

    DOCtha_COUNT
    } TextHorizontalAlignment;

/************************************************************************/
/*									*/
/*  Vertical alilignment of cells and rows.				*/
/*									*/
/************************************************************************/

typedef enum TextVerticalAlignment
    {
    DOCtvaTOP= 0,
    DOCtvaCENTERED,
    DOCtvaBOTTOM,

    DOCtva_COUNT
    } TextVerticalAlignment;

typedef enum TextFlowDirection
    {
    TXflowTXLRTB= 0,	/*  left to right,  top to bottom (default).	*/
    TXflowTXTBRL,	/*  right to left,  top to bottom.		*/
    TXflowTXBTLR,	/*  left to right,  bottom to top.		*/
    TXflowTXLRTBV,	/*  left to right,  top to bottom, vertical.	*/
    TXflowTXTBRLV,	/*  top to bottom,  right to left, vertical.	*/

    TXflow_COUNT
    } TextFlowDirection;

/************************************************************************/
/*									*/
/*  NOTE that these are hard values set in the RTF spec.		*/
/*  NOTE that some values only apply to shapes, not to frames.		*/
/*									*/
/************************************************************************/

typedef enum FrameXReference
    {
    FXrefMARGIN= 0,
    FXrefPAGE,
    FXrefCOLUMN,
#   define	FXref__FRAME_COUNT	(FXrefCOLUMN+1)
    FXrefCHARACTER,
    FXrefIGNORE,

    FXref_COUNT
    } FrameXReference;

/************************************************************************/
/*									*/
/*  NOTE that these are hard values set in the RTF spec.		*/
/*  NOTE that some values only apply to shapes, not to frames.		*/
/*									*/
/************************************************************************/

typedef enum FrameYReference
    {
    FYrefMARGIN= 0,
    FYrefPAGE,
    FYrefPARA,
#   define	FYref__FRAME_COUNT	(FYrefPARA+1)
		    /**
		     *  Position relative to the line of the anchor.
		     *  Experimenting with Word 2003 shows that for 
		     *  shapes the deepest level is positioning relative
		     *  to the paragraph. (For shapes) In the user interface,
		     *  it is possible to select the line. If you select
		     *  positioning relative to the line, MS-Word calculates 
		     *  the vertical offset relative to the paragraph.
		     */
    FYrefLINE,
    FYrefIGNORE,

    FYref_COUNT
    } FrameYReference;

/************************************************************************/
/*									*/
/*  Note that these are hard values set in the RTF spec.		*/
/*									*/
/************************************************************************/

typedef enum FrameXPosition
    {
    FXposXGIVEN= 0,
    FXposXL,
    FXposXC,
    FXposXR,
    FXposXI,
    FXposXO,

    FXpos_COUNT
    } FrameXPosition;

/************************************************************************/
/*									*/
/*  Note that these are hard values set in the RTF spec.		*/
/*									*/
/************************************************************************/

typedef enum FrameYPosition
    {
    FYposYGIVEN= 0,
    FYposYT,
    FYposYC,
    FYposYB,
    FYposYIN,
    FYposYOUT,

    FYpos_COUNT
    } FrameYPosition;

/************************************************************************/
/*									*/
/*  Units for table autoformatting:					*/
/*									*/
/************************************************************************/

typedef enum TableAutoFormatUnit
    {
    TRautoNONE= 0,
    TRautoAUTO,
    TRautoPERCENT_50,
    TRautoTWIPS,

    TRauto_COUNT
    } TableAutoFormatUnit;

#   endif	/*  DOC_PROP_VAL_H  */
