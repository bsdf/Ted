/************************************************************************/
/*									*/
/*  The frame on the page inside which content is laid out.		*/
/*									*/
/************************************************************************/

#   ifndef	DOC_BLOCK_FRAME_H
#   define	DOC_BLOCK_FRAME_H

#   include	<docDocumentField.h>
#   include	"docBuf.h"

/************************************************************************/
/*									*/
/*  Frame in which the regular text on a page is formatted. This is	*/
/*  either the page or a (newspaper style) column on the page. The	*/
/*  page headers and footers get their own block frame. The same	*/
/*  for the footnotes and endnotes. The space for footnotes is however	*/
/*  subtracted from the block frame for the page.			*/
/*									*/
/*  NOTE that table _columns_ are not implemented with a block frame.	*/
/*									*/
/************************************************************************/

typedef struct NotesReservation
    {
    DocumentField *		nrDfFirstFootnote;
    int				nrFootnoteCount;
    const struct BufferItem *	nrFootnoteSectBi;

    int				nrFtnsepHeight;
    int				nrFootnoteHeight;
    } NotesReservation;

typedef struct BlockFrame
    {
    int			bfPage;
    int			bfColumn;
			    /********************************************/
			    /*  Page and (newspaper) column number of	*/
			    /*  the frame.				*/
			    /********************************************/
    DocumentRectangle	bfFlowRect;
			    /********************************************/
			    /*  Is consumed by the formatter: footnotes	*/
			    /*  are subtracted from the bottom.		*/
			    /********************************************/
    DocumentRectangle	bfContentRect;
			    /********************************************/
			    /*  Is set at the beginning and is not	*/
			    /*  changed on the way.			*/
			    /********************************************/
    DocumentGeometry	bfPageGeometry;

    int			bfFootnotesPlaced;
			    /********************************************/
			    /*  Footnotes are formatted in two phases.	*/
			    /*  The first time at an arbitrary position	*/
			    /*  on the page to determine their height.	*/
			    /*  After the page layout is known, the	*/
			    /*  notes are placed at the correct		*/
			    /*  position. bfFootnotesPlaced is set when	*/
			    /*  the notes have been placed.		*/
			    /********************************************/
    NotesReservation	bfNotesReservation;
			    /********************************************/
			    /*  Keeps track of the space at the bottom	*/
			    /*  of the page that is reserved for	*/
			    /*  footnotes.				*/
			    /********************************************/
    } BlockFrame;

#   define	BF_HAS_FOOTNOTES( bf ) \
		    ((bf)->bfNotesReservation.nrDfFirstFootnote != (DocumentField *)0)

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docLayoutInitBlockFrame(	BlockFrame *		bf );
extern void docLayoutCleanBlockFrame(	BlockFrame *		bf );
extern void docInitNotesReservation(	NotesReservation *      nr );

extern void docLayoutReserveNoteHeight(
				BlockFrame *			bf,
				const NotesReservation *	nrLine );

#   endif	/*  DOC_BLOCK_FRAME_H  */
