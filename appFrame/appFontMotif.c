/************************************************************************/
/*									*/
/*  Motif font related code.						*/
/*  X11 structures.							*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"
#   include	"appGuiBase.h"
#   include	"appScreenFont.h"

#   ifdef	USE_MOTIF

#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<ctype.h>
#   include	<string.h>
#   include	<Xm/Label.h>

#   include	<appDebugon.h>

#   include	"drawScreenFontAdmin.h"
#   include	"drawDrawingSurfacePrivate.h"
#   include	<bitmap.h>

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
    APP_FONT *		xfs= (APP_FONT *)0;
    XmFontList		fontList= (XmFontList)0;
    APP_WIDGET		label= (APP_WIDGET)0;

    if  ( ! ds )
	{ XDEB(ds); return -1;	}

#   if 1
    XtVaGetValues( w,	XmNfontList,	&fontList,
			NULL );
#   else
    /*# include <Xm/XmP.h>*/
    fontList= _XmGetDefaultFontList( w, XmLABEL_FONTLIST );
#   endif

    if  ( ! fontList )
	{
	label= XmCreateLabel( w, WIDGET_NAME, NULL, 0 );

	XtVaGetValues( label,	XmNfontList,	&fontList,
				NULL );
	}

    if  ( ! fontList )
	{ XDEB(fontList);	}
    else{
	XmFontContext	context;
	XmFontListEntry	entry;

	XmFontListInitFontContext( &context, fontList );

	entry= XmFontListNextEntry( context );
	if  ( ! entry )
	    { XDEB(entry);	}
	else{
	    XtPointer		fontPtr;
	    XmFontType		fontType;

	    fontPtr= XmFontListEntryGetFont( entry, &fontType );

	    if  ( fontType == XmFONT_IS_FONT )
		{ xfs= (XFontStruct *)fontPtr;	}
	    else{
		int		fontCount;
		char **		fontNameList;
		XFontStruct **	fontList;

		fontCount = XFontsOfFontSet( (XFontSet)fontPtr,
						   &fontList, &fontNameList );
		if  ( fontCount == 0 )
		    { LDEB(fontCount);	}
		else{ xfs= fontList[0];	}
		}

	    /* NO! Pointer into the belly of the font list.
	    XmFontListEntryFree( &entry );
	    */
	    }

	XmFontListFreeFontContext( context );

	/* NO! Pointer into the belly of the label widget.
	XmFontListFree( fontList );
	*/
	}

    if  ( label )
	{ XtDestroyWidget( label );	}

    if  ( ! xfs )
	{ XDEB(xfs); return -1;	}

    return guiSystemFont( ds, xfs );
    }

#   endif
