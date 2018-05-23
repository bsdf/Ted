/************************************************************************/
/*									*/
/*  Text Editor A line of text.						*/
/*									*/
/************************************************************************/

#   ifndef	DOC_TEXT_LINE_H
#   define	DOC_TEXT_LINE_H

#   include	"docLayoutPosition.h"

/************************************************************************/
/*									*/
/*  Used to lay out the text on the page.				*/
/*  A text line consists of a series of particules.			*/
/*									*/
/*  1)  What part of the string in the paragraph.			*/
/*  2)  What particules.						*/
/*  3)  Where is it geo/typographically?				*/
/*  4)  Postscript geometry information.				*/
/*	tlAscY0 is negative, tlDescY1 is positive. Both are relative to	*/
/*	the base line of the line.					*/
/*  5)  Screen geometry.						*/
/*									*/
/*  6)  The width of the column for which the line was last formatted.	*/
/*	When the line is shifted to a column with the same width, it	*/
/*	is sufficient to change the coordinates of the line and its	*/
/*	contents during a reformat of the document. When the column	*/
/*	width is diffrerent, the line must be reformatted. (and most	*/
/*	probably subsequent lines as well).				*/
/*	NOTE that a short int cannot hold the width of A0 paper in	*/
/*	landscape orientation.						*/
/*  7)  The indent of the first particule in the line relative to the	*/
/*	pf.pfContentRect that is avialable in the formatter and in the	*/
/*	rendering routines.						*/
/*	NOTE that a short int cannot hold the width of A0 paper in	*/
/*	landscape orientation.						*/
/*									*/
/************************************************************************/

typedef struct TextLine
    {
								/*  1,2	*/
    int			tlStroff;
    int			tlFirstParticule;
    short int		tlStrlen;
    short int		tlParticuleCount;
    short int		tlWordCount;
								/*  3  */
    LayoutPosition	tlTopPosition;
								/*  4  */
    short int		tlAscY0;
    short int		tlDescY1;
    short int		tlLineStride;
								/*  5  */
    unsigned char	tlFlags;
#			define TLflagBLOCKBREAK		0x01
/*			define TLflagLINEBREAK		0x02 */
#			define TLflagINLINECONTENT	0x04
#			define TLflagSHADING		0x08
#			define TLflagBORDER		0x10

    unsigned short int	tlFlowWidthTwips;			/*  6  */
    short int		tlLineIndent;				/*  7  */
    } TextLine;

/************************************************************************/

# define TL_LEADING( tl ) ( (tl)->tlLineStride- (tl)->tlDescY1+ (tl)->tlAscY0 )
# define TL_BASELINE( tl ) ( -(tl)->tlAscY0+ TL_LEADING( tl ) )

/************************************************************************/

# define docInvalidateTextLine( tl ) (tl)->tlFlowWidthTwips= 0

/************************************************************************/

extern void docInitTextLine(		TextLine *	tl );

#   endif
