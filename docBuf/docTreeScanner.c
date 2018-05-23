/************************************************************************/
/*									*/
/*  Traverse a selection of a tree of nodes.				*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	<ctype.h>

#   include	<appDebugon.h>

#   include	"docTreeScanner.h"
#   include	<docTreeType.h>
#   include	"docTreeNode.h"

/************************************************************************/

typedef struct TreeScanner
    {
    const DocumentSelection *	tsSelection;
    const BufferItem *		tsBodySectNode;
    void *			tsThrough;
    NodeVisitor			tsEnterNode;
    NodeVisitor			tsLeaveNode;
    unsigned int		tsFlags;
    } TreeScanner;

/************************************************************************/
/*									*/
/*  Scan a buffer node, hierarchically descending to the leaves of the	*/
/*  document tree.							*/
/*									*/
/************************************************************************/

static int docScanNode(	TreeScanner *			ts,
			BufferItem *			node )
    {
    int			i;
    int			ret;
    int			isRow;

    /* Not needed: They are in the same tree.
    int filter=
	ts->tsSelection && docSelectionSameRoot( ts->tsSelection, node );
    */

    if  ( node->biTreeType == DOCinBODY			&&
	  node->biLevel == DOClevSECT				)
	{ ts->tsBodySectNode= node;	}

    if  ( docCompareNodePositionToSelection( node, ts->tsSelection ) != 0 )
	{ return ADVICEtsOK;	}

    if  ( ts->tsEnterNode )
	{
	ret= (*ts->tsEnterNode)( node, ts->tsSelection,
					ts->tsBodySectNode, ts->tsThrough );
	switch( ret )
	    {
	    case ADVICEtsOK:
		break;
	    case ADVICEtsSTOP:
		return ret;
	    case ADVICEtsSKIP:
		return ADVICEtsOK;
	    default:
		LDEB(ret); return -1;
	    }
	}

    isRow= docIsRowNode( node );

    for ( i= 0; i < node->biChildCount; i++ )
	{
	int		selected= 1;

	if  ( isRow )
	    {
	    const CellProperties *	cp= &(node->biRowCells[i]);

	    if  ( ! ( ts->tsFlags & FLAGtsSCAN_MERGED_CELLS )		&&
		  CELL_MERGED( cp )					)
		{ continue;	}

	    ret= docCompareCellPositionToSelection( &selected,
					node->biChildren[i], ts->tsSelection );
	    }
	else{
	    ret= docCompareNodePositionToSelection(
					node->biChildren[i], ts->tsSelection );
	    }
	if  ( ret > 0 )
	    { break;	}
	if  ( ret < 0 || ! selected )
	    { continue;	}

	ret= docScanNode( ts, node->biChildren[i] );
	switch( ret )
	    {
	    case ADVICEtsOK:
		break;
	    case ADVICEtsSTOP:
		return ret;
	    case ADVICEtsSKIP:
	    default:
		LLDEB(i,ret); return -1;
	    }
	}

    if  ( ts->tsLeaveNode )
	{
	ret= (*ts->tsLeaveNode)( node, ts->tsSelection,
					ts->tsBodySectNode, ts->tsThrough );
	switch( ret )
	    {
	    case ADVICEtsOK:
	    case ADVICEtsSTOP:
		return ret;
	    case ADVICEtsSKIP:
	    default:
		LLDEB(i,ret); return -1;
	    }
	}

    return ADVICEtsOK;
    }

static void docInitTreeScanner(	TreeScanner *		ts )
    {
    ts->tsSelection= (const DocumentSelection *)0;
    ts->tsBodySectNode= (const BufferItem *)0;
    ts->tsThrough= (void *)0;
    ts->tsEnterNode= (NodeVisitor)0;
    ts->tsLeaveNode= (NodeVisitor)0;
    ts->tsFlags= 0;
    return;
    }

int docScanTree(	BufferDocument *		bd,
			const DocumentTree *		dt,
			NodeVisitor			enterNode,
			NodeVisitor			leaveNode,
			int				flags,
			void *				through )
    {
    BufferItem *	root= dt->dtRoot;
    TreeScanner		ts;
    int			res;
    BufferItem *	bodySectNode= (BufferItem *)0;

    docInitTreeScanner( &ts );

    if  ( docGetTreeOfNode( (DocumentTree **)0, &bodySectNode, bd, root ) )
	{ LDEB(1); return -1;	}

    ts.tsSelection= (DocumentSelection *)0;
    ts.tsThrough= through;
    ts.tsEnterNode= enterNode;
    ts.tsLeaveNode= leaveNode;
    ts.tsBodySectNode= bodySectNode;
    ts.tsFlags= flags;

    res= docScanNode( &ts, root );
    if  ( res < 0 )
	{ LDEB(1); return -1;	}

    return res;
    }

int docScanSelection(	BufferDocument *		bd,
			const DocumentSelection *	ds,
			NodeVisitor			enterNode,
			NodeVisitor			leaveNode,
			int				flags,
			void *				through )
    {
    BufferItem *	root= bd->bdBody.dtRoot;
    TreeScanner		ts;
    int			res;
    BufferItem *	bodySectNode= (BufferItem *)0;

    docInitTreeScanner( &ts );

    if  ( ds )
	{
	DocumentTree *	dt= (DocumentTree *)0;

	root= docGetSelectionRoot( &dt, &bodySectNode, bd, ds );
	if  ( ! root )
	    { XDEB(root); return -1;	}
	}
    else{
	if  ( docGetTreeOfNode( (DocumentTree **)0, &bodySectNode, bd, root ) )
	    { LDEB(1); return -1;	}
	}

    ts.tsSelection= ds;
    ts.tsThrough= through;
    ts.tsEnterNode= enterNode;
    ts.tsLeaveNode= leaveNode;
    ts.tsBodySectNode= bodySectNode;
    ts.tsFlags= flags;

    res= docScanNode( &ts, root );
    if  ( res < 0 )
	{ LDEB(1); return -1;	}

    return res;
    }

int docScanTreeNode(	BufferDocument *		bd,
			BufferItem *			node,
			NodeVisitor			enterNode,
			NodeVisitor			leaveNode,
			int				flags,
			void *				through )
    {
    TreeScanner		ts;
    int			res;
    BufferItem *	bodySectNode= (BufferItem *)0;

    docInitTreeScanner( &ts );

    if  ( docGetTreeOfNode( (DocumentTree **)0, &bodySectNode, bd, node ) )
	{ LDEB(1); return -1;	}

    ts.tsSelection= (const DocumentSelection *)0;
    ts.tsThrough= through;
    ts.tsEnterNode= enterNode;
    ts.tsLeaveNode= leaveNode;
    ts.tsBodySectNode= bodySectNode;
    ts.tsFlags= flags;

    res= docScanNode( &ts, node );
    if  ( res < 0 )
	{ LDEB(1); return -1;	}

    return res;
    }
