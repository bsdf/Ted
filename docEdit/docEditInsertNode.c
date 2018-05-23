/************************************************************************/
/*									*/
/*  Buffer: Insert nodes in the tree.					*/
/*									*/
/************************************************************************/

#   include	"docEditConfig.h"

#   include	<stdio.h>

#   include	<appDebugon.h>

#   include	<docBuf.h>
#   include	<docNodeTree.h>
#   include	<docParaParticules.h>
#   include	"docEdit.h"

/************************************************************************/
/*									*/
/*  Insert a fresh paragraph near an existing one.			*/
/*									*/
/************************************************************************/

int docInsertParagraph(		EditOperation *		eo,
				DocumentPosition *	dpNew,
				BufferItem *		paraNode,
				int			after,
				int			textAttributeNumber )
    {
    int			pos= paraNode->biNumberInParent+ ( after != 0 );
    BufferItem *	parentNode= paraNode->biParent;

    BufferItem *	newParaBi;
    int			paraNr;

    PropertyMask	ppChgMask;
    PropertyMask	ppUpdMask;

    newParaBi= docInsertNode( eo->eoDocument, parentNode, pos, DOClevPARA );
    if  ( ! newParaBi )
	{ XDEB(newParaBi); return -1;	}

    paraNr= docNumberOfParagraph( newParaBi );

    {
    const int	sectShift= 0;
    const int	stroffFrom= 0;
    const int	stroffShift= 0;

    const int	paraShift= 1;

    docEditShiftReferences( eo, &(eo->eoSelectionScope),
				    paraNr+ 1, stroffFrom,
				    sectShift, paraShift, stroffShift );
    }

    utilPropMaskClear( &ppChgMask );

    utilPropMaskClear( &ppUpdMask );
    utilPropMaskFill( &ppUpdMask, PPprop_FULL_COUNT );

    if  ( docEditUpdParaProperties( eo, &ppChgMask, newParaBi,
				    &ppUpdMask, &(paraNode->biParaProperties),
				    (const DocumentAttributeMap *)0 ) )
	{ LDEB(1);	}

    if  ( ! docInsertTextParticule( newParaBi, 0, 0, 0,
					DOCkindSPAN, textAttributeNumber ) )
	{ LDEB(1); return -1;	}

    docEditIncludeNodeInReformatRange( eo, paraNode );
    docEditIncludeNodeInReformatRange( eo, newParaBi );

    if  ( docHeadPosition( dpNew, newParaBi ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Insert a fresh section near an existing one.			*/
/*									*/
/************************************************************************/

int docInsertSection(		EditOperation *		eo,
				DocumentPosition *	dpBeforeSplit,
				DocumentPosition *	dpAfterSplit,
				BufferItem *		paraNode,
				int			split,
				int			after,
				int			textAttributeNumber )
    {
    BufferItem *	sectNode;

    sectNode= docGetSectNode( paraNode );
    if  ( ! sectNode )
	{ XDEB(sectNode); return -1;	}

    if  ( split )
	{
	int	atExtremity;

	if  ( docNodeAtExtremity( &atExtremity, sectNode, paraNode, after ) )
	    { LDEB(after); return -1;	}
	if  ( atExtremity )
	    { split= 0;	}
	}

    if  ( split )
	{
	BufferItem *		beforeNode;
	BufferItem *		afterNode;

	if  ( docEditSplitParaParent( eo, &beforeNode, &afterNode,
					    paraNode, after, DOClevSECT ) )
	    { LDEB(DOClevSECT); return -1;	}

	if  ( docTailPosition( dpBeforeSplit, beforeNode ) )
	    { LDEB(1); return -1;	}
	if  ( docHeadPosition( dpAfterSplit, afterNode ) )
	    { LDEB(1); return -1;	}
	}
    else{
	BufferItem *	parentNode;
	BufferItem *	newSectNode;
	BufferItem *	newParaBi;
	const int	sectShift= 1;

	int		pos= sectNode->biNumberInParent+ ( after != 0 );

	parentNode= sectNode->biParent;

	newSectNode= docInsertNode( eo->eoDocument,
						parentNode, pos, DOClevSECT );
	if  ( ! newSectNode )
	    { XDEB(newSectNode); return -1;	}
	eo->eoSectionsAdded++;

	if  ( docCopySectDescription( newSectNode, eo->eoDocument,
						    sectNode, eo->eoDocument ) )
	    { LDEB(1);	}

	if  ( docSectionParagraph( eo, &newParaBi, newSectNode, sectShift,
			&(paraNode->biParaProperties), textAttributeNumber ) )
	    { LDEB(1); return -1;	}

	docEditIncludeNodeInReformatRange( eo, sectNode );
	docEditIncludeNodeInReformatRange( eo, newSectNode );

	if  ( after )
	    {
	    if  ( docTailPosition( dpBeforeSplit, sectNode ) )
		{ LDEB(1); return -1;	}
	    if  ( docHeadPosition( dpAfterSplit, newSectNode ) )
		{ LDEB(1); return -1;	}
	    }
	else{
	    if  ( docTailPosition( dpBeforeSplit, newSectNode ) )
		{ LDEB(1); return -1;	}
	    if  ( docHeadPosition( dpAfterSplit, sectNode ) )
		{ LDEB(1); return -1;	}
	    }
	}

    return 0;
    }

