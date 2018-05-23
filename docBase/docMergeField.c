/************************************************************************/
/*									*/
/*  Bookkeeping about merge fields.					*/
/*									*/
/************************************************************************/

#   include	"docBaseConfig.h"

#   include	<appDebugon.h>

#   include	"docDocumentField.h"
#   include	"docMergeField.h"
#   include	"docFieldFormat.h"

void docInitMergeField(	MergeField *	mf )
    {
    utilInitMemoryBuffer( &(mf->mfFieldName) );
    utilInitMemoryBuffer( &(mf->mfTextBefore) );
    utilInitMemoryBuffer( &(mf->mfTextAfter) );

    mf->mfIsMapped= 0;
    mf->mfEnableVertival= 0;
    mf->mfCaseShift= MERGE_ASIS;

    return;
    }

void docCleanMergeField(	MergeField *	mf )
    {
    utilCleanMemoryBuffer( &(mf->mfFieldName) );
    utilCleanMemoryBuffer( &(mf->mfTextBefore) );
    utilCleanMemoryBuffer( &(mf->mfTextAfter) );
    }

int docGetMergeField(		MergeField *			mf,
				const DocumentField *		df )
    {
    const FieldInstructions *		fi= &(df->dfInstructions);
    const InstructionsComponent *	ic;
    int					comp;

    if  ( df->dfKind != DOCfkMERGEFIELD )
	{ return 1;	}

    ic= fi->fiComponents+ 1;
    for ( comp= 1; comp < fi->fiComponentCount; ic++, comp++ )
	{
	if  ( docComponentIsFlag( fi, comp, 'm' ) )
	    { mf->mfIsMapped= 1; continue;	}
	if  ( docComponentIsFlag( fi, comp, 'v' ) )
	    { mf->mfEnableVertival= 1; continue;	}

	if  ( docComponentIsArgFlag( fi, comp, 'b' ) )
	    {
	    ic++, comp++;

	    if  ( utilCopyMemoryBuffer( &(mf->mfTextBefore), &(ic->icBuffer) ) )
		{ LDEB(comp);	}

	    continue;
	    }

	if  ( docComponentIsArgFlag( fi, comp, 'f' ) )
	    {
	    ic++, comp++;

	    if  ( utilCopyMemoryBuffer( &(mf->mfTextAfter), &(ic->icBuffer) ) )
		{ LDEB(comp);	}

	    continue;
	    }

	if  ( ! ic->icIsFlag )
	    {
	    if  ( utilCopyMemoryBuffer( &(mf->mfFieldName), &(ic->icBuffer) ) )
		{ LDEB(comp);	}

	    continue;
	    }

	if  ( docComponentIsArgFlag( fi, comp, '*' ) )
	    {
	    ic++, comp++;

	    if  ( docFieldComponentCaseShift( &(mf->mfCaseShift), ic ) )
		{ LDEB(comp);	}

	    continue;
	    }

	LDEB(comp);
	}

    return 0;
    }

