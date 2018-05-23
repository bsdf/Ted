#   ifndef		DOC_SELECTION_GEOMETRY_H
#   define		DOC_SELECTION_GEOMETRY_H

#   include		<docLayoutPosition.h>

/*  2  */
typedef struct PositionGeometry
    {
    int			pgLine;
    int			pgPositionFlags;

    int			pgXTwips;
    int			pgXPixels;
    LayoutPosition	pgTopPosition;
    LayoutPosition	pgBasePosition;
    LayoutPosition	pgBottomPosition;

    int			pgParaFrameX0;
    int			pgParaFrameX1;
    int			pgBlockFrameX0;
    int			pgBlockFrameX1;
    } PositionGeometry;

typedef struct SelectionGeometry
    {
    DocumentRectangle	sgRectangle;

    PositionGeometry	sgHead;
    PositionGeometry	sgTail;
    PositionGeometry	sgAnchor;
    } SelectionGeometry;

/************************************************************************/
/*									*/
/*  Routine Declarations.						*/
/*									*/
/************************************************************************/

extern void docInitPositionGeometry(	PositionGeometry *	pg );
extern void docInitSelectionGeometry(	SelectionGeometry *	sg );


#   endif	/*	DOC_SELECTION_GEOMETRY_H	*/
