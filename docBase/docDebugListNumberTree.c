#   include	"docBaseConfig.h"

#   include	<string.h>
#   include	<stdio.h>

#   include	"docDebugListNumberTree.h"
#   include	"docListNumberTree.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Debugging:								*/
/*									*/
/************************************************************************/

static void docDebugListNumberTreeNodeLine(
					int				level,
					int				idx,
					int				of,
					const ListNumberTreeNode *	lntn,
					const char *			bb,
					const char *			aa )
    {
    char	lev[22];

    if  ( level <= 0 )
	{ strcpy( lev, ":::" );			}
    else{ sprintf( lev, "L=%d", level -1 );	}

    appDebug( "%*s%s@%d/%d:P=%d%s%s%s\n", 4* level, "",
				    lev, idx, of,
				    lntn->lntnParaNr,
				    lntn->lntnParaNr>=0?"*":"",
				    bb, aa );
    return;
    }

static int docCheckListNumberTreeNode(	int				level,
					int				idx,
					int				of,
					int				list,
					const ListNumberTreeNode *	lntn,
					int				before,
					int				after )
    {
    int			rval= 0;

    const char *	bb= "";
    const char *	aa= "";

    if  ( before >= 0				&&
	  lntn->lntnParaNr >= 0	&&
	  lntn->lntnParaNr <= before	)
	{ bb= "#+#"; rval= -1; }
    if  ( after >= 0 && lntn->lntnParaNr >= after )
	{ aa= "#-#"; rval= -1; }

    if  ( list )
	{ docDebugListNumberTreeNodeLine( level, idx, of, lntn, bb, aa ); }

    if  ( idx > 0 && lntn->lntnParaNr == -1 )
	{ SLLDEB("##",idx,lntn->lntnParaNr); rval= -1;			}
    if  ( level > 0 && lntn->lntnChildCount == 0 && lntn->lntnParaNr == -1 )
	{
	SLLDEB("##",lntn->lntnChildCount,lntn->lntnParaNr);
	rval= -1;
	}

    if  ( lntn->lntnParaNr >= 0 )
	{ before= lntn->lntnParaNr;	}

    if  ( lntn->lntnChildCount > 0 )
	{
	int		i;

	if  ( ! lntn->lntnChildren )
	    { LDEB(lntn->lntnChildren); return -1;	}

	for ( i= 0; i < lntn->lntnChildCount- 1; i++ )
	    {
	    int		aft;

	    aft= lntn->lntnChildren[i+ 1]->lntnParaNr;
	    if  ( after >= 0 && aft > after )
		{ aft= after;	}

	    if  ( docCheckListNumberTreeNode( level+ 1,
				    i, lntn->lntnChildCount, list,
				    lntn->lntnChildren[i], before, aft ) )
		{ rval= -1;	}

	    if  ( lntn->lntnChildren[i]->lntnParaNr >= 0 )
		{ before= lntn->lntnChildren[i]->lntnParaNr;	}
	    }

	if  ( i < lntn->lntnChildCount )
	    {
	    if  ( docCheckListNumberTreeNode( level+ 1,
				    i, lntn->lntnChildCount, list,
				    lntn->lntnChildren[i], before, after ) )
		{ rval= -1;	}
	    }
	}

    return rval;
    }

int docListListNumberNode(	const ListNumberTreeNode *	lntn )
    {
    const int indent= 0;
    const int list= 1;
    const int idx= -1;
    const int of= 0;
    const int before= -1;
    const int after= -1;

    return docCheckListNumberTreeNode( indent, idx, of, list,
						    lntn, before, after );
    }

int docCheckListNumberNode(	const ListNumberTreeNode *	lntn )
    {
    const int indent= 0;
    const int list= 0;
    const int idx= -1;
    const int of= 0;
    const int before= -1;
    const int after= -1;

    return docCheckListNumberTreeNode( indent, idx, of, list,
						    lntn, before, after );
    }

int docListNumberTreeLogPath(	ListNumberTreeNode **	path,
				int *			nums,
				int			level )
    {
    int			rval= 0;

    int			lvl;
    const char * const	bb= "";
    const char * const	aa= "";

    if  ( ! path[0] )
	{ XDEB(path[0]); return -1;	}

    docDebugListNumberTreeNodeLine( 0, -1, 0, path[0], bb, aa );

    for ( lvl= 1; lvl <= level; lvl++ )
	{
	ListNumberTreeNode *	node= path[lvl-1];
	int			idx= nums[lvl-1];

	if  ( ! node )
	    {
	    appDebug( "%*sL=%d##\n", 4* lvl, "", lvl );
	    continue;
	    }

	if  ( idx < 0 )
	    {
	    appDebug( "%*sL=%d@%d <|\n", 4* lvl, "", lvl, idx );
	    continue;
	    }
	if  ( idx >= node->lntnChildCount )
	    {
	    appDebug( "%*sL=%d@%d >|\n", 4* lvl, "", lvl, idx );
	    continue;
	    }

	docDebugListNumberTreeNodeLine( lvl, idx, node->lntnChildCount,
					node->lntnChildren[idx], bb, aa );

	if  ( path[lvl] != node->lntnChildren[idx] )
	    {
	    SLXXDEB("##",lvl,path[lvl],&(node->lntnChildren[idx]));
	    rval= -1;
	    }
	}

    return rval;
    }

# if 0
@   include	<string.h>
@   include	"docListDepth.h"

extern void xxxx( void );

static int docCheckTreeLevels(		ListNumberTreeNode *		root,
					const unsigned char *		ilvl,
					int				count )
    {
    int		i;
    int		nums[DLmaxLEVELS];

    for ( i= 0; i < count; i++ )
	{
	if  ( ilvl[i] < PPoutlineBODYTEXT )
	    {
	    if  ( docListNumberTreeGetNumberPath( nums, root, ilvl[i], i ) )
		{
		SLLDEB("##MISSING (or wrong level)",i,ilvl[i]);
		docListListNumberNode( root );
		return -1;
		}
	    }
	else{
	    if  ( ! docListNumberTreeGetNumberPath( nums, root, ilvl[i], i ) )
		{
		SLLDEB("##PRESENT",i,ilvl[i]); docListListNumberNode( root );
		return -1;
		}
	    }
	}

    return 0;
    }

static int docListLevelTreeCheckNotSet(	ListNumberTreeNode *	root,
					int			paraNr )
    {
    ListNumberTreeNode *	path[DLmaxLEVELS+1];
    int				nums[DLmaxLEVELS+1];
    int				level;

    if  ( ! docListNumberTreeFindParagraph( &level, path, nums, root, paraNr ) )
	{
	LDEB(paraNr); docListListNumberNode( root );
	return -1;
	}

    return 0;
    }

static int docListLevelTreeSetLevel(	ListNumberTreeNode *	root,
					int			ilvl,
					int			paraNr )
    {
    ListNumberTreeNode *	path[DLmaxLEVELS+1];
    int				nums[DLmaxLEVELS+1];

    if  ( ilvl < PPoutlineBODYTEXT )
	{
	int	level;

	if  ( docListNumberTreeInsertParagraph( root, ilvl, paraNr ) )
	    {
	    LLDEB(paraNr,ilvl); docListListNumberNode( root );
	    return -1;
	    }

	if  ( docListNumberTreeFindParagraph( &level, path, nums,
							root, paraNr )	||
	      level != ilvl						)
	    {
	    LLDEB(paraNr,ilvl); docListListNumberNode( root );
	    return -1;
	    }

	if  ( docCheckListNumberNode( root ) )
	    { docListListNumberNode( root ); return -1;	}
	}

    return 0;
    }

static int docListLevelTreeUnsetLevel(	ListNumberTreeNode *	root,
					int			ilvl,
					int			paraNr )
    {
    if  ( ilvl < PPoutlineBODYTEXT )
	{
	if  ( docListNumberTreeDeleteParagraph( root, paraNr ) )
	    {
	    LLDEB(paraNr,ilvl);
	    return -1;
	    }

	if  ( docListLevelTreeCheckNotSet( root, paraNr ) )
	    { LLDEB(ilvl,paraNr); return -1;	}

	if  ( docCheckListNumberNode( root ) )
	    { docListListNumberNode( root ); return -1;	}
	}

    return 0;
    }

static int docListLevelCheckOutside(	ListNumberTreeNode *	root,
					int			count )
    {
    if  ( docListLevelTreeCheckNotSet( root, -2 ) )
	{ LDEB(-2); return -1;	}
    if  ( docListLevelTreeCheckNotSet( root, -1 ) )
	{ LDEB(-1); return -1;	}

    if  ( docListLevelTreeCheckNotSet( root, count ) )
	{ LDEB(count); return -1;	}
    if  ( docListLevelTreeCheckNotSet( root, count+ 1 ) )
	{ LDEB(count+ 1); return -1;	}

    return 0;
    }

static int docListLevelCheckEmpty(	ListNumberTreeNode *	root,
					int			count )
    {
    int		paraNr;

    for ( paraNr= -1; paraNr <= count; paraNr++ )
	{
	if  ( docListLevelTreeCheckNotSet( root, paraNr ) )
	    { LLDEB(count,paraNr); return -1;	}
	}

    return 0;
    }

static int docListLevelTreeFillForward( const char *		label,
					ListNumberTreeNode *	root,
					const unsigned char *	ilvl,
					int			count )
    {
    int		i;

    SSDEB("FILL",label);

    for ( i= 0; i < count; i++ )
	{
	if  ( docListLevelTreeSetLevel( root, ilvl[i], i ) )
	    { LLDEB(i,ilvl[i]); return -1;	}
	}

    SSDEB("RESULT",label); docListListNumberNode( root );

    if  ( docListLevelCheckOutside( root, count ) )
	{ LDEB(count); return -1;	}
    if  ( docCheckTreeLevels( root, ilvl, count ) )
	{ LDEB(1); return -1;	}

    SSDEB("FILLED",label); docListListNumberNode( root );

    return 0;
    }

static int docListLevelTreeFillBackward( const char *		label,
					ListNumberTreeNode *	root,
					const unsigned char *	ilvl,
					int			count )
    {
    int		i;

    SSDEB("FILL",label);

    for ( i=  count- 1; i >= 0; i-- )
	{
	if  ( docListLevelTreeSetLevel( root, ilvl[i], i ) )
	    { LLDEB(i,ilvl[i]); return -1;	}
	}

    SSDEB("RESULT",label); docListListNumberNode( root );

    if  ( docListLevelCheckOutside( root, count ) )
	{ LDEB(count); return -1;	}
    if  ( docCheckTreeLevels( root, ilvl, count ) )
	{ LDEB(1); return -1;	}

    SSDEB("FILLED",label); docListListNumberNode( root );

    return 0;
    }

static int docListLevelTreeFinishForward( const char *		label,
					ListNumberTreeNode *	root,
					const unsigned char *	ilvl,
					int			count )
    {
    int		i;

    SSDEB("CLEANUP",label); docListListNumberNode( root );

    for ( i= 0; i < count; i++ )
	{
	if  ( docListLevelTreeUnsetLevel( root, ilvl[i], i ) )
	    { LLDEB(i,ilvl[i]); return -1;	}
	}

    if  ( docListLevelCheckEmpty( root, count ) )
	{ LDEB(count); return -1;	}

    SSDEB("SUCCESS",label); docListListNumberNode( root );

    return 0;
    }

static int docListLevelTreeFinishBackward( const char *		label,
					ListNumberTreeNode *	root,
					const unsigned char *	ilvl,
					int			count )
    {
    int		i;

    SSDEB("CLEANUP",label); docListListNumberNode( root );

    for ( i=  count- 1; i >= 0; i-- )
	{
	if  ( docListLevelTreeUnsetLevel( root, ilvl[i], i ) )
	    { LLDEB(i,ilvl[i]); return -1;	}
	}

    if  ( docListLevelCheckEmpty( root, count ) )
	{ LDEB(count); return -1;	}

    SSDEB("SUCCESS",label); docListListNumberNode( root );

    return 0;
    }

static int docListLevelUnsetCase(	const char *		label,
					int			pos,
					ListNumberTreeNode *	root,
					unsigned char *		ilvl,
					int			count )
    {
    if  ( docListLevelTreeFillForward( label, root, ilvl, count ) )
	{ LDEB(count); return -1;	}

    if  ( docListLevelTreeUnsetLevel( root, ilvl[pos], pos ) )
	{ LLDEB(pos,ilvl[pos]); return -1;	}
    ilvl[pos]= PPoutlineBODYTEXT;

    if  ( docCheckTreeLevels( root, ilvl, count ) )
	{ LDEB(1); return -1;	}

    if  ( docListLevelTreeFinishForward( label, root, ilvl, count ) )
	{ LDEB(sizeof(ilvl)); return -1;	}

    return 0;
    }

static void docListLevelStartCase(	const char *		label,
					ListNumberTreeNode *	root,
					unsigned char *		ilvl,
					int			count )
    {
    SSDEB("START",label);

    memset( ilvl, PPoutlineBODYTEXT, count );
    docCleanListNumberTreeNode( root );
    docInitListNumberTreeNode( root );

    return;
    }

static int docListLevelTreeCase1(	ListNumberTreeNode *	root,
					unsigned char *		ilvl,
					int			count )
    {
    const char *	label= "CASE1";

    docListLevelStartCase( label, root, ilvl, count );

    ilvl[1]= 0;
	ilvl[2]= 3;
	ilvl[3]= 3;
	ilvl[4]= 3;
	ilvl[5]= 3;
	ilvl[6]= 3;
    ilvl[11]= 0;
	ilvl[12]= 3;
	ilvl[13]= 3;
	ilvl[14]= 3;
	ilvl[15]= 3;
	ilvl[16]= 3;

    if  ( docListLevelUnsetCase( label, 11,  root, ilvl, count ) )
	{ SDEB(label); return -1;	}

    return 0;
    }

static int docListLevelTreeCase2(	ListNumberTreeNode *	root,
					unsigned char *		ilvl,
					int			count )
    {
    const char *	label= "CASE2";

    docListLevelStartCase( label, root, ilvl, count );

    ilvl[1]= 0;
	ilvl[2]= 3;
	ilvl[3]= 3;
	ilvl[4]= 3;
	ilvl[5]= 3;
	ilvl[6]= 3;
    ilvl[11]= 0;
	ilvl[12]= 4;
	ilvl[13]= 4;
	ilvl[14]= 4;
	ilvl[15]= 4;
	ilvl[16]= 4;

    if  ( docListLevelUnsetCase( label, 11,  root, ilvl, count ) )
	{ SDEB(label); return -1;	}

    return 0;
    }

static int docListLevelTreeCase3(	ListNumberTreeNode *	root,
					unsigned char *		ilvl,
					int			count )
    {
    const char *	label= "CASE3";

    docListLevelStartCase( label, root, ilvl, count );

    ilvl[1]= 0;
	ilvl[2]= 4;
	ilvl[3]= 4;
	ilvl[4]= 4;
	ilvl[5]= 4;
	ilvl[6]= 4;
    ilvl[11]= 0;
	ilvl[12]= 3;
	ilvl[13]= 3;
	ilvl[14]= 3;
	ilvl[15]= 3;
	ilvl[16]= 3;

    if  ( docListLevelUnsetCase( label, 11, root, ilvl, count ) )
	{ SDEB(label); return -1;	}

    return 0;
    }

static int docListLevelSetCase(		const char *		label,
					int			pos,
					int			level,
					ListNumberTreeNode *	root,
					unsigned char *		ilvl,
					int			count )
    {
    if  ( docListLevelTreeFillForward( label, root, ilvl, count ) )
	{ LDEB(count); return -1;	}

    if  ( docListLevelTreeSetLevel( root, level, pos ) )
	{ SLLDEB(label,pos,level); return -1;	}
    ilvl[pos]= level;

    if  ( docCheckTreeLevels( root, ilvl, count ) )
	{ SDEB(label); return -1;	}

    if  ( docListLevelTreeFinishForward( label, root, ilvl, count ) )
	{ SDEB(label); return -1;	}

    return 0;
    }

static int docListLevelTreeCase4(	ListNumberTreeNode *	root,
					unsigned char *		ilvl,
					int			count )
    {
    const char *	label= "CASE4";
    const int		pos= 6;
    const int		level= 3;

    docListLevelStartCase( label, root, ilvl, count );

    ilvl[pos-3]= 4;
			/*  MAKES THE DIFFERENCE: ilvl[pos-2]= 5; */
    ilvl[pos-1]= 6;
    ilvl[pos+1]= 5;

    if  ( docListLevelSetCase( label, pos, level, root, ilvl, count ) )
	{ SLLDEB(label,pos,level); return -1;	}

    return 0;
    }

static int docListLevelTreeCase5(	ListNumberTreeNode *	root,
					unsigned char *		ilvl,
					int			count )
    {
    const char *	label= "CASE5";
    const int		pos= 20;
    const int		level= 2;

    SDEB(label);

    ilvl[3]= 2;
    ilvl[4]= 3;
	ilvl[5]= 4;
	ilvl[8]= 4;
    ilvl[11]= 3;
	ilvl[14]= 4;
	ilvl[17]= 4;
    ilvl[18]= 3;
	ilvl[23]= 4;
    ilvl[25]= 3;

    if  ( docListLevelSetCase( label, pos, level, root, ilvl, count ) )
	{ SLLDEB(label,pos,level); return -1;	}

    return 0;
    }

static int docListLevelTreeCase6(	ListNumberTreeNode *	root,
					unsigned char *		ilvl,
					int			count )
    {
    const char *	label= "CASE6";
    const int		pos= 39;
    const int		level= 1;

    docListLevelStartCase( label, root, ilvl, count );

    ilvl[2]= 1;
    ilvl[37]= 2;
	    ilvl[38]= 4;
	    ilvl[41]= 4;
	    ilvl[44]= 4;
	ilvl[46]= 3;
    ilvl[54]= 2;
	    ilvl[56]= 4;

    if  ( docListLevelSetCase( label, pos, level, root, ilvl, count ) )
	{ SLLDEB(label,pos,level); return -1;	}

    return 0;
    }

static int steps[PPoutlineBODYTEXT]=
    {
    3,		/*  0  */
    7,		/*  1  */
    17,		/*  2  */
    37,		/*  3  */
    89,		/*  4  */
    191,	/*  5  */
    439,	/*  6  */
    977,	/*  7  */
    2213,	/*  8  */
    };

static int nstep= sizeof(steps)/ sizeof(int);

static void docListLevelFillArray(	unsigned char *		ilvl,
					int			count )
    {
    int	l;
    int	deep= 0;

    while( deep < nstep && steps[deep] <  count )
	{ deep++;	}

    for ( l= 0; l < deep; l++ )
	{
	int	i;
	int	step= steps[l];

	for ( i= deep- l; i < count; i += step )
	    {
	    int		level= deep- l-1;

	    ilvl[i]= level;
	    }
	}

    return;
    }

static int docListLevelTreeCaseForward(	ListNumberTreeNode *	root,
					unsigned char *		ilvl,
					int			count )
    {
    const char *	label= "FORWARD";

    docListLevelStartCase( label, root, ilvl, count );

    docListLevelFillArray( ilvl, count );

    if  ( docListLevelTreeFillForward( label, root, ilvl, count ) )
	{ LDEB(count); return -1;	}

    if  ( docListLevelTreeFinishForward( label, root, ilvl, count ) )
	{ LDEB(sizeof(ilvl)); return -1;	}

    return 0;
    }

static int docListLevelTreeCaseBackward( ListNumberTreeNode *	root,
					unsigned char *		ilvl,
					int			count )
    {
    const char *	label= "BACKWARD";

    docListLevelStartCase( label, root, ilvl, count );

    docListLevelFillArray( ilvl, count );

    if  ( docListLevelTreeFillBackward( label, root, ilvl, count ) )
	{ LDEB(count); return -1;	}

    if  ( docListLevelTreeFinishBackward( label, root, ilvl, count ) )
	{ LDEB(sizeof(ilvl)); return -1;	}

    return 0;
    }

static int docListLevelTreeCaseChaos(	ListNumberTreeNode *	root,
					unsigned char *		ilvl,
					int			count )
    {
    const char *	label= "CHAOS";
    int			l;
    int			i;
    int			deep= 0;
    int			step= 0;

    docListLevelStartCase( label, root, ilvl, count );

    while( deep < nstep && steps[deep] <  count )
	{ deep++;	}

    for ( l= 0; l < deep; l++ )
	{
	int	i;
	int	step= steps[l];

	for ( i= deep- l; i < count; i += step )
	    {
	    int		level= deep- l-1;

	    if  ( ilvl[i] < PPoutlineBODYTEXT )
		{
		if  ( docListLevelTreeUnsetLevel( root, ilvl[i], i ) )
		    { SLLDEB(label,i,ilvl[i]); return -1;	}

		ilvl[i]= PPoutlineBODYTEXT;

		if  ( docCheckTreeLevels( root, ilvl, count ) )
		    { LDEB(1); return -1;	}
		}

	    if  ( docListLevelTreeSetLevel( root, level, i ) )
		{ SLLDEB(label,i,ilvl[i]); return -1;	}

	    ilvl[i]= level;

	    if  ( docCheckTreeLevels( root, ilvl, count ) )
		{ LDEB(1); return -1;	}
	    }
	}

    SSDEB(label,"RESULT");

    if  ( docListLevelCheckOutside( root, count ) )
	{ LDEB(count); return -1;	}
    if  ( docCheckTreeLevels( root, ilvl, count ) )
	{ LDEB(1); return -1;	}

    SSDEB(label,"CLEANUP");

    step= ( 2* count )/ 3;

    for ( i= 0; i < count; i++ )
	{
	int	paraNr= ( i* step ) % count;

	if  ( ilvl[paraNr] < PPoutlineBODYTEXT )
	    {
	    if  ( docListLevelTreeUnsetLevel( root, ilvl[paraNr], paraNr ) )
		{ LLDEB(paraNr,ilvl[paraNr]); return -1;	}

	    ilvl[paraNr]= PPoutlineBODYTEXT;
	    }
	}

    for ( i= 0; i < count; i++ )
	{
	if  ( ilvl[i] < PPoutlineBODYTEXT )
	    { LLDEB(i,ilvl[i]); return -1;	}
	}

    if  ( docListLevelCheckEmpty( root, count ) )
	{ LDEB(count); return -1;	}

    SSDEB(label,"SUCCESS");

    return 0;
    }

void xxxx( void )
    {
    ListNumberTreeNode	tree;

#   if 0
    static unsigned char ilvl[107];
    static unsigned char ilvl[257];
    static unsigned char ilvl[523];
    static unsigned char ilvl[4937];
#   else
    static unsigned char ilvl[4937];
#   endif

    memset( ilvl, PPoutlineBODYTEXT, sizeof(ilvl) );
    docInitListNumberTreeNode( &tree );

    if  ( docListLevelTreeCase6( &tree, ilvl, sizeof(ilvl) ) )
	{ LDEB(1); goto ready;	}

    if  ( docListLevelTreeCase5( &tree, ilvl, sizeof(ilvl) ) )
	{ LDEB(1); goto ready;	}

    if  ( docListLevelTreeCase4( &tree, ilvl, sizeof(ilvl) ) )
	{ LDEB(1); goto ready;	}

    if  ( docListLevelTreeCase3( &tree, ilvl, sizeof(ilvl) ) )
	{ LDEB(1); goto ready;	}

    if  ( docListLevelTreeCase2( &tree, ilvl, sizeof(ilvl) ) )
	{ LDEB(1); goto ready;	}

    if  ( docListLevelTreeCase1( &tree, ilvl, sizeof(ilvl) ) )
	{ LDEB(1); goto ready;	}

    if  ( docListLevelTreeCaseChaos( &tree, ilvl, sizeof(ilvl) ) )
	{ LDEB(1); goto ready;	}

    if  ( docListLevelTreeCaseForward( &tree, ilvl, sizeof(ilvl) ) )
	{ LDEB(1); goto ready;	}

    if  ( docListLevelTreeCaseBackward( &tree, ilvl, sizeof(ilvl) ) )
	{ LDEB(1); goto ready;	}

  ready:

    docCleanListNumberTreeNode( &tree );

    return;
    }
# endif
