/************************************************************************/
/*									*/
/*  Paragraph numbers.							*/
/*									*/
/************************************************************************/

#   include	"docBaseConfig.h"

#   include	<stdlib.h>

#   include	"docParaNumber.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Paragraph numbering.						*/
/*									*/
/************************************************************************/

void docCleanParagraphNumber(	ParagraphNumber*	pn )
    {
    if  ( pn->pnTextBefore )
	{ free( pn->pnTextBefore );	}
    if  ( pn->pnTextAfter )
	{ free( pn->pnTextAfter );	}

    return;
    }

void docInitParagraphNumber(	ParagraphNumber*	pn )
    {
    pn->pnLevel= -1;
    pn->pnNumberStyle= DOCpnDEC;
    pn->pnJustification= 0; /* left */
    pn->pnUsePrevText= 0;

    pn->pnUseHangingIndent= 0;

    pn->pnStartAt= 0;
    pn->pnIndent= 0;
    pn->pnSpace= 0;

    utilInitTextAttribute( &(pn->pnTextAttribute) );

    pn->pnTextBefore= (char *)0;
    pn->pnTextAfter= (char *)0;

    return;
    }

