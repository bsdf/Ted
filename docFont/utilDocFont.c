/************************************************************************/
/*									*/
/*  Font administration for a document.					*/
/*									*/
/************************************************************************/

#   include	"docFontConfig.h"

#   include	<stdlib.h>
#   include	<string.h>

#   include	<appDebugon.h>

#   include	"utilDocFont.h"
#   include	<psDocumentFontStyle.h>

/************************************************************************/
/*									*/
/*  Initialise a font.							*/
/*  Clean a font.							*/
/*  Copy a font.							*/
/*									*/
/************************************************************************/

void docInitDocumentFont(	DocumentFont *	df )
    {
    int		i;

    df->dfName= (char *)0;
    df->dfAltName= (char *)0;
    df->dfDocFontNumber= -1;
    df->dfStyleInt= DFstyleFNIL;
    df->dfPitch= FONTpitchDEFAULT;
    df->dfPanose[0]= '\0';

    for ( i= 0; i < FONTface_COUNT; i++ )
	{ df->dfPsFontInfo[i]= (AfmFontInfo *)0;	}

    utilInitIndexSet( &(df->dfUnicodesUsed) );
    utilInitIndexMapping( &(df->dfUnicodeToCharset) );

    df->dfCharsetIdxUsed[0]= -1;
    df->dfCharsetIdxUsedCount= 0;

    return;
    }

void docCleanDocumentFont(	DocumentFont *	df	)
    {
    if  ( df->dfName )
	{ free( df->dfName );	}

    if  ( df->dfAltName )
	{ free( df->dfAltName );	}

    utilCleanIndexSet( &(df->dfUnicodesUsed) );
    utilCleanIndexMapping( &(df->dfUnicodeToCharset) );
    }

int docFontSetFamilyName(		DocumentFont *	df,
					const char *	name )
    {
    char *	 saved= (char *)0;

    if  ( name )
	{
	saved= strdup( name );
	if  ( ! saved )
	    { XDEB(saved); return -1;	}
	}

    if  ( df->dfName )
	{ free( df->dfName );	}

    df->dfName= saved;

    return 0;
    }

int docFontSetAltName(		DocumentFont *	df,
				const char *	name )
    {
    char *	 saved= (char *)0;

    if  ( name )
	{
	saved= strdup( name );
	if  ( ! saved )
	    { XDEB(saved); return -1;	}
	}

    if  ( df->dfAltName )
	{ free( df->dfAltName );	}

    df->dfAltName= saved;

    return 0;
    }

int docFontSetFamilyStyle(	DocumentFont *	df,
				int		styleInt )
    {
    df->dfStyleInt= styleInt;
    return 0;
    }

int docCopyDocumentFont(	DocumentFont *		to,
				const DocumentFont *	from )
    {
    int			rval= 0;
    DocumentFont	copy;
    int			i;

    docInitDocumentFont( &copy );

    copy.dfDocFontNumber= from->dfDocFontNumber;
    copy.dfStyleInt= from->dfStyleInt;
    copy.dfPitch= from->dfPitch;
    memcpy( &copy.dfPanose, from->dfPanose, FONTlenPANOSE+ 1 );

    for ( i= 0; i < FONTface_COUNT; i++ )
	{ copy.dfPsFontInfo[i]= from->dfPsFontInfo[i];	}

    for ( i= 0; i < from->dfCharsetIdxUsedCount; i++ )
	{ copy.dfCharsetIdxUsed[i]= from->dfCharsetIdxUsed[i];	}
    copy.dfCharsetIdxUsedCount= from->dfCharsetIdxUsedCount;

    if  ( docFontSetFamilyName( &copy, from->dfName ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( docFontSetAltName( &copy, from->dfAltName ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( utilCopyIndexSet( &(copy.dfUnicodesUsed),
					    &(from->dfUnicodesUsed) ) < 0 )
	{ LDEB(1); return -1;	}

    docCleanDocumentFont( to );

    *to= copy; docInitDocumentFont( &copy ); /* steal */

  ready:

    docCleanDocumentFont( &copy );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Copy information on the match with the PostScript font from one	*/
/*  font to another. Usually this can be done safely as the PS list is	*/
/*  an application-wide constant.					*/
/*									*/
/************************************************************************/

void utilCopyFontFaceMatches(	DocumentFont *		dfTo,
				const DocumentFont *	dfFrom )
    {
    int		fac;

    for ( fac= 0; fac < FONTface_COUNT; fac++ )
	{ 
	if  ( ! dfTo->dfPsFontInfo[fac] )
	    { dfTo->dfPsFontInfo[fac]= dfFrom->dfPsFontInfo[fac]; }
	}
    }

/************************************************************************/
/*									*/
/*  Remove the character set suffix from a document font name.		*/
/*									*/
/*  In office documents, the same font in PostScript terms can occur	*/
/*  several times in the font list with a different character set and	*/
/*  with a suffix to the family name to differentiate between the	*/
/*  occurences.								*/
/*									*/
/*  1)  Find an office character set with the character set given. In	*/
/*	many cases, this does not really make sense: The font is given	*/
/*	with the 'default' character set.				*/
/*  2)  If an office character set was given, derive the family name	*/
/*	from the suffix of the character set. [I.E: remove it from the	*/
/*	name.]								*/
/*									*/
/************************************************************************/

int utilRemoveCharsetFromFontName(	DocumentFont *	df,
					int		charset )
    {
    int				charsetIdx= -1;
    const OfficeCharset *	oc= (const OfficeCharset *)0;
    const char *		suffix= (const char *)0;

    /*  1  */
    oc= utilGetOfficeCharsetByCharset( &charsetIdx, charset );
    if  ( ! oc )
	{
	oc= utilGetOfficeCharsetBySuffix( &charsetIdx, df->dfName );
	if  ( ! oc )
	    { return FONTcharsetDEFAULT;	}

	charset= oc->ocOfficeCharset;
	}

    /*  2  */
    suffix= oc->ocOfficeFontnameSuffix;
    if  ( suffix )
	{
	int			lenN= strlen( df->dfName );
	int			lenS= strlen( suffix );

	if  ( lenN > lenS					&&
	      ! strcmp( df->dfName+ lenN- lenS, suffix )	)
	    { df->dfName[lenN- lenS]= '\0';	}
	}

    return charset;
    }

