/************************************************************************/
/*									*/
/*  Simple io streams using raw file IO.				*/
/*									*/
/************************************************************************/

#   include	"appUtilConfig.h"

#   include	"sioFileio.h"
#   include	"sioFd.h"
#   include	"sioStdin.h"
#   include	"sioStdout.h"

#   include	<appDebugon.h>
#   include	<unistd.h>
#   include	<errno.h>
#   include	<string.h>
#   include	<stdlib.h>
#   include	<fcntl.h>

typedef struct FileIo
    {
    int		fiFd;
    int		fiOwnFd;
    } FileIo;

static int sioFileioClose(	void *	vfi )
    {
    FileIo *	fi= (FileIo *)vfi;
    int		rval= 0;

    if  ( fi->fiOwnFd )
	{
	rval= close( fi->fiFd );
	if  ( rval != 0 )
	    { LLSDEB(fi->fiFd,errno,strerror(errno));	}
	}

    free( vfi );

    return rval;
    }

int sioFdCloseFile(			int			fd )
    {
    int	rval= close( fd );
    if  ( rval != 0 )
	{ LLSDEB(fd,errno,strerror(errno));	}

    return rval;
    }

static int sioInFileioReadBytes(	void *			vfi,
					unsigned char *		buffer,
					unsigned int		count )
    {
    FileIo *	fi= (FileIo *)vfi;
    int		rval= read( fi->fiFd, buffer, count );

    if  ( rval < 0 )
	{ LLSDEB(fi->fiFd,errno,strerror(errno));	}

    return rval;
    }

static int sioFdOpenFileS(	const char *		filename,
				int			read,
				int			write,
				int			append,
				int			create,
				int			exclusive )
    {
    int		fd= -1;
    int		flags= 0;

    if  ( write )
	{
	if  ( read )
	    {
	    flags= O_RDWR;

	    if  ( append )
		{ flags |= O_APPEND;	}
	    }
	else{
	    flags= O_WRONLY;
	    if  ( append )
		{ flags |= O_APPEND;	}
	    else{ flags |= O_TRUNC;	}
	    }

	if  ( create )
	    {
	    flags |= O_CREAT;

	    if  ( exclusive )
		{ flags |= O_EXCL;	}
	    }
	else{
	    if  ( exclusive )
		{ LLDEB(create,exclusive); return -1;	}
	    }
	}
    else{
	if  ( create )
	    { LLDEB(write,create); return -1;	}
	if  ( exclusive )
	    { LLDEB(write,exclusive); return -1;	}

	flags= O_RDONLY;
	}

#   ifdef O_BINARY
    flags |= O_BINARY;
#   endif

    fd= open( filename, flags, 0666 );
    if  ( fd < 0 )
	{
	switch( errno )
	    {
	    case EPERM:
		return FDerrPERM;
	    case ENOENT:
		return FDerrNOENT;
	    case EEXIST:
		return FDerrEXIST;
	    case EACCES:
		return FDerrACCES;
	    case EISDIR:
		return FDerrISDIR;
	    default:
		SLSDEB(filename,errno,strerror(errno));
		return FDerrOTHER;
	    }
	}

    return fd;
    }

int sioFdOpenFile(		const MemoryBuffer *	filename,
				int			read,
				int			write,
				int			append,
				int			create,
				int			exclusive )
    {
    if  ( ! filename || filename->mbSize <= 0 )
	{ XDEB(filename); return FDerrOTHER;	}

    return sioFdOpenFileS( utilMemoryBufferGetString( filename ),
				    read, write, append, create, exclusive );
    }

SimpleInputStream * sioInFileioOpenS(	const char *	filename )
    {
    SimpleInputStream *	sis= (SimpleInputStream *)0;
    FileIo *		fi= malloc( sizeof(FileIo) );

    const int		read= 1;
    const int		write= 0;
    const int		create= 0;
    const int		append= 0;
    const int		exclusive= 0;

    if  ( ! fi )
	{ XDEB(fi); goto ready;	}

    fi->fiOwnFd= 0;
    fi->fiFd= sioFdOpenFileS( filename,
				read, write, append, create, exclusive );
    if  ( fi->fiFd < 0 )
	{ SLDEB(filename,fi->fiFd); goto ready;	}
    fi->fiOwnFd= 1;

    sis= sioInOpen( (void *)fi, sioInFileioReadBytes, sioFileioClose );
    if  ( ! sis )
	{ XDEB(sis); goto ready;	}

    fi= (FileIo *)0; /* steal */

  ready:

    if  ( fi )
	{ sioFileioClose( fi );	}

    return sis;
    }

SimpleInputStream * sioInFileioOpen(	const MemoryBuffer *	filename )
    {
    return sioInFileioOpenS( utilMemoryBufferGetString( filename ) );
    }

static int sioOutFileioWriteBytes(	void *			vfi,
					const unsigned char *	buffer,
					int			count )
    {
    FileIo *	fi= (FileIo *)vfi;
    int		rval= write( fi->fiFd, buffer, count );

    if  ( rval < 0 )
	{ LLSDEB(fi->fiFd,errno,strerror(errno));	}

    return rval;
    }

static SimpleOutputStream * sioOutFileioOpenSAt( const char *	filename,
						int		append,
						long		offset )
    {
    SimpleOutputStream *	sos= (SimpleOutputStream *)0;
    FileIo *			fi= malloc( sizeof(FileIo) );
    int				flags= O_CREAT|O_WRONLY;
    const int			mode= 0666;

#   ifdef O_BINARY
    flags |= O_BINARY;
#   endif

    if  ( ! fi )
	{ XDEB(fi); goto ready;	}

    if  ( append )
	{ flags |= O_APPEND;	}
    else{
	if  ( offset == 0 )
	    { flags |= O_TRUNC;	}
	}

    fi->fiOwnFd= 0;
    fi->fiFd= open( filename, flags, mode );
    if  ( fi->fiFd < 0 )
	{ SLSDEB(filename,errno,strerror(errno)); goto ready;	}
    fi->fiOwnFd= 1;

    if  ( ! append && offset > 0 )
	{
	if  ( ftruncate( fi->fiFd, offset ) )
	    { SLSDEB(filename,errno,strerror(errno)); goto ready;	}

	if  ( lseek( fi->fiFd, offset, SEEK_SET ) != offset )
	    { SLSDEB(filename,errno,strerror(errno)); goto ready;	}
	}

    sos= sioOutOpen( (void *)fi, sioOutFileioWriteBytes, sioFileioClose );
    if  ( ! sos )
	{ XDEB(sos); goto ready; }

    fi= (FileIo *)0; /* steal */

  ready:

    if  ( fi )
	{ sioFileioClose( fi );	}

    return sos;
    }

SimpleOutputStream * sioOutFileioOpenS(	const char *	filename )
    {
    const int		append= 0;
    const long		offset= 0L;

    return sioOutFileioOpenSAt( filename, append, offset );
    }

SimpleOutputStream * sioOutFileioOpen(	const MemoryBuffer *	filename )
    {
    const int		append= 0;
    const long		offset= 0L;

    return sioOutFileioOpenSAt( utilMemoryBufferGetString( filename ),
							    append, offset );
    }

SimpleOutputStream * sioOutFileioOpenForAppend(
					const MemoryBuffer *	filename )
    {
    const int		append= 1;
    const long		offset= 0L;

    return sioOutFileioOpenSAt( utilMemoryBufferGetString( filename ),
							    append, offset );
    }

/************************************************************************/
/*									*/
/*  Simple output stream to a particular file descriptor.		*/
/*									*/
/************************************************************************/

static SimpleOutputStream * sioOutFdOpenAtX(
					int			fd,
					int			seek,
					long			offset )
    {
    SimpleOutputStream *	sos= (SimpleOutputStream *)0;
    FileIo *			fi= malloc( sizeof(FileIo) );

    if  ( ! fi )
	{ XDEB(fi); goto ready;	}

    fi->fiOwnFd= 0;
    fi->fiFd= fd;

    if  ( seek )
	{
	if  ( lseek( fi->fiFd, offset, SEEK_SET ) != offset )
	    { LLLSDEB(fi->fiFd,offset,errno,strerror(errno)); goto ready; }

	if  ( ftruncate( fi->fiFd, offset ) )
	    { LLSDEB(offset,errno,strerror(errno)); goto ready;	}
	}

    sos= sioOutOpen( (void *)fi, sioOutFileioWriteBytes, sioFileioClose );
    if  ( ! sos )
	{ XDEB(sos); goto ready;	}

    fi= (FileIo *)0; /* steal */

  ready:

    if  ( fi )
	{ sioFileioClose( fi );	}

    return sos;
    }

SimpleOutputStream * sioOutFdOpenAt(	int			fd,
					long			offset )
    {
    int		seek= 1;

    return sioOutFdOpenAtX( fd, seek, offset );
    }

SimpleOutputStream * sioOutFdOpen( int		fd )
    {
    int		seek= 0;
    long	offset= 0;

    return sioOutFdOpenAtX( fd, seek, offset );
    }

/************************************************************************/
/*									*/
/*  Simple input stream from a particular file descriptor.		*/
/*									*/
/************************************************************************/

SimpleInputStream * sioInFdOpenAt(	int			fd,
					long			offset )
    {
    SimpleInputStream *		sis= (SimpleInputStream *)0;
    FileIo *			fi= malloc( sizeof(FileIo) );

    if  ( ! fi )
	{ XDEB(fi); goto ready;	}

    fi->fiOwnFd= 0;
    fi->fiFd= fd;

    if  ( lseek( fi->fiFd, offset, SEEK_SET ) != offset )
	{ LLSDEB(offset,errno,strerror(errno)); goto ready;	}

    sis= sioInOpen( (void *)fi, sioInFileioReadBytes, sioFileioClose );
    if  ( ! sis )
	{ XDEB(sis); goto ready;	}

    fi= (FileIo *)0; /* steal */

  ready:

    if  ( fi )
	{ sioFileioClose( fi );	}

    return sis;
    }

SimpleInputStream * sioInFdOpen( int		fd )
    {
    const long	offset= 0;

    return sioInFdOpenAt( fd, offset );
    }

/************************************************************************/
/*									*/
/*  Make a temporary file and return an output stream for it.		*/
/*									*/
/************************************************************************/

SimpleOutputStream * sioOutFileioOpenTempFile(
				    MemoryBuffer *		filename,
				    const char *		template )
    {
    int				fd;
    SimpleOutputStream *	sos= (SimpleOutputStream *)0;

    if  ( utilMemoryBufferSetString( filename, template ) )
	{ SDEB(template); return (SimpleOutputStream *)0;	}

    /* This is known and intended to scribble */
    fd= mkstemp( (char *)utilMemoryBufferGetString( filename ) );
    if  ( fd < 0 )
	{ LDEB(fd); return (SimpleOutputStream *)0;	}

    sos= sioOutFdOpen( fd );
    if  ( ! sos )
	{
	if  ( close( fd ) )
	    { LLDEB(fd,errno);	}

	if  ( unlink( utilMemoryBufferGetString( filename ) ) )
	    { LLDEB(fd,errno);	}
	}

    return sos;
    }

/************************************************************************/
/*									*/
/*  Simple input stream from 'stdin'					*/
/*									*/
/************************************************************************/

SimpleInputStream * sioInStdinOpen( void )
    {
    return sioInFdOpen( 0 );
    }

/************************************************************************/
/*									*/
/*  Simple output stream to 'stdout'					*/
/*									*/
/************************************************************************/

SimpleOutputStream * sioOutStdoutOpen( void )
    {
    int		seek= 0;
    long	offset= 0;

    return sioOutFdOpenAtX( 1, seek, offset );
    }
