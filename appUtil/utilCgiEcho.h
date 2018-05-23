#   include	"sioGeneral.h"
#   include	"appCgiIn.h"

/************************************************************************/
/*									*/
/*  Echo some information on a cgi request.				*/
/*									*/
/************************************************************************/

extern void utilCgiEchoValueList(	void *			tree,
					const char *		label,
					SimpleOutputStream *	sos );

extern void utilCgiEchoRequest(		const CGIRequest *	cgir,
					SimpleOutputStream *	sos );

