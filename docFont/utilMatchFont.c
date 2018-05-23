/************************************************************************/
/*									*/
/*  Mapping between Document fonts and PostScript fonts.		*/
/*									*/
/************************************************************************/

#   include	"docFontConfig.h"

#   include	<string.h>

#   include	<appDebugon.h>
#   include	<psDocumentFontStyle.h>

#   include	"utilDocFont.h"
#   include	"utilMatchFont.h"
#   include	<textOfficeCharset.h>

/************************************************************************/
/*									*/
/*  Map the RTF style faces to the actual faces of a PostScript font.	*/
/*									*/
/*  6)  Remember what the face numbers of the variants of the font.	*/
/*  7)  If no face number was found for plain, take an arbitrary one.	*/
/*  8)  For all variants without a face number, take the plian one.	*/
/*	(but see 7).							*/
/*									*/
/************************************************************************/

static void utilMatchPsFacesToDocFont(	DocumentFont *		df,
					const PsFontFamily *	psf )
    {
    int				fac;

    /*  6  */
    for ( fac= 0; fac < psf->psfFaceCount; fac++ )
	{
	int	b= FONT_IS_BOLD( psf->psfFaces[fac] );
	int	s= FONT_IS_SLANTED( psf->psfFaces[fac] );

	df->dfPsFontInfo[FACE_INDEX(s,b)]= psf->psfFaces[fac];
	}

    if  ( ! df->dfPsFontInfo[0] )
	{
	for ( fac= 0; fac < FONTface_COUNT; fac++ )
	    { 
	    if  ( df->dfPsFontInfo[fac] )
		{ df->dfPsFontInfo[0]= df->dfPsFontInfo[fac]; break; }
	    }
	}

    for ( fac= 0; fac < FONTface_COUNT; fac++ )
	{ 
	if  ( ! df->dfPsFontInfo[fac] )
	    { df->dfPsFontInfo[fac]= df->dfPsFontInfo[0]; }
	}

    return;
    }

/************************************************************************/

/************************************************************************/
/*									*/
/*  Temporarily add the PostScript fonts that we have on the machine to	*/
/*  the font list of the document. As we only save the fonts that are	*/
/*  actually used in the document, the number of fonts in the document	*/
/*  will not grow every time it is opened in a different environment.	*/
/*									*/
/*  Just insert the font as an ANSI font: The exact details only count	*/
/*  once the font has been used and the document is saved.		*/
/*									*/
/************************************************************************/

int utilAddPsFontsToDocList(	DocumentFontList *		dfl,
				const PostScriptFontList *	psfl )
    {
    int				fam;

    for ( fam= 0; fam < psfl->psflFamilyCount; fam++ )
	{
	DocumentFont *		df;
	const PsFontFamily *	aff= psfl->psflFamilies[fam];

	int			styleInt;
	int			pitch= FONTpitchDEFAULT;

	styleInt= psFontFamilyStyle( aff->psfFontFamilyName );
	if  ( aff->psfHasFixedWidth )
	    { pitch= FONTpitchFIXED;	}
	if  ( aff->psfHasProportionalWidth )
	    { pitch= FONTpitchVARIABLE;	}

	df= utilAddFontToDocList( dfl, aff->psfFontFamilyName,
							styleInt, pitch );
	if  ( ! df )
	    { SXDEB(aff->psfFontFamilyName,df); return -1;	}

	utilMatchPsFacesToDocFont( df, aff );
	}

    return 0;
    }

typedef struct Base35Family
    {
    const char *	b35fName;
    int			b35fStyle;
    } Base35Family;

# if 0
    /*
    136 font set:
    http://www.adobe.com/products/postscript/pdfs/ps3fonts.pdf
    */

    { "Albertus",		DFstyleFROMAN	},
    { "Antique Olive",		DFstyleFSWISS	},
    { "Apple Chancery",		DFstyleFSCRIPT	},
    { "Arial",			DFstyleFSWISS	},
    { "Bodoni",			DFstyleFROMAN	},
    { "Bodoni Compressed",	DFstyleFROMAN	},
    { "Carta",			DFstyleFSWISS	},
    { "Chicago",		DFstyleFSWISS	},
    { "Clarendon",		DFstyleFROMAN	},
    { "Cooper",			DFstyleFROMAN	},
    { "Copperplate",		DFstyleFSWISS	},
    { "Coronet",		DFstyleFSCRIPT	},
    { "Eurostile",		DFstyleFSWISS	},
    { "Eurostile Extended",	DFstyleFSWISS	},
    { "Geneva",			DFstyleFSWISS	},
    { "Gill Sans",		DFstyleFSWISS	},
    { "Gill Sans Condensed",	DFstyleFSWISS	},
    { "Goudy",			DFstyleFROMAN	},
    { "Hoefler Text",		DFstyleFROMAN	},
    { "Hoefler Ornaments",	DFstyleFTECH	},
    { "Joanna",			DFstyleFROMAN	},
    { "Letter",			DFstyleFSWISS	},
    { "ITC Lubalin Graph",	DFstyleFROMAN	},
    { "Marigold",		DFstyleFSCRIPT	},
    { "Monaco",			DFstyleFSWISS	},
    { "ITC Mona Lisa Recut",	DFstyleFROMAN	},
    { "New York",		DFstyleFROMAN	},
    { "Optima",			DFstyleFSWISS	},
    { "Oxford",			DFstyleFSCRIPT	},
    { "Stempel Garamond",	DFstyleFROMAN	},
    { "Tekton",			DFstyleFSCRIPT	},
    { "Times New Roman",	DFstyleFROMAN	},
    { "Univers",		DFstyleFSWISS	},
    { "Univers Condensed",	DFstyleFSWISS	},
    { "Univers Extended",	DFstyleFSWISS	},
    { "Wingdings",		DFstyleFTECH	},
# endif

static const Base35Family UtilBase35Families[]=
{
    { "Courier",			DFstyleFMODERN	},
    { "Helvetica",			DFstyleFSWISS	},
    { "Helvetica Narrow",		DFstyleFSWISS	},
    { "ITC Avant Garde Gothic",		DFstyleFSWISS	},
    { "ITC Bookman",			DFstyleFROMAN	},
    { "ITC Zapf Chancery",		DFstyleFSCRIPT	},
    { "ITC Zapf Dingbats",		DFstyleFTECH	},
    { "New Century Schoolbook",		DFstyleFROMAN	},
    { "Palatino",			DFstyleFROMAN	},
    { "Symbol",				DFstyleFTECH	},
    { "Times",				DFstyleFROMAN	},
};
static const int UtilBase35FamilyCount=
			    sizeof(UtilBase35Families)/sizeof(Base35Family);

int utilAddBase35FontsToDocList(	DocumentFontList *	dfl )
    {
    int				fam;
    const Base35Family *	b35f= UtilBase35Families;

    for ( fam= 0; fam < UtilBase35FamilyCount; b35f++, fam++ )
	{
	DocumentFont *		df;
	int			pitch= FONTpitchVARIABLE;

	if  ( b35f->b35fStyle == DFstyleFMODERN )
	    { pitch= FONTpitchFIXED;	}

	df= utilAddFontToDocList( dfl, b35f->b35fName,
						b35f->b35fStyle, pitch );
	if  ( ! df )
	    { SXDEB(b35f->b35fName,df); return -1;	}
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Return the mappings relating to a font.				*/
/*									*/
/*  1)  Dingbats are a bit from the past that we still have to support.	*/
/*  2)  For fonts without an encoding, assume the same encoding as the	*/
/*	document.							*/
/*  3)  But make an exception for yet another legacy font.		*/
/*									*/
/************************************************************************/

const char * utilGetEncodingName(	const char *	fontName,
					int		charset )
    {
    const OfficeCharset *	oc;
    int				charsetIdx;

    /*  1  */
    if  ( ! strcmp( fontName, "ITC Zapf Dingbats" )	||
	  ! strcmp( fontName, "ZapfDingbats" )		||
	  ! strcmp( fontName, "Dingbats" )		)
	{ return "DINGBATS"; }

    /*  3  */
    if  ( ! strcmp( fontName, "Symbol" ) )
	{ return "SYMBOL"; }

    oc= utilGetOfficeCharsetByCharset( &charsetIdx, charset );
    if  ( ! oc )
	{ return (const char *)0;	}

    return oc->ocEncodingName;
    }

/************************************************************************/
/*									*/
/*  Find the set of encodings that best covers the unicode values in	*/
/*  the document.							*/
/*									*/
/*  Use a naive implementation of the 'Greedy Set Cover' algorithm.	*/
/*									*/
/************************************************************************/

typedef struct FontCharset
    {
    DocumentFont *	fcFont;
    int			fcCharsetIdx;
    } FontCharset;

static int docFontSetUnicodeCharsetIdx(	int		unicode,
					void *		vfc )
    {
    const FontCharset *	fc= (const FontCharset *)vfc;

    if  ( utilIndexMappingPut( &(fc->fcFont->dfUnicodeToCharset),
						unicode, fc->fcCharsetIdx ) )
	{ LLDEB(unicode,fc->fcCharsetIdx); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Find encodings that are needed for a font.				*/
/*									*/
/*  1)  Find the legacy encodings that intersect the collection of	*/
/*	characters that the document uses in this font. Also find the	*/
/*	charset that has the highest number of characters.		*/
/*  2)  Ignore the best character set and always use ANSI as the first	*/
/*	character set to use.						*/
/*  3)  Add the remaining legacy encodings to the set of fonts. The	*/
/*	ones with the highest number of characters in the font have	*/
/*	priority.							*/
/*  4)  Remember the biggest charset that has a particular unicode.	*/
/*	this administration is used to switch while saving the document.*/
/*									*/
/************************************************************************/

int docFontFindLegacyEncodings(	DocumentFont *	df )
    {
    int			rval= 0;

    int			csIdx;

    unsigned char	isUsed[CHARSETidx_COUNT];
    int			hitCount[CHARSETidx_COUNT];
    int			hitsLeft[CHARSETidx_COUNT];
    IndexSet		legacyUsed[CHARSETidx_COUNT];

    IndexSet		isBest;

    int			bestCount;
    int			bestIndex;

    df->dfCharsetIdxUsedCount= 0;

    utilInitIndexSet( &isBest );

    for ( csIdx= 0; csIdx < CHARSETidx_COUNT; csIdx++ )
	{
	utilInitIndexSet( &(legacyUsed[csIdx]) );
	hitCount[csIdx]= 0;
	hitsLeft[csIdx]= 0;
	isUsed[csIdx]= 0;
	df->dfCharsetIdxUsed[csIdx]= -1;
	}

    if  ( ! strcmp( df->dfName, "ITC Zapf Dingbats" )	||
	  ! strcmp( df->dfName, "Symbol" )		)
	{ goto superfluous;	}

    /*  1  */
    bestCount= 0; bestIndex= -1;
    for ( csIdx= 0; csIdx < CHARSETidx_COUNT; csIdx++ )
	{
	const	IndexSet *	legacySet;

	legacySet= utilOfficeCharsetGetCodeSet( csIdx );
	if  ( ! legacySet )
	    { continue;	}

	hitCount[csIdx]= utilIndexSetIntersect( &(legacyUsed[csIdx]),
					&(df->dfUnicodesUsed), legacySet );
	if  ( hitCount[csIdx] < 0 )
	    { LLDEB(csIdx,hitCount[csIdx]); rval= -1; goto ready;	}
	hitsLeft[csIdx]= hitCount[csIdx];

	if  ( bestCount < hitsLeft[csIdx] )
	    {
	    bestCount= hitsLeft[csIdx];
	    bestIndex= csIdx;
	    }
	}

    /*  2  */
    if  ( bestIndex > 0 )
	{
	int ansiCount= utilIndexSetRemoveAll( &isBest, &(legacyUsed[0]),
						    &(legacyUsed[bestIndex]) );
	if  ( ansiCount > 0 )
	    { bestIndex= 0;	}
	}

    /*  3  */
    while( bestIndex >= 0 )
	{
	const IndexSet *	subtract= &(legacyUsed[bestIndex]);

	df->dfCharsetIdxUsed[df->dfCharsetIdxUsedCount]= bestIndex;
	df->dfCharsetIdxUsedCount++;
	isUsed[bestIndex]= 1;

	bestCount= 0; bestIndex= -1;
	for ( csIdx= 0; csIdx < CHARSETidx_COUNT; csIdx++ )
	    {
	    if  ( isUsed[csIdx] || hitsLeft[csIdx] == 0 )
		{ continue;	}

	    hitsLeft[csIdx]= utilIndexSetRemoveAll( &(legacyUsed[csIdx]),
					    &(legacyUsed[csIdx]), subtract );
	    if  ( hitsLeft[csIdx] < 0 )
		{ LLDEB(csIdx,hitsLeft[csIdx]); rval= -1; goto ready;	}

	    if  ( bestCount < hitsLeft[csIdx] )
		{
		bestCount= hitsLeft[csIdx];
		bestIndex= csIdx;
		}
	    }
	}

    /*  4  */
    bestIndex= df->dfCharsetIdxUsedCount- 1;
    while( bestIndex >= 0 )
	{
	FontCharset	fc;

	fc.fcFont= df;
	fc.fcCharsetIdx= df->dfCharsetIdxUsed[bestIndex];

	if  ( utilIndexSetForAll( &(legacyUsed[fc.fcCharsetIdx]),
			    docFontSetUnicodeCharsetIdx, (void *)&fc ) < 0 )
	    { LDEB(fc.fcCharsetIdx); rval= -1; goto ready;	}

	bestIndex--;
	}

  superfluous:

    /*  Nothing found.. Take FONTcharsetDEFAULT */
    if  ( utilIndexSetGetNext( &(df->dfUnicodesUsed), -1 ) >= 0	&&
    	  df->dfCharsetIdxUsedCount == 0			)
	{
	if  ( ! utilGetOfficeCharsetByCharset( &(df->dfCharsetIdxUsed[0]),
							FONTcharsetDEFAULT ) )
	    { LDEB(1);	}

	df->dfCharsetIdxUsedCount++;
	}

  ready:

    utilCleanIndexSet( &isBest );

    for ( csIdx= 0; csIdx < CHARSETidx_COUNT; csIdx++ )
	{ utilCleanIndexSet( &(legacyUsed[csIdx]) );	}

    return rval;
    }
