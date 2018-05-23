
#   ifndef	APP_CGI_IN_H
#   define	APP_CGI_IN_H	1

/************************************************************************/
/*									*/
/*  A full description of a CGI request.				*/
/*									*/
/************************************************************************/

typedef struct CGIRequest
    {
    void *		cgirHeaderValues;
    void *		cgirEnvironmentValues;
    void *		cgirQueryValues;
    void *		cgirCookies;
    const char *	cgirRequestMethod;
    int			cgirStdinUsed;
    } CGIRequest;

/************************************************************************/
/*									*/
/*  Procedure declarations.						*/
/*									*/
/************************************************************************/

extern CGIRequest *	appCgiInGetRequest( void );

extern void appCgiInFreeRequest( CGIRequest * cgir );

#   endif	/*  APP_CGI_IN_H  */
