/************************************************************************/
/*									*/
/*  Buffer manipulation routines.					*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	<appDebugon.h>

#   include	"docBuf.h"
#   include	"docTreeNode.h"

/************************************************************************/
/*									*/
/*  Set the properties of a table row.					*/
/*									*/
/*  1)  Adapt to an explicit \trkeepfollow.				*/
/*  2)  Adapt to an implicit \trkeepfollow.				*/
/*									*/
/************************************************************************/

int docChangeRowProperties(	PropertyMask *			rpDoneMask,
				BufferItem *			rowNode,
				const PropertyMask *		rpSetMask,
				const RowProperties *		rpSet )
    {
    RowProperties *		rpTo= &(rowNode->biRowProperties);

    DocumentPosition		dp;

    if  ( docUpdRowProperties( rpDoneMask, rpTo, rpSetMask, rpSet,
					(const DocumentAttributeMap *)0 ) )
	{ LDEB(1);	}

    if  ( ! docHeadPosition( &dp, rowNode ) )
	{
	BufferItem *	paraNode= dp.dpNode;

	/*  1  */
	if  ( PROPmaskISSET( rpSetMask, RPprop_KEEPFOLLOW ) )
	    { paraNode->biParaKeepWithNext= rpTo->rp_Keepfollow;	}

	/*  2  */
	rpTo->rp_Keepfollow= paraNode->biParaKeepWithNext;
	}

    return 0;
    }

