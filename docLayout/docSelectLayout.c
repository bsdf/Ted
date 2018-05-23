/************************************************************************/
/*									*/
/*  Geometry administration for 'DocumentTrees' I.E. headers/footers	*/
/*  and foonotes/endnotes.						*/
/*									*/
/************************************************************************/

#   include	"docLayoutConfig.h"

#   include	<appDebugon.h>

#   include	<docBuf.h>
#   include	<docDebug.h>
#   include	<docTextLine.h>
#   include	<docTreeNode.h>
#   include	<docNodeTree.h>
#   include	"docSelectLayout.h"

/************************************************************************/
/*									*/
/*  Go to the top/bottom of a certain page.				*/
/*									*/
/*  1)  Sections that start on an odd/even page may skip a page.	*/
/*									*/
/************************************************************************/

static int docParaGetFirstInColumn(
			DocumentPosition *		dp,
			int *				pLineTop,
			int *				pPartTop,
			BufferItem *			paraBi,
			int				lineUpto,
			int				page,
			int				column )
    {
    int			i;
    const TextLine *	tl= paraBi->biParaLines;

    for ( i= 0; i < lineUpto; tl++, i++ )
	{
	if  ( tl->tlTopPosition.lpPage >= page )
	    {
	    if  ( tl->tlTopPosition.lpPage > page )
		{ break;	}
	    if  ( tl->tlTopPosition.lpColumn >= column )
		{ break;	}
	    }
	}

    if  ( paraBi->biParaLineCount > 0 && i >= paraBi->biParaLineCount )
	{
	tl= paraBi->biParaLines+ paraBi->biParaLineCount- 1;

	if  ( tl->tlFlags & TLflagBLOCKBREAK )
	    {
	    BufferItem *	nextBi= docNextParagraph( paraBi );

	    if  ( nextBi && nextBi->biParaLineCount > 0 )
		{ i= 0; paraBi= nextBi; tl= paraBi->biParaLines;	}
	    }
	}

    if  ( i >= paraBi->biParaLineCount		||
	  tl->tlTopPosition.lpPage != page	||
	  tl->tlTopPosition.lpColumn != column	)
	{
	SDEB(docTreeTypeStr(paraBi->biTreeType));
	LLDEB(page,column);
	LDEB(paraBi->biBelowPosition.lpPage);
	LDEB(paraBi->biBelowPosition.lpColumn);
	/*docListNode(0,paraBi);*/
	LDEB(docNumberOfParagraph(paraBi));
	LLDEB(i,paraBi->biParaLineCount);
	return -1;
	}

    dp->dpNode= paraBi;
    dp->dpStroff= tl->tlStroff;

    *pLineTop= tl- paraBi->biParaLines;
    *pPartTop= tl->tlFirstParticule;

    return 0;
    }

int docGetFirstInColumnForNode(
			DocumentPosition *		dp,
			int *				pLineTop,
			int *				pPartTop,
			BufferItem *			node,
			int				page,
			int				column )
    {
    int			i;

    while( node && node->biLevel != DOClevPARA )
	{
	/*  1  */
	if  ( node->biTopPosition.lpPage > page )
	    {
	    /* LLDEB(node->biTopPosition.lpPage,page); */
	    return 1;
	    }
	if  ( node->biBelowPosition.lpPage < page )
	    {
	    /* LLDEB(node->biTopPosition.lpPage,page); */
	    return 1;
	    }

	for ( i= 0; i < node->biChildCount; i++ )
	    {
	    if  ( node->biChildren[i]->biBelowPosition.lpPage >= page )
		{
		if  ( node->biChildren[i]->biBelowPosition.lpPage > page )
		    { break;	}
		if  ( node->biChildren[i]->biBelowPosition.lpColumn >= column )
		    { break;	}
		}
	    }

	if  ( i >= node->biChildCount )
	    {
	    /*  NO! is possible e.g. when endnotes continue beyond the 
	        last page with body content.
	    for ( i= 0; i < node->biChildCount; i++ )
		{ LLDEB(i,node->biChildren[i]->biBelowPosition.lpPage); }
	    LLLDEB(node->biTopPosition.lpPage,node->biBelowPosition.lpPage,page);
	    return -1;
	    */
	    return 1;
	    }

	node= node->biChildren[i];
	}

    if  ( ! node || node->biLevel != DOClevPARA )
	{ XDEB(node); return -1;	}

    return docParaGetFirstInColumn( dp, pLineTop, pPartTop,
				    node, node->biParaLineCount, page, column );
    }

int docGetTopOfColumn(	DocumentPosition *		dp,
			int *				pLineTop,
			int *				pPartTop,
			BufferDocument *		bd,
			int				page,
			int				column )
    {
    return docGetFirstInColumnForNode( dp, pLineTop, pPartTop,
					    bd->bdBody.dtRoot, page, column );
    }

int docGetLastInColumnForNode(	DocumentPosition *		dp,
				int *				pLineBot,
				int *				pPartBot,
				BufferItem *			nodeIn,
				int				page,
				int				column )
    {
    int			i;
    const TextLine *	tl;
    BufferItem *	node= nodeIn;

    while( node && node->biLevel != DOClevPARA )
	{
	if  ( node->biTopPosition.lpPage > page )
	    { /*LLDEB(node->biBelowPosition.lpPage,page);*/ return 1;	}
	if  ( node->biBelowPosition.lpPage < page )
	    { /*LLDEB(node->biBelowPosition.lpPage,page);*/ return 1;	}

	for ( i= node->biChildCount- 1; i >= 0; i-- )
	    {
	    if  ( node->biChildren[i]->biTopPosition.lpPage <= page )
		{
		if  ( node->biChildren[i]->biTopPosition.lpPage < page )
		    { break;	}
		if  ( node->biChildren[i]->biTopPosition.lpColumn <= column )
		    { break;	}
		}
	    }

	if  ( i < 0 )
	    {
	    /*
	    for ( i= 0; i < node->biChildCount; i++ )
		{ LLDEB(i,node->biChildren[i]->biTopPosition.lpPage); }
	    LLLDEB(node->biTopPosition.lpPage,node->biBelowPosition.lpPage,page);
	    */
	    return 1;
	    }

	node= node->biChildren[i];
	}

    if  ( ! node || node->biLevel != DOClevPARA )
	{ XDEB(node); return -1;	}

    tl= node->biParaLines+ node->biParaLineCount- 1;
    for ( i= node->biParaLineCount- 1; i >= 0; tl--, i-- )
	{
	if  ( tl->tlTopPosition.lpPage <= page )
	    {
	    if  ( tl->tlTopPosition.lpPage < page )
		{ break;	}
	    if  ( tl->tlTopPosition.lpColumn <= column )
		{ break;	}
	    }
	}

    if  ( i < 0					||
	  tl->tlTopPosition.lpPage != page	||
	  tl->tlTopPosition.lpColumn != column	)
	{
	SSDEB(docTreeTypeStr(node->biTreeType),docLevelStr(node->biLevel));
	LLDEB(page,column);
	LDEB(node->biBelowPosition.lpPage);
	LDEB(node->biBelowPosition.lpColumn);
	/*docListNode(0,nodeIn,0);*/
	LDEB(docNumberOfParagraph(node));
	LLDEB(i,node->biParaLineCount);
	return -1;
	}

    dp->dpNode= node;
    dp->dpStroff= tl->tlStroff+ tl->tlStrlen;

    *pLineBot= tl- node->biParaLines;
    *pPartBot= tl->tlFirstParticule+ tl->tlParticuleCount- 1;

    return 0;
    }

int docGetBottomOfColumn(
			DocumentPosition *		dp,
			int *				pPartBot,
			BufferDocument *		bd,
			int				page,
			int				column )
    {
    int		lineBot;

    return docGetLastInColumnForNode( dp, &lineBot, pPartBot,
					    bd->bdBody.dtRoot, page, column );
    }

