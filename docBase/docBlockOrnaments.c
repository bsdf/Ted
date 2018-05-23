#   include	"docBaseConfig.h"

#   include	"docBlockOrnaments.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Initialize BlockOrnaments						*/
/*									*/
/************************************************************************/

void docInitBlockOrnaments(	BlockOrnaments *	bo )
    {
    utilPropMaskClear( &(bo->boPropMask) );

    bo->boTopBorderNumber= -1;
    bo->boLeftBorderNumber= -1;
    bo->boRightBorderNumber= -1;
    bo->boBottomBorderNumber= -1;

    docInitBorderProperties( &(bo->boTopBorder) );
    docInitBorderProperties( &(bo->boLeftBorder) );
    docInitBorderProperties( &(bo->boRightBorder) );
    docInitBorderProperties( &(bo->boBottomBorder) );

    docInitItemShading( &(bo->boShading) );
    }

