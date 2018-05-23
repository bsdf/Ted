#   include	"bitmapConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>

#   include	<sioFileio.h>

#   include	"bmintern.h"
#   include	"bmio.h"
#   include	<appDebugon.h>

#   include	"bm_gif_lib.h"

int _GifError = 0;

/************************************************************************/
/*									*/
/*  Read a GIF file.							*/
/*									*/
/************************************************************************/


static int bmGifReadImageBuffer(BitmapDescription *	bd,
				GifFileType *		gft,
				unsigned char *		buffer,
				int			tc,
				int			interlaced )
    {
    int			row;

    int			wide= bd->bdPixelsWide;
    int			bpr= bd->bdBytesPerRow;

    int			ft= 0;
    int			cc;

    if  ( interlaced )
	{
	for ( row= 0; row < bd->bdPixelsHigh; row += 8 )
	    {
	    if  ( bmGifGetPixels( gft, &ft, buffer+ row* bpr, wide, tc ) !=
								    GIF_OK )
		{ LDEB(row); return -1; }
	    }
	for ( row= 4; row < bd->bdPixelsHigh; row += 8 )
	    {
	    if  ( bmGifGetPixels( gft, &ft, buffer+ row* bpr, wide, tc ) !=
								    GIF_OK )
		{ LDEB(row); return -1; }
	    }
	for ( row= 2; row < bd->bdPixelsHigh; row += 4 )
	    {
	    if  ( bmGifGetPixels( gft, &ft, buffer+ row* bpr, wide, tc ) !=
								    GIF_OK )
		{ LDEB(row); return -1; }
	    }
	for ( row= 1; row < bd->bdPixelsHigh; row += 2 )
	    {
	    if  ( bmGifGetPixels( gft, &ft, buffer+ row* bpr, wide, tc ) !=
								    GIF_OK )
		{ LDEB(row); return -1; }
	    }
	}
    else{
	for ( row= 0; row < bd->bdPixelsHigh; row++ )
	    {
	    if  ( bmGifGetPixels( gft, &ft, buffer+ row* bpr, wide, tc ) !=
								    GIF_OK )
		{ LDEB(row); return -1; }
	    }
	}

    if  ( ! ft )
	{ bd->bdHasAlpha= 0;	}

    cc= 0;
    if  ( bd->bdHasAlpha )
	{
	for ( row= 0; row < bd->bdPixelsHigh; row++ )
	    {
	    unsigned char *	to=   buffer+ row* bpr+ bpr-  1;
	    unsigned char *	from= buffer+ row* bpr+ wide- 1;
	    int		col;

	    for ( col= 0; col < bd->bdPixelsWide; col++ )
		{
		if  ( *from == tc )
		    { *(to--)= 0x00; }
		else{ *(to--)= 0xff; }

		if  ( *from >= cc )
		    { cc= *from+ 1;	}

		*(to--)= *(from--);
		}
	    }
	}
    else{
	for ( row= 0; row < bd->bdPixelsHigh; row++ )
	    {
	    unsigned char *	from= buffer+ row* bpr+ bpr-  1;
	    int		col;

	    for ( col= 0; col < bd->bdPixelsWide; col++ )
		{
		if  ( *from >= cc )
		    { cc= *from+ 1;	}

		from--;
		}
	    }
	}

    bd->bdPalette.cpColorCount= cc;

    return 0;
    }

int bmGifReadGif(		BitmapDescription *	bd,
				unsigned char **	pBuffer,
				SimpleInputStream *	sis )
    {
    int			rval= 0;

    GifFileType *	gft= (GifFileType *)0;
    GifImageDesc *	gid;
    int			transparentColor= -1;
    int			gotImage= 0;

    unsigned char *	buffer= (unsigned char *)0;

    gft= DGifOpenFileHandle( sis );
    if  ( ! gft )
	{ XDEB(gft); rval= -1; goto ready;	}

    gid= &(gft->gftCurrentImageDescriptor);

    for (;;)
	{
	GifRecordType		grt;
	unsigned int		row;

	GifColorMap *		gcm;
	const RGB8Color *	rgb8From;
	RGB8Color *		rgb8To;

	if  ( bmGifGetRecordType( gft, &grt ) != GIF_OK )
	    { LDEB(1); rval= -1; goto ready;	}

	switch( grt )
	    {
	    case UNDEFINED_RECORD_TYPE:
		LDEB(grt); continue;

	    case SCREEN_DESC_RECORD_TYPE:
		LDEB(grt); continue;

	    case IMAGE_DESC_RECORD_TYPE:
		if  ( gotImage )
		    { LDEB(gotImage); break;	}
		gotImage= 1;

		if  ( DGifGetImageDesc( gft ) != GIF_OK )
		    { LDEB(1); rval= -1; goto ready; }
		bd->bdPixelsWide= gid->Width;
		bd->bdPixelsHigh= gid->Height;
		if  ( transparentColor >= 0 )
		    { bd->bdHasAlpha= 1;	}
		else{ bd->bdHasAlpha= 0;	}
		bd->bdUnit= BMunINCH;
		bd->bdXResolution= 72;
		bd->bdYResolution= 72;

		bd->bdBitsPerSample= 8;
		if  ( bd->bdHasAlpha )
		    { bd->bdBitsPerPixel= 16;	}
		else{ bd->bdBitsPerPixel= 8;	}
		bd->bdColorEncoding= BMcoRGB8PALETTE;
		if  ( utilPaletteSetCount( &(bd->bdPalette), 256 ) )
		    { LDEB(256); rval= -1; goto ready;	}

		if  ( bmCalculateSizes( bd ) )
		    { LDEB(1); rval= -1; goto ready;	}

		buffer= (unsigned char *)malloc( bd->bdBufferLength );
		if  ( ! buffer )
		    { LLDEB(bd->bdBufferLength,buffer); rval= -1; goto ready; }

		if  ( gid->gidImageColorMap.gcmColorCount > 0 )
		    { gcm= &(gid->gidImageColorMap);		  }
		else{ gcm= &(gft->gftScreenDescriptor.gsdScreenColorMap); }

		bd->bdPalette.cpColorCount= gcm->gcmColorCount;

		rgb8From= gcm->gcmColors;
		rgb8To= bd->bdPalette.cpColors;
		for ( row= 0; row < bd->bdPalette.cpColorCount;
					    rgb8From++, rgb8To++, row++ )
		    {
		    rgb8To->rgb8Red= rgb8From->rgb8Red;
		    rgb8To->rgb8Green= rgb8From->rgb8Green;
		    rgb8To->rgb8Blue= rgb8From->rgb8Blue;
		    rgb8To->rgb8Alpha= 255* ( row != transparentColor );
		    }

		if  ( bmGifReadImageBuffer( bd, gft, buffer,
					transparentColor, gid->Interlace ) )
		    { LDEB(1); rval= -1; goto ready;	}

		continue;

	    case EXTENSION_RECORD_TYPE:
		{
		int		extensionType;
		unsigned char	extension[256];
		int		got= 1;

		if  ( DGifGetExtension( gft, &extensionType, extension )
								    != GIF_OK )
		    { LDEB(1); rval= -1; goto ready; }

		switch( extensionType )
		    {
		    case GRAPHICS_EXT_FUNC_CODE:
			if  ( extension[1] & 0x1 )
			    { transparentColor= extension[4];	}
			break;

		    case COMMENT_EXT_FUNC_CODE:
			XDEB(extensionType);
			appDebug( "Comment=\"%.*s\"\n",
						extension[0], extension+ 1 );
			break;

		    case PLAINTEXT_EXT_FUNC_CODE:
			XDEB(extensionType); break;

		    case APPLICATION_EXT_FUNC_CODE:
			XDEB(extensionType);
			appDebug( "Application=\"%.8s\"\n", extension+ 1 );
			break;

		    default:
			XDEB(extensionType); break;
		    }

		while( got )
		    {
		    if  ( DGifGetExtensionNext( gft, &got, extension )
								!= GIF_OK )
			{ LDEB(1); rval= -1; goto ready; }
		    }
		}
		continue;
	    case TERMINATE_RECORD_TYPE:
		break;
	    }

	break;
	}

    /* Try to make monochrome */
    if  ( ! bd->bdHasAlpha )
	{ bmMakeMonochrome( bd, buffer );	}

    /*
    *pPrivateFormat= privateFormat;
    */
    *pBuffer= buffer; buffer= (unsigned char *)0;

  ready:

    if  ( gft )
	{ DGifCloseFile( gft );	}
    if  ( buffer )
	{ free( buffer );	}

    return rval;
    }

int bmReadGifFile(	const MemoryBuffer *	filename,
			unsigned char **	pBuffer,
			BitmapDescription *	bd,
			int *			pPrivateFormat )
    {
    SimpleInputStream *	sis;

    sis= sioInFileioOpen( filename );
    if  ( ! sis )
	{ XDEB(sis); return -1;	}

    if  ( bmGifReadGif( bd, pBuffer, sis ) )
	{ LDEB(1); sioInClose( sis ); return -1;	}

    *pPrivateFormat= 87;

    sioInClose( sis );
    return 0;
    }

/************************************************************************/
/*									*/
/*  Write a GIF file.							*/
/*									*/
/************************************************************************/

int bmGifWriteGif(		const BitmapDescription *	bd,
				const unsigned char *		buffer,
				SimpleOutputStream *		sos )
    {
    GifFileType *		gft;

    GifColorMap			gcm;

    const int			left= 0;
    const int			top= 0;
    const int			interlace= 0;

    unsigned int		row;
    const unsigned char *	from;

    int				bpcolor;

    int				transparentColor= -1;
    int				backgroundColor= 0;

    gft= EGifOpenFileHandle( sos );
    if  ( ! gft )
	{ XDEB(gft); return -1;	}

    bmGifInitGifColorMap( &gcm );

    /*  1  */
    switch( bd->bdColorEncoding )
	{
	case BMcoWHITEBLACK:
	case BMcoBLACKWHITE:
	    if  ( bmMakeGrayPalette( bd, &(gcm.gcmColorCount),
				    &transparentColor, gcm.gcmColors, 256 ) )
		{ LDEB(bd->bdBitsPerPixel); return -1;	}

	    gcm.gcmBitsPerPixel= bd->bdBitsPerPixel;

	    break;

	case BMcoRGB8PALETTE:
	    bpcolor= 1;
	    while( ( 1 << bpcolor ) < bd->bdPalette.cpColorCount )
		{ bpcolor++;	}

	    if  ( bpcolor > 8 )
		{
		LDEB(bpcolor);
		LLDEB(bd->bdColorEncoding,bd->bdBitsPerPixel);
		return -1;
		}

	    gcm.gcmBitsPerPixel= bpcolor;
	    gcm.gcmColorCount= 1 << gcm.gcmBitsPerPixel;

	    for ( row= 0; row < bd->bdPalette.cpColorCount; row++ )
		{
		gcm.gcmColors[row]= bd->bdPalette.cpColors[row];

		if  ( transparentColor < 0				&&
		      bd->bdPalette.cpColors[row].rgb8Alpha == 0	)
		    { transparentColor= backgroundColor= row;	}
		}

	    if  ( bd->bdHasAlpha && transparentColor < 0 )
		{
		transparentColor= bd->bdPalette.cpColorCount;

		if  ( gcm.gcmBitsPerPixel < 8			&&
		      transparentColor >= gcm.gcmColorCount	)
		    {
		    gcm.gcmBitsPerPixel++;
		    gcm.gcmColorCount *= 2;
		    }

		if  ( transparentColor >= gcm.gcmColorCount )
		    {
		    LDEB(transparentColor);
		    transparentColor= backgroundColor= gcm.gcmColorCount- 1;
		    }
		}

	    break;

	case BMcoRGB:
	    switch( bd->bdBitsPerPixel )
		{
		case 4:
		case 8:
		default:
		    LLDEB(bd->bdColorEncoding,bd->bdBitsPerPixel);
		    return -1;
		}

	default:
	    LLDEB(bd->bdColorEncoding,bd->bdBitsPerPixel);
	    return -1;
	}

    if  ( bd->bdHasAlpha )
	{ bmGifSetVersion( gft, "89a" );	}
    else{ bmGifSetVersion( gft, "87a" );	}

    if  ( EGifPutScreenDesc( gft, bd->bdPixelsWide, bd->bdPixelsHigh,
				    gcm.gcmBitsPerPixel, 0, &gcm ) != GIF_OK )
	{ LDEB(1); EGifCloseFile( gft ); return -1;	}

    if  ( bd->bdHasAlpha )
	{
	unsigned char	extension[4];

	extension[0]= 0x01;
	extension[1]= 0;
	extension[2]= 0;
	extension[3]= transparentColor;

	if  ( EGifPutExtension( gft, GRAPHICS_EXT_FUNC_CODE,
				sizeof(extension), extension ) != GIF_OK )
	    { LDEB(1); EGifCloseFile( gft ); return -1;	}
	}

    if  ( EGifPutImageDesc( gft, left, top,
				bd->bdPixelsWide, bd->bdPixelsHigh,
						interlace, &gcm ) != GIF_OK )
	{ LDEB(1); EGifCloseFile( gft ); return -1;	}

    if  ( bd->bdBitsPerPixel == 8 && ! bd->bdHasAlpha )
	{
	from= buffer;
	for ( row= 0; row < bd->bdPixelsHigh; row++ )
	    {
	    if  ( bmGifPutPixels( gft, (unsigned char *)from,
						bd->bdPixelsWide ) != GIF_OK )
		{ LDEB(row); EGifCloseFile( gft ); return -1; }

	    from += bd->bdBytesPerRow;
	    }
	}
    else{
	unsigned char *		line;

	line= (unsigned char *)malloc( bd->bdPixelsWide+ 7 );
	if  ( ! line )
	    { XDEB(line); EGifCloseFile( gft ); return -1; }

	switch( bd->bdBitsPerPixel )
	    {
	    case 1: case 2: case 4: case 8: case 16:
		for ( row= 0; row < bd->bdPixelsHigh; row++ )
		    {
		    from= buffer+ row* bd->bdBytesPerRow;

		    bmInflateTo8bit( line, from, bd,
					    transparentColor, bd->bdHasAlpha );

		    if  ( bmGifPutPixels( gft, line, bd->bdPixelsWide )
								    != GIF_OK )
			{
			LDEB(row);
			EGifCloseFile( gft ); free( line );
			return -1;
			}
		    }
		break;

	    default:
		LDEB(bd->bdBitsPerPixel);
		EGifCloseFile( gft ); free( line );
		return -1;
	    }

	free( line );
	}

    EGifCloseFile( gft );

    return 0;
    }

int bmWriteGifFile(	const MemoryBuffer *		filename,
			const unsigned char *		buffer,
			const BitmapDescription *	bd,
			int				privateFormat )
    {
    SimpleOutputStream *	sos;

    if  ( bmCanWriteGifFile( bd, privateFormat ) )
	{ LDEB(1); return -1;	}

    sos= sioOutFileioOpen( filename );
    if  ( ! sos )
	{ XDEB(sos); return -1;	}

    if  ( bmGifWriteGif( bd, buffer, sos ) )
	{ LDEB(1); sioOutClose( sos ); return -1;	}
    
    sioOutClose( sos );
    return 0;
    }

/************************************************************************/
/*  Can this bitmap be written in GIF?					*/
/************************************************************************/

int bmCanWriteGifFile(	const BitmapDescription *	bd,
			int				privateFormat )
    {
    if  ( bd->bdBitsPerPixel <= 8 )
	{ return 0;	}

    if  ( bd->bdColorEncoding == BMcoRGB8PALETTE	&&
	  bd->bdHasAlpha				&&
	  bd->bdBitsPerPixel <= 16			)
	{ return 0;	}

    return -1;
    }
