#   include	"ucdIntern.h"

/************************************************************************/
/*									*/
/*  Map a character value to a class of symbol.				*/
/*  such as:								*/
/*  - Mapping to general class.						*/
/*  - Mapping to bidirectional class.					*/
/*									*/
/*  The generated code consists of a hierarchy of arrays:		*/
/*  - The leaf level consists of 16* 16* 16 arrays of 16 values.	*/
/*    Identical arrays are mapped to the same array instance.		*/
/*  - One level below that is an array of 16* 16 pointers to 16 leaf	*/
/*    level mapping arrays. Again: identical patterns are mapped to the	*/
/*    same instance.							*/
/*  - One more level below that is a similar array of base pages with	*/
/*    references.							*/
/*  - And at the bottom is an array of 16 pointers to the base pages.	*/
/*									*/
/************************************************************************/

int ucdSymbolClass(	const int						sym,
			const unsigned char * const * const * const * const	pppp,
			int							def )
    {
    int						s= sym;
    const unsigned char * const * const *	ppp;
    const unsigned char * const *		pp;
    const unsigned char * 			p;

    if  ( sym < 0 || sym >= 256* 256 )
	{ return def;	}

    ppp= pppp[s/( 16* 16* 16 )];
    if  ( ! ppp )
	{ return def;	}
    s= s % ( 16* 16 * 16 );

    pp= ppp[s/( 16* 16 )];
    if  ( ! pp )
	{ return def;	}
    s= s % ( 16* 16 );

    p= pp[s/( 16 )];
    if  ( ! p )
	{ return def;	}
    s= s % ( 16 );

    return p[s];
    }
