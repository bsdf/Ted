/************************************************************************/
/*									*/
/*  Document statistics for the user.					*/
/*									*/
/************************************************************************/

#   ifndef		DOC_STATISTICS_H
#   define		DOC_STATISTICS_H

/************************************************************************/
/*									*/
/*  Statistics about a document. Used in the 'Document Properties'	*/
/*  dialog.								*/
/*									*/
/************************************************************************/

typedef struct DocumentStatistics
    {
    int		dsPageCount;
    int		dsParagraphCount;
    int		dsLineCount;
    int		dsWordCount;
    int		dsCharacterCount;
    } DocumentStatistics;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docInitDocumentStatistics(		DocumentStatistics *	ds );

#   endif	/*	DOC_STATISTICS_H	*/
