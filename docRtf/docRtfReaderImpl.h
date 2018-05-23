/************************************************************************/
/*									*/
/*  Reading RTF.							*/
/*									*/
/************************************************************************/

#   ifndef		RTF_READER_IMPL_H
#   define		RTF_READER_IMPL_H

#   include	<docBuf.h>
#   include	<docNoteProperties.h>
#   include	<sioGeneral.h>
#   include	<docTabStop.h>
#   include	<docTabStopList.h>
#   include	<docListOverride.h>
#   include	<docDocumentList.h>
#   include	<docBorderProperties.h>
#   include	"docRtfTextConverter.h"
#   include	"docRtfControlWord.h"

struct RtfReader;
typedef struct RtfReader RtfReader;

/************************************************************************/
/*									*/
/*  Context for Reading RTF.						*/
/*									*/
/************************************************************************/

struct RtfFieldStackLevel;

typedef struct RtfReadingState
    {
    int				rrsBytesPerUnicode;
    int				rrsUnicodeBytesToSkip;

    TextAttribute		rrsTextAttribute;
				    /************************************/
				    /*  Font number is in buffer terms.	*/
				    /*  NOT in file terms.		*/
				    /************************************/
    int				rrsTextCharset;
    ItemShading			rrsTextShading;
    int				rrsTextShadingChanged;
    ParagraphProperties		rrsParagraphProperties;
    TabStopList			rrsTabStopList;
    ItemShading			rrsParagraphShading;
    FrameProperties		rrsParaFrameProperties;

    MemoryBuffer		rrsSavedText;

    struct RtfReadingState *	rrsPrev;
    } RtfReadingState;

typedef int (*RtfAddTextParticule)(	struct RtfReader *	rr,
					const char *		text,
					int			len );

struct RtfReader
    {
    RtfReadingState *		rrcState;
    RtfAddTextParticule		rrcAddParticule;
    SimpleInputStream *		rrcSis;

				/**
				 *  Type of BufferItem. Still retated 
				 *  to the depth in the document
				 */
    int				rrcLevel;

				/**
				 *  Incremented/Decremented around groups 
				 *  with an unknown control word. This is 
				 *  to ignore everything inside the group and 
				 *  to only complain about the unknown group 
				 *  and to shut up about its contents.
				 */
    int				rrcInIgnoredGroup;

				/**
				 *  Complain about unknown control words
				 */
    int				rrcComplainUnknown;

				/**
				 *  Character that could not be unread
				 *  using ungetc().
				 */
    int				rrcCharacterAhead;

    int				rrcAfterNoteref;

    int				rrReadFlags;

    int				rrCurrentLine;
    struct BufferDocument *	rrDocument;
    struct DocumentTree *	rrcTree;
    struct BufferItem *		rrcNode;
				/****************************************/
				/*  The current position in the input.	*/
				/****************************************/
				/****************************************/
				/*  Text attributes of the current pos.	*/
				/****************************************/
    FrameProperties		rrcRowFrameProperties;

    CellProperties		rrcCellProperties;
    PropertyMask		rrcCellPropertyMask;
    ItemShading			rrcCellShading;

    SectionProperties		rrcSectionProperties;
    PropertyMask		rrcSectionPropertyMask;
    int				rrcSectionColumn;

    RowProperties		rrcRowProperties;
    PropertyMask		rrcRowPropertyMask;
    ItemShading			rrcRowShading;

    BorderProperties		rrcBorderProperties;

    struct DrawingShape *	rrDrawingShape;
    const RtfControlWord *	rrShapeProperty;
    MemoryBuffer		rrShapePropertyName;
    MemoryBuffer		rrShapePropertyValue;

    int				rrcNextObjectVertex;
				    /**
				     *  The style in the stylesheet that is 
				     *  built up before it is inserted in the 
				     *  document. It is abused to hold the 
				     *  new properties in a property change.
				     */
    DocumentStyle		rrcStyle;
    DocumentProperties		rrcDocumentProperties;
    PropertyMask		rrcDocPropertyMask;
    TabStop			rrcTabStop;
    RGB8Color			rrcColor;
    int				rrcGotComponent;
    struct tm			rrcTm;
    unsigned char *		rrcInfoText;
    SelectionScope		rrcSelectionScope;

    ParagraphNumber		rrcParagraphNumber;
    ItemShading			rrcParagraphNumberTextShading;
    ParagraphNumber *		rrcParagraphNumbers;
    int				rrcParagraphNumberCount;

    DocumentList		rrcDocumentList;
    ListLevel			rrcDocumentListLevel;
    MemoryBuffer		rrcListLevelText;
    MemoryBuffer		rrcListLevelNumbers;

    ListOverride		rrcListOverride;
    ListOverrideLevel		rrcListOverrideLevel;

				/****************************************/
				/*  Document properties.		*/
				/****************************************/
    DocumentFont		rrcCurrentFont;
    EncodedFont			rrcCurrentEncodedFont;
    PagedList			rrcEncodedFontList;

    int				rrcDefaultFont;
    int				rrcDefaultFontDbch;
    int				rrcDefaultFontLoch;
    int				rrcDefaultFontHich;
    int				rrcDefaultFontBi;

				/****************************************/
				/*  For reading the font table.		*/
				/*  Style sheet.			*/
				/****************************************/
    int				rrcInsertedObjectNr;
    PictureProperties		rrcPictureProperties;
    PropertyMask		rrcPicturePropMask;
    MemoryBuffer		rrcBookmark;
    struct RtfFieldStackLevel *	rrcFieldStack;
    int				rrcLastFieldNumber;
				/****************************************/
				/*  For reading 'objects' and pictures.	*/
				/*  For reading 'fields'.		*/
				/****************************************/
    int				rrAfterParaHeadField;
    int				rrParagraphBreakOverride;
    int				rrcGotDocGeometry;
				/****************************************/
				/*  For coping with the way word saves	*/
				/*  {\pntext ... }			*/
				/****************************************/

    struct TextConverter *	rrRtfTextConverter;
    struct TextConverter *	rrTextTextConverter;

				/**
				 * Only used for reading Undo/Redo traces.
				 * -1: old; 1: new
				 */
    int				rrcTraceReadWhat;
    int				rrcTraceCommand;
    int				rrcTraceSelectionPosition;
    int				rrcTraceFieldKind;
    int				rrcTraceInProps;

    SelectionScope		rrcTraceOldSelectionScope;
    EditRange			rrcTraceOldRange;
    int				rrcTraceOldCol0;
    int				rrcTraceOldCol1;
    int				rrcTraceOldPage;
    int				rrcTraceOldColumn;

    int				rrcTracePropLevel;
    int				rrcTraceTargetType;

    SelectionScope		rrcTraceNewSelectionScope;
    EditRange			rrcTraceNewRange;
    int				rrcTraceNewCol0;
    int				rrcTraceNewCol1;
    int				rrcTraceNewPage;
    int				rrcTraceNewColumn;

    NoteProperties		rrcNoteProperties;
    PropertyMask		rrcNotePropertyMask;
    };

/************************************************************************/
/*									*/
/*  What you can find in an RTF file.					*/
/*									*/
/************************************************************************/

#   define	RTFfiCTRLGROUP	1
#   define	RTFfiWORD	2
#   define	RTFfiCHAR	3
#   define	RTFfiTAB	4
#   define	RTFfiCLOSE	5
#   define	RTFfiSTARGROUP	6
#   define	RTFfiTEXTGROUP	7

/************************************************************************/

/************************************************************************/
/*									*/
/*  Shortcuts in the definition of property tables.			*/
/*									*/
/************************************************************************/

#   define RTF_TEXT_GROUP( s, id, co ) \
		RTF_DEST_CO( s, id, docRtfApplyDocEncodedTextGroup, co )

#   define RTF_BYTE_GROUP( s, id, co ) \
		RTF_DEST_CO( s, id, docRtfApplyRawBytesGroup, co )

/************************************************************************/

typedef enum RtfLookupId
    {
    RTFlookupXEF,
    RTFlookupBXE,
    RTFlookupIXE,

    RTFlookupTC,
    RTFlookupTCN,
    RTFlookupTCF,
    RTFlookupTCL,

    RTFlookup_COUNT
    } RtfLookupId;

/************************************************************************/
/*									*/
/*  Common data.							*/
/*									*/
/************************************************************************/

extern RtfControlWord	docRtfDocumentGroups[];
extern const char DOC_RTF_LENIENT_MESSAGE[];

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docRtfPopReadingState(	RtfReader *		rr );

extern const RtfControlWord * docRtfFindPropertyWord(
					const char *		controlWord );

extern const RtfControlWord * docRtfFindShapePropertyWord(
					const char *		controlWord );

extern int docRtfFindControl(	RtfReader *		rr,
				int *			pC,
				char *			controlWord,
				int *			pGotArg,
				int *			pArg );

extern int docRtfIgnoreWord(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr );

extern int docRtfIgnoreText(	RtfReader *		rr,
				const char *		text,
				int			len );

extern int docRtfRefuseText(	RtfReader *		rr,
				const char *		text,
				int			len );

extern int docRtfSaveDocEncodedText(
				RtfReader *		rr,
				const char *		text,
				int			len );

extern int docRtfSaveFontEncodedText(
				RtfReader *		rr,
				const char *		text,
				int			len );

extern int docRtfSaveRawBytes(	RtfReader *		rr,
				const char *		text,
				int			len );

extern int docRtfStoreSavedText(char **			pTarget,
				int *			pSize,
				RtfReader *		rr,
				int			removeSemicolon );

extern int docRtfFontTable(		const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfColorTable(		const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfRevisionTable(		const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfStylesheet(		const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfListTable(		const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfReadInfo(		const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfRememberDocProperty(	const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfReadUnknownGroup(	RtfReader *		rr );

extern int docRtfRememberParaFrameProperty(
					const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfRememberRowFrameProperty(
					const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfRememberProperty(	const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfPopScopeFromFieldStack( RtfReader *		rr );
extern int docRtfPopParaFromFieldStack( RtfReader *		rr,
					int			paraNr );

extern int docRtfRememberParagraphProperty( 
					const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfBeginBorder(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr );

extern int docRtfBrdrProperty(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr );

extern int docRtfRememberSectionProperty( 
					const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfRememberTextProperty(	const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfRememberPntextProperty( const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfRememberCellProperty(	const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfRememberCellInstanceProperty(
					const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfRememberRowProperty(	const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfRememberStyleProperty(	const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfRememberListLevelProperty(
					const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfRememberListProperty(	const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern void docRtfPushReadingState(	RtfReader *		rr,
					RtfReadingState *	rrs );

extern const RtfControlWord * docRtfFindWord(
					const char *		controlWord,
					const RtfControlWord *	controlWords );

extern int docRtfApplyControlWord(	const RtfControlWord *	rcw,
					int			gotArg,
					int			arg,
					RtfReader *		rr );

extern int docRtfReadGroup(	const RtfControlWord *	rcw,
				int			gotArg,
				int			arg,
				RtfReader *		rr,
				const RtfControlWord *	groupWords,
				RtfAddTextParticule	addParticule,
				RtfCommitGroup		commitGroup );

extern int docRtfReadGroupX(	const RtfControlWord *	rcw,
				const RtfControlWord *	applyFirst,
				int			gotArg,
				int			arg,
				RtfReader *		rr,
				const RtfControlWord *	groupWords,
				RtfAddTextParticule	addParticule,
				RtfCommitGroup		commitGroup );

extern int docRtfSkipGroup(	const RtfControlWord *	groupRcw,
				int			ignored_arg,
				RtfReader *		rr );

extern int docRtfConsumeGroup(	const RtfControlWord *	applyFirst,
				int			gotArg,
				int			arg,
				RtfReader *		rr,
				const RtfControlWord *	groupWords,
				RtfAddTextParticule	addParticule );

extern int docRtfReadPict(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr );

extern int docRtfReadShpXPict(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr );

extern int docRtfReadObject(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr );

extern int docRtfReadDrawingObject(
				const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr );

extern int docRtfReadShape(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr );

extern int docRtfTextParticule(	RtfReader *		rr,
				const char *		text,
				int			len );

extern int docRtfReadField(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr );

extern int docRtfReadTextField(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr );

extern int docRtfLookupEntry(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr );

extern int docRtfReadExtTree(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr );

extern int docRtfReadFootnote(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr );

extern int docRtfBkmkStart(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr );

extern int docRtfBkmkEnd(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr );

extern DocumentField * docRtfSpecialField(
				int			fieldKind,
				const char *		fieldinst,
				int			fieldsize,
				const char *		fieldRslt,
				RtfReader *		rr );

extern int docRtfTextSpecialToField(	const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfRememberListOverrideLevelProperty(
					const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfRememberListOverrideProperty(
					const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfListOverrideTable(	const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfRememberTabStopProperty(
				const RtfControlWord *		rcw,
				int				arg,
				RtfReader *			rr );

extern int docRtfFontProperty(	const RtfControlWord *		rcw,
				int				arg,
				RtfReader *			rr );

extern int docRtfObjectProperty(	const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfPictureProperty(	const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfTextSpecialChar(	const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfTextUnicode(		const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfTextSpecialParticule(	const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfTextBidiMark(		const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfDrawingObjectProperty(	const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfDrawingObjectCoordinate(
					const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfShpProperty(		const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfPnProperty(		const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfHierarchy(		const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfReadDocumentTree(	const RtfControlWord *	rcw,
					DocumentTree *		dt,
					int *			pExtItKind,
					RtfReader *		rr,
					int			ignoreEmpty,
					const SelectionScope *	ss );

extern int docRtfShapeProperty(		const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );
extern int docRtfShapePropertyName(	const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );
extern int docRtfShapePropertyValue(	const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfReadRowProperties(	const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfLookupWord(		const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfColorComp(		const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfColorIgnored(		const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfCommitDocPropText(	const RtfControlWord *	rcw,
					RtfReader *		rr );

extern int docRtfDocTimeGroup(		const RtfControlWord *  rcw,
					int			arg,
					RtfReader *		rr );

extern void docRtfRefreshTextShading(	RtfReader *		rr,
					RtfReadingState *	rrs );

extern int docRtfRefreshParagraphProperties(	BufferDocument *	bd,
						RtfReadingState *	rrs );

extern int docRtfSetParaProperties(	ParagraphProperties *	pp,
					BufferDocument *	bd,
					int			mindTable,
					RtfReadingState *	rrs,
					int			breakOverride );

extern int docRtfAdaptToParaProperties(	struct BufferItem *	paraNode,
					BufferDocument *	bd,
					RtfReadingState *	rrs,
					int			breakOverride );

extern void docRtfSetForRow(		struct BufferItem *	node );

extern void docRtfResetParagraphProperties(
					RtfReadingState *	rrs );

extern void docRtfResetTextAttribute(	RtfReadingState *	rrs,
					BufferDocument *	bd );

extern int docRtfSkipPn(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr );

extern int docRtfReadPnseclvl(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr );

extern int docRtfRememberFieldProperty(	const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfRememberNoteProperty(	const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfApplyDocEncodedTextGroup(
					const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfApplyRawBytesGroup(	const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfReadTimeField(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr );

extern struct BufferItem *	docRtfGetParaNode(	RtfReader *	rr );
extern struct BufferItem *	docRtfGetSectNode(	RtfReader *	rr );

extern int docRtfFinishCurrentNode(	const RtfControlWord *	rcw,
					RtfReader *		rr );

extern void docRtfRestartFont(			RtfReader *	rr );

extern int docRtfReadMapFont(	const RtfReader *		rr,
				int *				pDocFontNum,
				int *				pCharset,
				int				fileFontNum );

extern int docRtfRememberTextShadingProperty(
				const RtfControlWord *		rcw,
				int				arg,
				RtfReader *			rr );

extern int docRtfRememberParaShadingProperty(
					const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfRememberCellShadingProperty(
					const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfRememberRowShadingProperty(
					const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfReadGetBorderNumber(	RtfReader *		rr );

extern int docRtfReadListLevelGroup(	const RtfControlWord *	rcw,
					RtfReader *		rr );

extern int docRtfMemoryBufferSetText(
				MemoryBuffer *		mb,
				RtfReader *		rr,
				int			removeSemicolon );

extern int docRtfMemoryBufferAppendText(
				MemoryBuffer *		mb,
				RtfReader *		rr );

extern int docRtfSelectionScopeProperty(	const RtfControlWord *	rcw,
						int			arg,
						RtfReader *		rr );

extern int docRtfEditRangeProperty(		const RtfControlWord *	rcw,
						int			arg,
						RtfReader *		rr );

extern int docRtfSelectionOpen(		const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfStoreStyleProperties(	RtfReader *		rr );

extern int docRtfRememberList(	const RtfControlWord *		rcw,
				int				arg,
				RtfReader *			rr );

extern int docRtfRememberListLevel(	const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfCommitListLevelText(	const RtfControlWord *	rcw,
					RtfReader *		rr );
extern int docRtfCommitListLevelNumbers( const RtfControlWord *	rcw,
					RtfReader *		rr );
extern int docRtfCommitListName(	const RtfControlWord *	rcw,
					RtfReader *		rr );
extern int docRtfCommitListStyleName(	const RtfControlWord *	rcw,
					RtfReader *		rr );

extern void docRtfResetCellProperties(	RtfReader *		rr );

extern void docRtfReadSetupTextConverters(	RtfReader *	rr );

extern RtfReader * docRtfOpenReader(		SimpleInputStream *	sis,
						BufferDocument *	bd,
						int			flags );

extern void docRtfCloseReader(			RtfReader *		rr );

#   endif	/*	RTF_READER_IMPL_H	*/
