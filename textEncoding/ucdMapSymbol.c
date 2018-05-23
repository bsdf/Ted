#   include	"ucdIntern.h"

/************************************************************************/
/*									*/
/*  Map a character value to another one. This is used for mappings	*/
/*  such as:								*/
/*  - Mapping to upper/lower case.					*/
/*  - Retrieving the base character of a combined (accented) character.	*/
/*  - Retrieving the mirror character for bidirectional mirroring.	*/
/*									*/
/*  The generated code consists of a hierarchy of arrays:		*/
/*  - The leaf level consists of 16* 16* 16 arrays of 16 shorts. If an	*/
/*    array exists, the relevant position maps the code.		*/
/*  - One level below that is an array of 16* 16 pointers to 16 leaf	*/
/*    level mapping arrays. If at least one mapping exists on the leaf	*/
/*    level, the relevant position holds a pointer to the leaf mapping.	*/
/*  - One more level below that is a similar array of base pages with	*/
/*    references.							*/
/*  - And at the bottom is an array of 16 pointers to the base pages.	*/
/*									*/
/************************************************************************/

int ucdMapSymbol(	const int						sym,
			const unsigned short * const * const * const * const	pppp )
    {
    int						s= sym;
    const unsigned short * const * const *	ppp;
    const unsigned short * const *		pp;
    const unsigned short * 			p;

    if  ( sym < 0 || sym >= 256* 256 )
	{ return sym;	}

    ppp= pppp[s/( 16* 16* 16 )];
    if  ( ! ppp )
	{ return sym;	}
    s= s % ( 16* 16 * 16 );

    pp= ppp[s/( 16* 16 )];
    if  ( ! pp )
	{ return sym;	}
    s= s % ( 16* 16 );

    p= pp[s/( 16 )];
    if  ( ! p )
	{ return sym;	}
    s= s % ( 16 );

    return p[s];
    }
