#   ifndef	TED_DOC_FRONT_H
#   define	TED_DOC_FRONT_H

#   include	<appFrame.h>
#   include	<docRowProperties.h>
#   include	<docPictureProperties.h>
#   include	<docDocumentProperties.h>
#   include	<docNoteProperties.h>
#   include	<docSectProperties.h>
#   include	<docDocumentField.h>
#   include	<docTocField.h>
#   include	<docHyperlinkField.h>
#   include	<docExpandedTextAttribute.h>

struct ParagraphProperties;
struct TabStopList;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int tedDocReplace(		EditDocument *		ed,
					const char *		word );

extern int tedDocAddRowToTable(		EditDocument *		ed,
					int			after,
					int			traced );

extern int tedDocAddColumnToTable(	EditDocument *		ed,
					int			after,
					int			traced );

extern int tedDocSetTableProperties(	EditDocument *		ed,
					int			wholeRow,
					int			wholeColumn,
					const PropertyMask *	cpSetMask,
					const CellProperties *	cpSet,
					const PropertyMask *	rpSetMask,
					const RowProperties *	rpSet,
					int			traced );

extern int tedDocSetImageProperties(
				EditDocument *			ed,
				const PropertyMask *		pipSetMask,
				const PictureProperties *	pip,
				int				traced );

extern int tedDocSetHyperlink(	EditDocument *		ed,
				const HyperlinkField *	hf,
				int			traced );

extern int tedDocRemoveHyperlink(	EditDocument *		ed,
					int			traced );

extern int tedDocFollowLink(	APP_WIDGET		option,
				EditDocument *		ed,
				const MemoryBuffer *	fileName,
				const MemoryBuffer *	markName );

extern int tedDocDeleteSelectedParagraphs(
				EditDocument *		ed,
				int			traced );

extern int tedDocDeleteSelectedSections(
				EditDocument *		ed,
				int			traced );

extern int tedDocInsertParagraph(	EditDocument *		ed,
					int			after,
					int			traced );

extern int tedDocInsertSection(	EditDocument *		ed,
				int			after,
				int			traced );

extern int tedDocSetDocumentProperties(
				EditDocument *			ed,
				const PropertyMask *		dpSetMask,
				const DocumentProperties *	dpSet,
				int				traced );

extern int tedDocChangeCurrentNote(
				EditDocument *			ed,
				const PropertyMask *		npSetMask,
				const NoteProperties *		npSet,
				int				traced );

extern int tedDocChangeSectionProperties(
				EditDocument *			ed,
				const PropertyMask *		spUpdMask,
				const SectionProperties *	spNew,
				int				traced );

extern int tedDocChangeAllSectionProperties(
				EditDocument *			ed,
				const PropertyMask *		spUpdMask,
				const SectionProperties *	spNew,
				const PropertyMask *		dpSetMask,
				const DocumentProperties *	dpNew,
				int				traced );

extern int tedDocChangeParagraphProperties(
				EditDocument *			ed,
				const PropertyMask *		ppUpdMask,
				const struct ParagraphProperties *	ppNew,
				int				traced );

extern int tedDocSetParagraphTabs(
				EditDocument *			ed,
				const struct TabStopList *	tsl,
				int				traced );

extern int tedDocSetNewList(	EditDocument *			ed,
				int				traced );

extern int tedDocChangeCurrentList(
				EditDocument *			ed,
				const struct DocumentList *	dlNew,
				int				traced );

extern int tedDocDeleteHeaderFooter(	EditDocument *		ed,
					int			treeType,
					int			traced );

extern int tedDocInsertHeaderFooter(	EditDocument *		ed,
					int			treeType,
					int			traced );

extern void tedDocEditHeaderFooter(	EditDocument *		ed,
					APP_WIDGET		relative,
					APP_WIDGET		option,
					int			treeType );

extern int tedDocInsertNote(		EditDocument *		ed,
					int			noteTreeType,
					int			traced );

extern void tedDocGotoNoteDef(		EditDocument *		ed );
extern void tedDocGotoNoteRef(		EditDocument *		ed );

extern void tedDocNextNote(		EditDocument *		ed );
extern void tedDocPrevNote(		EditDocument *		ed );

extern void tedDocSelectTable(		EditDocument *		ed );
extern void tedDocSelectRow(		EditDocument *		ed,
					int			direction,
					int			allColumns );
extern void tedDocSelectWholeCell(	EditDocument *		ed,
					int			direction,
					int			allRows );

extern int tedDocSelectColumn(		EditDocument *		ed,
					int			col );

extern int tedDocDeleteTable(		EditDocument *		ed,
					int			traced );
extern int tedDocDeleteRow(		EditDocument *		ed,
					int			traced );
extern int tedDocDeleteColumn(		EditDocument *		ed,
					int			traced );

extern int tedDocSelectCurrentFrame(	EditDocument *		ed );
extern int tedDocDeleteCurrentFrame(	EditDocument *		ed,
					int			traced );

extern int tedDocSetBookmark(	EditDocument *		ed,
				const MemoryBuffer *	markName,
				int			traced );

extern int tedDocGoToBookmark(	EditDocument *		ed,
				const MemoryBuffer *	markName );

extern int tedDocRemoveBookmark(EditDocument *		ed,
				int			traced );

extern int tedDocFindBookmarkField(
				DocumentField **		pDf,
				EditDocument *			ed,
				const MemoryBuffer *		markName );

extern int tedDocSetTocField(	EditDocument *			ed,
				const TocField *		tf,
				int				traced );

extern void tedDocDeleteTocField( EditDocument *		ed,
				int				traced );

extern void tedDocAddTocField(	EditDocument *			ed,
				const TocField *		tf,
				int				traced );

extern void tedDocChangeTextAttribute(	EditDocument *		ed,
					const PropertyMask *	taSetMask,
					const TextAttribute *	taSet,
					int			traced );

extern int tedDocShiftRowsInTable(	EditDocument *		ed,
					int			direction );

extern int tedDocListFontToolSet( EditDocument *		ed,
				const PropertyMask *		taSetMask,
				const ExpandedTextAttribute *	etaSet );

extern int tedDocFontToolSet(	EditDocument *			ed,
				const PropertyMask *		taSetMask,
				const ExpandedTextAttribute *	etaSet );

extern void tedDocFormatSetParaAlignment(
				EditDocument *			ed,
				int				align,
				int				traced );

extern void tedDocSetParaOutlineLevel(
				EditDocument *			ed,
				int				level,
				int				traced );

extern void tedDocSetPageLayout( EditDocument *			ed,
				const PropertyMask *		dgSetMask,
				const DocumentGeometry *	dgSet,
				int				wholeDocument,
				int				traced );

extern int tedFormatShiftIndent( EditDocument *			ed,
				int				step,
				int				traced );

extern int tedObjectOpenImage(	EditApplication *		ea,
				void *				voided,
				APP_WIDGET			relative,
				APP_WIDGET			option,
				const MemoryBuffer *		filename );

APP_SCROLLBAR_CALLBACK_H( tedDocHorizontalScrollbarCallback, bar, voided, e );
APP_SCROLLBAR_CALLBACK_H( tedDocVerticalScrollbarCallback, bar, voided, e );

extern APP_MENU_CALLBACK_H( tedDocTableSelectTable, w, voided, event );
extern APP_MENU_CALLBACK_H( tedDocTableSelectRow, w, voided, event );
extern APP_MENU_CALLBACK_H( tedDocTableSelectColumn, w, voided, event );

extern APP_MENU_CALLBACK_H( tedDocTableDeleteTable, w, voided, event );
extern APP_MENU_CALLBACK_H( tedDocTableDeleteRow, w, voided, event );
extern APP_MENU_CALLBACK_H( tedDocTableDeleteColumn, w, voided, event );

extern void tedDocUndo(		EditDocument *			ed );
extern int tedDocRepeat(	EditDocument *			ed );

extern APP_MENU_CALLBACK_H( tedDocToolFind, w, voided, event );
extern APP_MENU_CALLBACK_H( tedDocToolFindNext, w, voided, event );
extern APP_MENU_CALLBACK_H( tedDocToolSpell, w, voided, event );

extern APP_MENU_CALLBACK_H( tedDocEditPaste, w, voided, event );
extern APP_MENU_CALLBACK_H( tedDocFontCopy, w, voided, event );
extern APP_MENU_CALLBACK_H( tedDocEditUndo, w, voided, event );
extern APP_MENU_CALLBACK_H( tedDocEditRepeat, w, voided, event );

extern APP_MENU_CALLBACK_H( tedDocFontBold, w, voided, event );
extern APP_MENU_CALLBACK_H( tedDocFontItalic, w, voided, event );
extern APP_MENU_CALLBACK_H( tedDocFontUnderlined, w, voided, event );
extern APP_MENU_CALLBACK_H( tedDocFontSupersub, w, voided, event );

extern APP_MENU_CALLBACK_H( tedDocInsertImage, w, voided, event );
extern APP_MENU_CALLBACK_H( tedDocInsertFile, w, voided, event );
extern APP_MENU_CALLBACK_H( tedDocInsertLink, w, voided, event );
extern APP_MENU_CALLBACK_H( tedDocInsertBookmark, w, voided, event );
extern APP_MENU_CALLBACK_H( tedDocInsertFootnote, w, voided, event );
extern APP_MENU_CALLBACK_H( tedDocInsertEndnote, w, voided, event );
extern APP_MENU_CALLBACK_H( tedDocInsertChftnsep, w, voided, event );
extern APP_MENU_CALLBACK_H( tedDocInsertPageNumber, w, voided, event );
extern APP_MENU_CALLBACK_H( tedDocInsertLineBreak, w, voided, event );
extern APP_MENU_CALLBACK_H( tedDocInsertPageBreak, w, voided, event );
extern APP_MENU_CALLBACK_H( tedDocInsertColumnBreak, w, voided, event );
extern APP_MENU_CALLBACK_H( tedDocMenuInsertSectBreak, w, voided, event );
extern APP_MENU_CALLBACK_H( tedDocToolInsertSymbol, w, voided, event );
extern APP_MENU_CALLBACK_H( tedDocFormatOnePara, w, voided, event );

extern APP_MENU_CALLBACK_H( tedDocFormatCopyRul, w, voided, event );
extern APP_MENU_CALLBACK_H( tedDocFormatPasteRul, w, voided, event );

extern APP_MENU_CALLBACK_H( tedDocFormatIncreaseIndent, w, voided, event );
extern APP_MENU_CALLBACK_H( tedDocFormatDecreaseIndent, w, voided, event );
extern APP_MENU_CALLBACK_H( tedDocFormatAlignLeft, w, voided, event );
extern APP_MENU_CALLBACK_H( tedDocFormatAlignRight, w, voided, event );
extern APP_MENU_CALLBACK_H( tedDocFormatAlignCenter, w, voided, event );
extern APP_MENU_CALLBACK_H( tedDocFormatAlignJustify, w, voided, event );

extern APP_MENU_CALLBACK_H( tedDocTableDrawGrid, w, voided, event );

extern APP_MENU_CALLBACK_H( tedDocFilePrint, printOption, voided, e );
extern APP_MENU_CALLBACK_H( tedDocTableInsertTable, option, voided, e );
extern APP_MENU_CALLBACK_H( tedDocTableAddRow, option, voided, e );
extern APP_MENU_CALLBACK_H( tedDocTableAddColumn, option, voided, e );
extern APP_MENU_CALLBACK_H( tedDocFormatTool, option, voided, e );
extern APP_MENU_CALLBACK_H( tedDocShowPageTool, pageOption, voided, e );
extern APP_MENU_CALLBACK_H( tedDocFontPaste, option, voided, e );
extern APP_MENU_CALLBACK_H( tedDocToolFont, option, voided, e );
extern APP_MENU_CALLBACK_H( tedDocFileProps, option, voided, e );
extern APP_MENU_CALLBACK_H( tedDocFileUnlock, option, voided, e );
extern APP_MENU_CALLBACK_H( tedDocFileRecover, option, voided, e );

extern APP_EVENT_HANDLER_H( tedDocObserveFocus, w, voided, event );

extern APP_PASTE_REPLY( tedPasteClipboardRtf, w, event, voided );
extern APP_PASTE_REPLY( tedPasteClipboardString, w, event, voided );
extern APP_PASTE_REPLY( tedPastePrimaryString, w, event, voided );
extern APP_PASTE_REPLY( tedPasteRulerTed, w, event, voided );
extern APP_PASTE_REPLY( tedPasteFontTed, w, event, voided );
extern APP_PASTE_REPLY( tedPasteClipboardPng, w, event, voided );
extern APP_PASTE_REPLY( tedPasteClipboardJfif, w, event, voided );

extern APP_GIVE_COPY( tedCopyClipboardRtf, w, event, voided );
extern APP_GIVE_COPY( tedCopyClipboardString, w, event, voided );
extern APP_GIVE_COPY( tedCopyPrimaryString, w, event, voided );
extern APP_GIVE_COPY( tedCopyClipboardPng, w, event, voided );
extern APP_GIVE_COPY( tedCopyClipboardJfif, w, event, voided );
extern APP_GIVE_COPY( tedCopyFontTed, w, event, voided );
extern APP_GIVE_COPY( tedCopyRulerTed, w, event, voided );

extern void tedClipboardLost(	APP_WIDGET			w,
				void *				voided,
				APP_EVENT *			clrEvent );

extern void tedPrimaryLost(	APP_WIDGET			w,
				void *				voided,
				APP_EVENT *			clrEvent );

extern APP_EVENT_HANDLER_H( tedMouseButtonPressed, w, voided, downEvent );
extern APP_EVENT_HANDLER_H( tedScrollEventHandler, w, voided, scrollEvent );

extern void tedDocGotKey(	void *				voided,
				int				keySym,
				unsigned int			state );

extern void tedDocGotString(	void *				voided,
				const char *			string,
				int				length );

extern void tedDocSplitParagraph( struct EditDocument *		ed,
				int				onNewPage,
				int				traced );

extern int tedDocInsertSectBreak( struct EditDocument *		ed,
				int				traced );

#   endif	/*  TED_DOC_FRONT_H	*/
