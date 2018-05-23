#   include	"psFontInfo.h"
#   include	"psTtfTableEntry.h"
#   include	"psTtfNameTable.h"

/************************************************************************/
/*									*/
/*  True Type font.							*/
/*									*/
/*  A considerable part of this code was borrowed from the ttf2pt1	*/
/*  project. Differences in purpose and validations against the		*/
/*  documentation made me change a lot.					*/
/*									*/
/*  SEE: http://developer.apple.com/fonts/TTRefMan/			*/
/*  SEE ALSO: http://www.microsoft.com/typography/otspec/		*/
/*									*/
/*  A substantial part of this code is experimental: E.G. it contains	*/
/*  code that can be used to convert from true type fonts to postscript	*/
/*  type1 that was adapted from 'ttf2pt1' but it is never called.	*/
/*									*/
/*  See doc of the 'name' table:					*/
/*  1)  Platform: 0=Unicode, 1= Mac, 3= Windows.			*/
/*  2)  Encoding: 0=Default, 1= Version 1.1, 2) ISO 10646-1993		*/
/*	deprecated, 3) Unicode 2.0 semantics.				*/
/*  3)  Platform,Encoding->bytes					*/
/*	0,*->2								*/
/*	1,0->1								*/
/*									*/
/************************************************************************/

#   define	PLATFORM_UNICODE	0
#   define	PLATFORM_MAC		1
#   define	PLATFORM_WINDOWS	2

# if 0

Unicode:
0  	Unicode 1.0 semantics
1 	Unicode 1.1 semantics
2 	ISO 10646:1993 semantics
3 	Unicode 2.0 and onwards semantics, Unicode BMP only.
4 	Unicode 2.0 and onwards semantics, Unicode full repertoire.

Microsoft:
3  	0  	Symbol
3 	1 	Unicode BMP only
3 	2 	ShiftJIS
3 	3 	PRC
3 	4 	Big5
3 	5 	Wansung
3 	6 	Johab
3 	7 	Reserved
3 	8 	Reserved
3 	9 	Reserved
3 	10 	Unicode full repertoire

Mac:
0  	Roman 
1 	Japanese 
2 	Traditional Chinese 
3 	Korean
4 	Arabic
5 	Hebrew
6 	Greek
7 	Russian
8 	RSymbol
9 	Devanagari
10 	Gurmukhi
11 	Gujarati
12 	Oriya
13 	Bengali
14 	Tamil
15 	Telugu
16 	Kannada
17  	Malayalam
18 	Sinhalese
19 	Burmese
20 	Khmer
21 	Thai
22 	Laotian
23 	Georgian
24 	Armenian
25 	Simplified Chinese
26 	Tibetan
27 	Mongolian
28 	Geez
29 	Slavic
30 	Vietnamese
31 	Sindhi
32 	(Uninterpreted)

# endif


typedef struct TrueTypeOS_2Table
    {
    unsigned int  	ttotVersion;
    int		 	ttotAvgCharWidth;
    unsigned int 	ttotWeightClass;
    unsigned int 	ttotWidthClass;
    int		 	ttotType;
    int		 	ttotSubscriptXSize;
    int		 	ttotSubscriptYSize;
    int		 	ttotSubscriptXOffset;
    int		 	ttotSubscriptYOffset;
    int		 	ttotSuperscriptXSize;
    int		 	ttotSuperscriptYSize;
    int		 	ttotSuperscriptXOffset;
    int		 	ttotSuperscriptYOffset;
    int		 	ttotStrikeoutSize;
    int		 	ttotStrikeoutPosition;
    int		 	ttotFamilyClass;
    unsigned char 	ttotPanose[10];
    unsigned long 	ttotCharRange[4];
    unsigned char 	ttotVendID[4+1];
    unsigned int 	ttotSelection;
    unsigned int 	ttotFirstCharIndex;
    unsigned int 	ttotLastCharIndex;
    } TrueTypeOS_2Table;

typedef struct TrueTypeHeadTable
    {
    unsigned long	tthtVersion;
    unsigned long	tthtFontRevision;
    unsigned long	tthtChecksumAdjust;
    unsigned long	tthtMagicNo;
    unsigned int	tthtFlags;
    unsigned int	tthtUnitsPerEm;
    unsigned char	tthtCreated[8+1];
    unsigned char	tthtModified[8+1];
    int			tthtXMin;
    int			tthtYMin;
    int			tthtXMax;
    int			tthtYMax;
    unsigned int	tthtMacStyle;
    unsigned int	tthtLowestRecPPem;
    int			tthtFontDirection;
    int			tthtIndexToLocFormat;
    int			tthtGlyphDataFormat;
    } TrueTypeHeadTable;

typedef struct TrueTypePostTable
    {
    int			ttptFormatUpper;
    unsigned int	ttptFormatLower;
    int			ttptItalicAngleUpper;
    unsigned int	ttptItalicAngleLower;
    int			ttptUnderlinePosition;
    int			ttptUnderlineThickness;
    unsigned int	ttptIsFixedPitch;
    /* unsigned int	ttptReserved */
    unsigned long	ttptMinMemType42;
    unsigned long	ttptMaxMemType42;
    unsigned long	ttptMinMemType1;
    unsigned long	ttptMaxMemType1;

    unsigned int	ttptIndexCount;
    unsigned short *	ttptIndices;
    unsigned int	ttptStringCount;
    unsigned char **	ttptStrings;
    } TrueTypePostTable;

typedef struct TrueTypeHheaTable
    {
    unsigned char	hheaVersion[4+1];
    int			hheaAscender;
    int			hheaDescender;
    int			hheaLineGap;
    unsigned int	hheaMaxAdvanceWidth;
    int			hheaMinLsb;
    int			hheaMinRsb;
    int			hheaXMaxExtent;
    int			hheaCaretSlopeRise;
    int			hheaCaretSlopeRun;
    /* int		hheaReserved[5]; */
    int			hheaMetricDataFormat;
    unsigned int	hheaMetricCount;
    } TrueTypeHheaTable;

typedef struct TrueTypeVheaTable
    {
    unsigned char	vheaVersion[4+1];
    int			vheaAscender;
    int			vheaDescender;
    int			vheaLineGap;
    unsigned int	vheaMaxAdvanceHeight;
    int			vheaMinTsb;
    int			vheaMinBsb;
    int			vheaYMaxExtent;
    int			vheaCaretSlopeRise;
    int			vheaCaretSlopeRun;
    int			vheaCaretOffset;
    /* int		vheaReserved[4]; */
    int			vheaMetricDataFormat;
    unsigned int	vheaMetricCount;
    } TrueTypeVheaTable;

typedef struct HorizontalMetric
    {
    unsigned int	hmAdvanceWidth;
    int			hmLsb;
    } HorizontalMetric;

typedef struct VerticalMetric
    {
    unsigned int	vmAdvanceHeight;
    int			vmTsb;
    } VerticalMetric;

typedef struct TrueTypeGlyph
    {
    int				ttgContourCount;	/* signed ! */
    int				ttgXMin;
    int				ttgYMin;
    int				ttgXMax;
    int				ttgYMax;

    const unsigned char *	ttgData;
    } TrueTypeGlyph;

typedef struct TrueTypeCmapRecord
    {
    unsigned int		ttcrPlatformID;		/*  1  */
    unsigned int		ttcrEncodingID;
    unsigned long		ttcrOffset;


    unsigned int		ttcrFormat;
    unsigned int		ttcrLength;
    unsigned int		ttcrLanguage;

				/*  If format == 0  */
    unsigned char		ttcr0GlyphIDArray[256];

				/*  If format == 2  */
    const unsigned char *	ttcr2SubHeaderKeys;
    const unsigned char *	ttcr2SubHeaders;
    int				ttcr2SubHeaderBytes;
    const unsigned char *	ttcr2GlyphIndexArray;

				/*  If format == 4  */
    unsigned int		ttcr4SegCountX2;
    unsigned int		ttcr4SearchRange;
    unsigned int		ttcr4EntrySelector;
    unsigned int		ttcr4RangeShift;
    const unsigned char *	ttcr4Data;

				/*  If format == 6  */
    unsigned int		ttcr6FirstCode;
    unsigned int		ttcr6_12CodeCount;
    unsigned int *		ttcr6_12GlyphCodes;

				/*  If format == 12; Use the 6 indices array  */
    unsigned int		ttcr12GroupCount;
    } TrueTypeCmapRecord;

typedef struct TrueTypeCmapTable
    {
    int				ttctVersion;
    int				ttctEncodingRecordCount;
    TrueTypeCmapRecord *	ttctEncodingRecords;
    } TrueTypeCmapTable;

typedef struct TrueTypeKernPair
    {
    unsigned int	ttkpLeft;
    unsigned int	ttkpRight;
    int			ttkpValue;
    } TrueTypeKernPair;

typedef struct TrueTypeKernSub
    {
    unsigned int	ttksVersion;
    unsigned int	ttksLength;
    unsigned int	ttksCoverage;
				/*  Bits in Coverage:			*/
				/*  0x8000: Has vertical kerning values	*/
				/*  0x4000: Has cross stream values	*/
				/*  0x4000: Has variation kerning vals	*/
				/*  0x1f00: Unused: must be 0.		*/
				/*  0x00ff: Contain the format in which	*/
				/*          the pairs are stored.	*/
				/*  So normal horizontal kerning stored	*/
				/*  as pairs has zero upper and lower	*/
				/*  bytes.				*/
    unsigned int	ttksPairCount;
    unsigned int	ttksSearchRange;
    unsigned int	ttksEntrySelector;
    unsigned int	ttksRangeShift;

    TrueTypeKernPair *	ttksPairs;
    } TrueTypeKernSub;

typedef struct TrueTypeKernTable
    {
    int				ttktVersion;
    int				ttktKernSubCount;
    TrueTypeKernSub *		ttktKernSubs;
    } TrueTypeKernTable;

typedef struct TrueTypeFont
    {
    unsigned long		ttfVersion;
    int				ttfTableCount;
    int				ttfSearchRange;
    int				ttfEntrySelector;
    int				ttfRangeShift;

    TrueTypeTableEntry *	ttfTables;

    TrueTypeNameTable		ttfNameTable;
    TrueTypePostTable		ttfPostTable;
    TrueTypeHeadTable		ttfHeadTable;
    TrueTypeHheaTable		ttfHheaTable;
    TrueTypeVheaTable		ttfVheaTable;
    TrueTypeCmapTable		ttfCmapTable;
    TrueTypeKernTable		ttfKernTable;
    TrueTypeOS_2Table		ttfOS_2Table;

    unsigned long *		ttfLocations;
    int				ttfLocationCount;

    HorizontalMetric *		ttfHorizontalMetrics;
    int				ttfHorizontalMetricCount;

    TrueTypeGlyph *		ttfGlyphs;
    int				ttfGlyphCount;

    IndexMapping		ttfToGlyphMapping;
    } TrueTypeFont;

/************************************************************************/
/*									*/
/*  Routine Declarations.						*/
/*									*/
/************************************************************************/

extern int psTtfLoadFont(	TrueTypeFont *		ttf,
				SimpleInputStream *	sisTtf ,
				long			filePos );

extern int psTtcLoadFont(	TrueTypeFont *		ttf,
				SimpleInputStream *	sisTtf,
				int			fontIndex );

extern void psTtfInitTrueTypeFont(	TrueTypeFont *	ttf );
extern void psTtfCleanTrueTypeFont(	TrueTypeFont *	ttf );

extern int psTtfFontInfo(	AfmFontInfo *		afi,
				const TrueTypeFont *	ttf );

