/************************************************************************/
/*									*/
/*  Ted: application level API for menu and tools.			*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<ctype.h>
#   include	<sioGeneral.h>

#   include	"tedAppFront.h"
#   include	"tedDocFront.h"
#   include	"tedDocument.h"

#   include	"tedApp.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Handle a replace from one of the tools.				*/
/*  (Used from the Find and Spell tools)				*/
/*									*/
/************************************************************************/

void tedAppReplace(	void *		voidea,
			const char *	word )
    {
    EditApplication *	ea= (EditApplication *)voidea;
    EditDocument *	ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return;	}

    (void) tedDocReplace( ed, word );

    return;
    }

int tedAppSelectCurrentFrame(	EditApplication *	ea )
    {
    EditDocument *		ed= ea->eaCurrentDocument;
    if  ( ! ed )
	{ XDEB(ed); return -1;	}

    return tedDocSelectCurrentFrame( ed );
    }

void tedAppSelectWholeCell(	EditApplication *	ea,
				int			direction,
				int			allRows )
    {
    EditDocument *		ed= ea->eaCurrentDocument;
    if  ( ! ed )
	{ XDEB(ed); return;	}

    tedDocSelectWholeCell( ed, direction, allRows );
    }

void tedAppSelectRow(		EditApplication *	ea,
				int			direction,
				int			allColumns )
    {
    EditDocument *		ed= ea->eaCurrentDocument;
    if  ( ! ed )
	{ XDEB(ed); return;	}

    tedDocSelectRow( ed, direction, allColumns );
    }

void tedAppSelectTable(		EditApplication *	ea )
    {
    EditDocument *		ed= ea->eaCurrentDocument;
    if  ( ! ed )
	{ XDEB(ed); return;	}

    tedDocSelectTable( ed );
    }

void tedAppGotoNoteDef(		EditApplication *	ea )
    {
    EditDocument *		ed= ea->eaCurrentDocument;
    if  ( ! ed )
	{ XDEB(ed); return;	}

    tedDocGotoNoteDef( ed );
    }

int tedAppDeleteCurrentFrame(	EditApplication *	ea )
    {
    EditDocument *		ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return -1;	}

    return tedDocDeleteCurrentFrame( ed,
			((TedDocument *)ed->edPrivateData)->tdTraced );
    }

int tedAppDeleteSelectedParagraphs(	EditApplication *	ea )
    {
    EditDocument *		ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return -1;	}

    return tedDocDeleteSelectedParagraphs( ed,
			((TedDocument *)ed->edPrivateData)->tdTraced );
    }

int tedAppDeleteSelectedSections(	EditApplication *	ea )
    {
    EditDocument *		ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return -1;	}

    return tedDocDeleteSelectedSections( ed,
			((TedDocument *)ed->edPrivateData)->tdTraced );
    }

int tedAppInsertParagraph(	EditApplication *	ea,
				int			after )
    {
    EditDocument *		ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return -1;	}

    return tedDocInsertParagraph( ed, after,
			((TedDocument *)ed->edPrivateData)->tdTraced );
    }

int tedAppInsertSection(	EditApplication *	ea,
				int			after )
    {
    EditDocument *		ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return -1;	}

    return tedDocInsertSection( ed, after,
			((TedDocument *)ed->edPrivateData)->tdTraced );
    }

void tedAppSetTocField(		EditApplication *	ea,
				const TocField *	tf )
    {
    EditDocument *	ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed);	}

    tedDocSetTocField( ed, tf, ((TedDocument *)ed->edPrivateData)->tdTraced );
    }

void tedAppAddTocField(		EditApplication *	ea,
				const TocField *	tf )
    {
    EditDocument *	ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed);	}

    tedDocAddTocField( ed, tf, ((TedDocument *)ed->edPrivateData)->tdTraced );
    }

void tedAppDeleteTocField(	EditApplication *	ea )
    {
    EditDocument *	ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return;	}

    tedDocDeleteTocField( ed, ((TedDocument *)ed->edPrivateData)->tdTraced );
    }

int tedAppSetHyperlink(	EditApplication *	ea,
			const HyperlinkField *	hf )
    {
    EditDocument *	ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return -1;	}

    return tedDocSetHyperlink( ed, hf,
			((TedDocument *)ed->edPrivateData)->tdTraced );
    }

int tedAppRemoveHyperlink(	EditApplication *	ea )
    {
    EditDocument *	ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return -1;	}

    return tedDocRemoveHyperlink( ed,
			((TedDocument *)ed->edPrivateData)->tdTraced );
    }

int tedAppFindBookmarkField(	DocumentField **		pDf,
				EditApplication *		ea,
				const MemoryBuffer *		markName )
    {
    EditDocument *	ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return -1;	}

    return tedDocFindBookmarkField( pDf, ed, markName );
    }

int tedAppSetBookmark(	EditApplication *	ea,
			const MemoryBuffer *	markName )
    {
    EditDocument *	ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return -1;	}

    return tedDocSetBookmark( ed, markName,
			((TedDocument *)ed->edPrivateData)->tdTraced );
    }

int tedAppRemoveBookmark(	EditApplication *	ea )
    {
    EditDocument *	ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return -1;	}

    return tedDocRemoveBookmark( ed,
			((TedDocument *)ed->edPrivateData)->tdTraced );
    }

int tedAppGoToBookmark(	EditApplication *	ea,
			const MemoryBuffer *	markName )
    {
    EditDocument *	ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return -1;	}

    return tedDocGoToBookmark( ed, markName );
    }

void tedAppSetParaOutlineLevel(	EditApplication *	ea,
				int			level )
    {
    EditDocument *	ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return;	}

    tedDocSetParaOutlineLevel( ed, level,
			((TedDocument *)ed->edPrivateData)->tdTraced );

    return;
    }

void tedAppDeleteTable(	EditApplication *	ea )
    {
    EditDocument *		ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return;	}

    tedDocDeleteTable( ed, ((TedDocument *)ed->edPrivateData)->tdTraced );
    }

void tedAppDeleteRow(	EditApplication *	ea )
    {
    EditDocument *		ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return;	}

    tedDocDeleteRow( ed, ((TedDocument *)ed->edPrivateData)->tdTraced );
    }

void tedAppDeleteColumn(	EditApplication *	ea )
    {
    EditDocument *		ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return;	}

    tedDocDeleteColumn( ed, ((TedDocument *)ed->edPrivateData)->tdTraced );
    }

void tedAppAddRowToTable(	EditApplication *	ea,
				int			after )
    {
    EditDocument *		ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return;	}

    tedDocAddRowToTable( ed, after,
			((TedDocument *)ed->edPrivateData)->tdTraced );

    return;
    }

void tedAppAddColumnToTable(	EditApplication *	ea,
				int			after )
    {
    EditDocument *		ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return;	}

    tedDocAddColumnToTable( ed, after,
			((TedDocument *)ed->edPrivateData)->tdTraced );

    return;
    }

void tedAppDeleteHeaderFooter(		EditApplication *	ea,
					int			treeType )
    {
    EditDocument *		ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return;	}

    tedDocDeleteHeaderFooter( ed, treeType,
			((TedDocument *)ed->edPrivateData)->tdTraced );

    return;
    }

void tedAppEditHeaderFooter(		EditApplication *	ea,
					APP_WIDGET		relative,
					APP_WIDGET		option,
					int			treeType )
    {
    EditDocument *		ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return;	}

    tedDocEditHeaderFooter( ed, relative, option, treeType );

    return;
    }

int tedAppChangeParagraphProperties(
				EditApplication *		ea,
				const PropertyMask *		ppSetMask,
				const ParagraphProperties *	ppNew )
    {
    EditDocument *		ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return -1;	}

    return tedDocChangeParagraphProperties( ed, ppSetMask, ppNew,
			((TedDocument *)ed->edPrivateData)->tdTraced );
    }

int tedAppChangeSectionProperties(
				EditApplication *		ea,
				const PropertyMask *		spSetMask,
				const SectionProperties *	spNew )
    {
    EditDocument *		ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return -1;	}

    return tedDocChangeSectionProperties( ed, spSetMask, spNew,
			((TedDocument *)ed->edPrivateData)->tdTraced );
    }

int tedAppChangeAllSectionProperties(
				EditApplication *		ea,
				const PropertyMask *		spSetMask,
				const SectionProperties *	spNew,
				const PropertyMask *		dpSetMask,
				const DocumentProperties *	dpSet )
    {
    EditDocument *		ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return -1;	}

    return tedDocChangeAllSectionProperties( ed,
				spSetMask, spNew, dpSetMask, dpSet,
				((TedDocument *)ed->edPrivateData)->tdTraced );
    }

int tedAppSetParagraphTabs(	EditApplication *		ea,
				const struct TabStopList *	tsl )
    {
    EditDocument *		ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return -1;	}

    return tedDocSetParagraphTabs( ed, tsl,
			((TedDocument *)ed->edPrivateData)->tdTraced );
    }

int tedAppSetDocumentProperties( EditApplication *		ea,
				const PropertyMask *		dpSetMask,
				const DocumentProperties *	dpSet )
    {
    EditDocument *		ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return -1;	}

    return tedDocSetDocumentProperties( ed, dpSetMask, dpSet,
			((TedDocument *)ed->edPrivateData)->tdTraced );
    }

int tedAppSetNewList(	EditApplication *		ea )
    {
    EditDocument *		ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return -1;	}

    return tedDocSetNewList( ed,
			((TedDocument *)ed->edPrivateData)->tdTraced );
    }

void tedAppSetImageProperties(	EditApplication *		ea,
				const PropertyMask *		pipSetMask,
				const PictureProperties *	pip )
    {
    EditDocument *	ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return;	}

    tedDocSetImageProperties( ed, pipSetMask, pip,
			((TedDocument *)ed->edPrivateData)->tdTraced );

    return;
    }

void tedAppChangeCurrentNote(	EditApplication *	ea,
				const PropertyMask *	npSetMask,
				const NoteProperties *	npSet )
    {
    EditDocument *	ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return;	}

    tedDocChangeCurrentNote( ed, npSetMask, npSet,
			((TedDocument *)ed->edPrivateData)->tdTraced );

    return;
    }

void tedAppGotoNoteRef(		EditApplication *	ea )
    {
    EditDocument *	ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return;	}

    tedDocGotoNoteRef( ed );

    return;
    }

void tedAppNextNote(		EditApplication *	ea )
    {
    EditDocument *	ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return;	}

    tedDocNextNote( ed );

    return;
    }

void tedAppPrevNote(		EditApplication *	ea )
    {
    EditDocument *	ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return;	}

    tedDocPrevNote( ed );

    return;
    }

int tedAppShiftRowsInTable(	EditApplication *	ea,
				int			direction )
    {
    EditDocument *	ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return -1;	}

    return tedDocShiftRowsInTable( ed, direction );
    }

void tedAppSetTableProperties(	EditApplication *	ea,
				int			wholeRow,
				int			wholeColumn,
				const PropertyMask *	cpSetMask,
				const CellProperties *	cpSet,
				const PropertyMask *	rpSetMask,
				const RowProperties *	rpSet )
    {
    EditDocument *	ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return;	}

    tedDocSetTableProperties( ed, wholeRow, wholeColumn,
				cpSetMask, cpSet, rpSetMask, rpSet,
				((TedDocument *)ed->edPrivateData)->tdTraced );

    return;
    }

int tedAppFollowLink(	APP_WIDGET		option,
			EditApplication *	ea,
			const MemoryBuffer *	fileName,
			const MemoryBuffer *	markName )
    {
    EditDocument *	ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return -1;	}

    return tedDocFollowLink( option, ed, fileName, markName );
    }

int tedAppChangeCurrentList(	EditApplication *		ea,
				const struct DocumentList *	dlNew )
    {
    EditDocument *		ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return -1;	}

    return tedDocChangeCurrentList( ed, dlNew,
			((TedDocument *)ed->edPrivateData)->tdTraced );
    }

int tedAppFindNext(		void *		voidea )
    {
    EditApplication *		ea= (EditApplication *)voidea;
    EditDocument *		ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return -1;	}

    return tedDocFindNext( ed );
    }

int tedAppFindPrev(		void *		voidea )
    {
    EditApplication *		ea= (EditApplication *)voidea;
    EditDocument *		ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return -1;	}

    return tedDocFindPrev( ed );
    }

int tedAppListFontToolSet(	void *				voidea,
				const PropertyMask *		taSetMask,
				const ExpandedTextAttribute *	etaSet )
    {
    EditApplication *		ea= (EditApplication *)voidea;
    EditDocument *		ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return -1;	}

    return tedDocListFontToolSet( ed, taSetMask, etaSet );
    }

int tedAppFontToolSet(		void *				voidea,
				const PropertyMask *		taSetMask,
				const ExpandedTextAttribute *	etaSet )
    {
    EditApplication *		ea= (EditApplication *)voidea;
    EditDocument *		ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return -1;	}

    return tedDocFontToolSet( ed, taSetMask, etaSet );
    }

