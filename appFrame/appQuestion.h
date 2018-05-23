/************************************************************************/
/*									*/
/*  Run dialogs.							*/
/*									*/
/************************************************************************/

#   ifndef	APP_QUESTION_H
#   define	APP_QUESTION_H

#   include	"appFrame.h"

# ifdef __cplusplus
extern "C"
    {
# endif

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int appQuestionRunSubjectYesNoCancelDialog(
					EditApplication *	ea,
					APP_WIDGET		relative,
					APP_WIDGET		option,
					const char *		subject,
					const char *		question );

extern int appQuestionRunYesNoCancelDialog(
					EditApplication *	ea,
					APP_WIDGET		relative,
					APP_WIDGET		option,
					const char *		question );

extern void appQuestionRunSubjectErrorDialog(
					EditApplication *	ea,
					APP_WIDGET		relative,
					APP_WIDGET		option,
					const char *		subject,
					const char *		message );

extern void appQuestionRunFilenameErrorDialog(
					EditApplication *	ea,
					APP_WIDGET		relative,
					APP_WIDGET		option,
					const MemoryBuffer *	filename,
					const char *		message );

extern void appQuestionRunErrorDialog(	EditApplication *	ea,
					APP_WIDGET		relative,
					APP_WIDGET		option,
					const char *		message );

extern int appQuestionRunOkCancelDialog( EditApplication * ea,
					APP_WIDGET		relative,
					APP_WIDGET		option,
					const char *		question );

extern int appQuestionRunSubjectOkCancelDialog( EditApplication * ea,
					APP_WIDGET		relative,
					APP_WIDGET		option,
					const char *		subject,
					const char *		question );

extern int appQuestionRunFilenameOkCancelDialog( EditApplication * ea,
					APP_WIDGET		relative,
					APP_WIDGET		option,
					const MemoryBuffer *	filename,
					const char *		question );

extern int appQuestionRunSubjectYesNoDialog( EditApplication *	ea,
					APP_WIDGET		relative,
					APP_WIDGET		option,
					const char *		subject,
					const char *		message );

# ifdef __cplusplus
    }
# endif

#   endif
