/************************************************************************/
/*									*/
/*  Ted: data structures and routines for on screen interactive		*/
/*  editing.								*/
/*									*/
/************************************************************************/

#   ifndef		TED_LAYOUT_H
#   define		TED_LAYOUT_H

#   include	<docLayout.h>
#   include	<docSelectionDescription.h>
#   include	<drawDrawingSurface.h>

struct EditDocument;
struct TedDocument;

/************************************************************************/
/*									*/
/*  Routine Declarations:						*/
/*									*/
/************************************************************************/

extern void tedSetScreenLayoutContext(	LayoutContext *		lc,
					struct EditDocument *	ed );

extern void tedSetDocumentLayoutContext( LayoutContext *	lc,
				DrawingSurface			ds,
				const PostScriptFontList *	psfl,
				struct TedDocument *		td );

extern int tedFindPosition(	DocumentPosition *		dp,
				PositionGeometry *		pg,
				int *				pPage,
				int *				pColumn,
				struct BufferItem *		selRootNode,
				const LayoutContext *		lc,
				int				docXPixels,
				int				docY );

extern int tedFindPositionInLine(
				DocumentPosition *		dp,
				PositionGeometry *		pg,
				const LayoutContext *		lc,
				const struct BufferItem *	paraNode,
				int				line,
				int				docXPixels );

extern void tedPositionGeometry(PositionGeometry *		pg,
				const DocumentPosition *	dp,
				int				lastOne,
				const LayoutContext *		lc );

extern void tedSelectionGeometry(
				SelectionGeometry *		sg,
				const DocumentSelection *	ds,
				const struct BufferItem *	bodySectNode,
				int				lastLine,
				const LayoutContext *		lc );

extern int tedArrowDown(	DocumentPosition *		bp,
				const PositionGeometry *	pg,
				const LayoutContext *		lc );

extern int tedArrowUp(		DocumentPosition *		bp,
				const PositionGeometry *	pg,
				const LayoutContext *		lc );

extern int tedLayoutDocumentBody(	int *			pReachedBottom,
					const LayoutContext *	lc );

extern void tedGetObjectRectangle(
				DocumentRectangle *		drObject,
				Point2DI *			xp,
				const InsertedObject *		io,
				const PositionGeometry *	pg,
				const LayoutContext *		lc,
				const struct EditDocument *	ed );

extern void tedSetObjectWindows(struct EditDocument *		ed,
				const PositionGeometry *	pg,
				const InsertedObject *		io,
				const LayoutContext *		lc );

extern int tedOpenNodeObjects(	struct BufferItem *	bi,
				const LayoutContext *	lc );

extern int tedOpenTreeObjects(	DocumentTree *		ei,
					const LayoutContext *	lc );

extern int tedReopenObject(	struct TextParticule *		tp,
				const LayoutContext *		lc );

extern InsertedObject * tedObjectMakeRasterObject(
					int *			pObjectNumber,
				    	struct EditDocument *	ed,
					struct BufferItem *	bi,
					const LayoutContext *	lc,
					RasterImage *		ri );

extern int tedOpenObject(	InsertedObject *	io,
				const LayoutContext *	lc );

extern void tedDocAdaptTopRuler( struct EditDocument *		ed,
				const DocumentSelection *	ds,
				const SelectionGeometry *	sg,
				const SelectionDescription *	sd,
				const struct BufferItem *	bodySectNode );

extern int tedGetParaLineHeight(	int *			pLineHeight,
					struct EditDocument *	ed );

#   endif	/*	TED_LAYOUT_H	*/
