#   ifndef	DRAW_SCREEN_FONT_UTIL_H
#   define	DRAW_SCREEN_FONT_UTIL_H

#   include	<drawDrawingSurface.h>

# ifdef __cplusplus
extern "C"
    {
# endif

/************************************************************************/
/*									*/
/*  Procedure declarations.						*/
/*									*/
/************************************************************************/

extern void drawFontUnderlineGeometry(	int *			pThick,
					int *			pY,
					const DrawingSurface	ds,
					int			screenFont,
					int			baselinePixels);

extern void drawFontStrikethroughGeometry(
					int *			pThick,
					int *			pY,
					const DrawingSurface	ds,
					int			screenFont,
					int			baselinePixels);

extern void drawFontGetSuperBaseline(	int *			pSuperBaseline,
					int			baselinePixels,
					const DrawingSurface	ds,
					int			screenFont );

extern void drawFontGetSubBaseline(	int *			pSubBaseline,
					int			baselinePixels,
					const DrawingSurface	ds,
					int			screenFont );

#   endif	/*  DRAW_SCREEN_FONT_UTIL_H	*/
