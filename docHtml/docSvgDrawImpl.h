/************************************************************************/
/*									*/
/*  SVG printing.							*/
/*									*/
/************************************************************************/

#   include	<sioGeneral.h>
#   include	<svgWriter.h>
#   include	<docBuf.h>
#   include	<docDraw.h>
#   include	<docParticuleData.h>
#   include	<geoAffineTransform.h>

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int docSvgEmitStroke(	SvgWriter *			sw,
				const struct DrawingShape *	ds );

extern int docSvgEmitFill(	SvgWriter *			sw,
				const struct DrawingShape *	ds );

extern int docSvgDrawDrawDrawingShape(
				const DocumentRectangle *	drTwips,
				int				page,
				struct DrawingShape *		ds,
				DrawingContext *		dc,
				void *				vsw );

extern int docSvgDrawObject(	const DrawTextLine *		dtl,
				int				part,
				InsertedObject *		io,
				int				x0Twips,
				int				x1Twips,
				const LayoutPosition *		baseLine );

extern int docSvgDrawTab(	const DrawTextLine *		dtl,
				int				part,
				int				textAttrNr,
				const TextAttribute *		ta,
				int				leader,
				int				x0Twips,
				int				x1Twips,
				const LayoutPosition *		baseLine );

extern int docSvgDrawFtnsep(	const DrawTextLine *		dtl,
				int				part,
				int				textAttrNr,
				const TextAttribute *		ta,
				int				x0Twips,
				int				x1Twips,
				const LayoutPosition *		baseLine );

extern int docSvgDrawSpan(	const DrawTextLine *		dtl,
				int				part,
				int				count,
				const LayoutPosition *		baseLine,
				int				textAttrNr,
				const TextAttribute *		ta,
				const char *			printString,
				int				nbLen );

extern int docSvgDrawTextLine(	struct BufferItem *		paraBi,
				int				line,
				const ParagraphFrame *		pf,
				const DocumentRectangle *	drLine,
				void *				vps,
				DrawingContext *		dc,
				const BlockOrigin *		bo );

extern int docSvgSaveShapeObject( SimpleOutputStream *	sos,
				int			page,
				int			pixelsWide,
				int			pixelsHigh,
				const InsertedObject *	io,
				struct BufferItem *	bodySectNode,
				const LayoutContext *	lc );

extern int docSvgDrawShapeText(	const DocumentRectangle *	drHere,
				const struct BufferItem *	bodySectNode,
				int				page,
				int				column,
				struct DrawingShape *		ds,
				DrawingContext *		dc,
				void *				vsw );

extern int docSvgDrawShapeImage( SvgWriter *			sw,
				DrawingContext *		dc,
				struct DrawingShape *		ds,
				const DocumentRectangle *	drTwips,
				const AffineTransform2D *	at );

