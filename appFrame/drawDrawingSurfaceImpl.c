/************************************************************************/
/*									*/
/*  Drawing surface implementation implementation.			*/
/*									*/
/************************************************************************/

#   include "appFrameConfig.h"

#   include "guiWidgetDrawingSurface.h"
#   include <drawDrawingSurface.h>
#   include "drawDrawingSurfaceImpl.h"
#   include "drawScreenFontImpl.h"
#   include "drawScreenFontAdmin.h"
#   include "drawImplXft.h"
#   include "drawDrawingSurfacePrivate.h"
#   include "drawImpl.h"
#   include <appDebugon.h>

int drawFontImplementationName(
			char *				target,
			int				maxlen,
			const DrawingSurface		ds,
			int				screenFont )
    {
    return drawFontImplementationNameImpl( target, maxlen,
					&(ds->dsScreenFontAdmin), screenFont );
    }

void drawInitDrawingSurface(	DrawingSurface	ds )
    {
    ds->dsPoints= (APP_POINT *)0;
    ds->dsPointCount= 0;

    ds->dsIsPixmap= 0;

#   ifdef USE_XFT
    appInitAppXftColorList( &(ds->dsXftColorList) );
    ds->dsXftDrawable= (XftDraw *)0;
#   endif

    ds->dsColors= (AppColors *)0;

#   ifdef USE_GTK
    ds->dsDrawable= (GdkDrawable *)0;
    ds->dsGc= (GdkGC *)0;
#   endif

#   ifdef USE_MOTIF
    ds->dsDrawable= (Drawable)0;
    ds->dsDisplay= (Display *)0;
    ds->dsScreen= 0;
    ds->dsGc= (GC)0;
#   endif

    ds->dsAvoidFontconfig= 0;
    drawStartScreenFontList( &(ds->dsScreenFontAdmin) );

    return;
    }

