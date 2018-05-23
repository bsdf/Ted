/************************************************************************/
/*									*/
/*  A mail content type.						*/
/*									*/
/************************************************************************/

#   ifndef	UTIL_MAIL_CONTENT_H
#   define	UTIL_MAIL_CONTENT_H

#   include	"sioGeneral.h"

typedef int (*WriteMail)(	SimpleOutputStream *	sos,
				const char *		mimeBoundary,
				void *			voiddoc );

typedef struct MailContent
    {
    const char *	mcResourceName;
    const char *	mcNameText;
    const char *	mcTypeSlashSubtype;
    int			mcNeedsBoundary;
    WriteMail		mcWriteMail;
    } MailContent;

#   endif
