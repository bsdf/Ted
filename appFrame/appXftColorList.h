#   ifndef	APP_XFT_COLOR_LIST_H
#   define	APP_XFT_COLOR_LIST_H

#   include	"appGuiBase.h"
#   include	"drawImpl.h"
#   include	<utilIntegerValueNode.h>
#   include	<utilNumberedPropertiesAdmin.h>

#   ifdef USE_XFT

struct AppXftColorList;

typedef struct AppXftColor
    {
    XRenderColor		axcXRenderColor;
    unsigned char		axcXftColorAllocated;
    XftColor			axcXftColor;

    struct AppXftColorList *	axcList;
    } AppXftColor;

typedef struct AppXftColorList
    {
    Display *			axclDisplay;
    Visual *			axclVisual;
    Colormap			axclColormap;

    NumberedPropertiesList	axclPropertiesList;

				/**
				  *  Is one of the colors in the list.
				  *  Do not deallocate or clean!
				  */
    AppXftColor			axclCurrentColor;
    } AppXftColorList;

typedef void (*AppXftColorFunction)(	AppXftColor *		axc,
					int			n,
					void *			through );

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void appSolidXftColor(		AppXftColor *		to,
					const APP_COLOR_RGB *	from );

extern int appXftAllocateColor(		AppXftColor *		axc,
					AppXftColorList *	axcl );

extern void appInitAppXftColorList(	AppXftColorList *	axcl );

extern void appCleanAppXftColorList(	AppXftColorList *	axcl );

extern int appAppXftColorNumber(	AppXftColorList *	axcl,
					const AppXftColor *	axc );

extern void appGetAppXftColorByNumber(	AppXftColor *		axc,
					const AppXftColorList *	axcl,
					int			n );

#   endif
#   endif	/* APP_XFT_COLOR_LIST_H */
