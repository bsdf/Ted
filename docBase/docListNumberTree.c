/************************************************************************/
/*									*/
/*  Manage the numbers in bulleted and numbered lists.			*/
/*  The same mechanism is used to manage the tree of outline levels of	*/
/*  paragraphs.								*/
/*									*/
/*  Indentation levels in the tree (Distance from the root) correspond	*/
/*  to the ilvl	or outlinelevel value of the paragraphs.		*/
/*									*/
/************************************************************************/

# if 0

In a document that follows the strict hierarchy, the approach is simple:
-   The tree holds nodes for all paragraphs at indentation level 0. (ilvl= 0)
    The ilvl= 0 paragraphs are stored as an array of children of the root 
    node. The array of child nodes is sorted on ascending paragraph number.
-   All paragraphs with an ilvl > 0 correspond to direct child node 
    of a node for a paragraph with ilvl- 1. The array of child nodes 
    is sorted on ascending paragraph number.

Now in practice, documents do not always follow the strict hierarchy,:
Levels are skipped, or the document starts with an indentation level 
other than ilvl= 0.
-   To accomodate for this situation, we insert extra nodes in the tree.
    They have a paragraph number of paraNr= -1.
-   In this way, the path to a none at ilvl= n, always has length n.
-   As the mechanism only exists to keep the administration of 
    skipped levels, and we want the children of every node to be sorted,
    only the first child of a node can be a dummy node with paraNr= -1.

# endif

#   include	"docBaseConfig.h"

#   include	<stdlib.h>

#   include	<appDebugon.h>

#   include	"docListNumberTree.h"
#   include	"docListDepth.h"
#   include	"docDebugListNumberTree.h"

#   define	LOG_TRANSACTIONS	0

/************************************************************************/
/*									*/
/*  Initialise a node.							*/
/*									*/
/************************************************************************/

void docInitListNumberTreeNode(	ListNumberTreeNode *	lntn )
    {
    lntn->lntnParaNr= -1;
    lntn->lntnChildren= (ListNumberTreeNode **)0;
    lntn->lntnChildCount= 0;

    return;
    }

/************************************************************************/
/*									*/
/*  Clean and free an array of nodes.					*/
/*									*/
/************************************************************************/

static void docFreeListNumberNode(	ListNumberTreeNode *	lntn )
    {
    docCleanListNumberTreeNode( lntn );

    free( lntn );

    return;
    }

void docCleanListNumberTreeNode(	ListNumberTreeNode *	lntn )
    {
    int		i;

    for ( i= 0; i < lntn->lntnChildCount; i++ )
	{
	if  ( lntn->lntnChildren[i] )
	    { docFreeListNumberNode( lntn->lntnChildren[i] );	}
	}

    if  ( lntn->lntnChildren )
	{ free( lntn->lntnChildren );	}

    return;
    }

/************************************************************************/
/*									*/
/*  Allocate nodes.							*/
/*									*/
/************************************************************************/

static int docClaimListNumberTreeNodes(	ListNumberTreeNode **	pLntn,
					int *			pCount,
					int			count )
    {
    ListNumberTreeNode *	fresh;
    int				i;

    if  ( *pCount >= count )
	{ LLDEB(*pCount,count); return 0;	}

    fresh= (ListNumberTreeNode *)realloc( *pLntn,
				    count* sizeof( ListNumberTreeNode ) );
    if  ( ! fresh )
	{ LXDEB(count,fresh); return -1;	}

    for ( i= *pCount; i < count; i++ )
	{ docInitListNumberTreeNode( &(fresh[i]) );	}

    *pLntn= fresh;
    *pCount= count;

    return 0;
    }

int docClaimListNumberTrees(	ListNumberTrees *	lnt,
				int			count )
    {
    return docClaimListNumberTreeNodes( &(lnt->lntTrees),
						&(lnt->lntTreeCount), count );
    }

ListNumberTreeNode * docGetListNumberTree(	ListNumberTrees *	lnt,
						int			ls )
    {
    if  ( ls >= lnt->lntTreeCount			&&
	  docClaimListNumberTrees( lnt, ls+ 1 )		)
	{ LDEB(ls); return (ListNumberTreeNode *)0;	}

    return lnt->lntTrees+ ls;
    }

void docInitListNumberTrees(	ListNumberTrees *	lnt )
    {
    lnt->lntTrees= (ListNumberTreeNode *)0;
    lnt->lntTreeCount= 0;
    }

void docCleanListNumberTrees(	ListNumberTrees *	lnt )
    {
    int		i;

    for ( i= 0; i < lnt->lntTreeCount; i++ )
	{ docCleanListNumberTreeNode( &(lnt->lntTrees[i]) );	}

    if  ( lnt->lntTrees )
	{ free( lnt->lntTrees );	}

    return;
    }

/************************************************************************/

static int docListNumberTreeFindChild(	const ListNumberTreeNode *	node,
					int				paraNr )
    {
    int		l= 0;
    int		r= node->lntnChildCount;
    int		m= ( l+ r )/ 2;

    /*  2  */
    while( l < m )
	{
	if  ( node->lntnChildren[m]->lntnParaNr <= paraNr )
	    { l= m;	}
	else{ r= m;	}

	m= ( l+ r )/ 2;
	}

    if  ( paraNr < node->lntnChildren[m]->lntnParaNr )
	{ m--;	}

    return m;
    }

/************************************************************************/
/*									*/
/*  Find a paragraph in the number tree.				*/
/*									*/
/*  1)  From root to leaves..						*/
/*  2)  Look for a child that is on, or before the paragraph that we	*/
/*	look for. (<=)							*/
/*  3)  Found?								*/
/*  4)  Enter the child before the position found, or if there is none,	*/
/*	report this as the possible insertion point.			*/
/*  5)  Not found.. Report where.					*/
/*									*/
/*  path is the series of parent nodes that leads us to the node.	*/
/*  nums is the series of child node numbers of the corresponding	*/
/*	positions in 'path'. If the node is found, the value is the	*/
/*	index of the node. If nothing is found, the value is the index	*/
/*	where it would be inserted at that particular level.		*/
/*									*/
/************************************************************************/

int docListNumberTreeFindParagraph(	int *			pLevel,
					ListNumberTreeNode **	path,
					int *			nums,
					ListNumberTreeNode *	root,
					int			paraNr )
    {
    ListNumberTreeNode *	node= root;
    int				level= 0;

    if  ( paraNr < 0 )
	{ /*LDEB(paraNr);*/ return 1;	}

    if  ( path )
	{ path[0]= node;	}
    nums[0]= -1;

    /*  1  */
    while( node->lntnChildCount > 0 )
	{
	int	m= docListNumberTreeFindChild( node, paraNr );

	/*  3  */
	if  ( m >= 0						&&
	      node->lntnChildren[m]->lntnParaNr == paraNr	)
	    {
	    nums[level]= m;
	    if  ( path )
		{ path[level+1]= node->lntnChildren[m];	}
	    *pLevel= level;
	    return 0;
	    }

	/*  4,5  */
	nums[level]= m;
	if  ( m < 0 )
	    {
	    if  ( path )
		{ path[level+1]= (ListNumberTreeNode *)0;	}

	    *pLevel= level;
	    return 1;
	    }

	if  ( m >= node->lntnChildCount )
	    {
	    LLLDEB(level,m,node->lntnChildCount);
	    return -1;
	    }

	node= node->lntnChildren[m];
	if  ( path )
	    { path[level+1]= node;	}

	level++;
	}

    /*  5  */
    *pLevel= level;
    return 1;
    }

/************************************************************************/
/*									*/
/*  Determine the list numbers on the path to a certain paragraph.	*/
/*									*/
/************************************************************************/

int docListNumberTreeGetNumberPath(	int *			nums,
					ListNumberTreeNode *	root,
					int			ilvl,
					int			paraNr )
    {
    int				level= 0;

    /*
    if  ( ilvl < 0 || ilvl >= DLmaxLEVELS )
	{ LLDEB(ilvl,DLmaxLEVELS); return -1;	}
    */

    /* LLDEB(ilvl,paraNr);docListListNumberNode(root); */

    if  ( docListNumberTreeFindParagraph( &level,
			    (ListNumberTreeNode **)0, nums, root, paraNr ) )
	{
	/* LDEB(paraNr); docListListNumberNode(root); */
	return -1;
	}

    if  ( level != ilvl )
	{ LLDEB(level,ilvl); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Determine the list numbers on the path to the predecessor of a	*/
/*  certain paragraph.							*/
/*									*/
/*  1)  Truncate internal nodes from the end of the path.		*/
/*  2)  If we find a leaf paragraph on the path.. That is the		*/
/*	predecessor.							*/
/*  3)  Nothing left: No predecessor.					*/
/*  4)  Switch to the previous paragraph in the parent.			*/
/*  5)  And navigate to the last node in its offspring.			*/
/*									*/
/************************************************************************/

static int docListNumberTreePrevPath(	int *			nums,
					int *			pLevel,
					ListNumberTreeNode **	path,
					int			level )
    {
    int				ilvl= level;
    ListNumberTreeNode *	node;

    /*  1  */
    while( ilvl >= 0 && nums[ilvl] == 0 )
	{
	/*  2  */
	if  ( path[ilvl]->lntnParaNr >= 0 )
	    {
	    *pLevel= ilvl- 1;
	    return 0;
	    }

	ilvl--;
	}

    /*  3  */
    if  ( ilvl < 0 )
	{ return 1;	}

    /*  4  */
    node= path[ilvl];
    nums[ilvl]--;

    /*  5  */
    node= node->lntnChildren[nums[ilvl]];
    ilvl++;
    while( node->lntnChildCount > 0 )
	{
	nums[ilvl]= node->lntnChildCount- 1;
	node= node->lntnChildren[nums[ilvl]];
	ilvl++;
	}

    *pLevel= ilvl- 1;
    return 0;
    }

/*  2  */
int docListNumberTreeGetPrevPath(	int *			nums,
					int *			pLevel,
					ListNumberTreeNode *	root,
					int			paraNr )
    {
    ListNumberTreeNode *	path[DLmaxLEVELS+1];
    int				level= 0;

    /* SLDEB("PREV",paraNr);docListListNumberNode(root); */

    if  ( docListNumberTreeFindParagraph( &level, path, nums, root, paraNr ) )
	{ LDEB(paraNr); return -1;	}

    return docListNumberTreePrevPath( nums, pLevel, path, level );
    }

/************************************************************************/
/*									*/
/*  Move child nodes from one parent to another.			*/
/*									*/
/*  1)  Allocate memory.						*/
/*  2)  Open hole.							*/
/*  3)  Move nodes.							*/
/*  4)  Close hole.							*/
/*  5)  Fix counts.							*/
/*									*/
/************************************************************************/

static int docListNumberTreeMoveChildren(	ListNumberTreeNode *	to,
						int			topos,
						ListNumberTreeNode *	fr,
						int			frpos,
						int			cnt )
    {
    int				i;
    ListNumberTreeNode **	fresh;

#   if LOG_TRANSACTIONS
    appDebug( "MOVE %d CHILDREN FROM %d[%d/%d] TO %d[%d/%d]\n", cnt,
			    fr->lntnParaNr, frpos, fr->lntnChildCount,
			    to->lntnParaNr, topos, to->lntnChildCount );
#   endif

    /*  1  */
    fresh= realloc( to->lntnChildren,
		( to->lntnChildCount+ cnt )* sizeof( ListNumberTreeNode * ) );
    if  ( ! fresh )
	{ LXDEB(to->lntnChildCount+ cnt,fresh); return -1;	}
    to->lntnChildren= fresh;
    to->lntnChildCount= to->lntnChildCount+ cnt;

    /*  2  */
    for ( i= to->lntnChildCount- 1; i >= topos+ cnt; i-- )
	{
	to->lntnChildren[i]= to->lntnChildren[i- cnt];
	}

    /*  3  */
    for ( i= 0; i < cnt; i++ )
	{
#	if LOG_TRANSACTIONS
	appDebug( "    MOVE %d FROM %d[%d/%d] TO %d[%d/%d]\n",
			fr->lntnChildren[frpos+ i]->lntnParaNr,
			fr->lntnParaNr, frpos+ i, fr->lntnChildCount,
			to->lntnParaNr, topos+ i, to->lntnChildCount );
#	endif

	to->lntnChildren[topos+ i]= fr->lntnChildren[frpos+ i];
	}

    /*  4  */
    if  ( fr->lntnChildCount > frpos+ cnt )
	{
	for ( i= 0; i < cnt; i++ )
	    { fr->lntnChildren[frpos+ i]= fr->lntnChildren[frpos+ i+ cnt]; }
	}

    /*  5  */
    fr->lntnChildCount -= cnt;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Insert a child into a parent node.					*/
/*									*/
/*  1)  Allocate memory.						*/
/*  2)  Open hole.							*/
/*  3)  Initialize fresh child.						*/
/*									*/
/************************************************************************/

static ListNumberTreeNode * docListNumberTreeInsertChild(
					ListNumberTreeNode *	node,
					int			pos,
					int			paraNr )
    {
    int				i;
    ListNumberTreeNode **	fresh= (ListNumberTreeNode **)0;
    ListNumberTreeNode *	child= (ListNumberTreeNode *)0;

#   if LOG_TRANSACTIONS
    appDebug("INSERT %d[%d] = %d\n", node->lntnParaNr, pos, paraNr );
#   endif

    child= malloc( sizeof(ListNumberTreeNode) );
    if  ( ! child )
	{ XDEB(child); return (ListNumberTreeNode *)0;	}

    /*  3  */
    docInitListNumberTreeNode( child );

    child->lntnParaNr= paraNr;

    /*  1  */
    fresh= realloc( node->lntnChildren,
		( node->lntnChildCount+ 1 )* sizeof( ListNumberTreeNode * ) );
    if  ( ! fresh )
	{
	LXDEB(node->lntnChildCount+ 1,fresh);
	free( child );
	return (ListNumberTreeNode *)0;
	}
    node->lntnChildren= fresh;
    node->lntnChildCount++;

    /*  2  */
    for ( i= node->lntnChildCount- 1; i > pos; i-- )
	{ node->lntnChildren[i]= node->lntnChildren[i-1];	}

    node->lntnChildren[pos]= child;

    return child;
    }

/************************************************************************/
/*									*/
/*  Delete a child node from a parent.					*/
/*									*/
/************************************************************************/

static void docListNumberTreeDeleteChild(	ListNumberTreeNode *	parent,
						int			pos )
    {
    int	child;

#   if LOG_TRANSACTIONS
    appDebug("REMOVE %d[%d] = %d (%d children)\n",
	    parent->lntnParaNr, pos,
	    parent->lntnChildren[pos]->lntnParaNr,
	    parent->lntnChildren[pos]->lntnChildCount );
#   endif

    docFreeListNumberNode( parent->lntnChildren[pos] );
    parent->lntnChildCount--;

    for ( child= pos; child < parent->lntnChildCount; child++ )
	{ parent->lntnChildren[child]= parent->lntnChildren[child+ 1];	}

    return;
    }

/************************************************************************/
/*									*/
/*  Extend a path in a list number tree to reach the desired level.	*/
/*									*/
/*  path, nums and level have been obtained by a miss call to		*/
/*	docListNumberTreeFindParagraph(). That means that the values in	*/
/*	path and nums are set upto level.				*/
/*									*/
/************************************************************************/

static int docListNumberTreeExtendPath(	
				    ListNumberTreeNode **	path,
				    int	*			nums,
				    int				paraNr,
				    int				level,
				    int				ilvl )
    {
    ListNumberTreeNode *	fresh;

    if  ( level >= ilvl )
	{ LLDEB(level,ilvl); return -1;	}

    while( level <= ilvl )
	{
	int			pos= 0;

	fresh= docListNumberTreeInsertChild( path[level], pos, -1 );
	if  ( ! fresh )
	    { LXDEB(pos,fresh); return -1;	}

	level++;

	path[level]= fresh;
	nums[level]= pos;
	}

    path[level]->lntnParaNr= paraNr;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Insert a paragraph in a list number tree.				*/
/*									*/
/*  1)  Find a path to the insertion point.				*/
/*  2)  If it does not go up do the desired level, extend the tree upto	*/
/*	the desired level.						*/
/*  3)  Insert the new leaf at the correct position.			*/
/*  4)  Split deeper levels if necessary.				*/
/*									*/
/************************************************************************/

int docListNumberTreeInsertParagraph(	ListNumberTreeNode *	root,
					int			ilvl,
					int			paraNr )
    {
    ListNumberTreeNode *	path[DLmaxLEVELS+1];
    int				nums[DLmaxLEVELS+1];

    int				level= 0;

    int				res;

#   if LOG_TRANSACTIONS
    SLLDEB("ADD",ilvl,paraNr);docListListNumberNode(root);
#   endif

    for ( res= 0; res <= DLmaxLEVELS; res++ )
	{ path[res]= (ListNumberTreeNode *)0; nums[res]= -2;	}

    if  ( ilvl < 0 || ilvl >= DLmaxLEVELS )
	{ LLDEB(ilvl,DLmaxLEVELS); return -1;	}

    /*  1  */
    res= docListNumberTreeFindParagraph( &level, path, nums, root, paraNr );
    if  ( res <= 0 )
	{ LLDEB(paraNr,res); docListListNumberNode( root ); return -1; }

    if  ( level < 0 )
	{ LDEB(level); return -1;	}

#   if LOG_TRANSACTIONS
    SLLDEB("PATH",level,ilvl); docListNumberTreeLogPath( path, nums, level );
#   endif

    /*  2  */
    if  ( level < ilvl )
	{
	if  ( docListNumberTreeExtendPath( path, nums, paraNr, level, ilvl ) )
	    { LLDEB(level,ilvl);	}

	return 0;
	}

    if  ( level == ilvl )
	{
	if  ( ilvl == 0 )
	    {
	    ListNumberTreeNode *	tgt= path[ilvl];
	    int				pos= 0;
	    ListNumberTreeNode *	fresh;

	    fresh= docListNumberTreeInsertChild( tgt, pos, paraNr );
	    if  ( ! fresh )
		{ XDEB(fresh); return -1;	}

#	    if LOG_TRANSACTIONS
	    SLLDEB("ADDED",ilvl,paraNr);
#	    endif

	    return 0;
	    }
	else{
	    ListNumberTreeNode *	tgt= path[ilvl-1];
	    int				pos= nums[ilvl-1];
	    ListNumberTreeNode *	fresh;

	    fresh= docListNumberTreeInsertChild(
				    tgt->lntnChildren[pos], 0, paraNr );
	    if  ( ! fresh )
		{ XDEB(fresh); return -1;	}

#	    if LOG_TRANSACTIONS
	    SLLDEB("ADDED",ilvl,paraNr);
#	    endif

	    return 0;
	    }
	}

	{
	ListNumberTreeNode *	tgt= path[ilvl];
	int			pos= nums[ilvl];
	ListNumberTreeNode *	fresh;

	int			splitLevel= ilvl;
	int			lev;

	int			cfrpos= nums[level-1];
	ListNumberTreeNode *	cfr= path[level-1];
	ListNumberTreeNode *	cfch= cfr->lntnChildren[cfrpos];

	for ( lev= ilvl+1; lev < level; lev++ )
	    {
	    int				lfrpos= nums[lev];
	    ListNumberTreeNode *	lfr= path[lev];

	    if  ( lfrpos+ 1 < lfr->lntnChildCount )
		{ splitLevel= lev;	}
	    }

	if  ( cfch->lntnChildCount > 0 )
	    { splitLevel= level- 1;	}

	if  ( ( ( cfch->lntnParaNr >= 0 || pos > 0 )	&&
	      cfch->lntnParaNr < paraNr )		)
	    { pos++;	}

	if  ( pos == 0					&&
	      tgt->lntnChildren[pos]->lntnParaNr == -1	)
	    {
	    tgt->lntnChildren[pos]->lntnParaNr= paraNr;

#	    if LOG_TRANSACTIONS
	    SLLDEB("ADDED",ilvl,paraNr);
#	    endif

	    return 0;
	    }
	else{
	    fresh= docListNumberTreeInsertChild( tgt, pos, paraNr );
	    if  ( ! fresh )
		{ XDEB(fresh); return -1;	}

	    if  ( splitLevel > ilvl )
		{
		int			ifrpos= nums[ilvl+ 1];
		ListNumberTreeNode *	ifr= path[ilvl+ 1];

		if  ( docListNumberTreeMoveChildren( fresh, 0,
			    ifr, ifrpos+ 1, ifr->lntnChildCount- ifrpos- 1 ) )
		    { LDEB(ifr->lntnChildCount); return -1;	}

		for ( lev= ilvl+ 1; lev <= splitLevel; lev++ )
		    {
		    int				shift= 0;
		    int				lfrpos= nums[lev];
		    ListNumberTreeNode *	lfr= path[lev];

#		    if 0
		    ListNumberTreeNode *	lfch= lfr->lntnChildren[lfrpos];

		    appDebug( "L=%d@%d/%d:P=%d BRANCH\n", lev, lfrpos,
						    lfr->lntnChildCount,
						    lfch->lntnParaNr );
#		    endif

		    if  ( lev < splitLevel || cfch->lntnChildCount > 0 )
			{
			ListNumberTreeNode *	f;

			f= docListNumberTreeInsertChild( fresh, 0, -1 );
			if  ( ! f )
			    { XDEB(f); return -1;	}

			shift++;
			}

		    if  ( docListNumberTreeMoveChildren( fresh, shift,
			    lfr, lfrpos+ 1, lfr->lntnChildCount- lfrpos- 1 ) )
			{ LDEB(lfr->lntnChildCount); return -1;	}

		    if  ( fresh->lntnChildCount == 0 )
			{ LDEB(fresh->lntnChildCount); return -1;	}

		    fresh= fresh->lntnChildren[0];
		    }

		if  ( cfch->lntnChildCount > 0 )
		    {
		    if  ( docListNumberTreeMoveChildren( fresh, 0,
					    cfch, 0, cfch->lntnChildCount ) )
			{ LDEB(cfch->lntnChildCount); return -1;	}

		    if  ( cfch->lntnParaNr < 0 )
			{ docListNumberTreeDeleteChild( cfr, 0 );	}
		    }

		for ( lev= splitLevel; lev >= ilvl+ 1; lev-- )
		    {
		    int				lfrpos= nums[lev];
		    ListNumberTreeNode *	lfr= path[lev];
		    
		    if  ( lfrpos == 0				&&
			  lfr->lntnChildren[0]->lntnParaNr < 0	&&
			  lfr->lntnChildren[0]->lntnChildCount == 0	)
			{ docListNumberTreeDeleteChild( lfr, 0 );	}
		    }
		}

#	    if LOG_TRANSACTIONS
	    SLLDEB("ADDED",ilvl,paraNr);
#	    endif

	    return 0;
	    }
	}
    }

int docListNumberTreesInsertParagraph(	ListNumberTrees *	lnt,
					int			ls,
					int			ilvl,
					int			paraNr )
    {
    if  ( ls < 0 )
	{ LLDEB(ls,lnt->lntTreeCount); return -1;	}

    if  ( ls >= lnt->lntTreeCount			&&
	  docClaimListNumberTrees( lnt, ls+ 1 )		)
	{ LDEB(ls); return -1;	}

    return docListNumberTreeInsertParagraph( lnt->lntTrees+ ls, ilvl, paraNr );
    }

/************************************************************************/
/*									*/
/*  Move the children of a node that we want to delete to a previous	*/
/*  one.								*/
/*									*/
/************************************************************************/

static int docListNumberTreeMoveChildrenBack(	ListNumberTreeNode *	prev,
						ListNumberTreeNode *	node )
    {
    while( node->lntnChildCount > 0 )
	{
	int		last= prev->lntnChildCount-1;

	if  ( prev->lntnChildCount == 0 )
	    {
	    if  ( docListNumberTreeMoveChildren( prev, prev->lntnChildCount,
					    node, 0, node->lntnChildCount ) )
		{ LDEB(node->lntnChildCount); return -1;	}

	    return 0;
	    }

	if  ( node->lntnChildren[0]->lntnParaNr >= 0 )
	    {
	    if  ( docListNumberTreeMoveChildren( prev, prev->lntnChildCount,
					    node, 0, node->lntnChildCount ) )
		{ LDEB(node->lntnChildCount); return -1;	}

	    return 0;
	    }

	if  ( docListNumberTreeMoveChildren( prev, prev->lntnChildCount,
					node, 1, node->lntnChildCount- 1 ) )
	    { LDEB(node->lntnChildCount); return -1;	}

	prev= prev->lntnChildren[last];
	node= node->lntnChildren[0];
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Delete a paragraph from the number tree.				*/
/*									*/
/*  1)  Look for the node.						*/
/*  2)  Demote the node to an internal node.				*/
/*  3)  For every node on the path to the root:				*/

/*  3)  Try to merge children into predeceding nodes.			*/
/*  4)  Look for the top level of the merge.				*/
/*  5)  Recursively merge all children into predecessor nodes.		*/
/*  6)  If no children are left, remove the node from the tree.		*/
/*  7)  Remove empty parents (unless a leaf!)				*/
/*									*/
/************************************************************************/

int docListNumberTreeDeleteParagraph(	ListNumberTreeNode *	root,
					int			paraNr )
    {
    ListNumberTreeNode *	path[DLmaxLEVELS+1];
    int				nums[DLmaxLEVELS+1];

    int				level;
    int				lev;

    int				cfrpos;
    ListNumberTreeNode *	cfr;
    ListNumberTreeNode *	cfch;

#   if LOG_TRANSACTIONS
    SLDEB("DEL",paraNr);docListListNumberNode(root);
#   endif

    /*  1  */
    if  ( docListNumberTreeFindParagraph( &level, path, nums, root, paraNr ) )
	{ /*LDEB(paraNr);*/ return -1;	}

#   if LOG_TRANSACTIONS
    SLDEB("@@",level); docListNumberTreeLogPath( path, nums, level+ 1 );
#   endif

    cfrpos= nums[level];
    cfr= path[level];
    cfch= cfr->lntnChildren[cfrpos];

    /*  2  */
    if  ( cfch->lntnParaNr != paraNr )
	{ LLLDEB(level,cfch->lntnParaNr,paraNr); return -1;	}

    cfch->lntnParaNr= -1;

    if  ( cfrpos > 0 && cfch->lntnChildCount > 0 )
	{
	if  ( docListNumberTreeMoveChildrenBack(
					cfr->lntnChildren[cfrpos-1], cfch ) )
	    { LLDEB(level,cfrpos); return -1;	}

	docListNumberTreeDeleteChild( cfr, cfrpos );
	level--;
	}

    /*  3  */
    for ( lev= level; lev >= 0; lev-- )
	{
	int			lfrpos= nums[lev];
	ListNumberTreeNode *	lfr= path[lev];
	ListNumberTreeNode *	lfch= lfr->lntnChildren[lfrpos];

	if  ( lfch->lntnParaNr < 0 )
	    {
	    if  ( lfrpos > 0 && lfch->lntnChildCount > 0 )
		{
		ListNumberTreeNode *	pfch= lfr->lntnChildren[lfrpos- 1];
		if  ( lfch->lntnChildCount > 0			&&
		      lfch->lntnChildren[0]->lntnParaNr < 0	)
		    {
		    ListNumberTreeNode *	lfch0;

		    lfch0= lfch->lntnChildren[0];

		    if  ( pfch->lntnChildCount > 0 )
			{
			ListNumberTreeNode *	pfchz;

			pfchz= pfch->lntnChildren[pfch->lntnChildCount- 1];

			if  ( docListNumberTreeMoveChildren(
					    pfchz, pfchz->lntnChildCount,
					    lfch0, 0, lfch0->lntnChildCount ) )
			    { LDEB(lfch->lntnChildCount); return -1;	}

			docListNumberTreeDeleteChild( lfch, 0 );
			}
		    else{ SLDEB("##",pfch->lntnChildCount);	}
		    }

		if  ( docListNumberTreeMoveChildren(
					    pfch, pfch->lntnChildCount,
					    lfch, 0, lfch->lntnChildCount ) )
		    { LDEB(lfch->lntnChildCount); return -1;	}
		}

	    if  ( lfch->lntnChildCount == 0 )
		{ docListNumberTreeDeleteChild( lfr, lfrpos );	}
	    }
	}

#   if LOG_TRANSACTIONS
    SLDEB("DELETED",paraNr);/*docListListNumberNode(root);*/
#   endif

    return 0;
    }

int docListNumberTreesDeleteParagraph(	ListNumberTrees *	lnt,
					int			ls,
					int			paraNr )
    {
    if  ( ls < 0 || ls >= lnt->lntTreeCount )
	{ LLDEB(ls,lnt->lntTreeCount); return -1;	}

    return docListNumberTreeDeleteParagraph( lnt->lntTrees+ ls, paraNr );
    }

/************************************************************************/
/*									*/
/*  Adapt references from bullets to the text because the number of	*/
/*  paragraphs changed in the document.					*/
/*									*/
/*  1)  Loop: To use less recursion.. handle the last child of a node	*/
/*	in a loop.							*/
/*  2)  Handle the other children in a recursive call.			*/
/*  3)  Skip children before the first paragraph.			*/
/*									*/
/************************************************************************/

void docShiftListNodeReferences(	ListNumberTreeNode *	root,
					int			paraFrom,
					int			paraShift )
    {
    ListNumberTreeNode *	node= root;

    /*  1  */
    for (;;)
	{
	if  ( node->lntnParaNr >= paraFrom )
	    {
#	    if LOG_TRANSACTIONS
	    appDebug( "SHIFT %2d+ %d -> %2d\n",
			node->lntnParaNr, paraShift,
			node->lntnParaNr+ paraShift );
#	    endif

	    node->lntnParaNr += paraShift;
	    }

	if  ( node->lntnChildCount == 0 )
	    { break;	}
	else{
	    int		pos;

	    /*  2  */
	    for ( pos= 0; pos < node->lntnChildCount- 1; pos++ )
		{
		/*  3  */
		if  ( node->lntnChildren[pos+ 1]->lntnParaNr <= paraFrom )
		    { continue;	}

		docShiftListNodeReferences( node->lntnChildren[pos],
							paraFrom, paraShift );
		}

	    node= node->lntnChildren[node->lntnChildCount- 1];
	    }
	}

    return;
    }

void docShiftListTreeReferences(	ListNumberTrees *	lnt,
					int			paraFrom,
					int			paraShift )
    {
    int				i;
    ListNumberTreeNode *	root;

    root= lnt->lntTrees;
    for ( i= 0; i < lnt->lntTreeCount; root++, i++ )
	{ docShiftListNodeReferences( root, paraFrom, paraShift );	}

    return;
    }

/************************************************************************/
/*									*/
/*  Call a function for all paragraph numbers in a list level tree.	*/
/*									*/
/************************************************************************/

int docListNumberTreeForAll(	ListNumberTreeNode *	node,
				LIST_TREE_FUNC		forOne,
				void *			through )
    {
    int		i;

    if  ( node->lntnParaNr >= 0 )
	{
	if  ( (*forOne)( node->lntnParaNr, through ) )
	    { LDEB(node->lntnParaNr); return -1;	}
	}

    for ( i= 0; i < node->lntnChildCount; i++ )
	{
	if  ( docListNumberTreeForAll( node->lntnChildren[i],
							forOne, through ) )
	    { LDEB(i); return -1;	}
	}

    return 0;
    }
