/************************************************************************/
/*									*/
/*  Extract a value from a command line argument.			*/
/*									*/
/************************************************************************/

#   include	<stdlib.h>

#   include	"utilArgToX.h"
#   include	<appDebugon.h>


int utilArgToInt(	int *		pVal,
			const char *	arg )
    {
    char *	past;
    int		val= strtol( arg, &past, 10 );

    if  ( past == arg )
	{ return -1;	}
    while( isspace( *past ) )
	{ past++;	}
    if  ( *past )
	{ return -1;	}

    *pVal= val; return 0;
    }

int utilArgToDouble(	double *		pVal,
			const char *		arg )
    {
    char *	past;
    double	val= strtod( arg, &past );

    if  ( past == arg )
	{ return -1;	}
    while( isspace( *past ) )
	{ past++;	}
    if  ( *past )
	{ return -1;	}

    *pVal= val; return 0;
    }

