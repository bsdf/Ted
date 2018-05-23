/************************************************************************/
/*									*/
/*  A node in the bidirectional tree.					*/
/*									*/
/************************************************************************/

#   include	"textEncodingConfig.h"

#   include	<stdlib.h>
#   include	<limits.h>

#   include	"bidiTree.h"
#   include	"bidiEmbedding.h"

#   include	<appDebugon.h>

void bidiInitTreeNode(			BidiNode *	bn )
    {
    bn->bnFrom= 0;
    bn->bnUpto= 0;
    bn->bnEmbedding= BIDIembedEMBEDDING;
    bn->bnLevel= 0;

    bn->bnParent= (BidiNode *)0;
    bn->bnChildren= (BidiNode **)0;
    bn->bnChildCount= 0;
    bn->bnNumberInParent= 0;

    return;
    }

void bidiCleanTreeNode(			BidiNode *	bn )
    {
    int		i;

    for ( i= 0; i < bn->bnChildCount; i++ )
	{
	if  ( bn->bnChildren[i] )
	    {
	    bidiCleanTreeNode( bn->bnChildren[i] );
	    free( bn->bnChildren[i] );
	    }
	}

    if  ( bn->bnChildren )
	{ free( bn->bnChildren );	}

    return;
    }

BidiNode * bidiNodeAddChild(	BidiNode *	parent,
				int		embedding,
				int		level,
				int		from,
				int		upto )
    {
    BidiNode *	child= (BidiNode *)0;
    BidiNode **	children= (BidiNode **)0;
    int		nip= 0;

    if  ( level <= parent->bnLevel	||
	  level >  parent->bnLevel+ 2	)
	{ LLDEB(level,parent->bnLevel); return (BidiNode *)0;	}

    children= (BidiNode **)realloc( parent->bnChildren,
			    ( parent->bnChildCount+ 1 )* sizeof(BidiNode *) );
    if  ( ! children )
	{ LXDEB(parent->bnChildCount,children); return (BidiNode *)0;	}
    parent->bnChildren= children;

    /*  Find the last child of parent with bnFrom >= from; if there is	*/
    /*  a previous one, it must have bnUpto >= from. (Avoid overlap)	*/

    if  ( parent->bnChildCount > 0 )
	{
	int	l= 0;
	int	r= parent->bnChildCount;
	int	m= ( l+ r )/ 2;

	while( l < m )
	    {
	    if  ( parent->bnChildren[m]->bnFrom < from )
		{ l= m;	}
	    else{ r= m;	}

	    m= ( l+ r )/ 2;
	    }

	if  ( from <= parent->bnChildren[m]->bnFrom )
	    { nip= m;		}
	else{ nip= m+ 1;	}

	/*  Overlap with subsequent is wrong	*/
	if  ( nip < parent->bnChildCount		&&
	      parent->bnChildren[nip]->bnFrom > upto	)
	    {
	    LLLDEB(nip,parent->bnChildren[nip]->bnFrom,upto);
	    return (BidiNode *)0;
	    }

	/*  Overlap with previous is wrong	*/
	if  ( nip > 0					&&
	      parent->bnChildren[nip-1]->bnUpto > from	)
	    {
	    LLLDEB(nip,parent->bnChildren[nip-1]->bnUpto,from);
	    return (BidiNode *)0;
	    }
	}

    if  ( bidiStretchNode( parent, upto ) )
	{ LDEB(upto); return (BidiNode *)0;	}

    child= malloc( sizeof(BidiNode) );
    if  ( ! child )
	{ XDEB(child); return (BidiNode *)0;	}

    bidiInitTreeNode( child );

    child->bnParent= parent;
    child->bnNumberInParent= nip;
    child->bnEmbedding= embedding;
    child->bnLevel= level;
    child->bnFrom= from;
    child->bnUpto= upto;

    if  ( nip < parent->bnChildCount )
	{
	int	i;

	for ( i= parent->bnChildCount; i > nip; i-- )
	    {
	    parent->bnChildren[i]= parent->bnChildren[i-1];
	    parent->bnChildren[i]->bnNumberInParent= i;
	    }
	}

    parent->bnChildren[nip]= child;
    parent->bnChildCount++;

    return child;
    }

int bidiStretchNode(	BidiNode *	bn,
			int		upto )
    {
    while( bn )
	{
	if  ( bn->bnUpto < upto )
	    { bn->bnUpto=  upto;	}

	bn= bn->bnParent;
	}

    return 0;
    }

/**
 *  Traverse a range of contiguous children
 */
typedef struct TraverseNode
    {
    int			tnFrom;
    int			tnUpto;
    BidiGotLevel	tnGotLevel;
    void *		tnThrough;
    } TraverseNode;

static int bidiTraverseNodeImpl(	const BidiNode *	bn,
					const TraverseNode *	tn );

static int bidiTraverseChildrenBwd(	const BidiNode *	bn,
					const TraverseNode *	tn,
					int			fr,
					int			to )
    {
    to--;

    while( to >= fr )
	{
	int	tt= to- 1;
	int	t;

	if  ( bn->bnChildren[to]->bnLevel == bn->bnLevel+ 2 )
	    {
	    while( tt >= fr && bn->bnChildren[tt]->bnLevel == bn->bnLevel+ 2 )
		{ tt--;	}
	    }

	for ( t= tt+ 1; t <= to; t++ )
	    {
	    if  ( bidiTraverseNodeImpl( bn->bnChildren[t], tn ) )
		{ LDEB(t); return -1;	}
	    }

	to= tt;
	}

    return 0;
    }

static int bidiTraverseChildrenFwd(	const BidiNode *	bn,
					const TraverseNode *	tn,
					int			fr,
					int			to )
    {
    fr++;

    while( fr <= to )
	{
	if  ( bidiTraverseNodeImpl( bn->bnChildren[fr], tn ) )
	    { LDEB(fr); return -1;	}
	fr++;
	}

    return 0;
    }

/**
 *  Traverse a node and its offspring in presentation (drawing) order
 *  NOTE: bnChildCount is expected to be so low that binary search 
 *	for the starting child is not worth the amount of code.
 */
static int bidiTraverseNodeImpl(	const BidiNode *	bn,
					const TraverseNode *	tn )
    {
    int		from= bn->bnFrom;
    int		upto= bn->bnUpto;
    int		forward;

    if  ( bn->bnParent					&&
	  ( bn->bnEmbedding == BIDIembedLRE	||
	    bn->bnEmbedding == BIDIembedRLE	||
	    bn->bnEmbedding == BIDIembedLRO	||
	    bn->bnEmbedding == BIDIembedRLO	)	)
	{ from++; upto--;	}

    if  ( tn->tnFrom >= 0 )
	{
	if  ( tn->tnFrom >= upto )
	    { return 0;	}
	if  ( from < tn->tnFrom )
	    { from=  tn->tnFrom;	}
	}

    if  ( tn->tnUpto >= 0 )
	{
	if  ( tn->tnUpto <= from )
	    { return 0;	}
	if  ( upto > tn->tnUpto )
	    { upto=  tn->tnUpto;	}
	}

    forward= ( bn->bnLevel % 2 ) == 0;

    if  ( forward )
	{
	int		fr= INT_MAX;
	int		to;

	for ( to= 0; to < bn->bnChildCount; to++ )
	    {
	    const BidiNode *	child= bn->bnChildren[to];

	    if  ( child->bnLevel <= bn->bnLevel		||
		  child->bnLevel > bn->bnLevel+ 2	)
		{ LLDEB(bn->bnLevel,child->bnLevel); return -1;	}

	    if  ( child->bnFrom >= upto )
		{ break;	}
	    if  ( child->bnUpto <= from )
		{ continue;	}

	    if  ( child->bnFrom > from )
		{
		if  ( bidiTraverseChildrenBwd( bn, tn, fr, to ) )
		    { LLDEB(fr,to);	}
		fr= INT_MAX;

		if  ( (*tn->tnGotLevel)( tn->tnThrough,
						bn->bnEmbedding, bn->bnLevel,
						from, child->bnFrom ) )
		    { LDEB(1); return -1;	}
		}

	    if  ( fr == INT_MAX )
		{ fr= to;	}

	    from= child->bnUpto;
	    }

	if  ( bidiTraverseChildrenBwd( bn, tn, fr, to ) )
	    { LLDEB(fr,to);	}

	if  ( from < upto )
	    {
	    if  ( (*tn->tnGotLevel)( tn->tnThrough,
						bn->bnEmbedding, bn->bnLevel,
						from, upto ) )
		{ LDEB(1); return -1;	}
	    }
	}
    else{
	int		to= INT_MIN;
	int		fr;

	for ( fr= bn->bnChildCount- 1; fr >= 0; fr-- )
	    {
	    const BidiNode *	child= bn->bnChildren[fr];

	    if  ( child->bnFrom >= upto )
		{ continue;	}
	    if  ( child->bnUpto <= from )
		{ break;	}

	    if  ( child->bnLevel <= bn->bnLevel		||
		  child->bnLevel > bn->bnLevel+ 2	)
		{ LLDEB(bn->bnLevel,child->bnLevel); return -1;	}

	    if  ( child->bnUpto < upto )
		{
		if  ( bidiTraverseChildrenFwd( bn, tn, fr, to ) )
		    { LLDEB(fr,to);	}
		to= INT_MIN;

		if  ( (*tn->tnGotLevel)( tn->tnThrough,
						bn->bnEmbedding, bn->bnLevel,
						child->bnUpto, upto ) )
		    { LDEB(1); return -1;	}
		}

	    if  ( to < 0 )
		{ to= fr;	}

	    upto= child->bnFrom;
	    }

	if  ( bidiTraverseChildrenFwd( bn, tn, fr, to ) )
	    { LLDEB(fr,to);	}

	if  ( from < upto )
	    {
	    if  ( (*tn->tnGotLevel)( tn->tnThrough,
						bn->bnEmbedding, bn->bnLevel,
						from, upto ) )
		{ LDEB(1); return -1;	}
	    }
	}

    return 0;
    }

int bidiTraverseNode(	const BidiNode *	bn,
			int			from,
			int			upto,
			BidiGotLevel		gotLevel,
			void *			through )
    {
    TraverseNode	tn;

    if  ( ! gotLevel )
	{ XDEB(gotLevel); return -1;	}

    tn.tnFrom= from;
    tn.tnUpto= upto;
    tn.tnGotLevel= gotLevel;
    tn.tnThrough= through;

    return bidiTraverseNodeImpl( bn, &tn );
    }

void bidiListNode(	const BidiNode *	bn,
			const char *		symbols )
    {
    int		i;
    int		from= bn->bnFrom;

    for ( i= 0; i < bn->bnChildCount; i++ )
	{
	const BidiNode *	ch= bn->bnChildren[i];

	if  ( ch->bnParent != bn )
	    { SXXDEB("####",ch->bnParent,bn);	}
	if  ( ch->bnNumberInParent != i )
	    { SLLDEB("####",ch->bnNumberInParent,i);	}
	if  ( ch->bnFrom < from )
	    { SLLLDEB("####",i,ch->bnFrom,from);	}

	if  ( i == 0 || ch->bnFrom > from )
	    {
	    appDebug( "%3d..%3d/%3d..%3d (%3d@%2d): %*s%s %d",
		from, ch->bnFrom, bn->bnFrom, bn->bnUpto,
		( ch->bnFrom- from ), bn->bnNumberInParent,
		3* bn->bnLevel, "",
		bidiEmbeddingStr( bn->bnEmbedding ), bn->bnLevel );

	    if  ( symbols )
		{
		appDebug( " \"%.*s\"", ch->bnFrom- from, symbols+ from );
		}

	    appDebug( "\n" );
	    }


	bidiListNode( ch, symbols );

	from= ch->bnUpto;
	}

    if  ( i == 0 || from < bn->bnUpto )
	{
	appDebug( "%3d..%3d/%3d..%3d (%3d@%2d): %*s%s %d",
		from, bn->bnUpto, bn->bnFrom, bn->bnUpto,
		( bn->bnUpto- from ), bn->bnNumberInParent,
		3* bn->bnLevel, "",
		bidiEmbeddingStr( bn->bnEmbedding ), bn->bnLevel );

	if  ( symbols )
	    {
	    appDebug( " \"%.*s\"", bn->bnUpto- from, symbols+ from );
	    }

	appDebug( "\n" );
	}
    }
