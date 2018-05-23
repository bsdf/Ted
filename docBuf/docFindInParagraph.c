/************************************************************************/
/*									*/
/*  Find: Look for something inside paragraphs.				*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	<ctype.h>

#   include	<appDebugon.h>

#   include	"docBuf.h"
#   include	"docTreeNode.h"
#   include	"docFind.h"

#   include	<reg.h>

/************************************************************************/
/*									*/
/*  Replace a find pattern.						*/
/*									*/
/************************************************************************/

int docFindSetPattern(		void **			pProg,
				const char *		pattern,
				int			useRegex )
    {
    regProg *			prog= (regProg *)0;
    int				options= 0;

    if  ( ! useRegex )
	{ options |= REGflagESCAPE_REGEX;	}

    if  ( pattern )
	{
	prog= regCompile( pattern, options );
	if  ( ! prog )
	    { SXDEB((char *)pattern,prog); return -1;	}
	}

    if  ( *pProg )
	{ regFree( *pProg );	}
    *pProg= (void *)prog;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Find the first occurrence of a regular expression in a paragraph.	*/
/*									*/
/************************************************************************/

int docFindParaFindNext(	DocumentSelection *		ds,
				BufferItem *			paraBi,
				BufferDocument *		bd,
				const DocumentPosition *	dpFrom,
				void *				through )
    {
    regProg *		prog= (regProg *)through;
    int			res;
    const int		direction= 1;
    ExpressionMatch	em;

    int			from;
    int			past;

    if  (  docParaStrlen( paraBi ) == 0 )
	{ return 1;	}

    res= regFindLeftToRight( &em, prog,
		    (char *)docParaString( paraBi, 0 ),
		    dpFrom->dpStroff, docParaStrlen( paraBi ) );

    if  ( ! res )
	{ return 1;	}

    regGetFullMatch( &from, &past, &em );
    docSetParaSelection( ds, paraBi, direction, from, past- from );

    return 0;
    }

int docFindParaFindPrev(	DocumentSelection *		ds,
				BufferItem *			paraBi,
				BufferDocument *		bd,
				const DocumentPosition *	dpFrom,
				void *				through )
    {
    regProg *		prog= (regProg *)through;
    int			res;
    const int		direction= -1;
    ExpressionMatch	em;

    int			from;
    int			past;

    if  (  docParaStrlen( paraBi ) == 0 )
	{ return 1;	}

    res= regFindRightToLeft( &em, prog,
		    (char *)docParaString( paraBi, 0 ),
		    dpFrom->dpStroff, docParaStrlen( paraBi ) );

    if  ( ! res )
	{ return 1;	}

    regGetFullMatch( &from, &past, &em );
    docSetParaSelection( ds, paraBi, direction, from, past- from );

    return 0;
    }

