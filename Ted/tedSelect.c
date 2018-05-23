/************************************************************************/
/*									*/
/*  Ted: Management of the selection and the current position.		*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	"tedApp.h"
#   include	"tedAppFront.h"
#   include	"tedSelect.h"
#   include	"tedLayout.h"
#   include	"tedDocument.h"
#   include	<docDescribeSetSelection.h>
#   include	<docParaParticules.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Is the selection exactly an object.					*/
/*									*/
/************************************************************************/

int tedGetObjectSelection(	const EditDocument *		ed,
				int *				pPartObj,
				DocumentPosition *		dpObj,
				InsertedObject **		pIo )
    {
    const TedDocument *		td= (const TedDocument *)ed->edPrivateData;
    const BufferDocument *	bd= td->tdDocument;
    int				rval;

    rval= docGetObjectSelection( &(td->tdSelection),
					    bd, pPartObj, dpObj, pIo );
    if  ( rval )
	{ return rval;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Scroll the slection into view.					*/
/*									*/
/************************************************************************/

static void tedScrollToPositions(EditDocument *			ed,
				const PositionGeometry *	pgB,
				const PositionGeometry *	pgE,
				const LayoutContext *		lc,
				int *				pScrolledX,
				int *				pScrolledY )
    {
    DocumentRectangle		drPixels;

    docPixelRectangleForPositions( &drPixels, pgB, pgE, lc );

    appScrollToRectangle( ed, &drPixels, pScrolledX, pScrolledY );

    return;
    }

/************************************************************************/
/*									*/
/*  Scroll a selection into view.					*/
/*									*/
/************************************************************************/

void tedScrollToSelection(	EditDocument *		ed,
				int *			pScrolledX,
				int *			pScrolledY )
    {
    DocumentSelection		dsDoc;
    SelectionGeometry		sgDoc;
    SelectionDescription	sdDoc;

    LayoutContext		lc;

    layoutInitContext( &lc );
    tedSetScreenLayoutContext( &lc, ed );

    if  ( tedGetSelection( &dsDoc, &sgDoc, &sdDoc,
			    (DocumentTree **)0, (BufferItem **)0, ed ) )
	{ LDEB(1); return;	}

    tedScrollToPositions( ed, &(sgDoc.sgHead), &(sgDoc.sgTail),
					    &lc, pScrolledX, pScrolledY );

    return;
    }

/************************************************************************/
/*									*/
/*  Extend a selection that begins outside a table cell and ends inside	*/
/*  that cell.								*/
/*									*/
/*  The speccial cases that occur when the selection begins in a table	*/
/*  are covered by tedExtendSelectionFromTable(), that has priority	*/
/*  over tedExtendSelectionIntoTable().					*/
/*									*/
/************************************************************************/

static int tedExtendSelectionIntoTable(	EditDocument *		ed,
					DocumentPosition *	dpEnd )
    {
    BufferItem *	cellNode= dpEnd->dpNode->biParent;
    BufferItem *	rowNode= cellNode->biParent;

    int			col1= rowNode->biChildCount -1;

    docTailPosition( dpEnd, rowNode->biChildren[col1] );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Extend a selection that begins inside a table cell and ends outside	*/
/*  that cell.								*/
/*									*/
/*  1)  Not in same section.						*/
/*  2)  Look for selection end in same table.				*/
/*  3)  Found?								*/
/*									*/
/************************************************************************/

static int tedExtendSelectionFromTable(	EditDocument *		ed,
					DocumentPosition *	dpHead,
					DocumentPosition *	dpTail,
					int *			pCol0,
					int *			pCol1 )
    {
    BufferItem *	hCellNode= dpHead->dpNode->biParent;
    BufferItem *	hRowNode= hCellNode->biParent;
    BufferItem *	hParentNode= hRowNode->biParent;

    BufferItem *	tRowNode= dpTail->dpNode->biParent->biParent;
    BufferItem *	tParentNode= tRowNode->biParent;

    int			row1= hRowNode->biNumberInParent;

    int			col0= -1;
    int			col1= -1;

    /*  1  */
    if  ( tParentNode != hParentNode )
	{
	docHeadPosition( dpHead, hRowNode->biChildren[0] );

	if  ( dpTail->dpNode->biParaTableNesting > 0 )
	    { tedExtendSelectionIntoTable( ed, dpTail ); }

	return 0;
	}

    /*  2  */
    while( row1 < hRowNode->biRowTablePast- 1 )
	{
	if  ( tRowNode == hParentNode->biChildren[row1] )
	    { break;	}

	row1++;
	}

    if  ( tRowNode == hParentNode->biChildren[row1] )
	{
	if  ( dpTail->dpNode->biParent->biNumberInParent <
					    hCellNode->biNumberInParent )
	    {
	    col0= dpTail->dpNode->biParent->biNumberInParent;
	    col1= dpHead->dpNode->biParent->biNumberInParent;

	    docHeadPosition( dpHead, hRowNode->biChildren[col0] );
	    hRowNode= dpTail->dpNode->biParent->biParent;
	    docTailPosition( dpTail, hRowNode->biChildren[col1] );
	    }
	else{
	    docHeadPosition( dpHead, dpHead->dpNode->biParent );
	    docTailPosition( dpTail, dpTail->dpNode->biParent );
	    }

	col0= dpHead->dpNode->biParent->biNumberInParent;
	col1= dpTail->dpNode->biParent->biNumberInParent;
	}
    else{
	docHeadPosition( dpHead, hRowNode->biChildren[0] );

	if  ( dpTail->dpNode->biParaTableNesting > 0 )
	    { tedExtendSelectionIntoTable( ed, dpTail ); }

	return 0;
	}

    *pCol0= col0; *pCol1= col1; return 0;
    }

/************************************************************************/
/*									*/
/*  Extend the selection upon subsequent MotionNotify events.		*/
/*									*/
/*  1)  Do not completely describe the selection now. This is enough	*/
/*	for the redraws. [The mouse up handler will fully describe!]	*/
/*									*/
/************************************************************************/

static void tedSetExtendedSelection(
				EditDocument *			ed,
				int				exposeSelection,
				const DocumentRectangle *	drOldSel,
				int				direction,
				const BufferItem *		bodySectNode,
				const DocumentPosition *	dpExposeBegin,
				const DocumentPosition *	dpExposeEnd,
				const DocumentPosition *	dpTo,
				const DocumentPosition *	dpBegin,
				const DocumentPosition *	dpEnd )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    DocumentSelection		dsExpose;

    DocumentSelection		dsDoc;
    SelectionGeometry		sgDoc;
    SelectionDescription	sdDoc;

    PositionGeometry		pgTo;

    int				scrolledX= 0;
    int				scrolledY= 0;
    const int			lastLine= 0;

    LayoutContext		lc;
    int				lastOne= (direction>0)?PARAfindLAST:PARAfindFIRST;

    layoutInitContext( &lc );
    tedSetScreenLayoutContext( &lc, ed );

    docSetRangeSelection( &dsExpose, dpExposeBegin, dpExposeEnd, 1 );

    tedPositionGeometry( &pgTo, dpTo, lastOne, &lc );

    tedScrollToPositions( ed, &pgTo, &pgTo, &lc, &scrolledX, &scrolledY );

    if  ( exposeSelection )
	{
	appDocExposeRectangle( ed, drOldSel, scrolledX, scrolledY );
	}

    tedExposeSelection( ed, &dsExpose, bodySectNode, scrolledX, scrolledY );

    docSetRangeSelection( &(td->tdSelection), dpBegin, dpEnd, direction );
    tedSelectionGeometry( &(td->tdSelectionGeometry),
				    &(td->tdSelection), bodySectNode,
				    lastLine, &lc );

    /*  1  */
    td->tdSelectionDescription.sdIsIBarSelection=
				docIsIBarSelection( &(td->tdSelection) );

    if  ( exposeSelection )
	{
	if  ( tedGetSelection( &dsDoc, &sgDoc, &sdDoc,
				(DocumentTree **)0, (BufferItem **)0, ed ) )
	    { LDEB(1); return;	}

	appDocExposeRectangle( ed, &(sgDoc.sgRectangle), scrolledX, scrolledY );
	}

    tedManagePrimarySelection( ed );

    return;
    }

static void tedBalanceFieldSelection(	BufferDocument *	bd,
					DocumentTree *		ei,
					int *			pBalanced,
					DocumentPosition *	dpBegin,
					DocumentPosition *	dpEnd )
    {
    DocumentField *		dfLeft;
    DocumentField *		dfRight;
    int				beginMoved= 0;
    int				endMoved= 0;
    int				headPart= -1;
    int				tailPart= -1;

    docBalanceFieldSelection( &dfLeft, &dfRight, &beginMoved, &endMoved,
						    &headPart, &tailPart,
						    dpBegin, dpEnd, ei, bd );

    if  ( beginMoved )
	{ *pBalanced= 1;	}
    if  ( endMoved )
	{ *pBalanced= 1;	}

    return;
    }

int tedExtendSelectionToPosition(
				EditDocument *			ed,
				const DocumentPosition *	dpAnchor,
				const DocumentPosition *	dpFound )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    int				col0= -1;
    int				col1= -1;

    DocumentPosition		dpFrom;
    DocumentPosition		dpTo;

    int				directionToAnchor;
    int				directionBeginAnchor;
    int				directionEndAnchor;
    int				dir;

    int				scrolledX= 0;
    int				scrolledY= 0;

    int				cellChanged= 0;
    int				balanced= 0;
    int				exposeSelection;

    DocumentSelection		dsDoc;
    SelectionGeometry		sgDoc;
    SelectionDescription	sdDoc;

    DocumentTree *		ei;
    BufferItem *		bodySectNode;

    dpFrom= *dpAnchor;
    dpTo= *dpFound;

    if  ( tedGetSelection( &dsDoc, &sgDoc, &sdDoc, &ei, &bodySectNode, ed ) )
	{ LDEB(1); return -1;	}

    directionToAnchor= docComparePositions( &dpTo, dpAnchor );
    directionBeginAnchor= docComparePositions( &(dsDoc.dsHead), dpAnchor );
    directionEndAnchor= docComparePositions( &(dsDoc.dsTail), dpAnchor );

    /********************/
    /*  Before anchor.	*/
    /********************/
    if  ( directionToAnchor < 0 )
	{
	int		directionToBegin;

	if  ( docPositionsInsideCell( &dpTo, &dpFrom ) )
	    {
	    if  ( dpFrom.dpNode->biParaTableNesting > 0 )
		{ col0= col1= dpFrom.dpNode->biParent->biNumberInParent; }
	    }
	else{
	    if  ( dpTo.dpNode->biParaTableNesting > 0 )
		{
		if  ( tedExtendSelectionFromTable( ed, &dpTo, &dpFrom,
							    &col0, &col1 ) )
		    { LDEB(1); return -1;	}
		}
	    else{
		if  ( dpFrom.dpNode->biParaTableNesting > 0 )
		    {
		    if  ( tedExtendSelectionIntoTable( ed, &dpFrom ) )
			{ LDEB(1); return -1;	}
		    }
		}
	    }

	tedBalanceFieldSelection( td->tdDocument, ei, &balanced, &dpTo, &dpFrom );

	directionToBegin= docComparePositions( &dpTo, &(dsDoc.dsHead) );
	cellChanged= ! docPositionsInsideCell( &dpTo, &(dsDoc.dsHead) );

	/****************************************/
	/*  Undraw selection after the anchor.	*/
	/****************************************/
	if  ( directionEndAnchor > 0	||
	      balanced			||
	      cellChanged		)
	    {
	    appDocExposeRectangle( ed, &(sgDoc.sgRectangle),
						    scrolledX, scrolledY );
	    }

	/************************/
	/*  Extended Left.	*/
	/************************/
	if  ( directionToBegin < 0 )
	    {
	    dir= -1;
	    exposeSelection=	directionEndAnchor > 0	||
				balanced		||
				cellChanged		;

	    tedSetExtendedSelection( ed, exposeSelection, &(sgDoc.sgRectangle),
				dir, bodySectNode,
				&dpTo, &(dsDoc.dsHead),
				&dpTo, &dpTo, &dpFrom );

	    td->tdVisibleSelectionCopied= 0;
	    return 0;
	    }

	/************************/
	/*  Shrunk Left.	*/
	/************************/
	if  ( directionToBegin > 0 )
	    {
	    dir= -1;
	    exposeSelection=	directionEndAnchor > 0	||
				balanced		||
				cellChanged		;

	    tedSetExtendedSelection( ed, exposeSelection, &(sgDoc.sgRectangle),
				    dir, bodySectNode,
				    &(dsDoc.dsHead), &dpTo,
				    &dpTo, &dpTo, &dpFrom );

	    td->tdVisibleSelectionCopied= 0;
	    return 0;
	    }

	return 0;
	}

    /********************/
    /*  After anchor.	*/
    /********************/
    if  ( directionToAnchor > 0 )
	{
	int		directionToEnd;

	if  ( docPositionsInsideCell( &dpTo, &dpFrom ) )
	    {
	    if  ( dpFrom.dpNode->biParaTableNesting > 0 )
		{ col0= col1= dpFrom.dpNode->biParent->biNumberInParent;	}
	    }
	else{
	    if  ( dpFrom.dpNode->biParaTableNesting > 0 )
		{
		if  ( tedExtendSelectionFromTable( ed, &dpFrom, &dpTo,
							    &col0, &col1 ) )
		    { LDEB(1); return -1;	}
		}
	    else{
		if  ( dpTo.dpNode->biParaTableNesting > 0 )
		    {
		    if  ( tedExtendSelectionIntoTable( ed, &dpTo ) )
			{ LDEB(1); return -1;	}
		    }
		}
	    }

	tedBalanceFieldSelection( td->tdDocument, ei, &balanced, &dpFrom, &dpTo );

	directionToEnd= docComparePositions( &dpTo, &(dsDoc.dsTail) );
	cellChanged= ! docPositionsInsideCell( &dpTo, &(dsDoc.dsTail) );

	/****************************************/
	/*  Undraw selection before the anchor.	*/
	/****************************************/
	if  ( directionBeginAnchor < 0	||
	      balanced			||
	      cellChanged		)
	    {
	    appDocExposeRectangle( ed, &(sgDoc.sgRectangle),
						    scrolledX, scrolledY );
	    }

	/************************/
	/*  Extended Right.	*/
	/************************/
	if  ( directionToEnd > 0 )
	    {
	    dir= 1;
	    exposeSelection=	directionBeginAnchor < 0	||
				balanced			||
				cellChanged			;

	    tedSetExtendedSelection( ed, exposeSelection, &(sgDoc.sgRectangle),
				    dir, bodySectNode,
				    &(dsDoc.dsTail), &dpTo,
				    &dpTo, &dpFrom, &dpTo );

	    td->tdVisibleSelectionCopied= 0;
	    return 0;
	    }

	/************************/
	/*  Shrunk Right.	*/
	/************************/
	if  ( directionToEnd < 0 )
	    {
	    dir= 1;
	    exposeSelection=	directionBeginAnchor < 0	||
				balanced			||
				cellChanged			;

	    tedSetExtendedSelection( ed, exposeSelection, &(sgDoc.sgRectangle),
				    dir, bodySectNode,
				    &dpTo, &(dsDoc.dsTail),
				    &dpTo, &dpFrom, &dpTo );

	    td->tdVisibleSelectionCopied= 0;
	    return 0;
	    }

	return 0;
	}

    /********************/
    /*  At anchor.	*/
    /********************/
	{
	dpFrom= *dpAnchor;
	dpTo= *dpAnchor;

	tedBalanceFieldSelection( td->tdDocument, ei, &balanced, &dpFrom, &dpTo );

	/****************************************/
	/*  Undraw selection before the anchor.	*/
	/*  Undraw selection after the anchor.	*/
	/****************************************/
	if  ( directionBeginAnchor < 0	||
	      directionEndAnchor > 0	)
	    {
	    appDocExposeRectangle( ed, &(sgDoc.sgRectangle),
						    scrolledX, scrolledY );
	    }

	dir= 0;
	exposeSelection=	directionBeginAnchor < 0	||
				directionEndAnchor > 0		||
				balanced			;

	tedSetExtendedSelection( ed, exposeSelection, &(sgDoc.sgRectangle),
				dir, bodySectNode,
				&dpFrom, &dpTo,
				&dpFrom, &dpFrom, &dpTo );

	td->tdVisibleSelectionCopied= 0;
	return 0;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Move to the next/previous position.					*/
/*									*/
/************************************************************************/

static int tedArrowFindPositionInLine(	DocumentPosition *	dp,
					const LayoutContext *	lc,
					int			line,
					int			docXPixels )
    {
    PositionGeometry	pg;

    docInitPositionGeometry( &pg );

    if  ( tedFindPositionInLine( dp, &pg, lc, dp->dpNode, line, docXPixels ) )
	{ LDEB(docXPixels); return -1;	}

    return 0;
    }

int tedArrowUp(		DocumentPosition *		dp,
			const PositionGeometry *	pg,
			const LayoutContext *		lc )
    {
    int			line;

    if  ( docLineUp( &line, dp, pg->pgLine ) )
	{ return -1;	}

    if  ( tedArrowFindPositionInLine( dp, lc, line, pg->pgXPixels ) )
	{ LDEB(pg->pgXPixels); return -1;	}

    return 0;
    }

int tedArrowDown(	DocumentPosition *		dp,
			const PositionGeometry *	pg,
			const LayoutContext *		lc )
    {
    int			line;

    if  ( docLineDown( &line, dp, pg->pgLine ) )
	{ return -1;	}

    if  ( tedArrowFindPositionInLine( dp, lc, line, pg->pgXPixels ) )
	{ LDEB(pg->pgXPixels); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  A selection was made, do bookkeeping and provoke drawing.		*/
/*									*/
/*  0)  If an object (Picture) was selected, remove the special window.	*/
/*  1)  Provoke the old selection to be redrawn. (Before scrolling)	*/
/*  2)  Stop Cursor blinking.						*/
/*  3)  Scroll to the new selection.					*/
/*  4)  Provoke the old selection to be redrawn. (It might have moved)	*/
/*  5)  Bookkeeping.							*/
/*  6)  Provoke the new selection to be redrawn.			*/
/*									*/
/*  NOTE:	As scrolling to the new selection can provoke a redraw	*/
/*		the selection needs to be set before the scrollbar is	*/
/*		adapted. Otherwise the selection administration can	*/
/*		still refer to deleted text.				*/
/*									*/
/*  A)  If the selection is partially inside and partially outside a	*/
/*	text level field, force it to contain the whole field. This is	*/
/*	to enforce proper text field nesting.				*/
/*									*/
/************************************************************************/

void tedSetSelectionLow(	EditDocument *			ed,
				const DocumentSelection *	dsSet,
				int				lastLine,
				int *				pScrolledX,
				int *				pScrolledY )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;
    int				hadSelection;

    DocumentSelection		dsOld;
    SelectionGeometry		sgOld;
    SelectionDescription	sdOld;

    DocumentSelection		dsNew;
    SelectionGeometry		sgNew;

    int				balanced= 0;

    DocumentRectangle		drExpose;
    DocumentTree *		treeSet= (DocumentTree *)0;
    BufferItem *		bodySectBiSet= (BufferItem *)0;

    DocumentRectangle		drExternalSet;
    int				bodySectNr= -1;

    int				redrawTreeOld= 0;
    int				redrawTreeSet= 0;

    int				scrolledX= 0;
    int				scrolledY= 0;

    LayoutContext		lc;

    layoutInitContext( &lc );
    tedSetScreenLayoutContext( &lc, ed );

    td->tdEditTrace.etTyping= TYPING_NO;

    if  ( tedGetSelection( &dsOld, &sgOld, &sdOld,
				(DocumentTree **)0, (BufferItem **)0, ed ) )
	{ LDEB(1); return;	}

    /*  0  */
    if  ( sdOld.sdIsObjectSelection )
	{ tedHideObjectWindows( ed );	}

    bodySectNr= docDescribeSetSelection( &treeSet, &bodySectBiSet,
			    &drExternalSet,
			    &redrawTreeOld, &redrawTreeSet,
			    &lc, bd, &dsOld, dsSet );
    if  ( bodySectNr < 0 )
	{ LDEB(bodySectNr); return;	}

    /*  A  */
    dsNew= *dsSet;

    if  ( td->tdOverstrike			&&
	  docIsIBarSelection( &dsNew )		)
	{
	DocumentPosition	dp= dsNew.dsHead;
	const int		direction= 1;

	if  ( ! docGotoNextPosition( &dp )	&&
	      dp.dpNode == dsNew.dsHead.dpNode	)
	    {
	    docSetRangeSelection( &dsNew, &(dsSet->dsHead), &dp, direction );
	    }
	}

    tedBalanceFieldSelection( td->tdDocument, treeSet, &balanced,
					    &(dsNew.dsHead), &(dsNew.dsTail) );

    hadSelection= tedHasSelection( ed );
    if  ( hadSelection )
	{
	/*  1  */
	drExpose= sgOld.sgRectangle;

	if  ( redrawTreeOld )
	    { geoUnionRectangle( &drExpose, &drExpose, &(ed->edFullRect) ); }

	/*  2  */
	if  ( sdOld.sdIsIBarSelection )
	    { tedStopCursorBlink( ed ); }

	appDocExposeRectangle( ed, &drExpose, scrolledX, scrolledY );
	}

    /*  5  */
    if  ( dsNew.dsHead.dpNode->biParaTableNesting > 0		&&
	  dsNew.dsTail.dpNode->biParaTableNesting > 0		&&
	  dsNew.dsHead.dpNode->biParent->biParent ==
			dsNew.dsTail.dpNode->biParent->biParent	)
	{
	dsNew.dsCol0= dsNew.dsHead.dpNode->biParent->biNumberInParent;
	dsNew.dsCol1= dsNew.dsTail.dpNode->biParent->biNumberInParent;
	}

    /*  5  */
    if  ( ! bodySectBiSet )
	{ XDEB(bodySectBiSet);	}

    tedSelectionGeometry( &sgNew, &dsNew, bodySectBiSet, lastLine, &lc );
    td->tdVisibleSelectionCopied= 0;
    td->tdSelection= dsNew;
    td->tdSelectionGeometry= sgNew;

    treeSet->dtPageSelectedUpon= dsNew.dsHead.dpNode->biTopPosition.lpPage;
    treeSet->dtColumnSelectedIn= dsNew.dsHead.dpNode->biTopPosition.lpColumn;

    /*  3  */
    tedScrollToSelection( ed, &scrolledX, &scrolledY );

    if  ( hadSelection )
	{
	/*  4  */
	drExpose= sgOld.sgRectangle;

	if  ( redrawTreeOld )
	    { geoUnionRectangle( &drExpose, &drExpose, &(ed->edFullRect) ); }

	appDocExposeRectangle( ed, &drExpose, scrolledX, scrolledY );
	}

    /*  6  */
    drExpose= sgNew.sgRectangle;

    if  ( redrawTreeSet )
	{ geoUnionRectangle( &drExpose, &drExpose, &drExternalSet ); }

    appDocExposeRectangle( ed, &drExpose, scrolledX, scrolledY );

    tedDescribeSelection( ed );

    if  ( td->tdSelectionDescription.sdIsObjectSelection )
	{ tedMoveObjectWindows( ed );	}

    td->tdBodySectionNumber= bodySectNr;

    tedManagePrimarySelection( ed );

    if  ( pScrolledX )
	{ *pScrolledX= scrolledX;	}
    if  ( pScrolledY )
	{ *pScrolledY= scrolledY;	}

    return;
    }

void tedSetSelection(	EditDocument *			ed,
			const DocumentSelection *	dsSet,
			int				lastLine,
			int *				pScrolledX,
			int *				pScrolledY )
    {
    tedSetSelectionLow( ed, dsSet, lastLine, pScrolledX, pScrolledY );

    /* in tedSetSelectionLow(): tedDescribeSelection() */
    tedAdaptToolsToSelection( ed );

    return;
    }

/************************************************************************/
/*									*/
/*  Collect a description of the current selection.			*/
/*									*/
/*  Remember the text attribute of the beginning of the selection.	*/
/*									*/
/************************************************************************/

void tedDescribeSelection(	EditDocument *			ed )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;
    const IndexMapping *	a2s= &(td->tdAttributeToScreenFont);
    SelectionDescription *	sd= &(td->tdSelectionDescription);

    docDescribeSelection( sd, &(td->tdSelection), bd,
			td->tdSelectionGeometry.sgHead.pgTopPosition.lpPage,
			td->tdSelectionGeometry.sgTail.pgTopPosition.lpPage,
			ed->edDocumentId, ed->edIsReadonly );

    td->tdCurrentScreenFont= utilIndexMappingGet( a2s,
					    sd->sdTextAttributeNumber );

    if  ( td->tdCurrentScreenFont < 0 )
	{ LLDEB(sd->sdTextAttributeNumber,td->tdCurrentScreenFont); }
    }

void tedSetSelectedPosition(	EditDocument *			ed,
				const DocumentPosition *	dpSet,
				int				lastLine,
				int *				pScrolledX,
				int *				pScrolledY )
    {
    DocumentSelection		dsNew;

    docInitDocumentSelection( &dsNew );

    docSetIBarSelection( &dsNew, dpSet );

    tedSetSelection( ed, &dsNew, lastLine, pScrolledX, pScrolledY );

    return;
    }

/************************************************************************/
/*									*/
/*  Set an I Bar selection and cause the I Bar to be drawn.		*/
/*									*/
/************************************************************************/

int tedSetIBarSelection(		EditDocument *		ed,
					const DocumentPosition * dp,
					int			lastLine,
					int *			pScrolledX,
					int *			pScrolledY )
    {
    TedDocument *	td= (TedDocument *)ed->edPrivateData;

    td->tdVisibleSelectionCopied= 0;

    if  ( td->tdSelectionDescription.sdIsObjectSelection )
	{ tedHideObjectWindows( ed );	}

    tedSetSelectedPosition( ed, dp, lastLine, pScrolledX, pScrolledY );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Change the selection to cover whole paragraphs.			*/
/*									*/
/*  Depending on the 'direction' argument:				*/
/*	<  0:	The previous one.					*/
/*	== 0:	The current one.					*/
/*	>  0:	The next one.						*/
/*									*/
/************************************************************************/

int tedAppSelectWholeParagraph(	EditApplication *	ea,
				int			direction )
    {
    EditDocument *		ed= ea->eaCurrentDocument;

    DocumentSelection		dsNew;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    int				ret;

    const int			lastLine= 0;

    if  ( ! ed )
	{ XDEB(ed); return -1;	}

    if  ( tedGetSelection( &dsNew, &sg, &sd,
				(DocumentTree **)0, (BufferItem **)0, ed ) )
	{ LDEB(1); return -1;	}

    ret= docSelectWholeParagraph( &dsNew, direction );
    if  ( ret < 0 )
	{ LDEB(ret); return -1;	}
    if  ( ret > 0 )
	{ return -1;	}

    tedSetSelection( ed, &dsNew, lastLine, (int *)0, (int *)0 );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Change the selection to cover whole sections.			*/
/*									*/
/*  Depending on the 'direction' argument:				*/
/*	<  0:	The previous one.					*/
/*	== 0:	The current one.					*/
/*	>  0:	The next one.						*/
/*									*/
/************************************************************************/

int tedAppSelectWholeSection(	EditApplication *	ea,
				int			direction )
    {
    EditDocument *		ed= ea->eaCurrentDocument;

    DocumentSelection		dsNew;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    int				ret;

    int				scrolledX= 0;
    int				scrolledY= 0;
    const int			lastLine= 0;

    if  ( ! ed )
	{ XDEB(ed); return -1;	}

    if  ( tedGetSelection( &dsNew, &sg, &sd,
				(DocumentTree **)0, (BufferItem **)0, ed ) )
	{ LDEB(1); return -1;	}

    ret= docSelectWholeSection( &dsNew, direction );
    if  ( ret < 0 )
	{ LDEB(ret); return -1;	}
    if  ( ret > 0 )
	{ return -1;	}

    tedSetSelection( ed, &dsNew, lastLine, &scrolledX, &scrolledY );

    return 0;
    }

