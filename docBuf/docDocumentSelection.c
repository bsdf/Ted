/************************************************************************/
/*									*/
/*  Administrative routines about selections.				*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	<appDebugon.h>

#   include	"docBuf.h"
#   include	"docTreeNode.h"
#   include	"docNodeTree.h"

/************************************************************************/
/*									*/
/*  Initialise Document Selection.					*/
/*									*/
/************************************************************************/

void docInitDocumentSelection(	DocumentSelection *	ds )
    {
    docInitSelectionScope( &(ds->dsSelectionScope) );

    docInitDocumentPosition( &(ds->dsHead) );
    docInitDocumentPosition( &(ds->dsTail) );
    docInitDocumentPosition( &(ds->dsAnchor) );

    ds->dsCol0= -1;
    ds->dsCol1= -1;

    ds->dsDirection= 0;

    return;
    }

void docSetIBarSelection(	DocumentSelection *		ds,
				const DocumentPosition *	dp )
    {
    ds->dsHead= *dp;
    ds->dsTail= *dp;
    ds->dsAnchor= *dp;

    ds->dsCol0= -1;
    ds->dsCol1= -1;
    ds->dsDirection= 0;

    docSetSelectionScope( ds );

    return;
    }

void docSetRangeSelection(	DocumentSelection *		ds,
				const DocumentPosition *	dpHead,
				const DocumentPosition *	dpTail,
				int				direction )
    {
    DocumentSelection	dsHere;

    docInitDocumentSelection( &dsHere );

    dsHere.dsHead= *dpHead;
    dsHere.dsTail= *dpTail;
    dsHere.dsDirection= ( direction > 0 ) - ( direction < 0 );

    dsHere.dsCol0= -1;
    dsHere.dsCol1= -1;

    if  ( dpHead->dpNode->biParaTableNesting > 0	&&
	  dpTail->dpNode->biParaTableNesting > 0	)
	{
	BufferItem *	cellNode0= docGetCellNode( dpHead->dpNode );
	BufferItem *	cellNode1= docGetCellNode( dpTail->dpNode );
	BufferItem *	rowNode0= cellNode0->biParent;
	BufferItem *	rowNode1= cellNode1->biParent;

	if  ( rowNode0->biParent == rowNode1->biParent			&&
	      rowNode0->biRowTableFirst >= 0				&&
	      rowNode0->biRowTableFirst == rowNode1->biRowTableFirst	)
	    {
	    if  ( rowNode0->biRowTablePast != rowNode1->biRowTablePast )
		{ LLDEB(rowNode0->biRowTablePast,rowNode1->biRowTablePast); }
	    if  ( rowNode0->biNumberInParent < rowNode1->biRowTableFirst )
		{ LLDEB(rowNode0->biNumberInParent,rowNode1->biRowTableFirst); }
	    if  ( rowNode0->biNumberInParent >= rowNode1->biRowTablePast )
		{ LLDEB(rowNode0->biNumberInParent,rowNode1->biRowTablePast); }

	    if  ( cellNode0->biNumberInParent > cellNode1->biNumberInParent )
		{
		LLDEB(cellNode0->biNumberInParent,cellNode1->biNumberInParent);
		dsHere.dsCol0= cellNode1->biNumberInParent;
		dsHere.dsCol1= cellNode0->biNumberInParent;
		}
	    else{
		dsHere.dsCol0= cellNode0->biNumberInParent;
		dsHere.dsCol1= cellNode1->biNumberInParent;
		}
	    }
	}
    
    if  ( direction >= 0 )
	{ dsHere.dsAnchor= *dpHead;	}
    else{ dsHere.dsAnchor= *dpTail;	}

    docSetSelectionScope( &dsHere );

    *ds= dsHere;
    return;
    }

int docIsIBarSelection( const DocumentSelection *		ds )
    {
    if  ( ! ds->dsHead.dpNode )
	{ return 0;	}

    if  ( ds->dsHead.dpNode != ds->dsTail.dpNode )
	{ return 0;	}

    if  ( ds->dsHead.dpStroff != ds->dsTail.dpStroff )
	{ return 0;	}

    return 1;
    }

int docIsParaSelection( const DocumentSelection *		ds )
    {
    if  ( ! ds->dsHead.dpNode )
	{ return 0;	}

    if  ( ds->dsHead.dpNode != ds->dsTail.dpNode )
	{ return 0;	}

    return 1;
    }

/************************************************************************/
/*									*/
/*  Reduce a selection to one paragraph.				*/
/*									*/
/************************************************************************/

void docConstrainSelectionToOneParagraph( int *			pHeadMoved,
					int *			pTailMoved,
					DocumentSelection *	ds )
    {
    int		lenBegin;
    int		lenEnd;

    if  ( docIsParaSelection( ds ) )
	{ return;	}

    lenBegin= docParaStrlen( ds->dsHead.dpNode )- ds->dsHead.dpStroff;
    lenEnd= ds->dsTail.dpStroff;

    if  ( lenEnd > lenBegin )
	{
	if  ( docHeadPosition( &(ds->dsHead), ds->dsTail.dpNode ) )
	    { LDEB(1);	}
	ds->dsCol0= ds->dsCol1;

	if  ( ds->dsAnchor.dpNode != ds->dsHead.dpNode )
	    { ds->dsAnchor= ds->dsHead;	}

	*pHeadMoved= 1; *pTailMoved= 0;
	return;
	}
    else{
	if  ( docTailPosition( &(ds->dsTail), ds->dsHead.dpNode ) )
	    { LDEB(1);	}
	ds->dsCol1= ds->dsCol0;

	if  ( ds->dsAnchor.dpNode != ds->dsTail.dpNode )
	    { ds->dsAnchor= ds->dsTail;	}

	*pHeadMoved= 0; *pTailMoved= 1;
	return;
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Translate the result of a search in a paragraph to a selection.	*/
/*									*/
/************************************************************************/

void docSetParaSelection(	DocumentSelection *	ds,
				BufferItem *		paraNode,
				int			direction,
				int			stroff,
				int			length )
    {
    if  ( paraNode->biLevel != DOClevPARA )
	{ LDEB(paraNode->biLevel);	}

    docSetDocumentPosition( &(ds->dsHead), paraNode, stroff );
    docSetDocumentPosition( &(ds->dsTail), paraNode, stroff+ length );

    ds->dsDirection= direction;

    ds->dsCol0= ds->dsCol1= -1;

    if  ( direction >= 0 )
	{ ds->dsAnchor= ds->dsHead;	}
    else{ ds->dsAnchor= ds->dsTail;	}

    docSetSelectionScope( ds );

    return;
    }

/************************************************************************/
/*									*/
/*  Select a whole node.						*/
/*									*/
/************************************************************************/

int docSetNodeSelection(	DocumentSelection *	ds,
				BufferItem *		node )
    {
    const int		direction= 1;
    DocumentPosition	dpHead;
    DocumentPosition	dpTail;

    if  ( docHeadPosition( &dpHead, node ) )
	{ LDEB(1); return -1;	}
    if  ( docTailPosition( &dpTail, node ) )
	{ LDEB(1); return -1;	}

    docSetRangeSelection( ds, &dpHead, &dpTail, direction );

    return 0;
    }
