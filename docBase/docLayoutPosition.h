#   ifndef		DOC_LAYOUT_POSITION_H
#   define		DOC_LAYOUT_POSITION_H

#   include	<geo2DInteger.h>

/************************************************************************/
/*									*/
/*  Used to layout lines of text.					*/
/*									*/
/*  A0 paper is 4* 29.7 cm high: 20* 28.3465* 4* 29.7= 67351.3 Twips.	*/
/*  So an unsigned short limits us to A1 paper.				*/
/*									*/
/************************************************************************/

typedef struct LayoutPosition
    {
    int			lpPageYTwips;
    unsigned short	lpPage;
    unsigned char	lpColumn;
    unsigned char	lpAtTopOfColumn;
    } LayoutPosition;

typedef struct BlockOrigin
    {
    int			boXShift;
    int			boYShift;
    LayoutPosition	boFrameOverride;
    int			boOverrideFrame;
    } BlockOrigin;

# define DOC_SAME_FRAME( lp1, lp2 ) ( \
		    (lp1)->lpPage == (lp2)->lpPage		&& \
		    (lp1)->lpColumn == (lp2)->lpColumn		)

# define DOC_SAME_POSITION( lp1, lp2 ) ( \
		    (lp1)->lpPageYTwips == (lp2)->lpPageYTwips	&& \
		    (lp1)->lpPage == (lp2)->lpPage		&& \
		    (lp1)->lpColumn == (lp2)->lpColumn		)

# define DOC_COLUMN_AFTER( lp1, lp2 ) ( \
		    (lp1)->lpPage > (lp2)->lpPage		|| \
		    ( (lp1)->lpPage == (lp2)->lpPage && \
		      (lp1)->lpColumn >  (lp2)->lpColumn )		)

#   define LPOSDEB(lp) appDebug( "%s(%3d): %s= [%2d.%d:%4d%c]\n",	\
			    __FILE__, __LINE__, #lp,		\
			    (lp)->lpPage, (lp)->lpColumn,	\
			    (lp)->lpPageYTwips,			\
			    (lp)->lpAtTopOfColumn?'^':'_' );

/************************************************************************/
/*									*/
/*  Routine Declarations.						*/
/*									*/
/************************************************************************/

extern void docInitLayoutPosition(	LayoutPosition *	lp );
extern void docInitBlockOrigin(		BlockOrigin *		bo );

extern void docLayoutPushBottomDown(	LayoutPosition *	lpRowBottom,
					const LayoutPosition *	lpColBottom );

extern void docLayoutPushBottomDownShifted(
					LayoutPosition *	lpRowBottom,
					const LayoutPosition *	lpColBottom,
					const BlockOrigin *	bo );

extern void docShiftPosition(		LayoutPosition *	to,
					const BlockOrigin *	bo,
					const LayoutPosition *	from );

extern int docCompareLayoutPositions(	const LayoutPosition *		lp1,
					const LayoutPosition *		lp2 );

extern void docShiftRectangle(	DocumentRectangle *		to,
				const BlockOrigin *		bo,
				const DocumentRectangle *	from );

#   endif	/*	DOC_LAYOUT_POSITION_H	*/
