/************************************************************************/
/*									*/
/*  Keep a collection XftColors by number.				*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<string.h>
#   include	<limits.h>
#   include	<utilColor.h>
#   include	"drawImpl.h"

#   include	<appDebugon.h>

#   ifdef USE_XFT

#   include	"appXftColorList.h"

/************************************************************************/
/*									*/
/*  1)  Number of text attributes per page in the list.			*/
/*									*/
/************************************************************************/

#   define	AXCLsizePAGE	256

static void appInitAppXftColor(	AppXftColor *	axc )
    {
    /* KEY */
    axc->axcXRenderColor.red= 0;
    axc->axcXRenderColor.green= 0;
    axc->axcXRenderColor.blue= 0;
    axc->axcXRenderColor.alpha= 0;

    /* DEP */
    axc->axcXftColorAllocated= 0;
    axc->axcList= (AppXftColorList *)0;

    return;
    }

static void appCleanAppXftColor(	AppXftColor *	axc )
    {
    if  ( axc->axcXftColorAllocated && axc->axcList )
	{
	AppXftColorList *	axcl= axc->axcList;

	XftColorFree( axcl->axclDisplay, axcl->axclVisual,
				    axcl->axclColormap, &(axc->axcXftColor) );
	}

    return;
    }

int appXftAllocateColor(	AppXftColor *		axc,
				AppXftColorList *	axcl )
    {
    int			num= appAppXftColorNumber( axcl, axc );
    AppXftColor *	got;

    if  ( num < 0 )
	{ LDEB(num); return -1;	}

    got= (AppXftColor *)utilPagedListGetItemByNumber(
			    &(axcl->axclPropertiesList.nplPagedList), num );
    if  ( ! got )
	{ LXDEB(num,got); return -1;	}

    if  ( ! got->axcXftColorAllocated )
	{
	if  ( ! XftColorAllocValue( axcl->axclDisplay,
						axcl->axclVisual,
						axcl->axclColormap, 
						&(got->axcXRenderColor),
						&(got->axcXftColor) ) )
	    { LDEB(1); return -1;	}

	got->axcXftColorAllocated= 1;
	got->axcList= axcl;
	}

    axc->axcXftColor= got->axcXftColor;
    axc->axcXftColorAllocated= got->axcXftColorAllocated;
    return 0;
    }

void appSolidXftColor(		AppXftColor *		to,
				const APP_COLOR_RGB *	from )
    {
    const int	solid= USHRT_MAX;

    /* KEY */
    if  ( to->axcXRenderColor.red == from->red		&&
	  to->axcXRenderColor.green == from->green	&&
	  to->axcXRenderColor.blue == from->blue	&&
	  to->axcXRenderColor.alpha == solid		)
	{ return;	}

    to->axcXRenderColor.red= from->red;
    to->axcXRenderColor.green= from->green;
    to->axcXRenderColor.blue= from->blue;
    to->axcXRenderColor.alpha= solid;

    /* DEP */
    to->axcXftColorAllocated= 0;

    return;
    }

static int appXftColorGetProperty(	const AppXftColor *	axc,
					int			prop )
    {
    switch( prop )
	{
	case RGBAcompRED:
	    return axc->axcXRenderColor.red;
	case RGBAcompGREEN:
	    return axc->axcXRenderColor.green;
	case RGBAcompBLUE:
	    return axc->axcXRenderColor.blue;
	case RGBAcompALPHA:
	    return axc->axcXRenderColor.alpha;

	default:
	    LDEB(prop); return -1;
	}
    }

/************************************************************************/
/*									*/
/*  Initialize/Clean attribute administration.				*/
/*									*/
/************************************************************************/

void appInitAppXftColorList(	AppXftColorList *	axcl )
    {
    int			num;

    axcl->axclDisplay= (Display *)0;
    axcl->axclVisual= (Visual *)0;
    axcl->axclColormap= (Colormap)0;

    utilInitNumberedPropertiesList( &(axcl->axclPropertiesList) );

    utilStartNumberedPropertyList( &(axcl->axclPropertiesList),
			RGBAcomp_COUNT,
			(NumberedPropertiesGetProperty)appXftColorGetProperty,

			sizeof(AppXftColor),
			(InitPagedListItem)appInitAppXftColor,
			(CleanPagedListItem)appCleanAppXftColor );

    appInitAppXftColor( &(axcl->axclCurrentColor) );

    num= appAppXftColorNumber( axcl, &(axcl->axclCurrentColor) );
    if  ( num != 0 )
	{ LDEB(num);	}

    return;
    }

void appCleanAppXftColorList(	AppXftColorList *	axcl )
    {
    utilCleanNumberedPropertiesList( &(axcl->axclPropertiesList) );

    return;
    }

/************************************************************************/
/*									*/
/*  Translate a color number to a struct value.				*/
/*									*/
/************************************************************************/

void appGetAppXftColorByNumber(	AppXftColor *			axc,
				const AppXftColorList *		axcl,
				int				n )
    {
    void *	vaxc;

    vaxc= utilPagedListGetItemByNumber(
				&(axcl->axclPropertiesList.nplPagedList), n );
    if  ( ! vaxc )
	{ LXDEB(n,vaxc); appInitAppXftColor( axc ); return;	}

    *axc= *((AppXftColor *)vaxc);
    return;
    }

/************************************************************************/
/*									*/
/*  Translate a text attribute to its number.				*/
/*									*/
/************************************************************************/

int appAppXftColorNumber(	AppXftColorList *		axcl,
				const AppXftColor *		axc )
    {
    const int	make= 1;
    return utilGetPropertyNumber( &(axcl->axclPropertiesList),
						    make, (void *)axc );
    }

#   endif
