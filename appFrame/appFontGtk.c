/************************************************************************/
/*									*/
/*  GTK font related code.						*/
/*  X11 structures.							*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"
#   include	"appGuiBase.h"
#   include	"appScreenFont.h"
#   include	"appMatchFont.h"

#   ifdef	USE_GTK

#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<ctype.h>
#   include	<string.h>

#   include	<appDebugon.h>
#   include	<utilPropMask.h>

#   include	"drawScreenFontAdmin.h"
#   include	"drawDrawingSurfacePrivate.h"
#   include	<bitmap.h>
#   include	<psDocumentFontStyle.h>

/************************************************************************/
/*									*/
/*  Open the default font belonging to a widget. (Or the system)	*/
/*									*/
/************************************************************************/

int drawOpenDefaultFont( DrawingSurface			ds,
			const PostScriptFontList *	psfl,
			APP_WIDGET			w,
			int				fontSizeHintPixels )
    {
    const char *	fontFamilyName= "Sans";
    int			pixelSize= fontSizeHintPixels;
    TextAttribute	ta;
    AfmFontInfo *	afi;

    utilInitTextAttribute( &ta );

    if  ( w )
	{
	GtkStyle *	gs= gtk_widget_get_style( w );

	if  ( gs )
	    {
	    if  ( gs->font_desc )
		{
		const char *	fam;
		int		style;
		int		weight;

		fam= pango_font_description_get_family( gs->font_desc );
		if  ( fam )
		    { fontFamilyName= fam;	}

		if  ( fontSizeHintPixels <= 0				&&
		      pango_font_description_get_size_is_absolute(
							gs->font_desc )	)
		    {
		    int		sz;

		    sz= pango_font_description_get_size( gs->font_desc );
		    if  ( sz > 0 )
			{ pixelSize= sz;	}
		    }

		style= pango_font_description_get_style( gs->font_desc );
		if  ( style == PANGO_STYLE_OBLIQUE	||
		      style == PANGO_STYLE_ITALIC	)
		    { ta.taFontIsSlanted= 1;	}

		weight= pango_font_description_get_weight( gs->font_desc );
		if  ( weight >=
			( PANGO_WEIGHT_NORMAL+ PANGO_WEIGHT_SEMIBOLD ) / 2 )
		    { ta.taFontIsBold= 1;	}
		}
	    else{
		GdkFont *	gf;

#		if GTK_MAJOR_VERSION < 2
		gf= gs->font;
#		else
		gf= gtk_style_get_font( gs );
#		endif

		gdk_font_ref( gf );
		return guiSystemFont( ds, gf );
		}
	    }
	}

    if  ( pixelSize < 6 )
	{ pixelSize= 10;	}

    afi= appGetFontInfo( fontFamilyName, DFstyleFNIL, (IndexSet *)0,
								&ta, psfl );
    if  ( ! afi )
	{ SXDEB(fontFamilyName,afi); return -1;	}

    return drawOpenScreenFont( ds, afi, pixelSize, (IndexSet *)0 );
    }

#   endif
