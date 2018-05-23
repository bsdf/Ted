#   include	"docBuf.h"

#   ifndef	DOC_RECALCULATE_FIELDS_H
#   define	DOC_RECALCULATE_FIELDS_H

#   include	"docBuf.h"

/************************************************************************/
/*									*/
/*  Pass through information for recalculating fields.			*/
/*									*/
/*  MERGEFIELDs and report generation.					*/
/*									*/
/************************************************************************/

typedef int (*FieldDataProvider)(	int *		pCalculated,
					MemoryBuffer *	mbResult,
					const char *	fieldName,
					void *		through );

typedef enum StreamPosition
    {
    StreamFirstInstance= 0,
    StreamNextInstance,
    StreamReady
    } StreamPosition;

typedef int (*InstanceStreamProvider)(	int *		pCalculated,
					const char *	streamName,
					int		streamPosition,
					void *		through );

/************************************************************************/
/*									*/
/*  Pass through information for recalculating fields.			*/
/*									*/
/************************************************************************/

typedef struct RecalculateFields
    {
    struct BufferDocument *	rfDocument;
    struct DocumentTree *	rfTree;
    struct DocumentTree *	rfSelectedTree;
    void *			rfMergeValueTree;
    DOC_CLOSE_OBJECT		rfCloseObject;
    unsigned int		rfFieldsUpdated;
    unsigned int		rfUpdateFlags;
    EditPosition		rfSelHead;
    EditPosition		rfSelTail;

    const struct BufferItem *	rfBodySectNode;
    int				rfBodySectPage;

    FieldDataProvider		rfFieldDataProvider;
    InstanceStreamProvider	rfInstanceStreamProvider;
    void *			rfMergeThrough;
    } RecalculateFields;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docInitRecalculateFields(	RecalculateFields *	rf );

extern void docRenumberNotes(		int *			pChanged,
					BufferDocument *	bd );

extern int docRenumberSeqFields(	int *			pChanged,
					DocumentTree *		dt,
					BufferDocument *	bd );

extern int docRenumberSeqField( int *				pChanged,
				DocumentField *			df,
				BufferDocument *		bd );

extern int docFieldReplaceContents(
				int *				pStroff,
				int *				pStroffShift,
				int *				pTextAttrNr,
				struct BufferItem *		paraBi,
				int				part,
				int				partCount,
				int				stroffShift,
				const char *			addedString,
				int				addedStrlen,
				const RecalculateFields *	rf );

extern int docRecalculateTextLevelFields(	RecalculateFields *	rf,
						struct BufferItem *	bi );

extern int docRecalculateTextLevelFieldsInDocumentTree(
				    RecalculateFields *		rf,
				    DocumentTree *		dt,
				    const struct BufferItem *	sectBi,
				    int				page );

extern int docRecalculateFieldParticulesFromString(
				int *				pCalculated,
				int *				pPartShift,
				int *				pStroffShift,
				struct BufferItem *		paraBi,
				int				part,
				int				partCount,
				const MemoryBuffer *		mbResult,
				const RecalculateFields *	rf );

#   endif	/*  DOC_RECALCULATE_FIELDS_H  */
