/************************************************************************/
/*									*/
/*  Ted, Screen drawing and forcing drawing through			*/
/*  guiExposeDrawingWidget().						*/
/*									*/
/************************************************************************/

#   ifndef	TED_DRAW_H
#   define	TED_DRAW_H

#   include	<docDraw.h>
#   include	<drawDrawingSurface.h>

#   define	LOG_REDRAWS	0
#   define	BLINK_IBAR	1

#   if		LOG_REDRAWS
#	undef	BLINK_IBAR
#	define	BLINK_IBAR	0
#   endif

typedef struct ScreenDrawingData
    {
    RGB8Color			sddForeColor;
    RGB8Color			sddBehindColor;
    RGB8Color			sddGridColor;

    DocumentTree *		sddSelRootTree;
    struct BufferItem *		sddSelRootNode;
    struct BufferItem *		sddSelRootBodySectNode;
    int				sddBoxFirstPage;
    int				sddBoxLastPage;

    const DocumentRectangle *	sddFullRect;
    } ScreenDrawingData;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int tedDrawDrawingShape(	const DocumentRectangle *	drTwips,
				int				page,
				struct DrawingShape *		ds,
				DrawingContext *		dc,
				void *				vsdd );

extern int tedDrawTextReverse(	struct BufferItem *		paraBi,
				int				line,
				const ParagraphFrame *		pf,
				const DocumentRectangle *	drLine,
				void *				vsdd,
				DrawingContext *		dc,
				const BlockOrigin *		bo );

extern int tedDrawTextSelected(	struct BufferItem *		paraBi,
				int				line,
				const ParagraphFrame *		pf,
				const DocumentRectangle *	drLine,
				void *				vsdd,
				DrawingContext *		dc,
				const BlockOrigin *		bo );

extern int tedDrawTextLine(	struct BufferItem *		paraBi,
				int				line,
				const ParagraphFrame *		pf,
				const DocumentRectangle *	drLine,
				void *				vsdd,
				DrawingContext *		dc,
				const BlockOrigin *		bo );

extern void tedOriginalClipping( DrawingContext *		dc,
				ScreenDrawingData *		sdd );

extern void tedDrawFreePixmap(	DrawingSurface			sp );

extern int tedDrawIBar(	const DocumentRectangle *	drPixels,
			const LayoutContext *		lc );

extern void tedGetIBarRect(
			DocumentRectangle *		drPixels,
			const PositionGeometry *	pg,
			const LayoutContext *		lc );

extern int tedDrawObject(
			const DrawTextLine *		dtl,
			int				part,
			InsertedObject *		io,
			int				x0Twips,
			int				x1Twips,
			const LayoutPosition *		baseLine );

extern void tedDrawShadedRectangle(
			const LayoutContext *		lc,
			DrawingSurface *		shadingPixmaps,
			int				pattern,
			const DocumentRectangle *	drShade );

extern void tedDrawHorizontalBorderLine(DrawingSurface		ds,
					int			style,
					int			minThick,
					int			x0,
					int			x1,
					int			y );

extern int tedDrawTab(	const DrawTextLine *		dtl,
			int				part,
			int				textAttrNr,
			const TextAttribute *		ta,
			int				leader,
			int				x0Twips,
			int				x1Twips,
			const LayoutPosition *		baseLine );

extern int tedDrawChftnsep( const DrawTextLine *	dtl,
			int				part,
			int				textAttrNr,
			const TextAttribute *		ta,
			int				x0Twips,
			int				x1Twips,
			const LayoutPosition *		baseLine );

extern int tedDrawSpan(	const DrawTextLine *		dtl,
			int				part,
			int				count,
			const LayoutPosition *		baseLine,
			int				textAttrNr,
			const TextAttribute *		ta,
			const char *			printString,
			int				nbLen );

extern int tedDrawRunUnderline(		const DrawTextLine *	dtl,
					int			part,
					int			upto,
					int			textAttrNr,
					const TextAttribute *	ta,
					int			x0Twips,
					int			x1Twips,
					const LayoutPosition *	baseLine );

extern int tedDrawRunStrikethrough(	const DrawTextLine *	dtl,
					int			part,
					int			upto,
					int			textAttrNr,
					const TextAttribute *	ta,
					int			x0Twips,
					int			x1Twips,
					const LayoutPosition *	baseLine );

extern int tedDrawOrnaments(	const BlockOrnaments *		bo,
				int				page,
				const DocumentRectangle *	drOutside,
				const DocumentRectangle *	drInside,
				void *				through,
				struct DrawingContext *		dc );

extern int tedDrawSetColorRgb(		DrawingContext *	dc,
					void *			vsdd,
					const RGB8Color *	rgb8 );

#   endif	/*  TED_DRAW_H	*/
