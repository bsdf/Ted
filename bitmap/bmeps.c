#   include	"bitmapConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<string.h>

#   include	<sioFileio.h>

#   include	"bmintern.h"
#   include	"bmBitmapPrinter.h"

#   include	<psPrint.h>
#   include	<utilMemoryBufferPrintf.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  We do our best to print anything.					*/
/*									*/
/************************************************************************/

int bmCanWriteEpsFile(	const BitmapDescription *	bd,
			int				privateFormat )
    {
    switch( bd->bdBitsPerSample )
	{
	case 1: case 2: case 4: case 8: case 16:
	    return 0;
	default:
	    /* LDEB(bd->bdBitsPerSample); */ return -1;	
	}
    }


/************************************************************************/
/*									*/
/*  Save a bitmap image to Encapsulated PostScript (EPS) Format.	*/
/*									*/
/*  1)  Only 1,2,4,8,16 bits per sample images are supported.		*/
/*  2)  Open the output file.						*/
/*  3)  Determine the bounding box of the image.			*/
/*  4)  Issue the EPS header comments.					*/
/*  5)  Issue the Image contents.					*/
/*  6)  Include a 'showpage' command.					*/
/*  7)  Finish with a general header.					*/
/*									*/
/************************************************************************/

static int bmWriteEps(	SimpleOutputStream *		sos,
			const char *			filename,
			const unsigned char *		buffer,
			const BitmapDescription *	bd,
			int				privateFormat )
    {
    int				useFilters= 1;
    int				indexedImages= 1;

    if  ( privateFormat < 2 )
	{ useFilters= indexedImages= 0;	}

    /*  1  */
    switch( bd->bdBitsPerSample )
	{
	case 1:
	case 2:
	case 4:
	case 8:
	case 16:
	    break;

	default:
	    LDEB(bd->bdBitsPerSample); return -1;	
	}

    bmStartEpsFile( sos, filename, bd );

    psSetPdfmarkEmulation( sos );
    if  ( useFilters )
	{ psImageQualityDistillerparams( sos );     }

    /*  5  */
    if  ( bmPsPrintBitmap( sos, privateFormat, 1.0, 1.0, 0, 0,
				    (const DocumentRectangle *)0,
				    useFilters, indexedImages, bd, buffer ) )
	{ LDEB(1); return -1;	}

    psFinishEpsFile( sos );

    return 0;
    }

int bmWriteEpsFile(	const MemoryBuffer *		filename,
			const unsigned char *		buffer,
			const BitmapDescription *	bd,
			int				privateFormat )
    {
    int				rval= 0;
    SimpleOutputStream *	sos;

    char *			psLevel= getenv( "PSLEVEL" );

    if  ( psLevel && ! strcmp( psLevel, "2" ) )
	{ privateFormat= 2;	}

    /*  2  */
    sos= sioOutFileioOpen( filename );
    if  ( ! sos )
	{ XDEB(sos); rval= -1; goto ready;	}

    if  ( bmWriteEps( sos, utilMemoryBufferGetString( filename ), buffer, bd, privateFormat ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    sioOutClose( sos );

    return rval;
    }

void bmStartEpsFile(	SimpleOutputStream *		sos,
			const char *			filename,
			const BitmapDescription *	bd )
    {
    int			twipsWide;
    int			twipsHigh;

    /*  3  */
    bmImageSizeTwips( &twipsWide, &twipsHigh, bd );

    /*  4  */
    psStartEpsFile( sos, 
		    "Scan, Mark de Does, mark@mdedoes.com",
		    filename, 
		    ( twipsWide+ 19 )/ 20, ( twipsHigh+ 19 )/ 20	);

    return;
    }

/************************************************************************/

# include <sioPipe.h>

int bmCanWritePdfFile(	const BitmapDescription *	bd,
			int				privateFormat )
    { return bmCanWriteEpsFile( bd, 2 );	}

int bmWritePdfFile(	const MemoryBuffer *		filename,
			const unsigned char *		buffer,
			const BitmapDescription *	bd,
			int				privateFormat )
    {
    int				rval= 0;
    SimpleOutputStream *	sos= (SimpleOutputStream *)0;
    int				rectangleWideTwips;
    int				rectangleHighTwips;

    MemoryBuffer		command;

    utilInitMemoryBuffer( &command );

    if  ( strchr( (char *)filename->mbBytes, '\'' )	||
	  filename->mbSize < 1				||
	  filename->mbBytes[filename->mbSize-1] == '\\'	)
	{ LDEB(1); rval= -1; goto ready;	}

    bmRectangleSizeTwips( &rectangleWideTwips, &rectangleHighTwips,
				    bd, bd->bdPixelsWide, bd->bdPixelsHigh );

    utilMemoryBufferPrintf( &command,
	    "ps2pdf -dDEVICEWIDTHPOINTS=%d -dDEVICEHEIGHTPOINTS=%d - '%s'",
				( rectangleWideTwips+ 19)/20,
				( rectangleHighTwips+ 19)/20,
				utilMemoryBufferGetString( filename ) );

    sos= sioOutPipeOpen( &command );
    if  ( ! sos )
	{ XDEB(sos); rval= -1; goto ready;	}

    if  ( bmWriteEps( sos, utilMemoryBufferGetString( filename ), buffer, bd, 2 ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    utilCleanMemoryBuffer( &command );

    if  ( sos )
	{
	if  ( sioOutClose( sos ) )
	    { LDEB(1); rval= -1;	}
	}

    return rval;
    }
