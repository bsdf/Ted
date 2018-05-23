#   include	"indConfig.h"

#   include	<stdlib.h>

#   include	"indlocal.h"
#   include	<appDebugon.h>

/*  #define	LOG(x) x */
#define	LOG(x)

/************************************************************************/
/*									*/
/*  Minimise an index.							*/
/*  For the algorithm:							*/
/*  AHO, Alfred V. & Ravi SETHI & Jeffrey D. ULLMAN, "Compilers		*/
/*	Principles, Techniques, and Tools", Addison- Wesley, Reading MA,*/
/*	1986, ISBN 0-201-10194-7. p 142.				*/
/*									*/
/************************************************************************/

static int	indPartition(	IND *	ind,
				int *	clas,
				int *	part,
				int *	pngroup,
				int *	gend )
    {
    int		turn= 0;
    int		ngroup= *pngroup;
    int	*	oldGend= (int *)malloc( gend[ngroup-1]* sizeof( int ) );

    if  ( ! oldGend )
	{ XDEB(oldGend); return -1; }

    for (;;)
	{
	int	past= 0;
	int	first;
	int	oldNgroup;
	int	group;

	for ( group= 0; group < ngroup; group++ )
	    { oldGend[group]= gend[group]; }
	oldNgroup= ngroup;
	ngroup= 0;

	/****************************************************************/
	/*  For all groups, divide into subgroups with different	*/
	/*  transitions.						*/
	/*  1)	Transitions of a group are different if either		*/
	/*	(a)	there is a transition for a different symbol, or*/
	/*	(b)	for a certain symbol, the transition is to a	*/
	/*		different group.				*/
	/****************************************************************/
	for ( group= 0; group < oldNgroup; group++ )
	    {
	    int	i;

	    first= past;

	    past= oldGend[group];

	    LOG(appDebug( "TURN %3d: PARTITION    %6d ... %6d\n",
			turn, first, past ));

	    /************************************************************/
	    /*  For all members of the group.				*/
	    /************************************************************/
	    while( first < past )
		{
		TrieNode *	fnode= NODE( ind, part[first] );
		int		fntr= fnode->tn_ntrans;

		for ( i= first+ 1; i < past; i++ )
		    {
		    TrieNode *	inode= NODE(ind,part[i]);
		    int		intr= inode->tn_ntrans;
		    int		cnt;

		    if  ( intr == fntr )
			{
			for ( cnt= 0; cnt < intr; cnt++ )
			    {
			    int	cf= fnode->tn_transitions+ cnt;
			    int	ci= inode->tn_transitions+ cnt;

			    /************************************/
			    /*  Transition symbol (1.a)		*/
			    /************************************/
			    /*  1.a	*/
			    if  ( LINK(ind,cf)->tl_key	!=
				  LINK(ind,ci)->tl_key	)
				{ break;	}
			    else{
				/********************************/
				/*  target of transition. (1.b)	*/
				/********************************/
				if  ( clas[LINK(ind,cf)->tl_to]	!=
				      clas[LINK(ind,ci)->tl_to] )
				    { break;	}
				}
			    }

			if  ( cnt == intr )
			    {
			    /********************************************/
			    /*  Set of transitions identical, Pull i	*/
			    /*	into first's group 			*/
			    /********************************************/
			    if  ( i > first+ 1 )
				{
				int	swap= part[first+ 1];
					      part[first+ 1]= part[i];
							  part[i]= swap;
				}
			    first++;
			    }
			}
		    }
		gend[ngroup++]= ++first;
		}
	    }

	/*  ready?	*/
	if  ( ngroup == oldNgroup )
	    { free( (char *)oldGend ); *pngroup= ngroup; return 0;	}

	LOG(appDebug( "TURN %3d %6d CLASSES\n", turn, ngroup ));

	past= 0;
	for ( group= 0; group < ngroup; group++ )
	    {
	    first= past;
	    past= gend[group];

	    LOG(appDebug( "TURN %3d CLASS %6d: %6d..%6d (%6d members)\n",
		    turn, group, first, past, past- first ) );

	    while( first < past )
		{ clas[part[first++]]= group; }
	    }

	turn++;
	}
    }

/************************************************************************/
/*  Copy an automaton to a minimal one.					*/
/*  The numbers refer to the steps in the algorithm on p142 of AHO,	*/
/*  SEHTI, ULLMAN. See above.						*/
/*  part)	Part is the partition of the automaton. It contains the	*/
/*		numbers of the states in the original automaton grouped	*/
/*		by their classification. The delimitation of the groups	*/
/*		is described by gend.					*/
/*  gend)	Is an array with ngroup indexes of the ends of the	*/
/*		groups in part.						*/
/*  clas)	Is the array with the classification of the states in	*/
/*		the original automaton.					*/
/************************************************************************/

IND *	indINDmini( IND *	ind )
    {
    TrieNode *	node;
    int		tn;
    int *	part= (int *)0;
    int *	clas= (int *)0;
    int *	gend= (int *)0;
    int		ngroup;
    int		l, r;
    int		first, past;

    IND *	rval= indINDmake( 0 );
    if  ( ! rval )
	{ XDEB(rval); return rval;	}

    LOG(indINDprint( ind ));

    part= (int *)malloc( ind->indNodeCount* sizeof(int) );
    if  ( ! part )
	{ LOG(appDebug("nomem %s(%d)\n", __FILE__, __LINE__ )); goto failure; }
    clas= (int *)malloc( ind->indNodeCount* sizeof(int) );
    if  ( ! clas )
	{ LOG(appDebug("nomem %s(%d)\n", __FILE__, __LINE__ )); goto failure; }
    gend= (int *)malloc( ind->indNodeCount* sizeof(int) );
    if  ( ! gend )
	{ LOG(appDebug("nomem %s(%d)\n", __FILE__, __LINE__ )); goto failure; }

    /********************/
    /*  Step 1		*/
    /********************/
    l= 0; r= ind->indNodeCount;
    for ( tn= 0; tn < ind->indNodeCount; tn++ )
	{
	node= NODE(ind,tn);
	if  ( node->tn_flags & TNfACCEPTS )
	    { clas[tn]= 0; part[l++]= tn;	}
	else{ clas[tn]= 1; part[--r]= tn;	}
	}
    ngroup= 2; gend[0]= l; gend[1]= ind->indNodeCount;

    /********************/
    /*  Step 2,3	*/
    /********************/
    if  ( indPartition( ind, clas, part, &ngroup, gend ) )
	{ LOG(appDebug("Partition\n")); goto failure;	}

#   if 0
    l= 0; past= 0;
    for ( r= 0; r < ngroup; r++ )
	{
	first= past; past= gend[r];

	appDebug( "GROUP %6d:", r );
	while( first < past )
	    { appDebug( " %4d", part[first++] ); }
	appDebug( "\n" );
	}
#   endif

    /****************************/
    /*  Step 5:Dead states	*/
    /****************************/
    l= 0; past= 0;
    for ( r= 0; r < ngroup; r++ )
	{
	first= past; past= gend[r];

	while( first < past )
	    {
	    TrieNode *	fnode= NODE(ind,part[first]);
	    int		fntr= fnode->tn_ntrans;
	    int		cnt;

	    /****************************/
	    /*  Originally accepting!	*/
	    /****************************/
	    if  ( fnode->tn_flags & TNfACCEPTS )
		{ break;	}

	    for ( cnt= 0; cnt < fntr; cnt++ )
		{
		int	cf= LINK(ind,fnode->tn_transitions+ cnt)->tl_to;
		if  ( clas[cf] != r )
		    { break;	}
		}
	    /****************************/
	    /*  Not to the same class	*/
	    /****************************/
	    if  ( cnt < fntr )
		{ break;	}
	    first++;
	    }
	if  ( first < past )
	    { gend[l++]= gend[r];				}
	else{ LOG(appDebug( "GROUP %6d IS DEAD\n", r ));	}
	}
    ngroup= l;

    /********************************************/
    /*  Allocate nodes and renumber groups.	*/
    /********************************************/
    past= 0;
    for ( r= 0; r < ngroup; r++ )
	{
	first= past;
	past= gend[r];

	l= indTNmake( rval );
	if  ( l < 0 )
	    { LDEB(l); goto failure;	}

	while( first < past )
	    { clas[part[first++]]= l; }
	}

    LOG(appDebug( "%6d GROUPS\n", ngroup ));

    /********************************************/
    /*  Set connections. Set Flags.		*/
    /********************************************/
    rval->ind_start= clas[ind->ind_start];
    past= 0;
    for ( r= 0; r < ngroup; r++ )
	{
	int		transitions;
	TrieNode *	newn;

	first= past; past= gend[r];
	newn= NODE(rval,clas[part[first]]);

	LOG(appDebug("GROUP= %4d NEW= %4d\n", r, clas[part[first]] ));

	node= NODE(ind,part[first]);
	if  ( node->tn_ntrans > 0 )
	    {
	    transitions= indTLalloc( rval, -1, node->tn_ntrans );
	    if  ( transitions < 0 )
		{ LDEB(transitions); goto failure;	}
	    for ( l= 0; l < node->tn_ntrans; l++ )
		{
		TrieLink *	link= LINK(ind,node->tn_transitions+ l);
		LINK(rval,transitions+l)->tl_to= clas[link->tl_to];
		LINK(rval,transitions+l)->tl_key= link->tl_key;
		}
	    }
	else{ transitions= -1;	}

	newn->tn_transitions= transitions;
	newn->tn_ntrans= node->tn_ntrans;
	newn->tn_flags |= node->tn_flags;
	newn->tn_flags |= TNfREAD_ONLY;
	}

    LOG(indINDprint( rval ));

    free( (char *)part );
    free( (char *)clas );
    free( (char *)gend );

    return rval;

failure:
    if  ( part )
	{ free( (char *)part );	}
    if  ( clas )
	{ free( (char *)clas );	}
    if  ( gend )
	{ free( (char *)gend );	}

    if  ( rval )
	{ indINDfree( rval );	}

    return (IND *)0;
    }
