/************************************************************************/
/*									*/
/*  Definitions for finding patterns in the document and spelling	*/
/*  checking.								*/
/*									*/
/************************************************************************/

#   ifndef	DOC_COLLECT_STATISTICS_H
#   define	DOC_COLLECT_STATISTICS_H

#   include	<docStatistics.h>

struct BufferDocument;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docCollectDocumentStatistics( DocumentStatistics *		ds,
					const struct BufferDocument *	bd );

#   endif
