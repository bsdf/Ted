/************************************************************************/
/*									*/
/*  Keep a trace of modifications to a document.			*/
/*									*/
/************************************************************************/

#   include	"docEditConfig.h"

#   include	<string.h>
#   include	<time.h>

#   include	<appSystem.h>
#   include	<sioFd.h>
#   include	<utilMemoryBufferPrintf.h>
#   include	"docEditTrace.h"
#   include	"docEditCommand.h"

#   include	<appDebugon.h>

void docInitTraceStep(	TraceStep *	ts )
    {
    ts->tsCommand= -1;
    ts->tsFieldKind= -1;
    ts->tsTraceOffset= -1;
    ts->tsByteCount= 0;
    }

void docInitEditTrace(	EditTrace *	et )
    {
    utilInitPagedList( &(et->etTraceSteps) );
    utilStartPagedList( &(et->etTraceSteps), sizeof(TraceStep),
					(InitPagedListItem)docInitTraceStep,
					(CleanPagedListItem)0 );

    et->etCount= 0;
    et->etIndex= 0;
    et->etBase= -1;
    et->etIsRecovery= 0;

    docInitTraceStep( &(et->etThisStep) );

    utilInitMemoryBuffer( &(et->etTraceFileName) );
    et->etTraceFileHandle= -1;
    et->etTraceStatus= TRACING_NO;

    et->etTyping= TYPING_NO;
    docInitEditRange( &(et->etTypingOldRange) );

    memset( et->etBaseMD5Digest64, '\0', sizeof(et->etBaseMD5Digest64) );
    }

void docCleanEditTrace(	EditTrace *	et )
    {
    if  ( et->etTraceFileHandle >= 0 )
	{ sioFdCloseFile( et->etTraceFileHandle );	}

    if  ( ! et->etIsRecovery					&&
	  ! utilMemoryBufferIsEmpty( &(et->etTraceFileName) )	&&
    	  et->etTraceFileHandle >= 0				)
	{
	if  ( appRemoveFile( &(et->etTraceFileName) ) )
	    { LDEB(1);	}
	}

    utilCleanPagedList( &(et->etTraceSteps) );
    utilCleanMemoryBuffer( &(et->etTraceFileName) );
    }

int docEditTraceSetTempName(	EditTrace *		et,
				const char *		extension )
    {
    char	scratch[MD5_DIGEST_SIZE_BASE64];
    char *	s= scratch;

    strcpy( scratch, et->etBaseMD5Digest64 );
    while( *s )
	{
	if  ( *s == '/' )
	    { *s=   '=';	}

	s++;
	}

    utilMemoryBufferPrintf( &(et->etTraceFileName), "/tmp/%s.%s",
						    scratch, extension );
    return 0;
    }

int docEditTraceSetDocumentName(	EditTrace *		et,
					const MemoryBuffer *	documentName,
					const char *		extension )
    {
    int			rval= 0;

    const int		relativeIsFile= 1;

    MemoryBuffer	docBaseName;

    utilInitMemoryBuffer( &docBaseName );

    if  ( appFileGetRelativeName( &docBaseName, documentName ) )
	{ LDEB(1); rval= -1; goto ready;	}
    if  ( appFileAddExtension( &(docBaseName), extension ) )
	{ LDEB(1); rval= -1; goto ready;	}
#   define	HIDE_TRACE_FILE	0
#   if		HIDE_TRACE_FILE
    if  ( utilMemoryBufferReplaceBytes( &(docBaseName), 0, 0,
					    (const unsigned char *)".", 1 ) )
	{ LDEB(1); rval= -1; goto ready;	}
#   endif
    if  ( appAbsoluteName( &(et->etTraceFileName), &docBaseName,
					    relativeIsFile, documentName ) < 0 )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    utilCleanMemoryBuffer( &docBaseName );

    return rval;
    }

int docEditTraceOpenTrace(	EditTrace *		et,
				int			restart,
				int			exclusive )
    {
    const int		read= 1;
    const int		write= 1;
    const int		append= 0;
    const int		create= restart;

    int			fdTrace;

    fdTrace= sioFdOpenFile( &(et->etTraceFileName),
				    read, write, append, create, exclusive );
    if  ( fdTrace < 0 )
	{
	/*SLDEB(utilMemoryBufferGetString(&(et->etTraceFileName)),et->etTraceFileHandle);*/
	et->etTraceFileHandle= -1;
	return fdTrace;
	}

    et->etTraceFileHandle= fdTrace;
    return fdTrace;
    }

int docEditTraceTryRelative(	EditTrace *		et,
				const MemoryBuffer *	documentName,
				const char *		extension )
    {
    const int	restart= 1;
    const int	exclusive= 1;

    int		fdTrace;

    if  ( docEditTraceSetDocumentName( et, documentName, extension ) )
	{ LDEB(1); return FDerrOTHER;	}

    fdTrace= docEditTraceOpenTrace( et, restart, exclusive );
    if  ( fdTrace >= 0 )
	{ et->etTraceStatus= TRACING_RELATIVE;	}
    else{
	switch( fdTrace )
	    {
	    case FDerrACCES:
		et->etTraceStatus= TRACING_ACCES;
		break;
	    case FDerrEXIST:
		et->etTraceStatus= TRACING_EXIST;
		break;
	    default:
		LDEB(fdTrace);
		et->etTraceStatus= TRACING_NO;
	    }
	}

    return fdTrace;
    }

int docEditTraceTryTemp(	EditTrace *		et,
				const char *		extension )
    {
    const int	restart= 1;
    const int	exclusive= 1;

    int		fdTrace;

    if  ( docEditTraceSetTempName( et, extension ) )
	{ LDEB(1); return FDerrOTHER;	}

    fdTrace= docEditTraceOpenTrace( et, restart, exclusive );
    if  ( fdTrace >= 0 )
	{ et->etTraceStatus= TRACING_TEMP;	}
    else{
	switch( fdTrace )
	    {
	    case FDerrACCES:
		et->etTraceStatus= TRACING_ACCES;
		break;
	    case FDerrEXIST:
		et->etTraceStatus= TRACING_EXIST;
		break;
	    default:
		LDEB(fdTrace);
		SDEB(utilMemoryBufferGetString(&(et->etTraceFileName)));
		et->etTraceStatus= TRACING_NO;
	    }
	}

    return fdTrace;
    }

/************************************************************************/
/*									*/
/*  Open a trace for an anonymous new file. We do our best to make sure	*/
/*  that we do not share the trace file with an other Ted instance.	*/
/*									*/
/************************************************************************/

int docEditTraceTryAnon(	EditTrace *		et,
				const char *		extension )
    {
    MemoryBuffer	try;
    int			fdTrace= FDerrOTHER;
    const int		maxTurns= 10000;
    int			turn;

    utilInitMemoryBuffer( &try );

    for ( turn= 0; turn < maxTurns; turn++ )
	{
	time_t		now= time( (time_t *)0 );

	const int	restart= 1;
	const int	exclusive= 1;
	const int	read= 1;
	const int	write= 1;
	const int	append= 0;
	const int	create= restart;

	int		fd;

	utilMemoryBufferPrintf( &try, "/tmp/%ld-%d.%s",
						    now, turn, extension );

	fd= sioFdOpenFile( &try, read, write, append, create, exclusive );

	if  ( fd >= 0 )
	    {
	    utilCopyMemoryBuffer( &(et->etTraceFileName), &try );
	    et->etTraceFileHandle= fd;
	    et->etTraceStatus= TRACING_ANON;

	    fdTrace= fd;
	    break;
	    }

	if  ( fd != FDerrEXIST )
	    {
	    LSDEB(fd,utilMemoryBufferGetString(&try));
	    break;
	    }
	}

    if  ( turn >= maxTurns )
	{ LDEB(turn);	}

    utilCleanMemoryBuffer( &try );

    return fdTrace;
    }

/************************************************************************/
/*									*/
/*  Get the next (direction>0) or previous (direction<0) applicable	*/
/*  editing step. Skip save and open actions.				*/
/*									*/
/************************************************************************/

int docEditGetTraceStep(	const TraceStep **		pTs,
				int *				pIsRepeat,
				int				direction,
				const EditTrace *		et,
				int				from )
    {
    int			n= from;
    const TraceStep *	ts= (const TraceStep *)0;
    int			isRepeat= 0;

    if  ( direction == 0 )
	{ LDEB(direction); return -1;	}

    /*  redo or repeat.. assume it is a redo */
    if  ( direction > 0 )
	{
	n= from;

	while( n < et->etCount )
	    {
	    ts= (const TraceStep *)utilPagedListGetItemByNumber(
						    &(et->etTraceSteps), n );
	    if  ( ! ts )
		{ LXDEB(n,ts); return -1;	}

	    if  ( ts->tsCommand < EDITcmd_EDIT_COUNT )
		{ break;	}

	    n++;
	    }

	if  ( n >= et->etCount )
	    { isRepeat= 1; direction= -1;	}
	}

    /*  undo or repeat.. look for the previous edit. (last for repeat) */
    if  ( direction < 0 )
	{
	n= from- 1;

	while( n >= 0 )
	    {
	    ts= (const TraceStep *)utilPagedListGetItemByNumber(
						    &(et->etTraceSteps), n );
	    if  ( ! ts )
		{ LXDEB(n,ts); return -1;	}

	    if  ( ts->tsCommand < EDITcmd_EDIT_COUNT )
		{ break;	}

	    n--;
	    }
	}

    if  ( n < 0 || n >= et->etCount )
	{ /*LLDEB(n,et->etCount);*/ return -1;	}

    if  ( n >= 0 && pIsRepeat )
	{ *pIsRepeat= isRepeat;	}
    if  ( n >= 0 && pTs )
	{ *pTs= ts;	}

    return n;
    }

