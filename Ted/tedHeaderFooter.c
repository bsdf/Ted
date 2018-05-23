/************************************************************************/
/*									*/
/*  Edit/Make headers and footers.					*/
/*  Jump back and forth between the body of a document and notes.	*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	"tedApp.h"
#   include	"tedSelect.h"
#   include	"tedAppResources.h"
#   include	"tedLayout.h"
#   include	"tedEdit.h"
#   include	"tedDocFront.h"
#   include	"tedDocument.h"
#   include	<docSelectLayout.h>
#   include	<docRtfTrace.h>
#   include	<docTreeNode.h>
#   include	<appQuestion.h>
#   include	<docEditCommand.h>
#   include	<docTreeType.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Insert a header or a footer in the document.			*/
/*									*/
/*  1)  Start edit opration and tracing.				*/
/*  2)  Find the header/footer and make sure it has content.		*/
/*  4)  Move the selection to the head of the fresh header/footer.	*/
/*  5)  Finish.								*/
/*									*/
/************************************************************************/

int tedDocInsertHeaderFooter(	EditDocument *		ed,
				int			treeType,
				int			traced )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    int				rval = 0;

    TedEditOperation		teo;
    EditOperation *		eo= &(teo.teoEo);

    SelectionGeometry		sg;
    SelectionDescription	sd;

    DocumentTree *		dtHdFt;
    BufferItem *		bodySectNode;

    BufferItem *		treeParaNode;
    const int			ownerNumber= -1;

    int				page= -1;
    const int			column= 0;
    DocumentPosition		dpNew;

    const int			fullWidth= 1;

    /*  1  */
    tedStartEditOperation( &teo, &sg, &sd, ed, fullWidth, traced );

    if  ( docIsHeaderType( treeType ) )
	{
	if  ( tedEditStartStep( &teo, EDITcmdINSERT_HEADER ) )
	    { LDEB(EDITcmdINSERT_HEADER); goto ready;	}
	}
    else{
	if  ( tedEditStartStep( &teo, EDITcmdINSERT_FOOTER ) )
	    { LDEB(EDITcmdINSERT_FOOTER); goto ready;	}
	}

    /*  2  */
    if  ( docGetHeaderFooter( &dtHdFt, &bodySectNode, &(eo->eoHeadDp),
						eo->eoDocument, treeType ) )
	{ LDEB(treeType); rval= -1; goto ready;	}

    treeParaNode= docMakeExternalParagraph( eo->eoDocument, dtHdFt, treeType,
			    bodySectNode, ownerNumber,
			    td->tdSelectionDescription.sdTextAttributeNumber );
    if  ( ! treeParaNode )
	{ XDEB(treeParaNode); rval= -1; goto ready;	}

    /*  4  */
    if  ( docHeadPosition( &dpNew, treeParaNode ) )
	{ LDEB(1); rval= -1; goto ready;	}
    docAvoidParaHeadField( &dpNew, (int *)0, eo->eoDocument );

    page= docHeaderFooterPage( eo->eoDocument, bodySectNode,
				sg.sgHead.pgTopPosition.lpPage, treeType );
    if  ( page < 0 )
	{ LDEB(page); rval= -1; goto ready;	}

    docInvalidateTreeLayout( dtHdFt );

    dtHdFt->dtPageSelectedUpon= page;
    dtHdFt->dtColumnSelectedIn= column;

    eo->eoTree= dtHdFt;
    docGetSelectionScope( &(eo->eoSelectionScope), bodySectNode );
    eo->eoReformatNeeded= REFORMAT_BODY_SECT;

    /*  5  */
    docSetIBarRange( &(eo->eoAffectedRange), &dpNew );
    docSetIBarRange( &(eo->eoSelectedRange), &dpNew );
    tedEditFinishSelectionTail( &teo );

    if  ( teo.teoEditTrace )
	{
	docRtfTraceNewPosition( eo,
			&(dtHdFt->dtRoot->biSectSelectionScope), SELposHEAD );
	}

    tedFinishEditOperation( &teo );

  ready:

    tedCleanEditOperation( &teo );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Move the current selection to a certain header/Footer. If the	*/
/*  header or footer does not exist, make it.				*/
/*									*/
/*  1)	Where are we now?						*/
/*  2)	Already in the correct header/footer: ready.			*/
/*  3)	Does the section of the body have enough pages to show this	*/
/*	kind of header or footer?					*/
/*  4)	Does the the header/footer already exist?			*/
/*  5)	If so, move the selection to the header/footer and we are ready	*/
/*  6)	Otherwise make it.						*/
/*									*/
/************************************************************************/

void tedDocEditHeaderFooter(		EditDocument *		ed,
					APP_WIDGET		relative,
					APP_WIDGET		option,
					int			treeType )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;

    DocumentTree *		ei;
    BufferItem *		bodySectNode;

    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    int				page= -1;
    const int			column= 0;

    LayoutContext		lc;

    layoutInitContext( &lc );
    tedSetScreenLayoutContext( &lc, ed );

    /*  1  */
    if  ( tedGetSelection( &ds, &sg, &sd, &ei, &bodySectNode, ed ) )
	{ LDEB(1); return;	}

    /*  2  */
    if  ( ds.dsSelectionScope.ssTreeType == treeType			&&
	  ds.dsSelectionScope.ssOwnerSectNr == bodySectNode->biNumberInParent )
	{ return;	}

    /*  3  */
    page= docHeaderFooterPage( bd, bodySectNode,
				sg.sgHead.pgTopPosition.lpPage, treeType );

    if  ( page < bodySectNode->biTopPosition.lpPage		||
	  page > bodySectNode->biBelowPosition.lpPage		)
	{
	EditApplication *	ea= ed->edApplication;
	TedAppResources *	tar= (TedAppResources *)ea->eaResourceData;

	appQuestionRunErrorDialog( ea, relative, option,
					    tar->tarHeadFootNotReachable );
	return;
	}

    /*  4  */
    if  ( docGetHeaderFooter( &ei, &bodySectNode, &(ds.dsHead), bd, treeType ) )
	{ LDEB(treeType); return;	}

    /*  5  */
    if  ( ei->dtRoot )
	{
	DocumentPosition	dpNew;
	const int		lastLine= 0;

	int			scrolledX= 0;
	int			scrolledY= 0;

	ei->dtPageSelectedUpon= page;
	ei->dtColumnSelectedIn= column;

	if  ( docHeadPosition( &dpNew, ei->dtRoot ) )
	    { LDEB(1); return;	}

	docAvoidParaHeadField( &dpNew, (int *)0, bd );
	tedSetSelectedPosition( ed, &dpNew, lastLine, &scrolledX, &scrolledY );

	return;
	}

    /*  6  */
    if  ( tedDocInsertHeaderFooter( ed, treeType, td->tdTraced ) )
	{ LDEB(treeType);	}

    return;
    }

/************************************************************************/
/*									*/
/*  Delete a particular page Header/Footer from the first section of	*/
/*  the selection.							*/
/*									*/
/*  1)  Get the given header/footer.					*/
/*  2)  If it exists.. Delete it.					*/
/*  3)  If the current selection is inside the header/footer to be	*/
/*	deleted.. Move the selection to the body somewhere nearby.	*/
/*									*/
/************************************************************************/

int tedDocDeleteHeaderFooter(		EditDocument *		ed,
					int			treeType,
					int			traced )
    {
    int				rval= 0;

    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;

    DocumentTree *		tree= (DocumentTree *)0;
    BufferItem *		bodySectNode;

    TedEditOperation		teo;
    EditOperation *		eo= &(teo.teoEo);
    SelectionGeometry		sg;
    SelectionDescription	sd;

    DocumentSelection		dsOld;

    const int			fullWidth= 1;

    tedStartEditOperation( &teo, &sg, &sd, ed, fullWidth, traced );

    docEditOperationGetSelection( &dsOld, eo );

    /*  1  */
    if  ( docGetHeaderFooter( &tree, &bodySectNode, &(eo->eoHeadDp),
							    bd, treeType ) )
	{ LDEB(treeType); rval= -1; goto ready;	}

    if  ( ! tree || ! tree->dtRoot )
	{ XDEB(tree); rval= -1; goto ready;	}

    if  ( docIsHeaderType( treeType ) )
	{
	if  ( tedEditStartStep( &teo, EDITcmdDELETE_HEADER ) )
	    { LDEB(EDITcmdDELETE_HEADER); goto ready;	}
	}
    else{
	if  ( tedEditStartStep( &teo, EDITcmdDELETE_FOOTER ) )
	    { LDEB(EDITcmdDELETE_FOOTER); goto ready;	}
	}

    if  ( teo.teoEditTrace && docRtfTraceHeaderFooter( eo, tree ) )
	{ LDEB(1); rval= -1; goto ready;	}

    /*  3  */
    if  ( eo->eoTree->dtRoot->biTreeType == treeType )
	{
	DocumentPosition	dpNew;
	SelectionScope		ssNew;

	if  ( docPositionNearHeadFoot( &dpNew, treeType, bodySectNode,
			    bd, eo->eoHeadDp.dpNode->biTopPosition.lpPage ) )
	    { LDEB(1); rval= -1; goto ready;	}

	docSetIBarRange( &(eo->eoAffectedRange), &dpNew );
	docSetIBarRange( &(eo->eoSelectedRange), &dpNew );

	if  ( docMoveEditOperationToBodySect( eo, bodySectNode ) )
	    { LDEB(1); rval= -1; goto ready;	}

	/*  2  */
	docEraseDocumentTree( bd, tree );

	docGetSelectionScope( &ssNew, dpNew.dpNode );

	tedEditFinishSelectionTail( &teo );

	if  ( teo.teoEditTrace )
	    {
	    /* make docReinsertNodes() happy */
	    docRtfTraceNewPosition( eo, &ssNew, SELposAFTER );
	    }
	}
    else{
	if  ( docMoveEditOperationToBodySect( eo, bodySectNode ) )
	    { LDEB(1); rval= -1; goto ready;	}

	/*  2  */
	docEraseDocumentTree( bd, tree );

	tedEditFinishSelection( &teo, &dsOld );

	if  ( teo.teoEditTrace )
	    {
	    /* make docReinsertNodes() happy */
	    docRtfTraceNewPosition( eo,
				(const SelectionScope *)0, SELposAFTER );
	    }
	}

    tedFinishEditOperation( &teo );

  ready:

    tedCleanEditOperation( &teo );

    return rval;
    }

