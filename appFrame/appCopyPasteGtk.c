/************************************************************************/
/*  Editor, File actions.						*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>

#   include	"appFrame.h"

#   include	<appDebugon.h>

#   ifdef	USE_GTK

# define ADEB(a) SDEB(((a)==0?"None":gdk_atom_name((a))))

/************************************************************************/

/* # define LIST_TARGET_TYPES */

# ifdef LIST_TARGET_TYPES
static GdkAtom targets_atom = GDK_NONE;

static void appAskForTargetTypes(	GtkWidget *		widget,
					const char *		selection )
    {
    GdkAtom	selectionAtom= gdk_atom_intern( selection, TRUE );

    if  ( targets_atom == GDK_NONE )
	{
	targets_atom= gdk_atom_intern( "TARGETS", FALSE );
	}

    gtk_selection_convert (widget, selectionAtom, targets_atom,
							     GDK_CURRENT_TIME);
    }
# endif

/************************************************************************/
/*									*/
/*  Look for a selection type.						*/
/*									*/
/************************************************************************/

static int appGetResponseType(	AppSelectionType **		pAst,
				AppSelectionTargetType **	pAstt,
				int *				pTtargetFound,
				AppSelectionType *		ast,
				int				astCount,
				APP_ATOM			selection,
				APP_ATOM			target )
    {
    int				i;

    AppSelectionTargetType *	astt;

    for ( i= 0; i < astCount; ast++, i++ )
	{
	if  ( ast->astSelectionAtom == selection )
	    { break;	}
	}

    if  ( i >= astCount )
	{ return -1; }

    astt= ast->astTargetTypes;
    for ( i= 0; i < ast->astTargetTypeCount; astt++, i++ )
	{
	if  ( astt->asttTargetAtom == target )
	    { break;	}
	}

    if  ( i >= ast->astTargetTypeCount )
	{ return -1; }

    *pAst= ast; *pTtargetFound= i; *pAstt= astt; return 0;
    }

/************************************************************************/
/*									*/
/*  Respond to selection events.					*/
/*									*/
/************************************************************************/

APP_EVENT_HANDLER_H( appAppGotPasteCall, w, voided, event )
    {
LDEB(1); return;
    }

int appDocReleaseSelection(	EditDocument *			ed,
				const char *			selection )
    {
    EditApplication *		ea= ed->edApplication;
    const AppSelectionType *	ast;

    ast= appDocGetSelectionType( ea, selection );
    if  ( ! ast )
	{ SXDEB(selection,ast); return -1;	}

    if  ( ! gtk_selection_owner_set( NULL, ast->astSelectionAtom,
							GDK_CURRENT_TIME ) )
	{ return -1;	}

    return 0;
    }

int appDocOwnSelection(		EditDocument *			ed,
				const char *			selection,
				AppSelectionTargetType * 	targets,
				int				targetCount )
    {
    int				i;
    EditApplication *		ea= ed->edApplication;
    const AppSelectionType *	ast;

    ast= appDocGetSelectionType( ea, selection );
    if  ( ! ast )
	{ SXDEB(selection,ast); return -1;	}

    if  ( ast->astTargetTypeCount == 0 )
	{ SLDEB(selection,ast->astTargetTypeCount); return -1;	}

    for ( i= 0; i < targetCount; i++ )
	{
	if  ( ! targets[i].asttTargetAtom )
	    {
	    targets[i].asttTargetAtom= gdk_atom_intern(
					    targets[i].asttTargetString, 0 );

	    if  ( ! targets[i].asttTargetAtom )
		{
		SDEB(targets[i].asttTargetString);
		XDEB(targets[i].asttTargetAtom);
		return -1;
		}
	    }

	gtk_selection_add_target( ed->edDocumentWidget.dwWidget,
						ast->astSelectionAtom,
						targets[i].asttTargetAtom,
						0 );
	}

    ed->edTargetTypes= targets;
    ed->edTargetTypeCount= targetCount;

    if  ( ! gtk_selection_owner_set( ed->edDocumentWidget.dwWidget,
						    ast->astSelectionAtom,
						    GDK_CURRENT_TIME ) )
	{ return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Paste loop:								*/
/*									*/
/************************************************************************/

static int appAskForPaste(	APP_WIDGET		w,
				EditApplication *	ea,
				const char *		selection )
    {
    const AppSelectionType *	ast;

    ea->eaGotPaste= 0;

    ast= appDocGetSelectionType( ea, selection );
    if  ( ! ast )
	{ SXDEB(selection,ast); return -1;	}

    if  ( ast->astTargetTypeCount == 0 )
	{ SLDEB(selection,ast->astTargetTypeCount); return -1;	}

#   ifdef LIST_TARGET_TYPES
    appAskForTargetTypes( w, selection );
#   endif

    if  ( ! gtk_selection_convert( w,
		/*  selection	*/  ast->astSelectionAtom,
		/*  target	*/  ast->astTargetTypes[0].asttTargetAtom,
				    GDK_CURRENT_TIME ) )
	{ SDEB(selection); return -1;	}

    gtk_main();

    /*  1  */
    if  ( ea->eaGotPaste < 0 )
	{ return -1;	}

    return 0;
    }

void appDocGotPasteReplyGtk(	GtkWidget *		w,
				GtkSelectionData *	gsd,
				guint			time,
				void *			voided )
    {
    EditDocument *		ed= (EditDocument *)voided;
    EditApplication *		ea= ed->edApplication;

    AppSelectionType *		ast;
    AppSelectionTargetType *	astt;
    int				targetFound;

#   ifdef LIST_TARGET_TYPES
    if  ( gsd->target == targets_atom )
	{
	int		t;
	GdkAtom *	atoms= (GdkAtom *)gsd->data;

	for ( t= 0; t < gsd->length/sizeof(GdkAtom); t++ )
	    {
boe
	    ADEB(atoms[t]);
	    }
	}
#   endif

    if  ( appGetResponseType( &ast, &astt, &targetFound,
					    ea->eaDocSelectionTypes,
					    ea->eaDocSelectionTypeCount,
					    gsd->selection, gsd->target ) )
	{ LDEB(1); return;	}

    if  ( ! gsd->type )
	{
	ea->eaGotPaste= -1;

	if  ( targetFound < ast->astTargetTypeCount- 1 )
	    {
	    if  ( ! gtk_selection_convert( w,
			/*  selection	*/  gsd->selection,
			/*  target	*/  astt[1].asttTargetAtom,
					    time ) )
		{ LDEB(1); return;	}

	    ea->eaGotPaste= 0;
	    }
	}
    else{
	(*astt->asttUsePaste)( w, gsd, time, voided );
	ea->eaGotPaste= 1;
	gtk_main_quit();
	}
    }

void appAppGotPasteReplyGtk(	GtkWidget *		w,
				GtkSelectionData *	gsd,
				guint			time,
				void *			voidea )
    {
    EditApplication *	ea= (EditApplication *)voidea;
LDEB(1);
ea->eaGotPaste= 1;
    gtk_main_quit();
    }

int appDocAskForPaste(		EditDocument *		ed,
				const char *		selection )
    {
    return appAskForPaste( ed->edDocumentWidget.dwWidget,
					ed->edApplication, selection );
    }

/*  2  */
int appAppAskForPaste(		EditApplication *	ea,
				const char *		selection )
    {
    int		rval;
    guint	id;

    id= gtk_signal_connect( GTK_OBJECT( ea->eaToplevel.atTopWidget ),
					"selection_received",
					(GtkSignalFunc)appAppGotPasteReplyGtk,
					(void *)ea );

    rval= appAskForPaste( ea->eaToplevel.atTopWidget, ea, selection );

    gtk_signal_disconnect( GTK_OBJECT( ea->eaToplevel.atTopWidget ), id );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Deliver the data that you previously copied to someone who wants	*/
/*  to paste it.							*/
/*									*/
/************************************************************************/

APP_GIVE_COPY( appDocReplyToCopyRequest, w, gsd, voided )
    {
    EditDocument *		ed= (EditDocument *)voided;
    EditApplication *		ea= ed->edApplication;

    AppSelectionType *		ast;
    AppSelectionTargetType *	astt;
    int				targetFound;

    if  ( appGetResponseType( &ast, &astt, &targetFound,
					    ea->eaDocSelectionTypes,
					    ea->eaDocSelectionTypeCount,
					    gsd->selection, gsd->target ) )
	{ LDEB(1); return;	}

    if  ( ! astt->asttGiveCopy )
	{ XDEB(astt->asttGiveCopy); return;	}

    (*astt->asttGiveCopy)( w, gsd, info, tim, voided );

    return;
    }

/************************************************************************/
/*									*/
/*  You can forget the selection you just copied because no one is	*/
/*  going to ask for it anymore.					*/
/*									*/
/************************************************************************/

static void appDocForgetCopiedSelection(	APP_WIDGET		w,
						void *			voided,
						APP_EVENT *		event )
    {
    GdkEventSelection *		clrEvent= &(event->selection);
    EditDocument *		ed= (EditDocument *)voided;
    EditApplication *		ea= ed->edApplication;

    int				i;
    AppSelectionType *		ast;

    ast= ea->eaDocSelectionTypes;
    for ( i= 0; i < ea->eaDocSelectionTypeCount; ast++, i++ )
	{
	if  ( ast->astSelectionAtom == clrEvent->selection )
	    { break;	}
	}

    if  ( i >= ea->eaDocSelectionTypeCount )
	{ ADEB(clrEvent->selection); return; }

    if  ( ast->astForgetCopy )
	{ (*ast->astForgetCopy)( w, voided, event );	}

    return;
    }

/************************************************************************/
/*									*/
/*  CopyPaste event handler.						*/
/*									*/
/************************************************************************/

APP_EVENT_HANDLER_H( appDocCopyPasteHandler, w, voided, event )
    {
    switch( event->type )
	{
	/*
	case GDK_SELECTION_NOTIFY:
	    appDocGotPasteReplyGtk( w, event, time, voided );
	    break;
	*/

	case GDK_SELECTION_CLEAR:
	    appDocForgetCopiedSelection( w, voided, event );
	    break;

	default:
	    LDEB(event->type);
	    break;
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Allocate atoms for the different selection types.			*/
/*									*/
/************************************************************************/

void appAllocateCopyPasteTargetAtoms(	EditApplication *	ea )
    {
    AppSelectionType *		ast;
    int				i;

    int				j;
    AppSelectionTargetType *	astt;

    ast= ea->eaDocSelectionTypes;
    for ( i= 0; i < ea->eaDocSelectionTypeCount; ast++, i++ )
	{
	if  ( ! ast->astSelectionAtom )
	    {
	    ast->astSelectionAtom= gdk_atom_intern(
					    ast->astSelectionString, 0 );
	    if  ( ! ast->astSelectionAtom )
		{ SLDEB(ast->astSelectionString, ast->astSelectionAtom); }
	    }
	astt= ast->astTargetTypes;
	for ( j= 0; j < ast->astTargetTypeCount; astt++, j++ )
	    {
	    if  ( ! astt->asttTargetAtom )
		{
		astt->asttTargetAtom= gdk_atom_intern(
					    astt->asttTargetString, 0 );
		if  ( ! astt->asttTargetAtom )
		    { SLDEB(astt->asttTargetString, astt->asttTargetAtom); }
		}
	    }
	}

    ast= ea->eaAppSelectionTypes;
    for ( i= 0; i < ea->eaAppSelectionTypeCount; ast++, i++ )
	{
	if  ( ! ast->astSelectionAtom )
	    {
	    ast->astSelectionAtom= gdk_atom_intern(
					    ast->astSelectionString, 0 );
	    if  ( ! ast->astSelectionAtom )
		{ SLDEB(ast->astSelectionString, ast->astSelectionAtom); }
	    }
	astt= ast->astTargetTypes;
	for ( j= 0; j < ast->astTargetTypeCount; astt++, j++ )
	    {
	    if  ( ! astt->asttTargetAtom )
		{
		astt->asttTargetAtom= gdk_atom_intern(
					    astt->asttTargetString, 0 );
		if  ( ! astt->asttTargetAtom )
		    { SLDEB(astt->asttTargetString, astt->asttTargetAtom); }
		}
	    }
	}

    return;
    }


#   endif
