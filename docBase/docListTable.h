/************************************************************************/
/*									*/
/*  List table of a document.						*/
/*									*/
/************************************************************************/

#   ifndef	DOC_LIST_TABLE_H
#   define	DOC_LIST_TABLE_H

struct DocumentList;

typedef struct DocumentListTable
    {
    struct DocumentList *	dltLists;
    int				dltListCount;
    } DocumentListTable;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docInitListTable(	DocumentListTable *		dlt );
extern void docCleanListTable(	DocumentListTable *		dlt );

extern int docCopyListTable(	DocumentListTable *		to,
				const DocumentListTable *	from );

extern int docDocumentListTableAddList(
				struct DocumentList **		pDl,
				DocumentListTable *		dlt,
				const struct DocumentList *	dl,
				int				copyIds,
				const int *			fontMap,
				const int *			rulerMap,
				const int *			colorMap );

extern int docMergeListIntoTable( DocumentListTable *		dlt,
				const struct DocumentList *	dl,
				const int *			fontMap,
				const int *			rulerMap,
				const int *			colorMap );

extern int docDocumentListTableDeleteList(
					DocumentListTable *	dlt,
					int			listIndex );

#   endif
