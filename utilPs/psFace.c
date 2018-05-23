/************************************************************************/
/*									*/
/*  Keep a list of font faces.						*/
/*									*/
/************************************************************************/

#   include	"utilPsConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<stdlib.h>
#   include	<string.h>
#   include	<time.h>

#   include	<utilTree.h>
#   include	"psFace.h"
#   include	"psGlyphs.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Find out what pages in a postscipt face are used.			*/
/*									*/
/************************************************************************/

static int psForOneUnicode(	int		code,
				int		glyph,
				void *		voidpsf )
    {
    PostScriptFace *		psf= (PostScriptFace *)voidpsf;
    const AfmFontInfo *		afi= psf->psfAfi;

    const AfmCharMetric *	acm= afi->afiMetrics[glyph];
    int				page= UtoPAGE(code);
    const char *		stdName= psUnicodeToGlyphName( code );

    if  ( ! acm || ! acm->acmN || ! acm->acmN[0] )
	{ return 0;	}

    psf->psfPageUsed[page]= 1;

    if  ( strcmp( acm->acmN, stdName ) )
	{ psf->psfPageNonStd[page]= 1;	}

    return 0;
    }

static void psFindFontEncodingPages(
				PostScriptFace *		psf )
    {
    const AfmFontInfo *		afi= psf->psfAfi;

    if  ( utilIndexMappingForAll( &(afi->afiUnicodeToGlyphMapping),
					    psForOneUnicode, (void *)psf ) )
	{ SDEB(afi->afiFontName);	}

    return;
    }

/************************************************************************/
/*									*/
/*  Collect the fonts in a document					*/
/*  This has two functions:						*/
/*	1)  They are needed for the DSC comments.			*/
/*	2)  This makes it possible to define shorthands in the prologue	*/
/*									*/
/************************************************************************/

int psRememberPostsciptFace(		PostScriptTypeList *	pstl,
					const AfmFontInfo *	afi,
					const TextAttribute *	ta,
					const char *		prefix,
					int			appearsInText )
    {
    PostScriptFace *	psf;
    const char *	fontName;
    int			faceIndex;

    char		faceId[FACElenID+1];
    char *		s;

    int			ref;
    FaceReference *	fr;

    if  ( ! pstl->pstlFaceTree )
	{
	const int	ownKeys= 1;

	pstl->pstlFaceTree= utilTreeMakeTree( ownKeys );
	if  ( ! pstl->pstlFaceTree )
	    { XDEB(pstl->pstlFaceTree); return -1;	}
	}

    psf= (PostScriptFace *)utilTreeGetEQ( pstl->pstlFaceTree,
						&fontName, afi->afiFontName );
    if  ( ! psf )
	{
	int	page;
	void *	prevPsf= (void *)0;

	psf= (PostScriptFace *)malloc( sizeof(PostScriptFace) );
	if  ( ! psf )
	    { XDEB(psf); return -1;	}

	psf->psfAfi= afi;

	psf->psfReferences= (FaceReference *)0;
	psf->psfReferenceCount= 0;
	psf->psfAppearsInText= 0;
	psf->psfEmbed= PSembedUNKNOWN;
	utilInitMemoryBuffer( &(psf->psfFontFileName) );
	psf->psfFontFileIndex= 0;

	memset( psf->psfPageUsed, 0, sizeof(psf->psfPageUsed) );
	memset( psf->psfPageNonStd, 0, sizeof(psf->psfPageNonStd) );

	if  ( utilTreeStoreValue( pstl->pstlFaceTree, &prevPsf,
					&fontName, afi->afiFontName,  psf ) )
	    { SDEB(afi->afiFontName); free( psf ); return -1; }

	psFindFontEncodingPages( psf );

	for ( page= 0; page < UNPAGE; page++ )
	    {
	    if  ( psf->psfPageUsed[page] )
		{ pstl->pstlPageUsed[page]= 1;	}
	    }
	}

    faceIndex= FACE_INDEX( ta->taFontIsSlanted, ta->taFontIsBold );

    s= faceId;
    sprintf( s, "%s%d", prefix, ta->taFontNumber );
    s += strlen( s );
    if  ( ta->taFontIsBold )
	{ strcpy( s, "b" ); s++;	}
    if  ( ta->taFontIsSlanted )
	{ strcpy( s, "i" ); s++;	}

    fr= psf->psfReferences;
    for ( ref= 0; ref < psf->psfReferenceCount; fr++, ref++ )
	{
	if  ( fr->frDocFontIndex == ta->taFontNumber	&&
	      fr->frDocFaceIndex == faceIndex		&&
	      ! strcmp( fr->frFaceId, faceId )		)
	    { break;	}
	}

    if  ( ref >= psf->psfReferenceCount )
	{
	fr= (FaceReference *)realloc( psf->psfReferences,
					    (ref+1)* sizeof(FaceReference) );
	if  ( ! fr )
	    { LXDEB(psf->psfReferenceCount,fr); return -1;	}
	psf->psfReferences= fr;
	fr += psf->psfReferenceCount++;

	fr->frDocFontIndex= ta->taFontNumber;
	fr->frDocFaceIndex= faceIndex;
	strcpy( fr->frFaceId, faceId );
	fr->frAppearsInText= appearsInText;
	}

    if  ( appearsInText )
	{
	fr->frAppearsInText= appearsInText;
	psf->psfAppearsInText= appearsInText;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Bookkeeping.							*/
/*									*/
/************************************************************************/

void psInitPostScriptFaceList( PostScriptTypeList *	pstl )
    {
    pstl->pstlFaceTree= (void *)0;
    pstl->pstlFaceCount= 0;

    utilInitMemoryBuffer( &(pstl->pstlFontDirectory) );

    memset( pstl->pstlPageUsed, 0, sizeof(pstl->pstlPageUsed) );

    return;
    }

static int utilFreePostScriptFace(	const char *	key,
					void *		vpsf,
					void *		through )
    {
    PostScriptFace *	psf= (PostScriptFace *)vpsf;

    if  ( psf )
	{
	if  ( psf->psfReferences )
	    { free( psf->psfReferences );	}

	utilCleanMemoryBuffer( &(psf->psfFontFileName) );

	free( psf );
	}

    return 0;
    }

void psCleanPostScriptFaceList( PostScriptTypeList *	pstl )
    {
    utilCleanMemoryBuffer( &(pstl->pstlFontDirectory) );

    if  ( pstl->pstlFaceTree )
	{
	utilTreeFreeTree( pstl->pstlFaceTree,
				    utilFreePostScriptFace, (void *)0 );
	}

    return;
    }

