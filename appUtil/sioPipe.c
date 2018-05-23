/************************************************************************/
/*									*/
/*  sioPipe.[ch]:	sio equivalents to popen/pclose.		*/
/*									*/
/************************************************************************/

#   include	"appUtilConfig.h"

#   include	"sioPipe.h"
#   include	"utilMemoryBuffer.h"

#   include	<stdarg.h>
#   include	<stdio.h>
#   include	<signal.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Simple io streams from and to commands using popen().		*/
/*									*/
/************************************************************************/

static int sioPipeClose(	void *	voidf )
    {
    int		rval;
    void	(*prevSigPipeHandler)( int );

    prevSigPipeHandler= signal( SIGPIPE, SIG_IGN );

    rval= pclose( (FILE *)voidf );

    signal( SIGPIPE, prevSigPipeHandler );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Open an input stream from a command using popen().			*/
/*									*/
/************************************************************************/

static int sioInPipeReadBytes(		void *			voidf,
					unsigned char *		buffer,
					unsigned int		count )
    {
    int		rval;

    rval= fread( buffer, 1, count, (FILE *)voidf );

    return rval;
    }

SimpleInputStream * sioInPipeOpen(	const MemoryBuffer *	command )
    {
    SimpleInputStream *		sis= (SimpleInputStream *)0;
    FILE *			f= (FILE *)0;

    if  ( utilMemoryBufferIsEmpty( command ) )
	{ LDEB(command->mbSize); goto ready;	}

    f= popen( utilMemoryBufferGetString( command ), "r" );
    if  ( ! f )
	{ XDEB(f); goto ready;	}

    sis= sioInOpen( (void *)f, sioInPipeReadBytes, sioPipeClose );
    if  ( ! sis )
	{ XDEB(sis); goto ready;	}

    f= (FILE *)0; /* steal */

  ready:

    if  ( f )
	{ pclose( f );	}

    return sis;
    }


/************************************************************************/
/*									*/
/*  Open an output stream to a command using popen().			*/
/*									*/
/************************************************************************/

static int sioOutPipeWriteBytes(	void *			voidf,
					const unsigned char *	buffer,
					int			count )
    {
    int		rval;
    void	(*prevSigPipeHandler)( int );

    prevSigPipeHandler= signal( SIGPIPE, SIG_IGN );

    rval= fwrite( buffer, 1, count, (FILE *)voidf );

    signal( SIGPIPE, prevSigPipeHandler );

    return rval;
    }

SimpleOutputStream * sioOutPipeOpen(	const MemoryBuffer *	command )
    {
    SimpleOutputStream *	sos= (SimpleOutputStream *)0;
    FILE *			f= (FILE *)0;

    void			(*prevSigPipeHandler)( int );

    prevSigPipeHandler= signal( SIGPIPE, SIG_IGN );

    if  ( utilMemoryBufferIsEmpty( command ) )
	{ LDEB(command->mbSize); goto ready;	}

    f= popen( utilMemoryBufferGetString( command ), "w" );
    if  ( ! f )
	{ XDEB(f); goto ready;	}

    sos= sioOutOpen( (void *)f, sioOutPipeWriteBytes, sioPipeClose );

    if  ( ! sos )
	{ XDEB(sos); goto ready;	}

    f= (FILE *)0; /* steal */

  ready:

    if  ( f )
	{ pclose( f );	}

    signal( SIGPIPE, prevSigPipeHandler );

    return sos;
    }

