/************************************************************************/
/*									*/
/*  Bookkeeping for SEQ fields.						*/
/*									*/
/************************************************************************/

#   include	"docBaseConfig.h"

#   include	<appDebugon.h>

#   include	"docSeqField.h"
#   include	"docFieldFormat.h"
#   include	"docDocumentField.h"

/************************************************************************/

void docInitSeqField(	SeqField *	sf )
    {
    sf->sfIncrement= 1;
    sf->sfHidden= 0;
    sf->sfNumberFormat= MERGE_ARABIC;

    sf->sfResetTo= -1;
    sf->sfStyle= -1;

    utilInitMemoryBuffer( &(sf->sfIdentifier) );
    utilInitMemoryBuffer( &(sf->sfBookmark) );

    return;
    }

void docCleanSeqField(	SeqField *	sf )
    {
    utilCleanMemoryBuffer( &(sf->sfIdentifier) );
    utilCleanMemoryBuffer( &(sf->sfBookmark) );

    return;
    }

typedef enum SeqString
    {
    SEQ_Identifier,
    SEQ_Bookmark,

    SEQ__COUNT
    } SeqString;

int docGetSeqField(		SeqField *			sf,
				const DocumentField *		df )
    {
    const FieldInstructions *		fi= &(df->dfInstructions);
    const InstructionsComponent *	ic;
    int					comp;
    int					str= SEQ_Identifier;

    if  ( df->dfKind != DOCfkSEQ )
	{ return 1;	}

    ic= fi->fiComponents+ 1;
    for ( comp= 1; comp < fi->fiComponentCount; ic++, comp++ )
	{
	if  ( docComponentIsArgFlag( fi, comp, '*' ) )
	    {
	    ic++, comp++;

	    if  ( docFieldComponentNumberFormat( &(sf->sfNumberFormat), ic ) )
		{ LDEB(comp);	}

	    continue;
	    }

	if  ( docComponentIsArgFlag( fi, comp, 'r' ) )
	    {
	    ic++, comp++;

	    if  ( docFieldComponentInt( &(sf->sfResetTo), ic ) )
		{ LDEB(comp);	}

	    continue;
	    }

	if  ( docComponentIsArgFlag( fi, comp, 's' ) )
	    {
	    ic++, comp++;

	    if  ( docFieldComponentInt( &(sf->sfStyle), ic ) )
		{ LDEB(comp);	}

	    continue;
	    }

	if  ( docComponentIsFlag( fi, comp, 'c' ) )
	    { sf->sfIncrement= 0; continue;	}

	if  ( docComponentIsFlag( fi, comp, 'h' ) )
	    { sf->sfHidden= 1; continue;	}

	if  ( docComponentIsFlag( fi, comp, 'n' ) )
	    { sf->sfIncrement= 1; continue;	}

	if  ( ! ic->icIsFlag		&&
	      str == SEQ_Identifier	)
	    {
	    if  ( utilCopyMemoryBuffer( &(sf->sfIdentifier), &(ic->icBuffer) ) )
		{ LDEB(comp);	}
	    str++; continue;
	    }

	if  ( ! ic->icIsFlag		&&
	      str == SEQ_Bookmark	)
	    {
	    if  ( utilCopyMemoryBuffer( &(sf->sfBookmark), &(ic->icBuffer) ) )
		{ LDEB(comp);	}
	    str++; continue;
	    }

	LDEB(comp);
	}

    return 0;
    }

