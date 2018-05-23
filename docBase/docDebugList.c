#   include	"docBaseConfig.h"

#   include	<uniUtf8.h>
#   include	"docDebugList.h"
#   include	"docDocumentList.h"
#   include	"docListOverride.h"
#   include	"docListOverrideTable.h"
#   include	"docListTable.h"

#   include	<appDebugon.h>

static int docListDocumentListLevelConst( int *			pLevel,
					const ListLevel *	ll,
					int			field )
    {
    int			constOff;
    int			constLen;
    int			constChars;
    int			level;

    int			chars= 0;

    if  ( docListLevelGetField( &constOff, &constLen, &constChars, &level,
								ll, field ) )
	{ LDEB(field); return -1;	}

    if  ( constLen > 0 )
	{
	const char *	from= utilMemoryBufferGetString( &(ll->llFormatString) )+ constOff;
	int		todo= constLen;

	appDebug( "%.*s", constLen, from );

	while( todo > 0 )
	    {
	    unsigned short	symbol;
	    int			step;

	    step= uniGetUtf8( &symbol, from );
	    if  ( step < 1 )
		{ SLDEB("###",step); break;	}

	    chars++; from += step; todo -= step;
	    }
	}

    if  ( chars != constChars )
	{ SLLLDEB("###",field,chars,constChars);	}

    *pLevel= level;
    return 0;
    }

void docListDocumentListLevel(	int			n,
				const ListLevel *	ll )
    {
    int			field;
    int			level;

    appDebug( "\t%d: format: \"%s\"\n", n,
				utilMemoryBufferGetString( &(ll->llFormatString) ) );
    appDebug( "\t%d: %d fields: \"", n, ll->llLevelNumberCount );

    for ( field= 0; field < ll->llLevelNumberCount; field++ )
	{
	if  ( docListDocumentListLevelConst( &level, ll, field ) )
	    { SDEB("###"); break;	}

	appDebug( "<%d>", level );
	if  ( level < 0 || level >= DLmaxLEVELS ||
	      ( n >= 0 && level > n )		)
	    { appDebug( "###" );	}
	}

    if  ( docListDocumentListLevelConst( &level, ll, field ) )
	{ SDEB("###");	}
    else{
	if  ( level != -1 )
	    {
	    appDebug( "<%d>", level );
	    appDebug( "###" );
	    }
	}

    appDebug( "\"\n" );
    }

void docListDocumentList(	int			n,
				const DocumentList *	dl )
    {
    int		l;

    appDebug( "LIST %3d:\n", n );

    for ( l= 0; l < dl->dlLevelCount; l++ )
	{
	docListDocumentListLevel( l, dl->dlLevels+ l );
	}
    }

void docListListOverrideLevel(	int				n,
				const ListOverrideLevel *	lol )
    {
    docListDocumentListLevel( n, &(lol->lolListLevel) );
    }

void docListListOverride(	int				n,
				const ListOverride *		lo )
    {
    int		l;

    for ( l= 0; l < lo->loLevelCount; l++ )
	{
	docListListOverrideLevel( l, lo->loLevels+ l );
	}
    }

void docListListOverrideTable(	int				n,
				const ListOverrideTable *	lot )
    {
    int		o;

    for ( o= 0; o < lot->lotOverrideCount; o++ )
	{
	docListListOverride( o, lot->lotOverrides+ o );
	}
    }

void docListDocumentListTable( const DocumentListTable *	dlt )
    {
    int		n;

    for ( n= 0; n < dlt->dltListCount; n++ )
	{
	docListDocumentList( n, dlt->dltLists+ n );
	}
    }

