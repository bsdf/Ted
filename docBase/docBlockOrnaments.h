/************************************************************************/
/*									*/
/*  Ornaments of a block as it is drawn.				*/
/*									*/
/************************************************************************/

#   ifndef	DOC_BLOCK_ORNAMENTS_H
#   define	DOC_BLOCK_ORNAMENTS_H

#   include	<utilPropMask.h>
#   include	"docBorderProperties.h"
#   include	"docItemShading.h"

typedef enum DrawOrnament
    {
    ORNdrawSHADE= 0,

    ORNdrawTOP_BORDER,
    ORNdrawLEFT_BORDER,
    ORNdrawRIGHT_BORDER,
    ORNdrawBOTTOM_BORDER,

    ORNdrawTOP_GRID,
    ORNdrawLEFT_GRID,
    ORNdrawRIGHT_GRID,
    ORNdrawBOTTOM_GRID,

    ORNdraw_COUNT
    } DrawOrnament;

typedef struct BlockOrnaments
    {
    PropertyMask	boPropMask;	/*  Flags from DrawOrnament	*/

    int			boTopBorderNumber;
    int			boLeftBorderNumber;
    int			boRightBorderNumber;
    int			boBottomBorderNumber;

    BorderProperties	boTopBorder;
    BorderProperties	boLeftBorder;
    BorderProperties	boRightBorder;
    BorderProperties	boBottomBorder;
    ItemShading		boShading;
    } BlockOrnaments;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docInitBlockOrnaments(	BlockOrnaments *	bo );

#   endif	/*  DOC_BLOCK_ORNAMENTS_H  */
