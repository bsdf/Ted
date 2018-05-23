/************************************************************************/
/*									*/
/*  A surface to draw upon. (Implementation)				*/
/*									*/
/************************************************************************/

#   include <drawDrawingSurface.h>
#   include "drawImpl.h"
#   include "appXftColorList.h"
#   include "drawColors.h"

struct DrawingSurface
    {
    AppColors *			dsColors;

    APP_POINT *			dsPoints;
    int				dsPointCount;

#   ifdef USE_MOTIF
    GC				dsGc;
    Drawable			dsDrawable;
    Display *			dsDisplay;
    int				dsScreen;
    Atom			dsCurrentFID;
#   endif

#   ifdef USE_GTK
    GdkGC *			dsGc;
    GdkDrawable *		dsDrawable;
#   endif

#   ifdef USE_XFT
    XftDraw *			dsXftDrawable;
    AppXftColorList		dsXftColorList;
#   endif

    unsigned char		dsIsPixmap;
    unsigned char		dsAvoidFontconfig;
    NumberedPropertiesList	dsScreenFontAdmin;
    };

/************************************************************************/
/*									*/
/*  Utility functions.							*/
/*									*/
/************************************************************************/

extern void drawInitDrawingSurface(	DrawingSurface	ds );

void drawSetSystemColor(	DrawingSurface			ds,
				APP_COLOR_RGB *			xc );
