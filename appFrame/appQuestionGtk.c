/************************************************************************/
/*									*/
/*  Utility routines for making remarks and posing questions.		*/
/*  Implementation for GTK+ 2.						*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   include	<stdio.h>
#   include	<stddef.h>

#   include	"appFrame.h"
#   include	"appQuestion.h"

#   include	<appDebugon.h>

# if USE_GTK_DIALOGS

int appQuestionRunSubjectYesNoCancelDialog(
					EditApplication *	ea,
					APP_WIDGET		relative,
					APP_WIDGET		option,
					const char *		subject,
					const char *		question )
    {
    GtkWidget *		dialog;
    int			response;

    if  ( ! ea->eaToplevel.atTopWidget )
	{
	appDebug( "%s: \"%s\" %s\n", ea->eaApplicationName, subject, question );
	return AQDrespCANCEL;
	}

    dialog=  gtk_message_dialog_new( GTK_WINDOW( relative ), GTK_DIALOG_MODAL,
		    GTK_MESSAGE_QUESTION,
		    GTK_BUTTONS_NONE,
		    question, 1 );
    g_object_set( G_OBJECT( dialog ),
	"secondary-text", subject,
	NULL );
    gtk_dialog_add_button( GTK_DIALOG( dialog ),
				    GTK_STOCK_YES, GTK_RESPONSE_YES );
    gtk_dialog_add_button( GTK_DIALOG( dialog ),
				    GTK_STOCK_NO, GTK_RESPONSE_NO );
    gtk_dialog_add_button( GTK_DIALOG( dialog ),
				    GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL );
    response= gtk_dialog_run( GTK_DIALOG( dialog ) );
    gtk_widget_destroy( dialog );

    switch( response )
	{
	case GTK_RESPONSE_YES:		return AQDrespYES;
	case GTK_RESPONSE_NO:		return AQDrespNO;
	case GTK_RESPONSE_CANCEL:	return AQDrespCANCEL;
	default:
	    LDEB(response); return AQDrespCANCEL;
	}
    }

int appQuestionRunYesNoCancelDialog(	EditApplication *	ea,
					APP_WIDGET		relative,
					APP_WIDGET		option,
					const char *		question )
    {
    GtkWidget *		dialog;
    int			response;

    if  ( ! ea->eaToplevel.atTopWidget )
	{
	appDebug( "%s: %s\n", ea->eaApplicationName, question );
	return AQDrespCANCEL;
	}

    dialog=  gtk_message_dialog_new( GTK_WINDOW( relative ), GTK_DIALOG_MODAL,
		    GTK_MESSAGE_QUESTION,
		    GTK_BUTTONS_NONE,
		    question, 1 );
    gtk_dialog_add_button( GTK_DIALOG( dialog ),
				    GTK_STOCK_YES, GTK_RESPONSE_YES );
    gtk_dialog_add_button( GTK_DIALOG( dialog ),
				    GTK_STOCK_NO, GTK_RESPONSE_NO );
    gtk_dialog_add_button( GTK_DIALOG( dialog ),
				    GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL );
    response= gtk_dialog_run( GTK_DIALOG( dialog ) );
    gtk_widget_destroy( dialog );

    switch( response )
	{
	case GTK_RESPONSE_YES:		return AQDrespYES;
	case GTK_RESPONSE_NO:		return AQDrespNO;
	case GTK_RESPONSE_CANCEL:	return AQDrespCANCEL;
	default:
	    LDEB(response); return AQDrespCANCEL;
	}
    }

void appQuestionRunSubjectErrorDialog(	EditApplication *	ea,
					APP_WIDGET		relative,
					APP_WIDGET		option,
					const char *		subject,
					const char *		message )
    {
    GtkWidget *		dialog;

    if  ( ! ea->eaToplevel.atTopWidget )
	{
	appDebug( "%s: \"%s\" %s\n", ea->eaApplicationName, subject, message );
	return;
	}

    dialog=  gtk_message_dialog_new( GTK_WINDOW( relative ), GTK_DIALOG_MODAL,
				GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, message, 1 );
    g_object_set( G_OBJECT( dialog ),
		    "secondary-text", subject,
		    NULL );
    gtk_dialog_run( GTK_DIALOG( dialog ) );
    gtk_widget_destroy( dialog );

    return;
    }

int appQuestionRunOkCancelDialog(	EditApplication *	ea,
					APP_WIDGET		relative,
					APP_WIDGET		option,
					const char *		question )
    {
    GtkWidget *		dialog;
    int			response;

    if  ( ! ea->eaToplevel.atTopWidget )
	{
	appDebug( "%s: %s\n", ea->eaApplicationName, question );
	return AQDrespCANCEL;
	}

    dialog=  gtk_message_dialog_new( GTK_WINDOW( relative ), GTK_DIALOG_MODAL,
		    GTK_MESSAGE_QUESTION, GTK_BUTTONS_OK_CANCEL, question, 1 );
    response= gtk_dialog_run( GTK_DIALOG( dialog ) );
    gtk_widget_destroy( dialog );

    switch( response )
	{
	case GTK_RESPONSE_OK:		return AQDrespOK;
	case GTK_RESPONSE_CANCEL:	return AQDrespCANCEL;
	default:
	    LDEB(response); return AQDrespCANCEL;
	}
    }

int appQuestionRunSubjectOkCancelDialog( EditApplication *	ea,
					APP_WIDGET		relative,
					APP_WIDGET		option,
					const char *		subject,
					const char *		message )
    {
    GtkWidget *		dialog;
    int			response;

    if  ( ! ea->eaToplevel.atTopWidget )
	{
	appDebug( "%s: \"%s\" %s\n", ea->eaApplicationName, subject, message );
	return AQDrespCANCEL;
	}

    dialog=  gtk_message_dialog_new( GTK_WINDOW( relative ), GTK_DIALOG_MODAL,
		    GTK_MESSAGE_QUESTION, GTK_BUTTONS_OK_CANCEL, message, 1 );
    g_object_set( G_OBJECT( dialog ),
			"secondary-text", subject,
			NULL );
    response= gtk_dialog_run( GTK_DIALOG( dialog ) );
    gtk_widget_destroy( dialog );

    switch( response )
	{
	case GTK_RESPONSE_OK:		return AQDrespOK;
	case GTK_RESPONSE_CANCEL:	return AQDrespCANCEL;
	default:
	    LDEB(response); return AQDrespCANCEL;
	}
    }

int appQuestionRunSubjectYesNoDialog(	EditApplication *	ea,
					APP_WIDGET		relative,
					APP_WIDGET		option,
					const char *		subject,
					const char *		message )
    {
    GtkWidget *		dialog;
    int			response;

    if  ( ! ea->eaToplevel.atTopWidget )
	{
	appDebug( "%s: \"%s\" %s\n", ea->eaApplicationName, subject, message );
	return AQDrespFAILURE;
	}

    dialog=  gtk_message_dialog_new( GTK_WINDOW( relative ), GTK_DIALOG_MODAL,
			GTK_MESSAGE_QUESTION, GTK_BUTTONS_NONE, message, 1 );
    g_object_set( G_OBJECT( dialog ),
		    "secondary-text", subject,
		    NULL );
    gtk_dialog_add_button( GTK_DIALOG( dialog ),
				    GTK_STOCK_YES, GTK_RESPONSE_YES );
    gtk_dialog_add_button( GTK_DIALOG( dialog ),
				    GTK_STOCK_NO, GTK_RESPONSE_NO );
    response= gtk_dialog_run( GTK_DIALOG( dialog ) );
    gtk_widget_destroy( dialog );

    switch( response )
	{
	case GTK_RESPONSE_NO:	return AQDrespNO;
	case GTK_RESPONSE_YES:	return AQDrespYES;
	default:
	    LDEB(response); return AQDrespNO;
	}
    }

void appQuestionRunErrorDialog(	EditApplication *	ea,
				APP_WIDGET		relative,
				APP_WIDGET		option,
				const char *		message )
    {
    GtkWidget *		dialog;

    if  ( ! ea->eaToplevel.atTopWidget )
	{
	appDebug( "%s: %s\n", ea->eaApplicationName, message );
	return;
	}

    dialog=  gtk_message_dialog_new( GTK_WINDOW( relative ), GTK_DIALOG_MODAL,
				GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, message, 1 );
    gtk_dialog_run( GTK_DIALOG( dialog ) );
    gtk_widget_destroy( dialog );
    }

# endif
