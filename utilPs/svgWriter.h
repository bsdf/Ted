/************************************************************************/
/*									*/
/*  Save graphics to SVG.						*/
/*									*/
/************************************************************************/

#   ifndef		SVG_WRITER_H
#   define		SVG_WRITER_H

#   include		<xmlWriter.h>
#   include		<geo2DInteger.h>
#   include		<utilColor.h>

/************************************************************************/
/*									*/
/*  Information used when writing SVG.					*/
/*									*/
/************************************************************************/

typedef struct SvgWriter
    {
    XmlWriter			swXmlWriter;
				/**
				 *  The viewbox: The rectangle that is visible
				 *  to the user. The coordinates are in the 
				 *  in the same coordinate system as those
				 *  of the objects inside the SVG.
				 */
    DocumentRectangle		swViewBox;
				/**
				 *  The width in the parent coordinate units
				 */
    int				swWide;
				/**
				 *  The height in the parent coordinate units
				 */
    int				swHigh;
				/**
				 *  The unit for wide/high.
				 *  Note that all units in the spec have 
				 *  two characters: pt, pc, mm, cm, in
				 */
    char			swUnit[3];
				/**
				 *  true in a multipage document. Note that
				 *  at this point (march 2010) no browsers
				 *  support multipage svg.
				 */
    int				swMultiPage;

    int				swPatternCount;
    } SvgWriter;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void svgInitSvgWriter(	SvgWriter *		sw );

extern int svgStartDocument(	SvgWriter *		sw );
extern int svgFinishDocument(	SvgWriter *		sw );

extern int svgDefineShading(	SvgWriter *		sw,
				int			pattern,
				int			shading,
				const RGB8Color *	rgb8 );

extern void svgWriteRectangleAttributes(
				SvgWriter *			sw,
				const DocumentRectangle *	dr );

extern void svgWriteViewBoxAttribute(	SvgWriter *			sw,
					const char *			name,
					const DocumentRectangle *	dr );

#   endif	/*	SVG_WRITER_H	*/

