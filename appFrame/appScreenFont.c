/************************************************************************/
/*									*/
/*  Basic screen font management.					*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>
#   include	<stdio.h>

#   include	<geo2DInteger.h>
#   include	"drawImpl.h"
#   include	"drawScreenFontAdmin.h"

#   include	<appDebugon.h>

int drawFontImplementationNameImpl(
			char *				target,
			int				maxlen,
			const NumberedPropertiesList *	npl,
			int				screenFont )
    {
    const AfmFontInfo *			afi;
    const DrawScreenFont *		dsf;

    dsf= drawGetScreenFontByNumber( npl, screenFont );
    if  ( ! dsf )
	{ LXDEB(screenFont,dsf); return -1;	}
    afi= dsf->apfPsFontInfo;

#   ifdef USE_XFT
    if  ( dsf->dsfXftFont )
	{
	if  ( utilMemoryBufferIsEmpty( &(afi->afiFontFileName) ) 	||
	      afi->afiFontFileName.mbSize >= maxlen			)
	    { return -1;	}

	strcpy( target, utilMemoryBufferGetString( &(afi->afiFontFileName) ) );
	return 0;
	}
#   endif

#   ifdef	USE_X11_FONTS
    if  ( dsf->dsfEncodedFonts.esfFontCount > 0 )
	{
	if  ( maxlen < sizeof(dsf->apfFontName) )
	    { return -1;	}

	strcpy( target, dsf->apfFontName );

	return 0;
	}
#   endif

    return -1;
    }

