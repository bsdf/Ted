/************************************************************************/
/*									*/
/*  Get/Move/Set Selections in paragraphs.				*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	<appDebugon.h>

#   include	"docBuf.h"
#   include	"docDebug.h"
#   include	"docParaParticules.h"
#   include	"docParaString.h"
#   include	"docNodeTree.h"

#   define DOCisADMINISTRATIVE(k) \
	    ( (k) == DOCkindFIELDHEAD || (k) == DOCkindFIELDTAIL )

/*  Navigate to the next valid document position.			*/

/************************************************************************/
/*									*/
/*  Return the next selectable string offset in the document.		*/
/*									*/
/*  Positions inside an UTF-8 sequence and positions between		*/
/*  administrative particules are invalid.				*/
/*  This means that hopping over an administrative particule is not a	*/
/*  step that counts.							*/
/*									*/
/************************************************************************/

int docGotoNextPosition(	DocumentPosition *	dp )
    {
    BufferItem *	paraNode= dp->dpNode;
    int			stroff= dp->dpStroff;
    int			tail= docParaLastStroff( paraNode );

    if  ( stroff >= tail )
	{ stroff= -1;	}

    while( stroff >= 0 && stroff < tail )
	{
	DocumentPosition	dpNext;
	int			part;

	stroff= docParaNextStroff( paraNode, stroff );
	if  ( stroff < 0 )
	    { break;	}

	docSetDocumentPosition( &dpNext, (BufferItem *)paraNode, stroff );
	if  ( docFindParticuleOfPosition( &part, (int *)0,
						    &dpNext, PARAfindFIRST ) )
	    { LDEB(1); return -1;	}

	if  ( ! DOCisADMINISTRATIVE( paraNode->biParaParticules[part].tpKind ) )
	    { break;	}
	}

    while( paraNode )
	{
	if  ( stroff >= 0 && stroff <= tail )
	    {
	    docSetDocumentPosition( dp, paraNode, stroff );
	    return 0;
	    }

	paraNode= docNextParagraph( paraNode );
	if  ( ! paraNode )
	    { break;	}

	stroff= docParaFirstStroff( paraNode );
	tail= docParaLastStroff( paraNode );

	if  ( docParaStrlen( paraNode ) == 0 )
	    {
	    docSetDocumentPosition( dp, paraNode, stroff );
	    return 0;
	    }
	}

    return -1;
    }

int docNextPosition(	DocumentPosition *	dp )
    {
    BufferItem *	paraNode= dp->dpNode;
    int			stroff= dp->dpStroff;
    int			tail= docParaLastStroff( paraNode );

    if  ( stroff >= tail )
	{ stroff= -1;	}

    if  ( stroff >= 0 && stroff < tail )
	{ stroff= docParaNextStroff( paraNode, stroff );	}

    while( paraNode )
	{
	if  ( stroff >= 0 && stroff <= tail )
	    {
	    docSetDocumentPosition( dp, paraNode, stroff );
	    return 0;
	    }

	paraNode= docNextParagraph( paraNode );
	if  ( ! paraNode )
	    { break;	}

	stroff= 0;
	tail= docParaStrlen( paraNode );

	if  ( docParaStrlen( paraNode ) == 0 )
	    {
	    docSetDocumentPosition( dp, paraNode, stroff );
	    return 0;
	    }
	}

    return -1;
    }

/************************************************************************/
/*									*/
/*  Return the previous valid string offset in the paragraph.		*/
/*									*/
/*  Positions inside an UTF-8 sequence and positions between		*/
/*  administrative particules are invalid.				*/
/*  This means that hopping over an administrative particule is not a	*/
/*  step that counts.							*/
/*									*/
/************************************************************************/

int docGotoPrevPosition(	DocumentPosition *	dp )
    {
    BufferItem *	paraNode= dp->dpNode;
    int			stroff= dp->dpStroff;

    int			head= docParaFirstStroff( paraNode );

    if  ( stroff <= head )
	{ stroff= -1;	}

    while( stroff > head )
	{
	DocumentPosition	dpPrev;
	int			part;

	stroff= docParaPrevStroff( paraNode, stroff );
	if  ( stroff < 0 )
	    { break;	}

	docSetDocumentPosition( &dpPrev, (BufferItem *)paraNode, stroff );
	if  ( docFindParticuleOfPosition( &part, (int *)0,
						    &dpPrev, PARAfindLAST ) )
	    { LDEB(1); return -1;	}

	if  ( ! DOCisADMINISTRATIVE( paraNode->biParaParticules[part].tpKind ) )
	    { break;	}
	}

    while( paraNode )
	{
	if  ( stroff >= head )
	    {
	    docSetDocumentPosition( dp, paraNode, stroff );
	    return 0;
	    }

	paraNode= docPrevParagraph( paraNode );
	if  ( paraNode )
	    {
	    stroff= docParaLastStroff( paraNode );
	    head= docParaFirstStroff( paraNode );
	    }
	}

    return -1;
    }

int docPrevPosition(	DocumentPosition *	dp )
    {
    BufferItem *	paraNode= dp->dpNode;
    int			stroff= dp->dpStroff;

    const int		head= 0;

    if  ( stroff <= head )
	{ stroff= -1;	}

    if  ( stroff >= head )
	{
	stroff= docParaPrevStroff( paraNode, stroff );
	}

    while( paraNode )
	{
	if  ( stroff >= head )
	    {
	    docSetDocumentPosition( dp, paraNode, stroff );
	    return 0;
	    }

	paraNode= docPrevParagraph( paraNode );
	if  ( paraNode )
	    { stroff= docParaStrlen( paraNode ); }
	}

    return -1;
    }

/************************************************************************/
/*									*/
/*  Return the first selectable string offset in the paragraph.		*/
/*									*/
/*  Return 0, or the position after a series of administrative		*/
/*  particules.								*/
/*									*/
/************************************************************************/

int docParaFirstStroff(	const BufferItem *	paraNode )
    {
    int		stroff= 0;
    int		part= 0;

    while( part < paraNode->biParaParticuleCount				&&
	   DOCisADMINISTRATIVE(paraNode->biParaParticules[part].tpKind)	)
	{
	stroff= paraNode->biParaParticules[part].tpStroff+
				    paraNode->biParaParticules[part].tpStrlen;
	part++;
	}

    return stroff;
    }

/************************************************************************/
/*									*/
/*  Return the last selectable string offset in the paragraph.		*/
/*									*/
/*  Return the last position in the paragraph or the position before	*/
/*  a series of administrative particules.				*/
/*									*/
/************************************************************************/

int docParaLastStroff(	const BufferItem *	paraNode )
    {
    int		stroff= docParaStrlen( paraNode );
    int		part= paraNode->biParaParticuleCount- 1;

    if  ( part < 0 )
	{ LDEB(part); docListNode(0,paraNode,0); return -1;	}

    while( part >= 0							&&
	   DOCisADMINISTRATIVE(paraNode->biParaParticules[part].tpKind)	)
	{
	stroff= paraNode->biParaParticules[part].tpStroff;
	part--;
	}

    return stroff;
    }

