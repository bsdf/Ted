/************************************************************************/
/*									*/
/*  Fields: parts of the document that are calculated in stead of typed	*/
/*  by the user.							*/
/*									*/
/************************************************************************/

#   ifndef DOC_FIELD_H
#   define DOC_FIELD_H

#   include	<utilMemoryBuffer.h>
#   include	<docDocumentField.h>
#   include	<docEditPosition.h>
#   include	<textAttribute.h>
#   include	<utilPropMask.h>

struct BufferItem;
struct BufferDocument;
struct DocumentSelection;
struct DocumentPosition;
struct RecalculateFields;

/************************************************************************/
/*									*/
/*  Kind of field. Is an index in a descriptive array of field kinds.	*/
/*									*/
/*  Fields in the 'Ted' sense are not necessarily fields in the 'Word'	*/
/*  sense. Everything inside the text, that can possibly result in	*/
/*  a piece of document, but is possibly under the control of the	*/
/*  application is a field in the sense of 'Ted'.			*/
/*									*/
/*  FieldKindInformation:						*/
/*  a)  The (case insensitive) string that identifies the field in the	*/
/*	field instructions. NOTE that the notion of a field is abused	*/
/*	by 'Ted' for a few things that are not fields in MS-Word.	*/
/*  b)  Some things that are a destination in RTF are stored as a field	*/
/*	in Ted. This is the control word of the destination.		*/
/*  c)  The maximum level in the block hierarchy that certainly holds	*/
/*	the result/content of this kind of field.			*/
/*  d)  Tells whether this kind of field is a field in the RTF file.	*/
/*  e)  Tells whether this kind of field is a destination in the RTF	*/
/*	file.								*/
/*  f1) Recalculate a TEXT level field.					*/
/*	The standard workhorse docRecalculateParaStringTextParticules()	*/
/*	can be used for all substitutions that simply return a string.	*/
/*	In its turn it uses fki->fkiCalculateTextString() to obtain	*/
/*	the string. Other particule avluators might decide to do	*/
/*	something similar.						*/
/*  f2) Recalculate the string for a TEXT level field.			*/
/*  g)  When to recalculate fields in the text.				*/
/*  h)  Is the field result editable such as with a HYPERLINK or	*/
/*	readonly such as with a PAGEREF?				*/
/*  i)  Is a field if this kind constrained to a single paragraph.	*/
/*									*/
/************************************************************************/

/*  f1  */
typedef int (*CALCULATE_TEXT_PARTICULES)(
			int *					pCalculated,
			int *					pPartShift,
			int *					pStroffShift,
			struct BufferItem *			paraBi,
			int					part,
			int					partCount,
			struct DocumentField *			df,
			const struct RecalculateFields *	rf );

/*  f2  */
typedef int (*CALCULATE_TEXT_STRING)(
			int *					pCalculated,
			MemoryBuffer *				mbResult,
			const struct DocumentField *		df,
			const struct RecalculateFields *	rf );

/*  g  */
#   define	FIELDdoNOTHING		0
#   define	FIELDdoNEVER		0
#   define	FIELDdoDOC_FORMATTED	(1<<0)
#   define	FIELDdoPAGE_NUMBER	(1<<1)
#   define	FIELDdoDOC_INFO		(1<<2)
#   define	FIELDdoDOC_COMPLETE	(1<<3)
#   define	FIELDdoCHFTN		(1<<4)
#   define	FIELDdoLISTTEXT		(1<<5)
#   define	FIELDdoSEQ		(1<<6)
#   define	FIELDdoTOC		(1<<7) /* Have their own call */

typedef struct FieldKindInformation
    {
    const char *		fkiLabel;			/*  a	*/
    int				fkiLevel;			/*  c	*/
    int				fkiIsFieldInRtf;		/*  d	*/
    int				fkiIsDestinationInRtf;		/*  e	*/
    CALCULATE_TEXT_PARTICULES	fkiCalculateTextParticules;	/*  f1	*/
    CALCULATE_TEXT_STRING	fkiCalculateTextString;		/*  f2	*/
    unsigned int		fkiCalculateWhen;		/*  g	*/
    unsigned int		fkiResultEditable;		/*  h	*/
    unsigned int		fkiSingleParagraph;		/*  i	*/
    const char *		fkiRtfTag;			/*  b	*/
    } FieldKindInformation;

/************************************************************************/
/*									*/
/*  Field Kind Information.						*/
/*									*/
/************************************************************************/

extern const FieldKindInformation	DOC_FieldKinds[];
extern const int			DOC_FieldKindCount;

/************************************************************************/
/*									*/
/*  Administrative routines.						*/
/*									*/
/************************************************************************/

extern int docSuggestNewBookmarkName(
			    MemoryBuffer *			markName,
			    const struct BufferDocument *	bd,
			    const struct DocumentSelection *	ds );

extern int docMakeBookmarkUnique(
			    const struct BufferDocument *	bd,
			    MemoryBuffer *			markName );

extern int docFieldKindFromInstructions(	const DocumentField *	df );

extern DocumentField * docFindFieldForPosition(
				    struct BufferDocument *		bd,
				    const struct DocumentPosition *	dp );

extern DocumentField * docFindTypedFieldForPosition(
			    struct BufferDocument *		bd,
			    const struct DocumentPosition *	dp,
			    int					kind,
			    int					lastOne );

extern DocumentField * docFindTypedFieldInSelection(
			    struct BufferDocument *		bd,
			    const struct DocumentSelection *	ds,
			    int					kind,
			    int					lastOne );

extern int docSetHyperlinkAttribute(	TextAttribute *		taSet,
					PropertyMask *		taSetMask,
					struct BufferDocument *	bd );

extern int docRemoveHyperlinkAttribute(	TextAttribute *		taSet,
					PropertyMask *		taSetMask,
					struct BufferDocument *	bd );

#   endif /*  DOC_FIELD_H  */
