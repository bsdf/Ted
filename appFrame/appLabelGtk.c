#   include	"appFrameConfig.h"

#   include	<stdio.h>

#   include	"guiWidgets.h"

#   include	<appDebugon.h>

#   ifdef USE_GTK

/************************************************************************/
/*									*/
/*  Make a row with a label and a text widget.				*/
/*									*/
/************************************************************************/

void appMakeLabelInRow(		APP_WIDGET *		pLabel,
				APP_WIDGET		row,
				int			column,
				int			colspan,
				const char *		labelText )
    {
    GtkWidget *		evbox= gtk_event_box_new();
    GtkWidget *		labelw= gtk_label_new( labelText );

    gtk_label_set_line_wrap( GTK_LABEL( labelw ), FALSE );
    gtk_label_set_justify( GTK_LABEL( labelw ), GTK_JUSTIFY_LEFT );
    gtk_misc_set_alignment( GTK_MISC( labelw ), 0.0, 0.5 );

    gtk_container_add( GTK_CONTAINER( evbox ), labelw );
    gtk_table_attach( GTK_TABLE( row ),
			evbox,
			column, column+ colspan,
			0, 1,
			GTK_FILL | GTK_EXPAND | GTK_SHRINK,
			GTK_FILL | GTK_EXPAND | GTK_SHRINK,
			/*
			ROW_XPADDING_GTK, ROW_YPADDING_GTK );
			*/
			ROW_XPADDING_GTK, ROW_YPADDING_GTK+ 2 );

    gtk_widget_show( labelw );
    gtk_widget_show( evbox );

    *pLabel= evbox;

    return;
    }

void appMakeLabelInHBox(	APP_WIDGET *		pLabel,
				APP_WIDGET		hbox,
				const char *		labelText )
    {
    GtkWidget *		evbox= gtk_event_box_new();
    GtkWidget *		labelw= gtk_label_new( labelText );

    gtk_label_set_line_wrap( GTK_LABEL( labelw ), FALSE );
    gtk_label_set_justify( GTK_LABEL( labelw ), GTK_JUSTIFY_LEFT );
    gtk_misc_set_alignment( GTK_MISC( labelw ), 0.0, 0.5 );

    gtk_container_add( GTK_CONTAINER( evbox ), labelw );
    gtk_box_pack_start( GTK_BOX( hbox ), evbox, FALSE, FALSE, 0 );

    gtk_widget_show( labelw );
    gtk_widget_show( evbox );

    *pLabel= evbox;

    return;
    }


void appMakeLabelInColumn(	APP_WIDGET *		pLabel,
				APP_WIDGET		column,
				const char *		labelText )
    {
    GtkWidget *		evbox= gtk_event_box_new();
    GtkWidget *		labelw= gtk_label_new( labelText );

    gtk_label_set_line_wrap( GTK_LABEL( labelw ), FALSE );
    gtk_label_set_justify( GTK_LABEL( labelw ), GTK_JUSTIFY_LEFT );
    gtk_misc_set_alignment( GTK_MISC( labelw ), 0.0, 0.5 );

    gtk_container_add( GTK_CONTAINER( evbox ), labelw );
    gtk_box_pack_start( GTK_BOX( column ), evbox, FALSE, TRUE, 0 );

    gtk_widget_show( labelw );
    gtk_widget_show( evbox );

    *pLabel= evbox;
    return;
    }

void appGuiChangeLabelText(	APP_WIDGET		evbox,
				const char *		labelt )
    {
#   if GTK_MAJOR_VERSION >= 2
    GtkWidget *		labelw= gtk_bin_get_child( GTK_BIN( evbox ) );
#   else
    GtkWidget *		labelw= GTK_BIN( evbox )->child;
#   endif

    gtk_label_set_text( GTK_LABEL( labelw ), (char *)labelt );
    
    return;
    }

#   endif
