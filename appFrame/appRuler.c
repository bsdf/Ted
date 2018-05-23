/************************************************************************/
/*									*/
/*  Common resources used by Rulers.					*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"
#   include	"guiWidgetDrawingSurface.h"
#   include	"guiDrawingWidget.h"
#   include	"drawDrawingSurfacePrivate.h"

#   include	<stdio.h>
#   include	<string.h>

#   define	y0	math_y0
#   define	y1	math_y1
#   include	<math.h>
#   undef	y0
#   undef	y1

#   include	<appUnit.h>
#   include	"appRuler.h"

#   include	<appDebugon.h>

#   ifndef	M_LN10
#	define	M_LN10	2.30258509299404568402
#   endif

#   define	SHOW_SCROLLS	0

void appInitRulerData(	RulerData *	rd )
    {
    rd->rdSizeAcross= 0;
    rd->rdSizeAlong= 0;
    rd->rdFontHeight= 0;

    rd->rdMinUsed= 0;
    rd->rdExtraAfterMaxUsed= 0;

    rd->rdDocumentC0= 0;
    rd->rdDocumentC1= 0;
    rd->rdVisibleC0= 0;
    rd->rdVisibleC1= 0;

    rd->rdScreenFont= -1;
    rd->rdDrawingSurface= (DrawingSurface)0;
    rd->rdPostScriptFontList= (const PostScriptFontList *)0;

    rd->rdBackgroundExplicit= 0;

    utilInitRGB8Color( &(rd->rdSelectedColor) );
    utilInitRGB8Color( &(rd->rdBackgroundColor) );
    utilInitRGB8Color( &(rd->rdForegroundColor) );
    utilInitRGB8Color( &(rd->rdTopShadowColor) );
    utilInitRGB8Color( &(rd->rdBottomShadowColor) );

    rd->rdUnit= -1;
    rd->rdTicksPerTag= 0;
    rd->rdTicksPerText= 0;
    rd->rdUnitsPerTick= 0.0;

    rd->rdTwipsPerUnit= 0.0;
    rd->rdMagnifiedPixelsPerTwip= 0.0;

    return;
    }

void appCleanRulerData(	RulerData *	rd )
    {
    if  ( rd->rdDrawingSurface )
	{ drawFreeDrawingSurface( rd->rdDrawingSurface );	}

    return;
    }

void appHorizontalRulerGetSizeFromConfigureEvent( RulerData *	rd,
						APP_WIDGET	w,
						APP_EVENT *	event )
    {
    int			wide;
    int			high;

    if  ( guiDrawGetSizeFromConfigureEvent( &wide, &high, w, event ) )
	{ return;	}

    rd->rdSizeAlong= wide;
    rd->rdSizeAcross= high;

    return;
    }

void appHorizontalRulerGetSizeFromWidget( 	RulerData *	rd,
						APP_WIDGET	w )
    {
    int			wide;
    int			high;

    if  ( guiDrawGetSizeOfWidget( &wide, &high, w ) )
	{ return;	}

    rd->rdSizeAlong= wide;
    rd->rdSizeAcross= high;

    return;
    }

void appVerticalRulerGetSizeFromConfigureEvent( RulerData *	rd,
						APP_WIDGET	w,
						APP_EVENT *	event )
    {
    int			wide;
    int			high;

    if  ( guiDrawGetSizeFromConfigureEvent( &wide, &high, w, event ) )
	{ return;	}

    rd->rdSizeAlong= high;
    rd->rdSizeAcross= wide;

    return;
    }

void appVerticalRulerGetSizeFromWidget( 	RulerData *	rd,
						APP_WIDGET	w )
    {
    int			wide;
    int			high;

    if  ( guiDrawGetSizeOfWidget( &wide, &high, w ) )
	{ return;	}

    rd->rdSizeAlong= high;
    rd->rdSizeAcross= wide;

    return;
    }

void appRulerDrawBackground(	RulerData *			rd,
				const DocumentRectangle *	drBack )
    {
    DrawingSurface	ds= rd->rdDrawingSurface;

    drawSetForegroundColor( ds, &(rd->rdBackgroundColor) );

    drawFillRectangle( ds, drBack );

    return;
    }

void appRulerMakeDrawingSurface( RulerData *		rd,
				int			vertical,
				int			fontSizeHintPixels,
				double			magnification,
				APP_WIDGET		w )
    {
    rd->rdFontHeight= fontSizeHintPixels;

    rd->rdDrawingSurface= guiDrawingSurfaceForNativeWidget( w,
			    rd->rdPostScriptFontList->psflAvoidFontconfig );

    drawSetLineAttributes( rd->rdDrawingSurface,
			1, LineStyleSolid, LineCapProjecting, LineJoinMiter,
			(const unsigned char *)0, 0 );

    if  ( ! rd->rdBackgroundExplicit )
	{
	guiGetBackgroundColor( &(rd->rdBackgroundColor), w );
	guiGetForegroundColor( &(rd->rdForegroundColor), w );
	guiGetTopShadowColor( &(rd->rdTopShadowColor), w );
	guiGetBottomShadowColor( &(rd->rdBottomShadowColor), w );
	rd->rdSelectedColor= rd->rdBottomShadowColor;
	}

    rd->rdScreenFont= drawOpenDefaultFont( rd->rdDrawingSurface,
						    rd->rdPostScriptFontList,
						    w, fontSizeHintPixels );
    if  ( rd->rdScreenFont < 0 )
	{ LLDEB(fontSizeHintPixels,rd->rdScreenFont);	}

    return;
    }

void appRulerSetBackground(	RulerData *		rd,
				const RGB8Color *	back )
    {
    rd->rdBackgroundExplicit= 1;

    rd->rdBackgroundColor= *back;

    utilGetForegroundColor( &(rd->rdForegroundColor), back );
    utilGetTopShadowColor( &(rd->rdTopShadowColor), back );
    utilGetBottomShadowColor( &(rd->rdBottomShadowColor), back );
    rd->rdSelectedColor= rd->rdBottomShadowColor;
    }

int appRulerTextWidth(	RulerData *		rd,
			const char *		s,
			int			len )
    {
    if  ( rd->rdScreenFont >= 0 )
	{
	DocumentRectangle	drText;

	const int		x= 0;
	const int		y= 0;

	drawGetTextExtents( &drText, rd->rdDrawingSurface, x, y,
						rd->rdScreenFont, s, len );

	return drText.drX1- drText.drX0+ 1;
	}
    else{ return  (6*rd->rdFontHeight*len)/10;			}
    }

/************************************************************************/
/*									*/
/*  Calculate suitable distances for Texts, tags, ticks			*/
/*									*/
/*  unitsPerTick= unitsPerTag/ticksPerTag;				*/
/*  screenUnitsPerTick= unitsPerTick* magnification.			*/
/*  pixelsPerTick= FAC* screenUnitsPerTick.				*/
/*									*/
/************************************************************************/

static double appRulerPerItem(	double		unitsPerItem )
    {
    double aa;
    double bb;

    aa= log( unitsPerItem )/ M_LN10;
    bb= (int)( aa+ 20 )- 20;

    if  ( aa- bb > log( 5.0 )/ M_LN10	)
	{ aa= bb+ 1;	}
    else{
	if  ( aa- bb > log( 2.0 )/ M_LN10	)
	    { aa= bb+ log( 5.0 )/ M_LN10;	}
	else{
	    if  ( aa- bb > 0.0			)
		{ aa= bb+ log( 2.0 )/ M_LN10;	}
	    }
	}

    return exp( M_LN10* aa );
    }

void appRulerCalculateIncrements(	RulerData *	rd,
					double		magnifiedPixelsPerTwip,
					double		magnification )
    {
    double		mmPerUnit;

    double		unitsPerText;
    double		unitsPerTag;
    double		unitsPerTick;

    int			tagsPerText;

    switch( rd->rdUnit )
	{
	case UNITtyMM:		mmPerUnit= 1.0;		break;
	case UNITtyCM:		mmPerUnit= 10.0;	break;
	case UNITtyINCH:	mmPerUnit= 24.4;	break;
	case UNITtyPOINTS:	mmPerUnit= 25.4/ 72.0;	break;
	case UNITtyPICAS:	mmPerUnit= 25.4/ 6.0;	break;
	default:
	    LDEB(rd->rdUnit);
	    rd->rdUnit= UNITtyMM;
	    mmPerUnit= 1.0;
	    break;
	}

    rd->rdTwipsPerUnit= appUnitToTwips( 1.0, rd->rdUnit );
    rd->rdMagnifiedPixelsPerTwip= magnifiedPixelsPerTwip;

    unitsPerText= appRulerPerItem( ( 30.0/ mmPerUnit )/ magnification );
    unitsPerTag=  appRulerPerItem( (  8.0/ mmPerUnit )/ magnification );
    unitsPerTick= appRulerPerItem( (  3.0/ mmPerUnit )/ magnification );

    tagsPerText= unitsPerText/ unitsPerTag+ 0.49;
    rd->rdTicksPerTag= unitsPerTag/ unitsPerTick+ 0.49;

    rd->rdTicksPerText= tagsPerText* rd->rdTicksPerTag;
    rd->rdUnitsPerTick= unitsPerText/ rd->rdTicksPerText;

    return;
    }

void appRulerTagText(	char *		to,
			int *		pWide,
			int *		pLen,
			RulerData *	rd,
			double		units )
    {
    char	scratch[30];
    char *	s;

    int		wide;
    int		len;

    sprintf ( scratch, "%12.3g", units );
    s= scratch; while( *s == ' ' ) { s++;	}
    len= strlen( s );

    wide= appRulerTextWidth( rd, s, len );

    strcpy( to, s ); *pWide= wide; *pLen= len;

    return;
    }

# if SHOW_SCROLLS
static void appShowCopy(	DrawingSurface			ds,
				int				x,
				int				y,
				const DocumentRectangle *	drSrc )
    {
    RGB8Color		blue;
    DocumentRectangle	drTarget;

    blue.rgb8Red= 0;
    blue.rgb8Green= 0;
    blue.rgb8Blue= 255;

    drTarget= *drSrc;

    geoShiftRectangle( &drTarget, x- drSrc->drX0, y- drSrc->drY0 );
    drTarget.drX1--;
    drTarget.drY1--;

    drawSetForegroundColor( ds, &blue );
    drawRectangle( ds, &drTarget );

    return;
    }
# endif

void appScrollHorizontalRuler(		RulerData *		rd,
					DocumentRectangle *	drClip,
					int			srcolledX )
    {
    int			maxUsed= rd->rdSizeAlong- rd->rdExtraAfterMaxUsed;
    DrawingSurface	ds= rd->rdDrawingSurface;

    DocumentRectangle	drAll;
    DocumentRectangle	drSrc;

    rd->rdVisibleC0 += srcolledX;
    rd->rdVisibleC1 += srcolledX;

    drAll.drX0= rd->rdMinUsed;
    drAll.drX1= maxUsed;
    drAll.drY0= 0;
    drAll.drY1= rd->rdSizeAcross- 1;

    *drClip= drAll;
    drSrc= drAll;

    if  ( srcolledX > 0 )
	{
	drSrc.drX0=   drAll.drX0+ srcolledX;
	drClip->drX0= drAll.drX1- srcolledX+ 1;

	if  ( drSrc.drX0 <= drSrc.drX1 )
	    {
	    drawMoveArea( ds, drAll.drX0, drAll.drY0, &drSrc );
#	    if SHOW_SCROLLS
	    appShowCopy( ds, drAll.drX0, drAll.drY0, &drSrc );
#	    endif
	    }
	}
    else{
	drSrc.drX1=   drAll.drX1+ srcolledX;
	drClip->drX1= drAll.drX0- srcolledX- 1;

	if  ( drSrc.drX0 <= drSrc.drX1 )
	    {
	    drawMoveArea( ds, drAll.drX0- srcolledX, drAll.drY0, &drSrc );
#	    if SHOW_SCROLLS
	    appShowCopy( ds, drAll.drX0- srcolledX, drAll.drY0, &drSrc );
#	    endif
	    }
	}

    return;
    }

void appScrollVerticalRuler(		RulerData *		rd,
					DocumentRectangle *	drClip,
					int			scrolledY )
    {
    int			maxUsed= rd->rdSizeAlong- rd->rdExtraAfterMaxUsed;
    int			high= maxUsed+ 1;
    int			wide= rd->rdSizeAcross;

    DrawingSurface	ds= rd->rdDrawingSurface;

    int			y0;

    rd->rdVisibleC0 += scrolledY;
    rd->rdVisibleC1 += scrolledY;

    y0= rd->rdMinUsed;

    drClip->drX0= 0;
    drClip->drX1= wide;

    if  ( scrolledY > 0 )
	{
	if  ( high > scrolledY )
	    {
	    DocumentRectangle	drSrc;

	    drSrc.drX0= 0;
	    drSrc.drX1= rd->rdSizeAcross- 1;
	    drSrc.drY0= scrolledY+ y0;
	    drSrc.drY1= maxUsed;

	    drawMoveArea( ds, 0, y0, &drSrc );

	    drClip->drY0= rd->rdVisibleC1- scrolledY;
	    drClip->drY1= rd->rdVisibleC1;
	    }
	else{
	    drClip->drY0= rd->rdVisibleC0;
	    drClip->drY1= rd->rdVisibleC1;
	    }
	}
    else{
	if  ( high+ scrolledY > 0 )
	    {
	    DocumentRectangle	drSrc;

	    drSrc.drX0= 0;
	    drSrc.drX1= rd->rdSizeAcross- 1;
	    drSrc.drY0= y0;
	    drSrc.drY1= high+ scrolledY;

	    drawMoveArea( ds, 0, y0- scrolledY, &drSrc );

	    drClip->drY0= rd->rdVisibleC0;
	    drClip->drY1= rd->rdVisibleC0- scrolledY;
	    }
	else{
	    drClip->drY0= rd->rdVisibleC0;
	    drClip->drY1= rd->rdVisibleC1;
	    }
	}

    return;
    }
