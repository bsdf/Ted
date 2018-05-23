/************************************************************************/
/*									*/
/*  Ted, Draw ornaments: Borders and Shading.				*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>

#   include	"docDraw.h"
#   include	"tedApp.h"
#   include	"tedDraw.h"
#   include	"tedLayout.h"
#   include	<psShading.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Start drawing a border.						*/
/*									*/
/************************************************************************/

static void tedDrawSetBorderColor(	DrawingContext *	dc,
					ScreenDrawingData *	sdd,
					int			colorNumber,
					int			asGrid )
    {
    if  ( asGrid )
	{ docDrawSetColorRgb( dc, (void *)sdd, &(sdd->sddGridColor) );	}
    else{ docDrawSetColorNumber( dc, (void *)sdd, colorNumber );	}

    return;
    }

/************************************************************************/
/*									*/
/*  Draw background ornaments.						*/
/*									*/
/************************************************************************/

int tedDrawOrnaments(	const BlockOrnaments *		bo,
			int				page,
			const DocumentRectangle *	drOutside,
			const DocumentRectangle *	drInside,
			void *				through,
			struct DrawingContext *		dc )
    {
    ScreenDrawingData *		sdd= (ScreenDrawingData *)through;
    const LayoutContext *	lc= &(dc->dcLayoutContext);
    const BufferDocument *	bd= lc->lcDocument;

    DocumentRectangle		drOutPixels;
    DocumentRectangle		drInPixels;

    int				topAsGrid= 0;
    int				leftAsGrid= 0;
    int				rightAsGrid= 0;
    int				bottomAsGrid= 0;

    docGetPixelRect( &drOutPixels, lc, drOutside, page );
    drInPixels= drOutPixels;

    if  ( dc->dcClipRect						&&
	  ! geoIntersectRectangle( (DocumentRectangle *)0,
					&drOutPixels, dc->dcClipRect )	)
	{ return 0;	}

    docGetPixelRect( &drInPixels, lc, drInside, page );

    if  ( drOutside->drX0 < drInside->drX0	&&
	  drOutPixels.drX0 >= drInPixels.drX0	)
	{ drInPixels.drX0= drOutPixels.drX0+ 1;	}

    if  ( drOutside->drY0 < drInside->drY0	&&
	  drOutPixels.drY0 >= drInPixels.drY0	)
	{ drInPixels.drY0= drOutPixels.drY0+ 1;	}

    if  ( drOutside->drY1 > drInside->drY1	&&
	  drOutPixels.drY1 <= drInPixels.drY1	)
	{ drInPixels.drY1= drOutPixels.drY1- 1;	}

    if  ( PROPmaskISSET( &(bo->boPropMask), ORNdrawTOP_GRID ) )
	{
	topAsGrid= 1;
	drInPixels.drY0= drOutPixels.drY0+ 1;
	}

    if  ( PROPmaskISSET( &(bo->boPropMask), ORNdrawLEFT_GRID ) )
	{
	leftAsGrid= 1;
	drInPixels.drX0= drOutPixels.drX0+ 1;
	}

    if  ( PROPmaskISSET( &(bo->boPropMask), ORNdrawRIGHT_GRID ) )
	{
	rightAsGrid= 1;
	drOutPixels.drX1++;
	}

    if  ( PROPmaskISSET( &(bo->boPropMask), ORNdrawBOTTOM_GRID ) )
	{
	bottomAsGrid= 1;
	drOutPixels.drY1++;
	}

    if  ( PROPmaskISSET( &(bo->boPropMask), ORNdrawTOP_BORDER )	)
	{
	if  ( drInPixels.drY0 == drOutPixels.drY0 )
	    { drInPixels.drY0++;	}
	}

    if  ( PROPmaskISSET( &(bo->boPropMask), ORNdrawLEFT_BORDER ) )
	{
	if  ( drInPixels.drX0 == drOutPixels.drX0 )
	    { drInPixels.drX0++;	}
	}

    if  ( PROPmaskISSET( &(bo->boPropMask), ORNdrawRIGHT_BORDER ) )
	{
	if  ( drInPixels.drX1 == drOutPixels.drX1 )
	    { drInPixels.drX1--;	}
	}

    if  ( PROPmaskISSET( &(bo->boPropMask), ORNdrawBOTTOM_BORDER ) )
	{
	if  ( drInPixels.drY1 == drOutPixels.drY1 )
	    { drInPixels.drY1--;	}
	}

    if  ( PROPmaskISSET( &(bo->boPropMask), ORNdrawSHADE ) )
	{
	DocumentRectangle	drShade= drInPixels;

	if  ( ! dc->dcClipRect						||
	      geoIntersectRectangle( &drShade, &drInPixels,
						    dc->dcClipRect )	)
	    {
	    ItemShading		is= bo->boShading;

	    int			isFilled= 0;
	    RGB8Color		rgb8;

	    if  ( docGetSolidRgbShadeOfItem( &isFilled, &rgb8, bd, &is ) )
		{ LDEB(1);	}

	    if  ( is.isPattern != DOCspSOLID )
		{
		int		level= 0;

		switch( is.isPattern )
		    {
		    case PSshdHORIZ:		level= 15; break;
		    case PSshdVERT:		level= 15; break;
		    case PSshdFDIAG:		level= 15; break;
		    case PSshdBDIAG:		level= 15; break;
		    case PSshdCROSS:		level= 15; break;
		    case PSshdDCROSS:		level= 15; break;

		    case PSshdDKHORIZ:	level= 30; break;
		    case PSshdDKVERT:		level= 30; break;
		    case PSshdDKFDIAG:	level= 30; break;
		    case PSshdDKBDIAG:	level= 30; break;
		    case PSshdDKCROSS:	level= 30; break;
		    case PSshdDKDCROSS:	level= 30; break;
		    }

		if  ( bo->boShading.isForeColor == 0 )
		    {
		    if  ( bo->boShading.isBackColor == 0 )
			{ utilRGB8SolidBlack( &rgb8 );	}

		    rgb8.rgb8Red= ( ( 100- level )* rgb8.rgb8Red )/ 100;
		    rgb8.rgb8Green= ( ( 100- level )* rgb8.rgb8Green )/ 100;
		    rgb8.rgb8Blue= ( ( 100- level )* rgb8.rgb8Blue )/ 100;
		    isFilled= 1;
		    }
		else{
		    is.isLevel= 100* level;
		    is.isPattern= DOCspSOLID;

		    if  ( docGetSolidRgbShadeOfItem( &isFilled, &rgb8,
								    bd, &is ) )
			{ LDEB(1);	}
		    }
		}

	    if  ( isFilled )
		{
		DocumentRectangle	drFill= drShade;

		docDrawSetColorRgb( dc, (void *)sdd, &rgb8 );

		geoShiftRectangle( &drFill, -lc->lcOx, -lc->lcOy );
		drawFillRectangle( lc->lcDrawingSurface, &drFill );
		}
	    }
	}

    if  ( PROPmaskISSET( &(bo->boPropMask), ORNdrawTOP_BORDER )	||
	  PROPmaskISSET( &(bo->boPropMask), ORNdrawTOP_GRID )	)
	{
	DocumentRectangle	drBorder= drOutPixels;

	drBorder.drY1= drInPixels.drY0;
	drBorder.drY1= drInPixels.drY0- 1;
	if  ( drBorder.drY1 < drBorder.drY0 )
	    { drBorder.drY1=  drBorder.drY0;	}

	if  ( ! dc->dcClipRect						  ||
	      geoIntersectRectangle( &drBorder, &drBorder, dc->dcClipRect ) )
	    {
	    tedDrawSetBorderColor( dc, sdd,
				    bo->boTopBorder.bpColor, topAsGrid );

	    geoShiftRectangle( &drBorder, -lc->lcOx, -lc->lcOy );
	    drawFillRectangle( lc->lcDrawingSurface, &drBorder );
	    }
	}

    if  ( PROPmaskISSET( &(bo->boPropMask), ORNdrawLEFT_BORDER )	||
	  PROPmaskISSET( &(bo->boPropMask), ORNdrawLEFT_GRID )		)
	{
	DocumentRectangle	drBorder= drOutPixels;

	drBorder.drY0= drInPixels.drY0;
	drBorder.drY1= drInPixels.drY1;
	drBorder.drX1= drInPixels.drX0- 1;

	if  ( drBorder.drX1 < drBorder.drX0 )
	    { drBorder.drX1=  drBorder.drX0;	}

	if  ( ! dc->dcClipRect						  ||
	      geoIntersectRectangle( &drBorder, &drBorder, dc->dcClipRect ) )
	    {
	    tedDrawSetBorderColor( dc, sdd,
				    bo->boLeftBorder.bpColor, leftAsGrid );

	    geoShiftRectangle( &drBorder, -lc->lcOx, -lc->lcOy );
	    drawFillRectangle( lc->lcDrawingSurface, &drBorder );
	    }
	}

    if  ( PROPmaskISSET( &(bo->boPropMask), ORNdrawRIGHT_BORDER )	||
	  PROPmaskISSET( &(bo->boPropMask), ORNdrawRIGHT_GRID )		)
	{
	DocumentRectangle	drBorder= drOutPixels;

	drBorder.drY0= drInPixels.drY0;
	drBorder.drY1= drInPixels.drY1;
	drBorder.drX0= drInPixels.drX1+ 1;

	if  ( drBorder.drX0 > drBorder.drX1 )
	    { drBorder.drX0=  drBorder.drX1;	}

	/* Avoid overlaps with fills
	drBorder.drX0--;
	drBorder.drX1--;
	*/

	if  ( ! dc->dcClipRect						  ||
	      geoIntersectRectangle( &drBorder, &drBorder, dc->dcClipRect ) )
	    {
	    tedDrawSetBorderColor( dc, sdd,
				    bo->boRightBorder.bpColor, rightAsGrid );

	    geoShiftRectangle( &drBorder, -lc->lcOx, -lc->lcOy );
	    drawFillRectangle( lc->lcDrawingSurface, &drBorder );
	    }
	}

    if  ( PROPmaskISSET( &(bo->boPropMask), ORNdrawBOTTOM_BORDER )	||
	  PROPmaskISSET( &(bo->boPropMask), ORNdrawBOTTOM_GRID )	)
	{
	DocumentRectangle	drBorder= drOutPixels;

	drBorder.drY0= drInPixels.drY1+ 1;

	if  ( drBorder.drY0 > drBorder.drY1 )
	    { drBorder.drY0=  drBorder.drY1;	}

	/* Avoid overlaps with fills
	drBorder.drY0--;
	drBorder.drY1--;
	*/

	if  ( ! dc->dcClipRect						  ||
	      geoIntersectRectangle( &drBorder, &drBorder, dc->dcClipRect ) )
	    {
	    tedDrawSetBorderColor( dc, sdd,
				    bo->boBottomBorder.bpColor, bottomAsGrid );

	    geoShiftRectangle( &drBorder, -lc->lcOx, -lc->lcOy );
	    drawFillRectangle( lc->lcDrawingSurface, &drBorder );
	    }
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Draw a shaded rectangle.						*/
/*									*/
/************************************************************************/

void tedDrawShadedRectangle(
		const LayoutContext *		lc,
		DrawingSurface			shadingPixmaps[PSshd_COUNT],
		int				pattern,
		const DocumentRectangle *	drShade )
    {
    if  ( ! shadingPixmaps[pattern] )
	{
	const int		wide= 8;
	const int		high= 8;
	const int		d= 4;
	int			t;
	int			s= 0;

	DocumentRectangle	drFill;

	int			x;
	int			y;

	shadingPixmaps[pattern]= drawMakeDrawingSurfaceForParent(
					lc->lcDrawingSurface, wide, high );
	if  ( ! shadingPixmaps[pattern] )
	    { XDEB(shadingPixmaps[pattern]); return;	}

	drFill.drX0= 0;
	drFill.drY0= 0;
	drFill.drX1= wide- 1;
	drFill.drY1= high- 1;

	drawSetForegroundColorWhite( shadingPixmaps[pattern] );
	drawFillRectangle( shadingPixmaps[pattern], &drFill );

	drawSetForegroundColorBlack( shadingPixmaps[pattern] );

	switch( pattern )
	    {
	    case DOCspSOLID:
		LDEB(pattern);
		t= 0; s= 0;
		break;

	    case PSshdHORIZ:
	    case PSshdVERT:
	    case PSshdFDIAG:
	    case PSshdBDIAG:
	    case PSshdCROSS:
	    case PSshdDCROSS:
		t= 1; s= 0;
		break;

	    case PSshdDKHORIZ:
	    case PSshdDKVERT:
	    case PSshdDKFDIAG:
	    case PSshdDKBDIAG:
	    case PSshdDKCROSS:
	    case PSshdDKDCROSS:
		t= 2; s= 1;
		break;

	    default:
		LDEB(pattern);
		return;
	    }

	if  ( t > 0 )
	    {
	    drawSetLineAttributes( shadingPixmaps[pattern],
			t, LineStyleSolid, LineCapProjecting, LineJoinMiter,
			(unsigned char *)0, 0 );
	    }

	switch( pattern )
	    {
	    case DOCspSOLID:
		LDEB(pattern);
		break;

	    case PSshdHORIZ:
	    case PSshdDKHORIZ:
		y= 0;
		while( y < high )
		    {
		    drawLine( shadingPixmaps[pattern], 0, y+ s, wide- 1, y+ s );
		    y += d;
		    }
		break;

	    case PSshdVERT:
	    case PSshdDKVERT:
		x= 0;
		while( x < wide )
		    {
		    drawLine( shadingPixmaps[pattern], x+ s, 0, x+ s, high- 1 );
		    x += d;
		    }
		break;

	    case PSshdFDIAG:
	    case PSshdDKFDIAG:
		x= -high;
		while( x < wide )
		    {
		    drawLine( shadingPixmaps[pattern], x+ s, 0, x+ high+ s, high );
		    x += d;
		    }
		break;

	    case PSshdBDIAG:
	    case PSshdDKBDIAG:
		x= 0;
		while( x < wide+ high )
		    {
		    drawLine( shadingPixmaps[pattern], x, 0, x- high, high );
		    x += d;
		    }
		break;

	    case PSshdCROSS:
	    case PSshdDKCROSS:
		y= 0;
		while( y < high )
		    {
		    drawLine( shadingPixmaps[pattern], 0, y+ s, wide- 1, y+ s );
		    y += d;
		    }
		x= 0;
		while( x < wide )
		    {
		    drawLine( shadingPixmaps[pattern], x+ s, 0, x+ s, high- 1 );
		    x += d;
		    }
		break;

	    case PSshdDCROSS:
	    case PSshdDKDCROSS:
		x= -high;
		while( x < wide )
		    {
		    /*
		    drawLine( shadingPixmaps[pattern], x+ s, 0, x+ high+ s, high );
		    */
		    drawLine( shadingPixmaps[pattern], x, 0, x+ high, high );
		    x += d;
		    }
		x= 0;
		while( x < wide+ high )
		    {
		    drawLine( shadingPixmaps[pattern], x, 0, x- high, high );
		    x += d;
		    }
		break;

	    default:
		LDEB(pattern);
		return;
	    }
	}

# if 0
#   ifdef USE_MOTIF
    XSetFillStyle( lc->lcAdd->addDisplay, lc->lcAdd->addGc, FillTiled );
    XSetTile( lc->lcAdd->addDisplay, lc->lcAdd->addGc, shadingPixmaps[pattern] );
#   endif
#   ifdef USE_GTK
    gdk_gc_set_fill( lc->lcAdd->addGc, GDK_TILED );
    gdk_gc_set_tile( lc->lcAdd->addGc, shadingPixmaps[pattern] );
#   endif

    drawFillRectangle( lc->lcAdd, drShade );

#   ifdef USE_MOTIF
    XSetFillStyle( lc->lcAdd->addDisplay, lc->lcAdd->addGc, FillSolid );
#   endif
#   ifdef USE_GTK
    gdk_gc_set_fill( lc->lcAdd->addGc, GDK_SOLID );
#   endif
# endif

    return;
    }


/************************************************************************/
/*									*/
/*  Draw actual lines for a segment in a horizontal border.		*/
/*									*/
/************************************************************************/

static void tedBorderSetLineSolid(	DrawingSurface		ds,
					int			thick )
    {
    drawSetLineAttributes( ds,
		    thick, LineStyleSolid, LineCapButt, LineJoinMiter,
		    (unsigned char *)0, 0 );
    }

static void tedBorderSetLineDashes(	DrawingSurface		ds,
					const unsigned char *	dashes,
					int			dashCount,
					int			thick )
    {
    drawSetLineAttributes( ds,
		    thick, LineStyleSingleDash, LineCapButt, LineJoinMiter,
		    dashes, dashCount );
    }

static const unsigned char TED_Border_DOT[]=	{ 1, 2 };
static const unsigned char TED_Border_DASH[]=	{ 3, 3 };
static const unsigned char TED_Border_DASHSM[]=	{ 3, 1 };
static const unsigned char TED_Border_DASHD[]=	{ 4, 2, 2, 2 };
static const unsigned char TED_Border_DASHDD[]=	{ 4, 2, 2, 2, 2, 2 };

void tedDrawHorizontalBorderLine(	DrawingSurface		ds,
					int			style,
					int			minThick,
					int			x0,
					int			x1,
					int			y )
    {
    int		yy;
    int		xx;

    switch( style )
	{
	case DOCbsNONE:
	    break;

	case DOCbsS:
	    tedBorderSetLineSolid( ds, 1 );
	    drawLine( ds, x0, y, x1, y );
	    break;

	case DOCbsTH:
	    tedBorderSetLineSolid( ds, 2 );
	    drawLine( ds, x0, y, x1, y );
	    break;

	case DOCbsSH:
	    tedBorderSetLineSolid( ds, 2 );
	    drawLine( ds, x0, y, x1, y );
	    break;

	case DOCbsDB:
	    tedBorderSetLineSolid( ds, 1 );
	    yy= y- 1;
	    drawLine( ds, x0, yy, x1, yy );
	    yy= y+ 1;
	    drawLine( ds, x0, yy, x1, yy );
	    break;

	case DOCbsHAIR:
	    tedBorderSetLineSolid( ds, 1 );
	    drawLine( ds, x0, y, x1, y );
	    break;

	case DOCbsDOT:
	    tedBorderSetLineDashes( ds,
			    TED_Border_DOT, sizeof( TED_Border_DOT ), 1 );
	    drawLine( ds, x0, y, x1, y );
	    break;

	case DOCbsDASH:
	    tedBorderSetLineDashes( ds,
			    TED_Border_DASH, sizeof( TED_Border_DASH ), 1 );
	    drawLine( ds, x0, y, x1, y );
	    break;

	case DOCbsDASHD:
	    tedBorderSetLineDashes( ds,
			TED_Border_DASHD, sizeof( TED_Border_DASHD ), 1 );
	    drawLine( ds, x0, y, x1, y );
	    break;

	case DOCbsDASHDD:
	    tedBorderSetLineDashes( ds,
			TED_Border_DASHDD, sizeof( TED_Border_DASHDD ), 1 );
	    drawLine( ds, x0, y, x1, y );
	    break;

	case DOCbsTRIPLE:
	    tedBorderSetLineSolid( ds, 1 );
	    yy= y- 2;
	    drawLine( ds, x0, yy, x1, yy );
	    yy= y;
	    drawLine( ds, x0, yy, x1, yy );
	    yy= y+ 2;
	    drawLine( ds, x0, yy, x1, yy );
	    break;

	case DOCbsTNTHSG:
	    tedBorderSetLineSolid( ds, 1 );
	    yy= y- 2;
	    drawLine( ds, x0, yy, x1, yy );
	    tedBorderSetLineSolid( ds, 2 );
	    yy= y+ 1;
	    drawLine( ds, x0, yy, x1, yy );
	    break;

	case DOCbsTHTNSG:
	    tedBorderSetLineSolid( ds, 2 );
	    yy= y- 1;
	    drawLine( ds, x0, yy, x1, yy );
	    tedBorderSetLineSolid( ds, 1 );
	    yy= y+ 1;
	    drawLine( ds, x0, yy, x1, yy );
	    break;

	case DOCbsTNTHTNSG:
	    tedBorderSetLineSolid( ds, 1 );
	    yy= y- 3;
	    drawLine( ds, x0, yy, x1, yy );
	    tedBorderSetLineSolid( ds, 2 );
	    yy= y+ 0;
	    drawLine( ds, x0, yy, x1, yy );
	    tedBorderSetLineSolid( ds, 1 );
	    yy= y+ 2;
	    drawLine( ds, x0, yy, x1, yy );
	    break;

	case DOCbsTNTHMG:
	    tedBorderSetLineSolid( ds, 1 );
	    yy= y- 2;
	    drawLine( ds, x0, yy, x1, yy );
	    tedBorderSetLineSolid( ds, 3 );
	    yy= y+ 1;
	    drawLine( ds, x0, yy, x1, yy );
	    break;

	case DOCbsTHTNMG:
	    tedBorderSetLineSolid( ds, 3 );
	    yy= y- 2;
	    drawLine( ds, x0, yy, x1, yy );
	    tedBorderSetLineSolid( ds, 1 );
	    yy= y+ 1;
	    drawLine( ds, x0, yy, x1, yy );
	    break;

	case DOCbsTNTHTNMG:
	    tedBorderSetLineSolid( ds, 1 );
	    yy= y- 3;
	    drawLine( ds, x0, yy, x1, yy );
	    tedBorderSetLineSolid( ds, 3 );
	    yy= y+ 0;
	    drawLine( ds, x0, yy, x1, yy );
	    tedBorderSetLineSolid( ds, 1 );
	    yy= y+ 3;
	    drawLine( ds, x0, yy, x1, yy );
	    break;

	case DOCbsTNTHLG:
	    tedBorderSetLineSolid( ds, 1 );
	    yy= y- 3;
	    drawLine( ds, x0, yy, x1, yy );
	    tedBorderSetLineSolid( ds, 4 );
	    yy= y+ 2;
	    drawLine( ds, x0, yy, x1, yy );
	    break;

	case DOCbsTHTNLG:
	    tedBorderSetLineSolid( ds, 4 );
	    yy= y- 2;
	    drawLine( ds, x0, yy, x1, yy );
	    tedBorderSetLineSolid( ds, 1 );
	    yy= y+ 2;
	    drawLine( ds, x0, yy, x1, yy );
	    break;

	case DOCbsTNTHTNLG:
	    tedBorderSetLineSolid( ds, 1 );
	    yy= y- 4;
	    drawLine( ds, x0, yy, x1, yy );
	    tedBorderSetLineSolid( ds, 4 );
	    yy= y+ 0;
	    drawLine( ds, x0, yy, x1, yy );
	    tedBorderSetLineSolid( ds, 1 );
	    yy= y+ 3;
	    drawLine( ds, x0, yy, x1, yy );
	    break;

	case DOCbsWAVY:
	    tedBorderSetLineSolid( ds, 1 );
	    xx= x0;
#	    define	W	2
#	    define	W2	4
	    drawLine( ds, xx, y, xx+ W, y+ W ); xx += W;
	    while( xx < x1 )
		{
		drawLine( ds, xx, y+ W, xx+ W2, y- W ); xx += W2;
		drawLine( ds, xx, y- W, xx+ W2, y+ W ); xx += W2;
		}
	    break;

	case DOCbsWAVYDB:
	    tedBorderSetLineSolid( ds, 1 );
	    yy= y- 2;
	    xx= x0;
	    drawLine( ds, xx, yy, xx+ W, yy+ W ); xx += W;
	    while( xx < x1 )
		{
		drawLine( ds, xx, yy+ W, xx+ W2, yy- W ); xx += W2;
		drawLine( ds, xx, yy- W, xx+ W2, yy+ W ); xx += W2;
		}

	    yy= y+ 2;
	    xx= x0;
	    drawLine( ds, xx, yy, xx+ W, yy+ W ); xx += W;
	    while( xx < x1 )
		{
		drawLine( ds, xx, yy+ W, xx+ W2, yy- W ); xx += W2;
		drawLine( ds, xx, yy- W, xx+ W2, yy+ W ); xx += W2;
		}
	    break;

	case DOCbsDASHSM:
	    tedBorderSetLineDashes( ds,
			TED_Border_DASHSM, sizeof( TED_Border_DASHSM ), 1 );
	    drawLine( ds, x0, y, x1, y );
	    break;

	case DOCbsDASHDOTSTR:
	    xx= x0- 2;
	    while( xx < x1 )
		{
		drawLine( ds, xx, y+ 1, xx+ 3, y- 2 ); xx += 2;
		drawLine( ds, xx, y+ 1, xx+ 3, y- 2 ); xx += 1;
		drawLine( ds, xx, y+ 1, xx+ 3, y- 2 ); xx += 2;
		}
	    break;

	case DOCbsEMBOSS:
	case DOCbsENGRAVE:
	case DOCbsFRAME:
	case DOCbsOUTSET:
	    tedBorderSetLineSolid( ds, 1 );
	    drawLine( ds, x0, y, x1, y );
	    break;

	case DOCbsTBL:
	case DOCbsNIL:
	    /*no border*/
	    break;

	default:
	    LDEB(style);
	    break;
	}

    return;
    }
