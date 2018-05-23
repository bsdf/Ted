/************************************************************************/
/*									*/
/*  Keep a collection of objects with integer properties by number.	*/
/*									*/
/************************************************************************/

#   include	"appUtilConfig.h"

#   include	<stdlib.h>

#   include	<appDebugon.h>

#   include	"utilIntegerValueNode.h"

/************************************************************************/
/*									*/
/*  Initialize/Clean value nodes.					*/
/*									*/
/************************************************************************/

void utilInitIntegerValueNode(	IntegerValueNode *	ivn )
    {
    ivn->ivnValue= 0;
    ivn->ivnIsLeaf= 0;
    ivn->ivnChildCount= 0;
    ivn->ivnChildren= (IntegerValueNode *)0;

    return;
    }

void utilCleanIntegerValueNode(	IntegerValueNode *	ivn )
    {
    int		i;

    if  ( ivn->ivnIsLeaf )
	{ return;	}

    for ( i= 0; i < ivn->ivnChildCount; i++ )
	{
	utilCleanIntegerValueNode( &(ivn->ivnChildren[i]) );
	}

    if  ( ivn->ivnChildren )
	{ free( ivn->ivnChildren );	}

    return;
    }

IntegerValueNode * utilChildIntegerValueNode(
					IntegerValueNode *	ivn,
					int			make,
					int			propval )
    {
    int		l= 0;
    int		r= ivn->ivnChildCount;
    int		m= ( l+ r )/ 2;

    if  ( ivn->ivnChildCount > 0 )
	{
	while ( l < m )
	    {
	    if  ( ivn->ivnChildren[m].ivnValue < propval )
		{ l= m;	}
	    else{ r= m;	}

	    m= ( l+ r )/ 2;
	    }

	if  ( propval > ivn->ivnChildren[m].ivnValue )
	    { m++;	}
	}

#   if 0
    if  ( m < ivn->ivnChildCount			&&
	  propval > ivn->ivnChildren[m].ivnValue	)
	{ LLLDEB(propval,m,ivn->ivnChildren[m].ivnValue); abort();	}
#   endif

    if  ( m >= ivn->ivnChildCount			||
	  propval < ivn->ivnChildren[m].ivnValue	)
	{
	int			i;
	IntegerValueNode *	fresh;

	if  ( ! make )
	    { return (IntegerValueNode *)0;	}

	fresh= (IntegerValueNode *)realloc( ivn->ivnChildren,
			(ivn->ivnChildCount+ 1)* sizeof( IntegerValueNode ) );
	if  ( ! fresh )
	    { XDEB(fresh); return (IntegerValueNode *)0;	}
	ivn->ivnChildren= fresh;

	for ( i= ivn->ivnChildCount; i > m; i-- )
	    { ivn->ivnChildren[i]= ivn->ivnChildren[i- 1];	}

	utilInitIntegerValueNode( &(ivn->ivnChildren[m]) );
	ivn->ivnChildren[m].ivnValue= propval;

	ivn->ivnChildCount++;
	}

    return &(ivn->ivnChildren[m]);
    }
