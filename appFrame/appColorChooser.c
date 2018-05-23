/************************************************************************/
/*									*/
/*  Simple color chooser on an inspector page.				*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<stddef.h>
#   include	<string.h>

#   include	"appColorChooser.h"
#   include	"guiWidgets.h"

#   include	"guiWidgetDrawingSurface.h"
#   include	"guiDrawingWidget.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Set the color of a ColorChooser.					*/
/*									*/
/************************************************************************/

void appColorChooserSetColor(	ColorChooser *		cc,
				int			colorExplicit,
				const RGB8Color *	rgb8 )
    {
    int		changed= 0;

    if  ( ! cc->ccColorSet )
	{
	cc->ccColorSet= 1;
	changed= 1;
	}

    if  ( cc->ccColorExplicit )
	{
	if  ( colorExplicit )
	    {
	    if  ( bmRGB8ColorsDiffer( &(cc->ccColorChosen), rgb8 ) )
		{
		cc->ccColorChosen= *rgb8;
		changed= 1;
		}
	    }
	else{
	    cc->ccColorExplicit= 0;
	    changed= 1;
	    }
	}
    else{
	if  ( colorExplicit )
	    {
	    cc->ccColorExplicit= 1;
	    cc->ccColorChosen= *rgb8;
	    changed= 1;
	    }
	}

    if  ( changed )
	{
	appExposeDrawnPulldownInplace( &(cc->ccPulldown) );
	}

    return;
    }

void appColorChooserUnset(	ColorChooser *	cc )
    {
    if  ( cc->ccColorSet )
	{
	cc->ccColorSet= 0;

	appExposeDrawnPulldownInplace( &(cc->ccPulldown) );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Redraw the 'Inplace' part of a Color Chooser.			*/
/*									*/
/************************************************************************/

static APP_EVENT_HANDLER_H( appColorChooserRedrawInplace, w, voidcc, exposeEvent )
    {
    ColorChooser *	cc= (ColorChooser *)voidcc;
    DrawingSurface	dsI= cc->ccPulldown.adpInplaceDrawingSurface;

    int			wide;
    int			high;

    DocumentRectangle	drClip;
    DocumentRectangle	drI;

    guiCollectExposures( &drClip, cc->ccPulldown.adpInplaceDrawing,
								exposeEvent );

    drawSetClipRect( dsI, &drClip );

    /* Also draws background */
    appDrawnPulldownDrawArrow( &drClip, &wide, &high,
						w, &(cc->ccPulldown) );

    drawNoClipping( dsI );

    if  ( ! cc->ccVisible )
	{ return;	}

    drI.drX0= 3; drI.drX1= wide- 3;
    drI.drY0= 3; drI.drY1= high- 3;

    if  ( ! cc->ccColorSet )
	{
	drawSetForegroundColorWhite( dsI );
	drawFillRectangle( dsI, &drI );

	drawSetForegroundColorBlack( dsI );
	drawRectangle( dsI, &drI );

	drawLine( dsI, drI.drX0, drI.drY0, drI.drX1, drI.drY1 );
	drawLine( dsI, drI.drX0, drI.drY1, drI.drX1, drI.drY0 );

	return;
	}

    if  ( ! cc->ccColorExplicit )
	{
	const ColorChooserResources *	ccr= cc->ccResources;
	const int			row= 0;

	drawSetForegroundColorWhite( dsI );
	drawFillRectangle( dsI, &drI );

	drawSetForegroundColorBlack( dsI );

	drawString( dsI, 
		    cc->ccXShift+ 4, ( row+ 1 )* cc->ccStripHigh- 7,
		    cc->ccInplaceScreenFont,
		    ccr->ccrAutomaticColor,
		    strlen( ccr->ccrAutomaticColor ) );

	return;
	}

    /**/
    drawSetForegroundColor( dsI, &(cc->ccColorChosen) );
    drawFillRectangle( dsI, &drI );

    /**/
    drawSetForegroundColorBlack( dsI );
    drawRectangle( dsI, &drI );

    return;
    }

/************************************************************************/

static int appColorChooserGetColor(	ColorChooser *		cc,
					RGB8Color **		pRgb8,
					int			row,
					int			col )
    {
    int				color;
    ColorChooserPaletteColor *	ccpc;

    color= ( row- cc->ccHasAutomatic )* cc->ccColumns+ col;
    if  ( color < 0 || color >= cc->ccColorCount )
	{ LLDEB(color,cc->ccColorCount); return 1;	}

    ccpc= cc->ccColors+ color;
    if  ( ccpc->ccpcStatus == CCstatusFREE )
	{ /*LDEB(ccpc->ccpcStatus);*/ return 1;	}
    *pRgb8= &(ccpc->ccpcRGB8Color);

    return 0;
    }

/************************************************************************/

static int appColorChooserAllocatePullDownDrawingData(
					ColorChooser *			cc )
    {
    AppDrawnPulldown *	adp= &(cc->ccPulldown);

    if  ( appFinishDrawnPulldownPulldown( adp ) )
	{ LDEB(1); return -1;	}

    cc->ccPulldownBackgroundColor.rgb8Red= 128;
    cc->ccPulldownBackgroundColor.rgb8Green= 128;
    cc->ccPulldownBackgroundColor.rgb8Blue= 128;
    cc->ccPulldownBackgroundColor.rgb8Alpha= 255;

    if  ( cc->ccPulldownScreenFont < 0 )
	{
	cc->ccPulldownScreenFont= guiGetLabelFont(
			    cc->ccPulldown.adpPulldownDrawingSurface,
			    cc->ccPostScriptFontList,
			    cc->ccLabelWidget );
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Redraw the 'Pulldown' part of a Color Chooser.			*/
/*									*/
/************************************************************************/

static APP_EVENT_HANDLER_H( appColorChooserRedrawPulldown, w, voidcc, exposeEvent )
    {
    ColorChooser *	cc= (ColorChooser *)voidcc;
    DrawingSurface	dsP;

    int			inplaceWide;
    int			inplaceHigh;
    int			pulldownWide;
    int			pulldownHigh;

    DocumentRectangle	drClip;

    int			row;
    int			col;

    int			currCol= -1;
    int			currRow= -1;

    DocumentRectangle	drPulldown;
    DocumentRectangle	drBox;

    if  ( appColorChooserAllocatePullDownDrawingData( cc ) )
	{ LDEB(1); return;	}
    dsP= cc->ccPulldown.adpPulldownDrawingSurface;

    guiDrawGetSizeOfWidget( &pulldownWide, &pulldownHigh, w );
    guiDrawGetSizeOfWidget( &inplaceWide, &inplaceHigh,
					cc->ccPulldown.adpInplaceDrawing );
    drPulldown.drX0= 0;
    drPulldown.drY0= 0;
    drPulldown.drX1= pulldownWide- 1;
    drPulldown.drY1= pulldownHigh- 1;

    guiCollectExposures( &drClip, cc->ccPulldown.adpPulldownDrawing,
								exposeEvent );

    drawSetForegroundColor( dsP, &(cc->ccPulldownBackgroundColor) );

    drawFillRectangle( dsP, &drClip );

    drawSetForegroundColorBlack( dsP );
    drawRectangle( dsP, &drPulldown );

    /**/
    if  ( cc->ccPulldownScreenFont < 0 )
	{ LDEB(cc->ccPulldownScreenFont);	}
    else{
	const ColorChooserResources *	ccr= cc->ccResources;

	if  ( cc->ccHasAutomatic )
	    {
	    row= 0;
	    drawString( dsP, 
			cc->ccXShift+ 4, ( row+ 1 )* cc->ccStripHigh- 5,
			cc->ccPulldownScreenFont,
			ccr->ccrAutomaticColor,
			strlen( ccr->ccrAutomaticColor ) );
	    }

	row= cc->ccStrips- 1;
	drawString( dsP, 
		    cc->ccXShift+ 4, ( row+ 1 )* cc->ccStripHigh- 8,
		    cc->ccPulldownScreenFont,
		    ccr->ccrMoreColors, strlen( ccr->ccrMoreColors ) );
	}

    for ( row= cc->ccHasAutomatic; row < cc->ccStrips- 1; row++ )
	{
	drBox.drY0= row* cc->ccStripHigh+ 3;
	drBox.drY1= drBox.drY0+ cc->ccStripHigh- 7;

	for ( col= 0; col < cc->ccColumns; col++ )
	    {
	    RGB8Color *			rgb8;

	    drBox.drX0= cc->ccXShift+ col* cc->ccColumnWide+ 3;
	    drBox.drX1= drBox.drX0+ cc->ccColumnWide- 7;

	    if  ( appColorChooserGetColor( cc, &rgb8, row, col ) )
		{ continue;	}

	    drawSetForegroundColor( dsP, rgb8 );
	    drawFillRectangle( dsP, &drBox );

	    if  ( cc->ccColorExplicit					&&
		  cc->ccColorSet					&&
		  rgb8->rgb8Red == cc->ccColorChosen.rgb8Red		&&
		  rgb8->rgb8Green == cc->ccColorChosen.rgb8Green	&&
		  rgb8->rgb8Blue == cc->ccColorChosen.rgb8Blue		)
		{ currCol= col; currRow= row;	}
	    }
	}

    /**/
    drawSetForegroundColorBlack( dsP );

    drawSetLineAttributes( dsP, 1,
				LineStyleSolid, LineCapButt, LineJoinMiter, 
				(const unsigned char *)0, 0 );

    for ( row= cc->ccHasAutomatic; row < cc->ccStrips- 1; row++ )
	{
	drBox.drY0= row* cc->ccStripHigh+ 3;
	drBox.drY1= drBox.drY0+ cc->ccStripHigh- 7;

	for ( col= 0; col < cc->ccColumns; col++ )
	    {
	    drBox.drX0= cc->ccXShift+ col* cc->ccColumnWide+ 3;
	    drBox.drX1= drBox.drX0+ cc->ccColumnWide- 7;

	    drawRectangle( dsP, &drBox );
	    }
	}

    if  ( currRow >= 0 && currCol >= 0 )
	{
	drBox.drY0= currRow* cc->ccStripHigh+ 3;
	drBox.drY1= drBox.drY0+ cc->ccStripHigh- 7;

	drBox.drX0= cc->ccXShift+ currCol* cc->ccColumnWide+ 3;
	drBox.drX1= drBox.drX0+ cc->ccColumnWide- 7;

	drawSetLineAttributes( dsP, 3,
				LineStyleSolid, LineCapButt, LineJoinMiter, 
				(const unsigned char *)0, 0 );

	drawRectangle( dsP, &drBox );

	drawSetForegroundColorWhite( dsP );
	drawSetLineAttributes( dsP, 1,
				LineStyleSolid, LineCapButt, LineJoinMiter, 
				(const unsigned char *)0, 0 );

	drBox.drX0--; drBox.drX1++;
	drBox.drY0--; drBox.drY1++;

	drawRectangle( dsP, &drBox );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  The user clicked in the ColorChooser puldown.			*/
/*									*/
/*  1)  Determine strip.						*/
/*  2)  Special cases..							*/
/*  3)  Determine color.						*/
/*  4)  Remember color							*/
/*  5)  Provoke a redraw of the image					*/
/*  6)  Tell target about the change.					*/
/*									*/
/************************************************************************/

static APP_EVENT_HANDLER_H( appColorChooserClickedPulldown, w, voidcc, mouseEvent )
    {
    ColorChooser *		cc= (ColorChooser *)voidcc;

    int				row;
    int				col;
    int				color;

    int				mouseX;
    int				mouseY;

    int				button;
    int				upDown;
    int				seq;
    unsigned int		keyState= 0;

    ColorChooserPaletteColor *	ccpc;

    /*  1  */
    if  ( appGuiDrawnPulldownGetStrip( &row,
					&(cc->ccPulldown), w, mouseEvent ) )
	{ return;	}

    /*  2  */
    if  ( cc->ccHasAutomatic && row == 0 )
	{
	if  ( cc->ccColorExplicit )
	    {
	    cc->ccColorExplicit= 0;
	    appExposeDrawnPulldownInplace( &(cc->ccPulldown) );
	    }

	if  ( cc->ccCallback )
	    {
	    (*cc->ccCallback)( cc, cc->ccWhich, cc->ccTarget,
				    CHOICEccDEFAULT, (const RGB8Color *)0 );
	    }

	return;
	}

    if  ( row == cc->ccStrips- 1 )
	{
	if  ( cc->ccCallback )
	    {
	    (*cc->ccCallback)( cc, cc->ccWhich, cc->ccTarget,
				    CHOICEccMORE, (const RGB8Color *)0 );
	    }

	return;
	}

    if  ( guiGetCoordinatesFromMouseButtonEvent( &mouseX, &mouseY,
					    &button, &upDown, &seq, &keyState,
					    w, mouseEvent ) )
	{ return;	}

    col= mouseX/ cc->ccColumnWide;
    if  ( col < 0 )
	{ col= 0;	}
    if  ( col >= cc->ccColumns )
	{ col= cc->ccColumns- 1;	}

    color= ( row- cc->ccHasAutomatic )* cc->ccColumns+ col;
    if  ( color < 0 || color >= cc->ccColorCount )
	{ LLDEB(color,cc->ccColorCount); return;	}

    ccpc= cc->ccColors+ color;
    if  ( ccpc->ccpcStatus == CCstatusFREE )
	{ LDEB(ccpc->ccpcStatus); return;	}

    if  ( cc->ccCallback )
	{
	const int	colorExplicit= 1;

	appColorChooserSetColor( cc, colorExplicit, &(ccpc->ccpcRGB8Color) );

	(*cc->ccCallback)( cc, cc->ccWhich, cc->ccTarget,
				CHOICEccCOLOR, &(ccpc->ccpcRGB8Color) );
	}

    return;
    }


/************************************************************************/
/*									*/
/*  Suggest a palette for the color chooser.				*/
/*									*/
/************************************************************************/

static int appColorChooserFindColor(	const ColorChooser *	cc,
					const RGB8Color *	rgb8 )
    {
    int					i;
    const ColorChooserPaletteColor *	ccpc;

    ccpc= cc->ccColors;
    for ( i= 0; i < cc->ccColorCount; ccpc++, i++ )
	{
	if  ( ccpc->ccpcStatus == CCstatusFREE )
	    { continue;	}

	if  ( bmRGB8ColorsDiffer( &(ccpc->ccpcRGB8Color), rgb8 ) )
	    { continue;	}

	return i;
	}

    return -1;
    }

static int appColorChooserAllocateColor(	const ColorChooser *	cc,
						const RGB8Color *	rgb8,
						int			status )
    {
    int					i;
    ColorChooserPaletteColor *		ccpc;

    ccpc= cc->ccColors;
    for ( i= 0; i < cc->ccColorCount; ccpc++, i++ )
	{
	if  ( ccpc->ccpcStatus != CCstatusFREE )
	    { continue;	}

	ccpc->ccpcRGB8Color= *rgb8;
	ccpc->ccpcStatus= status;

	return i;
	}

    return -1;
    }

static int appColorChooserFindNearest(	const RGB8Color *	colors,
					int			colorCount,
					int			dist,
					const RGB8Color *	rgb8 )
    {
    int					i;
    int					found= -1;
    int					d= 256;

    for ( i= 0; i < colorCount; colors++, i++ )
	{
	int	dd= 0;
	int	dc;

	dc= rgb8->rgb8Red- colors->rgb8Red;
	if  ( dc < 0 )
	    { dc= -dc;	}
	if  ( dd < dc )
	    { dd=  dc; }
	if  ( dd > dist || dd > d )
	    { continue;	}

	dc= rgb8->rgb8Green- colors->rgb8Green;
	if  ( dc < 0 )
	    { dc= -dc;	}
	if  ( dd < dc )
	    { dd=  dc; }
	if  ( dd > dist || dd > d )
	    { continue;	}

	dc= rgb8->rgb8Blue- colors->rgb8Blue;
	if  ( dc < 0 )
	    { dc= -dc;	}
	if  ( dd < dc )
	    { dd=  dc; }
	if  ( dd > dist || dd > d )
	    { continue;	}

	d= dd;
	found= i;

	if  ( d == 0 )
	    { break;	}
	}

    return found;
    }

/************************************************************************/
/*									*/
/*  Try to allocate a color for the palette.				*/
/*									*/
/************************************************************************/

static int appColorChooserTryColor(	ColorChooser *		cc,
					const RGB8Color *	colors,
					int			colorCount,
					int			dist,
					int			r,
					int			g,
					int			b )
    {
    int			found;
    int			status;
    RGB8Color		rgb8;

    rgb8.rgb8Red= r;
    rgb8.rgb8Green= g;
    rgb8.rgb8Blue= b;
    rgb8.rgb8Alpha= 255;

    status= CCstatusSYSTEM;

    found= appColorChooserFindNearest( colors, colorCount, dist, &rgb8 );
    if  ( found >= 0 && bmRGB8ColorsDiffer( &rgb8, &(colors[found]) ) )
	{
	rgb8= colors[found];
	status= CCstatusPALETTE;
	}

    if  ( appColorChooserFindColor( cc, &rgb8 ) >= 0 )
	{ return 0;	}

    if  ( appColorChooserAllocateColor( cc, &rgb8, status ) < 0 )
	{ return 1;	}

    return 0;
    }

static int appColorChooserCompareIntensity(	const void *	vccpc1,
						const void *	vccpc2 )
    {
    const ColorChooserPaletteColor *	ccpc1;
    const ColorChooserPaletteColor *	ccpc2;
    
    int					luma1= 0, luma2= 0;
    int					chroma1= 0, chroma2= 0;
    int					hue1= 0, hue2= 0;

    ccpc1= (const ColorChooserPaletteColor *)vccpc1;
    ccpc2= (const ColorChooserPaletteColor *)vccpc2;

    if  ( ccpc1->ccpcStatus == CCstatusFREE	&&
	  ccpc2->ccpcStatus == CCstatusFREE	)
	{ return  0;	}
    if  ( ccpc1->ccpcStatus != CCstatusFREE	&&
	  ccpc2->ccpcStatus == CCstatusFREE	)
	{ return -1;	}
    if  ( ccpc1->ccpcStatus == CCstatusFREE	&&
	  ccpc2->ccpcStatus != CCstatusFREE	)
	{ return  1;	}

    utilRGB8LumaChromaHue( &luma1, &chroma1, &hue1, &(ccpc1->ccpcRGB8Color) );
    utilRGB8LumaChromaHue( &luma2, &chroma2, &hue2, &(ccpc2->ccpcRGB8Color) );

    if  ( luma1 > luma2 )
	{ return  1;	}
    if  ( luma1 < luma2 )
	{ return -1;	}

    return 0;
    }

static int appColorChooserCompareHue(		const void *	vccpc1,
						const void *	vccpc2 )
    {
    const ColorChooserPaletteColor *	ccpc1;
    const ColorChooserPaletteColor *	ccpc2;

    int					luma1= 0, luma2= 0;
    int					chroma1= 0, chroma2= 0;
    int					hue1= 0, hue2= 0;

    ccpc1= (const ColorChooserPaletteColor *)vccpc1;
    ccpc2= (const ColorChooserPaletteColor *)vccpc2;

    if  ( ccpc1->ccpcStatus == CCstatusFREE	&&
	  ccpc2->ccpcStatus == CCstatusFREE	)
	{ return  0;	}
    if  ( ccpc1->ccpcStatus != CCstatusFREE	&&
	  ccpc2->ccpcStatus == CCstatusFREE	)
	{ return -1;	}
    if  ( ccpc1->ccpcStatus == CCstatusFREE	&&
	  ccpc2->ccpcStatus != CCstatusFREE	)
	{ return  1;	}

    utilRGB8LumaChromaHue( &luma1, &chroma1, &hue1, &(ccpc1->ccpcRGB8Color) );
    utilRGB8LumaChromaHue( &luma2, &chroma2, &hue2, &(ccpc2->ccpcRGB8Color) );

    if  ( hue1 > hue2 )
	{ return  1;	}
    if  ( hue1 < hue2 )
	{ return -1;	}

    if  ( luma1 > luma2 )
	{ return  1;	}
    if  ( luma1 < luma2 )
	{ return -1;	}

    if  ( chroma1 > chroma2 )
	{ return  1;	}
    if  ( chroma1 < chroma2 )
	{ return -1;	}

    return 0;
    }

static int appColorChooseMoreColors(	ColorChooser *		cc,
					const RGB8Color *	colors,
					int			colorCount,
					int *			rs,
					int			rc,
					int *			gs,
					int			gc,
					int *			bs,
					int			bc )
    {
    int	r;
    int	g;
    int	b;

    int	d= 30;

    for ( b= 0; b < bc; b++ )
	{
	for ( r= 0; r < rc; r++ )
	    {
	    for ( g= 0; g < gc; g++ )
		{
		if  ( appColorChooserTryColor( cc, colors, colorCount,
						    d, rs[r], gs[g], bs[b] ) )
		    { return 1;	}
		}
	    }
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Suggest a palette for the color chooser.				*/
/*									*/
/*  1)  Remove colors that came from the previous palette.		*/
/*  2)  Make sure that we have at least some grays in the palette.	*/
/*  3)  Try to get the corners of the color cube.			*/
/*									*/
/************************************************************************/

void appColorChooserSuggestPalette(	ColorChooser *		cc,
					int			avoidZero,
					const ColorPalette *	cp )
    {
    ColorChooserPaletteColor *	ccpc;

    int				useColors;
    int				i;

    const int			dist= 256/ 12;

    int				colorCount= cp->cpColorCount;
    const RGB8Color *		colors= cp->cpColors;

    if  ( avoidZero && colorCount > 0 )
	{ colors++; colorCount--; }

    useColors= cc->ccColumns* ( cc->ccStrips- cc->ccHasAutomatic- 1 );

    if  ( cc->ccColorCount < useColors )
	{
	ccpc= (ColorChooserPaletteColor *)realloc( cc->ccColors,
			    useColors* sizeof( ColorChooserPaletteColor ) );

	if  ( ! ccpc )
	    { LXDEB(useColors,ccpc); return;	}

	cc->ccColors= ccpc;
	ccpc += cc->ccColorCount;

	while( cc->ccColorCount < useColors )
	    {
	    ccpc->ccpcStatus= CCstatusFREE;
	    ccpc++; cc->ccColorCount++;
	    }
	}

    /*  1  */
    ccpc= cc->ccColors;
    for ( i= 0; i < cc->ccColorCount; ccpc++, i++ )
	{
	if  ( ccpc->ccpcStatus == CCstatusPALETTE )
	    { ccpc->ccpcStatus= CCstatusFREE;	}
	}

    /*  3  */
    if  ( appColorChooserTryColor( cc, colors, colorCount, dist, 0,0,0 ) )
	{ goto ready;	}
    if  ( appColorChooserTryColor( cc, colors, colorCount, dist, 255,255,255 ) )
	{ goto ready;	}
    if  ( appColorChooserTryColor( cc, colors, colorCount, dist, 255,0,0 ) )
	{ goto ready;	}
    if  ( appColorChooserTryColor( cc, colors, colorCount, dist, 0,255,0 ) )
	{ goto ready;	}
    if  ( appColorChooserTryColor( cc, colors, colorCount, dist, 0,0,255 ) )
	{ goto ready;	}
    if  ( appColorChooserTryColor( cc, colors, colorCount, dist, 0,255,255 ) )
	{ goto ready;	}
    if  ( appColorChooserTryColor( cc, colors, colorCount, dist, 255,0,255 ) )
	{ goto ready;	}
    if  ( appColorChooserTryColor( cc, colors, colorCount, dist, 255,255,0 ) )
	{ goto ready;	}

    /*  2  */
    for ( i= 0; i < 6; i++ )
	{
	int	c= ( 255* i )/ 5;

	if  ( appColorChooserTryColor( cc, colors, colorCount,
							dist, c, c, c ) )
	    { goto ready;	}
	}

    /*  4  */
    {
    int	bb0[]= { 0, 255 };
    int	bb1[]= { 0, 153, 255 };

    int	gg0[]= { 0, 153, 255 };
    int	gg1[]= { 0, 102, 204, 255 };

    int	rr0[]= { 0, 153, 255 };
    int	rr1[]= { 0, 102, 204, 255 };

    int	all[]= { 0, 53, 102, 153, 204, 255 };

    if  ( appColorChooseMoreColors( cc, colors, colorCount,
					    rr0, sizeof(rr0)/ sizeof(int),
					    gg0, sizeof(gg0)/ sizeof(int),
					    bb0, sizeof(bb0)/ sizeof(int) ) )
	{ goto ready;	}

    if  ( appColorChooseMoreColors( cc, colors, colorCount,
					    rr1, sizeof(rr1)/ sizeof(int),
					    gg1, sizeof(gg1)/ sizeof(int),
					    bb1, sizeof(bb1)/ sizeof(int) ) )
	{ goto ready;	}

    if  ( appColorChooseMoreColors( cc, colors, colorCount,
					    all, sizeof(all)/ sizeof(int),
					    all, sizeof(all)/ sizeof(int),
					    all, sizeof(all)/ sizeof(int) ) )
	{ goto ready;	}
    }

  ready:

    qsort( cc->ccColors, cc->ccColorCount, 
				    sizeof(ColorChooserPaletteColor),
				    appColorChooserCompareIntensity );

    for ( i= 0; i < cc->ccColorCount; i += cc->ccColumns )
	{
	qsort( cc->ccColors+ i, cc->ccColumns, 
				    sizeof(ColorChooserPaletteColor),
				    appColorChooserCompareHue );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Make a ColorChooser.						*/
/*									*/
/************************************************************************/

void appCleanColorChooser(		ColorChooser *		cc )
    {
    appCleanDrawnPulldown( &(cc->ccPulldown) );

    if  ( cc->ccColors )
	{ free( cc->ccColors );	}

    return;
    }

void appFinishColorChooser(	ColorChooser *			cc,
				const PostScriptFontList *	psfl,
				APP_WIDGET			fontWidget )
    {
    int			wide;
    int			strips;

#   ifdef USE_GTK /* Crashes with Motif */
    if  ( appFinishDrawnPulldownPulldown( &(cc->ccPulldown) ) )
	{ LDEB(1);	}
#   endif

    if  ( appFinishDrawnPulldownInplace( &(cc->ccPulldown) ) )
	{ LDEB(1);	}

    guiDrawGetSizeOfWidget( &wide, &(cc->ccStripHigh),
					    cc->ccPulldown.adpInplaceDrawing );

    cc->ccColumnWide= cc->ccStripHigh;
    strips= wide/ cc->ccStripHigh;
    cc->ccStrips= strips+ 2;
    cc->ccColumns= strips;

    cc->ccXShift= ( wide- cc->ccColumns* cc->ccColumnWide )/ 2;

    appGuiSetDrawnPulldownStrips( &(cc->ccPulldown), cc->ccStrips );

    cc->ccPostScriptFontList= psfl;
    cc->ccLabelWidget= fontWidget;

    /**/
    cc->ccInplaceScreenFont= guiGetLabelFont(
		cc->ccPulldown.adpInplaceDrawingSurface, psfl, fontWidget );
#   ifdef USE_GTK /* Crashes with Motif */
    cc->ccPulldownScreenFont= guiGetLabelFont(
		cc->ccPulldown.adpPulldownDrawingSurface, psfl, fontWidget );
#   endif

    return;
    }

void appInitColorChooser(	ColorChooser *			cc )
    {
    cc->ccFilled= 0;
    cc->ccEnabled= 1;
    cc->ccVisible= 1;

    cc->ccResources= (const ColorChooserResources *)0;
    cc->ccPostScriptFontList= (const PostScriptFontList *)0;

    appInitDrawnPulldown( &(cc->ccPulldown) );

    cc->ccLabelWidget= (APP_WIDGET)0;

    cc->ccInplaceScreenFont= -1;
    cc->ccPulldownScreenFont= -1;

    utilInitRGB8Color( &(cc->ccColorChosen) );
    cc->ccColorExplicit= 0;
    cc->ccColorSet= 0;

    cc->ccCallback= (ColorChooserCallback)0;
    cc->ccTarget= (void *)0;
    cc->ccWhich= -1;

    cc->ccStripHigh= 0;
    cc->ccColumnWide= 0;
    cc->ccStrips= 0;
    cc->ccColumns= 0;
    cc->ccXShift= 0;

    cc->ccColors= (ColorChooserPaletteColor *)0;
    cc->ccColorCount= 0;
    }

void appMakeColorChooserInRow(	ColorChooser *			cc,
				int				hasAutomatic,
				APP_WIDGET			row,
				int				col,
				int				colspan,
				const ColorChooserResources *	ccr,
				ColorChooserCallback		callback,
				int				which,
				void *				through )
    {
    AppDrawnPulldownPuldown	pullDown= (AppDrawnPulldownPuldown)0;

#   if 0
#   ifdef USE_GTK
    pullDown= appColorChooserAllocatePulldownResources;
#   endif
#   endif

    /**/
    cc->ccResources= ccr;

    cc->ccCallback= callback;
    cc->ccWhich= which;
    cc->ccTarget= through;

    appMakeDrawnPulldownInRow( &(cc->ccPulldown),
				    appColorChooserRedrawInplace,
				    appColorChooserRedrawPulldown,
				    appColorChooserClickedPulldown,
				    pullDown,
				    row, col, colspan,
				    (void *)cc );

    /**/
    cc->ccFilled= 1;
    cc->ccHasAutomatic= hasAutomatic;

    return;
    }

/************************************************************************/
/*									*/
/*  Make a row consisting of a label and a color chooser.		*/
/*									*/
/************************************************************************/

void appMakeLabelAndColorChooserRow(
				APP_WIDGET *			pRow,
				APP_WIDGET *			pLabel,
				ColorChooser *			cc,
				int				hasAutomatic,
				APP_WIDGET			column,
				const char *			labelText,
				const ColorChooserResources *	ccr,
				ColorChooserCallback		colorCallback,
				int				which,
				void *				through )
    {
    APP_WIDGET		label;
    APP_WIDGET		row;

    const int		labelColumn= 0;
    const int		labelColspan= 1;
    const int		ccColumn= 1;
    const int		ccColspan= 1;

    const int		columnCount= 2;
    const int		heightResizable= 0;

    row= appMakeRowInColumn( column, columnCount, heightResizable );

    appMakeLabelInRow( &label, row, labelColumn, labelColspan, labelText );
    appMakeColorChooserInRow( cc, hasAutomatic, row, ccColumn, ccColspan,
					ccr, colorCallback, which, through );

    *pRow= row; *pLabel= label; return;
    }

/************************************************************************/
/*									*/
/*  Make a row consisting of a toggle and a color chooser.		*/
/*									*/
/************************************************************************/

void appMakeToggleAndColorChooserRow(
				APP_WIDGET *			pRow,
				APP_WIDGET *			pToggle,
				ColorChooser *			cc,
				int				hasAutomatic,
				APP_WIDGET			column,
				const char *			toggleText,
				const ColorChooserResources *	ccr,
				APP_TOGGLE_CALLBACK_T		toggleCallback,
				ColorChooserCallback		colorCallback,
				int				which,
				void *				through )
    {
    APP_WIDGET		toggle;
    APP_WIDGET		row;

    const int		toggleColumn= 0;
    const int		toggleColspan= 1;
    const int		ccColumn= 1;
    const int		ccColspan= 1;

    const int		columnCount= 2;
    const int		heightResizable= 0;

    row= appMakeRowInColumn( column, columnCount, heightResizable );

    toggle= appMakeToggleInRow( row, toggleText, toggleCallback,
				    through, toggleColumn, toggleColspan );
    appMakeColorChooserInRow( cc, hasAutomatic, row, ccColumn, ccColspan,
					ccr, colorCallback, which, through );

    *pRow= row; *pToggle= toggle; return;
    }

/************************************************************************/
/*									*/
/*  Utility Routine: Update a color and a changed mask from a callback	*/
/*  of a color chooser.							*/
/*									*/
/************************************************************************/

void appColorChooserColorChosen(	PropertyMask *		isSetMask,
					int *			pChanged,
					RGB8Color *		rgb8To,
					int *			pExplicit,
					const RGB8Color *	rgb8Set,
					int			colorExplicit,
					int			which )
    {
    if  ( colorExplicit )
	{
	if  ( ! *pExplicit )
	    {
	    PROPmaskADD( isSetMask, which );
	    *pChanged= 1;

	    *pExplicit= 1;
	    *rgb8To= *rgb8Set;

	    return;
	    }
	else{
	    if  ( bmRGB8ColorsDiffer( rgb8To, rgb8Set ) )
		{
		PROPmaskADD( isSetMask, which );
		*pChanged= 1;

		*rgb8To= *rgb8Set;
		}
	    }
	}
    else{
	if  ( *pExplicit )
	    {
	    PROPmaskADD( isSetMask, which );
	    *pChanged= 1;

	    *pExplicit= 0;
	    }
	}

    return;
    }

void appEnableColorChooser(	ColorChooser *		cc,
				int			enabled )
    {
    if  ( cc->ccEnabled != enabled )
	{
	cc->ccEnabled= enabled;

	appGuiEnableDrawnPulldown( &(cc->ccPulldown),
					    cc->ccEnabled && cc->ccVisible );
	}
    }

void appShowColorChooser(	ColorChooser *		cc,
				int			visible )
    {
    if  ( cc->ccVisible != visible )
	{
	cc->ccVisible= visible;

	appGuiEnableDrawnPulldown( &(cc->ccPulldown),
					    cc->ccEnabled && cc->ccVisible );
	}
    }
