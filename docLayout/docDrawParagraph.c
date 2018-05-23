#   include	"docLayoutConfig.h"

#   include	"docDraw.h"
#   include	<docPageGrid.h>
#   include	<docTreeType.h>
#   include	<docTreeNode.h>
#   include	<docNodeTree.h>
#   include	<docTextLine.h>
#   include	"docParagraphLayout.h"
#   include	<docTextParticule.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Paragraph drawing machinery.					*/
/*									*/
/************************************************************************/

/************************************************************************/
/*									*/
/*  Draw paragraph borders and shading.					*/
/*									*/
/************************************************************************/

static int docDrawParaOrnaments( void *				through,
				const ParagraphDrawingStrip *	pds,
				const BufferItem *		paraNode,
				const ParagraphFrame *		pf,
				DrawingContext *		dc,
				const BlockOrigin *		bo )
    {
    const LayoutContext *	lc= &(dc->dcLayoutContext);
    const BufferDocument *	bd= lc->lcDocument;

    LayoutPosition		lpTop;
    LayoutPosition		lpBelow;

    BlockOrnaments		ornaments;
    DocumentRectangle		drPara;
    DocumentRectangle		drOutside;
    DocumentRectangle		drInside;

    docInitBlockOrnaments( &ornaments );

    drPara= pf->pfParaContentRect;
    if  ( paraNode->biParaFirstIndentTwips < 0 )
	{ drPara.drX0 += paraNode->biParaFirstIndentTwips;	}

    docShiftPosition( &lpTop, bo, &(pds->pdsShadeTop) );
    docShiftPosition( &lpBelow, bo, &(pds->pdsShadeBelow) );

    drPara.drY0= lpTop.lpPageYTwips;
    drPara.drY1= lpBelow.lpPageYTwips;

    if  ( paraNode->biParaTableNesting > 0 )
	{
	const BufferItem *	rowBi= docGetRowNode( (BufferItem *)paraNode );

	if  ( ! rowBi )
	    { XDEB(rowBi);	}
	else{
	    int	x0= pf->pfParaContentRect.drX0- rowBi->biRowHalfGapWidthTwips;
	    int	x1= pf->pfParaContentRect.drX1+ rowBi->biRowHalfGapWidthTwips;

	    if  ( drPara.drX0 < x0 )
		{ drPara.drX0=  x0;	}
	    if  ( drPara.drX1 > x1 )
		{ drPara.drX1=  x1;	}
	    }
	}

    docGetParaOrnaments( &ornaments, &drOutside, &drInside, &drPara,
			bd, paraNode, pds->pdsAtParaTop, pds->pdsAtParaBottom );

    if  ( (*dc->dcDrawOrnaments)( &ornaments, lpTop.lpPage,
						&drOutside, &drInside,
						through, dc ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Count the number of lines that fit on the current page.		*/
/*									*/
/************************************************************************/

static int docDelimitParagraphDrawingStrip(
			ParagraphDrawingStrip *		pds,
			const BufferItem *		paraNode,
			int				countAfter,
			const LayoutPosition *		lpShadeTop,
			int				lineFrom,
			const LayoutPosition *		lpThisFrame,
			const BlockOrigin *		bo )
    {
    const TextLine *	tl;
    LayoutPosition	lp;

    int			line= lineFrom;
    int			atTop= lineFrom == 0;
    int			atBottom= 0;
    int			afterPageBreak= 0;

    LayoutPosition	lpBelow= *lpShadeTop;

    tl= paraNode->biParaLines+ line;
    while( line < paraNode->biParaLineCount )
	{
	docShiftPosition( &lp, bo, &(tl->tlTopPosition) );

	if  ( DOC_COLUMN_AFTER( &lp, lpThisFrame ) )
	    { break;	}
	if  ( DOC_COLUMN_AFTER( lpThisFrame, &lp ) )
	    { line++; tl++; continue;	}

	lp.lpPageYTwips += tl->tlLineStride;
	lp.lpAtTopOfColumn= 0;

	docLayoutPushBottomDown( &lpBelow, &lp );

	afterPageBreak= paraNode->biParaParticules[
		    tl->tlFirstParticule+ tl->tlParticuleCount -1].tpKind ==
		    DOCkindPAGEBREAK;

	line++; tl++;
	}

    if  ( line == paraNode->biParaLineCount )
	{
	if  ( ! afterPageBreak )
	    {
	    lp= paraNode->biBelowPosition;

	    if  ( ! countAfter )
		{ lp.lpPageYTwips -= paraNode->biParaSpaceAfterTwips;	}

	    docLayoutPushBottomDownShifted( &lpBelow, &lp, bo );
	    }

	atBottom= 1;
	}

    pds->pdsLineFrom= lineFrom;
    pds->pdsLineUpto= line;
    pds->pdsAtParaTop= atTop;
    pds->pdsAtParaBottom= atBottom;

    pds->pdsShadeTop= *lpShadeTop;
    pds->pdsShadeBelow= lpBelow;

    return 0;
    }

/************************************************************************/

int docDrawParagraphStrip(		void *			through,
					ParagraphDrawingStrip *	pds,
					BufferItem *		paraNode,
					int			countAfter,
					const LayoutPosition *	lpShadeTop,
					int			lineFrom,
					const ParagraphFrame *	pf,
					DrawingContext *	dc,
					const LayoutPosition *	lpThisFrame,
					const BlockOrigin *	bo )
    {
    if  ( docDelimitParagraphDrawingStrip( pds, paraNode, countAfter,
				    lpShadeTop, lineFrom, lpThisFrame, bo ) )
	{ LDEB(1); return -1;	}

    if  ( pds->pdsLineUpto > lineFrom && dc->dcDrawOrnaments )
	{
	if  ( docDrawParaOrnaments( through, pds, paraNode, pf, dc, bo ) )
	    { LDEB(1); return -1;	}
	}

    if  ( docDrawTextLines( through, pds, paraNode, pf, dc, bo ) )
	{ LDEB(lpThisFrame->lpPage); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Print a node in the BufferItem hierarchy.				*/
/*									*/
/*  1)  The last page is finished by the caller.			*/
/*									*/
/************************************************************************/

int docDrawParaNode(		LayoutPosition *		lpBelow,
				BufferItem *			paraNode,
				void *				through,
				DrawingContext *		dc,
				const BlockOrigin *		bo )
    {
    int				rval= 0;
    ParagraphFrame		pf;
    int				line= 0;

    const LayoutContext *	lc= &(dc->dcLayoutContext);
    BufferDocument *		bd= lc->lcDocument;

    BlockFrame			bf;
    LayoutPosition		lpTop;
    LayoutPosition		lpShadeTop;

    docLayoutInitBlockFrame( &bf );
    docParaBlockFrameTwips( &bf, paraNode, bd, paraNode->biTopPosition.lpPage,
					    paraNode->biTopPosition.lpColumn );

    docParagraphFrameTwips( &pf, &bf, paraNode );

    docShiftPosition( &lpTop, bo, &(paraNode->biTopPosition) );
    lpShadeTop= lpTop;

    if  ( ! lpShadeTop.lpAtTopOfColumn )
	{ lpShadeTop.lpPageYTwips += paraNode->biParaSpaceBeforeTwips; }

    while( line < paraNode->biParaLineCount )
	{
	const int		countAfter= 0;

	ParagraphDrawingStrip	pds;

	if  ( docDrawParagraphStrip( through, &pds, paraNode, countAfter,
			    &lpShadeTop, line, &pf, dc, &lpTop, bo ) )
	    { LDEB(line); rval= -1; goto ready;	}

	line= pds.pdsLineUpto;
	if  ( lpBelow )
	    {
	    docLayoutPushBottomDown( lpBelow, &(pds.pdsShadeBelow) );
	    }

	/*  1  */
if  ( paraNode->biTreeType == DOCinSHPTXT )
    { break;	}

	if  ( DOC_COLUMN_AFTER( &(paraNode->biBelowPosition), &lpTop ) )
	    {
	    int			ret;
	    BufferItem *	bodyNode= paraNode;

	    if  ( paraNode->biTreeType != DOCinBODY )
		{
		bodyNode= docGetBodySectNode( paraNode, bd );
		if  ( ! bodyNode )
		    { XDEB(bodyNode); return -1;	}
		}

	    ret= docDrawToNextColumn( bodyNode, bodyNode,
					    through, &lpTop, &bf, dc );
	    if  ( ret < 0 )
		{
		SLDEB(docLevelStr(paraNode->biLevel),ret);
		rval= -1; goto ready;
		}
	    if  ( ret > 0 )
		{ break;						}

	    docParagraphFrameTwips( &pf, &bf, paraNode );
	    lpShadeTop= lpTop;
	    }
	}

    if  ( lpBelow && line >= paraNode->biParaLineCount )
	{
	docLayoutPushBottomDownShifted( lpBelow, &(paraNode->biBelowPosition),
									bo );
	}

  ready:

    docLayoutCleanBlockFrame( &bf );

    return rval;
    }

