#   include	"indConfig.h"

#   include	<stdlib.h>
#   include	<string.h>

#   include	"indlocal.h"
#   include	<appDebugon.h>

#   ifdef NeXT
#	include	<libc.h>
#	include	<mach/mach.h>
#	define	IND_MAP_FD
#   endif

#   ifdef __linux__
#	include	<sys/types.h>
#	include	<sys/mman.h>
#	include	<errno.h>
#	ifndef MAP_FAILED
#	    define	MAP_FAILED	((void *)-1)
#	endif
#	define	IND_MMAP
#   endif

#   include	<stddef.h>
#   include	<unistd.h>
#   include	<fcntl.h>
#   include	<utilEndian.h>

				/**
				 *  The size of the header. Originally,
				 *  the header was written through binary 
				 *  file IO. For that reason, the header
				 *  has unused space.
				 */
#   define	HEADER_BYTES	52

# define POS_MAGIC 0
# define POS_START 8
# define POS_NODE_COUNT 24
# define POS_NODES_ALLOCATED 28
# define POS_LINKS_ALLOCATED 40
# define POS_FREE_LINK 44
# define POS_LAST_HEAD_LINK 48

/************************************************************************/
/*  Swap a buffer							*/
/************************************************************************/

static void	indSwapInt( unsigned char * buf )
    {
    register unsigned char	c;

    c= buf[0]; buf[0]= buf[3]; buf[3]= c;
    c= buf[1]; buf[1]= buf[2]; buf[2]= c;
    }

/************************************************************************/
/*  Make an index.							*/
/************************************************************************/

IND * indINDmake( int	readOnly )
    {
    IND *	ind= (IND *)malloc( sizeof( IND ) );

    if  ( ind )
	{
	ind->ind_magic= INDMAGIC;
	ind->ind_fd= -1;

	ind->ind_start= -1;
	ind->ind_readonly= readOnly;

	ind->ind_nodes= (TrieNode *)0;
	ind->indNodePages= (TrieNode **)0;
	ind->indNodeCount= 0;
	ind->indAllocatedNodes= 0;

	ind->ind_links= (TrieLink *)0;
	ind->indLinkPages= (TrieLink **)0;
	ind->indAllocatedLinks= 0;
	ind->indFirstFreeLinkSlot= 0;
	ind->indLastHeadLinkSlot= 0;

	ind->indMmappedFile= (unsigned char *)0;
	ind->indMmappedSize= 0;
	}

    return ind;
    }

void	indINDfree( IND *	ind )
    {
    int		i;
    int		page;
    char *	p;
    int		pagesAreMemoryMapped= 0;

#   ifdef IND_MAP_FD
    if  ( ind->indMmappedFile )
	{
	kern_return_t	ret;

	ret= vm_deallocate( task_self(),
		    (vm_address_t)ind->indMmappedFile, ind->indMmappedSize );
	if  ( ret != KERN_SUCCESS )
	    { LDEB(ret); }

	pagesAreMemoryMapped= 1;
	}
#   endif


#   ifdef IND_MMAP
    if  ( ind->indMmappedFile )
	{
	int	ret;

	ret= munmap( (void *)ind->indMmappedFile, ind->indMmappedSize );
	if  ( ret )
	    { LDEB(ret); }

	pagesAreMemoryMapped= 1;
	}
#   endif

    if  ( ind->indNodePages )
	{
	page= 0;
	if  ( ! pagesAreMemoryMapped )
	    {
	    for ( i= 0; i < ind->indAllocatedNodes; i += TNsBLOCK )
		{
		p= (char *)ind->indNodePages[page++];
		if  ( p )
		    { free( p );	}
		}
	    }

	free( (char *)ind->indNodePages );
	}

    if  ( ind->indLinkPages )
	{
	page= 0;
	if  ( ! pagesAreMemoryMapped )
	    {
	    for ( i= 0; i < ind->indAllocatedLinks; i += TLsBLOCK )
		{
		p= (char *)ind->indLinkPages[page++];
		if  ( p )
		    { free( p );	}
		}
	    }

	free( (char *)ind->indLinkPages );
	}

    free( (char *)ind );
    }

/************************************************************************/
/*  Write an index to file						*/
/************************************************************************/
static int	indWriteData(	IND *		ind,
				int		fd,
				int		swap )
    {
    int		n;
    int		i;
    unsigned	sz;
    int		page;

    page= 0;
    for ( n= 0; n < ind->indAllocatedNodes; n += TNsBLOCK )
	{
	sz= TNsBLOCK * sizeof( TrieNode );
	if  ( swap )
	    {
	    for ( i= 0; i < TNsBLOCK; i++ )
		{
		TrieNode *	tn= &(ind->indNodePages[page][i]);

		indSwapInt( (unsigned char *)&tn->tn_transitions );
		}
	    }

	if  ( write( fd, (char *)ind->indNodePages[page], sz ) != sz )
	    { return -1; }

	if  ( swap )
	    {
	    for ( i= 0; i < TNsBLOCK; i++ )
		{
		TrieNode *	tn= &(ind->indNodePages[page][i]);

		indSwapInt( (unsigned char *)&tn->tn_transitions );
		}
	    }
	page++;
	}

    page= 0;
    for ( n= 0; n < ind->indAllocatedLinks; n += TLsBLOCK )
	{
	sz= TLsBLOCK * sizeof( TrieLink );
	if  ( swap )
	    {
	    for ( i= 0; i < TLsBLOCK; i++ )
		{ indSwapInt( (unsigned char *)&ind->indLinkPages[page][i] ); }
	    }
	if  ( write( fd, (char *)ind->indLinkPages[page], sz ) != sz )
	    { LDEB(fd); return -1; }
	if  ( swap )
	    {
	    for ( i= 0; i < TLsBLOCK; i++ )
		{ indSwapInt( (unsigned char *)&ind->indLinkPages[page][i] ); }
	    }
	page++;
	}

    return 0;
    }

int	indINDwrite(	IND *		ind,
			const char *	filename )
    {
    int			fd= creat( filename, 0666 );
    int			rval= 0;
    unsigned char	hd[HEADER_BYTES];

    unsigned short	one= 1;
    int			machineBigEndian= ( *(unsigned char *)&one == 0 );

    memset( hd, 0, sizeof(hd) );

    if  ( fd < 0 )
	{ return -1; }

#   if 0
    appDebug( "WRITE: ROOT= %d, nnode= %d, ncount= %d lcount= %d\n",
	ind->ind_start,	ind->indNodeCount, ind->indAllocatedNodes,
	ind->indAllocatedLinks );
#   endif

    if  ( sizeof(TrieLink) != 8 )
	{ LLDEB(sizeof(TrieLink),8); return -1;		}
    if  ( sizeof(TrieNode) != 8 )
	{ LLDEB(sizeof(TrieNode),8); return -1;	}

    utilEndianStoreLeInt32( ind->ind_magic, hd+ POS_MAGIC );
    utilEndianStoreLeInt32( ind->ind_start, hd+ POS_START );
    utilEndianStoreLeInt32( ind->indNodeCount, hd+ POS_NODE_COUNT );
    utilEndianStoreLeInt32( ind->indAllocatedNodes, hd+ POS_NODES_ALLOCATED );
    utilEndianStoreLeInt32( ind->indAllocatedLinks, hd+ POS_LINKS_ALLOCATED );
    utilEndianStoreLeInt32( ind->indFirstFreeLinkSlot, hd+ POS_FREE_LINK );
    utilEndianStoreLeInt32( ind->indLastHeadLinkSlot, hd+ POS_LAST_HEAD_LINK );

    if  ( write( fd, hd, sizeof(hd) ) != sizeof(hd) )
	{ LDEB(fd); rval= -1;	}
    else{
	if  ( indWriteData( ind, fd, machineBigEndian ) )
	    { LDEB(fd); rval= -1;	}
	}

    if  ( ! rval )
	{
	utilEndianStoreBeInt32( ind->ind_magic, hd+ POS_MAGIC );
	utilEndianStoreBeInt32( ind->ind_start, hd+ POS_START );
	utilEndianStoreBeInt32( ind->indNodeCount, hd+ POS_NODE_COUNT );
	utilEndianStoreBeInt32( ind->indAllocatedNodes, hd+ POS_NODES_ALLOCATED );
	utilEndianStoreBeInt32( ind->indAllocatedLinks, hd+ POS_LINKS_ALLOCATED );
	utilEndianStoreBeInt32( ind->indFirstFreeLinkSlot, hd+ POS_FREE_LINK );
	utilEndianStoreBeInt32( ind->indLastHeadLinkSlot, hd+ POS_LAST_HEAD_LINK );

	if  ( write( fd, hd, sizeof(hd) ) != sizeof(hd) )
	    { LDEB(fd); rval= -1;	}
	else{
	    if  ( indWriteData( ind, fd, ! machineBigEndian ) )
		{ LDEB(fd); rval= -1;	}
	    }
	}

    if  ( close( fd ) )
	{ rval= -1;	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Read an index from file						*/
/*									*/
/************************************************************************/

IND *	indINDread(	const char *	filename,
			int		readOnly )
    {
    int			fd;

    IND *		ind;
    TrieNode *		nodes;
    TrieLink *		links;
    int			n;
    unsigned		sz;
    int			page;
    long		fileOff= 0;
    long		memOff= 0;
    unsigned long	mmapSize;
    unsigned long	mmapOffset;
    int			memory_mapped= 0;
    int			oppositeEndian= 0;

    unsigned short	one= 1;
    int			machineBigEndian= ( *(unsigned char *)&one == 0 );

    unsigned char	hd[HEADER_BYTES];

    if  ( sizeof(TrieNode) != 8 )
	{ LLDEB(sizeof(TrieNode),8); return (IND *)0;	}
    if  ( sizeof(TrieLink) != 8 )
	{ LLDEB(sizeof(TrieLink),8); return (IND *)0;	}

    fd= open( filename, 0 );
    if  ( fd < 0 )
	{ SLDEB(filename,readOnly); return (IND *)0;	}

    ind= indINDmake( readOnly );
    if  ( ! ind )
	{ XDEB(ind); goto failure;	}

    ind->ind_readonly= readOnly;

    if  ( read( fd, hd, sizeof(hd) ) != sizeof(hd) )
	{ LDEB(sizeof(hd)); goto failure;	}

    if  ( machineBigEndian )
	{
	ind->ind_magic= utilEndianExtractBeInt32( hd );

	if  ( ind->ind_magic == INDMAGIC )
	    {
	    ind->ind_start= utilEndianExtractBeInt32( hd+ POS_START );
	    ind->indNodeCount= utilEndianExtractBeInt32( hd+ POS_NODE_COUNT );
	    ind->indAllocatedNodes= utilEndianExtractBeInt32( hd+ POS_NODES_ALLOCATED );
	    ind->indAllocatedLinks= utilEndianExtractBeInt32( hd+ POS_LINKS_ALLOCATED );
	    ind->indFirstFreeLinkSlot= utilEndianExtractBeInt32( hd+ POS_FREE_LINK );
	    ind->indLastHeadLinkSlot= utilEndianExtractBeInt32( hd+ POS_LAST_HEAD_LINK );

	    fileOff += sizeof(hd);
	    }
	else{
	    if  ( ind->ind_magic == INDMAGIC_R )
		{
		fileOff= sizeof(hd);
		fileOff += utilEndianExtractLeInt32( hd+ POS_NODES_ALLOCATED )* sizeof(TrieNode);
		fileOff += utilEndianExtractLeInt32( hd+ POS_LINKS_ALLOCATED )* sizeof(TrieLink);

		if  ( lseek( fd, fileOff, SEEK_SET ) != fileOff	||
		      read( fd, hd, sizeof(hd) ) != sizeof(hd)	)
		    { LDEB(fileOff); goto failure;	}

		ind->ind_magic= utilEndianExtractBeInt32( hd );
		if  ( ind->ind_magic != INDMAGIC )
		    { XXDEB(ind->ind_magic,INDMAGIC); goto failure;	}

		ind->ind_start= utilEndianExtractBeInt32( hd+ POS_START );
		ind->indNodeCount= utilEndianExtractBeInt32( hd+ POS_NODE_COUNT );
		ind->indAllocatedNodes= utilEndianExtractBeInt32( hd+ POS_NODES_ALLOCATED );
		ind->indAllocatedLinks= utilEndianExtractBeInt32( hd+ POS_LINKS_ALLOCATED );
		ind->indFirstFreeLinkSlot= utilEndianExtractBeInt32( hd+ POS_FREE_LINK );
		ind->indLastHeadLinkSlot= utilEndianExtractBeInt32( hd+ POS_LAST_HEAD_LINK );

		fileOff += sizeof(hd);

		oppositeEndian= 1;
		}
	    else{ LDEB(ind->ind_magic); goto failure;	}
	    }
	}
    else{
	ind->ind_magic= utilEndianExtractLeInt32( hd );

	if  ( ind->ind_magic == INDMAGIC )
	    {
	    ind->ind_start= utilEndianExtractLeInt32( hd+ POS_START );
	    ind->indNodeCount= utilEndianExtractLeInt32( hd+ POS_NODE_COUNT );
	    ind->indAllocatedNodes= utilEndianExtractLeInt32( hd+ POS_NODES_ALLOCATED );
	    ind->indAllocatedLinks= utilEndianExtractLeInt32( hd+ POS_LINKS_ALLOCATED );
	    ind->indFirstFreeLinkSlot= utilEndianExtractLeInt32( hd+ POS_FREE_LINK );
	    ind->indLastHeadLinkSlot= utilEndianExtractLeInt32( hd+ POS_LAST_HEAD_LINK );

	    fileOff += sizeof(hd);
	    }
	else{
	    if  ( ind->ind_magic == INDMAGIC_R )
		{
		fileOff= sizeof(hd);
		fileOff += utilEndianExtractBeInt32( hd+ POS_NODES_ALLOCATED )* sizeof(TrieNode);
		fileOff += utilEndianExtractBeInt32( hd+ POS_LINKS_ALLOCATED )* sizeof(TrieLink);

		if  ( lseek( fd, fileOff, SEEK_SET ) != fileOff	||
		      read( fd, hd, sizeof(hd) ) != sizeof(hd)	)
		    { LDEB(fileOff); goto failure;	}

		ind->ind_magic= utilEndianExtractLeInt32( hd );
		if  ( ind->ind_magic != INDMAGIC )
		    { XXDEB(ind->ind_magic,INDMAGIC); goto failure;	}

		ind->ind_start= utilEndianExtractLeInt32( hd+ POS_START );
		ind->indNodeCount= utilEndianExtractLeInt32( hd+ POS_NODE_COUNT );
		ind->indAllocatedNodes= utilEndianExtractLeInt32( hd+ POS_NODES_ALLOCATED );
		ind->indAllocatedLinks= utilEndianExtractLeInt32( hd+ POS_LINKS_ALLOCATED );
		ind->indFirstFreeLinkSlot= utilEndianExtractLeInt32( hd+ POS_FREE_LINK );
		ind->indLastHeadLinkSlot= utilEndianExtractLeInt32( hd+ POS_LAST_HEAD_LINK );

		fileOff += sizeof(hd);

		oppositeEndian= 1;
		}
	    else{ SXDEB(filename,ind->ind_magic); goto failure;	}
	    }
	}

#   if 0
    appDebug( "READ:  ROOT= %d, nnode= %d, ncount= %d lcount= %d\n",
	ind->ind_start,	ind->indNodeCount, ind->indAllocatedNodes,
	ind->indAllocatedLinks );
#   endif

    if  ( oppositeEndian )
	{ memOff= fileOff % 512;	}
    else{ memOff= fileOff; 		}

    mmapSize=	memOff+
		ind->indAllocatedNodes* sizeof(TrieNode)+
		ind->indAllocatedLinks* sizeof(TrieLink);
    mmapOffset= fileOff- memOff;

#   ifdef IND_MAP_FD
    if  ( readOnly )
	{
	kern_return_t	ret;

	ret= map_fd( fd, (vm_offset_t)mmapOffset,
		    (vm_address_t *)&(ind->indMmappedFile), 1, mmapSize );
	if  ( ret != KERN_SUCCESS )
	    { LDEB(ret);	}
	else{
	    memory_mapped= 1;
	    ind->indMmappedSize= mmapSize;
	    }

	}
#   endif

#   ifdef IND_MMAP
    if  ( readOnly )
	{
	void *	mmapResult;

	mmapResult= mmap( (void *)0, mmapSize,
			    PROT_READ, MAP_SHARED, fd, mmapOffset );

	if  ( mmapResult == MAP_FAILED )
	    { LSDEB(errno,strerror(errno));	}
	else{
	    ind->indMmappedFile= (unsigned char *)mmapResult;
	    ind->indMmappedSize= mmapSize;
	    memory_mapped= 1;
	    }
	}
#   endif

    if  ( memory_mapped )
	{
	ind->ind_nodes= (TrieNode *)( ind->indMmappedFile+ memOff );
	memOff += ind->indAllocatedNodes* sizeof(TrieNode);

	ind->ind_links= (TrieLink *)( ind->indMmappedFile+ memOff );
	memOff += ind->indAllocatedLinks* sizeof(TrieLink);
	}

    sz= (ind->indAllocatedNodes/TNsBLOCK)* sizeof( TrieNode * );
    ind->indNodePages= (TrieNode **)malloc( sz );

    if  ( ! ind->indNodePages )
	{ goto failure;	}

    page= 0;
    for ( n= 0; n < ind->indAllocatedNodes; n += TNsBLOCK )
	{ ind->indNodePages[page++]= (TrieNode *)0; }

    sz= (ind->indAllocatedLinks/TLsBLOCK)* sizeof( TrieLink * );
    ind->indLinkPages= (TrieLink **)malloc( sz );

    if  ( ! ind->indLinkPages )
	{ goto failure;	}

    page= 0;
    for ( n= 0; n < ind->indAllocatedLinks; n += TLsBLOCK )
	{ ind->indLinkPages[page++]= (TrieLink *)0; }

    page= 0;
    for ( n= 0; n < ind->indAllocatedNodes; n += TNsBLOCK )
	{
	if  ( ! memory_mapped )
	    {
	    sz= TNsBLOCK * sizeof( TrieNode );
	    ind->indNodePages[page++]= nodes= (TrieNode *)malloc( sz );
	    if  ( ! nodes )
		{ goto failure;	}

	    if  ( read( fd, (char *)nodes, sz ) != sz )
		{ goto failure;	}

	    fileOff += sz;
	    }
	else{ ind->indNodePages[page++]= ind->ind_nodes+ n;	}
	}

    page= 0;
    for ( n= 0; n < ind->indAllocatedLinks; n += TLsBLOCK )
	{
	if  ( ! memory_mapped )
	    {
	    sz= TLsBLOCK * sizeof( TrieLink );
	    ind->indLinkPages[page++]= links= (TrieLink *)malloc( sz );
	    if  ( ! links )
		{ goto failure;	}

	    if  ( read( fd, (char *)links, sz ) != sz )
		{ goto failure;	}

	    fileOff += sz;
	    }
	else{ ind->indLinkPages[page++]= ind->ind_links+ n;	}
	}

    if  ( close( fd ) )
	{ indINDfree( ind ); return (IND *)0; }

    return ind;

failure:
    close( fd );
    if  ( ind )
	{ indINDfree( ind );	}
    return (IND *)0;
    }
