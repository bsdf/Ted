/************************************************************************/
/*  An image description and its data.					*/
/************************************************************************/

#   include	"bitmapConfig.h"

#   include	<stdlib.h>
#   include	<string.h>

#   include	"bitmap.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Clean an image structure.						*/
/*									*/
/************************************************************************/

void bmCleanRasterImage(	RasterImage *	ri )
    {
    if  ( ri->riBytes )
	{
	free( ri->riBytes );
	ri->riBytes= (unsigned char *)0;
	}

    bmCleanDescription( &ri->riDescription );
    bmInitDescription( &ri->riDescription );
    }

void bmFreeRasterImage(	RasterImage *	ri )
    {
    bmCleanRasterImage( ri );
    free( ri );
    }

void bmInitRasterImage(	RasterImage *	ri )
    {
    bmInitDescription( &(ri->riDescription) );
    ri->riBytes= (unsigned char *)0;
    ri->riFormat= -1;
    }

int bmCopyRasterImage(	RasterImage *		to,
			const RasterImage *	from )
    {
    int			rval= 0;
    unsigned char *	copy= (unsigned char *)0;

    copy= (unsigned char *)malloc( from->riDescription.bdBufferLength );
    if  ( ! copy )
	{
	LXDEB(from->riDescription.bdBufferLength,copy);
	rval= -1; goto ready;
	}
    memcpy( copy, from->riBytes, from->riDescription.bdBufferLength );

    if  ( bmCopyDescription( &(to->riDescription), &(from->riDescription) ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( to->riBytes )
	{ free(to->riBytes);	}
    to->riBytes= copy; copy= (unsigned char *)0; /* steal */

  ready:

    if  ( copy )
	{ free( copy );	}

    return rval;
    }

