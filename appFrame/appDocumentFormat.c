/************************************************************************/
/*									*/
/*  Document window application independent functionality.		*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<stdlib.h>

#   include	<appSystem.h>
#   include	"appFrame.h"
#   include	"appQuestion.h"
#   include	"guiWidgetDrawingSurface.h"
#   include	"guiDrawingWidget.h"
#   include	"appFileChooser.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Find out whether a document can be saved in a certain format.	*/
/*									*/
/************************************************************************/

int appDocumentTestCanSave(	EditApplication *		ea,
				const AppFileExtension *	afe,
				void *				privateData,
				unsigned int			useFlags,
				int				format )
    {
    if  ( ( afe->afeUseFlags & useFlags ) != useFlags )
	{ return -1;	}

    if  ( privateData						&&
	  ea->eaCanSaveDocument					&&
	  (*ea->eaCanSaveDocument)( privateData, format )	)
	{ return -1;	}

    return 0;
    }

int appDocumentGetSaveFormat(	int *				pSuggestStdout,
				EditApplication *		ea,
				const MemoryBuffer *		filename,
				void *				privateData,
				unsigned int			useFlags,
				int				format )
    {
    int				rval= -1;
    const AppFileExtension *	afe;
    int				i;
    int				suggestStdout= 0;

    MemoryBuffer		ext;

    utilInitMemoryBuffer( &ext );

    if  ( appFileGetFileExtension( &ext, filename ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( utilMemoryBufferIsEmpty( &ext )		&&
	  filename->mbSize >= 3				&&
	  filename->mbBytes[0] == '-'			&&
	  filename->mbBytes[filename->mbSize-1] == '-'	)
	{
	if  ( utilMemoryBufferGetRange( &ext, filename,
						1, filename->mbSize-2 ) )
	    { LDEB(filename->mbSize); rval= -1; goto ready;	}

	suggestStdout= 1;
	}

    if  ( format >= 0 && format < ea->eaFileExtensionCount )
	{
	afe= ea->eaFileExtensions+ format;

	if  ( ! appDocumentTestCanSave( ea, afe, privateData,
							useFlags, format ) )
	    {
	    if  ( utilMemoryBufferIsEmpty( &ext )			||
		  ! afe->afeExtension					||
		  utilMemoryBufferEqualsString( &ext, afe->afeExtension ) )
		{ rval= format; goto ready;	}
	    }
	}

    afe= ea->eaFileExtensions;
    for ( i= 0; i < ea->eaFileExtensionCount; afe++, i++ )
	{
	if  ( appDocumentTestCanSave( ea, afe, privateData, useFlags, i ) )
	    { continue;	}

	if  ( utilMemoryBufferIsEmpty( &ext )				||
	      ! afe->afeExtension					||
	      utilMemoryBufferEqualsString( &ext, afe->afeExtension )	)
	    { rval= i; goto ready;	}
	}

  ready:

    utilCleanMemoryBuffer( &ext );

    if  ( rval >= 0 && pSuggestStdout )
	{ *pSuggestStdout= suggestStdout;	}

    return rval;
    }

int appDocumentGetOpenFormat(	int *				pSuggestStdin,
				const AppFileExtension *	testExts,
				int				testExtCount,
				const MemoryBuffer *		filename,
				int				format )
    {
    int				rval= -1;
    const AppFileExtension *	afe;
    int				i;
    int				suggestStdin= 0;

    MemoryBuffer		ext;

    utilInitMemoryBuffer( &ext );

    if  ( appFileGetFileExtension( &ext, filename ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( utilMemoryBufferIsEmpty( &ext )		&&
	  filename->mbSize >= 3				&&
	  filename->mbBytes[0] == '-'			&&
	  filename->mbBytes[filename->mbSize-1] == '-'	)
	{
	if  ( utilMemoryBufferGetRange( &ext, filename,
						1, filename->mbSize-2 ) )
	    { LDEB(filename->mbSize); rval= -1; goto ready;	}

	suggestStdin= 1;
	}

    if  ( format >= 0 && format < testExtCount )
	{
	afe= testExts+ format;

	if  ( afe->afeUseFlags & APPFILE_CAN_OPEN )
	    {
	    if  ( utilMemoryBufferIsEmpty( &ext )			||
		  ! afe->afeExtension					||
		  utilMemoryBufferEqualsString( &ext, afe->afeExtension ) )
		{ rval= format; goto ready;	}
	    }
	}

    afe= testExts;
    for ( i= 0; i < testExtCount; afe++, i++ )
	{
	if  ( ! ( afe->afeUseFlags & APPFILE_CAN_OPEN ) )
	    { continue;	}

	if  ( utilMemoryBufferIsEmpty( &ext )				||
	      ! afe->afeExtension					||
	      utilMemoryBufferEqualsString( &ext, afe->afeExtension )	)
	    { rval= i; goto ready;	}
	}

  ready:

    if  ( pSuggestStdin )
	{ *pSuggestStdin= suggestStdin;	}

    utilCleanMemoryBuffer( &ext );

    return rval;
    }

/************************************************************************/
/*									*/
/*  File Conversion entry point. ea is supposed to be initialized.	*/
/*  Depending on the application this routine may or mar not work with	*/
/*  an EditApplication struct that is not initialized.			*/
/*									*/
/************************************************************************/

int appFileConvert(	EditApplication *	ea,
			const MemoryBuffer *	fromName,
			const MemoryBuffer *	toName )
    {
    int			rval= 0;
    int			interactive= ( ea->eaToplevel.atTopWidget != NULL );

    void *		privateData= (void *)0;
    int			suggestStdin= 0;
    int			fromFormat= -1;
    const int		readOnly= 1;
    const int		isNewDocName= 0;
    int			toFormat;

    int			suggestStdout= 0;

    if  ( utilEqualMemoryBuffer( fromName, toName ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( ea->eaMakePrivateData	)
	{
	privateData= (*ea->eaMakePrivateData)();
	if  ( ! privateData )
	    { XDEB(privateData); rval= -1; goto ready; }
	}

    fromFormat= appDocumentGetOpenFormat( &suggestStdin,
			ea->eaFileExtensions, ea->eaFileExtensionCount,
			fromName, fromFormat );

    if  ( (*ea->eaOpenDocument)( ea, privateData, &fromFormat,
			    ea->eaToplevel.atTopWidget, (APP_WIDGET)0,
			    readOnly, suggestStdin, fromFormat, fromName ) )
	{ LDEB(1); rval= -1; goto ready; }

    toFormat= appDocumentGetSaveFormat( &suggestStdout, ea, toName, privateData,
						APPFILE_CAN_SAVE, fromFormat );
    if  ( toFormat < 0 )
	{ LDEB(toFormat); rval= -1; goto ready;	}

    {
    if  ( (*ea->eaSaveDocument)( ea, (DrawingSurface)0,
				    privateData, toFormat, toName,
				    suggestStdout, toName, isNewDocName ) )
	{
	if  ( interactive )
	    {
	    appReportSaveFailure( ea, (APP_WIDGET)0,
				    ea->eaToplevel.atTopWidget, toName );
	    }

	rval= -1; goto ready;
	}
    }

  ready:

    if  ( privateData )
	{ (*ea->eaFreeDocument)( privateData, fromFormat ); }

    return rval;
    }

/************************************************************************/

int appFileCanOpen(	const EditApplication *		ea,
			int				format )
    {
    if  ( format >= 0				&&
	  format < ea->eaFileExtensionCount	)
	{
	const AppFileExtension *	afe= &(ea->eaFileExtensions[format]);

	if  ( afe->afeUseFlags & APPFILE_CAN_OPEN )
	    { return 1;	}
	}

    return 0;
    }


/************************************************************************/
/*									*/
/*  Retrieve the descriptions of the filters in a file chooser.		*/
/*									*/
/************************************************************************/

int appFileFilterGetDescriptions(
				EditApplication *	ea,
				AppFileExtension *	extensions,
				int			extensionCount )
    {
    AppConfigurableResource *	acr;
    AppConfigurableResource *	acri;
    AppFileExtension *		afe;

    int				i;
    int				ignored= 0;

    acr= (AppConfigurableResource *)
		    malloc( extensionCount* sizeof(AppConfigurableResource) );
    if  ( ! acr )
	{ LXDEB(extensionCount,acr); return -1;	}

    afe= extensions;
    acri= acr;
    for ( i= 0; i < extensionCount; afe++, acri++, i++ )
	{
	APP_SET_RESOURCE( acri, afe->afeId,
			i* sizeof(AppFileExtension)+
				    offsetof(AppFileExtension,afeDescription),
			afe->afeDescription );
	}

    appGuiGetResourceValues( &ignored, ea, extensions, acr, extensionCount );

    free( acr );

    return 0;
    }

