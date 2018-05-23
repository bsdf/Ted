/************************************************************************/
/*									*/
/*  Ted: Handle user input.						*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	"tedApp.h"
#   include	"tedLayout.h"
#   include	"tedSelect.h"
#   include	"tedDocument.h"
#   include	"tedDocFront.h"
#   include	"tedEdit.h"
#   include	<docSelectLayout.h>
#   include	<docTreeType.h>
#   include	<docParaParticules.h>
#   include	<docNodeTree.h>
#   include	<docEditCommand.h>

#   include	<appGuiKeys.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Just log events that pass by for debugging purposes.		*/
/*									*/
/*  NOTE the silly constuction to do away with the 'unused' compiler	*/
/*	 warning.							*/
/*									*/
/************************************************************************/

#   ifdef USE_MOTIF_AND_NEEDED

static void tedLogEvent(	Widget		w,
				void *		voided,
				XEvent *	event,
				Boolean *	pRefused )
    {
    EditDocument *		ed= (EditDocument *)voided;

    appDebug( "EVENT \"%s\": %s\n",
			ed->edTitle, APP_X11EventNames[event->type] );

    *pRefused= 1;

    if  ( ! event )
	{ return;	}
    if  ( ! event )
	{ tedLogEvent( w, voided, event, pRefused );	}
    }

#   endif

/************************************************************************/
/*									*/
/*  Handle keyboard input.						*/
/*									*/
/*  a)  Handle miscelaneous keysyms as keysyms, even if they have a	*/
/*	string representation.						*/
/*									*/
/************************************************************************/

static void tedInputSetSelectedPosition(
				EditDocument *			ed,
				const DocumentPosition *	dp,
				int				lastLine )
    {
    int			scrolledX= 0;
    int			scrolledY= 0;

    tedSetSelectedPosition( ed, dp, lastLine, &scrolledX, &scrolledY );

    return;
    }

static void tedInputChangeSelection(
				EditDocument *			ed,
				unsigned int			keyState,
				const DocumentSelection *	dsOrig,
				const DocumentPosition *	dpSet,
				int				lastLine )
    {
    if  ( keyState & KEY_SHIFT_MASK )
	{
	tedExtendSelectionToPosition( ed, &(dsOrig->dsAnchor), dpSet );

	/* tedExtendSelectionToPosition() updates the selection description */

	tedAdaptToolsToSelection( ed );
	return;
	}

    tedInputSetSelectedPosition( ed, dpSet, lastLine );

    return;
    }

/************************************************************************/
/*									*/
/*  Delete a selection as the result of a BackSpace or Delete key with	*/
/*  an IBar selection.							*/
/*									*/
/************************************************************************/

static int tedInputSetIBarDelete( EditDocument *		ed,
				int				headAtLineHead,
				const DocumentPosition *	dpHead,
				const DocumentPosition *	dpTail )
    {
    DocumentSelection	ds;

    docSetRangeSelection( &ds, dpHead, dpTail, 1 );

    if  ( ! docSelectionInsideParagraph( &ds ) )
	{
	docTailPosition( &(ds.dsHead), ds.dsHead.dpNode );
	docHeadPosition( &(ds.dsTail), ds.dsTail.dpNode );
	}

    tedSetSelectionLow( ed, &ds, headAtLineHead, (int *)0, (int *)0 );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Navigation based on a Right arrow key.				*/
/*									*/
/************************************************************************/

static int tedMoveRightOnKey(	DocumentPosition *	dpNew,
				BufferDocument *	bd,
				int			keyState )
    {
    if  ( keyState & KEY_CONTROL_MASK )
	{
	if  ( docNextWord( dpNew ) )
	    { return -1;	}
	}
    else{
	if  ( docGotoNextPosition( dpNew ) )
	    { return -1;	}
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Navigation based on a Left arrow key.				*/
/*									*/
/************************************************************************/

static int tedMoveLeftOnKey(	DocumentPosition *	dpNew,
				BufferDocument *	bd,
				int			keyState )
    {
    if  ( keyState & KEY_CONTROL_MASK )
	{
	if  ( docPrevWord( dpNew ) )
	    { return -1;	}
	}
    else{
	if  ( docGotoPrevPosition( dpNew ) )
	    { return -1;	}
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Start a modification.						*/
/*									*/
/*  1)  Check if editing is possible and allowed			*/
/*  2)  Stop the blinking cursor.					*/
/*									*/
/************************************************************************/

static int tedStartEdit(	EditDocument *		ed )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    if  ( ! tedHasSelection( ed ) || ! td->tdSelectionDescription.sdCanReplace )
	{
#	ifdef NOT_USE_MOTIF
	const AppDrawingData *	add= &(ed->edDocumentWidget.dwDrawingData);

	XBell( add->addDisplay, 0 );
#	endif

#	ifdef USE_GTK
	gdk_beep();
#	endif

	return 1;
	}

    tedStopCursorBlink( ed );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Handle a key stroke. (Special keys only: Keys that result in text	*/
/*  are not handled here.						*/
/*									*/
/************************************************************************/

void tedDocGotKey(	void *			voided,
			int			keySym,
			unsigned int		state )
    {
    EditDocument *		ed= (EditDocument *)voided;

    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;

    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    DocumentTree *		ei;
    BufferItem *		bodySectNode;

    const int			traced= td->tdTraced;

    int				res;

    LayoutContext		lc;
    int				headAtLineHead;
    int				headAtLineTail;
    int				headAfterBreak;

    int				deleteCommand= EDITcmdDELETE_SELECTION;

    layoutInitContext( &lc );
    tedSetScreenLayoutContext( &lc, ed );

    if  ( tedStartEdit( ed ) )
	{ goto ready;	}

    if  ( tedGetSelection( &ds, &sg, &sd, &ei, &bodySectNode, ed ) )
	{ LDEB(1); goto ready;	}

    headAtLineHead= ( sg.sgHead.pgPositionFlags & POSflagLINE_HEAD ) != 0;
    headAtLineTail= ( sg.sgHead.pgPositionFlags & POSflagLINE_TAIL ) != 0;
    headAfterBreak= ( sg.sgHead.pgPositionFlags & POSflagPART_AFTER_BREAK ) != 0;
    /*
    tailAtLineHead= ( sg.sgTail.pgPositionFlags & POSflagLINE_HEAD ) != 0;
    */


    /* only interested in these 2 */
    state= state & ( KEY_CONTROL_MASK|KEY_SHIFT_MASK);

    switch( keySym )
	{
	DocumentPosition		dpNew;
	const PositionGeometry *	pgRef;

#	ifdef KEY_ISO_Left_Tab
	case  KEY_ISO_Left_Tab:
	    if  ( ds.dsHead.dpNode				&&
		  ds.dsHead.dpNode->biParaTableNesting > 0	&&
		  ! ( state & KEY_CONTROL_MASK )	)
		{ goto shiftTab;	}
	    else{ goto ready;		}
#	endif

	case KEY_i:
	    if  ( state != KEY_CONTROL_MASK )
		{ goto unknown; }
	    /*FALLTHROUGH*/
	case KEY_Tab:
	    if  ( ds.dsHead.dpNode				&&
		  ds.dsHead.dpNode->biParaTableNesting > 0	&&
		  ! ( state & KEY_CONTROL_MASK )	)
		{
		if  ( state & KEY_SHIFT_MASK )
		    {
		  shiftTab:
		    if  ( docGotoFirstPosition( &dpNew,
				    ds.dsHead.dpNode->biParent )	||
			  docGotoPrevPosition( &dpNew )			)
			{ goto ready;	}
		    }
		else{
		    if  ( docGotoLastPosition( &dpNew,
				    ds.dsHead.dpNode->biParent )	||
			  docGotoNextPosition( &dpNew )			)
			{ goto ready;	}
		    }

		{
		    const int	lastLine= 0;

		    tedInputSetSelectedPosition( ed, &dpNew, lastLine );
		}

		goto ready;
		}

	    if  ( state & KEY_SHIFT_MASK )
		{ goto ready;	}

	    if  ( ! sd.sdCanReplace )
		{ goto ready;	}

	    {
	    const int		redoLayout= 0;

	    tedEditInsertSpecialParticule( ed,
			    DOCkindTAB, EDITcmdREPLACE, redoLayout, traced );
	    }

	    goto ready;

	case KEY_j: case KEY_m:
	    if  ( state != KEY_CONTROL_MASK )
		{ goto unknown; }
	    /*FALLTHROUGH*/
	case KEY_KP_Enter:
	case KEY_Return:
	    if  ( ! sd.sdCanReplace )
		{ goto ready;	}

	    tedDocSplitParagraph( ed,
			    state == KEY_CONTROL_MASK	&&
			    keySym != KEY_j		&&
			    keySym != KEY_m,
			    traced );
	    goto ready;

	case KEY_KP_Delete:
	case KEY_Delete:

	    res= tedDeleteTableSliceSelection( ed, traced );
	    if  ( res < 0 )
		{ LDEB(res); goto ready;	}
	    if  ( res == 0 )
		{ goto ready;		}

	    if  ( ! sd.sdCanReplace )
		{ goto ready;	}

	    if  ( sd.sdIsIBarSelection )
		{
		dpNew= ds.dsTail;

		if  ( tedMoveRightOnKey( &dpNew, bd, state ) )
		    { goto ready;	}

		if  ( ( ds.dsHead.dpNode->biParaTableNesting > 0	||
		        dpNew.dpNode->biParaTableNesting > 0	)	&&
		      ! docPositionsInsideCell( &(ds.dsHead), &dpNew ) )
		    {
		    const int		keyState= 0;
		    const int		lastLine= 1;

		    tedInputChangeSelection( ed, keyState,
						    &ds, &dpNew, lastLine );
		    goto ready;
		    }

		tedInputSetIBarDelete( ed, headAtLineHead,
						    &(ds.dsHead), &dpNew );

		deleteCommand= EDITcmdDELETE_SELECTION_DEL;
		}

	    tedDocDeleteSelection( ed, deleteCommand, traced );

	    goto ready;

	case KEY_BackSpace:

	    res= tedDeleteTableSliceSelection( ed, traced );
	    if  ( res < 0 )
		{ LDEB(res); goto ready;	}
	    if  ( res == 0 )
		{ goto ready;		}

	    if  ( ! sd.sdCanReplace )
		{ goto ready;	}

	    if  ( sd.sdIsIBarSelection )
		{
		dpNew= ds.dsHead;

		if  ( tedMoveLeftOnKey( &dpNew, bd, state ) )
		    { goto ready;	}

		if  ( ( dpNew.dpNode->biParaTableNesting > 0		||
		        ds.dsTail.dpNode->biParaTableNesting > 0	) &&
		      ! docPositionsInsideCell( &dpNew, &(ds.dsTail) )	)
		    {
		    const int		keyState= 0;
		    const int		lastLine= 1;

		    tedInputChangeSelection( ed, keyState,
						    &ds, &dpNew, lastLine );
		    goto ready;
		    }

		tedInputSetIBarDelete( ed, headAtLineHead,
							&dpNew, &(ds.dsTail) );

		deleteCommand= EDITcmdDELETE_SELECTION_BS;
		}

	    tedDocDeleteSelection( ed, deleteCommand, traced );

	    goto ready;

	case KEY_KP_Home:
	case KEY_Home:
	    if  ( ( state & KEY_CONTROL_MASK ) )
		{
		if  ( docGotoFirstPosition( &dpNew, bd->bdBody.dtRoot ) )
		    { goto ready;	}
		}
	    else{
		if  ( ( state & KEY_SHIFT_MASK ) && ds.dsDirection >= 0 )
		    {
		    dpNew= ds.dsTail;

		    if  ( docLineHead( &dpNew, sg.sgTail.pgPositionFlags ) )
			{ goto ready;	}
		    }
		else{
		    dpNew= ds.dsHead;

		    if  ( docLineHead( &dpNew, sg.sgHead.pgPositionFlags ) )
			{ goto ready;	}
		    }
		}

	    docAvoidParaHeadField( &dpNew, (int *)0, bd );

	    {
	    const int	lastLine= 1;

	    tedInputChangeSelection( ed, state, &ds, &dpNew, lastLine );
	    }
	    goto ready;

	case KEY_KP_End:
	case KEY_End:
	    if  ( ( state & KEY_CONTROL_MASK ) )
		{
		if  ( docGotoLastPosition( &dpNew, bd->bdBody.dtRoot ) )
		    { goto ready;	}
		}
	    else{
		if  ( ! ( state & KEY_SHIFT_MASK ) || ds.dsDirection >= 0 )
		    {
		    dpNew= ds.dsTail;

		    if  ( docLineTail( &dpNew, sg.sgHead.pgPositionFlags ) )
			{ goto ready;	}
		    }
		else{
		    dpNew= ds.dsHead;

		    if  ( docLineTail( &dpNew, sg.sgTail.pgPositionFlags ) )
			{ goto ready;	}
		    }
		}

	    {
	    const int	lastLine= 0;

	    tedInputChangeSelection( ed, state, &ds, &dpNew, lastLine );
	    }
	    goto ready;

	case KEY_KP_Left:
	case KEY_Left:
	    if  ( ! state			&&
		  sd.sdIsIBarSelection		&&
		  ds.dsHead.dpStroff > 0	&&
		  headAtLineHead		&&
		  ! headAfterBreak		&&
		  ! headAtLineTail		)
		{
		const int	lastLine= 0;

		/*  To same position on previous line */
		tedInputChangeSelection( ed, state,
						&ds, &(ds.dsHead), lastLine );
		goto ready;
		}

	    if  ( ( state & KEY_SHIFT_MASK ) && ds.dsDirection >= 0 )
		{ dpNew= ds.dsTail;	}
	    else{ dpNew= ds.dsHead;	}

	    if  ( state != 0		||
		  ! sd.sdIsListBullet	||
		  sd.sdIsIBarSelection	)
		{
		if  ( tedMoveLeftOnKey( &dpNew, bd, state ) )
		    { goto ready;	}
		}

	    {
	    const int	lastLine= 1;

	    tedInputChangeSelection( ed, state, &ds, &dpNew, lastLine );
	    }
	    goto ready;

	case KEY_KP_Right:
	case KEY_Right:
	    if  ( ! state						&&
		  sd.sdIsIBarSelection					&&
		  ds.dsHead.dpStroff < docParaStrlen( ds.dsHead.dpNode ) &&
		  headAtLineTail				&&
		  ! headAtLineHead				)
		{
		const int	lastLine= 1;

		/*  To same position on next line */
		tedInputChangeSelection( ed, state,
						&ds, &ds.dsHead, lastLine );
		goto ready;
		}

	    if  ( ! ( state & KEY_SHIFT_MASK ) || ds.dsDirection >= 0 )
		{ dpNew= ds.dsTail;	}
	    else{ dpNew= ds.dsHead;	}

	    if  ( state != 0 || sd.sdIsIBarSelection )
		{
		if  ( tedMoveRightOnKey( &dpNew, bd, state ) )
		    { goto ready;	}
		}

	    {
	    const int	lastLine= 0;

	    tedInputChangeSelection( ed, state, &ds, &dpNew, lastLine );
	    }
	    goto ready;

	case KEY_KP_Up:
	case KEY_Up:
	    if  ( ! ( state & KEY_SHIFT_MASK ) || ds.dsDirection >= 0 )
		{ pgRef= &(sg.sgTail); dpNew= ds.dsTail;	}
	    else{ pgRef= &(sg.sgHead); dpNew= ds.dsHead;	}

	    if  ( state & KEY_CONTROL_MASK )
		{
		if  ( dpNew.dpStroff == 0 )
		    {
		    dpNew.dpNode= docPrevParagraph( dpNew.dpNode );
		    if  ( ! dpNew.dpNode )
			{ goto ready;	}

		    if  ( docGotoFirstPosition( &dpNew, dpNew.dpNode ) )
			{ goto ready;	}
		    }
		else{
		    if  ( docGotoFirstPosition( &dpNew, dpNew.dpNode ) )
			{ goto ready;	}
		    }
		}
	    else{
		if  ( tedArrowUp( &dpNew, pgRef, &lc ) )
		    { goto ready;	}
		}

	    {
	    const int	lastLine= headAtLineHead;

	    tedInputChangeSelection( ed, state, &ds, &dpNew, lastLine );
	    }
	    goto ready;

	case KEY_KP_Down:
	case KEY_Down:
	    if  ( ! ( state & KEY_SHIFT_MASK ) || ds.dsDirection >= 0 )
		{ pgRef= &(sg.sgTail); dpNew= ds.dsTail;	}
	    else{ pgRef= &(sg.sgHead); dpNew= ds.dsHead;	}

	    if  ( state & KEY_CONTROL_MASK )
		{
		if  ( dpNew.dpStroff < docParaStrlen( dpNew.dpNode )	&&
		      ( state & KEY_SHIFT_MASK )			)
		    {
		    if  ( docGotoLastPosition( &dpNew, dpNew.dpNode ) )
			{ goto ready;	}
		    }
		else{
		    dpNew.dpNode= docNextParagraph( dpNew.dpNode );
		    if  ( ! dpNew.dpNode )
			{ goto ready;	}

		    if  ( docGotoFirstPosition( &dpNew, dpNew.dpNode ) )
			{ goto ready;	}
		    }
		}
	    else{
		if  ( tedArrowDown( &dpNew, pgRef, &lc ) )
		    { goto ready;	}
		}

	    {
	    const int	lastLine= headAtLineHead;

	    tedInputChangeSelection( ed, state, &ds, &dpNew, lastLine );
	    }
	    goto ready;

	case KEY_KP_Prior:
	case KEY_Prior:
	    if  ( ( state & KEY_SHIFT_MASK ) && ds.dsDirection >= 0 )
		{ dpNew= ds.dsTail;	}
	    else{ dpNew= ds.dsHead;	}

	    if  ( dpNew.dpNode->biTreeType != DOCinBODY )
		{
		int		page= -1;
		int		column= -1;

		if  ( ! docPrevSimilarRoot( &dpNew, &page, &column, bd ) )
		    {
		    const int		lastLine= 0;
		    DocumentTree *	dtPrev= (DocumentTree *)0;
		    BufferItem *	bodySectNode;

		    if  ( docGetTreeOfNode( &dtPrev, &bodySectNode,
							bd, dpNew.dpNode ) )
			{ LDEB(1); goto ready;	}

		    if  ( dtPrev && dtPrev->dtPageSelectedUpon < 0 && page >= 0 )
			{
			dtPrev->dtPageSelectedUpon= page;
			dtPrev->dtColumnSelectedIn= column;
			}

		    docAvoidParaHeadField( &dpNew, (int *)0, bd );

		    tedInputSetSelectedPosition( ed, &dpNew, lastLine );
		    }

		goto ready;
		}

	    if  ( docGotoPrevPosition( &dpNew )				&&
		  docGotoFirstPosition( &dpNew, bd->bdBody.dtRoot )	)
		{ goto ready;	}

	    {
		PositionGeometry	pg;
		const int		lastLine= 1;
		int			partNew;
		int			lineNew;

		int			page;

		tedPositionGeometry( &pg, &dpNew, PARAfindFIRST, &lc );

		page= pg.pgTopPosition.lpPage;
		while( page >= 0 )
		    {
		    int		res;

		    res= docGetTopOfColumn( &dpNew, &lineNew, &partNew,
								bd, page, 0 );
		    if  ( res < 0 )
			{ LDEB(res); goto ready;	}
		    if  ( res == 0 )
			{
			tedInputChangeSelection( ed, state,
						    &ds, &dpNew, lastLine );
			goto ready;
			}

		    page--;
		    }
	    }
	    goto ready;

	case KEY_KP_Next:
	case KEY_Next:
	    if  ( ! ( state & KEY_SHIFT_MASK ) || ds.dsDirection >= 0 )
		{ dpNew= ds.dsTail;	}
	    else{ dpNew= ds.dsHead;	}

	    if  ( dpNew.dpNode->biTreeType != DOCinBODY )
		{
		int		page= -1;
		int		column= -1;

		if  ( ! docNextSimilarRoot( &dpNew, &page, &column, bd ) )
		    {
		    const int		lastLine= 0;
		    DocumentTree *	dtNext= (DocumentTree *)0;
		    BufferItem *	bodySectNode;

		    if  ( docGetTreeOfNode( &dtNext, &bodySectNode,
							bd, dpNew.dpNode ) )
			{ LDEB(1); goto ready;	}

		    if  ( dtNext && dtNext->dtPageSelectedUpon < 0 && page >= 0 )
			{
			dtNext->dtPageSelectedUpon= page;
			dtNext->dtColumnSelectedIn= column;
			}

		    docAvoidParaHeadField( &dpNew, (int *)0, bd );

		    tedInputSetSelectedPosition( ed, &dpNew, lastLine );
		    }

		goto ready;
		}

	    {
		PositionGeometry	pg;
		const int		lastLine= 1;
		int			partNew;
		int			lineNew;

		int			page;
		BufferItem *		bodyNode= bd->bdBody.dtRoot;

		tedPositionGeometry( &pg, &dpNew, PARAfindLAST, &lc );

		page= pg.pgTopPosition.lpPage+ 1;
		while( page <= bodyNode->biBelowPosition.lpPage )
		    {
		    int		res;

		    res= docGetTopOfColumn( &dpNew, &lineNew, &partNew,
								bd, page, 0 );
		    if  ( res < 0 )
			{ LDEB(res); goto ready;	}
		    if  ( res == 0 )
			{
			tedInputChangeSelection( ed, state, &ds, &dpNew, lastLine );
			goto ready;
			}

		    page++;
		    }

		page= bodyNode->biBelowPosition.lpPage;
		res= docGetBottomOfColumn( &dpNew, &partNew, bd, page, 0 );
		if  ( res == 0 )
		    {
		    tedInputChangeSelection( ed, state, &ds, &dpNew, lastLine );
		    goto ready;
		    }
	    }
	    goto ready;

	case KEY_Insert:
	case KEY_KP_Insert:
	    td->tdOverstrike= ! td->tdOverstrike;

	    if  ( tedHasIBarSelection( ed ) )
		{
		tedSetSelectionLow( ed, &ds, headAtLineHead,
							(int *)0, (int *)0 );
		}
	    break;

#	if 0
	case KEY_y:
	    docListNode(0,ds.dsHead.dpNode);
	    break;
#	endif

	case KEY_Shift_L:
	case KEY_Shift_R:
	case KEY_Alt_L:
	case KEY_Alt_R:
	case KEY_Control_L:
	case KEY_Control_R:
	case KEY_Caps_Lock:
	case KEY_Num_Lock:
#	ifdef XK_ISO_Level3_Shift
	case XK_ISO_Level3_Shift:
#	endif

	    goto ready;

	default: unknown:
#	    ifdef USE_GTK
#		if GTK_MAJOR_VERSION < 2
		gtk_accel_group_activate( ed->edToplevel.atAccelGroup,
							    keySym, state );
#		else
		gtk_accel_groups_activate(
			    G_OBJECT( ed->edToplevel.atAccelGroup ),
							    keySym, state );
#		endif
#	    endif
	    goto ready;
	}

  ready:

    if  ( tedHasIBarSelection( ed ) )
	{ tedStartCursorBlink( ed );	}

    return;
    }

void tedDocGotString(		void *			voided,
				const char *		str,
				int			length )
    {
    EditDocument *		ed= (EditDocument *)voided;

    if  ( tedStartEdit( ed ) )
	{ goto ready;	}

    if  ( tedDocReplaceSelectionTyping( ed, str, length ) )
	{ LDEB(length); goto ready;	}

  ready:

    if  ( tedHasIBarSelection( ed ) )
	{ tedStartCursorBlink( ed );	}
    }

APP_EVENT_HANDLER_H( tedScrollEventHandler, w, voided, scrollEvent )
    {
    EditDocument *	ed= (EditDocument *)voided;
    int			direction= SCROLL_DIRECTION_FROM_EVENT( scrollEvent );


    tedStopCursorBlink( ed );

    switch( direction )
	{
#	ifdef USE_MOTIF
	case Button1:
	case Button2:
	case Button3:
	    break;
#	endif

	case SCROLL_UP:
	    appMouseWheelUp( ed );
	    break;

	case SCROLL_DOWN:
	    appMouseWheelDown( ed );
	    break;

	default:
	    LLDEB(scrollEvent->type,direction);
	    break;
	}

    if  ( tedHasIBarSelection( ed ) )
	{ tedStartCursorBlink( ed );	}

    return;
    }

