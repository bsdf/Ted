/************************************************************************/
/*									*/
/*  The stylesheet of a document.					*/
/*									*/
/************************************************************************/

#   ifndef	DOC_STYLE_H
#   define	DOC_STYLE_H

#   include	<utilPropMask.h>

#   include	"docSectProperties.h"
#   include	"docRowProperties.h"
#   include	"docParaProperties.h"
#   include	"docParaNumber.h"
#   include	"docDocumentAttributeMap.h"

/************************************************************************/
/*									*/
/*  Styles in a document.						*/
/*									*/
/*  dsBusy is used to prevent recursion when styles are expanded.	*/
/*									*/
/************************************************************************/

typedef struct DocumentStyle
    {
    int			dsStyleNumber;

    int			dsBasedOn;

    int			dsNext;
    int			dsLink;
    int			dsLevel;

    unsigned char	dsAdditive;
    unsigned char	dsAutoupd;
    unsigned char	dsHidden;
    unsigned char	dsLocked;
    unsigned char	dsSemiHidden;
    unsigned char	dsPersonal;
    unsigned char	dsBusy;

			    /**
			     *  The outline level corresponding to this 
			     *  style. Used in TOC calculations and only 
			     *  set for styles in the stylesheet.
			     */
    unsigned char	dsOutlineLevel;

    SectionProperties	dsSectProps;
    RowProperties	dsRowProps;
    CellProperties	dsCellProps;
    ParagraphProperties	dsParaProps;
    TextAttribute	dsTextAttribute;

    char *		dsName;
    PropertyMask	dsSectMask;
    PropertyMask	dsRowMask;
    PropertyMask	dsCellMask;
    PropertyMask	dsParaMask;
    PropertyMask	dsTextMask;
    } DocumentStyle;

typedef enum DocumentStyleProperty
    {
    DSpropSTYLE_NUMBER= 0,
    DSpropBASED_ON,
    DSpropADDITIVE,
    DSpropNEXT,
    DSpropLINK,
    DSpropLEVEL,
    DSpropAUTO_UPD,
    DSpropHIDDEN,
    DSpropLOCKED,
    DSpropSEMI_HIDDEN,
    DSpropPERSONAL,
    DSpropNAME,

    DSpropIGNORED,

    DSprop_COUNT
    } DocumentStyleProperty;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docInitDocumentStyle(	DocumentStyle *	ds );
extern void docCleanDocumentStyle(	DocumentStyle *	ds );

extern int docCopyStyle(	DocumentStyle *			to,
				const DocumentStyle *		from,
				const DocumentAttributeMap *	dam );

extern int docSetStyleProperty(		DocumentStyle *		ds,
					int			prop,
					int			arg );

#   endif	/*  DOC_STYLE_H  */
