/************************************************************************/
/*									*/
/*  Text Editor Section specifics.					*/
/*									*/
/************************************************************************/

#   ifndef	DOC_SECT_NODE_H
#   define	DOC_SECT_NODE_H

#   include	<docSectProperties.h>
#   include	"docDocumentTree.h"

/************************************************************************/
/*									*/
/*  Type dependent part of a BufferItem of 'Section' level.		*/
/*									*/
/*  Section level BufferItems are the sections of the document and the	*/
/*  'Document Trees' that are not part of the regular tree of items	*/
/*  that forms the document. Examples of 'Document Trees' are the page	*/
/*  headers and footers and the footnotes and endnotes in the document.	*/
/*									*/
/*  The root of other trees than the body are recognised by the fact	*/
/*  that bi->biTreeType != DOCinBODY. There are no parents for the	*/
/*  section level items in external document trees.			*/
/*									*/
/************************************************************************/

typedef struct SectHeadersFooters
    {
				/**
				 *  Page headers.
				 */
    /* DocumentTree		shfHeader; Word 11+ uses Right */
    DocumentTree		shfFirstPageHeader;
    DocumentTree		shfLeftPageHeader;
    DocumentTree		shfRightPageHeader;

				/**
				 *  Page footers.
				 */
    /* DocumentTree		shfFooter; Word 11+ uses Right */
    DocumentTree		shfFirstPageFooter;
    DocumentTree		shfLeftPageFooter;
    DocumentTree		shfRightPageFooter;
    } SectHeadersFooters;

typedef struct SectNode
    {
				/**
				 * Formatting properties of the section.
				 */
    SectionProperties		bsProperties;
				/**
				 *  Headers and footers. (If any)
				 */
    SectHeadersFooters *	bsHeadersFooters;

				/**
				 *  The role in the document. This is most
				 *  important in the roots of the various
				 *  external trees.
				 */
    SelectionScope		bsSelectionScope;
    } SectNode;

/************************************************************************/
/*									*/
/*  Shortcut defines through the BufferItem union.			*/
/*									*/
/************************************************************************/

#   define biSectHeadersFooters		BIU.biuSect.bsHeadersFooters
#   define biSectSelectionScope		BIU.biuSect.bsSelectionScope
#   define biSectProperties		BIU.biuSect.bsProperties

#   define biSectDocumentGeometry	biSectProperties.spDocumentGeometry
#   define biSectColumnCount		biSectProperties.spColumnCount
#   define biSectHasTitlePage		biSectProperties.spHasTitlePage
#   define biSectBreakKind		biSectProperties.spBreakKind
#   define biSectPageNumberStyle	biSectProperties.spPageNumberStyle
#   define biSectPageNumberHyphen	biSectProperties.spPageNumberHyphen
#   define biSectRestartPageNumbers	biSectProperties.spRestartPageNumbers
#   define biSectStartPageNumber	biSectProperties.spStartPageNumber

#   define biSectNotesProperties	biSectProperties.spNotesProperties

#   define DOC_SECTnodeBELOW_PREVIOUS(bi) \
		    DOC_SECTpropsBELOW_PREVIOUS( &((bi)->biSectProperties) )

/************************************************************************/
/*									*/
/*  Routine dectarations.						*/
/*									*/
/************************************************************************/

extern void docCleanSectHeadersFooters(
				struct BufferDocument *		bd,
				SectHeadersFooters *		shf );

extern void docInitSectHeadersFooters(
				SectHeadersFooters *		shf );

#   endif
