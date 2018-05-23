/************************************************************************/
/*									*/
/*  Include a file from the file system in an object.			*/
/*									*/
/************************************************************************/

#   include	"docBaseConfig.h"

#   include	<appSystem.h>
#   include	"docObject.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Read a bitmap/pixmap image file.					*/
/*  Read a windows metafile.						*/
/*									*/
/************************************************************************/

static int docReadIncludedBitmapFile(	const MemoryBuffer *	fullName,
					InsertedObject *	io )
    { return docReadBitmapObject( io, fullName );	}

static int docCollectIncludedWmfFile(	const MemoryBuffer *	fullName,
					InsertedObject *	io )
    { return docReadWmfObject( io, fullName );	}

static int docCollectIncludedEmfFile(	const MemoryBuffer *	fullName,
					InsertedObject *	io )
    { return docReadEmfObject( io, fullName );	}

/************************************************************************/
/*									*/
/*  Evaluate an 'INCLUDEPICTURE' field.					*/
/*									*/
/************************************************************************/

int docReadFileObject(		const MemoryBuffer *	fullName,
				InsertedObject *	io )
    {
    int				res= -1;
    int				included= 0;

    MemoryBuffer	extension;

    utilInitMemoryBuffer( &extension );

    if  ( appFileGetFileExtension( &extension, fullName ) )
	{ LDEB(1); res= -1; goto ready;	}

    if  ( ! included							&&
	  ( utilMemoryBufferEqualsString( &extension, "ps" )	||
	    utilMemoryBufferEqualsString( &extension, "eps" )	||
	    utilMemoryBufferEqualsString( &extension, "PS" )	||
	    utilMemoryBufferEqualsString( &extension, "EPS" )	)	)
	{
	res= docReadEpsObject( fullName, io );
	if  ( res )
	    { LDEB(res); goto ready;	}
	else{ included= 1;		}
	}

    if  ( ! included							&&
	  ( utilMemoryBufferEqualsString( &extension, "wmf" )	||
	    utilMemoryBufferEqualsString( &extension, "WMF" ) 	)	)
	{
	res= docCollectIncludedWmfFile( fullName, io );
	if  ( res )
	    { LDEB(res); goto ready;	}
	else{ included= 1;		}
	}

    if  ( ! included							&&
	  ( utilMemoryBufferEqualsString( &extension, "emf" )	||
	    utilMemoryBufferEqualsString( &extension, "EMF" ) 	)	)
	{
	res= docCollectIncludedEmfFile( fullName, io );
	if  ( res )
	    { LDEB(res); goto ready;	}
	else{ included= 1;		}
	}

    if  ( ! included )
	{
	res= docReadIncludedBitmapFile( fullName, io );
	if  ( res )
	    { LDEB(res);	}
	}

  ready:

    utilCleanMemoryBuffer( &extension );

    return res;
    }

