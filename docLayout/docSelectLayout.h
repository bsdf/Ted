/************************************************************************/
/*									*/
/*  Select positions in a document, based on their geometric  position.	*/
/*									*/
/************************************************************************/

#   ifndef	DOC_SELECT_LAYOUT_H
#   define	DOC_SELECT_LAYOUT_H

#   include	<docBuf.h>

extern int docGetTopOfColumn(	DocumentPosition *		dp,
				int *				pLineTop,
				int *				pPartTop,
				BufferDocument *		bd,
				int				page,
				int				column );

extern int docGetFirstInColumnForNode(
				DocumentPosition *		dp,
				int *				pLineTop,
				int *				pPartTop,
				struct BufferItem *		bi,
				int				page,
				int				column );

extern int docGetBottomOfColumn(DocumentPosition *		dp,
				int *				pPartBot,
				BufferDocument *		bd,
				int				page,
				int				column );

extern int docGetLastInColumnForNode(
				DocumentPosition *		dp,
				int *				pLineBot,
				int *				pPartBot,
				struct BufferItem *		bi,
				int				page,
				int				column );

extern int docPositionNearHeadFoot( DocumentPosition *		dpNew,
				int				treeType,
				struct BufferItem *		bodySectNode,
				BufferDocument *		bd,
				int				page );

#   endif	/*  DOC_SELECT_LAYOUT_H  */
