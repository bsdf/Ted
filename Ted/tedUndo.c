/************************************************************************/
/*									*/
/*  Editor Undo functionality.						*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	"tedEdit.h"
#   include	"tedSelect.h"
#   include	"tedDocFront.h"
#   include	"tedLayout.h"
#   include	"tedDocument.h"
#   include	<docTrace.h>
#   include	<docRtfTrace.h>
#   include	<docTreeType.h>
#   include	<docTreeNode.h>
#   include	<docSelect.h>
#   include	<docNodeTree.h>
#   include	<docNotes.h>
#   include	<docDocumentNote.h>
#   include	<docListOverride.h>
#   include	<docEditCommand.h>
#   include	<docRtfTraceImpl.h>

#   include	<appDebugon.h>

#   define	VALIDATE_TREE	0

/************************************************************************/

static int tedStartUndoOperation( TedEditOperation *	teo,
				SelectionGeometry *	sg,
				SelectionDescription *	sd,
				EditDocument *		ed,
				const EditStep *	es )
    {
    EditOperation *		eo= &(teo->teoEo);
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;

    DocumentSelection		ds;
    DocumentSelection		dsGeo;
    const int			lastLine= 0;
    const int			fullWidth= 1;

#   if VALIDATE_TREE
    {
    const BufferDocument *	bd= td->tdDocument;
    LDEB(1);
    if  ( docCheckNode( &(bd->bdItem) ) )
	{ LDEB(2); docListNode( 0, &(bd->bdItem) ); abort(); }
    }
#   endif

    /**/
    teo->teoEditDocument= ed;
    teo->teoEditTrace= (EditTrace *)0;
    tedSetScreenLayoutContext( &(teo->teoLayoutContext), ed );
    eo->eoCloseObject= teo->teoLayoutContext.lcCloseObject;

    {
    BufferItem *	bodySectNode;

    if  ( docGetRootOfSelectionScope( &(eo->eoTree), &bodySectNode,
					    bd, &(es->esNewSelectionScope) ) )
	{ LDEB(1); return -1;	}

    eo->eoBodySectNode= bodySectNode;
    }

    if  ( docSelectionForEditPositionsInTree( &ds, eo->eoTree,
					&(es->esNewEditRange.erHead),
					&(es->esNewEditRange.erTail) ) )
	{ LDEB(1); return -1;	}

    dsGeo= ds;

    if  ( ! utilPropMaskIsEmpty( &(es->esNewStyle.dsCellMask) )		&&
	  ds.dsCol0 >= 0 && ds.dsCol1 >= 0				)
	{
	EditRange	er= es->esNewEditRange;

	docAvoidMergedTail( &dsGeo, &er );
	}

    tedSelectionGeometry( sg, &dsGeo, eo->eoBodySectNode, lastLine,
						&(teo->teoLayoutContext) );
    docDescribeSelection( sd, &ds, bd,
				    sg->sgHead.pgTopPosition.lpPage,
				    sg->sgTail.pgTopPosition.lpPage,
				    ed->edDocumentId,
				    ed->edFileReadOnly );

    if  ( docStartEditOperation( eo, &ds, bd ) )
	{ LDEB(1); return -1;	}

		/*
		 *  Save attributes to have the edit operation in a 
		 *  consistent state. As the undo operations always 
		 *  use an actual document as the source of document 
		 *  content, the values are never used.
		 */
    teo->teoSavedTextAttribute=
			td->tdSelectionDescription.sdTextAttribute;
    teo->teoSavedTextAttributeNumber=
			td->tdSelectionDescription.sdTextAttributeNumber;

    eo->eoIBarSelectionOld= sd->sdIsIBarSelection;
    eo->eoMultiParagraphSelectionOld= ! sd->sdIsSingleParagraph;

    tedIncludeRectangleInChange( teo, &(sg->sgRectangle) );
    teo->teoChangedRect.drX1= ed->edFullRect.drX1;

    if  ( fullWidth )
	{ teo->teoChangedRect.drX0= ed->edFullRect.drX0; }

    return 0;
    }

/************************************************************************/
/*									*/
/*  Finish an undo with the old selection.				*/
/*									*/
/*  Also set tyhe fiends that are normally set by the replacement	*/
/*  process.								*/
/*									*/
/************************************************************************/

static int tedUndoFinishOld(	TedEditOperation *	teo,
				const EditStep *	es )
    {
    EditOperation *		eo= &(teo->teoEo);

    DocumentTree *		dt;
    DocumentSelection		dsNew;

    if  ( docGetRootOfSelectionScope( &dt, (BufferItem **)0,
				eo->eoDocument, &(es->esOldSelectionScope) ) )
	{ LDEB(1); return -1;	}

    docInitDocumentSelection( &dsNew );

    if  ( docSelectionForEditPositionsInTree( &dsNew, dt,
					    &(es->esOldEditRange.erHead),
					    &(es->esOldEditRange.erTail) ) )
	{ LDEB(1); return -1;	}

    dsNew.dsCol0= es->esOldCol0;
    dsNew.dsCol1= es->esOldCol1;

    tedEditFinishSelection( teo, &dsNew );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Undo a replacement.							*/
/*									*/
/************************************************************************/

static int tedUndoReplace(	TedEditOperation *	teo,
				EditDocument *		ed,
				const EditStep *	es,
				int			posWhere,
				int			copyTailParaProps )
    {
    int				rval= 0;

    EditOperation *		eo= &(teo->teoEo);
    SelectionGeometry		sg;
    SelectionDescription	sd;

    DocumentPosition		dpFrom;

    DocumentCopyJob		dcj;

    docInitDocumentCopyJob( &dcj );

    tedStartUndoOperation( teo, &sg, &sd, ed, es );

    if  ( ! docHeadPosition( &dpFrom, es->esSourceDocument->bdBody.dtRoot ) )
	{
	if  ( docSetTraceDocumentCopyJob( &dcj, eo, es->esSourceDocument ) )
	    { LDEB(1); rval= -1; goto ready;	}

	dcj.dcjCopyTailParaProperties= copyTailParaProps;

	if  ( tedIncludeDocument( teo, &dcj ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}
    else{
	if  ( tedEditDeleteSelection( teo ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}

    switch( posWhere )
	{
	case SELposTAIL:
	    tedEditFinishPosition( teo, &(es->esOldEditRange.erTail) );
	    break;
	case SELposHEAD:
	    tedEditFinishPosition( teo, &(es->esOldEditRange.erHead) );
	    break;
	case SELposALL:
	    tedEditFinishRange( teo,
		    es->esOldCol0, es->esOldCol1, &(es->esOldEditRange) );
	    break;
	default:
	    LDEB(posWhere);
	    tedEditFinishRange( teo,
		    es->esOldCol0, es->esOldCol1, &(es->esOldEditRange) );
	    break;
	}

  ready:

    docCleanDocumentCopyJob( &dcj );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Undo the deletion of a range of paragraphs.				*/
/*									*/
/************************************************************************/

static int tedUndoDeletePara(	TedEditOperation *	teo,
				EditDocument *		ed,
				const EditStep *	es )
    {
    int				rval= 0;

    EditOperation *		eo= &(teo->teoEo);
    SelectionGeometry		sg;
    SelectionDescription	sd;

    DocumentPosition		dpFrom;

    BufferItem *		parentTo;
    int				to;
    BufferItem *		parentFrom;

    tedStartUndoOperation( teo, &sg, &sd, ed, es );

    if  ( docHeadPosition( &dpFrom, es->esSourceDocument->bdBody.dtRoot ) )
	{ LDEB(1); rval= -1; goto ready;	}

    parentFrom= dpFrom.dpNode->biParent;
    parentTo= eo->eoTailDp.dpNode->biParent;
    to= eo->eoTailDp.dpNode->biNumberInParent;

    if  ( docReinsertNodes( eo, parentTo, parentFrom, to, es ) )
	{ LDEB(to); rval= -1; goto ready;	}

    tedEditFinishRange( teo,
		    es->esOldCol0, es->esOldCol1, &(es->esOldEditRange) );

  ready:

    return rval;
    }

/************************************************************************/
/*									*/
/*  Undo the deletion of a range of columns.				*/
/*									*/
/************************************************************************/

static int tedUndoDeleteColumn(	TedEditOperation *	teo,
				EditDocument *		ed,
				const EditStep *	es )
    {
    int				rval= 0;

    EditOperation *		eo= &(teo->teoEo);
    SelectionGeometry		sg;
    SelectionDescription	sd;

    DocumentPosition		dpFrom;

    int				colFrom;
    int				rowFrom, row0From, row1From;
    BufferItem *		parentFrom;

    int				colTo;
    int				rowTo, row0To, row1To;
    BufferItem *		parentTo;

    tedStartUndoOperation( teo, &sg, &sd, ed, es );

    if  ( docHeadPosition( &dpFrom, es->esSourceDocument->bdBody.dtRoot ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( docDelimitTable( dpFrom.dpNode, &parentFrom,
				&colFrom, &row0From, &rowFrom, &row1From ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( docDelimitTable( eo->eoHeadDp.dpNode, &parentTo,
				&colTo, &row0To, &rowTo, &row1To ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( row0From != 0 || row1To- row0To != row1From- row0From )
	{ LLLLDEB(row0To,row1To,row0From,row1From); rval= -1; goto ready; }

    rowTo= row0To;
    for ( rowFrom= row0From; rowFrom <= row1From; rowTo++, rowFrom++ )
	{
	BufferItem *	rowNodeTo= parentTo->biChildren[rowTo];
	BufferItem *	rowNodeFrom= parentFrom->biChildren[rowFrom];

	if  ( docReinsertNodes( eo, rowNodeTo, rowNodeFrom, colTo, es ) )
	    { LDEB(colTo); rval= -1; goto ready;	}
	}

    tedEditFinishRange( teo,
		    es->esOldCol0, es->esOldCol1, &(es->esOldEditRange) );

  ready:

    return rval;
    }

/************************************************************************/
/*									*/
/*  Undo the deletion of a range of rows.				*/
/*									*/
/************************************************************************/

static int tedUndoDeleteRow(	TedEditOperation *	teo,
				EditDocument *		ed,
				const EditStep *	es )
    {
    int				rval= 0;

    EditOperation *		eo= &(teo->teoEo);
    SelectionGeometry		sg;
    SelectionDescription	sd;

    DocumentPosition		dpFrom;

    BufferItem *		parentTo;
    int				to;
    BufferItem *		parentFrom;

    BufferItem *		rowNodeFrom;
    BufferItem *		rowNodeTo;

    DocumentCopyJob		dcj;

    docInitDocumentCopyJob( &dcj );

    tedStartUndoOperation( teo, &sg, &sd, ed, es );

    if  ( docHeadPosition( &dpFrom, es->esSourceDocument->bdBody.dtRoot ) )
	{ LDEB(1); rval= -1; goto ready;	}

    rowNodeFrom= docGetRowNode( dpFrom.dpNode );
    if  ( ! rowNodeFrom )
	{ XDEB(rowNodeFrom); rval= -1; goto ready;	}
    parentFrom= rowNodeFrom->biParent;

    rowNodeTo= docGetRowNode( eo->eoTailDp.dpNode );
    if  ( rowNodeTo )
	{
	parentTo= rowNodeTo->biParent;
	to= rowNodeTo->biNumberInParent;

	if  ( docReinsertNodes( eo, parentTo, parentFrom, to, es ) )
	    { LDEB(to); rval= -1; goto ready;	}
	}
    else{
	if  ( docSetTraceDocumentCopyJob( &dcj, eo, es->esSourceDocument ) )
	    { LDEB(1); rval= -1; goto ready;	}

	/*  Do not copy paragraph properties */

	if  ( tedIncludeDocument( teo, &dcj ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}

    tedEditFinishRange( teo,
		    es->esOldCol0, es->esOldCol1, &(es->esOldEditRange) );

  ready:

    docCleanDocumentCopyJob( &dcj );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Undo the deletion of a range of sections.				*/
/*									*/
/************************************************************************/

static int tedUndoDeleteSect(	TedEditOperation *	teo,
				EditDocument *		ed,
				const EditStep *	es )
    {
    int				rval= 0;

    EditOperation *		eo= &(teo->teoEo);
    SelectionGeometry		sg;
    SelectionDescription	sd;

    DocumentPosition		dpFrom;

    BufferItem *		parentTo;
    int				to;
    BufferItem *		parentFrom;

    BufferItem *		sectNodeFrom;
    BufferItem *		sectNodeTo;

    tedStartUndoOperation( teo, &sg, &sd, ed, es );

    if  ( docHeadPosition( &dpFrom, es->esSourceDocument->bdBody.dtRoot ) )
	{ LDEB(1); rval= -1; goto ready;	}

    sectNodeFrom= docGetSectNode( dpFrom.dpNode );
    sectNodeTo= docGetSectNode( eo->eoTailDp.dpNode );
    if  ( ! sectNodeFrom || ! sectNodeTo )
	{ XXDEB(sectNodeFrom,sectNodeTo); rval= -1; goto ready;	}

    parentFrom= sectNodeFrom->biParent;
    parentTo= sectNodeTo->biParent;
    to= sectNodeTo->biNumberInParent;

    if  ( docReinsertNodes( eo, parentTo, parentFrom, to, es ) )
	{ LDEB(to); rval= -1; goto ready;	}

    tedEditFinishRange( teo,
		    es->esOldCol0, es->esOldCol1, &(es->esOldEditRange) );

  ready:


    return rval;
    }

static int tedUndoInsertNode(	TedEditOperation *	teo,
				const EditStep *	es,
				BufferItem *		node )
    {
    EditOperation *		eo= &(teo->teoEo);

    int				sectionsDeleted= 0;
    int				firstParaDeleted= -1;
    int				paragraphsDeleted= 0;

    eo->eoReformatRange= es->esNewEditRange;

    docEditIncludeNodeInReformatRange( eo, node->biParent );

    if  ( node->biLevel == DOClevSECT					&&
	  node->biTreeType == DOCinBODY				&&
	  node->biNumberInParent == node->biParent->biChildCount- 1	)
	{
	eo->eoSelectionScope.ssSectNr= node->biNumberInParent- 1;
	}

    docEditDeleteNodes( eo, &sectionsDeleted,
			    &firstParaDeleted, &paragraphsDeleted,
			    node->biParent, node->biNumberInParent, 1 );

    if  ( tedUndoFinishOld( teo, es ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

static int tedUndoInsertPara(	TedEditOperation *	teo,
				EditDocument *		ed,
				const EditStep *	es )
    {
    int				rval= 0;

    EditOperation *		eo= &(teo->teoEo);
    SelectionGeometry		sg;
    SelectionDescription	sd;

    tedStartUndoOperation( teo, &sg, &sd, ed, es );

    if  ( tedUndoInsertNode( teo, es, eo->eoHeadDp.dpNode ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    return rval;
    }

static int tedUndoInsertSect(	TedEditOperation *	teo,
				EditDocument *		ed,
				const EditStep *	es )
    {
    int				rval= 0;

    EditOperation *		eo= &(teo->teoEo);
    SelectionGeometry		sg;
    SelectionDescription	sd;

    BufferItem *		sectNode;

    tedStartUndoOperation( teo, &sg, &sd, ed, es );

    sectNode= docGetSectNode( eo->eoHeadDp.dpNode );
    if  ( ! sectNode )
	{ XDEB(sectNode); rval= -1; goto ready;	}

    if  ( tedUndoInsertNode( teo, es, sectNode ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    return rval;
    }

static int tedUndoInsertRow(	TedEditOperation *	teo,
				EditDocument *		ed,
				const EditStep *	es )
    {
    int				rval= 0;

    EditOperation *		eo= &(teo->teoEo);
    SelectionGeometry		sg;
    SelectionDescription	sd;

    BufferItem *		rowNode;

    tedStartUndoOperation( teo, &sg, &sd, ed, es );

    rowNode= docGetRowNode( eo->eoHeadDp.dpNode );
    if  ( ! rowNode )
	{ XDEB(rowNode); rval= -1; goto ready;	}

    if  ( tedUndoInsertNode( teo, es, rowNode ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    return rval;
    }

static int tedUndoInsertColumn(	TedEditOperation *	teo,
				EditDocument *		ed,
				const EditStep *	es )
    {
    int				rval= 0;

    EditOperation *		eo= &(teo->teoEo);
    SelectionGeometry		sg;
    SelectionDescription	sd;

    BufferItem *		parentNode;
    int				col;
    int				row;
    int				row0;
    int				row1;

    tedStartUndoOperation( teo, &sg, &sd, ed, es );

    if  ( docDelimitTable( eo->eoHeadDp.dpNode, &parentNode,
						&col, &row0, &row, &row1 ) )
	{ LDEB(1); rval= -1; goto ready;	}

    docEditIncludeNodeInReformatRange( eo, parentNode );

    for ( row= row0; row <= row1; row++ )
	{
	BufferItem *	rowNode= parentNode->biChildren[row];
	int		wide;

	int		sectionsDeleted= 0;
	int		firstParaDeleted= -1;
	int		paragraphsDeleted= 0;

	wide= docColumnWidth( &(rowNode->biRowProperties), col );

	docEditDeleteNodes( eo, &sectionsDeleted,
			    &firstParaDeleted, &paragraphsDeleted,
			    rowNode, col, 1 );

	switch( es->esSelectionPosition )
	    {
	    case SELposBEFORE:
		docRowPropertiesMakeColWider(
				&(rowNode->biRowProperties), col, wide );
		break;

	    case SELposAFTER:
		docRowPropertiesMakeColWider(
				&(rowNode->biRowProperties), col- 1, wide );
		break;

	    default:
		LDEB(es->esSelectionPosition);
		break;
	    }
	}

    if  ( tedUndoFinishOld( teo, es ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    return rval;
    }

/************************************************************************/
/*									*/
/*  Delete the header or footer that was inserted.			*/
/*									*/
/*  We know that the insert funtion moved the selection to the fresh	*/
/*  header or footer. So we can remove the currently selected tree.	*/
/*									*/
/************************************************************************/

static int tedUndoInsertHeaderFooter(	TedEditOperation *	teo,
					EditDocument *		ed,
					const EditStep *	es )
    {
    int				rval= 0;

    EditOperation *		eo= &(teo->teoEo);
    SelectionGeometry		sg;
    SelectionDescription	sd;

    DocumentTree * 		dtHdFt;

    tedStartUndoOperation( teo, &sg, &sd, ed, es );
    dtHdFt= eo->eoTree;

    docMoveEditOperationToBodySect( eo, eo->eoBodySectNode );

    docEraseDocumentTree( eo->eoDocument, dtHdFt );

    if  ( tedUndoFinishOld( teo, es ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    return rval;
    }

/************************************************************************/
/*									*/
/*  Undo the deletion of a header orv a footer.				*/
/*									*/
/*  1)  Determine wat kind of header or footer the source document	*/
/*	contains.							*/
/*  2)  Find the corresponding header or footer in the target documnent	*/
/*  3)  Rebuild the hear or footer in the target.			*/
/*  4)  And copy the source tree to it.					*/
/*									*/
/************************************************************************/

static int tedUndoDeleteHeaderFooter(	TedEditOperation *	teo,
					EditDocument *		ed,
					const EditStep *	es )
    {
    int				rval= 0;

    EditOperation *		eo= &(teo->teoEo);
    SelectionGeometry		sg;
    SelectionDescription	sd;

    DocumentPosition		dpFrom;

    int				i;
    const DocumentTree *	dtFrom= (const DocumentTree *)0;
    int				fromCount;

    DocumentTree *		dtTo= (DocumentTree *)0;
    const int			ownerNumber= -1;
    const int			to= 0;

    BufferItem *		sectNodeFrom;

    tedStartUndoOperation( teo, &sg, &sd, ed, es );

    /*  1  */
    if  ( docHeadPosition( &dpFrom, es->esSourceDocument->bdBody.dtRoot ) )
	{ LDEB(1); rval= -1; goto ready;	}

    sectNodeFrom= docGetSectNode( dpFrom.dpNode );

    fromCount= 0;
    for ( i= 0; i < DOC_HeaderFooterTypeCount; i++ )
	{
	const DocumentTree *	dt;

	dt= docSectionHeaderFooter( sectNodeFrom, (unsigned char *)0,
					    &(es->esSourceDocument->bdProperties),
					    DOC_HeaderFooterTypes[i] );

	if  ( dt && dt->dtRoot )
	    { dtFrom= dt; fromCount++;	}
	}

    if  ( fromCount != 1 || ! dtFrom )
	{ LXDEB(fromCount,dtFrom); rval= -1; goto ready;	}

    docMoveEditOperationToBodySect( eo, eo->eoBodySectNode );

    /*  2  */
    dtTo= docSectionHeaderFooter( eo->eoBodySectNode, (unsigned char *)0,
					    &(eo->eoDocument->bdProperties),
					    dtFrom->dtRoot->biTreeType );
    if  ( ! dtTo || dtTo->dtRoot )
	{ XDEB(dtTo); rval= -1; goto ready;	}

    /*  3  */
    dtTo->dtRoot= docMakeTreeRoot( eo->eoDocument, dtTo,
					eo->eoBodySectNode, ownerNumber,
					dtFrom->dtRoot->biTreeType );
    if  ( ! dtTo->dtRoot )
	{ XDEB(dtTo->dtRoot); rval= -1; goto ready;	}

    /*  4  */
    if  ( docReinsertNodes( eo, dtTo->dtRoot, dtFrom->dtRoot, to, es ) )
	{ LDEB(to); rval= -1; goto ready;	}

    docInvalidateTreeLayout( dtTo );
    docEditIncludeNodeInReformatRange( eo, eo->eoBodySectNode );

    if  ( docSelectionSameScope( &(es->esOldSelectionScope),
			    &(dtTo->dtRoot->biSectSelectionScope) )	&&
	  es->esOldPage >= 0						)
	{
	dtTo->dtPageSelectedUpon= es->esOldPage;

	if  ( es->esOldColumn >= 0 )
	    { dtTo->dtColumnSelectedIn= es->esOldColumn;	}
	}

    if  ( tedUndoFinishOld( teo, es ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    return rval;
    }

static int tedUndoUpdateField(		TedEditOperation *	teo,
					EditDocument *		ed,
					const EditStep *	es )
    {
    int				rval= 0;

    EditOperation *		eo= &(teo->teoEo);
    SelectionGeometry		sg;
    SelectionDescription	sd;

    const int			lastOne= 0;

    const DocumentField *	dfFrom;
    DocumentField *		dfTo;

    DocumentSelection		dsAround;

    tedStartUndoOperation( teo, &sg, &sd, ed, es );

    dfFrom= docTraceGetFromField( es );
    if  ( ! dfFrom )
	{ XDEB(dfFrom); rval= -1; goto ready;	}

    dfTo= docFindChildField( &(eo->eoTree->dtRootFields),
				    &(es->esNewEditRange.erHead), lastOne );
    if  ( ! dfTo )
	{ XDEB(dfTo); rval= -1; goto ready;	}

    if  ( docCopyFieldProperties( dfTo, dfFrom ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( dfTo->dfKind == DOCfkTOC			&&
	  tedRefreshTocField( &dsAround, teo, dfTo )	)
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( tedUndoFinishOld( teo, es ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    return rval;
    }

/************************************************************************/
/*									*/
/*  Undo the creation of a field.					*/
/*									*/
/*  Just remove the field: More advanced scenarios are handled as a	*/
/*  replace action.							*/
/*									*/
/************************************************************************/

static int tedUndoSetField(		TedEditOperation *	teo,
					EditDocument *		ed,
					const EditStep *	es )
    {
    int				rval= 0;

    EditOperation *		eo= &(teo->teoEo);
    SelectionGeometry		sg;
    SelectionDescription	sd;

    DocumentField *		df;

    const int			lastOne= 0;

    DocumentSelection		dsInsideField;
    DocumentSelection		dsAroundField;
    int				headPart;
    int				tailPart;

    tedStartUndoOperation( teo, &sg, &sd, ed, es );

    df= docFindChildField( &(eo->eoTree->dtRootFields),
				    &(es->esNewEditRange.erHead), lastOne );
    if  ( ! df )
	{ XDEB(df); rval= -1; goto ready;	}

    if  ( docDelimitFieldInDoc( &dsInsideField, &dsAroundField,
				&headPart, &tailPart, eo->eoDocument, df ) )
	{ SDEB(docFieldKindStr(df->dfKind)); rval= -1; goto ready; }

    if  ( tedFlattenFieldImpl( teo, (DocumentSelection *)0,
				    &dsAroundField, headPart, tailPart, df ) )
	{ LDEB(1); rval= -1; goto ready; }

    if  ( tedUndoFinishOld( teo, es ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    return rval;
    }

static int tedUndoDelField(		TedEditOperation *	teo,
					EditDocument *		ed,
					const EditStep *	es )
    {
    int				rval= 0;

    EditOperation *		eo= &(teo->teoEo);
    SelectionGeometry		sg;
    SelectionDescription	sd;

    const DocumentField *	dfFrom;
    DocumentField *		dfTo;

    DocumentSelection		dsInside;
    DocumentSelection		dsAround;
    int				headPart;
    int				tailPart;

    tedStartUndoOperation( teo, &sg, &sd, ed, es );

    dfFrom= docTraceGetFromField( es );
    if  ( ! dfFrom )
	{ XDEB(dfFrom); rval= -1; goto ready;	}

    if  ( docEditSurroundTextSelectionByField( eo, &dfTo,
			    &dsInside, &dsAround, &headPart, &tailPart,
			    (PropertyMask *)0, (TextAttribute *)0,
			    dfFrom->dfKind, &(dfFrom->dfInstructions) ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( tedUndoFinishOld( teo, es ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    return rval;
    }

/************************************************************************/
/*									*/
/*  Undo a property update.						*/
/*									*/
/************************************************************************/

static int tedUndoUpdPropsImpl(	TedEditOperation *	teo,
				const EditStep *	es )
    {
    int				rval= 0;
    EditOperation *		eo= &(teo->teoEo);

    DocumentCopyJob		dcj;

    int				haveTextUpdate;
    int				haveNodeUpdate;

    DocumentSelection		dsTo;
    DocumentSelection		dsFrom;
    const int			direction= 1;

    docInitDocumentCopyJob( &dcj );

    if  ( docSetTraceDocumentCopyJob( &dcj, eo, es->esSourceDocument ) )
	{ LDEB(1); rval= -1; goto ready;	}

    haveTextUpdate= ! utilPropMaskIsEmpty( &(es->esNewStyle.dsTextMask) );

    haveNodeUpdate= ! utilPropMaskIsEmpty( &(es->esNewStyle.dsParaMask) )||
		    ! utilPropMaskIsEmpty( &(es->esNewStyle.dsCellMask) )||
		    ! utilPropMaskIsEmpty( &(es->esNewStyle.dsRowMask) )||
		    ! utilPropMaskIsEmpty( &(es->esNewStyle.dsSectMask) )||
		    ! utilPropMaskIsEmpty( &(es->esNewDocPropMask) );

    if  ( ! haveTextUpdate && ! haveNodeUpdate )
	{ goto ready;	}

    docInitDocumentSelection( &dsTo );
    docInitDocumentSelection( &dsFrom );

    docSetRangeSelection( &dsTo, &(eo->eoHeadDp), &(eo->eoTailDp), direction );
    docSelectWholeBody( &dsFrom, es->esSourceDocument );

    if  ( haveTextUpdate )
	{
	if  ( haveNodeUpdate )
	    {
	    if  ( docCopySelectionProperties( &dcj, &dsTo, &dsFrom,
		    (PropertyMask *)0, &(es->esNewStyle.dsParaMask), /* pp */
		    (PropertyMask *)0, &(es->esNewStyle.dsCellMask), /* cp */
		    (PropertyMask *)0, &(es->esNewStyle.dsRowMask), /* rp */
		    (PropertyMask *)0, &(es->esNewStyle.dsSectMask), /* sp */
		    (PropertyMask *)0, &(es->esNewDocPropMask) ) ) /* dp */
		{ LDEB(1); rval= -1; goto ready;	}
	    }

	/*  Do not copy paragraph properties */

	if  ( tedIncludeDocument( teo, &dcj ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}
    else{
	if  ( haveNodeUpdate )
	    {
	    if  ( docCopySelectionProperties( &dcj, &dsTo, &dsFrom,
		    (PropertyMask *)0, &(es->esNewStyle.dsParaMask), /* pp */
		    (PropertyMask *)0, &(es->esNewStyle.dsCellMask), /* cp */
		    (PropertyMask *)0, &(es->esNewStyle.dsRowMask), /* rp */
		    (PropertyMask *)0, &(es->esNewStyle.dsSectMask), /* sp */
		    (PropertyMask *)0, &(es->esNewDocPropMask) ) ) /* dp */
		{ LDEB(1); rval= -1; goto ready;	}
	    }
	}

  ready:

    docCleanDocumentCopyJob( &dcj );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Undo a property update.						*/
/*									*/
/************************************************************************/

static int tedUndoUpdProps(	TedEditOperation *	teo,
				EditDocument *		ed,
				const EditStep *	es )
    {
    int				rval= 0;

    SelectionGeometry		sg;
    SelectionDescription	sd;

    tedStartUndoOperation( teo, &sg, &sd, ed, es );

    if  ( tedUndoUpdPropsImpl( teo, es ) )
	{ LDEB(1); rval= -1; goto ready;	}

    tedEditFinishRange( teo,
		    es->esOldCol0, es->esOldCol1, &(es->esOldEditRange) );

  ready:

    return rval;
    }

/************************************************************************/
/*									*/
/*  Undo a shift-indent.						*/
/*									*/
/************************************************************************/

static int tedUndoShiftIndent(	TedEditOperation *	teo,
				EditDocument *		ed,
				const EditStep *	es )
    {
    int				rval= 0;

    EditOperation *		eo= &(teo->teoEo);
    SelectionGeometry		sg;
    SelectionDescription	sd;

    DocumentCopyJob		dcj;

    PropertyMask		ppSetMask;

    DocumentSelection		dsTo;
    DocumentSelection		dsFrom;
    const int			direction= 1;

    utilPropMaskClear( &ppSetMask );
    PROPmaskADD( &ppSetMask, PPpropLISTLEVEL );
    PROPmaskADD( &ppSetMask, PPpropLEFT_INDENT );
    PROPmaskADD( &ppSetMask, PPpropFIRST_INDENT );

    tedStartUndoOperation( teo, &sg, &sd, ed, es );

    docInitDocumentCopyJob( &dcj );

    if  ( docSetTraceDocumentCopyJob( &dcj, eo, es->esSourceDocument ) )
	{ LDEB(1); rval= -1; goto ready;	}

    docInitDocumentSelection( &dsTo );
    docInitDocumentSelection( &dsFrom );

    docSetRangeSelection( &dsTo, &(eo->eoHeadDp), &(eo->eoTailDp), direction );
    docSelectWholeBody( &dsFrom, es->esSourceDocument );

    if  ( docCopySelectionProperties( &dcj, &dsTo, &dsFrom,
	    (PropertyMask *)0, &ppSetMask, /* pp */
	    (PropertyMask *)0, (PropertyMask *)0, /* cp */
	    (PropertyMask *)0, (PropertyMask *)0, /* rp */
	    (PropertyMask *)0, (PropertyMask *)0, /* sp */
	    (PropertyMask *)0, (PropertyMask *)0 ) ) /* dp */
	{ LDEB(1); rval= -1; goto ready;	}

    tedEditFinishRange( teo,
		    es->esOldCol0, es->esOldCol1, &(es->esOldEditRange) );

  ready:

    docCleanDocumentCopyJob( &dcj );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Undo a object/picture property update.				*/
/*									*/
/************************************************************************/

static int tedUndoUpdObject(	TedEditOperation *	teo,
				EditDocument *		ed,
				const EditStep *	es )
    {
    const PictureProperties *	pipFrom;

    int				partObjTo;
    DocumentPosition		dpObjTo;
    InsertedObject *		ioTo;

    int				rval= 0;

    SelectionGeometry		sg;
    SelectionDescription	sd;

    tedStartUndoOperation( teo, &sg, &sd, ed, es );

    pipFrom= docTraceGetFromPictureProps( es );
    if  ( ! pipFrom )
	{ XDEB(pipFrom); rval= -1; goto ready;	}

    if  ( tedGetObjectSelection( ed, &partObjTo, &dpObjTo, &ioTo ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( tedObjectSetImagePropertiesImpl( teo, ioTo, &dpObjTo, partObjTo,
					&(es->esPicturePropMask), pipFrom ) )
	{ LDEB(1); rval= -1; goto ready;	}

    tedEditFinishRange( teo,
		    es->esOldCol0, es->esOldCol1, &(es->esOldEditRange) );

  ready:

    return rval;
    }

/************************************************************************/
/*									*/
/*  Undo a foot/end note property update.				*/
/*									*/
/************************************************************************/

static int tedUndoUpdNote(	TedEditOperation *	teo,
				EditDocument *		ed,
				const EditStep *	es )
    {
    int				rval= 0;

    EditOperation *		eo= &(teo->teoEo);
    SelectionGeometry		sg;
    SelectionDescription	sd;

    int				selInNote= 0;

    const DocumentField *	dfFrom;
    const DocumentNote *	dnFrom= (DocumentNote *)0;
    DocumentField *		dfTo= (DocumentField *)0;
    DocumentNote *		dnTo= (DocumentNote *)0;

    tedStartUndoOperation( teo, &sg, &sd, ed, es );

    dfFrom= docTraceGetFromField( es );
    if  ( ! dfFrom )
	{ XDEB(dfFrom); rval= -1; goto ready;	}
    dnFrom= docGetNoteOfField( dfFrom, es->esSourceDocument );
    if  ( ! dnFrom )
	{ XDEB(dnFrom); rval= -1; goto ready;	}

    dfTo= docEditOperationGetSelectedNote( &dnTo, &selInNote, eo );
    if  ( ! dfTo )
	{ XDEB(dfTo); rval= -1; goto ready;	}

    if  ( tedChangeNoteImpl( eo, dfTo, dnTo, selInNote,
						&(es->esNotePropMask),
						&(dnFrom->dnNoteProperties) ) )
	{ LDEB(1); rval= -1; goto ready;	}

    tedEditFinishRange( teo,
		    es->esOldCol0, es->esOldCol1, &(es->esOldEditRange) );

  ready:

    return rval;
    }

/************************************************************************/
/*									*/
/*  Undo a change to a list.						*/
/*									*/
/************************************************************************/

static int tedUndoUpdList(	TedEditOperation *	teo,
				EditDocument *		ed,
				const EditStep *	es )
    {
    int				rval= 0;

    struct ListOverride *	lo;
    DocumentList *		dl;

    EditOperation *		eo= &(teo->teoEo);
    SelectionGeometry		sg;
    SelectionDescription	sd;

    tedStartUndoOperation( teo, &sg, &sd, ed, es );

    if  ( ! sd.sdHasLists		||
	  sd.sdListOverride < 1	)
	{ LLDEB(sd.sdHasLists,sd.sdListOverride); return -1;	}

    if  ( docGetListOfParagraph( &lo, &dl, sd.sdListOverride, eo->eoDocument ) )
	{ LDEB(1); return -1;	}

    if  ( docEditChangeList( eo, dl, lo, &(es->esDocumentList) ) )
	{ LDEB(1); rval= -1; goto ready;	}

    tedEditFinishRange( teo,
		    es->esOldCol0, es->esOldCol1, &(es->esOldEditRange) );

  ready:

    return rval;
    }

/************************************************************************/
/*									*/
/*  Undo the creation of a new list.					*/
/*									*/
/************************************************************************/

static int tedUndoSetNewList(	TedEditOperation *	teo,
				EditDocument *		ed,
				const EditStep *	es )
    {
    int				rval= 0;

    struct ListOverride *	lo;
    DocumentList *		dl;

    EditOperation *		eo= &(teo->teoEo);
    SelectionGeometry		sg;
    SelectionDescription	sd;

    tedStartUndoOperation( teo, &sg, &sd, ed, es );

    if  ( ! sd.sdHasLists	||
	  sd.sdListOverride < 1	)
	{ LLDEB(sd.sdHasLists,sd.sdListOverride); return -1;	}

    if  ( docGetListOfParagraph( &lo, &dl, sd.sdListOverride, eo->eoDocument ) )
	{ LDEB(1); return -1;	}

    if  ( tedUndoUpdPropsImpl( teo, es ) )
	{ LDEB(1); rval= -1; goto ready;	}

    {
    ListAdmin *		la= eo->eoDocument->bdProperties.dpListAdmin;

    if  ( docDocumentListTableDeleteList( &(la->laListTable),
							    lo->loListIndex ) )
	{ LDEB(lo->loListIndex); rval= -1;	}

    if  ( docListOverrideTableDeleteOverride( &(la->laListOverrideTable),
							    lo->loIndex ) )
	{ LDEB(lo->loIndex); rval= -1;	}
    }

    tedEditFinishRange( teo,
		    es->esOldCol0, es->esOldCol1, &(es->esOldEditRange) );

  ready:

    return rval;
    }

/************************************************************************/
/*									*/
/*  Undo roll table rows.						*/
/*									*/
/************************************************************************/

static int tedUndoShiftRows(	TedEditOperation *	teo,
				EditDocument *		ed,
				const EditStep *	es )
    {
    int				rval= 0;

    SelectionGeometry		sg;
    SelectionDescription	sd;

    const int			move= 1;
    int				rowsdown= 0;

    /*  It is an undo: swap rows down */
    if  ( es->esCommand == EDITcmdSHIFT_ROWS_DOWN )
	{ rowsdown= -1;	}
    else{ rowsdown=  1;	}

    tedStartUndoOperation( teo, &sg, &sd, ed, es );

    if  ( tedDocRollRowsInTableImpl( teo, &(sd.sdTableRectangle),
							    move, rowsdown ) )
	{ LDEB(1); rval= -1; goto ready;	}

    tedEditFinishRange( teo,
		    es->esOldCol0, es->esOldCol1, &(es->esOldEditRange) );

  ready:

    return rval;
    }

/************************************************************************/
/*									*/
/*  Undo.								*/
/*									*/
/************************************************************************/

void tedDocUndo(	EditDocument *	ed )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;
    EditTrace *			et= &(td->tdEditTrace);

    TedEditOperation		teo;
    EditStep			es;
    const int			direction= -1;

    docInitEditStep( &es );
    tedInitEditOperation( &teo );

    if  ( docEditReadTraceStep( &es, (int *)0, direction, et, bd ) )
	{ LDEB(1); goto ready;	}

    switch( es.esCommand )
	{
	case EDITcmdMERGE_PARAS:
	case EDITcmdREPLACE:
	case EDITcmdREPLACE_FTNSEP:
	case EDITcmdREPLACE_BODY_LEVEL:
	case EDITcmdDELETE_SELECTION:
	case EDITcmdSPLIT_SECT:
	case EDITcmdINS_NOTE:
	case EDITcmdINSERT_TABLE:
	    if  ( tedUndoReplace( &teo, ed, &es, SELposALL, 1 ) )
		{ LDEB(es.esCommand); goto ready;	}
	    break;

	case EDITcmdDELETE_SELECTION_DEL:
	    if  ( tedUndoReplace( &teo, ed, &es, SELposHEAD, 1 ) )
		{ LDEB(es.esCommand); goto ready;	}
	    break;

	case EDITcmdDELETE_SELECTION_BS:
	    if  ( tedUndoReplace( &teo, ed, &es, SELposTAIL, 1 ) )
		{ LDEB(es.esCommand); goto ready;	}
	    break;


	case EDITcmdDELETE_PARA:
	    if  ( tedUndoDeletePara( &teo, ed, &es ) )
		{ LDEB(es.esCommand); goto ready;	}
	    break;
	case EDITcmdDELETE_COLUMN:
	    if  ( tedUndoDeleteColumn( &teo, ed, &es ) )
		{ LDEB(es.esCommand); goto ready;	}
	    break;
	case EDITcmdDELETE_ROW:
	case EDITcmdDELETE_TABLE:
	    if  ( tedUndoDeleteRow( &teo, ed, &es ) )
		{ LDEB(es.esCommand); goto ready;	}
	    break;
	case EDITcmdDELETE_SECT:
	    if  ( tedUndoDeleteSect( &teo, ed, &es ) )
		{ LDEB(es.esCommand); goto ready;	}
	    break;
	case EDITcmdDELETE_HEADER:
	case EDITcmdDELETE_FOOTER:
	    if  ( tedUndoDeleteHeaderFooter( &teo, ed, &es ) )
		{ LDEB(es.esCommand); goto ready;	}
	    break;

	case EDITcmdINSERT_PARA:
	case EDITcmdAPPEND_PARA:
	    if  ( tedUndoInsertPara( &teo, ed, &es ) )
		{ LDEB(es.esCommand); goto ready;	}
	    break;
	case EDITcmdINSERT_COLUMN:
	case EDITcmdAPPEND_COLUMN:
	    if  ( tedUndoInsertColumn( &teo, ed, &es ) )
		{ LDEB(es.esCommand); goto ready;	}
	    break;
	case EDITcmdINSERT_ROW:
	case EDITcmdAPPEND_ROW:
	    if  ( tedUndoInsertRow( &teo, ed, &es ) )
		{ LDEB(es.esCommand); goto ready;	}
	    break;
	case EDITcmdINSERT_SECT:
	case EDITcmdAPPEND_SECT:
	    if  ( tedUndoInsertSect( &teo, ed, &es ) )
		{ LDEB(es.esCommand); goto ready;	}
	    break;
	case EDITcmdINSERT_HEADER:
	case EDITcmdINSERT_FOOTER:
	    if  ( tedUndoInsertHeaderFooter( &teo, ed, &es ) )
		{ LDEB(es.esCommand); goto ready;	}
	    break;

	case EDITcmdUPD_FIELD:
	    if  ( tedUndoUpdateField( &teo, ed, &es ) )
		{ LDEB(es.esCommand); goto ready;	}
	    break;

	case EDITcmdSET_HYPERLINK:
	case EDITcmdSET_BOOKMARK:
	    if  ( utilPropMaskIsEmpty( &(es.esNewStyle.dsTextMask) ) )
		{
		if  ( tedUndoSetField( &teo, ed, &es ) )
		    { LDEB(es.esCommand); goto ready;	}
		}
	    else{
		if  ( tedUndoReplace( &teo, ed, &es, SELposALL, 0 ) )
		    { LDEB(es.esCommand); goto ready;	}
		}
	    break;

	case EDITcmdDEL_FIELD:
	    if  ( utilPropMaskIsEmpty( &(es.esNewStyle.dsTextMask) ) )
		{
		if  ( tedUndoDelField( &teo, ed, &es ) )
		    { LDEB(es.esCommand); goto ready;	}
		}
	    else{
		if  ( tedUndoReplace( &teo, ed, &es, SELposALL, 0 ) )
		    { LDEB(es.esCommand); goto ready;	}
		}
	    break;

	case EDITcmdUPD_SPAN_PROPS:
	case EDITcmdUPD_PARA_PROPS:
	case EDITcmdUPD_TABLE_PROPS:
	case EDITcmdUPD_SECT_PROPS:
	case EDITcmdUPD_SECTDOC_PROPS:
	case EDITcmdUPD_DOC_PROPS:
	    if  ( tedUndoUpdProps( &teo, ed, &es ) )
		{ LDEB(es.esCommand); goto ready;	}
	    break;

	case EDITcmdUPD_OBJECT:
	    if  ( tedUndoUpdObject( &teo, ed, &es ) )
		{ LDEB(es.esCommand); goto ready;	}
	    break;
	case EDITcmdUPD_NOTE:
	    if  ( tedUndoUpdNote( &teo, ed, &es ) )
		{ LDEB(es.esCommand); goto ready;	}
	    break;

	case EDITcmdUPD_LIST:
	    if  ( tedUndoUpdList( &teo, ed, &es ) )
		{ LDEB(es.esCommand); goto ready;	}
	    break;
	case EDITcmdSET_NEW_LIST:
	    if  ( tedUndoSetNewList( &teo, ed, &es ) )
		{ LDEB(es.esCommand); goto ready;	}
	    break;

	case EDITcmdSHIFT_ROWS_DOWN:
	case EDITcmdSHIFT_ROWS_UP:
	    if  ( tedUndoShiftRows( &teo, ed, &es ) )
		{ LDEB(es.esCommand); goto ready;	}
	    break;

	case EDITcmdSHIFT_RIGHT:
	case EDITcmdSHIFT_LEFT:
	    if  ( tedUndoShiftIndent( &teo, ed, &es ) )
		{ LDEB(es.esCommand); goto ready;	}
	    break;

	case EDITcmd_OPEN:
	case EDITcmd_SAVE:
	case EDITcmd_NEW:
	default:
	    LSDEB(es.esCommand,docRtfGetTraceTag(es.esCommand));
	    goto ready;
	}

    et->etIndex= es.esTraceIndex;

    tedFinishEditOperation( &teo );

  ready:

    docCleanEditStep( &es );
    tedCleanEditOperation( &teo );

    return;
    }

