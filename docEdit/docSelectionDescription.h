#   ifndef		DOC_SELECTION_DESCRIPTION_H
#   define		DOC_SELECTION_DESCRIPTION_H

#   include		<docLayoutPosition.h>
#   include		<docSelectionScope.h>
#   include		<textAttribute.h>
#   include		<docObject.h>
#   include		<docEditPosition.h>
#   include		<docEditRange.h>
#   include		<docTableRectangle.h>
#   include		<docSelect.h>

/************************************************************************/
/*									*/
/*  Describe a selection and its relevance for application tools.	*/
/*									*/
/************************************************************************/

typedef struct SelectionDescription
    {
    unsigned int	sdDocumentId;

    unsigned char	sdIsSet;
    unsigned char	sdDocumentReadonly;
    unsigned char	sdIsIBarSelection;
    unsigned char	sdIsSingleParagraph;
    unsigned char	sdInContiguousParagraphs;
    unsigned char	sdIsSingleFrame;
    unsigned char	sdHeadInFrame;
    unsigned char	sdHeadInTable;
    unsigned char	sdTailInTable;
    unsigned char	sdHeadInTableHeader;
    unsigned char	sdHeadInMultiColumnSection;
			/**
			 *  The selection is in a single table.
			 */
    unsigned char	sdInOneTable;
			/**
			 *  The selection is a rectangular block in a single
			 *  table.
			 */
    unsigned char	sdIsTableSlice;
			/**
			 *  The selection is an object or image.
			 */
    unsigned char	sdIsObjectSelection;

    int			sdHeadPage;
    int			sdTailPage;
    int			sdDocumentPages;

    int			sdHeadSection;
    int			sdTailSection;
    int			sdDocumentSections;

    int			sdCanReplace;
    int			sdInTreeType;
    int			sdInDocumentBody;
    int			sdInHeaderFooter;
    int			sdHeaderTypeForSelection;
    int			sdFooterTypeForSelection;

    unsigned char	sdHeadInField;
    unsigned char	sdHeadInHyperlink;
    unsigned char	sdHeadInBookmark;
    unsigned char	sdHeadFieldKind;

    unsigned char	sdIsListBullet;
    unsigned char	sdHasNote;
    unsigned char	sdInNote;

    unsigned char	sdHasLists;
    int			sdFirstListParaNr;
    int			sdListOverride;
    int			sdListLevel;
    int			sdMultiList;
    int			sdMultiLevel;

			/**
			 *  The block in a table that holds the selection.
			 *  Is also set if we have a selection inside a single
			 *  cell.
			 */
    TableRectangle	sdTableRectangle;

    TextAttribute	sdTextAttribute;
    PropertyMask	sdTextAttributeMask;
    int			sdTextAttributeNumber;
    } SelectionDescription;

/************************************************************************/
/*									*/
/*  Routine Declarations.						*/
/*									*/
/************************************************************************/

extern void docInitSelectionDescription(	SelectionDescription *	sd );

extern void docDescribeSelection(
			    SelectionDescription *		sd,
			    const DocumentSelection *		ds,
			    struct BufferDocument *		bd,
			    int					headPage,
			    int					tailPage,
			    unsigned int			documentId,
			    int					documentRo );

extern void docEnableEditCommands(
				unsigned char *			cmdEnabled,
				const SelectionDescription *	sd );

#   endif	/*	DOC_SELECTION_DESCRIPTION_H	*/
