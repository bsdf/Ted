/************************************************************************/
/*									*/
/*  Levels in the Lists of the list table of a document.		*/
/*									*/
/*  O)	Word 7.0 == Word 95 compatibility				*/
/*									*/
/************************************************************************/

#   ifndef	DOC_LIST_LEVEL_H
#   define	DOC_LIST_LEVEL_H

#   include	<utilPropMask.h>
#   include	<utilMemoryBuffer.h>
#   include	<textAttribute.h>

#   include	"docParaProperties.h"

typedef enum ListLevelAlignment
    {
    DOCllaLEFT= 0,
    DOCllaCENTER,
    DOCllaRIGHT,

    DOClla_COUNT
    } ListLevelAlignment;

typedef enum ListLevelFollow
    {
    DOCllfTAB= 0,
    DOCllfSPACE,
    DOCllfNONE,

    DOCllf_COUNT
    } ListLevelFollow;

/************************************************************************/
/*									*/
/*  Holds the position of placeholders in the format that are replaced	*/
/*  the level numbers in the format string.				*/
/*  Actually there is an extra one that holds the length of the format	*/
/*  string. (In lnOffsetBytes)						*/
/*									*/
/************************************************************************/

typedef struct LevelNumber
    {
    int		lnOffsetBytes;	/*  Offset in llFormatString of the	*/
    				/*  end of the bytes that precede the	*/
				/*  level number.			*/
    int		lnConstChars;	/*  The number of unicode characters	*/
				/*  that is encoded in UTF-8 at the	*/
				/*  offset in the format string. This	*/
				/*  is only used to write RTF.		*/
    int		lnLevel;	/*  The list level that it shows.	*/
    } LevelNumber;

typedef struct ListLevel
    {
    int			llStartAt;
    unsigned char	llNumberStyle;
    unsigned char	llJustification;
    unsigned char	llFollow;
    unsigned char	llPrevToDigits;
    unsigned char	llNoRestart;
    unsigned char	llFontBias;
    short int		llPictureNumber;

    MemoryBuffer	llFormatString;
			/************************************************/
			/*  In memory:					*/
			/*  ----------					*/
			/*  A string thet holds the constant pieces of	*/
			/*  the format. The level numbers and the length*/
			/*  are in llLevelNumbers.			*/
			/*  In the file:				*/
			/*  ------------				*/
			/*  The first position holds the size of the	*/
			/*  rest of the string. The rest is a format	*/
			/*  string consisting of pieces of constant	*/
			/*  string and placeholders for the level	*/
			/*  numbers in the paragraph number. The	*/
			/*  placeholders have low values that indicate	*/
			/*  what level number to substitute. Use	*/
			/*  llLevelNumbers to find the placeholders	*/
			/*  Though the byte values are a clue,		*/
			/*  llLevelNumbers is leading. E.G:		*/
			/*  1.1.1 -> \'06\'00.\'01.\'02 in the file	*/
			/*  and ".." in memory.				*/
			/************************************************/
    long		llTemplateID;
    LevelNumber *	llLevelNumbers;
    int			llLevelNumberCount;
			/************************************************/
			/*  In the file:				*/
			/*  ------------				*/
			/*  Bytes giving the positions of level place-	*/
			/*  holders in llTextFormat. The subscript	*/
			/*  does not tell what level to substitute.	*/
			/*  The byte in llTextFormat holds the level	*/
			/*  number to substitute. As the format starts	*/
			/*  with its size, indices start from 1. E.G:	*/
			/*  1.1.1 -> \'01\'03\'05 in the file.		*/
			/*  NOTE that the indices are in terms of	*/
			/*  characters: As the format string might have	*/
			/*  a multi-byte encoding, the indices cannot	*/
			/*  be used as byte indices.			*/
			/************************************************/

    unsigned char	llFromOld;		/*  O  */
    unsigned char	llUsePrevText;		/*  O  */
    unsigned char	llUsePrevSpace;	/*  O  */
    int			llIndent;		/*  O  */
    int			llSpace;		/*  O  */

    int			llTabStopListNumber;
    int			llLeftIndentTwips;
    int			llFirstIndentTwips;
    PropertyMask	llParaPropertyMask;

    TextAttribute	llTextAttribute;
    PropertyMask	llTextAttributeMask;
    } ListLevel;

typedef enum ListLevelProperty
    {
    LLpropSTARTAT,
    LLpropSTYLE,
    LLpropJUSTIFY,
    LLpropFOLLOW,
    LLpropLEGAL,
    LLpropNORESTART,
    LLpropPICTURE,
    LLpropFBIAS,

    LLpropTEXT,
    LLpropTEMPLATEID,
    LLpropNUMBERS,

    LLpropOLD,
    LLpropPREV,
    LLpropPREVSPACE,
    LLpropINDENT,
    LLpropSPACE,

    LLprop_COUNT
    } ListLevelProperty;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docInitDocumentListLevel(	ListLevel *		ll );
extern void docCleanDocumentListLevel(	ListLevel *		ll );

extern int docCopyDocumentListLevel(	ListLevel *		to,
					const ListLevel *	from,
					int			copyIds,
					const int *		fontMap,
					const int *		colorMap,
					const int *		rulerMap );

extern int docListLevelSetStyle( ListLevel *			ll,
				const PropertyMask *		paraMask,
				const ParagraphProperties *	pp,
				const PropertyMask *		textMask,
				const TextAttribute *		ta );

extern int docListLevelSetText(	int *			pChanged,
				ListLevel *		ll,
				const char *		text,
				int			idx );

extern int docListLevelGetText(	char *			to,
				int			maxsize,
				const ListLevel *	ll,
				int			idx );

extern int docListLevelSetNumber(	int *			pChanged,
					ListLevel *		ll,
					int			number,
					int			idx );

extern int docListLevelInsertNumber(	ListLevel *		ll,
					int			number,
					int			idx );

extern int docListLevelDeleteNumber(	ListLevel *		ll,
					int			idx );

extern int docListLevelFormatParagraphNumber( char *		to,
					int			maxsize,
					int *			offsets,
					int			maxoffsets,
					int			ilvl,
					const int *		numberPath,
					const int *		startPath,
					const int *		formatPath,
					const ListLevel *	ll );

extern int docListLevelFormatLevelNumber(
				char *				target,
				int				maxsize,
				int				val,
				int				format );

extern int docDefaultListLevel(	ListLevel *			llTo,
				int				level,
				int				deftab,
				int				levelTemplateID,
				const PropertyMask *		taSetMask,
				const TextAttribute *		taSet );

extern int docListLevelGetField( int *				pConstOff,
				int *				pConstLen,
				int *				pConstChars,
				int *				pLevel,
				const ListLevel *		ll,
				int				field );

extern int docListLevelToRtfStrings(
				MemoryBuffer *			mbtext,
				MemoryBuffer *			mbnumbers,
				const ListLevel *		ll );

extern int docListLevelFromRtfStrings(
				ListLevel *			ll,
				const MemoryBuffer *		mbtext,
				const MemoryBuffer *		mbnumbers );

extern void docListDocumentListLevel(	int			n,
					const ListLevel *	ll );

extern int docSetListLevelProperty(	ListLevel *		ll,
					int			prop,
					int			val );

extern int docListLevelApplyTextAttribute(
					ListLevel *		ll,
					const PropertyMask *	taSetMask,
					const TextAttribute *	taSet );

#   endif
