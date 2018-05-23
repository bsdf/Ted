#   include	"tedConfig.h"

#   include	<stdio.h>

#   include	<appMetricRuler.h>
#   include	<appFrame.h>
#   include	<docTabStop.h>
#   include	<guiWidgetDrawingSurface.h>
#   include	<guiDrawingWidget.h>
#   include	"tedDrawRuler.h"

#   include	<appDebugon.h>

#   define	BACK_MARG(s)	((s)/3)
#   define	WHITE_TOP(s)	BACK_MARG(s)
#   define	WHITE_BOTTOM(s)	((s)-BACK_MARG(s))
#   define	WHITE_HEIGHT(s)	(WHITE_BOTTOM(s)-WHITE_TOP(s))

#   define	TAG_DIST(s)	1
#   define	TAG_TOP(s)	(WHITE_TOP(s)+TAG_DIST(s))
#   define	TAG_BOTTOM(s)	(WHITE_BOTTOM(s)-TAG_DIST(s))
#   define	TAG_HEIGHT(s)	(TAG_BOTTOM(s)-TAG_TOP(s))

#   define	FONT_HEIGHT(s)	(WHITE_HEIGHT(s)-2)
#   define	FONT_BOTTOM(s)	(TAG_BOTTOM(s)-1)

#   define	TICK_DIST(s)	((s)/12)
#   define	TICK_TOP(s)	(WHITE_TOP(s)+TICK_DIST(s))
#   define	TICK_BOTTOM(s)	(WHITE_BOTTOM(s)-TICK_DIST(s))
#   define	TICK_HEIGHT(s)	(TICK_BOTTOM(s)-TICK_TOP(s))

#   define	HBUTTON_BASE(s)		BUTTON_DIST(s)
#   define	HBUTTON_APEX(s)		(HBUTTON_BASE(s)+ \
					    BUTTON_RECTH(s)+BUTTON_TRIH(s))

#   define	SBUTTON_BASE(s)		((s)-BUTTON_DIST(s))
#   define	SBUTTON_APEX(s)		(SBUTTON_BASE(s)- \
					    BUTTON_RECTH(s)-BUTTON_TRIH(s))

#   define	BUTTON_DIST(s)		(((s)/6)-1)
#   define	BUTTON_RECTH(s)		((s)/6)
#   define	BUTTON_TRIH(s)		((s)/6)
#   define	BUTTON_HWIDTH(s)	((s)/6)

#   define	BUTTON_MARG		1

#   define	BUTTON_LEFT(v,s)	((v)-BUTTON_HWIDTH((s))-BUTTON_MARG)
#   define	BUTTON_RIGHT(v,s)	((v)+BUTTON_HWIDTH((s))+BUTTON_MARG)

#   define	TAB_HEIGHT(s)		((s)/6)
#   define	TAB_TOP(s)		(WHITE_BOTTOM(s)-TAB_HEIGHT(s))
#   define	TAB_WIDTH(s)		((s)/6)
#   define	TAB_THICK(s)		3

/************************************************************************/
/*									*/
/*  Draw the little button for the 'First Indent' of a paragraph	*/
/*									*/
/*	       v							*/
/*	4 ---- . ---- 0 = 5						*/
/*	|      .      |							*/
/*	|      .      |							*/
/*	3      .      1							*/
/*	 \     .     / 							*/
/*	  \    .    /  							*/
/*	   \   .   /   							*/
/*	    \  .  /    							*/
/*	     \ . /     							*/
/*	       2       							*/
/*									*/
/************************************************************************/

void tedHangingButtonRect(	DocumentRectangle *	drButton,
				int			value,
				const RulerData *	rd )
    {
    int		sizeAcross= rd->rdSizeAcross;

    drButton->drX0= BUTTON_LEFT(value,sizeAcross);
    drButton->drX1= BUTTON_RIGHT(value,sizeAcross);

    drButton->drY0= HBUTTON_BASE(sizeAcross)- 1;
    drButton->drY1= HBUTTON_APEX(sizeAcross)+ 1;

    return;
    }

void tedRulerDrawHangingButton(	int			value,
				const RulerData *	rd )
    {
    int			ox= rd->rdVisibleC0- rd->rdMinUsed;
    DrawingSurface	ds= rd->rdDrawingSurface;
    int			sizeAcross= rd->rdSizeAcross;
    Point2DI		points[6];
    int			w= BUTTON_MARG;

    value -= ox;

    points[0].x= value+ BUTTON_HWIDTH(sizeAcross);
    points[0].y= HBUTTON_BASE(sizeAcross);

    points[1].x= points[0].x;
    points[1].y= HBUTTON_BASE(sizeAcross)+ BUTTON_RECTH(sizeAcross);

    points[2].x= value;
    points[2].y= HBUTTON_APEX(sizeAcross);

    points[3].x= value- BUTTON_HWIDTH(sizeAcross);
    points[3].y= points[1].y;

    points[4].x= points[3].x;
    points[4].y= points[0].y;

    points[5]= points[0];

    drawSetForegroundColor( ds, &(rd->rdBackgroundColor) );

    drawFillPolygon( ds, points, 6 );

    drawSetForegroundColor( ds, &(rd->rdForegroundColor) );
    drawLines( ds, points, 6, 0 );

    points[0].x -= w; points[0].y += (w+1)/2;
    points[1].x -= w;
    points[2].y -= w;
    points[3].x += w;
    points[4].x += w; points[4].y += w;
    points[5].x -= w; points[5].y += w;

    drawSetForegroundColor( ds, &(rd->rdBottomShadowColor) );
    drawLines( ds, points, 4, 0 );
    drawSetForegroundColor( ds, &(rd->rdTopShadowColor) );
    drawLines( ds, points+ 3, 3, 0 );

    return;
    }

/************************************************************************/
/*									*/
/*  Draw the little buttons for the 'Left, Right Indent' of a paragraph	*/
/*									*/
/*	       3   							*/
/*	     / . \     							*/
/*	    /  .  \    							*/
/*	   /   .   \   							*/
/*	  /    .    \  							*/
/*	 /     .     \ 							*/
/*	4      .      2							*/
/*	|      .      |							*/
/*	|      .      |							*/
/*  5 = 0 ---- . ---- 1							*/
/*	       v							*/
/*									*/
/************************************************************************/

void tedStandingButtonRect(	DocumentRectangle *	drButton,
				int			value,
				const RulerData *	rd )
    {
    int		sizeAcross= rd->rdSizeAcross;

    drButton->drX0= BUTTON_LEFT(value,sizeAcross);
    drButton->drX1= BUTTON_RIGHT(value,sizeAcross);

    drButton->drY0= SBUTTON_APEX(sizeAcross)- 1;
    drButton->drY1= SBUTTON_BASE(sizeAcross)+ 1;

    return;
    }

void tedRulerDrawStandingButton(	int			value,
					const RulerData *	rd )
    {
    int			ox= rd->rdVisibleC0- rd->rdMinUsed;
    DrawingSurface	ds= rd->rdDrawingSurface;
    int			sizeAcross= rd->rdSizeAcross;
    Point2DI		points[6];
    int			w= BUTTON_MARG;

    value -= ox;

    points[0].x= value- BUTTON_HWIDTH(sizeAcross);
    points[0].y= SBUTTON_BASE(sizeAcross);

    points[1].x= value+ BUTTON_HWIDTH(sizeAcross);
    points[1].y= points[0].y;

    points[2].x= points[1].x;
    points[2].y= SBUTTON_BASE(sizeAcross)- BUTTON_RECTH(sizeAcross);

    points[3].x= value;
    points[3].y= SBUTTON_APEX(sizeAcross);

    points[4].x= points[0].x;
    points[4].y= points[2].y;

    points[5]= points[0];

    drawSetForegroundColor( ds, &(rd->rdBackgroundColor) );

    drawFillPolygon( ds, points, 6 );

    drawSetForegroundColor( ds, &(rd->rdForegroundColor) );
    drawLines( ds, points, 6, 0 );

    points[0].x += (w+1)/2; points[0].y -= w;
    points[1].x -= w; points[1].y -= w;
    points[2].x -= w;
    points[3].y += (w+1)/2;
    points[4].x += w;
    points[5].x += w; points[5].y -= w;

    drawSetForegroundColor( ds, &(rd->rdBottomShadowColor) );
    drawLines( ds, points, 4, 0 );
    drawSetForegroundColor( ds, &(rd->rdTopShadowColor) );
    drawLines( ds, points+ 3, 3, 0 );

    return;
    }

/************************************************************************/
/*									*/
/*  Draw the little holes for the table columns on the ruler.		*/
/*									*/
/*	3-------------2							*/
/*	|      .      |							*/
/*	|      .      |							*/
/*	|      .      |							*/
/*	|      .      |							*/
/*	|      .      |							*/
/*  4 = 0 ---- . ---- 1							*/
/*	       v							*/
/*									*/
/************************************************************************/

void tedRulerColumnControlRect(	DocumentRectangle *	drButton,
				int			x0,
				int			x1,
				const RulerData *	rd )
    {
    int		sizeAcross= rd->rdSizeAcross;

    if  ( x1- x0 < BUTTON_HWIDTH( sizeAcross ) )
	{
	int	m= ( x1+ x0 )/ 2;

	x0= m- BUTTON_HWIDTH( sizeAcross )/2;
	x1= m+ BUTTON_HWIDTH( sizeAcross )/2;
	}

    drButton->drX0= x0- 1;
    drButton->drX1= x1+ 1;

    drButton->drY0= WHITE_TOP(sizeAcross)- 1;
    drButton->drY1= WHITE_BOTTOM(sizeAcross)+ 1;

    return;
    }

void tedRulerDrawColumnControl(	int			x0,
				int			x1,
				int			ox,
				const RulerData *	rd )
    {
    int			sizeAcross= rd->rdSizeAcross;
    DrawingSurface	ds= rd->rdDrawingSurface;
    Point2DI		points[5];
    int			w= BUTTON_MARG;

    x0 -= ox;
    x1 -= ox;

    if  ( x1- x0 < BUTTON_HWIDTH( sizeAcross ) )
	{
	int	m= ( x1+ x0 )/ 2;

	x0= m- BUTTON_HWIDTH( sizeAcross )/2;
	x1= m+ BUTTON_HWIDTH( sizeAcross )/2;
	}

    points[0].x= x0;
    points[0].y= WHITE_BOTTOM(sizeAcross);

    points[1].x= x1;
    points[1].y= points[0].y;

    points[2].x= points[1].x;
    points[2].y= WHITE_TOP(sizeAcross);

    points[3].x= points[0].x;
    points[3].y= points[2].y;

    points[4]= points[0];

    drawSetForegroundColor( ds, &(rd->rdBackgroundColor) );

    drawFillPolygon( ds, points, 5 );

    drawSetForegroundColor( ds, &(rd->rdForegroundColor) );
    drawLines( ds, points, 5, 0 );

    points[0].x += w; points[0].y -= w;
    points[1].x -= w; points[1].y -= w;
    points[2].x -= w; points[2].y += w;
    points[3].x += w; points[3].y += w;
    points[4].x += w; points[4].y -= w;

    drawSetForegroundColor( ds, &(rd->rdTopShadowColor) );
    drawLines( ds, points, 3, 0 );
    drawSetForegroundColor( ds, &(rd->rdBottomShadowColor) );
    drawLines( ds, points+ 2, 2, 0 );


    return;
    }

/************************************************************************/
/*									*/
/*  Draw a 'Tab' symbol.						*/
/*									*/
/************************************************************************/

static void tedTabSymbolRectY(	DocumentRectangle *	drButton,
				int			tabX,
				int			tabY,
				const RulerData *	rd )
    {
    int		sizeAcross= rd->rdSizeAcross;
    int		thick= TAB_THICK(sizeAcross);
    int		tabWidth= TAB_WIDTH(sizeAcross);
    int		tabHeight= TAB_HEIGHT(sizeAcross);

    drButton->drX0= tabX+ (thick+1)/2- tabWidth;
    drButton->drX1= tabX- thick/2+ tabWidth;

    drButton->drY0= tabY- tabHeight- 1;
    drButton->drY1= tabY+ 1;

    return;
    }

void tedTabSymbolRect(	DocumentRectangle *	drButton,
			int			tabX,
			const RulerData *	rd )
    {
    int		sizeAcross= rd->rdSizeAcross;

    int		whiteY=  WHITE_TOP(sizeAcross);
    int		whiteH= WHITE_HEIGHT(sizeAcross);
    int		tabY= whiteY+ whiteH;

    tedTabSymbolRectY( drButton, tabX, tabY, rd );

    return;
    }

static void tedDrawTabSymbol(	DrawingSurface		ds,
				int			tabX,
				int			ox,
				int			tabY,
				const RulerData *	rd,
				int			kind )
    {
    int		sizeAcross= rd->rdSizeAcross;
    int		thick= TAB_THICK(sizeAcross);
    int		tabWidth= TAB_WIDTH(sizeAcross);
    int		tabHeight= TAB_HEIGHT(sizeAcross);

    DocumentRectangle	dr;

    /*  standing  */
    dr.drX0= tabX- thick/2;
    dr.drX1= dr.drX0+ thick- 1;
    dr.drY0= tabY- tabHeight+ 1;
    dr.drY1= tabY;

    geoShiftRectangle( &dr, -ox, 0 );
    drawFillRectangle( ds, &dr );

    if  ( kind == DOCtaLEFT		||
	  kind == DOCtaCENTER		||
	  kind == DOCtaDECIMAL		)
	{
	/*  laying right  */
	dr.drX0= tabX- thick/2;
	dr.drX1= dr.drX0+ tabWidth- 1;
	dr.drY0= tabY- thick+ 1;
	dr.drY1= tabY;

	geoShiftRectangle( &dr, -ox, 0 );
	drawFillRectangle( ds, &dr );
	}

    if  ( kind == DOCtaRIGHT		||
	  kind == DOCtaCENTER		||
	  kind == DOCtaDECIMAL		)
	{
	/*  laying left	*/
	dr.drX0= tabX+ (thick+1)/2- tabWidth;
	dr.drX1= dr.drX0+ tabWidth- 1;
	dr.drY0= tabY- thick+ 1;
	dr.drY1= tabY;

	geoShiftRectangle( &dr, -ox, 0 );
	drawFillRectangle( ds, &dr );
	}

    if  ( kind == DOCtaDECIMAL	)
	{
	int	dec= ( thick+ 1 )/ 2;

	/*  dot right  */
	dr.drX1= tabX- thick/2+ tabWidth- 1;
	dr.drX0= dr.drX1- dec+ 1;
	dr.drY0= tabY- tabHeight+ 1;
	dr.drY1= dr.drY0+ dec- 1;

	geoShiftRectangle( &dr, -ox, 0 );
	drawFillRectangle( ds, &dr );
	}

    return;
    }

void tedRulerDrawTab(		const RulerData *	rd,
				int			tabX,
				int			kind )
    {
    DrawingSurface	ds= rd->rdDrawingSurface;
    int			ox= rd->rdVisibleC0- rd->rdMinUsed;

    int			sizeAcross= rd->rdSizeAcross;
    int			whiteY= WHITE_TOP(sizeAcross);
    int			whiteH= WHITE_HEIGHT(sizeAcross);

    tedDrawTabSymbol( ds, tabX, ox, whiteY+ whiteH, rd, kind );
    }

void tedTabButtonRectangle(	DocumentRectangle *	drButton,
				int			x,
				const RulerData *	rd )
    {
    int			sizeAcross= rd->rdSizeAcross;
    int			whiteY= WHITE_TOP(sizeAcross);

    tedTabSymbolRectY( drButton, x, whiteY, rd );

    drButton->drX0 -= 3;
    drButton->drX1 += 3;

    drButton->drY0 -= 5;
    drButton->drY1 += 0;

    return;
    }

void tedDrawTabButton(	const RulerData *	rd,
			int			kind )
    {
    DrawingSurface	ds= rd->rdDrawingSurface;
    int			sizeAcross= rd->rdSizeAcross;
    int			whiteY=  WHITE_TOP(sizeAcross);
    int			tw= TAB_WIDTH(sizeAcross);
    DocumentRectangle	drButton;
    DocumentRectangle	drDraw;
    int			ox= 0;

    int			x= rd->rdMinUsed;

    tedTabButtonRectangle( &drButton, x, rd );

    /*  top, bottom */
    drawSetForegroundColor( ds, &(rd->rdForegroundColor) );

    drDraw= drButton;
    drDraw.drX1= drButton.drX1- 1;
    drDraw.drY0= drButton.drY0;
    drDraw.drY1= drButton.drY0;
    drawFillRectangle( ds, &drDraw );

    drDraw= drButton;
    drDraw.drX1= drButton.drX1- 1;
    drDraw.drY0= drButton.drY1;
    drDraw.drY1= drButton.drY1;
    drawFillRectangle( ds, &drDraw );

    /*  left, right */
    drDraw= drButton;
    drDraw.drX0= drButton.drX0;
    drDraw.drX1= drButton.drX0;
    drawFillRectangle( ds, &drDraw );

    drDraw= drButton;
    drDraw.drX0= drButton.drX1- 1;
    drDraw.drX1= drButton.drX1- 1;
    drDraw.drY1--;
    drawFillRectangle( ds, &drDraw );

    /*  top, left */
    drawSetForegroundColor( ds, &(rd->rdTopShadowColor) );

    drDraw= drButton;
    drDraw.drX0= drButton.drX0+ 1;
    drDraw.drY0= drButton.drY0+ 1;
    drDraw.drX1= drButton.drX1- 2;
    drDraw.drY1= drDraw.drY0;
    drawFillRectangle( ds, &drDraw );

    drDraw= drButton;
    drDraw.drX0= drButton.drX0+ 1;
    drDraw.drY0= drButton.drY0+ 1;
    drDraw.drX1= drDraw.drX0;
    drDraw.drY1= drButton.drY1- 1;
    drawFillRectangle( ds, &drDraw );

    /*  bottom, right */
    drawSetForegroundColor( ds, &(rd->rdBottomShadowColor) );

    drDraw= drButton;
    drDraw.drX0= drButton.drX0+ 2;
    drDraw.drY0= drButton.drY1- 1;
    drDraw.drX1= drButton.drX1- 2;
    drDraw.drY1= drDraw.drY0;
    drawFillRectangle( ds, &drDraw );

    drDraw= drButton;
    drDraw.drX0= drButton.drX1- 2;
    drDraw.drY0= drButton.drY0+ 2;
    drDraw.drX1= drDraw.drX0;
    drDraw.drY1= drButton.drY1- 1;
    drawFillRectangle( ds, &drDraw );

    drawSetForegroundColor( ds, &(rd->rdForegroundColor) );

    tedDrawTabSymbol( ds, x, ox, whiteY- tw/2+ 1, rd, kind );
    }

void tedRulerActiveRect(		DocumentRectangle *	drActive,
					const TedTopRuler * 	ttr )
    {
    const RulerData *	rd= &(ttr->ttrRulerData);
    int			sizeAcross= rd->rdSizeAcross;

    int			whiteY= WHITE_TOP(sizeAcross);
    int			whiteH= WHITE_HEIGHT(sizeAcross);

    drActive->drX0= ttr->ttrMarginX0;
    drActive->drX1= ttr->ttrMarginX1;
    drActive->drY0= whiteY;
    drActive->drY1= whiteY+ whiteH;

    if  ( drActive->drX0 < rd->rdVisibleC0 )
	{ drActive->drX0=  rd->rdVisibleC0;	}
    if  ( drActive->drX1 > rd->rdVisibleC1 )
	{ drActive->drX1=  rd->rdVisibleC1;	}

    return;
    }

/************************************************************************/
/*									*/
/*  Draw ticks on the horizontal ruler.					*/
/*									*/
/************************************************************************/

void tedTopRulerDrawTicks(	TedTopRuler *		ttr,
				int			ox,
				int			c0,
				int			c1 )
    {
    RulerData *		rd= &(ttr->ttrRulerData);
    int			sizeAcross= rd->rdSizeAcross;

    int			tagY0= TAG_TOP(sizeAcross);
    int			tagY1= tagY0+ TAG_HEIGHT(sizeAcross)- 1;

    int			tickY0= TICK_TOP(sizeAcross);
    int			tickY1= tickY0+ TICK_HEIGHT(sizeAcross)- 1;

    int			fontBottom= FONT_BOTTOM(sizeAcross);

    double		units= 0;
    int			tick= 0;

    if  ( c0 >= c1 )
	{ LLDEB(c0,c1); return;	}

    units= 0; tick= 0;
    for (;;)
	{
	int	twips= units* rd->rdTwipsPerUnit;
	int	iitem= rd->rdMagnifiedPixelsPerTwip* twips;

	if  ( iitem > c1 )
	    { break;	}

	if  ( iitem < c0 )
	    { units += rd->rdUnitsPerTick; tick++; continue; }

	appHorizontalRulerDrawMark( rd,
		    fontBottom, tagY0, tagY1, tickY0, tickY1,
		    iitem, tick, units, ox );

	units += rd->rdUnitsPerTick; tick++;
	continue;
	}
    }

/************************************************************************/
/*									*/
/*  Draw ticks on the horizontal ruler.					*/
/*									*/
/************************************************************************/

int tedRulerMakeDrawingSurface(	TedTopRuler *	ttr,
				APP_WIDGET	w )
    {
    RulerData *		rd= &(ttr->ttrRulerData);
    const int		vertical= 0;

    appRulerMakeDrawingSurface( rd, vertical,
		    FONT_HEIGHT(rd->rdSizeAcross), ttr->ttrMagnification, w );

    return 0;
    }
