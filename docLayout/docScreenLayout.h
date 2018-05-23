/************************************************************************/
/*									*/
/*  Header file for geometry/formatting/drawing related functionality	*/
/*									*/
/************************************************************************/

#   ifndef	DOC_SCREEN_LAYOUT_H
#   define	DOC_SCREEN_LAYOUT_H

#   include	<docBuf.h>
#   include	"docLayout.h"
#   include	<utilIndexMapping.h>

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int docScreenTextWidth(		int			screenFont,
					const TextAttribute *	ta,
					const LayoutContext *	lc,
					const char *		s,
					int			len );

extern int docOpenScreenFont(	
			const LayoutContext *		lc,
			int				attributeNumber );

extern int docStartScreenLayoutForTree(
				LayoutJob *		lj,
				DocumentTree *		ei,
				int			page,
				int			column );

extern int docScreenLayoutOpenParaFonts(
				const LayoutContext *		lc,
				struct BufferItem *		paraNode );

extern int docScreenLayoutNode(	int *				pReachedBottom,
				struct BufferItem *		node,
				const LayoutContext *		lc,
				DocumentRectangle *		drChanged );

extern int docScreenLayoutDocumentBody(
				int *				pReachedBottom,
				BufferDocument *		bd,
				const LayoutContext *		lc );

extern void docScreenCloseObject(	const BufferDocument *		bd,
					const struct TextParticule *	tp );

extern void docSetScreenLayoutFunctions(	LayoutJob *	lj );

#   endif	/*  DOC_SCREEN_LAYOUT_H	*/
