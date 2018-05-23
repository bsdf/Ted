/************************************************************************/
/*									*/
/*  Lists: Diagnostic routines.						*/
/*									*/
/************************************************************************/

#   ifndef	DOC_DEBUG_LIST_NUMBER_TREE_H
#   define	DOC_DEBUG_LIST_NUMBER_TREE_H

#   include	"docListNumberTree.h"

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int docListListNumberNode(	const ListNumberTreeNode *	lntn );
extern int docCheckListNumberNode(	const ListNumberTreeNode *	lntn );

extern int docListNumberTreeLogPath(	ListNumberTreeNode **	path,
					int *			nums,
					int			level );

extern int docListNumberTreeFindParagraph(
					int *			pLevel,
					ListNumberTreeNode **	path,
					int *			nums,
					ListNumberTreeNode *	root,
					int			paraNr );

#   endif	/*	DOC_DEBUG_LIST_NUMBER_TREE_H	*/
