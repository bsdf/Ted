/************************************************************************/
/*									*/
/*  Document Layout and Printing related functionality.			*/
/*									*/
/************************************************************************/

#   ifndef	DOC_ROW_LAYOUT_H
#   define	DOC_ROW_LAYOUT_H

#   include	<docBuf.h>
#   include	<docBlockOrnaments.h>

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docGetCellOrnaments(
			BlockOrnaments *		cellOrnaments,
			DocumentRectangle *		drOutside,
			DocumentRectangle *		drInside,
			const DocumentRectangle *	drCell,
			const BufferDocument *		bd,
			const struct BufferItem *	rowBi,
			int				col,
			int				atRowTop,
			int				atRowBottom,
			int				drawTableGrid );

extern void docLayoutCalculateRowTopInset(
				int *				pInset,
				const BufferDocument *		bd,
				const struct BufferItem *	rowBi,
				int				atRowTop );

extern void docGetCellBottomBorder(
				BorderProperties *		pBpBottom,
				int *				pBpBottomNr,
				int *				pUseBelow,
				const BufferDocument *		bd,
				const struct BufferItem *	rowBi,
				int				col,
				int				atRowBottom );

#   endif	/*  DOC_ROW_LAYOUT_H  */
