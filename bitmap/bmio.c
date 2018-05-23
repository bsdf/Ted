#   include	"bitmapConfig.h"

#   include	<stdlib.h>
#   include	"bitmap.h"
#   include	<appDebugon.h>
#   include	<appSystem.h>

int bmWrite(	const MemoryBuffer *		filename,
		const unsigned char *		buffer,
		const BitmapDescription *	bd,
		int				fileFormat )
    {
    if  ( ! bmFileFormats[fileFormat].bffFileType->bftWrite )
	{
	LLDEB(fileFormat,bmFileFormats[fileFormat].bffFileType->bftWrite);
	return -1;
	}

    return (*bmFileFormats[fileFormat].bffFileType->bftWrite)(
			filename,
			buffer,
			bd,
			bmFileFormats[fileFormat].bffPrivate );
    }

int bmCanWrite( const BitmapDescription *	bd,
		int				fileFormat )
    {
    if  ( ! bmFileFormats[fileFormat].bffFileType->bftWrite )
	{ return -1;	}

    return (*bmFileFormats[fileFormat].bffFileType->bftCanWrite)(
			bd, bmFileFormats[fileFormat].bffPrivate );
    }

/************************************************************************/
/*									*/
/*  Read a bitmap/pixmap file. Derive the type from the file name.	*/
/*									*/
/************************************************************************/

int bmRead(	const MemoryBuffer *	filename,
		unsigned char **	pBuffer,
		BitmapDescription *	bd,
		int *			pFileFormat )
    {
    int			rval= -1;
    int			fileType;
    int			privateType;
    int			fileFormat;

    MemoryBuffer	extension;
    unsigned char *	buffer= (unsigned char *)0;


    utilInitMemoryBuffer( &extension );

    appFileGetFileExtension( &extension, filename );
    if  ( utilMemoryBufferIsEmpty( &extension ) )
	{
	SLDEB(utilMemoryBufferGetString(filename),extension.mbSize);
	goto ready;
	}

    for ( fileType= 0; fileType < bmNumberOfFileTypes; fileType++ )
	{
	if  ( ! bmFileTypes[fileType]->bftRead )
	    { continue;	}

	if  ( utilMemoryBufferEqualsString( &extension,
				bmFileTypes[fileType]->bftFileExtension ) )
	    {
	    if  ( (*bmFileTypes[fileType]->bftRead)( filename,
						&buffer, bd, &privateType ) )
		{ LDEB(fileType); break;	}

	    for (	fileFormat= 0;
			fileFormat < bmNumberOfFileFormats;
			fileFormat++				)
		{
		if  ( bmFileFormats[fileFormat].bffFileType->bftRead	==
		      bmFileTypes[fileType]->bftRead			&&
		      bmFileFormats[fileFormat].bffPrivate	==
		      privateType					)
		    {
		    *pBuffer= buffer; buffer= (unsigned char *)0; /* steal */
		    *pFileFormat= fileFormat;
		    rval= 0; goto ready;
		    }
		}

	    SLDEB(utilMemoryBufferGetString(filename),privateType); break;
	    }
	}

  ready:

    if  ( buffer )
	{ free( buffer );	}

    utilCleanMemoryBuffer( &extension );

    return rval;
    }
