#   include	"appFrameConfig.h"

#   include	<stdio.h>

#   include	"appFrame.h"
#   include	"guiWidgetsGtk.h"
#   include	"guiWidgetsImpl.h"
#   include	"guiDrawingWidget.h"

#   include	<appDebugon.h>

#   ifdef USE_GTK

/************************************************************************/
/*									*/
/*  Change the label/state of a toggle button.				*/
/*									*/
/************************************************************************/

void appGuiSetToggleLabel(	APP_WIDGET		toggle,
				const char *		text )
    {
    if  ( ! appGuiGtkSetChildLabel( toggle, text ) )
	{ SDEB(text);	}
    }

void appGuiSetToggleState(	APP_WIDGET		toggle,
				int			set )
    {
    gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( toggle ), set != 0 );
    }

int appGuiGetToggleState(	APP_WIDGET		toggle )
    {
    return gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( toggle ) ) != 0;
    }

int appGuiGetToggleStateFromCallbackGtk( GtkWidget *	toggle )
    {
    return gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( toggle ) ) != 0;
    }

APP_WIDGET appMakeToggleInRow(	APP_WIDGET		row,
				const char *		text,
				APP_TOGGLE_CALLBACK_T	callback,
				void *			through,
				int			col,
				int			colspan )
    {
    GtkWidget *	toggle;

    if  ( text )
	{
	toggle= gtk_check_button_new_with_label( (char *)text );
	}
    else{
	toggle= gtk_check_button_new();
	}

    gtk_widget_set_name( GTK_WIDGET(toggle), "tedRowToggle" );

    gtk_table_attach( GTK_TABLE( row ),
			toggle,
			col, col+ colspan,
			0, 1,
			GTK_FILL | GTK_EXPAND | GTK_SHRINK,
			GTK_FILL | GTK_EXPAND | GTK_SHRINK,
			ROW_XPADDING_GTK, ROW_YPADDING_GTK );

    gtk_widget_show( toggle );

    if  ( callback )
	{
	gtk_signal_connect( GTK_OBJECT( toggle ), "toggled",
					    (GtkSignalFunc)callback, through );
	}

    return toggle;
    }

#   endif /* USE_GTK */
