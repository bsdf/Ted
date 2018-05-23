/************************************************************************/
/*									*/
/*  Layout of a section.						*/
/*									*/
/************************************************************************/

#   include	"docLayoutConfig.h"

#   include	<stddef.h>

#   include	<docPageGrid.h>
#   include	"docLayout.h"
#   include	<docTreeType.h>
#   include	<docTreeNode.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Adjust the position of the top of a section, depending on the kind	*/
/*  of break								*/
/*									*/
/*  1)  Note that even and odd are swapped as from the users point of	*/
/*	view page numbers count from one.				*/
/*  2)  As this routine is only called for the body of the document,	*/
/*	page wraps can be performed unconditionally.			*/
/*									*/
/************************************************************************/

static void docLayoutPlaceSectTop(	BufferItem *		sectNode,
					LayoutPosition *	lpHere,
					BlockFrame *		bf,
					LayoutJob *		lj )
    {
    int		changedFrame= 0;

    if  ( sectNode->biNumberInParent == 0 )
	{
	lpHere->lpPage= 0;
	lpHere->lpColumn= 0;
	lpHere->lpAtTopOfColumn= 1;

	if  ( lj->ljBodySectNode != sectNode )
	    { XXDEB(lj->ljBodySectNode,sectNode);	}

	docLayoutColumnTop( lpHere, bf, sectNode, lj );
	}
    else{
	if  ( ! DOC_SECTnodeBELOW_PREVIOUS( sectNode ) )
	    {
	    const int		belowText= 0;
	    LayoutPosition	lpBelowNotes;

	    if  ( BF_HAS_FOOTNOTES( bf )				&&
		  ( sectNode->biTreeType == DOCinBODY	||
		    sectNode->biTreeType == DOCinENDNOTE	)	&&
		  docLayoutFootnotesForColumn( &lpBelowNotes, lpHere, bf,
						      belowText, lj )	)
		{ LDEB(1); return;	}

	    docLayoutToNextColumn( lpHere, bf, sectNode, lj );
	    changedFrame= 1;
	    }
	}

    switch( sectNode->biSectBreakKind )
	{
	case DOCibkNONE:
	    if  ( ! DOC_SECTnodeBELOW_PREVIOUS( sectNode ) )
		{ goto pageCase;	}

	    lpHere->lpColumn= 0;

	    docLayoutBlockFrame( bf, sectNode, lj,
					lpHere->lpPage, lpHere->lpColumn );
	    break;
	case DOCibkCOL:
	    break;

	case DOCibkPAGE:
	  pageCase:
	    while( lpHere->lpColumn > 0 )
		{
		docLayoutToNextColumn( lpHere, bf, sectNode, lj );
		changedFrame= 1;
		}
	    break;

	case DOCibkEVEN:	/*  1  */
	    while( lpHere->lpColumn > 0 )
		{
		docLayoutToNextColumn( lpHere, bf, sectNode, lj );
		changedFrame= 1;
		}
	    while( ! ( lpHere->lpPage % 2 ) )
		{
		docLayoutToNextColumn( lpHere, bf, sectNode, lj );
		changedFrame= 1;
		}
	    break;

	case DOCibkODD:	/*  1  */
	    while( lpHere->lpColumn > 0 )
		{
		docLayoutToNextColumn( lpHere, bf, sectNode, lj );
		changedFrame= 1;
		}
	    while( lpHere->lpPage % 2 )
		{
		docLayoutToNextColumn( lpHere, bf, sectNode, lj );
		changedFrame= 1;
		}
	    break;

	default:
	    LDEB(sectNode->biSectBreakKind);
	    break;
	}

    sectNode->biTopPosition= *lpHere;
    if  ( changedFrame )
	{
	docLayoutBlockFrame( bf, sectNode, lj,
					lpHere->lpPage, lpHere->lpColumn );

	lpHere->lpPageYTwips= bf->bfContentRect.drY0;
	lpHere->lpAtTopOfColumn= 1;
	sectNode->biTopPosition= *lpHere;
	}

    if  ( sectNode->biNumberInParent == 0		&&
	  sectNode->biParent			)
	{ sectNode->biParent->biTopPosition= *lpHere; }

    return;
    }

/************************************************************************/
/*									*/
/*  Do the layout of the children of a section.				*/
/*									*/
/*  2)  See whether and where to restart if a row does not fit in the	*/
/*	column.								*/
/*  3)  Between tables.							*/
/*  4)  Tables.								*/
/*  5)  Finally do the layout of the end notes belonging to this	*/
/*	section. Though strictly spoken notes are not children, they	*/
/*	appear at the end of the section and they are part of the	*/
/*	balancing if multi column sections.				*/
/*									*/
/************************************************************************/

int docLayoutSectChildren(	LayoutPosition *	lpBelow,
				const LayoutPosition *	lpTop,
				BufferItem *		sectNode,
				int			from,
				BlockFrame *		bf,
				LayoutJob *		lj )
    {
    const LayoutContext *	lc= &(lj->ljContext);
    int				i;

    LayoutPosition		lpHere= *lpTop;

    /*  2  */
    i= from;
    while( i > 0 )
	{
	BufferItem *	childNode= sectNode->biChildren[--i];

	if  ( ! docIsRowNode( childNode ) )
	    { break;	}
	if  ( ! childNode->biRow_Keepfollow )
	    { break;	}
	}

    i= from;
    while( i < sectNode->biChildCount )
	{
	/*  3  */
	while( i < sectNode->biChildCount )
	    {
	    BufferItem *	childNode= sectNode->biChildren[i];

	    if  ( docIsRowNode( childNode ) )
		{ break;	}

	    if  ( docLayoutNodeImplementation( &lpHere, &lpHere,
							childNode, bf, lj ) )
		{ LDEB(i); return -1;	}

	    i++;
	    }

	/*  4  */
	if  ( i < sectNode->biChildCount )
	    {
	    int			upto= i+ 1;

	    while( upto < sectNode->biChildCount )
		{
		BufferItem *	childNode= sectNode->biChildren[upto];

		if  ( ! docIsRowNode( childNode ) )
		    { break;	}

		upto++;
		}

	    if  ( docLayoutTableSlice( &lpHere, &lpHere,
						sectNode, i, upto, bf, lj ) )
		{ LLDEB(i,upto); return -1;	}

	    docLayoutFinishNodeLayout( (int *)0,
				sectNode->biChildren[upto- 1], lj, &lpHere );

	    i= upto;
	    }
	}

    /*  5  */
    {
    const DocumentProperties *	dp= &(lc->lcDocument->bdProperties);
    const NotesProperties *	npEndnotes= &(dp->dpNotesProps.fepEndnotesProps);

    if  ( sectNode->biTreeType == DOCinBODY		&&
	  npEndnotes->npPlacement == FTNplaceSECT_END	)
	{
	if  ( docLayoutEndnotesForSection( &lpHere, &lpHere,
					sectNode->biNumberInParent, bf, lj ) )
	    { LDEB(sectNode->biNumberInParent); return -1;	}
	}
    }

    *lpBelow= lpHere;
    return 0;
    }

/************************************************************************/
/*									*/
/*  Balance the columns on the last page of a multi-column section.	*/
/*									*/
/*  The approach:							*/
/*  -------------							*/
/*									*/
/*  A)  Calculate the total surface of that the text of the section	*/
/*	covers on the last page of the section.				*/
/*  B)  Calculate the height that the section would have if it were	*/
/*	perfectly balanced. Realize that various formatting properties	*/
/*	of the text might make a perfect balance impossible.		*/
/*  C)  Use the value calculated in (B) as a first guess and use an	*/
/*	iterative algorithm to find the minimal height of the page	*/
/*	strip occupied by this section. The fact that the value from	*/
/*	(B) is irrealistic helps: We can use binary search to find the	*/
/*	correct value.							*/
/*									*/
/*  1)  MS-Word 2003 does not try to balance the columns of a section	*/
/*	that starts after the first column on its last page.		*/
/*									*/
/************************************************************************/

static int docBalanceSectionColumns(	LayoutPosition *	lpBelow,
					BlockFrame *		bf,
					BufferItem *		sectNode,
					const LayoutJob *	refLj )
    {
    int				rval= 0;
    const LayoutContext *	lc= &(refLj->ljContext);
    BufferDocument *		bd= lc->lcDocument;
    int				l, m, r;

    int				fromSectTop;
    int				pageY0;
    int				col0;
    int				from;

    int				col;

    BufferItem *		childNode;

    BlockFrame			bfFrom;
    LayoutPosition		lpFrom;
    DocumentPosition		dpFrom;

    LayoutPosition		lpHere;
    LayoutJob			balanceLj;

    const int			partFrom= 0;
    const int			columnFrom= 0;

    BlockFrame			bfCol;

    docLayoutInitBlockFrame( &bfFrom );
    docLayoutInitBlockFrame( &bfCol );

    if  ( sectNode->biTopPosition.lpPage < lpBelow->lpPage )
	{
	col0= 0;
	fromSectTop= 0;
	}
    else{
	if  ( sectNode->biTopPosition.lpPage > lpBelow->lpPage )
	    { LLDEB(sectNode->biTopPosition.lpPage,lpBelow->lpPage);	}

	col0= sectNode->biTopPosition.lpColumn;
	fromSectTop= 1;
	}

    /*  1  */
    if  ( col0 > 0 )
	{ goto ready;	}

    col= lpBelow->lpColumn;
    docBlockFrameTwips( &bfCol, sectNode, bd, lpBelow->lpPage, col );

    if  ( fromSectTop )
	{ pageY0= sectNode->biTopPosition.lpPageYTwips;	}
    else{ pageY0= bfCol.bfContentRect.drY0;		}

    childNode= (BufferItem *)0;
    for ( from= 0; from < sectNode->biChildCount; from++ )
	{
	childNode= sectNode->biChildren[from];

	if  ( childNode->biBelowPosition.lpPage >= lpBelow->lpPage )
	    { break;	}
	}
    if  ( from >= sectNode->biChildCount || ! childNode )
	{ LLDEB(sectNode->biChildCount,lpBelow->lpPage); rval= -1; goto ready; }

    lpFrom= childNode->biTopPosition;
    balanceLj= *refLj;
    balanceLj.ljBodySectNode= sectNode;
    balanceLj.ljBalancePage= lpBelow->lpPage;

    l= 240;
    r= bf->bfPageGeometry.dgPageHighTwips- pageY0-
				    bf->bfPageGeometry.dgBottomMarginTwips;
    m= ( l+ r )/ 2;

    bfFrom= *bf;

    if  ( docHeadPosition( &dpFrom, childNode ) )
	{ LDEB(1); rval= -1; goto ready;	}

    balanceLj.ljBalanceY1= pageY0+ m;
    docLayoutBlockFrame( &bfFrom, sectNode, &balanceLj,
					lpFrom.lpPage, lpFrom.lpColumn );

    if  ( docCollectFootnotesFromColumn( &bfFrom, &dpFrom, partFrom, bd,
						lpFrom.lpPage, columnFrom ) )
	{ LLDEB(lpFrom.lpPage,columnFrom); rval= -1; goto ready;	}

    for (;;)
	{
	balanceLj.ljBalanceY1= pageY0+ m;
	lpHere= lpFrom;
	*bf= bfFrom;
	docLayoutBlockFrame( bf, sectNode, &balanceLj,
					lpHere.lpPage, lpHere.lpColumn );
	if  ( docLayoutSectChildren( &lpHere, &lpHere, sectNode, from,
							bf, &balanceLj ) )
	    { LDEB(1); rval= -1; goto ready;	}

	if  ( lpHere.lpPage > lpBelow->lpPage )
	    {
	    l= m+ 1;
	    m= ( l+ r )/ 2;
	    continue;
	    }
	else{
	    r= m; m= ( l+ r )/ 2;
	    if  ( l == m )
		{ break;	}
	    }
	}

    lpHere.lpPageYTwips= balanceLj.ljBalanceY1;

    *lpBelow= lpHere;

  ready:
    docLayoutCleanBlockFrame( &bfCol );
    docLayoutCleanBlockFrame( &bfFrom );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Are the columns of a section node to be balanced?			*/
/*									*/
/************************************************************************/

int docSectColumnsAreBalanced(	const BufferItem *		sectNode )
    {
    const SectionProperties *	sp= &(sectNode->biSectProperties);
    int				nrInParent= sectNode->biNumberInParent;

    if  ( sp->spColumnCount > 1					&&
	  sectNode->biParent					&&
	  nrInParent < sectNode->biParent->biChildCount- 1	)
	{
	const BufferItem *	nextSectNode;

	nextSectNode= sectNode->biParent->biChildren[nrInParent+ 1];

	if  ( DOC_SECTnodeBELOW_PREVIOUS( nextSectNode ) )
	    { return 1;	}
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Last step in the layout of a section: Endnotes and balancing.	*/
/*									*/
/************************************************************************/

int docLayoutFinishSectNode(	LayoutPosition *		lpBelow,
				const LayoutPosition *		lpTop,
				BufferItem *			sectNode,
				BlockFrame *			bf,
				LayoutJob *			lj )
    {
    LayoutPosition		lpHere= *lpTop;

    if  ( docSectColumnsAreBalanced( sectNode ) )
	{
	if  ( docBalanceSectionColumns( &lpHere, bf, sectNode, lj ) )
	    { LDEB(1); return -1;	}
	}

    *lpBelow= lpHere;
    return 0;
    }

/************************************************************************/
/*									*/
/*  Do the layout of a section.						*/
/*									*/
/************************************************************************/

int docLayoutSectNode(	LayoutPosition *		lpBelow,
			const LayoutPosition *		lpTop,
			BufferItem *			sectNode,
			BlockFrame *			bf,
			LayoutJob *			lj )
    {
    LayoutPosition		lpHere= *lpTop;
    const int			recursively= 0;

    if  ( sectNode->biTreeType != DOCinBODY )
	{ SDEB(docTreeTypeStr(sectNode->biTreeType));	}

    /**/
    docDelimitTables( sectNode, recursively );

    /**/

    if  ( sectNode->biTreeType == DOCinBODY		&&
	  docSectHeaderFooterPrelayout( sectNode, lj )	)
	{ LDEB(1); return -1;	}

    /**/
    if  ( docSectNotesPrelayout( sectNode->biNumberInParent, sectNode, lj ) )
	{ LDEB(sectNode->biNumberInParent); return -1;	}

    /**/

    if  ( sectNode->biTreeType == DOCinBODY )
	{ lj->ljBodySectNode= sectNode;	}

    docLayoutPlaceSectTop( sectNode, &lpHere, bf, lj );

    if  ( docLayoutSectChildren( &lpHere, &lpHere, sectNode, 0, bf, lj ) )
	{ LDEB(1); return -1;	}

    if  ( sectNode->biChildCount > 0 )
	{ sectNode->biTopPosition=  sectNode->biChildren[0]->biTopPosition; }

    /**/

    if  ( docLayoutFinishSectNode( &lpHere, &lpHere, sectNode, bf, lj ) )
	{ LDEB(1); return -1;	}

    *lpBelow= lpHere;
    return 0;
    }

