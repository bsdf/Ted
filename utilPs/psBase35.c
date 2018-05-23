#   include	"utilPsConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<ctype.h>

#   include	"psFontInfo.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  HACK: map some (URW) names to standard font names (ADOBE base 35).	*/
/*									*/
/************************************************************************/

typedef struct Base35Font
    {
    const char *	nmFrom;
    const char *	nmTo;
    } Base35Font;

static const Base35Font PS_FamilyMap[]=
    {
	{ "Century Schoolbook L",	"New Century Schoolbook"	},
	{ "Nimbus Mono L",		"Courier"			},
	{ "Nimbus Roman No9 L",		"Times"				},
	{ "Nimbus Sans L",		"Helvetica"			},
	{ "Standard Symbols L",		"Symbol"			},
	{ "URW Antiqua T",		"New Century Schoolbook"	},
	{ "URW Bookman L",		"ITC Bookman"			},
	{ "URW Chancery L",		"ITC Zapf Chancery"		},
	{ "URW Gothic L",		"ITC Avant Garde Gothic"	},
	{ "URW Palladio L",		"Palatino"			},
	{ "Dingbats",			"ITC Zapf Dingbats"		},
    };

static const Base35Font PS_FullMap[]=
    {
    { "URW Gothic L Book",  "ITC Avant Garde Gothic Book"	},
    { "URW Gothic L Book Oblique",
			    "ITC Avant Garde Gothic Book Oblique"	},
    { "URW Gothic L Demi",  "ITC Avant Garde Gothic Demi"	},
    { "URW Gothic L Demi Oblique",
			    "ITC Avant Garde Gothic Demi Oblique"	},
    { "URW Bookman L Light",
			    "ITC Bookman Light"	},
    { "URW Bookman L Light Italic",
			    "ITC Bookman Light Italic"	},
    { "URW Bookman L Demi Bold",
			    "ITC Bookman Demi"	},
    { "URW Bookman L Demi Bold Italic",
			    "ITC Bookman Demi Italic"	},
    { "Nimbus Mono L Regular",
			    "Courier"	},
    { "Nimbus Mono L Regular Oblique",
			    "Courier Oblique"	},
    { "Nimbus Mono L Bold", "Courier Bold"	},
    { "Nimbus Mono L Bold Oblique",
			    "Courier Bold Oblique"	},
    { "Nimbus Sans L Regular",
			    "Helvetica"	},
    { "Nimbus Sans L Regular Italic",
			    "Helvetica Oblique"	},
    { "Nimbus Sans L Bold", "Helvetica Bold"	},
    { "Nimbus Sans L Bold Italic",
			    "Helvetica Bold Oblique"	},
    { "Nimbus Sans L Regular Condensed",
			    "Helvetica Narrow"	},
    { "Nimbus Sans L Regular Condensed Italic" ,
			    "Helvetica Narrow Oblique"	},
    { "Nimbus Sans L Bold Condensed",
			    "Helvetica Narrow Bold"	},
    { "Nimbus Sans L Bold Condensed Italic",
			    "Helvetica Narrow Bold Oblique"	},
    { "Century Schoolbook L Roman",
			    "New Century Schoolbook Roman"	},
    { "Century Schoolbook L Italic",
			    "New Century Schoolbook Italic"	},
    { "Century Schoolbook L Bold",
			    "New Century Schoolbook Bold"	},
    { "Century Schoolbook L Bold Italic",
			    "New Century Schoolbook Bold Italic"	},
    { "URW Palladio L Roman",
			    "Palatino Roman"	},
    { "URW Palladio L Italic",
			    "Palatino Italic"	},
    { "URW Palladio L Bold",
			    "Palatino Bold"	},
    { "URW Palladio L Bold Italic",
			    "Palatino Bold Italic"	},
    { "Standard Symbols L", "Symbol"	},		/* afm */
    { "Standard Symbols L Regular", "Symbol"	},	/* fontconfig */
    { "Nimbus Roman No9 L Regular",
			    "Times Roman"	},
    { "Nimbus Roman No9 L Regular Italic",
			    "Times Italic"	},
    { "Nimbus Roman No9 L Medium",
			    "Times Bold"	},
    { "Nimbus Roman No9 L Medium Italic",
			    "Times Bold Italic"	},
    { "URW Chancery L Medium Italic",
			    "ITC Zapf Chancery Medium Italic"	},
    { "Dingbats",	    "ITC Zapf Dingbats"	},	/* afm */
    { "Dingbats Regular",   "ITC Zapf Dingbats"	},	/* fontconfig */
    };


int psMapVisibleNamesToBase35(	AfmFontInfo *	afi )
    {
    int		i;
    int		mappedFull= -1;
    int		mappedFamily= -1;

    for ( i= 0; i < sizeof(PS_FamilyMap)/sizeof(Base35Font); i++ )
	{
	if  ( ! strcmp( afi->afiFamilyName, PS_FamilyMap[i].nmFrom ) )
	    { mappedFamily= i; break; }
	}

    for ( i= 0; i < sizeof(PS_FullMap)/sizeof(Base35Font); i++ )
	{
	if  ( ! strcmp( afi->afiFullName, PS_FullMap[i].nmFrom ) )
	    { mappedFull= i; break; }
	}

    if  ( mappedFamily >= 0 && mappedFull >= 0 )
	{
	if  ( afi->afiFamilyName_Orig )
	    { free( afi->afiFamilyName_Orig );	}
	afi->afiFamilyName_Orig= afi->afiFamilyName;

	afi->afiFamilyName= strdup( PS_FamilyMap[mappedFamily].nmTo );

	free( afi->afiFullName );
	afi->afiFullName= strdup( PS_FullMap[mappedFull].nmTo );

	return 0;
	}

    return 1;
    }

