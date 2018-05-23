/************************************************************************/
/*									*/
/*  Elementary functionalite to emit XML.				*/
/*									*/
/************************************************************************/

#   ifndef		XML_WRITER_H
#   define		XML_WRITER_H

#   include		"sioGeneral.h"
#   include		"utilColor.h"
#   include		"utilMemoryBuffer.h"

typedef struct XmlWriter
    {
    SimpleOutputStream *	xwSos;
    int				xwCrlf;
    int				xwColumn;
    } XmlWriter;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void xmlInitXmlWriter(		XmlWriter *		xw );

extern void xmlPutString(		const char *		s,
					XmlWriter *		xw );

extern void xmlNewLine(		XmlWriter *		xw );

extern void xmlWriteStringAttribute(	XmlWriter *		xw,
					const char *		name,
					const char *		value );

extern void xmlWriteIntAttribute(	XmlWriter *		xw,
					const char *		name,
					int			value );

extern void xmlWriteRgb8Attribute(	XmlWriter *		xw,
					const char *		name,
					const RGB8Color *	rgb8 );

extern void xmlEscapeCharacters(	XmlWriter *		xw,
					const char *		ss,
					int			len );

extern void xmlEscapeBuffer(		XmlWriter *		xw,
					const MemoryBuffer *	mb );

#   endif	/*	XML_WRITER_H	*/

