/************************************************************************/
/*									*/
/*  PostScript printing.						*/
/*									*/
/************************************************************************/

#   include	<sioGeneral.h>
#   include	<psPrint.h>
#   include	<docBuf.h>
#   include	"docDraw.h"
#   include	"docParticuleData.h"

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int docPsPrintGetDocumentFonts(
				PostScriptTypeList *		pstl,
				const LayoutContext *		lc );

extern int docPsListImageFonts( PostScriptTypeList *		pstl,
				const PictureProperties *	pip,
				const MemoryBuffer *		mb,
				const LayoutContext *		lc,
				const char *			prefix );

extern int docPsPrintTab(	const DrawTextLine *		dtl,
				int				part,
				int				textAttrNr,
				const TextAttribute *		ta,
				int				leader,
				int				x0Twips,
				int				x1Twips,
				const LayoutPosition *		baseLine );

extern void docPsSaveTabLeaderProcedures(	SimpleOutputStream *	sos );

extern int docPsPrintDrawDrawingShape(
				const DocumentRectangle *	drTwips,
				int				page,
				struct DrawingShape *		ds,
				DrawingContext *		dc,
				void *				vps );

extern int docPsPrintObject(	const DrawTextLine *		dtl,
				int				part,
				InsertedObject *		io,
				int				x0Twips,
				int				x1Twips,
				const LayoutPosition *		baseLine );

extern int docPsPrintShapeImage( PrintingState *		ps,
				DrawingContext *		dc,
				struct DrawingShape *		ds,
				const DocumentRectangle *	drTwips,
				const AffineTransform2D *	at );

extern int docPsPrintTextLine(	struct BufferItem *		paraBi,
				int				line,
				const ParagraphFrame *		pf,
				const DocumentRectangle *	drLine,
				void *				vps,
				DrawingContext *		dc,
				const BlockOrigin *		bo );

extern int docPsPrintStartField(	const DrawTextLine *	dtl,
					int			part,
					const DocumentField *	df );

extern int docPsPrintFinishField(	const DrawTextLine *	dtl,
					int			part,
					const DocumentField *	df );

extern int docPsPrintFtnsep(	const DrawTextLine *		dtl,
				int				part,
				int				textAttrNr,
				const TextAttribute *		ta,
				int				x0Twips,
				int				x1Twips,
				const LayoutPosition *		baseLine );

extern int docPsPrintSpan(	const DrawTextLine *		dtl,
				int				part,
				int				count,
				const LayoutPosition *		baseLine,
				int				textAttrNr,
				const TextAttribute *		ta,
				const char *			printString,
				int				nbLen );

extern int docPsPrintRunUnderline(	const DrawTextLine *	dtl,
					int			part,
					int			upto,
					int			textAttrNr,
					const TextAttribute *	ta,
					int			x0Twips,
					int			x1Twips,
					const LayoutPosition *	baseLine );

extern int docPsPrintRunStrikethrough(	const DrawTextLine *	dtl,
					int			part,
					int			upto,
					int			textAttrNr,
					const TextAttribute *	ta,
					int			x0Twips,
					int			x1Twips,
					const LayoutPosition *	baseLine );

extern void psDefineBorderProcs(	SimpleOutputStream *	sos );

extern int docPsPrintOrnaments(	const BlockOrnaments *		bo,
				int				page,
				const DocumentRectangle *	drOutside,
				const DocumentRectangle *	drInside,
				void *				through,
				struct DrawingContext *		dc );

extern int docPsDocinfoPdfmark( PrintingState *		ps,
			const char *			applicationName,
			const char *			applicationReference,
			const DocumentProperties *	dp );

extern int docPsOutlinePdfmarks(	PrintingState *		ps,
					BufferDocument *	bd );

