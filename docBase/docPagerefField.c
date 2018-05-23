/************************************************************************/
/*									*/
/*  Calculate page number and ref fields.				*/
/*									*/
/************************************************************************/

#   include	"docBaseConfig.h"

#   include	<appDebugon.h>

#   include	"docDocumentField.h"
#   include	"docPagerefField.h"
#   include	"docFieldFormat.h"

void docInitPagerefField(	PagerefField *	pf )
    {
    utilInitMemoryBuffer( &(pf->pfBookmark) );

    pf->pfNumberFormat= MERGE_ASIS;
    pf->pfAsHyperlink= 0;
    pf->pfWithPosition= 0;

    return;
    }

void docCleanPagerefField(	PagerefField *	pf )
    {
    utilCleanMemoryBuffer( &(pf->pfBookmark) );
    }

/************************************************************************/
/*									*/
/*  Evaluate REF fields.						*/
/*									*/
/************************************************************************/

int docGetPagerefField(		PagerefField *		pf,
				const DocumentField *	df )
    {
    const FieldInstructions *		fi= &(df->dfInstructions);
    const InstructionsComponent *	ic;
    int					comp;

    if  ( df->dfKind != DOCfkPAGEREF )
	{ return 1;	}

    ic= fi->fiComponents+ 1;
    for ( comp= 1; comp < fi->fiComponentCount; ic++, comp++ )
	{
	if  ( docComponentIsFlag( fi, comp, 'h' ) )
	    { pf->pfAsHyperlink= 1; continue;	}
	if  ( docComponentIsFlag( fi, comp, 'p' ) )
	    { pf->pfWithPosition= 1; continue;	}

	if  ( docComponentIsArgFlag( fi, comp, '*' ) )
	    {
	    ic++, comp++;

	    if  ( docFieldComponentNumberFormat( &(pf->pfNumberFormat), ic ) )
		{ LDEB(comp);	}

	    continue;
	    }

	if  ( ! ic->icIsFlag )
	    {
	    if  ( utilCopyMemoryBuffer( &(pf->pfBookmark), &(ic->icBuffer) ) )
		{ LDEB(comp);	}

	    continue;
	    }

	LDEB(comp);
	}

    return 0;
    }

int docSetPagerefField(		DocumentField *		df,
				const PagerefField *	pf )
    {
    FieldInstructions *		fi= &(df->dfInstructions);

    df->dfKind= DOCfkPAGEREF;

    if  ( docStartFieldInstructions( fi, "PAGEREF", 7 ) )
	{ LDEB(7); return -1;	}

    if  ( docFieldInstructionsAddComponent( fi, &(pf->pfBookmark) ) )
	{ LDEB(1); return -1;	}

    if  ( docFieldInstructionsAddFormat( fi, pf->pfNumberFormat ) )
	{ LDEB(1); return -1;	}

    if  ( pf->pfAsHyperlink )
	{
	if  ( docFieldInstructionsAddFlag( fi, 'h' ) )
	    { LDEB(1); return -1;	}
	}

    if  ( pf->pfWithPosition )
	{
	if  ( docFieldInstructionsAddFlag( fi, 'p' ) )
	    { LDEB(1); return -1;	}
	}

    return 0;
    }

