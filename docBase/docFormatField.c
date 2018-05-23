/************************************************************************/
/*									*/
/*  Buffer administration routines.					*/
/*									*/
/************************************************************************/

#   include	"docBaseConfig.h"

#   include	<string.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	<utilBase26.h>
#   include	<utilRoman.h>

#   include	<appDebugon.h>

#   include	"docDocumentField.h"
#   include	"docFieldFormat.h"

/************************************************************************/
/*									*/
/*  Format values following the arguments of the \\* format flags in	*/
/*  the fieldinstructions.						*/
/*									*/
/************************************************************************/

/************************************************************************/
/*									*/
/*  Format an integer following the arguments of the \\* format flags	*/
/*  in the fieldinstructions.						*/
/*									*/
/************************************************************************/

int docFieldFormatInteger(	MemoryBuffer *			mbResult,
				int				format,
				int				value )
    {
    char			scratch[100+1];

    switch( format )
	{
	case MERGE_ARABIC:
	    sprintf( scratch, "%d", value );
	    utilMemoryBufferAppendBytes( mbResult,
				(unsigned char *)scratch, strlen( scratch ) );
	    break;

	case MERGE_ALPHABETIC_UPPER:
	    if  ( utilBase26String( scratch, sizeof(scratch)-1, value, 1 ) )
		{ sprintf( scratch, "ALPHA:%d", value );	}
	    utilMemoryBufferAppendBytes( mbResult,
				(unsigned char *)scratch, strlen( scratch ) );
	    break;

	case MERGE_ALPHABETIC_LOWER:
	    if  ( utilBase26String( scratch, sizeof(scratch)-1, value, 0 ) )
		{ sprintf( scratch, "alpha:%d", value );	}
	    utilMemoryBufferAppendBytes( mbResult,
				(unsigned char *)scratch, strlen( scratch ) );
	    break;

	case MERGE_ROMAN_UPPER:
	    if  ( utilRomanString( scratch, sizeof(scratch)-1, value, 1 ) )
		{ sprintf( scratch, "ROMAN:%d", value );	}
	    utilMemoryBufferAppendBytes( mbResult,
				(unsigned char *)scratch, strlen( scratch ) );
	    break;

	case MERGE_ROMAN_LOWER:
	    if  ( utilRomanString( scratch, sizeof(scratch)-1, value, 0 ) )
		{ sprintf( scratch, "roman:%d", value );	}
	    utilMemoryBufferAppendBytes( mbResult,
				(unsigned char *)scratch, strlen( scratch ) );
	    break;

	case MERGE_HEX_UPPER:
	    sprintf( scratch, "%X", (unsigned)value );
	    utilMemoryBufferAppendBytes( mbResult,
				(unsigned char *)scratch, strlen( scratch ) );
	    break;

	case MERGE_HEX_LOWER:
	    sprintf( scratch, "%x", (unsigned)value );
	    utilMemoryBufferAppendBytes( mbResult,
				(unsigned char *)scratch, strlen( scratch ) );
	    break;

	case MERGE_CARDTEXT:
	case MERGE_DOLLARTEXT:
	case MERGE_ORDINAL:
	case MERGE_ORDTEXT:
	default:
	    LDEB(format);
	    sprintf( scratch, "(%d)", value );
	    utilMemoryBufferAppendBytes( mbResult,
				(unsigned char *)scratch, strlen( scratch ) );
	    break;
	}

    return 0;
    }

