/************************************************************************/
/*									*/
/*  Editor functionality.						*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	"tedEdit.h"
#   include	"tedDocFront.h"
#   include	<docRtfTrace.h>
#   include	<docEditImpl.h>
#   include	<docTreeNode.h>
#   include	<docNodeTree.h>
#   include	<docEditCommand.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Delete the current paragraph from a document.			*/
/*									*/
/************************************************************************/

static int tedEditDeleteSelectedParagaphs(	DocumentPosition *	dpNew,
						int *			pSide,
						TedEditOperation *	teo )
    {
    int				rval= 0;
    EditOperation *		eo= &(teo->teoEo);

    BufferItem *		parentNode;
    int				child0;
    int				child1;

    const int			direction= 0;
    int				side;

    DocumentSelection		dsParas;

    docInitDocumentSelection( &dsParas );
    docEditOperationGetSelection( &dsParas, eo );
    if  ( docSelectWholeParagraph( &dsParas, direction ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( docEditDeleteFieldsForBlockDelete( eo, &dsParas ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( docEditFindPositionOutsideBlockDelete( &side, dpNew, &dsParas ) )
	{ LDEB(1); rval= -1; goto ready;	}

    parentNode= dsParas.dsHead.dpNode->biParent;
    child0= dsParas.dsHead.dpNode->biNumberInParent;
    child1= dsParas.dsTail.dpNode->biNumberInParent;

    if  ( child0 <= child1 )
	{
	int		firstParaDeleted= -1;
	int		sectionsDeleted= 0;
	int		paragraphsDeleted= 0;

	tedEditIncludeNodeInRedraw( teo,  dsParas.dsHead.dpNode );

	docEditDeleteNodes( eo, &sectionsDeleted,
				    &firstParaDeleted, &paragraphsDeleted,
				    parentNode, child0, child1- child0+ 1 );

	docEditIncludeNodeInReformatRange( eo, parentNode );
	eo->eoHeadDp= *dpNew;
	eo->eoTailDp= *dpNew;
	}

    *pSide= side;

  ready:

    return rval;
    }

int tedDocDeleteSelectedParagraphs(	EditDocument *	ed,
					int		traced )
    {
    int				rval= 0;
    TedEditOperation		teo;
    EditOperation *		eo= &(teo.teoEo);
    SelectionGeometry		sg;
    SelectionDescription	sd;

    DocumentPosition		dpNew;
    int				allChildren= 0;
    int				side;
    DocumentSelection		dsTraced;

    const int			fullWidth= 1;

    tedStartEditOperation( &teo, &sg, &sd, ed, fullWidth, traced );

    if  ( ! sd.sdInContiguousParagraphs )
	{ LDEB(sd.sdInContiguousParagraphs); rval= -1; goto ready;	}

    if  ( eo->eoHeadDp.dpNode->biNumberInParent == 0 )
	{
	BufferItem *	parentNode= eo->eoHeadDp.dpNode->biParent;

	if  ( eo->eoTailDp.dpNode->biNumberInParent ==
						parentNode->biChildCount -1 )
	    { allChildren= 1;	}
	}

    if  ( allChildren )
	{
	if  ( tedEditStartReplace( &dsTraced, &teo,
					EDITcmdDELETE_PARA, DOClevPARA, 0 ) )
	    { LDEB(1); goto ready;	}

	/*  Fraudulous HACK */
	if  ( docHeadPosition( &(eo->eoHeadDp), eo->eoHeadDp.dpNode ) )
	    { LDEB(1); goto ready;	}
	if  ( docTailPosition( &(eo->eoTailDp), eo->eoTailDp.dpNode ) )
	    { LDEB(1); goto ready;	}
	eo->eoLastDp= eo->eoTailDp;

	if  ( tedDeleteSelectionImpl( &teo ) )
	    { LDEB(allChildren); goto ready;	}
	}
    else{
	if  ( tedEditStartReplace( &dsTraced, &teo,
					EDITcmdDELETE_PARA, DOClevPARA, 0 ) )
	    { LDEB(1); rval= -1; goto ready;	}

	if  ( tedEditDeleteSelectedParagaphs( &dpNew, &side, &teo ) )
	    { LDEB(1); rval= -1; goto ready;	}

	docSetIBarRange( &(eo->eoAffectedRange), &dpNew );
	docSetIBarRange( &(eo->eoSelectedRange), &dpNew );
	tedEditFinishSelectionTail( &teo );

	if  ( teo.teoEditTrace )
	    {
	    const SelectionScope * const s0= (const SelectionScope *)0;

	    if  ( side > 0 )
		{ docRtfTraceNewPosition( eo, s0, SELposAFTER );	}
	    else{ docRtfTraceNewPosition( eo, s0, SELposBEFORE );	}
	    }

	tedFinishEditOperation( &teo );
	}

  ready:

    tedCleanEditOperation( &teo );

    return rval;
    }

int tedDocDeleteCurrentFrame(	EditDocument *	ed,
				int		traced )
    {
    if  ( tedDocSelectCurrentFrame( ed ) )
	{ LDEB(1); return -1;	}

    return tedDocDeleteSelectedParagraphs( ed, traced );
    }

/************************************************************************/
/*									*/
/*  Delete the current section.						*/
/*									*/
/************************************************************************/

static int tedEditDeleteSelectedSections(
					DocumentPosition *	dpNew,
					int *			pSide,
					BufferItem *		sectNode0,
					BufferItem *		sectNode1,
					TedEditOperation *	teo )
    {
    int				rval= 0;
    EditOperation *		eo= &(teo->teoEo);

    BufferItem *		parentNode;
    int				child0;
    int				child1;

    const int			direction= 0;
    int				side;

    DocumentSelection		dsSects;

    docInitDocumentSelection( &dsSects );
    docEditOperationGetSelection( &dsSects, eo );
    if  ( docSelectWholeSection( &dsSects, direction ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( docEditDeleteFieldsForBlockDelete( eo, &dsSects ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( docEditFindPositionOutsideBlockDelete( &side, dpNew, &dsSects ) )
	{ LDEB(1); rval= -1; goto ready;	}

    parentNode= sectNode0->biParent;
    child0= sectNode0->biNumberInParent;
    child1= sectNode1->biNumberInParent;

    if  ( child0 <= child1 )
	{
	int		firstParaDeleted= -1;
	int		sectionsDeleted= 0;
	int		paragraphsDeleted= 0;

	tedEditIncludeNodeInRedraw( teo, sectNode0 );

	docEditDeleteNodes( eo, &sectionsDeleted,
				    &firstParaDeleted, &paragraphsDeleted,
				    parentNode, child0, child1- child0+ 1 );

	docEditIncludeNodeInReformatRange( eo, parentNode );
	eo->eoHeadDp= *dpNew;
	eo->eoTailDp= *dpNew;

	docGetSelectionScope( &(eo->eoSelectionScope), dpNew->dpNode );
	}

    *pSide= side;

  ready:

    return rval;
    }

/************************************************************************/
/*									*/
/*  Delete the current section.						*/
/*									*/
/************************************************************************/

int tedDocDeleteSelectedSections(	EditDocument *	ed,
					int		traced )
    {
    int				rval= 0;

    TedEditOperation		teo;
    EditOperation *		eo= &(teo.teoEo);
    SelectionGeometry		sg;
    SelectionDescription	sd;

    DocumentPosition		dpNew;
    int				side;
    int				allChildren= 0;

    BufferItem *		sectNode0;
    BufferItem *		sectNode1;
    DocumentSelection		dsTraced;

    const int			fullWidth= 1;

    tedStartEditOperation( &teo, &sg, &sd, ed, fullWidth, traced );

    sectNode0= docGetSectNode( eo->eoHeadDp.dpNode );
    sectNode1= docGetSectNode( eo->eoTailDp.dpNode );
    if  ( ! sectNode0 || ! sectNode1 )
	{ XXDEB(sectNode0,sectNode1); rval= -1; goto ready;	}
    if  ( ! sectNode0->biParent				||
	  sectNode0->biParent != sectNode1->biParent	)
	{
	XXDEB(sectNode0->biParent,sectNode1->biParent);
	rval= -1; goto ready;
	}

    if  ( sectNode0->biNumberInParent == 0 )
	{
	BufferItem *	parentNode= sectNode0->biParent;

	if  ( sectNode1->biNumberInParent == parentNode->biChildCount -1 )
	    { allChildren= 1;	}
	}

    if  ( allChildren )
	{
	if  ( tedEditStartReplace( &dsTraced, &teo,
					EDITcmdDELETE_SECT, DOClevSECT, 0 ) )
	    { LDEB(1); goto ready;	}

	/*  Fraudulous HACK */
	if  ( docHeadPosition( &(eo->eoHeadDp), sectNode0 ) )
	    { LDEB(1); goto ready;	}
	if  ( docTailPosition( &(eo->eoTailDp), sectNode1 ) )
	    { LDEB(1); goto ready;	}
	eo->eoLastDp= eo->eoTailDp;

	if  ( tedDeleteSelectionImpl( &teo ) )
	    { LDEB(allChildren); goto ready;	}
	}
    else{
	if  ( tedEditStartReplace( &dsTraced, &teo,
					EDITcmdDELETE_SECT, DOClevSECT, 0 ) )
	    { LDEB(1); goto ready;	}

	if  ( tedEditDeleteSelectedSections( &dpNew, &side,
						sectNode0, sectNode1, &teo ) )
	    { LDEB(1); rval= -1; goto ready;	}

	docSetIBarRange( &(eo->eoAffectedRange), &dpNew );
	docSetIBarRange( &(eo->eoSelectedRange), &dpNew );
	tedEditFinishSelectionTail( &teo );

	if  ( teo.teoEditTrace )
	    {
	    const SelectionScope * const s0= (const SelectionScope *)0;

	    if  ( side > 0 )
		{ docRtfTraceNewPosition( eo, s0, SELposAFTER );	}
	    else{ docRtfTraceNewPosition( eo, s0, SELposBEFORE );	}
	    }

	tedFinishEditOperation( &teo );
	}

  ready:

    tedCleanEditOperation( &teo );

    return rval;
    }

