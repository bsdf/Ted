/************************************************************************/
/*									*/
/*  Calculate page number and ref fields.				*/
/*									*/
/************************************************************************/

#   include	"docBaseConfig.h"

#   include	<appDebugon.h>

#   include	"docDocumentField.h"
#   include	"docIncludePictureField.h"

void docInitIncludePictureField(	IncludePictureField *	ipf )
    {
    utilInitMemoryBuffer( &(ipf->ipfFilename) );
    utilInitMemoryBuffer( &(ipf->ipfConverter) );

    ipf->ipfDoNotSaveResult= 0;

    return;
    }

void docCleanIncludePictureField(	IncludePictureField *	ipf )
    {
    utilCleanMemoryBuffer( &(ipf->ipfFilename) );
    utilCleanMemoryBuffer( &(ipf->ipfConverter) );
    }

/************************************************************************/
/*									*/
/*  Extract an INCLUDEPICTURE field.					*/
/*									*/
/************************************************************************/

int docGetIncludePictureField(		IncludePictureField *	ipf,
					const DocumentField *	df )
    {
    const FieldInstructions *		fi= &(df->dfInstructions);
    const InstructionsComponent *	ic;
    int					comp;
    int					gotName= 0;

    if  ( df->dfKind != DOCfkINCLUDEPICTURE )
	{ return 1;	}

    ic= fi->fiComponents+ 1;
    for ( comp= 1; comp < fi->fiComponentCount; ic++, comp++ )
	{
	if  ( docComponentIsFlag( fi, comp, 'd' ) )
	    { ipf->ipfDoNotSaveResult= 1; continue;	}

	if  ( docComponentIsArgFlag( fi, comp, 'c' ) )
	    {
	    ic++, comp++;

	    if  ( utilCopyMemoryBuffer( &(ipf->ipfConverter),
							&(ic->icBuffer) ) )
		{ LDEB(comp);	}

	    continue;
	    }

	if  ( ! ic->icIsFlag && ! gotName )
	    {
	    if  ( utilCopyMemoryBuffer( &(ipf->ipfFilename), &(ic->icBuffer) ) )
		{ LDEB(comp);	}

	    gotName= 1;
	    continue;
	    }

	if  ( docFieldHasMergeformat( fi, comp ) )
	    { comp++; continue;	}

	LLSDEB(gotName,comp,utilMemoryBufferGetString(&(ic->icBuffer)));
	}

    return 0;
    }

