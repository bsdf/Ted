#   include	"indConfig.h"

#   include	<stdlib.h>

#   include	"indlocal.h"
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  The series of links is divided into groups.				*/
/*									*/
/*  Every group is preceded by a group header.				*/
/*  If the group is used, the tl_to field of the header is TLgUSED	*/
/*  If the group is free, the tl_to field of the header is TLgFREE	*/
/*									*/
/*  The last header always exists (Except at the very beginning) and	*/
/*  always is TLgFREE.							*/
/*									*/
/************************************************************************/

/************************************************************************/
/*									*/
/*  Walk over the allocation chain of links and do some half hearted	*/
/*  checking.								*/
/*									*/
/************************************************************************/

int indTLwalk( IND *	ind )
    {
    int		pos= 0;

    while( pos < ind->indAllocatedLinks )
	{
	TrieLink *	link= LINK(ind,pos);

	if  ( link->tl_to != TLgUSED && link->tl_to != TLgFREE )
	    {
	    LLLLDEB(pos,link->tl_to,TLgUSED,TLgFREE);
	    indTLprint( ind, pos );
	    return -1;
	    }

	pos += link->tl_key+ 1;
	if  ( pos < 0 || pos > ind->indAllocatedLinks )
	    {
	    LLDEB(pos,ind->indAllocatedLinks);
	    indTLprint( ind, pos );
	    return -1;
	    }
	}

    if  ( pos != ind->indAllocatedLinks )
	{
	LLDEB(pos,ind->indAllocatedLinks);
	return -1;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Merge a series of free slots.					*/
/*									*/
/*  Return the size of the merged range.				*/
/*									*/
/************************************************************************/

static int indMergeFreeSlots(	IND *		ind,
				const int	slot )
    {
    TrieLink * const	link= LINK(ind,slot);

    if  ( link->tl_to != TLgFREE )
	{ LLLDEB(slot,link->tl_to,TLgFREE); return -1;	}

    if  ( slot == ind->indLastHeadLinkSlot )
	{ return link->tl_key- 1;	}

    while( slot+ 1+ link->tl_key <= ind->indLastHeadLinkSlot )
	{
	TrieLink *	next= LINK(ind,slot+ 1+ link->tl_key);

	if  ( next->tl_to == TLgUSED )
	    { break;	}

	if  ( slot+ 1+ link->tl_key == ind->indLastHeadLinkSlot )
	    { ind->indLastHeadLinkSlot= slot;	}

	if  ( link->tl_to == TLgFREE )
	    {
	    if  ( link->tl_key+ 1+ next->tl_key >= 0xfffffe )
		{ break;	}

	    link->tl_key += 1+ next->tl_key;
	    continue;
	    }

	LLLLDEB(slot,link->tl_to,TLgUSED,TLgFREE);
	return -1;
	}

    if  ( ind->indFirstFreeLinkSlot > slot && link->tl_key > 0 )
	{ ind->indFirstFreeLinkSlot=  slot;	}

    if  ( link->tl_to != TLgFREE )
	{ LLDEB(slot,link->tl_to); return -1; }

    if  ( slot == ind->indLastHeadLinkSlot )
	{ return link->tl_key- 1;	}
    else{ return link->tl_key;		}
    }

/************************************************************************/
/*									*/
/*  Determine the number of links that are available at a particular	*/
/*  position.								*/
/*									*/
/*  As an optimization: Merge free cells.				*/
/*									*/
/************************************************************************/

static int indSlotCapacity(	IND *		ind,
				const int	from )
    {
    int		next;
    int		space;
    TrieLink *	link= LINK(ind,from);

    if  ( link->tl_to != TLgUSED && link->tl_to != TLgFREE )
	{
	LLLLDEB(from,link->tl_to,TLgUSED,TLgFREE);
	indINDprint(ind);
	return -1;
	}

    if  ( link->tl_to == TLgFREE )
	{
	space= indMergeFreeSlots( ind, from );
	return space;
	}

    next= from+ 1+ link->tl_key;
    if  ( next == ind->indLastHeadLinkSlot )
	{ return next- from- 1;	}

    link= LINK(ind,next);
    if  ( link->tl_to == TLgUSED )
	{
	return next- from- 1;
	}

    if  ( link->tl_to == TLgFREE )
	{
	space= indMergeFreeSlots( ind, next );
	return next- from+ space;
	}

    LLLLDEB(next,link->tl_to,TLgUSED,TLgFREE);
    return -1;
    }

/************************************************************************/
/*									*/
/*  Claim a slot: Divide the range from pos to nextHeader in a		*/
/*  used slot and a free slot.						*/
/*									*/
/************************************************************************/

static int indSplit(	IND *		ind,
			int		slot,
			int		size,
			int		upto )
    {
    int		newHeader= slot+ 1+ size;
    TrieLink *	link= LINK(ind,slot);
    TrieLink *	next= LINK(ind,newHeader);

    /*
    SDEB("FROM");
    indTLprint(ind,slot);
    indTLprint(ind,upto);
    */

    next->tl_to= TLgFREE;
    next->tl_key= upto- newHeader- 1;

    link->tl_key= size;
    link->tl_to= TLgUSED;

    /*
    SDEB("SPLIT");
    indTLprint(ind,slot);
    indTLprint(ind,newHeader);
    indTLprint(ind,upto);
    */

    if  ( ind->indFirstFreeLinkSlot > slot	&&
	  ind->indFirstFreeLinkSlot < upto	)
	{ ind->indFirstFreeLinkSlot= newHeader;	}

    return newHeader;
    }

static void indClaimSlot(	IND *		ind,
				int		slot,
				int		size )
    {
    TrieLink *	link= LINK(ind,slot);

    if  ( size < 1 || size >= 0xfffffe )
	{
	LLDEB(slot,size); indINDprint(ind);
	return;
	}

    if  ( slot == ind->indLastHeadLinkSlot )
	{
	if  ( link->tl_to != TLgFREE )
	    {
	    indTLprint(ind,ind->indLastHeadLinkSlot);
	    return;
	    }

	ind->indLastHeadLinkSlot=
		indSplit( ind, slot, size, ind->indAllocatedLinks );
	}
    else{
	int	nextHeader= slot+ 1+ link->tl_key;

	if  ( size > link->tl_key )
	    {
	    if  ( nextHeader == ind->indLastHeadLinkSlot )
		{
		ind->indLastHeadLinkSlot=
		    indSplit( ind, slot, size, ind->indAllocatedLinks );
		}
	    else{
		TrieLink *	next= LINK(ind,nextHeader);
		int		upto= slot+ 1+ link->tl_key+ 1+ next->tl_key;

		if  ( slot+ 1+ size < upto )
		    { indSplit( ind, slot, size, upto );	}
		else{
		    link->tl_key= size;
		    link->tl_to= TLgUSED;

		    if  ( ind->indFirstFreeLinkSlot == nextHeader )
			{ ind->indFirstFreeLinkSlot= upto;	}
		    }
		}
	    }
	else{
	    if  ( size < link->tl_key )
		{ indSplit( ind, slot, size, nextHeader );	}
	    else{
		link->tl_key= size;
		link->tl_to= TLgUSED;
		}
	    }
	}
    }

/************************************************************************/
/*									*/
/*  Allocate more link pages.						*/
/*									*/
/************************************************************************/

static int indClaimLinkPages(		IND *		ind,
					int		n )
    {
    int			linksNeeded;
    int			pagesNeeded;
    TrieLink **		fresh;

    TrieLink *		link;

    linksNeeded= ind->indLastHeadLinkSlot+ 1+ n+ 1;
					    /* +1: For the last FREE!	*/
    pagesNeeded= ( linksNeeded+ TLsBLOCK- 1 )/ TLsBLOCK;

    fresh= (TrieLink **)realloc( ind->indLinkPages,
					    pagesNeeded* sizeof(TrieLink *) );
    if  ( ! fresh )
	{ LXDEB(pagesNeeded,fresh); return -1;	}
    ind->indLinkPages= fresh;

    while( ind->indAllocatedLinks < linksNeeded )
	{
	TrieLink *	newpage;

	newpage= (TrieLink *)malloc( TLsBLOCK* sizeof( TrieLink ) );
	if  ( ! newpage )
	    { XDEB(newpage); return -1;	}

	ind->indLinkPages[ind->indAllocatedLinks/TLsBLOCK]= newpage;
	ind->indAllocatedLinks += TLsBLOCK;

	link= LINK(ind,ind->indLastHeadLinkSlot);
	link->tl_key= ind->indAllocatedLinks- ind->indLastHeadLinkSlot- 1;
	link->tl_to= TLgFREE;
	}

    return ind->indLastHeadLinkSlot;
    }

/************************************************************************/

static void indReallocLinks(		IND *		ind,
					int		slot,
					int		from )
    {
    TrieLink *		link= LINK(ind,from);
    int			size= link->tl_key;
    int			i;

    link->tl_to= TLgFREE;
    if  ( ind->indFirstFreeLinkSlot > from )
	{ ind->indFirstFreeLinkSlot=  from;	}

    from++; slot++;
    for ( i= 0; i < size; i++ )
	{
	*LINK(ind,slot)= *LINK(ind,from);
	slot++;from++;
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Allocate links.							*/
/*									*/
/************************************************************************/

int	indTLalloc(	IND *	ind,
			int	old,
			int	n )
    {
    int		slot;
    int		space;

    /*APP_DEB(appDebug( "indTLalloc(,old=%d,n=%d)\n", old, n ));*/
    if  ( old == 0 )
	{ LDEB(old); return -1;	}


    if  ( n < 1 || n >= 65534 )
	{ appDebug("n=%d\n", n ); return -1;	}

    /********************************************************************/
    /*  Fits in old position?						*/
    /********************************************************************/
    if  ( old > 0 )
	{
	space= indSlotCapacity( ind, old- 1 );
	if  ( space >= n )
	    {
	    indClaimSlot( ind, old- 1, n );
	    return old;
	    }
	}

    /********************************************************************/
    /*  Look for an other position, it can be fitted into.		*/
    /*  Start at the first free position (candidate) and look for a	*/
    /*  slot where the new block can fit.				*/
    /********************************************************************/
    slot= 0;
    if  ( ind->indAllocatedLinks > 0 )
	{
	int		foundFree= 0;

	slot= ind->indFirstFreeLinkSlot;

	while( slot <= ind->indLastHeadLinkSlot )
	    {
	    TrieLink *	link= LINK(ind,slot);

	    /************************************************************/
	    /*  Shift free candidate to first slot that is actually	*/
	    /*  Or to a used one closely before it.			*/
	    /*  This makes this loop shorter in the next round.		*/
	    /************************************************************/
	    if  ( ! foundFree )
		{ ind->indFirstFreeLinkSlot= slot;	}

	    /*  Occupied slots cannot be used */
	    if  ( link->tl_to == TLgUSED )
		{ slot += link->tl_key+ 1; continue; }

	    if  ( link->tl_to == TLgFREE && link->tl_key > 0 )
		{
		space= indSlotCapacity( ind, slot );
		/*  Would avoid n+1: that gives empty FREE slots that	*/
		/*  will be reused difficultly. As this is dramatically	*/
		/*  slow, we do not do this:				*/
		/*  if  ( space == n || space > n+ 1 )			*/

		if  ( space >= n )
		    {
		    indClaimSlot( ind, slot, n );
		    if  ( old > 0 && slot != old- 1 )
			{ indReallocLinks( ind, slot, old- 1 );	}
		    return slot+ 1;
		    }

		foundFree= 1;
		slot += space+ 1; continue;
		}

	    if  ( link->tl_to == TLgFREE && link->tl_key == 0 )
		{ slot += link->tl_key+ 1; continue; }

	    LLLLDEB(slot,link->tl_to,TLgUSED,TLgFREE);
	    indINDprint(ind);
	    return -1;
	    }
	}

    /********************************************************************/
    /*  Reallocate links structure.					*/
    /********************************************************************/
    slot= indClaimLinkPages( ind, 1+ n );
    if  ( slot < 0 )
	{ LDEB(slot); return -1;	}

    space= indSlotCapacity( ind, slot );
    if  ( space < n )
	{ LLLDEB(space,n,slot); return -1;	}

    indClaimSlot( ind, slot, n );
    if  ( old > 0 && slot != old- 1 )
	{ indReallocLinks( ind, slot, old- 1 );	}

    return slot+ 1;
    }

/************************************************************************/
/*									*/
/*  Free a link.							*/
/*									*/
/************************************************************************/

void indTLfree(		IND *	ind,
			int	tl )
    {
    TrieLink *	link= LINK(ind,tl- 1);

    link->tl_to= TLgFREE;

    if  ( ind->indFirstFreeLinkSlot > tl && link->tl_key > 0 )
	{ ind->indFirstFreeLinkSlot= tl;	}
    }
