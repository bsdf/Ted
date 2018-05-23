/************************************************************************/
/*									*/
/*  Ted: Handle user input.						*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	<docScreenLayout.h>
#   include	"tedApp.h"
#   include	"tedSelect.h"
#   include	"tedDocFront.h"
#   include	"tedLayout.h"
#   include	"tedDocument.h"
#   include	"tedToolFront.h"
#   include	<docField.h>
#   include	<docHyperlinkField.h>
#   include	<docTreeType.h>
#   include	<guiWidgetDrawingSurface.h>
#   include	<guiDrawingWidget.h>
#   include	<docParaParticules.h>

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
    }

#   endif

/************************************************************************/
/*									*/
/*  Handle mouse clicks.						*/
/*									*/
/************************************************************************/

typedef struct DraggingContext
    {
    int				dcMouseX;
    int				dcMouseY;
    DocumentPosition		dcAnchorPosition;
    BufferItem *		dcSelRootNode;
    EditDocument *		dcEd;
    DocumentTree *		dcTree;
    LayoutContext		dcLayoutContext;
    } DraggingContext;

/************************************************************************/
/*									*/
/*  Find the document position under the mouse and extend the selection	*/
/*  to that position.							*/
/*									*/
/************************************************************************/

static int tedExtendSelectionToXY( DraggingContext *		dc )
    {
    const LayoutContext *	lc= &(dc->dcLayoutContext);
    EditDocument *		ed= dc->dcEd;

    DocumentPosition		dpTo;
    PositionGeometry		pgTo;

    int				ox= ed->edVisibleRect.drX0;
    int				oy= ed->edVisibleRect.drY0;

    int				pageFound;
    int				columnFound;

    dpTo= dc->dcAnchorPosition;

    if  ( tedFindPosition( &dpTo, &pgTo, &pageFound, &columnFound,
			    dc->dcSelRootNode,
			    lc, dc->dcMouseX+ ox, dc->dcMouseY+ oy ) )
	{ /*LLDEB(mouseX,mouseY);*/ return 0; }

    if  ( tedExtendSelectionToPosition( ed, &(dc->dcAnchorPosition), &dpTo ) )
	{ return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Supposing te click is in a diffrent document tree, reformat that	*/
/*  tree for the context of the click to determine where the location	*/
/*  of the click in the document hierarchy.				*/
/*									*/
/************************************************************************/

static int tedLayoutSelectedRoot(	const LayoutContext *	lc,
					DocumentTree *		treeFound,
					BufferItem *		rootNodeFound,
					BufferItem *		bodySectBiSet,
					int			page,
					int			column )
    {
    DocumentPosition		dp;
    int				inHeadFoot;
    DocumentRectangle		drChanged;

    docInitDocumentPosition( &dp );

    inHeadFoot= rootNodeFound->biTreeType;

    switch( inHeadFoot )
	{
	case DOCinBODY:
	    break;

	case DOCinFOOTNOTE:
	case DOCinENDNOTE:
	    if  ( ! treeFound )
		{ XDEB(treeFound); return 1;	}
	    else{
		treeFound->dtPageSelectedUpon= page;
		treeFound->dtColumnSelectedIn= column;
		}
	    break;

	case DOCinFIRST_HEADER:
	case DOCinLEFT_HEADER:
	case DOCinRIGHT_HEADER:

	case DOCinFIRST_FOOTER:
	case DOCinLEFT_FOOTER:
	case DOCinRIGHT_FOOTER:

	    if  ( ! treeFound )
		{ XDEB(treeFound); return 1;	}
	    else{
		treeFound->dtPageSelectedUpon= page;
		treeFound->dtColumnSelectedIn= 0;

		if  ( treeFound->dtPageFormattedFor != page )
		    {
		    const int	column0= 0;
		    const int	adjustDocument= 0;

		    /*  We do not expect the tree to change height here	*/
		    if  ( docLayoutDocumentTree( treeFound, &drChanged,
				page, column0, treeFound->dtY0UsedTwips,
				bodySectBiSet, lc,
				docStartScreenLayoutForTree, adjustDocument ) )
			{ LDEB(page); return 1; }
		    }
		}
	    break;

	case DOCinFTNSEP:
	case DOCinFTNSEPC:
	case DOCinFTNCN:
	case DOCinAFTNSEP:
	case DOCinAFTNSEPC:
	case DOCinAFTNCN:

	    if  ( ! treeFound )
		{ XDEB(treeFound); return 1;	}
	    else{
		int			changed= 0;
		DocumentRectangle	drExternalSetX;
		BufferItem *		bodySectBiSetX;

		treeFound->dtPageSelectedUpon= page;
		treeFound->dtColumnSelectedIn= column;

		if  ( docCheckPageOfSelectedTree( &changed, &bodySectBiSetX,
				    &drExternalSetX, treeFound, lc,
				    docStartScreenLayoutForTree ) )
		    { LDEB(1);	}

		if  ( changed )
		    { geoUnionRectangle( &drChanged, &drChanged, &drExternalSetX ); }
		}
	    break;

	case DOCinSHPTXT:
	    if  ( treeFound )
		{
		treeFound->dtPageSelectedUpon= page;
		treeFound->dtColumnSelectedIn= column;
		}
	    break;

	default:
	    LDEB(rootNodeFound->biTreeType);

	    if  ( treeFound )
		{
		treeFound->dtPageSelectedUpon= page;
		treeFound->dtColumnSelectedIn= column;
		}
	    break;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Handle a multi-click.						*/
/*									*/
/************************************************************************/

static void tedHandleMultiClick(EditDocument *			ed,
				const DocumentSelection *	dsWord,
				const DocumentPosition *	dpClick,
				const PositionGeometry *	pgClick,
				int				seq )
    {
    DocumentSelection	dsNew;

    docInitDocumentSelection( &dsNew );

    docSetIBarSelection( &dsNew, dpClick );

    if  ( seq > 2 )
	{
	int		partLineHead;
	int		partLineTail;

	docLineSelection( &dsNew, &partLineHead, &partLineTail,
					    dpClick->dpNode, pgClick->pgLine );
	}
    else{ dsNew= *dsWord;	}

    /*  Caller describes and adapts tools */
    tedSetSelectionLow( ed, &dsNew, pgClick->pgPositionFlags & POSflagLINE_HEAD,
							(int *)0, (int *)0 );

    }

/************************************************************************/
/*									*/
/*  Select the object that we just clicked.				*/
/*									*/
/************************************************************************/

static int tedSelectObject(	EditDocument *			ed,
				const LayoutContext *		lc,
				const DocumentSelection *	dsWord,
				int				docX )
    {
    BufferDocument *		bd= lc->lcDocument;

    int				partObject;
    DocumentPosition		dpObject;
    InsertedObject *		io;

    if  ( ! docGetObjectSelection( dsWord, bd, &partObject, &dpObject, &io ) )
	{
	DocumentRectangle	drObject;
	PositionGeometry	pgObject;
	int			marg= 0;
	int			wide;

	tedPositionGeometry( &pgObject, &dpObject, PARAfindLAST, lc );

	tedGetObjectRectangle( &drObject, (Point2DI *)0,
						    io, &pgObject, lc, ed );

	wide= drObject.drX1- drObject.drX0+ 1;
	if  ( wide > 10 )
	    { marg++;	}
	if  ( wide > 20 )
	    { marg++;	}

	if  ( docX >= drObject.drX0+ marg	&&
	      docX <= drObject.drX1- marg	)
	    {
	    /*  Caller describes and adapts tools */
	    tedSetSelectionLow( ed, dsWord,
				pgObject.pgPositionFlags & POSflagLINE_HEAD,
				(int *)0, (int *)0 );
	    return 0;
	    }
	}

    return 1;
    }

/************************************************************************/
/*									*/
/*  Locate a mouse click.						*/
/*									*/
/************************************************************************/

static int tedFindMousePosition( APP_WIDGET			w,
				EditDocument *			ed,
				DraggingContext *		dc,
				const DocumentSelection *	dsOld,
				int *				pSameRoot,
				int *				pSameInstance,
				PositionGeometry *		pgClick,
				int *				pDocX,
				int *				pSeq,
				unsigned int *			pKeyState,
				APP_EVENT *			downEvent )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;

    int				ox= ed->edVisibleRect.drX0;
    int				oy= ed->edVisibleRect.drY0;

    int				mouseX;
    int				mouseY;
    int				docX;
    int				docY;

    int				sameRoot= 1;
    int				sameInstance= 1;

    int				button;
    int				upDown;
    int				seq;
    unsigned int		keyState= 0;

    int				page;
    int				column;
    BufferItem *		rootNodeFound;

    if  ( guiGetCoordinatesFromMouseButtonEvent( &mouseX, &mouseY,
					    &button, &upDown, &seq, &keyState,
					    w, downEvent ) )
	{ LDEB(1); return 1;	}

    /********************************************************************/
    /*  Cope with GTK that returns events for clicks in the ruler and	*/
    /*  in the menu bar.						*/
    /********************************************************************/
    if  ( mouseX < 0 || mouseY < 0 )
	{ return 1;	}

    if  ( mouseX < -ox )
	{ mouseX=  -ox;	}
    if  ( mouseY < -oy )
	{ mouseY=  -oy;	}

    dc->dcEd= ed;
    tedSetScreenLayoutContext( &(dc->dcLayoutContext), ed );
    dc->dcMouseX= mouseX;
    dc->dcMouseY= mouseY;

    docX= mouseX+ ox;
    docY= mouseY+ oy;

    dc->dcSelRootNode= bd->bdBody.dtRoot;
    dc->dcTree= &(bd->bdBody);

    if  ( tedFindRootForPosition( &(dc->dcAnchorPosition), pgClick,
					    &(dc->dcTree), &rootNodeFound,
					    &page, &column, ed, docX, docY ) )
	{ /*LLDEB(docX,docY);*/ return 1;	}

    dc->dcSelRootNode= rootNodeFound;

    sameRoot= docSelectionSameRoot( dsOld, rootNodeFound );
    if  ( sameRoot )
	{
	sameInstance= docSelectionSameInstance( dc->dcTree, page, column );
	}

    if  ( ! sameRoot || ! sameInstance )
	{
	SelectionScope	ss;
	BufferItem *	bodySectNode;

	docGetSelectionScope( &ss, rootNodeFound );
	bodySectNode= docGetBodySectNodeOfScope( &ss, bd );

	if  ( tedLayoutSelectedRoot( &(dc->dcLayoutContext),
		    dc->dcTree, rootNodeFound, bodySectNode, page, column ) )
	    { LDEB(page); return 1;	}

	guiExposeDrawingWidget( ed->edDocumentWidget.dwWidget );
	}

    *pSameRoot= sameRoot;
    *pSameInstance= sameInstance;
    *pDocX= docX;
    *pSeq= seq;
    *pKeyState= keyState;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Avoid readonly fields.						*/
/*									*/
/************************************************************************/

static void tedAvoidReadonlyField(	DraggingContext *	dc,
					PositionGeometry *	pgClick,
					int			docX )
    {
    DocumentField *		dfHead;
    DocumentField *		dfTail;
    int				headMoved= 0;
    int				tailMoved= 0;
    int				headPart= -1;
    int				tailPart= -1;

    DocumentPosition		dpHead= dc->dcAnchorPosition;
    DocumentPosition		dpTail= dc->dcAnchorPosition;

    docBalanceFieldSelection( &dfHead, &dfTail, &headMoved, &tailMoved,
					    &headPart, &tailPart,
					    &dpHead, &dpTail, dc->dcTree,
					    dc->dcLayoutContext.lcDocument );

    if  ( headMoved )
	{
	PositionGeometry	pg;

	tedPositionGeometry( &pg, &dpHead, PARAfindLAST, &(dc->dcLayoutContext) );

	if  ( docX <= pg.pgXPixels )
	    {
	    dc->dcAnchorPosition= dpHead;
	    *pgClick= pg;
	    return;
	    }
	}

    if  ( tailMoved )
	{
	PositionGeometry	pg;

	tedPositionGeometry( &pg, &dpTail, PARAfindFIRST, &(dc->dcLayoutContext) );

	if  ( docX >= pg.pgXPixels )
	    {
	    dc->dcAnchorPosition= dpTail;
	    *pgClick= pg;
	    return;
	    }
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Select the current position of the mouse.				*/
/*									*/
/************************************************************************/

static int tedSelectMousePosition(
				unsigned int *			pKeyState,
				APP_WIDGET			w,
				EditDocument *			ed,
				DraggingContext *		dc,
				APP_EVENT *			downEvent )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;

    int				docX;

    int				sameRoot= 1;
    int				sameInstance= 1;

    int				scrolledX= 0;
    int				scrolledY= 0;

    int				seq;
    unsigned int		keyState= 0;

    DocumentSelection		dsOld;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    PositionGeometry		pgClick;
    DocumentSelection		dsWord;
    int				wordIsObject= 0;

    if  ( tedGetSelection( &dsOld, &sg, &sd,
			    (DocumentTree **)0, (BufferItem **)0, ed ) )
	{ LDEB(1); return 1;	}

    if  ( tedFindMousePosition( w, ed, dc, &dsOld, &sameRoot, &sameInstance,
				&pgClick, &docX, &seq, &keyState, downEvent ) )
	{ return 1;	}

    tedAvoidReadonlyField( dc, &pgClick, docX );

    docWordSelection( &dsWord, &wordIsObject, &(dc->dcAnchorPosition) );

    if  ( seq > 1 )
	{
	tedHandleMultiClick( ed, &dsWord,
				    &(dc->dcAnchorPosition), &pgClick, seq );
	goto ready;
	}

    if  ( sameRoot && sameInstance && ( keyState & KEY_SHIFT_MASK ) )
	{
	dc->dcAnchorPosition= dsOld.dsAnchor;

	if  ( tedExtendSelectionToXY( dc ) )
	    { LDEB(1); return 1;	}

	goto ready;
	}

    docAvoidParaHeadField( &(dc->dcAnchorPosition), (int *)0, bd );

    if  ( wordIsObject							&&
	  ! tedSelectObject( ed, &(dc->dcLayoutContext),
						      &dsWord, docX )	)
	{ goto ready;	}

    if  ( ! docIsIBarSelection( &dsOld )				||
	  ! docSamePosition( &(dsOld.dsHead), &(dc->dcAnchorPosition) )	||
	  ! sameInstance						)
	{
	tedSetSelectedPosition( ed, &(dc->dcAnchorPosition),
				    pgClick.pgPositionFlags & POSflagLINE_HEAD,
				    &scrolledX, &scrolledY );
	}

  ready:

    tedDescribeSelection( ed );
    tedAdaptToolsToSelection( ed );

    *pKeyState= keyState;
    return 0;
    }

#   define	TED_DRAG_INTERVAL	(150L)

static APP_EVENT_HANDLER_H( tedInputDragMouseMove, w, vdc, event )
    {
    DraggingContext *	dc= (DraggingContext *)vdc;
    EditDocument *	ed= dc->dcEd;

    int			ox= ed->edVisibleRect.drX0;
    int			oy= ed->edVisibleRect.drY0;

    int			mouseX;
    int			mouseY;

    if  ( guiGetCoordinatesFromMouseMoveEvent( &mouseX, &mouseY, w, event ) )
	{ return;	}

    if  ( mouseX < -ox )
	{ mouseX=  -ox;	}
    if  ( mouseY < -oy )
	{ mouseY=  -oy;	}

    if  ( dc->dcMouseX != mouseX || dc->dcMouseY != mouseY )
	{
	dc->dcMouseX= mouseX;
	dc->dcMouseY= mouseY;

	if  ( tedExtendSelectionToXY( dc ) )
	    { LDEB(1); return;	}
	}

    return;
    }

static APP_TIMER_HANDLER( tedTick, voiddc )
    {
    DraggingContext *		dc= (DraggingContext *)voiddc;
    EditDocument *		ed= dc->dcEd;

    int				ox= ed->edVisibleRect.drX0;
    int				oy= ed->edVisibleRect.drY0;

    int				mouseX;
    int				mouseY;

    int				scrolledX= 0;
    int				scrolledY= 0;

    DocumentRectangle		drMouse;

    guiGetCoordinatesRelativeToWidget( &mouseX, &mouseY,
					    ed->edDocumentWidget.dwWidget );

    if  ( mouseX < -ox )
	{ mouseX=  -ox;	}
    if  ( mouseY < -oy )
	{ mouseY=  -oy;	}
    if  ( mouseX > ed->edFullRect.drX1- ox )
	{ mouseX=  ed->edFullRect.drX1- ox;	}
    if  ( mouseY > ed->edFullRect.drY1- oy )
	{ mouseY=  ed->edFullRect.drY1- oy;	}

    drMouse.drX0= mouseX+ ox;
    drMouse.drX1= mouseX+ ox;
    drMouse.drY0= mouseY+ oy;
    drMouse.drY1= mouseY+ oy;

    appScrollToRectangle( ed, &drMouse, &scrolledX, &scrolledY );

    if  ( scrolledX || scrolledY )
	{
	if  ( tedExtendSelectionToXY( dc ) )
	    { LDEB(1);	}
	}

#   ifdef USE_MOTIF
    return;
#   endif
#   ifdef USE_GTK
    return 0;
#   endif
    }

static void tedButton1Pressed(	APP_WIDGET			w,
				EditDocument *			ed,
				APP_EVENT *			downEvent )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;

    DraggingContext		dc;
    unsigned int		keyState;

    int				startedWithObject;

    HyperlinkField		hf;

    docInitHyperlinkField( &hf );

    if  ( tedSelectMousePosition( &keyState, w, ed, &dc, downEvent ) )
	{ goto ready;	}

    startedWithObject= td->tdSelectionDescription.sdIsObjectSelection;

    if  ( ed->edFileReadOnly || ( keyState & KEY_CONTROL_MASK ) )
	{
	DocumentField *		dfHyperlink;

	dfHyperlink= docFindTypedFieldForPosition( bd,
				&(dc.dcAnchorPosition), DOCfkHYPERLINK, 0 );

	if  ( dfHyperlink )
	    {
	    if  ( ! docGetHyperlinkField( &hf, dfHyperlink ) )
		{
		tedDocFollowLink( (APP_WIDGET)0, ed,
					    &(hf.hfFile), &(hf.hfBookmark) );
		goto ready;
		}
	    }
	}

    appRunDragLoop( w, ed->edApplication, downEvent,
				(APP_EVENT_HANDLER_T)0, tedInputDragMouseMove,
				TED_DRAG_INTERVAL, tedTick, (void *)&dc );

    tedDescribeSelection( ed );
    tedAdaptToolsToSelection( ed );

    if  ( startedWithObject					&&
	  ! td->tdSelectionDescription.sdIsObjectSelection	)
	{ tedHideObjectWindows( ed );	}

  ready:

    docCleanHyperlinkField( &hf );

    return;
    }

/************************************************************************/
/*									*/
/*  Handle mouse button down events for the document widget.		*/
/*									*/
/************************************************************************/

static void tedButton3Pressed(	APP_WIDGET			w,
				EditDocument *			ed,
				APP_EVENT *			downEvent )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    int				ox= ed->edVisibleRect.drX0;
    int				oy= ed->edVisibleRect.drY0;

    int				mouseX;
    int				mouseY;
    int				docX;
    int				docY;

    int				otherRoot= 0;

    int				button;
    int				upDown;
    int				seq;
    unsigned int		keyState= 0;

    DraggingContext		dc;

    int				selectMousePosition= 0;

    DocumentSelection		dsOld;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    int				page;
    int				column;
    DocumentTree *		treeFound= (DocumentTree *)0;
    BufferItem *		rootNodeFound;

    LayoutContext		lc;
    DocumentPosition		dpClick;

    layoutInitContext( &lc );
    tedSetScreenLayoutContext( &lc, ed );

    if  ( tedGetSelection( &dsOld, &sg, &sd,
				(DocumentTree **)0, (BufferItem **)0, ed ) )
	{ LDEB(1); return;	}

    if  ( guiGetCoordinatesFromMouseButtonEvent( &mouseX, &mouseY,
					    &button, &upDown, &seq, &keyState,
					    w, downEvent ) )
	{ LDEB(1); return;	}

    if  ( mouseX < -ox )
	{ mouseX=  -ox;	}
    if  ( mouseY < -oy )
	{ mouseY=  -oy;	}

    docX= mouseX+ ox;
    docY= mouseY+ oy;

    if  ( tedFindRootForPosition( &dpClick, (PositionGeometry *)0,
					    &treeFound, &rootNodeFound,
					    &page, &column, ed, docX, docY ) )
	{ LLDEB(docX,docY); return;	}

    otherRoot= ! docSelectionSameRoot( &dsOld, rootNodeFound );

    if  ( otherRoot )
	{ selectMousePosition= 1;	}
    else{
	int			cmp1;
	int			cmp2;

	cmp1= docComparePositions( &dpClick, &(dsOld.dsHead) );
	cmp2= docComparePositions( &dpClick, &(dsOld.dsTail) );

	if  ( cmp1 < 0 || cmp2 > 0 )
	    { selectMousePosition= 1;	}
	}

    if  ( selectMousePosition )
	{
	if  ( tedSelectMousePosition( &keyState, w, ed, &dc, downEvent ) )
	    { return;	}
	}

    tedShowFormatTool( td->tdToolsFormatToolOption, ed->edApplication );

    tedAdaptFormatToolToDocument( ed, 1 );

    return;
    }

APP_EVENT_HANDLER_H( tedMouseButtonPressed, w, voided, downEvent )
    {
    EditDocument *	ed= (EditDocument *)voided;
    int			button= MOUSE_BUTTON_FROM_EVENT( downEvent );

    tedStopCursorBlink( ed );

    switch( button )
	{
	case MOUSE_BUTTON_1:
#	    ifdef USE_MOTIF
	    if  ( ! ed->edFileReadOnly		&&
		  downEvent->xbutton.subwindow	)
		{
		int	res= tedObjectDrag( w, ed, downEvent );

		if  ( res < 1 )
		    { return;	}
		}
#	    endif

#	    ifdef USE_GTK
	    if  ( ! ed->edFileReadOnly )
		{
		int	res= tedObjectDrag( w, ed, downEvent );

		if  ( res < 1 )
		    { return;	}
		}
#	    endif

	    tedButton1Pressed( w, ed, downEvent );
	    break;

	case MOUSE_BUTTON_2:
	    appDocAskForPaste( ed, "PRIMARY" );
	    break;

	case MOUSE_BUTTON_3:
	    tedButton3Pressed( w, ed, downEvent );
	    break;

#	ifdef USE_MOTIF
	case Button4:
	case Button5:
	    break;
#	endif

#	ifdef USE_GTK
#	if GTK_MAJOR_VERSION < 2

	case SCROLL_UP:
	    appMouseWheelUp( ed );
	    break;

	case SCROLL_DOWN:
	    appMouseWheelDown( ed );
	    break;

#	endif
#	endif

	default:
	    LDEB(button);
	    break;
	}

    if  ( tedHasIBarSelection( ed ) )
	{ tedStartCursorBlink( ed );	}

    return;
    }

