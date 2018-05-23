/************************************************************************/
/*									*/
/*  Buffer administration routines.					*/
/*									*/
/************************************************************************/

#   include	"docBaseConfig.h"

#   include	<stdlib.h>
#   include	<string.h>

#   include	<appDebugon.h>
#   include	<utilTree.h>

#   include	"docStyleSheet.h"
#   include	"docListDepth.h"

/************************************************************************/
/*									*/
/*  Copy a style sheet. This is done in reverse order to avoid		*/
/*  reallocations of the array for every style.				*/
/*									*/
/************************************************************************/

int docCopyStyleSheet(	DocumentStyleSheet *		to,
			const DocumentStyleSheet *	from,
			const DocumentAttributeMap *	dam )
    {
    int			i;

    docCleanStyleSheet( to );
    docInitStyleSheet( to );

    for ( i= from->dssStyleCount- 1; i >= 0; i-- )
	{
	DocumentStyle *	dsTo;

	if  ( ! from->dssStyles[i] )
	    { continue;	}

	dsTo= docInsertStyle( to, i, from->dssStyles[i], dam );
	if  ( ! dsTo )
	    { LPDEB(i,dsTo); return -1;	}
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Remove a style from the style names administration.			*/
/*									*/
/************************************************************************/

static int docDeleteStyleFromNames(	DocumentStyleSheet *	dss,
					const DocumentStyle *	ds )
    {
    char *	styleNames= ds->dsName;

    while( styleNames && styleNames[0] )
	{
	char *	comma;

	comma= strchr( styleNames, ',' );
	if  ( comma )
	    {
	    *comma= '\0';

	    utilTreeDeleteEQ( dss->dssStylesByName, styleNames,
					(UTIL_TREE_CALLBACK)0, (void *)0 );

	    *comma= ',';
	    styleNames= comma+ 1;
	    }
	else{
	    utilTreeDeleteEQ( dss->dssStylesByName, styleNames,
					(UTIL_TREE_CALLBACK)0, (void *)0 );
	    break;
	    }
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Insert a style into the style names administration.			*/
/*  Also do some administration based on the name fragments.		*/
/*									*/
/************************************************************************/

static int docInsertStyleByName(	DocumentStyleSheet *	dss,
					DocumentStyle *		ds,
					const char *		name,
					int			nameLen )
    {
    if  ( utilTreeStoreValue( dss->dssStylesByName,
				(void **)0, (const char **)0, name, ds ) )
	{ SDEB(name); return -1;	}

    if  ( nameLen == 9					&&
	  ! strncmp( name, "heading ", 8 )		&&
	  name[8] >= '1'				&&
	  name[8] <= '9'				)
	{ ds->dsOutlineLevel=  name[8]- '1';	}

    return 0;
    }

static int docInsertStyleIntoNames(	DocumentStyleSheet *	dss,
					DocumentStyle *		ds )
    {
    char *	styleNames= ds->dsName;
    int		rval= 0;

    while( styleNames && styleNames[0] )
	{
	char *	comma;

	comma= strchr( styleNames, ',' );
	if  ( comma )
	    {
	    *comma= '\0';

	    if  ( docInsertStyleByName( dss, ds,
					    styleNames, comma- styleNames ) )
		{ SDEB(styleNames); rval= 1;	}

	    *comma= ',';
	    styleNames= comma+ 1;
	    }
	else{
	    if  ( docInsertStyleByName( dss, ds,
					    styleNames, strlen( styleNames ) ) )
		{ SDEB(styleNames); rval= 1;	}

	    break;
	    }
	}

    return rval;
    }

DocumentStyle * docInsertStyle(	DocumentStyleSheet *		dss,
				int				n,
				const DocumentStyle *		dsFrom,
				const DocumentAttributeMap *	dam )
    {
    DocumentStyle *	ds;

    if  ( n < 0 )
	{ n= dss->dssStyleCount;	}

    if  ( ! dss->dssStylesByName )
	{
	const int ownKeys= 1;

	dss->dssStylesByName= utilTreeMakeTree( ownKeys );
	if  ( ! dss->dssStylesByName )
	    { XDEB(dss->dssStylesByName); return (DocumentStyle *)0;	}
	}

    if  ( n >= dss->dssStyleCount )
	{
	DocumentStyle **	fresh;

	fresh= (DocumentStyle **)realloc( dss->dssStyles,
				    ( n + 1 ) * sizeof(DocumentStyle *) );

	if  ( ! fresh )
	    { LLDEB(dss->dssStyleCount,fresh); return (DocumentStyle *)0; }
	dss->dssStyles= fresh;

	while( dss->dssStyleCount <= n )
	    { fresh[dss->dssStyleCount++]= (DocumentStyle *)0;	}
	}

    if  ( dss->dssStyles[n] )
	{ docDeleteStyleFromNames( dss, dss->dssStyles[n] );	}
    else{
	dss->dssStyles[n]= (DocumentStyle *)malloc( sizeof(DocumentStyle) );
	if  ( ! dss->dssStyles[n] )
	    { XDEB(dss->dssStyles[n]); return (DocumentStyle *)0;	}

	docInitDocumentStyle( dss->dssStyles[n] );
	}

    ds= dss->dssStyles[n];

    if  ( docCopyStyle( ds, dsFrom, dam ) )
	{ LDEB(n); return (DocumentStyle *)0; }

    ds->dsStyleNumber= n;
    ds->dsOutlineLevel= PPoutlineBODYTEXT;

    if  ( PROPmaskISSET( &(ds->dsParaMask), PPpropOUTLINELEVEL ) )
	{
	ds->dsOutlineLevel= ds->dsParaProps.ppOutlineLevel;
	}

    if  ( docInsertStyleIntoNames( dss, ds ) )
	{ LDEB(n);	}

    return ds;
    }

void docCleanStyleSheet(	DocumentStyleSheet *	dss )
    {
    int			i;

    for ( i= 0; i < dss->dssStyleCount; i++ )
	{
	if  ( dss->dssStyles[i] )
	    {
	    docCleanDocumentStyle( dss->dssStyles[i] );
	    free( dss->dssStyles[i] );
	    }
	}

    if  ( dss->dssStyles )
	{ free( dss->dssStyles );	}

    if  ( dss->dssStylesByName )
	{
	utilTreeFreeTree( dss->dssStylesByName,
				    (UTIL_TREE_CALLBACK)0, (void *)0 );
	}

    return;
    }

void docInitStyleSheet(	DocumentStyleSheet *	dss )
    {
    dss->dssStyles= (DocumentStyle **)0;
    dss->dssStyleCount= 0;
    dss->dssStylesByName= (void *)0;
    }

DocumentStyle * docGetStyleByName(
				const DocumentStyleSheet *	dss,
				const char *			styleName )
    {

    if  ( dss->dssStylesByName )
	{
	return (DocumentStyle *)utilTreeGetEQ( dss->dssStylesByName,
						(const char **)0, styleName );
	}

    return (DocumentStyle *)0;
    }

