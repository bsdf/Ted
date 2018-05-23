/************************************************************************/
/*									*/
/*  Calculate page number and ref fields.				*/
/*									*/
/************************************************************************/

#   include	"docBaseConfig.h"

#   include	<appDebugon.h>

#   include	"docDocumentField.h"
#   include	"docDateField.h"

void docInitDateField(	DateField *	dtf )
    {
    utilInitMemoryBuffer( &(dtf->dtfPicture) );

    dtf->dtfCalendar= FIELDcalendarDEFAULT;
    dtf->dtfUseLast= 0;

    return;
    }

void docCleanDateField(	DateField *	dtf )
    {
    utilCleanMemoryBuffer( &(dtf->dtfPicture) );
    }

/************************************************************************/
/*									*/
/*  Evaluate REF fields.						*/
/*									*/
/************************************************************************/

int docGetDateField(		DateField *		dtf,
				const DocumentField *	df )
    {
    const FieldInstructions *		fi= &(df->dfInstructions);
    const InstructionsComponent *	ic;
    int					comp;

    if  ( df->dfKind != DOCfkCREATEDATE		&&
	  df->dfKind != DOCfkPRINTDATE		&&
	  df->dfKind != DOCfkSAVEDATE		&&
	  df->dfKind != DOCfkDATE		&&
	  df->dfKind != DOCfkTIME		)
	{ return 1;	}

    ic= fi->fiComponents+ 1;
    for ( comp= 1; comp < fi->fiComponentCount; ic++, comp++ )
	{
	if  ( docComponentIsFlag( fi, comp, 'h' ) )
	    { dtf->dtfCalendar= FIELDcalendarLUNAR; continue;	}
	if  ( docComponentIsFlag( fi, comp, 's' ) )
	    { dtf->dtfCalendar= FIELDcalendarSAKA; continue;	}
	if  ( docComponentIsFlag( fi, comp, 'l' ) )
	    { dtf->dtfUseLast= 1; continue;	}

	if  ( docComponentIsArgFlag( fi, comp, '@' ) )
	    {
	    ic++, comp++;

	    if  ( utilCopyMemoryBuffer( &(dtf->dtfPicture), &(ic->icBuffer) ) )
		{ LDEB(comp);	}

	    continue;
	    }

	LDEB(comp);
	}

    return 0;
    }

