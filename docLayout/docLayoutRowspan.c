/************************************************************************/
/*									*/
/*  Layout of a table row in the document.				*/
/*  Setup rowspan related administration.				*/
/*									*/
/************************************************************************/

#   include	"docLayoutConfig.h"

#   include	<stddef.h>

#   include	"docLayout.h"
#   include	<docTreeNode.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Find the top of group of vertically merged cells.			*/
/*									*/
/*  1)  Look at the table rows above the cell in reverse order.		*/
/*  2)  Find a matching cell.						*/
/*									*/
/************************************************************************/

static void docLayoutTopOfRowspan(	ParagraphLayoutJob *	plj,
					BufferItem *		cellNode,
					const BufferItem *	rowNode,
					int			l,
					int			r )
    {
    int		topRow;
    int		row= rowNode->biNumberInParent;
    int		col= cellNode->biNumberInParent;

    /*  1  */
    for ( topRow= rowNode->biNumberInParent- 1;
	  topRow >= rowNode->biRowTableFirst;
	  topRow-- )
	{
	int			colspan;
	int			topCol;
	const BufferItem *	topRowNode;
	const CellProperties *	topCp;

	/*  2  */
	topRowNode= rowNode->biParent->biChildren[topRow];

	topCol= docGetMatchingCell( &colspan, topRowNode, l, r );
	if  ( topCol < 0 )
	    { LDEB(topCol); continue;	}

	topCp= topRowNode->biRowCells+ topCol;
	switch( topCp->cpVerticalMerge )
	    {
	    case CELLmergeFOLLOW:
		continue;

	    case CELLmergeHEAD:
		{
		const BufferItem *	topCellNode= topRowNode->biChildren[topCol];

		cellNode->biCellMergedCellTopRow= topRow;
		cellNode->biCellMergedCellTopCol= topCol;
		plj->cljMergedCellBelowPosion= topCellNode->biBelowPosition;
		}
		break;

	    default:
		LDEB(topCp->cpVerticalMerge);
		break;
	    }

	break;
	}

    if  ( cellNode->biCellMergedCellTopRow < 0 )
	{ LLLDEB(row,col,cellNode->biCellMergedCellTopRow);	}

    return;
    }

/************************************************************************/
/*									*/
/*  Calculate the layout of a row in the document.			*/
/*									*/
/*  3)  Columns with a colspan do not push the bottom of THIS row down.	*/
/*	Remember this.							*/
/*  4)  But in a cell with a rowspan, the top cell of the cell might	*/
/*	push the bottom down. Look for it.				*/
/*									*/
/************************************************************************/

void docRowLayoutRowspanAdmin(	BufferItem *			cellNode,
				const BufferItem *		rowNode,
				const BufferItem *		nextRowBi,
				int				nextRow,
				const CellProperties *		cp,
				int				l,
				int				r,
				ParagraphLayoutJob *		plj )

    {
    if  ( nextRow < rowNode->biRowTablePast )
	{
	int			nextColspan= 1;
	int			nextCol;

	nextCol= docGetMatchingCell( &nextColspan, nextRowBi, l, r );

	if  ( nextCol >= 0 )
	    {
	    const CellProperties *	nextCp;

	    nextCp= &(nextRowBi->biRowCells[nextCol]);

	    /*  3  */
	    if  ( cp->cpVerticalMerge == CELLmergeHEAD		&&
		  nextCp->cpVerticalMerge == CELLmergeFOLLOW	)
		{
		cellNode->biCellRowspan= docGetCellBottom(
					    (LayoutPosition *)0, cellNode );
		}

	    /*  4  */
	    if  ( cp->cpVerticalMerge == CELLmergeFOLLOW	&&
		  nextCp->cpVerticalMerge != CELLmergeFOLLOW	)
		{ docLayoutTopOfRowspan( plj, cellNode, rowNode, l, r ); }
	    }
	else{
	    if  ( cp->cpVerticalMerge == CELLmergeFOLLOW )
		{ docLayoutTopOfRowspan( plj, cellNode, rowNode, l, r ); }
	    }
	}
    else{
	/*  4  */
	if  ( cp->cpVerticalMerge == CELLmergeFOLLOW )
	    { docLayoutTopOfRowspan( plj, cellNode, rowNode, l, r );	}
	}
    }

