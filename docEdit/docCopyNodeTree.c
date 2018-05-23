/************************************************************************/
/*									*/
/*  Buffer administration routines. Functionality related to the node	*/
/*  tree.								*/
/*									*/
/************************************************************************/

#   include	"docEditConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>

#   include	<appDebugon.h>

#   include	<docBuf.h>
#   include	<docTreeNode.h>
#   include	<docNodeTree.h>
#   include	"docCopyNode.h"
#   include	"docEdit.h"
#   include	"docDocumentCopyJob.h"

#   define	VALIDATE_TREE	0

/************************************************************************/

static BufferItem * docCopyXNode(	DocumentCopyJob *	dcj,
					const SelectionScope *	ssRoot,
					BufferItem *		parentNodeTo,
					int			n,
					const BufferItem *	biFrom );

/************************************************************************/

static BufferItem * docCopyCellNode(
				DocumentCopyJob *	dcj,
				const SelectionScope *	ssRoot,
				BufferItem *		rowNodeTo,
				int			colTo,
				const BufferItem *	cellNodeFrom,
				int			copyProperties )
    {
    EditOperation *	eo= dcj->dcjEditOperation;
    BufferDocument *	bdTo= eo->eoDocument;

    BufferItem *	cellNodeTo;
    int			child;

    cellNodeTo= docInsertNode( bdTo, rowNodeTo, colTo, DOClevCELL );
    if  ( ! cellNodeTo )
	{ XDEB(cellNodeTo); return (BufferItem *)0;	}

    if  ( copyProperties )
	{
	const int		shiftTail= 1;
	const BufferItem *	rowNodeFrom= cellNodeFrom->biParent;
	RowProperties *		rpTo= &(rowNodeTo->biRowProperties);
	const RowProperties *	rpFrom= &(rowNodeFrom->biRowProperties);
	int			colFrom= cellNodeFrom->biNumberInParent;
	CellProperties		cpScratch= rpFrom->rpCells[colFrom];
	int			left;

	if  ( rpTo->rpCellCount > 0 && colTo == rpTo->rpCellCount )
	    { left= docColumnRight( rpTo, colTo- 1 );	}
	else{ left= docColumnLeft( rpTo, colTo );	}

	cpScratch.cpRightBoundaryTwips= left+
					    docColumnWidth( rpFrom, colFrom );

	if  ( docInsertRowColumn( rpTo, colTo, shiftTail,
					&cpScratch, &(dcj->dcjAttributeMap) ) )
	    { LDEB(1); return (BufferItem *)0;	}
	}

    for ( child= 0; child < cellNodeFrom->biChildCount; child++ )
	{
	BufferItem *	childNode= cellNodeFrom->biChildren[child];

	if  ( ! docCopyXNode( dcj, ssRoot, cellNodeTo, child, childNode ) )
	    { LDEB(child); return (BufferItem *)0;	}
	}

    return cellNodeTo;
    }

/************************************************************************/
/*									*/
/*  Copy a row node. As an optimisation, we copy the cell properties	*/
/*  at once.								*/
/*									*/
/************************************************************************/

static BufferItem * docCopyRowNode(	DocumentCopyJob *	dcj,
					const SelectionScope *	ssRoot,
					BufferItem *		sectNodeTo,
					int			n,
					const BufferItem *	rowBiFrom )
    {
    EditOperation *	eo= dcj->dcjEditOperation;
    BufferDocument *	bdTo= eo->eoDocument;

    BufferItem *	rowNodeTo;
    int			col;

    rowNodeTo= docInsertNode( bdTo, sectNodeTo, n, DOClevROW );
    if  ( ! rowNodeTo )
	{ XDEB(rowNodeTo); return rowNodeTo;	}

    if  ( docCopyRowProperties( &(rowNodeTo->biRowProperties),
		    &(rowBiFrom->biRowProperties), &(dcj->dcjAttributeMap ) ) )
	{ LDEB(1); return (BufferItem *)0;	}

    for ( col= 0; col < rowBiFrom->biChildCount; col++ )
	{
	BufferItem *	cellNodeTo;
	const int	copyCellProperties= 0;

	cellNodeTo= docCopyCellNode( dcj, ssRoot, rowNodeTo, col,
			    rowBiFrom->biChildren[col], copyCellProperties );
	if  ( ! cellNodeTo )
	    { XDEB(cellNodeTo); return (BufferItem *)0;	}
	}

    return rowNodeTo;
    }

/************************************************************************/
/*									*/
/*  Copy a section.							*/
/*									*/
/************************************************************************/

int docCopySectChildren(	DocumentCopyJob *	dcj,
				BufferItem *		sectNodeTo,
				const BufferItem *	sectBiFrom )
    {
    int		row;

    for ( row= 0; row < sectBiFrom->biChildCount; row++ )
	{
	BufferItem *	rowBiFrom= sectBiFrom->biChildren[row];
	BufferItem *	rowNodeTo;

	rowNodeTo= docCopyXNode( dcj, &(sectNodeTo->biSectSelectionScope),
						sectNodeTo, row, rowBiFrom );
	if  ( ! rowNodeTo )
	    { XDEB(rowNodeTo); return -1;	}
	}

    return 0;
    }

static BufferItem * docCopySectNode(	DocumentCopyJob *	dcj,
					const SelectionScope *	ssRoot,
					BufferItem *		parentNodeTo,
					int			n,
					const BufferItem *	sectBiFrom )
    {
    EditOperation *	eo= dcj->dcjEditOperation;
    BufferDocument *	bdTo= eo->eoDocument;

    BufferItem *	rval= (BufferItem *)0;
    BufferItem *	sectNodeTo= (BufferItem *)0;

    if  ( parentNodeTo )
	{
	if  ( parentNodeTo->biTreeType != ssRoot->ssTreeType )
	    {
	    LDEB(parentNodeTo->biTreeType);
	    LDEB(ssRoot->ssTreeType);
	    goto ready;
	    }

	sectNodeTo= docInsertNode( bdTo, parentNodeTo, n, DOClevSECT );
	if  ( ! sectNodeTo )
	    { XDEB(sectNodeTo); goto ready;	}
	}
    else{
	n= 0;
	sectNodeTo= (BufferItem *)malloc( sizeof(BufferItem) );

	if  ( ! sectNodeTo )
	    { XDEB(sectNodeTo); goto ready;	}

	docInitNode( sectNodeTo, (BufferItem *)0, bdTo, n,
				DOClevSECT, ssRoot->ssTreeType );
	}

    sectNodeTo->biSectSelectionScope= *ssRoot;

    if  ( docCopySectionProperties( &(sectNodeTo->biSectProperties),
					&(sectBiFrom->biSectProperties) ) )
	{ LDEB(1); goto ready;	}

    if  ( docCopySectChildren( dcj, sectNodeTo, sectBiFrom ) )
	{ LDEB(1); goto ready;	}

    if  ( ! dcj->dcjInExternalTree && parentNodeTo )
	{
	DocumentPosition		dp;

	if  ( docTailPosition( &dp, sectNodeTo ) )
	    { LDEB(1);	}
	else{
	    int		paraNr;
	    const int	stroff= 0;
	    const int	sectShift= 1;
	    const int	paraShift= 0;

	    paraNr= docNumberOfParagraph( dp.dpNode )+ 1;

	    docEditShiftReferences( eo, ssRoot, paraNr, stroff,
					    sectShift, paraShift, -stroff );
	    }
	}

    rval= sectNodeTo; sectNodeTo= (BufferItem *)0; /* steal */

  ready:

    if  ( sectNodeTo )
	{ docDeleteNode( eo->eoDocument, eo->eoTree, sectNodeTo );	}

    return rval;
    }

static BufferItem * docCopyXNode(	DocumentCopyJob *	dcj,
					const SelectionScope *	ssRoot,
					BufferItem *		parentNodeTo,
					int			n,
					const BufferItem *	biFrom )
    {
    const int	copyCellProperties= 1;

    switch( biFrom->biLevel )
	{
	case DOClevPARA:
	    return docCopyParaNode( dcj, ssRoot, parentNodeTo, n, biFrom );
	case DOClevCELL:
	    return docCopyCellNode( dcj, ssRoot, parentNodeTo, n, biFrom,
							copyCellProperties );
	case DOClevROW:
	    return docCopyRowNode(  dcj, ssRoot, parentNodeTo, n, biFrom );
	case DOClevSECT:
	    return docCopySectNode( dcj, ssRoot, parentNodeTo, n, biFrom );
	default:
	    LDEB(biFrom->biLevel); return (BufferItem *)0;
	}
    }

BufferItem * docCopyNode(	DocumentCopyJob *	dcj,
				BufferItem *		parentNodeTo,
				int			n,
				const BufferItem *	biFrom )
    {
    EditOperation *		eo= dcj->dcjEditOperation;
    const SelectionScope *	ssRoot= &(eo->eoSelectionScope);

    return docCopyXNode( dcj, ssRoot, parentNodeTo, n, biFrom );
    }

int docCopyDocumentTree( 	DocumentCopyJob *	dcj,
				DocumentTree *		eiTo,
				const SelectionScope *	ssRoot,
				DocumentTree *		eiFrom )
    {
    int				rval= 0;
    FieldCopyStackLevel *	fcsl= dcj->dcjFieldStack;
    SelectionScope		ssSave= dcj->dcjTargetSelectionScope;
    DocumentTree *		targetTreeSave= dcj->dcjTargetTree;
    DocumentTree *		sourceTreeSave= dcj->dcjSourceTree;

    dcj->dcjTargetSelectionScope= *ssRoot;
    dcj->dcjTargetTree= eiTo;
    dcj->dcjSourceTree= eiFrom;
    dcj->dcjFieldStack= (FieldCopyStackLevel *)0;
    dcj->dcjInExternalTree++;

    eiTo->dtRoot= docCopySectNode( dcj, ssRoot, (BufferItem *)0, 0,
							    eiFrom->dtRoot );
    if  ( ! eiTo->dtRoot )
	{ XDEB(eiTo->dtRoot); rval= -1;	}

    if  ( dcj->dcjFieldStack )
	{ XDEB(dcj->dcjFieldStack);	}

    dcj->dcjInExternalTree--;
    dcj->dcjFieldStack= fcsl;
    dcj->dcjTargetTree= targetTreeSave;
    dcj->dcjSourceTree= sourceTreeSave;
    dcj->dcjTargetSelectionScope= ssSave;

    return rval;
    }

