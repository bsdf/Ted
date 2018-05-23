#   include	"drawMetaConfig.h"

#   include	<stddef.h>
#   include	<string.h>

#   include	"drawImageSvg.h"
#   include	<sioBase64.h>
#   include	<sioMemory.h>
#   include	<sioHex.h>
#   include	<bmio.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Start an image.							*/
/*									*/
/************************************************************************/

static int drawSvgStartImage(	SvgWriter *			sw,
				const DocumentRectangle *	drDest )
    {
    XmlWriter *			xw= &(sw->swXmlWriter);

    xmlPutString( "<image ", xw );

    xmlWriteStringAttribute( xw, "overflow", "visible" );
    xmlWriteStringAttribute( xw, "preserveAspectRatio", "none" );
    svgWriteRectangleAttributes( sw, drDest );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Insert a raster image in the SVG output.				*/
/*									*/
/************************************************************************/

int drawRasterImageSvg(		SvgWriter *			sw,
				const RasterImage *		abiSrc,
				const DocumentRectangle *	drSrc,
				const DocumentRectangle *	drDest )
    {
    int				rval= 0;
    XmlWriter *			xw= &(sw->swXmlWriter);
    const RasterImage *	abi= abiSrc;

    RasterImage		abiDest;

    SimpleOutputStream *	sosBase64= (SimpleOutputStream *)0;

    bmInitRasterImage( &abiDest );

    if  ( drSrc )
	{
	if  ( drSrc->drX0 != 0					||
	      drSrc->drY0 != 0					||
	      drSrc->drX1 != abiSrc->riDescription.bdPixelsWide- 1	||
	      drSrc->drY1 != abiSrc->riDescription.bdPixelsHigh- 1	)
	    {
	    if  ( bmSelect( &abiDest, abiSrc, drSrc ) )
		{ LDEB(1); rval= -1; goto ready;	}

	    abi= &abiDest;
	    }
	}

    drawSvgStartImage( sw, drDest );

    xmlPutString( " xlink:href=\"data:image/png;base64,", xw );
    xmlNewLine( xw );

    sosBase64= sioOutBase64Open( xw->xwSos );
    if  ( ! sosBase64 )
	{ XDEB(sosBase64); rval= -1; goto ready;	}
    if  ( bmPngWritePng( &(abi->riDescription), abi->riBytes, sosBase64 ) )
	{ XDEB(sosBase64); rval= -1; goto ready;	}
    sioOutClose( sosBase64 ); sosBase64= (SimpleOutputStream *)0;
    xmlPutString( "\"", xw );
    xmlNewLine( xw );

    xmlPutString( "/>", xw );
    xmlNewLine( xw );

  ready:

    bmCleanRasterImage( &abiDest );

    if  ( sosBase64 )
	{ sioOutClose( sosBase64 );	}

    return rval;
    }


/************************************************************************/
/*									*/
/*  Insert an image from data in the SVG output.			*/
/*									*/
/************************************************************************/

int drawRasterImageSvgFromData(	SvgWriter *			sw,
				const char *			contentType,
				const MemoryBuffer *		mb,
				const DocumentRectangle *	drDest )
    {
    int				rval= 0;
    XmlWriter *			xw= &(sw->swXmlWriter);

    SimpleOutputStream *	sosBase64= (SimpleOutputStream *)0;
    SimpleInputStream *		sisMem= (SimpleInputStream *)0;
    SimpleInputStream *		sisHex= (SimpleInputStream *)0;

    int				done;
    unsigned char		buf[1024];

    drawSvgStartImage( sw, drDest );

    xmlPutString( " xlink:href=\"data:", xw );
    xmlEscapeCharacters( xw, contentType, strlen( contentType ) );
    xmlPutString( ";base64,", xw );
    xmlNewLine( xw );

    sosBase64= sioOutBase64Open( xw->xwSos );
    if  ( ! sosBase64 )
	{ XDEB(sosBase64); rval= -1; goto ready;	}

    sisMem= sioInMemoryOpen( mb );
    if  ( ! sisMem )
	{ XDEB(sisMem); rval= -1; goto ready;	}
    sisHex= sioInHexOpen( sisMem );
    if  ( ! sisHex )
	{ XDEB(sisHex); rval= -1; goto ready;	}

    while( ( done= sioInReadBytes( sisHex, buf, sizeof(buf) ) ) > 0 )
	{
	if  ( sioOutWriteBytes( sosBase64, buf, done ) != done )
	    { LDEB(done);	}
	}

    sioOutClose( sosBase64 ); sosBase64= (SimpleOutputStream *)0;
    xmlPutString( "\"", xw );
    xmlNewLine( xw );

    xmlPutString( "/>", xw );
    xmlNewLine( xw );

  ready:

    if  ( sisHex )
	{ sioInClose( sisHex );	}
    if  ( sisMem )
	{ sioInClose( sisMem );	}
    if  ( sosBase64 )
	{ sioOutClose( sosBase64 );	}

    return rval;
    }

