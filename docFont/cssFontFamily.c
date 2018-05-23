/************************************************************************/
/*									*/
/*  Make a font family name the could be correctly picked up in a CSS	*/
/*  stylesheet or a SVG file.						*/
/*									*/
/************************************************************************/

#   include	"docFontConfig.h"

#   include	<string.h>

#   include	<appDebugon.h>

#   include	"utilDocFont.h"
#   include	<psDocumentFontStyle.h>

/************************************************************************/
/*									*/
/*  Translate an RTF font designation to an HTML/CSS1 style one.	*/
/*									*/
/************************************************************************/

int cssFontFamilyIndicator(		char *		target,
					int		maxSize,
					int		styleInt,
					const char *	familyName )
    {
    const char *	genericFamily= (const char *)0;
    const char *	hintFamily= (const char *)0;

    int			genericSize= 0;
    int			hintSize= 0;
    int			nameSize= 0;
    int			size= 0;

    int			commas= 0;

    switch( styleInt )
	{
	case DFstyleFROMAN:
	    genericFamily= "serif";
	    hintFamily= "Times\",\"Times New Roman";
	    break;

	case DFstyleFNIL:
	case DFstyleFSWISS:
	    genericFamily= "sans-serif";
	    hintFamily= "Helvetica\",\"Arial";
	    break;

	case DFstyleFMODERN:
	    genericFamily= "monospace";
	    hintFamily= "Courier";
	    break;

	case DFstyleFSCRIPT:
	case DFstyleFDECOR:
	    genericFamily= "cursive";
	    hintFamily= "Zapf-Chancery";
	    break;

	case DFstyleFTECH:
	    hintFamily= "Symbol";
	    break;
	}

    if  ( genericFamily )
	{
	commas++;
	genericSize= strlen( genericFamily ); /* keyword -> no quotes */
	}
    if  ( hintFamily )
	{
	commas++;
	hintSize= 1+ strlen( hintFamily )+ 1;
	}
    nameSize= 1+ strlen( familyName )+ 1;

    size= nameSize+ genericSize+ hintSize+ commas;
    if  ( size > maxSize )
	{ LLLLDEB(nameSize,genericSize,hintSize,maxSize); return -1; }

    *(target++)= '"';
    strcpy( target, familyName ); target += nameSize- 2;
    *(target++)= '"';

    if  ( hintFamily )
	{
	*(target++)= ',';
	*(target++)= '"';
	strcpy( target, hintFamily ); target += hintSize- 2;
	*(target++)= '"';
	}

    if  ( genericFamily )
	{
	*(target++)= ',';
	strcpy( target, genericFamily ); target += genericSize;
	}

    *(target  )= '\0';
    return size;
    }

