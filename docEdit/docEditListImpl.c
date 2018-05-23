/************************************************************************/
/*									*/
/*  Manipulate lists.							*/
/*									*/
/************************************************************************/

#   include	"docEditConfig.h"

#   include	<stdlib.h>

#   include	<appDebugon.h>

#   include	<docBuf.h>
#   include	<docDocumentList.h>
#   include	"docEditImpl.h"

/************************************************************************/
/*									*/
/*  Make a fresh default list level to be inserted in a document.	*/
/*									*/
/************************************************************************/

static int docFindFreeLevelTemplateId(
				const DocumentListTable *	dlt )
    {
    int		found= 1;
    int		retLevelTemplateID= -1;

    while( found )
	{
	int			i;
	const DocumentList *	dl;
	int			levelTemplateID= rand();

	found= 0;

	dl= dlt->dltLists;
	for ( i= 0; i < dlt->dltListCount; dl++, i++ )
	    {
	    int			level;
	    const ListLevel *	ll;

	    ll= dl->dlLevels;
	    for ( level= 0; level < dl->dlLevelCount; ll++, level++ )
		{
		if  ( ll->llTemplateID == levelTemplateID )
		    { found= 1; break;	}
		}

	    if  ( found )
		{ break;	}
	    }

	if  ( ! found )
	    { retLevelTemplateID= levelTemplateID; break; }
	}

    return retLevelTemplateID;
    }

/************************************************************************/
/*									*/
/*  Make a fresh default list to be inserted in a document.		*/
/*									*/
/*  0)  Restart from scratch.						*/
/*  1)  Make a unique list ID. The procedure is far from optimal, but	*/
/*	the number of lists in the document is small, this does not	*/
/*	really hurt.							*/
/*  2)  Make a unique template ID.					*/
/*  3)  Word does not set list style properties by default. (probably	*/
/*	never)								*/
/*  4)  Word makes anonymous lists.					*/
/*  5)  Word 2000+ makes hybrid lists by default.			*/
/*									*/
/************************************************************************/

static int docDefaultList(	DocumentList *			dlTo,
				const DocumentListTable *	dlt,
				int				deftab,
				const PropertyMask *		taSetMask,
				const TextAttribute *		taSet )
    {
    int			found;
    int			level;

    /*  0,3,4  */
    docCleanDocumentList( dlTo );
    docInitDocumentList( dlTo );

    /*  1  */
    found= 1;
    while( found )
	{
	int			i;
	const DocumentList *	dl;
	int			listID= rand();

	found= 0;

	dl= dlt->dltLists;
	for ( i= 0; i < dlt->dltListCount; dl++, i++ )
	    {
	    if  ( dl->dlListID == listID )
		{ found= 1; break; }
	    }

	if  ( ! found )
	    { dlTo->dlListID= listID;	}
	}

    /*  2  */
    found= 1;
    while( found )
	{
	int			i;
	const DocumentList *	dl;
	int			templateID= rand();

	found= 0;

	dl= dlt->dltLists;
	for ( i= 0; i < dlt->dltListCount; dl++, i++ )
	    {
	    if  ( dl->dlListTemplateID == templateID )
		{ found= 1; break; }
	    }

	if  ( ! found )
	    { dlTo->dlListTemplateID= templateID;	}
	}

    /*  5  */
    dlTo->dlListHybrid= 1;
    dlTo->dlRestartPerSect= 0;
    dlTo->dlLevelCount= DLmaxLEVELS;

    for ( level= 0; level < DLmaxLEVELS; level++ )
	{
	int	levelTemplateID= -1;

	if  ( dlTo->dlListHybrid )
	    { levelTemplateID= docFindFreeLevelTemplateId( dlt );	}

	if  ( docDefaultListLevel( dlTo->dlLevels+ level,
					level, deftab, levelTemplateID,
					taSetMask, taSet ) )
	    { LDEB(level); return -1;	}
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Insert a fresh list into the document and a list override that	*/
/*  refers to it.							*/
/*									*/
/************************************************************************/

int docNewList(			const DocumentList **		pDl,
				const struct ListOverride **	pLo,
				int				lsExample,
				BufferDocument *		bd,
				const PropertyMask *		taSetMask,
				const TextAttribute *		taSet )
    {
    DocumentProperties *	dp= &(bd->bdProperties);
    ListAdmin *			la= dp->dpListAdmin;
    DocumentListTable *		dlt= &(la->laListTable);
    ListOverrideTable *		lot= &(la->laListOverrideTable);

    int				ls;
    int				listIndex;

    DocumentList *		dl= (DocumentList *)0;
    struct ListOverride *	lo= (struct ListOverride *)0;

    const int			copyIds= 0;
    const int * const		fontMap= (const int *)0;
    const int * const		colorMap= (const int *)0;
    const int * const		rulerMap= (const int *)0;

    DocumentList *		dlExample= (DocumentList *)0;

    if  ( lsExample > 0							&&
	  docGetListOfParagraph( (struct ListOverride **)0, &dlExample,
						    lsExample, bd ) < 0	)
	{ LDEB(lsExample); return -1;	}

    listIndex= docDocumentListTableAddList( &dl, dlt, dlExample,
					copyIds, fontMap, colorMap, rulerMap );
    if  ( listIndex < 0 )
	{ LLDEB(listIndex,dlt->dltListCount); return -1;	}

    if  ( ! dlExample )
	{
	if  ( docDefaultList( dl, dlt, dp->dpTabIntervalTwips,
							taSetMask, taSet ) )
	    { LDEB(1); return -1;	}
	}

    ls= docListOverrideTableAddOverride( &lo, lot,
						-1, dl->dlListID, listIndex );
    if  ( ls < 0 )
	{ LLDEB(ls,lot->lotOverrideCount); return -1;	}

    if  ( pLo )
	{ *pLo= lo;	}
    if  ( pDl )
	{ *pDl= dl;	}

    return ls;
    }

