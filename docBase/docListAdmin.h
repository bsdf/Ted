/************************************************************************/
/*									*/
/*  Administration of the lists in a document.				*/
/*									*/
/*  Refer to page 21 of the RTF 1.7 document.				*/
/*									*/
/************************************************************************/

#   ifndef	DOC_LIST_ADMIN_H
#   define	DOC_LIST_ADMIN_H

#   include	"docListTable.h"
#   include	"docListOverrideTable.h"

struct ListLevel;

typedef struct ListAdmin
    {
    DocumentListTable	laListTable;
    ListOverrideTable	laListOverrideTable;
    } ListAdmin;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docInitListAdmin(	ListAdmin *		la );
extern void docCleanListAdmin(	ListAdmin *		la );

extern int docCopyListAdmin(	ListAdmin *		to,
				const ListAdmin *	from );

extern int docMakeOverrideForEveryList(	ListAdmin *	la );

extern int docMergeListAdmins(	ListAdmin *			laTo,
				const ListAdmin *		laFrom,
				const int *			lsUsed,
				int *				lsMap,
				const int *			listUsed,
				const int *			fontMap,
				const int *			colorMap,
				const int *			rulerMap );

extern int docGetListForStyle(	struct ListOverride **		pLo,
				struct DocumentList **		pDl,
				int				ls,
				const ListAdmin *		la );

extern int docGetListOfOverride(	struct ListOverride *		lo,
					const DocumentListTable *	dlt );

extern int docListGetFormatPath( int *				startPath,
				int *				formatPath,
				const struct ListLevel **	pLl,
				int				ilvl,
				const struct DocumentList *	dl,
				const struct ListOverride *	lo );

#   endif
