/************************************************************************/
/*  Really Quit, Really Close dialogs.					*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>

#   include	"appFrame.h"
#   include	"appQuestion.h"
#   include	"appFileChooser.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Run a 'Really Close' dialog.					*/
/*									*/
/************************************************************************/

void appRunReallyCloseDialog(	APP_WIDGET			option,
				EditDocument *			ed )
    {
    EditApplication *		ea= ed->edApplication;
    int				rcc;

    AppFileMessageResources *	afmr= &(ea->eaFileMessageResources);

    rcc= appQuestionRunSubjectYesNoCancelDialog( ea,
				ed->edToplevel.atTopWidget, option,
				utilMemoryBufferGetString( &(ed->edTitle) ),
				afmr->afmrReallyCloseQuestion );

    switch( rcc )
	{
	case AQDrespYES:
	    if  ( ! ea->eaSaveDocument )
		{ XDEB(ea->eaSaveDocument); return;	}

	    if  ( utilMemoryBufferIsEmpty( &(ed->edFilename) ) )
		{
		appRunSaveChooser( option, ed->edToplevel.atTopWidget,
					APPFILE_CAN_SAVE, appDocSaveDocument,
					ed, ed->edPrivateData );
		}
	    else{
		if  ( ! appDocSaveDocument( ed, (void *)0,
					    ed->edToplevel.atTopWidget, option,
					    ed->edFormat, &(ed->edFilename) ) )
		    { appDocumentChanged( ed, 0 ); }
		}

	    if  ( ! ed->edHasBeenChanged )
		{ appCloseDocument( ed );	}
	    break;

	case AQDrespNO:
	    appCloseDocument( ed );
	    break;

	case AQDrespCANCEL:
	case AQDrespCLOSED:
	    break;
	default:
	    LDEB(rcc); break;
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Run a 'Really Quit' dialog.						*/
/*									*/
/************************************************************************/

int appRunReallyQuitDialog(	APP_WIDGET			option,
				APP_WIDGET			relative,
				EditApplication *		ea )
    {
    AppFileMessageResources *	afmr= &(ea->eaFileMessageResources);

    return appQuestionRunYesNoCancelDialog( ea,
					relative, option,
					afmr->afmrReallyQuitQuestion );
    }
