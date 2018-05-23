/************************************************************************/
/*									*/
/*  Run file choosers for edit applications.				*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<string.h>

#   include	<appSystem.h>
#   include	<sioFileio.h>

#   include	"appFrame.h"
#   include	"appPrintJob.h"
#   include	<appDebugon.h>

#   include	"appFileChooser.h"
#   include	"appQuestion.h"

/************************************************************************/
/*									*/
/*  Error messages.							*/
/*									*/
/************************************************************************/

static AppConfigurableResource APP_ChooserResourceTable[]=
    {
	APP_RESOURCE( "chooserNoFilename",
		    offsetof(AppFileChooserResources,acrNoFilenameMessage),
		    "Please Give a Name" ),
	APP_RESOURCE( "chooserIsDirectory",
		    offsetof(AppFileChooserResources,acrIsDirecoryMessage),
		    "Is a directory" ),
	APP_RESOURCE( "chooserNotWritable",
		    offsetof(AppFileChooserResources,acrNotWritableMessage),
		    "You have no permission to write this file." ),
	APP_RESOURCE( "chooserNotReadable",
		    offsetof(AppFileChooserResources,acrNotReadableMessage),
		    "You have no permission to read this file." ),
	APP_RESOURCE( "chooserOverwrite",
		    offsetof(AppFileChooserResources,acrOverwriteMessage),
		    "Do you want to overwrite this file?" ),
	APP_RESOURCE( "chooserNoSuchDir",
		    offsetof(AppFileChooserResources,acrNoSuchDirMessage),
		    "This directory does not exist." ),
    };

/************************************************************************/
/*									*/
/*  Retrieve the configurable texts of a file chooser.			*/
/*									*/
/************************************************************************/

void appFileChooserGetTexts(	EditApplication *		ea,
				AppChooserInformation *		aci )
    {
    static int				gotResources= 0;
    static AppFileChooserResources	APP_ChooserResourceValues;

    if  ( ! gotResources )
	{
	appGuiGetResourceValues( &gotResources, ea,
				    (void *)&APP_ChooserResourceValues,
				    APP_ChooserResourceTable,
				    sizeof(APP_ChooserResourceTable)/
				    sizeof(AppConfigurableResource) );
	}

    aci->aciResources= &APP_ChooserResourceValues;

    return;
    }

/************************************************************************/
/*									*/
/*  Some tests on the name of a file that is to be opened.		*/
/*									*/
/************************************************************************/

int appFileChooserTestNameForOpen(
				const AppChooserInformation *	aci )
    {
    EditApplication *		ea= aci->aciApplication;
    APP_WIDGET			option= aci->aciOption;

    int				fileExists= 0;
    int				fileReadable= 0;
    int				isDir= 0;

    const AppFileChooserResources *	acr= aci->aciResources;

    fileExists= appTestFileExists( &(aci->aciFilename) ) == 0;

    if  ( fileExists )
	{ fileReadable= appTestFileReadable( &(aci->aciFilename) ) == 0; }
    else{ isDir= appTestDirectory( &(aci->aciFilename) ) == 0;		}

    if  ( isDir )
	{
	appQuestionRunFilenameErrorDialog( ea, aci->aciRelativeTo,
		    option, &(aci->aciFilename), acr->acrIsDirecoryMessage );
	return -1;
	}

    if  ( ! fileExists )
	{
	AppFileMessageResources *	afmr= &(ea->eaFileMessageResources);

	appQuestionRunFilenameErrorDialog( ea, aci->aciRelativeTo,
		    option, &(aci->aciFilename), afmr->afmrNoSuchFileMessage );

	return -1;
	}

    if  ( ! fileReadable )
	{
	appQuestionRunFilenameErrorDialog( ea, aci->aciRelativeTo,
		    option, &(aci->aciFilename), acr->acrNotReadableMessage );
	return -1;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Partially validate the file name of a file chooser: if if has the	*/
/*  wrong extension.. set the rigth one.				*/
/*									*/
/*  Return:  0:  Have saved an acceptable file name.			*/
/*  Return:  1:  Not an acceptable file name.				*/
/*  Return: -1:  Failure.						*/
/*									*/
/************************************************************************/

int appChooserSaveFilename(	AppChooserInformation *		aci,
				const MemoryBuffer *		filename,
				const char *			newExtension )
    {
    int					rval= 0;
    EditApplication *			ea= aci->aciApplication;
    APP_WIDGET				option= aci->aciOption;

    const AppFileChooserResources *	acr= aci->aciResources;

    MemoryBuffer			relative;

    utilInitMemoryBuffer( &relative );

    if  ( appFileGetRelativeName( &relative, filename ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( utilMemoryBufferIsEmpty( &relative ) )
	{
	appQuestionRunErrorDialog( ea, aci->aciRelativeTo, option,
						acr->acrNoFilenameMessage );

	rval= 1; goto ready;
	}

    if  ( utilCopyMemoryBuffer( &(aci->aciFilename), filename ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( newExtension && newExtension[0] )
	{
	if  ( appFileSetExtension( &(aci->aciFilename), newExtension ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}

  ready:

    utilCleanMemoryBuffer( &relative );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Some tests on the name for a file to save a document to.		*/
/*									*/
/************************************************************************/

int appFileChooserTestNameForWrite(
				const AppChooserInformation *	aci )
    {
    int				isDir= 0;
    int				fileExists= 0;
    int				dirExists= 0;
    int				fileWritable= 0;

    const AppFileChooserResources *	acr= aci->aciResources;

    const char *	filename= utilMemoryBufferGetString( &(aci->aciFilename) );

    fileExists= appTestFileExists( &(aci->aciFilename) ) == 0;

    if  ( fileExists )
	{ fileWritable= appTestFileWritable( &(aci->aciFilename) ) == 0; }
    else{ isDir= appTestDirectory( &(aci->aciFilename) ) == 0;		}

    if  ( ! fileExists && ! isDir )
	{
	MemoryBuffer	dir;

	utilInitMemoryBuffer( &dir );

	if  ( appDirectoryOfFileName( &dir, &(aci->aciFilename) ) )
	    { LDEB(1); return ACIrespFAILURE;	}

	if  ( ! utilMemoryBufferIsEmpty( &dir ) )
	    { dirExists= appTestDirectory( &dir ) == 0;	}

	utilCleanMemoryBuffer( &dir );
	}

    if  ( isDir )
	{
	appQuestionRunSubjectErrorDialog( aci->aciApplication,
					aci->aciRelativeTo, aci->aciOption,
					filename, acr->acrIsDirecoryMessage );
	return ACIrespNONE;
	}

    if  ( fileExists )
	{
	if  ( ! fileWritable )
	    {
	    appQuestionRunSubjectErrorDialog( aci->aciApplication,
					aci->aciRelativeTo, aci->aciOption,
					filename, acr->acrNotWritableMessage );
	    return ACIrespNONE;
	    }
	else{
	    return appFileChooserConfirmOverWrite( aci, filename );
	    }
	}
    else{
	if  ( ! dirExists )
	    {
	    char *	slash= strrchr( filename+ 1, '/' );

	    if  ( slash )
		{
		*slash= '\0';

		appQuestionRunSubjectErrorDialog( aci->aciApplication,
					aci->aciRelativeTo, aci->aciOption,
					filename, acr->acrNoSuchDirMessage );
		return ACIrespNONE;
		}
	    }
	}

    return ACIrespSAVE;
    }

static int appDocPrintDocument(	EditDocument *		ed,
				void *			through,
				APP_WIDGET		relative,
				APP_WIDGET		option,
				int			format,
				const MemoryBuffer *	filename )
    {
    SimpleOutputStream *	sos;

    PrintJob *			pj= (PrintJob *)through;

    sos= sioOutFileioOpen( filename );

    if  ( ! sos )
	{ XDEB(sos); return -1;	}

    appCallPrintFunction( sos, pj );

    sioOutClose( sos );

    return 0;
    }

void appDocPrintToFile(	APP_WIDGET			option,
			APP_WIDGET			panel,
			EditDocument *			ed,
			const struct PrintGeometry *	pg )
    {
    EditApplication *		ea= ed->edApplication;
    PrintJob			pj;

    if  ( ! ea->eaPrintDocument )
	{ XDEB(ea->eaPrintDocument); return;	}

    appPrintJobForEditDocument( &pj, ed, pg );

    appRunPrintToFileChooser( option, panel, appDocPrintDocument, ed, &pj );
    }

/************************************************************************/
/*									*/
/*  Callbacks from the file menu: Save a document and remember name and	*/
/*  type=format of the output file.					*/
/*									*/
/************************************************************************/

APP_MENU_CALLBACK_H( appDocFileSaveAs, option, voided, e )
    {
    EditDocument *	ed= (EditDocument *)voided;
    EditApplication *	ea= ed->edApplication;

    if  ( ! ea->eaSaveDocument )
	{ XDEB(ea->eaSaveDocument); return;	}

    appRunSaveChooser( option, ed->edToplevel.atTopWidget,
			    APPFILE_CAN_SAVE, appDocSaveDocument,
			    ed, ed->edPrivateData );
    }

/************************************************************************/
/*									*/
/*  Callback from the file-open dialog.					*/
/*									*/
/************************************************************************/

int appChooserOpenDocument(	EditApplication *	ea,
				void *			through,
				APP_WIDGET		relative,
				APP_WIDGET		option,
				const MemoryBuffer *	filename )
    {
    const int	readOnly= 0;

    if  ( ! appOpenDocument( ea, relative, option, readOnly, filename ) )
	{ return -1;	}

    return 0;
    }

