/************************************************************************/
/*									*/
/*  Change the font of stretches of text.				*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	<appDebugon.h>

#   include	"docBuf.h"
#   include	"docTreeNode.h"
#   include	<docTextParticule.h>

/************************************************************************/
/*									*/
/*  Change the TextAttributes E.G. the font of a stretch of text	*/
/*  particules.								*/
/*									*/
/************************************************************************/

int docChangeParticuleAttributes(	int *			pChanged,
					PropertyMask *		pTaAllMask,
					BufferDocument *	bd,
					BufferItem *		paraBi,
					int			part,
					int			partUpto,
					const TextAttribute *	taSet,
					const PropertyMask *	taSetMask )
    {
    TextParticule *		tp;
    int				i;
    int				changed= 0;

    tp= paraBi->biParaParticules+ part;
    for ( i= part; i < partUpto; tp++, i++ )
	{
	TextAttribute	ta;
	int		attributeNumber= tp->tpTextAttrNr;
	PropertyMask	doneMask;

	utilPropMaskClear( &doneMask );

	docGetTextAttributeByNumber( &ta, bd, tp->tpTextAttrNr );

	utilUpdateTextAttribute( &doneMask, &ta, taSetMask, taSet );

	if  ( ! utilPropMaskIsEmpty( &doneMask )	||
	      attributeNumber < 0			)
	    {
	    changed= 1;

	    if  ( pTaAllMask )
		{ utilPropMaskOr( pTaAllMask, pTaAllMask, &doneMask );	}

	    attributeNumber= docTextAttributeNumber( bd, &ta );
	    if  ( attributeNumber < 0 )
		{ LDEB(attributeNumber); return -1;	}

	    tp->tpTextAttrNr= attributeNumber;
	    }
	}

    if  ( pChanged && changed )
	{ *pChanged= 1;	}

    return 0;
    }

