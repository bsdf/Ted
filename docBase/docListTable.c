/************************************************************************/
/*									*/
/*  Management of the list table of a document.				*/
/*									*/
/************************************************************************/

#   include	"docBaseConfig.h"

#   include	<stdlib.h>

#   include	<appDebugon.h>

#   include	"docDocumentList.h"
#   include	"docListTable.h"

void docInitListTable(		DocumentListTable *		dlt )
    {
    dlt->dltLists= (DocumentList *)0;
    dlt->dltListCount= 0;

    return;
    }

void docCleanListTable(		DocumentListTable *		dlt )
    {
    int		i;

    for ( i= 0; i < dlt->dltListCount; i++ )
	{ docCleanDocumentList( &(dlt->dltLists[i]) );	}

    if  ( dlt->dltLists )
	{ free( (void *)dlt->dltLists );	}

    return;
    }

int docCopyListTable(		DocumentListTable *		to,
				const DocumentListTable *	from )
    {
    int			i;

    if  ( to->dltListCount > from->dltListCount )
	{
	for ( i= from->dltListCount; i < to->dltListCount; i++ )
	    { docCleanDocumentList( &(to->dltLists[i]) );	}

	to->dltListCount= from->dltListCount;
	}

    if  ( to->dltListCount < from->dltListCount )
	{
	DocumentList *	fresh;

	fresh= (DocumentList *)realloc( to->dltLists,
				from->dltListCount* sizeof(DocumentList) );
	if  ( ! fresh )
	    { LXDEB(from->dltListCount,fresh); return -1;	}

	to->dltLists= fresh;

	while( to->dltListCount < from->dltListCount )
	    { docInitDocumentList( &(to->dltLists[to->dltListCount++]) ); }
	}

    for ( i= 0; i < from->dltListCount; i++ )
	{
	if  ( docCopyDocumentListSameDocument(
				&(to->dltLists[i]), &(from->dltLists[i]) ) )
	    { LDEB(i); return -1;	}
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  delete a list from the table.					*/
/*									*/
/************************************************************************/

int docDocumentListTableDeleteList(	DocumentListTable *	dlt,
					int			listIndex )
    {
    if  ( listIndex < 0 || listIndex >= dlt->dltListCount )
	{ LLDEB(listIndex,dlt->dltListCount); return -1;	}

    docCleanDocumentList( dlt->dltLists+ listIndex );
    docInitDocumentList( dlt->dltLists+ listIndex );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Add a list to the listtable.					*/
/*									*/
/************************************************************************/

int docDocumentListTableAddList( DocumentList **		pDl,
				DocumentListTable *		dlt,
				const DocumentList *		dl,
				int				copyIds,
				const int *			fontMap,
				const int *			colorMap,
				const int *			rulerMap )
    {
    DocumentList *	fresh;

    fresh= (DocumentList *)realloc( dlt->dltLists,
			    (dlt->dltListCount+ 1)* sizeof(DocumentList) );
    if  ( ! fresh )
	{ LXDEB(dlt->dltListCount,fresh); return -1;	}
    dlt->dltLists= fresh;

    fresh += dlt->dltListCount;

    docInitDocumentList( fresh );
    if  ( dl && docCopyDocumentList( fresh, dl,
					copyIds, fontMap, colorMap, rulerMap ) )
	{ LDEB(dlt->dltListCount); return -1;	}

    if  ( pDl )
	{ *pDl= fresh;	}

    return dlt->dltListCount++;
    }

int docMergeListIntoTable(	DocumentListTable *		dlt,
				const DocumentList *		dlNew,
				const int *			fontMap,
				const int *			colorMap,
				const int *			rulerMap )
    {
    DocumentList *	dl;
    int			i;
    const int		copyIds= 1;

    dl= dlt->dltLists;
    for ( i= 0; i < dlt->dltListCount; dl++, i++ )
	{
	int		level;

	if  ( dl->dlListID != dlNew->dlListID )
	    { continue;	}
	if  ( dl->dlListTemplateID != dlNew->dlListTemplateID )
	    { LLDEB(dl->dlListTemplateID,dlNew->dlListTemplateID);	}
	if  ( dl->dlListStyleID != dlNew->dlListStyleID )
	    { LLDEB(dl->dlListTemplateID,dlNew->dlListTemplateID);	}

	for ( level= dl->dlLevelCount; level < dlNew->dlLevelCount; level++ )
	    {
	    if  ( docDocumentListAddLevel( dl, dlNew->dlLevels+ level,
				    copyIds, fontMap, colorMap, rulerMap ) )
		{ LDEB(level); return -1;	}
	    }

	return i;
	}

    i= docDocumentListTableAddList( (DocumentList **)0, dlt, dlNew,
					copyIds, fontMap, colorMap, rulerMap );
    if  ( i < 0 )
	{ LLDEB(i,dlt->dltListCount); return -1;	}

    return i;
    }
