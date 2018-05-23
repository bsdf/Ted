/************************************************************************/
/*									*/
/*  Delimit ranges in a line that can bedrawn in one instruction.	*/
/*									*/
/************************************************************************/

#   include	"docLayoutConfig.h"

#   include	<stddef.h>

#   include	"docTextRun.h"

#   include	<appDebugon.h>
#   include	<docTreeNode.h>
#   include	<docTextParticule.h>

/************************************************************************/
/*									*/
/*  Delimit a string that can be drawn at once.				*/
/*									*/
/*  Delimit a stretch of text that can be drawn using one string	*/
/*  drawing instruction. For smallcaps, the routine is still usefull	*/
/*  but the stretch will not be drawn in one instruction.		*/
/*									*/
/************************************************************************/

int docLayoutDelimitRun(		TextRun *		tr,
					const int		x0Twips,
					BufferDocument *	bd,
					const BufferItem *	paraNode,
					int			part,
					int			past,
					int			separate )
    {
    int				count= past- part;
    int				done;
    int				upto;
    int				len;
    int				x1Twips= x0Twips;
    int				haveUnderline= 0;
    int				haveStriketrough= 0;

    int				textAttrNr;
    TextAttribute		taFound;

    const TextParticule *	tp= paraNode->biParaParticules+ part;

    textAttrNr= docGetEffectiveTextAttributes( &taFound, bd, paraNode, part );
    haveUnderline= taFound.taTextIsUnderlined != 0;
    haveStriketrough= taFound.taHasStrikethrough != 0;

    x1Twips += tp[0].tpTwipsWide;

    for ( done= 1; done < count; done++ )
	{
	int			nx;
	TextAttribute		taNx;

	nx= docGetEffectiveTextAttributes( &taNx, bd, paraNode, part+ done );

	if  ( nx != textAttrNr			||
	      tp[done].tpKind != DOCkindSPAN	)
	    { break;	}

	if  ( separate )
	    { break;	}

	if  ( taFound.taTextIsUnderlined )
	    { haveUnderline= 1;	}
	if  ( taFound.taHasStrikethrough != 0 )
	    { haveStriketrough= 1;	}

	x1Twips += tp[done].tpTwipsWide;
	}

    upto= tp[done-1].tpStroff+ tp[done-1].tpStrlen;
    upto= docParaPastLastNonBlank( paraNode, tp->tpStroff, upto );
    len= upto- tp->tpStroff;

    if  ( part+ done < paraNode->biParaParticuleCount- 1	&&
	  tp[done].tpKind == DOCkindOPT_HYPH			&&
	  tp[done].tpTwipsWide > 0				)
	{
	len += tp[done].tpStrlen;
	x1Twips += tp[done].tpTwipsWide;
	done++;
	}

    tr->trX0Twips= x0Twips;
    tr->trX1Twips= x1Twips;
    tr->trStroff= tp->tpStroff;
    tr->trStrlenNb= len;
    tr->trPartFrom= part;
    tr->trPartUpto= part+ done;
    tr->trTextAttribute= taFound;
    tr->trTextAttributeNr= textAttrNr;

    tr->trHasUnderline= haveUnderline;
    tr->trHasStrikethrough= haveStriketrough;

    return done;
    }

int docNextUnderlinedRun(		int *			pHead,
					int *			pPast,
					int *			pTextAttrNr,
					TextAttribute *		taLine,
					BufferDocument *	bd,
					const BufferItem *	paraNode,
					int			part,
					int			upto )
    {
    int			head= part;
    TextAttribute	taFound;
    int			textAttrNrFound= -1;

    while( part < upto )
	{
	TextAttribute	ta;
	int		textAttrNr;

	textAttrNr= docGetEffectiveTextAttributes( &ta, bd, paraNode, part );
	if  ( ! ta.taTextIsUnderlined )
	    { part++; continue;	}

	head= part;
	taFound= ta;
	textAttrNrFound= textAttrNr;
	while( part < upto )
	    {
	    docGetEffectiveTextAttributes( &ta, bd, paraNode, part );

	    if  ( ! ta.taTextIsUnderlined )
		{ break;	}

	    part++;
	    }

	if  ( part > head )
	    {
	    *pHead= head;
	    *pPast= part;
	    *pTextAttrNr= textAttrNrFound;
	    *taLine= taFound;
	    return 0;
	    }
	}

    return 1;
    }

int docNextStrikethroughRun(		int *			pHead,
					int *			pPast,
					int *			pTextAttrNr,
					TextAttribute *		taLine,
					BufferDocument *	bd,
					const BufferItem *	paraNode,
					int			part,
					int			upto )
    {
    int			head= part;
    TextAttribute	taFound;
    int			textAttrNrFound= -1;

    while( part < upto )
	{
	TextAttribute	ta;
	int		textAttrNr;

	textAttrNr= docGetEffectiveTextAttributes( &ta, bd, paraNode, part );
	if  ( ! ta.taHasStrikethrough )
	    { part++; continue;	}

	head= part;
	taFound= ta;
	textAttrNrFound= textAttrNr;
	while( part < upto )
	    {
	    docGetEffectiveTextAttributes( &ta, bd, paraNode, part );

	    if  ( ! ta.taHasStrikethrough )
		{ break;	}

	    part++;
	    }

	if  ( part > head )
	    {
	    *pHead= head;
	    *pPast= part;
	    *pTextAttrNr= textAttrNrFound;
	    *taLine= taFound;
	    return 0;
	    }
	}

    return 1;
    }

