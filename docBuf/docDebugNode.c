#   include	"docBufConfig.h"

#   include	"docBuf.h"
#   include	"docTreeNode.h"
#   include	"docDebug.h"
#   include	<docTextParticule.h>
#   include	<docTextLine.h>
#   include	<appDebugon.h>
#   include	"docNodeTree.h"

#   define	LIST_PARA_STRING	1
#   define	LIST_PARTICULES		1
#   define	LIST_FIRST_LINE		0
#   define	LIST_LINES		1

#   define	IS			2

static int docCheckGroupLeft(	const BufferItem *	node )
    {
    int		rval= 0;

    int		prev= 0;
    int		here;

    if  ( node->biParent && node->biNumberInParent > 0 )
	{
	prev= node->biParent->biChildren[node->biNumberInParent-1]->
						biLeftParagraphs;
	}

    here= node->biLeftParagraphs;

    if  ( here- prev !=
	    node->biChildren[node->biChildCount- 1]->biLeftParagraphs )
	{
	SLDEB(docLevelStr(node->biLevel),here- prev);
	LDEB(node->biChildren[node->biChildCount- 1]->biLeftParagraphs);
	rval= -1;
	}

    return rval;
    }

static int docCheckChild(	const BufferItem *	parent,
				const BufferItem *	child,
				int			i,
				const LayoutPosition *	lpTop,
				LayoutPosition *	lpBelow,
				int			checkGeometry )
    {
    int				rval= 0;
    const LayoutPosition *	belowChild;

    if  ( docIsRowNode( child ) )
	{ belowChild= &(child->biRowBelowAllCellsPosition);	}
    else{ belowChild= &(child->biBelowPosition);		}

    if  ( checkGeometry )
	{
	const LayoutPosition *	belowParent;

	if  ( docIsRowNode( parent ) )
	    { belowParent= &(parent->biRowBelowAllCellsPosition);	}
	else{ belowParent= &(parent->biBelowPosition);			}

	if  ( lpTop )
	    {
	    int d= docCompareLayoutPositions( &(child->biTopPosition), lpTop );
	    const char * direction= (char *)0;
	    if  ( d > 0 )
		{ direction= "ABOVE";	}
	    if  ( d < 0 )
		{ direction= "BELOW";	}

	    if  ( direction )
		{
		appDebug( "############## %s %d in %s: %s Expected\n",
					docLevelStr(child->biLevel), i,
					docLevelStr(parent->biLevel),
					direction );

		LLDEB(child->biTopPosition.lpPage,lpTop->lpPage);
		LLDEB(child->biTopPosition.lpColumn,lpTop->lpColumn);
		LLDEB(child->biTopPosition.lpPageYTwips,lpTop->lpPageYTwips);
		rval= -1;
		}
	    }

	if  ( docCompareLayoutPositions( &(child->biTopPosition),
					    &(parent->biTopPosition) ) < 0 )
	    {
	    appDebug( "############## %s %d in %s:\n",
					docLevelStr(child->biLevel), i,
					docLevelStr(parent->biLevel) );
	    rval= -1;
	    }

	if  ( docCompareLayoutPositions( belowChild, belowParent ) > 0 )
	    {
	    appDebug( "############## %s %d in %s:\n",
					docLevelStr(child->biLevel), i,
					docLevelStr(parent->biLevel) );
	    rval= -1;
	    }
	}

    if  ( child->biParent != parent )
	{
	appDebug( "############## %s %d in %s:\n",
				    docLevelStr(child->biLevel), i,
				    docLevelStr(parent->biLevel) );
	XXDEB(child->biParent,parent);
	rval= -1;
	}

    if  ( child->biNumberInParent != i )
	{
	appDebug( "############## %s %d in %s:\n",
				    docLevelStr(child->biLevel), i,
				    docLevelStr(parent->biLevel) );
	LLDEB(child->biNumberInParent,i);
	rval= -1;
	}

    if  ( ! docValidChildLevel( parent->biLevel, child->biLevel ) )
	{
	appDebug( "############## %s %d in %s:\n",
				    docLevelStr(child->biLevel), i,
				    docLevelStr(parent->biLevel) );
	LLDEB(child->biLevel,parent->biLevel);
	rval= -1;
	}

    if  ( child->biTreeType != parent->biTreeType )
	{
	appDebug( "############## %s %d in %s:\n",
				    docLevelStr(child->biLevel), i,
				    docLevelStr(parent->biLevel) );
	LLDEB(child->biTreeType,parent->biTreeType);
	rval= -1;
	}

    if  ( child->biLevel == DOClevPARA )
	{
	int	tableNesting= docTableNesting( child );

	if  ( tableNesting != child->biParaTableNesting )
	    {
	    SDEB("#######");
	    LLDEB(tableNesting,child->biParaTableNesting);
	    rval= -1;
	    }
	}
    else{
	if  ( child->biChildCount < 1 )
	    { SLDEB("#######",child->biChildCount);	}
	}

    if  ( lpBelow )
	{ *lpBelow= *belowChild;	}

    return rval;
    }

int docCheckNode(	const BufferItem *	node,
			int			checkGeometry )
    {
    int			i;
    int			rval= 0;

    LayoutPosition	lpHere;

    if  ( node->biLevel == DOClevPARA )
	{
	int			part;
	const TextParticule *	tp;
	int			stroff;
	int			tableNesting= docTableNesting( node );

	if  ( node->biLeftParagraphs != node->biNumberInParent+ 1 )
	    {
	    appDebug( "############## %s:\n",
					docLevelStr(node->biLevel) );
	    LLDEB(node->biLeftParagraphs,node->biNumberInParent+1);
	    }

	if  ( tableNesting != node->biParaTableNesting )
	    {
	    SDEB("#######");
	    LLDEB(tableNesting,node->biParaTableNesting);
	    rval= -1;
	    }

	stroff= 0;
	tp= node->biParaParticules;
	for ( part= 0; part < node->biParaParticuleCount; tp++, part++ )
	    {
	    if  ( tp->tpStroff > docParaStrlen( node )	||
		  tp->tpStroff != stroff		)
		{
		appDebug( "############## %s:\n",
					docLevelStr(node->biLevel) );
		LLLLDEB(part,tp->tpStroff,stroff,docParaStrlen(node));
		rval= -1;
		}

	    stroff= tp->tpStroff+ tp->tpStrlen;
	    if  ( stroff < 0			||
		  stroff >  docParaStrlen( node )	)
		{
		appDebug( "############## %s:\n",
					docLevelStr(node->biLevel) );
		LLLLDEB(part,stroff,tp->tpStrlen,docParaStrlen(node));
		rval= -1;
		}
	    }
	if  ( stroff != docParaStrlen( node ) )
	    {
	    appDebug( "############## %s:\n",
					docLevelStr(node->biLevel) );
	    LLDEB(stroff,docParaStrlen(node));
	    rval= -1;
	    }
	}
    else{
	if  ( node->biChildCount < 1 )
	    { SLDEB("#######",node->biChildCount);	}
	}

    if  ( node->biLevel == DOClevROW )
	{
	if  ( docIsRowNode( node )			&&
	      node->biRowCellCount != node->biChildCount	)
	    {
	    appDebug( "############## %s:\n", docLevelStr(node->biLevel) );
	    LLDEB(node->biRowCellCount,node->biChildCount);
	    }
	}

    if  ( node->biChildCount > 0 )
	{
	if  ( docCheckGroupLeft( node ) )
	    { rval= -1;	}

	if  ( ! node->biParent )
	    {
	    DocumentPosition	dp;
	    docInitDocumentPosition( &dp );

	    if  ( ! docHeadPosition( &dp, (BufferItem *)node ) )
		{
		const BufferItem *	paraBi= dp.dpNode;
		int			n= 1;

		for (;;)
		    {
		    if  ( docNumberOfParagraph( paraBi ) != n )
			{ LLDEB(docNumberOfParagraph(paraBi),n); rval= -1; }

		    paraBi= docNextParagraph( (BufferItem *)paraBi );
		    if  ( ! paraBi )
			{ break;	}
		    n++;
		    }
		}
	    }
	}

    for ( i= 0; i < node->biChildCount; i++ )
	{
	LayoutPosition	lpBelowChild= lpHere;

	if  ( docCheckChild( node, node->biChildren[i], i,
				    &lpHere, &lpBelowChild, checkGeometry ) )
	    { rval= -1;	}

	if  ( ! docIsRowNode( node ) )
	    { lpHere= lpBelowChild;	}

	if  ( docCheckNode( node->biChildren[i], checkGeometry ) )
	    { rval= -1;	}
	}

    return rval;
    }

int docCheckRootNode(	const BufferItem *	node,
			int			checkGeometry )
    {
    while( node->biParent )
	{ node= node->biParent;	}

    return docCheckNode( node, checkGeometry );
    }

static void docListChildren(	int			indent,
				const BufferItem *	node,
				int			checkGeometry )
    {
    int			i;

    LayoutPosition	lp;

    lp= node->biTopPosition;

    for ( i= 0; i < node->biChildCount; i++ )
	{
	docCheckChild( node, node->biChildren[i], i, &lp, &lp, checkGeometry );

	docListNode( indent+ 1, node->biChildren[i], checkGeometry );
	}

    if  ( node->biChildCount > 0 )
	{ docCheckGroupLeft( node );	}

    if  ( checkGeometry						&&
	  ! docIsRowNode( node )					&&
	  ! DOC_SAME_POSITION( &(node->biBelowPosition), &lp )	)
	{
	appDebug( "############## %s :\n", docLevelStr(node->biLevel) );

	LLDEB(node->biBelowPosition.lpPage,lp.lpPage);
	LLDEB(node->biBelowPosition.lpColumn,lp.lpColumn);
	LLDEB(node->biBelowPosition.lpPageYTwips,lp.lpPageYTwips);
	}
    }

static void docListParaNode(	int			indent,
				const BufferItem *	node )
    {
    int				i;
    const TextLine *		tl;
    LayoutPosition		lpHere;

    int				stroff;
    const TextParticule *	tp;

    appDebug( "%*s{ NR= %d TN=%d brk=%s %d particules, %d lines\n",
				    IS* indent+ IS, "",
				    docNumberOfParagraph( node ),
				    node->biParaTableNesting,
				    docBreakKindStr( node->biParaBreakKind ),
				    node->biParaParticuleCount,
				    node->biParaLineCount );

#   if LIST_PARA_STRING
    {
    int		length= docParaStrlen( node );

    stroff= 0;
    while( stroff < length )
	{
	int		l= length- stroff;

	if  ( l > 60 )
	    { l= 60; }

	appDebug( "%*s\"%.*s\"\n",
			IS* indent+ IS, "",
			(int)l, (char *)docParaString( node, stroff ) );

	stroff += l;
	}
    }
#   endif

    stroff= 0;
    tp= node->biParaParticules;
    for ( i= 0; i < node->biParaParticuleCount; tp++, i++ )
	{
	int		list= LIST_PARTICULES;
	const char *	label= "PART";

	if  ( tp->tpStroff != stroff	)
	    { list= 1; label= "####";	}

	if  ( list )
	    { docListParticule( IS* indent+ IS, label, i, node, tp ); }

	stroff= tp->tpStroff+ tp->tpStrlen;
	}

    if  ( stroff != docParaStrlen( node ) )
	{
	SLLDEB("####",stroff,docParaStrlen( node ) );
	if  ( node->biParaParticuleCount > 0 )
	    {
	    i= node->biParaParticuleCount- 1;
	    tp= node->biParaParticules+ i;
	    docListParticule( IS* indent+ IS, "####", i, node, tp );
	    }
	}

    tl= node->biParaLines;
    lpHere= node->biTopPosition;
    for ( i= 0; i < node->biParaLineCount; tl++, i++ )
	{
	int	listLine= LIST_LINES || ( i == 0 && LIST_FIRST_LINE );

	if  ( docCompareLayoutPositions( &(tl->tlTopPosition),
							&lpHere ) < 0 )
	    {
	    appDebug( "###### Line= %d\n", i );
	    LDEB(i);
	    listLine= 1;
	    }

	lpHere= tl->tlTopPosition;
	lpHere.lpPageYTwips += tl->tlLineStride;

	if  ( docCompareLayoutPositions( &(node->biBelowPosition),
							&lpHere ) < 0 )
	    {
	    appDebug( "###### Line= %d\n", i );
	    LDEB(i);
	    listLine= 1;
	    }

	if  ( listLine )
	    { docListTextLine( IS* indent+ IS, "LINE", i, node, tl ); }
	}

    return;
    }

void docListNode(	int			indent,
			const BufferItem *	node,
			int			checkGeometry )
    {
    int				i;

    for ( i= 0; i < indent; i++ )
	{ appDebug( "%-*s", IS, "." );	}

    appDebug( "NODE %4d 0x%08lx: %s(%s) LEFT= %d PG: %d:%d..%d:%d\n",
			    node->biNumberInParent,
			    (unsigned long)node,
			    docLevelStr( node->biLevel ),
			    docTreeTypeStr( node->biTreeType ),
			    node->biLeftParagraphs,
			    node->biTopPosition.lpPage,
			    node->biTopPosition.lpColumn,
			    node->biBelowPosition.lpPage,
			    node->biBelowPosition.lpColumn );

    switch( node->biLevel )
	{
	case DOClevBODY:
	case DOClevSECT:
	case DOClevCELL:
	    appDebug( "%*s{ %4d children\n", IS* indent+ IS, "",
						node->biChildCount );

	    docListChildren( indent, node, checkGeometry );

	    break;

	case DOClevROW:
	    appDebug( "%*s{ %s %4d children %d cells\n", IS* indent+ IS, "",
				docIsRowNode(node)?"T:":"--",
				node->biChildCount,
				node->biRowCellCount );

	    if  ( node->biRowTableFirst >= 0	||
		  node->biRowTablePast >= 0	||
		  node->biRowTableHeaderRow >= 0	)
		{
		appDebug( "%*s  Table %d .. %d Header@%d\n", IS* indent+ IS, "",
						node->biRowTableFirst,
						node->biRowTablePast,
						node->biRowTableHeaderRow );

		if  ( node->biRowTableFirst >= node->biParent->biChildCount )
		    {
		    SLLDEB("####",
			    node->biRowTableFirst,node->biParent->biChildCount);
		    }
		if  ( node->biRowTablePast > node->biParent->biChildCount )
		    {
		    SLLDEB("####",
			    node->biRowTablePast,node->biParent->biChildCount);
		    }
		}

	    if  ( docIsRowNode( node )				&&
		  node->biChildCount != node->biRowCellCount	)
		{ SLLDEB("####", node->biChildCount,node->biRowCellCount); }

	    docListChildren( indent, node, checkGeometry );

	    break;

	case DOClevPARA:
	    docListParaNode( indent, node );
	    break;

	case DOClevOUT:
	default:
	    break;
	}

    appDebug( "%*s}\n", IS* indent+ IS, "" );
    
    return;
    }

void docListRootNode(	int			indent,
			const BufferItem *	node,
			int			checkGeometry )
    {
    while( node->biParent )
	{ node= node->biParent;	}

    docListNode( indent, node, checkGeometry );

    return;
    }

