/************************************************************************/
/*									*/
/*  Document Layout and Printing related functionality.			*/
/*									*/
/************************************************************************/

#   ifndef	DOC_PAGE_GRID_H
#   define	DOC_PAGE_GRID_H

#   include	"docBuf.h"
#   include	"docBlockFrame.h"
#   include	"docStripFrame.h"

/************************************************************************/

#   define	includeRectangleY( dr1, dr2 ) \
		{ \
		if  ( (dr1)->drY0 > (dr2)->drY0 )  \
		    { (dr1)->drY0=  (dr2)->drY0; } \
		if  ( (dr1)->drY1 < (dr2)->drY1 )  \
		    { (dr1)->drY1=  (dr2)->drY1; } \
		}

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docBlockFrameTwips(	BlockFrame *			bf,
				struct BufferItem *		bi,
				BufferDocument *		bd,
				int				page,
				int				column );

extern void docCellFrameTwips(	ParagraphFrame *		pf,
				const BlockFrame *		bf,
				const struct BufferItem *	cellNode );

extern void docCellRectangleTwips(
				DocumentRectangle *		drCell,
				const BlockFrame *		bf,
				const struct BufferItem *	cellNode );

extern void docLayoutSectColumnTop(
				LayoutPosition *		lpTop,
				BlockFrame *			bf,
				struct BufferItem *		bodySectNode,
				BufferDocument *		bd );

extern void docParaBlockFrameRectangle( DocumentRectangle *	dr,
					struct BufferItem *	paraBi,
					BufferDocument *	bd,
					int			page,
					int			column );

extern void docParaBlockFrameTwips(	BlockFrame *		bf,
					struct BufferItem *	paraBi,
					BufferDocument *	bd,
					int			page,
					int			column );

extern void docParagraphFrameTwips(	ParagraphFrame *	pf,
					const BlockFrame *	bf,
					const struct BufferItem * paraBi );

extern void docLayoutSetTextFrame(
				BlockFrame *			bfTextFrame,
				const LayoutPosition *		lpRef,
				const BlockFrame *		bfRef,
				const FrameProperties *		fp,
				int				frameHighVal );

extern void docLayoutFrameX(	BlockFrame *			bfTextFrame,
				int				xRefProp,
				int				xPosProp,
				int				xPosVal,
				int				layoutInCell,
				const ParagraphFrame *		pfRef,
				const BlockFrame *		bfRef,
				int				xChar,
				int				frameWide );

extern void docLayoutFrameY(	BlockFrame *			bfTextFrame,
				int				yRefProp,
				int				yPosProp,
				int				yPosVal,
				const LayoutPosition *		lpLineTop,
				const LayoutPosition *		lpParaTop,
				const BlockFrame *		bfRef,
				int				frameHighProp,
				int				frameHighVal );

#   endif	/*  DOC_PAGE_GRID_H  */
