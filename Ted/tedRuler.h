/************************************************************************/
/*									*/
/*  Definitions relating to rulers.					*/
/*									*/
/************************************************************************/

#   include	<appFrame.h>
#   include	"tedColumnSeparator.h"

struct TabStopList;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int tedDocSetLeftRuler(		EditDocument *	ed );
extern int tedDocSetRightRuler(		EditDocument *	ed );
extern int tedDocSetTopRuler(		EditDocument *	ed );
extern int tedDocSetBottomRuler(	EditDocument *	ed );

extern void tedVerticalRulerSetBackground( void *		voidrd,
					const RGB8Color *	back );
extern void tedTopRulerSetBackground(	void *			voidttr,
					const RGB8Color *	back );
extern void tedBottomRulerSetBackground( void *			voidtbr,
					const RGB8Color *	back );

extern void * tedMakeTopRuler(
			int				height,
			double				magnifiedPixelsPerTwip,
			double				magnification,
			const PostScriptFontList *	psfl,

			int				leftRulerWidth,
			int				rightRulerWidth,

			int				docX0,
			int				docX1,
			int				marginC0,
			int				marginC1,
			int				docVisibleX0,
			int				docVisibleX1,

			int				whatUnit );

extern void * tedMakeBottomRuler(
			const PostScriptFontList *	psfl,
			int				height,
			int				leftRulerWidth,
			int				rightRulerWidth,
			const char *			pageFormat );

extern void tedFreeVerticalRuler(	void *		voidrd );
extern void tedFreeTopRuler(		void *		voidttr );
extern void tedFreeBottomRuler(		void *		voidtlr );

extern APP_EVENT_HANDLER_H( tedRedrawTopRuler, w, voidttr, event );
extern APP_EVENT_HANDLER_H( tedRedrawBottomRuler, w, voidtlr, event );

extern void tedScrollTopRuler(		void *			voidttr,
					APP_WIDGET		w,
					int			d );

extern void tedSetTopRulerRange(	void *			voidttr,
					APP_WIDGET		w,
					int			docFullX1,
					int			docVisX0,
					int			docVisX1 );

extern void tedAdaptTopRuler(		void *			voidttr,
					APP_WIDGET		w,
					int			docX0,
					int			docX1,
					int			marginC0,
					int			marginC1,
					int			firstIndent,
					int			leftIndent,
					int			rightIndent,
					const struct TabStopList *	tslSet,
					int			x0GeometryPx,
					double			xfac );

extern int tedTopRulerFindMouse(	int *			pIndex,
					APP_WIDGET		w,
					int			mouseX,
					int			mouseY,
					EditDocument *		ed );

extern int tedTopRulerTrackMouse( 	int *			pFirstIndent,
					int *			pLeftIndent,
					int *			pRightIndent,
					struct TabStopList *	tsl,
					int *			pCsCount,
					ColumnSeparator **	pCs,
					APP_WIDGET		w,
					APP_EVENT *		downEvent,
					EditDocument *		ed );

extern APP_EVENT_HANDLER_H( tedTopRulerConfigure, w, voidttr, event );
extern APP_EVENT_HANDLER_H( tedBottomRulerConfigure, w, voidtlr, event );

extern int tedSetRulerColumns(	APP_WIDGET		w,
				void *			voidttr,
				int			bfX0Pixels,
				int			bfX1Pixels,
				int			pfX0Pixels,
				int			pfX1Pixels,
				ColumnSeparator *	cs,
				int			csCount );

extern void tedSetBottomRulerValues(
				void *			voidtbr,
				APP_WIDGET		w,
				int			page,
				int			pages,
				int			section,
				int			sections );

