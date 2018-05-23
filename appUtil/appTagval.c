#   include	"appUtilConfig.h"

#   include	<stdlib.h>

#   include	"utilTree.h"
#   include	<appTagval.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Retrieve a string,long,double value from a TaggedValueList.		*/
/*									*/
/************************************************************************/

int appTagvalGetStringValue(	const char **		pValue,
					int *			pNull,
					const void *		tree,
					const char *		name )
    {
    const void *	val= utilTreeGetEQ( tree, (const char **)0, name );
    const char *	value;

    if  ( ! val )
	{ return 1;	}

    value= (const char *)val;
    *pNull= *value == '\0';

    *pValue= value;
    return 0;
    }

int appTagvalGetLongValue(	long *			pValue,
					int *			pNull,
					const void *		tree,
					const char *		name )
    {
    char *		past;
    long		l;

    const char *	s;
    int			n;

    int		res= appTagvalGetStringValue( &s, &n, tree, name );

    if  ( res )
	{ SLDEB(name,res); return res;	}

    if  ( n )
	{ *pNull= 1; return 0;	}

    while( *s == ' ' )
	{ s++;	}
    if  ( ! *s )
	{ *pNull= 1; return 0;	}

    l= strtol( s, &past, 10 );

    while( *past == ' ' )
	{ past++;	}

    if  ( *past )
	{ SDEB(s); return -1;	}

    *pNull= 0; *pValue= l; return 0;
    }


int appTagvalGetDoubleValue(	double *		pValue,
					int *			pNull,
					const void *		tree,
					const char *		name )
    {
    char *		past;
    double		d;

    const char *	s;
    int			n;

    int		res= appTagvalGetStringValue( &s, &n, tree, name );

    if  ( res )
	{ SLDEB(name,res); return res;	}

    if  ( n )
	{ *pNull= 1; return 0;	}
    while( *s == ' ' )
	{ s++;	}

    if  ( ! *s )
	{ *pNull= 1; return 0;	}

    d= strtod( s, &past );

    while( *past == ' ' )
	{ past++;	}

    if  ( *past )
	{ SDEB(s); return -1;	}

    *pNull= 0; *pValue= d; return 0;
    }
