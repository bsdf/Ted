#   include	"appFrameConfig.h"

#   include	<stdio.h>

#   include	"guiWidgets.h"

#   include	<appDebugon.h>

#   ifdef USE_GTK

#   include	"guiWidgetsGtk.h"

void appMakeButtonInRow(	APP_WIDGET *		pButton,
				APP_WIDGET		row,
				const char *		text,
				APP_BUTTON_CALLBACK_T	callback,
				void *			through,
				int			position,
				int			colspan,
				int			showAsDefault )
    {
    GtkWidget *		button= gtk_button_new_with_label( (char *)text );

    gtk_button_set_relief ( GTK_BUTTON (button), GTK_RELIEF_HALF );
    gtk_widget_set_name( GTK_WIDGET(button), "tedRowButton" );

    if  ( ! showAsDefault )
	{
	GTK_WIDGET_UNSET_FLAGS( button, GTK_CAN_FOCUS );
	GTK_WIDGET_UNSET_FLAGS( button, GTK_CAN_DEFAULT );
	}

    gtk_table_attach( GTK_TABLE( row ),
		    button,
		    position, position+ colspan,
		    0, 1,
		    (GtkAttachOptions)( GTK_FILL | GTK_EXPAND | GTK_SHRINK ),
		    (GtkAttachOptions)( GTK_FILL | GTK_EXPAND | GTK_SHRINK ),
		    ROW_XPADDING_GTK, ROW_YPADDING_GTK );

    gtk_widget_show( button );

    if  ( callback )
	{
	gtk_signal_connect( GTK_OBJECT( button ), "clicked",
					(GtkSignalFunc)callback, through );
	}

    *pButton= button;

    return;
    }

void appGuiChangeButtonText(	APP_WIDGET		button,
				const char *		label )
    {
    if  ( ! appGuiGtkSetChildLabel( button, (char *)label ) )
	{ SDEB((char *)label);	}
    }

void appMakeButtonInColumn(	APP_WIDGET *		pButton,
				APP_WIDGET		column,
				const char *		buttonText,
				APP_BUTTON_CALLBACK_T	callback,
				void *			through,
				int			showAsDefault )
    {
    GtkWidget *		button= gtk_button_new_with_label( (char *)buttonText );

    gtk_box_pack_start( GTK_BOX( column ), button, FALSE, TRUE, 0 );

    gtk_button_set_relief (GTK_BUTTON (button), GTK_RELIEF_HALF);
    gtk_widget_set_name(GTK_WIDGET (button), "tedColumnButton");

    if  ( ! showAsDefault )
	{
	GTK_WIDGET_UNSET_FLAGS(button, GTK_CAN_FOCUS);
	GTK_WIDGET_UNSET_FLAGS( button, GTK_CAN_DEFAULT );
	}

    gtk_widget_show( button );

    if  ( callback )
	{
	gtk_signal_connect( GTK_OBJECT( button ), "clicked",
					(GtkSignalFunc)callback, through );
	}

    *pButton= button;

    return;
    }

#   endif
