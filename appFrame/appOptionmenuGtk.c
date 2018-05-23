#   include	"appFrameConfig.h"

#   include	<stdio.h>

#   include	"guiWidgets.h"
#   include	"guiOptionmenu.h"
#   include	"guiWidgetsImpl.h"

#   include	<appDebugon.h>

#   ifdef USE_GTK

#   define	USE_OPT_MENU	1

/************************************************************************/

static int appGuiGetOptionmenuFindItem(	GtkWidget *	w,
					GtkMenuItem *	it )
    {
    int		rval= -1;
    GList *	glf= (GList *)0;

    if  ( GTK_IS_CONTAINER( w ) )
	{
	GList *	gl;
	int	idx= 0;

	gl= glf= gtk_container_children( GTK_CONTAINER( w ) );

	while( gl )
	    {
	    GtkWidget *	c= (GtkWidget *)gl->data;

	    if  ( GTK_IS_MENU_ITEM( c )		)
		{
		if  ( GTK_MENU_ITEM( c ) == it	)
		    { rval= idx; goto ready;	}

		idx++;
		}
	    else{
		if  ( GTK_IS_CONTAINER( c ) )
		    {
		    rval= appGuiGetOptionmenuFindItem( c, it );

		    if  ( rval >= 0 )
			{ goto ready;	}
		    }
		}

	    gl= gl->next;
	    }
	}

  ready:
    if  ( glf )
	{ g_list_free( glf );	}

    return rval;
    }

static void appOptionMenuGtkCallback(	GtkMenuItem *	it,
					void *		voidaom )
    {
    AppOptionmenu *	aom= (AppOptionmenu *)voidaom;
    int			idx;

    idx= appGuiGetOptionmenuFindItem( aom->aomPulldown, it );
    if  ( idx < 0 )
	{ LDEB(idx); return;	}

    if  ( aom->aomCallback )
	{ (*aom->aomCallback)( idx, aom->aomTarget ); }
    }

static int GTKcomboSetting= 0;

static void appComboBoxGtkCallback(	GtkComboBox *	combo,
					void *		voidaom )
    {
    AppOptionmenu *	aom= (AppOptionmenu *)voidaom;
    int			idx;

    if  ( GTKcomboSetting )
	{ return;	}

    idx= gtk_combo_box_get_active( combo );
    if  ( idx < 0 )
	{ LDEB(idx); return;	}

    if  ( aom->aomCallback )
	{ (*aom->aomCallback)( idx, aom->aomTarget ); }
    }

/************************************************************************/
/*									*/
/*  Used to get around Motif's ridiculous resize behavior. Probably	*/
/*  not needed for GTK.							*/
/*									*/
/************************************************************************/

void appOptionmenuRefreshWidth(	AppOptionmenu *		aom )
    { return;	}

void appMakeOptionmenuInRow(		AppOptionmenu *		aom,
					APP_WIDGET		row,
					int			column,
					int			colspan,
					OptionmenuCallback	callBack,
					void *			target )
    {
    APP_WIDGET	pulldown;
    APP_WIDGET	inplace;

    pulldown= gtk_menu_new();
#   if USE_OPT_MENU
    inplace= gtk_option_menu_new();
#   else
    inplace= gtk_combo_box_new_text();
#   endif

    gtk_widget_set_name(GTK_WIDGET (pulldown), "tedRowOptMenu");
    gtk_widget_set_name(GTK_WIDGET (inplace), "tedRowOptMenu");

    gtk_table_attach( GTK_TABLE( row ),
			inplace,
			column, column+ colspan,
			0, 1,
			GTK_FILL | GTK_EXPAND | GTK_SHRINK,
			GTK_FILL | GTK_EXPAND | GTK_SHRINK,
			ROW_XPADDING_GTK, ROW_YPADDING_GTK );

    gtk_widget_show( inplace );

    if  ( GTK_IS_OPTION_MENU( inplace ) )
	{
	gtk_option_menu_set_menu( GTK_OPTION_MENU( inplace ), pulldown );
	}

    if  ( GTK_IS_COMBO_BOX( inplace ) && callBack )
	{
	gtk_signal_connect( GTK_OBJECT( inplace ), "changed",
				(GtkSignalFunc)appComboBoxGtkCallback, aom );
	}

    aom->aomPulldown= pulldown;
    aom->aomInplace= inplace;
    aom->aomCallback= callBack;
    aom->aomTarget= target;

    return;
    }

void appMakeOptionmenuInColumn(		AppOptionmenu *		aom,
					APP_WIDGET		column,
					OptionmenuCallback	callBack,
					void *			target )
    {
    APP_WIDGET	pulldown;
    APP_WIDGET	inplace;

    pulldown= gtk_menu_new();
#   if USE_OPT_MENU
    inplace= gtk_option_menu_new();
#   else
    inplace= gtk_combo_box_new_text();
#   endif

    gtk_widget_set_name(GTK_WIDGET (pulldown), "tedRowOptMenu");
    gtk_widget_set_name(GTK_WIDGET (inplace), "tedRowOptMenu");

    gtk_box_pack_start( GTK_BOX( column ), inplace, FALSE, TRUE, 0 );

    gtk_widget_show( inplace );

    if  ( GTK_IS_OPTION_MENU( inplace ) )
	{
	gtk_option_menu_set_menu( GTK_OPTION_MENU( inplace ), pulldown );
	}

    if  ( GTK_IS_COMBO_BOX( inplace ) && callBack )
	{
	gtk_signal_connect( GTK_OBJECT( inplace ), "changed",
				(GtkSignalFunc)appComboBoxGtkCallback, aom );
	}

    aom->aomPulldown= pulldown;
    aom->aomInplace= inplace;
    aom->aomCallback= callBack;
    aom->aomTarget= target;

    return;
    }

/************************************************************************/
/*									*/
/*  Add an option to a pulldown.					*/
/*									*/
/************************************************************************/

APP_WIDGET appAddItemToOptionmenu(	AppOptionmenu *		aom,
					const char *		label )
    {
    APP_WIDGET	option;

    option= gtk_menu_item_new_with_label( label );
    gtk_widget_set_name( GTK_WIDGET(option), "tedOptMenuItem");

    gtk_menu_append( GTK_MENU( aom->aomPulldown ), option );

    if  ( GTK_IS_COMBO_BOX( aom->aomInplace ) )
	{
	if  ( ! label )
	    { label= "";	}

	gtk_combo_box_append_text( GTK_COMBO_BOX( aom->aomInplace ), label );
	}

    gtk_widget_show( option );

    if  ( aom->aomCallback )
	{
	gtk_signal_connect( GTK_OBJECT( option ), "activate",
				(GtkSignalFunc)appOptionMenuGtkCallback, aom );
	}

    return option;
    }

/************************************************************************/
/*									*/
/*  Select a particular option in an option menu.			*/
/*									*/
/************************************************************************/

void appSetOptionmenu(	AppOptionmenu *	aom,
			int		num )
    {
    if  ( GTK_IS_OPTION_MENU( aom->aomInplace ) )
	{
	gtk_option_menu_set_history( GTK_OPTION_MENU( aom->aomInplace ), num );
	}

    if  ( GTK_IS_COMBO_BOX( aom->aomInplace ) )
	{
	GTKcomboSetting++;
	gtk_combo_box_set_active( GTK_COMBO_BOX( aom->aomInplace ), num );
	GTKcomboSetting--;
	}

    return;
    }

void appEmptyOptionmenu(	AppOptionmenu *		aom )
    {
    appEmptyParentWidget( aom->aomPulldown );
    }

void appGuiEnableOptionmenu(	AppOptionmenu *		aom,
				int			sensitive )
    {
    gtk_widget_set_sensitive( aom->aomInplace, sensitive != 0 );
    }

void appInitOptionmenu(		AppOptionmenu *		aom )
    {
    aom->aomPulldown= (APP_WIDGET)0;
    aom->aomInplace= (APP_WIDGET)0;

    aom->aomCallback= (OptionmenuCallback)0;
    aom->aomTarget= (void *)0;
    }

void appOptionmenuItemSetVisibility(	APP_WIDGET	w,
					int		visible )
    {
    visible= ( visible != 0 );

    if  ( GTK_WIDGET_VISIBLE( w ) == visible )
	{ return;	}

    if  ( visible )
	{ gtk_widget_show( w );	}
    else{ gtk_widget_hide( w );	}

    return;
    }

#   endif
