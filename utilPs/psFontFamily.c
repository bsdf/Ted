#   include	"utilPsConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<ctype.h>

#   include	"psFontFamily.h"
#   include	"psCompareFontInfo.h"

#   include	<appDebugon.h>

/************************************************************************/

static int appPsSetFontInfo(	PsFontFamily *		aff,
				int			n,
				AfmFontInfo *		afi )
    {
    int		fl;

    aff->psfFaces[n]= afi;

    if  ( afi->afiIsFixedPitch )
	{ aff->psfHasFixedWidth= 1;		}
    else{ aff->psfHasProportionalWidth= 1;	}

    fl= strlen( afi->afiFamilyName );

    /* HACK */
    if  ( strncmp( afi->afiFamilyName, afi->afiFullName, fl ) )
	{
	if  ( ! strcmp( afi->afiFamilyName, "FreeMono" ) )
	    {
	    free( afi->afiFamilyName );
	    afi->afiFamilyName= strdup( "Free Monospaced" );
	    fl= strlen( afi->afiFamilyName );
	    aff->psfFontFamilyName= afi->afiFamilyName;
	    }
	if  ( ! strcmp( afi->afiFamilyName, "FreeSans" ) )
	    {
	    free( afi->afiFamilyName );
	    afi->afiFamilyName= strdup( "Free Sans" );
	    fl= strlen( afi->afiFamilyName );
	    aff->psfFontFamilyName= afi->afiFamilyName;
	    }
	if  ( ! strcmp( afi->afiFamilyName, "FreeSerif" ) )
	    {
	    free( afi->afiFamilyName );
	    afi->afiFamilyName= strdup( "Free Serif" );
	    fl= strlen( afi->afiFamilyName );
	    aff->psfFontFamilyName= afi->afiFamilyName;
	    }
	if  ( ! strcmp( afi->afiFamilyName, "TlwgTypewriter" ) )
	    {
	    free( afi->afiFamilyName );
	    afi->afiFamilyName= strdup( "Tlwg Typewriter" );
	    fl= strlen( afi->afiFamilyName );
	    aff->psfFontFamilyName= afi->afiFamilyName;
	    }
	if  ( ! strcmp( afi->afiFamilyName, "TlwgMono" ) )
	    {
	    free( afi->afiFamilyName );
	    afi->afiFamilyName= strdup( "Tlwg Mono" );
	    fl= strlen( afi->afiFamilyName );
	    aff->psfFontFamilyName= afi->afiFamilyName;
	    }
	}

    /*  1  */
    if  ( ! strncmp( afi->afiFamilyName, afi->afiFullName, fl )	&&
	  afi->afiFullName[fl] == '-'			)
	{ afi->afiFullName[fl]=   ' ';	}

    /*  2  */
    if  ( ! strncmp( afi->afiFamilyName, afi->afiFullName, fl )	&&
	  ( afi->afiFullName[fl] == ' '		||
	    afi->afiFullName[fl] == '\0'	)	)
	{
	afi->afiStyle= afi->afiFullName+ fl;
	while( afi->afiStyle[0] == ' ' )
	    { afi->afiStyle++;	}
	if  ( ! afi->afiStyle[0] )
	    { afi->afiStyle= afi->afiWeightStr;	}
	}
    else{
	SSDEB(afi->afiFamilyName,afi->afiFullName);
	if  ( afi->afiWeightStr )
	    { afi->afiStyle= afi->afiWeightStr;	}
	else{ afi->afiStyle= afi->afiFullName;	}
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Set the faces of a font family.					*/
/*									*/
/*  1)  Some hacked fonts have a hyphen here.				*/
/*  2)  The face name is the tail of the full name.			*/
/*									*/
/************************************************************************/

static int appPsSetFontFaces(	PsFontFamily *		aff,
				AfmFontInfo **		afis,
				int			count )
    {
    AfmFontInfo **	saved;
    int			face;

    if  ( count == 0 )
	{ LDEB(count); return 0;	}

    saved= (AfmFontInfo **)realloc( aff->psfFaces,
					count* sizeof(AfmFontInfo *) );
    if  ( ! saved )
	{ LXDEB(count,saved); return -1;	}
    aff->psfFaces= saved;

    aff->psfFontFamilyName_Orig= afis[0]->afiFamilyName_Orig;
    aff->psfFontFamilyName= afis[0]->afiFamilyName;

    appPsSetFontInfo( aff, 0, afis[0] );
    aff->psfWidthInt= afis[0]->afiWidthInt;

    for ( face= 1; face < count; face++ )
	{
	if  ( afis[face]->afiIsFixedPitch )
	    {
	    if  ( aff->psfHasProportionalWidth )
		{
		SLDEB(afis[face]->afiFullName,afis[face]->afiIsFixedPitch);
		}
	    }
	else{
	    /*
	    "Nimbus Mono L Regular Oblique"
	    "Nimbus Mono L Bold"
	    "Nimbus Mono L Bold Oblique"
	    if  ( aff->psfHasFixedWidth )
		{ SLDEB(afis[face]->afiFullName,afis[face]->afiIsFixedPitch); }
	    */
	    }

	appPsSetFontInfo( aff, face, afis[face] );
	}

    aff->psfFaceCount= count;

    if  ( aff->psfFaceCount > 0 )
	{
	qsort( aff->psfFaces, aff->psfFaceCount, sizeof(AfmFontInfo *),
					psFontCompareInfosExcludingFamily );
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Consume as many infos from a sorted array as belong to a single	*/
/*  family. Make a family from them.					*/
/*									*/
/************************************************************************/

PsFontFamily * psFontFamilyFromFaces(	AfmFontInfo **	afis,
					int *		pConsumed,
					int		count )
    {
    PsFontFamily *	rval= (PsFontFamily *)0;
    PsFontFamily *	aff= (PsFontFamily *)0;
    int			n;

    for ( n= 1; n < count; n++ )
	{
	if  ( strcmp( afis[n]->afiFamilyName, afis[0]->afiFamilyName )	||
	      afis[n]->afiWidthInt != afis[0]->afiWidthInt		)
	    { break;	}
	}

    aff= (PsFontFamily *)malloc( sizeof(PsFontFamily) );
    if  ( ! aff )
	{ XDEB(aff); goto ready;	}
    psInitFontFamily( aff );

    if  ( appPsSetFontFaces( aff, afis, n ) )
	{ LDEB(n); goto ready;	}

    rval= aff; aff= (PsFontFamily *)0;
    *pConsumed= n;

  ready:

    if  ( aff )
	{ psFreeFontFamily( aff );	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Initialise a font family struct.					*/
/*									*/
/************************************************************************/

void psInitFontFamily(	PsFontFamily *	aff )
    {
    aff->psfFontFamilyName_Orig= (char *)0;
    aff->psfFontFamilyName= (char *)0;
    aff->psfFaces= (AfmFontInfo **)0;
    aff->psfFaceCount= 0;
    aff->psfHasFixedWidth= 0;
    aff->psfHasProportionalWidth= 0;

    return;
    }

static void psCleanFontFamily( PsFontFamily * aff )
    {
    /*  aff->psfFontFamilyName_Orig: Managed by afis	*/
    /*  aff->psfFontFamilyName: Managed by afis		*/

    if  ( aff->psfFaces )
	{ free( aff->psfFaces );	}
    }

void psFreeFontFamily( PsFontFamily * aff )
    {
    psCleanFontFamily( aff );

    free( aff );
    }
