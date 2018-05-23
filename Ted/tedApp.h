#   ifndef	TED_APP_H
#   define	TED_APP_H

#   include	<appFrame.h>

#   include	<utilPropMask.h>
#   include	<utilMemoryBuffer.h>

#   include	<geo2DInteger.h>
#   include	<docBuf.h>
#   include	<utilIndexMapping.h>

/************************************************************************/

struct TedDocument;
struct DocumentStatistics;

/************************************************************************/
/*									*/
/*  Shared resources.							*/
/*									*/
/************************************************************************/

extern const int TedClipboardTextTargetCount;
extern AppSelectionTargetType TedClipboardTextTargets[];

extern const int TedPrimaryTargetCount;
extern AppSelectionTargetType TedPrimaryTargets[];

extern const int TedRulerTargetCount;
extern AppSelectionTargetType TedRulerTargets[];

extern const int TedFontTargetCount;
extern AppSelectionTargetType TedFontTargets[];

/************************************************************************/
/*									*/
/*  Routine Declarations.						*/
/*									*/
/************************************************************************/

extern void tedDocCopy(		EditDocument *	ed );
extern void tedDocCut(		EditDocument *	ed );

extern void tedDocSelAll(	EditDocument *	ed );


extern void tedAdaptFontIndicatorsToSelection(	EditDocument *		ed );

extern void tedAdaptToolsToSelection(	EditDocument *		ed );

extern int tedExtendSelectionToPosition(
				EditDocument *			ed,
				const DocumentPosition *	dpAnchor,
				const DocumentPosition *	dpFound );

extern void tedScrollToSelection(	EditDocument *		ed,
					int *			pScrolledX,
					int *			pScrolledY );

extern void tedAdaptPageToolToDocument(	EditApplication *	ea,
					EditDocument *		ed );

extern void tedGetNamedPictures(	EditApplication *	ea );

extern void tedUndrawIBar(	const EditDocument *	ed );

extern void tedStartCursorBlink(	EditDocument *	ed );
extern void tedStopCursorBlink(		EditDocument *	ed );
extern void tedCleanCursorBlink(	struct TedDocument *	td );

extern void tedMoveObjectWindows(	EditDocument *		ed );

extern int tedSaveObjectPicture(	RasterImage *		abiTo,
					const InsertedObject *	io );

extern void tedDrawRectangle(	EditDocument *		ed,
				DocumentRectangle *	drClip,
				int			ox,
				int			oy );

extern int tedFinishDocumentSetup(	EditDocument *		ed );
extern void tedDocumentFirstVisible(	EditDocument *		ed );

extern int tedLayoutDocument(	DocumentRectangle *		drScreen,
				DocumentRectangle *		drVisible,
				void *				privateData,
				int				format,
				DrawingSurface			ds,
				const PostScriptFontList *	psfl,
				const DocumentGeometry *	defDg );

extern int tedMakeDocumentWidget(	EditApplication *	ea,
					EditDocument *		ed );

extern void tedAdaptFormatToolToDocument(	EditDocument *	ed,
						int		choosePage );

extern int tedRunPropertyDialog( EditDocument *			ed,
				const struct DocumentStatistics *	ds,
				APP_WIDGET			option,
				const char *			pixmapName );

extern int tedSaveDocument(	EditApplication *		ea,
				DrawingSurface			ds,
				const void *			privateData,
				int				format,
				const MemoryBuffer *		documentTitle,
				int				suggestStdout,
				const MemoryBuffer *		filename,
				int				isDocName );

extern void * tedMakePrivateData( void );

extern int tedNewDocument(	EditDocument *		ed,
				const MemoryBuffer *	filename );

extern int tedPrintDocument(	SimpleOutputStream *		sos,
				const struct PrintJob *		pj,
				const struct PrintGeometry *	pg );

extern void tedSuggestPageSetup( struct PrintGeometry *		pg,
				void *				privateData,
				int				sheetSize );

extern void tedFreeDocument(		void *			voidtd,
					int			format );

extern void tedRedoDocumentLayout(	EditDocument *		ed );

extern void tedFormatSetFindPattern(
				EditApplication *		ea,
				const char *			findPattern,
				int				findRegexp );

extern void tedHideObjectWindows(	EditDocument *		ed );

extern int tedSetIBarSelection(		EditDocument *		ed,
					const DocumentPosition *	dp,
					int			lastLine,
					int *			pScrolledX,
					int *			pScrolledY );

extern int tedOpenDocumentFile(	unsigned char *		digest,
				int *			pFormat,
				BufferDocument **	pBd,
				EditApplication *	ea,
				int			suggestStdin,
				int			formatHint,
				const MemoryBuffer *	filename,
				int			complain,
				APP_WIDGET		relative,
				APP_WIDGET		option );

extern int tedObjectDrag(	APP_WIDGET			w,
				EditDocument *			ed,
				APP_EVENT *			downEvent );

extern int tedOpenDocument(	EditApplication *		ea,
				void *				voidtd,
				int *				pFormat,
				APP_WIDGET			relative,
				APP_WIDGET			option,
				int				readOnly,
				int				suggestStdin,
				int				formatHint,
				const MemoryBuffer *		filename );

extern void tedManagePrimarySelection(	EditDocument *			ed );

extern void tedToggleTextAttribute(	EditDocument *			ed,
					int				set,
					int				prop );

extern void tedSetFontSupersub(		EditDocument *		ed,
					int			supersub );

extern int tedDocFindNext(		EditDocument *		ed );
extern int tedDocFindPrev(		EditDocument *		ed );

extern void tedShowFindTool(		EditDocument *		ed );

extern void tedObserveFocus(		EditDocument *		ed,
					int			inout );

extern int tedMakeDocumentCursor(	EditApplication *	ea );

extern int tedDocUnlock(	EditDocument *		ed );
extern int tedDocRecover(	EditDocument *		ed );

extern void tedMakeDocumentReadonly(	EditDocument *	ed,
					int		readonly );

extern void tedSetTracedChangedFlag(	EditDocument *	ed );

#   endif	/*  TED_APP_H	*/
