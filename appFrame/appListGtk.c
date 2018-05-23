#   include	"appFrameConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>

#   include	"guiWidgets.h"
#   include	"appListGtk.h"

#   include	<appDebugon.h>

#   ifdef USE_GTK

/************************************************************************/
/*									*/
/*  List specific functionality.					*/
/*									*/
/************************************************************************/

static const char	APP_ListAction[]= "ListAction";

static void appGuiGtkFreeListAction( void * voidla )
    { free( voidla );	}

GtkWidget * appListGtkMakeParent(	GtkAdjustment **	pVAdjustment,
					GtkWidget *		column )
    {
    GtkWidget *		scrolled_window;
    GtkAdjustment *	vadjust;

    gboolean		expand= TRUE;
    gboolean		fill= TRUE;

    scrolled_window= gtk_scrolled_window_new(
				(GtkAdjustment *)0, (GtkAdjustment *)0 );

    gtk_box_pack_start( GTK_BOX( column ), scrolled_window, expand, fill, 0 );

    gtk_scrolled_window_set_placement( GTK_SCROLLED_WINDOW( scrolled_window ),
						    GTK_CORNER_BOTTOM_LEFT );
    gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW( scrolled_window ),
						    GTK_POLICY_NEVER,
						    GTK_POLICY_AUTOMATIC );

    vadjust= gtk_scrolled_window_get_vadjustment(
				    GTK_SCROLLED_WINDOW( scrolled_window ) );
    gtk_adjustment_set_value( vadjust, 0 );

    gtk_container_set_focus_vadjustment(
				    GTK_CONTAINER( scrolled_window ), vadjust );

    gtk_widget_show( scrolled_window );

    *pVAdjustment= vadjust;
    return scrolled_window;
    }

void appListGtkSetVisibleSize(		GtkWidget *		list,
					GtkWidget *		scrolled_window,
					int			visibleItems )
    {
    int		pixelSize= 9;
    GtkStyle *	gs= gtk_widget_get_style( list );

    if  ( gs->font_desc )
	{
	if  ( pango_font_description_get_size_is_absolute( gs->font_desc ) )
	    {
	    int		sz;

	    sz= pango_font_description_get_size( gs->font_desc );
	    if  ( sz > 0 )
		{ pixelSize= sz;	}
	    }
	}
    else{
	GdkFont *	gf;

#	if GTK_MAJOR_VERSION < 2
	gf= gs->font;
#	else
	gf= gtk_style_get_font( gs );
#	endif

	pixelSize= gf->ascent+ gf->descent;
	}

#   if GTK_MAJOR_VERSION < 2
    gtk_widget_set_usize( scrolled_window, -1,
				( 110* visibleItems* pixelSize )/ 100 );
#   else
    gtk_widget_set_size_request( scrolled_window, 0,
				( 110* visibleItems* pixelSize )/ 100 );
#   endif
    }

static void appListGtkResized(	GtkWidget *		w,
				GdkRectangle *		allocation,
				gpointer		voidla )
    {
    ListAction *	la= (ListAction *)voidla;

    la->laListHigh= allocation->height;

    if  ( la->laListHigh > 0		&&
	  la->laParentHigh > 0		&&
	  la->laSelectedPosition >= 0	)
	{
	appGuiGtkSetListAdjustment( la, la->laSelectedPosition );
	}

    /*  NO gtk_adjustment_set_upper( la->laAdjustment ): crashes Gtk ) */
    }

static void appListGtkParentResized(	GtkWidget *		w,
					GdkRectangle *		allocation,
					gpointer		voidla )
    {
    ListAction *	la= (ListAction *)voidla;

    la->laParentHigh= allocation->height;

    if  ( la->laListHigh > 0		&&
	  la->laParentHigh > 0		&&
	  la->laSelectedPosition >= 0	)
	{
	appGuiGtkSetListAdjustment( la, la->laSelectedPosition );
	}

    /*  NO gtk_adjustment_set_page_size( la->laAdjustment ): crashes Gtk ) */
    }

static void appListGtkSetFocusChild(	GtkTreeView *		parent,
					GtkWidget *		w,
					gpointer		voidla )
    {
    ListAction *	la= (ListAction *)voidla;

    if  ( la->laListHigh > 0		&&
	  la->laParentHigh > 0		&&
	  la->laSelectedPosition >= 0	)
	{
	appGuiGtkSetListAdjustment( la, la->laSelectedPosition );
	}

    }

ListAction * appListGtkSetCallbacks(
				GtkWidget *		widget,
				GtkWidget *		scrolled_window,
				GtkAdjustment *		adjustment,
				APP_LIST_CALLBACK_T	listCallback,
				APP_BUTTON_CALLBACK_T	actionCallback,
				void *			through )
    {
    ListAction *	la= malloc( sizeof(ListAction) );

    if  ( ! la )
	{ XDEB(la); return (void *)0;	}

    la->laWidget= widget;
    la->laParentWidget= scrolled_window;
    la->laAdjustment= adjustment;
    la->laThrough= through;
    la->laActionCallback= actionCallback;
    la->laListCallback= listCallback;

    la->laListHigh= 0;
    la->laParentHigh= 0;
    la->laItemCount= 0;
    la->laSelectedPosition= -1;

    gtk_object_set_data_full( GTK_OBJECT(widget), APP_ListAction, la,
						    appGuiGtkFreeListAction );

    gtk_signal_connect( GTK_OBJECT( la->laWidget ), "size-allocate",
			    (GCallback)appListGtkResized, (void *)la );
    if  ( la->laParentWidget )
	{
	gtk_signal_connect( GTK_OBJECT( la->laParentWidget ), "size-allocate",
			    (GCallback)appListGtkParentResized, (void *)la );
	}

    gtk_signal_connect( GTK_OBJECT( la->laParentWidget ), "set-focus-child",
			    (GCallback)appListGtkSetFocusChild, (void *)la );

    return la;
    }

ListAction * appListGtkGetListAction(
				GtkWidget *		widget )
    {
    return (ListAction *)gtk_object_get_data(
			    GTK_OBJECT(widget), APP_ListAction );
    }

void appGuiGtkSetListAdjustment(		ListAction *	la,
						int		position )
    {
    int	increment;
    int	old_value;
    int	new_value;

    la->laSelectedPosition= position;

    if  ( la->laItemCount == 0 )
	{ /*LLDEB(position,la->laItemCount);*/ return; }

    increment= la->laListHigh/ la->laItemCount;
    if  ( increment == 0 )
	{ /*LDEB(increment);*/ return;	}

    old_value= gtk_adjustment_get_value( la->laAdjustment );
    new_value= position* increment;

    if  ( new_value < old_value )
	{
	gtk_adjustment_set_value( la->laAdjustment, new_value );
	}
    else{
	int	pagesize= la->laParentHigh;

	if  ( new_value+ increment > old_value+ pagesize )
	    {
	    int	upper= la->laListHigh;
	    int	visible_increments= la->laParentHigh/ increment;

	    new_value -= ( visible_increments- 1 )* increment;

	    if  ( new_value > upper- pagesize )
		{ new_value= upper- pagesize;	}

	    gtk_adjustment_set_value( la->laAdjustment, new_value );
	    }
	}

    return;
    }

#   endif
