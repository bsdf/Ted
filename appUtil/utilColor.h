/************************************************************************/
/*									*/
/*  Color allocation, mainly for raster images.				*/
/*									*/
/************************************************************************/

#   ifndef	UTIL_COLOR_H
#   define	UTIL_COLOR_H

/************************************************************************/
/*									*/
/*  An RGB8 Color.							*/
/*  The most usual way to display images on a computer screen.		*/
/*									*/
/*  An RGB16 Color.							*/
/*  Used to match X11's way of handling colors.				*/
/*									*/
/*  NOTE that the structs use native types so the 8/16 indication is	*/
/*	more a suggestion than an implementation reality.		*/
/*									*/
/************************************************************************/

# define RGB8DEB(rgb8) appDebug("%s(%3d) %s= RGBA:%d:%d:%d:%d\n", \
				__FILE__, __LINE__, #rgb8, \
				(rgb8)->rgb8Red, (rgb8)->rgb8Green, \
				(rgb8)->rgb8Blue, (rgb8)->rgb8Alpha )

# define LRGB8DEB(l,rgb8) appDebug("%s(%3d) %s=%ld %s= RGBA:%d:%d:%d:%d\n", \
				__FILE__, __LINE__, \
				#l, ((long)l), #rgb8, \
				(rgb8)->rgb8Red, (rgb8)->rgb8Green, \
				(rgb8)->rgb8Blue, (rgb8)->rgb8Alpha )

typedef struct	RGB8Color
    {
    unsigned char	rgb8Red;
    unsigned char	rgb8Green;
    unsigned char	rgb8Blue;
    unsigned char	rgb8Alpha;
    } RGB8Color;

typedef struct	RGB16Color
    {
    unsigned short	rgb16Red;
    unsigned short	rgb16Green;
    unsigned short	rgb16Blue;
    unsigned char	rgb16Alpha;
    } RGB16Color;

typedef enum RGBAColorComponent
    {
    RGBAcompRED= 0,
    RGBAcompGREEN,
    RGBAcompBLUE,
    RGBAcompALPHA,

    RGBAcomp_COUNT
    } RGBAColorComponent;

/************************************************************************/
/*									*/
/*  Compare RGB8Colors.							*/
/*									*/
/************************************************************************/

# define bmRGB8ColorsDiffer( a, b ) ( \
			    (a)->rgb8Red != (b)->rgb8Red	|| \
			    (a)->rgb8Green != (b)->rgb8Green	|| \
			    (a)->rgb8Blue != (b)->rgb8Blue	|| \
			    (a)->rgb8Alpha != (b)->rgb8Alpha	)

# define utilRGB8SolidWhite( a ) { \
			    (a)->rgb8Red= (a)->rgb8Green= (a)->rgb8Blue= 255; \
			    (a)->rgb8Alpha= 255;	}

# define utilRGB8SolidBlack( a ) { \
			    (a)->rgb8Red= (a)->rgb8Green= (a)->rgb8Blue= 0; \
			    (a)->rgb8Alpha= 255;	}

/************************************************************************/
/*									*/
/*  Routine declarations:						*/
/*									*/
/************************************************************************/

extern void utilInitRGB8Color(		RGB8Color *		rgb8 );

extern int utilRGB8LumaChromaHue( int *			pLuma,
				int *			pChroma,
				int *			pHue,
				const RGB8Color *	rgb8 );

extern int utilRGB8FromLumaChromaHue(	RGB8Color *	rgb8,
					int		luma,
					int		chroma,
					int		hue );

extern int utilRGB8GetComponent(	const RGB8Color *	rgb8,
					int			prop );

extern void utilGetTopShadowColor(	RGB8Color *		top,
					const RGB8Color *	back );

extern void utilGetBottomShadowColor(	RGB8Color *		top,
					const RGB8Color *	back );

extern void utilGetForegroundColor(	RGB8Color *		bottom,
					const RGB8Color *	back );

#   endif	/*  UTIL_COLOR_H  */
