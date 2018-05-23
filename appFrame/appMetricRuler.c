#   include	"appFrameConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>

#   include	"guiWidgets.h"
#   include	"appMetricRuler.h"
#   include	"guiWidgetDrawingSurface.h"
#   include	"guiDrawingWidget.h"

#   include	<appDebugon.h>

#   define	BACK_MARG(s)	((s)/2)

#   define	TAG_TOP(s)	((s)/3)
#   define	TAG_BOTTOM(s)	(s)
#   define	TAG_HEIGHT(s)	(TAG_BOTTOM(s)-TAG_TOP(s))

#   define	FONT_HEIGHT(s)	(5*(s)/9)
#   define	FONT_BOTTOM(s)	(5*(s)/9)

#   define	TICK_TOP(s)	((2*(s))/3)
#   define	TICK_BOTTOM(s)	(s)
#   define	TICK_HEIGHT(s)	(TICK_BOTTOM(s)-TICK_TOP(s))

#   define	HAIR_THICK(s)	(2)

typedef struct MetricRuler
    {
    RulerData		mrRulerData;

    int			mrV0Pixels;		/*  hairs lower value	*/
    int			mrV1Pixels;		/*  hairs upper value	*/
    double		mrMagnification;
    } MetricRuler;

typedef struct MetricRulerDrag
    {
    int *		mrdValuePointer;
    MetricRuler *	mrdMr;
    APP_WIDGET		mrdWidget;
    int			mrdOc;
    EditDocument *	mrdDocument;
    } MetricRulerDrag;

void * appMakeMetricRuler(
			int				sizeAcross,
			double				magnifiedPixelsPerTwip,
			double				magnification,
			const PostScriptFontList *	psfl,

			int				minUnused,
			int				maxUnused,

			int				documentC0,
			int				documentC1,
			int				visibleC0,
			int				visibleC1,

			int				whatUnit )
    {
    MetricRuler *	mr;
    RulerData *		rd;

    mr= malloc( sizeof( MetricRuler ) );
    if  ( ! mr )
	{ XDEB(mr); return (void *)0;	}
    rd= &(mr->mrRulerData);

    appInitRulerData( &(mr->mrRulerData) );

    mr->mrMagnification= magnification;

    rd->rdPostScriptFontList= psfl;

    rd->rdUnit= whatUnit;
    appRulerCalculateIncrements( rd, magnifiedPixelsPerTwip, magnification );

    rd->rdMinUsed= minUnused;
    rd->rdExtraAfterMaxUsed= maxUnused;

    rd->rdDocumentC0= documentC0;
    rd->rdDocumentC1= documentC1;
    rd->rdVisibleC0= visibleC0;
    rd->rdVisibleC1= visibleC1;

    rd->rdSizeAcross= sizeAcross;
    rd->rdSizeAlong= 0;

    mr->mrV0Pixels= -1;
    mr->mrV1Pixels= -1;

    return (void *)mr;
    }

void appFreeMetricRuler(	void *		voidmr )
    {
    MetricRuler *		mr= (MetricRuler *)voidmr;

    appCleanRulerData( &(mr->mrRulerData) );

    free( mr );

    return;
    }

/************************************************************************/
/*									*/
/*  Draw a mark on an horizontal ruler.					*/
/*									*/
/************************************************************************/

void appHorizontalRulerDrawMark(	RulerData *		rd,
					int			fontBottom,
					int			tagY0,
					int			tagY1,
					int			tickY0,
					int			tickY1,
					int			iitem,
					int			tick,
					double			units,
					int			ox )
    {
    DrawingSurface	ds= rd->rdDrawingSurface;
    DocumentRectangle	drMark;
    int			maxUsed= rd->rdSizeAlong- rd->rdExtraAfterMaxUsed;

    drMark.drX0= drMark.drX1= iitem- ox;

    if  ( iitem != 0 && tick % rd->rdTicksPerText == 0 )
	{
	char		scratch[30];
	int		len;
	int		wide;

	appRulerTagText( scratch, &wide, &len, rd, units );

	if  ( rd->rdScreenFont >= 0			&&
	      iitem- ox- wide/2 >= rd->rdMinUsed	&&
	      iitem- ox+ wide/2 <  maxUsed		)
	    {
	    drawString( ds, iitem- ox- wide/2, fontBottom, rd->rdScreenFont,
								scratch, len );

	    drMark.drY0= fontBottom+ 2;
	    drMark.drY1= tagY1;
	    }
	else{
	    drMark.drY0= tagY0;
	    drMark.drY1= tagY1;
	    }
	}
    else{
	if  ( tick % rd->rdTicksPerTag == 0 )
	    {
	    drMark.drY0= tagY0;
	    drMark.drY1= tagY1;
	    }
	else{
	    drMark.drY0= tickY0;
	    drMark.drY1= tickY1;
	    }
	}

    if  ( drMark.drY1 >= drMark.drY0 )
	{ drawFillRectangle( ds, &drMark );	}

    return;
    }

/************************************************************************/
/*									*/
/*  Draw a horizontal ruler.						*/
/*									*/
/************************************************************************/

static void appDrawHorizontalRuler(	APP_WIDGET		w,
					MetricRuler *		mr,
					DocumentRectangle *	drClip,
					int			ox )
    {
    RulerData *		rd= &(mr->mrRulerData);
    int			sizeAcross= rd->rdSizeAcross;
    int			maxUsed;

    int			tick;
    double		units;

    int			tagY0= TAG_TOP(sizeAcross);
    int			tagY1= tagY0+ TAG_HEIGHT(sizeAcross) -1;

    int			tickY0= TICK_TOP(sizeAcross);
    int			tickY1= tickY0+ TICK_HEIGHT(sizeAcross)- 1;

    int			fontBottom= FONT_BOTTOM(sizeAcross);

    DocumentRectangle	drMark;

    DrawingSurface	ds= rd->rdDrawingSurface;
    DocumentRectangle	drBack;

    if  ( rd->rdSizeAlong == 0 )
	{ appHorizontalRulerGetSizeFromWidget( rd, w );	}

    maxUsed= rd->rdSizeAlong- rd->rdExtraAfterMaxUsed;

    drBack= *drClip;
    drBack.drX0 -= ox;
    /*drBack.drY0 -= oy;*/
    appRulerDrawBackground( rd, &drBack );

    if  ( mr->mrV0Pixels >= 0 || mr->mrV1Pixels >= 0 )
	{
	DocumentRectangle	drSel;

	drSel.drX0= 0;
	drSel.drX1= maxUsed;
	if  ( mr->mrV0Pixels >= 0 )
	    { drSel.drX0= mr->mrV0Pixels;	}
	if  ( mr->mrV1Pixels >= 0 )
	    { drSel.drX1= mr->mrV1Pixels;	}
	drSel.drY0= 0;
	drSel.drY1= sizeAcross;

	if  ( geoIntersectRectangle( &drSel, &drSel, drClip ) )
	    {
	    drawSetForegroundColor( ds, &(rd->rdSelectedColor) );

	    drSel.drX0 -= ox;
	    /*drSel.drY0 -= oy; */
	    drawFillRectangle( ds, &drSel );
	    }
	}

    drawSetForegroundColor( ds, &(rd->rdForegroundColor) );

    drMark.drX0= 0- ox;
    drMark.drX1= rd->rdDocumentC1- ox;
    drMark.drY0= drMark.drY1= sizeAcross- 1;

    if  ( drMark.drX0 < rd->rdMinUsed )
	{ drMark.drX0=  rd->rdMinUsed;	}
    if  ( drMark.drX1 > maxUsed )
	{ drMark.drX1=  maxUsed;	}

    if  ( drMark.drX1 >= drMark.drX0 )
	{ drawFillRectangle( ds, &drMark );	}

    units= 0; tick= 0;
    for (;;)
	{
	int	twips= units* rd->rdTwipsPerUnit;
	int	iitem= rd->rdMagnifiedPixelsPerTwip* twips;

	if  ( iitem > drClip->drX1 )
	    { break;	}

	if  ( iitem- ox <  rd->rdMinUsed	||
	      iitem- ox >= maxUsed		)
	    { units += rd->rdUnitsPerTick; tick++; continue; }

	appHorizontalRulerDrawMark( rd,
		    fontBottom, tagY0, tagY1, tickY0, tickY1,
		    iitem, tick, units, ox );

	units += rd->rdUnitsPerTick; tick++;
	continue;
	}

    /* Already done:
    XSetForeground( display, rd->rdGc, mr->trForePixel );
    */

    drMark.drY0= 0;
    drMark.drY1= sizeAcross- 1;

    if  ( mr->mrV0Pixels >= 0				&&
	  mr->mrV0Pixels- ox- 2 >= rd->rdMinUsed	&&
	  mr->mrV0Pixels- ox <  maxUsed			)
	{
	drMark.drX0= drMark.drX1= mr->mrV0Pixels- ox- 1;
	drawFillRectangle( ds, &drMark );
	}

    if  ( mr->mrV1Pixels >= 0				&&
	  mr->mrV1Pixels- ox >= rd->rdMinUsed		&&
	  mr->mrV1Pixels- ox+ 2 < maxUsed		)
	{
	drMark.drX0= drMark.drX1= mr->mrV1Pixels- ox;
	drawFillRectangle( ds, &drMark );
	}

    if  ( mr->mrV0Pixels >= 0 || mr->mrV1Pixels >= 0 )
	{
	drawSetForegroundColorWhite( ds );

	if  ( mr->mrV0Pixels >= 0			&&
	      mr->mrV0Pixels- ox- 2 >= rd->rdMinUsed	&&
	      mr->mrV0Pixels- ox < maxUsed		)
	    {
	    drMark.drX0= drMark.drX1= mr->mrV0Pixels- ox- 2;
	    drawFillRectangle( ds, &drMark );
	    }

	if  ( mr->mrV1Pixels >= 0			&&
	      mr->mrV1Pixels- ox >= rd->rdMinUsed	&&
	      mr->mrV1Pixels- ox+ 2 < maxUsed		)
	    {
	    drMark.drX0= drMark.drX1= mr->mrV1Pixels- ox+ 1;
	    drawFillRectangle( ds, &drMark );
	    }
	}

    drawNoClipping( ds );

    return;
    }

/************************************************************************/
/*									*/
/*  Draw a vertical ruler.						*/
/*									*/
/************************************************************************/

static void appDrawVerticalRuler(	APP_WIDGET		w,
					MetricRuler *		mr,
					DocumentRectangle *	drClip,
					int			oy )
    {
    RulerData *		rd= &(mr->mrRulerData);
    int			sizeAcross= rd->rdSizeAcross;
    int			maxUsed;

    int			tick;
    double		units;

    int			tagX0= TAG_TOP(sizeAcross);
    int			tagX1= tagX0+ TAG_HEIGHT(sizeAcross)- 1;

    int			tickX0= TICK_TOP(sizeAcross);
    int			tickX1= tagX1+ TICK_HEIGHT(sizeAcross)- 1;

    int			fontBottom= FONT_BOTTOM(sizeAcross);

    DocumentRectangle	drMark;

    DrawingSurface	ds= rd->rdDrawingSurface;

    /*  2  */
    drawSetForegroundColor( ds, &(rd->rdBackgroundColor) );

    if  ( rd->rdSizeAlong == 0 )
	{ appVerticalRulerGetSizeFromWidget( rd, w );	}

    maxUsed= rd->rdSizeAlong- rd->rdExtraAfterMaxUsed;

    drMark= *drClip;
    drMark.drY0 -= oy;
    drMark.drY1 -= oy;

    drawFillRectangle( ds, &drMark );

    if  ( mr->mrV0Pixels >= 0 || mr->mrV1Pixels >= 0 )
	{
	DocumentRectangle	drSel;

	drSel.drX0= 0;
	drSel.drX1= sizeAcross;
	drSel.drY0= 0;
	drSel.drY1= maxUsed;
	if  ( mr->mrV0Pixels >= 0 )
	    { drSel.drY0= mr->mrV0Pixels;	}
	if  ( mr->mrV1Pixels >= 0 )
	    { drSel.drY1= mr->mrV1Pixels;	}

	if  ( geoIntersectRectangle( &drSel, &drSel, drClip ) )
	    {
	    drawSetForegroundColor( ds, &(rd->rdSelectedColor) );

	    drSel.drY0 -= oy;
	    drSel.drY1 -= oy;

	    drawFillRectangle( ds, &drSel );
	    }
	}

    drawSetForegroundColor( ds, &(rd->rdForegroundColor) );

    drMark.drY0= 0- oy;
    drMark.drY1= rd->rdDocumentC1- oy;

    if  ( drMark.drY0 < rd->rdMinUsed )
	{ drMark.drY0=  rd->rdMinUsed;	}
    if  ( drMark.drY1 > maxUsed )
	{ drMark.drY1=  maxUsed;	}

    if  ( drMark.drY1 >= drMark.drY0 )
	{
	drMark.drX0= drMark.drX1= sizeAcross- 1;
	drawFillRectangle( ds, &drMark );
	}

    units= 0; tick= 0;
    for (;;)
	{
	int	twips= units* rd->rdTwipsPerUnit;
	int	iitem= rd->rdMagnifiedPixelsPerTwip* twips;

	if  ( iitem > drClip->drY1 )
	    { break;	}

	drMark.drY0= drMark.drY1= iitem- oy;

	if  ( iitem- oy <  rd->rdMinUsed	||
	      iitem- oy >= maxUsed		)
	    { units += rd->rdUnitsPerTick; tick++; continue; }

	if  ( iitem != 0 && tick % rd->rdTicksPerText == 0 )
	    {
	    char		scratch[30];
	    int			len;
	    int			wide;

	    int			ascent;
	    int			descent;

	    appRulerTagText( scratch, &wide, &len, rd, units );

	    ascent= FONT_HEIGHT(sizeAcross)/ 2;
	    descent= FONT_HEIGHT(sizeAcross)/ 2;

	    if  ( iitem- oy- descent >= rd->rdMinUsed	&&
		  iitem- oy+ ascent  <  maxUsed		)
		{
SDEB((char *)scratch);
		/*
		appRulerDrawVerticalString( rd,
			    fontBottom, iitem+ wide/2- oy, scratch, len );
		*/

		drMark.drX0= fontBottom+ 2;
		drMark.drX1= sizeAcross- 1;
		}
	    else{
		drMark.drX0= tagX0;
		drMark.drX1= tagX1;
		}
	    }
	else{
	    if  ( tick % rd->rdTicksPerTag == 0 )
		{
		drMark.drX0= tagX0;
		drMark.drX1= tagX1;
		}
	    else{
		drMark.drX0= tickX0;
		drMark.drX1= tickX1;
		}
	    }

	drawFillRectangle( ds, &drMark );

	units += rd->rdUnitsPerTick; tick++; continue;
	}

    /* Already done:
    XSetForeground( display, rd->rdGc, mr->trForePixel );
    */

    drMark.drX0= 0;
    drMark.drX1= sizeAcross- 1;

    if  ( mr->mrV0Pixels >= 0				&&
	  mr->mrV0Pixels- oy- 2 >= rd->rdMinUsed	&&
	  mr->mrV0Pixels- oy < maxUsed			)
	{
	drMark.drY0= drMark.drY1= mr->mrV0Pixels- oy- 1;
	drawFillRectangle( ds, &drMark );
	}

    if  ( mr->mrV1Pixels >= 0				&&
	  mr->mrV1Pixels- oy >= rd->rdMinUsed		&&
	  mr->mrV1Pixels- oy+ 2 < maxUsed		)
	{
	drMark.drY0= drMark.drY1= mr->mrV1Pixels- oy;
	drawFillRectangle( ds, &drMark );
	}

    if  ( mr->mrV0Pixels >= 0 || mr->mrV1Pixels >= 0 )
	{
	drawSetForegroundColorWhite( ds );

	if  ( mr->mrV0Pixels >= 0			&&
	      mr->mrV0Pixels- oy- 2 >= rd->rdMinUsed	&&
	      mr->mrV0Pixels- oy < maxUsed		)
	    {
	    drMark.drY0= drMark.drY1= mr->mrV0Pixels- oy- 2;
	    drawFillRectangle( ds, &drMark );
	    }

	if  ( mr->mrV1Pixels >= 0			&&
	      mr->mrV1Pixels- oy >= rd->rdMinUsed	&&
	      mr->mrV1Pixels- oy+ 2 < maxUsed		)
	    {
	    drMark.drY0= drMark.drY1= mr->mrV1Pixels- oy+ 1;
	    drawFillRectangle( ds, &drMark );
	    }
	}

    drawNoClipping( ds );

    return;
    }

APP_EVENT_HANDLER_H( appRedrawHorizontalMetricRuler, w, voidmr, event )
    {
    MetricRuler *	mr= (MetricRuler *)voidmr;
    RulerData *		rd= &(mr->mrRulerData);

    int			ox= rd->rdVisibleC0- rd->rdMinUsed;

    DocumentRectangle	drClip;

    if  ( ! rd->rdDrawingSurface )
	{
	const int	vertical= 0;

	appRulerMakeDrawingSurface( rd, vertical,
		    FONT_HEIGHT(rd->rdSizeAcross), mr->mrMagnification, w );
	}

    guiCollectExposures( &drClip, w, event );

    appDrawHorizontalRuler( w, mr, &drClip, ox );
    }

APP_EVENT_HANDLER_H( appRedrawVerticalMetricRuler, w, voidmr, event )
    {
    MetricRuler *	mr= (MetricRuler *)voidmr;
    RulerData *		rd= &(mr->mrRulerData);

    int			oy= rd->rdVisibleC0- rd->rdMinUsed;

    DocumentRectangle	drClip;

    if  ( ! rd->rdDrawingSurface )
	{
	const int	vertical= 1;

	appRulerMakeDrawingSurface( rd, vertical,
		    FONT_HEIGHT(rd->rdSizeAcross), mr->mrMagnification, w );
	}

    guiCollectExposures( &drClip, w, event );

    appDrawVerticalRuler( w, mr, &drClip, oy );
    }

APP_EVENT_HANDLER_H( appHorizontalRulerConfigure, w, voidmr, event )
    {
    MetricRuler *	mr= (MetricRuler *)voidmr;
    RulerData *		rd= &(mr->mrRulerData);

    appHorizontalRulerGetSizeFromConfigureEvent( rd, w, event );

    return;
    }

APP_EVENT_HANDLER_H( appVerticalRulerConfigure, w, voidmr, event )
    {
    MetricRuler *	mr= (MetricRuler *)voidmr;
    RulerData *		rd= &(mr->mrRulerData);

    appVerticalRulerGetSizeFromConfigureEvent( rd, w, event );

    return;
    }

/************************************************************************/
/*									*/
/*  Scroll the ruler.							*/
/*									*/
/************************************************************************/

void appScrollHorMetricRuler(	void *			voidmr,
				APP_WIDGET		w,
				int			d )
    {
    MetricRuler *		mr= (MetricRuler *)voidmr;
    RulerData *			rd= &(mr->mrRulerData);

    DocumentRectangle		drClip;

    int				ox;

    appScrollHorizontalRuler( rd, &drClip, d );

    ox= rd->rdVisibleC0- rd->rdMinUsed;

    appDrawHorizontalRuler( w, mr, &drClip, ox );
    }

void appScrollVertMetricRuler(	void *			voidmr,
				APP_WIDGET		w,
				int			d )
    {
    MetricRuler *		mr= (MetricRuler *)voidmr;
    RulerData *			rd= &(mr->mrRulerData);

    DocumentRectangle		drClip;

    int				oy;

    appScrollVerticalRuler( rd, &drClip, d );

    oy= rd->rdVisibleC0- rd->rdMinUsed;

    appDrawVerticalRuler( w, mr, &drClip, oy );
    }

void appSetMetricRulerRange(	void *			voidmr,
				APP_WIDGET		w,
				int			docFullV1,
				int			docVisV0,
				int			docVisV1 )
    {
    MetricRuler *	mr= (MetricRuler *)voidmr;
    RulerData *		rd= &(mr->mrRulerData);

    rd->rdDocumentC1= docFullV1;
    rd->rdVisibleC0= docVisV0;
    rd->rdVisibleC1= docVisV1;

    if  ( ! rd->rdDrawingSurface )
	{
XDEB(rd->rdDrawingSurface);
	}

    guiExposeDrawingWidget( w );

    return;
    }

/************************************************************************/

void appMetricRulerDrawHair(	EditDocument *		ed )
    {
    const SelectRectangle *	sr= &(ed->edSelectRectangle);
    DocumentRectangle		drWhite;
    DocumentRectangle		drBlack;
    int				ox= ed->edVisibleRect.drX0;
    int				oy= ed->edVisibleRect.drY0;

    drWhite= ed->edVisibleRect;
    drBlack= ed->edVisibleRect;

    switch( sr->srDirection )
	{
	case DOCselW:
	    drWhite.drX0= sr->srSelected.drX0+ sr->srLTM.drX0;
	    drWhite.drX1= sr->srSelected.drX0;
	    drBlack.drX0= sr->srSelected.drX0;
	    drBlack.drX1= sr->srSelected.drX0+ sr->srLTM.drX1;
	    break;

	case DOCselN:
	    drWhite.drY0= sr->srSelected.drY0+ sr->srLTM.drY0;
	    drWhite.drY1= sr->srSelected.drY0;
	    drBlack.drY0= sr->srSelected.drY0;
	    drBlack.drY1= sr->srSelected.drY0+ sr->srLTM.drY1;
	    break;

	case DOCselE:
	    drBlack.drX0= sr->srSelected.drX1+ sr->srRBM.drX0;
	    drBlack.drX1= sr->srSelected.drX1;
	    drWhite.drX0= sr->srSelected.drX1;
	    drWhite.drX1= sr->srSelected.drX1+ sr->srRBM.drX1;
	    break;

	case DOCselS:
	    drBlack.drY0= sr->srSelected.drY1+ sr->srRBM.drY0;
	    drBlack.drY1= sr->srSelected.drY1;
	    drWhite.drY0= sr->srSelected.drY1;
	    drWhite.drY1= sr->srSelected.drY1+ sr->srRBM.drY1;
	    break;

	default:
	    LDEB(sr->srDirection);
	    return;
	}

    drBlack.drX0 -= ox;
    drBlack.drX1 -= ox;
    drBlack.drY0 -= oy;
    drBlack.drY1 -= oy;

    drWhite.drX0 -= ox;
    drWhite.drX1 -= ox;
    drWhite.drY0 -= oy;
    drWhite.drY1 -= oy;

    drawSetForegroundColorBlack( ed->edDrawingSurface );
    drawFillRectangle( ed->edDrawingSurface, &drBlack );

    drawSetForegroundColorWhite( ed->edDrawingSurface );
    drawFillRectangle( ed->edDrawingSurface, &drWhite );

    return;
    }


/************************************************************************/

void appMetricRulerExposeValue(	EditDocument *		ed )
    {
    const SelectRectangle *	sr= &(ed->edSelectRectangle);
    DocumentRectangle		drExpose;
    int				ox= ed->edVisibleRect.drX0;
    int				oy= ed->edVisibleRect.drY0;

    drExpose= ed->edVisibleRect;

    switch( sr->srDirection )
	{
	case DOCselW:
	    drExpose.drX0= sr->srSelected.drX0+ sr->srLTM.drX0;
	    drExpose.drX1= sr->srSelected.drX0+ sr->srLTM.drX1;
	    break;

	case DOCselN:
	    drExpose.drY0= sr->srSelected.drY0+ sr->srLTM.drY0;
	    drExpose.drY1= sr->srSelected.drY0+ sr->srLTM.drY1;
	    break;

	case DOCselE:
	    drExpose.drX0= sr->srSelected.drX1+ sr->srRBM.drX0;
	    drExpose.drX1= sr->srSelected.drX1+ sr->srRBM.drX1;
	    break;

	case DOCselS:
	    drExpose.drY0= sr->srSelected.drY1+ sr->srRBM.drY0;
	    drExpose.drY1= sr->srSelected.drY1+ sr->srRBM.drY1;
	    break;

	default:
	    LDEB(sr->srDirection);
	    return;
	}

    geoShiftRectangle( &drExpose, -ox, -oy );
    guiExposeDrawingWidgetRectangle( ed->edDocumentWidget.dwWidget,
								&drExpose );

    return;
    }

void appMetricRulerChangeValue(	EditDocument *		ed,
				int			newValue )
    {
    SelectRectangle *	sr= &(ed->edSelectRectangle);

    appMetricRulerExposeValue( ed );

    switch( sr->srDirection )
	{
	case DOCselW:
	    sr->srSelected.drX0= newValue;
	    break;

	case DOCselN:
	    sr->srSelected.drY0= newValue;
	    break;

	case DOCselE:
	    sr->srSelected.drX1= newValue;
	    break;

	case DOCselS:
	    sr->srSelected.drY1= newValue;
	    break;

	default:
	    LDEB(sr->srDirection);
	    return;
	}

    appMetricRulerExposeValue( ed );

    return;
    }

/************************************************************************/
/*									*/
/*  Let the user drag one of the controls on the top ruler.		*/
/*									*/
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

static void appHorizontalRulerSetValue(	MetricRulerDrag *	mrd,
					APP_WIDGET		w,
					int			newValue )
    {
    MetricRuler *		mr= mrd->mrdMr;
    RulerData *			rd= &(mr->mrRulerData);
    int				ox= rd->rdVisibleC0- rd->rdMinUsed;
    const int			oy= 0;

    int				oldValue= *(mrd->mrdValuePointer);

    DocumentRectangle		drExpose;

    if  ( newValue == oldValue )
	{ return;	}

    if  ( mrd->mrdValuePointer )
	{ *(mrd->mrdValuePointer)= newValue;	}

    if  ( oldValue < newValue )
	{
	drExpose.drX0= oldValue- rd->rdSizeAcross;
	drExpose.drX1= newValue+ rd->rdSizeAcross;
	drExpose.drY0= 0;
	drExpose.drY1= rd->rdSizeAcross;
	}
    else{
	drExpose.drX0= newValue- rd->rdSizeAcross;
	drExpose.drX1= oldValue+ rd->rdSizeAcross;
	drExpose.drY0= 0;
	drExpose.drY1= rd->rdSizeAcross;
	}

    /*
    appDrawHorizontalRuler( mrd->mrdWidget, mr, &drExpose, mrd->mrdOc );
    */
    geoShiftRectangle( &drExpose, -ox, -oy );
    guiExposeDrawingWidgetRectangle( w, &drExpose );

    /*  5  */
    appMetricRulerChangeValue( mrd->mrdDocument, newValue );

    return;
    }

static APP_EVENT_HANDLER_H( appHorizontalRulerMouseMove, w, vmrd, event )
    {
    MetricRulerDrag *		mrd= (MetricRulerDrag *)vmrd;

    int				x;
    int				y;

    if  ( guiGetCoordinatesFromMouseMoveEvent( &x, &y, w, event ) )
	{ return;	}

    appHorizontalRulerSetValue( mrd, w, x+ mrd->mrdOc );

    return;
    }

static APP_EVENT_HANDLER_H( appHorizontalRulerMouseUp, w, vmrd, event )
    {
    MetricRulerDrag *		mrd= (MetricRulerDrag *)vmrd;

    int				x;
    int				y;

    int				button;
    int				upDown;
    int				seq;
    unsigned int		keyState;

    if  ( guiGetCoordinatesFromMouseButtonEvent(
			&x, &y, &button, &upDown, &seq, &keyState, w, event ) )
	{ return;	}

    appHorizontalRulerSetValue( mrd, w, x+ mrd->mrdOc );
    appMetricRulerExposeValue( mrd->mrdDocument );

    return;
    }

static void appHorizontalRulerDragItem(	int *			pValue,
					int *			pChanged,
					APP_WIDGET		w,
					EditApplication *	ea,
					APP_EVENT *		downEvent,
					MetricRuler *		mr,
					int			ox,
					EditDocument *		ed,
					int			direction )
    {
    SelectRectangle *		sr= &(ed->edSelectRectangle);
    MetricRulerDrag		mrd;
    int				startValue= *pValue;
    int				oldDirection= sr->srDirection;

    mrd.mrdValuePointer= pValue;
    mrd.mrdMr= mr;
    mrd.mrdWidget= w;
    mrd.mrdOc= ox;
    mrd.mrdDocument= ed;

    switch( direction )
	{
	case DOCselW:
	    sr->srSelected.drX0= *pValue;
	    break;

	case DOCselE:
	    sr->srSelected.drX1= *pValue;
	    break;

	default:
	    LDEB(sr->srDirection);
	    return;
	}

    /*  1  */
    sr->srDirection= direction;

    sr->srLTM.drX0= -1;
    sr->srLTM.drX1=  1;
    sr->srLTM.drY0= -1;
    sr->srLTM.drY1=  1;

    sr->srRBM.drX0= -1;
    sr->srRBM.drX1=  1;
    sr->srRBM.drY0= -1;
    sr->srRBM.drY1=  1;

    appMetricRulerExposeValue( ed );

    appRunDragLoop( w, ea, downEvent,
				appHorizontalRulerMouseUp,
				appHorizontalRulerMouseMove,
				0, (APP_TIMER_CALLBACK)0,
				(void *)&mrd );

    if  ( pChanged && *pValue != startValue )
	{ *pChanged= 1;	}

    sr->srDirection= oldDirection;
    return;
    }

/************************************************************************/
/*									*/
/*  The user clicked on the top ruler, make her happy.			*/
/*									*/
/*  1)  Only accept 'left mouse down' events.				*/
/*									*/
/************************************************************************/

void     appHorizontalRulerTrackMouse(	int *			pX0pixels,
					int *			pX1pixels,
					int *			pChanged,
					APP_WIDGET		w,
					EditApplication *	ea,
					APP_EVENT *		downEvent,
					void *			voidmr,
					EditDocument *		ed )
    {
    MetricRuler  *	mr= (MetricRuler *)voidmr;
    RulerData *		rd= &(mr->mrRulerData);

    int			x;
    int			y;
    int			ox= rd->rdVisibleC0- rd->rdMinUsed;
    int			maxUsed= rd->rdSizeAlong- rd->rdExtraAfterMaxUsed;

    int			changed= 0;

    int			xp= 2;

    int			button;
    int			upDown;
    int			seq;
    unsigned int	keyState;

    if  ( guiGetCoordinatesFromMouseButtonEvent(
		    &x, &y, &button, &upDown, &seq, &keyState, w, downEvent ) )
	{ return;	}

    if  ( upDown < 1 || button != 1 || seq > 1 )
	{ return;	}

    if  ( mr->mrV0Pixels >= 0					&&
	  x+ ox <  mr->mrV0Pixels + xp				&&
	  x+ ox >= mr->mrV0Pixels- HAIR_THICK( sizeAcross ) -xp	)
	{
	appHorizontalRulerDragItem( &mr->mrV0Pixels, &changed,
				    w, ea, downEvent, mr, ox, ed, DOCselW );
	goto ready;
	}

    if  ( mr->mrV1Pixels >= 0					&&
	  x+ ox >= mr->mrV1Pixels- xp				&&
	  x+ ox <  mr->mrV1Pixels+ HAIR_THICK( sizeAcross )+ xp	)
	{
	/* asUpper= 1; */

	appHorizontalRulerDragItem( &mr->mrV1Pixels, &changed,
				    w, ea, downEvent, mr, ox, ed, DOCselE );

	goto ready;
	}

    if  ( mr->mrV0Pixels >= 0 )
	{
	if  ( mr->mrV1Pixels >= 0 )
	    { return;	}

	if  ( x+ ox < mr->mrV1Pixels )
	    {
	    /* asUpper= 0; */
	    mr->mrV1Pixels= mr->mrV0Pixels;

	    mr->mrV0Pixels= x+ ox;

	    changed= 1;
	    appHorizontalRulerDragItem( &mr->mrV0Pixels, &changed,
				w, ea, downEvent, mr, ox, ed, DOCselW );

	    goto ready;
	    }
	else{
	    /* asUpper= 1; */
	    mr->mrV1Pixels= x+ ox;

	    changed= 1;

	    appHorizontalRulerDragItem( &mr->mrV1Pixels, &changed,
				    w, ea, downEvent, mr, ox, ed, DOCselE );

	    goto ready;
	    }
	}

    if  ( mr->mrV1Pixels >= 0 )
	{
	if  ( x+ ox > mr->mrV1Pixels )
	    {
	    /* asUpper= 1; */
	    mr->mrV0Pixels= mr->mrV1Pixels;

	    mr->mrV1Pixels= x+ ox;

	    changed= 1;

	    appHorizontalRulerDragItem( &mr->mrV1Pixels, &changed,
				    w, ea, downEvent, mr, ox, ed, DOCselE );

	    goto ready;
	    }
	else{
	    /* asUpper= 0; */
	    mr->mrV0Pixels= x+ ox;

	    changed= 1;

	    appHorizontalRulerDragItem( &mr->mrV0Pixels, &changed,
				    w, ea, downEvent, mr, ox, ed, DOCselE );

	    goto ready;
	    }
	}

    if  ( x+ ox <= ( rd->rdMinUsed+ maxUsed )/ 2 )
	{
	/* asUpper= 0; */
	mr->mrV0Pixels= x+ ox;

	changed= 1;
	appHorizontalRulerDragItem( &mr->mrV0Pixels, &changed,
				    w, ea, downEvent, mr, ox, ed, DOCselW );
	}
    else{
	/* asUpper= 1; */
	mr->mrV1Pixels= x+ ox;

	changed= 1;
	appHorizontalRulerDragItem( &mr->mrV1Pixels, &changed,
				    w, ea, downEvent, mr, ox, ed, DOCselE );
	}

  ready:
    if  ( mr->mrV0Pixels >= 0			&&
	  mr->mrV1Pixels >= 0			&&
	  mr->mrV0Pixels > mr->mrV1Pixels	)
	{
	int		swap;

	swap= mr->mrV1Pixels;
	mr->mrV1Pixels= mr->mrV0Pixels;
	mr->mrV0Pixels= swap;
	changed= 1;
	}

    if  ( changed )
	{
	*pChanged= 1;
	guiExposeDrawingWidget( w );
	}

    *pX0pixels= mr->mrV0Pixels;
    *pX1pixels= mr->mrV1Pixels;

    return;
    }

/************************************************************************/
/*									*/
/*  Let the user drag one of the controls on the left ruler.		*/
/*									*/
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

static void appVerticalRulerSetValue(	MetricRulerDrag *	mrd,
					APP_WIDGET		w,
					int			newValue )
    {
    MetricRuler *		mr= mrd->mrdMr;
    RulerData *			rd= &(mr->mrRulerData);
    const int			ox= 0;
    int				oy= rd->rdVisibleC0- rd->rdMinUsed;

    int				oldValue= *(mrd->mrdValuePointer);

    DocumentRectangle		drExpose;

    if  ( newValue < rd->rdDocumentC0 )
	{ newValue= rd->rdDocumentC0;	}
    if  ( newValue > rd->rdDocumentC1 )
	{ newValue= rd->rdDocumentC1;	}

    if  ( newValue == oldValue )
	{ return;	}

    *(mrd->mrdValuePointer)= newValue;

    if  ( oldValue < newValue )
	{
	drExpose.drX0= 0;
	drExpose.drX1= rd->rdSizeAcross;
	drExpose.drY0= oldValue- rd->rdSizeAcross;
	drExpose.drY1= newValue+ rd->rdSizeAcross;
	}
    else{
	drExpose.drX0= 0;
	drExpose.drX1= rd->rdSizeAcross;
	drExpose.drY0= newValue- rd->rdSizeAcross;
	drExpose.drY1= oldValue+ rd->rdSizeAcross;
	}

    geoShiftRectangle( &drExpose, -ox, -oy );
    guiExposeDrawingWidgetRectangle( w, &drExpose );

    /*  5  */
    appMetricRulerChangeValue( mrd->mrdDocument, newValue );


    return;
    }

static APP_EVENT_HANDLER_H( appVerticalRulerMouseMove, w, vmrd, event )
    {
    MetricRulerDrag *		mrd= (MetricRulerDrag *)vmrd;

    int				x;
    int				y;

    if  ( guiGetCoordinatesFromMouseMoveEvent( &x, &y, w, event ) )
	{ return;	}

    appVerticalRulerSetValue( mrd, w, y+ mrd->mrdOc );

    return;
    }

static APP_EVENT_HANDLER_H( appVerticalRulerMouseUp, w, vmrd, event )
    {
    MetricRulerDrag *	mrd= (MetricRulerDrag *)vmrd;

    int			x;
    int			y;

    int			button;
    int			upDown;
    int			seq;
    unsigned int	keyState;

    if  ( guiGetCoordinatesFromMouseButtonEvent(
			&x, &y, &button, &upDown, &seq, &keyState, w, event ) )
	{ return;	}

    appVerticalRulerSetValue( mrd, w, y+ mrd->mrdOc );
    appMetricRulerExposeValue( mrd->mrdDocument );

    return;
    }

static void appVerticalRulerDragItem(	int *			pValue,
					int *			pChanged,
					APP_WIDGET		w,
					EditApplication *	ea,
					APP_EVENT *		downEvent,
					MetricRuler *		mr,
					int			oy,
					EditDocument *		ed,
					int			direction )
    {
    SelectRectangle *		sr= &(ed->edSelectRectangle);
    MetricRulerDrag		mrd;
    int				startValue= *pValue;
    int				oldDirection= sr->srDirection;

    mrd.mrdValuePointer= pValue;
    mrd.mrdMr= mr;
    mrd.mrdWidget= w;
    mrd.mrdOc= oy;
    mrd.mrdDocument= ed;

    switch( direction )
	{
	case DOCselN:
	    sr->srSelected.drY0= *pValue;
	    break;

	case DOCselS:
	    sr->srSelected.drY1= *pValue;
	    break;

	default:
	    LDEB(sr->srDirection);
	    return;
	}

    /*  1  */
    sr->srDirection= direction;

    sr->srLTM.drX0= -1;
    sr->srLTM.drX1=  1;
    sr->srLTM.drY0= -1;
    sr->srLTM.drY1=  1;

    sr->srRBM.drX0= -1;
    sr->srRBM.drX1=  1;
    sr->srRBM.drY0= -1;
    sr->srRBM.drY1=  1;

    appMetricRulerExposeValue( ed );

    appRunDragLoop( w, ea, downEvent,
				appVerticalRulerMouseUp,
				appVerticalRulerMouseMove,
				0, (APP_TIMER_CALLBACK)0,
				(void *)&mrd );
    if  ( pChanged && *pValue != startValue )
	{ *pChanged= 1;	}

    sr->srDirection= oldDirection;
    return;
    }

/************************************************************************/
/*									*/
/*  The user clicked on the left ruler, make her happy.			*/
/*									*/
/*  1)  Only accept 'left mouse down' events.				*/
/*									*/
/************************************************************************/

void     appVerticalRulerTrackMouse(	int *			pY0pixels,
					int *			pY1pixels,
					int *			pChanged,
					APP_WIDGET		w,
					EditApplication *	ea,
					APP_EVENT *		downEvent,
					void *			voidmr,
					EditDocument *		ed )
    {
    MetricRuler  *	mr= (MetricRuler *)voidmr;
    RulerData *		rd= &(mr->mrRulerData);

    int			x;
    int			y;
    int			oy= rd->rdVisibleC0- rd->rdMinUsed;
    int			maxUsed= rd->rdSizeAlong- rd->rdExtraAfterMaxUsed;

    int			changed= 0;

    int			xp= 2;

    int			button;
    int			upDown;
    int			seq;
    unsigned int	keyState;

    if  ( guiGetCoordinatesFromMouseButtonEvent(
		    &x, &y, &button, &upDown, &seq, &keyState, w, downEvent ) )
	{ return;	}

    if  ( upDown < 1 || button != 1 || seq > 1 )
	{ return;	}

    if  ( mr->mrV0Pixels >= 0					&&
	  y+ oy <  mr->mrV0Pixels + xp				&&
	  y+ oy >= mr->mrV0Pixels- HAIR_THICK( sizeAcross ) -xp	)
	{
	/* asUpper= 0; */

	appVerticalRulerDragItem( &(mr->mrV0Pixels), &changed,
				    w, ea, downEvent, mr, oy, ed, DOCselN );

	goto ready;
	}

    if  ( mr->mrV1Pixels >= 0					&&
	  y+ oy >= mr->mrV1Pixels- xp			&&
	  y+ oy <  mr->mrV1Pixels+ HAIR_THICK( sizeAcross )+ xp	)
	{
	/* asUpper= 1; */

	appVerticalRulerDragItem( &mr->mrV1Pixels, &changed,
				w, ea, downEvent, mr, oy, ed, DOCselS );

	goto ready;
	}

    if  ( mr->mrV0Pixels >= 0 )
	{
	if  ( mr->mrV1Pixels >= 0 )
	    { return;	}

	if  ( y+ oy < mr->mrV1Pixels )
	    {
	    /* asUpper= 0; */
	    mr->mrV1Pixels= mr->mrV0Pixels;

	    mr->mrV0Pixels= y+ oy;

	    changed= 1;
	    appVerticalRulerDragItem( &mr->mrV0Pixels, &changed,
				    w, ea, downEvent, mr, oy, ed, DOCselN );

	    goto ready;
	    }
	else{
	    /* asUpper= 1; */
	    mr->mrV1Pixels= y+ oy;

	    changed= 1;
	    appVerticalRulerDragItem( &mr->mrV1Pixels, &changed,
				    w, ea, downEvent, mr, oy, ed, DOCselS );

	    goto ready;
	    }
	}

    if  ( mr->mrV1Pixels >= 0 )
	{
	if  ( y+ oy > mr->mrV1Pixels )
	    {
	    /* asUpper= 1; */
	    mr->mrV0Pixels= mr->mrV1Pixels;

	    mr->mrV1Pixels= y+ oy;

	    changed= 1;
	    appVerticalRulerDragItem( &mr->mrV1Pixels, &changed,
				w, ea, downEvent, mr, oy, ed, DOCselS );

	    goto ready;
	    }
	else{
	    /* asUpper= 0; */
	    mr->mrV0Pixels= y+ oy;

	    changed= 1;
	    appVerticalRulerDragItem( &mr->mrV0Pixels, &changed,
				    w, ea, downEvent, mr, oy, ed, DOCselN );

	    goto ready;
	    }
	}

    if  ( y+ oy <= ( rd->rdMinUsed+ maxUsed )/ 2 )
	{
	/* asUpper= 0; */
	mr->mrV0Pixels= y+ oy;

	changed= 1;
	appVerticalRulerDragItem( &mr->mrV0Pixels, &changed,
				    w, ea, downEvent, mr, oy, ed, DOCselN );

	goto ready;
	}
    else{
	/* asUpper= 1; */
	mr->mrV1Pixels= y+ oy;

	changed= 1;
	appVerticalRulerDragItem( &mr->mrV1Pixels, &changed,
				    w, ea, downEvent, mr, oy, ed, DOCselS );

	goto ready;
	}

  ready:
    if  ( mr->mrV0Pixels >= 0			&&
	  mr->mrV1Pixels >= 0			&&
	  mr->mrV0Pixels > mr->mrV1Pixels	)
	{
	int		swap;

	swap= mr->mrV1Pixels;
	mr->mrV1Pixels= mr->mrV0Pixels;
	mr->mrV0Pixels= swap;
	changed= 1;
	}

    if  ( changed )
	{
	*pChanged= 1;
	guiExposeDrawingWidget( w );
	}

    *pY0pixels= mr->mrV0Pixels;
    *pY1pixels= mr->mrV1Pixels;

    return;
    }

/************************************************************************/
/*									*/
/*  Set the selection of a horizontal/vertical ruler.			*/
/*									*/
/************************************************************************/

void appSetMetricRulerValues(	APP_WIDGET	w,
				void *		voidmr,
				int		v0,
				int		v1 )
    {
    MetricRuler  *	mr= (MetricRuler *)voidmr;
    int			changed= 0;

    if  ( v0 != mr->mrV0Pixels )
	{ mr->mrV0Pixels= v0; changed= 1; }

    if  ( v1 != mr->mrV1Pixels )
	{ mr->mrV1Pixels= v1; changed= 1; }

    if  ( changed )
	{ guiExposeDrawingWidget( w );	}
    }

void appSetMetricRulerMagnification(	APP_WIDGET	w,
					void *		voidmr,
					double		magnifiedPixelsPerTwip,
					double		magnification )
    {
    MetricRuler  *		mr= (MetricRuler *)voidmr;
    RulerData *			rd= &(mr->mrRulerData);

    appRulerCalculateIncrements( rd, magnifiedPixelsPerTwip, magnification );

    guiExposeDrawingWidget( w );

    return;
    }

void appAdaptMetricRuler(	APP_WIDGET	w,
				void *		voidmr,
				int		documentC1 )
    {
    MetricRuler  *	mr= (MetricRuler *)voidmr;
    RulerData *		rd= &(mr->mrRulerData);

    if  ( rd->rdDocumentC1 != documentC1 )
	{
	rd->rdDocumentC1= documentC1;

	guiExposeDrawingWidget( w );
	}
    }

