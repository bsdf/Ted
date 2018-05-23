/************************************************************************/
/*									*/
/*  Layout of a document.						*/
/*									*/
/************************************************************************/

#   include	"docLayoutConfig.h"

#   include	<stddef.h>

#   include	"docLayout.h"
#   include	<docDebug.h>
#   include	<docTreeType.h>
#   include	<docTreeNode.h>
#   include	<docNodeTree.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  layout a paragraph or a cell: Both are shortcuts to			*/
/*  docLayoutStackedStrip(): trkeep and widow/orphan control makes the	*/
/*  formatting of paragraphs dependent on eachother.			*/
/*									*/
/************************************************************************/

static int docLayoutParaNode(	LayoutPosition *	lpBelow,
				const LayoutPosition *	lpTop,
				BufferItem *		paraBi,
				BlockFrame *		bf,
				LayoutJob *		lj )
    {
    int				rval= 0;
    ParagraphLayoutJob		plj;
    const int			line= 0;
    const int			part= 0;

    docInitParagraphLayoutJob( &plj );

    docBeginParagraphLayoutProgress( &plj,
			paraBi->biNumberInParent, line, part,
			paraBi->biNumberInParent+ 1,
			lpTop );

    if  ( docLayoutStackedStrip( paraBi->biParent, bf, lj, &plj ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( paraBi->biParaLineCount < 1 )
	{
	LDEB(paraBi->biParaLineCount); docListNode(0,paraBi,0);
	rval= -1; goto ready;
	}

    *lpBelow= plj.pljPos.plpPos;

  ready:

    docCleanParagraphLayoutJob( &plj );

    return rval;
    }

int docLayoutCellNode(	LayoutPosition *	lpBelow,
			const LayoutPosition *	lpTop,
			BufferItem *		cellNode,
			int			from,
			BlockFrame *		bf,
			LayoutJob *		lj )
    {
    int				rval= 0;
    ParagraphLayoutJob		plj;
    const int			line= 0;
    const int			part= 0;

    docInitParagraphLayoutJob( &plj );

    docBeginParagraphLayoutProgress( &plj,
				from, line, part, cellNode->biChildCount,
				lpTop );

    if  ( docLayoutStackedStrip( cellNode, bf, lj, &plj ) )
	{ LDEB(1); rval= -1; goto ready;	}

    *lpBelow= plj.pljPos.plpPos;

  ready:
    docCleanParagraphLayoutJob( &plj );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Redo the layout of a section because one of its external items	*/
/*  changed size.							*/
/*									*/
/************************************************************************/

static void docItemLayoutStartPosition(	LayoutPosition *	lp,
					const BufferItem *	node )
    {
    if  ( node->biNumberInParent == 0 )
	{
	if  ( node->biParent )
	    { *lp= node->biParent->biTopPosition;	}
	else{
	    if  ( node->biTreeType == DOCinBODY )
		{ docInitLayoutPosition( lp );		}
	    else{ *lp= node->biTopPosition;		}
	    }
	}
    else{
	const BufferItem *	prevBi;

	prevBi= node->biParent->biChildren[node->biNumberInParent- 1];

	*lp= prevBi->biBelowPosition;
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Do the layout of the document root item.				*/
/*									*/
/************************************************************************/

static int docLayoutBodyNode(		LayoutPosition *	lpBelow,
					const LayoutPosition *	lpTop,
					BufferItem *		docNode,
					BlockFrame *		bf,
					LayoutJob *		lj )
    {
    const LayoutContext *	lc= &(lj->ljContext);
    BufferDocument *		bd= lc->lcDocument;
    const DocumentProperties *	dp= &(bd->bdProperties);
    const NotesProperties *	npEndnotes= &(dp->dpNotesProps.fepEndnotesProps);

    LayoutPosition		lpHere= *lpTop;

    int				i;

    if  ( docNode->biChildCount > 0 )
	{
	lj->ljBodySectNode= docNode->biChildren[0];

	if  ( docTreePrelayout( &(bd->bdEiFtnsep), lj->ljBodySectNode, lj ) )
	    { LDEB(1); return -1;	}
	if  ( docTreePrelayout( &(bd->bdEiFtnsepc), lj->ljBodySectNode, lj ) )
	    { LDEB(1); return -1;	}
	if  ( docTreePrelayout( &(bd->bdEiFtncn), lj->ljBodySectNode, lj ) )
	    { LDEB(1); return -1;	}

	if  ( docTreePrelayout( &(bd->bdEiAftnsep), lj->ljBodySectNode, lj ) )
	    { LDEB(1); return -1;	}
	if  ( docTreePrelayout( &(bd->bdEiAftnsepc), lj->ljBodySectNode, lj ) )
	    { LDEB(1); return -1;	}
	if  ( docTreePrelayout( &(bd->bdEiAftncn), lj->ljBodySectNode, lj ) )
	    { LDEB(1); return -1;	}
	}

    for ( i= 0; i < docNode->biChildCount; i++ )
	{
	lj->ljBodySectNode= docNode->biChildren[i];

	if  ( docLayoutNodeImplementation( &lpHere, &lpHere,
					    docNode->biChildren[i], bf, lj ) )
	    { LDEB(1); return -1;	}
	}

    if  ( docNode->biTreeType == DOCinBODY		&&
	  npEndnotes->npPlacement == FTNplaceDOC_END	)
	{
	if  ( docLayoutEndnotesForDocument( &lpHere, &lpHere, bf, lj ) )
	    { LDEB(1); return -1;	}
	}

    *lpBelow= lpHere;
    return 0;
    }

/************************************************************************/
/*									*/
/*  Do the layout of a document item.					*/
/*									*/
/*  This is the main entry poin of the formatter.			*/
/*									*/
/*  1)  While balancing section columns.. Do not reformat items that	*/
/*	are irrelevant.							*/
/*									*/
/************************************************************************/

int docLayoutNodeImplementation(	LayoutPosition *	lpBelow,
					const LayoutPosition *	lpTop,
					BufferItem *		node,
					BlockFrame *		bf,
					LayoutJob *		lj )
    {
    const LayoutContext *	lc= &(lj->ljContext);
    int				i;

    LayoutPosition		lpHere= *lpTop;

    /*  1  */
    if  ( lj->ljBalancePage >= 0				&&
	  node->biBelowPosition.lpPage < lj->ljBalancePage	)
	{ *lpBelow= node->biBelowPosition; return 0;	}

    docLayoutStartNodeLayout( node, lj, &lpHere );

    switch( node->biLevel )
	{
	case DOClevBODY:

	    if  ( docLayoutBodyNode( &lpHere, &lpHere, node, bf, lj ) )
		{ LDEB(1); return -1;	}
	    break;

	rowAsGroup:
	sectAsGroup:
	    for ( i= 0; i < node->biChildCount; i++ )
		{
		if  ( docLayoutNodeImplementation( &lpHere, &lpHere,
						node->biChildren[i], bf, lj ) )
		    { LDEB(1); return -1;	}
		}
	    if  ( node->biChildCount > 0 )
		{ node->biTopPosition= node->biChildren[0]->biTopPosition; }
	    break;

	case DOClevCELL:
	    if  ( docLayoutCellNode( &lpHere, &lpHere, node, 0, bf, lj ) )
		{ LDEB(1); return -1;	}
	    break;

	case DOClevSECT:
	    if  ( ! node->biParent )
		{ goto sectAsGroup;	}

	    if  ( docLayoutSectNode( &lpHere, &lpHere, node, bf, lj ) )
		{ LDEB(1); return -1;	}
	    break;

	case DOClevROW:
	    {
	    int		stopCode= FORMATstopREADY;
	    int		stayInThisColumn= 0;

	    if  ( ! docIsRowNode( node ) )
		{
		if  ( node->biNumberInParent > 0 )
		    {
		    docLayoutCalculateAfterRowTopInset( node, lc->lcDocument );
		    }

		goto rowAsGroup;
		}

	    if  ( docLayoutRowNode( &stopCode, &lpHere, &lpHere,
					    node, bf, stayInThisColumn, lj ) )
		{ LDEB(1); return -1;	}
	    }
	    break;

	case DOClevPARA:
	    if  ( docLayoutParaNode( &lpHere, &lpHere, node, bf, lj ) )
		{ LDEB(1); return -1;	}

	    break;

	default:
	    LDEB(node->biLevel); return -1;
	}

    docLayoutFinishNodeLayout( (int *)0, node, lj, &lpHere );

    *lpBelow= lpHere;
    return 0;
    }

/************************************************************************/
/*									*/
/*  Recalculate the layout for a block in the document hierarchy.	*/
/*									*/
/*  1)  If we do not have to recalculate the layout of the document as	*/
/*	a whole..							*/
/*  2)  Calculate the frame in which the text is to be laid out.	*/
/*  3)  If the preceding paragraph ends on the same page where this	*/
/*	nodes begins, reserve space for the footnotes upto the		*/
/*	beginning of this block and subtract the height from the buttom	*/
/*	of the frame.							*/
/*  4)  Perform the actual layout operation.				*/
/*  5)  Adjust the positions of the parent nodes and any children of	*/
/*	the parent below this node.					*/
/*									*/
/************************************************************************/

int docLayoutNodeAndParents(	BufferItem *		node,
				LayoutJob *		lj )
    {
    int				rval= 0;
    BlockFrame			bf;
    LayoutPosition		lpHere;
    const LayoutContext *	lc= &(lj->ljContext);
    const BufferItem *		bodyNode= lc->lcDocument->bdBody.dtRoot;
    const BufferItem *		sectNode= docGetSectNode( node );

    docLayoutInitBlockFrame( &bf );

    docItemLayoutStartPosition( &lpHere, node );

    if  ( node->biTreeType == DOCinBODY )
	{
	if  ( ! sectNode )
	    { sectNode= bodyNode->biChildren[0];		}

	lj->ljBodySectNode= sectNode;
	}
    else{
	const BufferItem *		bodySectNode;

	if  ( ! sectNode )
	    { XDEB(sectNode); bodySectNode= bodyNode->biChildren[0];	}
	else{
	    const SelectionScope *	ss= &(sectNode->biSectSelectionScope);

	    bodySectNode= bodyNode->biChildren[ss->ssOwnerSectNr];
	    }

	lj->ljBodySectNode= bodySectNode;
	}

    /*  1  */
    if  ( node->biLevel != DOClevBODY				&&
	  docLayoutGetInitialFrame( &bf, lj, &lpHere, node )	)
	{ LDEB(1); rval= -1; goto ready;	}

    /*  4  */
    if  ( docLayoutNodeImplementation( &lpHere, &lpHere, node, &bf, lj ) )
	{ LDEB(1); rval= -1; goto ready;	}

    /*  5  */
    if  ( docAdjustParentLayout( &lpHere, node, &bf, lj ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    docLayoutCleanBlockFrame( &bf );

    return rval;
    }
