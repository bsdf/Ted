/************************************************************************/
/*									*/
/*  Description of a TOC field.						*/
/*									*/
/************************************************************************/

# ifndef DOC_TOC_FIELD_H
# define DOC_TOC_FIELD_H

#   include	<utilMemoryBuffer.h>

struct DocumentField;
struct FieldInstructions;

typedef enum TocType
    {
    TOCtypeTOC,
    TOCtypeSEQ,

    TOCtype_COUNT
    } TocType;

typedef struct StyleNameLevel
    {
    int		snlLevel;
    char *	snlStyleName;
    } StyleNameLevel;

typedef struct TocField
    {
			/* a,c,f */
    MemoryBuffer	tfIdentifierName;
			/* b */
    MemoryBuffer	tfMarkName;
			/* p */
    MemoryBuffer	tfSeparatorBytes;
			/* s */
    MemoryBuffer	tfSeqName;
			/* d */
    MemoryBuffer	tfSeqSeparatorBytes;
			/* t */
    MemoryBuffer	tfStyleNamesBytes;

			/* include what */
    unsigned char	tfType;

    unsigned char	tfUseTcEntries;		/* \\l */
    unsigned char	tfEntryLevel0;
    unsigned char	tfEntryLevel1;

    unsigned char	tfUseStyleNames;	/* \\t */
    unsigned char	tfUseStyleLevels;	/* \\o */
    unsigned char	tfStyleLevel0;
    unsigned char	tfStyleLevel1;
    unsigned char	tfUseOutlineLevels;	/* \\u */

			/* presentation */
    unsigned char	tfIncludeSeqLabelAndNumber;
    unsigned char	tfHyperlinks;
    unsigned char	tfWebNoNumber;
    unsigned char	tfPreserveTabs;
    unsigned char	tfPreserveNewlines;
    unsigned char	tfNLevel0;
    unsigned char	tfNLevel1;

			/* derived */
    StyleNameLevel *	tfStyleNameLevels;
    int			tfStyleNameLevelCount;
    } TocField;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docInitTocField(	TocField *			tf );
extern void docCleanTocField(	TocField *			tf );

extern int docCopyTocField(	TocField *			to,
				const TocField *		from );

extern int docFieldGetToc(	TocField *			tf,
				const struct DocumentField *	df );

extern int docTocFieldSetTocInstructions(
				struct FieldInstructions *	fi,
				const TocField *		tf );

#   endif
