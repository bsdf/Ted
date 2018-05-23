/************************************************************************/
/*									*/
/*  Calculate 'Hyperlink' fields.					*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	<appDebugon.h>

#   include	"docBuf.h"
#   include	"docRecalculateFields.h"
#   include	"docEvalField.h"

/************************************************************************/
/*									*/
/*  Evaluate a 'HYPERLINK' field.					*/
/*									*/
/*  Just make the contents blue as a compatibility hack with previous	*/
/*  versions.								*/
/*									*/
/************************************************************************/

int docRecalculateHyperlinkField(
				int *				pCalculated,
				int *				pPartShift,
				int *				pStroffShift,
				struct BufferItem *		paraBi,
				int				partFrom,
				int				partCount,
				DocumentField *			df,
				const RecalculateFields *	rf )
    {
#   if 0
    const int				partUpto= partFrom+ partCount+ 2;

    TextAttribute			taSet;
    PropertyMask			taSetMask;
    int					taChanged= 0;

    if  ( docSetHyperlinkAttribute( &taSet, &taSetMask, rf->rfDocument ) )
	{ LDEB(1); return -1;	}

    if  ( docChangeParticuleAttributes( &taChanged, (PropertyMask *)0,
						rf->rfDocument,
						paraBi, partFrom, partUpto,
						&taSet, &taSetMask ) )
	{ LDEB(1); return -1;	}

    if  ( taChanged )
	{ *pCalculated= 1;	}
    else{ *pCalculated= 0;	}

    /*
    *pPartShift += 0;
    *pStroffShift += 0;
    */

#   endif
    return 0;
    }

