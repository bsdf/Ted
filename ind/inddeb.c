#   include	"indConfig.h"

#   include	"indlocal.h"
#   include	<appDebugoff.h>

/************************************************************************/
/*  Print a list of Nodes with their transitions.			*/
/************************************************************************/

void indTLprint(	IND *	ind,
			int	tl )
    {
    TrieLink *	link;
    char	flags[3]= "  ";

    if  ( tl == ind->indFirstFreeLinkSlot )
	{ flags[0]= 'F'; }
    if  ( tl == ind->indLastHeadLinkSlot )
	{ flags[1]= 'L'; }

    if  ( tl < 0 || tl >= ind->indAllocatedLinks )
	{
	appDebug("\tLINK %6d #### >= %d\n", tl, ind->indAllocatedLinks );
	return;
	}

    link= LINK(ind,tl);

    if  ( link->tl_to == TLgFREE )
	{
	appDebug("\tLINK %6d FREE %d slots => %d %s\n", tl,
				    link->tl_key, tl+ 1+ link->tl_key, flags);
	return;
	}
    if  ( link->tl_to == TLgUSED )
	{
	appDebug("\tLINK %6d USED %d slots => %d %s\n", tl,
				    link->tl_key, tl+ 1+ link->tl_key, flags);
	return;
	}

    if  ( link->tl_key > 0 && link->tl_key < 256 )
	{
	appDebug("\tLINK %6d  '%c' -> %6d %s\n",
				    tl, link->tl_key, link->tl_to, flags);
	}
    else{
	appDebug("\tLINK %6d  0x%x -> %6d %s\n",
				    tl, link->tl_key, link->tl_to, flags);
	}
    }

void indTLprint2(	IND *	ind,
			int	from,
			int	upto )
    {
    while( from < upto )
	{ indTLprint( ind, from++ ); }
    }

void indTNprint(	IND *	ind,
			int	tn )
    {
    TrieNode *	node= NODE(ind,tn);
    int		transitions= node->tn_transitions;
    int		tl;

    appDebug( "NODE %6d: %3d transitions AT %6d %s\n",
	tn, node->tn_ntrans, transitions,
	(node->tn_flags&TNfACCEPTS)?"ACCEPTS":"INTERN" );

    for ( tl= 0; tl < node->tn_ntrans; tl++ )
	{ indTLprint(ind,transitions+ tl); }
    }

void indINDprint( IND *	ind )
    {
    int	i;

    appDebug( "INDEX: root= %d, %d nodes, %d links firstFreeLink: %d\n",
	ind->ind_start, ind->indNodeCount,
	ind->indAllocatedLinks, ind->indFirstFreeLinkSlot );

    /*
    for ( i= 0; i < ind->indNodeCount; i++ )
	{ indTNprint( ind, i ); }
    */

    for ( i= 0; i <= ind->indLastHeadLinkSlot && i < ind->indAllocatedLinks; i++ )
	{ indTLprint( ind, i ); }
    }

/************************************************************************/
/*									*/
/*  Count the different length of deterministic path in the automaton.	*/
/*									*/
/************************************************************************/

void indINDcount( IND *	ind )
    {
    int		tn;
    int		l;

    static int lengths[200];

    for ( tn= 0; tn < ind->indNodeCount; tn++ )
	{
	TrieNode *	node= NODE(ind,tn);

	if  ( node->tn_ntrans == 1 )
	    {
	    int		transitions= node->tn_transitions;
	    TrieLink *	link= LINK(ind,transitions);

	    l= 0;
	    while( node->tn_ntrans == 1 )
		{
		node= NODE(ind,link->tl_to); l++;
		transitions= node->tn_transitions;
		link= LINK(ind,transitions);
		}
	    lengths[l]++;
	    }
	}

    for ( l= 1; l < 199; l++ )
	{
	if  ( lengths[l] > 0 )
	    { appDebug( "%3d: %5d\n", l, lengths[l]- lengths[l+1] ); }
	}
    }

