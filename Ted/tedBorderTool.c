/************************************************************************/
/*									*/
/*  A Border Tool. Used on the pages of the format tool.		*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdio.h>
#   include	<stddef.h>

#   include	"tedDraw.h"
#   include	"tedBorderTool.h"
#   include	<guiToolUtil.h>
#   include	<guiTextUtil.h>

#   include	<guiWidgetDrawingSurface.h>
#   include	<guiDrawingWidget.h>
#   include	<appUnit.h>
#   include	<appDebugon.h>

#   define	MAKE_COLOR_ROW		0

#   define	USE_FRAME		0
#   define	USE_FRAME_TITLE		0
#   define	USE_TOGGLE_TITLE	1

#   define	THICK_CHARS		8

#   if USE_FRAME
#	define	ONOFF_COLUMN		0
#	define	ONOFF_COLSPAN		1

#	define	THICK_COLUMN		(ONOFF_COLUMN+ONOFF_COLSPAN)
#	define	THICK_COLSPAN		3

#	define	COLOR_COLUMN		(THICK_COLUMN+THICK_COLSPAN)
#	define	COLOR_COLSPAN		7
#   else
#	define	ONOFF_COLUMN		0
#	define	ONOFF_COLSPAN		1

#	define	THICK_COLUMN		(ONOFF_COLUMN+ONOFF_COLSPAN)
#	define	THICK_COLSPAN		1

#	define	COLOR_COLUMN		(THICK_COLUMN+THICK_COLSPAN)
#	define	COLOR_COLSPAN		2
#   endif

#   define	ONOFF_ROW_COLUMNS	(ONOFF_COLSPAN+THICK_COLSPAN+COLOR_COLSPAN)

/************************************************************************/
/*									*/
/*  Show the border width in a border tool.				*/
/*									*/
/************************************************************************/

static void tedBorderToolShowWide(	BorderTool *		bt )
    {
    ExpandedBorderProperties *	ebp= &(bt->btPropertiesChosen);

    guiEnableText( bt->btWideText,
			bt->btEnabled && ebp->ebpStyle != DOCbsNONE  );

    if  ( ebp->ebpStyle != DOCbsNONE )
	{
	appLengthToTextWidget( bt->btWideText,
					ebp->ebpPenWideTwips, UNITtyPOINTS );
	}
    else{ appStringToTextWidget( bt->btWideText, "" );	}

    return;
    }

/************************************************************************/

void tedEnableBorderTool(	BorderTool *		bt,
				int			enabled )
    {
    if  ( bt->btEnabled != enabled )
	{
	bt->btEnabled= enabled;

	appEnableColorChooser( &(bt->btColorChooser), enabled );
	guiEnableWidget( bt->btOnOffToggle, enabled );

	tedBorderToolShowWide( bt );

#	if USE_FRAME
	guiEnableWidget( bt->btFrame, enabled );
#	else
	guiEnableWidget( bt->btOnOffRow, enabled );
#	endif
	}
    }

/************************************************************************/
/*									*/
/*  Conform to border settings from the application.			*/
/*									*/
/************************************************************************/

static void tedBorderToolSetProperties(	BorderTool *			bt,
					const DocumentProperties *	dp,
					const BorderProperties *	bpSet )
    {
    ExpandedBorderProperties *	ebp= &(bt->btPropertiesChosen);
    int				set= 0;

    const int			avoidZero= 1;

    docExpandBorderProperties( ebp, bpSet, dp->dpColorPalette );

    if  ( ebp->ebpStyle != DOCbsNONE )
	{ set= 1;	}

    appGuiSetToggleState( bt->btOnOffToggle, set );
    tedBorderToolShowWide( bt );

    if  ( bt->btStyleRow )
	{
	appExposeDrawnPulldownInplace( &(bt->btStylePulldown) );
	}

    appColorChooserSuggestPalette( &(bt->btColorChooser),
					    avoidZero, dp->dpColorPalette );
    appColorChooserSetColor( &(bt->btColorChooser),
				    ebp->ebpColorExplicit,
				    &(ebp->ebpColor) );
    appShowColorChooser( &(bt->btColorChooser), set );
    if  ( bt->btColorRow != bt->btOnOffRow )
	{ guiEnableWidget( bt->btColorRow, set );	}

    utilPropMaskClear( &(bt->btPropertiesChanged) );

    return;
    }

void tedBorderToolSetPropertiesByNumber( BorderTool *			bt,
					const BufferDocument *		bd,
					int				num )
    {
    const DocumentProperties *	dp= &(bd->bdProperties);
    BorderProperties		bp;

    docGetBorderPropertiesByNumber( &bp, bd, num );

    tedBorderToolSetProperties( bt, dp, &bp );

    return;
    }

/************************************************************************/
/*									*/
/*  Draw the widgets of a border tool.					*/
/*									*/
/************************************************************************/

static void tedDrawBorderStyle(	DrawingSurface			ds,
				BorderTool *			bt,
				int				style,
				const DocumentRectangle *	drI )
    {
    if  ( style != DOCbsNONE )
	{
	const int	minThick= 1;

	tedDrawHorizontalBorderLine( ds, style, minThick,
			    drI->drX0,
			    drI->drX1, ( drI->drY0+ drI->drY1 )/ 2 );
	}

    /*  NO.. Confuses
    drawLine( ds, drI->drX0, drI->drY0, drI->drX1, drI->drY0 );
    drawLine( ds, drI->drX1, drI->drY0, drI->drX1, drI->drY1 );
    drawLine( ds, drI->drX1, drI->drY1, drI->drX0, drI->drY1 );
    drawLine( ds, drI->drX0, drI->drY1, drI->drX0, drI->drY0 );
    */

    return;
    }

static APP_EVENT_HANDLER_H( tedBorderToolRedrawInplace, w, voidbt, exposeEvent )
    {
    BorderTool *	bt= (BorderTool *)voidbt;
    DrawingSurface	dsI= bt->btStylePulldown.adpInplaceDrawingSurface;

    int			wide;
    int			high;

    DocumentRectangle	drClip;
    DocumentRectangle	drI;

    guiCollectExposures( &drClip, bt->btStylePulldown.adpInplaceDrawing,
								exposeEvent );

    drawSetClipRect( dsI, &drClip );

    appDrawnPulldownDrawArrow( &drClip, &wide, &high, w,
						&(bt->btStylePulldown) );

    drawNoClipping( dsI );

    drI.drX0= 0; drI.drX1= wide- 1;
    drI.drY0= 0; drI.drY1= high- 1;
    drawSetForegroundColorWhite( dsI );
    drawFillRectangle( dsI, &drI );

    drI.drX0= 3; drI.drX1= wide- 3;
    drI.drY0= 3; drI.drY1= high- 3;
    drawSetForegroundColorBlack( dsI );
    tedDrawBorderStyle( dsI, bt, bt->btPropertiesChosen.ebpStyle, &drI );

    return;
    }

static APP_EVENT_HANDLER_H( tedBorderToolRedrawPulldown, w, voidbt, exposeEvent )
    {
    BorderTool *	bt= (BorderTool *)voidbt;
    DrawingSurface	dsP;

    int			inplaceWide;
    int			inplaceHigh;
    int			pulldownWide;
    int			pulldownHigh;

    DocumentRectangle	drClip;
    DocumentRectangle	drBox;

    int			i;

    if  ( appFinishDrawnPulldownPulldown( &(bt->btStylePulldown) ) )
	{ LDEB(1); return;	}

    dsP= bt->btStylePulldown.adpPulldownDrawingSurface;

    guiDrawGetSizeOfWidget( &pulldownWide, &pulldownHigh, w );
    guiDrawGetSizeOfWidget( &inplaceWide, &inplaceHigh,
					bt->btStylePulldown.adpInplaceDrawing );

    guiCollectExposures( &drClip,  bt->btStylePulldown.adpPulldownDrawing,
								exposeEvent );

    drawSetForegroundColorWhite( dsP );
    drawFillRectangle( dsP, &drClip );

    drBox.drX0= 0;
    drBox.drY0= 0;
    drBox.drX1= pulldownWide- 1;
    drBox.drY1= pulldownHigh- 1;

    drawSetForegroundColorBlack( dsP );
    drawRectangle( dsP, &drBox );

    for ( i= DOCbsNONE+ 1; i < DOCbs_COUNT; i++ )
	{
	DocumentRectangle	drI;
	DocumentRectangle	drX;

	drI.drX0= 3;
	drI.drX1= pulldownWide- 3;
	drI.drY0= ( i+ 0 )* inplaceHigh+ 3;
	drI.drY1= ( i+ 1 )* inplaceHigh- 3;

	if  ( ! geoIntersectRectangle( &drX, &drI, &drClip ) )
	    { continue;	}

	tedDrawBorderStyle( dsP, bt, i, &drI );
	}

    return;
    }


/************************************************************************/
/*									*/
/*  One of the border on/off toggles has been activated by a user.	*/
/*									*/
/************************************************************************/

static APP_TOGGLE_CALLBACK_H( tedBorderToolOnOffToggled, w, voidbt, voidtbcs )
    {
    BorderTool *		bt= (BorderTool *)voidbt;
    ExpandedBorderProperties *	ebp= &(bt->btPropertiesChosen);

    int				set;

    PropertyMask	bpSetMask;

    utilPropMaskClear( &bpSetMask );

    set= appGuiGetToggleStateFromCallback( w, voidtbcs );

    if  ( set )
	{
	if  ( ebp->ebpStyle == DOCbsNONE )
	    {
	    ebp->ebpStyle= DOCbsS;
	    PROPmaskADD( &bpSetMask, BRDRpropSTYLE );
	    PROPmaskADD( &(bt->btPropertiesChanged), BRDRpropSTYLE );
	    /*changed= 1;*/
	    }

	if  ( ebp->ebpPenWideTwips == 0 )
	    {
	    ebp->ebpPenWideTwips= 15;
	    PROPmaskADD( &bpSetMask, BRDRpropPEN_WIDE );
	    PROPmaskADD( &(bt->btPropertiesChanged), BRDRpropPEN_WIDE );
	    /*changed= 1;*/
	    }
	}
    else{
	if  ( ebp->ebpStyle != DOCbsNONE )
	    {
	    ebp->ebpStyle= DOCbsNONE;
	    PROPmaskADD( &bpSetMask, BRDRpropSTYLE );
	    PROPmaskADD( &(bt->btPropertiesChanged), BRDRpropSTYLE );
	    /*changed= 1;*/
	    }
	}

    /*
    if  ( changed && bt->btCallback )
	{ (*bt->btCallback)( bt, bt->btTarget, &bpSetMask, bp );	}
    */

    tedBorderToolShowWide( bt );
    appShowColorChooser( &(bt->btColorChooser), set );
    if  ( bt->btColorRow != bt->btOnOffRow )
	{ guiEnableWidget( bt->btColorRow, set );	}

    return;
    }

/************************************************************************/
/*									*/
/*  A border style was chosen.						*/
/*									*/
/*  1)  Determine strip.						*/
/*  2)  Value in range?							*/
/*  3)  If the value changed..						*/
/*  4)  Remember new value						*/
/*  5)  Provoke a redraw of the image					*/
/*									*/
/************************************************************************/

static APP_EVENT_HANDLER_H( tedBorderToolClickedPulldown, w, voidbt, mouseEvent )
    {
    BorderTool *		bt= (BorderTool *)voidbt;
    ExpandedBorderProperties *	ebp= &(bt->btPropertiesChosen);

    int			style;

    /*  1  */
    if  ( appGuiDrawnPulldownGetStrip( &style,
				&(bt->btStylePulldown), w, mouseEvent ) )
	{ return;	}

    /*  2  */
    if  ( style < 0 || style >= DOCbs_COUNT )
	{ LLDEB(style,DOCbs_COUNT); return;	}

    /*  3  */
    if  ( ebp->ebpStyle != style )
	{
	/*  4  */
	ebp->ebpStyle= style;
	PROPmaskADD( &(bt->btPropertiesChanged), BRDRpropSTYLE );

	/*  5  */
	appExposeDrawnPulldownInplace( &(bt->btStylePulldown) );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  1)  The user typed Something in the 'Width' text box.		*/
/*  2)  The user typed 'Enter' in the 'Width' text box.			*/
/*									*/
/************************************************************************/

/*  1  */
static APP_TXTYPING_CALLBACK_H( tedFormatBorderWidthTyped, w, voidbt )
    {
    BorderTool *	bt= (BorderTool *)voidbt;

    PROPmaskADD( &(bt->btPropertiesChanged), BRDRpropPEN_WIDE );

    return;
    }

/*  2  */
static APP_TXACTIVATE_CALLBACK_H( tedFormatBorderWidthChanged, w, voidbt )
    {
    BorderTool *		bt= (BorderTool *)voidbt;
    ExpandedBorderProperties *	ebp= &(bt->btPropertiesChosen);

    int			wide= 0;
    int			changed= 0;

    const int		minValue= 1;
    const int		adaptToMin= 0;
    const int		maxValue= 75;
    const int		adaptToMax= 0;

    PropertyMask	bpSetMask;

    utilPropMaskClear( &bpSetMask );

    wide= ebp->ebpPenWideTwips;
    if  ( appGetLengthFromTextWidget( bt->btWideText,
				&wide, &changed, UNITtyPOINTS,
				minValue, adaptToMin, maxValue, adaptToMax ) )
	{ return;	}

    appLengthToTextWidget( bt->btWideText, wide, UNITtyPOINTS );

    if  ( changed )
	{
	ebp->ebpPenWideTwips= wide;
	PROPmaskADD( &bpSetMask, BRDRpropPEN_WIDE );
	PROPmaskADD( &(bt->btPropertiesChanged), BRDRpropPEN_WIDE );
	}

    /*
    if  ( changed && bt->btCallback )
	{ (*bt->btCallback)( bt, bt->btTarget, &bpSetMask, bp ); }
    */

    return;
    }

/************************************************************************/
/*									*/
/*  A color for the border was chosen.					*/
/*									*/
/*  1)  Actually.. No color was selected but the user wants to use a	*/
/*	more detailed tool.						*/
/*  2)  color chosen.							*/
/*									*/
/************************************************************************/

static void tedBorderColorChosen(
				ColorChooser *			cc,
				int				which,
				void *				voidbt,
				int				choice,
				const RGB8Color *		rgb8Set )
    {
    BorderTool *		bt= (BorderTool *)voidbt;
    ExpandedBorderProperties *	ebp= &(bt->btPropertiesChosen);
    int				changed= 0;

    /*  1  */
    if  ( choice == CHOICEccMORE )
	{
	appInspectorShowRgbPage( bt->btInspector,
			    bt->btSubjectPage, which, &(ebp->ebpColor) );
	return;
	}

    /*  2  */
    appColorChooserColorChosen( &(bt->btPropertiesChanged), &changed,
			&(ebp->ebpColor), &(ebp->ebpColorExplicit),
			rgb8Set, choice != CHOICEccDEFAULT, BRDRpropCOLOR );

    return;
    }

void tedBorderSetExplicitColorChoice(	BorderTool *		bt,
					const RGB8Color *	rgb8Set )
    {
    const int			colorExplicit= 1;
    ExpandedBorderProperties *	ebp= &(bt->btPropertiesChosen);
    int				changed= 0;

    appColorChooserColorChosen( &(bt->btPropertiesChanged), &changed,
				&(ebp->ebpColor), &(ebp->ebpColorExplicit),
				rgb8Set, colorExplicit, BRDRpropCOLOR );

    if  ( changed )
	{
	appColorChooserSetColor( &(bt->btColorChooser),
				    ebp->ebpColorExplicit, &(ebp->ebpColor) );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Make a BorderTool.							*/
/*									*/
/************************************************************************/

void tedInitBorderTool(		BorderTool *		bt )
    {
    bt->btInspector= (AppInspector *)0;
    bt->btSubjectPage= -1;
    bt->btWhich= -1;

    bt->btLabel= (const char *)0;

    bt->btEnabled= 0;

    bt->btFrame= (APP_WIDGET)0;
    bt->btPaned= (APP_WIDGET)0;

    bt->btOnOffRow= (APP_WIDGET)0;
    bt->btOnOffToggle= (APP_WIDGET)0;
    bt->btWideText= (APP_WIDGET)0;

    bt->btStyleRow= (APP_WIDGET)0;
    bt->btStyleLabel= (APP_WIDGET)0;
    appInitDrawnPulldown( &(bt->btStylePulldown) );

    bt->btColorRow= (APP_WIDGET)0;
    bt->btColorLabel= (APP_WIDGET)0;
    appInitColorChooser( &(bt->btColorChooser) );

    docInitExpandedBorderProperties( &(bt->btPropertiesChosen) );
    utilPropMaskClear( &(bt->btPropertiesChanged) );

    return;
    }

void tedCleanBorderTool(		BorderTool *		bt )
    {
    appCleanDrawnPulldown( &(bt->btStylePulldown) );

    return;
    }

void tedMakeBorderTool(		BorderTool *			bt,
				AppInspector *			ai,
				APP_WIDGET			pageWidget,
				const char *			title,
				const BorderToolResources *	btr,
				int				subjectPage,
				int				which )
    {
    const int	bordersHaveAutomaticColor= 1;

    const int	heightResizable= 0;

    const int	textEnabled= 0;

    const int	labelColumn= 0;
    const int	labelColspan= 1;

    const int	valueColumn= 1;
    const int	valueColspan= 1;

    APP_WIDGET	rowParent= pageWidget;

    /**/
    bt->btInspector= ai;
    bt->btSubjectPage= subjectPage;
    bt->btWhich= which;

    bt->btEnabled= 1;

    /**/
    docInitExpandedBorderProperties( &(bt->btPropertiesChosen) );

    /**/
#   if USE_FRAME
#	if USE_FRAME_TITLE
	appMakeColumnFrameInColumn( &(bt->btFrame), &(bt->btPaned), pageWidget,
				title );
#	else
	appMakeColumnFrameInColumn( &(bt->btFrame), &(bt->btPaned), pageWidget,
				(const char *)0 );
#	endif

    rowParent= bt->btPaned;

#   else
    rowParent= pageWidget;
#   endif

    /**/
    bt->btOnOffRow= appMakeRowInColumn( rowParent,
					ONOFF_ROW_COLUMNS, heightResizable );

#   if USE_TOGGLE_TITLE
    bt->btOnOffToggle= appMakeToggleInRow( bt->btOnOffRow, title,
				tedBorderToolOnOffToggled, (void *)bt,
				ONOFF_COLUMN, ONOFF_COLSPAN );
#   else
    bt->btOnOffToggle= appMakeToggleInRow( bt->btOnOffRow, (const char *)0,
				tedBorderToolOnOffToggled, (void *)bt,
				ONOFF_COLUMN, ONOFF_COLSPAN );
#   endif

    appMakeTextInRow( &(bt->btWideText), bt->btOnOffRow,
				THICK_COLUMN, THICK_COLSPAN, THICK_CHARS,
				textEnabled );

    appGuiSetGotValueCallbackForText( bt->btWideText,
				tedFormatBorderWidthChanged, (void *)bt );
    appGuiSetTypingCallbackForText( bt->btWideText,
				tedFormatBorderWidthTyped, (void *)bt );

#   if MAKE_COLOR_ROW
    appMakeLabelAndColorChooserRow( &(bt->btColorRow), &(bt->btColorLabel),
			    &(bt->btColorChooser),
			    bt->btPaned, btr->btrColor,
			    &(btr->btrColorChooserResources),
			    tedBorderColorChosen, which, (void *)bt );
#   else
    bt->btColorRow= bt->btOnOffRow;

    appMakeColorChooserInRow( &(bt->btColorChooser), bordersHaveAutomaticColor,
				    bt->btColorRow,
				    COLOR_COLUMN, COLOR_COLSPAN,
				    &(btr->btrColorChooserResources),
				    tedBorderColorChosen, which, (void *)bt );
#   endif

    /**/
#   if 0
    bt->btStyleRow= appMakeRowInColumn( bt->btPaned, 2, heightResizable );
#   else
    bt->btStyleRow= (APP_WIDGET)0;
#   endif

    if  ( bt->btStyleRow )
	{
	appMakeLabelInRow( &(bt->btStyleLabel), bt->btStyleRow,
				labelColumn, labelColspan, btr->btrStyle );

	appMakeDrawnPulldownInRow( &(bt->btStylePulldown),
				    tedBorderToolRedrawInplace,
				    tedBorderToolRedrawPulldown,
				    tedBorderToolClickedPulldown,
				    (AppDrawnPulldownPuldown)0,
				    bt->btStyleRow, valueColumn, valueColspan,
				    (void *)bt );
	}

    return;
    }

void tedFinishBorderTool(	BorderTool *			bt,
				const PostScriptFontList *	psfl )
    {
    if  ( bt->btStyleRow )
	{
	if  ( appFinishDrawnPulldownInplace( &(bt->btStylePulldown) ) )
	    { LDEB(1); return;	}

	appGuiSetDrawnPulldownStrips( &(bt->btStylePulldown), DOCbs_COUNT );
	}

    if  ( bt->btColorLabel )
	{
	appFinishColorChooser( &(bt->btColorChooser), psfl, bt->btColorLabel );
	}
    else{
	appFinishColorChooser( &(bt->btColorChooser), psfl, bt->btOnOffToggle );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Retrieve the border properties from the border tool.		*/
/*									*/
/*  The properties managed by the toggle and the various option menus	*/
/*  or pulldowns are already stored in bt->btPropertiesChosen.		*/
/*  The typing callback sets the changed flag for the pen width.	*/
/*									*/
/*  Retrieve the pen width if its changed flag is set.			*/
/*									*/
/************************************************************************/

static int tedBorderToolGetProperties(	BorderProperties *	bpSet,
					PropertyMask *		pSetMask,
					const BorderTool *	bt,
					DocumentProperties *	dp )
    {
    const ExpandedBorderProperties *	ebpChosen= &(bt->btPropertiesChosen);

    int				changed= 0;

    const int			minValue= 1;
    const int			adaptToMin= 0;
    const int			maxValue= 75;
    const int			adaptToMax= 0;

    int				wide= ebpChosen->ebpPenWideTwips;

    PropertyMask		bpDoneMask;

    if  ( PROPmaskISSET( &(bt->btPropertiesChanged), BRDRpropPEN_WIDE )	&&
	  ebpChosen->ebpStyle != DOCbsNONE				)
	{
	if  ( appGetLengthFromTextWidget( bt->btWideText,
			    &wide, &changed, UNITtyPOINTS,
			    minValue, adaptToMin, maxValue, adaptToMax ) )
	    { return -1;	}
	}

    docIndirectBorderProperties( &bpDoneMask, bpSet,
				    &(bt->btPropertiesChanged), ebpChosen,
				    dp->dpColorPalette );
    if  ( PROPmaskISSET( &(bt->btPropertiesChanged), BRDRpropPEN_WIDE )	&&
	  changed							)
	{ bpSet->bpPenWideTwips= wide; }

    *pSetMask= bt->btPropertiesChanged;

    return 0;
    }

int tedBorderToolGetNumber(	int *			pNum,
				int *			pChanged,
				const BorderTool *	bt,
				BufferDocument *	bd )
    {
    int			num;
    PropertyMask	doneMask;
    BorderProperties	bp;

    docGetBorderPropertiesByNumber( &bp,  bd, *pNum );

    utilPropMaskClear( &doneMask );

    if  ( tedBorderToolGetProperties( &bp, &doneMask, bt,
						&(bd->bdProperties) ) )
	{ LDEB(1); return -1;	}

    if  ( ! DOCisBORDER( &bp ) )
	{
	docCleanBorderProperties( &bp );
	docInitBorderProperties( &bp );
	}

    num= docBorderPropertiesNumber( bd, &bp );
    if  ( num < 0 )
	{ LDEB(num); return -1;	}

    if  ( pChanged )
	{ *pChanged= ( *pNum != num );	}
    *pNum= num;

    return 0;
    }

