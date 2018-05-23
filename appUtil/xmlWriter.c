/************************************************************************/
/*									*/
/*  Elementary XML writing functionality.				*/
/*									*/
/************************************************************************/

#   include	"appUtilConfig.h"

#   include	<stdio.h>
#   include	<string.h>

#   include	"sioGeneral.h"

#   include	"xmlWriter.h"

/************************************************************************/

void xmlInitXmlWriter(	XmlWriter *	xw )
    {
    xw->xwSos= (SimpleOutputStream *)0;
    xw->xwCrlf= 0;
    xw->xwColumn= 0;
    }

/************************************************************************/
/*									*/
/*  Save a tag with an argument.					*/
/*									*/
/************************************************************************/

void xmlPutString(		const char *		s,
				XmlWriter *		xw )
    {
    SimpleOutputStream *	sos= xw->xwSos;
    int				col= xw->xwColumn;

    /*  mime  */
    if  ( col == 0 && s[0] == '-' )
	{ sioOutPutString( "&#45;", sos ); col += 5; s++; }

    sioOutPutString( s, sos ); col += strlen( s );

    xw->xwColumn= col; return;
    }

void xmlNewLine(	XmlWriter *	xw )
    {
    SimpleOutputStream *	sos= xw->xwSos;

    if  ( xw->xwCrlf )
	{ (void)sioOutPutByte( '\r', sos );	}

    (void)sioOutPutByte( '\n', sos );
    xw->xwColumn= 0;
    }

void xmlWriteStringAttribute(	XmlWriter *		xw,
					const char *		name,
					const char *		value )
    {
    if  ( xw->xwColumn > 1				&&
	  xw->xwColumn+ 1+ strlen( name )+ 1+ 3 > 76	)
	{ sioOutPutString( "\n", xw->xwSos ); xw->xwColumn= 0;	}
    else{ sioOutPutString( " ", xw->xwSos ); (xw->xwColumn)++;	}

    sioOutPutString( name, xw->xwSos ); (xw->xwColumn) += strlen( name );
    sioOutPutString( "=\"", xw->xwSos ); (xw->xwColumn) += 2;
    xmlEscapeCharacters( xw, value, strlen( value ) );
    sioOutPutString( "\"", xw->xwSos ); (xw->xwColumn) += 1;

    return;
    }

void xmlWriteRgb8Attribute(	XmlWriter *		xw,
				const char *		name,
				const RGB8Color *	rgb8 )
    {
    char	scratch[20+1];

    sprintf( scratch, "#%02x%02x%02x",
			rgb8->rgb8Red, rgb8->rgb8Green, rgb8->rgb8Blue );

    xmlWriteStringAttribute( xw, name, scratch );
    }

void xmlWriteIntAttribute(		XmlWriter *		xw,
					const char *		name,
					int			value )
    {
    char	scratch[20];

    sprintf( scratch, "%d", value );

    xmlWriteStringAttribute( xw, name, scratch );

    return;
    }

void xmlEscapeCharacters(	XmlWriter *		xw,
				const char *		ss,
				int			len )
    {
    const unsigned char *	us= (const unsigned char *)ss;

    while( len > 0 )
	{
	const char *	escaped= (const char *)0;
	char		scratch[10];

	if  ( ( *us == '-' && xw->xwColumn == 0 )	||
	      *us == '<'				||
	      *us == '>'				||
	      *us == '&'				||
	      *us == '\''				||
	      *us == '"'				)
	    { sprintf( scratch, "&#%03d;", *us ); escaped= scratch;	}

	if  ( escaped )
	    {
	    sioOutPutString( escaped, xw->xwSos );
	    (xw->xwColumn) += strlen( escaped );
	    }
	else{
	    (void)sioOutPutByte( *us, xw->xwSos );
	    (xw->xwColumn)++;
	    }

	us++; len--;
	}

    return;
    }

void xmlEscapeBuffer(	XmlWriter *		xw,
			const MemoryBuffer *	mb )
    {
    xmlEscapeCharacters( xw, (const char *)mb->mbBytes, mb->mbSize );
    }
