/************************************************************************/
/*									*/
/*  Document Layout and Printing related functionality.			*/
/*									*/
/************************************************************************/

#   ifndef	DOC_TEXT_RUN_H
#   define	DOC_TEXT_RUN_H

#   include	<docBuf.h>
#   include	<docEditRange.h>
#   include	<utilIndexMapping.h>

/************************************************************************/
/*									*/
/*  A run of text that can be drawn with a singe 'draw string' call.	*/
/*  The current implementation delimits a run with the same attributes.	*/
/*  A better implementation would look for runs with color and font in	*/
/*  the PostScript sense. (Bold is a different font) But in to minimise	*/
/*  the number of calls in the future, we separate the administration	*/
/*  of underline, strikethrough, shading and borders from the approach:	*/
/*  A drawin routine must handle them separately.			*/
/*									*/
/*  Just to make sure: A text run (as opposed to a span) is confined to	*/
/*  a single line in the formatted text.				*/
/*									*/
/************************************************************************/

typedef struct TextRun
    {
    int			trX0Twips;
    int			trX1Twips;
    int			trStroff;
    int			trStrlenNb;
    int			trPartFrom;
    int			trPartUpto;
    TextAttribute	trTextAttribute;
    int			trTextAttributeNr;
    unsigned char	trHasUnderline;
    unsigned char	trHasStrikethrough;
    } TextRun;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int docLayoutDelimitRun( TextRun *			tr,
				int				x0Twips,
				BufferDocument *		bd,
				const struct BufferItem *	paraNode,
				int				part,
				int				upto,
				int				separate );

extern int docNextUnderlinedRun( int *				pHead,
				int *				pPast,
				int *				pTextAttrNr,
				TextAttribute *			taLine,
				BufferDocument *		bd,
				const struct BufferItem *	paraNode,
				int				part,
				int				upto );

extern int docNextStrikethroughRun(
				int *				pHead,
				int *				pPast,
				int *				pTextAttrNr,
				TextAttribute *			taLine,
				BufferDocument *		bd,
				const struct BufferItem *	paraNode,
				int				part,
				int				upto );

#   endif	/*  DOC_TEXT_RUN_H  */
