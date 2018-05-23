/************************************************************************/
/*									*/
/*  Run file choosers for edit applications.				*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>
#   include	<stdio.h>

#   include	<appSystem.h>

#   include	"appFrame.h"
#   include	"appFileChooser.h"
#   include	"appQuestion.h"
#   include	"guiWidgetsImpl.h"
#   include	"guiTextUtil.h"

#   include	<appDebugon.h>

#   ifdef USE_MOTIF

#   include	<Xm/Text.h>
#   include	<Xm/TextF.h>
#   include	<Xm/FileSB.h>
#   include	<Xm/Form.h>
#   include	<Xm/MwmUtil.h>
#   include	<Xm/RowColumn.h>

/************************************************************************/
/*									*/
/*  Cleanup when a chooser is destroyed.				*/
/*									*/
/************************************************************************/

static void appFreeAci(		Widget		fileChooser,
				XtPointer	voidaci,
				XtPointer	voidcbs	 )
    {
    AppChooserInformation *	aci= (AppChooserInformation *)voidaci;

    utilCleanMemoryBuffer( &(aci->aciFilename) );

    free( aci );
    }

static void appInitAci(		AppChooserInformation *	aci )
    {
    appInitOptionmenu( &(aci->aciFilterOptionmenu) );

    utilInitMemoryBuffer( &(aci->aciFilename) );
    aci->aciDocument= (EditDocument *)0;
    aci->aciThrough= (void *)0;
    aci->aciOpenDocument= (APP_OPEN_DOCUMENT)0;
    aci->aciSaveDocument= (APP_SAVE_DOCUMENT)0;
    aci->aciExtensions= (AppFileExtension *)0;
    aci->aciExtensionCount= 0;
    aci->aciApplication= (EditApplication *)0;
    aci->aciOption= (Widget)0;
    aci->aciRelativeTo= (Widget)0;
    aci->aciFormat= -1;

    aci->aciResources= (const AppFileChooserResources *)0;
    }

/************************************************************************/
/*									*/
/*  Callback for cancel buttons on choosers.				*/
/*									*/
/************************************************************************/

static void appChooserCancel(	APP_WIDGET	fileChooser,
				void *		voidaci,
				void *		voidcbs )
    {
    AppChooserInformation *	aci= (AppChooserInformation *)voidaci;

    appGuiBreakDialog( &(aci->aciDialog), ACIrespCANCEL );

    return;
    }

/************************************************************************/
/*									*/
/*  Retrieve the file name from a chooser.				*/
/*									*/
/************************************************************************/

static int appChooserGetFilename(	void *			voidcbs,
					AppChooserInformation *	aci,
					const char *		extension )
    {
    int					rval= 0;
    int					ret;
    XmFileSelectionBoxCallbackStruct *	cbs;

    char *				fnm= (char *)0;

    MemoryBuffer			filename;

    utilInitMemoryBuffer( &filename );

    cbs= (XmFileSelectionBoxCallbackStruct *)voidcbs;

    if  ( ! XmStringGetLtoR( cbs->value, XmFONTLIST_DEFAULT_TAG, &fnm )	||
	  utilMemoryBufferSetString( &filename, fnm )			)
	{
	LDEB(1);
	appGuiBreakDialog( &(aci->aciDialog), ACIrespFAILURE );
	rval= -1; goto ready;
	}

    aci->aciRelativeTo= aci->aciDialog.adTopWidget;
    ret= appChooserSaveFilename( aci, &filename, extension );

    if  ( ret < 0 )
	{
	appGuiBreakDialog( &(aci->aciDialog), ACIrespFAILURE );
	rval= -1; goto ready;
	}
    if  ( ret > 0 )
	{ rval= -1; goto ready;		}

  ready:

    if  ( fnm )
	{ XtFree( fnm );	}

    utilCleanMemoryBuffer( &filename );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Set the directory for a chooser, based on the file name.		*/
/*  Set a file name derived from the previous name and an extension.	*/
/*									*/
/************************************************************************/

static void appChooserSetText(		Widget  		w,
					const MemoryBuffer *	mbText )
    {
    const char *	s= utilMemoryBufferGetString( mbText );

    if  ( XmIsText( w ) )
	{
	XmTextSetString( w, (char *)s );
	XmTextSetInsertionPosition( w, mbText->mbSize );

	return;
	}

    if  ( XmIsTextField( w ) )
	{
	XmTextFieldSetString( w, (char *)s );
	XmTextFieldSetInsertionPosition( w, mbText->mbSize );

	return;
	}

    SDEB( s );
    }

static void appChooserSetDirectory(	const MemoryBuffer *	dir,
					AppChooserInformation *	aci )
    {
    XmString		directoryString= (XmString)0;

    directoryString= XmStringCreateLocalized(
			    (char *)utilMemoryBufferGetString( dir ) );

    XtVaSetValues( aci->aciDialog.adDialog,
				XmNdirectory,		directoryString,
				NULL );

    if  ( directoryString )
	{ XmStringFree( directoryString );	}

    return;
    }

static void appChooserSetFileDirectory(	const MemoryBuffer *	filename,
					AppChooserInformation *	aci )
    {
    MemoryBuffer	directory;

    utilInitMemoryBuffer( &directory );

    if  ( ! appDirectoryOfFileName( &directory, filename )	&&
	  ! utilMemoryBufferIsEmpty( &directory )		)
	{
	appChooserSetDirectory( &directory, aci );
	}

    utilCleanMemoryBuffer( &directory );

    return;
    }

/************************************************************************/
/*									*/
/*  Change the extension of the file name.				*/
/*									*/
/************************************************************************/

static void appChooserSetFileExtension(
				const MemoryBuffer *		filename,
				const char *			extension,
				Widget				text )
    {
    MemoryBuffer	changed;

    utilInitMemoryBuffer( &changed );

    if  ( appFileSetExtension( &changed, extension ) )
	{ LDEB(1); goto ready;	}

    appChooserSetText( text, &changed );

  ready:

    utilCleanMemoryBuffer( &changed );

    return;
    }

/************************************************************************/
/*									*/
/*  Callback from the file chooser to open a file.			*/
/*									*/
/************************************************************************/

static void appOpenOkPushed(	Widget		fileChooser,
				XtPointer	voidaci,
				XtPointer	voidcbs	 )
    {
    AppChooserInformation *	aci= (AppChooserInformation *)voidaci;
    Widget			option= aci->aciOption;
    const char *		extension= (const char *)0;

    if  ( aci->aciExtensionCount > 0 )
	{
	if  ( aci->aciFormat >= 0 && aci->aciFormat < aci->aciExtensionCount )
	    {
	    const AppFileExtension *	afe= aci->aciExtensions+ aci->aciFormat;

	    extension= afe->afeExtension;
	    }
	}

    if  ( appChooserGetFilename( voidcbs, aci, extension ) )
	{ return;	}

    aci->aciRelativeTo= aci->aciDialog.adTopWidget;
    if  ( appFileChooserTestNameForOpen( aci ) )
	{ utilEmptyMemoryBuffer( &(aci->aciFilename) ); return; }

    if  ( (*aci->aciOpenDocument)( aci->aciApplication, aci->aciThrough,
					    aci->aciDialog.adTopWidget, option,
					    &(aci->aciFilename) ) )
	{ utilEmptyMemoryBuffer( &(aci->aciFilename) ); return; }

    utilEmptyMemoryBuffer( &(aci->aciFilename) );

    appGuiBreakDialog( &(aci->aciDialog), ACIrespOPEN );

    return;
    }

/************************************************************************/
/*									*/
/*  The file selection box was told to filter via the normal motif	*/
/*  mechanisms: Adapt the OptionMenu with extensions to the filter.	*/
/*									*/
/************************************************************************/

static void appFileFilterActivated(	Widget		w,
					XtPointer	voidaci,
					XtPointer	e )
    {
    AppChooserInformation *	aci= (AppChooserInformation *)voidaci;

    Widget text= XmFileSelectionBoxGetChild( aci->aciDialog.adDialog,
							XmDIALOG_FILTER_TEXT );

    char *		filter= XmTextGetString( text );

    MemoryBuffer	mbFilter;
    MemoryBuffer	mbExt;

    utilInitMemoryBuffer( &mbFilter );
    utilInitMemoryBuffer( &mbExt );

    if  ( ! filter )
	{ goto ready;	}

    if  ( utilMemoryBufferSetString( &mbFilter, filter ) )
	{ LDEB(1); goto ready;	}
    if  ( appFileGetFileExtension( &mbExt, &mbFilter ) < 0 )
	{ LDEB(1); goto ready;	}

    if  ( ! utilMemoryBufferIsEmpty( &mbExt ) )
	{
	int				i;
	const AppFileExtension *	afe= aci->aciExtensions;

	Widget				current= (Widget)0;
	WidgetList			children;
	Cardinal			childCount= 0;

	XtVaGetValues( aci->aciFilterOptionmenu.aomPulldown,
			XmNmenuHistory,		&current,
			XmNchildren,		&children,
			XmNnumChildren,		&childCount,
			NULL );

	for ( i= 0; i < childCount; i++ )
	    {
	    if  ( children[i] == current )
		{ break;	}
	    }

	if  ( i >= childCount				||
	      i >= aci->aciExtensionCount		||
	      ! utilMemoryBufferEqualsString( &mbExt, afe[i].afeExtension ) )
	    {
	    for ( i= 0; i < aci->aciExtensionCount; afe++, i++ )
		{
		if  ( i >= childCount )
		    { continue;	}
		if  ( ! XtIsManaged( children[i] ) )
		    { continue;	}
		if  ( ! XtIsSensitive( children[i] ) )
		    { continue;	}

		if  ( utilMemoryBufferEqualsString( &mbExt,
						    afe->afeExtension )	)
		    { break;	}
		}

	    if  ( i < aci->aciExtensionCount )
		{ appSetOptionmenu( &(aci->aciFilterOptionmenu), i );	}
	    }
	}

  ready:

    utilCleanMemoryBuffer( &mbFilter );
    utilCleanMemoryBuffer( &mbExt );

    if  ( filter )
	{ XtFree( filter );	}

    return;
    }

/************************************************************************/
/*									*/
/*  Add a pulldown with filters to a file chooser.			*/
/*									*/
/************************************************************************/

static void appFileFilterChosen(	int		formatChosen,
					void *		voidaci )
    {
    AppChooserInformation *	aci= (AppChooserInformation *)voidaci;
    const AppFileExtension *	afe= aci->aciExtensions;
    Widget			chooser= aci->aciDialog.adDialog;
    Widget			text;

    int				previousFormat= aci->aciFormat;

    const char *		filter;
    XmString			filterString;

    MemoryBuffer		fileSelected;
    MemoryBuffer		extension;

    utilInitMemoryBuffer( &fileSelected );
    utilInitMemoryBuffer( &extension );

    text= XmFileSelectionBoxGetChild( chooser, XmDIALOG_TEXT );
    appBufferFromTextWidget( &fileSelected, text );

    filter= afe[formatChosen].afeFilter;
    filterString= XmStringCreateLocalized( (char *)filter );

    XtVaSetValues( chooser,
			XmNpattern,		filterString,
			NULL );

    if  ( ! utilMemoryBufferIsEmpty( &fileSelected ) )
	{
	if  ( ! appFileGetFileExtension( &extension, &fileSelected )	&&
	      ! utilMemoryBufferIsEmpty( &extension )			&&
	      previousFormat >= 0					&&
	      afe[previousFormat].afeExtension				&&
	      afe[formatChosen].afeExtension				&&
	      utilMemoryBufferEqualsString( &extension,
				  afe[previousFormat].afeExtension )	)
	    {
	    const char *	ext= afe[formatChosen].afeExtension;

	    appChooserSetFileExtension( &fileSelected, ext, text );
	    }
	else{ appChooserSetText( text, &fileSelected ); }
	}

    XmStringFree( filterString );

    aci->aciFormat= formatChosen;

    utilCleanMemoryBuffer( &fileSelected );
    utilCleanMemoryBuffer( &extension );

    return;
    }

/************************************************************************/
/*									*/
/*  Insert a filter dropdown in a file chooser.				*/
/*									*/
/************************************************************************/

static void appChooserMakeOptionmenu(	AppChooserInformation *	aci,
					Widget			parent )
    {
    Arg				al[20];
    int				ac= 0;

    AppOptionmenu *		aom= &(aci->aciFilterOptionmenu);

    appInitOptionmenu( aom );

    ac= 0;
    XtSetArg( al[ac], XmNresizeWidth,		False ); ac++;
    XtSetArg( al[ac], XmNtopAttachment,		XmATTACH_FORM ); ac++;
    XtSetArg( al[ac], XmNleftAttachment,	XmATTACH_FORM ); ac++;
    XtSetArg( al[ac], XmNrightAttachment,	XmATTACH_FORM ); ac++;
    XtSetArg( al[ac], XmNbottomAttachment,	XmATTACH_FORM ); ac++;

    aom->aomPulldown= XmCreatePulldownMenu( parent, WIDGET_NAME, al, ac );

    ac= 0;
    XtSetArg( al[ac], XmNsubMenuId,		aom->aomPulldown ); ac++;

    XtSetArg( al[ac], XmNmarginHeight,		0 ); ac++;
    XtSetArg( al[ac], XmNmarginWidth,		0 ); ac++;
    XtSetArg( al[ac], XmNspacing,		0 ); ac++;

    XtSetArg( al[ac], XmNtopAttachment,		XmATTACH_FORM ); ac++;
    XtSetArg( al[ac], XmNleftAttachment,	XmATTACH_FORM ); ac++;
    XtSetArg( al[ac], XmNrightAttachment,	XmATTACH_FORM ); ac++;

    XtSetArg( al[ac], XmNtopOffset,		0 ); ac++;
    XtSetArg( al[ac], XmNleftOffset,		5 ); ac++;
    XtSetArg( al[ac], XmNrightOffset,		5 ); ac++;

    aom->aomInplace= XmCreateOptionMenu( parent, WIDGET_NAME, al, ac );

    appFinishOptionmenuMotif( aom->aomInplace, aom->aomPulldown );

    XtVaSetValues( aom->aomPulldown,
			    XmNresizeWidth,	False,
			    NULL );
    XtVaSetValues( aom->aomInplace,
			    XmNresizeWidth,	False,
			    NULL );

    XtManageChild( aom->aomInplace );

    aom->aomCallback= appFileFilterChosen;
    aom->aomTarget= (void *)aci;

    return;
    }

static void appFileMakeFilter(	AppChooserInformation *	aci )
    {
    Arg				al[20];
    int				ac= 0;

    Widget			child;

    ac= 0;
    XtSetArg( al[ac], XmNresizePolicy,	XmRESIZE_NONE );
    child= XmCreateForm( aci->aciDialog.adDialog, WIDGET_NAME, al, ac );

    appChooserMakeOptionmenu( aci, child );

    XtManageChild( child );

    return;
    }

static void appFileFillFilter(	EditApplication *	ea,
				AppChooserInformation *	aci,
				AppFileExtension *	extensions,
				int			extensionCount,
				const char *		defaultFilter )
    {
    AppFileExtension *		afe;

    int				i;

    int				selected= -1;
    const char *		child0Filter= (char *)0;

    appFileFilterGetDescriptions( ea, extensions, extensionCount );

    appEmptyOptionmenu( &(aci->aciFilterOptionmenu) );

    afe= extensions;
    for ( i= 0; i < extensionCount; afe++, i++ )
	{
	appAddItemToOptionmenu( &(aci->aciFilterOptionmenu),
						    afe->afeDescription );

	if  ( selected < 0 )
	    {
	    if  ( defaultFilter )
		{
		if  ( ! strcmp( afe->afeFilter, defaultFilter ) )
		    {
		    selected= i;
		    child0Filter= afe->afeFilter;
		    }
		}
	    else{
		selected= i;
		child0Filter= afe->afeFilter;
		}
	    }
	}

    if  ( selected >= 0 )
	{
	XmString	filterString= (XmString)0;

	appSetOptionmenu( &(aci->aciFilterOptionmenu), selected );

	if  ( child0Filter )
	    {
	    filterString= XmStringCreateLocalized( (char *)child0Filter );

	    XtVaSetValues( aci->aciDialog.adDialog,
					XmNpattern,	filterString,
					NULL );

	    XmStringFree( filterString );
	    }
	}

    aci->aciExtensions= extensions;
    aci->aciExtensionCount= extensionCount;
    }

/************************************************************************/
/*									*/
/*  Make a chooser.							*/
/*									*/
/************************************************************************/

static int appMakeFileChooser( AppChooserInformation **	pAci,
				Widget				relative,
				EditApplication *		ea,
				int				withFilter,
				const char *			defaultFilter,
				Widget				option )
    {
    Display *			display= XtDisplay( ea->eaToplevel.atTopWidget );
    int				screen= DefaultScreen( display );
    Widget			text;

    Arg				al[20];
    int				ac= 0;

    XmString			filterString= (XmString)0;

    Widget			helpButton;
    Widget			filterButton;
    AppChooserInformation *	aci;

    MwmHints			hints;

    aci= (AppChooserInformation *)malloc( sizeof(AppChooserInformation) );
    if  ( ! aci )
	{ XDEB(aci); return -1;	}

    appInitAci( aci );

    appFileChooserGetTexts( ea, aci );

    hints.flags= MWM_HINTS_FUNCTIONS|MWM_HINTS_DECORATIONS;
    hints.functions=	MWM_FUNC_MOVE		|
			MWM_FUNC_RESIZE		;
    hints.decorations=	MWM_DECOR_BORDER	|
			MWM_DECOR_RESIZEH	|
			MWM_DECOR_TITLE		|
			MWM_DECOR_MENU		;

    ac= 0;
    XtSetArg( al[ac], XmNmwmDecorations,	hints.decorations ); ac++;
    XtSetArg( al[ac], XmNmwmFunctions,		hints.functions ); ac++;
    XtSetArg( al[ac], XmNdeleteResponse,	XmDO_NOTHING ); ac++;
    XtSetArg( al[ac], XmNallowShellResize,	True ); ac++;
    XtSetArg( al[ac], XmNuseAsyncGeometry,	True ); ac++;
    XtSetArg( al[ac], XmNwaitForWm,		False ); ac++; /* LessTif BUG */
    XtSetArg( al[ac], XmNwmTimeout,		0 ); ac++; /* LessTif BUG */

    XtSetArg( al[ac], XmNchildPlacement,XmPLACE_ABOVE_SELECTION ); ac++;
#   if	XmVersion >= 2000 && ! defined( LESSTIF_VERSION )
    XtSetArg( al[ac], XmNfileFilterStyle,
					XmFILTER_HIDDEN_FILES ); ac++;
#   endif

    if  ( defaultFilter )
	{
	filterString= XmStringCreateLocalized( (char *)defaultFilter );
	XtSetArg( al[ac], XmNpattern, filterString ); ac++;
	}

    aci->aciDialog.adDialog= XmCreateFileSelectionDialog(
						ea->eaToplevel.atTopWidget,
						WIDGET_NAME, al, ac );
    aci->aciDialog.adTopWidget= XtParent( aci->aciDialog.adDialog );

    if  ( filterString )
	{ XmStringFree( filterString );	}

    helpButton= XmFileSelectionBoxGetChild( aci->aciDialog.adDialog,
						XmDIALOG_HELP_BUTTON );
    XtUnmanageChild( helpButton );

    text= XmFileSelectionBoxGetChild( aci->aciDialog.adDialog,
							XmDIALOG_FILTER_TEXT );
    XtVaSetValues( text,
			XmNbackground,	WhitePixel( display, screen ),
			XmNforeground,	BlackPixel( display, screen ),
			NULL );

    /*  Not necessary
    if  ( withFilter )
	{
	XtAddCallback( text, XmNactivateCallback,
				    appFileFilterActivated, (void *)aci );
	}
    */

    filterButton= XmFileSelectionBoxGetChild( aci->aciDialog.adDialog,
						    XmDIALOG_APPLY_BUTTON );

    if  ( withFilter )
	{
	XtAddCallback( filterButton, XmNactivateCallback,
				    appFileFilterActivated, (void *)aci );
	}

    text= XmFileSelectionBoxGetChild( aci->aciDialog.adDialog, XmDIALOG_TEXT );
    XtVaSetValues( text,
			XmNbackground,	WhitePixel( display, screen ),
			XmNforeground,	BlackPixel( display, screen ),
			NULL );

    appSetShellTitle( aci->aciDialog.adTopWidget, option,
						    ea->eaApplicationName );

    XtAddEventHandler( aci->aciDialog.adTopWidget, StructureNotifyMask, False,
					appSetSizeAsMinimum, (void *)0 );

    XtAddCallback( aci->aciDialog.adDialog,
		    XmNcancelCallback, appChooserCancel, (XtPointer)aci );

    XtAddCallback( aci->aciDialog.adDialog,
		    XmNdestroyCallback, appFreeAci, (void *)aci );

    aci->aciApplication= ea;

    *pAci= aci;
    return 0;
    }

/************************************************************************/
/*									*/
/*  Run some kind of a file chooser.					*/
/*									*/
/*  1)  Cope with Motif's silly behavior.				*/
/*									*/
/************************************************************************/

static void appRunFileChooser(	EditApplication *		ea,
				AppChooserInformation *		aci,
				Widget				relative )
    {
    utilInitMemoryBuffer( &(aci->aciFilename) );

    appGuiShowDialog( ea, &(aci->aciDialog), relative );

    /*  1  */
    if  ( aci->aciFilterOptionmenu.aomInplace )
	{
	XtVaSetValues( XtParent( aci->aciFilterOptionmenu.aomInplace ),
				XmNresizePolicy,	XmRESIZE_NONE,
				NULL );
	}

    appGuiRunDialog( &(aci->aciDialog), ACIrespNONE, ea );

    appGuiHideDialog( &(aci->aciDialog) );

    return;
    }

/************************************************************************/
/*									*/
/*  Open a document.							*/
/*									*/
/************************************************************************/

void appRunOpenChooser( Widget				option,
			Widget				relative,
			int				extensionCount,
			AppFileExtension *		extensions,
			const char *			defaultFilter,
			const MemoryBuffer *		dir,
			APP_OPEN_DOCUMENT		openDocument,
			EditApplication *		ea,
			void *				through )
    {
    AppChooserInformation *	aci= (AppChooserInformation *)0;
    const int			withFilter= extensionCount > 0;

    if  ( appMakeFileChooser( &aci, relative, ea,
					withFilter, defaultFilter, option ) )
	{ LDEB(1); return;	}

    XtAddCallback( aci->aciDialog.adDialog,
			XmNokCallback, appOpenOkPushed, (XtPointer)aci );

    if  ( withFilter )
	{
	appFileMakeFilter( aci );

	appGuiShowDialog( ea, &(aci->aciDialog), relative );

	appFileFillFilter( ea, aci,
				extensions, extensionCount, defaultFilter );
	}

    aci->aciOption= option;
    aci->aciRelativeTo= relative;
    aci->aciThrough= through;
    aci->aciOpenDocument= openDocument;
    utilInitMemoryBuffer( &(aci->aciFilename) );

    if  ( withFilter )
	{
	WidgetList		children;
	Cardinal		childCount= 0;
	AppFileExtension *	afe;

	unsigned		i;

	XtVaGetValues( aci->aciFilterOptionmenu.aomPulldown,
			    XmNchildren,		&children,
			    XmNnumChildren,		&childCount,
			    NULL );

	afe= extensions;
	for ( i= 0; i < childCount; afe++, i++ )
	    {
	    if  ( ! FILE_CHOOSER_CAN_OPEN( afe ) )
		{ XtUnmanageChild( children[i] ); continue;	}

	    XtManageChild( children[i] );
	    }
	}

    if  ( dir && ! utilMemoryBufferIsEmpty( dir ) )
	{ appChooserSetDirectory( dir, aci );	}

    appRunFileChooser( ea, aci, relative );

    XtDestroyWidget( aci->aciDialog.adTopWidget );

    return;
    }

APP_MENU_CALLBACK_H( appAppFileOpen, option, voidea, e )
    {
    EditApplication *	ea= (EditApplication *)voidea;

    appRunOpenChooser( option, ea->eaToplevel.atTopWidget,
			ea->eaFileExtensionCount, ea->eaFileExtensions,
			ea->eaDefaultFileFilter, (MemoryBuffer *)0,
			appChooserOpenDocument, ea, (void *)ea );
    }

void appDocFileOpen(	APP_WIDGET	option,
			void *		voided,
			void *		e )
    {
    EditDocument *	ed= (EditDocument *)voided;
    EditApplication *	ea= ed->edApplication;

    appRunOpenChooser( option, ed->edToplevel.atTopWidget,
			ea->eaFileExtensionCount, ea->eaFileExtensions,
			ea->eaDefaultFileFilter, (MemoryBuffer *)0,
			appChooserOpenDocument, ea, (void *)ea );
    }

static void appFileOkPushed(		AppChooserInformation *	aci,
					XtPointer		voidcbs,
					const char *		extension )
    {
    int			response;

    if  ( appChooserGetFilename( voidcbs, aci, extension ) )
	{ return;	}

    aci->aciRelativeTo= aci->aciDialog.adTopWidget;
    response= appFileChooserTestNameForWrite( aci );

    switch( response )
	{
	case ACIrespSAVE:
	    if  ( (*aci->aciSaveDocument)( aci->aciDocument, aci->aciThrough, 
			    aci->aciDialog.adTopWidget, aci->aciOption,
			    aci->aciFormat, &(aci->aciFilename) ) )
		{
		utilEmptyMemoryBuffer( &(aci->aciFilename) );
		}
	    else{
		appGuiBreakDialog( &(aci->aciDialog), ACIrespSAVE );
		}
	    return;

	case ACIrespCANCEL:
	    appGuiBreakDialog( &(aci->aciDialog), ACIrespCANCEL );
	    return;

	case ACIrespNONE:
	    utilEmptyMemoryBuffer( &(aci->aciFilename) );
	    return;

	default:
	    LDEB(response);
	    utilEmptyMemoryBuffer( &(aci->aciFilename) );
	    appGuiBreakDialog( &(aci->aciDialog), ACIrespFAILURE );
	    return;
	}

    return;
    }

static void appSaveOkPushed(		Widget		fileChooser,
					XtPointer	voidaci,
					XtPointer	voidcbs	 )
    {
    AppChooserInformation *	aci= (AppChooserInformation *)voidaci;
    const char *		extension= (const char *)0;

    if  ( aci->aciExtensionCount > 0 )
	{
	if  ( aci->aciFormat >= 0 && aci->aciFormat < aci->aciExtensionCount )
	    {
	    const AppFileExtension *	afe= aci->aciExtensions+ aci->aciFormat;

	    extension= afe->afeExtension;
	    }
	}

    appFileOkPushed( aci, voidcbs, extension );
    }

static void appPrintOkPushed(		Widget		fileChooser,
					XtPointer	voidaci,
					XtPointer	voidcbs	 )
    {
    AppChooserInformation *	aci= (AppChooserInformation *)voidaci;
    const char *		extension= "ps";

    appFileOkPushed( aci, voidcbs, extension );
    }

/************************************************************************/
/*									*/
/*  Run Filechooser for save.						*/
/*									*/
/************************************************************************/

static int appSaveChooserFilterExtensions( EditApplication *	ea,
					const EditDocument *	ed,
					unsigned int		useFlags,
					AppChooserInformation *	aci )
    {
    WidgetList		children;
    Cardinal		childCount= 0;
    AppFileExtension *	afe;

    int			choice= -1;
    int			i;

    XtVaGetValues( aci->aciFilterOptionmenu.aomPulldown,
			XmNchildren,		&children,
			XmNnumChildren,		&childCount,
			NULL );

    afe= ea->eaFileExtensions;
    for ( i= 0; i < (int)childCount; afe++, i++ )
	{
	if  ( appDocumentTestCanSave( ea, afe, ed->edPrivateData,
							useFlags, i ) )
	    { XtUnmanageChild( children[i] ); continue;	}

	XtManageChild( children[i] );

	if  ( i == ed->edFormat || choice < 0 )
	    { choice= i;	}
	}

    if  ( choice >= 0 )
	{
	XmString	filterString;

	filterString= XmStringCreateLocalized(
			    (char *)ea->eaFileExtensions[choice].afeFilter );

	appSetOptionmenu( &(aci->aciFilterOptionmenu), choice );

	XtVaSetValues( aci->aciDialog.adDialog,
			XmNpattern,			filterString,
			NULL );

	XmStringFree( filterString );

	if  ( ! utilMemoryBufferIsEmpty( &(ed->edFilename) ) )
	    {
	    Widget	text;

	    text= XmFileSelectionBoxGetChild( aci->aciDialog.adDialog,
							    XmDIALOG_TEXT );

	    if  ( choice == ed->edFormat )
		{ appChooserSetText( text, &(ed->edFilename) ); }
	    else{
		const char *	ext= ea->eaFileExtensions[choice].afeExtension;

		if  ( ext )
		    {
		    appChooserSetFileExtension( &(ed->edFilename),
							    ext, text );
		    }
		}
	    }

	aci->aciFormat= choice;
	}

    return 0;
    }

int appRunSaveChooser(	APP_WIDGET		option,
			APP_WIDGET		relative,
			unsigned int		useFlags,
			APP_SAVE_DOCUMENT	saveDocument,
			EditDocument *		ed,
			void *			through )
    {
    EditApplication *		ea= ed->edApplication;
    AppChooserInformation *	aci= (AppChooserInformation *)0;
    const int			withFilter= ea->eaFileExtensionCount > 0;
    int				response;

    if  ( appMakeFileChooser( &aci, relative, ea, withFilter,
					    ea->eaDefaultFileFilter, option ) )
	{ return ACIrespFAILURE;	}

    XtAddCallback( aci->aciDialog.adDialog,
			    XmNokCallback, appSaveOkPushed, (XtPointer)aci );

    if  ( withFilter )
	{
	appFileMakeFilter( aci );

	appGuiShowDialog( ea, &(aci->aciDialog), relative );

	appFileFillFilter( ea, aci,
		ea->eaFileExtensions,
		ea->eaFileExtensionCount,
		ea->eaDefaultFileFilter );
	}

    aci->aciOption= option;
    aci->aciRelativeTo= relative;
    aci->aciDocument= ed;
    aci->aciThrough= through;
    aci->aciSaveDocument= saveDocument;

    if  ( ! utilMemoryBufferIsEmpty( &(ed->edFilename) ) )
	{ appChooserSetFileDirectory( &(ed->edFilename), aci ); }

    if  ( ea->eaFileExtensionCount > 0					&&
	  appSaveChooserFilterExtensions( ea, ed, useFlags, aci )	)
	{ LDEB(1); return ACIrespFAILURE;	}

    appRunFileChooser( ea, aci, relative );

    response= aci->aciDialog.adResponse;

    XtDestroyWidget( aci->aciDialog.adTopWidget );

    return response;
    }

/************************************************************************/
/*									*/
/*  Run Filechooser for print to file.					*/
/*									*/
/************************************************************************/

int appRunPrintToFileChooser(
			APP_WIDGET		option,
			APP_WIDGET		relative,
			APP_SAVE_DOCUMENT	printDocument,
			EditDocument *		ed,
			void *			through )
    {
    EditApplication *		ea= ed->edApplication;
    AppChooserInformation *	aci= (AppChooserInformation *)0;
    const int			withFilter= 0;
    int				response;

    if  ( appMakeFileChooser( &aci, relative, ea, withFilter, "*.ps", option ) )
	{ LDEB(1); return ACIrespFAILURE;	}

    XtAddCallback( aci->aciDialog.adDialog,
				XmNokCallback, appPrintOkPushed, (void *)aci );

    aci->aciOption= option;
    aci->aciRelativeTo= relative;
    aci->aciDocument= ed;
    aci->aciThrough= through;
    aci->aciSaveDocument= printDocument;

    if  ( ! utilMemoryBufferIsEmpty( &(ed->edFilename) ) )
	{
	Widget	text;

	text= XmFileSelectionBoxGetChild(
				    aci->aciDialog.adDialog, XmDIALOG_TEXT );

	appChooserSetFileDirectory( &(ed->edFilename), aci );
	appChooserSetFileExtension( &(ed->edFilename), "ps", text );
	}

    appRunFileChooser( ea, aci, relative );

    response= aci->aciDialog.adResponse;

    XtDestroyWidget( aci->aciDialog.adTopWidget );

    return response;
    }

int appFileChooserConfirmOverWrite(	const AppChooserInformation *	aci,
					const char *		filename )
    {
    int			rcc;

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
