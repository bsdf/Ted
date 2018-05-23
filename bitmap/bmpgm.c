#   include	"bitmapConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<ctype.h>
#   include	<string.h>

#   include	"bmintern.h"
#   include	<appDebugon.h>
#   include	<sioFileio.h>

/************************************************************************/
/*									*/
/*  Read a PBM/PGM/PPM file.						*/
/*									*/
/************************************************************************/

static int bmPbmGetNumber(	int *			pNum,
				SimpleInputStream *	sis )
    {
    int		num= 0;
    int		c= sioInGetByte( sis );

    for (;;)
	{
	if  ( isspace( c ) )
	    { c= sioInGetByte( sis ); continue;	}

	if  ( isdigit( c ) )
	    { break;	}

	if  ( c == '#' )
	    {
	    for (;;)
		{
		c= sioInGetByte( sis );
		if  ( c == EOF )
		    { CDEB(c); return -1;	}
		if  ( c == '\n' )
		    { c= sioInGetByte( sis ); break;	}
		}

	    continue;
	    }

	CDEB(c); return -1;
	}

    while( isdigit( c ) )
	{ num= 10* num+ c- '0'; c= sioInGetByte( sis ); }

    sioInUngetLastRead( sis ); *pNum= num; return 0;
    }

int bmReadPbmFile(	const MemoryBuffer *	filename,
			unsigned char **	pBuf,
			BitmapDescription *	bd,
			int *			pPrivateFormat )
    {
    SimpleInputStream *		sis;
    int				privateFormat;
    int				c;
    int				row;
    int				col;

    int				wide;
    int				high;
    int				mval= 1;
    int				shift;

    unsigned char *		buffer;
    unsigned char *		to;

    sis= sioInFileioOpen( filename );
    if  ( ! sis )
	{ XDEB(sis); return -1;	}

    c= sioInGetByte( sis );
    if  ( c != 'P' )
	{ CDEB(c); sioInClose( sis ); return -1;	}

    c= sioInGetByte( sis );
    switch( c )
	{
	case '1': case '2': case '3': case '4': case '5': case '6':
	    privateFormat= c- '0'; break;
	default:
	    CDEB(c); sioInClose( sis ); return -1;
	}

    if  ( bmPbmGetNumber( &wide, sis ) )
	{ LDEB(1); sioInClose( sis ); return -1;	}
    if  ( bmPbmGetNumber( &high, sis ) )
	{ LDEB(1); sioInClose( sis ); return -1;	}

    bd->bdXResolution= bd->bdYResolution= 1;
    bd->bdUnit= BMunPIXEL;

    bd->bdHasAlpha= 0;

    bd->bdPixelsWide= wide;
    bd->bdPixelsHigh= high;

    switch( privateFormat )
	{
	case 1: case 4:
	    bd->bdColorEncoding= BMcoBLACKWHITE;
	    bd->bdSamplesPerPixel= 1;
	    bd->bdBitsPerSample= 1;
	    break;

	case 2: case 5:
	    if  ( bmPbmGetNumber( &mval, sis ) )
		{ LDEB(1); sioInClose( sis ); return -1;	}

	    bd->bdColorEncoding= BMcoWHITEBLACK;
	    bd->bdSamplesPerPixel= 1;
	    bd->bdBitsPerSample= 8;
	    break;

	case 3: case 6:
	    if  ( bmPbmGetNumber( &mval, sis ) )
		{ LDEB(1); sioInClose( sis ); return -1;	}

	    bd->bdColorEncoding= BMcoRGB;
	    bd->bdBitsPerSample= 8;
	    break;

	default:
	    CDEB(c); sioInClose( sis ); return -1;
	}

    bmCalculateSizes( bd );

    buffer= (unsigned char *)malloc( bd->bdBufferLength );
    if  ( ! buffer )
	{ XDEB(buffer); sioInClose( sis ); return -1;	}

    switch( privateFormat )
	{
	case 1:
	    memset( buffer, 0, bd->bdBufferLength );
	    for ( row= 0; row < high; row++ )
		{
		to= buffer+ row* bd->bdBytesPerRow; shift= 7;
		for ( col= 0; col < wide; col++ )
		    {
		    if  ( bmPbmGetNumber( &c, sis ) )
			{
			LLDEB(row,col);
			free( buffer ); sioInClose( sis ); return -1;
			}
		    *to |= ( c != 0 ) << shift;
		    if  ( shift == 0 )
			{ shift= 7; to++;	}
		    else{ shift--;		}
		    }
		}
	    break;

	case 4:
	    c= sioInGetByte( sis );
	    for ( row= 0; row < high; row++ )
		{
		to= buffer+ row* bd->bdBytesPerRow;
		if  ( sioInReadBytes( sis, to, bd->bdBytesPerRow ) !=
							bd->bdBytesPerRow )
		    {
		    LDEB(bd->bdBufferLength);
		    free( buffer ); sioInClose( sis ); return -1;
		    }
		}
	    break;

	case 5:
	    switch( bd->bdBitsPerPixel )
		{
		case 8:
		    c= sioInGetByte( sis );
		    if  ( sioInReadBytes( sis, buffer, bd->bdBufferLength ) !=
							bd->bdBufferLength )
			{
			LDEB(bd->bdBufferLength);
			free( buffer ); sioInClose( sis ); return -1;
			}
		    break;
		case 1: case 2: case 4:
		default:
		    LLDEB(privateFormat,bd->bdBitsPerPixel);
		    free( buffer ); sioInClose( sis ); return -1;
		}
	    break;

	case 2:
	    switch( bd->bdBitsPerPixel )
		{
		case 8:
		    to= buffer;
		    for ( col= 0; col < bd->bdBufferLength; col++ )
			{
			if  ( bmPbmGetNumber( &c, sis ) )
			    {
			    LDEB(col);
			    free( buffer ); sioInClose( sis );
			    return -1;
			    }
			if  ( c > 255 )
			    { CDEB(c); c= 255;	}
			*(to++)= ( 255* c )/ mval;
			}
		    break;
		case 1: case 2: case 4:
		default:
		    LLDEB(privateFormat,bd->bdBitsPerPixel);
		    free( buffer ); sioInClose( sis ); return -1;
		}
	    break;

	case 3:
	    switch( bd->bdBitsPerSample )
		{
		case 8:
		    to= buffer;
		    for ( col= 0; col < bd->bdBufferLength; col++ )
			{
			if  ( bmPbmGetNumber( &c, sis ) )
			    {
			    LDEB(col);
			    free( buffer ); sioInClose( sis );
			    return -1;
			    }
			if  ( c > 255 )
			    { CDEB(c); c= 255;	}
			*(to++)= ( 255* c )/ mval;
			}
		    break;
		case 1: case 2: case 4:
		default:
		    LLDEB(privateFormat,bd->bdBitsPerSample);
		    free( buffer ); sioInClose( sis ); return -1;
		}
	    break;

	case 6:
	    switch( bd->bdBitsPerSample )
		{
		case 8:
		    c= sioInGetByte( sis );
		    if  ( sioInReadBytes( sis, buffer, bd->bdBufferLength ) !=
							bd->bdBufferLength )
			{
			LDEB(bd->bdBufferLength);
			free( buffer ); sioInClose( sis ); return -1;
			}
		    break;
		case 1: case 2: case 4:
		default:
		    LLDEB(privateFormat,bd->bdBitsPerSample);
		    free( buffer ); sioInClose( sis ); return -1;
		}
	    break;

	default:
	    free( buffer );
	    LDEB(privateFormat); sioInClose( sis ); return -1;
	}

    sioInClose( sis );

    *pBuf= buffer;
    *pPrivateFormat= privateFormat;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Write a PGM file.							*/
/*									*/
/************************************************************************/

int bmCanWritePbmFile(	const BitmapDescription *	bd,
			int				privateFormat )
    {
    if  ( bd->bdHasAlpha )
	{ return -1;	}

    if  ( privateFormat == 1			&& 
	  bd->bdColorEncoding == BMcoWHITEBLACK	&&
	  bd->bdBitsPerPixel == 1		)
	{ return 0; }
    if  ( privateFormat == 1			&& 
	  bd->bdColorEncoding == BMcoBLACKWHITE	&&
	  bd->bdBitsPerPixel == 1		)
	{ return 0; }

    if  ( privateFormat == 2			&& 
	  bd->bdColorEncoding == BMcoWHITEBLACK	&&
	  bd->bdBitsPerPixel == 1		)
	{ return 0; }
    if  ( privateFormat == 2			&& 
	  bd->bdColorEncoding == BMcoBLACKWHITE	&&
	  bd->bdBitsPerPixel == 1		)
	{ return 0; }

    if  ( privateFormat == 4			&& 
	  bd->bdColorEncoding == BMcoBLACKWHITE	&&
	  bd->bdBitsPerPixel == 1		)
	{ return 0; }

    if  ( privateFormat == 5			&& 
	  bd->bdColorEncoding == BMcoWHITEBLACK	&&
	  bd->bdBitsPerPixel == 8		)
	{ return 0; }

    return -1;
    }

/************************************************************************/
/*									*/
/*  Write some kinds of pbm files. Code is written to visually inspect	*/
/*  the files do debug images. Simplicity is preferred over efficiency.	*/
/*									*/
/************************************************************************/

static void bmWriteBitmapBitsAscii(
			SimpleOutputStream *		sos,
			const unsigned char *		buffer,
			const BitmapDescription *	bd,
			const char *			b1,
			const char *			b0 )
    {
    int		row;

    for ( row= 0; row < bd->bdPixelsHigh; row++ )
	{
	int			col;
	const unsigned char *	r= buffer+ row* bd->bdBytesPerRow;

	for ( col= 0; col < bd->bdPixelsWide; col++ )
	    {
	    if  ( r[col/8] & ( 0x80 >> ( col % 8 ) ) )
		{ sioOutPutString( b1, sos );	}
	    else{ sioOutPutString( b0, sos );	}
	    }

	sioOutPrintf( sos, "\n" );
	}

    return;
    }

static void bmStartPbm(	SimpleOutputStream *		sos,
			int				privateFormat,
			const BitmapDescription *	bd )
    {
    sioOutPrintf( sos, "P%d\n", privateFormat );

    sioOutPrintf( sos, "%d %d\n\n", bd->bdPixelsWide, bd->bdPixelsHigh );
    return;
    }

static void bmStartPnm(	SimpleOutputStream *		sos,
			int				privateFormat,
			const BitmapDescription *	bd )
    {
    int		mval= ( 1 << bd->bdBitsPerSample )- 1;


    sioOutPrintf( sos, "P%d\n", privateFormat );

    sioOutPrintf( sos, "%d %d %d\n\n",
				bd->bdPixelsWide, bd->bdPixelsHigh, mval );
    return;
    }

static void bmPbmRuler(	SimpleOutputStream *		sos,
			const BitmapDescription *	bd )
    {
    int	col;

    sioOutPrintf( sos, "#0" );

    for ( col= 1; col < bd->bdPixelsWide; col++ )
	{ sioOutPrintf( sos, " %d", col % 10 ); }

    sioOutPrintf( sos, "\n" );
    }

int bmWritePbmFile(	const MemoryBuffer *		filename,
			const unsigned char *		buffer,
			const BitmapDescription *	bd,
			int				privateFormat )
    {
    SimpleOutputStream *	sos= (SimpleOutputStream *)0;
    int				rval= 0;

    if  ( bd->bdHasAlpha )
	{ LDEB(bd->bdHasAlpha); rval= -1; goto ready;	}

    sos= sioOutFileioOpen( filename );
    if  ( ! sos )
	{ XDEB(sos); rval= -1; goto ready;	}

    switch( privateFormat )
	{
	case 1:
	    switch( bd->bdColorEncoding )
		{
		case BMcoWHITEBLACK:
		    switch( bd->bdBitsPerPixel )
			{
			case 1:
			    bmStartPbm( sos, privateFormat, bd );

			    if  ( bd->bdPixelsWide <= 40	)
				{ bmPbmRuler( sos, bd );	}

			    bmWriteBitmapBitsAscii( sos, buffer, bd,
								" 0", " 1" );
			    break;

			default:
			    LDEB(bd->bdBitsPerPixel); rval= -1; goto ready;
			}
		    break;

		case BMcoBLACKWHITE:
		    switch( bd->bdBitsPerPixel )
			{
			case 1:
			    bmStartPbm( sos, privateFormat, bd );

			    if  ( bd->bdPixelsWide <= 40	)
				{ bmPbmRuler( sos, bd );	}

			    bmWriteBitmapBitsAscii( sos, buffer, bd,
								" 1", " 0" );
			    break;

			default:
			    LDEB(bd->bdBitsPerPixel); rval= -1; goto ready;
			}
		    break;

		default:
		    LDEB(bd->bdColorEncoding); rval= -1; goto ready;
		}
	    break;

	case 2:
	    switch( bd->bdColorEncoding )
		{
		case BMcoWHITEBLACK:
		    switch( bd->bdBitsPerPixel )
			{
			case 1:
			    bmStartPnm( sos, privateFormat, bd );

			    if  ( bd->bdPixelsWide <= 40	)
				{ bmPbmRuler( sos, bd );	}

			    bmWriteBitmapBitsAscii( sos, buffer, bd,
								" 1", " 0" );
			    break;

			default:
			    LDEB(bd->bdBitsPerPixel); rval= -1; goto ready;
			}
		    break;

		case BMcoBLACKWHITE:
		    switch( bd->bdBitsPerPixel )
			{
			case 1:
			    bmStartPnm( sos, privateFormat, bd );

			    if  ( bd->bdPixelsWide <= 40	)
				{ bmPbmRuler( sos, bd );	}

			    bmWriteBitmapBitsAscii( sos, buffer, bd,
								" 0", " 1" );
			    break;

			default:
			    LDEB(bd->bdBitsPerPixel); rval= -1; goto ready;
			}
		    break;

		default:
		    LDEB(bd->bdColorEncoding); rval= -1; goto ready;
		}
	    break;

	case 4:
	    switch( bd->bdColorEncoding )
		{
		case BMcoBLACKWHITE:
		    switch( bd->bdBitsPerPixel )
			{
			case 1:
			    bmStartPbm( sos, privateFormat, bd );
			    sioOutWriteBytes( sos, buffer, bd->bdBufferLength );
			    break;

			default:
			    LDEB(bd->bdBitsPerPixel); rval= -1; goto ready;
			}
		    break;

		case BMcoWHITEBLACK:
		default:
		    LDEB(bd->bdColorEncoding); rval= -1; goto ready;
		}
	    break;

	case 5:
	    switch( bd->bdColorEncoding )
		{
		case BMcoWHITEBLACK:
		    switch( bd->bdBitsPerPixel )
			{
			case 8:
			    bmStartPnm( sos, privateFormat, bd );

			    sioOutWriteBytes( sos, buffer, bd->bdBufferLength );
			    break;

			default:
			    LDEB(bd->bdBitsPerPixel); rval= -1; goto ready;
			}
		    break;

		case BMcoBLACKWHITE:
		default:
		    LDEB(bd->bdColorEncoding); rval= -1; goto ready;
		}
	    break;


	default:
	    LDEB(privateFormat); rval= -1; goto ready;
	}

  ready:

    if  ( sos )
	{ sioOutClose( sos );	}

    return rval;
    }
