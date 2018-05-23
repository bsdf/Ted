/************************************************************************/
/*									*/
/*  Data structures that describe postscript fonts.			*/
/*									*/
/************************************************************************/

#   ifndef	PS_FONT_INFO_H
#   define	PS_FONT_INFO_H

#   include	<geo2DInteger.h>
#   include	<utilIndexSet.h>
#   include	<utilIndexMapping.h>
#   include	<utilMemoryBuffer.h>

#   define	SCAPS_SIZE( sz )	( 8*(sz)/10 )
#   define	SUPERSUB_SIZE( sz )	( 6*(sz)/10 )

#   define	PSflagDEFER_METRICS	0x01
#   define	PSflagIGNORE_KERNING	0x02

/************************************************************************/
/*									*/
/*  Information extracted from an AFM file. (Or directly from a font)	*/
/*									*/
/*  The afm file format suggests that a font is a mapping from glyph	*/
/*  to code point while both type1 fonts and TTF fonts actually work	*/
/*  the other way: They have a collection of glyphs and the encoding	*/
/*  maps a code point to the glyph.					*/
/*									*/
/*  Bounding boxes are vertically oriented in the numeric orientation	*/
/*  of the numbers. I.E: Bottom:drY0, Top:drY1.				*/
/*									*/
/************************************************************************/

typedef struct AfmKerningPair
    {
    short int		akpPosition;
    short int		akpXVec;
    } AfmKerningPair;

typedef struct AfmCodeList
    {
    unsigned short int *	aclCodes;
    int				aclCodeCount;
    } AfmCodeList;

typedef struct AfmCharMetric
    {
    AfmCodeList		acmDefaultCodeList;
    AfmCodeList		acmUnicodeCodeList;
				    /************************************/
				    /*  Potentially empty array of code	*/
				    /*  points associated to this glyph.*/
				    /*  This is because it is possible	*/
				    /*  to map more than one code point	*/
				    /*  in the default encoding to the	*/
				    /*  same glyph.			*/
				    /*  Only values < 256 are saved to	*/
				    /*  the afm file.			*/
				    /************************************/

    short int		acmWX;
    DocumentRectangle	acmBBox;
    char *		acmN;
    short int		acmGlyphIndex;
				    /************************************/
				    /*  Index of the glyph in the font.	*/
				    /************************************/
    AfmKerningPair *	acmKernPairs;
    short int		acmKernPairCount;
    } AfmCharMetric;

struct AfmFontInfo;

typedef int (*ResolveFontMetrics)(	struct AfmFontInfo *	afi );

typedef struct AfmFontInfo
    {
			    /********************************************/
			    /*  Must be unique in the font list.	*/
			    /********************************************/
    int			afiFaceNumber;
			    /********************************************/
			    /*  General font information. At least an	*/
			    /*  Approximation must be known while the	*/
			    /*  font catalog is made.			*/
			    /********************************************/
    char *		afiFontName;
    char *		afiFullName;
    char *		afiFamilyName;
    char *		afiFamilyName_Orig;
    char *		afiWeightStr;
    short int		afiWeightInt;
    char *		afiWidthStr;
    short int		afiWidthInt;
    double		afiItalicAngle;
    double		afiTanItalicAngle;
    unsigned char	afiIsFixedPitch;

    char *		afiNotice;
    char *		afiVersion;

			    /********************************************/
			    /*  Metrics information: Must be known as	*/
			    /*  soon as text is formatted.		*/
			    /********************************************/
    int			afiUnitsPerEm;
    DocumentRectangle	afiFontBBox;
    int			afiUnderlinePosition;
    int			afiUnderlineThickness;
    int			afiCapHeight;
    int			afiXHeight;
    int			afiAscender;
    int			afiDescender;

			    /********************************************/
			    /*  Glyph Metrics information: Must be	*/
			    /*  known as soon as text is formatted.	*/
			    /********************************************/
    MemoryBuffer	afiAfmFileName;
    unsigned char	afiMetricsDeferred;
    unsigned char	afiIgnoreKerning;
    int			afiMetricCount;
    AfmCharMetric **	afiMetrics;
    void *		afiNameToMetric;
    int			afiFallbackGlyph;

			    /********************************************/
			    /*  Collected screen font information.	*/
			    /********************************************/
    char **		afiX11Fonts;
    int			afiX11FontCount;

    char *		afiEncodingScheme;
    char *		afiCharacterSet;
    char *		afiVendor;

    char *		afiResourceName;
    MemoryBuffer	afiFontFileName;
    int			afiFontFileIndex;

    IndexSet		afiUnicodesProvided;
    IndexMapping	afiUnicodeToGlyphMapping;
			    /********************************************/
			    /*  Hacked screen related functionality.	*/
			    /********************************************/
    char *		afiStyle;

    ResolveFontMetrics	afiResolveMetrics;
    } AfmFontInfo;

# define psFontHasUnicode( a, u ) \
		utilIndexSetContains( &((a)->afiUnicodesProvided), (u) )

/************************************************************************/
/*									*/
/*  Face information.							*/
/*									*/
/*  Used to map a users idea of fonts to implementation fonts.		*/
/*  Used by the font chooser to select fonts.				*/
/*									*/
/*  1)  Set width defines. The values math those in fontconfig.		*/
/*  2)  Set weight defines. The values math those in fontconfig.	*/
/*									*/
/************************************************************************/

/*  1  */
#   define	FONTwidthULTRACONDENSED		50
#   define	FONTwidthEXTRACONDENSED		63
#   define	FONTwidthCONDENSED		75
#   define	FONTwidthSEMICONDENSED		87
#   define	FONTwidthNORMAL			100
#   define	FONTwidthSEMIEXPANDED		113
#   define	FONTwidthEXPANDED		125
#   define	FONTwidthEXTRAEXPANDED		150
#   define	FONTwidthULTRAEXPANDED		200

#   define	FONTwidth_MIN			FONTwidthULTRACONDENSED
#   define	FONTwidth_MAX			FONTwidthULTRAEXPANDED

/*  2  */
#   define	FONTweightTHIN			0
#   define	FONTweightEXTRALIGHT		40
#   define	FONTweightLIGHT			50
#   define	FONTweightBOOK			75
#   define	FONTweightREGULAR		80
#   define	FONTweightMEDIUM		100
#   define	FONTweightDEMIBOLD		180
#   define	FONTweightBOLD			200
#   define	FONTweightEXTRABOLD		205
#   define	FONTweightBLACK			210

#   define	FONTweightULTRALIGHT		FONTweightEXTRALIGHT
#   define	FONTweightNORMAL		FONTweightREGULAR
#   define	FONTweightSEMIBOLD		FONTweightDEMIBOLD
#   define	FONTweightULTRABOLD		FONTweightEXTRABOLD
#   define	FONTweightHEAVY			FONTweightBLACK

#   define	FONTweight_MIN			FONTweightTHIN
#   define	FONTweight_MAX			FONTweightBLACK

/************************************************************************/

# define FONT_IS_BOLD( afi ) ((afi)->afiWeightInt >= FONTweightDEMIBOLD )
# define FONT_IS_SLANTED( afi ) ((afi)->afiItalicAngle < -1.0 )

/************************************************************************/
/*									*/
/*  Declarations.							*/
/*									*/
/************************************************************************/

extern int psGetUnicodesFromGlyphNames(	AfmFontInfo *		afi );
extern int psGetAlternateGlyphs(	AfmFontInfo *		afi );
extern int psResolveFallbackGlyph(	AfmFontInfo *		afi );

extern void psInitAfmFontInfo(		AfmFontInfo *		afi );
extern void psCleanAfmFontInfo(		AfmFontInfo *		afi );
extern void psFreeAfmFontInfo(		AfmFontInfo *		afi );
extern void psRemoveMetricsFromInfo(	AfmFontInfo *		afi );

extern int psFindAlternate(		const AfmFontInfo *	afi,
					const char *		glyphName );

extern int psGetDeferredMetrics(	AfmFontInfo *		afi );

extern int psFontInfoAddMetric(		AfmFontInfo *			afi,
					int				C,
					int				WX,
					const char *			N,
					const DocumentRectangle *	abb );

extern int psFontInfoGetGlyphIndex(	const AfmFontInfo *	afi,
					const char *		glyphName );

extern int psFontInfoSetGlyphName(	AfmFontInfo *		afi,
					int			glyphIndex,
					const char *		glyphName );

extern const char * psFontInfoGetGlyphName(
					const AfmFontInfo *	afi,
					int			glyphIndex );

extern int psFontInfoSetGlyphCode(	AfmFontInfo *		afi,
					int			glyphIndex,
					int			c );

extern int psFontInfoSetGlyphUnicode(	AfmFontInfo *		afi,
					int			glyphIndex,
					int			c );

extern int psAddX11FontToInfo(		AfmFontInfo *		afi,
					const char *		x11Font );

extern int psMapVisibleNamesToBase35(	AfmFontInfo *		afi );

#   endif
