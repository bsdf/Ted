/************************************************************************/
/*									*/
/*  Attributes of text.							*/
/*									*/
/************************************************************************/

#   ifndef	UTIL_TEXT_ATTRIBUTE_H
#   define	UTIL_TEXT_ATTRIBUTE_H

#   include	<utilPropMask.h>

typedef struct TextAttribute
    {
    int			taFontNumber;
    int			taFontSizeHalfPoints;
    int			taTextColorNumber;
    int			taTextStyleNumber;

    int			taBorderNumber;
    int			taShadingNumber;
    int			taBaselineShiftHalfPoints;

    unsigned char	taFontIsBold;
    unsigned char	taFontIsSlanted;
    unsigned char	taTextIsUnderlined;
#   define			DOCunderlineNONE	0
#   define			DOCunderlineSINGLE	1
#   define			DOCunderlineDOUBLE	2

    unsigned char	taSmallCaps;
    unsigned char	taHasStrikethrough;
    unsigned char	taCapitals;

    unsigned char	taSuperSub;
    } TextAttribute;

typedef enum TextSuperSub
    {
    TEXTvaREGULAR= 0,
    TEXTvaSUPERSCRIPT,
    TEXTvaSUBSCRIPT,

    TEXTva_COUNT
    } TextSuperSub;

typedef enum TextProperty
    {
    TAprop_NONE= -1,

    TApropFONT_NUMBER= 0,
    TApropFONTSIZE,
    TApropFONTBOLD,
    TApropFONTSLANTED,
    TApropTEXTUNDERLINED,
    TApropSUPERSUB,
    TApropSMALLCAPS,
    TApropCAPITALS,
    TApropSTRIKETHROUGH,
    TApropTEXT_COLOR,
    TApropTEXT_STYLE,

    TApropBORDER,
    TApropSHADING,

    TApropBASELINE_SHIFT,

    TAprop_BASELINE_SHIFT_DOWN,
#   define TAprop_COUNT TAprop_BASELINE_SHIFT_DOWN

    TAprop_FULL_COUNT
    } TextProperty;

#   define	docEqualFontAttributes(ta1,ta2) ( \
		(ta1)->taFontIsBold == (ta2)->taFontIsBold && \
		(ta1)->taFontIsSlanted == (ta2)->taFontIsSlanted )

#   define	docEqualFont(ta1,ta2) ( \
		(ta1)->taFontNumber == (ta2)->taFontNumber && \
		(ta1)->taFontSizeHalfPoints == (ta2)->taFontSizeHalfPoints && \
		(ta1)->taSuperSub == (ta2)->taSuperSub && \
		(ta1)->taSmallCaps == (ta2)->taSmallCaps && \
		docEqualFontAttributes( (ta1), (ta2) ) )

#   define	docEqualTextAttributes(ta1,ta2) ( \
		docEqualFont((ta1),(ta2)) && \
		(ta1)->taTextIsUnderlined == (ta2)->taTextIsUnderlined && \
		(ta1)->taHasStrikethrough == (ta2)->taHasStrikethrough && \
		(ta1)->taCapitals == (ta2)->taCapitals && \
		(ta1)->taTextColorNumber == (ta2)->taTextColorNumber && \
		(ta1)->taBorderNumber == (ta2)->taBorderNumber && \
		(ta1)->taShadingNumber == (ta2)->taShadingNumber && \
		(ta1)->taBaselineShiftHalfPoints == (ta2)->taBaselineShiftHalfPoints )

#define FACE_INDEX( isS, isB ) ( 2*( (isS) != 0 )+ ( (isB) != 0 ) )
#define FACE_BOLD( idx ) ( (idx) % 2 != 0 )
#define FACE_SLANTED( idx ) ( ((idx)/2) % 2 != 0 )

/************************************************************************/
/*									*/
/*  Default attributes.							*/
/*									*/
/************************************************************************/

extern TextAttribute   DocDefaultAttributes;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void utilInitTextAttribute(	TextAttribute *		ta );

extern void utilUpdateTextAttribute(	PropertyMask *		pDoneMask,
					TextAttribute *		ta,
					const PropertyMask *	taSetMask,
					const TextAttribute *	taSet );

extern void utilAttributeDifference(	PropertyMask *		pDifMask,
					const TextAttribute *	ta1,
					const PropertyMask *	cmpMask,
					const TextAttribute *	ta2 );

extern int utilSetTextProperty(		TextAttribute *		ta,
					int			prop,
					int			arg );

extern int utilGetTextProperty(		const TextAttribute *	ta,
					int			prop );

#   endif	/*  UTIL_TEXT_ATTRIBUTE_H	*/
