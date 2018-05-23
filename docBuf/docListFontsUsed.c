/************************************************************************/
/*									*/
/*  List the fonts in a document.					*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	<stddef.h>

#   include	<uniUtf8.h>
#   include	"docListFonts.h"
#   include	<docDocumentList.h>
#   include	<docListOverride.h>
#   include	"docTreeNode.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Recursively retrieve the list of Document Fonts that is used in	*/
/*  a document.								*/
/*									*/
/************************************************************************/

typedef struct GetCharsUsed
    {
    DocumentFontList *			gcuDocumentFontList;
    } GetCharsUsed;

static int docCharsUsedGotSpan(	BufferDocument *		bd,
				BufferItem *			paraNode,
				int				textAttrNr,
				const TextAttribute *		ta,
				int				from,
				int				upto,
				void *				through )
    {
    GetCharsUsed *		gcu= (GetCharsUsed *)through;
    DocumentFontList *		dfl= gcu->gcuDocumentFontList;
    const char *		s;

    DocumentFont *		df;
    int				fontNumber= ta->taFontNumber;

    if  ( fontNumber < 0			||
	  fontNumber >= dfl->dflFontCount	)
	{
	if  ( fontNumber >= dfl->dflFontCount )
	    { LLDEB(fontNumber,dfl->dflFontCount);	}

	fontNumber= docGetDefaultFont( bd );
	}

    df= docFontListGetFontByNumber( dfl, fontNumber );
    if  ( ! df )
	{ LXDEB(fontNumber,df); return -1;	}

    if  ( from == upto )
	{
	if  ( utilIndexSetAdd( &(df->dfUnicodesUsed), 0x20 ) )
	    { XDEB(0x20); return -1;	}
	}

    s= (const char *)docParaString( paraNode, from );
    while( from < upto )
	{
	unsigned short			symbol;
	int				step;

	step= uniGetUtf8( &symbol, s );
	if  ( step < 1 )
	    { LDEB(step); return -1;	}

	if  ( symbol == 0xa0 )
	    { symbol= 0x20;	}

	if  ( utilIndexSetAdd( &(df->dfUnicodesUsed), symbol ) )
	    { LDEB(symbol); return -1;	}

	s += step; from += step;
	}

    return 0;
    }

static int docInludeFontOfAttribute(	const BufferDocument *	bd,
					const TextAttribute *	ta )
    {
    const DocumentFontList *		dfl= bd->bdProperties.dpFontList;
    DocumentFont *		df;

    df= docFontListGetFontByNumber( dfl, ta->taFontNumber );
    if  ( ! df )
	{ LXDEB(ta->taFontNumber,df); return -1;	}

    if  ( utilIndexSetAdd( &(df->dfUnicodesUsed), 0x20 ) )
	{ XDEB(0x20); return -1;	}

    return 0;
    }

static int docCharsListObject(	const BufferDocument *	bd,
				BufferItem *		paraNode,
				const TextAttribute *	ta,
				void *			through )
    {
    GetCharsUsed *		gcu= (GetCharsUsed *)through;
    DocumentFontList *		dfl= gcu->gcuDocumentFontList;
    DocumentFont *		df;

    df= docFontListGetFontByNumber( dfl, ta->taFontNumber );
    if  ( ! df )
	{ LXDEB(ta->taFontNumber,df); return -1;	}

    if  ( utilIndexSetAdd( &(df->dfUnicodesUsed), 0x20 ) )
	{ XDEB(0x20); return -1;	}

    return 0;
    }

int docGetCharsUsed(	BufferDocument *		bd )
    {
    int				i;
    DocumentFontList *		dfl= bd->bdProperties.dpFontList;

    const DocumentProperties *	dp= &(bd->bdProperties);
    const ListAdmin *		la= dp->dpListAdmin;
    const DocumentListTable *	dlt= &(la->laListTable);
    const ListOverrideTable *	lot= &(la->laListOverrideTable);

    ScanDocumentFonts		sdf;
    GetCharsUsed		gcu;

    const DocumentList *	dl;
    const ListOverride *	lo;

    utilDocFontListClearCharsUsed( dfl );
    docInitScanDocumentFonts( &sdf );

    gcu.gcuDocumentFontList= dfl;

    sdf.sdfListObjectFonts= (DocListObjectFonts)0;
    sdf.sdfDocListSpanFont= docCharsUsedGotSpan;
    sdf.sdfListObject= docCharsListObject;
    sdf.sdfThrough= &gcu;

    /*  a  */
    if  ( docListDocumentFonts( bd, &sdf ) )
	{ LDEB(1); return -1;;	}

    for ( i= 0; i < bd->bdStyleSheet.dssStyleCount; i++ )
	{
	const DocumentStyle *	ds= bd->bdStyleSheet.dssStyles[i];

	if  ( ! ds							||
	      ! PROPmaskISSET( &(ds->dsTextMask), TApropFONT_NUMBER ) )
	    { continue;	}

	/*  Include the font.. If this fails lets hope the style is not used */
	if  ( docInludeFontOfAttribute( bd, &(ds->dsTextAttribute) ) )
	    { XDEB(0x20); /* return -1; */	}
	}

    lo= lot->lotOverrides;
    for ( i= 0; i < lot->lotOverrideCount; lo++, i++ )
	{
	int				level;
	const ListOverrideLevel *	lol;

	lol= lo->loLevels;
	for ( level= 0; level < lo->loLevelCount; lol++, level++ )
	    {
	    const ListLevel *	ll= &(lol->lolListLevel);

	    if  ( ! lol->lolOverrideFormat				||
		  ! PROPmaskISSET( &(ll->llTextAttributeMask),
						TApropFONT_NUMBER )	)
		{ continue;	}

	    if  ( docInludeFontOfAttribute( bd, &(ll->llTextAttribute) ) )
		{ XDEB(0x20); return -1;	}
	    }
	}

    dl= dlt->dltLists;
    for ( i= 0; i < dlt->dltListCount; dl++, i++ )
	{
	int			level;
	const ListLevel *	ll= dl->dlLevels;

	for ( level= 0; level < dl->dlLevelCount; ll++, level++ )
	    {
	    if  ( ! PROPmaskISSET( &(ll->llTextAttributeMask),
						    TApropFONT_NUMBER ) )
		{ continue;	}

	    if  ( ll->llTextAttribute.taFontNumber < 0 )
		{
		LLLDEB(i,level,ll->llTextAttribute.taFontNumber);
		continue;
		}

	    if  ( docInludeFontOfAttribute( bd, &(ll->llTextAttribute) ) )
		{ XDEB(0x20); return -1;	}
	    }
	}

    return 0;
    }

