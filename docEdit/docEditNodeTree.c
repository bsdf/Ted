/************************************************************************/
/*									*/
/*  Buffer: Manipulation of node tree.					*/
/*									*/
/************************************************************************/

#   include	"docEditConfig.h"

#   include	<stdio.h>

#   include	<appDebugon.h>

#   include	<docBuf.h>
#   include	<docTreeNode.h>
#   include	<docNodeTree.h>
#   include	"docEdit.h"
#   include	"docCopyNode.h"
#   include	<docField.h>

#   define	VALIDATE_TREE	0

/************************************************************************/
/*									*/
/*  The current selection consisted of several paragraphs. An edit	*/
/*  operation was executed on the first one.. The rest is to be		*/
/*  deleted.								*/
/*									*/
/************************************************************************/

void docDeleteEmptyParents(	EditOperation *		eo,
				int *			pSectsDeleted,
				BufferItem *		node )
    {
    int			sectionsDeleted= 0;
    const int		recursively= 0;

    while( node && node->biChildCount == 0 )
	{
	int		numberInParent= node->biNumberInParent;
	BufferItem *	parent= node->biParent;

	if  ( ! parent )
	    { LXDEB(node->biNumberInParent,node->biParent); break;	}

	if  ( node->biLevel == DOClevSECT )
	    { sectionsDeleted++;	}

	docDeleteNode( eo->eoDocument, eo->eoTree, node );

	if  ( parent				&&
	      docIsRowNode( parent )		)
	    {
	    const int	shiftTail= 1;

	    if  ( docDeleteColumnsFromRow( &(parent->biRowProperties),
					    numberInParent, 1, shiftTail ) )
		{ LDEB(numberInParent);	}
	    }

	node= parent;
	if  ( ! node )
	    { break;	}

	if  ( docValidChildLevel( node->biLevel, DOClevROW )	&&
	      node->biChildCount > 0				)
	    { docDelimitTables( node, recursively );	}
	}

    *pSectsDeleted= sectionsDeleted;
    return;
    }

int docRemoveSelectionTail(	EditOperation *			eo )
    {
    BufferItem *	lastParaBi;
    int			paraShift= 0;
    int			sectShift= 0;

    int			firstDeleted= -1;
    int			parentCount= 0;
    int			mergeTail= 0;

    lastParaBi= eo->eoLastDp.dpNode;

    for (;;)
	{
	BufferItem *	parent= lastParaBi->biParent;
	BufferItem *	prevParaBi= docPrevParagraph( lastParaBi );

	int		sectionsDeleted= 0;
	int		firstParaDeleted= -1;
	int		paragraphsDeleted= 0;

	int		from;
	int		count;

	while( prevParaBi					&&
	       prevParaBi != eo->eoTailDp.dpNode		&&
	       prevParaBi->biParent == lastParaBi->biParent	)
	    { prevParaBi= docPrevParagraph( prevParaBi );	}

	if  ( ! prevParaBi )
	    { XDEB(prevParaBi); return -1;	}

	if  ( prevParaBi->biParent == parent )
	    { from= prevParaBi->biNumberInParent+ 1;	}
	else{
	    from= 0;

	    if  ( parentCount == 0					&&
		  lastParaBi->biNumberInParent < parent->biChildCount	)
		{ mergeTail= 1;	}
	    }

	count= lastParaBi->biNumberInParent- from+ 1;

	parentCount++;
	docEditDeleteNodes( eo, &sectionsDeleted,
					&firstParaDeleted, &paragraphsDeleted,
					parent, from, count );

	sectShift -= sectionsDeleted;
	paraShift -= paragraphsDeleted;
	if  ( firstParaDeleted >= 0 )
	    {
	    if  ( firstDeleted < 0 || firstDeleted > firstParaDeleted )
		{ firstDeleted= firstParaDeleted;	}
	    }

	if  ( parent->biChildCount > 0 )
	    { docEditIncludeNodeInReformatRange( eo, parent );	}

	sectionsDeleted= 0;
	docDeleteEmptyParents( eo, &sectionsDeleted, parent );

	sectShift -= sectionsDeleted;

	if  ( prevParaBi == eo->eoTailDp.dpNode )
	    { break;	}

	lastParaBi= prevParaBi;
	if  ( ! lastParaBi )
	    { XDEB(lastParaBi); break;	}
	}

    if  ( mergeTail )
	{
	lastParaBi= docNextParagraph( eo->eoTailDp.dpNode );
	if  ( lastParaBi )
	    {
	    int			sectionsDeleted;
	    BufferItem *	lastParent;

	    lastParent= lastParaBi->biParent;

	    if  ( docMergeGroupNodes( eo->eoTailDp.dpNode->biParent,
							    lastParent ) )
		{ LDEB(mergeTail); return -1;	}

	    sectionsDeleted= 0;
	    docDeleteEmptyParents( eo, &sectionsDeleted, lastParent );
	    sectShift -= sectionsDeleted;
	    }
	}

    if  ( sectShift != 0 )
	{
	const int	paraNr= firstDeleted+ 1;
	const int	stroffFrom= 0;
	const int	stroffShift= 0;
	const int	paraShiftX= 0;

	eo->eoSectionsDeleted -= sectShift;

	docEditShiftReferences( eo, &(eo->eoSelectionScope),
				    paraNr, stroffFrom,
				    sectShift, paraShiftX, stroffShift );
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Delete some buffer items but first close the objects in them.	*/
/*									*/
/*  1)  Preliminary administration for shifting the bottom of the range	*/
/*	of paragraphs that are to be reformatted.			*/
/*  2)  Include the whole of the parent in the range of items to be	*/
/*	reformatted.							*/
/*									*/
/************************************************************************/

void docEditDeleteNodes(	EditOperation *		eo,
				int *			pSectionsDeleted,
				int *			pFirstParaDeleted,
				int *			pParagraphsDeleted,
				BufferItem *		parentNode,
				int			first,
				int			count )
    {
    int		i;

    int		firstParaDeleted= -1;
    int		bulletsDeleted= 0;
    int		sectionsDeleted= 0;
    int		paragraphsDeleted= 0;
    int		lastStroff= 0;

    if  ( count == 0 )
	{ LDEB(count);	}

    if  ( count > 0 )
	{
	DocumentPosition	dp;

	/*  1  */
	if  ( docHeadPosition( &dp, parentNode->biChildren[first] ) )
	    { LDEB(1);							}
	else{ firstParaDeleted= docNumberOfParagraph( dp.dpNode );	}

	/*  1  */
	if  ( docTailPosition( &dp, parentNode->biChildren[first+count-1] ) )
	    { LDEB(1);							}
	else{ lastStroff= docParaStrlen( dp.dpNode );			}
	}

    for ( i= first+ count- 1; i >= first; i-- )
	{
	docCleanNodeObjects( &bulletsDeleted, &paragraphsDeleted,
						eo->eoTree, eo->eoDocument,
						parentNode->biChildren[i],
						eo->eoCloseObject );

	if  ( parentNode->biChildren[i]->biLevel == DOClevSECT )
	    { eo->eoSectionsDeleted++;	}
	}

    docDeleteNodes( eo->eoDocument, eo->eoTree, parentNode, first, count );

    if  ( parentNode->biLevel == DOClevBODY )
	{ sectionsDeleted= count;	}

    if  ( docIsRowNode( parentNode ) )
	{
	const int	shiftTail= 1;

	if  ( docDeleteColumnsFromRow( &(parentNode->biRowProperties),
						first, count, shiftTail ) )
	    { LLDEB(first,count);	}
	}

    if  ( count > 0 )
	{
	const int		sectShift= -sectionsDeleted;
	const int		stroffShift= 0;

	const int		recursively= 0;

	if  ( firstParaDeleted < 0 )
	    { LDEB(firstParaDeleted);	}
	else{
	    docEditAvoidDeletedParagraphs( eo, &(eo->eoSelectionScope),
		    firstParaDeleted, firstParaDeleted+ paragraphsDeleted );

	    docEditShiftReferences( eo, &(eo->eoSelectionScope),
			    firstParaDeleted+ paragraphsDeleted-1 , lastStroff,
			    sectShift, -paragraphsDeleted, stroffShift );
	    }

	if  ( docValidChildLevel( parentNode->biLevel, DOClevROW ) )
	    { docDelimitTables( parentNode, recursively );	}

	/*  Must be done by caller!
	docEditIncludeNodeInReformatRange( eo, parentNode );
	*/
	}

    if  ( first == 0					&&
	  ( parentNode->biLevel == DOClevCELL	||
	    parentNode->biLevel == DOClevROW	)	)
	{
	DocumentPosition	dp;

	if  ( ! docHeadPosition( &dp, parentNode )	&&
	      dp.dpNode->biNumberInParent == 0		)
	    { docEditAdaptRowPropertiesToFirstChild( eo, dp.dpNode ); }
	}

    if  ( bulletsDeleted > 0 )
	{ eo->eoFieldUpdate |= FIELDdoLISTTEXT;	}

    *pSectionsDeleted= sectionsDeleted;
    *pFirstParaDeleted= firstParaDeleted;
    *pParagraphsDeleted= paragraphsDeleted;
    return;
    }

/************************************************************************/
/*									*/
/*  Make sure that a Section contains an empty paragraph.		*/
/*									*/
/************************************************************************/

int docSectionParagraph( EditOperation *		eo,
			BufferItem **			pParaBi,
			BufferItem *			sectBi,
			int				sectShift,
			const ParagraphProperties *	pp,
			int				textAttributeNumber )
    {
    BufferDocument *	bd= eo->eoDocument;
    BufferItem *	paraBi;

    paraBi= docInsertEmptyParagraph( bd, sectBi, textAttributeNumber );
    if  ( ! paraBi )
	{ XDEB(paraBi); return -1;	}

    if  ( pp )
	{
	PropertyMask		ppChgMask;
	PropertyMask		ppUpdMask;

	utilPropMaskClear( &ppChgMask );

	utilPropMaskClear( &ppUpdMask );
	utilPropMaskFill( &ppUpdMask, PPprop_FULL_COUNT );
	PROPmaskUNSET( &ppUpdMask, PPpropTABLE_NESTING );

	if  ( docEditUpdParaProperties( eo, &ppChgMask, paraBi,
				    &ppUpdMask, pp,
				    (const DocumentAttributeMap *)0 ) )
	    { LDEB(1);	}
	}

    {
    int			paraNr= docNumberOfParagraph( paraBi );
    const int		paraShift= 1;
    const int		stroffFrom= 0;
    const int		stroffShift= 0;

    docEditShiftReferences( eo, &(sectBi->biSectSelectionScope),
				    paraNr, stroffFrom,
				    sectShift, paraShift, stroffShift );
    }

    *pParaBi= paraBi; return 0;
    }

/************************************************************************/
/*									*/
/*  Roll the children of a group item.					*/
/*									*/
/************************************************************************/

int docRollNodeChildren(	EditOperation *		eo,
				BufferItem *		parentNode,
				int			from,
				int			upto,
				int			by )
    {
    int				rval= 0;
    int				del0= upto;
    int				del1= upto;
    const int			copyFields= 1;
    int				count= upto- from;

    DocumentCopyJob		dcj;

    DocumentSelection		dsClean;

    docInitDocumentCopyJob( &dcj );

    if  ( by < 1- count || by > count- 1 )
	{ LLDEB(by,count); rval= -1; goto ready;	}
    if  ( count < 2 )
	{ LDEB(count); goto ready;			}

    by= ( count+ by ) % count;
    if  ( by > count/ 2 )
	{ by= by- count;	}

#   if VALIDATE_TREE
    if  ( docCheckNode( parentNode ) )
	{ LDEB(2); docListNode( 0, parentNode ); abort(); }
#   endif

    if  ( docSet1DocumentCopyJob( &dcj, eo, copyFields ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( by > 0 )
	{
	int	i;

	for ( i= 0; i < by; i++ )
	    {
	    if  ( ! docCopyNode( &dcj, parentNode, from,
					    parentNode->biChildren[upto- 1] ) )
		{ LDEB(from); rval= -1; goto ready;	}
	    }

	del0= upto;
	del1= upto+ by- 1;
	}

    if  ( by < 0 )
	{
	int	i;

	for ( i= 0; i < -by; i++ )
	    {
	    if  ( ! docCopyNode( &dcj, parentNode, upto+ i,
					    parentNode->biChildren[from+ i] ) )
		{ LDEB(upto+ i); rval= -1; goto ready;	}
	    }

	del0= from;
	del1= from- by- 1;
	}

    if  ( docHeadPosition( &(dsClean.dsHead), parentNode->biChildren[del0] ) )
	{ LDEB(del0); rval= -1; goto ready;	}
    if  ( docTailPosition( &(dsClean.dsTail), parentNode->biChildren[del1] ) )
	{ LDEB(del1); rval= -1; goto ready;	}

    if  ( docEditDeleteFieldsForBlockDelete( eo, &dsClean ) )
	{ LLDEB(del0,del1); rval= -1; goto ready;	}

    {
    int		sectionsDeleted= 0;
    int		firstParaDeleted= -1;
    int		paragraphsDeleted= 0;

    docEditDeleteNodes( eo, &sectionsDeleted,
				&firstParaDeleted, &paragraphsDeleted,
				parentNode, del0, del1- del0+ 1 );
    }

#   if VALIDATE_TREE
    if  ( docCheckNode( parentNode ) )
	{ LDEB(2); docListNode( 0, parentNode ); abort(); }
#   endif

  ready:

    docCleanDocumentCopyJob( &dcj );

    return rval;
    }

