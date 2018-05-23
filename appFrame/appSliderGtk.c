#   include	"appFrameConfig.h"

#   include	<stdio.h>

#   include	"guiWidgets.h"

#   include	<appDebugon.h>

#   ifdef USE_GTK

void appGuiMakeSliderInRow(	APP_WIDGET *		pSlider,
				APP_WIDGET		row,
				int			column,
				int			colspan,
				APP_BUTTON_CALLBACK_T	callback,
				void *			through )
    {
    GtkAdjustment *	ga;
    GtkWidget *		scale;

    ga= GTK_ADJUSTMENT( gtk_adjustment_new(
				    0.0, 0.0, 100.0, 1.0, 10.0, 10.0 ) );
    scale= gtk_hscale_new( ga );
    gtk_scale_set_draw_value( GTK_SCALE( scale ), FALSE );

    if  ( callback )
	{
	gtk_signal_connect( GTK_OBJECT( ga ),
			"value_changed", (GtkSignalFunc)callback, through );
	}

    gtk_table_attach( GTK_TABLE( row ),
			scale,
			column, column+ colspan,
			0, 1,
			GTK_FILL | GTK_EXPAND | GTK_SHRINK,
			GTK_FILL | GTK_EXPAND | GTK_SHRINK,
			ROW_XPADDING_GTK, ROW_YPADDING_GTK );

    gtk_widget_show( scale );

    *pSlider= scale;
    }

void appGuiSetSlider(		APP_WIDGET	slider,
				int		minVal,
				int		maxVal,
				int		curVal )
    {
    GtkAdjustment *	ga;

    ga= gtk_range_get_adjustment( GTK_RANGE( slider ) );

    ga->lower= minVal;
    ga->upper= maxVal;
    ga->value= curVal;

    gtk_adjustment_changed( ga );
    }

void appGuiSetSliderValue(		APP_WIDGET	slider,
					int		curVal )
    {
    GtkAdjustment *	ga;

    ga= gtk_range_get_adjustment( GTK_RANGE( slider ) );

    ga->value= curVal;

    gtk_adjustment_changed( ga );
    }

int appGuiGetValueFromSliderCallback(	int *		pValue,
					APP_WIDGET	w,
					void *		voidscs )
    {
    GtkAdjustment *	ga= (GtkAdjustment *)w;

    *pValue= ga->value; return 0;
    }

#   endif

