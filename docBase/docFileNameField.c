/************************************************************************/
/*									*/
/*  Calculate page number and ref fields.				*/
/*									*/
/************************************************************************/

#   include	"docBaseConfig.h"

#   include	<appDebugon.h>

#   include	"docDocumentField.h"
#   include	"docFileNameField.h"

void docInitFileNameField(	FileNameField *	fnf )
    {
    fnf->fnfIncludePath= 0;

    return;
    }

void docCleanFileNameField(	FileNameField *	fnf )
    {
    }

/************************************************************************/
/*									*/
/*  Evaluate REF fields.						*/
/*									*/
/************************************************************************/

int docGetFileNameField(	FileNameField *		fnf,
				const DocumentField *	df )
    {
    const FieldInstructions *		fi= &(df->dfInstructions);
    const InstructionsComponent *	ic;
    int					comp;

    if  ( df->dfKind != DOCfkFILENAME )
	{ return 1;	}

    ic= fi->fiComponents+ 1;
    for ( comp= 1; comp < fi->fiComponentCount; ic++, comp++ )
	{
	if  ( docComponentIsFlag( fi, comp, 'p' ) )
	    { fnf->fnfIncludePath= 1; continue;	}

	LDEB(comp);
	}

    return 0;
    }

