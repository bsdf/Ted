#   ifndef	DRAW_SCREEN_FONT_IMPL_H
#   define	DRAW_SCREEN_FONT_IMPL_H

#   include	"appGuiBase.h"
#   include	"drawImpl.h"
#   include	<utilIndexMapping.h>
#   include	<psFontInfo.h>

#   ifdef USE_GTK
    typedef		GdkFont			(APP_FONT);
#   endif

#   ifdef USE_MOTIF
    typedef		XFontStruct		(APP_FONT);
#   endif

/************************************************************************/
/*									*/
/*  For the administration of the relation between PostScript fonts and	*/
/*  X11 fonts.								*/
/*									*/
/************************************************************************/

# define X_FONTNAME_MAX	100

typedef struct EncodedScreenFont
    {
    APP_FONT *			esfFontStruct;
    char			esfFontName[X_FONTNAME_MAX+ 1];
    unsigned char		esfIsTwoByte;
    unsigned char		esfFontBorrowed;
    const IndexMapping *	esfSymbolToByteMapping;
    } EncodedScreenFont;

typedef struct EncodedScreenFontList
    {
    EncodedScreenFont *	esfFonts;
    int			esfFontCount;
    IndexMapping	esflSymbolToFont;
    } EncodedScreenFontList;

#   define	DSFkeyFACE_NUMBER	0
#   define	DSFkeySIZE_PIXELS	1
#   define	DSFkey_COUNT		2

typedef struct DrawScreenFont
    {
				/*  KEY  */
    short int			apfPsFaceNumber;
    short int			dsfSizePixels;

				/*  DEP  */
    const AfmFontInfo *		apfPsFontInfo;
    int				apfUnderlinePositionPixels;
    int				apfUnderlineThicknessPixels;
    int				apfStrikethroughPositionPixels;
    int				apfStrikethroughThicknessPixels;
    int				apfSuperBaseline;
    int				apfSubBaseline;
    int				apfXHeightPixels;
    EncodedScreenFontList	dsfEncodedFonts;
    char			apfFontName[X_FONTNAME_MAX+ 1];

    const IndexSet *		dsfUnicodesUsed;

#   if defined(USE_XFT) || defined(USE_MOTIF)
    Display *			dsfDisplay;
    APP_DRAWABLE		dsfDrawable;
    APP_GC			dsfGc;
#   endif

#   ifdef USE_XFT
    XftDraw *			dsfXftDrawable;
    XftFont *			dsfXftFont;
#   endif
    } DrawScreenFont;

/************************************************************************/
/*									*/
/*  Procedure declarations.						*/
/*									*/
/************************************************************************/

extern void drawCleanScreenFont(		DrawScreenFont * 	dsf );
extern void drawInitScreenFont(			DrawScreenFont *	dsf );

extern int drawGetScreenFontKey(		const DrawScreenFont *	dsf,
						int			prop );

extern void drawCleanEncodedScreenFont(	DrawScreenFont *	dsf,
						EncodedScreenFont *	esf );

extern void drawInitEncodedScreenFont(	EncodedScreenFont *	esf );

extern int drawFontOpenScreenFont(
				DrawScreenFont *	dsf,
				int			avoidFontconfig );

extern XftFont * drawOpenXftFont(	DrawScreenFont *	dsf );

extern int drawTextExtentsXft(		DocumentRectangle *	drText,
					int			x,
					int			y,
					const DrawScreenFont *	hDsf,
					const DrawScreenFont *	vDsf,
					const char *		s,
					int			len );

#   ifdef USE_X11_FONTS

extern int appFontOpenX11Fonts(		DrawScreenFont *		dsf );

extern void addDrawX11GetFontProperties(	EncodedScreenFont *	esf,
						DrawScreenFont *	dsf );
#   endif

#   endif
