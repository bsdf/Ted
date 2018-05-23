/************************************************************************/
/*									*/
/*  Document Properties administration routines.			*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	<stdlib.h>

#   include	<docTextParticule.h>
#   include	<docDocumentList.h>
#   include	<docListOverride.h>

#   include	"docBuf.h"
#   include	"docDebug.h"
#   include	"docFind.h"
#   include	"docTreeNode.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Merge the font list of one document into that of another one.	*/
/*  Update the references accordingly.					*/
/*									*/
/*  a)  Scan all paragraphs to remember text attributes. This is done	*/
/*	with a 'Find' routine thet finds nothing.			*/
/*									*/
/*  1)  NOTE that the default color 0 is not really a color.		*/
/*  2)  As we do not want to look for colors everywhere, we also map	*/
/*	unused colors. This differs from the approach to fonts.		*/
/*									*/
/************************************************************************/

typedef struct ResourceUsed
    {
    int *	ruFontsUsed;
    int *	ruListStyleUsed;
    } ResourceUsed;

/*  a  */
static int docMergeScanParaAttributes(
				DocumentSelection *		ds,
				BufferItem *			paraNode,
				BufferDocument *		bd,
				const DocumentPosition *	dpFrom,
				void *				through )
    {
    ResourceUsed *		ru= (ResourceUsed *)through;

    TextParticule *		tp;
    int				part;
    int				errors= 0;

    tp= paraNode->biParaParticules;
    for ( part= 0; part < paraNode->biParaParticuleCount; part++, tp++ )
	{
	TextAttribute	ta;

	if  ( tp->tpTextAttrNr < 0 )
	    { errors++; tp->tpTextAttrNr= 0;	}

	docGetTextAttributeByNumber( &ta, bd, tp->tpTextAttrNr );

	ru->ruFontsUsed[ta.taFontNumber]= 1;
	}

    if  ( errors > 0 )
	{
	LDEB(errors);
	docListNode(0,paraNode,0);
	}

    if  ( paraNode->biParaListOverride > 0 )
	{ ru->ruListStyleUsed[paraNode->biParaListOverride]= 1;	}

    return 1;
    }

static int docMergeListTables(	BufferDocument *		bdTo,
				BufferDocument *		bdFrom,
				const int *			lsUsed,
				int *				lsMap,
				const int *			listUsed,
				const int *			fontMap,
				const int *			colorMap,
				const int *			rulerMap )
    {
    DocumentProperties *	dpTo= &(bdTo->bdProperties);
    const DocumentProperties *	dpFrom= &(bdFrom->bdProperties);
    ListAdmin *			laTo= dpTo->dpListAdmin;
    const ListAdmin *		laFrom= dpFrom->dpListAdmin;

    int				listStylesAdded;

    listStylesAdded= docMergeListAdmins( laTo, laFrom,
					    lsUsed, lsMap, listUsed,
					    fontMap, colorMap, rulerMap );
    if  ( listStylesAdded < 0 )
	{ LDEB(listStylesAdded); return -1;	}

    if  ( listStylesAdded > 0						&&
	  docClaimListNumberTrees( &(bdTo->bdBody.dtListNumberTrees),
			   laTo->laListOverrideTable.lotOverrideCount )	)
	{ LDEB(listStylesAdded); return -1;	}

    return 0;
    }

static void docCountListFontsUsed(	BufferDocument *	bdFrom,
					const int *		lsUsed,
					int *			listUsed,
					int *			fontUsed )
    {
    const DocumentProperties *	dpFrom= &(bdFrom->bdProperties);
    const ListAdmin *		laFrom= dpFrom->dpListAdmin;
    const DocumentListTable *	dltFrom= &(laFrom->laListTable);
    const ListOverrideTable *	lotFrom= &(laFrom->laListOverrideTable);

    const DocumentList *	dl;
    ListOverride *		lo;

    int				from;

    lo= lotFrom->lotOverrides;
    for ( from= 0; from < lotFrom->lotOverrideCount; lo++, from++ )
	{
	int				listIndex;

	int				level;
	const ListOverrideLevel *	lol;

	if  ( ! lsUsed[from] )
	    { continue;	}

	listIndex= docGetListOfOverride( lo, dltFrom );
	if  ( listIndex < 0 )
	    { LDEB(listIndex);		}
	else{ listUsed[listIndex]= 1;	}

	lol= lo->loLevels;
	for ( level= 0; level < lo->loLevelCount; lol++, level++ )
	    {
	    const ListLevel *	ll= &(lol->lolListLevel);

	    if  ( ! lol->lolOverrideFormat )
		{ continue;	}
	    if  ( ! PROPmaskISSET( &(ll->llTextAttributeMask),
						    TApropFONT_NUMBER ) )
		{ continue;	}

	    fontUsed[ll->llTextAttribute.taFontNumber]= 1;
	    }
	}

    dl= dltFrom->dltLists;
    for ( from= 0; from < dltFrom->dltListCount; dl++, from++ )
	{
	int			level;
	const ListLevel *	ll= dl->dlLevels;

	if  ( ! listUsed[from] )
	    { continue;	}

	for ( level= 0; level < dl->dlLevelCount; ll++, level++ )
	    {
	    if  ( ! PROPmaskISSET( &(ll->llTextAttributeMask),
						    TApropFONT_NUMBER ) )
		{ continue;	}

	    fontUsed[ll->llTextAttribute.taFontNumber]= 1;
	    }
	}

    return;
    }

int docMergeDocumentLists(	int **				pFontMap,
				int **				pListStyleMap,
				BufferDocument *		bdTo,
				BufferDocument *		bdFrom,
				const int *			colorMap,
				const int *			rulerMap )
    {
    int				rval= 0;

    DocumentProperties *	dpTo= &(bdTo->bdProperties);
    DocumentFontList *		dflTo= dpTo->dpFontList;

    const DocumentProperties *	dpFrom= &(bdFrom->bdProperties);
    const DocumentFontList *	dflFrom= dpFrom->dpFontList;

    const ListAdmin *		laFrom= dpFrom->dpListAdmin;
    const DocumentListTable *	dltFrom= &(laFrom->laListTable);
    const ListOverrideTable *	lotFrom= &(laFrom->laListOverrideTable);

    int *			fontMap= (int *)0;
    int *			fontUsed= (int *)0;

    int *			lsUsed= (int *)0;
    int *			lsMap= (int *)0;
    int *			listUsed= (int *)0;

    int				from;
    int				to;

    DocumentPosition		dpBeginFrom;
    DocumentSelection		dsIgnored;
    int				res;

    ResourceUsed		ru;

    /*****/

    if  ( dflFrom->dflFontCount > 0 )
	{
	fontMap= (int *)malloc( dflFrom->dflFontCount* sizeof( int ) );
	if  ( ! fontMap )
	    { LXDEB(dflFrom->dflFontCount,fontMap); rval= -1; goto ready; }
	fontUsed= (int *)malloc( dflFrom->dflFontCount* sizeof( int ) );
	if  ( ! fontUsed )
	    { LXDEB(dflFrom->dflFontCount,fontUsed); rval= -1; goto ready; }

	for ( from= 0; from < dflFrom->dflFontCount; from++ )
	    { fontMap[from]= -1; fontUsed[from]= 0; }
	}

    if  ( lotFrom->lotOverrideCount > 0 )
	{
	lsUsed= (int *)malloc( lotFrom->lotOverrideCount* sizeof(int) );
	if  ( ! lsUsed )
	    { LXDEB(lotFrom->lotOverrideCount,lsUsed); rval= -1; goto ready; }
	lsMap= (int *)malloc( lotFrom->lotOverrideCount* sizeof(int) );
	if  ( ! lsMap )
	    { LXDEB(lotFrom->lotOverrideCount,lsMap); rval= -1; goto ready; }

	for ( from= 0; from < lotFrom->lotOverrideCount; from++ )
	    { lsMap[from]= -1; lsUsed[from]= 0; }

	lsMap[0]= 0;
	}

    if  ( dltFrom->dltListCount > 0 )
	{
	listUsed= (int *)malloc( dltFrom->dltListCount* sizeof(int) );
	if  ( ! listUsed )
	    { LXDEB(dltFrom->dltListCount,listUsed); rval= -1; goto ready; }

	for ( from= 0; from < dltFrom->dltListCount; from++ )
	    { listUsed[from]= 0; }
	}

    /*****/

    /*  a  */
    ru.ruFontsUsed= fontUsed;
    ru.ruListStyleUsed= lsUsed;
    if  ( docDocumentHead( &dpBeginFrom, bdFrom ) )
	{ LDEB(1); rval= -1; goto ready;	}
    res= docFindFindNextInDocument( &dsIgnored, &dpBeginFrom, bdFrom,
				    docMergeScanParaAttributes, (void *)&ru );
    if  ( res != 1 )
	{ LDEB(res); rval= -1; goto ready;	}

    docCountListFontsUsed( bdFrom, lsUsed, listUsed, fontUsed );

    /*****/

    for ( from= 0; from < dflFrom->dflFontCount; from++ )
	{
	const DocumentFont *	dfFrom;
	DocumentFont *		dfTo;

	if  ( ! fontUsed || ! fontUsed[from] )
	    { continue;	}

	dfFrom= docFontListGetFontByNumber( dflFrom, from );
	if  ( ! dfFrom )
	    { continue;	}
	if  ( ! dfFrom->dfName )
	    { XDEB(dfFrom->dfName); continue;	}

	to= docMergeFontIntoFontlist( dflTo, dfFrom );
	if  ( to < 0 )
	    { LDEB(to); rval= -1; goto ready;	}

	fontMap[from]= to;

	dfTo= docFontListGetFontByNumber( dflTo, to );
	if  ( ! dfTo )
	    { XDEB(dfTo); continue;	}
	}

    /*****/

    if  ( docMergeListTables( bdTo, bdFrom, lsUsed, lsMap, listUsed,
						fontMap, colorMap, rulerMap ) )
	{ LDEB(1); rval= -1; goto ready;	}

    /*  steal */
    *pFontMap= fontMap; fontMap= (int *)0;
    *pListStyleMap= lsMap; lsMap= (int *)0;

  ready:

    if  ( fontMap )
	{ free( fontMap );	}
    if  ( fontUsed )
	{ free( fontUsed );	}

    if  ( lsMap )
	{ free( lsMap );	}
    if  ( lsUsed )
	{ free( lsUsed );	}

    if  ( listUsed )
	{ free( listUsed );	}

    return rval;
    }

int docMergeColorTables(	int **				pColorMap,
				BufferDocument *		bdTo,
				const BufferDocument *		bdFrom )
    {
    const int			avoidZero= 1;
    const int			maxColors= 256;

    DocumentProperties *	dpTo= &(bdTo->bdProperties);
    ColorPalette *		cpTo= dpTo->dpColorPalette;

    const DocumentProperties *	dpFrom= &(bdFrom->bdProperties);
    const ColorPalette *	cpFrom= dpFrom->dpColorPalette;

    return utilMergeColorPalettes( pColorMap, cpTo, cpFrom,
							avoidZero, maxColors );
    }

