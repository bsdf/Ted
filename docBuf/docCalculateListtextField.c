/************************************************************************/
/*									*/
/*  Buffer administration routines.					*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	<appDebugon.h>

#   include	"docBuf.h"
#   include	"docEvalField.h"
#   include	"docParaParticules.h"
#   include	"docRecalculateFields.h"
#   include	<docListLevel.h>
#   include	<docListDepth.h>

/************************************************************************/
/*									*/
/*  Calculate the string for a listtext field.				*/
/*  The caller (below) makes the particules.				*/
/*									*/
/*  1)  The actual number						*/
/*  2)  The space for the separator.					*/
/*									*/
/************************************************************************/

static int docCalculateListtextParaString( char *		target,
					int			maxsize,
					int			ilvl,
					const int *		numberPath,
					const int *		startPath,
					const int *		formatPath,
					const ListLevel *	ll )
    {
    int		done;

    /*  1  */
    done= docListLevelFormatParagraphNumber( target, maxsize, (int *)0, 0,
			    ilvl, numberPath, startPath, formatPath, ll );
    if  ( done < 0 )
	{ LDEB(done); return done;	}

    target += done;

    /*  2  */
    if  ( ll->llFollow == DOCllfTAB || ll->llFollow == DOCllfSPACE )
	{
	if  ( done+ 1 >= maxsize )
	    { LLDEB(done,maxsize); }

	*(target++)= ' '; done++;
	target[0]= '\0';
	}

    return done;
    }

/************************************************************************/
/*									*/
/*  Evaluate Listtext fields for bulleted and numbered lists &c.	*/
/*									*/
/*  1)  Paranoia: check list number.					*/
/*  2)  Get override and number tree.					*/
/*  3)  Link override to list if nexessary.				*/
/*  4)  Paranoia again: Is it an existing list?				*/
/*  5)  Obtain the path in the number tree. ( E.G: 13.1.18.11 )		*/
/*  6)  Calculate number or bullet text.				*/
/*  7)  Replace old field contents with the new string.			*/
/*  8)  Calculate attribute number for added text.			*/
/*  9)  Insert text particule.						*/
/*  10) Insert tab.							*/
/*									*/
/************************************************************************/

int docRecalculateParaListtextTextParticules(
				int *				pCalculated,
				int *				pPartShift,
				int *				pStroffShift,
				BufferItem *			paraNode,
				int				part,
				int				partCount,
				DocumentField *			dfListtext,
				const RecalculateFields *	rf )
    {
    int				rval= 0;
    const ParagraphProperties *	pp= &(paraNode->biParaProperties);

    int				ilvl= pp->ppListLevel;

    struct ListOverride *	lo;
    struct DocumentList *	dl;
    ListNumberTreeNode *	root;

    int				paraNr= docNumberOfParagraph( paraNode );

    int				startPath[DLmaxLEVELS+1];
    int				formatPath[DLmaxLEVELS+1];
    const ListLevel *		ll= (const ListLevel *)0;
    int				numberPath[DLmaxLEVELS+1];

    char			addedString[200];
    int				addedStrlen;

    int				stroff;
    int				stroffShift;
    int				textAttrNr= -1;

    int				partsMade= 0;
    int				makeSep= 0;
    int				sepKind= DOCkindTAB;

    TextParticule *		tp;

    if  ( pp->ppListOverride <= 0 )
	{ /*LDEB(pp->ppListOverride);*/ *pCalculated= 0; goto ready; }

    /*  1,2,3,4  */
    if  ( docGetListLevelOfParagraph( startPath, formatPath,
					&lo, &dl, &ll, pp, rf->rfDocument ) )
	{
	LLDEB(pp->ppListOverride,pp->ppListLevel);
	*pCalculated= 0; goto ready;
	}

    root= docGetListNumberTree( &(rf->rfTree->dtListNumberTrees),
							pp->ppListOverride );
    if  ( ! root )
	{ LXDEB(pp->ppListOverride,root);  *pCalculated= 0; goto ready;	}

    /*  5  */
    if  ( docListNumberTreeGetNumberPath( numberPath, root, ilvl, paraNr ) )
	{
	LLDEB(pp->ppListOverride,ilvl);
	SDEB(docTreeTypeStr(paraNode->biTreeType));
	*pCalculated= 0; goto ready;
	}

    /*  6  */
    addedStrlen= docCalculateListtextParaString(
				    addedString, sizeof(addedString)- 1, ilvl,
				    numberPath, startPath, formatPath, ll );
    if  ( addedStrlen < 0 )
	{ LLDEB(ilvl,addedStrlen); *pCalculated= 0; goto ready;	}

    /*  7  */
    if  ( docFieldReplaceContents( &stroff, &stroffShift, &textAttrNr,
			    paraNode, part, partCount,
			    *pStroffShift, addedString, addedStrlen, rf ) )
	{ LDEB(1); rval= -1; goto ready;	}

    /*  9  */
    if  ( ll->llFollow == DOCllfTAB )
	{ makeSep= 1; sepKind= DOCkindTAB;	}

    if  ( ll->llFollow == DOCllfSPACE					&&
	  ( PROPmaskISSET( &(ll->llTextAttributeMask),
					TApropTEXTUNDERLINED )	||
	    PROPmaskISSET( &(ll->llTextAttributeMask),
					TApropSTRIKETHROUGH )	)	)
	{ makeSep= 1; sepKind= DOCkindSPAN;	}

    tp= docInsertTextParticule( paraNode, part+ 1,
					    stroff, addedStrlen- makeSep,
					    DOCkindSPAN, textAttrNr );
    if  ( ! tp )
	{ XDEB(tp); *pCalculated= 0; goto ready;	}
    partsMade++;

    /*  10  */
    if  ( makeSep )
	{
	tp= docInsertTextParticule( paraNode, part+ 2,
					    stroff+ addedStrlen- makeSep, 1,
					    sepKind, textAttrNr );
	if  ( ! tp )
	    { XDEB(tp); *pCalculated= 0; goto ready;	}
	partsMade++;
	}

    *pCalculated= 1;
    *pPartShift= partsMade- partCount;
    *pStroffShift += stroffShift;

  ready:

    return rval;
    }

