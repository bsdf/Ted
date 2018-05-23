#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<string.h>
#   include	<ctype.h>

#   include	<sioMemory.h>
#   include	<appSystem.h>
#   include	"tedApp.h"
#   include	"tedDocFront.h"
#   include	"tedDocument.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Follow a link.							*/
/*									*/
/*  1)  As a HACK, invoke netscape for all links with a protocol	*/
/*	different from 'file:'.						*/
/*  2)  Theoretically, mailcap/mimecap should have been consulted for	*/
/*	the application corresponding to the extension of the file.	*/
/*									*/
/************************************************************************/

/*  1  */
static int tedCallNetscape(	const MemoryBuffer *		fileName,
				const MemoryBuffer *		markName )
    {
    int				rval= 0;

    MemoryBuffer		mbCmd;
    SimpleOutputStream *	sosCmd= (SimpleOutputStream *)0;
    int				totalSize= 0;

    utilInitMemoryBuffer( &mbCmd );

    /**** Prevent tricks to run arbitrary commands! */
    if  ( fileName )
	{
	int		i;

	for ( i= 0; i < fileName->mbSize; i++ )
	    {
	    if  ( fileName->mbBytes[i] == '\'' )
		{ CDEB(fileName->mbBytes[i]); rval= -1; goto ready;	}
	    }
	totalSize += fileName->mbSize;
	}
    if  ( markName )
	{
	int		i;

	for ( i= 0; i < markName->mbSize; i++ )
	    {
	    if  ( markName->mbBytes[i] == '\'' )
		{ CDEB(markName->mbBytes[i]); rval= -1; goto ready;	}
	    }

	totalSize += markName->mbSize;
	}

    if  ( totalSize == 0 )
	{ LDEB(totalSize); rval= -1; goto ready;	}

    sosCmd= sioOutMemoryOpen( &mbCmd );
    if  ( ! sosCmd )
	{ XDEB(sosCmd); rval= -1; goto ready;	}

    sioOutPrintf( sosCmd, "xdg-open '" );
    if  ( fileName && ! utilMemoryBufferIsEmpty( fileName ) )
	{
	sioOutPrintf( sosCmd, "%s", utilMemoryBufferGetString( fileName ) );
	}

    if  ( markName && ! utilMemoryBufferIsEmpty( markName ) )
	{
	sioOutPrintf( sosCmd, "#%s", utilMemoryBufferGetString( markName ) );
	}

    sioOutPrintf( sosCmd, "'" );

    sioOutClose( sosCmd ); sosCmd= (SimpleOutputStream *)0;

    if  ( system( utilMemoryBufferGetString( &mbCmd ) ) )
	{ SDEB(utilMemoryBufferGetString(&mbCmd)); rval= -1;	}

  ready:

    if  ( sosCmd )
	{ sioOutClose( sosCmd );	}

    utilCleanMemoryBuffer( &mbCmd );

    return rval;
    }

static void tedAnalyseFileNamePart(	int *			pIsFile,
					int *			pIsAbsolute,
					int *			pProtocolSize,
					const MemoryBuffer *	fileBuffer )
    {
    const char *	fileName= utilMemoryBufferGetString( fileBuffer );
    int			fileSize= fileBuffer->mbSize;
    int			isAbsolute= fileName[0] == '/';
    int			isFile;
    int			protocolSize= 0;

    if  ( isAbsolute )
	{ isFile= 1;	}
    else{
	while( protocolSize < fileSize && isalpha( fileName[protocolSize] ) )
	    { protocolSize++;	}

	/*  1  */
	if  ( protocolSize < fileSize && fileName[protocolSize] == ':' )
	    {
	    if  ( protocolSize == 4			&&
		  ! strncmp( fileName, "file", 4 )	)
		{
		fileName += 5;
		fileSize -= 5;
		isAbsolute= fileName[0] == '/';
		protocolSize= 5;
		isFile= 1;
		}
	    else{
		isFile= 0;
		isAbsolute= protocolSize > 0;
		}
	    }
	else{ protocolSize= 0; isFile= 1;	}
	}

    *pIsFile= isFile;
    *pIsAbsolute= isAbsolute;
    *pProtocolSize= protocolSize;

    return;
    }

int tedDocFollowLink(	APP_WIDGET		option,
			EditDocument *		edFrom,
			const MemoryBuffer *	fileName,
			const MemoryBuffer *	markName )
    {
    int				rval= 0;
    EditApplication *		ea= edFrom->edApplication;

    EditDocument *		edTo;

    char *			scratch= (char *)0;

    TedDocument *		tdFrom= (TedDocument *)edFrom->edPrivateData;
    BufferDocument *		bdFrom= tdFrom->tdDocument;
    const DocumentProperties *	dpFrom= &(bdFrom->bdProperties);

    MemoryBuffer		base;
    MemoryBuffer		file;
    MemoryBuffer		target;

    APP_WIDGET			relative= edFrom->edToplevel.atTopWidget;

    int				baseIsFile;
    int				baseIsAbsolute;
    int				baseProtocolSize;

    utilInitMemoryBuffer( &base );
    utilInitMemoryBuffer( &file );
    utilInitMemoryBuffer( &target );

    if  ( fileName && utilMemoryBufferIsEmpty( fileName ) )
	{ fileName= (const MemoryBuffer *)0;	}
    if  ( markName && utilMemoryBufferIsEmpty( markName ) )
	{ markName= (const MemoryBuffer *)0;	}

    if  ( ! utilMemoryBufferIsEmpty( &(dpFrom->dpHlinkbase) ) )
	{
	tedAnalyseFileNamePart( &baseIsFile, &baseIsAbsolute, &baseProtocolSize,
						    &(dpFrom->dpHlinkbase) );

	if  ( utilCopyMemoryBuffer( &base, &(dpFrom->dpHlinkbase) ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}
    else{
	baseIsFile= 1;
	baseProtocolSize= 0;
	baseIsAbsolute= 0;

	if  ( appDirectoryOfFileName( &base, &(edFrom->edFilename) ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}

    if  ( fileName )
	{
	const int	readOnly= 1;
	int		fileIsFile;
	int		fileIsAbsolute;
	int		fileProtocolSize;

	tedAnalyseFileNamePart( &fileIsFile, &fileIsAbsolute, &fileProtocolSize,
								fileName );
	if  ( ! fileIsFile )
	    {
	    if  ( tedCallNetscape( fileName, markName ) )
		{ rval= -1; goto ready;	}

	    goto ready;
	    }

	if  ( utilMemoryBufferGetRange( &file, fileName,
					fileProtocolSize, fileName->mbSize ) )
	    { LDEB(1); rval= -1; goto ready;	}

	if  ( fileIsAbsolute || utilMemoryBufferIsEmpty( &base ) )
	    {
	    edTo= appOpenDocument( ea, relative, option, readOnly, &file );
	    if  ( ! edTo )
		{ SXDEB(scratch,edTo);	}
	    }
	else{
	    if  ( utilCopyMemoryBuffer( &target, &base ) )
		{ LDEB(1); rval= -1; goto ready;	}
	    if  ( utilMemoryBufferAppendBytes( &target, (unsigned char *)"/", 1 ) )
		{ LDEB(1); rval= -1; goto ready;	}
	    if  ( utilMemoryAppendBuffer( &target, &file ) )
		{ LDEB(1); rval= -1; goto ready;	}

	    edTo= appOpenDocument( ea, relative, option, readOnly, &target );
	    if  ( ! edTo )
		{ /* SXDEB(scratch,edTo); */ rval= -1;	}
	    }
	}
    else{ edTo= edFrom;	}

    if  ( edTo && markName )
	{ tedDocGoToBookmark( edTo, markName );	}

  ready:

    utilCleanMemoryBuffer( &base );
    utilCleanMemoryBuffer( &file );
    utilCleanMemoryBuffer( &target );

    if  ( scratch )
	{ free( scratch );	}

    return rval;
    }

