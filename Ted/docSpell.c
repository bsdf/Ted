/************************************************************************/
/*									*/
/*  Spell checking utility routines.					*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdio.h>
#   include	<ctype.h>

#   include	<appDebugon.h>

#   include	"tedFind.h"
#   include	<docBuf.h>
#   include	<docParaParticules.h>

#   include	<ind.h>
#   include	<reg.h>

/*
 * OpenOffice dictionaries are stored in:
 * Ubuntu 6.06, OpenOffice 2.0.2: /usr/lib/openoffice/share/dict/ooo/
*/

/* PCRE syntax */
/*
Only work for ASCII:
static const char docSpellWordPattern[]= "\\b(\\w+)\\W*";
static const char docSpellWordPattern[]= "\\b([[:alnum:]]+)[^[:alnum:]]*";
Look like unicode, but \\b only works for ASCII
static const char docSpellWordPattern[]= "\\b([\\p{N}\\p{L}]+)[^\\p{N}\\p{L}]*";
*/

static const char docSpellFirstWordPat[]= "[^\\p{N}\\p{L}]*([\\p{N}\\p{L}]+)[^\\p{N}\\p{L}]*";
static regProg * docSpellFirstWordExpr= (regProg *)0;

static const char docSpellToWordPat[]= "[\\p{N}\\p{L}]*[^\\p{N}\\p{L}]*";
static regProg * docSpellToWordExpr= (regProg *)0;

static const char docSpellNextWordPat[]= "([\\p{N}\\p{L}]+)[^\\p{N}\\p{L}]*";
static regProg * docSpellNextWordExpr= (regProg *)0;

/************************************************************************/

/************************************************************************/
/*									*/
/*  Find the first misspelling from a certain position in a paragraph.	*/
/*									*/
/*  1)	Skip initial particules: Find the first particule that starts	*/
/*	after the starting point for spell checking.			*/
/*  2)	Skip particules that can be included in a spell check range.	*/
/*  3)	If spell checking starts in the middle of a word, skip to the	*/
/*	end of the word.						*/
/*  4)	Do not extend the string beyond the beginning/end of a field.	*/
/*									*/
/*  TODO: Do not check the contents of fields that are not editable.	*/
/*									*/
/************************************************************************/

int docSpellParaFindNext(	DocumentSelection *		ds,
				BufferItem *			paraBi,
				BufferDocument *		bd,
				const DocumentPosition *	dpFrom,
				void *				through )
    {
    SpellDictionary *		spd= (SpellDictionary *)through;

    DocumentPosition		dpHead= *dpFrom;
    DocumentPosition		dpHere= *dpFrom;
    int				acceptedPos= dpHere.dpStroff;
    char *			paraStr= (char *)docParaString( paraBi, 0 );

    SpellCheckContext *		scc= (SpellCheckContext *)spd->sdSpellCheckContext;
    SpellScanJob		ssj;

    int				fromHead;
    const regProg *		stepExpr;

    if  ( ! docSpellFirstWordExpr )
	{
	int		options= 0;

	docSpellFirstWordExpr= regCompile( docSpellFirstWordPat, options );

	if  ( ! docSpellFirstWordExpr )
	    { SXDEB(docSpellFirstWordPat,docSpellFirstWordExpr); return -1; }
	}

    if  ( ! docSpellToWordExpr )
	{
	int		options= 0;

	docSpellToWordExpr= regCompile( docSpellToWordPat, options );

	if  ( ! docSpellToWordExpr )
	    { SXDEB(docSpellToWordPat,docSpellToWordExpr); return -1; }
	}

    if  ( ! docSpellNextWordExpr )
	{
	int		options= 0;

	docSpellNextWordExpr= regCompile( docSpellNextWordPat, options );

	if  ( ! docSpellNextWordExpr )
	    { SXDEB(docSpellNextWordPat,docSpellNextWordExpr); return -1; }
	}

    if  ( docHeadPosition( &dpHead, paraBi ) )
	{ LDEB(1); return -1;	}

    if  ( dpFrom->dpNode != dpHead.dpNode )
	{ XXDEB(dpFrom->dpNode,dpHead.dpNode); return -1;	}

    fromHead= dpFrom->dpStroff == dpHead.dpStroff;

    if  ( fromHead )
	{ stepExpr= docSpellFirstWordExpr;	}
    else{ stepExpr= docSpellToWordExpr;		}

    indInitSpellScanJob( &ssj );

    while( dpHere.dpStroff < docParaStrlen( paraBi ) )
	{
	int			foundWord;
	int			tail;
	int			from;
	ExpressionMatch		em;

	int			count;

	int			part;
	const TextParticule *	tp;

	dpHead= dpHere;

	foundWord= regFindLeftToRight( &em, stepExpr,
			paraStr, dpHere.dpStroff, docParaStrlen( paraBi ) );

	if  ( ! foundWord )
	    { break;	}

	if  ( regGetFullMatch( &(dpHead.dpStroff), &(dpHere.dpStroff), &em ) )
	    { LDEB(1); return -1;	}

	if  ( stepExpr == docSpellToWordExpr )
	    { stepExpr= docSpellNextWordExpr; continue;	}
	stepExpr= docSpellNextWordExpr;

	if  ( regGetMatch( &(dpHead.dpStroff), &tail, &em, 0 ) )
	    { LDEB(1); return -1;	}

	if  ( indNewPossibility( &ssj, dpHead.dpStroff ) )
	    { CDEB(paraStr[dpHead.dpStroff]); return -1;	}

	if  ( docFindParticuleOfPosition( &part, (int *)0,
						    &dpHead, PARAfindLAST ) )
	    { LDEB(dpHead.dpStroff); return -1;	}
	part++; tp= paraBi->biParaParticules+ part;
	while( part < paraBi->biParaParticuleCount	&&
	       tp->tpStroff < dpHere.dpStroff		)
	    {
	    if  ( tp->tpStroff > dpHead.dpStroff	&&
		  tp->tpKind != DOCkindSPAN		)
		{ dpHere.dpStroff= tp->tpStroff; break; }
	    part++; tp++;
	    }

	from= dpHead.dpStroff;
	while( from < tail )
	    { indAddCharacterToPossibilities( &ssj, paraStr[from++] ); }

	/*  Does the word match here?  */
	count= indCountPossibilities( &ssj, scc, tail,
				dpHere.dpStroff >= docParaStrlen( paraBi ) );

	if  ( count == 0 )
	    {
	    PossibleWord *	maxpw;
	    const int		direction= 1;

	    maxpw= indMaximalPossibility( &ssj );

	    if  ( ! maxpw )
		{ XDEB(maxpw); return -1;	}

	    docSetParaSelection( ds, paraBi, direction,
						maxpw->pwStartAt,
						maxpw->pwInsertionPoint );
	    indCleanSpellScanJob( &ssj );

	    return 0;
	    }

	while( from < dpHere.dpStroff )
	    { indAddCharacterToPossibilities( &ssj, paraStr[from++] ); }

	/*  Is the word and the trailing white space a valid prefix? */
	count= indCountPossibilities( &ssj, scc, dpHere.dpStroff,
				dpHere.dpStroff >= docParaStrlen( paraBi ) );

	indRejectPossibilities( &acceptedPos, acceptedPos, &ssj );
	}

    indCleanSpellScanJob( &ssj );

    return 1;
    }

