/************************************************************************/
/*									*/
/*  Manage lines in a paragraph.					*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	<stdlib.h>

#   include	<appDebugon.h>

#   include	"docBuf.h"
#   include	"docParaParticules.h"
#   include	"docDebug.h"

/************************************************************************/
/*									*/
/*  Remember about a line in a Text Item.				*/
/*									*/
/************************************************************************/

TextLine * docInsertTextLine(	BufferItem *	bi,
				int		line )
    {
    TextLine *		tl;
    int			newSize;

    if  ( bi->biParaLineCount % 10 )
	{ newSize= bi->biParaLineCount+  1; }
    else{ newSize= bi->biParaLineCount+ 11; }

    newSize *= sizeof(TextLine);

    tl= (TextLine *)realloc( bi->biParaLines, newSize );
    if  ( ! tl )
	{ LXDEB(bi->biParaLineCount,tl); return (TextLine *)0; }
    bi->biParaLines= tl;

    if  ( line == -1 )
	{ line= bi->biParaLineCount; }
    else{
	int		i;

	for ( i= bi->biParaLineCount; i > line; i-- )
	    { tl[i]= tl[i-1];	}
	}

    tl += line;

    docInitTextLine( tl );

    bi->biParaLineCount++;
    return tl;
    }

/************************************************************************/
/*									*/
/*  Find the line number for a certain string offset in the paragraph.	*/
/*									*/
/*  NOTE: This does not expect the paragraph to be formatted. We just	*/
/*	fail because there are no lines. We do not crash however.	*/
/*									*/
/************************************************************************/

int docFindLineOfPosition(	int *				pLine,
				int *				pFlags,
				const DocumentPosition *	dp,
				int				lastOne )
    {
    const BufferItem *	paraNode= dp->dpNode;
    const int		stroff= dp->dpStroff;

    int			l= 0;
    int			r= paraNode->biParaLineCount;
    int			m= ( l+ r )/ 2;

    const TextLine *	tl= paraNode->biParaLines+ m;
    const int		paraStrlen= docParaStrlen( paraNode );

    if  ( r <= 0 )
	{ LSDEB(r,docTreeTypeStr(paraNode->biTreeType)); return -1;	}
    if  ( stroff < 0 || stroff > paraStrlen )
	{ LDEB(stroff); return -1;	}

    switch( lastOne )
	{
	case PARAfindPAST:
	    if  ( stroff == paraStrlen )
		{
		*pLine= paraNode->biParaLineCount;
		if  ( pFlags )
		    {
		    *pFlags= POSflagPARA_TAIL;

		    if  ( stroff == 0 )
			{ *pFlags |= POSflagPARA_HEAD; }
		    }
		return 0;
		}

	    /*FALLTHROUGH*/
	case PARAfindLAST:
	    /* Binary search for last: tl->tlStroff <= stroff */
	    while( l < m )
		{
		if  ( tl->tlStroff <= stroff )
		    { l= m;	}
		else{ r= m;	}

		m= ( l+ r )/ 2; tl= paraNode->biParaLines+ m;
		}

	    if  ( stroff < tl->tlStroff )
		{ m--;	}
	    break;

	case PARAfindFIRST:
	    /* Binary search for first: tl->tlStroff+ tl->tlStrlen >= stroff */
	    while( l < m )
		{
		if  ( tl->tlStroff+ tl->tlStrlen < stroff )
		    { l= m;	}
		else{ r= m;	}

		m= ( l+ r )/ 2; tl= paraNode->biParaLines+ m;
		}

	    if  ( stroff > tl->tlStroff+ tl->tlStrlen )
		{ m++;	}
	    break;

	default:
	    LDEB(lastOne); return -1;
	}

    if  ( m < 0 || m >= paraNode->biParaLineCount )
	{
	const int checkGeometry= 0;

	LLLLDEB(stroff,m,lastOne,paraNode->biParaLineCount);
	docListNode( 0, paraNode, checkGeometry );
	return -1;
	}

    *pLine= m;

    if  ( pFlags )
	{
	const TextLine *	tl= paraNode->biParaLines+ m;

	*pFlags= 0;

	if  ( stroff == tl->tlStroff )
	    { *pFlags |= POSflagLINE_HEAD; }
	if  ( stroff == tl->tlStroff+ tl->tlStrlen )
	    { *pFlags |= POSflagLINE_TAIL; }

	if  ( stroff == 0 )
	    { *pFlags |= POSflagPARA_HEAD; }
	if  ( stroff == paraStrlen )
	    { *pFlags |= POSflagPARA_TAIL; }
	}

    return 0;
    }

