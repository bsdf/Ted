/************************************************************************/
/*									*/
/*  Infrastructure for setting the properties in a selection.		*/
/*									*/
/************************************************************************/

#   ifndef	DOC_EDIT_SET_PROPS_H
#   define	DOC_EDIT_SET_PROPS_H

#   include	<stdio.h>

#   include	<docBuf.h>
#   include	"docEditOperation.h"

typedef struct SetProperties
    {
    EditOperation *			spEditOperation;
    int					spRedoParaLayout;
    const DocumentAttributeMap *	spAttributeMap;
    TableRectangle			spTableRectangle;

    int					spGotPara;
    int					spGotCell;
    int					spGotRow;
    int					spGotSect;

    PropertyMask *			sp_taDoneMask;
    const PropertyMask *		sp_taSetMask;
    const TextAttribute *		sp_taSet;

    PropertyMask *			sp_ppDoneMask;
    const PropertyMask *		sp_ppSetMask;
    const ParagraphProperties *		sp_ppSet;

    PropertyMask *			sp_cpDoneMask;
    const PropertyMask *		sp_cpSetMask;
    const CellProperties *		sp_cpSet;

    PropertyMask *			sp_rpDoneMask;
    const PropertyMask *		sp_rpSetMask;
    const RowProperties *		sp_rpSet;

    PropertyMask *			sp_spDoneMask;
    const PropertyMask *		sp_spSetMask;
    const SectionProperties *		sp_spSet;
    } SetProperties;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int docEditChangeParaProperties(
				SetProperties *			setProps,
				const DocumentSelection *	ds,
				struct BufferItem *		paraNode,
				const TextAttribute *		taSet,
				const ParagraphProperties *	ppSet );

extern int docEditChangeCellProperties(
				SetProperties *			setProps,
				const DocumentSelection *	ds,
				struct BufferItem *		cellNode,
				const CellProperties *		cpSet );

extern int docEditChangeRowProperties(
				SetProperties *			setProps,
				const DocumentSelection *	ds,
				struct BufferItem *		rowNode,
				const RowProperties *		rpSet );

extern int docChangeDocumentProperties(
				EditOperation *			eo,
				PropertyMask *			docDpDoneMask,
				const PropertyMask *		dpSetMask,
				const DocumentProperties *	dpSet,
				const DocumentAttributeMap *	dam );

#    endif	/*  DOC_EDIT_SET_PROPS_H	*/
