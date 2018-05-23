/************************************************************************/
/*									*/
/*  Writing RTF.							*/
/*									*/
/************************************************************************/

#   ifndef		RTF_WRITER_IMPL_H
#   define		RTF_WRITER_IMPL_H

#   include	<docBuf.h>
#   include	<sioGeneral.h>
#   include	"docRtfWriter.h"

struct TabStopList;

/************************************************************************/
/*									*/
/*  The writer.								*/
/*									*/
/************************************************************************/

typedef struct PushedAttribute
    {
    struct PushedAttribute *	paNext;
    TextAttribute		paTextAttribute;
    int				paTextCharset;
    } PushedAttribute;

struct RtfWriter
    {
    struct BufferDocument *	rwDocument;
				/**
				 *  The current text attribute. The state of 
				 *  the writer is also determined by the charset
				 *  of the current text, as the RTF file format 
				 *  includes the charset of the text in the 
				 *  identity of the font.
				 */
    TextAttribute		rwTextAttribute;
				/**
				 *  The rtf charset of the current text attribute.
				 *  This is needed as the RTF file format 
				 *  represents text with 8-bit characters belonging
				 *  to different fonts if there are too many 
				 *  different characters in the font.
				 */
    int				rwTextCharset;
    ParagraphProperties		rwcParagraphProperties;
    RowProperties		rwRowProperties;

    ParagraphProperties		rwcOutsideTableParagraphProperties;

    PagedList			rwcEncodedFontList;

    PushedAttribute *		rwcPushedAttribute;
    int				rwcInFldrslt;
				/**
				  *  The current depth of table nesting:
				  *  0: outside tables, 1: In table,
				  *  2+: in a nested table.
				  */
    int				rwTableNesting;
				/**
				  *  The deepest depth of table nesting:
				  *  If the value >1, Ted will emit 
				  *  itap tags along with intbl tags.
				  *  This is because itap tags confuse wordpad.
				  */
    int				rwDeepestTableNesting;

				/**
				  *  While writing a selection, it is not 
				  *  obvious that the section properties 
				  *  are saved: If the selection is contained 
				  *  in a single child of the section, we 
				  *  start in that child. So we never actually 
				  *  enter the section node.
				  */
    int				rwSectionPropertiesSaved;

    int				rwcAfter;
#				define RTFafterTAG	'T'
#				define RTFafterARG	'0'
#				define RTFafterTEXT	'*'
    int				rwcLastNodeLevel;

    int				rwSaveFlags;

    int				rwCol;
    SimpleOutputStream *	rwSosOut;

				/**
				 * Used for RTF conversions. I.E where 
				 * no font applies
				 */
    struct TextConverter *	rwRtfTextConverter;
				/**
				 * Used for Text conversions. I.E where 
				 * a font applies
				 */
    struct TextConverter *	rwTextTextConverter;

    const PropertyMask *	rwPpExtraMask;
    const PropertyMask *	rwCpExtraMask;
    const PropertyMask *	rwRpExtraMask;
    const PropertyMask *	rwSpExtraMask;
    };

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int docRtfWriteArgTag(		RtfWriter *		rwc,
					const char *		tag,
					int			arg );

extern void docRtfWriteFlagTag(		RtfWriter *		rwc,
					const char *		tag,
					int			arg );

extern void docRtfWriteAltTag(		RtfWriter *		rwc,
					const char *		yesTag,
					const char *		noTag,
					int			arg );

extern void docRtfWriteEnumTag(		RtfWriter *		rwc,
					const char * const *	tags,
					int			arg,
					int			tagCount,
					int			enumCount );

extern int docRtfWriteTag(		RtfWriter *		rwc,
					const char *		tag );

extern void docRtfWriteDocEncodedString(RtfWriter *		rwc,
					const char *		s,
					int			n );

extern void docRtfWriteFontEncodedString( RtfWriter *	rwc,
					const char *		s,
					int			n );

extern void docRtfWriteRawBytes(	RtfWriter *	rwc,
					const char *		s,
					int			n );

extern void docRtfWriteDocEncodedStringDestination(
					RtfWriter *		rwc,
					const char *		tag,
					const char *		s,
					int			n,
					int			addSemicolon );

extern void docRtfWriteRawBytesDestination(
					RtfWriter *		rwc,
					const char *		tag,
					const char *		s,
					int			n );

extern void docRtfWriteNextLine(	RtfWriter *		rwc );

extern void docRtfSaveBorderByNumber(	RtfWriter *			rwc,
					const char *			tag,
					int				num,
					int				anywy );

extern void docRtfSaveTextAttribute(	RtfWriter *		rwc,
					const PropertyMask *	updMask,
					const TextAttribute *	ta );

extern void docRtfSaveParagraphProperties(
				RtfWriter *			rwc,
				const PropertyMask *		updMask,
				const ParagraphProperties *	pp );

extern int docRtfWriteDestinationBegin( RtfWriter *		rwc,
					const char *		tag );

extern int docRtfWriteDestinationEnd(	RtfWriter *		rwc );

extern int docRtfSavePictureTags(
				RtfWriter *			rwc,
				const PropertyMask *		pipSetMask,
				const PictureProperties *	pipSet );

extern int docRtfPicturePropertyMask(
				PropertyMask *			pipSetMask,
				const PictureProperties *	pipSet );

extern int docRtfWriteMemoryBuffer(	RtfWriter *			rwc,
					const MemoryBuffer *		mb );

extern void docRtfSaveSectionProperties( RtfWriter *		rwc,
				const PropertyMask *		updMask,
				const SectionProperties *	sp );

extern int docRtfSaveSectionPropertiesOfNode(
				RtfWriter *			rwc,
				const DocumentSelection *	ds,
				const struct BufferItem *	sectNode );

extern int docRtfWriteGetDefaultFont(
				RtfWriter *			rw,
				int				deff );

extern void docRtfWriteFontTable( RtfWriter *			rwc );

extern int docRtfWriteColorTable( RtfWriter *			rwc,
				const DocumentProperties *	dp );

extern void docRtfWriteStyleSheet(
				RtfWriter *			rwc,
				const DocumentStyleSheet *	dss );

extern int docRtfWriteArgDestinationBegin( RtfWriter *		rwc,
					const char *		tag,
					int			arg );

extern int docRtfSaveDocumentProperties( RtfWriter *			rwc,
					int				fet,
					const PropertyMask *		dpMask,
					const DocumentProperties *	dp );

extern int docRtfSaveNotesProperties(
		RtfWriter *			rw,
		const PropertyMask *		mask,
		const NotesProperties *		np,
		const int			propMap[NOTESprop_COUNT],
		const char *			startNrTag,
		const char * const *		justificationTags,
		int				justificationTagCount,
		const char * const *		placementTags,
		int				placementTagCount,
		const char * const *		restartTags,
		int				restartTagCount,
		const char * const *		styleTags,
		int				styleTagCount );

extern void docRtfSaveParaFrameProperties(
				RtfWriter *			rwc,
				const FrameProperties *		fp );
extern void docRtfSaveRowFrameProperties(
				RtfWriter *			rwc,
				const FrameProperties *		fp );

extern int docRtfSaveSectHeadersFooters( RtfWriter *		rwc,
					const struct BufferItem *	bi );

extern int docRtfSaveHeaderFooter(	RtfWriter *		rwc,
					const DocumentTree *	dt );

extern int docRtfSaveDocumentTree(	RtfWriter *		rwc,
					const char *		tag,
					const DocumentTree *	dt,
					int			evenIfAbsent,
					int			forcePar );

extern int docRtfSaveDocNotesSeparators( RtfWriter *		rwc,
					const BufferDocument *	bd );

extern void docRtfSaveCellProperties(	RtfWriter *		rwc,
					const PropertyMask *	cpSetMask,
					const CellProperties *	cpSet,
					int			shiftLeft );

extern void docRtfSaveRowProperties(	RtfWriter *		rwc,
					const PropertyMask *	rpSetMask,
					const RowProperties *	rpSet,
					int			col0,
					int			col1 );

extern void docRtfSaveAutoSpace(	RtfWriter *		rwc,
					const char *		unitTag,
					int			unit,
					const char *		numberTag,
					int			number );

extern int docRtfWriteListProps(	RtfWriter *			rw,
					const struct DocumentList *	dl );

extern void docRtfWriteListTable(
				RtfWriter *			rwc,
				const DocumentListTable *	dlt );

extern void docRtfWriteListOverrideTable(
				RtfWriter *			rwc,
				const ListOverrideTable *	lot );

extern void docRtfWriteListLevel(
				RtfWriter *			rwc,
				const struct ListLevel *	ll );

extern void docRtfSaveTabStopList(
				RtfWriter *			rwc,
				const struct TabStopList *	tsl );

extern void docRtfSaveShadingByNumber(	RtfWriter *		rwc,
					int			num,
					const char * const *	patTags,
					int			patTagCount,
					const char *		foreTag,
					const char *		backTag,
					const char *		levelTag );

extern void docRtfReserveColumns(	RtfWriter *		rwc,
					int			cols );

extern int docRtfSaveParaNode(	RtfWriter *			rwc,
				const struct BufferItem *	bi,
				const DocumentSelection *	ds,
				const int			flattenCell,
				const int			firstInRow );

extern void docRtfWriteSemicolon(	RtfWriter *	rwc );

extern void docRtfSaveParaTableNesting(	RtfWriter *		rwc,
					int			nest );

extern int docRtfPushAttribute(		RtfWriter *	rwc );
extern int docRtfPopAttribute(		RtfWriter *	rwc );

extern void docRtfWriteSwitchTextAttributes(
					RtfWriter *		rwc,
					int			textAttrNr,
					const char *		first );

extern void docRtfWriteSwitchToPlain(	RtfWriter *		rwc );

extern int docRtfWriteBuildFontAdmin(	RtfWriter *		rwc );

extern int docRtfWriteGetCharset(
				RtfWriter *			rwc,
				int *				pCharset,
				const DocumentFont *		df,
				int				symbol );


extern int docRtfSaveFieldHead(		RtfWriter *		rwc,
					const struct BufferItem *	paraNode,
					int			part );

extern int docRtfSaveFieldTail(		RtfWriter *		rwc,
					const struct BufferItem *	paraNode,
					int			part );

extern int docRtfDocPropMask(	PropertyMask *			dpSaveMask,
				const DocumentProperties *	dpDoc );

extern int docRtfWriteSelection(	RtfWriter *			rw,
					const DocumentSelection *	ds );

extern int docRtfSaveDate(		RtfWriter *		rw,
					const char *		tag,
					const struct tm *	tm );

extern void docRtfWriteSetupTextConverters(	RtfWriter *	rw );

#   endif	/*	RTF_WRITER_IMPL_H	*/
