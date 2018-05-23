/************************************************************************/
/*									*/
/*  Utility functions for CGI programming.				*/
/*									*/
/************************************************************************/

#   include	"appUtilConfig.h"

#   include	<stdio.h>
#   include	<string.h>
#   include	<stdlib.h>

#   include	"appCgiIn.h"
#   include	"utilTree.h"
#   include	"appTagval.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Return an array of values with headers from the environment.	*/
/*									*/
/************************************************************************/

extern char **		environ;

static void * appCgiInGetHeaderList( void )
    {
    char **		e;

    const int		ownKeys= 1;
    void *		rval= (void *)0;
    void *		tree= utilTreeMakeTree( ownKeys );

    if  ( ! tree )
	{ XDEB(tree); goto ready;	}

    e= environ;
    while( *e )
	{
	char *		eq;
	char *		val;
	void *		prev= (void *)0;

	if  ( strncmp( *e, "HTTP_", 5 ) )
	    { e++; continue;	}

	eq= strchr( *e, '=' );
	if  ( ! eq )
	    { SDEB(*e); goto ready;	}

	val= strdup( eq+ 1 );
	if  ( ! val )
	    { XDEB(val); goto ready;	}

	*eq= '\0';
	if  ( utilTreeStoreValue( tree, &prev, (const char **)0, *(e+5), val ) )
	    { *eq= '='; SDEB(*e); goto ready;	}
	*eq= '=';

	if  ( prev )
	    { free( prev );	}
	}

    rval= tree; tree= (void *)0; /* steal */

  ready:

    if  ( tree )
	{ utilTreeFreeTree( tree, utilTreeFreeValue, (void *)0 );	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Return an array of values with the standard environment variables.	*/
/*									*/
/************************************************************************/

static void * appCgiInGetEnvironmentList( void )
    {
    static const char *	environmentVariables[]=
	{
	"SERVER_PROTOCOL",
	"SERVER_PORT",
	"REQUEST_METHOD",
	"PATH_INFO",
	"PATH_TRANSLATED",
	"SCRIPT_NAME",
	"QUERY_STRING",
	"REMOTE_HOST",
	"REMOTE_ADDR",
	"AUTH_TYPE",
	"REMOTE_USER",
	"REMOTE_IDENT",
	"CONTENT_TYPE",
	"CONTENT_LENGTH",

	"GATEWAY_INTERFACE",
	"SERVER_NAME",
	"SERVER_SOFTWARE",
	};

    const int		count= sizeof(environmentVariables)/sizeof(char *);
    int			i;

    const int		ownKeys= 0;
    void *		rval= (void *)0;
    void *		tree= utilTreeMakeTree( ownKeys );

    if  ( ! tree )
	{ XDEB(tree); goto ready;	}

    for ( i= 0; i < count; i++ )
	{
	const char *	key= environmentVariables[i];
	char *		value= getenv( key );
	void *		prev= (void *)0;

	if  ( value )
	    {
	    value= strdup( value );
	    if  ( ! value )
		{ XDEB(value); goto ready;	}
	    }

	if  ( utilTreeStoreValue( tree, &prev, (const char **)0, key, value ) )
	    { SDEB(key); goto ready;	}

	if  ( prev )
	    { free( prev );	}
	}

    rval= tree; tree= (void *)0; /* steal */

  ready:

    if  ( tree )
	{ utilTreeFreeTree( tree, utilTreeFreeValue, (void *)0 );	}

    return rval;
    }

/************************************************************************/

static void utilCgiSave(	char *		to,
				const char *	from,
				const char *	upto )
    {
    while( from < upto )
	{
	int	c= *(from++);

	if  ( c == '%' && isxdigit( from[0] ) && isxdigit( from[1] ) )
	    {
	    char		b[3];
	    unsigned int	uc;

	    b[0]= *(from++);
	    b[1]= *(from++);
	    b[2]= '\0';

	    sscanf( b, "%x", &uc );
	    *(to++)= uc;

	    from += 2;
	    }
	else{ *(to)++= c;	}
	}

    *to= '\0';
    }

/************************************************************************/
/*									*/
/*  Analyse a CGI Query string.						*/
/*									*/
/************************************************************************/

static void * appCgiInAnalyseQueryString(	const char *	queryString )
    {
    char *		key= (char *)0;
    char *		value= (char *)0;

    const int		ownKeys= 1;
    void *		rval= (void *)0;
    void *		tree= utilTreeMakeTree( ownKeys );

    if  ( ! tree )
	{ XDEB(tree); goto ready;	}

    while( * queryString )
	{
	int		c;
	void *		prev= (void *)0;

	const char *	from= queryString;
	const char *	val= (const char *)0;

	if  ( *from == '&' )
	    { queryString= from+ 1; continue; }

	while( * from )
	    {
	    c= *(from++);

	    if  ( c == '&' )
		{ break;	}
	    if  ( ! c )
		{ XDEB(c); goto ready;	}

	    if  ( c == '=' && ! val )
		{ val= from; }

	    /*  No.. Only in the argument
	    if  ( c == '+' )
		{ pair[pos++]= ' '; continue;	}
	    */

	    if  ( c == '%' && isxdigit( from[0] ) && isxdigit( from[1] ) )
		{ from += 2;	}
	    }

	if  ( ! val )
	    { SXDEB(queryString,val); queryString= from; continue;	}

	key= (char *)malloc( val- queryString );
	if  ( ! key )
	    { XDEB(key); goto ready;	}

	value= (char *)malloc( from- val+ 1 );
	if  ( ! value )
	    { XDEB(value); goto ready;	}

	utilCgiSave( key, queryString, val- 1 );
	utilCgiSave( value, val, from );

	free( key ); key= (char *)0;
	free( value ); value= (char *)0;

	if  ( utilTreeStoreValue( tree, &prev, (const char **)0, key, value ) )
	    { LDEB(1);	}

	if  ( prev )
	    { free( prev );	}

	queryString= from;
	}

    rval= tree; tree= (void *)0; /* steal */

  ready:

    if  ( tree )
	{ utilTreeFreeTree( tree, utilTreeFreeValue, (void *)0 );	}
    if  ( key )
	{ free( key );		}
    if  ( value )
	{ free( value );	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Read a chunk of data from a stream and reduce it to TaggedValue's.	*/
/*									*/
/************************************************************************/

static void * appCgiInReadValueList(	FILE *		from,
					unsigned int	contentLength )
    {
    void *	tree= (void *)0;

    char *	buffer= (char *)0;

    if  ( contentLength == 0 )
	{ LDEB(contentLength); goto ready;	}

    buffer= (char *)malloc( contentLength+ 1 );
    if  ( ! buffer )
	{ XDEB(buffer); goto ready;	}

    if  ( fread( buffer, 1, contentLength, from ) != contentLength )
	{ LDEB(contentLength); goto ready;	}
    buffer[contentLength]= '\0';

    tree= appCgiInAnalyseQueryString( buffer );

ready:

    if  ( buffer )
	{ free( buffer );	}

    return tree;
    }

/************************************************************************/
/*									*/
/*  Analyse the value of the COOKIE header.				*/
/*									*/
/************************************************************************/

static void * appCgiInAnalyseCookieString( const char *	cookieString )
    {
    char *		key= (char *)0;
    char *		value= (char *)0;

    const int		ownKeys= 1;
    void *		rval= (void *)0;
    void *		tree= utilTreeMakeTree( ownKeys );

    if  ( ! tree )
	{ XDEB(tree); goto ready;	}

    while( * cookieString )
	{
	int		c;
	void *		prev= (void *)0;

	const char *	from= cookieString;
	const char *	val= (const char *)0;

	if  ( *from == ';' )
	    { cookieString= from+ 1; continue; }

	while( * from )
	    {
	    c= *(from++);

	    if  ( c == ';' )
		{ break;	}
	    if  ( ! c )
		{ XDEB(c); goto ready;	}

	    if  ( c == '=' && ! val )
		{ val= from; }
	    }

	if  ( ! val )
	    { SXDEB(cookieString,val); cookieString= from; continue;	}

	key= (char *)malloc( val- cookieString );
	if  ( ! key )
	    { XDEB(key); goto ready;	}

	value= (char *)malloc( from- val+ 1 );
	if  ( ! value )
	    { XDEB(value); goto ready;	}

	strncpy( key, cookieString, val- cookieString- 1 )[val- cookieString- 1]= '\0';
	strncpy( value, val, from- val )[from- val]= '\0';

	free( key ); key= (char *)0;
	free( value ); value= (char *)0;

	if  ( utilTreeStoreValue( tree, &prev, (const char **)0, key, value ) )
	    { LDEB(1);	}

	if  ( prev )
	    { free( prev );	}

	cookieString= from;
	}

    rval= tree; tree= (void *)0; /* steal */

  ready:

    if  ( tree )
	{ utilTreeFreeTree( tree, utilTreeFreeValue, (void *)0 );	}
    if  ( key )
	{ free( key );		}
    if  ( value )
	{ free( value );	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Retrieve the REQUEST_METOD field from the environment values.	*/
/*									*/
/************************************************************************/

static int appCgiInGetRequestMethod(	const char **	pValue,
					void *		tree )
    {
    const char *	s;
    int			n;

    int		res= appTagvalGetStringValue( &s, &n, tree, "REQUEST_METHOD" );

    if  ( res )
	{ LDEB(res); return res;	}

    if  ( n )
	{ return 1;	}

    *pValue= s; return 0;
    }

/************************************************************************/
/*									*/
/*  Build the request struct.						*/
/*									*/
/************************************************************************/

CGIRequest * appCgiInGetRequest( void )
    {
    CGIRequest *	cgir= (CGIRequest *)malloc( sizeof(CGIRequest) );
    int			res;
    int			n;
    const char *	contentType;
    long		contentLength;

    if  ( ! cgir )
	{ XDEB(cgir); return cgir;	}

    cgir->cgirHeaderValues= (void *)0;
    cgir->cgirEnvironmentValues= (void *)0;
    cgir->cgirQueryValues= (void *)0;
    cgir->cgirCookies= (void *)0;
    cgir->cgirRequestMethod= (char *)0;
    cgir->cgirStdinUsed= 0;

    cgir->cgirEnvironmentValues= appCgiInGetEnvironmentList();
    if  ( ! cgir->cgirEnvironmentValues )
	{
	XDEB(cgir->cgirEnvironmentValues);
	appCgiInFreeRequest( cgir );
	return (CGIRequest *)0;
	}

    cgir->cgirHeaderValues= appCgiInGetHeaderList();
    if  ( ! cgir->cgirHeaderValues )
	{
	XDEB(cgir->cgirHeaderValues);
	appCgiInFreeRequest( cgir );
	return (CGIRequest *)0;
	}

    if  ( appCgiInGetRequestMethod( &cgir->cgirRequestMethod,
					    cgir->cgirEnvironmentValues ) )
	{
	LDEB(1);
	appCgiInFreeRequest( cgir );
	return (CGIRequest *)0;
	}

    if  ( ! strcmp( cgir->cgirRequestMethod, "POST" )	||
	  ! strcmp( cgir->cgirRequestMethod, "PUT" )	)
	{
	res= appTagvalGetStringValue( &contentType, &n,
			    cgir->cgirEnvironmentValues, "CONTENT_TYPE" );
	if  ( res || n )
	    {
	    LLDEB(res,n);
	    appCgiInFreeRequest( cgir );
	    return (CGIRequest *)0;
	    }

	res= appTagvalGetLongValue( &contentLength, &n,
			    cgir->cgirEnvironmentValues, "CONTENT_LENGTH" );
	if  ( res || n )
	    {
	    LLDEB(res,n);
	    appCgiInFreeRequest( cgir );
	    return (CGIRequest *)0;
	    }

	if  ( ! strcmp( contentType, "application/x-www-form-urlencoded" ) )
	    {
	    cgir->cgirStdinUsed= 1;

	    cgir->cgirQueryValues=
			    appCgiInReadValueList( stdin, (int)contentLength );

	    if  ( ! cgir->cgirQueryValues )
		{
		XDEB(cgir->cgirQueryValues);
		appCgiInFreeRequest( cgir );
		return (CGIRequest *)0;
		}
	    }
	}

    if  ( ! strcmp( cgir->cgirRequestMethod, "GET" )	)
	{
	const char *	queryString;

	res= appTagvalGetStringValue( &queryString, &n,
			    cgir->cgirEnvironmentValues, "QUERY_STRING" );

	if  ( res )
	    {
	    LLDEB(res,n);
	    appCgiInFreeRequest( cgir );
	    return (CGIRequest *)0;
	    }

	if  ( ! n && queryString[0] )
	    {
	    cgir->cgirQueryValues= appCgiInAnalyseQueryString( queryString );

	    if  ( ! cgir->cgirQueryValues )
		{
		XDEB(cgir->cgirQueryValues);
		appCgiInFreeRequest( cgir );
		return (CGIRequest *)0;
		}
	    }
	}

    {
    const char *	cookieString;

    res= appTagvalGetStringValue( &cookieString, &n,
					cgir->cgirHeaderValues, "COOKIE" );

    if  ( res < 0 )
	{
	LLDEB(res,n);
	appCgiInFreeRequest( cgir );
	return (CGIRequest *)0;
	}

    if  ( res == 0 && ! n && cookieString[0] )
	{
	cgir->cgirCookies= appCgiInAnalyseCookieString( cookieString );

	if  ( ! cgir->cgirCookies )
	    {
	    XDEB(cgir->cgirCookies);
	    appCgiInFreeRequest( cgir );
	    return (CGIRequest *)0;
	    }
	}
    }

    return cgir;
    }

void appCgiInFreeRequest( CGIRequest * cgir )
    {
    if  ( cgir->cgirHeaderValues )
        { utilTreeFreeTree( cgir->cgirHeaderValues, utilTreeFreeValue, (void *)0 );	}
    if  ( cgir->cgirEnvironmentValues )
	{ utilTreeFreeTree( cgir->cgirEnvironmentValues, utilTreeFreeValue, (void *)0 );	}
    if  ( cgir->cgirQueryValues )
	{ utilTreeFreeTree( cgir->cgirQueryValues, utilTreeFreeValue, (void *)0 );	}
    if  ( cgir->cgirCookies )
	{ utilTreeFreeTree( cgir->cgirCookies, utilTreeFreeValue, (void *)0 );	}

    free( cgir );

    return;
    }
