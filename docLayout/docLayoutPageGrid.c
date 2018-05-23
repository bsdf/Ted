/************************************************************************/
/*									*/
/*  Layout of a document.						*/
/*									*/
/************************************************************************/

#   include	"docLayoutConfig.h"

#   include	<stddef.h>

#   include	<docBuf.h>
#   include	"docLayout.h"
#   include	<docPageGrid.h>
#   include	<docTreeType.h>
#   include	<docTreeNode.h>
#   include	<docNodeTree.h>

#   include	<appDebugon.h>

void docLayoutBlockFrame(	BlockFrame *			bf,
				BufferItem *			node,
				const LayoutJob *		lj,
				int				page,
				int				column )
    {
    const LayoutContext *	lc= &(lj->ljContext);
    BufferDocument *		bd= lc->lcDocument;

    docBlockFrameTwips( bf, node, bd, page, column );

    if  ( page == lj->ljBalancePage )
	{
	bf->bfContentRect.drY1= lj->ljBalanceY1;
	bf->bfFlowRect.drY1= lj->ljBalanceY1;
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Continue to lay out the text on a subsequent page.			*/
/*									*/
/*  1)  Continuous sections wrap to the same position as where they	*/
/*	started on the page.						*/
/*									*/
/************************************************************************/

void docLayoutColumnTop(	LayoutPosition *	lpTop,
				BlockFrame *		bf,
				BufferItem *		bodySectNode,
				const LayoutJob *	lj )
    {
    if  ( bodySectNode->biTreeType != DOCinBODY )
	{ SDEB(docTreeTypeStr(bodySectNode->biTreeType));	}

    /* Can actually happen while formatting endnotes
    if  ( lj->ljBodySectNode != bodySectNode )
	{ XXDEB(lj->ljBodySectNode,bodySectNode);	}
    */

    docLayoutBlockFrame( bf, bodySectNode, lj, lpTop->lpPage, lpTop->lpColumn );

    lpTop->lpPageYTwips= bf->bfContentRect.drY0;
    lpTop->lpAtTopOfColumn= 1;

    /*  1  */
    if  ( DOC_SECTnodeBELOW_PREVIOUS( bodySectNode )		&&
	  lpTop->lpPage == bodySectNode->biTopPosition.lpPage	&&
	  bodySectNode->biSectColumnCount > 1			&&
	  lpTop->lpColumn > 0					)
	{
	lpTop->lpPageYTwips= bodySectNode->biTopPosition.lpPageYTwips;
	/* NO: we are at the top for this section lpTop->lpAtTopOfColumn= 0; */
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Skip to the next column. Without newspaper style columns, that is	*/
/*  the next page.							*/
/*									*/
/************************************************************************/

void docLayoutToNextColumn(	LayoutPosition *	lpTop,
				BlockFrame *		bf,
				BufferItem *		node,
				const LayoutJob *	lj )
    {
    const LayoutContext *	lc= &(lj->ljContext);
    const BufferDocument *	bd= lc->lcDocument;
    BufferItem *		bodyNode= bd->bdBody.dtRoot;
    BufferItem *		bodySectNode;

    node= docGetSectNode( node );
    if  ( ! node )
	{ XDEB(node); return;	}

    switch( node->biTreeType )
	{
	SelectionScope *	ss;
	int			extTo;

	case DOCinBODY:
	    bodySectNode= node;
	    break;

	case DOCinAFTNSEP:
	    extTo= bodyNode->biChildCount- 1;
	    bodySectNode= bodyNode->biChildren[extTo];
	    break;

	default:
	    ss= &(node->biSectSelectionScope);
	    extTo= ss->ssOwnerSectNr;
	    if  ( extTo < 0 )
		{
		SLDEB(docTreeTypeStr(node->biTreeType),extTo);
		return;
		}
	    bodySectNode= bodyNode->biChildren[extTo];
	    break;
	}

    lpTop->lpColumn++;
    if  ( lpTop->lpColumn >= bodySectNode->biSectColumnCount )
	{ lpTop->lpPage++; lpTop->lpColumn= 0; }

    if  ( lj->ljBodySectNode != bodySectNode		&&
	  node->biTreeType != DOCinENDNOTE		)
	{
	XXDEB(lj->ljBodySectNode,bodySectNode);
	SDEB(docTreeTypeStr(node->biTreeType));
	}

    docLayoutColumnTop( lpTop, bf, bodySectNode, lj );

    return;
    }

/************************************************************************/
/*									*/
/*  Adjust geometry to position paragraphs in a text frame.		*/
/*									*/
/************************************************************************/

void docLayoutFinishFrame(	const FrameProperties *		fp,
				BlockFrame *			bfTextFrame,
				const BlockFrame *		bfFlow,
				const LayoutJob *		lj,
				const ParagraphLayoutPosition *	plpFlow,
				BufferItem *			cellBi,
				int				paraFrom,
				int				paraUpto )
    {
    BufferItem *		paraBi0= cellBi->biChildren[paraFrom];
    BufferItem *		paraBi1= cellBi->biChildren[paraUpto- 1];

    int				y0= bfTextFrame->bfContentRect.drY0;
    int				y1= paraBi1->biBelowPosition.lpPageYTwips;

    int				frameHeight;
    BlockFrame			bfRedo;

    if  ( ! DOCisFRAME( fp ) )
	{ LDEB(1); return;	}

    if  ( fp->fpHighTwips < 0 )
	{
	y1= y0- fp->fpHighTwips;

	/*
	if  ( paraBi1->biBelowPosition.lpPageYTwips > y1 )
	    { LLDEB(paraBi1->biBelowPosition.lpPageYTwips,y1);	}
	*/
	}

    if  ( fp->fpHighTwips > 0 )
	{ y1= y0+ fp->fpHighTwips;	}

    if  ( paraBi1->biBelowPosition.lpPageYTwips < y1 )
	{ paraBi1->biBelowPosition.lpPageYTwips=  y1;	}

    frameHeight= y1- y0;
    docLayoutInitBlockFrame( &bfRedo );
    docLayoutSetTextFrame( &bfRedo, &(plpFlow->plpPos),
						bfFlow, fp, frameHeight );

    if  ( bfRedo.bfContentRect.drY0		!=
	  bfTextFrame->bfContentRect.drY0	)
	{
	int		yBelow;
	LayoutPosition	lpTop= paraBi0->biTopPosition;

	lpTop.lpPageYTwips= bfRedo.bfContentRect.drY0;

	yBelow= bfRedo.bfContentRect.drY0+ frameHeight;

	docRedoParaStripLayout( lj, bfTextFrame, &lpTop,
						cellBi, paraFrom, paraUpto );

	paraBi1->biBelowPosition.lpPageYTwips= yBelow;
	}

    return;
    }

