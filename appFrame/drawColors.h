#   ifndef	DRAW_COLORS_H
#   define	DRAW_COLORS_H

#   include	"appGuiBase.h"

#   include	<bmcolor.h>

# ifdef __cplusplus
extern "C"
    {
# endif

/************************************************************************/
/*									*/
/*  Administration on colors.						*/
/*									*/
/************************************************************************/

typedef struct AppColors
    {
    ColorAllocator	acAllocator;

#   ifdef		USE_MOTIF
    Display *		acDisplay;
    Colormap		acColormap;
    int			acVisualClass;
#   endif

#   ifdef		USE_GTK
    GdkColormap *	acColormap;
    int			acVisualClass;
#   endif
    } AppColors;

/************************************************************************/
/*									*/
/*  Procedure declarations.						*/
/*									*/
/************************************************************************/

extern void appInitColors(	AppColors *		ac );

extern int appColorRgb(		APP_COLOR_RGB *		xc,
				AppColors *		ac,
				unsigned int		r,
				unsigned int		g,
				unsigned int		b );

extern int appColorFindRgb(	APP_COLOR_RGB *		xc,
				AppColors *		ac,
				unsigned int		r,
				unsigned int		g,
				unsigned int		b );

extern int appColorNamed(	APP_COLOR_RGB *		xc,
				AppColors *		ac,
				const char *		name );

# ifdef __cplusplus
    }
# endif

#   endif	/*  DRAW_COLORS_H	*/
