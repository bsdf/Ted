/************************************************************************/
/*									*/
/*  Document Layout: The frame in which a series of paragraps and	*/
/*  table rows is to be layed out on the page.				*/
/*									*/
/************************************************************************/

#   ifndef	DOC_STRIP_FRAME_H
#   define	DOC_STRIP_FRAME_H

#   include	<geo2DInteger.h>

/************************************************************************/
/*									*/
/*  The frame in which a series of successive lines in a paragraph is	*/
/*  is formatted. The frame lies on one page.				*/
/*									*/
/*  pfContentRect: The rectangle in which text lines are fitted. The	*/
/*	first line potentially has a different rectangle (x0). With a	*/
/*	negative first line indent, the first line protrudes outside	*/
/*	pfContentRect to the left. [This is what CCS2 calls the content	*/
/*	rectangle of a box. I think.]					*/
/*  pfParentRect: The rectangle determined by the parent block or table	*/
/*	column. Indents are subtracted from this rectangle. Tabs are	*/
/*	relative to its left side.					*/
/*									*/
/*  Coordinates are absolute coordinates on the page or on the window.	*/
/*  [On the window they are shifted by the scrollbars.]			*/
/*									*/
/************************************************************************/

typedef struct ParagraphFrame
    {
    DocumentRectangle	pfParaContentRect;
    DocumentRectangle	pfCellContentRect;
    DocumentRectangle	pfCellRect;

    int			pfRedrawX0Twips;
    int			pfRedrawX1Twips;
    } ParagraphFrame;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docInitParagraphFrame(	ParagraphFrame *	pf );

#   endif /*	DOC_STRIP_FRAME_H */
