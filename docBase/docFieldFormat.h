/************************************************************************/
/*									*/
/*  The format '*' flag of a field.					*/
/*									*/
/************************************************************************/

#   ifndef DOC_FIELD_FORMAT_H
#   define DOC_FIELD_FORMAT_H

#   include	"docFieldInstructions.h"

/************************************************************************/
/*									*/
/*  Modifiers to the field format. Follow the \\* flag.			*/
/*									*/
/************************************************************************/

typedef enum FieldFormat
    {
    MERGE_ASIS= 0,

	    /* Case conversion */
    MERGE_LOWER,
    MERGE_UPPER,
    MERGE_FIRSTCAP,
    MERGE_CAPS,

	    /* Number formatting */
    MERGE_ALPHABETIC_UPPER,	/*  Depends on first letter	*/
    MERGE_ARABIC,
    MERGE_CARDTEXT,
    MERGE_DOLLARTEXT,
    MERGE_HEX_UPPER,		/*  Depends on first letter	*/
    MERGE_ORDINAL,
    MERGE_ORDTEXT,
    MERGE_ROMAN_UPPER,		/*  Depends on first letter	*/

    MERGE_CHARFORMAT,
    MERGE_MERGEFORMAT,

    MERGE_ALPHABETIC_LOWER,	/*  Depends on first letter	*/
    MERGE_HEX_LOWER,		/*  Depends on first letter	*/
    MERGE_ROMAN_LOWER,		/*  Depends on first letter	*/

    MERGE__COUNT
    } FieldFormat;

/************************************************************************/
/*									*/
/*  Administrative routines.						*/
/*									*/
/************************************************************************/

extern int docFieldFormatInt(	const InstructionsComponent *	ic );

extern int docFieldInstructionsAddFormat(
				FieldInstructions *		fi,
				int				format );

#   endif /*  DOC_FIELD_FORMAT_H  */
