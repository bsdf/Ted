/************************************************************************/
/*									*/
/*  Application, Main module.						*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<string.h>
#   include	<locale.h>
#   include	<signal.h>

#   include	<appSystem.h>

#   include	"appFrame.h"
#   include	"appQuestion.h"
#   include	"appFileChooser.h"
#   include	<appUnit.h>
#   include	<appPaper.h>
#   include	<geoString.h>
#   include	<utilArgToX.h>
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Retrieve configurable resource values from the GUI environment.	*/
/*									*/
/*  1)  This should be done wih something like the GNU message catalog	*/
/*	system. For the moment, just install the default values.	*/
/*									*/
/************************************************************************/

void appGuiGetResourceValues(	int *				pGotResources,
				EditApplication *		ea,
				void *				pValues,
				AppConfigurableResource *	acrList,
				int				acrCount )
    {
    AppConfigurableResource *	acr;
    char *			values= (char *)pValues;
    int				i;

    if  ( *pGotResources )
	{ LDEB(*pGotResources); return;	}

    if  ( ! *pGotResources )
	{ appSetResourceDefaults( ea, acrList, acrCount );	}

    acr= acrList;
    for ( i= 0; i < acrCount; acr++, i++ )
	{
	*((const char **)(values+acr->acrStructOffset))= acr->acrDefaultValue;
	}

    *pGotResources= 1;
    return;
    }

/************************************************************************/
/*									*/
/*  Callback that is invoked when the application is closed through	*/
/*  the window manager.							*/
/*									*/
/************************************************************************/

APP_CLOSE_CALLBACK_H( appAppWmClose, w, voidea )
    {
    EditApplication *		ea= (EditApplication *)voidea;

    if  ( ea->eaMainVisibleAsAbout )
	{
	ea->eaMainVisibleAsAbout= 0;

	if  ( ea->eaVisibleDocumentCount > 0 )
	    { appHideShellWidget( ea->eaToplevel.atTopWidget ); }
	}
    else{
	appQuitApplication( (APP_WIDGET)0,
				    ea->eaToplevel.atTopWidget, ea );
	}
    }

/************************************************************************/
/*									*/
/*  Quit the application.						*/
/*									*/
/*  1)  Close the documents in reverse order to avoid the subscript of	*/
/*	unhandeled documents to change.					*/
/*									*/
/************************************************************************/

static int appCountChangedDocuments(	EditApplication *	ea )
    {
    int		i;
    int		changedCount= 0;

    for ( i= 0; i < ea->eaOpenCount; i++ )
	{
	EditDocument *	ed= ea->eaOpenDocuments[i];

	if  ( ed->edHasBeenChanged )
	    { changedCount++;	}
	}

    return changedCount;
    }

static void appExitApplication(	EditApplication *	ea )
    {
    int		i;

    for ( i= ea->eaOpenCount- 1; i >= 0; i-- )
	{ appCloseDocument( ea->eaOpenDocuments[i] );	}

    appDiscardImagePixmaps( ea );

    appDestroyShellWidget( ea->eaToplevel.atTopWidget );

    exit( 0 ); LDEB(1); return;
    }

static void appAskCloseDocuments(	APP_WIDGET		option,
					EditApplication *	ea )
    {
    int		i;

    /*  1  */
    for ( i= ea->eaOpenCount- 1; i >= 0; i-- )
	{
	EditDocument *	ed= ea->eaOpenDocuments[i];

	if  ( ed->edHasBeenChanged )
	    {
	    appMakeDocVisible( ea, ed );
	    appRunReallyCloseDialog( option, ed );
	    }
	}

    return;
    }

void appQuitApplication(	APP_WIDGET		option,
				APP_WIDGET		relative,
				EditApplication *	ea )
    {
    int				rcc;
    int				changedCount= 0;

    changedCount= appCountChangedDocuments( ea );
    if  ( changedCount == 0 )
	{ appExitApplication( ea ); LDEB(1); return;	}

    if  ( changedCount > 1 )
	{
	rcc= appRunReallyQuitDialog( option, relative, ea );

	switch( rcc )
	    {
	    case AQDrespCANCEL:
	    case AQDrespCLOSED:
		return;
	    case AQDrespNO:
		appExitApplication( ea );
		LDEB(1); return;

	    default:
		LDEB(rcc); return;
	    case AQDrespYES:
		break;
	    }
	}

    appAskCloseDocuments( option, ea );

    changedCount= appCountChangedDocuments( ea );
    if  ( changedCount == 0 )
	{ appExitApplication( ea ); LDEB(1); return;	}

    return;
    }

APP_MENU_CALLBACK_H( appAppFileQuit, option, voidea, e )
    {
    EditApplication *	ea= (EditApplication *)voidea;

    appQuitApplication( option, ea->eaToplevel.atTopWidget, ea );
    }

/************************************************************************/
/*									*/
/*  Open a new document.						*/
/*									*/
/************************************************************************/

APP_MENU_CALLBACK_H( appAppFileNew, option, voidea, e )
    {
    EditApplication *		ea= (EditApplication *)voidea;
    const MemoryBuffer *	filename= (const MemoryBuffer *)0;

    if  ( appNewDocument( ea, filename ) )
	{ LDEB(1); }

    return;
    }

/************************************************************************/
/*									*/
/*  Make a document the current Document.				*/
/*									*/
/************************************************************************/

static void appHighlightCurrentDocument( EditApplication *	ea,
					EditDocument *		ed,
					int			current )
    {
    guiEnableWidget( ed->edMenuBar, current != 0 );
    return;
    }

void appSetCurrentDocument(	EditApplication *	ea,
				EditDocument *		ed	)
    {
    if  ( ea->eaCurrentDocument )
	{ appHighlightCurrentDocument( ea, ea->eaCurrentDocument, 0 ); }

    ea->eaCurrentDocument= ed;

    appHighlightCurrentDocument( ea, ea->eaCurrentDocument, 1 );
    }

/************************************************************************/
/*									*/
/*  Remember whether a document has been changed.			*/
/*									*/
/************************************************************************/

void appDocumentChanged(	EditDocument *		ed,
				int			changed	)
    {
    EditApplication *	ea= ed->edApplication;

    changed= changed != 0;

    if  ( ed->edHasBeenChanged != changed )
	{
	int		i;

	for ( i= 0; i < ea->eaOpenCount; i++ )
	    {
	    if  ( ea->eaOpenDocuments[i] == ed )
		{ continue;	}

	    appSetWindowsItemState( ea->eaOpenDocuments[i]->edWindowMenu,
								ed, changed );
	    }

	appSetWindowsItemState( ea->eaWinMenu, ed, changed );

	ed->edHasBeenChanged= changed;
	}
    }

/************************************************************************/
/*									*/
/*  Count the number of visible documents.				*/
/*									*/
/************************************************************************/

void appDocVisible(	EditApplication *	ea,
			EditDocument *		ed,
			int			visible	)
    {
    int	from;

    if  ( visible )
	{ from= ea->eaVisibleDocumentCount++;	}
    else{ from= ea->eaVisibleDocumentCount--;	}

    if  ( ea->eaVisibleDocumentCount == 0 )
	{ appShowShellWidget( ea, ea->eaToplevel.atTopWidget ); }
    else{
	if  ( ! ea->eaMainVisibleAsAbout )
	    { appHideShellWidget( ea->eaToplevel.atTopWidget );	}
	}

    if  ( ea->eaVisibleDocumentCountChanged )
	{
	(*ea->eaVisibleDocumentCountChanged)
				    ( ea, from, ea->eaVisibleDocumentCount );
	}
    }

/************************************************************************/
/*									*/
/*  Remove a reference to a document.					*/
/*									*/
/************************************************************************/

void appRemoveDocument(	EditApplication *	ea,
			EditDocument *		ed )
    {
    int		i;
    int		count= 0;

    for ( i= 0; i < ea->eaOpenCount; i++ )
	{
	if  ( ea->eaOpenDocuments[i] == ed )
	    { continue;	}

	appRemoveWindowsOption( ea->eaOpenDocuments[i]->edWindowMenu, ed );
	ea->eaOpenDocuments[count++]= ea->eaOpenDocuments[i];
	}

    appRemoveWindowsOption( ea->eaWinMenu, ed );

    ea->eaOpenCount= count;

    if  ( ea->eaCurrentDocument == ed )
	{ ea->eaCurrentDocument= (EditDocument *)0;	}

    if  ( ed->edIsVisible )
	{
	ed->edIsVisible= 0;
	appDocVisible( ea, ed, ed->edIsVisible );
	}

    return;
    }

void appRenameDocumentOptions(	EditApplication *	ea,
				EditDocument *		ed,
				const MemoryBuffer *	title )
    {
    int		i;

    for ( i= 0; i < ea->eaOpenCount; i++ )
	{
	if  ( ea->eaOpenDocuments[i] == ed )
	    { continue;	}

	appRenameWindowsOption( ea->eaOpenDocuments[i]->edWindowMenu,
								ed, title );
	}

    appRenameWindowsOption( ea->eaWinMenu, ed, title );

    return;
    }

/************************************************************************/
/*									*/
/*  Remember a reference to a document.					*/
/*									*/
/************************************************************************/

static void appAppSetWindowsOption(	APP_WIDGET		menu,
					EditDocument *		ed,
					const MemoryBuffer *	title )
    {
    AppMenuItem		ami;

    if  ( ed->edHasBeenChanged )
	{ ami.amiItemType= ITEMtyTOGGLE_ON;	}
    else{ ami.amiItemType= ITEMtyTOGGLE_OFF;	}

    ami.amiItemText= utilMemoryBufferGetString( title );
    ami.amiKey= (char *)0;
    ami.amiKeyText= (char *)0;
    ami.amiCallback= (APP_MENU_CALLBACK_T)appDocToFront;

    appSetToggleMenuItem( menu, &(ed->edToplevel), &ami, (void *)ed );
    }

void appSetDocument(	EditApplication *	ea,
			EditDocument *		newEd )
    {
    EditDocument **	fresh;
    int			i;

    fresh= (EditDocument **)realloc( ea->eaOpenDocuments,
			( ea->eaOpenCount+ 1 )* sizeof(EditDocument *) );
    if  ( ! fresh )
	{ XDEB(fresh); return;	}

    fresh[ea->eaOpenCount]= newEd;
    ea->eaOpenDocuments= fresh;

    for ( i= 0; i < ea->eaOpenCount; i++ )
	{
	appAppSetWindowsOption( ea->eaOpenDocuments[i]->edWindowMenu,
					    newEd, &(newEd->edTitle) );

	appAppSetWindowsOption( newEd->edWindowMenu,
					ea->eaOpenDocuments[i],
					&(ea->eaOpenDocuments[i]->edTitle) );
	}

    appAppSetWindowsOption( ea->eaWinMenu, newEd, &(newEd->edTitle) );

    ea->eaOpenCount++;

    return;
    }

/************************************************************************/

int appDetermineBoolean(	int *			pIval,
				const char *		sVal )
    {
    if  ( sVal		&&
	  *pIval == 0	)
	{
	if  ( ! strcmp( sVal, "0" ) )
	    { *pIval= -1;	}
	if  ( ! strcmp( sVal, "1" ) )
	    { *pIval=  1;	}

	if  ( *pIval == 0 )
	    { SDEB(sVal);	}
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Generic editor type application main().				*/
/*									*/
/*  1)  Make an application shell.					*/
/*  1b) Obtain resource values.						*/
/*  2)  Remember X11 properties.					*/
/*  3)  Remember Window manager properties.				*/
/*									*/
/************************************************************************/

static AppConfigurableResource	APP_ApplicationResourceTable[]=
{
    APP_RESOURCE( "unit",
		offsetof(EditApplication,eaUnitString),
		"inch" ),
    APP_RESOURCE( "paper",
		offsetof(EditApplication,eaPaperString),
		"letter" ),
    APP_RESOURCE( "leftMargin",
		offsetof(EditApplication,eaLeftMarginString),
		"90pt" ),
    APP_RESOURCE( "rightMargin",
		offsetof(EditApplication,eaRightMarginString),
		"90pt" ),
    APP_RESOURCE( "topMargin",
		offsetof(EditApplication,eaTopMarginString),
		"72pt" ),
    APP_RESOURCE( "bottomMargin",
		offsetof(EditApplication,eaBottomMarginString),
		"72pt" ),
    APP_RESOURCE( "magnification",
		offsetof(EditApplication,eaMagnificationString),
		"100%" ),

    APP_RESOURCE( "usePostScriptFilters",
		offsetof(EditApplication,eaUsePostScriptFiltersString),
		"1" ),
    APP_RESOURCE( "usePostScriptIndexedImages",
		offsetof(EditApplication,eaUsePostScriptIndexedImagesString),
		"1" ),
    APP_RESOURCE( "sevenBitsPostScript",
		offsetof(EditApplication,ea7BitsPostScriptString),
		"0" ),

    APP_RESOURCE( "skipEmptyPages",
		offsetof(EditApplication,eaSkipEmptyPagesString),
		"0" ),
    APP_RESOURCE( "skipBlankPages",
		offsetof(EditApplication,eaSkipBlankPagesString),
		"0" ),
    APP_RESOURCE( "omitHeadersOnEmptyPages",
		offsetof(EditApplication,eaOmitHeadersOnEmptyPagesString),
		"0" ),
    APP_RESOURCE( "customPsSetupFilename",
		offsetof(EditApplication,eaCustomPsSetupFilename),
		(char *)0 ),

    APP_RESOURCE( "avoidFontconfig",
		offsetof(EditApplication,eaAvoidFontconfigString),
		"0" ),
    APP_RESOURCE( "preferBase35Fonts",
		offsetof(EditApplication,eaPreferBase35FontsString),
		"0" ),
    APP_RESOURCE( "embedFonts",
		offsetof(EditApplication,eaEmbedFontsString),
		"1" ),
    APP_RESOURCE( "useKerning",
		offsetof(EditApplication,eaUseKerningString),
		"1" ),
    APP_RESOURCE( "styleTool",
		offsetof(EditApplication,eaStyleToolString),
		"1" ),

    APP_RESOURCE( "leftRulerWidthMM",
		offsetof(EditApplication,eaLeftRulerWidthMMString),
		(char *)0 ),
    APP_RESOURCE( "topRulerHeightMM",
		offsetof(EditApplication,eaTopRulerHeightMMString),
		(char *)0 ),
    APP_RESOURCE( "rightRulerWidthMM",
		offsetof(EditApplication,eaRightRulerWidthMMString),
		(char *)0 ),
    APP_RESOURCE( "bottomRulerHeightMM",
		offsetof(EditApplication,eaBottomRulerHeightMMString),
		(char *)0 ),

    APP_RESOURCE( "author",
		offsetof(EditApplication,eaAuthor),
		(char *)0 ),

    APP_RESOURCE( "afmDirectory",
		offsetof(EditApplication,eaAfmDirectory),
		AFMDIR ),
    APP_RESOURCE( "fontDirectory",
		offsetof(EditApplication,eaFontDirectory),
		(char *)0 ),
    APP_RESOURCE( "ghostscriptFontmap",
		offsetof(EditApplication,eaGhostscriptFontmap),
		(char *)0 ),
    APP_RESOURCE( "ghostscriptFontToXmapping",
		offsetof(EditApplication,eaGhostscriptFontToXmapping),
		(char *)0 ),
    APP_RESOURCE( "customPrintCommand",
		offsetof(EditApplication,eaCustomPrintCommand),
		(char *)0 ),
    APP_RESOURCE( "customPrinterName",
		offsetof(EditApplication,eaCustomPrinterName),
		(char *)0 ),
    APP_RESOURCE( "customPrintCommand2",
		offsetof(EditApplication,eaCustomPrintCommand2),
		(char *)0 ),
    APP_RESOURCE( "customPrinterName2",
		offsetof(EditApplication,eaCustomPrinterName2),
		(char *)0 ),
};

/************************************************************************/
/*									*/
/*  Initialize geometry related settings for the application.		*/
/*									*/
/************************************************************************/

static void appDetermineGeometrySettings(	EditApplication *	ea )
    {
    int				paperFormat;

    ea->eaUnitInt= appUnitTypeInt( ea->eaUnitString );
    if  ( ea->eaUnitInt < 0 )
	{ SDEB(ea->eaUnitString); ea->eaUnitInt= UNITtyINCH; }

    ea->eaMagnification= 1.2;
    if  ( ea->eaMagnificationString )
	{
	double	mag= 0.0;
	char	c;
	int	res;

	res= sscanf( ea->eaMagnificationString, "%lg%c", &mag, &c );
	if  ( res == 1 )
	    { ea->eaMagnification= mag;		}
	if  ( res == 2 && c == '%' )
	    { ea->eaMagnification= mag/100.0;	}

	if  ( mag < 0.1				||
	      ea->eaMagnification < 0.1		||
	      ea->eaMagnification > 10.0	)
	    {
	    SFDEB(ea->eaMagnificationString,mag);
	    ea->eaMagnification= 1.2;
	    }
	}

    if  ( utilPaperSizeFromString( &paperFormat,
			    &(ea->eaDefaultDocumentGeometry.dgPageWideTwips),
			    &(ea->eaDefaultDocumentGeometry.dgPageHighTwips),
			    ea->eaUnitInt, ea->eaPaperString ) < 0 )
	{ SDEB(ea->eaPaperString);	}

    if  ( ea->eaLeftMarginString )
	{
	if  ( geoLengthFromString( ea->eaLeftMarginString, ea->eaUnitInt,
			&(ea->eaDefaultDocumentGeometry.dgLeftMarginTwips) ) )
	    { SDEB(ea->eaLeftMarginString);	}
	}

    if  ( ea->eaRightMarginString )
	{
	if  ( geoLengthFromString( ea->eaRightMarginString, ea->eaUnitInt,
			&(ea->eaDefaultDocumentGeometry.dgRightMarginTwips) ) )
	    { SDEB(ea->eaRightMarginString);	}
	}

    if  ( ea->eaTopMarginString )
	{
	if  ( geoLengthFromString( ea->eaTopMarginString, ea->eaUnitInt,
			&(ea->eaDefaultDocumentGeometry.dgTopMarginTwips) ) )
	    { SDEB(ea->eaTopMarginString);	}
	}

    if  ( ea->eaBottomMarginString )
	{
	if  ( geoLengthFromString( ea->eaBottomMarginString, ea->eaUnitInt,
			&(ea->eaDefaultDocumentGeometry.dgBottomMarginTwips) ) )
	    { SDEB(ea->eaBottomMarginString);	}
	}

    if  ( ea->eaLeftRulerWidthMMString )
	{
	utilArgToInt( &(ea->eaLeftRulerWidthMM),
					ea->eaLeftRulerWidthMMString );
	}
    if  ( ea->eaRightRulerWidthMMString )
	{
	utilArgToInt( &(ea->eaRightRulerWidthMM),
					ea->eaRightRulerWidthMMString );
	}
    if  ( ea->eaTopRulerHeightMMString )
	{
	utilArgToInt( &(ea->eaTopRulerHeightMM),
					ea->eaTopRulerHeightMMString );
	}
    if  ( ea->eaBottomRulerHeightMMString )
	{
	utilArgToInt( &(ea->eaBottomRulerHeightMM),
					ea->eaBottomRulerHeightMMString );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Resolve application resources.					*/
/*									*/
/*  9)  Theoretically a program could have more than one application	*/
/*	object. This has never beem tested. The use of a single		*/
/*	table and different flags to check for reuse here is one of the	*/
/*	things to fix.							*/
/*									*/
/************************************************************************/

void appGetApplicationResourceValues(	EditApplication *	ea )
    {
    /*  b  */
    if  ( ! ea->eaGotResourceTable )
	{
	appGuiGetResourceValues( &(ea->eaGotResourceTable), ea,
					    ea->eaResourceData,
					    ea->eaResourceTable,
					    ea->eaResourceCount );
	}

    if  ( ! ea->eaGotFileMessageResourceTable )
	{
	appGuiGetResourceValues( &(ea->eaGotFileMessageResourceTable), ea,
					&(ea->eaFileMessageResources),
					ea->eaFileMessageResourceTable,
					ea->eaFileMessageResourceCount );
	}

    /*  9  */
    if  ( ! ea->eaGotApplicationResources )
	{
	appGuiGetResourceValues( &(ea->eaGotApplicationResources), ea,
				(void *)ea,
				APP_ApplicationResourceTable,
				sizeof(APP_ApplicationResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    appDetermineBoolean( &(ea->eaUsePostScriptFiltersInt),
				    ea->eaUsePostScriptFiltersString );
    appDetermineBoolean( &(ea->eaUsePostScriptIndexedImagesInt),
				    ea->eaUsePostScriptIndexedImagesString );
    appDetermineBoolean( &(ea->ea7BitsPostScriptInt),
				    ea->ea7BitsPostScriptString );
    appDetermineBoolean( &(ea->eaSkipEmptyPagesInt),
				    ea->eaSkipEmptyPagesString );
    appDetermineBoolean( &(ea->eaSkipBlankPagesInt),
				    ea->eaSkipBlankPagesString );
    appDetermineBoolean( &(ea->eaOmitHeadersOnEmptyPagesInt),
				    ea->eaOmitHeadersOnEmptyPagesString );
    appDetermineBoolean( &(ea->eaAvoidFontconfigInt),
				    ea->eaAvoidFontconfigString );
    appDetermineBoolean( &(ea->eaPreferBase35FontsInt),
				    ea->eaPreferBase35FontsString );
    appDetermineBoolean( &(ea->eaEmbedFontsInt),
				    ea->eaEmbedFontsString );
    appDetermineBoolean( &(ea->eaUseKerningInt),
				    ea->eaUseKerningString );
    appDetermineBoolean( &(ea->eaStyleToolInt),
				    ea->eaStyleToolString );

    appDetermineGeometrySettings( ea );

    return;
    }

/************************************************************************/
/*									*/
/*  Fill the application main widget.					*/
/*									*/
/************************************************************************/

static int appFinishApplicationWindow(	EditApplication *	ea )
    {
    APP_BITMAP_IMAGE		pixmap= (APP_BITMAP_IMAGE)0;
    APP_BITMAP_MASK		mask= (APP_BITMAP_MASK)0;

    APP_WIDGET			pictureLabel= (APP_WIDGET)0;
    APP_WIDGET			label;

    appGuiInsertColumnInWindow( &(ea->eaMainWindow),
					    ea->eaToplevel.atTopWidget );

    appGuiInsertMenubarInColumn( &(ea->eaMenuBar), ea->eaMainWindow );

    ea->eaFileMenu= appMakeMenu( &(ea->eaFileMenuButton),
			&(ea->eaToplevel), ea, ea->eaMenuBar,
			*(ea->eaAppFileMenuText), 0,
			ea->eaAppFileMenuItems, ea->eaAppFileMenuItemCount,
			(void *)ea );

    ea->eaWinMenu= appMakeMenu( &(ea->eaWinMenuButton),
			&(ea->eaToplevel), ea, ea->eaMenuBar,
			*(ea->eaAppWinMenuText), 0,
			ea->eaAppWinMenuItems, ea->eaAppWinMenuItemCount,
			(void *)ea );

    if  ( ea->eaMakePrivateApplicationMenus )
	{ (*ea->eaMakePrivateApplicationMenus)( ea, ea->eaMenuBar ); }

    if  ( ea->eaAppHelpMenuItems )
	{
	ea->eaHelpMenu= appMakeMenu( &(ea->eaHelpMenuButton),
			&(ea->eaToplevel), ea, ea->eaMenuBar,
			*(ea->eaAppHelpMenuText), 1,
			ea->eaAppHelpMenuItems, ea->eaAppHelpMenuItemCount,
			(void *)ea );
	}

#	ifdef USE_MOTIF
	/*  Work around BUG in LessTif	*/
	{
	Dimension		high;

	XtVaGetValues( ea->eaMenuBar,
				XmNheight,	&high,
				NULL );
	if  ( high < 8 )
	    {
	    XtWidgetGeometry	preferred;

	    XtQueryGeometry( ea->eaMenuBar, (XtWidgetGeometry *)0, &preferred );

	    high= preferred.height+ preferred.border_width;
	    XtVaSetValues( ea->eaMenuBar,
				XmNheight,	high,
				NULL );
	    }
	}
#	endif

    if  ( ea->eaMainPicture )
	{
	APP_BITMAP_IMAGE	labelPixmap;
	APP_BITMAP_MASK		labelMask;

	if  ( ! appGetImagePixmap( ea, ea->eaMainPicture,
						&labelPixmap, &labelMask ) )
	    {
	    appMakeImageInColumn( &pictureLabel, ea->eaMainWindow,
						    labelPixmap, labelMask );
	    }
	}

    if  ( ea->eaNameAndVersion )
	{
	char *		ident= (char *)0;


	if  ( 0 && ea->eaPlatformCompiled )
	    {
	    ident= malloc( strlen( ea->eaNameAndVersion )+ 3+
					strlen( ea->eaPlatformCompiled )+ 1 );
	    if  ( ! ident )
		{ XDEB(ident); }
	    else{
		sprintf( ident, "%s - %s",
				ea->eaNameAndVersion, ea->eaPlatformCompiled );
		}
	    }
	else{
	    ident= strdup( ea->eaNameAndVersion );
	    }

	if  ( ! ident )
	    { XDEB(ident);	}
	else{
	    appMakeLabelInColumn( &label, ea->eaMainWindow, ident );

	    free( ident );
	    }
	}

    if  ( ea->eaReference )
	{
	appMakeLabelInColumn( &label, ea->eaMainWindow,
						    ea->eaReference );
	}

    if  ( ea->eaMainIcon						&&
	  appGetImagePixmap( ea, ea->eaMainIcon, &pixmap, &mask )	)
	{ SDEB(ea->eaMainIcon); return -1;	}

    if  ( pixmap )
	{
#	ifdef USE_MOTIF
	XtVaSetValues( ea->eaToplevel.atTopWidget,
				    XmNiconPixmap,	pixmap,
				    NULL );
#	endif
#	ifdef USE_GTK
	gdk_window_set_icon( ea->eaToplevel.atTopWidget->window,
				    ea->eaToplevel.atTopWidget->window,
				    pixmap, mask );
#	endif
	}

    return 0;
    }

/************************************************************************/

static int app_version(		EditApplication *		ea,
				const char *			prog,
				const char *			call,
				int				argc,
				char **				argv )
    {
    printf( "%s\n", ea->eaNameAndVersion );
    return 0;
    }

static int app_platform(	EditApplication *		ea,
				const char *			prog,
				const char *			call,
				int				argc,
				char **				argv )
    {
    printf( "%s\n", ea->eaPlatformCompiled );
    return 0;
    }

static int app_build(		EditApplication *		ea,
				const char *			prog,
				const char *			call,
				int				argc,
				char **				argv )
    {
    printf( "%s\n", ea->eaHostDateCompiled );
    return 0;
    }

static int app_fullVersion(	EditApplication *		ea,
				const char *			prog,
				const char *			call,
				int				argc,
				char **				argv )
    {
    printf( "%s %s %s %s\n",
		    ea->eaNameAndVersion,
		    ea->eaOptionalComponents,
		    ea->eaPlatformCompiled,
		    ea->eaHostDateCompiled );

    return 0;
    }

static int app_saveTo(		EditApplication *		ea,
				const char *			prog,
				const char *			call,
				int				argc,
				char **				argv )
    {
    int		rval= 2;

    MemoryBuffer	a0;
    MemoryBuffer	a1;

    utilInitMemoryBuffer( &a0 );
    utilInitMemoryBuffer( &a1 );

    if  ( argc < 2 )
	{ SLDEB(call,argc); rval= -1; goto ready;	}

    if  ( utilMemoryBufferSetString( &a0, argv[0] ) )
	{ LDEB(1); rval= -1; goto ready;	}
    if  ( utilMemoryBufferSetString( &a1, argv[1] ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( appFileConvert( ea, &a0, &a1 ) )
	{ SSSDEB(call,argv[0],argv[1]); rval= -1; goto ready; }

  ready:

    utilCleanMemoryBuffer( &a0 );
    utilCleanMemoryBuffer( &a1 );

    return rval;
    }

static int app_printToFile(	EditApplication *		ea,
				const char *			prog,
				const char *			call,
				int				argc,
				char **				argv )
    {
    if  ( argc < 2 )
	{ SLDEB(call,argc); return -1;	}

    if  ( appPrintToFile( ea, argv[0], argv[1], (const char *)0 ) )
	{ SSSDEB(call,argv[0],argv[1]); return -1; }

    return 2;
    }

static int app_printToFilePaper( EditApplication *		ea,
				const char *			prog,
				const char *			call,
				int				argc,
				char **				argv )
    {
    if  ( argc < 3 )
	{ SLDEB(call,argc); return -1;	}

    if  ( appPrintToFile( ea, argv[0], argv[1], argv[2] ) )
	{ SSSDEB(call,argv[0],argv[2]); return -1; }

    return 3;
    }

static int app_print( 		EditApplication *		ea,
				const char *			prog,
				const char *			call,
				int				argc,
				char **				argv )
    {
    if  ( argc < 1 )
	{ SLDEB(call,argc); return -1;	}

    if  ( appPrintToPrinter( ea, argv[0], (const char *)0, (const char *)0 ) )
	{ SSDEB(call,argv[0]); return -1; }

    return 1;
    }

static int app_printPaper( 	EditApplication *		ea,
				const char *			prog,
				const char *			call,
				int				argc,
				char **				argv )
    {
    if  ( argc < 2 )
	{ SLDEB(call,argc); return -1;	}

    if  ( appPrintToPrinter( ea, argv[0], (const char *)0, argv[1] ) )
	{ SSDEB(call,argv[0]); return -1; }

    return 2;
    }

static int app_printToPrinter( 	EditApplication *		ea,
				const char *			prog,
				const char *			call,
				int				argc,
				char **				argv )
    {
    if  ( argc < 2 )
	{ SLDEB(call,argc); return -1;	}

    if  ( appPrintToPrinter( ea, argv[0], argv[1], (const char *)0 ) )
	{ SSDEB(call,argv[0]); return -1; }

    return 2;
    }

static int app_printToPrinterPaper(
				EditApplication *		ea,
				const char *			prog,
				const char *			call,
				int				argc,
				char **				argv )
    {
    if  ( argc < 3 )
	{ SLDEB(call,argc); return -1;	}

    if  ( appPrintToPrinter( ea, argv[0], argv[1], argv[2] ) )
	{ SSDEB(call,argv[0]); return -1; }

    return 3;
    }

static const SpecialCall AppSpecialCalls[]=
{
    { "version",		app_version,			},
    { "platform",		app_platform,			},
    { "build",			app_build,			},
    { "fullVersion",		app_fullVersion,		},
    { "saveTo",			app_saveTo,			},
    { "printToFile",		app_printToFile,		},
    { "printToFilePaper",	app_printToFilePaper,		},
    { "print",			app_print,			},
    { "printPaper",		app_printPaper,			},
    { "printToPrinter",		app_printToPrinter,		},
    { "printToPrinterPaper",	app_printToPrinterPaper,	},
};

static const int AppSpecialCallCount= sizeof(AppSpecialCalls)/sizeof(SpecialCall);

/************************************************************************/
/*									*/
/*  Handle Special Calls.						*/
/*									*/
/*  1)  It is not necessary to initialize TedApplication to convert	*/
/*	files. Intercept conversion calls to prevent the program from	*/
/*	becoming interactive. NOTE that the calls differ from the calls	*/
/*	that initialize the application: They begin with -- instead of	*/
/*	++ . To override configurable options, the ++Something calls	*/
/*	should be used.							*/
/*									*/
/************************************************************************/

static const SpecialCall * appGetSpecialCall(
				const SpecialCall *	calls,
				int			callCount,
				const char *		call )
    {
    int		c;

    for ( c= 0; c < callCount; calls++, c++ )
	{
	if  ( ! strcmp( call, calls->scCall ) )
	    { return calls;	}
	}

    return (SpecialCall *)0;
    }

static int appMainHandleSpecialCalls(	EditApplication *	ea,
					const char *		prefix,
					int			getResources,
					const char *		prog,
					int			argc,
					char *			argv[] )
    {
    int			prefixLength= strlen( prefix );
    int			done= 0;

    if  ( argc < 1 )
	{ return 0;	}

    while( done < argc )
	{
	const char *		call;
	const SpecialCall *	sc;
	int			args;
	int			off;

	if  ( strncmp( argv[done], prefix, prefixLength ) )
	    { break;	}

	call= argv[done]+ prefixLength;
	sc= appGetSpecialCall( ea->eaSpecialCalls,
					    ea->eaSpecialCallCount, call );
	if  ( ! sc )
	    {
	    sc= appGetSpecialCall( AppSpecialCalls,
					    AppSpecialCallCount, call );
	    }
	if  ( ! sc )
	    { break;	}

	if  ( getResources )
	    {
	    appGetApplicationResourceValues( ea );
	    getResources= 0;
	    }

	off= done+ 1;
	args= (*sc->scExecuteCall)( ea, prog, call, argc- off, argv+ off );
	if  ( args < 0 )
	    { SLDEB(argv[1+done],args); return -1;	}
	done += 1+ args;
	}

    return done;
    }

/************************************************************************/
/*									*/
/*  Use the contents of /etc/papersize as the default papersize.	*/
/*									*/
/************************************************************************/

static void appDefaultPapersize(	EditApplication *	ea )
    {
    const char *	defsz= utilPaperDefaultSize();

    if  ( defsz )
	{
	if  ( appSetSystemProperty( ea, "paper", defsz ) )
	    { SDEB(defsz);	}
	}

    return;
    }

/************************************************************************/

static int appOpenExistingDocument(	EditApplication *	ea,
					int			readOnly,
					int			formatHint,
					const MemoryBuffer *	filename )
    {
    int				rval= 0;
    const MemoryBuffer * const	fileRelativeTo= (const MemoryBuffer *)0;
    int				relativeIsFile= 1;
    EditDocument *		ed;

    MemoryBuffer		absolute;

    const int			suggestStdin= 0;

    utilInitMemoryBuffer( &absolute );

    if  ( appAbsoluteName( &absolute,
		    filename, relativeIsFile, fileRelativeTo ) < 0 )
	{
	SDEB(utilMemoryBufferGetString(filename));
	ed= appOpenDocumentFile( ea, ea->eaToplevel.atTopWidget, (APP_WIDGET)0,
				readOnly, suggestStdin, formatHint, filename );
	}
    else{
	ed= appOpenDocumentFile( ea, ea->eaToplevel.atTopWidget, (APP_WIDGET)0,
				readOnly, suggestStdin, formatHint, &absolute );
	}

    if  ( ! ed )
	{ XDEB(ed); rval= -1;	}

    utilCleanMemoryBuffer( &absolute );

    return rval;
    }

/************************************************************************/

static int appSetProperties(		EditApplication *	ea,
					int			argc,
					char *			argv[] )
    {
    int		arg;
    int		argTo= 1;

    for ( arg= 1; arg < argc; arg++ )
	{
	if  ( arg+ 3 <= argc				&&
	      ! strcmp( argv[arg], "--setProperty" )	)
	    {
	    if  ( appSetUserProperty( ea, argv[arg+ 1], argv[arg+ 2] ) )
		{ SSDEB(argv[arg+ 1], argv[arg+ 2]); return -1;	}

	    arg += 2; continue;
	    }

	if  ( ! strncmp( argv[arg], "--", 2 ) )
	    {
	    char *	eq= strchr( argv[arg], '=' );

	    if  ( eq )
		{
		int	res;

		*eq= '\0';
		res= appSetUserProperty( ea, argv[arg]+ 2, eq+ 1 );
		*eq= '=';

		if  ( res )
		    { SDEB(argv[arg]); return -1;	}

		continue;
		}
	    }

	argv[argTo++]= argv[arg];
	}

    return argTo;
    }

/************************************************************************/
/*									*/
/*  Generic main() procedure.						*/
/*									*/
/*  1)  Connect to the GUI system.					*/
/*  2)  Retrieve resource values and derive some application wide	*/
/*	settings from them.						*/
/*  3)  Do not die too easily.						*/
/*	And try to be immune for shells that make you sensitive for	*/
/*	signals that an X11 application does not want to know about.	*/
/*									*/
/************************************************************************/

int appMain(	EditApplication *	ea,
		int			argc,
		char *			argv[] )
    {
    int			rval= 0;

    int			arg;
    int			res;
    int			didSpecial= 0;
    char *		prog;
    char *		locale= (char *)0;

    MemoryBuffer	filename;
    MemoryBuffer	absolute;
    MemoryBuffer	ext;

    utilInitMemoryBuffer( &filename );
    utilInitMemoryBuffer( &absolute );
    utilInitMemoryBuffer( &ext );

    locale= setlocale( LC_ALL, "" );
    if  ( locale )
	{
	char *	s= locale;
	while( isalpha( *s ) || *s == '_' )
	    { s++;	}

	if  ( s > locale )
	    {
	    int	len= s- locale;

	    ea->eaLocaleName= malloc( len+ 1 );
	    if  ( ! ea->eaLocaleName )
		{ LXDEB(len,ea->eaLocaleName); return -1;	}

	    strncpy( ea->eaLocaleName, locale, len )[len]= '\0';
	    }
	}

    appDefaultPapersize( ea );

    res= appReadUserProperties( ea );
    if  ( res )
	{ LDEB(res); rval= 1; goto ready;	}

    res= appReadSystemProperties( ea );
    if  ( res )
	{ LDEB(res); rval= 1; goto ready;	}

    res= appSetProperties( ea, argc, argv );
    if  ( res < 0 )
	{ LDEB(res); rval= 1; goto ready;	}
    argc= res;

    prog= argv[0];
    res= appMainHandleSpecialCalls( ea, "--", 1, prog, argc- 1, argv+ 1 );
    if  ( res < 0 )
	{ LDEB(res); rval= 1; goto ready;	}
    if  ( res > 0 )
	{ didSpecial= 1; argc -= res; argv += res; argv[0]= prog;	}

    if  ( argc <= 1 && didSpecial )
	{ goto ready;	}

    utilInitDocumentGeometry( &(ea->eaDefaultDocumentGeometry) );

    /*  b  */
    appGetApplicationResourceValues( ea );

    /*  1  */
    if  ( appGuiInitApplication( ea, &argc, &argv ) )
	{ LDEB(1); rval= 1; goto ready;	}

    if  ( ea->eaToplevel.atTopWidget )
	{ appGetPixelsPerTwip( ea );	}

    if  ( appFinishApplicationWindow( ea ) )
	{ LDEB(1); rval= -1; goto ready;	}

    appAllocateCopyPasteTargetAtoms( ea );

#   ifdef USE_MOTIF
    if  ( ea->eaAppSelectionTypeCount > 0 )
	{
	XtAddEventHandler( ea->eaToplevel.atTopWidget,
			PropertyChangeMask, True, appAppGotPasteCall, ea );
	}
#   endif

    res= appMainHandleSpecialCalls( ea, "++", 0, prog, argc- 1, argv+ 1 );
    if  ( res < 0 )
	{ LDEB(res); rval= 1; goto ready;	}
    if  ( res > 0 )
	{ didSpecial= 1; argc -= res; argv += res; argv[0]= prog;	}

    if  ( argc <= 1 && didSpecial )
	{ goto ready;	}

    /*  3  */
    (void) signal( SIGHUP, SIG_IGN );
    (void) signal( SIGINT, SIG_IGN );
#   ifdef SIGWINCH
    (void) signal( SIGWINCH, SIG_IGN );
#   endif

    for ( arg= 1; arg < argc; arg++ )
	{
	const int			readOnly= 0;
	AppFileMessageResources *	afmr= &(ea->eaFileMessageResources);

	int				suggestStdin= 0;
	int				openFormat= -1;

	if  ( utilMemoryBufferSetString( &filename, argv[arg] ) )
	    { LDEB(1); rval= 1; goto ready;	}

	if  ( ! appTestDirectory( &filename ) )
	    {
	    appRunOpenChooser( (APP_WIDGET)0, ea->eaToplevel.atTopWidget,
			ea->eaFileExtensionCount, ea->eaFileExtensions,
			ea->eaDefaultFileFilter, &filename,
			appChooserOpenDocument, ea, (void *)ea );

	    continue;
	    }

	openFormat= appDocumentGetOpenFormat( &suggestStdin,
			ea->eaFileExtensions, ea->eaFileExtensionCount,
			&filename, openFormat );

	if  ( appTestFileExists( &filename ) )
	    {
	    int		createNew= ea->eaCreateNewFromCommand;

	    int		saveFormat= -1;
	    int		suggestStdout= 0;

	    if  ( argc == 2 && openFormat >= 0 && suggestStdin )
		{
		appOpenDocumentFile( ea,
				ea->eaToplevel.atTopWidget, (APP_WIDGET)0,
				readOnly, suggestStdin, openFormat, &filename );

		continue;
		}

	    saveFormat= appDocumentGetSaveFormat( &suggestStdout, ea,
						&filename, (void *)0,
						APPFILE_CAN_SAVE, saveFormat );
	    if  ( saveFormat < 0 || suggestStdout )
		{ createNew= 0;	}

	    if  ( createNew )
		{
		int	ynRes;

		ynRes= appQuestionRunSubjectYesNoDialog( ea,
				    ea->eaToplevel.atTopWidget, (APP_WIDGET)0,
				    argv[arg], afmr->afmrMakeItQuestion );

		if  ( ynRes == AQDrespYES )
		    {
		    if  ( appNewDocument( ea, &filename ) )
			{ SDEB(argv[arg]);	}
		    }
		}
	    else{
		appQuestionRunFilenameErrorDialog( ea,
				    ea->eaToplevel.atTopWidget, (APP_WIDGET)0,
				    &filename, afmr->afmrNoSuchFileMessage );
		}
	    }
	else{
	    (void) appOpenExistingDocument( ea,
					readOnly, openFormat, &filename );
	    }
	}

    if  ( ea->eaVisibleDocumentCount > 0 )
	{ appHideShellWidget( ea->eaToplevel.atTopWidget );	}
    else{ appShowShellWidget( ea, ea->eaToplevel.atTopWidget );	}

#   ifdef USE_MOTIF
    XtAppMainLoop( ea->eaContext );
#   endif

#   ifdef USE_GTK
    gtk_main();
#   endif

#   ifdef USE_QT
    appRunMainLoop( ea );
#   endif

  ready:

    utilCleanMemoryBuffer( &filename );
    utilCleanMemoryBuffer( &absolute );
    utilCleanMemoryBuffer( &ext );

    return rval;
    }
