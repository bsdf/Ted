#   include	"appFrameConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<string.h>

#   include	"guiWidgets.h"

#   include	<appDebugon.h>

#   ifdef USE_GTK

void appMakeTextInRow(		APP_WIDGET *		pText,
				APP_WIDGET		row,
				int			column,
				int			colspan,
				int			textColumns,
				int			textEnabled )
    {
    APP_WIDGET		text= gtk_entry_new();

    gtk_widget_set_name(GTK_WIDGET (text), "tedRowText");

#   if GTK_MAJOR_VERSION >= 2
    if  ( textColumns > 0 )
	{ gtk_entry_set_width_chars( GTK_ENTRY( text ), textColumns );	}
#   endif

    gtk_table_attach( GTK_TABLE( row ),
			text,
			column, column+ colspan,
			0, 1,
			GTK_FILL | GTK_EXPAND | GTK_SHRINK,
			GTK_FILL | GTK_EXPAND | GTK_SHRINK,
			ROW_XPADDING_GTK, ROW_YPADDING_GTK );

    gtk_widget_show( text );

    gtk_entry_set_editable( GTK_ENTRY( text ), textEnabled != 0 );

    if  ( ! textEnabled )
	{ gtk_widget_set_sensitive( text, FALSE );	}

    *pText= text;
    }

void appMakeTextInHBox(		APP_WIDGET *		pText,
				APP_WIDGET		hbox,
				int			textColumns,
				int			textEnabled )
    {
    APP_WIDGET		text= gtk_entry_new();

#   if GTK_MAJOR_VERSION >= 2
    if  ( textColumns > 0 )
	{ gtk_entry_set_width_chars( GTK_ENTRY( text ), textColumns );	}
#   endif

    gtk_box_pack_start( GTK_BOX( hbox ), text, FALSE, FALSE, 0 );

    gtk_widget_show( text );

    gtk_entry_set_editable( GTK_ENTRY( text ), textEnabled != 0 );

    if  ( ! textEnabled )
	{ gtk_widget_set_sensitive( text, FALSE );	}

    *pText= text;
    }


/************************************************************************/
/*									*/
/*  Make a text widget that is contained in a column of widgets		*/
/*									*/
/************************************************************************/

void appMakeTextInColumn(	APP_WIDGET *	pText,
				APP_WIDGET	column,
				int		textColumns,
				int		textEnabled )
    {
    APP_WIDGET		text;

    text= gtk_entry_new();

    gtk_widget_set_name(GTK_WIDGET (text), "tedColumnText");

#   if GTK_MAJOR_VERSION >= 2
    if  ( textColumns > 0 )
	{ gtk_entry_set_width_chars( GTK_ENTRY( text ), textColumns );	}
#   endif

    gtk_box_pack_start( GTK_BOX( column ), text, FALSE, TRUE, 0 );

    gtk_entry_set_editable( GTK_ENTRY( text ), textEnabled != 0 );

    gtk_widget_show( text );

    *pText= text;
    }

void appRefuseTextValue(	APP_WIDGET		text )
    {
    gtk_entry_select_region( GTK_ENTRY( text ), 0, 
			strlen( gtk_entry_get_text( GTK_ENTRY( text ) ) ) );

    gtk_widget_grab_focus( text );

    return;
    }

/************************************************************************/
/*									*/
/*  Insert an integer in a text widget.					*/
/*									*/
/************************************************************************/

void appStringToTextWidget(		APP_WIDGET	text,
					const char *	s )
    {
    gtk_entry_set_text( GTK_ENTRY( text ), s );

    return;
    }

/************************************************************************/
/*									*/
/*  Select text in a text entry box.					*/
/*									*/
/************************************************************************/

void appTextSelectContents(		APP_WIDGET	w,
					int		from,
					int		upto )
    {
    gtk_entry_select_region( GTK_ENTRY(w), from, upto );
    return;
    }

/************************************************************************/
/*									*/
/*  Turn a text widget on or off.					*/
/*									*/
/************************************************************************/

void guiEnableText(		APP_WIDGET	text,
				int		enabled )
    {
    gtk_widget_set_sensitive( text, enabled != 0 );
    gtk_entry_set_editable( GTK_ENTRY( text ), enabled != 0 );
    }

/************************************************************************/
/*									*/
/*  1)  Retrieve strings from text widgets.				*/
/*  2)  Free the result obtained in this way.				*/
/*									*/
/************************************************************************/

/*  1  */
char *	appGetStringFromTextWidget(	APP_WIDGET	text )
    {
    return strdup( gtk_entry_get_text( GTK_ENTRY( text ) ) );
    }

/*  2  */
void appFreeStringFromTextWidget(	char *		s )
    {
    free( s );
    }

void appGuiSetTypingCallbackForText(	APP_WIDGET		text,
					APP_TXTYPING_CALLBACK_T	callBack,
					void *			through )
    {
    gtk_signal_connect( GTK_OBJECT( text ), "changed",
					(GtkSignalFunc)callBack, through );

    return;
    }

void appGuiSetGotValueCallbackForText(
				APP_WIDGET			text,
				APP_TXACTIVATE_CALLBACK_T	callBack,
				void *				through )
    {
    gtk_signal_connect( GTK_OBJECT( text ), "activate",
					(GtkSignalFunc)callBack, through );

    return;
    }

#   endif
