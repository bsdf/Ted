/************************************************************************/
/*									*/
/*  Manipulation of the node tree and navigation over it.		*/
/*									*/
/************************************************************************/

#   ifndef	DOC_NODE_TREE_H
#   define	DOC_NODE_TREE_H

/************************************************************************/
/*									*/
/*  Forward declarations of structs.					*/
/*									*/
/************************************************************************/

struct BufferDocument;
struct BufferItem;
struct DocumentTree;
struct RowProperties;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docFreeNode(	struct BufferDocument *	bd,
				struct DocumentTree *		dt,
				struct BufferItem *		bi );

extern struct BufferItem * docInsertNode( const struct BufferDocument *	bd,
					struct BufferItem *		parent,
					int				n,
					int				level );

extern void docDeleteNodes(		struct BufferDocument *	bd,
					struct DocumentTree *	dt,
					struct BufferItem *	bi,
					int			first,
					int			count );

extern int docSplitGroupNode(		struct BufferDocument *	bd,
					struct BufferItem **	pNewBi,
					struct BufferItem *	oldBi,
					int			n );

extern int docSplitGroupNodeAtLevel(	struct BufferDocument *	bd,
					struct BufferItem **	pBeforeNode,
					struct BufferItem **	pAfterNode,
					struct BufferItem *	splitNode,
					int			n,
					int			level );

extern int docMergeGroupNodes(		struct BufferItem *	to,
					struct BufferItem *	from );

extern struct BufferItem * docNextParagraph(	struct BufferItem *	bi );
extern struct BufferItem * docPrevParagraph(	struct BufferItem *	bi );

extern struct BufferItem * docNextSection(	struct BufferItem *	bi );
extern struct BufferItem * docPrevSection(	struct BufferItem *	bi );

extern void docDeleteNode(		struct BufferDocument *		bd,
					struct DocumentTree *		dt,
					struct BufferItem *		bi );

extern void docInitNode( struct BufferItem *		bi,
			struct BufferItem *		parent,
			const struct BufferDocument *	bd,
			int				numberInParent,
			int				level,
			int				treeType );

extern struct BufferItem * docInsertRowNode(
				struct BufferDocument *		bd,
				struct BufferItem *		sectBi,
				int				n,
				const struct RowProperties *	rp,
				int				textAttrNr );

extern struct BufferItem * docGetCellNode(	struct BufferItem *	bi );
extern struct BufferItem * docGetRowNode(	struct BufferItem *	bi );
extern struct BufferItem * docGetRowLevelNode(	struct BufferItem *	bi );
extern struct BufferItem * docGetSectNode(	struct BufferItem *	bi );

extern int docRowNesting(		const struct BufferItem *	bi );
extern int docTableNesting(		const struct BufferItem *	bi );
extern void docSetParaTableNesting(	struct BufferItem *	paraNode );

extern int docValidChildLevel(		int		parentLevel,
					int		childLevel );

extern struct BufferItem * docMakeNode( void );

extern struct BufferItem * docGetBodySectNode(
					struct BufferItem *		bi,
					const struct BufferDocument *	bd );

#   endif
