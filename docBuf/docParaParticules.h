/************************************************************************/
/*									*/
/*  Manage the particules and lines in a paragraph.			*/
/*									*/
/************************************************************************/

#   ifndef	DOC_PARA_PARICULES_H
#   define	DOC_PARA_PARICULES_H

#   include	"docBuf.h"
#   include	<docTextParticule.h>
#   include	<docTextLine.h>
#   include	"docTreeNode.h"

		/**
		  * Find the first particule that containd this position
		  */
#   define	PARAfindFIRST	0
		/**
		  * Find the last particule that containd this position
		  */
#   define	PARAfindLAST	1
		/**
		  * Find the last particule that contains this position
		  * Or.. If it is the last position in the paragraph,
		  * return the position past the last particule.
		  */
#   define	PARAfindPAST	2

/************************************************************************/

			/**
			 *  The position is at the head of the particule.
			 */
#   define	POSflagPART_HEAD	0x01
			/**
			 *  The position is at the tail of the particule.
			 */
#   define	POSflagPART_TAIL	0x02
			/**
			 *  The position is immediately after a line or page
			 *  break.
			 */
#   define	POSflagPART_AFTER_BREAK	0x04

			/**
			 *  The position is at the head of the line.
			 */
#   define	POSflagLINE_HEAD	0x08
			/**
			 *  The position is at the tail of the line.
			 */
#   define	POSflagLINE_TAIL	0x10

			/**
			 *  The position is at the head of the paragraph.
			 */
#   define	POSflagPARA_HEAD	0x20
			/**
			 *  The position is at the tail of the paragraph.
			 */
#   define	POSflagPARA_TAIL	0x40

			/**
			 *  Used when a position is compared to a selection.
			 *  The flag is set if the position is in the selection.
			 */
#   define	POSflagPARA_FOUND	0x80

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int docFindParticuleOfPosition(
				int *				pPart,
				int *				pFlags,
				const DocumentPosition *	dp,
				int				lastOne );

extern int docFindLineOfPosition( int *				pLine,
				int *				pFlags,
				const DocumentPosition *	dp,
				int				lastOne );

extern void docInitParaNode(	BufferItem *			paraBi );
extern int docDeleteEmptySpan(	BufferItem *			paraBi );

extern void docCleanParaNode(	BufferDocument *		bd,
				DocumentTree *			dt,
				BufferItem *			paraBi );

extern TextParticule *	docInsertParticules(	BufferItem *	paraBi,
						int		part,
						int		count );

extern void docDeleteParticules(	BufferItem *	bi,
					int		first,
					int		count );

extern int docInsertAdminParticule(	BufferDocument *	bd,
					BufferItem *		paraBi,
					int			n,
					int			off,
					int			objectNumber,
					int			kind,
					const TextAttribute *	ta );

extern TextParticule *	docInsertTextParticule(	BufferItem *	bi,
						int		n,
						int		off,
						int		len,
						int		kind,
						int		textAttrNr );

extern TextParticule * docMakeSpecialParticule(
				BufferItem *			paraBi,
				int				n,
				int				stroff,
				int				kind,
				int				textAttrNr );

extern int docSaveSpecialParticule(	BufferDocument *	bd,
					BufferItem *		paraBi,
					const TextAttribute *	ta,
					int			kind );

extern int docShiftParticuleOffsets(	BufferDocument *	bd,
					BufferItem *		paraBi,
					int			partFrom,
					int			partUpto,
					int			stroffShift );

extern int docSplitTextParticule( TextParticule **		pTpPart,
				TextParticule **		pTpNext,
				BufferItem *			paraBi,
				int				part,
				int				stroff );

extern TextLine * docInsertTextLine(	BufferItem *	bi,
					int		line );

extern TextParticule *	docAppendObject(	BufferDocument *	bd,
						BufferItem *		paraNode,
						const TextAttribute *	ta );

extern void docCleanParticuleObject(	BufferDocument *	bd,
					TextParticule *		tp );

extern int docIntersectSelectionWithParagraph(
				    DocumentSelection *		dsPara,
				    int *			pPartFrom,
				    int *			pPartUpto,
				    int *			pHeadFlags,
				    int *			pTailFlags,
				    const BufferItem *		paraNode,
				    const DocumentSelection *	ds );

#   endif		/*  DOC_PARA_PARICULES_H	*/
