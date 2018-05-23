/************************************************************************/
/*									*/
/*  Get headers and footers for a certain page/position.		*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	<appDebugon.h>

#   include	"docBuf.h"
#   include	<docTreeType.h>
#   include	"docTreeNode.h"
#   include	"docNodeTree.h"

/************************************************************************/
/*									*/
/*  What BufferItem is the common root of the selection.		*/
/*									*/
/************************************************************************/

int docGetTreeOfNode(		DocumentTree **		pTree,
				BufferItem **		pBodySectNode,
				BufferDocument *	bd,
				BufferItem *		node )
    {
    BufferItem *	selSectBi;
    BufferItem *	bodySectNode= (BufferItem *)0;
    DocumentTree *	dt;

    if  ( node->biLevel == DOClevBODY && node->biChildCount > 0 )
	{ selSectBi= node->biChildren[0];	}
    else{
	node= docGetSectNode( node );
	if  ( ! node )
	    { XDEB(node); return -1;	}

	selSectBi= node;
	}

    if  ( docGetRootOfSelectionScope( &dt, &bodySectNode,
				    bd, &(selSectBi->biSectSelectionScope) ) )
	{ LDEB(1); return -1;	}

    if  ( pTree )
	{ *pTree= dt;			}
    if  ( pBodySectNode )
	{ *pBodySectNode= bodySectNode;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  What BufferItem is the common root of the selection.		*/
/*									*/
/************************************************************************/

BufferItem * docGetSelectionRoot(
			DocumentTree **			pTree,
			BufferItem **			pBodySectNode,
			BufferDocument *		bd,
			const DocumentSelection *	ds )
    {
    BufferItem *	selRootBi= (BufferItem *)0;

    if  ( ! ds->dsHead.dpNode || ! ds->dsTail.dpNode )
	{
	XXDEB(ds->dsHead.dpNode,ds->dsTail.dpNode);
	return (BufferItem *)0;
	}

    selRootBi= docGetCommonParent( ds->dsHead.dpNode, ds->dsTail.dpNode );

    if  ( pTree || pBodySectNode )
	{
	DocumentTree *	dt= (DocumentTree *)0;
	BufferItem *	bodySectNode= (BufferItem *)0;

	if  ( docGetTreeOfNode( &dt, &bodySectNode, bd, selRootBi ) )
	    { LDEB(1); return (BufferItem *)0;	}

	if  ( pTree )
	    { *pTree= dt;			}
	if  ( pBodySectNode )
	    { *pBodySectNode= bodySectNode;	}
	}

    return selRootBi;
    }

/************************************************************************/

BufferItem * docGetBodySectNodeOfScope(	const SelectionScope *	ss,
					const BufferDocument *	bd )
    {
    BufferItem *		sectBi;
    int				sect;

    if  ( ss->ssTreeType == DOCinBODY )
	{ sect= ss->ssSectNr;		}
    else{ sect= ss->ssOwnerSectNr;	}

    if  ( sect < 0 || sect >= bd->bdBody.dtRoot->biChildCount )
	{
	SLLDEB(docTreeTypeStr(ss->ssTreeType),sect,bd->bdBody.dtRoot->biChildCount);
	return (BufferItem *)0;
	}

    sectBi= bd->bdBody.dtRoot->biChildren[sect];

    if  ( sectBi->biLevel != DOClevSECT )
	{ LDEB(sectBi->biLevel); return (BufferItem *)0;	}

    return sectBi;
    }

BufferItem * docGetBodySectNode(	BufferItem *		node,
					const BufferDocument *	bd )
    {
    int				sect;

    if  ( node->biLevel != DOClevSECT )
	{
	node= docGetSectNode( node );
	if  ( ! node )
	    { XDEB(node); return (BufferItem *)0;	}
	}

    if  ( node->biTreeType == DOCinBODY )
	{ sect= node->biNumberInParent;			}
    else{ sect= node->biSectSelectionScope.ssOwnerSectNr;	}

    if  ( sect < 0 || sect >= bd->bdBody.dtRoot->biChildCount )
	{
	LLDEB(sect,bd->bdBody.dtRoot->biChildCount);
	return (BufferItem *)0;
	}

    return bd->bdBody.dtRoot->biChildren[sect];
    }

