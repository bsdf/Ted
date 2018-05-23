/************************************************************************/
/*									*/
/*  Print SVG : draw one line of text.					*/
/*									*/
/************************************************************************/

#   include	"docHtmlConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>

#   include	<sioGeneral.h>

#   include	<psFontMetrics.h>
#   include	"docSvgDrawImpl.h"
#   include	<docDraw.h>
#   include	<docLayout.h>
#   include	<docTabStop.h>
#   include	<docTextLine.h>
#   include	<docTreeNode.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Print a series of particules with the same attributes.		*/
/*									*/
/************************************************************************/

int docSvgDrawSpan(		const DrawTextLine *	dtl,
				int			part,
				int			count,
				const LayoutPosition *	baseline,
				int			textAttrNr,
				const TextAttribute *	ta,
				const char *		printString,
				int			nbLen )
    {
    SvgWriter *			sw= (SvgWriter *)dtl->dtlThrough;
    DrawingContext *		dc= dtl->dtlDrawingContext;
    XmlWriter *			xw= &(sw->swXmlWriter);
    int				fontSizeTwips;
    int				textSizeTwips;

    int				spanBaseline= baseline->lpPageYTwips;

    const ParticuleData *	pd= dtl->dtlParticuleData+ part;

    fontSizeTwips= 10* ta->taFontSizeHalfPoints;
    textSizeTwips= fontSizeTwips;

    if  ( nbLen > 0 )
	{
	int			y;
	const DocumentFont *	df;

	docDrawSetFont( dc, (void *)sw, textAttrNr, ta );
	docDrawSetColorNumber( dc, (void *)sw,
				    ta->taTextColorNumber );

	df= docFontListGetFontByNumber(
		    dc->dcLayoutContext.lcDocument->bdProperties.dpFontList,
		    ta->taFontNumber );

	y= spanBaseline;

	if  ( ta->taSuperSub == TEXTvaSUPERSCRIPT )
	    {
	    psGetSuperBaseline( &y, spanBaseline, fontSizeTwips, pd->pdAfi );
	    textSizeTwips= SUPERSUB_SIZE( fontSizeTwips );
	    }

	if  ( ta->taSuperSub == TEXTvaSUBSCRIPT )
	    {
	    psGetSubBaseline( &y, spanBaseline, fontSizeTwips, pd->pdAfi );
	    textSizeTwips= SUPERSUB_SIZE( fontSizeTwips );
	    }

	xmlPutString( "<text ", xw );

#	if 1
	xmlWriteStringAttribute( xw, "font-family", df->dfName );
	xmlWriteIntAttribute( xw, "font-size", textSizeTwips );
	xmlWriteRgb8Attribute( xw, "fill", &(dc->dcCurrentColor) );
	if  ( ta->taFontIsSlanted )
	    { xmlWriteStringAttribute( xw, "font-style", "italic" );	}
	if  ( ta->taFontIsBold )
	    { xmlWriteStringAttribute( xw, "font-weight", "bold" );	}

	if  ( ta->taTextIsUnderlined )
	    {
	    /* Does not work with Firefox */
	    xmlWriteRgb8Attribute( xw, "stroke", &(dc->dcCurrentColor) );
	    xmlWriteStringAttribute( xw, "text-decoration", "underline" );
	    }
#	else
	{
	char	scratch[25];

	sprintf( scratch, "t%d", textAttrNr );
	xmlWriteStringAttribute( xw, "class", scratch );
	xmlWriteIntAttribute( xw, "font-size", textSizeTwips );
	}
#	endif

	xmlWriteIntAttribute( xw, "x",
			    pd->pdX0+ pd->pdLeftBorderWidth+ dtl->dtlXShift );
	xmlWriteIntAttribute( xw, "y", y+ dtl->dtlYShift );

	xmlPutString( ">", xw );
	xmlNewLine( xw );

	xmlEscapeCharacters( xw, printString, nbLen );
	xmlPutString( "</text>", xw );
	xmlNewLine( xw );
	}

    return 0;
    }

int docSvgDrawFtnsep(		const DrawTextLine *	dtl,
				int			part,
				int			textAttrNr,
				const TextAttribute *	ta,
				int			x0Twips,
				int			x1Twips,
				const LayoutPosition *	baseLine )
    {
    SvgWriter *			sw= (SvgWriter *)dtl->dtlThrough;
    DrawingContext *		dc= dtl->dtlDrawingContext;
    XmlWriter *			xw= &(sw->swXmlWriter);
    int				fontSizeTwips;

    int				xHeight;

    int				y0;
    int				h;

    fontSizeTwips= 10* ta->taFontSizeHalfPoints;

    xHeight= ( fontSizeTwips+ 1 )/ 2;

    y0= baseLine->lpPageYTwips- xHeight/2- 15/2;
    h= 15;

    docDrawSetColorNumber( dc, (void *)sw, ta->taTextColorNumber );

    xmlPutString( "<line ", xw );
    xmlWriteIntAttribute( xw, "x1", x0Twips );
    xmlWriteIntAttribute( xw, "y1", y0 );
    xmlWriteIntAttribute( xw, "x2", x1Twips );
    xmlWriteIntAttribute( xw, "y2", y0 );
    xmlWriteIntAttribute( xw, "stroke-width", h );
    xmlWriteRgb8Attribute( xw, "stroke", &(dc->dcCurrentColor) );
    xmlPutString( "/>", xw );
    xmlNewLine( xw );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Draw tabs.								*/
/*									*/
/************************************************************************/

static int svgDrawTab(		DrawingContext *	dc,
				const DrawTextLine *	dtl,
				XmlWriter *		xw,
				const TextAttribute *	ta,
				int			x0,
				int			x1,
				int			baseLine,
				int			step,
				const char *		tabProc )
    {
    x0= step* ( ( x0+ step- 1 )/ step );
    if  ( x1 <= x0 )
	{ return 0;	}

LDEB(1);
    return 0;
    }

int docSvgDrawTab(	const DrawTextLine *		dtl,
			int				part,
			int				textAttrNr,
			const TextAttribute *		ta,
			int				leader,
			int				x0Twips,
			int				x1Twips,
			const LayoutPosition *		baseLine )
    {
    SvgWriter *			sw= (SvgWriter *)dtl->dtlThrough;
    DrawingContext *		dc= dtl->dtlDrawingContext;
    XmlWriter *			xw= &(sw->swXmlWriter);

    const TextLine *		tl= dtl->dtlTextLine;
    int				lineHeight= tl->tlDescY1- tl->tlAscY0;

    int				x0= x0Twips+ lineHeight/ 4;
    int				x1= x1Twips- lineHeight/2;

    int				spanBaseline= baseLine->lpPageYTwips;

    switch( leader )
	{
	case DOCtlNONE:
	    break;

	case DOCtlDOTS:

	    if  ( ta->taFontIsBold )
		{
		svgDrawTab( dc, dtl, xw, ta, x0, x1, spanBaseline,
							60, "dot-tab-bold" );
		}
	    else{
		svgDrawTab( dc, dtl, xw, ta, x0, x1, spanBaseline,
							60, "dot-tab" );
		}

	    break;

	case DOCtlUNDERLINE:

	    if  ( ta->taFontIsBold )
		{
		svgDrawTab( dc, dtl, xw, ta, x0, x1, spanBaseline,
							20, "ul-tab-bold" );
		}
	    else{
		svgDrawTab( dc, dtl, xw, ta, x0, x1, spanBaseline,
							20, "ul-tab" );
		}

	    break;

	case DOCtlHYPH:

	    if  ( ta->taFontIsBold )
		{
		svgDrawTab( dc, dtl, xw, ta, x0, x1, spanBaseline,
							140, "dash-tab-bold" );
		}
	    else{
		svgDrawTab( dc, dtl, xw, ta, x0, x1, spanBaseline,
							140, "dash-tab" );
		}

	    break;

	case DOCtlTHICK:
	    LDEB(leader);
	    break;

	case DOCtlEQUAL:
	    LDEB(leader);
	    break;

	default:
	    LDEB(leader);
	    break;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Layout and print successive lines of a paragraph.			*/
/*									*/
/************************************************************************/

int docSvgDrawTextLine(		BufferItem *			paraBi,
				int				line,
				const ParagraphFrame *		pf,
				const DocumentRectangle *	drLine,
				void *				vsw,
				DrawingContext *		dc,
				const BlockOrigin *		bo )
    {
    const LayoutContext *	lc= &(dc->dcLayoutContext);

    const TextLine *		tl= paraBi->biParaLines+ line;
    int				part= tl->tlFirstParticule;

    int				done= 0;
    int				accepted;
    TextLine			boxLine;

    ParticuleData *		pd;

    int				xTwips;
    DrawTextLine		dtl;

    docInitDrawTextLine( &dtl );

    if  ( docPsClaimParticuleData( paraBi->biParaParticuleCount, &pd ) )
	{ LDEB(paraBi->biParaParticuleCount); return -1;	}

    boxLine= *tl;
    accepted= docLayoutLineBox( &boxLine, paraBi, part, lc, pd+ part, pf );

    if  ( accepted < 1 )
	{ LDEB(accepted); return -1;	}

    docSetDrawTextLine( &dtl, vsw, dc, tl, paraBi, bo, pf, drLine );
    dtl.dtlParticuleData= pd;
    dtl.dtlDrawParticulesSeparately= paraBi->biParaAlignment == DOCthaJUSTIFIED;

    done= 0;
    xTwips= pf->pfParaContentRect.drX0+ tl->tlLineIndent;
    while( done < dtl.dtlTextLine->tlParticuleCount )
	{
	int		drawn;

	drawn= docDrawLineParticules( &dtl, &xTwips, part );
	if  ( drawn < 1 )
	    { LDEB(drawn); return -1;	}

	done += drawn; part += drawn;
	}

    return accepted;
    }

