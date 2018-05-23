/************************************************************************/
/*									*/
/*  Issue SVG primitives.						*/
/*  See: http://www.w3.org/TR/SVG11/					*/
/*									*/
/************************************************************************/

#   include	"utilPsConfig.h"

#   include	<stdio.h>
#   include	<ctype.h>

#   include	<sioGeneral.h>
#   include	"svgWriter.h"

#   include	<appDebugon.h>

/************************************************************************/

static const char	DocSvgXmlDeclaration[]=
			"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";

static const char *	DocSvgSvgStart[]=
{
  DocSvgXmlDeclaration,
# if 0
  "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\"\n",
  "  \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">\n",
# endif
};

static const char DocSvgSvgHead[]=
	"  <svg "
	    " width=\"%d%s\""
	    " height=\"%d%s\""
	    " viewBox=\"%d %d %d %d\""
	    /*
	    " preserveAspectRatio=\"none\""
	    */
	    " xmlns=\"http://www.w3.org/2000/svg\""
	    " xmlns:xlink=\"http://www.w3.org/1999/xlink\""
	    " xmlns:html=\"http://www.w3.org/1999/xhtml\""
	    " version=\"1.1\">\n";

static const char DocSvgSvgTail[]=
	"  </svg>\n";

/************************************************************************/

int svgStartDocument(	SvgWriter *	sw )
    {
    int		l;

    for ( l= 0; l < sizeof(DocSvgSvgStart)/sizeof(char *); l++ )
	{ sioOutPutString( DocSvgSvgStart[l], sw->swXmlWriter.xwSos );	}

    sioOutPrintf( sw->swXmlWriter.xwSos, DocSvgSvgHead,
			    sw->swWide, sw->swUnit,
			    sw->swHigh, sw->swUnit,
			    sw->swViewBox.drX0,
			    sw->swViewBox.drY0,
			    sw->swViewBox.drX1- sw->swViewBox.drX0+ 1,
			    sw->swViewBox.drY1- sw->swViewBox.drY0+ 1 );

    return 0;
    }

int svgFinishDocument(	SvgWriter *	sw )
    {
    sioOutPutString( DocSvgSvgTail, sw->swXmlWriter.xwSos );

    return 0;
    }

void svgInitSvgWriter(	SvgWriter *	sw )
    {
    xmlInitXmlWriter( &(sw->swXmlWriter) );
    geoInitRectangle( &(sw->swViewBox) );

    sw->swWide= 0;
    sw->swHigh= 0;
    sw->swUnit[0]= '\0';
    sw->swMultiPage= 0;

    sw->swPatternCount= 0;
    }

/************************************************************************/

void svgWriteRectangleAttributes(	SvgWriter *			sw,
					const DocumentRectangle *	dr )
    {
    XmlWriter *	xw= &(sw->swXmlWriter);

    xmlWriteIntAttribute( xw, "x", dr->drX0 );
    xmlWriteIntAttribute( xw, "y", dr->drY0 );
    xmlWriteIntAttribute( xw, "width", dr->drX1- dr->drX0+ 1 );
    xmlWriteIntAttribute( xw, "height", dr->drY1- dr->drY0+ 1 );

    return;
    }

void svgWriteViewBoxAttribute(		SvgWriter *			sw,
					const char *			name,
					const DocumentRectangle *	dr )
    {
    XmlWriter *	xw= &(sw->swXmlWriter);
    char	scratch[100];

    sprintf( scratch, "%d %d %d %d", dr->drX0, dr->drY0,
					dr->drX1- dr->drX0+ 1,
					dr->drY1- dr->drY0+ 1 );

    xmlWriteStringAttribute( xw, name, scratch );

    return;
    }

/************************************************************************/

static const char PsSvgArrowMarker[]=

    "<marker id=\"Arrow\"\n"
    "    viewBox=\"0 0 10 10\" refX=\"0\" refY=\"5\"\n"
    "    markerUnits=\"strokeWidth\"\n"
    "    markerWidth=\"4\" markerHeight=\"3\"\n"
    "    orient=\"auto\">\n"
    "  <path d=\"M 0 0 L 10 5 L 0 10 z\" />\n"
    "</marker>\n";

static const char PsSvgStealthArrowMarker[]=

    "<marker id=\"StealthArrow\"\n"
    "    viewBox=\"0 0 10 10\" refX=\"0\" refY=\"5\"\n"
    "    markerUnits=\"strokeWidth\"\n"
    "    markerWidth=\"4\" markerHeight=\"3\"\n"
    "    orient=\"auto\">\n"
    "  <path d=\"M 0 0 L 10 5 L 0 10 z\" />\n"
    "</marker>\n";

static const char PsSvgDiamondMarker[]=

    "<marker id=\"Diamond\"\n"
    "    viewBox=\"0 0 10 10\" refX=\"0\" refY=\"5\"\n"
    "    markerUnits=\"strokeWidth\"\n"
    "    markerWidth=\"4\" markerHeight=\"3\"\n"
    "    orient=\"auto\">\n"
    "  <path d=\"M 0 0 L 10 5 L 0 10 z\" />\n"
    "</marker>\n";

static const char PsSvgOpenArrowMarker[]=

    "<marker id=\"OpenArrow\"\n"
    "    viewBox=\"0 0 10 10\" refX=\"0\" refY=\"5\"\n"
    "    markerUnits=\"strokeWidth\"\n"
    "    markerWidth=\"4\" markerHeight=\"3\"\n"
    "    orient=\"auto\">\n"
    "  <path d=\"M 0 0 L 10 5 L 0 10 z\" />\n"
    "</marker>\n";

static const char PsSvgOvalMarker[]=

    "<marker id=\"Oval\"\n"
    "    viewBox=\"0 0 10 10\" refX=\"0\" refY=\"5\"\n"
    "    markerUnits=\"strokeWidth\"\n"
    "    markerWidth=\"4\" markerHeight=\"3\"\n"
    "    orient=\"auto\">\n"
    "  <path d=\"M 0 0 L 10 5 L 0 10 z\" />\n"
    "</marker>\n";

/************************************************************************/

static const char PsSvgF_Head[]=
				" patternUnits=\"userSpaceOnUse\""
				" width=\"160\""
				" height=\"160\""
				" viewBox=\"0 0 8 8\""
				">";

static const char *	PsSvgF_Horiz[]=
{
    "<line x1=\"0\" y1=\"4\" x2=\"8\" y2=\"4\" stroke-width=\"1.5\" ",

    (const char *)0
};

static const char *	PsSvgF_Vert[]=
{
    "<line x1=\"4\" y1=\"0\" x2=\"4\" y2=\"8\" stroke-width=\"1.5\" ",

    (const char *)0
};

static const char *	PsSvgF_Fdiag[]=
{
    "<line x1=\"0\" y1=\"4\" x2=\"4\" y2=\"8\" stroke-width=\"1.5\" ",
    "<line x1=\"4\" y1=\"0\" x2=\"8\" y2=\"4\" stroke-width=\"1.5\" ",

    (const char *)0
};

static const char *	PsSvgF_Bdiag[]=
{
    "<line x1=\"0\" y1=\"4\" x2=\"4\" y2=\"0\" stroke-width=\"1.5\" ",
    "<line x1=\"4\" y1=\"8\" x2=\"8\" y2=\"4\" stroke-width=\"1.5\" ",

    (const char *)0
};

static const char *	PsSvgF_Cross[]=
{
    "<line x1=\"0\" y1=\"4\" x2=\"8\" y2=\"4\" stroke-width=\"1\" ",
    "<line x1=\"4\" y1=\"0\" x2=\"4\" y2=\"8\" stroke-width=\"1\" ",

    (const char *)0
};


static const char *	PsSvgF_Dcross[]=
{
    "<line x1=\"0\" y1=\"4\" x2=\"4\" y2=\"8\" stroke-width=\"1\" ",
    "<line x1=\"4\" y1=\"0\" x2=\"8\" y2=\"4\" stroke-width=\"1\" ",
    "<line x1=\"0\" y1=\"4\" x2=\"4\" y2=\"0\" stroke-width=\"1\" ",
    "<line x1=\"4\" y1=\"8\" x2=\"8\" y2=\"4\" stroke-width=\"1\" ",

    (const char *)0
};

static const char *	PsSvgF_DkHoriz[]=
{
    "<line x1=\"0\" y1=\"4\" x2=\"8\" y2=\"4\" stroke-width=\"2.5\" ",

    (const char *)0
};

static const char *	PsSvgF_DkVert[]=
{
    "<line x1=\"4\" y1=\"0\" x2=\"4\" y2=\"8\" stroke-width=\"2.5\" ",

    (const char *)0
};

static const char *	PsSvgF_DkFdiag[]=
{
    "<line x1=\"0\" y1=\"4\" x2=\"4\" y2=\"8\" stroke-width=\"2.5\" ",
    "<line x1=\"4\" y1=\"0\" x2=\"8\" y2=\"4\" stroke-width=\"2.5\" ",

    (const char *)0
};

static const char *	PsSvgF_DkBdiag[]=
{
    "<line x1=\"0\" y1=\"4\" x2=\"4\" y2=\"0\" stroke-width=\"2.5\" ",
    "<line x1=\"4\" y1=\"8\" x2=\"8\" y2=\"4\" stroke-width=\"2.5\" ",

    (const char *)0
};

static const char *	PsSvgF_DkCross[]=
{
    "<line x1=\"0\" y1=\"4\" x2=\"8\" y2=\"4\" stroke-width=\"2\" ",
    "<line x1=\"4\" y1=\"0\" x2=\"4\" y2=\"8\" stroke-width=\"2\" ",

    (const char *)0
};

static const char *	PsSvgF_DkDcross[]=
{
    "<line x1=\"0\" y1=\"4\" x2=\"4\" y2=\"8\" stroke-width=\"2\" ",
    "<line x1=\"4\" y1=\"0\" x2=\"8\" y2=\"4\" stroke-width=\"2\" ",
    "<line x1=\"0\" y1=\"4\" x2=\"4\" y2=\"0\" stroke-width=\"2\" ",
    "<line x1=\"4\" y1=\"8\" x2=\"8\" y2=\"4\" stroke-width=\"2\" ",

    (const char *)0
};

static const char **	PsSvgF_Patterns[]=
    {
    (const char **)0,
    PsSvgF_Horiz,
    PsSvgF_Vert,
    PsSvgF_Fdiag,
    PsSvgF_Bdiag,
    PsSvgF_Cross,
    PsSvgF_Dcross,
    PsSvgF_DkHoriz,
    PsSvgF_DkVert,
    PsSvgF_DkFdiag,
    PsSvgF_DkBdiag,
    PsSvgF_DkCross,
    PsSvgF_DkDcross,
    };

int svgDefineShading(		SvgWriter *		sw,
				int			pattern,
				int			shading,
				const RGB8Color *	rgb8 )
    {
    XmlWriter *	xw= &(sw->swXmlWriter);

    const char **	def= PsSvgF_Patterns[shading];
    int			i= 0;

    sioOutPrintf( xw->xwSos, "<pattern id=\"pat%d\" ", pattern );
    sioOutPutString( PsSvgF_Head, xw->xwSos );

    while( def[i] )
	{
	sioOutPrintf( xw->xwSos, "%s", def[i] );
	xmlWriteRgb8Attribute( xw, "stroke", rgb8 );
	sioOutPrintf( xw->xwSos, "/>" );
	xmlNewLine( xw );

	i++;
	}

    sioOutPrintf( xw->xwSos, "</pattern>" );
    xmlNewLine( xw );

    return 0;
    }
