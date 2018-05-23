/************************************************************************/
/*									*/
/*  A node in the bidirectional tree.					*/
/*									*/
/************************************************************************/

#   ifndef		TEXT_BIDI_TREE_H
#   define		TEXT_BIDI_TREE_H

#   include		"bidiEmbedding.h"

typedef struct BidiNode
    {
			/**
			 *  String offset in bytes of the run.
			 *  Implicitly assumes UTF-8
			 */
    unsigned short	bnFrom;
			/**
			 *  Past the last byte in the run.
			 */
    unsigned short	bnUpto;

			/**
			 *  The parent node. Is NULL in the root of a tree.
			 */
    struct BidiNode *	bnParent;
			/**
			 *  The children (If any) of the node.
			 */
    struct BidiNode **	bnChildren;
			/**
			 *  The number of children of the node. 
			 */
    unsigned short	bnChildCount;
			/**
			 *  The index in the parents bnChildren array.
			 */
    unsigned short	bnNumberInParent;

			/**
			 *  The embedding direction of the associated text 
			 *  run.
			 */
    unsigned char	bnEmbedding;
			/**
			 *  The embedding level of the associated text run.
			 *  As levels never exceed 63, an unsigned char is 
			 *  largely sufficient.
			 */
    unsigned char	bnLevel;
    } BidiNode;

/************************************************************************/
/*									*/
/*  Routine declarations						*/
/*									*/
/************************************************************************/

extern void bidiInitTreeNode(		BidiNode *		bn );
extern void bidiCleanTreeNode(		BidiNode *		bn );

extern BidiNode * bidiNodeAddChild(	BidiNode *		parent,
					int			embedding,
					int			level,
					int			from,
					int			upto );

extern int bidiTraverseNode(		const BidiNode *	bn,
					int			from,
					int			upto,
					BidiGotLevel		gotLevel,
					void *			through );

extern void bidiListNode(		const BidiNode *	bn,
					const char *		symbols );

extern int bidiStretchNode(		BidiNode *		bn,
					int			upto );

#   endif	/*	TEXT_BIDI_TREE_H	*/
