/************************************************************************/
/*									*/
/*  The format '*' flag of a field.					*/
/*									*/
/************************************************************************/

#   include	"docBaseConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<ctype.h>

#   include	<appDebugon.h>

#   include	"docFieldFormat.h"


int docFieldHasMergeformat(		const FieldInstructions *	fi,
					int				comp )
    {
    const InstructionsComponent *	ic= fi->fiComponents+ comp;

    if  ( comp < fi->fiComponentCount -1			&&
	  docComponentIsFlag( fi, comp, '*' )			&&
	  docComponentEqualsWordNoCase( ic+ 1, "mergeformat", 11 )	)
	{ return 1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Translate the argument of the \\* format flag in the field		*/
/*  instructions to an integer value.					*/
/*									*/
/************************************************************************/

static const char * FieldFormats[]=
    {
    "",			/*  MERGE_ASIS	*/

	    /* Case shifting	*/
    "Lower",
    "Upper",
    "FirstCap",
    "Caps",

	    /* Number formatting */
    "Alphabetic",	/*  MERGE_ALPHABETIC_UPPER	*/
    "Arabic",
    "CardText",
    "DollarText",
    "Hex",		/*  MERGE_HEX_UPPER		*/
    "Ordinal",
    "OrdText",
    "Roman",		/*  MERGE_ROMAN_UPPER		*/

    "CharFormat",
    "MergeFormat",

    "alphabetic",	/*  MERGE_ALPHABETIC_LOWER	*/
    "hex",		/*  MERGE_HEX_LOWER		*/
    "roman",		/*  MERGE_ROMAN_LOWER		*/
    };

const int FieldFormatCount= sizeof(FieldFormats)/sizeof(char *);

static int FieldFormatLength[sizeof(FieldFormats)/sizeof(char *)]=
    {
    -1,
    };

int docFieldFormatInt(	const InstructionsComponent *	ic ) 
    {
    const char *	format;
    int			i;

    format= utilMemoryBufferGetString( &(ic->icBuffer) );

    if  ( FieldFormatCount != MERGE__COUNT )
	{ LLDEB(FieldFormatCount,MERGE__COUNT);	}

    if  ( FieldFormatLength[0] < 0 )
	{
	for ( i= 0; i < FieldFormatCount; i++ )
	    { FieldFormatLength[i]= strlen( FieldFormats[i] ); }
	}

    for ( i= 1; i < FieldFormatCount; i++ )
	{
	int		j;
	const char *	ref= FieldFormats[i];

	if  ( FieldFormatLength[i] != ic->icBuffer.mbSize )
	    { continue;	}

	for ( j= 0; j < ic->icBuffer.mbSize; j++ )
	    {
	    if  ( tolower( format[j] ) != tolower( ref[j] ) )
		{ break;	}
	    }

	if  ( j == ic->icBuffer.mbSize )
	    {
	    if  ( i == MERGE_ROMAN_UPPER	&&
		  islower( format[0] )		)
		{ return MERGE_ROMAN_LOWER;		}

	    if  ( i == MERGE_ALPHABETIC_UPPER	&&
		  islower( format[0] )		)
		{ return MERGE_ALPHABETIC_LOWER;	}

	    if  ( i == MERGE_HEX_UPPER		&&
		  islower( format[0] )		)
		{ return MERGE_HEX_LOWER;		}

	    return i;
	    }
	}

    return -1;
    }

int docFieldInstructionsAddFormat(	FieldInstructions *	fi,
					int			format )
    {
    MemoryBuffer	mb;

    if  ( FieldFormatCount != MERGE__COUNT )
	{ LLDEB(FieldFormatCount,MERGE__COUNT);	}

    if  ( format < 0 || format >= FieldFormatCount )
	{ LLDEB(format,FieldFormatCount);	}

    if  ( format == MERGE_ASIS )
	{ return 0;	}

    mb.mbBytes= (unsigned char *)FieldFormats[format];
    mb.mbSize= strlen( FieldFormats[format] );

    return docFieldInstructionsAddArgFlag( fi, '*', &mb );
    }

/************************************************************************/
/*									*/
/*  Extract an integer value from a component in the field instructions.*/
/*									*/
/************************************************************************/

int docFieldComponentInt(	int *				pVal,
				const InstructionsComponent *	ic ) 
    {
    const char *	format;
    char *		p;
    long		l;

    if  ( ic->icBuffer.mbSize < 1 )
	{ LDEB(ic->icBuffer.mbSize); return -1;	}

    format= (const char *)ic->icBuffer.mbBytes;
    p= (char *)format;

    l= strtol( format, &p, 10 );
    if  ( p == format )
	{ return -1;	}
    else{
	while( p- format < ic->icBuffer.mbSize && p[0] == ' ' )
	    { p++;	}
	if  ( p- format < ic->icBuffer.mbSize )
	    { return -1;	}
	}

    *pVal= l;
    return 0;
    }

/************************************************************************/

int docFieldComponentNumberFormat(
				unsigned char *			pNumberFormat,
				const InstructionsComponent *	ic ) 
    {
    int		f;

    f= docFieldFormatInt( ic );
    switch( f )
	{
	case MERGE_ALPHABETIC_UPPER:
	case MERGE_ARABIC:
	case MERGE_CARDTEXT:
	case MERGE_DOLLARTEXT:
	case MERGE_HEX_UPPER:
	case MERGE_ORDINAL:
	case MERGE_ORDTEXT:
	case MERGE_ROMAN_UPPER:

	case MERGE_ALPHABETIC_LOWER:
	case MERGE_HEX_LOWER:
	case MERGE_ROMAN_LOWER:
	    *pNumberFormat= f;
	    break;

	case MERGE_MERGEFORMAT:
	    break;

	default:
	    LDEB(f); return -1;
	}

    return 0;
    }


int docFieldComponentCaseShift(	unsigned char *			pCaseShift,
				const InstructionsComponent *	ic ) 
    {
    int		f;

    f= docFieldFormatInt( ic );
    switch( f )
	{
	case MERGE_MERGEFORMAT:
	    break;

	case MERGE_LOWER:
	case MERGE_UPPER:
	case MERGE_FIRSTCAP:
	case MERGE_CAPS:
	    *pCaseShift= f;
	    break;

	default:
	    LDEB(f); return -1;
	}

    return 0;
    }

