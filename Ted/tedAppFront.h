#   ifndef	TED_APP_FRONT_H
#   define	TED_APP_FRONT_H

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

extern void tedAppReplace(	void *		voidea,
				const char *	word	);

extern void tedAppAddRowToTable(	EditApplication *	ea,
					int			after );

extern void tedAppAddColumnToTable(	EditApplication *	ea,
					int			after );

extern void tedAppSetTableProperties(	EditApplication *	ea,
					int			wholeRow,
					int			wholeColumn,
					const PropertyMask *	cpSetMask,
					const CellProperties *	cpSet,
					const PropertyMask *	rpSetMask,
					const RowProperties *	rpSet );

extern void tedAppSetImageProperties(
				EditApplication *		ea,
				const PropertyMask *		pipSetMask,
				const PictureProperties *	pip );

extern int tedAppShiftRowsInTable(	EditApplication *	ea,
					int			direction );

extern int tedAppSetHyperlink(	EditApplication *	ea,
				const HyperlinkField *	hf );

extern int tedAppRemoveHyperlink(	EditApplication *	ea );

extern int tedAppFollowLink(	APP_WIDGET		option,
				EditApplication *	ea,
				const MemoryBuffer *	fileName,
				const MemoryBuffer *	markName );

extern int tedAppDeleteSelectedParagraphs(	EditApplication *	ea );
extern int tedAppDeleteSelectedSections(	EditApplication *	ea );

extern int tedAppInsertParagraph(	EditApplication *	ea,
					int			after );

extern int tedAppInsertSection(	EditApplication *	ea,
				int			after );

extern int tedAppSetDocumentProperties(
				EditApplication *		ea,
				const PropertyMask *		dpSetMask,
				const DocumentProperties *	dpSet );

extern void tedAppChangeCurrentNote(
				EditApplication *		ea,
				const PropertyMask *		npSetMask,
				const NoteProperties *		npSet );

extern int tedAppChangeSectionProperties(
				EditApplication *		ea,
				const PropertyMask *		spUpdMask,
				const SectionProperties *	spNew );

extern int tedAppChangeAllSectionProperties(
				EditApplication *		ea,
				const PropertyMask *		spUpdMask,
				const SectionProperties *	spNew,
				const PropertyMask *		dpSetMask,
				const DocumentProperties *	dpNew );

extern int tedAppChangeParagraphProperties(
			EditApplication *			ea,
			const PropertyMask *			ppUpdMask,
			const struct ParagraphProperties *	ppNew );

extern int tedAppSetParagraphTabs(
				EditApplication *		ea,
				const struct TabStopList *	tsl );

extern int tedAppSetNewList(	EditApplication *		ea );

extern int tedAppChangeCurrentList(
				EditApplication *		ea,
				const struct DocumentList *	dlNew );

extern void tedAppDeleteHeaderFooter(	EditApplication *	ea,
					int			treeType );

extern void tedAppEditHeaderFooter(	EditApplication *	ea,
					APP_WIDGET		relative,
					APP_WIDGET		option,
					int			treeType );

extern void tedAppGotoNoteDef(		EditApplication *	ea );
extern void tedAppGotoNoteRef(		EditApplication *	ea );
extern void tedAppNextNote(		EditApplication *	ea );
extern void tedAppPrevNote(		EditApplication *	ea );

extern void tedAppSelectTable(		EditApplication *	ea );
extern void tedAppSelectRow(		EditApplication *	ea,
					int			direction,
					int			allColumns );
extern void tedAppSelectWholeCell(	EditApplication *	ea,
					int			direction,
					int			allRows );

extern void tedAppDeleteTable(		EditApplication *	ea );
extern void tedAppDeleteRow(		EditApplication *	ea );
extern void tedAppDeleteColumn(		EditApplication *	ea );

extern int tedAppSelectCurrentFrame(	EditApplication *	ea );
extern int tedAppDeleteCurrentFrame(	EditApplication *	ea );

extern int tedAppSetBookmark(	EditApplication *	ea,
				const MemoryBuffer *	markName );

extern int tedAppGoToBookmark(	EditApplication *	ea,
				const MemoryBuffer *	markName );

extern int tedAppRemoveBookmark(EditApplication *	ea );

extern int tedAppFindBookmarkField(
				DocumentField **		pDf,
				EditApplication *		ea,
				const MemoryBuffer *		markName );

extern void tedAppSetTocField(	EditApplication *		ea,
				const TocField *		tf );

extern void tedAppDeleteTocField( EditApplication *		ea );

extern void tedAppAddTocField(	EditApplication *		ea,
				const TocField *		tf );

extern int tedAppFindNext(		void *		voidea );

extern int tedAppFindPrev(		void *		voidea );

extern int tedAppListFontToolSet(
				void *				voidea,
				const PropertyMask *		taSetMask,
				const ExpandedTextAttribute *	etaSet );

extern int tedAppFontToolSet(	void *				voidea,
				const PropertyMask *		taSetMask,
				const ExpandedTextAttribute *	etaSet );

extern void tedAppSetParaOutlineLevel(
				EditApplication *		ea,
				int				level );

extern int tedAppSelectWholeSection(	EditApplication *	ea,
					int			direction );

extern int tedAppSelectWholeParagraph(	EditApplication *	ea,
					int			direction );

extern void tedManual(			APP_WIDGET		option,
					EditApplication *	ea,
					APP_WIDGET		relative );

extern void tedVisibleDocumentCountChanged(
					EditApplication *	ea,
					int			from,
					int			to );

#   endif	/*  TED_APP_FRONT_H	*/
