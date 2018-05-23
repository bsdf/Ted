/************************************************************************/
/*									*/
/*  Analysis and manipulation of font names.				*/
/*									*/
/************************************************************************/

#   include	"utilPsConfig.h"

#   include	<stdlib.h>
#   include	<string.h>

#   include	<appDebugon.h>

#   include	"psFontName.h"

/************************************************************************/
/*									*/
/*  Derive a font weight from the string designation.			*/
/*									*/
/************************************************************************/

int utilFontWeightFromString(		int *		pWeight,
					int *		pStart,
					int *		pLength,
					const char *	name )
    {
    const char *	s= name;

    while( s[0] == ' ' )
	{ s++;	}

    while( *s )
	{
	if  ( ! strncmp( s, "Extra Light", 11 ) )
	    {
	    *pWeight= FONTweightEXTRALIGHT;
	    *pStart= s- name;
	    *pLength= 11;
	    return 1;
	    }

	if  ( ! strncmp( s, "Ultralight", 10 ) )
	    {
	    *pWeight= FONTweightEXTRALIGHT;
	    *pStart= s- name;
	    *pLength= 10;
	    return 1;
	    }

	if  ( ! strncmp( s, "Light", 5 ) )
	    {
	    *pWeight= FONTweightLIGHT;
	    *pStart= s- name;
	    *pLength= 5;
	    return 1;
	    }

	if  ( ! strncmp( s, "Book", 4 ) )
	    {
	    *pWeight= FONTweightBOOK;
	    *pStart= s- name;
	    *pLength= 4;
	    return 1;
	    }

	if  ( ! strncmp( s, "Regular", 7 ) )
	    {
	    *pWeight= FONTweightREGULAR;
	    *pStart= s- name;
	    *pLength= 7;
	    return 1;
	    }
	if  ( ! strncmp( s, "Normal", 6 ) )
	    {
	    *pWeight= FONTweightREGULAR;
	    *pStart= s- name;
	    *pLength= 6;
	    return 1;
	    }
	if  ( ! strncmp( s, "Medium", 6 ) )
	    {
	    *pWeight= FONTweightMEDIUM;
	    *pStart= s- name;
	    *pLength= 6;
	    return 1;
	    }
	if  ( ! strncmp( s, "Roman", 5 ) )
	    {
	    *pWeight= FONTweightMEDIUM;
	    *pStart= s- name;
	    *pLength= 5;
	    return 1;
	    }


	if  ( ! strncmp( s, "Semibold", 8 ) )
	    {
	    *pWeight= FONTweightSEMIBOLD;
	    *pStart= s- name;
	    *pLength= 8;
	    return 1;
	    }
	if  ( ! strncmp( s, "Demibold", 8 ) )
	    {
	    *pWeight= FONTweightSEMIBOLD;
	    *pStart= s- name;
	    *pLength= 8;
	    return 1;
	    }
	if  ( ! strncmp( s, "Demi", 4 ) )
	    {
	    *pWeight= FONTweightSEMIBOLD;
	    *pStart= s- name;
	    *pLength= 4;
	    return 1;
	    }

	if  ( ! strncmp( s, "Bold", 4 ) )
	    {
	    *pWeight= FONTweightBOLD;
	    *pStart= s- name;
	    *pLength= 4;
	    return 1;
	    }

	if  ( ! strncmp( s, "Extrabold", 9 ) )
	    {
	    *pWeight= FONTweightEXTRABOLD;
	    *pStart= s- name;
	    *pLength= 9;
	    return 1;
	    }

	if  ( ! strncmp( s, "Heavy", 5 ) )
	    {
	    *pWeight= FONTweightEXTRABOLD;
	    *pStart= s- name;
	    *pLength= 5;
	    return 1;
	    }

	if  ( ! strncmp( s, "Black", 5 ) )
	    {
	    *pWeight= FONTweightBLACK;
	    *pStart= s- name;
	    *pLength= 5;
	    return 1;
	    }

	while( s[0] && s[0] != ' ' )
	    { s++;	}
	while( s[0] == ' ' )
	    { s++;	}
	}

    return 0;
    }


int utilFontWidthFromString(		int *		pWidth,
					int *		pStart,
					int *		pLength,
					const char *	name )
    {
    const char *	s= name;

    while( s[0] == ' ' )
	{ s++;	}

    while( *s )
	{
	if  ( ! strncmp( s, "Narrow", 6 )		&&
	      ( s[6] == '\0' || s[6] == ' ' )	)
	    {
	    *pWidth= FONTwidthCONDENSED;
	    *pStart= s- name;
	    *pLength= 6;
	    return 1;
	    }

	if  ( ! strncmp( s, "Extra Condensed", 15 )	&&
	      ( s[6] == '\0' || s[6] == ' ' )	)
	    {
	    *pWidth= FONTwidthEXTRACONDENSED;
	    *pStart= s- name;
	    *pLength= 15;
	    return 1;
	    }
	
	if  ( ! strncmp( s, "Condensed", 9 )	&&
	      ( s[9] == '\0' || s[9] == ' ' )	)
	    {
	    *pWidth= FONTwidthCONDENSED;
	    *pStart= s- name;
	    *pLength= 9;
	    return 1;
	    }
	if  ( ! strncmp( s, "Compressed", 9 )	&&
	      ( s[9] == '\0' || s[9] == ' ' )	)
	    {
	    *pWidth= FONTwidthCONDENSED;
	    *pStart= s- name;
	    *pLength= 9;
	    return 1;
	    }
	if  ( ! strncmp( s, "Compact", 9 )	&&
	      ( s[9] == '\0' || s[9] == ' ' )	)
	    {
	    *pWidth= FONTwidthCONDENSED;
	    *pStart= s- name;
	    *pLength= 9;
	    return 1;
	    }
	
	if  ( ! strncmp( s, "Extended", 8 )	&&
	      ( s[8] == '\0' || s[8] == ' ' )	)
	    {
	    *pWidth= FONTwidthEXPANDED;
	    *pStart= s- name;
	    *pLength= 8;
	    return 1;
	    }

	while( s[0] && s[0] != ' ' )
	    { s++;	}
	while( s[0] == ' ' )
	    { s++;	}
	}

    return 0;
    }

int psRemoveWidthFromName(		char *			target,
					int			maxlen,
					int *			pWidth,
					const char *		name )
    {
    int			rval= 0;
    int			width= FONTwidthNORMAL;
    int			widthStart= -1;
    int			widthLength= -1;

    int			len= strlen( name );

    if  ( len > maxlen )
	{ SDEB(name); return -1;	}
    strcpy( target, name );

    if  ( utilFontWidthFromString( &width,
				    &widthStart, &widthLength, target ) )
	{
	char *	to=  target+ widthStart;
	int	tail= len- widthStart- widthLength;

	memmove( to, to+ widthLength, tail+ 1 );
	len -= widthLength;

	while( len > 0 && target[len-1] == ' ' )
	    { target[len-1]= '\0'; len--;	}

	rval= 1;
	}

    *pWidth= width;
    return rval;
    }

/************************************************************************/
/*									*/
/*  Move the piece of the name that gives the width from the name of	*/
/*  the face to that of the family.					*/
/*									*/
/*  The purpose of this action is to reach a situation where the	*/
/*  variants of a family vary in weight and in slant, not in width.	*/
/*  In that way, we can use the RTF perspective, where a font family	*/
/*  can be used with different weight, slant and size.			*/
/*									*/
/*  1)  If a width appears in the full name.....			*/
/*  2)  Append it to the family name.					*/
/*  3)  If there is no width string yet, allocate one.			*/
/*  4)  Move pieces fragments of the full name: We want the family name	*/
/*	to be a prefix of the full name again.				*/
/*									*/
/************************************************************************/

int psFontInfoMoveWidthToFamilyName(	AfmFontInfo *	afi )
    {
    int		width= FONTwidthNORMAL;
    int		widthStart= -1;
    int		widthLength= 0;

    int		familyLength= strlen( afi->afiFamilyName );
    int		familyIsPrefix;
    int		hasWidth;
    int		familyHasWidth;

    familyHasWidth= utilFontWidthFromString( &width, &widthStart, &widthLength,
							afi->afiFamilyName );

    familyIsPrefix= strncmp( afi->afiFullName,
				    afi->afiFamilyName, familyLength ) == 0;

    if  ( familyIsPrefix )
	{
        hasWidth= utilFontWidthFromString( &width, &widthStart, &widthLength,
					    afi->afiFullName+ familyLength );
	if  ( hasWidth )
	    { widthStart += familyLength;	}
	}
    else{
        hasWidth= utilFontWidthFromString( &width, &widthStart, &widthLength,
							afi->afiFullName );
	}

    /*  1  */
    if  ( hasWidth )
	{
	int		fullLength= strlen( afi->afiFullName );

#	if 0
	/* "DejaVu Sans/Serif Condensed" from FontConfig, MdD dec 2006 */
	if  ( width != afi->afiWidthInt )
	    { SLLDEB(afi->afiFullName,width,afi->afiWidthInt);	}
#	endif

	/*  2  */
	if  ( ! familyHasWidth )
	    {
	    char *		fresh;

	    fresh= (char *)realloc( afi->afiFamilyName,
					familyLength+ 1+ widthLength+ 1 );
	    if  ( ! fresh )
		{ XDEB(fresh); return -1;	}
	    afi->afiFamilyName= fresh;

	    afi->afiFamilyName[familyLength]= ' ';
	    strncpy( afi->afiFamilyName+ familyLength+ 1,
		afi->afiFullName+ widthStart, widthLength )[widthLength]= '\0';
	    }

	/*  3  */
	if  ( ! afi->afiWidthStr )
	    {
	    afi->afiWidthStr= (char *)malloc( widthLength+ 1 );
	    if  ( afi->afiWidthStr )
		{
		strncpy( afi->afiWidthStr, afi->afiFullName+ widthStart,
					    widthLength )[widthLength]= '\0';
		}
	    }

	/*  4  */
	if  ( familyIsPrefix )
	    {
	    char *	full;
	    char *	to;
	    char *	from= afi->afiFullName+ familyLength;

	    while( widthStart > 0 && afi->afiFullName[widthStart- 1] == ' ' )
		{ widthStart--; widthLength++;	}
	    while( afi->afiFullName[widthStart+ widthLength] == ' ' )
		{ widthLength++;	}

	    full= (char *)malloc( fullLength+ 2+ 1 );
	    if  ( ! full )
		{ XDEB(full); return -1;	}
	    else{
		strcpy( full, afi->afiFamilyName );
		to= full+ strlen( full );

		if  ( from- afi->afiFullName < widthStart )
		    {
		    *(to++)= ' ';
		    while( *from == ' ' )
			{ from++;	}

		    while( from- afi->afiFullName < widthStart )
			{ *(to++)= *(from++);	}
		    }

		from= afi->afiFullName+ widthStart+ widthLength;
		if  ( *from )
		    {
		    *(to++)= ' ';
		    while( *from )
			{ *(to++)= *(from++);	}
		    }

		*to= '\0';
		}

	    free( afi->afiFullName );
	    afi->afiFullName= full;
	    }
	}

    return 0;
    }

/************************************************************************/

typedef struct Int2Str
    {
    int			isI;
    const char *	isS;
    } Int2Str;

static const char * psInt2String( int i, const Int2Str * is, int n )
    {
    int		j;

    for ( j= 1; j < n; j++ )
	{
	if  ( i < ( is[j-1].isI+ is[j].isI )/ 2 )
	    { return is[j- 1].isS;	}
	}

    return is[n-1].isS;
    }

const char * psWidthStr( int width )
    {
    static const Int2Str ws[]=
	{
	    { FONTwidthULTRACONDENSED,	"Ultracondensed"	},
	    { FONTwidthEXTRACONDENSED,	"Extracondensed"	},
	    { FONTwidthCONDENSED,	"Condensed"		},
	    { FONTwidthSEMICONDENSED,	"Semicondensed"		},
	    { FONTwidthNORMAL,		"Normal"		},
	    { FONTwidthSEMIEXPANDED,	"Semiexpanded"		},
	    { FONTwidthEXPANDED,	"Expanded"		},
	    { FONTwidthEXTRAEXPANDED,	"Extraexpanded"		},
	    { FONTwidthULTRAEXPANDED,	"Ultraexpanded"		},
	};

    return psInt2String( width, ws, sizeof(ws)/sizeof(Int2Str) );
    }

const char * psWeightStr( int weight )
    {
    static const Int2Str ws[]=
	{
	    { FONTweightTHIN,		"Thin"		},
	    { FONTweightEXTRALIGHT,	"Extralight"	},
	    { FONTweightLIGHT,		"Light"		},
	    { FONTweightBOOK,		"Book"		},
	    { FONTweightREGULAR,	"Regular"	},
	    { FONTweightMEDIUM,		"Medium"	},
	    { FONTweightDEMIBOLD,	"Demibold"	},
	    { FONTweightBOLD,		"Bold"		},
	    { FONTweightEXTRABOLD,	"Extrabold"	},
	    { FONTweightBLACK,		"Black"		},
	};

    return psInt2String( weight, ws, sizeof(ws)/sizeof(Int2Str) );
    }

/************************************************************************/
/*									*/
/*  Try to make something of the X11 font weight field.			*/
/*									*/
/*  As the content of the X11 field is not covered by a standard, we	*/
/*  have to live with what font designers and font servers choose to	*/
/*  report.								*/
/*									*/
/************************************************************************/

int psFontGetWeight(	unsigned char *		pWeight,
			const char *		weight )
    {
    if  ( ! strcmp( weight, "extralight" ) )
	{ *pWeight= FONTweightEXTRALIGHT; return 0;	}
    if  ( ! strcmp( weight, "ultralight" ) )
	{ *pWeight= FONTweightEXTRALIGHT; return 0;	}
    if  ( ! strcmp( weight, "thin" ) )
	{ *pWeight= FONTweightEXTRALIGHT; return 0;	}

    if  ( ! strcmp( weight, "light" ) )
	{ *pWeight= FONTweightLIGHT; return 0;	}

    if  ( ! strcmp( weight, "demilight" ) )
	{ *pWeight= FONTweightBOOK; return 0;	}
    if  ( ! strcmp( weight, "semilight" ) )
	{ *pWeight= FONTweightBOOK; return 0;	}
    if  ( ! strcmp( weight, "book" ) )
	{ *pWeight= FONTweightBOOK; return 0;	}

    if  ( ! strcmp( weight, "" ) )
	{ *pWeight= FONTweightMEDIUM; return 0;	}
    if  ( ! strcmp( weight, "roman" ) )
	{ *pWeight= FONTweightMEDIUM; return 0;	}
    if  ( ! strcmp( weight, "medium" ) )
	{ *pWeight= FONTweightMEDIUM; return 0;	}
    if  ( ! strcmp( weight, "normal" ) )
	{ *pWeight= FONTweightMEDIUM; return 0;	}
    if  ( ! strcmp( weight, "regular" ) )
	{ *pWeight= FONTweightMEDIUM; return 0;	}

    if  ( ! strcmp( weight, "semi bold" ) )
	{ *pWeight= FONTweightSEMIBOLD; return 0;	}
    if  ( ! strcmp( weight, "demi bold" ) )
	{ *pWeight= FONTweightSEMIBOLD; return 0;	}
    if  ( ! strcmp( weight, "semibold" ) )
	{ *pWeight= FONTweightSEMIBOLD; return 0;	}
    if  ( ! strcmp( weight, "demibold" ) )
	{ *pWeight= FONTweightSEMIBOLD; return 0;	}
    if  ( ! strcmp( weight, "demi" ) )
	{ *pWeight= FONTweightSEMIBOLD; return 0;	}

    if  ( ! strcmp( weight, "bold" ) )
	{ *pWeight= FONTweightBOLD; return 0;	}
    if  ( ! strcmp( weight, "heavy" ) )
	{ *pWeight= FONTweightBOLD; return 0;	}

    if  ( ! strcmp( weight, "ultrabold" ) )
	{ *pWeight= FONTweightEXTRABOLD; return 0;	}
    if  ( ! strcmp( weight, "extrablack" ) )
	{ *pWeight= FONTweightEXTRABOLD; return 0;	}
    if  ( ! strcmp( weight, "extrabold" ) )
	{ *pWeight= FONTweightEXTRABOLD; return 0;	}
    if  ( ! strcmp( weight, "extra bold" ) )
	{ *pWeight= FONTweightEXTRABOLD; return 0;	}

    if  ( ! strcmp( weight, "black" ) )
	{ *pWeight= FONTweightBLACK; return 0;	}
    if  ( ! strcmp( weight, "ultrablack" ) )
	{ *pWeight= FONTweightBLACK; return 0;	}

    SDEB(weight); return -1;
    }

/************************************************************************/
/*									*/
/*  Try to make something of the X11 font width field.			*/
/*									*/
/*  As the content of the X11 field is not covered by a standard, we	*/
/*  have to live with what font designers and font servers choose to	*/
/*  report.								*/
/*									*/
/************************************************************************/

int psFontGetWidth(	int *			pWidth,
			const char *		swdth )
    {
    if  ( ! strcmp( swdth, "narrow" ) )
	{ *pWidth= FONTwidthCONDENSED; return 0;	}
    if  ( ! strcmp( swdth, "extra condensed" ) )
	{ *pWidth= FONTwidthEXTRACONDENSED; return 0;	}

    if  ( ! strcmp( swdth, "condensed" ) )
	{ *pWidth= FONTwidthCONDENSED; return 0;	}

    if  ( ! strcmp( swdth, "semi condensed" ) )
	{ *pWidth= FONTwidthSEMICONDENSED; return 0;	}
    if  ( ! strcmp( swdth, "semicondensed" ) )
	{ *pWidth= FONTwidthSEMICONDENSED; return 0;	}

    if  ( ! strcmp( swdth, "normal" ) )
	{ *pWidth= FONTwidthNORMAL; return 0;	}

    if  ( ! strcmp( swdth, "semi extended" ) )
	{ *pWidth= FONTwidthSEMIEXPANDED; return 0;	}
    if  ( ! strcmp( swdth, "semi expanded" ) )
	{ *pWidth= FONTwidthSEMIEXPANDED; return 0;	}
    if  ( ! strcmp( swdth, "semiexpanded" ) )
	{ *pWidth= FONTwidthSEMIEXPANDED; return 0;	}

    if  ( ! strcmp( swdth, "extended" ) )
	{ *pWidth= FONTwidthEXPANDED; return 0;	}
    if  ( ! strcmp( swdth, "expanded" ) )
	{ *pWidth= FONTwidthEXPANDED; return 0;	}

    SDEB(swdth); return -1;
    }

