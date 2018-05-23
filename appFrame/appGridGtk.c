#   include	"appFrameConfig.h"

#   include	<stdio.h>
#   include	<appDebugon.h>

#   include	"guiWidgets.h"

#   ifdef USE_GTK

/************************************************************************/
/*									*/
/*  Make a row in a column: Rows are implemented as..			*/
/*									*/
/************************************************************************/

APP_WIDGET appMakeRowInColumn(	APP_WIDGET	column,
				int		columnCount,
				int		heightResizable )
    {
    gboolean	expand= FALSE;
    gboolean	fill= TRUE;

    GtkWidget *	row= gtk_table_new( 1, columnCount, TRUE );

    if  ( heightResizable )
	{ expand= TRUE;	}

    gtk_box_pack_start( GTK_BOX( column ), row, expand, fill, 0 );

    gtk_widget_show( row );

    return row;
    }

void appMakeHBoxInRow(		APP_WIDGET *	pHbox,
				APP_WIDGET	row,
				int		position,
				int		colspan )
    {
    int			spacing= 0;
    GtkWidget *		hbox= gtk_hbox_new( FALSE, spacing );

    gtk_table_attach( GTK_TABLE( row ),
			hbox,
			position, position+ colspan,
			0, 1,
			GTK_FILL | GTK_EXPAND | GTK_SHRINK,
			GTK_FILL | GTK_EXPAND | GTK_SHRINK,
			ROW_XPADDING_GTK, ROW_YPADDING_GTK );

    gtk_widget_show( hbox );

    *pHbox= hbox;
    return;
    }

void appMakeColumnInRow(	APP_WIDGET *	pColumn,
				APP_WIDGET	row,
				int		position,
				int		colspan )
    {
    GtkWidget *		column= gtk_vbox_new( FALSE, COLUMN_SPACING_GTK );

    gtk_table_attach( GTK_TABLE( row ),
			column,
			position, position+ colspan,
			0, 1,
			GTK_FILL | GTK_EXPAND | GTK_SHRINK,
			GTK_FILL | GTK_EXPAND | GTK_SHRINK,
			ROW_XPADDING_GTK, ROW_YPADDING_GTK );

    gtk_widget_show( column );

    *pColumn= column;
    }

#   endif
