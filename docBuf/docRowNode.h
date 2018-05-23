/************************************************************************/
/*									*/
/*  Text Editor Row specific definitions.				*/
/*									*/
/************************************************************************/

#   ifndef	DOC_ROW_NODE_H
#   define	DOC_ROW_NODE_H

#   include	<docLayoutPosition.h>
#   include	<docRowProperties.h>

/************************************************************************/
/*									*/
/*  Row specific information.						*/
/*									*/
/*  1)  The rtf file format does not really support tables. Keep an	*/
/*	administration of the set of rows that looks like a table.	*/
/*	First and past refer to similar rows. The header can come	*/
/*	before 'first'.							*/
/*  2)  Properties of the row.						*/
/*  3)  Cells with a rowspan can protrude below the actual row: To	*/
/*	optimize position lookups and to delimit areas to redraw	*/
/*	correctly, the position below all 'rowspan' columns is		*/
/*	remembered.							*/
/*  4)  If a row is a table header, it might be repeated on every page	*/
/*	where the table is continued. When the row is drawn on a	*/
/*	different page. No separate administration has to be kept to	*/
/*	handle the case where a row starts on a new page and the header	*/
/*	appears above it: It we make the header row behave as if it	*/
/*	has the (obsolete) \\trkeepfollow tag, it will appear in the	*/
/*	correct location in the normal text flow.			*/
/*  6)  The top position of the table header row that appears above an	*/
/*	ordinary row. (This must be kept with ordinary rows at the top	*/
/*	of a page.)							*/
/*  7)  A flag indicating that this row is preceded by a table header	*/
/*	that does not appear at its home position.			*/
/*  8)  Space to skip for borders. The top inset is also used		*/
/*	immediately after a real table row in plain text.		*/
/*  9)  Is this row going to be a table row? Only used while parsing	*/
/*	rtf. In a complete document biRowCellCount counts. (pun)	*/
/*									*/
/************************************************************************/

typedef struct RowNode
    {
				/*  1  */
    int				brTableHeaderRow;
    int				brTableFirst;
    int				brTablePast;

				/*  2  */
    RowProperties		brRowProperties;

				/*  3  */
    LayoutPosition		brBelowAllCellsPosition;

				/*  4,6  */
    LayoutPosition		brAboveHeaderPosition;

				/*  8  */
    short int			brTopInset;

				/*  4,7  */
    unsigned char		brPrecededByHeader;

				/*  9  */
    unsigned char		brForTable;
    } RowNode;

/************************************************************************/
/*									*/
/*  Shortcut defines throuw the BufferItem union.			*/
/*									*/
/************************************************************************/


#	define	biRowTopInset		BIU.biuRow.brTopInset

#	define	biRowTableHeaderRow	BIU.biuRow.brTableHeaderRow
#	define	biRowTableFirst		BIU.biuRow.brTableFirst
#	define	biRowTablePast		BIU.biuRow.brTablePast
#	define	biRowPrecededByHeader	BIU.biuRow.brPrecededByHeader
#	define	biRowBelowAllCellsPosition	BIU.biuRow.brBelowAllCellsPosition
#	define	biRowAboveHeaderPosition \
					BIU.biuRow.brAboveHeaderPosition
#	define	biRowForTable		BIU.biuRow.brForTable

#	define	biRowProperties		BIU.biuRow.brRowProperties

#	define	biRowHalfGapWidthTwips	biRowProperties.rpHalfGapWidthTwips
#	define	biRowLeftIndentTwips	biRowProperties.rpLeftIndentTwips
#	define	biRowHeightTwips	biRowProperties.rpHeightTwips
#	define	biRowIsTableHeader	biRowProperties.rpIsTableHeader
#	define	biRowKeepOnOnePage	biRowProperties.rpKeepOnOnePage
#	define	biRow_Keepfollow	biRowProperties.rp_Keepfollow
#	define	biRowAutofit		biRowProperties.rpAutofit
#	define	biRowCells		biRowProperties.rpCells
#	define	biRowCellCount		biRowProperties.rpCellCount
#	define	biRowAlignment		biRowProperties.rpAlignment

#	define	biRowFrameNumber	biRowProperties.rpFrameNumber
#	define	biRowTopBorderNumber	biRowProperties.rpTopBorderNumber
#	define	biRowBottomBorderNumber	biRowProperties.rpBottomBorderNumber
#	define	biRowLeftBorderNumber	biRowProperties.rpLeftBorderNumber
#	define	biRowRightBorderNumber	biRowProperties.rpRightBorderNumber
#	define	biRowVerticalBorderNumber \
					biRowProperties.rpVerticalBorderNumber
#	define	biRowHorizontalBorderNumber \
					biRowProperties.rpHorizontalBorderNumber

# define BI_ROW_IS_ONE_PAGE( rowBi ) \
		    RP_IS_ONE_PAGE( &((rowBi)->biRowProperties) )


#   endif
