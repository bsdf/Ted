/************************************************************************/
/*									*/
/*  Ted, Screen drawing and forcing drawing through			*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>
#   include	<stdio.h>

#   include	"tedApp.h"
#   include	"tedDraw.h"
#   include	"tedLayout.h"
#   include	<docTabStop.h>
#   include	<drawScreenFontUtil.h>
#   include	<geoGrid.h>
#   include	<docParaParticules.h>
#   include	<docNodeTree.h>

#   include	<appDebugon.h>

static void tedDrawParticuleLine(	DrawingContext *	dc,
					ScreenDrawingData *	sdd,
					const BufferItem *	paraNode,
					int			head,
					int			past,
					int			textColorNr,
					int			x0,
					int			y0,
					int			h )
    {
    const LayoutContext *	lc= &(dc->dcLayoutContext);
    const TextParticule *	tp= paraNode->biParaParticules;
    /*
    DocumentPosition		dpHead;
    PositionGeometry		pgHead;
    */
    DocumentPosition		dpTail;
    PositionGeometry		pgTail;

    DocumentRectangle		drFill;

    docSetDocumentPosition( &dpTail, (BufferItem *)paraNode,
				tp[past- 1].tpStroff+ tp[past- 1].tpStrlen );
    tedPositionGeometry( &pgTail, &dpTail, PARAfindFIRST, lc );

    drFill.drX0= x0;
    drFill.drX1= pgTail.pgXPixels;
    drFill.drY0= y0;
    drFill.drY1= y0+ h- 1;

    if  ( drFill.drX1 > drFill.drX0 )
	{
	docDrawSetColorNumber( dc, (void *)sdd, textColorNr );

	geoShiftRectangle( &drFill, -lc->lcOx, -lc->lcOy );
	drawFillRectangle( lc->lcDrawingSurface, &drFill );
	}

    return;
    }

int tedDrawRunUnderline(		const DrawTextLine *	dtl,
					int			part,
					int			upto,
					int			textAttrNr,
					const TextAttribute *	ta,
					int			x0Twips,
					int			x1Twips,
					const LayoutPosition *	baseLine )
    {
    ScreenDrawingData *		sdd= (ScreenDrawingData *)dtl->dtlThrough;
    DrawingContext *		dc= dtl->dtlDrawingContext;
    const LayoutContext *	lc= &(dc->dcLayoutContext);

    const BufferItem *		paraNode= dtl->dtlParaNode;
    const TextParticule *	tp= paraNode->biParaParticules;

    int				baselinePixels;
    int				x0FrameShifted;
    int				screenFont;
    int				x0;
    int				y0;
    int				h;

    baselinePixels= docLayoutYPixels( lc, baseLine );
    x0FrameShifted= docLayoutXPixels( lc,
	    dtl->dtlParagraphFrame->pfParaContentRect.drX0+ dtl->dtlXShift );

    screenFont= utilIndexMappingGet( lc->lcAttributeToScreenFont, textAttrNr );

    x0= x0FrameShifted+ tp[part].tpXContentXPixels;
    drawFontUnderlineGeometry( &h, &y0,
			    lc->lcDrawingSurface, screenFont, baselinePixels );

    tedDrawParticuleLine( dc, sdd, paraNode, part, upto,
					ta->taTextColorNumber, x0, y0, h );

    return 0;
    }

int tedDrawRunStrikethrough(	const DrawTextLine *	dtl,
					int			part,
					int			upto,
					int			textAttrNr,
					const TextAttribute *	ta,
					int			x0Twips,
					int			x1Twips,
					const LayoutPosition *	baseLine )
    {
    ScreenDrawingData *		sdd= (ScreenDrawingData *)dtl->dtlThrough;
    DrawingContext *		dc= dtl->dtlDrawingContext;
    const LayoutContext *	lc= &(dc->dcLayoutContext);

    const BufferItem *		paraNode= dtl->dtlParaNode;
    const TextParticule *	tp= paraNode->biParaParticules;

    int				baselinePixels;
    int				x0FrameShifted;
    int				screenFont;
    int				x0;
    int				y0;
    int				h;

    baselinePixels= docLayoutYPixels( lc, baseLine );
    x0FrameShifted= docLayoutXPixels( lc,
	    dtl->dtlParagraphFrame->pfParaContentRect.drX0+ dtl->dtlXShift );

    screenFont= utilIndexMappingGet( lc->lcAttributeToScreenFont, textAttrNr );

    x0= x0FrameShifted+ tp[part].tpXContentXPixels;
    drawFontStrikethroughGeometry( &h, &y0,
			    lc->lcDrawingSurface, screenFont, baselinePixels );

    tedDrawParticuleLine( dc, sdd, paraNode, part, upto,
					ta->taTextColorNumber, x0, y0, h );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Draw a tab.								*/
/*									*/
/*  1)  Tabs need not to be drawn.					*/
/*									*/
/************************************************************************/

int tedDrawTab(		const DrawTextLine *		dtl,
			int				part,
			int				textAttrNr,
			const TextAttribute *		ta,
			int				leader,
			int				x0Twips,
			int				x1Twips,
			const LayoutPosition *		baseLine )
    {
    ScreenDrawingData *		sdd= (ScreenDrawingData *)dtl->dtlThrough;
    DrawingContext *		dc= dtl->dtlDrawingContext;
    const LayoutContext *	lc= &(dc->dcLayoutContext);
    double			xfac= lc->lcPixelsPerTwip;
    const BufferItem *		paraNode= dtl->dtlParaNode;
    const TextParticule *	tp= paraNode->biParaParticules+ part;

    int				screenFont;

    int				pixelsWide;
    int				pixelSize;
    int				baselinePixels;
    int				x0FrameShifted;

    DocumentRectangle		drSpan= dtl->dtlLineRectangle;

    baselinePixels= docLayoutYPixels( lc, baseLine );
    x0FrameShifted= docLayoutXPixels( lc,
	    dtl->dtlParagraphFrame->pfParaContentRect.drX0+ dtl->dtlXShift );

    pixelsWide= COORDtoGRID( xfac, tp->tpTwipsWide );
    drSpan.drX0= x0FrameShifted+ tp->tpXContentXPixels;
    drSpan.drX1= x0FrameShifted+ tp->tpXContentXPixels+ pixelsWide- 1;

    if  ( dc->dcClipRect						&&
	  ! geoIntersectRectangle( (DocumentRectangle *)0,
					    dc->dcClipRect, &drSpan )	)
	{ return 0;	}

    screenFont= utilIndexMappingGet( lc->lcAttributeToScreenFont, textAttrNr );

    pixelSize= COORDtoGRID( xfac, 2* ta->taFontSizeHalfPoints );

    switch( leader )
	{
	case DOCtlNONE:
	    break;

	case DOCtlDOTS:

	    if  ( screenFont < 0 )
		{ LLDEB(textAttrNr,screenFont); return -1;	}

	    drSpan.drX0 += pixelSize/ 4;
	    drSpan.drX1 -= pixelSize/ 2;
	    drSpan.drX0= 3* ( ( drSpan.drX0+ 2 )/ 3 );

	    if  ( drSpan.drX1 <= drSpan.drX0 )
		{ return 0;	}

	    if  ( ta->taFontIsBold )
		{
		static const unsigned char	boldDot[]= { 2, 1 };

		drawSetLineAttributes( lc->lcDrawingSurface,
			2, LineStyleSingleDash, LineCapButt, LineJoinMiter,
			boldDot, sizeof( boldDot ) );
		}
	    else{
		static const unsigned char	dot[]= { 1, 2 };

		drawSetLineAttributes( lc->lcDrawingSurface,
			1, LineStyleSingleDash, LineCapButt, LineJoinMiter,
			dot, sizeof( dot ) );
		}

	    docDrawSetColorNumber( dc, sdd, ta->taTextColorNumber );

	    drawLine( lc->lcDrawingSurface,
			    drSpan.drX0- lc->lcOx, baselinePixels- lc->lcOy,
			    drSpan.drX1- lc->lcOx, baselinePixels- lc->lcOy );

	    break;

	case DOCtlUNDERLINE:
	    if  ( screenFont < 0 )
		{ LLDEB(textAttrNr,screenFont); return -1;	}

	    drSpan.drX0 += pixelSize/ 4;
	    drSpan.drX1 -= pixelSize/ 2;
	    if  ( drSpan.drX1 <= drSpan.drX0 )
		{ return 0;	}

	    if  ( ta->taFontIsBold )
		{
		drawSetLineAttributes( lc->lcDrawingSurface,
				2, LineStyleSolid, LineCapButt, LineJoinMiter,
				(const unsigned char *)0, 0 );
		}
	    else{
		drawSetLineAttributes( lc->lcDrawingSurface,
				1, LineStyleSolid, LineCapButt, LineJoinMiter,
				(const unsigned char *)0, 0 );
		}

	    docDrawSetColorNumber( dc, sdd, ta->taTextColorNumber );

	    drawLine( lc->lcDrawingSurface,
			    drSpan.drX0- lc->lcOx, baselinePixels- lc->lcOy,
			    drSpan.drX1- lc->lcOx, baselinePixels- lc->lcOy );

	    break;

	case DOCtlHYPH:
	    if  ( screenFont < 0 )
		{ LLDEB(textAttrNr,screenFont); return -1;	}

	    drSpan.drX0 += pixelSize/ 4;
	    drSpan.drX1 -= pixelSize/ 2;
	    drSpan.drX0= 7* ( ( drSpan.drX0+ 6 )/ 7 );

	    if  ( drSpan.drX1 <= drSpan.drX0 )
		{ return 0;	}

	    if  ( ta->taFontIsBold )
		{
		static const unsigned char	boldDash[]= { 4, 3 };

		drawSetLineAttributes( lc->lcDrawingSurface,
			2, LineStyleSingleDash, LineCapButt, LineJoinMiter,
			boldDash, sizeof( boldDash ) );
		}
	    else{
		static const unsigned char	dash[]= { 3, 4 };

		drawSetLineAttributes( lc->lcDrawingSurface,
			1, LineStyleSingleDash, LineCapButt, LineJoinMiter,
			dash, sizeof( dash ) );
		}

	    docDrawSetColorNumber( dc, sdd, ta->taTextColorNumber );

	    drawLine( lc->lcDrawingSurface,
			    drSpan.drX0- lc->lcOx, baselinePixels- lc->lcOy,
			    drSpan.drX1- lc->lcOx, baselinePixels- lc->lcOy );

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
/*  Draw a footnote separator.						*/
/*									*/
/************************************************************************/

int tedDrawChftnsep(	const DrawTextLine *		dtl,
			int				part,
			int				textAttrNr,
			const TextAttribute *		ta,
			int				x0Twips,
			int				x1Twips,
			const LayoutPosition *		baseLine )
    {
    ScreenDrawingData *		sdd= (ScreenDrawingData *)dtl->dtlThrough;
    DrawingContext *		dc= dtl->dtlDrawingContext;
    const LayoutContext *	lc= &(dc->dcLayoutContext);
    double			xfac= lc->lcPixelsPerTwip;

    const BufferItem *		paraNode= dtl->dtlParaNode;
    const TextParticule *	tp= paraNode->biParaParticules+ part;

    int				y0;
    int				h;
    int				baselinePixels;
    int				pixelsWide;

    int				screenFont;
    int				x0FrameShifted;
    DocumentRectangle		drSpan= dtl->dtlLineRectangle;

    baselinePixels= docLayoutYPixels( lc, baseLine );
    x0FrameShifted= docLayoutXPixels( lc,
	    dtl->dtlParagraphFrame->pfParaContentRect.drX0+ dtl->dtlXShift );

    pixelsWide= COORDtoGRID( xfac, tp->tpTwipsWide );
    drSpan.drX0= x0FrameShifted+ tp->tpXContentXPixels;
    drSpan.drX1= x0FrameShifted+ tp->tpXContentXPixels+ pixelsWide- 1;

    if  ( dc->dcClipRect						&&
	  ! geoIntersectRectangle( (DocumentRectangle *)0,
					    dc->dcClipRect, &drSpan )	)
	{ return 0;	}

    drawSetLineAttributes( lc->lcDrawingSurface,
			    1, LineStyleSolid, LineCapButt, LineJoinMiter,
			    (const unsigned char *)0, 0 );

    screenFont= utilIndexMappingGet( lc->lcAttributeToScreenFont, textAttrNr );
    if  ( screenFont < 0 )
	{ LLDEB(textAttrNr,screenFont); return 0;	}

    drawFontStrikethroughGeometry( &h, &y0, lc->lcDrawingSurface,
						screenFont, baselinePixels );

    pixelsWide= COORDtoGRID( xfac, tp->tpTwipsWide );

    drSpan.drY0= y0;
    drSpan.drY1= y0+ h- 1;

    docDrawSetColorNumber( dc, (void *)sdd, ta->taTextColorNumber );

    geoShiftRectangle( &drSpan, -lc->lcOx, -lc->lcOy );
    drawFillRectangle( lc->lcDrawingSurface, &drSpan );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Draw the individual segments of a smallcaps string.			*/
/*									*/
/************************************************************************/

static void tedDrawSegments(	DrawingContext *	dc,
				int			x,
				int			y,
				int			fullScreenFont,
				int			scapsScreenFont,
				const char *		s,
				const int *		segments,
				int			segmentCount )
    {
    const LayoutContext *	lc= &(dc->dcLayoutContext);
    int				seg;

    DocumentRectangle		drText;

    for ( seg= 0; seg < segmentCount; seg++ )
	{
	if  ( segments[2* seg+ 0] > 0 )
	    {
	    drawString( lc->lcDrawingSurface,
					x, y, fullScreenFont,
					s, segments[2* seg+ 0] );

	    drawGetTextExtents( &drText, lc->lcDrawingSurface,
					x, y, fullScreenFont,
					s, segments[2* seg+ 0] );

	    s += segments[2* seg+ 0];
	    x= drText.drX1+ 1;
	    }

	if  ( segments[2* seg+ 1] > 0 )
	    {
	    drawString( lc->lcDrawingSurface,
					x, y, scapsScreenFont,
					s, segments[2* seg+ 1] );

	    drawGetTextExtents( &drText, lc->lcDrawingSurface,
					x, y, scapsScreenFont,
					s, segments[2* seg+ 1] );

	    s += segments[2* seg+ 1];
	    x= drText.drX1+ 1;
	    }
	}

    return;
    }

/************************************************************************/

int tedDrawSpan(	const DrawTextLine *		dtl,
			int				part,
			int				count,
			const LayoutPosition *		baseLine,
			int				textAttrNr,
			const TextAttribute *		ta,
			const char *			printString,
			int				nbLen )
    {
    int				rval= 0;

    ScreenDrawingData *		sdd= (ScreenDrawingData *)dtl->dtlThrough;
    DrawingContext *		dc= dtl->dtlDrawingContext;
    const LayoutContext *	lc= &(dc->dcLayoutContext);

    char *			upperString= (char *)0;
    int *			segments= (int *)0;
    int				segmentCount= 0;

    int				baselinePixels;
    int				fullScreenFont;
    int				y;

    int				x0FrameShifted;

    const BufferItem *		paraNode= dtl->dtlParaNode;
    const TextLine *		tl= dtl->dtlTextLine;
    const TextParticule *	tp= paraNode->biParaParticules+ part;

    int				spanX0;

    baselinePixels= docLayoutYPixels( lc, baseLine );
    x0FrameShifted= docLayoutXPixels( lc,
	    dtl->dtlParagraphFrame->pfParaContentRect.drX0+ dtl->dtlXShift );
    spanX0= x0FrameShifted+ tp->tpXContentXPixels;

    if  ( dc->dcClipRect						&&
	  part+ count < tl->tlFirstParticule+ tl->tlParticuleCount	)
	{
	const TextParticule *	tpp= paraNode->biParaParticules+ part+ count;
	DocumentRectangle	drSpan= dtl->dtlLineRectangle;

	drSpan.drX0= spanX0;
	drSpan.drX1= x0FrameShifted+ tpp->tpXContentXPixels;

	if  ( ! geoIntersectRectangle( (DocumentRectangle *)0,
					    dc->dcClipRect, &drSpan )	)
	    { return 0;	}
	}

    fullScreenFont= utilIndexMappingGet( lc->lcAttributeToScreenFont,
						    textAttrNr );
    if  ( fullScreenFont < 0 )
	{ LLDEB(textAttrNr,fullScreenFont); rval= -1; goto ready;	}

    if  ( ta->taSmallCaps || ta->taCapitals )
	{
	if  ( docMakeCapsString( &upperString, &segments, &segmentCount,
						    ta, printString, nbLen ) )
	    { LDEB(nbLen); rval= -1; goto ready;	}

	printString= upperString;
	}

    switch( ta->taSuperSub )
	{
	case TEXTvaSUPERSCRIPT:
	    drawFontGetSuperBaseline( &y, baselinePixels,
				    lc->lcDrawingSurface, fullScreenFont );
	    break;

	case TEXTvaSUBSCRIPT:
	    drawFontGetSubBaseline( &y, baselinePixels,
				    lc->lcDrawingSurface, fullScreenFont );
	    break;

	default:
	    LDEB(ta->taSuperSub);
	case TEXTvaREGULAR:
	    y= baselinePixels;
	    break;
	}

    docDrawSetFont( dc, (void *)sdd, textAttrNr, ta );
    docDrawSetColorNumber( dc, (void *)sdd, ta->taTextColorNumber );

    if  ( ta->taSmallCaps && ! ta->taCapitals )
	{
	int	scapsScreenFont;

	scapsScreenFont= docLayoutScapsScreenFont( lc, ta );
	if  ( scapsScreenFont < 0 )
	    { LDEB(scapsScreenFont); rval= -1; goto ready;	}

	tedDrawSegments( dc, spanX0- lc->lcOx, y- lc->lcOy,
					fullScreenFont, scapsScreenFont,
					printString, segments, segmentCount );
	}
    else{
	drawString( lc->lcDrawingSurface,
				spanX0- lc->lcOx, y- lc->lcOy,
					fullScreenFont, printString, nbLen );
	}

    /*
    appDebug( "[%3d,%3d] -- %.*s\n", y, x0, len, printString );
    */

  ready:

    if  ( upperString )
	{ free( upperString );	}
    if  ( segments )
	{ free( segments );	}

    return rval;
    }

int tedDrawTextLine(		BufferItem *			paraNode,
				int				line,
				const ParagraphFrame *		pf,
				const DocumentRectangle *	drLine,
				void *				vsdd,
				DrawingContext *		dc,
				const BlockOrigin *		bo )
    {
    const TextLine *		tl= paraNode->biParaLines+ line;
    int				part= tl->tlFirstParticule;
    int				done;

    int				xTwips;
    DrawTextLine		dtl;

    docInitDrawTextLine( &dtl );

    if  ( dc->dcClipRect						&&
	  ! geoIntersectRectangle( (DocumentRectangle *)0,
					    dc->dcClipRect, drLine )	)
	{ return tl->tlParticuleCount;	}

    docSetDrawTextLine( &dtl, vsdd, dc, tl, paraNode, bo, pf, drLine );
    dtl.dtlDrawParticulesSeparately= 1;

    done= 0;
    xTwips= pf->pfParaContentRect.drX0+ tl->tlLineIndent;
    while( done < tl->tlParticuleCount )
	{
	int		drawn;

	drawn= docDrawLineParticules( &dtl, &xTwips, part );
	if  ( drawn < 1 )
	    { LDEB(drawn); return -1;	}

	done += drawn; part += drawn;
	}

    return done;
    }

/************************************************************************/
/*									*/
/*  Draw a rectangle to show that (part of) a line is selected.		*/
/*									*/
/*  1)  Delimit the line and calculate a preliminary rectangle for the	*/
/*	line.								*/
/*  2)  Table rectangle selections look nicer when we select the full	*/
/*	width of the table cell.					*/
/*  3)  Where to we start the selected rectangle? (left)		*/
/*  4)  Where to we end the selected rectangle? (right)			*/
/*  5)  Do the line and the selection overlap?				*/
/*	We do not have to consider I-Bar selections, so that is the	*/
/*	case if the selection begins before the end of the line and	*/
/*	ends after the begin of the line.				*/
/*	Include the line of an empty paragraph at either end of the	*/
/*	selection.							*/
/*  6)  Intersect with clipping rectangle.				*/
/*									*/
/************************************************************************/

static int tedLineRectangle(	DocumentRectangle *		drRedraw,
				int *				pInSelection,
				BufferItem *			paraNode,
				DrawingContext *		dc,
				ScreenDrawingData *		sdd,
				int				line,
				const DocumentRectangle *	drParaContent )
    {
    const LayoutContext *	lc= &(dc->dcLayoutContext);
    const DocumentSelection *	ds= dc->dcSelection;
    const SelectionGeometry *	sg= dc->dcSelectionGeometry;

    DocumentSelection	dsLine;
    SelectionGeometry	sgLine;

    const int		lastLine= 0;
    DocumentRectangle	drLine;

    int			tableRectangle;
    int			cmp_ShLh;
    int			cmp_StLh;
    int			cmp_ShLt;
    int			cmp_StLt;
    int			is_StPh;
    int			is_ShPt;

    int			partLineBegin;
    int			partLineEnd;

    int			inSelection= 0;

    if  ( ! docSelectionSameRoot( ds, paraNode ) )
	{ return 0;	}

    /*  1  */
    docLineSelection( &dsLine, &partLineBegin, &partLineEnd, paraNode, line );
    tedSelectionGeometry( &sgLine, &dsLine,
		docGetBodySectNode( paraNode, lc->lcDocument ), lastLine, lc );

    if  ( sgLine.sgHead.pgLine != line )
	{ LLDEB(sgLine.sgHead.pgLine,line);	}
    if  ( sgLine.sgTail.pgLine != line )
	{ LLDEB(sgLine.sgTail.pgLine,line);	}

    drLine= sgLine.sgRectangle;

    /*  2  */
    tableRectangle= 0;
    if  ( ds->dsCol0 >= 0						&&
	  ds->dsCol1 >= 0						&&
	  ! docPositionsInsideCell( &(ds->dsHead), &(ds->dsTail) )	)
	{ tableRectangle= 1; }

    cmp_ShLh= docComparePositions( &(ds->dsHead), &(dsLine.dsHead) );
    cmp_StLh= docComparePositions( &(ds->dsTail), &(dsLine.dsHead) );
    cmp_ShLt= docComparePositions( &(ds->dsHead), &(dsLine.dsTail) );
    cmp_StLt= docComparePositions( &(ds->dsTail), &(dsLine.dsTail) );

    is_StPh= ( sg->sgTail.pgPositionFlags & POSflagPARA_HEAD ) != 0;
    is_ShPt= ( sg->sgHead.pgPositionFlags & POSflagPARA_TAIL ) != 0;

    /*  3  */
    if  ( cmp_ShLh <= 0 || ( cmp_ShLh == 0 && tableRectangle ) )
	{
	int	x0Twips= drParaContent->drX0;

	if  ( paraNode->biParaFirstIndentTwips < 0 )
	    { x0Twips += paraNode->biParaFirstIndentTwips;	}

	drLine.drX0= docLayoutXPixels( lc, x0Twips );

	if  ( cmp_ShLh == 0 )
	    {
	    x0Twips= drParaContent->drX0;

	    if  ( line == 0 )
		{ x0Twips += paraNode->biParaFirstIndentTwips;	}

	    drLine.drX0= docLayoutXPixels( lc, x0Twips );
	    }
	}
    else{ drLine.drX0= sg->sgHead.pgXPixels;	}

    /*  4  */
    if  ( cmp_StLt > 0 || ( cmp_StLt == 0 && tableRectangle ) )
	{ drLine.drX1= docLayoutXPixels( lc, drParaContent->drX1 );	}
    else{ drLine.drX1= sg->sgTail.pgXPixels;				}

    /*  5  */
    inSelection= 0;

    if  ( docParaStrlen( ds->dsTail.dpNode ) == 0	&&
	  cmp_StLh == 0					)
	{ cmp_StLh=  1;	}
    if  ( docParaStrlen( ds->dsHead.dpNode ) == 0	&&
	  cmp_ShLt == 0					)
	{ cmp_ShLt= -1;	}

    if  ( ( cmp_ShLt <  0 && cmp_StLh > 0 )		|| /* Overlap	   */
	  ( cmp_ShLt <= 0 && cmp_StLt > 0 && is_ShPt )	|| /* head at tail */
	  ( cmp_StLh <= 0 && cmp_ShLh < 0 && is_StPh )	)  /* tail at head */
	{ inSelection= 1;	}

    /*  Highlight tail of selection at line head  */
    if  ( ( cmp_StLh == 0 && cmp_ShLh < 0 )	)	/* tail at head	*/
	{ drLine.drX0 -= 3;	}

    /*  6  */
    if  ( ! dc->dcClipRect						||
	  geoIntersectRectangle( drRedraw, dc->dcClipRect, &drLine )	)
	{ *pInSelection= inSelection; return 1;	}
    else{ return 0;	}
    }

int tedDrawTextReverse(		BufferItem *			paraNode,
				int				line,
				const ParagraphFrame *		pf,
				const DocumentRectangle *	drLine,
				void *				vsdd,
				DrawingContext *		dc,
				const BlockOrigin *		bo )
    {
    const LayoutContext *	lc= &(dc->dcLayoutContext);
    ScreenDrawingData *		sdd= (ScreenDrawingData *)vsdd;

    DocumentRectangle		drRedraw;
    int				done;
    int				inSelection;

    if  ( tedLineRectangle( &drRedraw, &inSelection, paraNode, dc, sdd, line,
						    &(pf->pfParaContentRect) ) )
	{
	geoShiftRectangle( &drRedraw, -lc->lcOx, -lc->lcOy );
	drawSetClipRect( lc->lcDrawingSurface, &drRedraw );

	done= tedDrawTextLine( paraNode, line, pf, drLine, vsdd, dc, bo );

	tedOriginalClipping( dc, sdd );
	}
    else{
	TextLine *	tl= paraNode->biParaLines+ line;

	done= tl->tlParticuleCount;
	}

    return done;
    }

int tedDrawTextSelected(	BufferItem *			paraNode,
				int				line,
				const ParagraphFrame *		pf,
				const DocumentRectangle *	drLine,
				void *				vsdd,
				DrawingContext *		dc,
				const BlockOrigin *		bo )
    {
    const LayoutContext *	lc= &(dc->dcLayoutContext);
    ScreenDrawingData *		sdd= (ScreenDrawingData *)vsdd;

    DocumentRectangle		drRedraw;
    int				done;
    int				inSelection;

    if  ( tedLineRectangle( &drRedraw, &inSelection, paraNode, dc, sdd, line,
						    &(pf->pfParaContentRect) ) )
	{
	geoShiftRectangle( &drRedraw, -lc->lcOx, -lc->lcOy );
	drawSetClipRect( lc->lcDrawingSurface, &drRedraw );

	if  ( inSelection )
	    {
	    docDrawSetColorRgb( dc, (void *)sdd, &(sdd->sddBehindColor) );
	    drawFillRectangle( lc->lcDrawingSurface, &drRedraw );
	    }

	done= tedDrawTextLine( paraNode, line, pf, drLine, vsdd, dc, bo );

	tedOriginalClipping( dc, sdd );
	}
    else{
	TextLine *	tl= paraNode->biParaLines+ line;

	done= tl->tlParticuleCount;
	}

    return done;
    }

