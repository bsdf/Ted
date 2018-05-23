/************************************************************************/
/*									*/
/*  Print PostScript : print one line of text.				*/
/*									*/
/************************************************************************/

#   include	"docLayoutConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<stdlib.h>

#   include	<sioGeneral.h>
#   include	<textUtf8Util.h>

#   include	<psFontMetrics.h>
#   include	"docPsPrintImpl.h"
#   include	"docDraw.h"
#   include	"docLayout.h"
#   include	<docTreeType.h>
#   include	<docTreeNode.h>
#   include	<docTextLine.h>
#   include	<docHyperlinkField.h>
#   include	<docBookmarkField.h>
#   include	<utilMemoryBufferPrintf.h>
#   include	<docNotes.h>

#   include	<appDebugon.h>


/************************************************************************/
/*									*/
/*  Print a string.							*/
/*									*/
/************************************************************************/

static void docPsPrintString(	const DrawTextLine *		dtl,
				int				baseline,
				const ParticuleData *		pd,
				int				textAttr,
				const TextAttribute *		ta,
				const unsigned char *		s,
				int				len )
    {
    PrintingState *		ps= (PrintingState *)dtl->dtlThrough;
    DrawingContext *		dc= dtl->dtlDrawingContext;
    int				fontSizeTwips= 10* ta->taFontSizeHalfPoints;

    if  ( len > 0 )
	{
	int			y;

	docDrawSetFont( dc, (void *)ps, textAttr, ta );
	docDrawSetColorNumber( dc, (void *)ps, ta->taTextColorNumber );

	y= baseline;

	if  ( ta->taSuperSub == TEXTvaSUPERSCRIPT )
	    { psGetSuperBaseline( &y, baseline, fontSizeTwips, pd->pdAfi ); }

	if  ( ta->taSuperSub == TEXTvaSUBSCRIPT )
	    { psGetSubBaseline( &y, baseline, fontSizeTwips, pd->pdAfi ); }

	psMoveShowString( ps, s, len,
			pd->pdX0+ pd->pdLeftBorderWidth+ dtl->dtlXShift, y );

	ps->psLastPageMarked= ps->psPagesPrinted;
	}

    return;
    }

static void psPrintSegment(	const DrawTextLine *		dtl,
				int				textAttr,
				const TextAttribute *		ta,
				const unsigned char *		s,
				int				len )
    {
    PrintingState *		ps= (PrintingState *)dtl->dtlThrough;
    DrawingContext *		dc= dtl->dtlDrawingContext;

    docDrawSetFont( dc, (void *)ps, textAttr, ta );
    psShowString( ps, s, len );
    return;
    }

static void psPrintSegments(	const DrawTextLine *		dtl,
				int				baseLine,
				const ParticuleData *		pd,
				int				textAttr,
				const TextAttribute *		ta,
				const unsigned char *		s,
				const int *			segments,
				int				segmentCount )
    {
    TextAttribute		taN= *ta;
    int				seg;

    taN.taSmallCaps= 0;

    if  ( segments[0] > 0 )
	{
	docPsPrintString( dtl, baseLine, pd, textAttr, &taN, s, segments[0] );
	s += segments[0];

	psPrintSegment( dtl, textAttr, ta, s, segments[1] );
	s += segments[1];
	}
    else{
	docPsPrintString( dtl, baseLine, pd, textAttr, ta, s, segments[1] );
	s += segments[1];
	}

    for ( seg= 1; seg < segmentCount; seg++ )
	{
	if  ( segments[2* seg+ 0] > 0 )
	    {
	    psPrintSegment( dtl, textAttr, &taN, s, segments[2* seg+ 0] );
	    s += segments[2* seg+ 0];
	    }

	if  ( segments[2* seg+ 1] > 0 )
	    {
	    psPrintSegment( dtl, textAttr, ta, s, segments[2* seg+ 1] );
	    s += segments[2* seg+ 1];
	    }
	}

    return;
    }

int docPsPrintRunUnderline(		const DrawTextLine *	dtl,
					int			part,
					int			upto,
					int			textAttrNr,
					const TextAttribute *	ta,
					int			x0Twips,
					int			x1Twips,
					const LayoutPosition *	baseLine )
    {
    const ParticuleData *	pd= dtl->dtlParticuleData;

    int				y0;
    int				h;

    psUnderlineGeometry( &y0, &h, baseLine->lpPageYTwips,
			10* ta->taFontSizeHalfPoints, pd[part].pdAfi );

    docDrawSetColorNumber( dtl->dtlDrawingContext, dtl->dtlThrough,
						    ta->taTextColorNumber );
    psFillRectangle( (PrintingState *)dtl->dtlThrough,
			x0Twips+ dtl->dtlXShift, y0, x1Twips- x0Twips, h );

    return 0;
    }

int docPsPrintRunStrikethrough(	const DrawTextLine *	dtl,
					int			part,
					int			upto,
					int			textAttrNr,
					const TextAttribute *	ta,
					int			x0Twips,
					int			x1Twips,
					const LayoutPosition *	baseLine )
    {
    const ParticuleData *	pd= dtl->dtlParticuleData;

    int				y0;
    int				h;

    psStrikethroughGeometry( &y0, &h, baseLine->lpPageYTwips,
			10* ta->taFontSizeHalfPoints, pd[part].pdAfi );

    docDrawSetColorNumber( dtl->dtlDrawingContext, dtl->dtlThrough,
						    ta->taTextColorNumber );
    psFillRectangle( (PrintingState *)dtl->dtlThrough,
			x0Twips+ dtl->dtlXShift, y0, x1Twips- x0Twips, h );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Print a series of particules with the same attributes.		*/
/*									*/
/************************************************************************/

int docPsPrintSpan(		const DrawTextLine *	dtl,
				int			part,
				int			count,
				const LayoutPosition *	baseLine,
				int			textAttrNr,
				const TextAttribute *	ta,
				const char *		printString,
				int			nbLen )
    {
    PrintingState *		ps= (PrintingState *)dtl->dtlThrough;
    int				rval= 0;

    char *			scratchString= (char *)0;
    int *			segments= (int *)0;
    int				segmentCount= 0;

    int				spanBaseline= baseLine->lpPageYTwips;

    const ParticuleData *	pd= dtl->dtlParticuleData+ part;

    if  ( ta->taSmallCaps || ta->taCapitals )
	{
	if  ( docMakeCapsString( &scratchString, &segments, &segmentCount,
						ta, printString, nbLen ) )
	    { LDEB(nbLen); rval= -1; goto ready;	}

	printString= scratchString;
	}

    if  ( 0 )
	{
	scratchString= malloc( nbLen+ 1 );
	if  ( ! scratchString )
	    { XDEB(scratchString); rval= -1; goto ready;	}

	if  ( textMirrorUtf8String( scratchString, printString, nbLen ) )
	    { LDEB(nbLen); rval= -1; goto ready;	}

	printString= scratchString;
	}

    if  (  ta->taSmallCaps && ! ta->taCapitals )
	{
	psPrintSegments( dtl, spanBaseline, pd, textAttrNr, ta,
			(unsigned char *)printString, segments, segmentCount );
	}
    else{
	docPsPrintString( dtl, spanBaseline, pd, textAttrNr, ta,
					(unsigned char *)printString, nbLen );
	}

    ps->psLinkParticulesDone += count;

  ready:

    if  ( scratchString )
	{ free( scratchString );	}
    if  ( segments )
	{ free( segments );		}

    return rval;
    }

int docPsPrintFtnsep(		const DrawTextLine *	dtl,
				int			part,
				int			textAttrNr,
				const TextAttribute *	ta,
				int			x0Twips,
				int			x1Twips,
				const LayoutPosition *	baseLine )
    {
    PrintingState *		ps= (PrintingState *)dtl->dtlThrough;
    DrawingContext *		dc= dtl->dtlDrawingContext;

    int				fontSizeTwips;

    int				xHeight;

    int				y0;
    int				h;

    fontSizeTwips= 10* ta->taFontSizeHalfPoints;

    xHeight= ( fontSizeTwips+ 1 )/ 2;

    y0= baseLine->lpPageYTwips- xHeight/2- 15/2;
    h= 15;

    docDrawSetColorNumber( dc, (void *)ps, ta->taTextColorNumber );
    psFillRectangle( ps, x0Twips, y0, x1Twips- x0Twips, h );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Start a field							*/
/*									*/
/************************************************************************/

int docPsPrintStartField(	const DrawTextLine *	dtl,
				int			part,
				const DocumentField *	df )
    {
    int				rval= 0;

    PrintingState *		ps= (PrintingState *)dtl->dtlThrough;
    DrawingContext *		dc= dtl->dtlDrawingContext;
    const LayoutContext *	lc= &(dc->dcLayoutContext);
    const BufferDocument *	bd= lc->lcDocument;

    const TextLine *		tl= dtl->dtlTextLine;
    int				lineTop= tl->tlTopPosition.lpPageYTwips;

    const BufferItem *		paraNode= dtl->dtlParaNode;
    const ParticuleData *	pd= dtl->dtlParticuleData+ part;

    const MemoryBuffer *	mbBookmark;

    HyperlinkField		hf;

    docInitHyperlinkField( &hf );

    if  ( df->dfKind == DOCfkBOOKMARK )
	{
	if  ( ! docFieldGetBookmark( &mbBookmark, df ) )
	    {
	    psDestPdfmark( ps, lineTop, mbBookmark );
	    }

	goto ready;
	}

    if  ( df->dfKind == DOCfkCHFTN )
	{
	const MemoryBuffer *	refName;
	const MemoryBuffer *	markName;

	struct DocumentNote *	dn;

	int			cnt;
	int			closed;

	cnt= docCountParticulesInField( paraNode, &closed, part,
					paraNode->biParaParticuleCount );
	if  ( cnt < 0 )
	    { LDEB(cnt); }

	dn= docGetNoteOfField( df, bd );
	if  ( ! dn )
	    { LXSDEB(cnt,dn,docTreeTypeStr(paraNode->biTreeType)); }

	utilMemoryBufferPrintf( &(ps->psNoteRef),
				    "_NREF_%d", df->dfFieldNumber+ 1 );
	utilMemoryBufferPrintf( &(ps->psNoteDef),
				    "_NDEF_%d", df->dfFieldNumber+ 1 );

	if  ( paraNode->biTreeType == DOCinBODY )
	    {
	    markName= &(ps->psNoteDef);
	    refName=  &(ps->psNoteRef);
	    }
	else{
	    markName= &(ps->psNoteRef);
	    refName=  &(ps->psNoteDef);
	    }

	psDestPdfmark( ps, lineTop, refName );

	utilEmptyMemoryBuffer( &(ps->psLinkFile) );
	if  ( utilCopyMemoryBuffer( &(ps->psLinkMark), markName ) )
	    { LDEB(1); rval= -1; goto ready;	}

	ps->psInsideLink= 1;
	ps->psLinkParticulesDone= 0;
	ps->psLinkRectLeft= pd->pdX0;

	goto ready;
	}

    if  ( df->dfKind == DOCfkHYPERLINK )
	{
	if  ( ! docGetHyperlinkField( &hf, df ) )
	    {
	    ps->psInsideLink= 1;
	    ps->psLinkParticulesDone= 0;
	    ps->psLinkRectLeft= pd->pdX0;

	    if  ( utilCopyMemoryBuffer( &(ps->psLinkFile), &(hf.hfFile) ) )
		{ LDEB(1); rval= -1; goto ready;	}
	    if  ( utilCopyMemoryBuffer( &(ps->psLinkMark), &(hf.hfBookmark) ) )
		{ LDEB(1); rval= -1; goto ready;	}
	    }

	goto ready;
	}

  ready:

    docCleanHyperlinkField( &hf );

    return rval;
    }

int docPsPrintFinishField(	const DrawTextLine *	dtl,
				int			part,
				const DocumentField *	df )
    {
    PrintingState *		ps= (PrintingState *)dtl->dtlThrough;
    const ParticuleData *	pd= dtl->dtlParticuleData+ part;

    const TextLine *		tl= dtl->dtlTextLine;
    int				lineTop= tl->tlTopPosition.lpPageYTwips;
    int				lineHeight= tl->tlDescY1- tl->tlAscY0;

    if  ( ps->psInsideLink )
	{
	psFlushLink( ps, pd->pdX0, pd->pdVisibleBBox.drX1,
					    lineTop, lineHeight );
	ps->psInsideLink= 0;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Layout and print successive lines of a paragraph.			*/
/*									*/
/************************************************************************/

int docPsPrintTextLine(		BufferItem *			paraNode,
				int				line,
				const ParagraphFrame *		pf,
				const DocumentRectangle *	drLine,
				void *				vps,
				DrawingContext *		dc,
				const BlockOrigin *		bo )
    {
    const LayoutContext *	lc= &(dc->dcLayoutContext);
    PrintingState *		ps= (PrintingState *)vps;

    const TextLine *		tl= paraNode->biParaLines+ line;
    int				part= tl->tlFirstParticule;

    int				done= 0;
    int				accepted;
    TextLine			boxLine;

    ParticuleData *		pd;

    int				xTwips;
    DrawTextLine		dtl;

    docInitDrawTextLine( &dtl );

    if  ( docPsClaimParticuleData( paraNode->biParaParticuleCount, &pd ) )
	{ LDEB(paraNode->biParaParticuleCount); return -1;	}

    boxLine= *tl;
    accepted= docLayoutLineBox( &boxLine, paraNode, part, lc, pd+ part, pf );

    if  ( accepted < 1 )
	{ LDEB(accepted); return -1;	}

    docSetDrawTextLine( &dtl, vps, dc, tl, paraNode, bo, pf, drLine );
    dtl.dtlParticuleData= pd;
    dtl.dtlDrawParticulesSeparately= paraNode->biParaAlignment == DOCthaJUSTIFIED;

    if  ( ps->psInsideLink )
	{ ps->psLinkRectLeft= pd->pdX0;	}

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

    if  ( done > 0 && ps->psInsideLink )
	{
	int			lineTop= tl->tlTopPosition.lpPageYTwips;
	int			lineHeight= tl->tlDescY1- tl->tlAscY0;

	const ParticuleData *	ppd= dtl.dtlParticuleData+ done- 1;

	psFlushLink( ps, ppd->pdX0, ppd->pdVisibleBBox.drX1,
						    lineTop, lineHeight );
	}

    return accepted;
    }

