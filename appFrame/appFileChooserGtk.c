/************************************************************************/
/*									*/
/*  Run file choosers for edit applications.				*/
/*									*/
/*  GTK specific code. This code is based on original code by Andrea	*/
/*  Frome.								*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>

#   include	<appSystem.h>

#   include	"appFrame.h"
#   include	"appQuestion.h"
#   include	"appFileChooser.h"

#   include	<appDebugon.h>

#   ifdef USE_GTK

# ifndef GTK_FILE_CHOOSER
#	define	HAS_DIRECTORY			0
#	define	MAKE_SELECTION			1
#	define	MAKE_CHOOSER			0
#	define	GtkFileChooser			GtkFileSelection
#	define	gtk_file_chooser_set_filename	gtk_file_selection_set_filename 
#	define	gtk_file_chooser_get_filename	gtk_file_selection_get_filename 
# else
#	define	HAS_DIRECTORY			1
#	define	MAKE_SELECTION			0
#	define	MAKE_CHOOSER			1
# endif

/************************************************************************/
/*									*/
/*  Cleanup when a chooser is destroyed.				*/
/*									*/
/************************************************************************/

static void appChooserDestroyed(APP_WIDGET	fileChooser,
				GdkEvent *	event,
				void *		voidaci )
    {
    AppChooserInformation *	aci= (AppChooserInformation *)voidaci;

    utilCleanMemoryBuffer( &(aci->aciFilename) );

    free( aci );
    }

static void appInitAci(		AppChooserInformation *	aci )
    {
    appInitOptionmenu( &(aci->aciFilterOptionmenu) );

    aci->aciApplication= (EditApplication *)0;
    aci->aciDocument= (EditDocument *)0;
    utilInitMemoryBuffer( &(aci->aciFilename) );
    aci->aciThrough= (void *)0;
    aci->aciOpenDocument= (APP_OPEN_DOCUMENT)0;
    aci->aciExtensions= (AppFileExtension *)0;
    aci->aciExtensionCount= 0;
    aci->aciOption= (APP_WIDGET)0;
    aci->aciRelativeTo= (APP_WIDGET)0;
    aci->aciFormat= -1;

    aci->aciResources= (const AppFileChooserResources *)0;
    }

#   if HAS_DIRECTORY
static void appChooserSetDirectory(	const MemoryBuffer *	filename,
					GtkFileChooser *	gfs )
    {
    MemoryBuffer	dir;

    utilInitMemoryBuffer( &dir );

    if  ( appDirectoryOfFileName( &dir, filename ) )
	{ LDEB(1); return;	}

    if  ( ! utilMemoryBufferIsEmpty( &dir ) )
	{
	gtk_file_chooser_set_current_folder( gfs,
					utilMemoryBufferGetString( &dir ) );
	}

    utilCleanMemoryBuffer( &dir );

    return;
    }
#   endif /* HAS_DIRECTORY */

static void appFileChooserSetFilter( AppChooserInformation *	aci,
				     int			filter )
    {
    GtkFileChooser *		gfs= GTK_FILE_CHOOSER( aci->aciWidget );
    GtkFileFilter *		fileFilter;
    const AppFileExtension *	afe= aci->aciExtensions+ filter;

    fileFilter= gtk_file_filter_new();
    gtk_file_filter_add_pattern( fileFilter, afe->afeFilter );
    gtk_file_filter_set_name( fileFilter, afe->afeDescription );
    gtk_file_chooser_set_filter( gfs, fileFilter );
    }

/************************************************************************/
/*									*/
/*  Change the extension of the file name.				*/
/*									*/
/************************************************************************/

static void appChooserSetFileExtension(	const MemoryBuffer *	filename,
					const char *		newExtension,
					GtkFileChooser *	gfs,
					int			selectFile )
    {
    MemoryBuffer	oldExtension;
    MemoryBuffer	newFileName;

    utilInitMemoryBuffer( &oldExtension );
    utilInitMemoryBuffer( &newFileName );

    if  ( appFileGetFileExtension( &oldExtension, filename ) )
	{ goto ready;	}

    if  ( ! utilMemoryBufferEqualsString( &oldExtension, newExtension ) )
	{
	const char *	s;

	if  ( utilCopyMemoryBuffer( &newFileName, filename ) )
	    { LDEB(1); goto ready;	}
	if  ( appFileSetExtension( &newFileName, newExtension ) )
	    { LDEB(1); goto ready;	}

	s= utilMemoryBufferGetString( &newFileName );

	if  ( selectFile )
	    { gtk_file_chooser_set_filename( gfs, s );	}

	gtk_file_chooser_set_current_name( gfs, s );
	}

  ready:

    utilCleanMemoryBuffer( &oldExtension );
    utilCleanMemoryBuffer( &newFileName );

    return;
    }

/************************************************************************/
/*									*/
/*  Make a chooser.							*/
/*									*/
/************************************************************************/

#   if	MAKE_CHOOSER
static int appMakeFileChooser(	AppChooserInformation **	pAci,
				GtkFileChooserAction		action,
				const gchar *			actionText,
				APP_WIDGET			relative,
				EditApplication *		ea,
				APP_WIDGET			option )
    {
    AppChooserInformation *	aci;
    const char *		title= "<@v@>";

    while( relative )
	{
	if  ( GTK_IS_WINDOW( relative ) )
	    { break;	}

	relative= relative->parent;
	}

    aci= (AppChooserInformation *)malloc( sizeof(AppChooserInformation) );
    if  ( ! aci )
	{ XDEB(aci); return -1;	}

    appInitAci( aci );

    appFileChooserGetTexts( ea, aci );

    if  ( option )
	{
	/* needs not to be freed for GTK */
	title= appGetTextFromMenuOption( option );
	}

    aci->aciWidget= gtk_file_chooser_dialog_new( title,
					GTK_WINDOW( relative ),
					action,
					GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
					actionText, GTK_RESPONSE_ACCEPT,
					NULL);

    gtk_signal_connect( GTK_OBJECT(  aci->aciWidget ), "destroy_event",
			GTK_SIGNAL_FUNC( appChooserDestroyed ), (void *)aci );

    aci->aciApplication= ea;

    if  ( option )
	{
	appSetShellTitle( aci->aciWidget, option, ea->eaApplicationName );
	}

    *pAci= aci;
    return 0;
    }
#   endif /*	MAKE_CHOOSER  */

#   if	MAKE_SELECTION

/************************************************************************/
/*									*/
/*  Callback for cancel buttons on choosers.				*/
/*									*/
/************************************************************************/

static int appMakeFileSelection(AppChooserInformation **	pAci,
				APP_WIDGET			relative,
				EditApplication *		ea,
				APP_WIDGET			option )
    {
    AppChooserInformation *	aci;
    const char *		title= "<@v@>";

    GtkFileSelection *		gfs;

    aci= (AppChooserInformation *)malloc( sizeof(AppChooserInformation) );
    if  ( ! aci )
	{ XDEB(aci); return -1;	}

    appInitAci( aci );

    appFileChooserGetTexts( ea, aci );

    if  ( option )
	{
	/* needs not to be freed for GTK */
	title= appGetTextFromMenuOption( option );
	}

    aci->aciWidget= gtk_file_selection_new( title );
    gfs= GTK_FILE_SELECTION( aci->aciWidget );

    gtk_file_selection_hide_fileop_buttons( gfs );

    appSetShellTitle( aci->aciWidget, option, ea->eaApplicationName );

    gtk_signal_connect( GTK_OBJECT( gfs ), "destroy_event",
			GTK_SIGNAL_FUNC( appChooserDestroyed ), (void *)aci );

    aci->aciApplication= ea;

    *pAci= aci;
    return 0;
    }

#   endif  /*	MAKE_SELECTION */

/************************************************************************/
/*									*/
/*  Run some kind of a file chooser.					*/
/*									*/
/************************************************************************/

static int appRunFileChooser(	EditApplication *		ea,
				int				acceptResponse,
				AppChooserInformation *		aci,
				APP_WIDGET			relative )
    {
    int		rval= ACIrespNONE;
    int		response;

#   ifdef GTK_FILE_CHOOSER
    response= gtk_dialog_run( GTK_DIALOG( aci->aciWidget ) );
    switch( response )
	{
	case GTK_RESPONSE_ACCEPT:
	case GTK_RESPONSE_OK:
	    rval= acceptResponse;
	    break;

	case GTK_RESPONSE_CANCEL:
	case GTK_RESPONSE_DELETE_EVENT:
	    rval= ACIrespCANCEL;
	    break;

	default:
	    LDEB(response);
	    rval= ACIrespFAILURE;
	    break;
	}
#   else
    utilEmptyMemoryBuffer( &(aci->aciFilename) );

    appGuiShowDialog( ea, &(aci->aciDialog), relative );

    appGuiRunDialog( &(aci->aciDialog), ACIrespNONE, ea );

    appGuiHideDialog( &(aci->aciDialog) );
#   endif

    return rval;
    }

/************************************************************************/
/*									*/
/*  Open a document.							*/
/*									*/
/************************************************************************/

static void appFileChooserOpenFile(	AppChooserInformation *	aci,
					const MemoryBuffer *	filename,
					APP_OPEN_DOCUMENT	openDocument,
					void *			through )
    {
    int			ret;
    const char *	extension= (const char *)0;

    ret= appChooserSaveFilename( aci, filename, extension );

    if  ( ! ret )
	{
	ret= appFileChooserTestNameForOpen( aci );
	if  ( ret )
	    { LDEB(1);	}
	}

    if  ( ! ret && (*openDocument)( aci->aciApplication,
					through, aci->aciRelativeTo,
					aci->aciOption, &(aci->aciFilename) ) )
	{ LDEB(1);	}

    return;
    }

#   if MAKE_CHOOSER

static void appFileChooserOpenFiles(	AppChooserInformation *	aci,
					GtkFileChooser *	gfs,
					APP_OPEN_DOCUMENT	openDocument,
					void *			through )
    {
    GSList *	filenames;
    GSList *	fn;

    MemoryBuffer	filename;

    utilInitMemoryBuffer( &filename );

    fn= filenames= gtk_file_chooser_get_filenames( gfs );
    if  ( filenames )
	{
	while( fn )
	    {
	    gchar *		fnm= (gchar *)(fn->data);

	    if  ( utilMemoryBufferSetString( &filename, fnm ) )
		{ LDEB(1); goto ready;	}

	    appFileChooserOpenFile( aci, &filename, openDocument, through );

	    g_free( fn->data );
	    fn= fn->next;
	    }

	g_slist_free( filenames );
	}

  ready:

    utilCleanMemoryBuffer( &filename );

    return;
    }

#   endif

#   if MAKE_SELECTION

static void appFileChooserOpenFiles(	AppChooserInformation *	aci,
					APP_WIDGET		option,
					GtkFileChooser *	gfs,
					APP_OPEN_DOCUMENT	openDocument,
					void *			through )
    {
#   if GTK_MAJOR_VERSION >= 2
    gchar **	filenames;
    gchar **	fn;

    fn= filenames= gtk_file_selection_get_selections( gfs );
    if  ( filenames )
	{
	while( fn[0] )
	    {
	    gchar *		filename= *fn;

	    appFileChooserOpenFile( aci, filename, openDocument, through );

	    fn++;
	    }

	g_strfreev( filenames );
	}
#   else

    gchar * filename= gtk_file_selection_get_filename( gfs );
    if  ( filename )
	{
	appFileChooserOpenFile( aci, filename, openDocument, through );
	}

#   endif

    return;
    }

#   endif

void appRunOpenChooser( APP_WIDGET			option,
			APP_WIDGET			relative,
			int				extensionCount,
			AppFileExtension *		extensions,
			const char *			defaultFilter,
			const MemoryBuffer *		dir,
			APP_OPEN_DOCUMENT		openDocument,
			EditApplication *		ea,
			void *				through )
    {
    int				response;
    GtkFileChooser *		gfs;
    AppChooserInformation *	aci= (AppChooserInformation *)0;

#   if MAKE_CHOOSER
    int				withFilter= extensionCount > 0;

    if  ( appMakeFileChooser( &aci, GTK_FILE_CHOOSER_ACTION_OPEN,
				    GTK_STOCK_OPEN, relative, ea, option ) )
	{ LDEB(1); return;	}

    gfs= GTK_FILE_CHOOSER( aci->aciWidget );

    gtk_file_chooser_set_select_multiple( gfs, TRUE );

    if  ( withFilter )
	{
	AppFileExtension *	afe;
	int			i;

	GtkFileFilter *		defaultFileFilter= (GtkFileFilter *)0;

	if  ( appFileFilterGetDescriptions( ea, extensions, extensionCount ) )
	    { LDEB(extensionCount);	}

	afe= extensions;
	for ( i= 0; i < extensionCount; afe++, i++ )
	    {
	    GtkFileFilter *	fileFilter;

	    if  ( ! FILE_CHOOSER_CAN_OPEN( afe ) )
		{ continue;	}

	    fileFilter= gtk_file_filter_new();
	    gtk_file_filter_add_pattern( fileFilter, afe->afeFilter );
	    gtk_file_filter_set_name( fileFilter, afe->afeDescription );

	    gtk_file_chooser_add_filter( gfs, fileFilter );

	    if  ( ! defaultFileFilter )
		{ defaultFileFilter= fileFilter;	}
	    if  ( defaultFilter && ! strcmp( afe->afeFilter, defaultFilter ) )
		{ defaultFileFilter= fileFilter;	}
	    }

	if  ( defaultFileFilter )
	    { gtk_file_chooser_set_filter( gfs, defaultFileFilter );	}
	}
#   endif /* MAKE_CHOOSER */

#   if MAKE_SELECTION

    if  ( appMakeFileSelection( &aci, relative, ea, option ) )
	{ LDEB(1); return;	}

    gfs= GTK_FILE_SELECTION( aci->aciWidget );

#   if GTK_MAJOR_VERSION >= 2
    gtk_file_selection_set_select_multiple( gfs, TRUE );
#   endif

#   endif /*  MAKE_SELECTION  */

    if  ( dir && ! utilMemoryBufferIsEmpty( dir ) )
	{
	gtk_file_chooser_set_current_folder( gfs,
					utilMemoryBufferGetString( dir ) );
	}

    aci->aciApplication= ea;
    aci->aciOption= option;
    aci->aciRelativeTo= relative;
    aci->aciThrough= through;
    aci->aciOpenDocument= openDocument;

    response= appRunFileChooser( ea, ACIrespOPEN, aci, relative );
    switch( response )
	{
	case ACIrespOPEN:
	    appFileChooserOpenFiles( aci, gfs, openDocument, through );
	    break;

	case ACIrespCANCEL:
	    break;

	default:
	    LDEB(response);
	    break;
	}

    gtk_widget_destroy( aci->aciWidget );

    return;
    }

APP_MENU_CALLBACK_H( appAppFileOpen, option, voidea, e )
    {
    EditApplication *	ea= (EditApplication *)voidea;

    appRunOpenChooser( GTK_WIDGET( option ), ea->eaToplevel.atTopWidget,
			ea->eaFileExtensionCount, ea->eaFileExtensions,
			ea->eaDefaultFileFilter, (MemoryBuffer *)0,
			appChooserOpenDocument, ea, (void *)ea );
    }

APP_MENU_CALLBACK_H( appDocFileOpen, option, voided, e )
    {
    EditDocument *	ed= (EditDocument *)voided;
    EditApplication *	ea= ed->edApplication;

    appRunOpenChooser( GTK_WIDGET( option ), ed->edToplevel.atTopWidget,
			ea->eaFileExtensionCount, ea->eaFileExtensions,
			ea->eaDefaultFileFilter, (MemoryBuffer *)0,
			appChooserOpenDocument, ea, (void *)ea );
    }

static int appChooserGetFileName(	MemoryBuffer *		fileName,
					GtkFileChooser *	gfs )
    {
    const gchar *	filename;

#   if MAKE_CHOOSER
    filename= gtk_file_chooser_get_filename( gfs );
#   endif

#   if MAKE_SELECTION
    filename= gtk_file_selection_get_filename( gfs );
#   endif

    if  ( utilMemoryBufferSetString( fileName, filename ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

static int appSaveChooserSave(	AppChooserInformation *	aci,
				APP_WIDGET		relative,
				unsigned int		useFlags,
				GtkFileChooser *	gfs,
				EditDocument *		ed )
    {
    int			rval= 0;
    EditApplication *	ea= ed->edApplication;

    int			format= -1;
    const char *	extension= (const char *)0;
    int			ret;
    int			resp;
    int			suggestStdout= 0;

    MemoryBuffer	filename;

    utilInitMemoryBuffer( &filename );

    if  ( appChooserGetFileName( &filename, gfs ) )
	{ LDEB(1); rval= -1; goto ready;	}

    format= appDocumentGetSaveFormat( &suggestStdout, ea, &filename,
				ed->edPrivateData, useFlags, ed->edFormat );

    if  ( format >= 0 && format < ea->eaFileExtensionCount )
	{ extension= ea->eaFileExtensions[format].afeExtension; }

    ret= appChooserSaveFilename( aci, &filename, extension );
    if  ( ret )
	{ SDEB(extension); rval= -1; goto ready;	}

    resp= appFileChooserTestNameForWrite( aci );

    if  ( resp == ACIrespSAVE )
	{
	if  ( (*aci->aciSaveDocument)( ed, aci->aciThrough,
				    aci->aciRelativeTo, aci->aciOption,
				    format, &(aci->aciFilename) ) )
	    { LDEB(format); rval= -1; goto ready;	}
	}

  ready:

    utilCleanMemoryBuffer( &filename );

    return rval;
    }

static void appFileFilterChosen(	int		ext,
					void *		voidaci )
    {
    AppChooserInformation *	aci= (AppChooserInformation *)voidaci;
    GtkFileChooser *		gfs= GTK_FILE_CHOOSER( aci->aciWidget );
    const AppFileExtension *	afe= aci->aciExtensions;
    char *			fnm;

    MemoryBuffer		filename;

    utilInitMemoryBuffer( &filename );

    fnm= gtk_file_chooser_get_filename( gfs );

    if  ( fnm )
	{
	const int	selectFile= 0;
	const char *	newExtension= afe[ext].afeExtension;

	if  ( utilMemoryBufferSetString( &filename, fnm ) )
	    { LDEB(1); goto ready;	}

	appFileChooserSetFilter( aci, ext );
	appChooserSetFileExtension( &filename, newExtension, gfs, selectFile );
	}

  ready:

    utilCleanMemoryBuffer( &filename );

    return;
    }

static void appFillSaveFilter(	AppChooserInformation *	aci,
				unsigned int		useFlags,
				EditDocument *		ed )
    {
    EditApplication *		ea= ed->edApplication;
    AppOptionmenu *		aom= &(aci->aciFilterOptionmenu);

    AppFileExtension *		afe;
    int				i;

    int				defaultFilterIndex= -1;

    if  ( appFileFilterGetDescriptions( ea,
			ea->eaFileExtensions, ea->eaFileExtensionCount ) )
	{ LDEB(ea->eaFileExtensionCount);	}

    aci->aciExtensions= ea->eaFileExtensions;
    aci->aciExtensionCount= ea->eaFileExtensionCount;

    afe= ea->eaFileExtensions;
    for ( i= 0; i < ea->eaFileExtensionCount; afe++, i++ )
	{
	APP_WIDGET	filterWidget;

	filterWidget= appAddItemToOptionmenu( aom, afe->afeDescription );

	if  ( appDocumentTestCanSave( ea, afe, ed->edPrivateData,
							    useFlags, i ) )
	    {
	    const int	visible= 0;

	    guiShowMenuOption( filterWidget, visible );
	    continue;
	    }

	/*
	fileFilter= gtk_file_filter_new();
	gtk_file_filter_add_pattern( fileFilter, afe->afeFilter );
	gtk_file_filter_set_name( fileFilter, afe->afeDescription );

	gtk_file_chooser_add_filter( gfs, fileFilter );
	*/

	if  ( defaultFilterIndex < 0 || i == ed->edFormat )
	    { defaultFilterIndex= i;	}
	}

    if  ( defaultFilterIndex >= 0 )
	{
	appFileChooserSetFilter( aci, defaultFilterIndex );
	appSetOptionmenu( aom, defaultFilterIndex );
	}

    return;
    }

int appRunSaveChooser(	APP_WIDGET		option,
			APP_WIDGET		relative,
			unsigned int		useFlags,
			APP_SAVE_DOCUMENT	saveDocument,
			EditDocument *		ed,
			void *			through )
    {
    EditApplication *		ea= ed->edApplication;
    int				response= ACIrespNONE;
    GtkFileChooser *		gfs;
    AppChooserInformation *	aci= (AppChooserInformation *)0;

#   if  MAKE_CHOOSER

    int				withFilter= ea->eaFileExtensionCount > 0;

    if  ( appMakeFileChooser( &aci, GTK_FILE_CHOOSER_ACTION_SAVE,
				    GTK_STOCK_SAVE, relative, ea, option ) )
	{ return ACIrespFAILURE;	}

    gfs= GTK_FILE_CHOOSER( aci->aciWidget );

    if  ( withFilter )
	{
	AppOptionmenu *	aom= &(aci->aciFilterOptionmenu);

	aom->aomPulldown= gtk_menu_new();
	aom->aomInplace= gtk_option_menu_new();
	aom->aomCallback= appFileFilterChosen;
	aom->aomTarget= (void *)aci;

	gtk_option_menu_set_menu( GTK_OPTION_MENU( aom->aomInplace ),
							    aom->aomPulldown );

	gtk_file_chooser_set_extra_widget( gfs, aom->aomInplace );

	appFillSaveFilter( aci, useFlags, ed );
	}

    if  ( ! utilMemoryBufferIsEmpty( &(ed->edFilename) ) )
	{
	const char *	fnm= utilMemoryBufferGetString( &(ed->edFilename) );

	gtk_file_chooser_set_filename( gfs, fnm );
	gtk_file_chooser_set_current_name( gfs, fnm );
	appChooserSetDirectory( &(ed->edFilename), gfs );
	}

    gtk_file_chooser_set_do_overwrite_confirmation( gfs, TRUE );

#   endif /* MAKE_CHOOSER */

#   if MAKE_SELECTION

    if  ( appMakeFileSelection( &aci, relative, ea, option ) )
	{ LDEB(1); return ACIrespFAILURE;	}

    gfs= GTK_FILE_SELECTION( aci->aciWidget );

#   endif /*  MAKE_SELECTION  */

    aci->aciOption= option;
    aci->aciRelativeTo= relative;
    aci->aciDocument= ed;
    aci->aciThrough= through;
    aci->aciSaveDocument= saveDocument;

    response= appRunFileChooser( ea, ACIrespSAVE, aci, relative );

    switch( response )
	{
	case ACIrespSAVE:
	    appSaveChooserSave( aci, relative, useFlags, gfs, ed );
	    break;

	case ACIrespCANCEL:
	    break;

	default:
	    LDEB(response);
	    break;
	}

    gtk_widget_destroy( aci->aciWidget );

    return response;
    }

/************************************************************************/
/*									*/
/*  Run Filechooser for print to file.					*/
/*									*/
/************************************************************************/

static int appSaveChooserPrint(	AppChooserInformation *	aci,
				const char *		extension )
    {
    int			rval= 0;
    GtkFileChooser *	gfs= GTK_FILE_CHOOSER( aci->aciWidget );
    const int		format= -1;

    const gchar *	fnm;
    int			ret;
    int			resp;

    MemoryBuffer	filename;

    utilInitMemoryBuffer( &filename );

    fnm= gtk_file_chooser_get_filename( gfs );
    if  ( utilMemoryBufferSetString( &filename, fnm ) )
	{ LDEB(1); rval= -1; goto ready;	}
    ret= appChooserSaveFilename( aci, &filename, extension );
    if  ( ret )
	{ SSDEB(fnm,extension); rval= -1; goto ready;	}

    resp= appFileChooserTestNameForWrite( aci );

    if  ( resp == ACIrespSAVE )
	{
	if  ( (*aci->aciSaveDocument)( aci->aciDocument, aci->aciThrough,
				    aci->aciRelativeTo, aci->aciOption,
				    format, &(aci->aciFilename) ) )
	    { SLDEB(fnm,format); rval= -1; goto ready;	}
	}

  ready:

    utilCleanMemoryBuffer( &filename );

    return rval;
    }


int appRunPrintToFileChooser(	APP_WIDGET		option,
				APP_WIDGET		relative,
				APP_SAVE_DOCUMENT	printDocument,
				EditDocument *		ed,
				void *			through )
    {
    EditApplication *		ea= ed->edApplication;
    int				response= ACIrespNONE;
    GtkFileChooser *		gfs;
    AppChooserInformation *	aci= (AppChooserInformation *)0;
    const char * const		psExt= "ps";

#   if MAKE_CHOOSER
    if  ( appMakeFileChooser( &aci, GTK_FILE_CHOOSER_ACTION_SAVE,
				    GTK_STOCK_SAVE, relative, ea, option ) )
	{ LDEB(1); return ACIrespFAILURE;	}

    gfs= GTK_FILE_CHOOSER( aci->aciWidget );

    {
    GtkFileFilter *		fileFilter= (GtkFileFilter *)0;

    fileFilter= gtk_file_filter_new();
    gtk_file_filter_add_pattern( fileFilter, "*.ps" );
    gtk_file_filter_set_name( fileFilter, "PostScript" );
    gtk_file_chooser_set_filter( gfs, fileFilter );
    }

    if  ( ! utilMemoryBufferIsEmpty( &(ed->edFilename) ) )
	{
	const int	selectFile= 1;
	appChooserSetFileExtension( &(ed->edFilename), psExt, gfs, selectFile );
	appChooserSetDirectory( &(ed->edFilename), gfs );
	}

    gtk_file_chooser_set_do_overwrite_confirmation( gfs, TRUE );

#   endif

#   if MAKE_SELECTION

    if  ( appMakeFileSelection( &aci, relative, ea, option ) )
	{ LDEB(1); return ACIrespFAILURE;	}

    gfs= GTK_FILE_SELECTION( aci->aciWidget );

    if  ( ! utilMemoryBufferIsEmpty( &(ed->edFilename) ) )
	{
	const int	selectFile= 1;
	appChooserSetFileExtension( &(ed->edFilename), psExt, gfs, selectFile );
	}

#   endif

    aci->aciOption= option;
    aci->aciRelativeTo= relative;
    aci->aciDocument= ed;
    aci->aciApplication= ed->edApplication;
    aci->aciThrough= through;
    aci->aciSaveDocument= printDocument;

    response= appRunFileChooser( ea, ACIrespSAVE, aci, relative );

    switch( response )
	{
	case ACIrespSAVE:
	    appSaveChooserPrint( aci, psExt );
	    break;

	case ACIrespCANCEL:
	    break;

	default:
	    LDEB(response);
	    break;
	}

    gtk_widget_destroy( aci->aciWidget );

    return response;
    }

int appFileChooserConfirmOverWrite(	const AppChooserInformation *	aci,
					const char *		filename )
    {
    GtkFileChooser *	gfs= GTK_FILE_CHOOSER( aci->aciWidget );
    int			rcc;

    /*  Do not ask if the GTK chooser already did */
    if  ( gtk_file_chooser_get_do_overwrite_confirmation( gfs ) )
	{ return ACIrespSAVE; }

    rcc= appQuestionRunSubjectYesNoCancelDialog(
			aci->aciApplication, aci->aciRelativeTo, aci->aciOption,
			filename, aci->aciResources->acrOverwriteMessage );

    switch( rcc )
	{
	case AQDrespYES:
	    return ACIrespSAVE;
	case AQDrespNO:
	    return ACIrespNONE;
	default:
	    LDEB(rcc);
	    /*FALLTHROUGH*/
	case AQDrespCANCEL:
	case AQDrespCLOSED:
	    return ACIrespCANCEL;
	}
    }

#   endif
