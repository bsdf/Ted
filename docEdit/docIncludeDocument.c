/************************************************************************/
/*									*/
/*  Replace the selection in a document with another document.		*/
/*  ( Used with 'paste', 'insert file', 'undo', 'redo'. )		*/
/*									*/
/************************************************************************/

#   include	"docEditConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	"docEdit.h"
#   include	"docEditImpl.h"
#   include	"docCopyNode.h"
#   include	<docParaParticules.h>
#   include	<docField.h>
#   include	<docNodeTree.h>

#   include	<appDebugon.h>

#   define	SHOW_SELECTION_RANGE	0

/************************************************************************/

static int docIncludeTransferParaProperties(
					BufferItem *		paraNodeTo,
					const BufferItem *	paraNodeFrom,
					DocumentCopyJob *	dcjInsert )
    {
    PropertyMask	ppSetMask;

    utilPropMaskClear( &ppSetMask );
    utilPropMaskFill( &ppSetMask, PPprop_COUNT );
    PROPmaskUNSET( &ppSetMask, PPpropTABLE_NESTING );

    if  ( docEditUpdParaProperties( dcjInsert->dcjEditOperation,
			    (PropertyMask *)0, paraNodeTo,
			    &ppSetMask, &(paraNodeFrom->biParaProperties),
			    &(dcjInsert->dcjAttributeMap) ) )
	{ LDEB(1); return -1; }

    return 0;
    }

/************************************************************************/
/*									*/
/*  Replace the selection in a document with another document.		*/
/*  ( Used with 'paste', 'insert file', 'TOC' )				*/
/*									*/
/*  B)  The first particule of the line was split, probably, part fits	*/
/*	on the previous line. Reformat from the previous particule.	*/
/*	If paragraphs were merged, redraw the whole neighbourhood.	*/
/*									*/
/*  1)  Replace the selection of the target with the text of those	*/
/*	particules at the beginning of the source that have the same	*/
/*	attributes.							*/
/*  2)  Insert the rest of the first paragraph of the source into the	*/
/*	target.								*/
/*  4)  If the insertion consists of more than one paragraph, split the	*/
/*	target paragraph.						*/
/*  5)  Insert the last particule of the insertion as text.		*/
/*									*/
/*  z)  Copy all paragraphs between the first and last (exclusive) of	*/
/*	the source to the target.					*/
/*									*/
/************************************************************************/

/************************************************************************/
/*									*/
/*  Delete the selected range.						*/
/*									*/
/************************************************************************/

static int docReplaceMultipleParagraphsWithSingle(
				DocumentCopyJob *		dcjInsert,
				const DocumentPosition *	dpFrom,
				int				partFrom,
				const int			paraNrBegin )
    {
    EditOperation *		eo= dcjInsert->dcjEditOperation;

    int				rval= 0;
    DocumentCopyJob		dcjTail;

    EditPosition		epSave;

    DocumentPosition		tailDp= eo->eoTailDp;

    docInitDocumentCopyJob( &dcjTail );

    if  ( eo->eoHeadDp.dpNode->biParaTableNesting > 0	&&
	  eo->eoTailDp.dpNode->biParaTableNesting == 0	)
	{ docFlattenRow( eo->eoHeadDp.dpNode );	}

    /*  2  */
    if  ( docParaDeleteText( eo, paraNrBegin, &tailDp,
		    &(eo->eoHeadDp), docParaStrlen( eo->eoHeadDp.dpNode ) ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( docParaInsertTail( dcjInsert, paraNrBegin,
					&tailDp, &tailDp, dpFrom ) )
	{ LDEB(partFrom); rval= -1; goto ready;	}

    epSave= eo->eoAffectedRange.erTail;
    epSave.epParaNr= paraNrBegin;
    epSave.epStroff= docParaStrlen( eo->eoHeadDp.dpNode );

    /*  3  */
    if  ( eo->eoTailDp.dpStroff < docParaStrlen( eo->eoTailDp.dpNode ) )
	{
	DocumentPosition	afterDp;
	const int		copyFields= 1;

	if  ( docSet1DocumentCopyJob( &dcjTail, eo, copyFields ) )
	    { LDEB(1); rval= -1; goto ready;	}

	if  ( docParaInsertTail( &dcjTail, paraNrBegin,
					&afterDp, &tailDp, &(eo->eoTailDp) ) )
	    { LDEB(eo->eoTailDp.dpStroff); rval= -1; goto ready;	}
	}

    eo->eoTailDp= tailDp;

    eo->eoAffectedRange.erTail= epSave;
    eo->eoSelectedRange.erTail= epSave;

    /*  4  */
    if  ( docRemoveSelectionTail( eo ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    docCleanDocumentCopyJob( &dcjTail );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Include a single paragraph in the current document. It will		*/
/*  replace the  current selection.					*/
/*									*/
/************************************************************************/

static int docInsertSingleParagraph(
				DocumentCopyJob *		dcjInsert,
				const DocumentSelection *	dsFrom,
				int				paraNrBegin )
    {
    EditOperation *		eo= dcjInsert->dcjEditOperation;

    int				rval= 0;

    const int			partFrom= 0;

    /*  1  */
    if  ( eo->eoHeadDp.dpNode != eo->eoTailDp.dpNode )
	{
	if  ( docReplaceMultipleParagraphsWithSingle( dcjInsert,
				    &(dsFrom->dsHead), partFrom, paraNrBegin ) )
	    { LDEB(partFrom); rval= -1; goto ready;	}
	}
    else{
	/*  5  */
	if  ( docParaDeleteText( eo, paraNrBegin, &(eo->eoTailDp),
				    &(eo->eoHeadDp), eo->eoTailDp.dpStroff ) )
	    { LDEB(1); rval= -1; goto ready;	}

	if  ( docParaInsertTail( dcjInsert, paraNrBegin,
			&(eo->eoTailDp), &(eo->eoTailDp), &(dsFrom->dsHead) ) )
	    { LDEB(partFrom); rval= -1; goto ready;	}
	}

    if  ( ( dcjInsert->dcjCopyHeadParaProperties	||
	    dcjInsert->dcjCopyHeadParaProperties	)		&&
	  docIncludeTransferParaProperties( eo->eoTailDp.dpNode,
				  dsFrom->dsHead.dpNode, dcjInsert )	)
	{ LDEB(1); rval= -1; goto ready; }

    docEditFinishStep( eo );

  ready:

    return rval;
    }

/************************************************************************/
/*									*/
/*  Copy a node as part of an include action.				*/
/*									*/
/************************************************************************/

typedef struct TreeCopyJob
    {
    BufferDocument *	tcjDocument;
    const BufferItem *	tcjFromNode;

    BufferItem *	tcjToNode;
    int			tcjToIndex;
    BufferItem *	tcjTailNode;
    } TreeCopyJob;

static int docInsertNextStep(		TreeCopyJob *		tcj )
    {
    const BufferItem *	fromNode= tcj->tcjFromNode;
    
    if  ( ! docNextParagraph( (BufferItem *)fromNode ) )
	{ return 1;	}

    while( fromNode )
	{
	int	next= fromNode->biNumberInParent+ 1;

	if  ( fromNode->biParent			&&
	      next < fromNode->biParent->biChildCount	)
	    {
	    fromNode= fromNode->biParent->biChildren[next];
	    break;
	    }
	else{
	    fromNode= fromNode->biParent;
	    if  ( ! fromNode || ! fromNode->biParent )
		{ XDEB(fromNode); return -1;		}

	    if  ( ! tcj->tcjToNode )
		{ XDEB(tcj->tcjToNode); return -1;	}
	    if  ( tcj->tcjToIndex < tcj->tcjToNode->biChildCount )
		{
		BufferItem *	insertedNode= (BufferItem *)0;

		if  ( docSplitGroupNode( tcj->tcjDocument, &insertedNode,
					tcj->tcjToNode, tcj->tcjToIndex ) )
		    { LDEB(1); return -1;	}

		tcj->tcjToIndex= insertedNode->biNumberInParent+ 1;
		tcj->tcjToNode= insertedNode->biParent;
		tcj->tcjTailNode= insertedNode;
		}
	    else{
		tcj->tcjToIndex= tcj->tcjToNode->biNumberInParent+ 1;
		tcj->tcjToNode= tcj->tcjToNode->biParent;
		tcj->tcjTailNode= tcj->tcjToNode;
		}
	    }
	}

    if  ( ! fromNode )
	{ XDEB(fromNode); return -1;	}

    tcj->tcjFromNode= fromNode;
    return 0;
    }

/************************************************************************/
/*									*/
/*  Include a multi paragraph document in the current document. It will	*/
/*  replace the  current selection.					*/
/*									*/
/*  1)  Delete the selection from the target document.			*/
/*  2)  Split the current paragraph upto the desired level.		*/
/*  3)  Append the first paragraph of the source to the current		*/
/*	paragraph.							*/
/*  4)  Insert the source document tree.				*/
/*	Append the items to the parents at the same level in the	*/
/*	target.								*/
/*  5)  Attach the bottom side of the inserted wedge to the document.	*/
/*	This is done by merging parents of the same level. [Table rows	*/
/*	are not merged.]						*/
/*									*/
/************************************************************************/

static int docInsertMultipleParagraphs(
				DocumentCopyJob *		dcjInsert,
				const DocumentSelection *	dsFrom,
				int				paraNrBegin )
    {
    int				rval= 0;

    EditOperation *		eo= dcjInsert->dcjEditOperation;
    const BufferDocument *	bdFrom= dcjInsert->dcjSourceDocument;

    DocumentCopyJob		dcjTail;

    const int			partFrom= 0;

    DocumentPosition		tailDp;
    int				sectShift;
    int				lastInserted;

    int				headSplitLevel= DOClevPARA;
    int				tailSplitLevel= DOClevPARA;

    BufferItem *		nextRowBi= (BufferItem *)0;

    TreeCopyJob			tcj;

    int				part;
    int				partFlags;

    docInitDocumentCopyJob( &dcjTail );

    /*  1  */
    if  ( docDeleteSelection( eo ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( ! docSelectionInsideCell( dsFrom )			&&
	  eo->eoTailDp.dpNode->biParaTableNesting == 0		&&
	  dsFrom->dsHead.dpNode->biParaTableNesting > 0		)
	{ headSplitLevel= DOClevROW;	}

    if  ( ! docSelectionInsideCell( dsFrom )			&&
	  eo->eoTailDp.dpNode->biParaTableNesting == 0		&&
	  dsFrom->dsTail.dpNode->biParaTableNesting > 0		)
	{ tailSplitLevel= DOClevROW;	}

    if  ( docFindParticuleOfPosition( &part, &partFlags,
					    &(eo->eoTailDp), PARAfindPAST ) )
	{ LDEB(1); rval= -1; goto ready;	}

    /*  2  */
    if  ( headSplitLevel == DOClevPARA				||
	  ! ( partFlags & (POSflagPARA_HEAD|POSflagPARA_TAIL) )	)
	{
	BufferItem *		newParaBi;

	if  ( docSplitParaNode( &newParaBi, eo, headSplitLevel ) )
	    { LDEB(headSplitLevel); return -1;	}

	tcj.tcjDocument= eo->eoDocument;

	if  ( headSplitLevel == DOClevPARA )
	    {
	    tcj.tcjToNode= newParaBi->biParent;
	    tcj.tcjToIndex= newParaBi->biNumberInParent;
	    tcj.tcjFromNode= dsFrom->dsHead.dpNode;
	    tcj.tcjTailNode= eo->eoTailDp.dpNode;
	    }
	else{
	    BufferItem *	rowBi;

	    rowBi= docGetRowLevelNode( eo->eoTailDp.dpNode );
	    tcj.tcjToNode= rowBi->biParent;
	    tcj.tcjToIndex= rowBi->biNumberInParent+ 1;
	    tcj.tcjFromNode= docGetRowNode( dsFrom->dsHead.dpNode );
	    tcj.tcjTailNode= rowBi;
	    }
	}
    else{
	const int		after= ( partFlags & POSflagPARA_HEAD ) == 0;
	BufferItem *		rowBi;
	int			atExtremity;

	rowBi= docGetRowLevelNode( eo->eoTailDp.dpNode );

	if  ( docNodeAtExtremity( &atExtremity,
				    rowBi, eo->eoTailDp.dpNode, after ) )
	    { LDEB(1); return -1;	}

	if  ( atExtremity )
	    {
	    tcj.tcjToNode= rowBi->biParent;
	    tcj.tcjToIndex= rowBi->biNumberInParent+ after;

	    tcj.tcjDocument= eo->eoDocument;
	    tcj.tcjFromNode= docGetRowNode( dsFrom->dsHead.dpNode );
	    tcj.tcjTailNode= rowBi; /* Wrong for ! after, but irrelevant */

	    docEditIncludeNodeInReformatRange( eo, rowBi->biParent );
	    }
	else{
	    BufferItem *		beforeNode;
	    BufferItem *		afterNode;

	    if  ( docEditSplitParaParent( eo, &beforeNode, &afterNode,
				eo->eoTailDp.dpNode, after, headSplitLevel ) )
		{ LDEB(headSplitLevel); return -1;	}

	    rowBi= docGetRowLevelNode( eo->eoTailDp.dpNode );
	    tcj.tcjToNode= rowBi->biParent;
	    tcj.tcjToIndex= rowBi->biNumberInParent+ after;

	    tcj.tcjDocument= eo->eoDocument;
	    tcj.tcjFromNode= docGetRowNode( dsFrom->dsHead.dpNode );
	    tcj.tcjTailNode= afterNode;

	    if  ( after )
		{
		eo->eoAffectedRange.erTail.epParaNr=
				docNumberOfParagraph( eo->eoTailDp.dpNode )+ 1;
		eo->eoAffectedRange.erTail.epStroff= 0;
		}
	    }
	}

    if  ( headSplitLevel == DOClevPARA )
	{
	int	step;

	/*  3  */
	docTailPosition( &tailDp, eo->eoTailDp.dpNode );

	if  ( docParaInsertTail( dcjInsert, paraNrBegin,
				    &tailDp, &tailDp, &(dsFrom->dsHead) ) )
	    { LDEB(partFrom); rval= -1; goto ready;	}

	if  ( dcjInsert->dcjCopyHeadParaProperties			&&
	      docIncludeTransferParaProperties( tailDp.dpNode,
				  dsFrom->dsHead.dpNode, dcjInsert )	)
	    { LDEB(1); rval= -1; goto ready; }

	step= docInsertNextStep( &tcj );
	if  ( step < 0 )
	    { LDEB(step); rval= -1; goto ready;	}
	if  ( step > 0 )
	    { tcj.tcjFromNode= (const BufferItem *)0;	}
	}

    /*  4  */
    while( tcj.tcjFromNode )
	{
	BufferItem *	insertedNode;
	int		step;

	if  ( ! tcj.tcjToNode )
	    { XDEB(tcj.tcjToNode); rval= -1; goto ready;	}

	insertedNode= docCopyNode( dcjInsert,
			    tcj.tcjToNode, tcj.tcjToIndex, tcj.tcjFromNode );
	if  ( ! insertedNode )
	    { XDEB(insertedNode); rval= -1; goto ready;	}

	tcj.tcjToNode= insertedNode->biParent;
	tcj.tcjToIndex= insertedNode->biNumberInParent+ 1;
	tcj.tcjTailNode= insertedNode;

	step= docInsertNextStep( &tcj );
	if  ( step < 0 )
	    { LDEB(step); rval= -1; goto ready;	}
	if  ( step > 0 )
	    { break;	}
	}

    if  ( docTailPosition( &tailDp, tcj.tcjTailNode ) )
	{ XDEB(tcj.tcjToNode); rval= -1; goto ready;	}

    if  ( tailSplitLevel == DOClevROW )
	{
	DocumentPosition	dpNext;

	dpNext= tailDp;
	if  ( ! docNextPosition( &dpNext ) )
	    { nextRowBi= docGetRowLevelNode( dpNext.dpNode );	}
	}

    lastInserted= docNumberOfParagraph( tailDp.dpNode );
    sectShift= 0;

    /*  5  */
    while( tcj.tcjTailNode->biLevel != DOClevPARA )
	{
	BufferItem *	biParent= tcj.tcjTailNode->biParent;
	int		n= tcj.tcjTailNode->biNumberInParent;
	int		lastChild= tcj.tcjTailNode->biChildCount- 1;

	if  ( biParent == nextRowBi )
	    { break;	}

	if  ( n+ 1 < biParent->biChildCount )
	    {
	    int			sectionsDeleted;
	    BufferItem *	biMerged;

	    biMerged= biParent->biChildren[n+ 1];

	    if  ( biMerged == nextRowBi )
		{ break;	}

	    if  ( docMergeGroupNodes( tcj.tcjTailNode, biMerged ) )
		{ LDEB(1); rval= -1; goto ready;	}

	    sectionsDeleted= 0;
	    docDeleteEmptyParents( eo, &sectionsDeleted, biMerged );
	    sectShift -= sectionsDeleted;
	    }

	tcj.tcjTailNode= tcj.tcjTailNode->biChildren[lastChild];
	}

    if  ( sectShift != 0 )
	{
	const int	paraNr= lastInserted+ 1;
	const int	stroffFrom= 0;
	const int	stroffShift= 0;
	const int	paraShift= 0;

	eo->eoSectionsDeleted -= sectShift;

	docEditShiftReferences( eo, &(eo->eoSelectionScope),
				    paraNr, stroffFrom,
				    sectShift, paraShift, stroffShift );
	}

    if  ( bdFrom->bdProperties.dpHasOpenEnd				&&
	  tailSplitLevel == DOClevPARA					&&
	  tailDp.dpNode->biNumberInParent <
			  tailDp.dpNode->biParent->biChildCount -1	)
	{
	const int		partTailFrom= 0;
	DocumentPosition	dpTailFrom;

	DocumentPosition	dpTailTo;

	int			paraNrTail;
	const int		copyFields= 1;

	EditPosition		affSav= eo->eoAffectedRange.erTail;

	eo->eoTailDp= tailDp;

	docSetEditPosition( &(eo->eoAffectedRange.erTail), &(eo->eoTailDp) );
	affSav= eo->eoAffectedRange.erTail;

	paraNrTail= docNumberOfParagraph( tailDp.dpNode );

	dpTailFrom= eo->eoTailDp;
	if  ( docNextPosition( &dpTailFrom ) )
	    { LDEB(1); rval= -1; goto ready;	}

	if  ( docTailPosition( &(eo->eoLastDp), dpTailFrom.dpNode ) )
	    { LDEB(1); rval= -1; goto ready;	}

	if  ( docSet1DocumentCopyJob( &dcjTail, eo, copyFields ) )
	    { LDEB(1); rval= -1; goto ready;	}

	if  ( docParaInsertTail( &dcjTail, paraNrTail,
				&dpTailTo, &(eo->eoTailDp), &dpTailFrom ) )
	    { LDEB(partTailFrom); rval= -1; goto ready;	}

	if  ( docRemoveSelectionTail( eo ) )
	    { LDEB(1); rval= -1; goto ready;	}

	if  ( dcjInsert->dcjCopyTailParaProperties			&&
	      docIncludeTransferParaProperties( dpTailTo.dpNode,
				  dsFrom->dsTail.dpNode, dcjInsert )	)
	    { LDEB(1); rval= -1; goto ready; }

	eo->eoAffectedRange.erTail= affSav;
	eo->eoSelectedRange.erTail= affSav;
	}

  ready:

    docCleanDocumentCopyJob( &dcjTail );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Include a document in the current document. It will replace the	*/
/*  current selection.							*/
/*									*/
/*  1)  Find out what to insert.					*/
/*  2)  Insert text belonging to a single paragraph.			*/
/*  3)  Insert text belonging to a multiple paragraphs.			*/
/*									*/
/************************************************************************/

int docIncludeDocument(		DocumentCopyJob *		dcjInsert )
    {
    EditOperation *		eo= dcjInsert->dcjEditOperation;
    BufferDocument *		bdFrom= dcjInsert->dcjSourceDocument;

    DocumentSelection		dsFrom;
    DocumentSelection		dsTo;
    TableRectangle		trTo;

    int				paraNrBegin;

    paraNrBegin= docNumberOfParagraph( eo->eoHeadDp.dpNode );

    if  ( docEditDeleteReplacedFields( eo ) )
	{ LDEB(1);	}

#   if SHOW_SELECTION_RANGE
    appDebug( "INCLUDE.. %3d:%-3d .. %3d:%-3d :: %s\n",
		    eo->eoAffectedRange.erHead.epParaNr,
		    eo->eoAffectedRange.erHead.epStroff,
		    eo->eoAffectedRange.erTail.epParaNr,
		    eo->eoAffectedRange.erTail.epStroff,
		    bdFrom->bdProperties.dpHasOpenEnd?"OPEN":"CLOSED");
#   endif

    /*  1  */
    if  ( docHeadPosition( &(dsFrom.dsHead), bdFrom->bdBody.dtRoot ) )
	{ LDEB(1); return -1;	}
    if  ( docTailPosition(  &(dsFrom.dsTail), bdFrom->bdBody.dtRoot ) )
	{ LDEB(1); return -1;	}
    if  ( docSelectWholeBody( &dsFrom, bdFrom ) )
	{ LDEB(1); return -1;	}

    docEditOperationGetSelection( &dsTo, eo );
    docInitTableRectangle( &trTo );
    docGetTableRectangle( &trTo, &dsTo );

    /*  2,3  */
    if  ( bdFrom->bdProperties.dpHasOpenEnd		&&
	  dsFrom.dsHead.dpNode == dsFrom.dsTail.dpNode	)
	{
	if  ( docInsertSingleParagraph( dcjInsert, &dsFrom, paraNrBegin ) )
	    { LDEB(1); return -1;	}
	}
    else{
	if  ( docInsertMultipleParagraphs( dcjInsert, &dsFrom, paraNrBegin ) )
	    { LDEB(1); return -1;	}
	}

    if ( utilIndexSetUnion( &(eo->eoNoteFieldsAdded),
				    &(eo->eoNoteFieldsAdded),
				    &(dcjInsert->dcjNoteFieldsCopied) ) > 0 )
	{ eo->eoFieldUpdate |= FIELDdoCHFTN;	}
    if  ( dcjInsert->dcjBulletsCopied > 0 )
	{ eo->eoFieldUpdate |= FIELDdoLISTTEXT;	}

    return 0;
    }

