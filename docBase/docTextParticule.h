/************************************************************************/
/*									*/
/*  Text Editor Describes a run of text.				*/
/*									*/
/************************************************************************/

#   ifndef	DOC_TEXT_PARTICULE_H
#   define	DOC_TEXT_PARTICULE_H

/************************************************************************/
/*									*/
/*  An homogenous piece of text. Without hyphenation, it will not be	*/
/*  divided over lines.							*/
/*  It consists of some characters and then some spaces.		*/
/*									*/
/************************************************************************/

typedef enum DocParticuleKind
    {
    DOCkindUNKNOWN= 0,

    DOCkindSPAN,
    DOCkindTAB,
    DOCkindOBJECT,
    DOCkindFIELDHEAD,
    DOCkindFIELDTAIL,
    DOCkindLINEBREAK,
    DOCkindPAGEBREAK,
    DOCkindCOLUMNBREAK,
    DOCkindCHFTNSEP,
    DOCkindCHFTNSEPC,
    DOCkindOPT_HYPH,

    DOCkindLTR_MARK,
    DOCkindRTL_MARK,

    DOCkind_PARA_END,
    DOCkind_COUNT
    } DocParticuleKind;

typedef struct TextParticule
    {
			/**
			 * String offset of the particule
			 */
    unsigned short	tpStroff;
			/**
			 * Number of bytes in the particule
			 */
    unsigned short	tpStrlen;
    unsigned char	tpKind;
    unsigned char	tpFlags;
#				define TPflagLEFT_BORDER	0x01
#				define TPflagRIGHT_BORDER	0x02
#				define TPflagXMATCH_LEFT	0x04
#				define TPflagXMATCH_RIGHT	0x08
    unsigned short	tpTwipsWide;	
			    /********************************************/
			    /*  Cannot hold A0 in landscape: 67336 tw	*/
			    /*  Left and right vertical text borders	*/
			    /*  are included in the width.		*/
			    /********************************************/
    short		tpXContentXPixels;
			    /********************************************/
			    /*  On screen drawing: to correct for	*/
			    /*  differences between Screen and		*/
			    /*  PostScript font. Relative to paragraph	*/
			    /*  frame. This is the offset of the text.	*/
			    /*  if the particule has a left text border	*/
			    /*  that border is drawn left of this	*/
			    /*  offset.					*/
			    /*  Must be signed: particules can protrude	*/
			    /*  to the left of the paragraph frame.	*/
			    /********************************************/
    int			tpObjectNumber;
    int			tpTextAttrNr;
    } TextParticule;

#   endif
