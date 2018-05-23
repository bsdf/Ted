/************************************************************************/
/*									*/
/*  Data structures that describe postscript fonts families.		*/
/*									*/
/*  Note that the document concept of a font family and the postscript	*/
/*  one contain the same set of fonts/faces. PostScript distinguishes	*/
/*  the styles. Here the encoding does not play a role: A face simply	*/
/*  has a set of characters.						*/
/*  The document variant works differently: The style variants are	*/
/*  treated as part of the text style. Here the encoding in the file	*/
/*  does play a role. Text with a different encoding has a different	*/
/*  font number.							*/
/*									*/
/*  This observation is implemented by the private Ted matcher and by	*/
/*  the font list for the font tool. It is completely ignored if fonts	*/
/*  are matched through fontconfig.					*/
/*									*/
/************************************************************************/

#   include	"psFontInfo.h"

#   ifndef	PS_FONT_FAMILY_H
#   define	PS_FONT_FAMILY_H

typedef struct PsFontFamily
    {
    char *			psfFontFamilyName;		/*  1	*/
    char *			psfFontFamilyName_Orig;		/*  2	*/
    AfmFontInfo **		psfFaces;			/*  3	*/
    short int			psfFaceCount;
    unsigned char		psfHasFixedWidth;		/*  4	*/
    unsigned char		psfHasProportionalWidth;	/*  4	*/
    unsigned char		psfWidthInt;
    } PsFontFamily;

/************************************************************************/
/*									*/
/*  Declarations.							*/
/*									*/
/************************************************************************/

extern void psInitFontFamily(		PsFontFamily *		psf );
extern void psFreeFontFamily(		PsFontFamily *		psf );

extern PsFontFamily * psFontFamilyFromFaces(	AfmFontInfo **	afis,
						int *		pConsumed,
						int		count );

#   endif
