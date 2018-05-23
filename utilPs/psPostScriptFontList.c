#   include	"utilPsConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<ctype.h>

#   include	"psPostScriptFontList.h"
#   include	"psCompareFontInfo.h"
#   include	"psFontName.h"
#   include	<utilTree.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Bookkeeping on font lists.						*/
/*									*/
/************************************************************************/

void psInitPostScriptFontList(	PostScriptFontList *	psfl )
    {
    psfl->psflFamilies= (PsFontFamily **)0;
    psfl->psflFamilyCount= 0;
    psfl->psflInfos= (AfmFontInfo **)0;
    psfl->psflInfoCount= 0;

    psfl->psflAvoidFontconfig= 0;

    psfl->psflFontNameToInfo= (void *)0;
    psfl->psflFontFileToInfo= (void *)0;
    }

void psCleanPostScriptFontList(	PostScriptFontList *	psfl )
    {
    int			i;

    for ( i= 0; i < psfl->psflFamilyCount; i++ )
	{
	if  ( psfl->psflFamilies[i] )
	    { psFreeFontFamily( psfl->psflFamilies[i] );	}
	}
    if  ( psfl->psflFamilies )
	{ free( psfl->psflFamilies );	}

    for ( i= 0; i < psfl->psflInfoCount; i++ )
	{
	if  ( psfl->psflInfos[i] )
	    { psFreeAfmFontInfo( psfl->psflInfos[i] );	}
	}
    if  ( psfl->psflInfos )
	{ free( psfl->psflInfos );	}

    if  ( psfl->psflFontNameToInfo )
	{
	utilTreeFreeTree( psfl->psflFontNameToInfo,
					(UTIL_TREE_CALLBACK)0, (void *)0 );
	}
    if  ( psfl->psflFontFileToInfo )
	{
	utilTreeFreeTree( psfl->psflFontFileToInfo,
					(UTIL_TREE_CALLBACK)0, (void *)0 );
	}

    return;
    }

int psPostScriptFontListInfosToFamilies( PostScriptFontList *	psfl )
    {
    int		from;

    for ( from= 0; from < psfl->psflInfoCount; from++ )
	{ psFontInfoMoveWidthToFamilyName( psfl->psflInfos[from] );	}

    qsort( psfl->psflInfos, psfl->psflInfoCount,
		    sizeof(AfmFontInfo *), psFontCompareInfosIncludingFamily );

    for ( from= 0; from < psfl->psflInfoCount; from++ )
	{ psfl->psflInfos[from]->afiFaceNumber= from;	}

    from= 0;
    while( from < psfl->psflInfoCount )
	{
	int		consumed= 0;
	PsFontFamily *	aff;

	aff= psFontFamilyFromFaces( psfl->psflInfos+ from, &consumed,
						psfl->psflInfoCount- from );

	if  ( ! aff )
	    { XDEB(aff); return -1;	}

	if  ( psPostScriptFontListAddFamily( psfl, aff ) )
	    { LDEB(1); return -1;	}

	from += consumed;
	}

    return 0;
    }

int psPostScriptFontListAddInfo(	PostScriptFontList *	psfl,
					AfmFontInfo *		afi )
    {
    AfmFontInfo **		fresh;
    void *			vprev= (void *)0;

    fresh= (AfmFontInfo **)realloc( psfl->psflInfos,
			( psfl->psflInfoCount+ 1 )* sizeof(AfmFontInfo *) );
    if  ( ! fresh )
	{ XDEB(fresh); return -1;	}

    psfl->psflInfos= fresh;
    psfl->psflInfos[psfl->psflInfoCount]= afi;
    afi->afiFaceNumber= psfl->psflInfoCount++;

    if  ( ! psfl->psflFontNameToInfo )
	{
	psfl->psflFontNameToInfo= utilTreeMakeTree( 1 );
	if  ( ! psfl->psflFontNameToInfo )
	    { XDEB(psfl->psflFontNameToInfo); return -1;	}
	}

    if  ( utilTreeStoreValue( psfl->psflFontNameToInfo, &vprev,
				    (const char **)0, afi->afiFontName, afi ) )
	{ SDEB(afi->afiFontName);	}
    if  ( vprev )
	{ SXDEB(afi->afiFontName,vprev);	}

    if  ( ! utilMemoryBufferIsEmpty( &(afi->afiFontFileName) ) )
	{
	if  ( ! psfl->psflFontFileToInfo )
	    {
	    psfl->psflFontFileToInfo= utilTreeMakeTree( 1 );
	    if  ( ! psfl->psflFontFileToInfo )
		{ XDEB(psfl->psflFontFileToInfo); return -1;	}
	    }

	if  ( utilTreeStoreValue( psfl->psflFontFileToInfo, &vprev,
			(const char **)0,
			utilMemoryBufferGetString( &(afi->afiFontFileName) ),
			afi ) )
	    { LDEB(1);	}
	if  ( vprev )
	    {
	    /*
	    SXDEB(afi->afiFontFileName,vprev);
	    SDEB(afi->afiFontName);
	    SDEB(((AfmFontInfo *)vprev)->afiFontName);
	    */
	    }
	}

    return 0;
    }

int psPostScriptFontListAddFamily(	PostScriptFontList *	psfl,
					PsFontFamily *		aff )
    {
    PsFontFamily **		fresh;

    fresh= (PsFontFamily **)realloc( psfl->psflFamilies,
			( psfl->psflFamilyCount+ 1 )* sizeof(PsFontFamily *) );
    if  ( ! fresh )
	{ XDEB(fresh); return -1;	}

    psfl->psflFamilies= fresh;
    psfl->psflFamilies[psfl->psflFamilyCount++]= aff;
    return 0;
    }

int psPostScriptFontListGetFamilyByName(const PostScriptFontList *	psfl,
					const char *			name )
    {
    int				fam;

    for ( fam= 0; fam < psfl->psflFamilyCount; fam++ )
	{
	const PsFontFamily *	aff= psfl->psflFamilies[fam];

	if  ( ! aff || ! aff->psfFontFamilyName )
	    { continue;	}

	if  ( ! strcmp( aff->psfFontFamilyName, name ) )
	    { return fam;	}
	}

    return -1;
    }

AfmFontInfo * psPostScriptFontListGetFontInfoByFaceFile(
					const PostScriptFontList *	psfl,
					const char *			file )
    {
    AfmFontInfo *		afi;

    if  ( ! psfl->psflFontFileToInfo )
	{ return (AfmFontInfo *)0;	}

    afi= (AfmFontInfo *)utilTreeGetEQ( psfl->psflFontFileToInfo,
							(const char **)0, file );
    return afi;
    }

int psPostScriptFontListSetFontInfoForFaceFile(
					PostScriptFontList *		psfl,
					AfmFontInfo *			afi,
					const char *			file )
    {
    if  ( ! psfl->psflFontFileToInfo )
	{
	psfl->psflFontFileToInfo= utilTreeMakeTree( 1 );
	if  ( ! psfl->psflFontFileToInfo )
	    { XDEB(psfl->psflFontFileToInfo); return -1;	}
	}

    if  ( utilTreeStoreValue( psfl->psflFontFileToInfo, (void **)0,
						(const char **)0, file, afi ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

AfmFontInfo * psPostScriptFontListGetFontInfoByFontName(
					const PostScriptFontList *	psfl,
					const char *			name )
    {
    AfmFontInfo *		afi;

    if  ( ! psfl->psflFontNameToInfo )
	{ return (AfmFontInfo *)0;	}

    afi= (AfmFontInfo *)utilTreeGetEQ( psfl->psflFontNameToInfo,
							(const char **)0, name );
    return afi;
    }
