/************************************************************************/
/*									*/
/*  Editor functionality.						*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	<docScreenLayout.h>
#   include	<docField.h>
#   include	"tedEdit.h"
#   include	"tedLayout.h"
#   include	"tedSelect.h"
#   include	"tedDocument.h"
#   include	"tedApp.h"
#   include	<docTreeType.h>
#   include	<docTreeNode.h>
#   include	<docRecalculateFields.h>
#   include	<docRtfTrace.h>
#   include	<docDocumentNote.h>
#   include	<docNotes.h>

#   include	<appDebugon.h>

#   define	VALIDATE_TREE		0

/************************************************************************/
/*									*/
/*  Redo tree layout after editing.					*/
/*									*/
/************************************************************************/

static int tedEditRedoTreeLayout(	TedEditOperation *	teo,
					BufferItem *		bodySectNode,
					DocumentTree *		dt )
    {
    const int		page= dt->dtPageFormattedFor;
    const int		column= dt->dtColumnFormattedFor;
    const int		adjustDocument= 1;

    docInvalidateTreeLayout( dt );

    if  ( docLayoutDocumentTree( dt, &(teo->teoChangedRect),
			    page, column, dt->dtY0UsedTwips,
			    bodySectNode, 
			    &(teo->teoLayoutContext),
			    docStartScreenLayoutForTree, adjustDocument ) )
	{ LDEB(1); return -1;	}

    if  ( tedOpenTreeObjects( dt, &(teo->teoLayoutContext) ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

/************************************************************************/

static int tedEditRecalculateTextLevelFields(	EditOperation *		eo,
						RecalculateFields *	rf,
						DocumentTree *		tree )
    {
    rf->rfSelectedTree= eo->eoTree;
    rf->rfSelHead= eo->eoSelectedRange.erHead;
    rf->rfSelTail= eo->eoSelectedRange.erTail;

    if  ( docRecalculateTextLevelFields( rf, tree->dtRoot ) )
	{ LDEB(1);	}

    eo->eoSelectedRange.erHead= rf->rfSelHead;
    eo->eoSelectedRange.erTail= rf->rfSelTail;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Redo the layout of a range after editing.				*/
/*									*/
/************************************************************************/

static int tedEditRedoRangeLayout(	TedEditOperation *	teo,
					DocumentTree *		tree,
					BufferItem *		bodySectNode )
    {
    EditOperation *		eo= &(teo->teoEo);
    BufferDocument *		bd= eo->eoDocument;
    EditRange *			er= &(eo->eoReformatRange);

    BufferItem *		selRootNode;
    DocumentSelection		dsLayout;

    if  ( docLayoutInvalidateRange( &dsLayout, bd, tree, er ) )
	{ LDEB(1); return -1;	}

    selRootNode= docGetSelectionRoot( (DocumentTree **)0,
					(BufferItem **)0, bd, &dsLayout );
    if  ( ! selRootNode )
	{ XDEB(selRootNode); return -1;	}

    while( selRootNode->biParent				&&
	   selRootNode->biParent->biChildCount == 1	)
	{ selRootNode= selRootNode->biParent;	}

    if  ( selRootNode->biTreeType != DOCinBODY	&&
	  ! selRootNode->biParent		)
	{
	if  ( tedEditRedoTreeLayout( teo, bodySectNode, tree ) )
	    { LDEB(1); return -1;	}
	}
    else{
	int		reachedBottom= 0;

	docInvalidateNodeLayout( selRootNode );

	if  ( docScreenLayoutNode( &reachedBottom, selRootNode,
					    &(teo->teoLayoutContext),
					    &(teo->teoChangedRect) ) )
	    { LDEB(1);	}

	if  ( tedOpenNodeObjects( selRootNode, &(teo->teoLayoutContext) ) )
	    { LDEB(1); return -1;	}

	if  ( reachedBottom )
	    { teo->teoRefreshScreenRectangle= 1;	}
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Redo document layout after editing.					*/
/*									*/
/************************************************************************/

static int tedEditRedoNoteLayout(	int			n,
					void *			voidteo );

static int tedEditRefreshLayout(	TedEditOperation *	teo )
    {
    const LayoutContext  *	lc= &(teo->teoLayoutContext);
    EditOperation *		eo= &(teo->teoEo);
    BufferDocument *		bd= eo->eoDocument;
    EditRange *			er= &(eo->eoReformatRange);

    EditDocument *		ed= teo->teoEditDocument;

    RecalculateFields		rf;

    DocumentTree *		tree;
    BufferItem *		bodySectNode;
    int				reachedBottom= 0;

    DocumentPosition		dpLast;
    int				lastParaNr;

    if  ( docGetRootOfSelectionScope( &tree, &bodySectNode,
						bd, &(eo->eoSelectionScope) ) )
	{ LDEB(1); return -1;	}

    if  ( docTailPosition( &dpLast, tree->dtRoot ) )
	{ LDEB(1); return -1;	}
    lastParaNr= docNumberOfParagraph( dpLast.dpNode );

    if  ( er->erHead.epParaNr <= lastParaNr )
	{
	if  ( docEditFixParaBreakKind( eo, tree, bd, er->erHead.epParaNr ) )
	    { LDEB(er->erHead.epParaNr);	}
	}
    if  ( er->erTail.epParaNr <= lastParaNr )
	{
	if  ( docEditFixParaBreakKind( eo, tree, bd, er->erTail.epParaNr ) )
	    { LDEB(er->erTail.epParaNr);	}
	}

#   if LOG_RELAYOUT
    docLogRectangle( "EDITED", &(teo->teoChangedRect) );
#   endif

    docInitRecalculateFields( &rf );

    rf.rfDocument= bd;
    rf.rfTree= tree;
    rf.rfSelectedTree= tree;
    rf.rfCloseObject= lc->lcCloseObject;
    rf.rfUpdateFlags= eo->eoFieldUpdate;
    rf.rfFieldsUpdated= 0;
    rf.rfBodySectNode= bodySectNode;

    if  ( ( eo->eoFieldUpdate & FIELDdoCHFTN ) != 0		||
	  utilIndexSetGetFirst( &(eo->eoNoteFieldsAdded) ) >= 0	||
	  eo->eoSectionsDeleted > 0				||
	  eo->eoSectionsAdded > 0				)
	{
	int			changed= 0;

	docRenumberNotes( &changed, bd );

	if  ( changed )
	    { rf.rfUpdateFlags |= FIELDdoCHFTN;	}
	}

    if  ( rf.rfUpdateFlags )
	{
	if  ( tedEditRecalculateTextLevelFields( eo, &rf, &(bd->bdBody) ) )
	    { LDEB(1);	}
	}

    if  ( utilIndexSetForAll( &(eo->eoNoteFieldsAdded),
				    tedEditRedoNoteLayout, (void *)teo ) < 0 )
	{ LDEB(1); return -1;	}

    if  ( rf.rfFieldsUpdated > 0 )
	{ eo->eoReformatNeeded= REFORMAT_DOCUMENT;	}

    if  ( eo->eoReformatNeeded != REFORMAT_DOCUMENT )
	{
	if  ( ( eo->eoFieldUpdate & FIELDdoCHFTN ) != 0			||
	      utilIndexSetGetFirst( &(eo->eoNoteFieldsAdded) ) >= 0	||
	      eo->eoSectionsDeleted > 0					||
	      eo->eoSectionsAdded > 0					)
	    {
	    docInvalidateNodeLayout( bd->bdBody.dtRoot );

	    eo->eoReformatNeeded= REFORMAT_DOCUMENT;
	    }
	}

    switch( eo->eoReformatNeeded )
	{
	DocumentSelection	dsLayout;

	case REFORMAT_ADJUST_PARAGRAPH:
	    if  ( eo->eoParaAdjustParagraphNumber < 0	||
		  tedAdjustParagraphLayout( teo, tree )	)
		{ LDEB(eo->eoParaAdjustParagraphNumber); }

#	    if LOG_RELAYOUT
	    docLogRectangle( "ADJPAR", &(teo->teoChangedRect) );
#	    endif
	    break;

	case REFORMAT_RANGE:

	    if  ( tedEditRedoRangeLayout( teo, tree, bodySectNode ) )
		{ LDEB(1); return -1;	}

#	    if LOG_RELAYOUT
	    docLogRectangle( "RANGE-", &(teo->teoChangedRect) );
#	    endif
	    break;

	case REFORMAT_BODY_SECT:
	    docInvalidateNodeLayout( eo->eoBodySectNode );

	    if  ( docScreenLayoutNode( &reachedBottom, eo->eoBodySectNode,
				    &(teo->teoLayoutContext),
				    &(teo->teoChangedRect) ) )
		{ LDEB(1);	}

	    if  ( tedOpenNodeObjects( eo->eoBodySectNode,
						&(teo->teoLayoutContext) ) )
		{ LDEB(1); return -1;	}
#	    if LOG_RELAYOUT
	    docLogRectangle( "SECT--", &(teo->teoChangedRect) );
#	    endif
	    break;

	case REFORMAT_DOCUMENT:
	    if  ( docLayoutInvalidateRange( &dsLayout, bd, tree, er ) )
		{ LDEB(1); return -1;	}

	    if  ( tedLayoutDocumentBody( &reachedBottom,
						&(teo->teoLayoutContext) ) )
		{ LDEB(1); return -1;	}

	    if  ( tedOpenNodeObjects( bd->bdBody.dtRoot, lc ) )
		{ LDEB(1); return -1;	}

	    tedIncludeRectangleInChange( teo, &(ed->edFullRect) );
#	    if LOG_RELAYOUT
	    docLogRectangle( "DOC---", &(teo->teoChangedRect) );
#	    endif

	    break;

	case REFORMAT_NOTHING:
	    break;

	default:
	    LDEB(eo->eoReformatNeeded);
	}

    if  ( reachedBottom )
	{ teo->teoRefreshScreenRectangle= 1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Redo note layout after editing.					*/
/*									*/
/************************************************************************/

static int tedEditRedoNoteLayout(	int			fieldNr,
					void *			voidteo )
    {
    TedEditOperation *		teo= (TedEditOperation *)voidteo;
    EditOperation *		eo= &(teo->teoEo);
    const LayoutContext  *	lc= &(teo->teoLayoutContext);
    BufferDocument *		bd= lc->lcDocument;
    DocumentField *		df;
    DocumentNote *		dn;

    RecalculateFields		rf;
    const unsigned int		fieldUpdMask= FIELDdoCHFTN;

    int				reachedBottom= 0;

    df= docGetFieldByNumber( &(bd->bdFieldList), fieldNr );
    if  ( ! df || df->dfKind != DOCfkCHFTN )
	{ LXDEB(fieldNr,df); return -1;	}
    dn= docGetNoteOfField( df, bd );
    if  ( ! dn )
	{ XDEB(dn); return -1;	}

    if  ( docCheckSeparatorItemForNoteType( bd,
					dn->dnNoteProperties.npTreeType ) )
	{ LDEB(1); return -1;	}

    /*  3  */
    docInitRecalculateFields( &rf );

    rf.rfDocument= bd;
    rf.rfTree= &(dn->dnDocumentTree);
    rf.rfCloseObject= teo->teoLayoutContext.lcCloseObject;
    rf.rfUpdateFlags= fieldUpdMask;
    rf.rfFieldsUpdated= 0;

    if  ( tedEditRecalculateTextLevelFields( eo, &rf, &(dn->dnDocumentTree) ) )
	{ XDEB(fieldUpdMask);	}

    /*  NO! Is just to setup the note.
    docEditIncludeNodeInReformatRange( eo, dn->dnDocumentTree.dtRoot );
    */

    /*  Exclude from Edit operation: In another tree */
    if  ( docScreenLayoutNode( &reachedBottom, dn->dnDocumentTree.dtRoot,
					    &(teo->teoLayoutContext),
					    &(teo->teoChangedRect) ) )
	{ LDEB(1); return -1;	}

    if  ( reachedBottom )
	{ teo->teoRefreshScreenRectangle= 1;	}

    return 0;
    }

/************************************************************************/

static void tedEditRefreshScreenRectangle(	TedEditOperation *	teo )
    {
    EditDocument *		ed= teo->teoEditDocument;
    const LayoutContext  *	lc= &(teo->teoLayoutContext);
    BufferDocument *		bd= lc->lcDocument;

    DocumentRectangle		drFull;
    BufferItem *		rootNode= bd->bdBody.dtRoot;

    docGetPixelRectangleForPages( &drFull,
					&(teo->teoLayoutContext),
					rootNode->biTopPosition.lpPage,
					rootNode->biBelowPosition.lpPage );

    if  ( drFull.drX1 != ed->edFullRect.drX1	||
	  drFull.drY1 != ed->edFullRect.drY1	)
	{
	DocumentRectangle	drAround;
	DocumentRectangle	drExtra;

	drAround= drFull;

	if  ( drAround.drX1 < ed->edFullRect.drX1 )
	    { drAround.drX1=  ed->edFullRect.drX1;	}
	if  ( drAround.drY1 < ed->edFullRect.drY1 )
	    { drAround.drY1=  ed->edFullRect.drY1;	}

	drExtra= drAround;
	if  ( ed->edFullRect.drX1 < drFull.drX1 )
	    {
	    drExtra.drX0= ed->edFullRect.drX1+ 1;
	    drExtra.drX1= drFull.drX1;

	    geoUnionRectangle( &(teo->teoChangedRect),
					&(teo->teoChangedRect), &drExtra );
	    }

	if  ( drFull.drX1 < ed->edFullRect.drX1 )
	    {
	    drExtra.drX0= drFull.drX1+ 1;
	    drExtra.drX1= ed->edFullRect.drX1;

	    geoUnionRectangle( &(teo->teoChangedRect),
					&(teo->teoChangedRect), &drExtra );
	    }

	drExtra= drAround;
	if  ( ed->edFullRect.drY1 < drFull.drY1 )
	    {
	    drExtra.drY0= ed->edFullRect.drY1+ 1;
	    drExtra.drY1= drFull.drY1;

	    geoUnionRectangle( &(teo->teoChangedRect),
					&(teo->teoChangedRect), &drExtra );
	    }

	if  ( drFull.drY1 < ed->edFullRect.drY1 )
	    {
	    drExtra.drY0= drFull.drY1+ 1;
	    drExtra.drY1= ed->edFullRect.drY1;

	    geoUnionRectangle( &(teo->teoChangedRect),
					&(teo->teoChangedRect), &drExtra );
	    }

	ed->edFullRect= drFull;
	}
    }

/************************************************************************/
/*									*/
/*  Do the last steps that set an I-Bar selection after an editing	*/
/*  action.								*/
/*									*/
/*  1)  The order in which the size of the window is adapted and the	*/
/*	window is scrolled to the selection depends on the direction	*/
/*	of the change, as the scrollbars complain when the corrent	*/
/*	position is not in range.					*/
/*  2)  Do not place the IBar in a bullet or note number.		*/
/*									*/
/************************************************************************/

static int tedEditFinishIBarSelection2(
			    TedEditOperation *		teo,
			    const DocumentPosition *	dpNew )
    {
    EditDocument *		ed= teo->teoEditDocument;
    BufferDocument *		bd= teo->teoEo.eoDocument;

    int				scrolledX= 0;
    int				scrolledY= 0;

    const int			lastLine= 0;

    /*  1  */
    if  ( teo->teoRefreshScreenRectangle )
	{ tedEditRefreshScreenRectangle( teo );	}

    if  ( ed->edFullRect.drY1 > teo->teoOldScreenRectangle.drY1 )
	{
	appDocSetScrollbarValues( ed );
	}

    if  ( dpNew->dpNode )
	{
	DocumentPosition	dpIbar;

	/*  2  */
	dpIbar= *dpNew;
	docAvoidParaHeadField( &dpIbar, (int *)0, bd );

	tedSetIBarSelection( ed, &dpIbar, lastLine, &scrolledX, &scrolledY );
	}
    else{ XDEB(dpNew->dpNode);	}


    /*  1  */
    if  ( ed->edFullRect.drY1 < teo->teoOldScreenRectangle.drY1 )
	{
	appDocSetScrollbarValues( ed );
	}

    appDocExposeRectangle( ed, &(teo->teoChangedRect), scrolledX, scrolledY );

    if  ( tedHasIBarSelection( ed ) )
	{ tedStartCursorBlink( ed );	}

    return 0;
    }

int tedEditFinishPosition(	TedEditOperation *		teo,
				const EditPosition *		epNew )
    {
    EditOperation *		eo= &(teo->teoEo);
    DocumentPosition		dpNew;

    if  ( tedEditRefreshLayout( teo ) )
	{ LDEB(1); /* return -1; */	}

    docPositionForEditPosition( &dpNew, epNew, eo->eoTree );

    return tedEditFinishIBarSelection2( teo, &dpNew );
    }

int tedEditFinishSelectionTail(	TedEditOperation *		teo )
    {
    EditOperation *		eo= &(teo->teoEo);

    return tedEditFinishPosition( teo, &(eo->eoSelectedRange.erTail) );
    }

int tedEditFinishSelectionHead(	TedEditOperation *		teo )
    {
    EditOperation *		eo= &(teo->teoEo);

    return tedEditFinishPosition( teo, &(eo->eoAffectedRange.erHead) );
    }

int tedEditFinishSelectionHeadNext(	TedEditOperation *		teo )
    {
    EditOperation *		eo= &(teo->teoEo);
    DocumentPosition		dpNew;

    if  ( tedEditRefreshLayout( teo ) )
	{ LDEB(1); /* return -1; */	}

    docPositionForEditPosition( &dpNew,
			    &(eo->eoAffectedRange.erHead), eo->eoTree );
    if  ( docNextPosition( &dpNew ) )
	{ LDEB(1);	}

    return tedEditFinishIBarSelection2( teo, &dpNew );
    }

/************************************************************************/
/*									*/
/*  Adapt to a formatting change.					*/
/*									*/
/************************************************************************/

static int tedEditFinishSelection2(	TedEditOperation *		teo,
					const DocumentSelection *	dsNew )
    {
    EditDocument *		ed= teo->teoEditDocument;

    int				scrolledX= 0;
    int				scrolledY= 0;

    const int			lastLine= 0;

    if  ( docIsIBarSelection( dsNew ) )
	{ return tedEditFinishIBarSelection2( teo, &(dsNew->dsHead) );	}

    if  ( teo->teoRefreshScreenRectangle )
	{ tedEditRefreshScreenRectangle( teo );	}

    if  ( ed->edFullRect.drY1 > teo->teoOldScreenRectangle.drY1 )
	{
	appDocSetScrollbarValues( ed );
	}

    tedSetSelection( ed, dsNew, lastLine, &scrolledX, &scrolledY );

    if  ( ed->edFullRect.drY1 < teo->teoOldScreenRectangle.drY1 )
	{
	appDocSetScrollbarValues( ed );
	}

    appDocExposeRectangle( ed, &(teo->teoChangedRect), scrolledX, scrolledY );

    return 0;
    }


int tedEditFinishSelection(	TedEditOperation *		teo,
				const DocumentSelection *	dsNew )
    {
    int		rval;

    if  ( tedEditRefreshLayout( teo ) )
	{ LDEB(1); return -1;	}

    rval= tedEditFinishSelection2( teo, dsNew );

    return rval;
    }

int tedEditFinishRange(	TedEditOperation *		teo,
			int				col0,
			int				col1,
			const EditRange *		erNew )
    {
    int				rval;
    EditOperation *		eo= &(teo->teoEo);
    DocumentSelection		dsNew;

    if  ( tedEditRefreshLayout( teo ) )
	{ LDEB(1); return -1;	}

    docInitDocumentSelection( &dsNew );

    if  ( docSelectionForEditPositionsInTree( &dsNew, eo->eoTree,
				    &(erNew->erHead), &(erNew->erTail) ) )
	{ LDEB(1); return -1;	}

    dsNew.dsCol0= col0;
    dsNew.dsCol1= col1;

    rval= tedEditFinishSelection2( teo, &dsNew );

    return rval;
    }

int tedEditFinishOldSelection(		TedEditOperation *		teo )
    {
    EditOperation *		eo= &(teo->teoEo);
    const EditRange *		er= &(teo->teoEo.eoSelectedRange);
    int				rval;

    EditDocument *		ed= teo->teoEditDocument;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    DocumentSelection		dsNew;

    if  ( tedEditRefreshLayout( teo ) )
	{ LDEB(1); return -1;	}

    docInitDocumentSelection( &dsNew );

    if  ( docSelectionForEditPositionsInTree( &dsNew, eo->eoTree,
					&(er->erHead), &(er->erTail) ) )
	{ LDEB(1); return -1;	}

    rval= tedEditFinishSelection2( teo, &dsNew );

    td->tdSelectionDescription.sdTextAttribute= teo->teoSavedTextAttribute;
    td->tdSelectionDescription.sdTextAttributeNumber= teo->teoSavedTextAttributeNumber;

    return rval;
    }

/************************************************************************/
/*									*/
/*  1)  Start an Edit Operation.					*/
/*  2)  Finish an Edit Operation.					*/
/*									*/
/*  The reference positions in the Start and Finish calls should be	*/
/*  the same position in the document from a logical point of view.	*/
/*									*/
/************************************************************************/

void tedInitEditOperation(	TedEditOperation *	teo )
    {
    docInitEditOperation( &(teo->teoEo) );

    teo->teoEditDocument= (EditDocument *)0;
    layoutInitContext( &(teo->teoLayoutContext) );

    geoInitRectangle( &(teo->teoChangedRect) );
    geoInitRectangle( &(teo->teoOldScreenRectangle) );
    teo->teoChangedRectSet= 0;
    teo->teoRefreshScreenRectangle= 0;
    }

void tedCleanEditOperation(	TedEditOperation *	teo )
    {
    docCleanEditOperation( &(teo->teoEo) );

    return;
    }

/*  1  */
int tedStartEditOperation(	TedEditOperation *	teo,
				SelectionGeometry *	sg,
				SelectionDescription *	sd,
				EditDocument *		ed,
				int			fullWidth,
				int			traced )
    {
    EditOperation *		eo= &(teo->teoEo);
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;

    DocumentSelection		ds;

#   if VALIDATE_TREE
    {
    LDEB(1);
    if  ( docCheckNode( &(bd->bdItem) ) )
	{ LDEB(2); docListNode( 0, &(bd->bdItem) ); abort(); }
    }
#   endif

    tedInitEditOperation( teo );

    /**/
    teo->teoEditDocument= ed;
    tedSetScreenLayoutContext( &(teo->teoLayoutContext), ed );
    eo->eoCloseObject= teo->teoLayoutContext.lcCloseObject;

    if  ( tedGetSelection( &ds, sg, sd,
				&(eo->eoTree), &(eo->eoBodySectNode), ed ) )
	{ LDEB(1); return -1;	}

    if  ( docStartEditOperation( eo, &ds, bd ) )
	{ LDEB(1); return -1;	}

    teo->teoSavedTextAttribute= td->tdSelectionDescription.sdTextAttribute;
    teo->teoSavedTextAttributeNumber= td->tdSelectionDescription.sdTextAttributeNumber;

    eo->eoIBarSelectionOld= sd->sdIsIBarSelection;
    eo->eoMultiParagraphSelectionOld= ! sd->sdIsSingleParagraph;

    tedIncludeRectangleInChange( teo, &(sg->sgRectangle) );
    teo->teoChangedRect.drX1= ed->edFullRect.drX1;

    if  ( fullWidth )
	{ teo->teoChangedRect.drX0= ed->edFullRect.drX0; }

    teo->teoOldScreenRectangle= ed->edFullRect;

    if  ( traced )
	{ teo->teoEditTrace= &(td->tdEditTrace);	}
    else{ teo->teoEditTrace= (EditTrace *)0;		}

    tedStopCursorBlink( ed );

    return 0;
    }

int tedEditStartReplace(	DocumentSelection *	dsTraced,
				TedEditOperation *	teo,
				int			command,
				int			level,
				unsigned int		flags )
    {
    if  ( teo->teoEditTrace						&&
	  docTraceStartReplace( dsTraced, &(teo->teoEo), teo->teoEditTrace,
						    command, level, flags ) )
	{ LLDEB(command,level); return -1;	}

    return 0;
    }

int tedEditStartTypedStep(	TedEditOperation *	teo,
				int			command,
				int			fieldKind )
    {
    if  ( teo->teoEditTrace						&&
	  docTraceStartStep( &(teo->teoEo), teo->teoEditTrace,
						  command, fieldKind ) )
	{ LDEB(command); return -1;	}

    return 0;
    }


int tedEditStartStep(		TedEditOperation *	teo,
				int			command )
    {
    int		fieldKind= -1;

    return tedEditStartTypedStep( teo, command, fieldKind );
    }

int tedFinishEditOperation(		TedEditOperation *	teo )
    {
    EditDocument *		ed= teo->teoEditDocument;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    if  ( teo->teoEditTrace )
	{
	if  ( docRtfTraceCloseTrace( &(teo->teoEo), teo->teoEditTrace ) )
	    { LDEB(1); return -1;	}
	}

    if  ( td->tdTraced )
	{ tedSetTracedChangedFlag( teo->teoEditDocument );	}
    else{ appDocumentChanged( teo->teoEditDocument, 1 );	}

    tedAdaptToolsToSelection( teo->teoEditDocument );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Widen redraw rectangle to whole BufferItems.			*/
/*									*/
/************************************************************************/

int tedEditIncludeNodeInRedraw(	TedEditOperation *	teo,
				const BufferItem *	bi )
    {
    EditDocument *		ed= teo->teoEditDocument;
    const LayoutContext  *	lc= &(teo->teoLayoutContext);
    DocumentRectangle		drLocal;

    drLocal.drX0= ed->edFullRect.drX0;
    drLocal.drX1= ed->edFullRect.drX1;
    drLocal.drY0= docLayoutYPixels( lc, &(bi->biTopPosition) )- 1;
    drLocal.drY1= docLayoutYPixels( lc, &(bi->biBelowPosition) )- 1;

    if  ( docIsRowNode( bi ) )
	{
	drLocal.drY1=
		docLayoutYPixels( lc, &(bi->biRowBelowAllCellsPosition) )- 1;
	}

    tedIncludeRectangleInChange( teo, &drLocal );

    return 0;
    }

int tedEditIncludeRowsInRedraw(	TedEditOperation *	teo,
				const BufferItem *	parentNode,
				int			row0,
				int			row1 )
    {
    EditDocument *		ed= teo->teoEditDocument;
    const LayoutContext  *	lc= &(teo->teoLayoutContext);
    const BufferItem *		rowNode;
    DocumentRectangle		drLocal;

    drLocal.drX0= ed->edFullRect.drX0;
    drLocal.drX1= ed->edFullRect.drX1;

    rowNode= parentNode->biChildren[row0];
    drLocal.drY0= docLayoutYPixels( lc, &(rowNode->biTopPosition) )- 1;

    rowNode= parentNode->biChildren[row1];
    drLocal.drY1= docLayoutYPixels( lc, &(rowNode->biRowBelowAllCellsPosition) )- 1;

    tedIncludeRectangleInChange( teo, &drLocal );

    return 0;
    }

void tedIncludeRectangleInChange(	TedEditOperation *		teo,
					const DocumentRectangle *	dr )
    {
    if  ( teo->teoChangedRectSet )
	{
	geoUnionRectangle( &(teo->teoChangedRect), &(teo->teoChangedRect), dr );
	return;
	}
    else{
	teo->teoChangedRect= *dr;
	teo->teoChangedRectSet= 1;
	}
    }

