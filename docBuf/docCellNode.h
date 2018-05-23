/************************************************************************/
/*									*/
/*  Text Editor Buffer structure of a (table) cell.			*/
/*									*/
/************************************************************************/

#   ifndef	DOC_CELL_NODE_H
#   define	DOC_CELL_NODE_H

/************************************************************************/
/*									*/
/*  A Cell.								*/
/*									*/
/************************************************************************/

typedef struct CellNode
    {
				/**
				 *  Used for the vertical alignment of
				 *  table cells: The first line of the 
				 *  first paragraph is shifted by this 
				 *  amount.
				 */
    int				bcTopInset;

				/**
				 *  Set by the formatter to know whether 
				 *  the cell is part of a rowspan. It is 
				 *  used to know when to calculate the 
				 *  vertical alignment of the cell and 
				 *  to reformat the correct rows after the 
				 *  height of the cell changes.
				 */
    int				bcRowspan;

				/**
				 *  The number of the row in the table that 
				 *  holds the top cell of a stack of cells 
				 *  in a rowspan.
				 *
				 *  This is determined while the row is
				 *  formatted. It is only set in the 
				 *  bottom cell of the rowspan.
				 */
    int				bcMergedCellTopRow;
				/**
				 *  The number of the column in the table that 
				 *  holds the top cell of a stack of cells 
				 *  in a rowspan.
				 *
				 *  This is determined while the row is
				 *  formatted. It is only set in the 
				 *  bottom cell of the rowspan.
				 */
    int				bcMergedCellTopCol;
    } CellNode;

/************************************************************************/
/*									*/
/*  Shortcut defines through the BufferItem union.			*/
/*									*/
/************************************************************************/

#   define	biCellTopInset		BIU.biuCell.bcTopInset
#   define	biCellRowspan		BIU.biuCell.bcRowspan
#   define	biCellMergedCellTopRow	BIU.biuCell.bcMergedCellTopRow
#   define	biCellMergedCellTopCol	BIU.biuCell.bcMergedCellTopCol

#   endif
