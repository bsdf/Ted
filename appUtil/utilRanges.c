/************************************************************************/
/*									*/
/*  Split a set of ranges as they appear in the print dialog.		*/
/*									*/
/************************************************************************/

#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<string.h>
#   include	"utilRanges.h"

#   include	<appDebugoff.h>

int utilRangesString(	char *			str,
			int			maxSize,
			const unsigned char *	pages,
			int			size )
    {
    char *		target= str;
    int			from= 0;
    int			upto;

    int			comma= 0;

    while( from < size )
	{
	while( from < size && ! pages[from] )
	    { from++;	}
	upto= from;
	while( upto < size &&   pages[upto] )
	    { upto++;	}

	if  ( from < upto )
	    {
	    char	f[20];
	    int		nf;

	    sprintf( f, "%d", from+ 1 );
	    nf= strlen( f );

	    if  ( from == upto- 1 )
		{
		if  ( comma+ nf > maxSize )
		    { str[0]= '\0'; return -1;	}

		if  ( comma )
		    { *(target++)= ','; maxSize--;	}
		strcpy( target, f ); target += nf; maxSize -= nf;
		}
	    else{
		char	t[20];
		int	nt;

		sprintf( t, "-%d", upto );
		nt= strlen( t );

		if  ( comma+ nf+ nt > maxSize )
		    { str[0]= '\0'; return -1;	}

		if  ( comma )
		    { *(target++)= ','; maxSize--;	}
		strcpy( target, f ); target += nf; maxSize -= nf;
		strcpy( target, t ); target += nt; maxSize -= nt;
		}
	    }

	from= upto+ 1;
	comma= 1;
	}

    target[0]= '\0';
    return target- str;
    }

int utilParseRanges(	unsigned char *		pages,
			int			count,
			const char *		str )
    {
    const char *	s;
    char *		p;
    long		from;
    long		to;

    for ( from= 0; from < count; from++ )
	{ pages[from]= 0;	}

    s= str;
    while( *s )
	{
	while( *s == ' ' || *s == ',' )
	    { s++;	}

	if  ( ! *s )
	    { break;	}

	/* -9 */
	if  ( *s == '-' )
	    {
	    from= 1; s++;
	    to= strtol( s, &p, 10 );
	    if  ( p == s )
		{ SDEB(s); return -1;	}
	    if  ( to < 0 || to > count )
		{ LDEB(to); return -1;	}

	    while( from <= to )
		{ pages[from-1]= 1; from++;	}
	    s= p; continue;
	    }

	from= strtol( s, &p, 10 );
	if  ( p == s )
	    { SDEB(s); return -1;	}
	if  ( from < 0 || from > count )
	    { LDEB(from); return -1;	}
	s= p;
	while( *s == ' ' )
	    { s++;	}
	/*  8  */
	if  ( ! *s || *s == ',' )
	    { pages[from-1]= 1; continue; }

	/*  6-7  */
	if  ( *s != '-' )
	    { SDEB(s); return -1;	}
	s++;

	to= strtol( s, &p, 10 );
	if  ( p == s )
	    { SDEB(s); return -1;	}
	if  ( to < 0 || to > count )
	    { LDEB(to); return -1;	}

	while( from <= to )
	    { pages[from-1]= 1; from++;	}
	s= p; continue;
	}

    return 0;
    }

int utilRangesGetFirst(	const unsigned char *		pages,
			int				count )
    {
    int		i;

    for ( i= 0; i < count; i++ )
	{
	if  ( pages[i] )
	    { return i;	}
	}

    return -1;
    }

int utilRangesGetLast(	const unsigned char *		pages,
			int				count )
    {
    int		i;

    for ( i= count- 1; i >= 0; i-- )
	{
	if  ( pages[i] )
	    { return i;	}
	}

    return -1;
    }

int utilRangesCountOn(	const unsigned char *		pages,
			int				count )
    {
    int		i;
    int		rval= 0;

    for ( i= 0; i < count; i++ )
	{
	if  ( pages[i] )
	    { rval++;	}
	}

    return rval;
    }

int utilParseRange(	int *			pPage0,
			int *			pPage1,
			const char *		str )
    {
    int		p0, p1;
    int		got;
    char	c;

    got= sscanf( str, "%d-%d%c", &p0, &p1, &c );
    
    if  ( got == 2 )
	{ *pPage0= p0-1; *pPage1= p1-1; return 0; }

    SLDEB(str,got); return -1;
    }
