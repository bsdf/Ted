/************************************************************************/
/*									*/
/*  Run dialogs.							*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   include	"appQuestion.h"

void appQuestionRunFilenameErrorDialog(	EditApplication *	ea,
					APP_WIDGET		relative,
					APP_WIDGET		option,
					const MemoryBuffer *	filename,
					const char *		message )
    {
    appQuestionRunSubjectErrorDialog( ea, relative, option,
			utilMemoryBufferGetString( filename ), message );
    }

int appQuestionRunFilenameOkCancelDialog( EditApplication * ea,
					APP_WIDGET		relative,
					APP_WIDGET		option,
					const MemoryBuffer *	filename,
					const char *		question )
    {
    return appQuestionRunSubjectOkCancelDialog( ea, relative, option,
			utilMemoryBufferGetString( filename ), question );
    }
