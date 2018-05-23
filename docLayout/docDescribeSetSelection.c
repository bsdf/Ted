#   include	"docLayoutConfig.h"

#   include	"docLayout.h"
#   include	<docTreeType.h>
#   include	<docTreeNode.h>
#   include	<docNodeTree.h>
#   include	"docScreenLayout.h"
#   include	"docDescribeSetSelection.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Describe what is to be done because a new selection is set in a	*/
/*  formatted document.							*/
/*									*/
/************************************************************************/

int docDescribeSetSelection(
			DocumentTree **			pTreeSet,
			BufferItem **			pBodySectNodeSet,
			DocumentRectangle *		drExternalSet,
			int *				pRedrawOldTree,
			int *				pRedrawNewTree,
			LayoutContext *			lc,
			BufferDocument *		bd,
			const DocumentSelection *	dsOld,
			const DocumentSelection *	dsSet )
    {
    BufferItem *	rootNodeSet;
    BufferItem *	bodySectNodeSet;
    DocumentTree *	treeSet;

    int			sameRoot;
    int			bodySectNr= -1;

    int			redrawTreeOld= 0;
    int			redrawTreeSet= 0;

    const int		justUsed= 0;

    rootNodeSet= docGetSelectionRoot( &treeSet, &bodySectNodeSet, bd, dsSet );
    if  ( ! rootNodeSet )
	{ XDEB(rootNodeSet); return -1;	}

    sameRoot= docSelectionSameRoot( dsOld, rootNodeSet );

    /******/
    switch( dsOld->dsSelectionScope.ssTreeType )
	{
	case DOCinBODY:
	    break;

	case DOCinFIRST_HEADER:
	case DOCinLEFT_HEADER:
	case DOCinRIGHT_HEADER:

	case DOCinFIRST_FOOTER:
	case DOCinLEFT_FOOTER:
	case DOCinRIGHT_FOOTER:
	    if  ( ! sameRoot )
		{ redrawTreeOld= 1;	}
	    break;

	case DOCinFOOTNOTE:
	case DOCinENDNOTE:
	    if  ( ! sameRoot )
		{ redrawTreeOld= 1;	}
	    break;

	case DOCinFTNSEP:
	case DOCinFTNSEPC:
	case DOCinFTNCN:
	case DOCinAFTNSEP:
	case DOCinAFTNSEPC:
	case DOCinAFTNCN:
	    if  ( ! sameRoot )
		{ redrawTreeOld= 1;	}
	    break;

	case DOCinSHPTXT:
	    if  ( ! sameRoot )
		{ redrawTreeOld= 1;	}
	    break;

	default:
	    LDEB(dsOld->dsSelectionScope.ssTreeType);
	    break;
	}

    /******/
    switch( rootNodeSet->biTreeType )
	{
	case DOCinBODY:
	    bodySectNodeSet= docGetSectNode( dsSet->dsHead.dpNode );
	    if  ( ! bodySectNodeSet )
		{ XDEB(bodySectNodeSet); return -1;	}

	    bodySectNr= bodySectNodeSet->biNumberInParent;
	    break;

	case DOCinFIRST_HEADER:
	case DOCinLEFT_HEADER:
	case DOCinRIGHT_HEADER:

	case DOCinFIRST_FOOTER:
	case DOCinLEFT_FOOTER:
	case DOCinRIGHT_FOOTER:

	    {
	    const int		page= treeSet->dtPageSelectedUpon;
	    const int		column= 0; /* irrelevant */
	    int			changed= 0;
	    DocumentTree *	tree= (DocumentTree *)0;

	    if  ( ! sameRoot )
		{ redrawTreeSet= 1;	}

	    if  ( docGetHeaderFooter( &tree, &bodySectNodeSet,
					    &(dsSet->dsHead), bd,
					    rootNodeSet->biTreeType ) )
		{ LDEB(rootNodeSet->biTreeType);	}

	    /*
	    page= docSectionHeaderFooterFirstPage( &usedInDoc, bodySectNodeSet,
					rootNodeSet->biTreeType, dp );

	    if  ( page < 0 || ! usedInDoc )
		{ LLDEB(page,usedInDoc); return;	}
	    */

	    if  ( docCheckPageOfSelectedTree( &changed, &bodySectNodeSet,
				(DocumentRectangle *)0, treeSet, lc,
				docStartScreenLayoutForTree ) )
		{ LDEB(1);	}

	    if  ( changed )
		{ redrawTreeSet= 1;	}

	    if  ( redrawTreeSet )
		{
		if  ( docGetBoxAroundTree( drExternalSet, bodySectNodeSet,
				    treeSet, justUsed, page, column, lc ) )
		    {
		    LDEB(rootNodeSet->biTreeType);
		    redrawTreeSet= 0;
		    }
		}

	    bodySectNr= bodySectNodeSet->biNumberInParent;
	    break;
	    }

	case DOCinFOOTNOTE:
	case DOCinENDNOTE:
	    {
	    DocumentTree *		tree= (DocumentTree *)0;

	    if  ( docGetTreeOfNode( &tree, &bodySectNodeSet, bd, rootNodeSet ) )
		{ LDEB(1); return -1;	}

	    if  ( ! sameRoot )
		{
		const int		page= treeSet->dtPageFormattedFor;
		const int		column= treeSet->dtColumnFormattedFor;

		redrawTreeSet= 1;

		if  ( docGetBoxAroundTree( drExternalSet, bodySectNodeSet,
				    treeSet, justUsed, page, column, lc ) )
		    {
		    LSDEB(rootNodeSet->biTreeType,
			docTreeTypeStr(rootNodeSet->biTreeType));
		    redrawTreeSet= 0;
		    }
		}

	    bodySectNr= bodySectNodeSet->biNumberInParent;
	    }
	    break;

	case DOCinFTNSEP:
	case DOCinFTNSEPC:
	case DOCinFTNCN:
	case DOCinAFTNSEP:
	case DOCinAFTNSEPC:
	case DOCinAFTNCN:
	    {
	    const int		page= treeSet->dtPageSelectedUpon;
	    const int		column= treeSet->dtColumnSelectedIn;
	    int			changed= 0;

	    if  ( ! sameRoot )
		{ redrawTreeSet= 1;	}

	    if  ( docCheckPageOfSelectedTree( &changed, &bodySectNodeSet,
				drExternalSet, treeSet, lc,
				docStartScreenLayoutForTree ) )
		{ LDEB(1);	}

	    if  ( changed )
		{ redrawTreeSet= 1;	}

	    if  ( docGetBoxAroundTree( drExternalSet, bodySectNodeSet,
				    treeSet, justUsed, page, column, lc ) )
		{
		LDEB(rootNodeSet->biTreeType);
		redrawTreeSet= 0;
		}

	    bodySectNr= bodySectNodeSet->biNumberInParent;
	    }
	    break;

	case DOCinSHPTXT:
	    if  ( ! sameRoot )
		{ redrawTreeSet= 1;	}
	    bodySectNr= bodySectNodeSet->biNumberInParent;
	    break;

	default:
	    LDEB(rootNodeSet->biTreeType);
	    break;
	}

    *pTreeSet= treeSet;
    *pBodySectNodeSet= bodySectNodeSet;
    *pRedrawOldTree= redrawTreeOld;
    *pRedrawNewTree= redrawTreeSet;

    return bodySectNr;
    }

