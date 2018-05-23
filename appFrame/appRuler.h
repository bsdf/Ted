/************************************************************************/
/*									*/
/*  Common resources used by Rulers.					*/
/*									*/
/************************************************************************/

#   ifndef	APP_RULER_H
#   define	APP_RULER_H

#   include	"appGuiBase.h"
#   include	<drawDrawingSurface.h>
#   include	<psPostScriptFontList.h>

typedef struct RulerData
    {
				/**
				 *  The size in pixels across the ruler.
				 *  This is the actual size of the widget
				 *
				 *  Height of a horizontal ruler, or width 
				 *  of a vertical ruler.
				 */
    int				rdSizeAcross;

				/**
				 *  The size in pixels along the ruler.
				 *  This is the actual size of the widget
				 */
    int				rdSizeAlong;

				/**
				 *  The size in pixels of the font that 
				 *  is used to draw symbols on the ruler.
				 */
    int				rdFontHeight;

				/**
				 *  Space on the widget that is not used 
				 *  for the ruler. E.G: The area on a top 
				 *  ruler that is above the left ruler.
				 */
    int				rdMinUsed;
				/**
				 *  Space on the widget that is not used 
				 *  for the ruler. E.G: The area on a top 
				 *  ruler that is above the right ruler.
				 */
    int				rdExtraAfterMaxUsed;

				/**
				  *  Pixel coordinate ranges for the ruler.
				  */
    int				rdDocumentC0;
    int				rdDocumentC1;
    int				rdVisibleC0;
    int				rdVisibleC1;

    int				rdScreenFont;
    DrawingSurface		rdDrawingSurface;
    const PostScriptFontList *	rdPostScriptFontList;

    int				rdBackgroundExplicit;

				/**
				 *  Colors to use to draw the ruler:
				 *  Draw the background of the range that is 
				 *  selected.
				 */
    RGB8Color			rdSelectedColor;
				/**
				 *  Colors to use to draw the ruler:
				 *  Draw the background of the ruler.
				 */
    RGB8Color			rdBackgroundColor;
				/**
				 *  Colors to use to draw the ruler:
				 *  Draw the foreground of the ruler.
				 *  Only applies for those parts of the ruler 
				 *  that have a background close to the 
				 *  foreground color.
				 */
    RGB8Color			rdForegroundColor;
				/**
				 *  Colors to use to draw the ruler:
				 *  Draw the top shadow of three dimensional 
				 *  controls.
				 */
    RGB8Color			rdTopShadowColor;
				/**
				 *  Colors to use to draw the ruler:
				 *  Draw the bottom shadow of three dimensional 
				 *  controls.
				 */
    RGB8Color			rdBottomShadowColor;

    int				rdUnit;
    int				rdTicksPerTag;
    int				rdTicksPerText;
    double			rdUnitsPerTick;

    double			rdTwipsPerUnit;
    double			rdMagnifiedPixelsPerTwip;
    } RulerData;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void appRulerSetBackground(	RulerData *		rd,
					const RGB8Color *	back );

extern void appInitRulerData(		RulerData *	rd );

extern void appCleanRulerData(		RulerData *	rd );

extern void appRulerDrawBackground(	RulerData *			rd,
					const DocumentRectangle *	drBck );

extern void appRulerMakeDrawingSurface( RulerData *	rd,
					int		vertical,
					int		fontSizeHintPixels,
					double		magnification,
					APP_WIDGET	w );

extern int appRulerTextWidth(		RulerData *		rd,
					const char *		s,
					int			len );

extern void appRulerCalculateIncrements( RulerData *	rd,
					double		magnifiedPixelsPerTwip,
					double		magnification );

extern void appRulerTagText(		char *		to,
					int *		pWide,
					int *		pLen,
					RulerData *	rd,
					double		units );

extern void appScrollHorizontalRuler(	RulerData *		rd,
					DocumentRectangle *	drClip,
					int			d );

void appScrollVerticalRuler(		RulerData *		rd,
					DocumentRectangle *	drClip,
					int			d );

extern void appHorizontalRulerGetSizeFromConfigureEvent(
					RulerData *		rd,
					APP_WIDGET		w,
					APP_EVENT *		event );

extern void appVerticalRulerGetSizeFromConfigureEvent(
					RulerData *		rd,
					APP_WIDGET		w,
					APP_EVENT *		event );

extern void appHorizontalRulerGetSizeFromWidget(
					RulerData *		rd,
					APP_WIDGET		w );

extern void appVerticalRulerGetSizeFromWidget(
					RulerData *		rd,
					APP_WIDGET		w );

#   endif
