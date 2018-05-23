/************************************************************************/
/*									*/
/*  Build trees to represent the hierarchy defined by the unicode	*/
/*  bidirectionals algorithm.						*/
/*  See: http://unicode.org/reports/tr9/				*/
/*									*/
/************************************************************************/

#   ifndef		TEXT_BIDI_TREE_BUILDER_H
#   define		TEXT_BIDI_TREE_BUILDER_H

#   include		"bidiTree.h"

typedef struct BidiTreeBuilder
    {
			/**
			 *  The root (bottom node) of the tree.
			 */
    BidiNode *		btbBottomNode;

			/**
			 *  The current explicit node.
			 *  (Initially, this is the root node.)
			 */
    BidiNode *		btbExplicitNode;

			/**
			 *  The insertion point. (Like a C string offset)
			 */
    int			btbOffset;

			/**
			 *  The higest level run found on the way.
			 */
    int			btbHighestLevel;
    } BidiTreeBuilder;

/************************************************************************/
/*									*/
/*  Routine declarations						*/
/*									*/
/************************************************************************/

extern void bidiInitTreeBuilder(	BidiTreeBuilder *	btb );

extern void bidiStartTreeBuilder(	BidiTreeBuilder *	btb,
					BidiNode *		bottom,
					int			embedding,
					int			level );

extern int bidiTreeBuilderAddRun(	BidiTreeBuilder *	btb,
					int			embedding,
					int			level,
					int			from,
					int			upto );

#   endif	/*	TEXT_BIDI_TREE_BUILDER_H	*/
