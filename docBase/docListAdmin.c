/************************************************************************/
/*									*/
/*  Buffer administration routines.					*/
/*									*/
/************************************************************************/

#   include	"docBaseConfig.h"

#   include	<appDebugon.h>

#   include	"docListAdmin.h"
#   include	"docDocumentList.h"
#   include	"docListOverride.h"

/************************************************************************/
/*									*/
/*  Initialise Document Properties.					*/
/*									*/
/************************************************************************/

void docInitListAdmin(	ListAdmin *	la )
    {
    docInitListTable( &(la->laListTable) );
    docInitListOverrideTable( &(la->laListOverrideTable) );

    }

void docCleanListAdmin(	ListAdmin *	la )
    {
    docCleanListTable( &(la->laListTable) );
    docCleanListOverrideTable( &(la->laListOverrideTable) );

    return;
    }

int docCopyListAdmin(	ListAdmin *		to,
			const ListAdmin *	from )
    {
    if  ( docCopyListTable( &(to->laListTable), &(from->laListTable) ) )
	{ LDEB(1); return -1;	}

    if  ( docCopyListOverrideTable( &(to->laListOverrideTable),
					    &(from->laListOverrideTable) ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  As at least MS-Word 2000 crashes on lists without an override,	*/
/*  and on the other hand staroffice/openoffice creates them under	*/
/*  certain circumstances: Make an empty override for lists without	*/
/*  overrides.								*/
/*									*/
/************************************************************************/

int docMakeOverrideForEveryList(	ListAdmin *	la )
    {
    const DocumentListTable *	dlt= &(la->laListTable);
    const DocumentList *	dl;

    int				li;

    ListOverrideTable *		lot= &(la->laListOverrideTable);

    dl= dlt->dltLists;
    for ( li= 0; li < dlt->dltListCount; dl++, li++ )
	{
	int			ov;
	const ListOverride *	lo;

	lo= lot->lotOverrides;
	for ( ov= 0; ov < lot->lotOverrideCount; lo++, ov++ )
	    {
	    if  ( lo->loIndex < 1 )
		{ continue;	}

	    if  ( lo->loListID == dl->dlListID )
		{ break; }
	    }

	if  ( ov >= lot->lotOverrideCount )
	    {
	    int			ls;

	    ls= docListOverrideTableAddOverride( (ListOverride **)0, lot,
						    -1, dl->dlListID, li );
	    if  ( ls < 0 )
		{ LDEB(ls); return -1;	}
	    }
	}

    return 0;
    }

int docMergeListAdmins(	ListAdmin *			laTo,
			const ListAdmin *		laFrom,
			const int *			lsUsed,
			int *				lsMap,
			const int *			listUsed,
			const int *			fontMap,
			const int *			colorMap,
			const int *			rulerMap )
    {
    const DocumentListTable *	dltFrom= &(laFrom->laListTable);
    DocumentListTable *		dltTo= &(laTo->laListTable);
    const ListOverrideTable *	lotFrom= &(laFrom->laListOverrideTable);
    ListOverrideTable *		lotTo= &(laTo->laListOverrideTable);
    const DocumentList *	dl;
    const ListOverride *	lo;

    int				from;
    int				listStylesAdded= 0;

    lo= lotFrom->lotOverrides;
    for ( from= 0; from < lotFrom->lotOverrideCount; lo++, from++ )
	{
	int	oldCount= lotTo->lotOverrideCount;

	int	listStyle;

	if  ( ! lsUsed[from] )
	    { continue;	}

	listStyle= docMergeListOverrideIntoTable( lotTo, lo,
						fontMap, colorMap, rulerMap );
	if  ( listStyle < 0 )
	    { LDEB(listStyle); return -1;	}
	lsMap[from]= listStyle;

	if  ( listStyle >= oldCount )
	    { listStylesAdded++;	}
	}

    dl= dltFrom->dltLists;
    for ( from= 0; from < dltFrom->dltListCount; dl++, from++ )
	{
	int	listIndex;

	if  ( ! listUsed[from] )
	    { continue;	}

	listIndex= docMergeListIntoTable( dltTo, dltFrom->dltLists+ from,
						fontMap, colorMap, rulerMap );
	if  ( listIndex < 0 )
	    { LLDEB(from,listIndex); return -1;	}
	}

    return listStylesAdded;
    }

int docGetListOfOverride(	ListOverride *			lo,
				const DocumentListTable *	dlt )
    {
    int				li;
    const DocumentList *	dl;

    if  ( lo->loListIndex >= 0 )
	{ return lo->loListIndex;	}

    dl= dlt->dltLists;
    for ( li= 0; li < dlt->dltListCount; dl++, li++ )
	{
	if  ( dl->dlListID == lo->loListID )
	    { lo->loListIndex= li; return li; }
	}

    LDEB(lo->loListID);
    return -1;
    }

int docGetListForStyle(		ListOverride **			pLo,
				DocumentList **			pDl,
				int				ls,
				const ListAdmin *		la )
    {
    const ListOverrideTable *	lot= &(la->laListOverrideTable);
    const DocumentListTable *	dlt= &(la->laListTable);

    ListOverride *		lo;
    DocumentList *		dl= (DocumentList *)0;

    /*  1  */
    if  ( ls < 0 || ls >= lot->lotOverrideCount )
	{ LLDEB(ls,lot->lotOverrideCount); return -1;	}

    /*  2  */
    lo= lot->lotOverrides+ ls;

    /*  3,4  */
    if  ( lo->loListIndex < 0 && docGetListOfOverride( lo, dlt ) < 0 )
	{ LLDEB(lo->loListIndex,dlt->dltListCount); return -1; }

    dl= dlt->dltLists+ lo->loListIndex;

    if  ( pLo )
	{ *pLo= lo;	}
    if  ( pDl )
	{ *pDl= dl; 	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Determine the path of number styles and formatting instructions	*/
/*  to a certain level.							*/
/*									*/
/************************************************************************/

int docListGetFormatPath(	int *			startPath,
				int *			formatPath,
				const ListLevel **	pLl,
				int			ilvl,
				const DocumentList *	dl,
				const ListOverride *	lo )
    {
    int			level= 0;
    const ListLevel *	dllFound= (const ListLevel *)0;

    if  ( dl->dlLevelCount < 1			||
	  dl->dlLevelCount > DLmaxLEVELS	)
	{ LLDEB(dl->dlLevelCount,DLmaxLEVELS); return -1; }

    while( level < dl->dlLevelCount )
	{
	const ListLevel *		ll= (const ListLevel *)0;
	const ListOverrideLevel *	lol= (const ListOverrideLevel *)0;

	int				startAt= 0;

	if  ( level < lo->loLevelCount )
	    { lol= &(lo->loLevels[level]);	}
	if  ( level < dl->dlLevelCount )
	    { ll= &(dl->dlLevels[level]);	}

	if  ( ll )
	    { startAt= ll->llStartAt;	}
	if  ( lol && lol->lolOverrideStartAt )
	    { startAt= lol->lolListLevel.llStartAt;	}

	if  ( lol && lol->lolOverrideFormat )
	    { ll= &(lol->lolListLevel);	}

	if  ( startPath )
	    { startPath[level]= startAt;		}
	if  ( formatPath && ll )
	    { formatPath[level]= ll->llNumberStyle;	}

	if  ( level == ilvl )
	    { dllFound= ll;	}

	level++;
	}

    if  ( ! dllFound )
	{ XDEB(dllFound); return -1; }

    *pLl= dllFound;
    return 0;
    }

