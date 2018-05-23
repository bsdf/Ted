/************************************************************************/
/*									*/
/*  A surface to draw upon.						*/
/*									*/
/************************************************************************/

#   ifndef		GUI_DRAWING_SURFACE_H
#   define		GUI_DRAWING_SURFACE_H

#   include		<utilColor.h>
#   include		<geo2DInteger.h>
#   include		<geoAffineTransform.h>
#   include		<psFontInfo.h>
#   include		<bitmap.h>

struct DrawingSurface;
typedef struct DrawingSurface * DrawingSurface;

/************************************************************************/
/*									*/
/*  Line attributes.							*/
/*									*/
/************************************************************************/

typedef enum LineStyle
    {
    LineStyleSolid,
    LineStyleSingleDash,
    LineStyleDoubleDash,

    LineStyle_Count
    } LineStyle;

typedef enum LineCap
    {
    LineCapNotLast,
    LineCapButt,
    LineCapRound,
    LineCapProjecting,

    LineCap_Count
    } LineCap;

typedef enum LineJoin
    {
    LineJoinMiter,
    LineJoinRound,
    LineJoinBevel,

    LineJoin_Count
    } LineJoin;

/************************************************************************/
/*									*/
/*  Routine Declarations.						*/
/*									*/
/************************************************************************/

extern void drawFreeDrawingSurface(	DrawingSurface		ds );

extern int drawSetForegroundColor(	DrawingSurface		ds,
					const RGB8Color *	rgb8 );

extern void drawFillRectangle(	DrawingSurface			ds,
				const DocumentRectangle *	drLogical );
extern void drawRectangle(	DrawingSurface			ds,
				const DocumentRectangle *	drLogical );

extern int drawOpenScreenFont(	DrawingSurface			ds,
				const AfmFontInfo *		afi,
				int				pixelSize,
				const IndexSet *		unicodesWanted);

extern DrawingSurface drawMakeDrawingSurfaceForParent(
				DrawingSurface			parent,
				int				wide,
				int				high );

extern DrawingSurface drawMakeDrawingSurfaceForImageAndParent(
				DrawingSurface			parent,
				const RasterImage *		abi,
				const DocumentRectangle *	drSrc,
				int				wide,
				int				high );

extern int drawRasterImage(	DrawingSurface			ds,
				const DocumentRectangle *	drDest,
				const RasterImage *		abi,
				const DocumentRectangle *	drSrc );

extern int drawString(		DrawingSurface			ds,
				int				x0,
				int				y0,
				int				screenFont,
				const char *			s,
				int				len );

extern void drawSymbol(		DrawingSurface			ds,
				int				x,
				int				y,
				int				screenFont,
				int				symbol );

extern int drawSetLineAttributes(	DrawingSurface		ds,
					int			lineWidth,
					int			lineStyle,
					int			capStyle,
					int			joinStyle,
					const unsigned char *	dashList,
					int			dashCount );

extern int drawLine(		DrawingSurface			ds,
				int				x0,
				int				y0,
				int				x1,
				int				y1 );

extern int drawLines(		DrawingSurface			ds,
				const Point2DI *		points,
				int				count,
				int				close );

extern int drawFillPolygon(	DrawingSurface			ds,
				const Point2DI *		points,
				int				count );

extern void drawArc(		DrawingSurface			ds,
				const Arc2DI *			arc );

extern void drawFillArc(	DrawingSurface			ds,
				const Arc2DI *			arc );

extern int drawGetTextExtents(	DocumentRectangle *		drText,
				const DrawingSurface		ds,
				int				x0,
				int				y0,
				int				screenFont,
				const char *			s,
				int				len );

extern int drawGetSymbolExtents( DocumentRectangle *	drText,
				DrawingSurface		ds,
				int			x0,
				int			y0,
				int			screenFont,
				int			symbol );

extern int drawGetUnderlineGeometry(
				int *			pThick,
				int *			pY,
				const DrawingSurface	ds,
				int			screenFont,
				int			baselinePixels );

extern int drawGetStrikethroughGeometry(
				int *			pThick,
				int *			pY,
				const DrawingSurface	ds,
				int			screenFont,
				int			baselinePixels );

extern int drawSetForegroundColorWhite(		DrawingSurface	ds );
extern int drawSetForegroundColorBlack(		DrawingSurface	ds );

extern int drawRoundedRect(	DrawingSurface			ds,
				const DocumentRectangle *	dr,
				int				w,
				int				h );

extern int drawFillRoundedRect(	DrawingSurface			ds,
				const DocumentRectangle *	dr,
				int				w,
				int				h );

extern int drawGetSubBaseline(	int *			pSubBaseline,
				const DrawingSurface	ds,
				int			screenFont,
				int			baselinePixels );

extern int drawGetSuperBaseline(int *			pSuperBaseline,
				const DrawingSurface	ds,
				int			screenFont,
				int			baselinePixels );

extern void drawSetClipRect(	DrawingSurface			ds,
				const DocumentRectangle *	drClip );

extern void drawNoClipping(	DrawingSurface			ds );

extern void drawChildSurface(	DrawingSurface			ds,
				const DrawingSurface		child,
				int				xDest,
				int				yDest,
				const DocumentRectangle *	drChild );

#   endif	/*	GUI_DRAWING_SURFACE_H	*/
