#   include	"utilCgiEcho.h"
#   include	"utilTree.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Echo some information on a cgi request.				*/
/*									*/
/************************************************************************/

static int utilCgiEchoValue(	const char *		key,
				void *			val,
				void *			through )
    {
    SimpleOutputStream *	sos= (SimpleOutputStream *)through;
    char *			value= (char *)val;

    sioOutPutString( "<tr><td>", sos );

    sioOutPutString( key, sos );
    sioOutPutString( "</td>\r\n<td>", sos );

    if  ( value )
	{ sioOutPutString( value, sos );	}
    else{ sioOutPutString( "&#160;", sos );	}

    sioOutPutString( "</td></tr>\r\n", sos );

    return 0;
    }

void utilCgiEchoValueList(	void *			tree,
				const char *		label,
				SimpleOutputStream *	sos )
    {
    const int		stopOnFailure= 0;

    sioOutPutString( "<table>\r\n", sos );

    if  ( label && label[0] )
	{
	sioOutPutString( "<tr><td colspan=\"2\"><b>", sos );
	sioOutPutString( label, sos );
	sioOutPutString( "</b></td></tr>\r\n", sos );
	}

    utilTreeForAll( tree, stopOnFailure, utilCgiEchoValue, (void *)sos );

    sioOutPutString( "</table>\r\n", sos );

    return;
    }

void utilCgiEchoRequest(		const CGIRequest *	cgir,
					SimpleOutputStream *	sos )
    {
    if  ( cgir->cgirHeaderValues )
	{
	utilCgiEchoValueList( cgir->cgirHeaderValues, "Headers", sos );
	}

    if  ( cgir->cgirEnvironmentValues )
	{
	utilCgiEchoValueList( cgir->cgirEnvironmentValues,
							"Environment", sos );
	}

    if  ( cgir->cgirQueryValues )
	{
	utilCgiEchoValueList( cgir->cgirQueryValues, "Query", sos );
	}

    if  ( cgir->cgirCookies )
	{
	utilCgiEchoValueList( cgir->cgirCookies, "Cookies", sos );
	}

    return;
    }

#   ifdef EXAMPLE_CODE

int main(	int		argc,
		char **		argv )
    {
    int				rval= 0;
    CGIRequest *		cgir= (CGIRequest *)0;

    SimpleOutputStream *	sos= (SimpleOutputStream *)0;

    cgir= appCgiInGetRequest();
    if  ( ! cgir )
	{ XDEB(cgir); rval= -1; goto ready;	}

    sos= sioOutStdoutOpen();
    if  ( ! sos )
	{ XDEB(sos); rval= -1; goto ready;	}

    sioOutPutString( "Content-Type: text/html\r\n", sos );
    sioOutPutString( "\r\n", sos );

    utilCgiEchoRequest( cgir, sos );

  ready:

    if  ( sos )
	{ sioOutClose( sos );	}

    if  ( cgir )
	{ appCgiInFreeRequest( cgir );	}

    return rval;
    }

#   endif
