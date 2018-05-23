/************************************************************************/
/*									*/
/*  Basic screen font management.					*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>
#   include	<stdio.h>

#   include	"drawImpl.h"
#   include	"drawScreenFontImpl.h"

#   include	<appDebugon.h>

void drawCleanEncodedScreenFont(	DrawScreenFont *	dsf,
					EncodedScreenFont *	esf )
    {
    if  ( esf->esfFontStruct )
	{
#	ifdef USE_MOTIF
	if  ( ! esf->esfFontBorrowed )
	    { XFreeFont( dsf->dsfDisplay, esf->esfFontStruct );	}
#	endif

#	ifdef USE_GTK
	gdk_font_unref( esf->esfFontStruct );
#	endif
	}

    return;
    }

static void drawCleanEncodedScreenFontList(	DrawScreenFont *	dsf,
						EncodedScreenFontList * esfl )
    {
    int			i;

    for ( i= 0; i < esfl->esfFontCount; i++ )
	{ drawCleanEncodedScreenFont( dsf, esfl->esfFonts+ i ); }

    if  ( esfl->esfFonts )
	{ free( esfl->esfFonts );	}

    utilCleanIndexMapping( &(esfl->esflSymbolToFont) );
    }

void drawCleanScreenFont(	DrawScreenFont *	dsf )
    {
#   ifdef USE_XFT
    if  ( dsf->dsfXftFont )
	{ XftFontClose( dsf->dsfDisplay, dsf->dsfXftFont );	}
#   endif

    drawCleanEncodedScreenFontList( dsf, &(dsf->dsfEncodedFonts) );

    return;
    }

static void drawInitEncodedScreenFontList(	EncodedScreenFontList * esfl )
    {
    esfl->esfFonts= (EncodedScreenFont *)0;
    esfl->esfFontCount= 0;

    utilInitIndexMapping( &(esfl->esflSymbolToFont) );
    }

void drawInitScreenFont(	DrawScreenFont *	dsf )
    {
    /*  KEY  */
    dsf->apfPsFaceNumber= -1;
    dsf->dsfSizePixels= 0;

    /*  DEP  */
    dsf->apfPsFontInfo= (AfmFontInfo *)0;
    dsf->apfUnderlinePositionPixels= 0;
    dsf->apfUnderlineThicknessPixels= 0;
    dsf->apfStrikethroughPositionPixels= 0;
    dsf->apfStrikethroughThicknessPixels= 0;
    dsf->apfSuperBaseline= 0;
    dsf->apfSubBaseline= 0;
    dsf->apfXHeightPixels= 0;

    drawInitEncodedScreenFontList( &(dsf->dsfEncodedFonts) );

    dsf->apfFontName[0]= '\0';

    dsf->dsfUnicodesUsed= (const IndexSet *)0;

#   if defined(USE_XFT) || defined(USE_MOTIF)
    dsf->dsfDisplay= (Display *)0;
    dsf->dsfDrawable= (APP_DRAWABLE)0;
    dsf->dsfGc= (APP_GC)0;
#   endif

#   ifdef USE_XFT
    dsf->dsfXftDrawable= (XftDraw *)0;
    dsf->dsfXftFont= (XftFont *)0;
#   endif

    return;
    }

void drawInitEncodedScreenFont(	EncodedScreenFont *	esf )
    {
    esf->esfFontStruct= (APP_FONT *)0;
    esf->esfFontName[0]= '\0';
    esf->esfIsTwoByte= 0;
    esf->esfFontBorrowed= 0;
    esf->esfSymbolToByteMapping= (IndexMapping *)0;

    return;
    }

int drawGetScreenFontKey(		const DrawScreenFont *	dsf,
					int			prop )
    {
    switch( prop )
	{
	case DSFkeyFACE_NUMBER:
	    return dsf->apfPsFaceNumber;
	case DSFkeySIZE_PIXELS:
	    return dsf->dsfSizePixels;
	default:
	    LDEB(prop); return -1;
	}
    }

/************************************************************************/
/*									*/
/*  Obtain the system resources for a screen font.			*/
/*									*/
/*  1)  Is it an open font? If so return its number.			*/
/*  2)  Claim memory.							*/
/*  3)  Obtain a list of the available fonts.				*/
/*									*/
/************************************************************************/

int drawFontOpenScreenFont(	DrawScreenFont *	dsf,
				int			avoidFontconfig )
    {
    int				openFullFont= 1;

    if  ( dsf->apfPsFaceNumber < 0 || ! dsf->apfPsFontInfo )
	{ LXDEB(dsf->apfPsFaceNumber,dsf->apfPsFontInfo); return -1;	}

    /********************************************************************/
    /*  Open freetype fonts (if configured)				*/
    /********************************************************************/
#   ifdef USE_XFT
    if  ( ! avoidFontconfig )
	{
	if  ( openFullFont )
	    {
	    dsf->dsfXftFont= drawOpenXftFont( dsf );
	    if  ( dsf->dsfXftFont )
		{ openFullFont= 0; }
	    }
	}
#   endif

#   ifdef USE_X11_FONTS
    /********************************************************************/
    /*  Open X11 fonts (if freetype is not configured, or it failed)	*/
    /********************************************************************/
    if  ( openFullFont )
	{
	if  ( ! appFontOpenX11Fonts( dsf ) )
	    { openFullFont= 0; }
	}
#   endif

    if  ( openFullFont )
	{
	SLDEB(dsf->apfPsFontInfo->afiFontName,openFullFont);
	return -1;
	}

    return 0;
    }
