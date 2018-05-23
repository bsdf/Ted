#   include	"appFrameConfig.h"

#   include	<stdio.h>

#   include	"appFrame.h"
#   include	"appInspector.h"
#   include	"guiWidgetsImpl.h"

#   include	<appDebugon.h>

#   ifdef USE_GTK

void appInspectorMakePageParent(	AppInspector *	ai )
    {
    GtkWidget *		notebook;

    notebook= gtk_notebook_new();

    gtk_notebook_set_show_tabs( GTK_NOTEBOOK( notebook ), FALSE );
    gtk_notebook_set_show_border( GTK_NOTEBOOK( notebook ), FALSE );

    gtk_widget_show( notebook );

    gtk_box_pack_start( GTK_BOX( ai->aiPaned ), notebook, FALSE, TRUE, 0 );

    ai->aiPageParent= notebook;

    gtk_widget_set_name(GTK_WIDGET (ai->aiPaned), "tedInspector");

    return;
    }

void appInspectorChoosePage(	AppInspector *		ai,
				int			andMenu,
				int			subject )
    {
    gtk_notebook_set_page( GTK_NOTEBOOK( ai->aiPageParent ), subject );

    if  ( andMenu )
	{ appSetOptionmenu( &(ai->aiSubjectOptionmenu), subject );	}

    return;
    }

int appMakeVerticalInspectorPage(	APP_WIDGET *	pPage,
					APP_WIDGET *	pMenuitem,
					AppInspector *	ai,
					const char *	label )
    {
    GtkWidget *		option;
    GtkWidget *		page;

    page= gtk_vbox_new( FALSE, COLUMN_SPACING_GTK );

#   if 0
    gtk_widget_set_name( GTK_WIDGET(page), label );
#   endif

    gtk_notebook_append_page( GTK_NOTEBOOK( ai->aiPageParent ),
						    page, (GtkWidget *)0 );

    gtk_widget_show( page );

    option= appAddItemToOptionmenu( &(ai->aiSubjectOptionmenu), label );

    *pPage= page; *pMenuitem= option; return 0;
    }

void appInspectorDeleteSubject(	AppInspector *		ai,
				int			subject )
    {
    GtkWidget *		page;

    page= gtk_notebook_get_nth_page( GTK_NOTEBOOK( ai->aiPageParent ),
								subject );

    gtk_widget_destroy( page );

    gtk_widget_destroy( ai->aiSubjects[subject].isMenuitem );

    ai->aiSubjectCount--;
    while( subject < ai->aiSubjectCount )
	{ ai->aiSubjects[subject]= ai->aiSubjects[subject+ 1];	}

    return;
    }


void appInspectorEnablePage(	AppInspector *	ai,
				int		subject,
				int		enabled )
    {
    GtkWidget *		page;

    page= gtk_notebook_get_nth_page( GTK_NOTEBOOK( ai->aiPageParent ),
								subject );

    gtk_widget_set_sensitive( page, enabled != 0 );

    gtk_widget_set_sensitive( ai->aiSubjects[subject].isMenuitem,
							    enabled != 0 );

    return;
    }

#   endif /*  USE_GTK	*/
