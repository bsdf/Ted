/************************************************************************/
/*									*/
/*  Layout of a document. Layout of a series of paragraphs in a common	*/
/*  parent.								*/
/*									*/
/************************************************************************/

#   include	"docLayoutConfig.h"

#   include	<stddef.h>

#   include	"docLayout.h"
#   include	<docTreeNode.h>
#   include	<docTextLine.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Layout as much of a paragraph as fits on the current page		*/
/*  page.								*/
/*									*/
/*  2)  When Widow/Orphan control is active and we could only place	*/
/*	one line on the page without finishing the paragraph. Force	*/
/*	the formatting of this paragraph to restart on the next page.	*/
/*  3)  When the paragraph is to be kept on one page, and it is not	*/
/*	possible to finish it on this page, Force the formatting of	*/
/*	paragraph to restart on the next page.				*/
/*  4)  When Widow/Orphan control is active and we finish the paragraph	*/
/*	by placing its last line on this page, skip back to the		*/
/*	penulimate line.						*/
/*  5)	NOTE the special case for 3 line paragraphs: The whole		*/
/*	paragraph is moved to the next page.				*/
/*									*/
/*  Return FORMATstopBLOCK_FULL						*/
/*	Nothing done because the paragraph does not fit on the page	*/
/*	This includes the cases with explicit page breaks.		*/
/*  Return FORMATstopPARTIAL						*/
/*	Starting point rewound because of widow/orphan control.		*/
/*  Return FORMATstopREADY						*/
/*	Paragraph successfully finished.				*/
/*  Return -1								*/
/*	Error.								*/
/*									*/
/************************************************************************/

int docLayoutParagraphInStrip(	int *				pStopCode,
				ParagraphLayoutPosition *	plp,
				BlockFrame *			bf,
				const LayoutJob *		lj,
				int				cellTopInset,
				BufferItem *			paraNode )
    {
    const LayoutContext *	lc= &(lj->ljContext);
    int				stopCode= FORMATstopREADY;
    int				accepted;
    int				prevLine;
    LayoutPosition		lpBefore= plp->plpPos;
    const int			isRedo= 0;

    int				changed= 0;

    if  ( plp->pspPart == 0 )
	{
	int		xStopCode= FORMATstopREADY;

	/*  1  */
	if  ( docLayoutStartParagraph( lj, &xStopCode, paraNode, bf, plp ) )
	    { LDEB(1); return -1;	}

	if  ( xStopCode != FORMATstopREADY )
	    { *pStopCode= xStopCode; return xStopCode; }

	paraNode->biTopPosition= plp->plpPos;

	if  ( plp->pspChild == 0 )
	    { plp->plpPos.lpPageYTwips += cellTopInset;	}
	}

    prevLine= plp->pspLine;

    accepted= docLayoutParaLines( &stopCode, isRedo,
				    &(plp->plpParagraphFrame),
				    &(plp->plpPos), &(plp->pspLine),
				    bf, lj, paraNode, plp->pspPart );
    if  ( accepted < 0 )
	{ LDEB(accepted); return -1;	}

    /*  2  */
    if  ( paraNode->biParaWidowControl					&&
	  stopCode == FORMATstopBLOCK_FULL				&&
	  ! lpBefore.lpAtTopOfColumn					&&
	  plp->pspLine == 1						&&
	  ! ( paraNode->biParaLines[0].tlFlags & TLflagBLOCKBREAK )	)
	{
	docStripLayoutStartChild( plp, plp->pspChild );
	*pStopCode= stopCode; /* FORMATstopBLOCK_FULL */
	return FORMATstopBLOCK_FULL;
	}

    /*  3  */
    if  ( ( paraNode->biParaKeepOnPage	||
	    paraNode->biParaKeepWithNext	)			&&
	  ! lpBefore.lpAtTopOfColumn					&&
	  stopCode == FORMATstopBLOCK_FULL				&&
	  paraNode->biParaLineCount >= plp->pspLine			&&
	  plp->pspLine > 0						&&
	  ! ( paraNode->biParaLines[plp->pspLine- 1].tlFlags &
						TLflagBLOCKBREAK )	)
	{
	docStripLayoutStartChild( plp, plp->pspChild );
	*pStopCode= stopCode; /* FORMATstopBLOCK_FULL */
	return FORMATstopBLOCK_FULL;
	}

    plp->pspPart += accepted;
    if  ( accepted > 0 )
	{ plp->pspChildAdvanced= 1;	}

    if  ( stopCode != FORMATstopREADY				&&
	  ( accepted > 0 || ! lpBefore.lpAtTopOfColumn )	)
	{
	*pStopCode= stopCode; return FORMATstopBLOCK_FULL;
	}

    /*  4  */
    if  ( paraNode->biParaWidowControl				&&
	  plp->pspLine > 1					&&
	  plp->pspLine- prevLine == 1				)
	{
	TextLine *		tl= paraNode->biParaLines+ prevLine- 1;

	if  ( ! ( tl[0].tlFlags & TLflagBLOCKBREAK )			&&
	      ! tl[0].tlTopPosition.lpAtTopOfColumn			&&
	      tl[0].tlTopPosition.lpPage < tl[1].tlTopPosition.lpPage	)
	    {
	    /*  5  */
	    if  ( paraNode->biParaLineCount == 3 )
		{
		BufferItem *	node= paraNode;

		docStripLayoutStartChild( plp, plp->pspChild );
		plp->plpPos= tl[1].tlTopPosition;

		while( node->biNumberInParent > 0 )
		    {
		    node= node->biParent->biChildren[node->biNumberInParent-1];
		    if  ( ! node->biParaKeepWithNext )
			{ *pStopCode= FORMATstopBLOCK_FULL; break; }

		    plp->pspChild--;
		    }
		}
	    else{
		plp->pspLine= prevLine- 1;
		plp->pspPart= tl->tlFirstParticule;
		plp->plpPos= tl[1].tlTopPosition;

		plp->pspChildAdvanced= ( plp->pspPart > 0 );
		}

	    return FORMATstopPARTIAL;
	    }
	}

    docLayoutSetNodeBottom( &changed, paraNode, &(plp->plpPos),
				    lc, lj->ljChangedRectanglePixels );

    docStripLayoutNextChild( plp );
    return FORMATstopREADY;
    }

