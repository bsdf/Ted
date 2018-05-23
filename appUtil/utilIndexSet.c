/************************************************************************/
/*									*/
/*  Implement sets of (positive) integers. The implementation through	*/
/*  a paged bit mask was originally designed for 16 bit unicode values.	*/
/*									*/
/************************************************************************/

#   include	"appUtilConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>
#   include	<string.h>

#   include	"utilIndexSet.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Bookkeeping.							*/
/*									*/
/************************************************************************/

void utilInitIndexSet(	IndexSet *	is )
    {
    is->isPages= (unsigned char **)0;
    is->isPageCount= 0;
    }

/************************************************************************/
/*									*/
/*  Free the memory that is used inside an integer set.			*/
/*									*/
/************************************************************************/

void utilCleanIndexSet(	IndexSet *	is )
    {
    int		page;

    for ( page= 0; page < is->isPageCount; page++ )
	{
	if  ( is->isPages[page] )
	    { free( is->isPages[page] );	}
	}

    if  ( is->isPages )
	{ free( is->isPages );	}
    }

/************************************************************************/
/*									*/
/*  Claim a page in an integer set.					*/
/*									*/
/************************************************************************/

static unsigned char * utilIndexSetClaimPage(	IndexSet *	is,
						int		page )
    {
    if  ( page >= is->isPageCount )
	{
	unsigned char **	pages;

	pages= (unsigned char **)realloc(
			    is->isPages, (page+ 1)* sizeof(unsigned char *) );
	if  ( ! pages )
	    { LXDEB(page,pages); return (unsigned char *)0;	}
	is->isPages= pages;

	while( is->isPageCount <= page )
	    { is->isPages[is->isPageCount++]= (unsigned char *)0; }
	}

    if  ( ! is->isPages[page] )
	{
	unsigned char *	fresh;

	fresh= (unsigned char *)malloc( ISET_PSZB* sizeof(unsigned char) );
	if  ( ! fresh )
	    { XDEB(fresh); return (unsigned char *)0;	}

	memset( fresh, 0, ISET_PSZB );

	is->isPages[page]= fresh;
	}

    return is->isPages[page];
    }

static void utilIndexSetReleasePage(	IndexSet *	is,
					int		page )
    {
    if  ( page < is->isPageCount && is->isPages[page] )
	{
	free( is->isPages[page] );
	is->isPages[page]= (unsigned char *)0;
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Set the bit belonging to a certain integer.				*/
/*									*/
/************************************************************************/

int utilIndexSetAdd(		IndexSet *		is,
				int			v )
    {
    int			page;
    int			pidx;
    unsigned char	mask;
    unsigned char *	pbuf;

    if  ( v < 0 )
	{ LDEB(v); return 1;	}

    page= ISET_PAGE(v);
    pidx= ISET_PIDX(v);
    mask= ISET_MASK(v);

    pbuf= utilIndexSetClaimPage( is, page );
    if  ( ! pbuf )
	{ LXDEB(v,pbuf); return -1;	}

    pbuf[pidx] |= mask;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Clear the bit belonging to a certain integer.			*/
/*									*/
/************************************************************************/

void utilIndexSetRemove(	IndexSet *		is,
				int			v )
    {
    int			page;
    int			pidx;
    unsigned char	mask;

    if  ( v < 0 )
	{ LDEB(v); return;	}

    page= ISET_PAGE(v);
    pidx= ISET_PIDX(v);
    mask= ISET_MASK(v);

    if  ( page >= is->isPageCount )
	{ return;	}

    if  ( ! is->isPages[page] )
	{ return;	}

    is->isPages[page][pidx] &= ~mask;

    return;
    }

/************************************************************************/
/*									*/
/*  Number of 1 bits in a byte value.					*/
/*									*/
/************************************************************************/

static unsigned char UTIL_IndexSetBitCounts[256];

static void utilIndexSetFillBitCounts( void )
    {
    int		pos;

    for ( pos= 0; pos < 256; pos++ )
	{
	unsigned int	p= pos;

	UTIL_IndexSetBitCounts[pos]= 0;

	while( p )
	    {
	    if  ( p & 0x01 )
		{ UTIL_IndexSetBitCounts[pos]++;	}

	    p >>= 1;
	    }
	}
    }

/************************************************************************/
/*									*/
/*  Discard the potential result of an operation.			*/
/*									*/
/************************************************************************/

static void utilIndexSetTrimResult(	IndexSet *	res,
					int		upto )
    {
    while( res->isPageCount > upto )
	{
	res->isPageCount--;
	if  ( res->isPages[res->isPageCount] )
	    { free( res->isPages[res->isPageCount] );	}
	}
    }

/************************************************************************/
/*									*/
/*  Copy a page to the result set.					*/
/*									*/
/************************************************************************/

static int utilIndexSetCopyPage(	IndexSet *		to,
					const IndexSet *	from,
					int			page )
    {
    int			n= 0;

    if  ( UTIL_IndexSetBitCounts[255] == 0 )
	{ utilIndexSetFillBitCounts();	}

    if  ( from->isPages[page] )
	{
	unsigned char *	bufr= (unsigned char *)0;
	int		pidx;

	bufr= utilIndexSetClaimPage( to, page );
	if  ( ! bufr )
	    { LXDEB(page,bufr); return -1;	}

	memcpy( bufr, from->isPages[page], ISET_PSZB );

	for ( pidx= 0; pidx < ISET_PSZB; bufr++, pidx++ )
	    { n += UTIL_IndexSetBitCounts[*bufr];	}
	}

    if  ( n == 0 )
	{ utilIndexSetReleasePage( to, page );	}

    return n;
    }

/************************************************************************/
/*									*/
/*  Intersect two sets of integers.					*/
/*									*/
/*  Return the member count of the result, or -1 on failure.		*/
/*									*/
/************************************************************************/

int utilIndexSetIntersect(	IndexSet *		res,
				const IndexSet *	is1,
				const IndexSet *	is2 )
    {
    int			count= 0;
    int			pages= 0;

    int			page;

    if  ( UTIL_IndexSetBitCounts[255] == 0 )
	{ utilIndexSetFillBitCounts();	}

    page= 0;
    while( page < is1->isPageCount && page < is2->isPageCount )
	{
	int				n= 0;

	if  ( is1->isPages[page] && is2->isPages[page] )
	    {
	    unsigned char *		bufr;
	    const unsigned char *	buf1= is1->isPages[page];
	    const unsigned char *	buf2= is2->isPages[page];

	    int				pidx;

	    bufr= utilIndexSetClaimPage( res, page );
	    if  ( ! bufr )
		{ LXDEB(page,bufr); return -1;	}

	    n= 0;
	    for ( pidx= 0; pidx < ISET_PSZB; bufr++, buf1++, buf2++, pidx++ )
		{
		*bufr= *buf1 & *buf2;
		n += UTIL_IndexSetBitCounts[*bufr];
		}

	    if  ( n == 0 )
		{ utilIndexSetReleasePage( res, page );	}
	    }

	if  ( n > 0 )
	    { pages= page+ 1;				}
	count += n; page++;
	}

    utilIndexSetTrimResult( res, pages );

    return count;
    }

/************************************************************************/
/*									*/
/*  Union two sets of integers.						*/
/*									*/
/*  Return the member count of the result, or -1 on failure.		*/
/*									*/
/************************************************************************/

int utilIndexSetUnion(	IndexSet *		res,
				const IndexSet *	is1,
				const IndexSet *	is2 )
    {
    int			count= 0;
    int			pages= 0;

    int			page;

    if  ( UTIL_IndexSetBitCounts[255] == 0 )
	{ utilIndexSetFillBitCounts();	}

    page= 0;
    while( page < is1->isPageCount && page < is2->isPageCount )
	{
	int			n= 0;

	if  ( is1->isPages[page] && is2->isPages[page] )
	    {
	    const unsigned char *	buf1= is1->isPages[page];
	    const unsigned char *	buf2= is2->isPages[page];
	    unsigned char *		bufr= (unsigned char *)0;

	    int				pidx;

	    bufr= utilIndexSetClaimPage( res, page );
	    if  ( ! bufr )
		{ LXDEB(page,bufr); return -1;	}

	    for ( pidx= 0; pidx < ISET_PSZB; bufr++, buf1++, buf2++, pidx++ )
		{
		*bufr= *buf1 | *buf2;
		n += UTIL_IndexSetBitCounts[*bufr];
		}
	    }
	else{
	    if  ( is1->isPages[page] )
		{
		n= utilIndexSetCopyPage( res, is1, page );
		if  ( n < 0 )
		    { LDEB(n); return -1;	}
		}

	    if  ( is2->isPages[page] )
		{
		n= utilIndexSetCopyPage( res, is2, page );
		if  ( n < 0 )
		    { LDEB(n); return -1;	}
		}
	    }

	if  ( n > 0 )
	    { pages= page+ 1;				}
	else{ utilIndexSetReleasePage( res, page );	}
	count += n; page++;
	}

    while( page < is1->isPageCount )
	{
	int			n= 0;

	n= utilIndexSetCopyPage( res, is1, page );
	if  ( n < 0 )
	    { LDEB(n); return -1;	}

	if  ( n > 0 )
	    { pages= page+ 1;				}
	else{ utilIndexSetReleasePage( res, page );	}
	count += n; page++;
	}

    while( page < is2->isPageCount )
	{
	int			n= 0;

	n= utilIndexSetCopyPage( res, is2, page );
	if  ( n < 0 )
	    { LDEB(n); return -1;	}

	if  ( n > 0 )
	    { pages= page+ 1;				}
	else{ utilIndexSetReleasePage( res, page );	}
	count += n; page++;
	}

    utilIndexSetTrimResult( res, pages );

    return count;
    }

/************************************************************************/
/*									*/
/*  Subtract the contents of the second set from the first one.		*/
/*									*/
/************************************************************************/

int utilIndexSetRemoveAll(	IndexSet *		res,
				const IndexSet *	is1,
				const IndexSet *	is2 )
    {
    int			count= 0;
    int			pages= 0;

    int			page;

    if  ( UTIL_IndexSetBitCounts[255] == 0 )
	{ utilIndexSetFillBitCounts();	}

    page= 0;
    while( page < is1->isPageCount && page < is2->isPageCount )
	{
	int			n= 0;

	if  ( is1->isPages[page] && is2->isPages[page] )
	    {
	    const unsigned char *	buf1= is1->isPages[page];
	    const unsigned char *	buf2= is2->isPages[page];
	    unsigned char *		bufr= (unsigned char *)0;

	    int				pidx;

	    bufr= utilIndexSetClaimPage( res, page );
	    if  ( ! bufr )
		{ LXDEB(page,bufr); return -1;	}

	    for ( pidx= 0; pidx < ISET_PSZB; bufr++, buf1++, buf2++, pidx++ )
		{
		*bufr= *buf1 & ~*buf2;
		n += UTIL_IndexSetBitCounts[*bufr];
		}
	    }
	else{
	    if  ( is1->isPages[page] )
		{
		n= utilIndexSetCopyPage( res, is1, page );
		if  ( n < 0 )
		    { LDEB(n); return -1;	}
		}
	    }

	if  ( n > 0 )
	    { pages= page+ 1;				}
	else{ utilIndexSetReleasePage( res, page );	}
	count += n; page++;
	}

    while( page < is1->isPageCount )
	{
	int			n= 0;

	n= utilIndexSetCopyPage( res, is1, page );
	if  ( n < 0 )
	    { LDEB(n); return -1;	}

	if  ( n > 0 )
	    { pages= page+ 1;				}
	else{ utilIndexSetReleasePage( res, page );	}
	count += n; page++;
	}

    utilIndexSetTrimResult( res, pages );

    return count;
    }

/************************************************************************/
/*									*/
/*  Copy an integer set.						*/
/*									*/
/************************************************************************/

int utilCopyIndexSet(		IndexSet *		to,
				const IndexSet *	from )
    {
    utilCleanIndexSet( to );
    utilInitIndexSet( to );

    return utilIndexSetUnion( to, to, from );
    }

/************************************************************************/
/*									*/
/*  Traverse an integer set.						*/
/*									*/
/*  Return the number of visited entries or -1 on failure.		*/
/*									*/
/************************************************************************/

int utilIndexSetForAll(	const IndexSet *		is,
				IndexSetForOne		forOne,
				void *			through )
    {
    int		page;
    int		n= 0;

    for ( page= 0; page < is->isPageCount; page++ )
	{
	int			pidx;
	const unsigned char *	pbuf= is->isPages[page];
	int			one= page* ISET_PSZb;

	if  ( ! pbuf )
	    { continue;	}

	for ( pidx= 0; pidx < ISET_PSZB; pbuf++, pidx++ )
	    {
	    unsigned char	mask= 0x01;
	    int			b;

	    for ( b= 0; b < 8; b++ )
		{
		if  ( (*pbuf) & mask )
		    {
		    if  ( (*forOne)( one, through ) )
			{ LDEB(one); return -1;	}
		    n++;
		    }

		mask <<= 1; one++;
		}
	    }
	}

    return n;
    }

/************************************************************************/
/*									*/
/*  Navigation over sets.						*/
/*									*/
/************************************************************************/

int utilIndexSetGetNext(		const IndexSet *	is,
					int			n )
    {
    int		page;
    int		pidx;
    int		mbit;
    int		mask;

    if  ( n < 0 )
	{ n= 0;		}
    else{ n++;		}

    page= ISET_PAGE(n);
    pidx= ISET_PIDX(n);
    mbit= ISET_MBIT(n);
    mask= ISET_MASK(n);

    while( page < is->isPageCount )
	{
	const unsigned char *	pbuf= is->isPages[page];

	if  ( pbuf )
	    {
	    pbuf += pidx;

	    while( pidx < ISET_PSZB )
		{
		if  ( *pbuf )
		    {
		    while( mbit < 8 )
			{
			if  ( (*pbuf) & mask )
			    { return ISET_INDEX( page, pidx, mbit );	}

			mbit++; mask <<= 1;
			}
		    }

		pbuf++; pidx++; mbit= 0; mask= 0x01;
		}
	    }

	page++; pidx= 0;
	}

    return -1;
    }

int utilIndexSetGetPrev(		const IndexSet *	is,
					int			n )
    {
    int		page;
    int		pidx;
    int		mbit;
    int		mask;

    if  ( n < 0 )
	{ n= ISET_INDEX(is->isPageCount,0,0)-1;		}
    else{ n--;						}

    if  ( n < 0 )
	{ return -1;	}

    page= ISET_PAGE(n);
    pidx= ISET_PIDX(n);
    mbit= ISET_MBIT(n);
    mask= ISET_MASK(n);

    while( page >= 0 )
	{
	const unsigned char *	pbuf= is->isPages[page];

	if  ( pbuf )
	    {
	    pbuf += pidx;

	    while( pidx >= 0 )
		{
		if  ( *pbuf )
		    {
		    while( mbit >= 0 )
			{
			if  ( (*pbuf) & mask )
			    { return ISET_INDEX( page, pidx, mbit );	}

			mbit--; mask >>= 1;
			}
		    }

		pbuf--; pidx--; mbit= 8-1; mask= 0x80;
		}
	    }

	page--; pidx= ISET_PSZB- 1;
	}

    return -1;
    }

int utilIndexSetGetNextHole(	const IndexSet *	is,
				int			n )
    {
    int		page;
    int		pidx;
    int		mbit;
    int		mask;

    if  ( n < 0 )
	{ n= 0;		}
    else{ n++;		}

    page= ISET_PAGE(n);
    pidx= ISET_PIDX(n);
    mbit= ISET_MBIT(n);
    mask= ISET_MASK(n);

    while( page < is->isPageCount )
	{
	const unsigned char *	pbuf= is->isPages[page];

	if  ( ! pbuf )
	    { return ISET_INDEX( page, pidx, mbit );	}
	else{
	    pbuf += pidx;

	    while( pidx < ISET_PSZB )
		{
		if  ( ! *pbuf )
		    { return ISET_INDEX( page, pidx, mbit );	}
		else{
		    while( mbit < 8 )
			{
			if  ( ! ( (*pbuf) & mask ) )
			    { return ISET_INDEX( page, pidx, mbit );	}

			mbit++; mask <<= 1;
			}
		    }

		pbuf++; pidx++; mbit= 0; mask= 0x01;
		}
	    }

	page++; pidx= 0;
	}

    return ISET_INDEX( page, pidx, mbit );
    }

/************************************************************************/
/*									*/
/*  Construct a set for an array of unicode code points.		*/
/*									*/
/************************************************************************/

int utilIndexSetAddArray(		IndexSet *		is,
					const int *		indexes,
					int			indexCount )
    {
    int		g;

    for ( g= 0; g < indexCount; g++ )
	{
	if  ( indexes[g] >= 0			&&
	      utilIndexSetAdd( is, indexes[g] )	)
	    { LLDEB(g,indexes[g]);	}
	}

    return 0;
    }
