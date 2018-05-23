/************************************************************************/
/*									*/
/*  Calculate page number and ref fields.				*/
/*									*/
/************************************************************************/

#   include	"docBaseConfig.h"

#   include	<appDebugon.h>

#   include	"docDocumentField.h"
#   include	"docRefField.h"

void docInitRefField(	RefField *	rf )
    {
    utilInitMemoryBuffer( &(rf->rfBookmark) );
    utilInitMemoryBuffer( &(rf->rfSeparators) );

    rf->rfToNote= 0;
    rf->rfAsHyperlink= 0;
    rf->rfAsParagraphNumber= 0;
    rf->rfWithPosition= 0;

    return;
    }

void docCleanRefField(	RefField *	rf )
    {
    utilCleanMemoryBuffer( &(rf->rfBookmark) );
    utilCleanMemoryBuffer( &(rf->rfSeparators) );
    }

/************************************************************************/
/*									*/
/*  Evaluate REF fields.						*/
/*									*/
/************************************************************************/

int docGetRefField(		RefField *		rf,
				const DocumentField *	df )
    {
    const FieldInstructions *		fi= &(df->dfInstructions);
    const InstructionsComponent *	ic;
    int					comp;

    if  ( df->dfKind != DOCfkREF )
	{ return 1;	}

    ic= fi->fiComponents+ 1;
    for ( comp= 1; comp < fi->fiComponentCount; ic++, comp++ )
	{
	if  ( docComponentIsFlag( fi, comp, 'f' ) )
	    { rf->rfToNote= 1; continue;	}
	if  ( docComponentIsFlag( fi, comp, 'h' ) )
	    { rf->rfAsHyperlink= 1; continue;	}
	if  ( docComponentIsFlag( fi, comp, 'n' ) )
	    { rf->rfAsParagraphNumber= 1; continue;	}
	if  ( docComponentIsFlag( fi, comp, 'p' ) )
	    { rf->rfWithPosition= 1; continue;	}

	if  ( docComponentIsArgFlag( fi, comp, 'd' ) )
	    {
	    ic++, comp++;

	    if  ( utilCopyMemoryBuffer( &(rf->rfSeparators), &(ic->icBuffer) ) )
		{ LDEB(comp);	}

	    continue;
	    }

	if  ( ! ic->icIsFlag )
	    {
	    if  ( utilCopyMemoryBuffer( &(rf->rfBookmark), &(ic->icBuffer) ) )
		{ LDEB(comp);	}

	    continue;
	    }

	LDEB(comp);
	}

    return 0;
    }

int docSetRefField(		DocumentField *		df,
				const RefField *	rf )
    {
    FieldInstructions *		fi= &(df->dfInstructions);

    df->dfKind= DOCfkREF;

    if  ( docStartFieldInstructions( fi, "REF", 3 ) )
	{ LDEB(3); return -1;	}

    if  ( docFieldInstructionsAddComponent( fi, &(rf->rfBookmark) ) )
	{ LDEB(1); return -1;	}

    if  ( docFieldInstructionsAddArgFlagIfSet( fi, 'd', &(rf->rfSeparators) ) )
	{ LDEB(1); return -1;	}

    if  ( rf->rfToNote )
	{
	if  ( docFieldInstructionsAddFlag( fi, 'f' ) )
	    { LDEB(1); return -1;	}
	}

    if  ( rf->rfAsHyperlink )
	{
	if  ( docFieldInstructionsAddFlag( fi, 'h' ) )
	    { LDEB(1); return -1;	}
	}

    if  ( rf->rfAsParagraphNumber )
	{
	if  ( docFieldInstructionsAddFlag( fi, 'n' ) )
	    { LDEB(1); return -1;	}
	}

    if  ( rf->rfWithPosition )
	{
	if  ( docFieldInstructionsAddFlag( fi, 'p' ) )
	    { LDEB(1); return -1;	}
	}

    return 0;
    }
