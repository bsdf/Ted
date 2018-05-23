/************************************************************************/
/*									*/
/*  Manage a list of fields.						*/
/*									*/
/************************************************************************/

#   include	"docBaseConfig.h"

#   include	<stdlib.h>
#   include	<string.h>

#   include	<appDebugon.h>

#   include	"docDocumentFieldList.h"
#   include	"docBookmarkField.h"
#   include	"docDocumentField.h"

/************************************************************************/
/*									*/
/*  Manage a list of fields.						*/
/*									*/
/************************************************************************/

/************************************************************************/
/*									*/
/*  Make a copy of a field.						*/
/*									*/
/************************************************************************/

DocumentField *	docClaimFieldCopy(	DocumentFieldList *	dfl,
					const DocumentField *	dfFrom,
					const SelectionScope *	ss,
					const EditPosition *	epStart )
    {
    DocumentField *	dfTo;

    dfTo= docClaimField( dfl );
    if  ( ! dfTo )
	{ XDEB(dfTo); return (DocumentField *)0;	}

    if  ( docCopyFieldProperties( dfTo, dfFrom ) )
	{
	docDeleteFieldFromList( dfl, dfTo );
	return (DocumentField *)0;
	}

    dfTo->dfPage= -1;
    dfTo->dfSelectionScope= *ss;
    dfTo->dfHeadPosition= *epStart;

    return dfTo;
    }

DocumentField * docGetFieldByNumber(	const DocumentFieldList *	dfl,
					int				n )
    {
    void *	vdf= utilPagedListGetItemByNumber( &(dfl->dflPagedList), n );

    return (DocumentField *)vdf;
    }

int docGetFieldKindByNumber(	const DocumentFieldList *	dfl,
				int				n )
    {
    DocumentField *	df= docGetFieldByNumber( dfl, n );

    if  ( ! df )
	{ return -1;	}

    return df->dfKind;
    }

void docDeleteFieldFromList(	DocumentFieldList *		dfl,
				DocumentField *			df )
    {
    if  ( docGetFieldByNumber( dfl, df->dfFieldNumber ) != df )
	{ LDEB(df->dfFieldNumber);	}

    utilPagedListDeleteItemByNumber( &(dfl->dflPagedList), df->dfFieldNumber );

    return;
    }

DocumentField * docClaimField(	DocumentFieldList *	dfl )
    {
    void *		vdf;
    int			n;
    DocumentField *	df;

    vdf= utilPagedListClaimNewItem( &n, &(dfl->dflPagedList) );
    if  ( ! vdf )
	{ PDEB(vdf); return (DocumentField *)0;	}

    df= (DocumentField *)vdf;
    df->dfFieldNumber= n;

    return df;
    }

void docInitFieldList(	DocumentFieldList *	dfl )
    {
    utilInitPagedList( &(dfl->dflPagedList) );
    utilStartPagedList( &(dfl->dflPagedList), sizeof(DocumentField),
			    (InitPagedListItem)docInitDocumentField,
			    (CleanPagedListItem)docCleanDocumentField );
    return;
    }

void docCleanFieldList(	DocumentFieldList *	dfl )
    { utilCleanPagedList( &(dfl->dflPagedList) );	}

/************************************************************************/
/*									*/
/*  Remember the page where a field occurs.				*/
/*									*/
/************************************************************************/

int docSetPageOfField(		DocumentFieldList *	dfl,
				int			n,
				int			page )
    {
    DocumentField *	df= docGetFieldByNumber( dfl, n );

    if  ( ! df )
	{ LPDEB(n,df); return -1;	}

    df->dfPage= page;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Look for a bookmark in the field list.				*/
/*  1)  Scan in reverse order to cope with duplicate bookmarks in the	*/
/*	RTF parser.							*/
/*									*/
/************************************************************************/

int docFindBookmarkField(	DocumentField **		pDf,
				const DocumentFieldList *	dfl,
				const MemoryBuffer *		markName )
    {
    int			n;
    const int		fieldCount= dfl->dflPagedList.plItemCount;

    /*  1  */
    for ( n= fieldCount- 1; n >= 0; n-- )
	{
	DocumentField *	df= docGetFieldByNumber( dfl, n );

	if  ( ! df )
	    { continue;	}

	if  ( docFieldMatchesBookmark( df, markName )	)
	    { *pDf= df; return n;	}
	}

    return -1;
    }

/************************************************************************/
/*									*/
/*  Build a list of bookmarks to be used in a GUI.			*/
/*									*/
/************************************************************************/

static int docCompareBookmarkStrings(	const void *	pv1,
					const void *	pv2 )
    {
    const char * const *	ps1= (const char * const *)pv1;
    const char * const *	ps2= (const char * const *)pv2;

    return strcmp( *ps1, *ps2 );
    }

int docMakeBookmarkList(	char ***			pBookmarks,
				int *				pBookmarkCount,
				int				includeTocMarks,
				const DocumentFieldList *	dfl )
    {
    int				rval= 0;
    int				n;

    int				bookmarkCount= 0;
    char **			bookmarks= (char **)0;
    const int			fieldCount= dfl->dflPagedList.plItemCount;

    const MemoryBuffer *	mbBookmark;

    if  ( fieldCount == 0 )
	{ goto ready;	}

    /*  2  */
    bookmarks= (char **)malloc( fieldCount* sizeof(char *) );
    if  ( ! bookmarks )
	{ LXDEB(fieldCount,bookmarks); rval= -1; goto ready;	}

    for ( n= 0; n < fieldCount; n++ )
	{
	DocumentField *	df= docGetFieldByNumber( dfl, n );
	int		size;

	if  ( ! df )
	    { continue;	}

	if  ( df->dfKind != DOCfkBOOKMARK		||
	      docFieldGetBookmark( &mbBookmark, df )	)
	    { continue;		}

	size= mbBookmark->mbSize;
	if  ( size == 0					||
	      ( ! includeTocMarks && mbBookmark->mbBytes[0] == '_' )	||
	      size > DOCmaxBOOKMARK			)
	    { continue;	}

	bookmarks[bookmarkCount]= (char *)malloc( size+ 1 );
	if  ( ! bookmarks[bookmarkCount] )
	    { LXDEB(size,bookmarks[bookmarkCount]); rval= -1; goto ready; }

	strncpy( bookmarks[bookmarkCount],
				(char *)mbBookmark->mbBytes, size )[size]= '\0';
	bookmarkCount++;
	}

    /*  3  */
    qsort( bookmarks, bookmarkCount, sizeof(char *),
					    docCompareBookmarkStrings );

    *pBookmarks= bookmarks; bookmarks= (char **)0; /* steal */
    *pBookmarkCount= bookmarkCount; bookmarkCount= 0;

  ready:

    if  ( bookmarks )
	{
	for ( n= 0; n < bookmarkCount; n++ )
	    {
	    if  ( bookmarks[n] )
		{ free( bookmarks[n] );	}
	    }

	free( bookmarks );
	}

    return rval;
    }

