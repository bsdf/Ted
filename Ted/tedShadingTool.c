/************************************************************************/
/*									*/
/*  A Shading Tool. (Used on the pages of the format tool).		*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdio.h>
#   include	<stddef.h>

#   include	"tedShadingTool.h"
#   include	<appDebugon.h>
#   include	<guiToolUtil.h>
#   include	<guiTextUtil.h>

#   define	SHOW_PATTERN	0

/************************************************************************/
/*									*/
/*  Show the shading level width in a shading tool.			*/
/*									*/
/************************************************************************/

static void tedShadingToolShowLevel(	ShadingTool *		st,
					const ItemShading *	is )
    {
    if  ( ! st->stLevelText )
	{ return;	}

    guiEnableText( st->stLevelText, is->isPattern == DOCspSOLID );

    if  ( is->isPattern == DOCspSOLID )
	{
	appIntegerToTextWidget( st->stLevelText, is->isLevel/ 100 );
	}
    else{ appStringToTextWidget( st->stLevelText, "" );	}

    return;
    }

/************************************************************************/

void tedEnableShadingTool(		ShadingTool *		st,
					int			enabled )
    {
    ExpandedItemShading *	eis= &(st->stShadingChosen);

    if  ( st->stFrame )
	{ guiEnableWidget( st->stFrame, enabled );		}
    else{ guiEnableWidget( st->stRow, enabled );		}

    appEnableColorChooser( &(st->stBackColorChooser), enabled );
    appShowColorChooser( &(st->stBackColorChooser), eis->eisBackColorExplicit );
    }

/************************************************************************/
/*									*/
/*  Set the current shading in a shading tool.				*/
/*									*/
/************************************************************************/

static void tedSetShadingTool(		ShadingTool *			st,
					const DocumentProperties *	dp,
					const ItemShading *		is )
    {
    ExpandedItemShading *	eis= &(st->stShadingChosen);
    const int			avoidZero= 1;

    tedShadingToolShowLevel( st, is );

    /**/
    docExpandItemShading( eis, is, dp->dpColorPalette );

    /**/
    appColorChooserSuggestPalette( &(st->stBackColorChooser),
					    avoidZero, dp->dpColorPalette );
    appColorChooserSetColor( &(st->stBackColorChooser),
				    eis->eisBackColorExplicit,
				    &(eis->eisBackColor) );
    appGuiSetToggleState( st->stOnOffToggle, eis->eisBackColorExplicit );
    appShowColorChooser( &(st->stBackColorChooser),
				    eis->eisBackColorExplicit );

    /**/
    if  ( st->stForeColorChooser.ccFilled )
	{
	appColorChooserSuggestPalette( &(st->stForeColorChooser),
					    avoidZero, dp->dpColorPalette );

	appColorChooserSetColor( &(st->stForeColorChooser),
				eis->eisForeColorExplicit,
				&(eis->eisForeColor) );
	}

    /**/
    if  ( st->stShowPattern )
	{
	appExposeDrawnPulldownInplace( &(st->stPatternPulldown) );
	}

    utilPropMaskClear( &(st->stShadingChanged) );

    return;
    }

void tedSetShadingToolByNumber(		ShadingTool *			st,
					const BufferDocument *		bd,
					int				num )
    {
    const DocumentProperties *		dp= &(bd->bdProperties);

    ItemShading			is;

    docGetItemShadingByNumber( &is, bd, num );

    tedSetShadingTool( st, dp, &is );

    return;
    }

/************************************************************************/
/*									*/
/*  Draw the widgets of a shading tool.					*/
/*									*/
/************************************************************************/

# if SHOW_PATTERN

static void tedDrawShadedBox(	AppDrawingData *		add,
				ShadingTool *			st,
				int				pattern,
				const DocumentRectangle *	drI )
    {
    if  ( pattern != DOCspSOLID )
	{
	tedDrawShadedRectangle( add, st->stPatternPixmaps, pattern, drI );
	}

    drawLine( add, drI->drX0, drI->drY0, drI->drX1, drI->drY0 );
    drawLine( add, drI->drX1, drI->drY0, drI->drX1, drI->drY1 );
    drawLine( add, drI->drX1, drI->drY1, drI->drX0, drI->drY1 );
    drawLine( add, drI->drX0, drI->drY1, drI->drX0, drI->drY0 );

    return;
    }

static APP_EVENT_HANDLER_H( tedShadingRedrawInplace, w, voidst, exposeEvent )
    {
    ShadingTool *	st= (ShadingTool *)voidst;
    AppDrawingData *	add= &(st->stPatternInplaceDrawingData);

    int			wide;
    int			high;

    DocumentRectangle	drI;

    appDrawnPulldownDrawArrow( &wide, &high, w, &(xx) );

    drI.drX0= 3; drI.drX1= wide- 3;
    drI.drY0= 3; drI.drY1= high- 3;

    drawSetForegroundColorWhite( add );
    drawFillRectangle( add, drI.drX0, drI.drY0,
			    drI.drX1- drI.drX0+ 1, drI.drY1- drI.drY0+ 1 );

    drawSetForegroundColorBlack( add );

    tedDrawShadedBox( add, st, st->stShadingChosen.eisPattern, &drI );

    return;
    }

static APP_EVENT_HANDLER_H( tedShadingRedrawPulldown, w, voidst, exposeEvent )
    {
    ShadingTool *	st= (ShadingTool *)voidst;
    AppDrawingData *	add= &(st->stPatternPulldownDrawingData);

    int			inplaceWide;
    int			inplaceHigh;
    int			pulldownWide;
    int			pulldownHigh;

    const int		ox= 0;
    const int		oy= 0;
    DocumentRectangle	drClip;

    int			i;

    if  ( ! st->stPulldownDrawingDataSet )
	{
	appSetDrawingDataForWidget( st->stPatternPulldown.adpPulldownDrawing,
				    1.0, &(st->stPatternPulldownDrawingData) );

	st->stPulldownDrawingDataSet= 1;
	}

    guiDrawGetSizeOfWidget( &pulldownWide, &pulldownHigh, w );
    guiDrawGetSizeOfWidget( &inplaceWide, &inplaceHigh,
				    st->stPatternPulldown.adpInplaceDrawing );

    guiCollectExposures( &drClip, add, ox, oy, exposeEvent );

    appDrawSetForegroundWhite( add );

    drawFillRectangle( add, drClip.drX0, drClip.drY0,
					    drClip.drX1- drClip.drX0+ 1,
					    drClip.drY1- drClip.drY0+ 1 );

    drawSetForegroundBlack( add );
    drawRectangle( add, 0, 0, pulldownWide- 1, pulldownHigh- 1 );

    for ( i= DOCspSOLID+ 1; i < PSshd_COUNT; i++ )
	{
	DocumentRectangle	drI;
	DocumentRectangle	drX;

	drI.drX0= 3;
	drI.drX1= pulldownWide- 3;
	drI.drY0= ( i+ 0 )* inplaceHigh+ 3;
	drI.drY1= ( i+ 1 )* inplaceHigh- 3;

	if  ( ! geoIntersectRectangle( &drX, &drI, &drClip ) )
	    { continue;	}

	tedDrawShadedBox( add, st, i, &drI );
	}

    return;
    }
# endif

/************************************************************************/
/*									*/
/*  The user typed something in the 'Level' text box.			*/
/*									*/
/************************************************************************/

static APP_TXTYPING_CALLBACK_H( tedShadingLevelTyped, w, voidst )
    {
    ShadingTool *	st= (ShadingTool *)voidst;

    PROPmaskADD( &(st->stShadingChanged), ISpropLEVEL );

    return;
    }

/************************************************************************/
/*									*/
/*  A shading pattern was chosen.					*/
/*									*/
/************************************************************************/

# if SHOW_PATTERN
static APP_EVENT_HANDLER_H( tedShadingClickedPulldown, w, voidst, mouseEvent )
    {
    ShadingTool *		st= (ShadingTool *)voidst;
    ExpandedItemShading *	eis= &(st->stShadingChosen);

    int			pattern;

    if  ( appGuiDrawnPulldownGetStrip( &pattern,
				&(st->stPatternPulldown), w, mouseEvent ) )
	{ return;	}

    if  ( pattern < 0 || pattern >= PSshd_COUNT )
	{ LLDEB(pattern,PSshd_COUNT); return;	}

    if  ( eis->eisPattern != pattern )
	{
	PropertyMask	isSetMask;

	utilPropMaskClear( &isSetMask );
	PROPmaskADD( &isSetMask, ISpropPATTERN );
	PROPmaskADD( &(st->stShadingChanged), ISpropPATTERN );

	eis->eisPattern= pattern;

	guiExposeDrawingWidget( &(st->stPatternInplaceDrawingData) );

	if  ( st->stCallback )
	    {
	    (*st->stCallback)( st, st->stTarget, &isSetMask, eis );
	    }
	}

    return;
    }
# endif

/************************************************************************/

static APP_TOGGLE_CALLBACK_H( tedShadeToggled, w, voidst, voidtbcs )
    {
    ShadingTool *		st= (ShadingTool *)voidst;
    ExpandedItemShading *	eis= &(st->stShadingChosen);

    int				set;

    set= appGuiGetToggleStateFromCallback( w, voidtbcs );

    eis->eisBackColorExplicit= set != 0;
    appShowColorChooser( &(st->stBackColorChooser), set );

    PROPmaskADD( &(st->stShadingChanged), ISpropBACK_COLOR );

    if  ( set )
	{
	appColorChooserSetColor( &(st->stBackColorChooser),
					    eis->eisBackColorExplicit,
					    &(eis->eisBackColor) );
	}
    }

/************************************************************************/
/*									*/
/*  A background/foreground has been selected in one of the color	*/
/*  choosers of the the shading tool.					*/
/*									*/
/*  1)  Actually.. No color was selected but the user wants to use a	*/
/*	more detailed tool.						*/
/*  2)  Background chosen.						*/
/*  3)  Foreground chosen.						*/
/*									*/
/************************************************************************/

static void tedShadeColorChosen( ColorChooser *			cc,
				int				which,
				void *				voidst,
				int				choice,
				const RGB8Color *		rgb8Set )
    {
    ShadingTool *		st= (ShadingTool *)voidst;
    ExpandedItemShading *	eis= &(st->stShadingChosen);
    int				changed= 0;

    /*  1  */
    if  ( choice == CHOICEccMORE )
	{
	if  ( which == st->stBackWhich )
	    {
	    appInspectorShowRgbPage( st->stInspector,
			    st->stSubjectPage, which, &(eis->eisBackColor) );
	    return;
	    }

	if  ( which == st->stForeWhich )
	    {
	    appInspectorShowRgbPage( st->stInspector,
			    st->stSubjectPage, which, &(eis->eisForeColor) );
	    return;
	    }

	LLDEB(CHOICEccMORE,which);
	return;
	}

    /*  2  */
    if  ( which == st->stBackWhich )
	{
	appColorChooserColorChosen( &(st->stShadingChanged), &changed,
			&(eis->eisBackColor), &(eis->eisBackColorExplicit),
			rgb8Set, choice != CHOICEccDEFAULT, ISpropBACK_COLOR );

	return;
	}

    /*  3  */
    if  ( which == st->stForeWhich )
	{
	appColorChooserColorChosen( &(st->stShadingChanged), &changed,
			&(eis->eisForeColor), &(eis->eisForeColorExplicit),
			rgb8Set, choice != CHOICEccDEFAULT, ISpropFORE_COLOR );

	return;
	}

    LDEB(which); return;
    }

void tedShadeSetExplicitColorChoice(
				ShadingTool *			st,
				int				which,
				const RGB8Color *		rgb8Set )
    {
    const int			colorExplicit= 1;
    ExpandedItemShading *	eis= &(st->stShadingChosen);
    int				changed= 0;

    /*  2  */
    if  ( which == st->stBackWhich )
	{
	appColorChooserColorChosen( &(st->stShadingChanged), &changed,
			&(eis->eisBackColor), &(eis->eisBackColorExplicit),
			rgb8Set, colorExplicit, ISpropBACK_COLOR );

	if  ( changed )
	    {
	    appColorChooserSetColor( &(st->stBackColorChooser),
					    eis->eisBackColorExplicit,
					    &(eis->eisBackColor) );
	    }

	return;
	}

    /*  3  */
    if  ( which == st->stForeWhich )
	{
	appColorChooserColorChosen( &(st->stShadingChanged), &changed,
			&(eis->eisForeColor), &(eis->eisForeColorExplicit),
			rgb8Set, colorExplicit, ISpropFORE_COLOR );

	if  ( changed && st->stForeColorChooser.ccFilled )
	    {
	    appColorChooserSetColor( &(st->stForeColorChooser),
					    eis->eisForeColorExplicit,
					    &(eis->eisForeColor) );
	    }

	return;
	}

    LDEB(which); return;
    }

/************************************************************************/
/*									*/
/*  Make a ShadingTool.							*/
/*									*/
/************************************************************************/

void tedInitShadingTool(		ShadingTool *		st )
    {
    int			i;

    st->stFrame= (APP_WIDGET)0;
    st->stPaned= (APP_WIDGET)0;
    st->stRow= (APP_WIDGET)0;

    st->stPatternLabel= (APP_WIDGET)0;
    st->stLevelLabel= (APP_WIDGET)0;
    st->stBackLabel= (APP_WIDGET)0;
    st->stForeLabel= (APP_WIDGET)0;

    st->stOnOffToggle= (APP_WIDGET)0;

    st->stShowPattern= 0;
    appInitDrawnPulldown( &(st->stPatternPulldown) );

    for ( i= 0; i < PSshd_COUNT; i++ )
	{ st->stPatternPixmaps[i]= (DrawingSurface)0;	}

    st->stLevelText= (APP_WIDGET)0;

    appInitColorChooser( &(st->stForeColorChooser) );
    appInitColorChooser( &(st->stBackColorChooser) );

    st->stCallback= (TedShadingToolCallback)0;
    st->stTarget= (void *)0;
    st->stInspector= (AppInspector *)0;

    st->stSubjectPage= -1;
    st->stBackWhich= -1;
    st->stBackWhich= -1;

    return;
    }

void tedCleanShadingTool(		ShadingTool *		st )
    {
    int			i;

    for ( i= 0; i < PSshd_COUNT; i++ )
	{
	if  ( st->stPatternPixmaps[i] )
	    { drawFreeDrawingSurface( st->stPatternPixmaps[i] );	}
	}

    appCleanDrawnPulldown( &(st->stPatternPulldown) );

    appCleanColorChooser( &(st->stForeColorChooser) );
    appCleanColorChooser( &(st->stBackColorChooser) );

    return;
    }

void tedFormatMakeShadingTool(	ShadingTool *			st,
				AppInspector *			ai,
				APP_WIDGET			pageWidget,
				const char *			title,
				const ShadingToolResources *	str,
				int				subjectPage,
				int				backWhich,
				int				foreWhich,
				TedShadingToolCallback		callback,
				void *				target )
    {
    const int	shadingHasAutomaticColor= 0;
    APP_WIDGET	parent= pageWidget;

    /*
    const int	heightResizable= 0;
    const int	labelColumn= 0;
    const int	labelColspan= 1;
    */

    /*
    const int	textColumns= 6;
    */
    /*
    const int	valueColumn= 1;
    const int	valueColspan= 1;
    */

    /****/
    st->stCallback= callback;
    st->stTarget= target;
    st->stInspector= ai;

    st->stSubjectPage= subjectPage;
    st->stBackWhich= backWhich;
    st->stForeWhich= foreWhich;

    docInitExpandedItemShading( &(st->stShadingChosen) );

    /****/
    if  ( title )
	{
	appMakeColumnFrameInColumn( &(st->stFrame),
					&(st->stPaned), pageWidget, title );
	parent= st->stPaned;
	}

    /****/
#   if SHOW_PATTERN
    row= appMakeRowInColumn( parent, 2, heightResizable );

    appMakeLabelInRow( &(st->stPatternLabel), row,
				labelColumn, labelColspan, str->strPattern );
    appMakeDrawnPulldownInRow( &(st->stPatternPulldown),
					    tedShadingRedrawInplace,
					    tedShadingRedrawPulldown,
					    tedShadingClickedPulldown,
					    row, valueColumn, valueColspan,
					    (void *)st );
    st->stShowPattern= 1;
#   else
    st->stShowPattern= 0;
#   endif

    /****/
#   if 0
    guiToolMakeLabelAndTextRow( &row, &(st->stLevelLabel), &(st->stLevelText),
				parent, str->strLevel, textColumns, 1 );
#   else
    st->stLevelLabel= (APP_WIDGET)0;
    st->stLevelText= (APP_WIDGET)0;
#   endif

    if  ( st->stLevelText )
	{
	appGuiSetTypingCallbackForText( st->stLevelText,
					tedShadingLevelTyped, (void *)st );
	}

    /****/
#   if 0
    appMakeLabelAndColorChooserRow( &(st->stRow), &(st->stBackLabel),
			    &(st->stBackColorChooser),
			    parent, str->strBackColor,
			    &(str->strBackColorChooserResources),
			    tedShadeColorChosen, backWhich, (void *)st );
#   else
    appMakeToggleAndColorChooserRow( &(st->stRow), &(st->stOnOffToggle),
			    &(st->stBackColorChooser), shadingHasAutomaticColor,
			    parent, str->strBackColor,
			    &(str->strBackColorChooserResources),
			    tedShadeToggled, tedShadeColorChosen,
			    backWhich, (void *)st );
#   endif

    /****/
#   if 0
    appMakeLabelAndColorChooserRow( &(st->stRow2), &(st->stForeLabel),
			    &(st->stForeColorChooser),
			    parent, str->strForeColor,
			    &(str->strForeColorChooserResources),
			    tedShadeColorChosen, foreWhich, (void *)st );
#   endif

    return;
    }

void tedFinishShadingTool(	ShadingTool *			st,
				const PostScriptFontList *	psfl )
    {
    if  ( st->stShowPattern )
	{
	if  ( appFinishDrawnPulldownInplace( &(st->stPatternPulldown) ) )
	    { LDEB(1); return;	}

	appGuiSetDrawnPulldownStrips( &(st->stPatternPulldown), PSshd_COUNT );
	}

    /**/
    if  ( st->stBackLabel )
	{
	appFinishColorChooser( &(st->stBackColorChooser),
						    psfl, st->stBackLabel );
	}
    else{
	appFinishColorChooser( &(st->stBackColorChooser),
						    psfl, st->stOnOffToggle );
	}

    if  ( st->stForeColorChooser.ccFilled )
	{
	appFinishColorChooser( &(st->stForeColorChooser),
						    psfl, st->stForeLabel );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Retrieve the shading from the shading tool.				*/
/*									*/
/*  The properties managed by the toggle and the various option menus	*/
/*  or pulldowns are already stored in st->stShadingChosen.		*/
/*  The typing callback sets the changed flag for the shading level.	*/
/*									*/
/*  Retrieve the shading level if its changed flag is set.		*/
/*									*/
/************************************************************************/

static int tedShadingToolGetShading(
				ExpandedItemShading *	eisSet,
				PropertyMask *		pSetMask,
				const ShadingTool *	st )
    {
    const ExpandedItemShading *	eisChosen= &(st->stShadingChosen);

    int				changed= 0;

    const int			minValue= 1;
    const int			adaptToMin= 0;
    const int			maxValue= 100;
    const int			adaptToMax= 0;

    int				level= eisChosen->eisLevel/ 100;

    PropertyMask		isDoneMask;

    if  ( st->stLevelText					&&
	  PROPmaskISSET( &(st->stShadingChanged), ISpropLEVEL )	&&
	  eisChosen->eisPattern == DOCspSOLID			)
	{
	int sav= level;

	if  ( appGetIntegerFromTextWidget( st->stLevelText, &level,
			    minValue, adaptToMin, maxValue, adaptToMax ) )
	    { return -1;	}

	if  ( level != sav )
	    { changed= 1;	}
	}

    docUpdateExpandedItemShading( &isDoneMask, eisSet,
					&(st->stShadingChanged), eisChosen );

    if  ( PROPmaskISSET( &(st->stShadingChanged), ISpropLEVEL )	&&
	  changed						)
	{ eisSet->eisLevel= 100* level; }

    *pSetMask= st->stShadingChanged;

    return 0;
    }

int tedShadingToolGetShadingNumber(	int *			pNum,
					PropertyMask *		isSetMask,
					const ShadingTool *	st,
					BufferDocument *	bd )
    {
    DocumentProperties *	dp= &(bd->bdProperties);
    int				num;

    ItemShading			is;
    ExpandedItemShading		eis;

    docInitExpandedItemShading( &eis );

    if  ( tedShadingToolGetShading( &eis, isSetMask, st ) )
	{ LDEB(1); return -1;	}

    docGetItemShadingByNumber( &is, bd, *pNum );

    if  ( ! utilPropMaskIsEmpty( isSetMask ) )
	{
	PropertyMask	isDoneMask;

	utilPropMaskClear( &isDoneMask );

	if  ( docIndirectItemShading( &isDoneMask, &is, isSetMask, &eis,
							dp->dpColorPalette ) )
	    { LDEB(1); return -1;	}
	}

    num= docItemShadingNumber( bd, &is );
    if  ( num < 0 )
	{ LDEB(num); return -1;	}

    *pNum= num;
    return 0;
    }

/************************************************************************/
/*									*/
/*  Retrieve the shading for a table rectangle from a shading tool.	*/
/*									*/
/************************************************************************/

int tedFormatToolGetCellShading(	CellProperties *	cp,
					BufferDocument *	bd,
					ShadingTool *		st )
    {
    PropertyMask		isSetMask;

    utilPropMaskClear( &isSetMask );

    /*  1  */
    if  ( tedShadingToolGetShadingNumber( &(cp->cpShadingNumber), &isSetMask,
								    st, bd  ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

