#   include	"docLayoutConfig.h"

#   include	<limits.h>

#   include	"docLayout.h"
#   include	<docBlockOrnaments.h>
#   include	<docPageGrid.h>
#   include	<docTreeNode.h>

#   include	<appDebugon.h>

/************************************************************************/

static void docRowRectangleTwips(	DocumentRectangle *	drRow,
					const BufferItem *	rowNode,
					const BlockFrame *	bf )
    {
    int			col;

    col= 0;
    docCellRectangleTwips( drRow, bf, rowNode->biChildren[col] );

    for ( col= 1; col < rowNode->biChildCount; col++ )
	{
	DocumentRectangle	drCell;

	docCellRectangleTwips( &drCell, bf, rowNode->biChildren[col] );
	geoUnionRectangle( drRow, drRow, &drCell );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Return a rectangle that contains a node and all its children.	*/
/*									*/
/*  4)  No xShift: bf is shifted because of the column.			*/
/*									*/
/************************************************************************/

void docNodeRectangle(	DocumentRectangle *		drPixels,
			BufferItem *			node,
			const LayoutContext *		lc,
			const BlockOrigin *		bo )
    {
    BlockFrame			bf;
    ParagraphFrame		pf;

    LayoutPosition		lpTop;
    LayoutPosition		lpBelow;

    BlockOrnaments		ornaments;
    DocumentRectangle		drTwips;

    docLayoutInitBlockFrame( &bf );
    docInitBlockOrnaments( &ornaments );

    lpTop= node->biTopPosition;
    lpBelow= node->biBelowPosition;

    if  ( bo )
	{
	docShiftPosition( &lpTop, bo, &(node->biTopPosition) );
	docShiftPosition( &lpBelow, bo, &(node->biBelowPosition) );
	}

    /********************************************************************/
    /*  If the item covers more than one page/column, completely	*/
    /*  include all pages that have it.					*/
    /********************************************************************/
    if  ( node->biLevel == DOClevBODY		||
	  lpTop.lpPage != lpBelow.lpPage	||
	  lpTop.lpColumn != lpBelow.lpColumn	)
	{
	docGetPixelRectangleForPages( drPixels, lc,
					    lpTop.lpPage, lpBelow.lpPage );
	goto ready;
	}

    docBlockFrameTwips( &bf, node, lc->lcDocument,
					    lpTop.lpPage, lpTop.lpColumn );

    drTwips.drX0= bf.bfContentRect.drX0;
    drTwips.drX1= bf.bfContentRect.drX1;
    drTwips.drY0= lpTop.lpPageYTwips;
    drTwips.drY1= lpBelow.lpPageYTwips;

    switch( node->biLevel )
	{
	case DOClevSECT:
	    drTwips.drX0= 0;
	    drTwips.drX1= INT_MAX;
	    break;

	case DOClevROW:
	    if  ( docIsRowNode( node ) )
		{
		DocumentRectangle	drRow;

		docRowRectangleTwips( &drRow, node, &bf );

		drRow.drX0 -= BRDRW_MAX2;
		drRow.drX1 += BRDRW_MAX2;
		drRow.drY0 -= BRDRW_MAX2;
		drRow.drY1 += BRDRW_MAX2;

		if  ( bo )
		    { docShiftRectangle( &drRow, bo, &drRow );	}

		geoUnionRectangle( &drTwips, &drTwips, &drRow );
		}

	    break;

	case DOClevCELL:
	    if  ( docIsRowNode( node->biParent ) )
		{
		DocumentRectangle	drCell;

		docCellRectangleTwips( &drCell, &bf, node );

		drCell.drX0 -= BRDRW_MAX2;
		drCell.drX1 += BRDRW_MAX2;
		drCell.drY0 -= BRDRW_MAX2;
		drCell.drY1 += BRDRW_MAX2;

		if  ( bo )
		    { docShiftRectangle( &drCell, bo, &drCell );	}

		geoUnionRectangle( &drTwips, &drTwips, &drCell );
		}
	    break;

	case DOClevPARA:
	    docParagraphFrameTwips( &pf, &bf, node );

	    /*  4  */
	    drTwips.drX0= pf.pfCellContentRect.drX0;
	    drTwips.drX1= pf.pfCellContentRect.drX1;
	    break;

	default:
	    LDEB(node->biLevel);
	    break;
	}

    lpTop.lpPageYTwips= drTwips.drY0;
    lpBelow.lpPageYTwips= drTwips.drY1;

    docGetPixelRectForPos( drPixels, lc,
			    drTwips.drX0, drTwips.drX1,
			    &lpTop, &lpBelow );
if  ( drPixels->drX0 > 0 )
drPixels->drX0--; /* rounding? */

  ready:

    docLayoutCleanBlockFrame( &bf );

    return;
    }

