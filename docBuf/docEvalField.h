/************************************************************************/
/*									*/
/*  Declare field evaluation routines.					*/
/*									*/
/************************************************************************/

#   ifndef	DOC_EVAL_FIELD_H
#   define	DOC_EVAL_FIELD_H

#   include	"docBuf.h"
#   include	"docNoteProperties.h"

struct BufferItem;
struct DocumentField;
struct RecalculateFields;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int docCalculateChftnFieldString(
			int *					pCalculated,
			MemoryBuffer *				mbResult,
			const struct DocumentField *		df,
			const struct RecalculateFields *	rf );

extern int docCalculateSymbolFieldString(
			int *					pCalculated,
			MemoryBuffer *				mbResult,
			const struct DocumentField *		df,
			const struct RecalculateFields *	rf );

extern int docCalculatePagerefFieldString(
			int *					pCalculated,
			MemoryBuffer *				mbResult,
			const struct DocumentField *		df,
			const struct RecalculateFields *	rf );

extern int docCalculateSectionFieldString(
			int *					pCalculated,
			MemoryBuffer *				mbResult,
			const struct DocumentField *		df,
			const struct RecalculateFields *	rf );

extern int docCalculateSeqFieldString(
			int *					pCalculated,
			MemoryBuffer *				mbResult,
			const struct DocumentField *		df,
			const struct RecalculateFields *	rf );

extern int docCalculateSectionPagesFieldString(
			int *					pCalculated,
			MemoryBuffer *				mbResult,
			const struct DocumentField *		df,
			const struct RecalculateFields *	rf );

extern int docCalculatePageFieldString(
			int *					pCalculated,
			MemoryBuffer *				mbResult,
			const struct DocumentField *		df,
			const struct RecalculateFields *	rf );

extern int docCalculateNumpagesFieldString(
			int *					pCalculated,
			MemoryBuffer *				mbResult,
			const struct DocumentField *		df,
			const struct RecalculateFields *	rf );

extern int docCalculateDocDateFieldString(
			int *					pCalculated,
			MemoryBuffer *				mbResult,
			const struct DocumentField *		df,
			const struct RecalculateFields *	rf );

extern int docCalculateDocStringFieldString(
			int *					pCalculated,
			MemoryBuffer *				mbResult,
			const struct DocumentField *		df,
			const struct RecalculateFields *	rf );

extern int docCalculateFilenameFieldString(
			int *					pCalculated,
			MemoryBuffer *				mbResult,
			const struct DocumentField *		df,
			const struct RecalculateFields *	rf );

extern int docCalculateRefFieldString(
			int *					pCalculated,
			MemoryBuffer *				mbResult,
			const struct DocumentField *		df,
			const struct RecalculateFields *	rf );

extern int docRecalculateHyperlinkField(
			int *					pCalculated,
			int *					pPartShift,
			int *					pStroffShift,
			struct BufferItem *			paraNode,
			int					part,
			int					partCount,
			struct DocumentField *			df,
			const struct RecalculateFields *	rf );

extern int docRecalculateMergeField(
			int *					pCalculated,
			int *					pPartShift,
			int *					pStroffShift,
			struct BufferItem *			paraNode,
			int					part,
			int					partCount,
			struct DocumentField *			df,
			const struct RecalculateFields *	rf );

extern int docRecalculateIncludePictureField(
			int *					pCalculated,
			int *					pPartShift,
			int *					pStroffShift,
			struct BufferItem *			paraNode,
			int					part,
			int					partCount,
			struct DocumentField *			df,
			const struct RecalculateFields *	rf );

extern int docRecalculateParaStringTextParticules(
			int *					pCalculated,
			int *					pPartShift,
			int *					pStroffShift,
			struct BufferItem *			paraNode,
			int					part,
			int					partCount,
			struct DocumentField *			df,
			const struct RecalculateFields *	rf );

extern int docRecalculateParaChftnTextParticules(
			int *					pCalculated,
			int *					pPartShift,
			int *					pStroffShift,
			struct BufferItem *			paraNode,
			int					part,
			int					partCount,
			struct DocumentField *			df,
			const struct RecalculateFields *	rf );

extern int docRecalculateParaListtextTextParticules(
			int *					pCalculated,
			int *					pPartShift,
			int *					pStroffShift,
			struct BufferItem *			paraNode,
			int					part,
			int					partCount,
			struct DocumentField *			df,
			const struct RecalculateFields *	rf );

extern int docRecalculateParaSymbolTextParticules(
			int *					pCalculated,
			int *					pPartShift,
			int *					pStroffShift,
			struct BufferItem *			paraNode,
			int					part,
			int					partCount,
			struct DocumentField *			df,
			const struct RecalculateFields *	rf );

extern int docCalculateRefFieldValue(
			MemoryBuffer *				mbResult,
			const MemoryBuffer *			markName,
			const struct BufferDocument *		bd );

extern int docFormatChftnField(
			MemoryBuffer *				mbResult,
			const FootEndNotesProperties *		fep,
			const NoteProperties *			np,
			int					noteNumber );

#   endif	/*  DOC_EVAL_FIELD_H  */
