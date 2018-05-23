/************************************************************************/
/*									*/
/*  Ted: Manipulation of font and text attributes.			*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	<stddef.h>
#   include	<ctype.h>

#   include	"docBuf.h"
#   include	"docNodeTree.h"
#   include	"docParaParticules.h"
#   include	<docListLevel.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Get font properties of the current position.			*/
/*									*/
/************************************************************************/

static int docGetPositionAttributes(
				PropertyMask *			pUpdMask,
				TextAttribute *			pTaNew,
				const BufferDocument *		bd,
				const TextParticule *		tp )
    {
    TextAttribute		ta;

    docGetTextAttributeByNumber( &ta, bd, tp->tpTextAttrNr );

    *pTaNew= ta;

    if  ( pUpdMask )
	{
	utilPropMaskClear( pUpdMask );
	utilPropMaskFill( pUpdMask, TAprop_COUNT );
	}

    return tp->tpTextAttrNr;
    }

/************************************************************************/
/*									*/
/*  Get font properties of a text particule. Use the aatributes of the	*/
/*  list for particules in a list text field.				*/
/*									*/
/************************************************************************/

int docGetEffectiveTextAttributes(
				TextAttribute *			ta,
				BufferDocument *		bd,
				const BufferItem *		paraNode,
				int				part )
    {
    const TextParticule *	tp= paraNode->biParaParticules+ part;
    int				textAttrNr= tp->tpTextAttrNr;

    docGetTextAttributeByNumber( ta, bd, tp->tpTextAttrNr );

    if  ( paraNode->biParaListOverride > 0 )
	{
	DocumentSelection	dsInside;
	DocumentSelection	dsAround;
	int			headPart;
	int			tailPart;

	if  ( ! docDelimitParaHeadField( (DocumentField **)0,
		    &dsInside, &dsAround,
		    &headPart, &tailPart, paraNode, bd )		&&
		    part > headPart					&&
		    part < tailPart					)
	    {
	    const ListLevel *	ll;

	    if  ( docGetListLevelOfParagraph( (int *)0, (int *)0,
		    (struct ListOverride **)0, (struct DocumentList **)0, &ll,
			    &(paraNode->biParaProperties), bd ) )
		{ LDEB(1);	}
	    else{
		PropertyMask		taSetMask;

		taSetMask= ll->llTextAttributeMask;

		if  ( ll->llFollow != DOCllfNONE		&&
		      part == tailPart- 1			&&
		      ( PROPmaskISSET( &taSetMask, TApropTEXTUNDERLINED ) ||
		        PROPmaskISSET( &taSetMask, TApropSTRIKETHROUGH ) ) )
		    {
		    PROPmaskUNSET( &taSetMask, TApropTEXTUNDERLINED );
		    PROPmaskUNSET( &taSetMask, TApropSTRIKETHROUGH );
		    }

		utilUpdateTextAttribute( (PropertyMask *)0, ta,
					&taSetMask, &(ll->llTextAttribute) );

		textAttrNr= docTextAttributeNumber( bd, ta );
		}
	    }
	}

    return textAttrNr;
    }

/************************************************************************/
/*									*/
/*  Get the common text properties of the current selection.		*/
/*									*/
/*  Remember the text attribute of the beginning of the selection.	*/
/*									*/
/*  NOTE that for I-Bar selections, in case of ambiguity, there is a	*/
/*									*/
/*	preference for the attribute of the particule before the	*/
/*	current position.						*/
/*  1)  Get the last particule number for the beginning of the 		*/
/*	selection.							*/
/*  2)  If the paragraph is part of a list, exclude the bullet from	*/
/*	the selection.							*/
/*									*/
/************************************************************************/

int docGetSelectionAttributes(	BufferDocument *		bd,
				const DocumentSelection *	ds,
				PropertyMask *			pUpdMask,
				TextAttribute *			ta )
    {
    BufferItem *		paraNode;
    int				part;
    const TextParticule *	tp;
    int				textAttrNr;

    TextAttribute		taNew;
    PropertyMask		updMask;

    int				bulletPartBegin= -1;
    int				bulletPartEnd= -1;

    int				returnBullet= 0;
    int				IBar= docIsIBarSelection( ds );
    int				flags;

    paraNode= ds->dsHead.dpNode;

    /*  1  */
    if  ( docFindParticuleOfPosition( &part, &flags,
					    &(ds->dsHead), PARAfindLAST ) )
	{
	LDEB(ds->dsHead.dpStroff);
	utilPropMaskClear( pUpdMask );
	return -1;
	}

    /*  2  */
    if  ( paraNode->biParaListOverride > 0 )
	{
	DocumentField *		dfBullet= (DocumentField *)0;
	DocumentSelection	dsInsideBullet;
	DocumentSelection	dsAroundBullet;

	if  ( docDelimitParaHeadField( &dfBullet,
					&dsInsideBullet, &dsAroundBullet,
					&bulletPartBegin, &bulletPartEnd,
					paraNode, bd ) )
	    { LDEB(1);	}

	if  ( part <= bulletPartEnd )
	    {
	    if  ( ! docSelectionInsideParagraph( ds )			||
		  ds->dsTail.dpStroff >= dsAroundBullet.dsTail.dpStroff	)
		{ part= bulletPartEnd+ 1;	}
	    else{ returnBullet= 1;		}
	    }
	}

    tp= paraNode->biParaParticules+ part;

    if  ( IBar				&&
	  part > 0			&&
	  ( flags & POSflagPART_HEAD )	&&
	  tp[-1].tpKind == DOCkindSPAN	)
	{ tp--; part--; }

    textAttrNr= docGetPositionAttributes( &updMask, &taNew, bd, tp );

    if  ( IBar || returnBullet )
	{
	*pUpdMask= updMask;
	*ta= taNew;
	return textAttrNr;
	}

    utilPropMaskClear( &updMask );

    for (;;)
	{
	int	col;

	col= paraNode->biParent->biNumberInParent;

	if  ( ds->dsCol0 < 0					||
	      ds->dsCol1 < 0					||
	      ( col >= ds->dsCol0 && col <= ds->dsCol1 )	)
	    {
	    while( part < paraNode->biParaParticuleCount )
		{
		PropertyMask	pm;
		PropertyMask	pmAll;

		TextAttribute	ta;

		if  ( paraNode == ds->dsTail.dpNode			&&
		      tp->tpStroff >= ds->dsTail.dpStroff	)
		    { break;	}

		docGetTextAttributeByNumber( &ta, bd, tp->tpTextAttrNr );

		utilPropMaskClear( &pm );

		utilPropMaskClear( &pmAll );
		utilPropMaskFill( &pmAll, TAprop_COUNT );

		utilAttributeDifference( &pm, &taNew, &pmAll, &ta );

		utilPropMaskOr( &updMask, &updMask, &pm );

		tp++; part++;
		}
	    }

	if  ( paraNode == ds->dsTail.dpNode )
	    { break;	}

	paraNode= docNextParagraph( paraNode );
	if  ( ! paraNode )
	    { break;	}

	part= 0;
	tp= paraNode->biParaParticules+ part;

	/*  2  */
	if  ( paraNode->biParaListOverride > 0 )
	    {
	    DocumentField *	dfBullet= (DocumentField *)0;
	    DocumentSelection	dsInsideBullet;
	    DocumentSelection	dsAroundBullet;

	    if  ( docDelimitParaHeadField( &dfBullet,
					&dsInsideBullet, &dsAroundBullet,
					&bulletPartBegin, &bulletPartEnd,
					paraNode, bd ) )
		{ LDEB(1);	}

	    part= bulletPartEnd+ 1;
	    tp= paraNode->biParaParticules+ part;
	    }
	}

    utilPropMaskNot( &updMask, &updMask );

    *pUpdMask= updMask;
    *ta= taNew;

    return textAttrNr;
    }

