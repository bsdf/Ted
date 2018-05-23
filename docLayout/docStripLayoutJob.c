/************************************************************************/
/*									*/
/*  Strip layout: Administrative routines.				*/
/*									*/
/************************************************************************/

#   include	"docLayoutConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>
#   include	<limits.h>

#   include	"docStripLayoutJob.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Initialize paragraph layout progress.				*/
/*									*/
/************************************************************************/

void docInitParagraphFrame(	ParagraphFrame *	pf )
    {
    geoInitRectangle( &(pf->pfParaContentRect) );
    geoInitRectangle( &(pf->pfCellContentRect) );
    geoInitRectangle( &(pf->pfCellRect) );

    pf->pfParaContentRect.drY1= INT_MAX/ 2;
    pf->pfCellContentRect.drY1= INT_MAX/ 2;
    pf->pfCellRect.drY1= INT_MAX/ 2;

    pf->pfRedrawX0Twips= -1;
    pf->pfRedrawX1Twips= -1;
    }

void docInitParagraphLayoutPosition( ParagraphLayoutPosition *	plp )
    {
    docInitParagraphFrame( &(plp->plpParagraphFrame) );

    plp->pspChild= 0;
    plp->pspChildAdvanced= 0;
    plp->pspLine= 0;
    plp->pspPart= 0;

    plp->pspChildren= (ParagraphLayoutJob *)0;
    plp->pspChildCount= 0;

    docInitLayoutPosition( &(plp->plpPos) );

    return;
    }

void docStripLayoutStartChild(	ParagraphLayoutPosition *	plp,
				int				child )
    {
    plp->pspChild= child;
    plp->pspChildAdvanced= 0;

    plp->pspLine= 0;
    plp->pspPart= 0;
    }

void docStripLayoutNextChild(	ParagraphLayoutPosition *	plp )
    {
    plp->pspChild++;
    plp->pspChildAdvanced= 0;

    plp->pspLine= 0;
    plp->pspPart= 0;
    }

void docInitParagraphLayoutJob(	ParagraphLayoutJob *	plj )
    {
    plj->pljChildUpto= 0;

    /**/
    docInitLayoutPosition( &(plj->cljMergedCellBelowPosion) );

    docInitParagraphLayoutPosition( &(plj->pljPos) );
    docInitParagraphLayoutPosition( &(plj->pljPos0) );

    return;
    }

int docParagraphLayoutPosClaimChildren(	ParagraphLayoutPosition *	plp,
					int				n )
    {
    ParagraphLayoutJob *	p;
    int				i;

    p= plp->pspChildren;
    for ( i= 0; i < plp->pspChildCount; p++, i++ )
	{
	docCleanParagraphLayoutJob( p );
	docInitParagraphLayoutJob( p );
	}

    if  ( n > 0 )
	{
	p= (ParagraphLayoutJob *)realloc( plp->pspChildren, n* sizeof(ParagraphLayoutJob) );
	if  ( ! p )
	    { LXDEB(n,p); return -1;	}
	plp->pspChildren= p;

	for ( i= 0; i < n; p++, i++ )
	    { docInitParagraphLayoutJob( p );	}
	}

    plp->pspChildCount= n;

    return 0;
    }

static void docCleanParagraphLayoutPos(	ParagraphLayoutPosition *	plp )
    {
    int				i;
    ParagraphLayoutJob *	p;

    p= plp->pspChildren;
    for ( i= 0; i < plp->pspChildCount; p++, i++ )
	{ docCleanParagraphLayoutJob( p );	}

    if  ( plp->pspChildren )
	{ free( plp->pspChildren );	}
    }

void docCleanParagraphLayoutJob(	ParagraphLayoutJob *	plj )
    {
    docCleanParagraphLayoutPos( &(plj->pljPos) );
    docCleanParagraphLayoutPos( &(plj->pljPos0) );
    }

void docBeginParagraphLayoutProgress(	ParagraphLayoutJob *	plj,
					int			child,
					int			line,
					int			part,
					int			paraUpto,
					const LayoutPosition *	lp )
    {
    plj->pljPos.pspChild= child;
    plj->pljPos.pspChildAdvanced= ( part > 0 );
    plj->pljPos.pspLine= line;
    plj->pljPos.pspPart= part;

    plj->pljChildUpto= paraUpto;

    plj->pljPos.plpPos= *lp;

    return;
    }
