/************************************************************************/
/*									*/
/*  Text Editor Buffer structure.					*/
/*									*/
/************************************************************************/

#   ifndef	DOC_BUF_H
#   define	DOC_BUF_H

#   include	<time.h>

#   include	<bitmap.h>
#   include	<psPostScriptFontList.h>
#   include	<geo2DInteger.h>
#   include	<utilMemoryBuffer.h>

#   include	<docFramePropertiesAdmin.h>

#   include	<docStyleSheet.h>
#   include	<docDocumentProperties.h>
#   include	<docDocumentFieldList.h>
#   include	"docDrawingShapeList.h"
#   include	<docInsertedObjectList.h>
#   include	<docObject.h>
#   include	"docSelect.h"
#   include	"docDocumentTree.h"
#   include	<docListTable.h>

#   include	<docEditPosition.h>
#   include	<docEditRange.h>

/************************************************************************/
/*									*/
/*  Forward declarations of structs.					*/
/*									*/
/************************************************************************/

struct BufferDocument;
struct BufferItem;
struct TextLine;
struct TextParticule;
struct DocumentNote;
struct NoteProperties;
struct ParagraphProperties;
struct BorderProperties;
struct TabStopList;
struct DocumentPropertyLists;

/************************************************************************/
/*									*/
/*  The document as a whole.						*/
/*									*/
/************************************************************************/

typedef struct NotesList
    {
    struct DocumentNote *	nlNotes;
    int				nlNoteCount;
    } NotesList;

typedef struct BufferDocument
    {
				/**
				 *  Numbered properties of the document.
				 *  In some cases, the lists are shared 
				 *  between documents.
				 */
    struct DocumentPropertyLists *	bdPropertyLists;

				/**
				  *  The body of the document. This is the 
				  *  main document tree. Page headers and 
				  *  page footers are attached to the sections.
				  *  
				  *  Notes are kept in bdNotesList. The 
				  *  separators between the notes and ordinary
				  *  text are declared below.
				  */
    DocumentTree		bdBody;
    DocumentProperties		bdProperties;

    InsertedObjectList		bdObjectList;
    DocumentStyleSheet		bdStyleSheet;
    DocumentFieldList		bdFieldList;
    DrawingShapeList		bdShapeList;

				/**
				  *  The notes in this document. References 
				  *  from the contents of the various document 
				  *  trees to this list always pass through 
				  *  the field list of that tree.
				  *
				  *  In practice, only the document body holds
				  *  notes. The implementation of notes 
				  *  implcitly relies on this assumption.
				  */
    NotesList			bdNotesList;

				/**
				  *  The Identifiers of the sequence 
				  *  fields in this document. It is map from 
				  *  the identifier to an int. The value of 
				  *  the int is only relevant inside the loop 
				  *  that renumbers the SEQ fields of a tree 
				  *  in the document.
				  */
    void *			bdSeqFieldIdentifiers;

    DocumentTree		bdEiFtnsep;
    DocumentTree		bdEiFtnsepc;
    DocumentTree		bdEiFtncn;

    DocumentTree		bdEiAftnsep;
    DocumentTree		bdEiAftnsepc;
    DocumentTree		bdEiAftncn;

				/**
				  *  Used to smuggle the command line property 
				  *  into the line formatter.
				  */
    unsigned char		bdAutoHyphenate;
    } BufferDocument;

/************************************************************************/

# define docGetObject( bd, n ) \
		docGetObjectByNumber( &((bd)->bdObjectList), (n) )
# define docClaimObject( p, bd ) \
		docClaimInsertedObject( (p), &((bd)->bdObjectList) )

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern const char * docKindStr(		int			kind );
extern const char * docLevelStr(	int			level );
extern const char * docTreeTypeStr(	int			level );
extern const char * docFieldKindStr(	int			kind );
extern const char * docObjectKindStr(	int			kind );
extern const char * docBreakKindStr(	int			kind );
extern const char * docAttributeStr(	const TextAttribute *	ta );
extern const char * docParticuleFlagsStr( int			flags );

extern int docGetDefaultFont(		BufferDocument *	bd );

extern BufferDocument * docNewFile(
				TextAttribute *			taDefault,
				const char *			defaultFontName,
				int				defaultFontSize,
				const PostScriptFontList *	psfl,
				const DocumentGeometry * 	dg );

extern void docFreeDocument( BufferDocument *	bd );
extern BufferDocument * docNewDocument( const BufferDocument *	bdRef );

extern int docNextPosition(	DocumentPosition *	dp );	/* technical */
extern int docPrevPosition(	DocumentPosition *	dp );	/* technical */

extern int docGotoNextPosition(	DocumentPosition *	dp );	/* navigation */
extern int docGotoPrevPosition(	DocumentPosition *	dp );	/* navigation */

extern int docNextWord(		DocumentPosition *	dp );
extern int docPrevWord(		DocumentPosition *	dp );

extern int docLineDown(		int *			pLine,
				DocumentPosition *	dp,
				int			line );

extern int docLineUp(		int *			pLine,
				DocumentPosition *	dp,
				int			line );

extern int docGotoFirstPosition( DocumentPosition *	dp,	/* navigation */
				struct BufferItem *	node );
extern int docGotoLastPosition(	DocumentPosition *	dp,	/* navigation */
				struct BufferItem *	node );
extern int docHeadPosition(	DocumentPosition *	dp,	/* technical */
				struct BufferItem *	node );
extern int docTailPosition(	DocumentPosition *	dp,	/* technical */
				struct BufferItem *	node );

extern int docDocumentHead(	DocumentPosition *	dp,
				BufferDocument *	bd );

extern int docDocumentTail(	DocumentPosition *	dp,
				BufferDocument *	bd );

extern void docAvoidParaHeadField(	DocumentPosition *	dp,
					int *			pPart,
					const BufferDocument *	bd );

extern void docSetParaSelection( DocumentSelection *	ds,
				struct BufferItem *	node,
				int			direction,
				int			start,
				int			length );

extern int docComparePositions(	const DocumentPosition *	dp1,
				const DocumentPosition *	dp2 );

extern int docSetDocumentPosition(	DocumentPosition *	dp,
					struct BufferItem *	node,
					int			stroff );

extern int docCompareNodePositionToSelection(
				const struct BufferItem *	node,
				const DocumentSelection *	ds );

extern int docCompareCellPositionToSelection(
				int *				pSelected,
				const struct BufferItem *	cellBi,
				const DocumentSelection *	ds );

extern void docDeleteDocumentTree(	BufferDocument *	bd,
					DocumentTree *		dt );

extern void docEraseDocumentTree(	BufferDocument *	bd,
					DocumentTree *		dt );

extern InsertedObject * docClaimObjectCopy(
					BufferDocument *	bd,
					int *			pNr,
					const InsertedObject *	ioFrom );

extern int docDelimitTable(	const struct BufferItem *	paraNode,
				struct BufferItem **		pParentNode,
				int *			pCol,
				int *			pRow0,
				int *			pRow,
				int *			pRow1 );

extern void docTableDetermineCellspans(	int *			pRowspan,
					int *			pColspan,
					const struct BufferItem *	cellNode );

extern int docGetCellBottom(	LayoutPosition *		lpBottom,
				const struct BufferItem *	cellNode );

extern void docLineSelection(	DocumentSelection *	dsLine,
				int *			pPartLineBegin,
				int *			pPartLineEnd,
				const struct BufferItem *	node,
				int			line );

extern void docWordSelection(
			DocumentSelection *		dsWord,
			int *				pIsObject,
			const DocumentPosition *	dpAround );

extern int docWhatPageHeader(	DocumentTree **			pTree,
				int *				pIsEmpty,
				const struct BufferItem *	bodySectNode,
				int				page,
				const BufferDocument *		bd );

extern int docWhatPageFooter(	DocumentTree **			pTree,
				int *				pIsEmpty,
				const struct BufferItem *	bodySectNode,
				int				page,
				const BufferDocument *		bd );

extern int docWhatPagesForHeaderFooter(
				const DocumentProperties *	dp,
				const SectionProperties *	sp,
				int				treeType );

extern struct BufferItem * docGetSelectionRoot(
				DocumentTree **			pTree,
				struct BufferItem **		pBodySectNode,
				BufferDocument *		bd,
				const DocumentSelection *	ds );

extern int docGetHeaderFooter(	DocumentTree **			pTree,
				struct BufferItem **		pBodySectNode,
				const DocumentPosition *	dp,
				BufferDocument *		bd,
				int				treeType );

extern int docInquireHeadersFooters(
				int *				pDocHasHeaders,
				int *				pDocHasFooters,
				const BufferDocument *		bd );

extern int docMakeDocumentTree(	BufferDocument *		bd,
				DocumentTree *			dt,
				const SelectionScope *		ss,
				const SectionProperties *	sp );

extern struct BufferItem * docMakeExternalParagraph(
				BufferDocument *		bd,
				DocumentTree *			dt,
				int				treeType,
				const struct BufferItem *	bodyNode,
				int				ownerNumber,
				int				txtAttrNr );

extern struct BufferItem * docMakeTreeRoot(
				BufferDocument *		bd,
				DocumentTree *			dt,
				const struct BufferItem *	bodyBi,
				int				ownerNumber,
				int				treeType );

extern struct BufferItem * docInsertEmptyParagraph(
				BufferDocument *		bd,
				struct BufferItem *		node,
				int				txtAttrNr );

extern int docTableRectangleSelection(	DocumentSelection *	ds,
					struct BufferItem **	pSelParentBi,
					BufferDocument *	bd,
					const TableRectangle *	tr );

extern int docMergeDocumentLists(
				int **				pFontMap,
				int **				pListStyleMap,
				BufferDocument *		bdTo,
				BufferDocument *		bdFrom,
				const int *			colorMap,
				const int *			rulerMap );

extern int docMergeColorTables(	int **				pColorMap,
				BufferDocument *		bdTo,
				const BufferDocument *		bdFrom );

extern int docFindBookmarkInDocument(	DocumentSelection *	dsInside,
					int *			pHeadPart,
					int *			pTailPart,
					const BufferDocument *	bd,
					const MemoryBuffer *	markName );

extern DocumentField * docMakeField(
				BufferDocument *		bd,
				DocumentTree *			dt,
				const DocumentSelection *	dsInput,
				int				part0,
				int				part1,
				int				attr0,
				int				attr1 );

extern void docConstrainSelectionToOneParagraph( int *		pHeadMoved,
					int *			pTailMoved,
					DocumentSelection *	ds );

extern int docParaNextWord(		const struct BufferItem *	paraNode,
					int			stroff );

extern int docParaPrevWord(		const struct BufferItem *	paraNode,
					int			stroff );

extern int docParaHeadOfWord(		int *				pStroff,
					const struct BufferItem *		paraNode,
					int				part );

extern int docParaTailOfWord(		int *				pStroff,
					const struct BufferItem *		paraNode,
					int				part );

extern int docParaPastLastNonBlank(	const struct BufferItem *	paraNode,
					int			from,
					int			upto );

extern DocumentTree * docSectionHeaderFooter(
				const struct BufferItem *	bodySectNode,
				unsigned char *			pApplies,
				const DocumentProperties *	dp,
				int				treeType );

extern int docSectionHasHeaderFooter(
				const struct BufferItem *	bodySectNode,
				unsigned char  *		pApplies,
				const DocumentProperties *	dp,
				int				treeType );

DocumentTree *	docDocumentNoteSeparator(
					BufferDocument *	bd,
					int			which );

extern int docGetTreeOfNode(		DocumentTree **		pTree,
					struct BufferItem **	pBodySectNode,
					BufferDocument *	bd,
					struct BufferItem *		node );

extern int docGetRootOfSelectionScope(	DocumentTree **		pTree,
					struct BufferItem **	pBodySectNode,
					BufferDocument *	bd,
					const SelectionScope *	ss );

extern int docSectionHeaderFooterFirstPage(
				int *				pUsedInDocument,
				const struct BufferItem *	bodySectNode,
				int				treeType,
				const DocumentProperties *	dp );

extern int docSelectWholeSection(	DocumentSelection *	bs,
					int			direction );
extern int docSelectWholeTable(		DocumentSelection *	bs );
extern int docSelectRow(		DocumentSelection *	bs,
					int			direction,
					int			allColumns );
extern int docSelectWholeCell(		DocumentSelection *	bs,
					int			direction,
					int			allRows );
extern int docSelectColumn(		DocumentSelection *	ds,
					int			col );
extern int docSelectWholeParagraph(	DocumentSelection *	bs,
					int			direction );

extern int docSelectWholeBody(		DocumentSelection *	ds,
					BufferDocument *	bd );

extern int docNumberOfParagraph(	const struct BufferItem *	paraNode );

extern struct BufferItem * docGetParagraphByNumber(
					const DocumentTree *	dt,
					int			n );

extern struct BufferItem * docGetCommonParent(
				    struct BufferItem *		paraNode1,
				    struct BufferItem *		paraNode2 );

extern void docSetTreeTypeOfNode(	struct BufferItem *		node,
					int			treeType );

extern int docCheckNoteSeparatorItem(	BufferDocument *	bd,
					int			sepTreeType );
extern int docCheckSeparatorItemForNoteType(
					BufferDocument *	bd,
					int			noteTreeType );

extern int docCountParticulesInField(	const struct BufferItem *	paraNode,
					int *			pClosed,
					int			part,
					int			partUpto );

extern int docGetSolidRgbShadeOfItem(	int *			pIsFilled,
					RGB8Color *		rgb8,
					const BufferDocument *	bd,
					const ItemShading *	is );

extern void docDelimitTables(		struct BufferItem *	parentNode,
					int			recursively );

extern int docTextAttributeNumber(	BufferDocument *	bd,
					const TextAttribute *	ta );

extern void docGetTextAttributeByNumber( TextAttribute *		ta,
					const BufferDocument *		bd,
					int				n );

extern int docItemShadingNumber(	BufferDocument *		bd,
					const ItemShading *		is );

extern void docGetItemShadingByNumber(	ItemShading *			is,
					const BufferDocument *		bd,
					int				n );

extern int docShadingNumberIsShading(	const BufferDocument *		bd,
					int				n );

extern int docFramePropertiesNumber(	BufferDocument *	bd,
					const FrameProperties *	fp );

extern void docGetFramePropertiesByNumber(
				FrameProperties *		fp,
				const BufferDocument *		bd,
				int				n );

extern int docBorderPropertiesNumber(	BufferDocument *		bd,
					const struct BorderProperties *	bp );

extern void docGetBorderPropertiesByNumber(
				struct BorderProperties *	bp,
				const BufferDocument *		bd,
				int				n );

extern int docBorderNumberIsBorder(	const BufferDocument *		bd,
					int				n );


extern int docTabStopListNumber(	BufferDocument *	bd,
					const struct TabStopList *		tsl );

extern void docGetTabStopListByNumber(
				struct TabStopList *		tsl,
				const BufferDocument *		bd,
				int				n );

extern int docBalanceFieldSelection(	DocumentField **	pLeftField,
					DocumentField **	pRightField,
					int *			pBeginMoved,
					int *			pEndMoved,
					int *			pHeadPart,
					int *			pTailPart,
					DocumentPosition *	dpBegin,
					DocumentPosition *	dpEnd,
					DocumentTree *		dt,
					BufferDocument *	bd );

extern int docGetListOfParagraph(	struct ListOverride **	pLo,
					struct DocumentList **	pDl,
					int			ls,
					const BufferDocument *	bd );

extern int docGetListLevelOfParagraph(
				int *				startPath,
				int *				formatPath,
				struct ListOverride **		pLo,
				struct DocumentList **		pDl,
				const struct ListLevel **	pLl,
				const ParagraphProperties *	pp,
				const BufferDocument *		bd );

extern int docRemoveParagraphFromList(	struct BufferItem *	paraNode,
					DocumentTree *		dt,
					BufferDocument *	bd );

extern int docParaHeadFieldKind( const struct BufferItem *	paraNode,
				const BufferDocument *		bd );

extern int docDelimitParaHeadField(
				DocumentField **	pField,
				DocumentSelection *	dsInsideField,
				DocumentSelection *	dsAroundField,
				int *			pPartBegin,
				int *			pPartEnd,
				const struct BufferItem *	paraNode,
				const BufferDocument *	bd );

extern int docFindListOfSelection(
				int *				pLs,
				int *				pLevel,
				int *				pMultiList,
				int *				pMultiLevel,
				int *				pParaNr,
				const DocumentSelection *	ds,
				DocumentTree *			tree,
				BufferDocument *		bd );

extern int docApplyListRuler(	const struct DocumentList *	dl,
				const struct ListOverride *	lo,
				BufferDocument *		bd );

extern int docGetEffectiveTextAttributes(
				TextAttribute *			ta,
				BufferDocument *		bd,
				const struct BufferItem *	paraNode,
				int				part );

extern int docGetSelectionAttributes(
				BufferDocument *		bd,
				const DocumentSelection *	ds,
				PropertyMask *			pUpdMask,
				TextAttribute *			pTaNew );

extern int docInsertParaHeadField(
				DocumentField **		pField,
				DocumentSelection *		dsInsideField,
				DocumentSelection *		dsAroundField,
				int *				pHeadPart,
				int *				pTailPart,
				struct BufferItem *		paraNode,
				BufferDocument *		bd,
				DocumentTree *			dt,
				int				fieldKind,
				int				textAttrNr );

extern int docDelimitFieldInDoc(DocumentSelection *		dsInside,
				DocumentSelection *		dsAround,
				int *				pPart0,
				int *				pPart1,
				const BufferDocument *		bd,
				const DocumentField *		df );

extern int docDelimitFieldInTree(
				DocumentSelection *		dsInside,
				DocumentSelection *		dsAround,
				int *				pPart0,
				int *				pPart1,
				const BufferDocument *		bd,
				const DocumentTree *		dt,
				const DocumentField *		df );

extern int docDeleteFieldFromParent(
				DocumentTree *			dt,
				DocumentField *			df );

extern int docNextSimilarRoot(	DocumentPosition *		dp,
				int *				pPage,
				int *				pColumn,
				BufferDocument *		bd );

extern int docPrevSimilarRoot(	DocumentPosition *		dp,
				int *				pPage,
				int *				pColumn,
				BufferDocument *		bd );

extern int docAdaptParagraphToListLevel(
				int *				pChanged,
				struct BufferItem *		paraNode,
				const BufferDocument *		bd );

extern int docMakeCapsString(	char **			pUpperString,
				int **			pSegments,
				int *			pSegmentCount,
				const TextAttribute *	ta,
				const char *		printString,
				int			len );

extern int docChangeRowProperties(
				PropertyMask *			rpDonePask,
				struct BufferItem *		rowNode,
				const PropertyMask *		rpSetMask,
				const RowProperties *		rpFrom );

extern int docSelectionForEditPositionsInDoc(
				DocumentSelection *		ds,
				const BufferDocument *		bd,
				const SelectionScope *		ss,
				const EditPosition *		epHead,
				const EditPosition *		epTail );

extern int docSelectionForEditPositionsInTree(
				DocumentSelection *		ds,
				const DocumentTree *		dt,
				const EditPosition *		epHead,
				const EditPosition *		epTail );

extern int docPositionForEditPosition(	DocumentPosition *		dpNew,
					const EditPosition *		ep,
					const DocumentTree *		dt );

extern int docShiftFieldReferences(	DocumentTree *		dt,
					int			sectFrom,
					int			paraFrom,
					int			stroffFrom,
					int			sectShift,
					int			paraShift,
					int			stroffShift );

extern int docGetCellRight(		int *			pColspan,
					const struct BufferItem *	cellNode );

extern int docGetMatchingCell(		int *			pColspan,
					const struct BufferItem *	rowNode,
					int			l,
					int			r );

extern const CellProperties *	docGetCellProperties(
					const RowProperties **	pRp,
					const struct BufferItem *	cellNode );

extern void docSetCellRectangleProperties( CellProperties *		cpSet,
					const struct BufferItem *	rowNode,
					const TableRectangle *		tr,
					const DocumentAttributeMap *	dam );

extern void docDeleteFieldFromDocument(	BufferDocument *	bd,
					DocumentField *		df );

extern int docDeleteFieldRange(		int *			pUpdateFlags,
					BufferDocument *	bd,
					const EditRange *	er,
					ChildFields *		rootFields,
					DocumentField *		dfLeft,
					DocumentField *		dfRight );

extern void docDeleteChildFields(	int *			pUpdateFlags,
					BufferDocument *	bd,
					ChildFields *		cf );

extern int docFindSetPattern(		void **			pProg,
					const char *		pattern,
					int			useRegex );

extern int docGetCharsUsed(		BufferDocument *	bd );

extern int docGetAttributesUsedInTree(BufferDocument *		bd,
				const DocumentTree *		dt,
				IndexSet *			isUsed );

extern int docGetAttributesUsedInShape(
				BufferDocument *		bd,
				const struct DrawingShape *	ds,
				IndexSet *			isUsed );

extern void docPlainTextAttribute(	TextAttribute *		ta,
					BufferDocument *	bd );

extern int docScapsAttributeNumber(		BufferDocument *	bd,
						const TextAttribute *	ta );

extern void docCleanNodeObjects(	int *			pBulletsDeleted,
					int *			pParagraphCount,
					DocumentTree *		dt,
					BufferDocument *	bd,
					struct BufferItem *	node,
					DOC_CLOSE_OBJECT	closeObject );

extern void docCleanDocumentObjects(	BufferDocument *	bd,
					DOC_CLOSE_OBJECT	closeObject );

extern const DocumentFont * docRtfGetCurrentFont(
				BufferDocument *		bd,
				TextAttribute *			ta );

extern const char * docGetEncodingName(
				BufferDocument *		bd,
				TextAttribute *			ta,
				int				charset );

extern void docGetColorByNumber(	RGB8Color *		rgb8,
					const BufferDocument *	bd,
					int			colorNumber );

extern void docDeleteObject(	BufferDocument *		bd,
				int				n );

extern int docChangeParticuleAttributes( int *			pChanged,
					PropertyMask *		pTaAllMask,
					BufferDocument *	bd,
					struct BufferItem *		paraNode,
					int			part,
					int			partUpto,
					const TextAttribute *	taSet,
					const PropertyMask *	taSetMask );

extern int docCopySectHeadersFooters(	struct BufferItem *	sectNodeTo,
					BufferDocument *	bdTo,
					struct BufferItem *	sectNodeFrom,
					BufferDocument *	bdFrom );

extern int docCopySectDescription(	struct BufferItem *	sectNodeTo,
					BufferDocument *	bdTo,
					struct BufferItem *	sectNodeFrom,
					BufferDocument *	bdFrom );

extern int docNodeAtExtremity(	int *				pAtExtremity,
				const struct BufferItem *	parentNode,
				const struct BufferItem *	paraNode,
				int				after );

extern int docHeaderFooterPage(		const BufferDocument *	bd,
					const struct BufferItem *	bodySectNode,
					int			currentPage,
					int			treeType );

extern void docInvalidateSectHeaderFooterLayout( struct BufferItem *	sectNode );

extern void docInvalidateParagraphLayout(	struct BufferItem *	paraNode );
extern void docInvalidateNodeLayout(		struct BufferItem *	node );

extern struct BufferItem * docGetBodySectNodeOfScope(
					const SelectionScope *	ss,
					const BufferDocument *	bd );

extern void docOverridePaperSize(	struct BufferDocument *		bd,
					const DocumentGeometry *	dgFrom );

#   endif
