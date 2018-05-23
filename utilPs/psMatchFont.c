/************************************************************************/
/*									*/
/*  Mapping between Document fonts and PostScript fonts.		*/
/*									*/
/************************************************************************/

#   include	"utilPsConfig.h"

#   include	<string.h>
#   include	<limits.h>

#   include	<appDebugon.h>

#   include	"psFontFamily.h"
#   include	"psDocumentFontStyle.h"
#   include	"psPostScriptFontList.h"
#   include	"textAttribute.h"
#   include	"psMatchFont.h"

/************************************************************************/

# define SHOW_MISSING 0
# if SHOW_MISSING

static int utilMatchFontShowMissing( int n, AfmFontInfo * afi )
    {
    SXDEB(afi->afiFullName,n);
    return 0;
    }

# endif

/************************************************************************/
/*									*/
/*  Decide whether a font can be used to render an office charset.	*/
/*									*/
/*  1)  Arbitrarily pick the first face of the family to check.		*/
/*  2)  If not unicodes are required, match without checking.		*/
/*									*/
/************************************************************************/

static int psFontFamilySupportsUnicodes(
				    const PsFontFamily *	aff,
				    int *			pMissingCount,
				    const IndexSet *		required )
    {
    int				rval= 1;
    AfmFontInfo *		afi= aff->psfFaces[0]; /* 1 */

    IndexSet			missing;
    int				missingCount= 0;

    utilInitIndexSet( &missing );

    if  ( aff->psfFaceCount == 0 || ! afi )
	{ LXDEB(aff->psfFaceCount,afi); rval= 0; goto ready;	}

    /*  2  */
    if  ( ! required )
	{ rval= 1; goto ready;	}

    if  ( afi->afiMetricsDeferred )
	{
	if  ( afi->afiResolveMetrics )
	    {
	    if  ( (*afi->afiResolveMetrics)( afi ) )
		{ SLDEB(afi->afiFullName,afi->afiMetricsDeferred);	}
	    }
	else{
	    SLDEB(afi->afiFullName,afi->afiMetricsDeferred);
	    }
	}

    missingCount= utilIndexSetRemoveAll( &missing, required,
						&(afi->afiUnicodesProvided) );
# if SHOW_MISSING
    if  ( missingCount > 0 )
	{
	IndexSet		xx;
	int			n;

	utilInitIndexSet( &xx );

	SLDEB(afi->afiFullName,missingCount);

	n= utilIndexSetIntersect( &xx, required, required );

	appDebug( "For %s %d/%d chars are missing\n",
				afi->afiFullName, missingCount, n );

	utilIndexSetForAll( &missing, utilMatchFontShowMissing, afi );
	}
# endif

    if  ( missingCount < 0 )
	{ LDEB(missingCount); rval= -1; goto ready;	}

    rval= ( missingCount == 0 );

  ready:

    utilCleanIndexSet( &missing );

    *pMissingCount= missingCount;
    return rval;
    }

/************************************************************************/
/*									*/
/*  An half hearted attempt to derive the font style from the family	*/
/*  name.								*/
/*									*/
/************************************************************************/

int psFontFamilyStyle(	const char *	fontFamilyName )
    {
    int		l= strlen( fontFamilyName );

    while( l > 0 )
	{
	int		past= -1;
	int		i;

	if  ( l == 5 && ! strncmp( fontFamilyName, "Arial", 5 ) )
	    { return DFstyleFSWISS;	}

	if  ( l == 9 && ! strncmp( fontFamilyName, "Helvetica", 9 ) )
	    { return DFstyleFSWISS;	}

	if  ( l == 5 && ! strncmp( fontFamilyName, "Times", 5 ) )
	    { return DFstyleFROMAN;	}

	if  ( l == 8 && ! strncmp( fontFamilyName, "Palatino", 8 ) )
	    { return DFstyleFROMAN;	}

	if  ( l == 6 && ! strncmp( fontFamilyName, "Symbol", 6 ) )
	    { return DFstyleFTECH;	}

	if  ( l == 7 && ! strncmp( fontFamilyName, "Courier", 7 ) )
	    { return DFstyleFMODERN;	}

	if  ( l == 5 && ! strncmp( fontFamilyName, "Ohlfs", 5 ) )
	    { return DFstyleFMODERN;	}

	if  ( l == 9 && ! strncmp( fontFamilyName, "Wingdings", 9 ) )
	    { return DFstyleFTECH;	}
	if  ( l == 8 && ! strncmp( fontFamilyName, "Webdings", 8 ) )
	    { return DFstyleFTECH;	}

	if  ( l == 7 && ! strncmp( fontFamilyName, "Corsiva", 7 ) )
	    { return DFstyleFSCRIPT;	}

	for ( i= 0; i < l- 1; i++ )
	    {
	    if  ( fontFamilyName[i] != ' ' && fontFamilyName[i+ 1] == ' ' )
		{ past= i+ 1;	}
	    }
	if  ( past <= 0 )
	    { break;	}
	l= past;

	for ( i= 0; i < l; i++ )
	    {
	    if  ( fontFamilyName[i] != ' ' )
		{ past= i+ 1;	}
	    }
	l= past;
	}

    return DFstyleFNIL;
    }

/************************************************************************/
/*									*/
/*  Get a font.								*/
/*									*/
/*  1)  Is it an open font? If so return its number.			*/
/*  2)  Claim memory.							*/
/*  3)  Obtain a list of the available fonts.				*/
/*  4)  Try to find an exact match based on the name.			*/
/*  4a) Try the first word.						*/
/*  5)  Try to map 'fnil' to 'Helvetica'.				*/
/*  6)  Try to map 'fswiss' to 'Helvetica'.				*/
/*  7)  Try to map 'froman' to 'Times'.					*/
/*  8)  Chose the first family that has a fixed width font for 'fmodern'*/
/*  9)  Try to map 'fscript' to 'ITC Zapf Chancery'.			*/
/*									*/
/************************************************************************/

static int psGetFamilyByName(
				const PsFontFamily **		pPsf,
				int *				pMissing,
				const PsFontFamily **		pPsfFallback,
				const char *			name,
				const IndexSet *		unicodes,
				const PostScriptFontList *	psfl )
    {
    int				fam;
    const PsFontFamily *	aff;
    int				missingCount= INT_MAX;

    if  ( ! name )
	{ return -1;	}

    fam= psPostScriptFontListGetFamilyByName( psfl, name );
    if  ( fam < 0 )
	{ return -1;	}

    aff= psfl->psflFamilies[fam];
    if  ( aff )
	{
	if  ( ! unicodes )
	    { *pPsf= aff; return 0; }

	if  ( psFontFamilySupportsUnicodes( aff, &missingCount, unicodes ) )
	    { *pPsf= aff; return 0; }

	if  ( missingCount < *pMissing )
	    {
	    *pMissing= missingCount;
	    *pPsfFallback= aff;
	    }
	}

    return -1;
    }

static int psGetFamilyByModifiedName(
				const PsFontFamily **		pPsf,
				int *				pMissing,
				const PsFontFamily **		pPsfFallback,
				const char *			name,
				const IndexSet *		unicodes,
				const PostScriptFontList *	psfl )
    {
    if  ( ! name )
	{ return -1;	}

    if  ( ! psGetFamilyByName( pPsf, pMissing, pPsfFallback,
						    name, unicodes, psfl ) )
	{ return 0;	}

    if  ( ! strncmp( name, "Times New Roman", 15 ) )
	{ name= "Times";	}

    if  ( ! strncmp( name, "Courier New", 11 ) )
	{ name= "Courier";	}

    if  ( ! strcmp( name, "Arial" ) )
	{ name= "Helvetica";	}
    if  ( ! strcmp( name, "Arial Narrow" ) )
	{ name= "Helvetica Narrow";	}

    if  ( ! strcmp( name, "Apple Chancery" ) )
	{ name= "ITC Zapf Chancery";	}

    if  ( ! strncmp( name, "Wingdings", 9 ) )
	{ name= "ITC Zapf Dingbats";	}
    if  ( ! strncmp( name, "Webdings", 8 ) )
	{ name= "ITC Zapf Dingbats";	}
    if  ( ! strcmp( name, "ZapfDingbats" ) )
	{ name= "ITC Zapf Dingbats";	}
    if  ( ! strcmp( name, "ZapfChancery" ) )
	{ name= "ITC Zapf Chancery";	}

    if  ( ! strcmp( name, "Lucida Console" ) )
	{ name= "Lucida Sans Typewriter";	}

    if  ( ! strcmp( name, "Bookman Old Style" ) )
	{ name= "ITC Bookman";	}

    if  ( ! strcmp( name, "Century Schoolbook" ) )
	{ name= "New Century Schoolbook";	}

    if  ( ! psGetFamilyByName( pPsf, pMissing, pPsfFallback,
						    name, unicodes, psfl ) )
	{ return 0;	}

    if  ( ! strncmp( name, "ITC Zapf Dingbats", 17 ) )
	{ name= "Dingbats";	}

    if  ( ! psGetFamilyByName( pPsf, pMissing, pPsfFallback,
						    name, unicodes, psfl ) )
	{ return 0;	}

    return -1;
    }

const char * styleFamilies[DFstyle_COUNT];

static int psGetFamilyByStyle(	const PsFontFamily **		pPsf,
				int *				pMissing,
				const PsFontFamily **		pPsfFallback,
				int				styleInt,
				const IndexSet *		unicodes,
				const PostScriptFontList *	psfl )
    {
    if  ( styleInt < 0 )
	{ styleInt= DFstyleFNIL;	}

    if  ( ! styleFamilies[DFstyleFNIL] )
	{
	styleFamilies[DFstyleFNIL]= "Helvetica";		/*  5  */
	styleFamilies[DFstyleFROMAN]= "Times";			/*  7  */
	styleFamilies[DFstyleFSWISS]= "Helvetica";		/*  6  */
	styleFamilies[DFstyleFMODERN]= "Courier";		/*  8  */
	styleFamilies[DFstyleFSCRIPT]= "ITC Zapf Chancery";	/*  9  */
	styleFamilies[DFstyleFDECOR]= "ITC Zapf Chancery";	/*  10  */
	styleFamilies[DFstyleFTECH]= "Symbol";			/*  11  */
	}

    if  ( styleFamilies[styleInt] )
	{
	if  ( ! psGetFamilyByName( pPsf, pMissing, pPsfFallback,
				styleFamilies[styleInt], unicodes, psfl ) )
	    { return 0;	}
	}

    return -1;
    }

static int psGetFamilyByStyleFromName(
				const PsFontFamily **		pPsf,
				int *				pMissing,
				const PsFontFamily **		pPsfFallback,
				const char *			name,
				const IndexSet *		unicodes,
				const PostScriptFontList *	psfl )
    {
    int			styleInt= psFontFamilyStyle( name );

    if  ( styleInt < 0 )
	{ SLDEB(name,styleInt); return -1; }

    if  ( psGetFamilyByStyle( pPsf, pMissing, pPsfFallback,
						styleInt, unicodes, psfl ) )
	{ /*SSDEB(name,styleStr);*/ return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Find the PostScript font that best matches a document font.		*/
/*  NOTE that we match the families.					*/
/*									*/
/*  3)  Find Document family: The name is used to resolve the font	*/
/*	name.								*/
/*  4)  Find the required encoding for the font.			*/
/*  5)  All kind of attempts to find a postscript font.			*/
/*									*/
/************************************************************************/

AfmFontInfo * psGetPsFontInfoForAttribute(
				const char *			familyName,
				int				styleInt,
				const IndexSet *		unicodesUsed,
				const TextAttribute *		ta,
				const PostScriptFontList *	psfl )
    {
    const PsFontFamily *		psf;
    int					fac;

    int					missing= INT_MAX;
    const PsFontFamily *		psfFallback= (const PsFontFamily *)0;

    /*  5  */
    if  ( psGetFamilyByName( &psf, &missing, &psfFallback,
				familyName, unicodesUsed, psfl )	&&
	  psGetFamilyByModifiedName( &psf, &missing, &psfFallback,
				familyName, unicodesUsed, psfl )	&&
	  psGetFamilyByStyle( &psf, &missing, &psfFallback,
				styleInt, unicodesUsed, psfl )		&&
	  psGetFamilyByStyleFromName( &psf, &missing, &psfFallback,
				familyName, unicodesUsed, psfl )	)
	{
	if  ( psfFallback )
	    { psf= psfFallback;		}
	else{ return (AfmFontInfo *)0;	}
	}

    for ( fac= 0; fac < psf->psfFaceCount; fac++ )
	{
	int	b= FONT_IS_BOLD( psf->psfFaces[fac] );
	int	s= FONT_IS_SLANTED( psf->psfFaces[fac] );

	if  ( b == ta->taFontIsBold && s == ta->taFontIsSlanted )
	    { return psf->psfFaces[fac];	}
	}

    for ( fac= 0; fac < psf->psfFaceCount; fac++ )
	{
	int	b= FONT_IS_BOLD( psf->psfFaces[fac] );
	int	s= FONT_IS_SLANTED( psf->psfFaces[fac] );

	if  ( b == ta->taFontIsBold || s == ta->taFontIsSlanted )
	    { return psf->psfFaces[fac];	}
	}

    if  ( psf->psfFaceCount > 0 )
	{ return psf->psfFaces[0];	}

    SDEB(familyName);
    return (AfmFontInfo *)0;
    }

