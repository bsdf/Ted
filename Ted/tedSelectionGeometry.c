/************************************************************************/
/*									*/
/*  Geometry calculations when looking for a position in the document.	*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<limits.h>

#   include	"tedApp.h"
#   include	"tedLayout.h"
#   include	<docScreenLayout.h>
#   include	<docPageGrid.h>
#   include	<docDebug.h>
#   include	<docParaParticules.h>
#   include	<docTreeType.h>
#   include	<geoGrid.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Return the X position for an I bar.					*/
/*									*/
/************************************************************************/

static int tedCalculateX(	const BufferItem *	paraNode,
				const TextLine *	tl,
				const TextParticule *	tp,
				int			part,
				int			x0FramePixels,
				const LayoutContext *	lc,
				int			stroff )
    {
    BufferDocument *		bd= lc->lcDocument;
    double			xfac= lc->lcPixelsPerTwip;
    int				len;
    int				docXPixels;

    TextAttribute		ta;
    int				textAttrNr;
    int				screenFont;

    int				x0Pixels= x0FramePixels+ tp->tpXContentXPixels;
    int				pixelsWide= COORDtoGRID( xfac, tp->tpTwipsWide );

    switch( tp->tpKind )
	{
	case DOCkindOBJECT:
	case DOCkindTAB:
	case DOCkindCHFTNSEP:
	case DOCkindCHFTNSEPC:
	    len= stroff- tp->tpStroff;

	    if  ( len == 0 )
		{ docXPixels= x0Pixels;	}
	    else{
		if  ( len == 1 )
		    { docXPixels= x0Pixels+ pixelsWide;	}
		else{
		    LLDEB(stroff,tp->tpStroff);
		    docListParticule( 0, "?-?", tp-paraNode->biParaParticules,
								    paraNode, tp );
		    docListNode( 0, paraNode, 1 );
		    docXPixels= x0Pixels;
		    }
		}
	    return docXPixels;

	case DOCkindSPAN:
	    textAttrNr= docGetEffectiveTextAttributes( &ta, bd,
							    paraNode, part );

	    screenFont= utilIndexMappingGet( lc->lcAttributeToScreenFont,
								textAttrNr );
	    if  ( screenFont < 0 )
		{ LLDEB(textAttrNr,screenFont); return x0Pixels;	}

	    len= stroff- tp->tpStroff;

	    if  ( len == tp->tpStrlen					&&
		  part+ 1 < tl->tlFirstParticule+ tl->tlParticuleCount	)
		{
		docXPixels= x0FramePixels+ tp[1].tpXContentXPixels;
		}
	    else{
		const char *	s= (char *)docParaString( paraNode, tp->tpStroff );

		docXPixels= x0Pixels+ docScreenTextWidth( screenFont, &ta,
							    lc, s, len );
		}

	    return docXPixels;

	case DOCkindFIELDHEAD:
	case DOCkindFIELDTAIL:
	case DOCkindLINEBREAK:
	case DOCkindPAGEBREAK:
	case DOCkindCOLUMNBREAK:
	case DOCkindOPT_HYPH:
	case DOCkindLTR_MARK:
	case DOCkindRTL_MARK:
	    docXPixels= x0Pixels;
	    return docXPixels;

	default:
	    LDEB(tp->tpKind);
	    docXPixels= x0Pixels;
	    return docXPixels;
	}
    }

/************************************************************************/
/*									*/
/*  Calculate the smallest rectangle that contains the selection.	*/
/*									*/
/*  1)  Same paragraph?							*/
/*  1b) For a single position, widen to contain the whole I-Bar.	*/
/*  2)  Same table row?							*/
/*  3)  Expose the whole cell.						*/
/*									*/
/************************************************************************/

#   define	IW		5

static void tedSelectionRectangle(
				SelectionGeometry *		sg,
				const LayoutContext *		lc,
				const DocumentSelection *	ds,
				const BufferItem *		bodySectNode )
    {
    DocumentRectangle *		drSel= &(sg->sgRectangle);

    BlockFrame			bf;
    ParagraphFrame		pf;
    DocumentRectangle *		drParaContent= &(pf.pfParaContentRect);

    const TextLine *		tlBegin;
    const TextLine *		tlEnd;

    docLayoutInitBlockFrame( &bf );

    if  ( sg->sgHead.pgLine < 0					||
	  sg->sgHead.pgLine >= ds->dsHead.dpNode->biParaLineCount	||
	  sg->sgTail.pgLine < 0						||
	  sg->sgTail.pgLine >= ds->dsTail.dpNode->biParaLineCount	)
	{
	LLDEB(sg->sgTail.pgLine,ds->dsTail.dpNode->biParaLineCount);
	LLDEB(sg->sgHead.pgLine,ds->dsHead.dpNode->biParaLineCount);
	goto ready;
	}

    tlBegin= ds->dsHead.dpNode->biParaLines+ sg->sgHead.pgLine;
    tlEnd= ds->dsTail.dpNode->biParaLines+ sg->sgTail.pgLine;

    docPixelRectangleForPositions( drSel, &(sg->sgHead), &(sg->sgTail), lc );

    if  ( tlBegin->tlTopPosition.lpPage != tlEnd->tlTopPosition.lpPage ||
	  tlBegin->tlTopPosition.lpColumn != tlEnd->tlTopPosition.lpColumn )
	{
	LayoutPosition		lp;
	DocumentRectangle	dr;

	int			useColumns;

	useColumns= docTreeInColumnType( ds->dsHead.dpNode->biTreeType );

	drSel->drX0= 0;
	drSel->drX1= INT_MAX;

	if  ( useColumns						&&
	      tlBegin->tlTopPosition.lpColumn < bodySectNode->biSectColumnCount- 1 )
	    {
	    docParaBlockFrameRectangle( &dr, ds->dsHead.dpNode,
					    lc->lcDocument,
					    tlBegin->tlTopPosition.lpPage,
					    tlBegin->tlTopPosition.lpColumn );
	    lp= tlBegin->tlTopPosition;
	    lp.lpPageYTwips= dr.drY1;
	    lp.lpAtTopOfColumn= 0;

	    if  ( drSel->drY1 < docLayoutYPixels( lc, &lp ) )
		{ drSel->drY1=  docLayoutYPixels( lc, &lp );	}
	    }

	if  ( useColumns						&&
	      tlEnd->tlTopPosition.lpColumn > 0				)
	    {
	    docParaBlockFrameRectangle( &dr, ds->dsTail.dpNode,
					    lc->lcDocument,
					    tlEnd->tlTopPosition.lpPage,
					    tlEnd->tlTopPosition.lpColumn );
	    lp= tlEnd->tlTopPosition;
	    lp.lpPageYTwips= dr.drY0;
	    lp.lpAtTopOfColumn= 1;

	    if  ( drSel->drY0 > docLayoutYPixels( lc, &lp ) )
		{ drSel->drY0=  docLayoutYPixels( lc, &lp );	}
	    }

	goto ready;
	}

    /*  1  */
    if  ( ds->dsTail.dpNode == ds->dsHead.dpNode )
	{
	BufferItem *	paraNode= ds->dsHead.dpNode;

	if  ( sg->sgTail.pgLine == sg->sgHead.pgLine )
	    {
	    /*  1b  */
	    if  ( ds->dsTail.dpStroff == ds->dsHead.dpStroff )
		{
		drSel->drX0= sg->sgHead.pgXPixels- IW;
		drSel->drX1= sg->sgTail.pgXPixels+ IW;
		}
	    else{
		drSel->drX0= sg->sgHead.pgXPixels;
		drSel->drX1= sg->sgTail.pgXPixels;
		}

	    goto ready;
	    }

	docBlockFrameTwips( &bf, paraNode,
				    lc->lcDocument,
				    paraNode->biTopPosition.lpPage,
				    paraNode->biTopPosition.lpColumn );

	docParagraphFrameTwips( &pf, &bf, paraNode );

	if  ( paraNode->biParaFirstIndentTwips < 0 )
	    { drSel->drX0= docLayoutXPixels( lc, drParaContent->drX0+
				    paraNode->biParaFirstIndentTwips );	}
	else{ drSel->drX0= docLayoutXPixels( lc, drParaContent->drX0 );	}
	drSel->drX1= docLayoutXPixels( lc, drParaContent->drX1 );

	goto ready;
	}

    /*  2  */
    if  ( ds->dsHead.dpNode->biParaTableNesting == 0	||
	  ds->dsTail.dpNode->biParaTableNesting == 0	||
	  ! docSelectionInsideRow( ds )			)
	{
	BufferItem *	parentNode;

	parentNode= docGetCommonParent( ds->dsHead.dpNode, ds->dsTail.dpNode );
	if  ( ! parentNode )
	    { XDEB(parentNode);	}
	else{
	    DocumentRectangle	parentRect;

	    docNodeRectangle( &parentRect, parentNode, lc,
					(const BlockOrigin *)0 );

	    drSel->drX0= parentRect.drX0;
	    drSel->drX1= parentRect.drX1;
	    }
	}

    /*  3  */
    if  ( ds->dsHead.dpNode->biParaTableNesting > 0 )
	{
	BufferItem *		paraNode= ds->dsHead.dpNode;
	DocumentRectangle	dr;

	docBlockFrameTwips( &bf, paraNode,
				    lc->lcDocument,
				    paraNode->biTopPosition.lpPage,
				    paraNode->biTopPosition.lpColumn );

	docParagraphFrameTwips( &pf, &bf, paraNode );

	if  ( paraNode->biParaFirstIndentTwips < 0 )
	    { dr.drX0= docLayoutXPixels( lc, drParaContent->drX0+
				    paraNode->biParaFirstIndentTwips );	}
	else{ dr.drX0= docLayoutXPixels( lc, drParaContent->drX0 );	}
	dr.drX1= docLayoutXPixels( lc, drParaContent->drX1 );

	if  ( docSelectionInsideCell( ds ) )
	    {
	    dr.drY0= docLayoutYPixels( lc, &(paraNode->biTopPosition) );
	    dr.drY1= docLayoutYPixels( lc, &(paraNode->biBelowPosition) );
	    }
	else{
	    BufferItem *	rowBi= paraNode->biParent->biParent;

	    dr.drY0= docLayoutYPixels( lc, &(rowBi->biTopPosition) );
	    dr.drY1= docLayoutYPixels( lc, &(rowBi->biBelowPosition) );
	    }

	geoUnionRectangle( drSel, drSel, &dr );
	}

    /*  3  */
    if  ( ds->dsTail.dpNode->biParaTableNesting > 0 )
	{
	BufferItem *		paraNode= ds->dsTail.dpNode;
	DocumentRectangle	dr;

	docBlockFrameTwips( &bf, paraNode,
				    lc->lcDocument,
				    paraNode->biTopPosition.lpPage,
				    paraNode->biTopPosition.lpColumn );

	docParagraphFrameTwips( &pf, &bf, paraNode );

	if  ( paraNode->biParaFirstIndentTwips < 0 )
	    { dr.drX0= docLayoutXPixels( lc, drParaContent->drX0+
				    paraNode->biParaFirstIndentTwips );	}
	else{ dr.drX0= docLayoutXPixels( lc, drParaContent->drX0 );		}
	dr.drX1= docLayoutXPixels( lc, drParaContent->drX1 );

	if  ( docSelectionInsideCell( ds ) )
	    {
	    dr.drY0= docLayoutYPixels( lc, &(paraNode->biTopPosition) );
	    dr.drY1= docLayoutYPixels( lc, &(paraNode->biBelowPosition) );
	    }
	else{
	    BufferItem *	rowBi= paraNode->biParent->biParent;

	    dr.drY0= docLayoutYPixels( lc, &(rowBi->biTopPosition) );
	    dr.drY1= docLayoutYPixels( lc, &(rowBi->biBelowPosition) );
	    }

	geoUnionRectangle( drSel, drSel, &dr );
	}

  ready:

    docLayoutCleanBlockFrame( &bf );

    return;
    }

/************************************************************************/
/*									*/
/*  Determine the poition on paper/screen of a certain position in the	*/
/*  document.								*/
/*									*/
/************************************************************************/

void tedPositionGeometry(	PositionGeometry *		pg,
				const DocumentPosition *	dp,
				int				lastOne,
				const LayoutContext *		lc )
    {
    int			part;
    int			line;

    TextLine *		tl;
    TextParticule *	tp;

    BlockFrame		bf;
    ParagraphFrame	pf;

    int			x0FramePixels;

    int			partFlags= 0;
    int			lineFlags= 0;

    docLayoutInitBlockFrame( &bf );

    if  ( docFindParticuleOfPosition( &part, &partFlags, dp, lastOne ) )
	{ LDEB(dp->dpStroff); goto ready;	}

    if  ( docFindLineOfPosition( &line, &lineFlags, dp, lastOne ) )
	{ LDEB(dp->dpStroff); goto ready;	}

    tp= dp->dpNode->biParaParticules+ part;
    tl= dp->dpNode->biParaLines+ line;

    if  ( dp->dpStroff == tl->tlStroff+ tl->tlStrlen	&&
	  part < dp->dpNode->biParaParticuleCount- 1	&&
	  line < dp->dpNode->biParaLineCount- 1		)
	{
	if  ( tp->tpKind == DOCkindLINEBREAK	||
	      tp->tpKind == DOCkindPAGEBREAK	||
	      tp->tpKind == DOCkindCOLUMNBREAK	)
	    { line++; tl++; part++; tp++;	}
	}

    docBlockFrameTwips( &bf, dp->dpNode, lc->lcDocument,
					tl->tlTopPosition.lpPage,
					tl->tlTopPosition.lpColumn );
    docParagraphFrameTwips( &pf, &bf, dp->dpNode );

    pg->pgParaFrameX0= pf.pfCellContentRect.drX0;
    pg->pgParaFrameX1= pf.pfCellContentRect.drX1;
    pg->pgBlockFrameX0= bf.bfContentRect.drX0;
    pg->pgBlockFrameX1= bf.bfContentRect.drX1;

    pg->pgLine= line;
    pg->pgPositionFlags= partFlags | lineFlags;

    x0FramePixels= docLayoutXPixels( lc, pf.pfParaContentRect.drX0 );

    pg->pgXPixels= tedCalculateX( dp->dpNode, tl, tp, part,
					    x0FramePixels, lc, dp->dpStroff );
    pg->pgTopPosition= tl->tlTopPosition;
    pg->pgBasePosition= tl->tlTopPosition;
    pg->pgBottomPosition= tl->tlTopPosition;

    pg->pgBasePosition.lpPageYTwips -= tl->tlAscY0;
    pg->pgBottomPosition.lpPageYTwips += tl->tlLineStride;

#   if 0
    Gives silly high I Bar.
    if  ( sg->sgLine == bi->biParaLineCount- 1 )
	{ sg->sgY1 += bi->biParaSpaceAfterPixels; }
#   endif

  ready:
    docLayoutCleanBlockFrame( &bf );

    return;
    }

void tedSelectionGeometry(	SelectionGeometry *		sg,
				const DocumentSelection *	ds,
				const BufferItem *		bodySectNode,
				int				lastLine,
				const LayoutContext *		lc )
    {
    int		lastOneHead= PARAfindFIRST;
    int		lastOneTail= PARAfindFIRST;

    if  ( ds->dsDirection != 0 )
	{ lastLine= 0;	}

    if  ( ds->dsDirection > 0 || lastLine )
	{ lastOneHead= PARAfindLAST;	}
    if  ( lastLine )
	{ lastOneTail= PARAfindLAST;	}

    tedPositionGeometry( &(sg->sgHead), &(ds->dsHead), lastOneHead, lc );
    tedPositionGeometry( &(sg->sgAnchor), &(ds->dsAnchor), lastOneHead, lc );
    tedPositionGeometry( &(sg->sgTail), &(ds->dsTail), lastOneTail, lc );

    tedSelectionRectangle( sg, lc, ds, bodySectNode );

    return;
    }

