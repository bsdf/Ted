/************************************************************************/
/*									*/
/*  Map unicodes to alternatives.					*/
/*									*/
/*  The purpose is not to reject a font because it misses a few glyphs	*/
/*  and resonable alternatives are available.				*/
/*									*/
/************************************************************************/

#   include	"textEncodingConfig.h"

#   include	"uniAlternatives.h"
#   include	<appDebugon.h>

typedef struct AlternativeChar
    {
    int		acFrom;
    int		acWith;
    } AlternativeChar;

static const AlternativeChar alternatives[] =
{
    { 0x00a6,	0x007c }, /*  broken bar -> bar.	*/
    { 0x00ac,	0x007e }, /*  logical not -> tilde.	*/
    { 0x00ad,	0x002d }, /*  soft hyphen -> minus	*/
    { 0x00b7,	0x2022 }, /*  .centered -> bullet.	*/
    { 0x00ff,	0x0079 }, /*  ydieresis -> y		*/
    { 0x0394,	0x2206 }, /*  Delta -> Increment.	*/
    { 0x03a9,	0x2126 }, /*  Omega -> Ohm sign.	*/
    { 0x03bc,	0x00b5 }, /*  mu -> micro sign.		*/
    { 0x20ac,	0x0045 }, /*  Euro -> E			*/
    { 0x2215,	0x002f }, /*  fraction -> slash.	*/
};

/************************************************************************/
/*									*/
/*  Map a unicode value to an alternative.				*/
/*									*/
/************************************************************************/

int uniGetAlternative(	int unicode )
    {
    int		l= 0;
    int		r= sizeof(alternatives)/sizeof(AlternativeChar);
    int		m= ( l+ r )/ 2;

    while( m != l )
	{
	if  ( alternatives[m].acFrom > unicode )
	    { r= m;	}
	else{ l= m;	}

	m= ( l+ r )/ 2;
	}

    if  ( alternatives[m].acFrom == unicode )
	{ return alternatives[m].acWith;	}

    return -1;
    }

/************************************************************************/
/*									*/
/*  Extend a set with those values that it has alternatives for.	*/
/*									*/
/************************************************************************/

int uniIncludeWithAlternatives(	IndexSet *		is )
    {
    int		i;

    for ( i= 0; i < sizeof(alternatives)/sizeof(AlternativeChar); i++ )
	{
	if  (   utilIndexSetContains( is, alternatives[i].acWith )	&&
	      ! utilIndexSetContains( is, alternatives[i].acFrom )	&&
	      utilIndexSetAdd( is, alternatives[i].acFrom )		)
	    { LDEB(alternatives[i].acFrom); return -1;	}
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Extend a mapping such that unmapped values are mapped to their	*/
/*  alternatives.							*/
/*									*/
/************************************************************************/

int uniMapToAlternatives(	IndexMapping *		im )
    {
    int		i;

    for ( i= 0; i < sizeof(alternatives)/sizeof(AlternativeChar); i++ )
	{
	int	f= utilIndexMappingGet( im, alternatives[i].acFrom );
	int	w;

	if  ( f >= 0 )
	    { continue;	}

	w= utilIndexMappingGet( im, alternatives[i].acWith );
	if  ( w < 0 )
	    { continue;	}

	if  ( utilIndexMappingPut( im, alternatives[i].acFrom, w ) )
	    { LLDEB(alternatives[i].acFrom,w); return -1;	}
	}

    return 0;
    }

