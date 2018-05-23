#   include	"indConfig.h"

#   include	<stdlib.h>

#   include	<uniUtf8.h>
#   include	"indlocal.h"
#   include	<appDebugon.h>

/************************************************************************/
/*  Allocate a new node. In Theory, it might be advisable to look for	*/
/*  a hole in the array of nodes. As in the current version of the	*/
/*  software, no nodes are really freed. this does not really make	*/
/*  sense.								*/
/************************************************************************/

int indTNmake( IND *	ind )
    {
    TrieNode *	node;

    if  ( ind->indNodeCount >= ind->indAllocatedNodes )
	{
	int		newm= ( ind->indAllocatedNodes+ TNsBLOCK )/ TNsBLOCK;
	unsigned	sz= newm* sizeof(TrieNode *);
	TrieNode **	fresh;
	TrieNode *	nwpg;

	if  ( ind->indNodePages )
	    { fresh= (TrieNode **) realloc( (char *)ind->indNodePages, sz );}
	else{ fresh= (TrieNode **) malloc( sz );				}
	if  ( ! fresh )
	    { return -1;	}
	ind->indNodePages= fresh;
	nwpg= (TrieNode *)malloc( TNsBLOCK* sizeof( TrieNode ) );
	if  ( ! nwpg )
	    { return -1;	}
	ind->indNodePages[newm-1]= nwpg;
	ind->indAllocatedNodes= newm* TNsBLOCK;
	}

    node= NODE(ind,ind->indNodeCount);
    node->tn_transitions= -1;
    node->tn_ntrans= 0;
    node->tn_flags= TNfUSED;
    node->tn_unused= 0;

    /*appDebug( "NODE %7d of %7d\n", ind->indNodeCount, ind->indAllocatedNodes );*/
    return ind->indNodeCount++;
    }

void indTNfree(	IND *	ind,
		int	tn )
    {
    TrieNode *	node= NODE(ind,tn);

    if  ( node->tn_transitions >= 0 )
	{ indTLfree( ind, node->tn_transitions ); node->tn_transitions= -1; }
    node->tn_flags= 0;
    node->tn_ntrans= 0;

    while( ind->indNodeCount > 0)
	{
	node= NODE(ind,ind->indNodeCount-1);

	if  ( node->tn_flags == 0	)
	    { ind->indNodeCount--;	}
	else{ break;		}
	}
    }

/************************************************************************/
/*									*/
/*  Follow one transition in the automaton.				*/
/*									*/
/*  Return:	The index of the transition from this node in *pTrans.	*/
/*		If there is no transition for the input symbol, *pTrans	*/
/*		is set to the candidate insertion point.		*/
/*									*/
/*  Return:	The number in the automaton of the next state (If any)	*/
/*  Return:	-1 if there is no state for this value.			*/
/*									*/
/************************************************************************/

int indINDstep(	int *				pTrans,
		IND *				ind,
		int				tn,
		int				sym )
    {
    TrieNode *	node;
    int		l, r;
    int		transitions;

    node= NODE(ind,tn);

    l= 0;
    r= node->tn_ntrans;
    *pTrans= ( l+ r )/2;
    transitions= node->tn_transitions;

    if  ( r <= 0 )
	{ return -1;	}

    while( l < *pTrans )
	{
	if  ( sym < LINK(ind,transitions+*pTrans)->tl_key )
	    { r= *pTrans;	}
	else{ l= *pTrans;	}
	*pTrans= ( l+ r )/2;
	}

    if  ( sym != LINK(ind,transitions+*pTrans)->tl_key )
	{ return -1;	}

    return LINK(ind,transitions+*pTrans)->tl_to;
    }

/************************************************************************/
/*									*/
/*  Retrieve a word in an automaton.					*/
/*									*/
/************************************************************************/

int indINDgetUtf8(	int *		paccept,
			IND *		ind,
			int		tn,
			const char *	key )
    {
    int		m;

    if  ( tn < 0 || tn >= ind->indNodeCount )
	{ LLDEB(tn,ind->indNodeCount); return -1;	}

    for (;;)
	{
	int		step;
	unsigned short	symbol;

	if  ( ! * key )
	    {
	    if  ( NODE(ind,tn)->tn_flags & TNfACCEPTS )
		{ *paccept= 1;	}
	    else{ *paccept= 0;	}

	    return tn;
	    }

	step= uniGetUtf8( &symbol, key );
	if  ( step < 1 )
	    { LDEB(step); return -1;	}
	key += step;

	tn= indINDstep( &m, ind, tn, symbol );

	if  ( tn < 0 )
	    { return tn;	}
	}
    }

int indINDgetUtf16(	int *			paccept,
			IND *			ind,
			int			tn,
			const unsigned short *	key )
    {
    int		m;

    if  ( tn < 0 || tn >= ind->indNodeCount )
	{ LLDEB(tn,ind->indNodeCount); return -1;	}

    for (;;)
	{
	unsigned short	symbol;

	if  ( ! * key )
	    {
	    if  ( NODE(ind,tn)->tn_flags & TNfACCEPTS )
		{ *paccept= 1;	}
	    else{ *paccept= 0;	}

	    return tn;
	    }

	symbol= *key;
	key += 1;

	tn= indINDstep( &m, ind, tn, symbol );

	if  ( tn < 0 )
	    { return tn;	}
	}
    }

/************************************************************************/
/*									*/
/*  Remove a word from the automaton.					*/
/*  Simply remove the 'accepts' flag from its final node.		*/
/*									*/
/*  This is not correct in a minimized automaton.			*/
/*  This is not correct if the automaton is used to find out whether	*/
/*  strings are a prefix of accepted strings.				*/
/*									*/
/************************************************************************/

int indINDforget(	IND *		ind,
			const char *	key )
    {
    int		accepted;
    int		tn= indINDgetUtf8( &accepted, ind, ind->ind_start, key );
    TrieNode *	node;

    if  ( tn < 0 || ! accepted )
	{ return -1;	}

    node= NODE(ind,tn);

    node->tn_flags &= ~TNfACCEPTS;

    return 0;
    }

/************************************************************************/

static int indAddLinkToNode(		IND *		ind,
					TrieNode *	node,
					int		m,
					int		symbol,
					int		to )
    {
    int		transitions= node->tn_transitions;

    if  ( node->tn_flags & TNfREAD_ONLY )
	{ XXDEB(node->tn_flags,TNfREAD_ONLY); return -1;	}

    if  ( node->tn_ntrans > 0 )
	{
	int	r;

	transitions= indTLalloc( ind, transitions, node->tn_ntrans+ 1 );
	if  ( transitions < 0 )
	    { LLDEB(node->tn_ntrans,transitions); return -1;	}
	else{ node->tn_transitions= transitions;		}

	if  ( symbol > LINK(ind,transitions+m)->tl_key )
	    { m++; }
	r= node->tn_ntrans++;
	while( r > m )
	    {
	    *LINK(ind,transitions+r)= *LINK(ind,transitions+r-1);
	    r--;
	    }
	LINK(ind,transitions+m)->tl_to= to;
	LINK(ind,transitions+m)->tl_key= symbol;
	}
    else{
	node->tn_transitions= transitions= indTLalloc( ind, -1, 1 );
	if  ( transitions < 0 )
	    { LDEB(transitions); return -1;	}

	node->tn_ntrans= 1;
	LINK(ind,transitions)->tl_to= to;
	LINK(ind,transitions)->tl_key= symbol;
	m= 0;
	}

    return transitions+ m;
    }

static int indAddNodeToNode(		IND *		ind,
					TrieNode *	node,
					int		m,
					int		symbol )
    {
    int		to;

    if  ( node->tn_flags & TNfREAD_ONLY )
	{ XXDEB(node->tn_flags,TNfREAD_ONLY); return -1;	}

    to= indTNmake( ind );
    if  ( to < 0 )
	{ LDEB(to); return -1;	}

    if  ( indAddLinkToNode( ind, node, m, symbol, to ) < 0 )
	{ LLDEB(to,symbol); return -1;	}

    return to;
    }

/************************************************************************/
/*									*/
/*  Store a word in the automaton.					*/
/*									*/
/************************************************************************/

int indINDputUtf8(	IND *		ind,
			int		tn,
			const char *	key )
    {
    for (;;)
	{
	TrieNode *	node;
	int		m;

	int		step;
	unsigned short	symbol;

	if  ( tn < 0 || tn >= ind->indNodeCount )
	    { LLDEB(tn,ind->indNodeCount); return -1; }
	node= NODE(ind,tn);

	/*indTLwalk(ind);*/

	if  ( ! * key )
	    {
	    if  ( node->tn_flags & TNfREAD_ONLY )
		{ XXDEB(node->tn_flags,TNfREAD_ONLY); return -1;	}

	    node->tn_flags |= TNfACCEPTS;
	    return tn;
	    }

	step= uniGetUtf8( &symbol, key );
	if  ( step < 1 )
	    { LDEB(step); return -1;	}
	key += step;

	tn= indINDstep( &m, ind, tn, symbol );
	if  ( tn < 0 )
	    {
	    tn= indAddNodeToNode( ind, node, m, symbol );
	    if  ( tn < 0 )
		{ LDEB(tn); return -1;	}
	    }
	}
    }

int indINDputUtf16(	IND *			ind,
			int			tn,
			const unsigned short *	key )
    {
    for (;;)
	{
	TrieNode *	node;
	int		m;

	unsigned short	symbol;

	if  ( tn < 0 || tn >= ind->indNodeCount )
	    { LLDEB(tn,ind->indNodeCount); return -1; }
	node= NODE(ind,tn);

	/*indTLwalk(ind);*/

	if  ( ! * key )
	    {
	    if  ( node->tn_flags & TNfREAD_ONLY )
		{ XXDEB(node->tn_flags,TNfREAD_ONLY); return -1;	}

	    node->tn_flags |= TNfACCEPTS;
	    return tn;
	    }

	symbol= *key;
	key += 1;

	tn= indINDstep( &m, ind, tn, symbol );
	if  ( tn < 0 )
	    {
	    tn= indAddNodeToNode( ind, node, m, symbol );
	    if  ( tn < 0 )
		{ LDEB(tn); return -1;	}
	    }
	}
    }


/************************************************************************/
/*									*/
/*  Do something for all nodes in a trie.				*/
/*  NOTE: The approach is naive and loops on cyclic structures.		*/
/*									*/
/************************************************************************/

int indINDforall(	IND *		ind,
			int		tn,
			void *		through,
			IndForAllFun	fun )
    {
    const TrieNode *		frNode;
    int				i;
    int				n;

    if  ( tn < 0 )
	{ return 0;	}
    frNode= NODE(ind,tn);
    n= frNode->tn_ntrans;

    for ( i= 0; i < n; i++ )
	{
	const TrieLink *	toLink= LINK(ind,frNode->tn_transitions+i);
	const TrieNode *	toNode= NODE(ind,toLink->tl_to);

	if  ( (*fun)( through, +1, tn, toLink->tl_to, toLink->tl_key,
				(toNode->tn_flags & TNfACCEPTS) != 0 ) < 0 )
	    { LLLDEB(tn,toLink->tl_to,toLink->tl_key); return -1;	}

	if  ( indINDforall( ind, toLink->tl_to, through, fun ) < 0 )
	    { LDEB(1); return -1;	}

	if  ( (*fun)( through, -1, tn, toLink->tl_to, toLink->tl_key,
				(toNode->tn_flags & TNfACCEPTS) != 0 ) < 0 )
	    { LLLDEB(tn,toLink->tl_to,toLink->tl_key); return -1;	}
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Add a (static) suffix to a node.					*/
/*									*/
/************************************************************************/

int indINDaddSuffix(		IND *		ind,
				int		tnTo,
				int		tnSuf )
    {
    TrieNode *		nodeTo= NODE(ind,tnTo);
    const TrieNode *	nodeSuf= NODE(ind,tnSuf);

    int			m;

    int			i;

    if  ( nodeTo->tn_flags & TNfREAD_ONLY )
	{ XXDEB(nodeTo->tn_flags,TNfREAD_ONLY); return -1;	}
    if  ( nodeSuf->tn_flags & TNfACCEPTS )
	{ nodeTo->tn_flags |= TNfACCEPTS;	}

    i= nodeSuf->tn_ntrans;
    while( --i >= 0 )
	{
	const TrieLink *	linkSuf= LINK(ind,nodeSuf->tn_transitions+i);

	tnSuf= linkSuf->tl_to;

	tnTo= indINDstep( &m, ind, tnTo, linkSuf->tl_key );
	if  ( tnTo < 0 )
	    {
	    /*  Connect suffix here */
	    int	tlTo;

	    tlTo= indAddLinkToNode( ind, nodeTo, m, linkSuf->tl_key, tnSuf );
	    if  ( tlTo < 0 )
		{ LDEB(tlTo); return -1;	}

	    continue; /* Ready with this branch */
	    }

	if  ( i > 0 )
	    {
	    if  ( indINDaddSuffix( ind, tnTo, tnSuf ) < 0 )
		{ LLDEB(tnTo,tnSuf);	}
	    }
	else{
	    /* Avoid chain recursion stay in same stack frame */
	    nodeTo= NODE(ind,tnTo);
	    nodeSuf= NODE(ind,tnSuf);
	    i= nodeSuf->tn_ntrans;

	    if  ( nodeTo->tn_flags & TNfREAD_ONLY )
		{ XXDEB(nodeTo->tn_flags,TNfREAD_ONLY); return -1;	}
	    if  ( nodeSuf->tn_flags & TNfACCEPTS )
		{ nodeTo->tn_flags |= TNfACCEPTS;	}
	    }
	}

    return 0;
    }
