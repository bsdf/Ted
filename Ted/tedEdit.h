/************************************************************************/
/*									*/
/*  Ted: data structures and routines for on screen interactive		*/
/*  editing.								*/
/*									*/
/************************************************************************/

#   ifndef		TED_EDIT_H
#   define		TED_EDIT_H

#   include	<docEdit.h>
#   include	<docEditTrace.h>
#   include	<layoutContext.h>
#   include	<docSelectionGeometry.h>
#   include	<docSelectionDescription.h>

#   define	LOG_RELAYOUT	0

struct EditDocument;

typedef struct TedEditOperation
    {
    struct EditDocument *	teoEditDocument;
    EditOperation		teoEo;
    LayoutContext		teoLayoutContext;

    DocumentRectangle		teoChangedRect;
    int				teoChangedRectSet;
    DocumentRectangle		teoOldScreenRectangle;
    int				teoRefreshScreenRectangle;

    /**/
    TextAttribute		teoSavedTextAttribute;
    int				teoSavedTextAttributeNumber;

				/**
				 *   The edit trace. Only set of the 
				 *   edit operation is to be traced.
				 */
    EditTrace *			teoEditTrace;
    } TedEditOperation;

/************************************************************************/
/*									*/
/*  Routine Declarations:						*/
/*									*/
/************************************************************************/

extern int tedAdjustParagraphLayout(
				TedEditOperation *		teo,
				DocumentTree *			dt );

extern int tedStartEditOperation(
				TedEditOperation *		teo,
				SelectionGeometry *		sg,
				SelectionDescription *		sd,
				struct EditDocument *		ed,
				int				fullWidth,
				int				traced );

extern void tedInitEditOperation(	TedEditOperation *		teo );
extern void tedCleanEditOperation(	TedEditOperation *		teo );

extern int tedEditFinishSelectionHead(	TedEditOperation *		teo );
extern int tedEditFinishSelectionTail(	TedEditOperation *		teo );
extern int tedEditFinishSelectionHeadNext( TedEditOperation *		teo );

extern int tedEditFinishSelection(
				TedEditOperation *		teo,
				const DocumentSelection *	dsNew );

extern int tedEditFinishRange(	TedEditOperation *		teo,
				int				col0,
				int				col1,
				const EditRange *		erNew );

extern int tedEditFinishPosition( TedEditOperation *		teo,
				const EditPosition *		epNew );

extern int tedEditFinishOldSelection(
				TedEditOperation *		teo );

extern int tedEditReplaceSelection(
				TedEditOperation *	teo,
				const char *		addedText,
				int			addedLength,
				int			textAttributeNr );

extern int tedEditDeleteSelection(	TedEditOperation *	teo );

extern int tedEditIncludeNodeInRedraw(	TedEditOperation *		teo,
					const struct BufferItem *	bi );

extern int tedEditIncludeRowsInRedraw(	TedEditOperation *		teo,
					const struct BufferItem *	sectBi,
					int				row0,
					int				row1 );

extern int tedChangeParticuleAttributes( PropertyMask *		pTaAllMask,
					const LayoutContext *	lc,
					struct BufferItem *	bi,
					int			partFrom,
					int			partUpto,
					const TextAttribute *	taSet,
					const PropertyMask *	taSetMask );

extern void tedIncludeRectangleInChange( TedEditOperation *		teo,
					const DocumentRectangle *	dr );

extern int tedDocReplaceSelectionWithField(
				    TedEditOperation *		teo,
				    DocumentField **		pDf,
				    int *			pHeadPart,
				    int *			pTailPart,
				    DocumentSelection *		dsInside,
				    DocumentSelection *		dsAround,
				    const FieldInstructions *	fi,
				    int				fieldKind,
				    const PropertyMask *	taSetMask,
				    const TextAttribute *	taSet );

extern int tedLayoutNodeOfField(	TedEditOperation *	teo,
					DocumentSelection *	dsAround,
					unsigned int		whenMask );

extern int tedDocFlattenTypedField(
				struct EditDocument *		ed,
				int				fieldType,
				const PropertyMask *		taSetMask,
				const TextAttribute *		taSet,
				int				traced );

extern int tedReplaceSelectionWithRasterImage(
				struct EditDocument *		ed,
				RasterImage *			ri,
				int				traced );

extern int tedIncludeDocument(		TedEditOperation *	teo,
					DocumentCopyJob *	dcj );

extern int tedEditIncludeDocument(	TedEditOperation *	teo,
					DocumentCopyJob *	dcj,
					int			command,
					int			posWhere );

extern int tedEditInsertSection(	DocumentPosition *	dpBeforeSplit,
					DocumentPosition *	dpAfterSplit,
					TedEditOperation *	teo,
					int			split,
					int			after );

extern int tedDeleteSelectionImpl(	TedEditOperation *	teo );

extern int tedFlattenFieldImpl(	TedEditOperation *		teo,
				DocumentSelection *		dsExInside,
				const DocumentSelection *	dsAroundField,
				int				headPart,
				int				tailPart,
				DocumentField *			df );

extern int tedEditChangeSelectionProperties(
				TedEditOperation *		teo,
				const DocumentSelection *	ds,
				int				level,
				int				command,

				const PropertyMask *		taSetMask,
				const TextAttribute *		taSet,

				const PropertyMask *		ppSetMask,
				const ParagraphProperties *	ppSet,

				const PropertyMask *		cpSetMask,
				const CellProperties *		cpSet,

				const PropertyMask *		rpSetMask,
				const RowProperties *		rpSet,

				const PropertyMask *		spSetMask,
				const SectionProperties *	spSet,

				const PropertyMask *		dpSetMask,
				const DocumentProperties *	dpSet );

extern int tedEditChangeSelectionPropertiesImpl(
				TedEditOperation *		teo,
				const DocumentSelection *	ds,

				const PropertyMask *		taSetMask,
				const TextAttribute *		taSet,

				const PropertyMask *		ppSetMask,
				const ParagraphProperties *	ppSet,

				const PropertyMask *		cpSetMask,
				const CellProperties *		cpSet,

				const PropertyMask *		rpSetMask,
				const RowProperties *		rpSet,

				const PropertyMask *		spSetMask,
				const SectionProperties *	spSet,

				const PropertyMask *		dpSetMask,
				const DocumentProperties *	dpSet );

extern int tedObjectSetImagePropertiesImpl(
				TedEditOperation *		teo,
				InsertedObject *		io,
				const DocumentPosition *	dpObj,
				int				partObj,
				const PropertyMask *		pipSetMask,
				const PictureProperties *	pipSet );

extern int tedChangeNoteImpl(	EditOperation *			eo,
				DocumentField *			dfNote,
				struct DocumentNote *		dn,
				int				selInNote,
				const PropertyMask *		npSetMask,
				const struct NoteProperties *	npSet );

extern int tedDocRollRowsInTableImpl(
				TedEditOperation *		teo,
				const TableRectangle  *		tr,
				int				move,
				int				rowsdown );

extern int tedDocUpdField(	TedEditOperation *		teo,
				DocumentField *			df,
				const FieldInstructions *	fi );

extern int tedDocSetField(	TedEditOperation *		teo,
				const DocumentSelection *	ds,
				int				command,
				int				fieldKind,
				const FieldInstructions *	fi,
				const PropertyMask *		taSetMask,
				const TextAttribute *		taSet );

extern int tedMergeParagraphsInSelection(
					struct EditDocument *	ed,
					int			traced );

extern int tedEditShiftRowsInTable(	struct EditDocument *	ed,
					int			direction,
					int			traced );

extern int tedRefreshTocField(	DocumentSelection *	dsAroundToc,
				TedEditOperation *	teo,
				DocumentField *		dfToc );

extern int tedDeleteTableSliceSelection( struct EditDocument *	ed,
					int			traced );

extern int tedDocReplaceSelection(	struct EditDocument *	ed,
					int			command,
					const char *		word,
					int			len,
					int			traced );

int tedDocReplaceSelectionTyping(	struct EditDocument *	ed,
					const char *		str,
					int			length );

extern int tedDocDeleteSelection(	struct EditDocument *	ed,
					int			command,
					int			traced );

extern int tedObjectSetImageProperties(
				struct EditDocument *		ed,
				InsertedObject *		io,
				const DocumentPosition *	dpObject,
				int				partObject,
				const PropertyMask *		pipSetMask,
				const PictureProperties *	pipFrom,
				int				traced );

extern int tedInsertTable(		struct EditDocument *	ed,
					int			rows,
					int			columns,
					int			traced );

extern int tedEditStartReplace(	DocumentSelection *	dsTraced,
				TedEditOperation *	teo,
				int			command,
				int			level,
				unsigned int		flags );

extern int tedEditStartStep(		TedEditOperation *	teo,
					int			command );
extern int tedEditStartTypedStep(	TedEditOperation *	teo,
					int			command,
					int			fieldKind );

extern int tedFinishEditOperation(	TedEditOperation *	teo );

extern void tedDocInsertStringWithAttribute(
					struct EditDocument *	ed,
					const char *		bytes,
					int			size,
					const TextAttribute *	taSet,
					const PropertyMask *	taSetMask,
					int			traced );

extern void tedEditInsertSpecialParticule(
				struct EditDocument *		ed,
				int				kind,
				int				command,
				int				redoLayout,
				int				traced );

extern int tedInsertPageNumber( struct EditDocument *		ed,
				int				traced );

extern int tedIncludePlainDocument(	struct EditDocument *	ed,
					BufferDocument *	bdFrom,
					int			traced );

extern int tedIncludeRtfDocument(	struct EditDocument *	ed,
					BufferDocument *	bdFrom,
					int			traced );

#   endif	/*	TED_EDIT_H	*/
