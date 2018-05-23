/************************************************************************/
/*									*/
/*  Management of the lists in the list table of a document.		*/
/*									*/
/************************************************************************/

#   include	"docBaseConfig.h"

#   include	<appDebugon.h>

#   include	"docDocumentList.h"

/************************************************************************/
/*									*/
/*  Individual lists.							*/
/*									*/
/************************************************************************/

void docInitDocumentList(	DocumentList *	dl )
    {
    int		i;

    dl->dlListID= -1;
    dl->dlListTemplateID= -1;
    dl->dlListHybrid= 1;
    dl->dlRestartPerSect= 0;
    utilInitMemoryBuffer( &(dl->dlListName) );

    dl->dlListStyleID= -1;
    utilInitMemoryBuffer( &(dl->dlListStyleName) );

    dl->dlLevelCount= 0;
    for ( i= 0; i < DLmaxLEVELS; i++ )
	{ docInitDocumentListLevel( &(dl->dlLevels[i]) );	}

    return;
    }

void docCleanDocumentList(	DocumentList *	dl )
    {
    int		i;

    utilCleanMemoryBuffer( &(dl->dlListName) );
    utilCleanMemoryBuffer( &(dl->dlListStyleName) );

    for ( i= 0; i < dl->dlLevelCount; i++ )
	{ docCleanDocumentListLevel( &(dl->dlLevels[i]) );	}

    return;
    }

int docCopyDocumentList(	DocumentList *		to,
				const DocumentList *	from,
				int			copyIds,
				const int *		fontMap,
				const int *		colorMap,
				const int *		rulerMap )
    {
    int			rval= 0;
    int			i;

    if  ( copyIds )
	{
	if  ( utilCopyMemoryBuffer( &(to->dlListName), &(from->dlListName) ) )
	    { LDEB(1); return -1;	}
	if  ( utilCopyMemoryBuffer( &(to->dlListStyleName),
						&(from->dlListStyleName) ) )
	    { LDEB(1); return -1;	}
	}

    for ( i= 0; i < from->dlLevelCount && i < to->dlLevelCount; i++ )
	{
	if  ( docCopyDocumentListLevel( &(to->dlLevels[i]),
				    &(from->dlLevels[i]),
				    copyIds, fontMap, colorMap, rulerMap ) )
	    { LDEB(i); rval= -1; goto ready;	}
	}

    while( to->dlLevelCount < from->dlLevelCount )
	{
	i= to->dlLevelCount;

	docInitDocumentListLevel( &(to->dlLevels[i]) );

	if  ( docCopyDocumentListLevel( &(to->dlLevels[i]),
				    &(from->dlLevels[i]),
				    copyIds, fontMap, colorMap, rulerMap ) )
	    { LDEB(i); rval= -1; goto ready;	}

	to->dlLevelCount++;
	}

    while( to->dlLevelCount > from->dlLevelCount )
	{
	to->dlLevelCount--;

	docCleanDocumentListLevel( &(to->dlLevels[to->dlLevelCount]) );
	docInitDocumentListLevel( &(to->dlLevels[to->dlLevelCount]) );
	}

    if  ( copyIds )
	{
	to->dlListID= from->dlListID;
	to->dlListTemplateID= from->dlListTemplateID;
	}

    to->dlListStyleID= from->dlListStyleID;
    to->dlListHybrid= from->dlListHybrid;
    to->dlRestartPerSect= from->dlRestartPerSect;

  ready:

    return rval;
    }

int docCopyDocumentListSameDocument(	DocumentList *		to,
					const DocumentList *	from )
    {
    const int		copyIds= 1;
    const int * const	fontMap= (const int *)0;
    const int * const	colorMap= (const int *)0;
    const int * const	rulerMap= (const int *)0;

    return docCopyDocumentList( to, from,
					copyIds, fontMap, colorMap, rulerMap );
    }

int docDocumentListAddLevel(	DocumentList *		dl,
				const ListLevel *	ll,
				int			copyIds,
				const int *		fontMap,
				const int *		colorMap,
				const int *		rulerMap )
    {
    if  ( dl->dlLevelCount < 0 || dl->dlLevelCount >= DLmaxLEVELS )
	{ LLDEB(dl->dlLevelCount,DLmaxLEVELS); return -1;	}

    if  ( docCopyDocumentListLevel( &(dl->dlLevels[dl->dlLevelCount]), ll,
				    copyIds, fontMap, colorMap, rulerMap ) )
	{ LDEB(dl->dlLevelCount); return -1;	}

    dl->dlLevelCount++;

    return 0;
    }

int docSetListProperty(		DocumentList *		dl,
				int			prop,
				int			val )
    {
    switch( prop )
	{
	case DLpropLISTID:
	    dl->dlListID= val;
	    break;

	case DLpropTEMPLATEID:
	    dl->dlListTemplateID= val;
	    break;

	case DLpropHYBRID:
	    dl->dlListHybrid= val;
	    break;

	case DLpropRESTARTHDN:
	    dl->dlRestartPerSect= val != 0;
	    break;

	case DLpropSTYLEID:
	    dl->dlListStyleID= val;
	    break;

	/* DLpropNAME */
	/* DLpropLEVELS */

	default:
	    LDEB(prop); return -1;
	}

    return 0;
    }

