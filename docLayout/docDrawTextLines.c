#   include	"docLayoutConfig.h"

#   include	"docDraw.h"
#   include	"docLayout.h"
#   include	<docTextLine.h>
#   include	<docTabStop.h>
#   include	<docParaRulerAdmin.h>

#   include	<appDebugon.h>
#   include	<docTreeNode.h>
#   include	"docTextRun.h"
#   include	<docTextParticule.h>

/************************************************************************/
/*									*/
/*  Draw the underlines of a text run.					*/
/*									*/
/************************************************************************/

static void docDrawRunUnderlines(	const DrawTextLine *	dtl,
					int			part,
					int			upto,
					const LayoutPosition *	baseLine )
    {
    DrawingContext *		dc= dtl->dtlDrawingContext;
    const LayoutContext *	lc= &(dc->dcLayoutContext);
    BufferDocument *		bd= lc->lcDocument;

    const BufferItem *		paraNode= dtl->dtlParaNode;
    const TextParticule *	tp= paraNode->biParaParticules;
    const ParticuleData *	pd= dtl->dtlParticuleData;

    while( part < upto )
	{
	int		textAttrNr;
	TextAttribute	ta;
	int		x0Twips= 0;
	int		x1Twips= 0;

	int		head;
	int		past;

	if  ( docNextUnderlinedRun( &head, &past, &textAttrNr, &ta,
						bd, paraNode, part, upto ) )
	    { break;	}

	if  ( pd )
	    {
	    x0Twips= pd[head].pdX0;
	    x1Twips= pd[past- 1].pdX0+ tp[past- 1].tpTwipsWide;
	    }

	(*dc->dcDrawUnderline)( dtl, head, past,
				textAttrNr, &ta, x0Twips, x1Twips, baseLine );

	part= past;
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Draw the strikethrough of a text run.				*/
/*									*/
/************************************************************************/

static void docDrawRunStrikethrough(	const DrawTextLine *	dtl,
					int			part,
					int			upto,
					const LayoutPosition *	baseLine )
    {
    DrawingContext *		dc= dtl->dtlDrawingContext;
    const LayoutContext *	lc= &(dc->dcLayoutContext);
    BufferDocument *		bd= lc->lcDocument;

    const BufferItem *		paraNode= dtl->dtlParaNode;
    const TextParticule *	tp= paraNode->biParaParticules;
    const ParticuleData *	pd= dtl->dtlParticuleData;

    while( part < upto )
	{
	int		textAttrNr;
	TextAttribute	ta;
	int		x0Twips= 0;
	int		x1Twips= 0;

	int		head;
	int		past;

	if  ( docNextStrikethroughRun( &head, &past, &textAttrNr, &ta,
						bd, paraNode, part, upto ) )
	    { break;	}

	if  ( pd )
	    {
	    x0Twips= pd[head].pdX0;
	    x1Twips= pd[past- 1].pdX0+ tp[past- 1].tpTwipsWide;
	    }

	(*dc->dcDrawStrikethrough)( dtl, head, past,
				textAttrNr, &ta, x0Twips, x1Twips, baseLine );

	part= past;
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Generic Document drawing machinery.					*/
/*									*/
/************************************************************************/

static int docDrawShadingStretch(	int *			pNext,
					int *			pXTwips,
					DocumentRectangle *	drText,
					int			baseline,
					int			skip,
					DrawingContext *	dc,
					const BufferItem *	paraNode,
					int			part,
					int			partUpto )
    {
    const LayoutContext *	lc= &(dc->dcLayoutContext);
    BufferDocument *		bd= lc->lcDocument;
    const TextParticule *	tp= paraNode->biParaParticules+ part;
    TextAttribute		ta;
    int				x0Twips= *pXTwips;
    int				xTwips= *pXTwips;

    docGetEffectiveTextAttributes( &ta, bd, paraNode, part );

    while( ta.taShadingNumber == skip )
	{
	xTwips += tp->tpTwipsWide;
	part++; tp++;

	if  ( part >= partUpto )
	    { break;	}

	docGetEffectiveTextAttributes( &ta, bd, paraNode, part );
	}

    *pNext= ta.taShadingNumber;
    *pXTwips= xTwips;
    if  ( drText )
	{
	drText->drX0= x0Twips;
	drText->drX1= xTwips;
	}

    return part;
    }

static int docDrawBorderStretch(	int *			pNext,
					int *			pXTwips,
					DocumentRectangle *	drText,
					int			baseline,
					int			skip,
					DrawingContext *	dc,
					const BufferItem *	paraNode,
					int			part,
					int			partUpto )
    {
    const LayoutContext *	lc= &(dc->dcLayoutContext);
    BufferDocument *		bd= lc->lcDocument;
    const TextParticule *	tp= paraNode->biParaParticules+ part;
    TextAttribute		ta;
    int				x0Twips= *pXTwips;
    int				xTwips= *pXTwips;

    docGetEffectiveTextAttributes( &ta, bd, paraNode, part );

    while( ta.taBorderNumber == skip )
	{
	xTwips += tp->tpTwipsWide;
	part++; tp++;

	if  ( part >= partUpto )
	    { break;	}

	docGetEffectiveTextAttributes( &ta, bd, paraNode, part );
	}

    *pNext= ta.taBorderNumber;
    *pXTwips= xTwips;
    if  ( drText )
	{
	drText->drX0= x0Twips;
	drText->drX1= xTwips;
	}

    return part;
    }

/************************************************************************/

static int docDrawTextBorders(
			const BufferItem *		paraNode,
			const TextLine *		tl,
			int				baseline,
			const DocumentRectangle *	drLine,
			int				page,
			int				xTwips,
			void *				through,
			DrawingContext *		dc )
    {
    const LayoutContext *	lc= &(dc->dcLayoutContext);
    const BufferDocument *	bd= lc->lcDocument;

    BlockOrnaments		ornaments;

    int				part;
    int				partUpto;

    docInitBlockOrnaments( &ornaments );

    part= tl->tlFirstParticule;
    partUpto= tl->tlFirstParticule+ tl->tlParticuleCount;
    while( part < partUpto )
	{
	int			borderNumber;
	int			ignored;
	int			next;

	DocumentRectangle	drText;

	part= docDrawBorderStretch( &borderNumber, &xTwips,
					(DocumentRectangle *)0, baseline,
					0, dc, paraNode, part, partUpto );

	if  ( part >= partUpto )
	    { break;	}

	drText= *drLine;

	next= docDrawBorderStretch( &ignored, &xTwips, &drText, baseline,
				borderNumber, dc, paraNode, part, partUpto );

	if  ( next > part && xTwips > drText.drX0 )
	    {
	    docGetBorderPropertiesByNumber( &(ornaments.boTopBorder),
							    bd, borderNumber );
	    ornaments.boTopBorderNumber= borderNumber;

	    if  ( DOCisBORDER( &(ornaments.boTopBorder) ) )
		{
		int			thick;
		int			space;
		DocumentRectangle	drInside;

		PROPmaskADD( &(ornaments.boPropMask), ORNdrawTOP_BORDER );
		PROPmaskADD( &(ornaments.boPropMask), ORNdrawLEFT_BORDER );
		PROPmaskADD( &(ornaments.boPropMask), ORNdrawRIGHT_BORDER );
		PROPmaskADD( &(ornaments.boPropMask), ORNdrawBOTTOM_BORDER );

		ornaments.boLeftBorder= ornaments.boTopBorder;
		ornaments.boRightBorder= ornaments.boTopBorder;
		ornaments.boBottomBorder= ornaments.boTopBorder;

		ornaments.boLeftBorderNumber= ornaments.boTopBorderNumber;
		ornaments.boRightBorderNumber= ornaments.boTopBorderNumber;
		ornaments.boBottomBorderNumber= ornaments.boTopBorderNumber;

		geoIntersectRectangle( &drText, &drText, drLine );

		thick= docBorderThick( &space, &(ornaments.boTopBorder) );
		drInside= drText;

		drInside.drX0 += thick;
		drInside.drX1 -= thick;
		drInside.drY0 += thick;
		drInside.drY1 -= thick;

		if  ( (*dc->dcDrawOrnaments)( &ornaments, page,
					    &drText, &drInside, through, dc ) )
		    { LDEB(1); return -1;	}
		}
	    }

	part= next;
	}

    return 0;
    }

/************************************************************************/

static int docDrawTextShading(
			const BufferItem *		paraNode,
			const TextLine *		tl,
			int				baseline,
			const DocumentRectangle *	drLine,
			int				page,
			int				xTwips,
			void *				through,
			DrawingContext *		dc )
    {
    const LayoutContext *	lc= &(dc->dcLayoutContext);
    const BufferDocument *	bd= lc->lcDocument;

    BlockOrnaments		ornaments;

    int				part;
    int				partUpto;

    docInitBlockOrnaments( &ornaments );
    PROPmaskADD( &(ornaments.boPropMask), ORNdrawSHADE );

    part= tl->tlFirstParticule;
    partUpto= tl->tlFirstParticule+ tl->tlParticuleCount;
    while( part < partUpto )
	{
	int			shadingNumber;
	int			ignored;
	int			next;
	DocumentRectangle	drText;

	part= docDrawShadingStretch( &shadingNumber, &xTwips,
					    (DocumentRectangle *)0, baseline,
					    0, dc, paraNode, part, partUpto );

	if  ( part >= partUpto )
	    { break;	}

	drText= *drLine;

	next= docDrawShadingStretch( &ignored, &xTwips, &drText, baseline,
				shadingNumber, dc, paraNode, part, partUpto );

	if  ( next > part && xTwips > drText.drX0 )
	    {
	    docGetItemShadingByNumber( &(ornaments.boShading),
							bd, shadingNumber );

	    geoIntersectRectangle( &drText, &drText, drLine );

	    if  ( (*dc->dcDrawOrnaments)( &ornaments,
					page, &drText, &drText, through, dc ) )
		{ LDEB(1); return -1;	}
	    }

	part= next;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Draw borders and shading below the text of a line.			*/
/*									*/
/*  Experimentation with MS-Word reveals:				*/
/*  1)  That MS-Word draws the border over the text.			*/
/*  2)  The the border is subtracted from the rectangle.		*/
/*  3)  That the same rectangle is used for borders and shading. This	*/
/*	same rectangle is shown in reverse when text is selected.	*/
/*									*/
/************************************************************************/

static int docDrawLineBackground(
			const BufferItem *		paraNode,
			const TextLine *		tl,
			const ParagraphFrame *		pf,
			void *				through,
			DrawingContext *		dc,
			const BlockOrigin *		bo )
    {
    int				page;

    DocumentRectangle		drLine;
    int				lineHeight= tl->tlLineStride;
    int				xTwips;
    int				baseline;

    page= tl->tlTopPosition.lpPage;
    if  ( bo->boOverrideFrame )
	{ page= bo->boFrameOverride.lpPage;	}

    drLine= pf->pfParaContentRect;
    drLine.drY0= tl->tlTopPosition.lpPageYTwips+ bo->boYShift;
    drLine.drY1= drLine.drY0+ lineHeight;

    baseline= drLine.drY0+ TL_BASELINE( tl );

    xTwips= pf->pfParaContentRect.drX0+ tl->tlLineIndent;

    if  ( tl->tlFlags & TLflagSHADING )
	{
	if  ( docDrawTextShading( paraNode, tl, baseline,
				    &drLine, page, xTwips, through, dc ) )
	    { LDEB(1); return -1;	}
	}

    if  ( tl->tlFlags & TLflagBORDER )
	{
	if  ( docDrawTextBorders( paraNode, tl, baseline,
				    &drLine, page, xTwips, through, dc ) )
	    { LDEB(1); return -1;	}
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Draw a series of text lines in a paragraph.				*/
/*									*/
/*  1)  For all lines that apply: Those in paraNode in the current	*/
/*	page/column combination.					*/
/*  2)  If the line is on a subsequent page.. stop.			*/
/*  3)  Is line is on a page/column before the pages/column to draw?	*/
/*  4)  Is the line in the rectangle to redraw?				*/
/*	As this is inside a column, a line that is outside the clipping	*/
/*	rectangle vertically is out of range inside this column.	*/
/*  5)  Printing a page range.. and not in range?			*/
/*  6)  Drawing a selection? E.G. To highlight it?			*/
/*									*/
/************************************************************************/

int docDrawTextLines(	void *				through,
			const ParagraphDrawingStrip *	pds,
			BufferItem *			paraNode,
			const ParagraphFrame *		pf,
			DrawingContext *		dc,
			const BlockOrigin *		bo )
    {
    const LayoutContext *	lc= &(dc->dcLayoutContext);
    int				done= 0;
    int				pastSelectionEnd= 0;

    int				line= pds->pdsLineFrom;
    const TextLine *		tl= paraNode->biParaLines+ line;

    if  ( pds->pdsShadeTop.lpPage != pds->pdsShadeBelow.lpPage		||
	  pds->pdsShadeTop.lpColumn != pds->pdsShadeBelow.lpColumn	)
	{
	LLDEB(pds->pdsShadeTop.lpPage,pds->pdsShadeBelow.lpPage);
	LLDEB(pds->pdsShadeTop.lpColumn,pds->pdsShadeBelow.lpColumn);
	}

    /*  1  */
    while( line < paraNode->biParaLineCount )
	{
	int			accepted;
	int			beforeSelectionBegin= 0;

	LayoutPosition		lpTop;
	LayoutPosition		lpBottom;
	DocumentRectangle	drLinePixels;

	docShiftPosition( &lpTop, bo, &(tl->tlTopPosition) );
	lpBottom= lpTop;
	lpBottom.lpPageYTwips += tl->tlLineStride;

	docGetPixelRectForPos( &drLinePixels, lc,
			pf->pfCellContentRect.drX0+ bo->boXShift,
			pf->pfCellContentRect.drX1+ bo->boXShift,
			&lpTop, &lpBottom );

	/*  2  */
	if  ( lpTop.lpPage > pds->pdsShadeTop.lpPage )
	    { break;	}

	/*  3  */
	if  ( lpTop.lpPage < pds->pdsShadeTop.lpPage )
	    { beforeSelectionBegin= 1;	}
	else{
	    if  ( lpTop.lpColumn > pds->pdsShadeTop.lpColumn )
		{ break;	}
	    if  ( lpTop.lpColumn < pds->pdsShadeTop.lpColumn )
		{ beforeSelectionBegin= 1;	}
	    }

	/*  4  */
	if  ( dc->dcClipRect )
	    {
	    if  ( drLinePixels.drY0 > dc->dcClipRect->drY1 )
		{ pastSelectionEnd= 1;	}
	    if  ( drLinePixels.drY1 < dc->dcClipRect->drY0 )
		{ beforeSelectionBegin= 1;	}
	    }

	/*  5  */
	if  ( dc->dcFirstPage >= 0				&&
	      pds->pdsShadeTop.lpPage < dc->dcFirstPage	)
	    { beforeSelectionBegin= 1;	}

	/*  6  */
	if  ( dc->dcSelection						&&
	      ! beforeSelectionBegin					&&
	      ! pastSelectionEnd					&&
	      ( paraNode == dc->dcSelection->dsHead.dpNode	||
	        paraNode == dc->dcSelection->dsTail.dpNode	)	)
	    {
	    const DocumentSelection *	ds= dc->dcSelection;

	    DocumentSelection		dsLine;

	    int				partLineBegin;
	    int				partLineEnd;

	    docLineSelection( &dsLine, &partLineBegin, &partLineEnd,
							    paraNode, line );

	    if  ( paraNode == ds->dsTail.dpNode				&&
		  docComparePositions( &(dsLine.dsHead), &(ds->dsTail) ) > 0 )
		{ pastSelectionEnd= 1;	}

	    if  ( paraNode == ds->dsHead.dpNode				&&
		  docComparePositions( &(dsLine.dsTail), &(ds->dsHead) ) < 0 )
		{ beforeSelectionBegin= 1;	}
	    }

	/****************************************************************/
	/*  Do not clip here: The find code might want to look for	*/
	/*  lines in the immediate proximity of a mouse click.		*/
	/****************************************************************/
	if  ( beforeSelectionBegin	||
	      pastSelectionEnd		||
	      ! dc->dcDrawTextLine	)
	    { accepted= tl->tlParticuleCount;		}
	else{
	    if  ( dc->dcDrawOrnaments				&&
		  ( ( tl->tlFlags & TLflagSHADING )	||
		    ( tl->tlFlags & TLflagBORDER )	)	)
		{
		docDrawLineBackground( paraNode, tl, pf, through, dc, bo );
		}

	    accepted= (*dc->dcDrawTextLine)( paraNode, line, pf, &drLinePixels,
							    through, dc, bo );
	    }

	if  ( accepted < 1 )
	    { LDEB(accepted); return -1; }

	done += accepted; line++; tl++;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Draw a series of particules with the same attributes.		*/
/*									*/
/************************************************************************/

int docDrawLineParticules(	const DrawTextLine *		dtl,
				int *				pXTwips,
				int				part )
    {
    DrawingContext *		dc= dtl->dtlDrawingContext;
    const LayoutContext *	lc= &(dc->dcLayoutContext);
    BufferDocument *		bd= lc->lcDocument;
    const BufferItem *		paraNode= dtl->dtlParaNode;

    const TextParticule *	tp= paraNode->biParaParticules+ part;

    int				drawn;

    TextAttribute		ta;

    int				x0Twips= *pXTwips+ dtl->dtlXShift;
    int				x1Twips= x0Twips+ tp->tpTwipsWide;

    TabStopList			tsl;
    LayoutPosition		spanBaseline;
    int				textAttrNr;

    docGetTabStopListByNumber( &tsl, bd, paraNode->biParaTabStopListNumber );

    textAttrNr= docGetEffectiveTextAttributes( &ta, bd, paraNode, part );

    spanBaseline= dtl->dtlShiftedBaselinePosition;
    spanBaseline.lpPageYTwips += 10* ta.taBaselineShiftHalfPoints;

    /*  1  */
    switch( tp->tpKind )
	{
	case DOCkindTAB:
	    if  ( dc->dcDrawTab )
		{
		int		leader= DOCtlNONE;

		if  ( tp->tpObjectNumber >= 0			&&
		      tp->tpObjectNumber < tsl.tslTabStopCount	)
		    {
		    const TabStop *	ts;
		    ts= tsl.tslTabStops+ tp->tpObjectNumber;
		    leader= ts->tsLeader;
		    }

		if  ( (*dc->dcDrawTab)( dtl, part, textAttrNr, &ta,
				leader, x0Twips, x1Twips, &spanBaseline ) )
		    { LDEB(1);	}

		if  ( ta.taTextIsUnderlined && dc->dcDrawUnderline )
		    {
		    docDrawRunUnderlines( dtl,
					part, part+ 1, &spanBaseline );
		    }
		if  ( ta.taHasStrikethrough && dc->dcDrawStrikethrough )
		    {
		    docDrawRunStrikethrough( dtl,
					part, part+ 1, &spanBaseline );
		    }
		}
	    (*pXTwips) += tp->tpTwipsWide;
	    return drawn= 1;

	case DOCkindSPAN:
	    break;

	case DOCkindFIELDHEAD:
	    if  ( dc->dcStartField )
		{
		const DocumentField *	df;

		df= docGetFieldByNumber( &(bd->bdFieldList),
						    tp->tpObjectNumber );
		if  ( ! df )
		    { LXDEB(tp->tpObjectNumber,df);	}
		else{
		    if  ( (*dc->dcStartField)( dtl, part, df ) )
			{ LDEB(part); return drawn= 1;	}
		    }
		}
	    (*pXTwips) += tp->tpTwipsWide;
	    return drawn= 1;

	case DOCkindFIELDTAIL:
	    if  ( dc->dcFinishField )
		{
		const DocumentField *	df;

		df= docGetFieldByNumber( &(bd->bdFieldList),
						    tp->tpObjectNumber );
		if  ( ! df )
		    { LXDEB(tp->tpObjectNumber,df);	}
		else{
		    if  ( (*dc->dcFinishField)( dtl, part, df ) )
			{ LDEB(part); return drawn= 1;	}
		    }
		}
	    (*pXTwips) += tp->tpTwipsWide;
	    return drawn= 1;

	case DOCkindLINEBREAK:
	case DOCkindPAGEBREAK:
	case DOCkindCOLUMNBREAK:
	    (*pXTwips) += tp->tpTwipsWide;
	    return drawn= 1;

	case DOCkindOBJECT:
	    if  ( dc->dcDrawObject )
		{
		InsertedObject *	io;

		io= docGetObject( bd, tp->tpObjectNumber );
		if  ( ! io )
		    { LPDEB(tp->tpObjectNumber,io); return 1;	}
		drawn= (*dc->dcDrawObject)( dtl, part, io,
					    x0Twips, x1Twips, &spanBaseline );

		if  ( drawn < 0 )
		    { LDEB(drawn);	}
		}
	    (*pXTwips) += tp->tpTwipsWide;
	    return drawn= 1;

	case DOCkindCHFTNSEP:
	case DOCkindCHFTNSEPC:
	    if  ( dc->dcDrawFtnsep )
		{
		(*dc->dcDrawFtnsep)( dtl, part,
					    textAttrNr, &ta,
					    x0Twips, x1Twips, &spanBaseline );
		}
	    (*pXTwips) += tp->tpTwipsWide;
	    return drawn= 1;

	case DOCkindOPT_HYPH:
	case DOCkindLTR_MARK:
	case DOCkindRTL_MARK:
	    (*pXTwips) += tp->tpTwipsWide;
	    if  ( tp->tpTwipsWide != 0 )
		{ LLDEB(tp->tpKind,tp->tpTwipsWide);	}
	    return drawn= 1;

	default:
	    LDEB(tp->tpKind); return -1;
	}

    {
    TextRun	tr;

    drawn= docLayoutDelimitRun( &tr, *pXTwips, bd, paraNode,
					    part, dtl->dtlPartUpto,
					    dtl->dtlDrawParticulesSeparately );

    if  ( tr.trStrlenNb > 0 && dc->dcDrawSpan )
	{
	const char *    printString;

	printString= (char *)docParaString( paraNode, tp->tpStroff );

	if  ( (*dc->dcDrawSpan)( dtl, part, drawn, &spanBaseline,
				tr.trTextAttributeNr, &(tr.trTextAttribute),
				printString, tr.trStrlenNb ) )
	    { LDEB(tr.trStrlenNb); return -1;	}
	}

	if  ( tr.trHasUnderline && dc->dcDrawUnderline )
	    {
	    docDrawRunUnderlines( dtl,
				tr.trPartFrom, tr.trPartUpto, &spanBaseline );
	    }
	if  ( tr.trHasStrikethrough && dc->dcDrawStrikethrough )
	    {
	    docDrawRunStrikethrough( dtl,
				tr.trPartFrom, tr.trPartUpto, &spanBaseline );
	    }

	*pXTwips += tr.trX1Twips- tr.trX0Twips;
    }

    return drawn;
    }

