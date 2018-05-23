/************************************************************************/
/*									*/
/*  Calculate 'Symbol' fields.						*/
/*									*/
/************************************************************************/

#   include	"docBaseConfig.h"

#   include	<appDebugon.h>

#   include	"docDocumentField.h"
#   include	"docSymbolField.h"

void docInitSymbolField(	SymbolField *	sf )
    {
    utilInitMemoryBuffer( &(sf->sfFontName) );
    sf->sfEncoding= SYMBOLencANSI;
    sf->sfSymbol= -1;
    sf->sfSizePoints= -1;
    sf->sfIsHigh= 0;

    return;
    }

void docCleanSymbolField(	SymbolField *	sf )
    {
    utilCleanMemoryBuffer( &(sf->sfFontName) );
    }

/************************************************************************/
/*									*/
/*  Extract the values from a symbol field.				*/
/*									*/
/************************************************************************/

int docFieldGetSymbol(	SymbolField *		sf,
			const DocumentField *	df )
    {
    const FieldInstructions *		fi= &(df->dfInstructions);
    const InstructionsComponent *	ic;
    int					comp;

    if  ( df->dfKind != DOCfkSYMBOL )
	{ return 1;	}

    ic= fi->fiComponents+ 1;
    for ( comp= 1; comp < fi->fiComponentCount; ic++, comp++ )
	{
	if  ( docComponentIsFlag( fi, comp, 'a' ) )
	    { sf->sfEncoding= SYMBOLencANSI; continue;	}

	if  ( docComponentIsArgFlag( fi, comp, 'f' ) )
	    {
	    ic++; comp++;

	    if  ( utilCopyMemoryBuffer( &(sf->sfFontName), &(ic->icBuffer) ) )
		{ LDEB(1); return -1;	}

	    continue;
	    }

	if  ( docComponentIsFlag( fi, comp, 'h' ) )
	    { sf->sfIsHigh= 1; continue;	}

	if  ( docComponentIsFlag( fi, comp, 'j' ) )
	    { sf->sfEncoding= SYMBOLencSHIFT_JIS; continue;	}

	if  ( docComponentIsArgFlag( fi, comp, 's' ) )
	    {
	    ic++, comp++;

	    if  ( docFieldComponentInt( &(sf->sfSizePoints), ic ) )
		{ LDEB(comp);	}

	    continue;
	    }

	if  ( docComponentIsFlag( fi, comp, 'u' ) )
	    { sf->sfEncoding= SYMBOLencUNICODE; continue;	}

	if  ( ! ic->icIsFlag )
	    {
	    if  ( docFieldComponentInt( &(sf->sfSymbol), ic ) )
		{ LLDEB(comp,fi->fiComponentCount);	}

	    continue;
	    }

	LDEB(comp);
	}

    if  ( sf->sfSymbol < 0 )
	{ LDEB(sf->sfSymbol); return -1;	}

    return 0;
    }

