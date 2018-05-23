/************************************************************************/
/*									*/
/*  Manage the scope of selections: In which sub tree if the document	*/
/*  are they rooted?							*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	<appDebugon.h>

#   include	"docBuf.h"
#   include	<docTreeType.h>
#   include	"docTreeNode.h"
#   include	"docNodeTree.h"

int docSelectionSameRoot(
			const DocumentSelection *	dsFrom,
			const BufferItem *		biTo )
    {
    const SelectionScope *	ssFrom= &(dsFrom->dsSelectionScope);
    const SelectionScope *	ssTo;

    if  ( biTo->biTreeType == DOCinBODY	&&
          ssFrom->ssTreeType == DOCinBODY	)
	{ return 1;	}

    biTo= docGetSectNode( (BufferItem *)biTo );
    if  ( ! biTo )
	{ XDEB(biTo); return -1;	}
    ssTo= &(biTo->biSectSelectionScope);

    return docSelectionSameScope( ssFrom, ssTo );
    }

int docSelectionSameInstance(	const DocumentTree *		tree,
				int				page,
				int				column )
    {
    if  ( ! tree->dtRoot )
	{ XDEB(tree->dtRoot); return -1;	}

    switch( tree->dtRoot->biTreeType )
	{
	case DOCinBODY:
	    return 1;

	case DOCinFTNSEP:
	case DOCinFTNSEPC:
	case DOCinFTNCN:
	case DOCinAFTNSEP:
	case DOCinAFTNSEPC:
	case DOCinAFTNCN:

	    return tree->dtPageSelectedUpon == page && 
		    tree->dtColumnSelectedIn == column;

	case DOCinFIRST_HEADER:
	case DOCinLEFT_HEADER:
	case DOCinRIGHT_HEADER:

	case DOCinFIRST_FOOTER:
	case DOCinLEFT_FOOTER:
	case DOCinRIGHT_FOOTER:

	    return tree->dtPageSelectedUpon == page;

	case DOCinFOOTNOTE:
	case DOCinENDNOTE:

	    return 1;

	default:
	    LDEB(tree->dtRoot->biTreeType);
	    return -1;
	}
    }

/************************************************************************/
/*									*/
/*  Derive the scope of a selection from a buffer node inside.		*/
/*									*/
/************************************************************************/

void docGetSelectionScope(	SelectionScope *	ss,
				const BufferItem *	node )
    {
    if  ( ! node )
	{ XDEB(node);	}

    node= docGetSectNode( (BufferItem *)node );
    if  ( ! node )
	{ XDEB(node);				}
    else{ (*ss)= node->biSectSelectionScope;	}

    return;
    }

void docSetSelectionScope(	DocumentSelection *	ds )
    { docGetSelectionScope( &(ds->dsSelectionScope), ds->dsAnchor.dpNode ); }
