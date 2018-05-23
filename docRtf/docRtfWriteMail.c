/************************************************************************/
/*									*/
/*  Mail an RTF document.						*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<string.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	<appDebugon.h>

#   include	<sioGeneral.h>
#   include	<sioBase64.h>
#   include	"docRtfReadWrite.h"
#   include	"docPlainReadWrite.h"
#   include	<docTreeNode.h>

int docWriteRtfMail(		SimpleOutputStream *		sos,
				const char *			mimeBoundary,
				BufferDocument *		bd )
    {
    SimpleOutputStream *	sosOpened= (SimpleOutputStream *)0;
    SimpleOutputStream *	sosBody= (SimpleOutputStream *)0;
    int				rval= 0;

    const DocumentProperties *	dp= &(bd->bdProperties);

    const char *		file= utilMemoryBufferGetString( &(dp->dpFilename) );

    if  ( file )
	{
	const char *		relative;

	relative= strrchr( file, '/' );
	if  ( relative )
	    { file= relative+ 1;	}
	}

    if  ( ! file || ! file[0] )
	{ file= "file.rtf";	}

    sosBody= sos;

    if  ( mimeBoundary )
	{
	sioOutPutString( "--", sos );
	sioOutPutString( mimeBoundary, sos );
	sioOutPutString( "\r\n", sos );
	sioOutPutString( "Content-Type: text/plain", sos );
	sioOutPutString( "\r\n", sos );
	sioOutPutString( "Content-Transfer-Encoding: 8bit", sos );
	sioOutPutString( "\r\n", sos );

	sioOutPutString( "\r\n", sos );

	if  ( ! utilMemoryBufferIsEmpty( &(dp->dpTitle) )		&&
	      strcmp( mimeBoundary, utilMemoryBufferGetString( &(dp->dpTitle) ) )	)
	    {
	    sioOutPutString( utilMemoryBufferGetString( &(dp->dpTitle) ), sos );
	    sioOutPutString( "\r\n", sos );
	    }
	else{
	    DocumentPosition	dpFirst;
	    DocumentSelection	ds;

	    if  ( docDocumentHead( &dpFirst, bd ) )
		{ sioOutPutString( "application/rtf\r\n", sos );	}
	    else{
		const int		fold= 1;
		const int		direction= 1;

		struct BufferItem *	bi= dpFirst.dpNode;

		docSetParaSelection( &ds, bi, direction,
						0, docParaStrlen( bi ) );

		if  ( docPlainSaveDocument( sos, bd, &ds, fold ) )
		    { rval= -1; goto ready;	}

		goto ready;
		}
	    }

	sioOutPutString( "--", sos );
	sioOutPutString( mimeBoundary, sos );
	sioOutPutString( "\r\n", sos );

	sioOutPutString( "Content-Type: ", sos );
	if  ( file && file[0] )
	    {
	    sioOutPutString( "application/rtf; name=\"", sos );
	    sioOutPutString( file, sos );
	    sioOutPutString( "\"", sos );
	    }
	else{
	    sioOutPutString( "application/rtf", sos );
	    }
	sioOutPutString( "\r\n", sos );

	sioOutPutString( "Content-Transfer-Encoding: ", sos );
	sioOutPutString( "base64", sos );
	sioOutPutString( "\r\n", sos );

	sioOutPutString( "\r\n", sos );

	sosOpened= sioOutBase64Open( sos );
	if  ( ! sosOpened )
	    { XDEB(sosOpened); rval= -1; goto ready;	}
	sosBody= sosOpened;
	}

    rval= docRtfSaveDocument( sosBody, bd, (const DocumentSelection *)0, 0 );

    if  ( mimeBoundary )
	{
	sioOutClose( sosOpened ); sosOpened= (SimpleOutputStream *)0;

	sioOutPutString( "--", sos );
	sioOutPutString( mimeBoundary, sos );
	sioOutPutString( "--", sos );
	sioOutPutString( "\r\n", sos );
	}

  ready:

    if  ( sosOpened )
	{ sioOutClose( sosOpened );	}

    return rval;
    }

