#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>

#   include	"tedRuler.h"
#   include	"tedDrawRuler.h"
#   include	<appMetricRuler.h>
#   include	<appFrame.h>
#   include	<docBuf.h>
#   include	<geoGrid.h>
#   include	<guiWidgetDrawingSurface.h>
#   include	<guiDrawingWidget.h>

#   include	<appDebugon.h>

#   define	SHOW_SCROLLS		0

typedef struct TopRulerDrag
    {
    int			trdValue;
    int *		trdV0Pointer;
    int *		trdV1Pointer;
    int *		trdYPointer;
    TedTopRuler *	trdTtr;
    APP_WIDGET		trdWidget;
    int			trdOc;
    int			trdXmin;
    int			trdXmax;
    EditDocument *	trdDocument;
    } TopRulerDrag;

/************************************************************************/
/*									*/
/*  Make the top ruler for a document widget.				*/
/*									*/
/************************************************************************/

void * tedMakeTopRuler(	int				height,
			double				magnifiedPixelsPerTwip,
			double				magnification,
			const PostScriptFontList *	psfl,

			int				leftRulerWidth,
			int				rightRulerWidth,

			int				documentC0,
			int				documentC1,
			int				marginC0,
			int				marginC1,
			int				visibleC0,
			int				visibleC1,

			int				whatUnit )
    {
    TedTopRuler *		ttr;
    RulerData *			rd;

    ttr= (TedTopRuler *)malloc( sizeof( TedTopRuler ) );
    if  ( ! ttr )
	{ XDEB(ttr); return (void *)0;	}
    rd= &(ttr->ttrRulerData);

    appInitRulerData( rd );

    rd->rdUnit= whatUnit;
    rd->rdPostScriptFontList= psfl;

    appRulerCalculateIncrements( rd, magnifiedPixelsPerTwip, magnification );

    rd->rdMinUsed= leftRulerWidth;
    rd->rdExtraAfterMaxUsed= rightRulerWidth;

    rd->rdDocumentC0= documentC0;
    rd->rdDocumentC1= documentC1;
    ttr->ttrMarginX0= marginC0;
    ttr->ttrMarginX1= marginC1;
    rd->rdVisibleC0= visibleC0;
    rd->rdVisibleC1= visibleC1;

    ttr->ttrMagnification= magnification;

    ttr->ttrBlockFrameX0= documentC0;
    ttr->ttrBlockFrameX1= documentC1;
    ttr->ttrParaFrameX0= documentC0;
    ttr->ttrParaFrameX1= documentC1;

    rd->rdSizeAcross= height;
    rd->rdSizeAlong= 0;

    docInitTabStopList( &(ttr->ttrTabStopList) );
    ttr->ttrTabKind= DOCtaLEFT;

    ttr->ttrColumnCount= 0;
    ttr->ttrColumns= (ColumnSeparator *)0;

    ttr->ttrFirstIndent= -1;
    ttr->ttrLeftIndent= -1;
    ttr->ttrRightIndent= -1;

    return (void *)ttr;
    }

void tedFreeTopRuler(	void *		voidttr )
    {
    TedTopRuler *		ttr= (TedTopRuler *)voidttr;

    docCleanTabStopList( &(ttr->ttrTabStopList) );

    appCleanRulerData( &(ttr->ttrRulerData) );

    free( ttr );

    return;
    }

/************************************************************************/
/*									*/
/*  Draw a horizontal ruler.						*/
/*									*/
/************************************************************************/

static void tedDrawTopRuler(	APP_WIDGET			w,
				TedTopRuler *			ttr,
				const DocumentRectangle *	drClip )
    {
    RulerData *		rd= &(ttr->ttrRulerData);
    DrawingSurface	ds= rd->rdDrawingSurface;
    int			sizeAcross= rd->rdSizeAcross;
    int			ox= rd->rdVisibleC0- rd->rdMinUsed;
    int			maxUsed;

    DocumentRectangle	drActive;
    DocumentRectangle	drWhite;
    DocumentRectangle	drButton;
    DocumentRectangle	drBack;

    if  ( rd->rdSizeAlong == 0 )
	{ appHorizontalRulerGetSizeFromWidget( rd, w );	}

    maxUsed= rd->rdSizeAlong- rd->rdExtraAfterMaxUsed;

    if  ( drClip )
	{ drBack= *drClip;	}
    else{
	drBack.drX0= 0;
	drBack.drY0= 0;
	drBack.drX1= rd->rdSizeAlong- 1;
	drBack.drY1= sizeAcross- 1;
	}

    geoShiftRectangle( &drBack, -ox, 0 );
    appRulerDrawBackground( &(ttr->ttrRulerData), &drBack );

    tedRulerActiveRect( &drActive, ttr );
    drWhite= drActive;
    if  ( ! drClip || geoIntersectRectangle( &drWhite, &drWhite, drClip ) )
	{
	drawSetForegroundColorWhite( ds );

	geoShiftRectangle( &drWhite, -ox, 0 );

	if  ( drWhite.drX0 < rd->rdMinUsed )
	    { drWhite.drX0=  rd->rdMinUsed;	}
	if  ( drWhite.drX1 > maxUsed )
	    { drWhite.drX1=  maxUsed;	}

	drawFillRectangle( ds, &drWhite );
	}

    tedDrawTabButton( rd, ttr->ttrTabKind );

    if  ( rd->rdVisibleC0 <= rd->rdVisibleC1 )
	{
	drawSetForegroundColor( ds, &(rd->rdForegroundColor) );

	if  ( rd->rdVisibleC0 < drActive.drX0 )
	    {
	    tedTopRulerDrawTicks( ttr, ox, rd->rdVisibleC0, drActive.drX0 );
	    }
	if  ( drActive.drX1 < rd->rdVisibleC1 )
	    {
	    tedTopRulerDrawTicks( ttr, ox, drActive.drX1, rd->rdVisibleC1 );
	    }
	}

    if  ( drActive.drX0 <= drActive.drX1 )
	{
	drawSetForegroundColorBlack( ds );
	tedTopRulerDrawTicks( ttr, ox, drActive.drX0, drActive.drX1 );
	}

    {
    int				col;
    const ColumnSeparator *	cs= ttr->ttrColumns;

    for ( col= 0; col < ttr->ttrColumnCount; cs++, col++ )
	{
	tedRulerColumnControlRect( &drButton, cs->csX0, cs->csX1, rd );

	if  ( drClip							&&
	      ! geoIntersectRectangle( &drButton, &drButton, drClip )	)
	    { continue;	}

	/* Do not draw partial controls */
	if  ( drButton.drX0 < rd->rdVisibleC0	||
	      drButton.drX1 > rd->rdVisibleC1	)
	    { continue;	}

	tedRulerDrawColumnControl( cs->csX0, cs->csX1, ox, rd );
	}
    }

    if  ( ttr->ttrFirstIndent >= 0					&&
	  ttr->ttrFirstIndent >= rd->rdVisibleC0			&&
	  ttr->ttrFirstIndent <= rd->rdVisibleC1			)
	{
	tedHangingButtonRect( &drButton, ttr->ttrFirstIndent, rd );

	if  ( ( ! drClip						||
	        geoIntersectRectangle( &drButton, &drButton, drClip )	) &&
	      drButton.drX0 >= rd->rdVisibleC0				  &&
	      drButton.drX1 <= rd->rdVisibleC1				  )
	    {
	    tedRulerDrawHangingButton( ttr->ttrFirstIndent, rd );
	    }
	}

    if  ( ttr->ttrLeftIndent >= 0				&&
	  ttr->ttrLeftIndent >= rd->rdVisibleC0			&&
	  ttr->ttrLeftIndent <= rd->rdVisibleC1			)
	{
	tedStandingButtonRect( &drButton, ttr->ttrLeftIndent, rd );

	if  ( ( ! drClip						||
	        geoIntersectRectangle( &drButton, &drButton, drClip )	) &&
	      drButton.drX0 >= rd->rdVisibleC0				  &&
	      drButton.drX1 <= rd->rdVisibleC1				  )
	    {
	    tedRulerDrawStandingButton( ttr->ttrLeftIndent, rd );
	    }
	}

    if  ( ttr->ttrRightIndent >= 0				&&
	  ttr->ttrRightIndent >= rd->rdVisibleC0		&&
	  ttr->ttrRightIndent <= rd->rdVisibleC1		)
	{
	tedStandingButtonRect( &drButton, ttr->ttrRightIndent, rd );

	if  ( ( ! drClip						||
	        geoIntersectRectangle( &drButton, &drButton, drClip )	) &&
	      drButton.drX0 >= rd->rdVisibleC0				  &&
	      drButton.drX1 <= rd->rdVisibleC1				  )
	    {
	    tedRulerDrawStandingButton( ttr->ttrRightIndent, rd );
	    }
	}

    {
    int			tab;
    TabStop *		ts= ttr->ttrTabStopList.tslTabStops;

    drawSetForegroundColorBlack( ds );

    for ( tab= 0; tab < ttr->ttrTabStopList.tslTabStopCount; ts++, tab++ )
	{
	if  ( ts->tsPixels < rd->rdVisibleC0	||
	      ts->tsPixels > rd->rdVisibleC1	)
	    { continue;	}

	tedTabSymbolRect( &drButton, ts->tsPixels, rd );

	if  ( drClip							&&
	      ! geoIntersectRectangle( &drButton, &drButton, drClip )	)
	    { continue;	}

	/* Do not draw partial controls */
	if  ( drButton.drX0 < rd->rdVisibleC0	||
	      drButton.drX1 > rd->rdVisibleC1	)
	    { continue;	}

	tedRulerDrawTab( rd, ts->tsPixels, ts->tsAlignment );
	}
    }

    return;
    }

APP_EVENT_HANDLER_H( tedRedrawTopRuler, w, voidttr, event )
    {
    TedTopRuler *	ttr= (TedTopRuler *)voidttr;
    RulerData *		rd= &(ttr->ttrRulerData);

    int			ox= rd->rdVisibleC0- rd->rdMinUsed;
    const int		oy= 0;

    DocumentRectangle	drClip;

    if  ( ! rd->rdDrawingSurface )
	{ tedRulerMakeDrawingSurface( ttr, w );	}

    guiCollectExposures( &drClip, w, event );
    drawSetClipRect( rd->rdDrawingSurface, &drClip );

    geoShiftRectangle( &drClip, ox, oy );
    tedDrawTopRuler( w, ttr, &drClip );
    drawNoClipping( rd->rdDrawingSurface );
    }

APP_EVENT_HANDLER_H( tedTopRulerConfigure, w, voidttr, event )
    {
    TedTopRuler *	ttr= (TedTopRuler *)voidttr;
    RulerData *		rd= &(ttr->ttrRulerData);

    appHorizontalRulerGetSizeFromConfigureEvent( rd, w, event );

    return;
    }

void tedTopRulerSetBackground(	void *			voidttr,
				const RGB8Color *	back )
    {
    TedTopRuler *	ttr= (TedTopRuler *)voidttr;
    RulerData *		rd= &(ttr->ttrRulerData);

    appRulerSetBackground( rd, back );
    }

/************************************************************************/
/*									*/
/*  Scroll the horizontal ruler.					*/
/*									*/
/*  1)  Stretch the rectangle to contain whole controls.		*/
/*									*/
/************************************************************************/

# if SHOW_SCROLLS

static void tedShowClip(	DrawingSurface			ds,
				const DocumentRectangle *	dr )
    {
    RGB8Color		red;
    DocumentRectangle	drCopy= *dr;

    drCopy.drX1--;
    drCopy.drY1--;

    red.rgb8Red= 255;
    red.rgb8Green= 0;
    red.rgb8Blue= 0;

    drawSetForegroundColor( ds, &red );
    drawRectangle( ds, &drCopy );
    }

# endif

void tedScrollTopRuler(	void *			voidttr,
			APP_WIDGET		w,
			int			d )
    {
    TedTopRuler *		ttr= (TedTopRuler *)voidttr;
    RulerData *			rd= &(ttr->ttrRulerData);

    DocumentRectangle		drClip;
    DocumentRectangle		drRedraw;
    DocumentRectangle		drTabButton;

    const int			oy= 0;
    int				ox; /* Set after shift */

    appScrollHorizontalRuler( rd, &drClip, d );
    ox= rd->rdVisibleC0- rd->rdMinUsed; /* Set after shift */

    if  ( d < 0 )
	{
	tedTabButtonRectangle( &drTabButton, 0, rd );

	drClip.drX1 += drTabButton.drX1;
	}

    drawSetClipRect( rd->rdDrawingSurface, &drClip );

    drRedraw= drClip;
    geoShiftRectangle( &drRedraw, ox, oy );
    tedDrawTopRuler( w, ttr, &drRedraw );

    drawNoClipping( rd->rdDrawingSurface );

    /* Redraw tab type button if it is mutilated by the low level scroll */
    if  ( d > 0 )
	{
	tedTabButtonRectangle( &drTabButton, rd->rdMinUsed, rd );
	drawSetClipRect( rd->rdDrawingSurface, &drTabButton );

	drawSetForegroundColor( rd->rdDrawingSurface,
					    &(rd->rdBackgroundColor) );
	drawFillRectangle( rd->rdDrawingSurface, &drTabButton );
	tedDrawTabButton( rd, ttr->ttrTabKind );
	drawNoClipping( rd->rdDrawingSurface );
	}

#   if SHOW_SCROLLS
    tedShowClip( rd->rdDrawingSurface, &drClip );
#   endif

    }

/************************************************************************/
/*									*/
/*  Adapt to a change in size of the document: Remember the visible	*/
/*  range.								*/
/*									*/
/************************************************************************/

void tedSetTopRulerRange(	void *			voidttr,
				APP_WIDGET		w,
				int			docFullX1,
				int			docVisX0,
				int			docVisX1 )
    {
    TedTopRuler *		ttr= (TedTopRuler *)voidttr;
    RulerData *			rd= &(ttr->ttrRulerData);

    rd->rdDocumentC1= docFullX1;
    rd->rdVisibleC0= docVisX0;
    rd->rdVisibleC1= docVisX1;

    guiExposeDrawingWidget( w );

    return;
    }

/************************************************************************/
/*									*/
/*  Let the user drag one of the controls on the top ruler.		*/
/*									*/
/*  0)  Refuse to move outside the permitted range, but do not jump	*/
/*	back into it: Some documents do have strange settings and a	*/
/*	forced correction confuses the user. This is implemented by	*/
/*	refusing to move outside the possible range, and to refuse to	*/
/*	move even farther outside the permissible range.		*/
/*  1)  Draw a 'hair' over the document that corresponds to the		*/
/*	position of the control.					*/
/*  2)  Until the mouse button is released, observe events.		*/
/*  3)  Merge as many mouse motion events as possible.			*/
/*  4)  If the event is a mouse motion or a button release, redraw the	*/
/*	old an new position of the control on the ruler. (Assume that	*/
/*	the buttons are wider than the tab marks.			*/
/*  5)  Let the document move the hair.					*/
/*  6)  When the mouse button is released, the loop ends.		*/
/*  7)  Let the toolkit dispatch all events that are not relevant for	*/
/*	this routine.							*/
/*									*/
/************************************************************************/

static void tedTopRulerSetValue(	TopRulerDrag *		trd,
					int			newValue )
    {
    TedTopRuler *		ttr= trd->trdTtr;
    RulerData *			rd= &(ttr->ttrRulerData);
    int				oldValue= trd->trdValue;
    int				ox= trd->trdOc;
    const int			oy= 0;

    int				v0;
    int				v1;
    int				nv0;
    int				nv1;

    DocumentRectangle		drExpose;
    DocumentRectangle		drButton;

    if  ( newValue == oldValue )
	{ return;	}

    trd->trdValue= newValue;

    v1= v0= *(trd->trdV0Pointer);
    nv1= nv0= *(trd->trdV0Pointer)+ newValue- oldValue;

    if  ( trd->trdV1Pointer )
	{
	v1= *(trd->trdV1Pointer);
	nv1= *(trd->trdV1Pointer)+ newValue- oldValue;
	}

    /*  0  */
    if  ( nv0 < v0 && nv0 < trd->trdXmin )
	{
	if  ( v0 < trd->trdXmin )
	    { nv0= v0;			}
	else{ nv0= trd->trdXmin;	}

	nv1= nv0+ ( v1- v0 );
	}

    if  ( nv1 > v1 && nv1 > trd->trdXmax )
	{
	if  ( v1 > trd->trdXmax )
	    { nv1= v1;			}
	else{ nv1= trd->trdXmax;	}

	nv0= nv1- ( v1- v0 );
	}

    *(trd->trdV0Pointer)= nv0;
    if  ( trd->trdV1Pointer )
	{ *(trd->trdV1Pointer)= nv1;	}

    if  ( v0 < nv0 )
	{
	/* standing button has same width */
	tedHangingButtonRect( &drExpose,  v0, rd );
	tedHangingButtonRect( &drButton, nv1, rd );
	}
    else{
	/* standing button has same width */
	tedHangingButtonRect( &drExpose, nv0, rd );
	tedHangingButtonRect( &drButton,  v1, rd );
	}

    geoUnionRectangle( &drExpose, &drExpose, &drButton );
    drExpose.drY0= 0;
    drExpose.drY1= rd->rdSizeAcross;

    geoShiftRectangle( &drExpose, -ox, -oy );
    guiExposeDrawingWidgetRectangle( trd->trdWidget, &drExpose );

    /*  5  */
    appMetricRulerChangeValue( trd->trdDocument, newValue );

    return;
    }

static APP_EVENT_HANDLER_H( tedTopRulerMouseMove, w, vtrd, event )
    {
    TopRulerDrag *		trd= (TopRulerDrag *)vtrd;

    int				x;
    int				y;

    if  ( guiGetCoordinatesFromMouseMoveEvent( &x, &y, w, event ) )
	{ return;	}

    if  ( trd->trdYPointer )
	{ *(trd->trdYPointer)= y;	}

    tedTopRulerSetValue( trd, x+ trd->trdOc );

    return;
    }

static APP_EVENT_HANDLER_H( tedTopRulerMouseUp, w, vtrd, event )
    {
    TopRulerDrag *		trd= (TopRulerDrag *)vtrd;

    int				x;
    int				y;

    int				button;
    int				upDown;
    int				seq;
    unsigned int		keyState;

    if  ( guiGetCoordinatesFromMouseButtonEvent(
			&x, &y, &button, &upDown, &seq, &keyState, w, event ) )
	{ return;	}

    tedTopRulerSetValue( trd, x+ trd->trdOc );
    appMetricRulerExposeValue( trd->trdDocument );

    if  ( trd->trdYPointer )
	{ *(trd->trdYPointer)= y;	}

    return;
    }

static int tedTopRulerDragItem(	int *			pV0,
				int *			pV1,
				int *			pY,
				int			candidateProp,
				APP_WIDGET		w,
				APP_EVENT *		downEvent,
				TedTopRuler *		ttr,
				int			xMin,
				int			xMax,
				int			v,
				int			ox,
				EditDocument *		ed )
    {
    SelectRectangle *	sr= sr= &(ed->edSelectRectangle);
    TopRulerDrag	trd;
    int			startValue= *pV0;
    int			oldDirection= sr->srDirection;

    EditApplication *	ea= ed->edApplication;

    int			nv0;
    int			nv1;

    trd.trdValue= v;
    trd.trdV0Pointer= pV0;
    trd.trdV1Pointer= pV1;
    trd.trdYPointer= pY;
    trd.trdTtr= ttr;
    trd.trdWidget= w;
    trd.trdXmin= xMin;
    trd.trdXmax= xMax;
    trd.trdOc= ox;
    trd.trdDocument= ed;

    nv0= nv1= *(trd.trdV0Pointer);
    if  ( trd.trdV1Pointer )
	{ nv1= *(trd.trdV1Pointer); }

    sr->srSelected.drX0= ( nv0+ nv1 )/ 2;
    sr->srDirection= DOCselW;

    sr->srLTM.drX0= nv0- sr->srSelected.drX0;
    sr->srLTM.drX1= nv1- sr->srSelected.drX0;

    appMetricRulerExposeValue( ed );

    appRunDragLoop( w, ea, downEvent,
				tedTopRulerMouseUp,
				tedTopRulerMouseMove,
				0, (APP_TIMER_CALLBACK)0,
				(void *)&trd );

    appMetricRulerExposeValue( ed );

    sr->srDirection= oldDirection;

    if  ( *pV0 != startValue )
	{ return candidateProp;	}
    else{ return PPprop_NONE;	}
    }

/************************************************************************/
/*									*/
/*  The user clicked on the top ruler, make her happy.			*/
/*									*/
/*  1)  Only accept 'left mouse down' events.				*/
/*  2)  If the click is on one of the tab marks, apparently the user	*/
/*	wants to move the tab.						*/
/*  3)  If the tab was dragged from the ruler, assume that the user	*/
/*	wants to delete it.						*/
/*  4)  Move first indent?						*/
/*  5)  Move left indent?						*/
/*  6)  Move right indent?						*/
/*  7)  Not on one of the controls.. If the click is on the white band,	*/
/*	assume she wants to set a new tab.				*/
/*  8)  Is it on one of the table column separators? Allow the user to	*/
/*	drag it.							*/
/*  9)  Is it on the button with the kind of thabs that is to be added?	*/
/*	Rotate the type.						*/
/*									*/
/************************************************************************/

static int tedCompareTabStopsPixels(	const void *	voidts1,
					const void *	voidts2	)
    {
    const TabStop *	ts1= (const TabStop *)voidts1;
    const TabStop *	ts2= (const TabStop *)voidts2;

    return ts1->tsPixels- ts2->tsPixels;
    }

/*  2  */
static int tedTopRulerDragTab(	APP_WIDGET		w,
				APP_EVENT *		downEvent,
				TedTopRuler *		ttr,
				int			item,
				int			x,
				int			ox,
				int			mouseX,
				int			mouseY,
				EditDocument *		ed )
    {
    TabStop *		ts= ttr->ttrTabStopList.tslTabStops+ item;

    int			x0= ts->tsPixels;
    int			x1;
    int			dxAbs;
    DocumentRectangle	drActive;

    int			prop;

    x1= ts->tsPixels;

    prop= tedTopRulerDragItem( &x1, (int *)0, &mouseY, PPpropTAB_STOPS,
		    w, downEvent,
		    ttr, ttr->ttrParaFrameX0, ttr->ttrParaFrameX1, x, ox, ed );

    ts->tsPixels= x1;

    tedRulerActiveRect( &drActive, ttr );

    dxAbs= x1- x0;
    if  ( dxAbs < 0 )
	{ dxAbs= -dxAbs;	}

    /*  3  */
    if  ( dxAbs < 2* ( drActive.drY1- drActive.drY0+ 1 ) )
	{
	const int	n1= 6;
	const int	n2= -1;
	const int	n= n1+ n2;

	if  ( mouseY <  ( n1* drActive.drY0+ n2* drActive.drY1 )/ n	||
	      mouseY >  ( n2* drActive.drY0+ n1* drActive.drY1 )/ n	)
	    {
	    docDeleteTabFromList( &(ttr->ttrTabStopList), item );
	    prop= PPpropTAB_STOPS;

	    guiExposeDrawingWidget( w );
	    }
	}

    qsort( ttr->ttrTabStopList.tslTabStops,
				ttr->ttrTabStopList.tslTabStopCount,
				sizeof(TabStop), tedCompareTabStopsPixels );

    return prop;
    }

/************************************************************************/
/*									*/
/*  Handle a tab related click.						*/
/*									*/
/************************************************************************/

static int tedTopRulerTabClick(	TabStopList *		tsl,
				APP_WIDGET		w,
				APP_EVENT *		downEvent,
				TedTopRuler *		ttr,
				int			item,
				int			x,
				int			ox,
				int			mouseX,
				int			mouseY,
				EditDocument *		ed )
    {
    int		property= PPpropTAB_STOPS;

    if  ( item >= 0 )
	{
	/*  2  */
	property= tedTopRulerDragTab( w, downEvent, ttr,
			item, mouseX+ ox, ox, mouseX, mouseY, ed );

	if  ( property == PPpropTAB_STOPS )
	    {
	    if  ( docCopyTabStopList( tsl, &(ttr->ttrTabStopList) ) )
		{ LDEB(1); return -1;	}

	    guiExposeDrawingWidget( w );
	    }

	return property;
	}
    else{
	/*  7  */
	TabStop		tsNew;

	docInitTabStop( &tsNew );

	tsNew.tsPixels= mouseX+ ox;
	tsNew.tsAlignment= ttr->ttrTabKind;

	item= docAddTabToListPixels( &(ttr->ttrTabStopList), &tsNew );
	if  ( item < 0 )
	    { LDEB(item); return PPprop_NONE;	}

	guiExposeDrawingWidget( w );

	tedTopRulerDragTab( w, downEvent, ttr,
			    item, mouseX+ ox, ox, mouseX, mouseY, ed );

	if  ( docCopyTabStopList( tsl, &(ttr->ttrTabStopList) ) )
	    { LDEB(1); return -1;	}

	guiExposeDrawingWidget( w );
	return PPpropTAB_STOPS;
	}
    }

/************************************************************************/
/*									*/
/*  Find the target of a mouse click on the ruler.			*/
/*									*/
/************************************************************************/

int tedTopRulerFindMouse(	int *			pIndex,
				APP_WIDGET		w,
				int			mouseX,
				int			mouseY,
				EditDocument *		ed )
    {
    TedTopRuler  *	ttr= (TedTopRuler *)ed->edTopRuler;
    RulerData *		rd= &(ttr->ttrRulerData);

    int			ox= rd->rdVisibleC0- rd->rdMinUsed;

    DocumentRectangle	drActive;
    DocumentRectangle	drButton;

    const TabStopList *	rtsl= &(ttr->ttrTabStopList);

    tedRulerActiveRect( &drActive, ttr );

    /*  2  */
    if  ( mouseY >= drActive.drY0	&&
	  mouseY <= drActive.drY1	)
	{
	int			tab;
	const TabStop *		ts;

	ts= rtsl->tslTabStops;
	for ( tab= 0; tab < rtsl->tslTabStopCount; ts++, tab++ )
	    {
	    tedTabSymbolRect( &drButton, ts->tsPixels, rd );

	    if  ( mouseX+ ox >= drButton.drX0	&&
		  mouseX+ ox <  drButton.drX1	)
		{ *pIndex= tab; return PPpropTAB_STOPS;	}
	    }
	}

    /*  4  */
    tedHangingButtonRect( &drButton, ttr->ttrFirstIndent, rd );
    if  ( ttr->ttrFirstIndent >= 0				&&
	  geo2DIXYInBox( mouseX+ ox, mouseY, &drButton	)	)
	{ *pIndex= -1; return PPpropFIRST_INDENT;	}

    /*  5  */
    tedStandingButtonRect( &drButton, ttr->ttrLeftIndent, rd );
    if  ( ttr->ttrLeftIndent >= 0				&&
	  geo2DIXYInBox( mouseX+ ox, mouseY, &drButton	)	)
	{ *pIndex= -1; return PPpropLEFT_INDENT;	}

    /*  6  */
    tedStandingButtonRect( &drButton, ttr->ttrRightIndent, rd );
    if  ( ttr->ttrRightIndent >= 0				&&
	  geo2DIXYInBox( mouseX+ ox, mouseY, &drButton	)	)
	{ *pIndex= -1; return PPpropRIGHT_INDENT;	}

    /*  7  */
    if  ( mouseY >= drActive.drY0		&&
	  mouseY <= drActive.drY1		&&
	  mouseX+ ox >= ttr->ttrParaFrameX0	&&
	  mouseX+ ox <= ttr->ttrParaFrameX1	)
	{ *pIndex= -1; return PPpropTAB_STOPS;	}

    /*  8  */
    if  ( mouseY >= drActive.drY0	&&
	  mouseY <= drActive.drY1	)
	{
	int			col;
	const ColumnSeparator *	cs= ttr->ttrColumns;

	for ( col= 0; col < ttr->ttrColumnCount; cs++, col++ )
	    {
	    if  ( mouseX+ ox >= cs->csX0	&&
		  mouseX+ ox <  cs->csX1	)
		{ *pIndex= col; return PPprop_COLUMNS;	}
	    }
	}

    /*  9  */
    {
    DocumentRectangle	dr;

    tedTabButtonRectangle( &dr, rd->rdMinUsed, rd );
    if  ( geo2DIXYInBox( mouseX, mouseY, &dr ) )
	{ return PPprop_TAB_KIND_BUTTON;	}
    }

    return PPprop_NONE;
    }

int tedTopRulerTrackMouse(	int *			pFirstIndent,
				int *			pLeftIndent,
				int *			pRightIndent,
				TabStopList *		tsl,
				int *			pCsCount,
				ColumnSeparator **	pCs,
				APP_WIDGET		w,
				APP_EVENT *		downEvent,
				EditDocument *		ed )
    {
    TedTopRuler  *	ttr= (TedTopRuler *)ed->edTopRuler;
    RulerData *		rd= &(ttr->ttrRulerData);

    int			mouseX;
    int			mouseY;
    int			ox= rd->rdVisibleC0- rd->rdMinUsed;

    int			item;
    int			property;

    int			button;
    int			upDown;
    int			seq;
    unsigned int	keyState;

    if  ( guiGetCoordinatesFromMouseButtonEvent(
			&mouseX, &mouseY, &button, &upDown, &seq, &keyState,
			w, downEvent ) )
	{ return -1;	}

    if  ( upDown < 1 || button != 1 || seq > 1 )
	{ return -1;	}

    property= tedTopRulerFindMouse( &item, w, mouseX, mouseY, ed );
    switch( property )
	{
	case PPpropTAB_STOPS:
	    return tedTopRulerTabClick( tsl, w, downEvent, ttr, item,
					mouseX+ ox, ox, mouseX, mouseY, ed );

	case PPpropFIRST_INDENT:
	    /*  4  */
	    property= tedTopRulerDragItem( &(ttr->ttrFirstIndent),
			    (int *)0, (int *)0, property, w, downEvent, ttr,
			    ttr->ttrParaFrameX0, ttr->ttrParaFrameX1,
			    mouseX+ ox, ox, ed );

	    if  ( property == PPpropFIRST_INDENT )
		{
		*pFirstIndent= ttr->ttrFirstIndent;
		guiExposeDrawingWidget( w );
		}
	    return property;

	case PPpropLEFT_INDENT:
	    /*  5  */
	    property= tedTopRulerDragItem( &(ttr->ttrLeftIndent),
			    (int *)0, (int *)0, property, w, downEvent, ttr,
			    ttr->ttrParaFrameX0, ttr->ttrParaFrameX1,
			    mouseX+ ox, ox, ed );

	    if  ( property == PPpropLEFT_INDENT )
		{
		*pLeftIndent= ttr->ttrLeftIndent;
		guiExposeDrawingWidget( w );
		}
	    return property;

	case PPpropRIGHT_INDENT:
	    /*  6  */
	    property= tedTopRulerDragItem( &(ttr->ttrRightIndent),
			    (int *)0, (int *)0, property, w, downEvent, ttr,
			    ttr->ttrParaFrameX0, ttr->ttrParaFrameX1,
			    mouseX+ ox, ox, ed );

	    if  ( property == PPpropRIGHT_INDENT )
		{
		*pRightIndent= ttr->ttrRightIndent;
		guiExposeDrawingWidget( w );
		}
	    return property;

	case PPprop_COLUMNS:
	    /*  8  */
	    {
	    int		wide;
	    int		xMin;
	    int		xMax;

	    wide= ttr->ttrColumns[item].csX1- ttr->ttrColumns[item].csX0;
	    xMin= ttr->ttrBlockFrameX0- wide;
	    xMax= ttr->ttrBlockFrameX1+ wide;

	    if  ( item > 0 )
		{ xMin= ttr->ttrColumns[item- 1].csX1; }
	    if  ( item < ttr->ttrColumnCount- 1 )
		{ xMax= ttr->ttrColumns[item+ 1].csX0; }

	    property= tedTopRulerDragItem( &(ttr->ttrColumns[item].csX0),
				&(ttr->ttrColumns[item].csX1),
				(int *)0, property,
				w, downEvent, ttr,
				xMin, xMax, mouseX+ ox, ox, ed );

	    if  ( property == PPprop_COLUMNS )
		{
		*pCsCount= ttr->ttrColumnCount; *pCs= ttr->ttrColumns;
		guiExposeDrawingWidget( w );
		}
	    return property;
	    }

	case PPprop_TAB_KIND_BUTTON:
	    /*  9  */
	    {
	    DocumentRectangle	dr;

	    tedTabButtonRectangle( &dr, rd->rdMinUsed, rd );

	    switch( ttr->ttrTabKind )
		{
		case DOCtaLEFT:		ttr->ttrTabKind= DOCtaRIGHT;	break;
		case DOCtaRIGHT:	ttr->ttrTabKind= DOCtaCENTER;	break;
		case DOCtaCENTER:	ttr->ttrTabKind= DOCtaDECIMAL;	break;
		case DOCtaDECIMAL:	ttr->ttrTabKind= DOCtaLEFT;	break;
		default:
		    LDEB(ttr->ttrTabKind);
		}

	    guiExposeDrawingWidgetRectangle( w, &dr );
	    return PPprop_TAB_KIND_BUTTON;
	    }

	case PPprop_NONE:
	    return property;
	default:
	    LDEB(property); return -1;
	}
    }

/************************************************************************/
/*									*/
/*  Adapt an horizontal ruler to the paragraph.				*/
/*									*/
/************************************************************************/

void tedAdaptTopRuler(		void *			voidttr,
				APP_WIDGET		w,
				int			documentC0,
				int			documentC1,
				int			marginC0,
				int			marginC1,
				int			firstIndent,
				int			leftIndent,
				int			rightIndent,
				const TabStopList *	tslSet,
				int			x0GeometryPixels,
				double			xfac )
    {
    TedTopRuler *	ttr= (TedTopRuler *)voidttr;
    RulerData *		rd= &(ttr->ttrRulerData);

    int			redraw= 0;

    TabStopList		tslCopy;
    int			i;

    docInitTabStopList( &tslCopy );

    if  ( docCopyTabStopList( &tslCopy, tslSet ) )
	{ LDEB(1); goto ready;	}

    if  ( rd->rdDocumentC0 != documentC0 )
	{ rd->rdDocumentC0=   documentC0; redraw= 1;	}
    if  ( rd->rdDocumentC1 != documentC1 )
	{ rd->rdDocumentC1=   documentC1; redraw= 1;	}

    if  ( ttr->ttrMarginX0 != marginC0 )
	{ ttr->ttrMarginX0=   marginC0; redraw= 1;	}
    if  ( ttr->ttrMarginX1 != marginC1 )
	{ ttr->ttrMarginX1=   marginC1; redraw= 1;	}

    if  ( ttr->ttrFirstIndent != firstIndent )
	{ ttr->ttrFirstIndent=   firstIndent; redraw= 1;	}

    if  ( ttr->ttrLeftIndent != leftIndent )
	{ ttr->ttrLeftIndent=   leftIndent; redraw= 1;	}

    if  ( ttr->ttrRightIndent != rightIndent )
	{ ttr->ttrRightIndent=   rightIndent; redraw= 1;	}

    if  ( tslCopy.tslTabStopCount != ttr->ttrTabStopList.tslTabStopCount )
	{ redraw= 1;	}
    for ( i= 0; i < tslCopy.tslTabStopCount; i++ )
	{
	tslCopy.tslTabStops[i].tsPixels= x0GeometryPixels+
			COORDtoGRID( xfac, tslCopy.tslTabStops[i].tsTwips );

	if  ( i < ttr->ttrTabStopList.tslTabStopCount			&&
	      ! docEqualTabStop( &(tslCopy.tslTabStops[i]),
				&(ttr->ttrTabStopList.tslTabStops[i]) )	)
	    { redraw= 1;	}
	}

    /* steal by swapping */
    {
    TabStopList	sw= ttr->ttrTabStopList;
                    ttr->ttrTabStopList= tslCopy;
		                         tslCopy= sw;
    }

    {
    if  ( ! rd->rdDrawingSurface )
	{ tedRulerMakeDrawingSurface( ttr, w );		}
    if  ( ! rd->rdDrawingSurface )
	{ XDEB(rd->rdDrawingSurface); goto ready;	}

    if  ( redraw )
	{ guiExposeDrawingWidget( w );	}
    }

  ready:

    docCleanTabStopList( &tslCopy );

    return;
    }

int tedSetRulerColumns(		APP_WIDGET		w,
				void *			voidttr,
				int			bfX0Pixels,
				int			bfX1Pixels,
				int			pfX0Pixels,
				int			pfX1Pixels,
				ColumnSeparator *	cs,
				int			csCount )
    {
    TedTopRuler *	ttr= (TedTopRuler *)voidttr;
    int			changed= 0;
    int			i;

    ttr->ttrBlockFrameX0= bfX0Pixels;
    ttr->ttrBlockFrameX1= bfX1Pixels;
    ttr->ttrParaFrameX0= pfX0Pixels;
    ttr->ttrParaFrameX1= pfX1Pixels;

    if  ( csCount != ttr->ttrColumnCount )
	{
	if  ( csCount > ttr->ttrColumnCount )
	    {
	    ColumnSeparator *	fresh;

	    fresh= (ColumnSeparator *)
		realloc( ttr->ttrColumns, csCount* sizeof(ColumnSeparator) );
	    if  ( ! fresh )
		{ LXDEB(csCount,fresh); return -1;	}

	    ttr->ttrColumns= fresh;
	    fresh += ttr->ttrColumnCount;

	    while( ttr->ttrColumnCount < csCount )
		{
		fresh->csX0= 0;
		fresh->csX1= 0;

		ttr->ttrColumnCount++; fresh++;
		}
	    }

	ttr->ttrColumnCount= csCount;
	changed= 1;
	}

    for ( i= 0; i < csCount; i++ )
	{
	if  ( ttr->ttrColumns[i].csX0 != cs[i].csX0 )
	    { ttr->ttrColumns[i].csX0=   cs[i].csX0; changed= 1; }
	if  ( ttr->ttrColumns[i].csX1 != cs[i].csX1 )
	    { ttr->ttrColumns[i].csX1=   cs[i].csX1; changed= 1; }
	}

#   ifdef USE_MOTIF
    if  ( changed && XtIsRealized( w ) )
#   endif
#   ifdef USE_GTK
    if  ( changed && GTK_WIDGET_REALIZED( w ) )
#   endif
	{
	guiExposeDrawingWidget( w );
	}

    return 0;
    }

