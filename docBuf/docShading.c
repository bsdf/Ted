/************************************************************************/
/*									*/
/*  Shading.								*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	<appDebugon.h>

#   include	<psShading.h>
#   include	"docBuf.h"

/************************************************************************/
/*									*/
/*  Derive rgb values for a solidly shaded cell.			*/
/*									*/
/************************************************************************/

int docGetSolidRgbShadeOfItem(	int *			pIsFilled,
				RGB8Color *		rgb8,
				const BufferDocument *	bd,
				const ItemShading *	is )
    {
    const DocumentProperties *	dp= &(bd->bdProperties);
    const ColorPalette *	cp= dp->dpColorPalette;

    RGB8Color			cf;
    RGB8Color			cb;

    int				fl= is->isLevel;
    int				bl= 10000- is->isLevel;

    int				r;
    int				g;
    int				b;

    cf.rgb8Red= 0;
    cf.rgb8Green= 0;
    cf.rgb8Blue= 0;

    cb.rgb8Red= 255;
    cb.rgb8Green= 255;
    cb.rgb8Blue= 255;

    if  ( is->isBackColor <= 0		&&
	  is->isForeColor <= 0		&&
	  is->isLevel == 0		)
	{ *pIsFilled= 0; return 0;	}

    if  ( is->isBackColor > 0			&&
	  is->isBackColor < cp->cpColorCount	)
	{ cb= cp->cpColors[is->isBackColor];	}

    if  ( is->isPattern == DOCspSOLID && is->isLevel > 0 )
	{
	if  ( is->isForeColor > 0			&&
	      is->isForeColor < cp->cpColorCount	)
	    { cf= cp->cpColors[is->isForeColor];	}
	}
    else{ cf= cb;					}

    r= ( fl* cf.rgb8Red+ bl* cb.rgb8Red )/ 10000;
    g= ( fl* cf.rgb8Green+ bl* cb.rgb8Green )/ 10000;
    b= ( fl* cf.rgb8Blue+ bl* cb.rgb8Blue )/ 10000;

    if  ( r == 255 && g == 255 && b == 255 )
	{ *pIsFilled= 0;	}
    else{
	*pIsFilled= 1;
	rgb8->rgb8Red= r;
	rgb8->rgb8Green= g;
	rgb8->rgb8Blue= b;
	}

    return 0;
    }

