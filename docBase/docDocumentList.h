/************************************************************************/
/*									*/
/*  List table of a document.						*/
/*									*/
/*  Refer to page 21 of the RTF 1.7 document.				*/
/*									*/
/************************************************************************/

#   ifndef	DOC_DOCUMENT_LIST_H
#   define	DOC_DOCUMENT_LIST_H

#   include	"docListDepth.h"
#   include	"docListLevel.h"

typedef struct DocumentList
    {
    int			dlListID;
    int			dlListTemplateID;
    MemoryBuffer	dlListName;
    MemoryBuffer	dlListStyleName;
    int			dlListStyleID;

    int			dlListHybrid;
    int			dlRestartPerSect;

    ListLevel		dlLevels[DLmaxLEVELS];
    int			dlLevelCount;
    } DocumentList;

typedef enum DocumentListProperty
    {
    DLpropLISTID,
    DLpropTEMPLATEID,
    DLpropHYBRID,
    DLpropRESTARTHDN,
    DLpropSTYLEID,
    DLpropSTYLENAME,

    DLpropNAME,

    DLpropLEVELS,

    DLprop_COUNT
    } DocumentListProperty;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docInitDocumentList(	DocumentList *		dl );
extern void docCleanDocumentList(	DocumentList *		dl );

extern int docCopyDocumentList(	DocumentList *		to,
				const DocumentList *	from,
				int			copyIds,
				const int *		fontMap,
				const int *		colorMap,
				const int *		rulerMap );

extern int docDocumentListAddLevel(
				DocumentList *			dl,
				const ListLevel *		ll,
				int				copyIds,
				const int *			fontMap,
				const int *			colorMap,
				const int *			rulerMap );

extern int docCopyDocumentListSameDocument(	DocumentList *		to,
						const DocumentList *	from );

extern int docSetListProperty(	DocumentList *			dl,
				int				prop,
				int				val );

#   endif
