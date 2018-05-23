/************************************************************************/
/*									*/
/*  Evaluate TOC fields.						*/
/*									*/
/************************************************************************/

#   include	<docTocField.h>
#   include	<docBlockFrame.h>
#   include	<docListDepth.h>

#   ifndef	DOC_CALCULATE_TOC_H
#   define	DOC_CALCULATE_TOC_H

typedef struct TocEntry
    {
    int				teLevel;
    const DocumentField *	teField;
    DocumentSelection		teDsInside;
    DocumentSelection		teDsAround;
    int				tePart0;
    int				tePart1;
    const MemoryBuffer *	teMarkName;
    int				teNumbered;
    } TocEntry;

/************************************************************************/

typedef struct CalculateToc
    {
				/**
				 *  The intermediate document holding the 
				 *  table of contents. Finally we will 
				 *  insert it in the document.
				 */
    BufferDocument *		ctBdToc;
				/**
				 *  The document that we index and that will 
				 *  recieve the table of contents.
				 */
    BufferDocument *		ctBdDoc;
				/**
				 *  The section that will hold the table of 
				 *  contents.
				 */
    struct BufferItem *		ctSectNode;
    DocumentField *		ctDfTocTo;
    TocField			ctTocField;
    BlockFrame			ctBlockFrame;
    ParagraphProperties		ctRefPP;
    TextAttribute		ctTextAttribute;
    int				ctDefaultTextAttributeNumber;
    const DocumentStyle *	ctLevelStyles[PPoutline_COUNT];
    int				ctLevelAttributeNumbers[PPoutline_COUNT];
				/**
				 *  The numbers of the styles that are 
				 *  listed in the table of contents.
				 */
    IndexSet			ctStyleNumbers;

    TocEntry *			ctEntries;
    int				ctEntryCount;
    int				ctDepth;

    unsigned char *		ctStyleLevels;
    int				ctStyleLevelCount;
    } CalculateToc;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docInitCalculateToc(	CalculateToc *	ct );
extern void docCleanCalculateToc(	CalculateToc *	ct );

extern int docCollectTocInput(	CalculateToc *			ct );

#   endif /*	DOC_CALCULATE_TOC_H	*/
