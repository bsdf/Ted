#   include	"utilLongestCommonSubstring.h"

#   include	<string.h>
#   include	<stdlib.h>
#   include	<appDebugon.h>

int utilLongestCommonSubstring(	const char *	s1,
				const char *	s2 )
    {
    int *	num= (int *)0;
    int		l1;
    int		l2;
    int		i1;
    int		i2;
    int		maxlen= 0;

    if  ( ! s1 || ! *s1 || ! s2 || ! *s2 )
	{ return 0;	}

    l1= strlen( s1 );
    l2= strlen( s2 );

#   define x(i,j) ((i)*l2+(j))

    num= (int *)malloc( l1* l2* sizeof(int) );
    if  ( ! num )
	{ LLXDEB(l1,l2,num); return -1;	}

    for ( i1= 0; i1 < l1; i1++ )
	{
	for ( i2= 0; i2 < l2; i2++ )
	    {
	    if  ( s1[i1] == s2[i2] )
		{
		if  ( i1 == 0 || i2 == 0 )
		    { num[x(i1,i2)]= 1;		}
		else{
		    num[x(i1,i2)]= num[x(i1-1,i2-1)]+ 1;

		    if  ( maxlen < num[x(i1,i2)] )
			{ maxlen=  num[x(i1,i2)];	}
		    }
		}
	    else{ num[x(i1,i2)]= 0;	}
	    }
	}

    free( num );
    return maxlen;
    }
