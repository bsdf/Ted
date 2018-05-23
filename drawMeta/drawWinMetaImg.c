#   include	"drawMetaConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>

#   include	"drawMetafile.h"
#   include	"drawWinMetaImpl.h"
#   include	<sioEndian.h>

#   include	<appDebugon.h>

#   ifndef	M_PI
#	define	M_PI	3.14159265358979323846
#   endif

# if 0
#    define	WMFDEB(x)	(x)
# else
#    define	WMFDEB(x)	/*nothing*/
# endif

# if 0
#    define	WMFLOG(x)	(x)
# else
#    define	WMFLOG(x)	/*nothing*/
# endif

/************************************************************************/
/*									*/
/*  Retrieve the bitmap that might be in a WMF.				*/
/*									*/
/************************************************************************/

static int appMetaBitmapImageImg(	RasterImage **	pAbi,
					SimpleInputStream *	sis,
					int			expectBytes,
					int			srcX,
					int			srcY,
					int			srcXExt,
					int			srcYExt,
					int			dstX,
					int			dstY,
					int			dstXExt,
					int			dstYExt )
    {
    int			skip;
    RasterImage *	abi= (RasterImage *)0;

    if  ( srcX != 0 || srcY != 0 )
	{ LLDEB(srcX,srcY); return 0;	}

    if  ( appMetaReadBitmapImage( &skip, &abi, sis, expectBytes ) )
	{ LDEB(expectBytes); return -1;	}

    if  ( skip )
	{ return 0;	}

    if  ( srcXExt != abi->riDescription.bdPixelsWide	||
	  srcYExt != abi->riDescription.bdPixelsHigh	)
	{
	LLDEB(srcXExt,abi->riDescription.bdPixelsWide);
	LLDEB(srcYExt,abi->riDescription.bdPixelsHigh);
	bmCleanRasterImage( abi ); free( abi );
	return 0;
	}

    if  ( *pAbi )
	{ bmCleanRasterImage( *pAbi ); free( *pAbi ); }

    *pAbi= abi;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Play the metafile records in a metafile.				*/
/*									*/
/************************************************************************/

static int appWinMetaPlayRecordsImg(	SimpleInputStream *	sis,
					RasterImage **	pAbi )
    {
    RasterImage *	abi= (RasterImage *)0;

    for (;;)
	{
	int		x0;
	int		y0;
	int		w0;
	int		h0;

	int		x1;
	int		y1;
	int		w1;
	int		h1;

	long		recordSize;
	int		function;

	recordSize= sioEndianGetLeInt32( sis );
	function= sioEndianGetLeInt16( sis );

	if  ( recordSize < 0 || function < 0 )
	    { LLDEB(recordSize,function); return -1;	}

	switch( function )
	    {
	    case META_STRETCHBLT:
		(void) /* rop= */ sioEndianGetLeInt32( sis );
		h0= sioEndianGetLeInt16( sis );
		w0= sioEndianGetLeInt16( sis );
		y0= sioEndianGetLeInt16( sis );
		x0= sioEndianGetLeInt16( sis );
		h1= sioEndianGetLeInt16( sis );
		w1= sioEndianGetLeInt16( sis );
		y1= sioEndianGetLeInt16( sis );
		x1= sioEndianGetLeInt16( sis );

		WMFDEB(appDebug("StretchBlt([%d+%d,%d+%d]->[%d+%d,%d+%d],..)\n",
				x0,w0,y0,h0, x1,w1,y1,h1 ));

		if  ( appMetaBitmapImageImg( &abi, sis, 2*(recordSize-3-2-8*1),
					x0, y0, w0, h0, x1, y1, w1, h1 ) )
		    { LDEB(1); return -1; }

		continue;
	    case META_STRETCHDIB:
		(void) /* rop= */ sioEndianGetLeInt32( sis );
		(void) sioEndianGetLeInt16( sis );
		/*  source	*/
		h0= sioEndianGetLeInt16( sis );
		w0= sioEndianGetLeInt16( sis );
		y0= sioEndianGetLeInt16( sis );
		x0= sioEndianGetLeInt16( sis );
		/*  destination	*/
		h1= sioEndianGetLeInt16( sis );
		w1= sioEndianGetLeInt16( sis );
		y1= sioEndianGetLeInt16( sis );
		x1= sioEndianGetLeInt16( sis );

		WMFDEB(appDebug(
			"StretchDIBits([%d+%d,%d+%d]->[%d+%d,%d+%d],..)\n",
			x0,w0,y0,h0, x1,w1,y1,h1 ));

		if  ( appMetaBitmapImageImg( &abi, sis, 2*(recordSize-3-2-9*1),
					x0, y0, w0, h0, x1, y1, w1, h1 ) )
		    { LDEB(1); return -1; }

		continue;

	    case 0:
		if  ( recordSize != 3 )
		    { XLDEB(function,recordSize); return -1;	}
		break;

	    default:
		{
		int	done;

		for ( done= 3; done < recordSize; done++ )
		    { (void) /* ignore= */ sioEndianGetLeInt16( sis ); }
		continue;
		}
	    }

	break;
	}

    *pAbi= abi;
    return 0;
    }

/************************************************************************/
/*									*/
/*  Play the contents of a 'wmf' original windows metafile.		*/
/*									*/
/************************************************************************/

int appMetaPlayWmfImg(	BitmapDescription *	pBd,
			unsigned char **	pBuffer,
			SimpleInputStream *	sis )
    {
    /*
    int			fileType;
    int			headerSize;
    int			windowsVersion;
    long		fileSize;
    int			objectCount;
    long		maxRecordSize;
    int			parameterCount;
    */

    RasterImage *	abi= (RasterImage *)0;

    if  ( sioInGetByte( sis ) == EOF )
	{ return -1;	}
    sioInUngetLastRead( sis );

    (void) /* fileType= */ sioEndianGetLeInt16( sis );
    (void) /* headerSize= */ sioEndianGetLeInt16( sis );
    (void) /* windowsVersion= */ sioEndianGetLeInt16( sis );
    (void) /* fileSize= */ sioEndianGetLeInt32( sis );
    (void) /* objectCount= */ sioEndianGetLeInt16( sis );
    (void) /* maxRecordSize= */ sioEndianGetLeInt32( sis );
    (void) /* parameterCount= */ sioEndianGetLeInt16( sis );

    WMFDEB(appDebug( "IMG WMF ...\n" ));

    if  ( appWinMetaPlayRecordsImg( sis, &abi ) )
	{ LDEB(1); return -1; }

    if  ( ! abi )
	{ /*XDEB(abi);*/ return 1;	}

    *pBd= abi->riDescription;
    *pBuffer= abi->riBytes;

    free( abi );

    return 0;
    }

