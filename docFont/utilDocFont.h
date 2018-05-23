/************************************************************************/
/*									*/
/*  The font list of a document.					*/
/*									*/
/************************************************************************/

#   ifndef	DOC_FONT_H
#   define	DOC_FONT_H

#   include	<psFontInfo.h>
#   include	<utilIndexMapping.h>
#   include	<textOfficeCharset.h>
#   include	<utilIndexSet.h>

/************************************************************************/
/*									*/
/*  Font Administration in terms of 'Document Fonts'			*/
/*									*/
/*  1)  A document font is characterized by an item in the font list	*/
/*	in an office document. The main key is the name of the font.	*/
/*	It is possible that two fonts with the same name exist. In that	*/
/*	case, the character set helps to distinguish between them.	*/
/*									*/
/*  2)  The administration distinguishes between fonts that are 'used'	*/
/*	and those that are not. For efficient PostScript production,	*/
/*	the distincion might be too coarse: Many fonts that have been	*/
/*	used count as used. The purpose of the administration is to	*/
/*	decide what fonts to save in a document. Typically the set of	*/
/*	fonts that count as used consistes of the fonts that were in	*/
/*	the font list of a document when it was opened plus the ones	*/
/*	that have been assigned to a stretch of text during editing.	*/
/*									*/
/*  3)	fnil, fswiss ..							*/
/*  4)	Helvetica, Times..						*/
/*  5)	\\falt in rtf							*/
/*  6)	f0, f1 ...							*/
/*  7)	fcharsetN							*/
/*  8)	cpgN								*/
/*  9)	fprqN								*/
/*									*/
/*  10)	If the character set is given, (dfCharset) it is used to find	*/
/*	how the text is encoded. The actual index in the array of	*/
/*	office character sets is determined through a lookup. Otherwise	*/
/*	the index is derived from the code page (dfCodepage) or guessed	*/
/*	from the matching postscript font.				*/
/*									*/
/************************************************************************/

					/*  fprqN			*/
#   define FONTpitchDEFAULT		0
#   define FONTpitchFIXED		1
#   define FONTpitchVARIABLE		2

#   define FONTlenPANOSE		20

typedef enum FontFaceIndex
    {
    FONTfaceREGULAR= 0,
    FONTfaceBOLD,
    FONTfaceSLANTED,
    FONTfaceBOLD_SLANTED,

    FONTface_COUNT
    } FontFaceIndex;

typedef struct DocumentFont
    {
    char *				dfName;		/*  4		*/
    char *				dfAltName;	/*  5		*/
    short int				dfDocFontNumber;/*  6		*/

    unsigned char			dfStyleInt;	/*  3		*/
    unsigned char			dfPitch;	/*  9		*/
    char				dfPanose[FONTlenPANOSE+1];

    AfmFontInfo *			dfPsFontInfo[FONTface_COUNT];

					/**
					 * Mostly used while reading/
					 * opening/writing a document:
					 */
    IndexSet				dfUnicodesUsed;
    IndexMapping			dfUnicodeToCharset;

    int					dfCharsetIdxUsed[CHARSETidx_COUNT];
    int					dfCharsetIdxUsedCount;
    } DocumentFont;

typedef enum DocumentFontProperty
    {
    DFpropFAMILY_STYLE= 0,
    DFpropNAME,
    DFpropALT_NAME,
    DFpropCHARSET,
    DFpropCODEPAGE,
    DFpropPITCH,
    DFpropPANOSE,

    DFprop_COUNT
    } DocumentFontProperty;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docInitDocumentFont(	DocumentFont *		df );

extern void docCleanDocumentFont(	DocumentFont *		df );

extern int docCopyDocumentFont(		DocumentFont *		to,
					const DocumentFont *	from );

extern int docFontSetFamilyStyle(	DocumentFont *	df,
					int		style );

extern int docFontSetFamilyName(	DocumentFont *	df,
					const char *	name );

extern int docFontSetAltName(		DocumentFont *	df,
					const char *	name );

extern void utilCopyFontFaceMatches(	DocumentFont *		dfTo,
					const DocumentFont *	dfFrom );

extern int docFontFindLegacyEncodings(	DocumentFont *		df );

extern int utilRemoveCharsetFromFontName( DocumentFont *	df,
					int			charset );

extern int cssFontFamilyIndicator(	char *			target,
					int			maxSize,
					int			styleInt,
					const char *		familyName );

#   endif	/*  DOC_FONT_H	*/
