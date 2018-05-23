#   include	"appFrameConfig.h"

#   include	<stdio.h>
#   include	<string.h>

#   include	<uniUtf8.h>

#   include	"guiWidgets.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Keyboard event handling. Largely based on gtkentry.c		*/
/*									*/
/************************************************************************/

#   ifdef USE_GTK

# include	"appGuiKeysGtk.h"
# define	IS_MODIFIER(k) ( ( (k)>=GDK_Shift_L && (k) <= GDK_Hyper_R ) || \
				 (k) == GDK_Num_Lock )

static APP_EVENT_HANDLER_H( appKeyPressed, w, voiddw, event )
    {
    DocumentWidget *	dw= (DocumentWidget *)voiddw;
    GdkEventKey *	keyEvent= (GdkEventKey *)event;

    if  ( dw->dwInputContext )
	{
#	ifdef GTK_TYPE_IM_CONTEXT
	if  ( gtk_im_context_filter_keypress( dw->dwInputContext, keyEvent ) )
	    { return;	}
#	else
	SDEB( "## Impossible" );
#	endif
	}
    else{
#	if GTK_MAJOR_VERSION >= 2
	int	unicode= gdk_keyval_to_unicode( keyEvent->keyval );
#	else
	int	unicode= appKeysymX11ToUnicode( keyEvent->keyval );
#	endif

	if  ( dw->dwGotString && unicode > 0 )
	    {
	    char	scratch[8];
	    int		step= uniPutUtf8( scratch, unicode );

	    if  ( step > 0 )
		{
		scratch[step]= '\0';
		(*dw->dwGotString)( dw->dwOwner, (char *)scratch, step );
		return;
		}
	    }
	}

    if  ( ! IS_MODIFIER(keyEvent->keyval) && dw->dwGotKey )
	{
	(*dw->dwGotKey)( dw->dwOwner, keyEvent->keyval, keyEvent->state );
	return;
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Input context found a string: Replace the documents selection.	*/
/*									*/
/************************************************************************/

#   ifdef GTK_TYPE_IM_CONTEXT

static void appDocumentCommitText(	GtkIMContext *	im,
					const gchar *	str,
					void *		voiddw )
    {
    DocumentWidget *	dw= (DocumentWidget *)voiddw;

    if  ( dw->dwGotString )
	{
	(*dw->dwGotString)( dw->dwOwner, str, strlen( str ) );
	}
    }

#   endif

/************************************************************************/
/*									*/
/*  Set an input context for the document widget.			*/
/*									*/
/************************************************************************/

void appDocumentSetInputContext(	APP_INPUT_METHOD	im,
					DocumentWidget *	dw )
    {
    gtk_widget_add_events( dw->dwWidget, GDK_KEY_PRESS_MASK );
    gtk_widget_add_events( dw->dwWidget, GDK_KEY_RELEASE_MASK );

    gtk_signal_connect_after( GTK_OBJECT( dw->dwWidget ), "key_press_event",
				    (GtkSignalFunc)appKeyPressed, (void *)dw );

#   ifdef GTK_TYPE_IM_CONTEXT

    dw->dwInputContext= gtk_im_multicontext_new();

    if  ( dw->dwInputContext )
	{
	g_signal_connect( dw->dwInputContext, "commit",
			G_CALLBACK (appDocumentCommitText), (void *)dw );
#	if 0
	What is this? What is it needed for?

	g_signal_connect( ed->edInputContext, "preedit_changed",
			G_CALLBACK (gtk_entry_preedit_changed_cb), entry);
	g_signal_connect( ed->edInputContext, "retrieve_surrounding",
			G_CALLBACK (gtk_entry_retrieve_surrounding_cb), entry);
	g_signal_connect( ed->edInputContext, "delete_surrounding",
			G_CALLBACK (gtk_entry_delete_surrounding_cb), entry);
#	endif
	}

#   endif

    }

#   endif
