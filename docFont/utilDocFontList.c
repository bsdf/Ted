/************************************************************************/
/*									*/
/*  Font administration for a document.					*/
/*									*/
/************************************************************************/

#   include	"docFontConfig.h"

#   include	<stdlib.h>
#   include	<string.h>

#   include	<appDebugon.h>

#   include	"utilDocFontList.h"
#   include	<textOfficeCharset.h>
#   include	<psDocumentFontStyle.h>

/************************************************************************/
/*									*/
/*  Initialise a Font List.						*/
/*  Clean a Font List.							*/
/*  Copy a Font List.							*/
/*									*/
/************************************************************************/

void docInitFontList(	DocumentFontList *	dfl )
    {
    dfl->dflFontCount= 0;
    dfl->dflFontPages= (DocumentFont **)0;
    dfl->dflFontPageCount= 0;
    dfl->dflFontSortIndex= (DocFontSort *)0;
    }

void docCleanFontList(	DocumentFontList *	dfl )
    {
    int		page;
    int		i;

    for ( page = 0; page < dfl->dflFontPageCount; page++ )
	{
	DocumentFont *	fontPage= dfl->dflFontPages[page];

	if  ( fontPage )
	    {
	    for ( i= 0; i < DFL_PGSZ; i++ )
		{ docCleanDocumentFont( fontPage+ i );	}

	    free( fontPage );
	    }
	}

    if  ( dfl->dflFontPages )
	{ free( dfl->dflFontPages );	}

    if  ( dfl->dflFontSortIndex )
	{ free( dfl->dflFontSortIndex );	}

    return;
    }

DocumentFont * docFontListGetFontByNumber(
				    const DocumentFontList *	dfl,
				    int				n )
    {
    int			page= n/ DFL_PGSZ;
    DocumentFont *	fontPage;

    if  ( n < 0 || page >= dfl->dflFontPageCount )
	{ return (DocumentFont *)0;	}

    fontPage= dfl->dflFontPages[page];
    if  ( ! fontPage )
	{ return (DocumentFont *)0;	}

    if  ( fontPage[n%DFL_PGSZ].dfDocFontNumber != n )
	{ return (DocumentFont *)0;	}

    return &(fontPage[n%DFL_PGSZ]);
    }

static DocumentFont *	docFontListClaimFont(
					DocumentFontList *		dfl,
					int				n )
    {
    int			page= n/ DFL_PGSZ;
    DocumentFont *	fontPage;

    if  ( dfl->dflFontSortIndex )
	{ free( dfl->dflFontSortIndex );	}
    dfl->dflFontSortIndex= (DocFontSort *)0;

    if  ( page >= dfl->dflFontPageCount )
	{
	int	newPageCount= page+ 1;

	DocumentFont **	fresh= (DocumentFont **)realloc( dfl->dflFontPages,
				    newPageCount* sizeof(DocumentFont *) );

	if  ( ! fresh )
	    { LXDEB(n,fresh); return (DocumentFont *)0;	}
	dfl->dflFontPages= fresh;

	while( dfl->dflFontPageCount < newPageCount )
	    { fresh[dfl->dflFontPageCount++]= (DocumentFont *)0;	}
	}

    fontPage= dfl->dflFontPages[page];
    if  ( ! fontPage )
	{
	int		i;

	dfl->dflFontPages[page]= fontPage=
		    (DocumentFont *)malloc( DFL_PGSZ* sizeof(DocumentFont) );
	if  ( ! fontPage )
	    { XDEB(fontPage); return (DocumentFont *)0;	}

	for ( i= 0; i < DFL_PGSZ; i++ )
	    { docInitDocumentFont( fontPage+ i );	}
	}

    if  ( dfl->dflFontCount <= n )
	{ dfl->dflFontCount= n+ 1;	}

    return fontPage+ ( n % DFL_PGSZ );
    }


int docCopyFontList(	DocumentFontList *		to,
			const DocumentFontList *	from )
    {
    int				rval= 0;

    int				i;
    DocumentFontList		copy;

    docInitFontList( &copy );

    for ( i = 0; i < from->dflFontCount; i++ )
	{
	DocumentFont *		dfTo;
	const DocumentFont *	dfFrom;

	dfFrom= docFontListGetFontByNumber( from, i );
	if  ( ! dfFrom )
	    { continue;	}
	dfTo= docFontListClaimFont( &copy, i );
	if  ( ! dfTo )
	    { XDEB(dfTo); rval= -1; goto ready;	}

	if  ( docCopyDocumentFont( dfTo, dfFrom ) )
	    { LDEB(i); rval= -1; goto ready;	}
	}

    docCleanFontList( to );
    *to= copy; docInitFontList( &copy ); /* steal contents */

  ready:
    docCleanFontList( &copy );

    return rval;
    }

/************************************************************************/

static int docFontListFindOpenSlot(
				const DocumentFontList *	dfl )
    {
    int		page;

    for ( page = 0; page < dfl->dflFontPageCount; page++ )
	{
	int		i;
	DocumentFont *	fontPage= dfl->dflFontPages[page];

	if  ( ! fontPage )
	    { return DFL_PGSZ* page;	}

	for ( i= 0; i < DFL_PGSZ; i++ )
	    {
	    if  ( fontPage[i].dfDocFontNumber < 0 )
		{ return DFL_PGSZ* page+ i;	}
	    }
	}

    return dfl->dflFontCount;
    }

/************************************************************************/
/*									*/
/*  Insert a font. The font comes from the document as it is read.	*/
/*									*/
/************************************************************************/

DocumentFont * utilDocFontListInsertFont(	DocumentFontList *	dfl,
						const DocumentFont *	dfSet )
    {
    DocumentFont *	dfTo= (DocumentFont *)0;
    int			n= docFontListFindOpenSlot( dfl );

    dfTo= docFontListClaimFont( dfl, n );
    if  ( ! dfTo )
	{ XDEB(dfTo); goto failed;	}

    if  ( docCopyDocumentFont( dfTo, dfSet ) )
	{ LDEB(1); goto failed;	}

    dfTo->dfDocFontNumber= n;

    return dfTo;

  failed:

    if  ( dfTo )
	{ docCleanDocumentFont( dfTo );	}

    return (DocumentFont *)0;
    }

/************************************************************************/

static int docFindFontByName(	DocumentFontList *	dflTo,
				const char *		fontFamilyName )
    {
    int		i;

    for ( i= 0; i < dflTo->dflFontCount; i++ )
	{
	const DocumentFont *	dfTo= docFontListGetFontByNumber( dflTo, i );
	if  ( ! dfTo || dfTo->dfDocFontNumber < 0 )
	    { continue;	} 

	if  ( ! strcmp( dfTo->dfName, fontFamilyName )	)
	    { return i;	}
	}

    return -1;
    }

static int docMakeFontByName(	DocumentFontList *	dflTo,
				const char *		fontFamilyName )
    {
    int			fontFamilyStyle;
    DocumentFont	dfNew;
    DocumentFont *	dfTo= (DocumentFont *)0;
    int			rval= -1;

    fontFamilyStyle= psFontFamilyStyle( fontFamilyName );

    docInitDocumentFont( &dfNew );

    if  ( docFontSetFamilyStyle( &dfNew, fontFamilyStyle ) )
	{ LDEB(fontFamilyStyle); goto ready;	}
    if  ( docFontSetFamilyName( &dfNew, fontFamilyName ) )
	{ SDEB(fontFamilyName); goto ready;	}

    dfTo= utilDocFontListInsertFont( dflTo, &dfNew );
    if  ( ! dfTo )
	{ LDEB(dflTo->dflFontCount); goto ready;	}
    rval= dfTo->dfDocFontNumber;

  ready:

    docCleanDocumentFont( &dfNew );

    return rval;
    }

int docGetFontByName(	DocumentFontList *	dflTo,
			const char *		fontFamilyName )
    {
    int			rval= docFindFontByName( dflTo, fontFamilyName );

    if  ( rval < 0 )
	{
	rval= docMakeFontByName( dflTo, fontFamilyName );
	if  ( rval < 0 )
	    { SLDEB(fontFamilyName,rval); return rval;	}
	}

    return rval;
    }

int docMergeFontIntoFontlist(	DocumentFontList *	dflTo,
				const DocumentFont *	dfFrom )
    {
    DocumentFont *	dfTo;
    int			rval= -1;

    rval= docFindFontByName( dflTo, dfFrom->dfName );
    if  ( rval >= 0 )
	{ return rval;	}

    dfTo= utilDocFontListInsertFont( dflTo, dfFrom );
    if  ( ! dfTo )
	{ XDEB(dfTo); return -1;	}

    utilCopyFontFaceMatches( dfTo, dfFrom );

    return dfTo->dfDocFontNumber;
    }

/************************************************************************/
/*									*/
/*  Add a font from a PostScript font list to the document font list.	*/
/*									*/
/************************************************************************/

DocumentFont * utilAddFontToDocList(
				DocumentFontList *		dfl,
				const char *			name,
				int				styleInt,
				int				pitch )
    {
    int				fontNumber;
    DocumentFont *		df;

    fontNumber= docGetFontByName( dfl, name );
    if  ( fontNumber < 0 )
	{ SLDEB(name,fontNumber); return (DocumentFont *)0; }

    df= docFontListGetFontByNumber( dfl, fontNumber );
    if  ( ! df )
	{ XDEB(df); return df;	}

    df->dfStyleInt= styleInt;
    df->dfPitch= pitch;

    return df;
    }

static const DocumentFontList * dflsort;
static int utilFontCompareFontIdx(	const void * vdfs1,
					const void * vdfs2 )
    {
    const DocFontSort *		dfs1= (const DocFontSort *)vdfs1;
    const DocFontSort *		dfs2= (const DocFontSort *)vdfs2;

    const DocumentFont *	df1;
    const DocumentFont *	df2;

    df1= docFontListGetFontByNumber( dflsort, dfs1->arrayIndex );
    df2= docFontListGetFontByNumber( dflsort, dfs2->arrayIndex );

    if  ( ! df1 && ! df2 )
	{ return  0;	}
    if  (   df1 && ! df2 )
	{ return -1;	}
    if  ( ! df1 &&   df2 )
	{ return +1;	}

    return strcmp( df1->dfName, df2->dfName );
    }

static int utilDocumentFontListBuildSortIndex(	DocumentFontList *	dfl )
    {
    int		idx;

    dfl->dflFontSortIndex= (DocFontSort *)malloc( dfl->dflFontCount* sizeof(DocFontSort) );
    if  ( ! dfl->dflFontSortIndex )
	{ LXDEB(dfl->dflFontCount,dfl->dflFontSortIndex); return -1;	}

    for ( idx= 0; idx < dfl->dflFontCount; idx++ )
	{
	dfl->dflFontSortIndex[idx].arrayIndex= idx;
	dfl->dflFontSortIndex[idx].sortIndex= idx;
	}

    dflsort= dfl;
    qsort( dfl->dflFontSortIndex, dfl->dflFontCount,
			sizeof(DocFontSort), utilFontCompareFontIdx );
    dflsort= (const DocumentFontList *)0;

    for ( idx= 0; idx < dfl->dflFontCount; idx++ )
	{
	int		aidx= dfl->dflFontSortIndex[idx].arrayIndex;

	dfl->dflFontSortIndex[aidx].sortIndex= idx;
	}

    return 0;
    }

const DocumentFont * utilDocumentFontListGetFontBySortIndex(
						DocumentFontList *	dfl,
						int			idx )
    {
    if  ( idx < 0 || idx >= dfl->dflFontCount )
	{ LLDEB(idx,dfl->dflFontCount); return (const DocumentFont *)0;	}

    if  ( ! dfl->dflFontSortIndex			&&
	  utilDocumentFontListBuildSortIndex( dfl )	)
	{ LDEB(1); return (const DocumentFont *)0;	}

    return docFontListGetFontByNumber( dfl,
				    dfl->dflFontSortIndex[idx].arrayIndex );
    }

int utilDocumentFontListGetSortIndex(	DocumentFontList *	dfl,
					int			aidx )
    {
    if  ( aidx < 0 || aidx >= dfl->dflFontCount )
	{ LLDEB(aidx,dfl->dflFontCount); return -1;	}

    if  ( ! dfl->dflFontSortIndex			&&
	  utilDocumentFontListBuildSortIndex( dfl )	)
	{ LDEB(1); return -1;	}

    return dfl->dflFontSortIndex[aidx].sortIndex;
    }

int utilDocumentFontListGetArrayIndex(	DocumentFontList *	dfl,
					int			sidx )
    {
    if  ( sidx < 0 || sidx >= dfl->dflFontCount )
	{ LLDEB(sidx,dfl->dflFontCount); return -1;	}

    if  ( ! dfl->dflFontSortIndex			&&
	  utilDocumentFontListBuildSortIndex( dfl )	)
	{ LDEB(1); return -1;	}

    return dfl->dflFontSortIndex[sidx].arrayIndex;
    }

void utilDocFontListClearCharsUsed(	DocumentFontList *	dfl )
    {
    int				i;

    for ( i= 0; i < dfl->dflFontCount; i++ )
	{
	DocumentFont *	df= docFontListGetFontByNumber( dfl, i );

	if  ( ! df )
	    { continue;	}

	utilCleanIndexSet( &(df->dfUnicodesUsed) );
	utilInitIndexSet( &(df->dfUnicodesUsed) );

	utilCleanIndexMapping( &(df->dfUnicodeToCharset) );
	utilInitIndexMapping( &(df->dfUnicodeToCharset) );
	}
    }
