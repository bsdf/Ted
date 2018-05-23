/************************************************************************/
/*									*/
/*  Manage the nesting of fields inside the text.			*/
/*									*/
/************************************************************************/

#   include	"docEditConfig.h"

#   include	<stdio.h>

#   include	<appDebugon.h>

#   include	<docBuf.h>
#   include	<docNodeTree.h>
#   include	<docField.h>
#   include	<docParaString.h>
#   include	<docParaParticules.h>
#   include	"docEdit.h"

/************************************************************************/
/*									*/
/*  Insert the particule that terminates a field.			*/
/*									*/
/*  1)  Find the particule.						*/
/*  2)  Should it be split?						*/
/*  3)  Insert end particule.						*/
/*  4)  Change its attributes. (Superscript for notes!)			*/
/*									*/
/************************************************************************/

static int docFieldInsertEndParticule(
				BufferDocument *		bd,
				const DocumentPosition *	dpEnd,
				int				part1,
				int *				pTailPart,
				int				fieldNumber,
				const PropertyMask *		taSetMask,
				const TextAttribute *		taSet )
    {
    TextParticule *	tp;
    int			textAttrNr;

    BufferItem *	paraBi= dpEnd->dpNode;

    /*  1  */
    tp= paraBi->biParaParticules+ part1;
    if  ( tp->tpStrlen > 0				&&
	  dpEnd->dpStroff == tp->tpStroff+ tp->tpStrlen	)
	{ part1++; tp++;	}

    if  ( part1 < paraBi->biParaParticuleCount )
	{ textAttrNr= tp[ 0].tpTextAttrNr; }
    else{ textAttrNr= tp[-1].tpTextAttrNr; }

    /*  2  */
    if  ( part1 < paraBi->biParaParticuleCount	&&
	  tp->tpStroff != dpEnd->dpStroff	)
	{
	if  ( docSplitTextParticule( (TextParticule **)0, &tp, paraBi,
						    part1, dpEnd->dpStroff ) )
	    { LDEB(part1); return -1;	}

	part1++;
	}

    /*  3  */
    tp= docMakeSpecialParticule( paraBi, part1,
				dpEnd->dpStroff, DOCkindFIELDTAIL, textAttrNr );
    if  ( ! tp )
	{ XDEB(tp); return -1;	}

    tp->tpObjectNumber= fieldNumber;
    docShiftParticuleOffsets( bd, paraBi, part1+ 1,
				paraBi->biParaParticuleCount, tp->tpStrlen );

    /*  4  */
    if  ( taSetMask && ! utilPropMaskIsEmpty( taSetMask ) )
	{
	if  ( docChangeParticuleAttributes( (int *)0, (PropertyMask *)0,
						bd, paraBi, part1, part1+ 1,
						taSet, taSetMask ) )
	    { LDEB(part1); return -1;	}
	}

    *pTailPart= part1;
    return 0;
    }

/************************************************************************/
/*									*/
/*  Insert the particule that starts a field.				*/
/*									*/
/*  1)  Find the particule.						*/
/*  2)  Should it be split?						*/
/*  3)  Insert start particule.						*/
/*  4)  Change its attributes. (Superscript for notes!)			*/
/*									*/
/************************************************************************/

static int docFieldInsertStartParticule(
				BufferDocument *		bd,
				const DocumentPosition *	dpStart,
				int *				pStroffShift,
				int				part0,
				int *				pHeadPart,
				int *				pTailPart,
				int				fieldNumber,
				const PropertyMask *		taSetMask,
				const TextAttribute *		taSet )
    {
    TextParticule *	tp;
    int			textAttrNr;

    int			part1= *pTailPart;

    BufferItem *	paraBi= dpStart->dpNode;

    /*  1  */
    tp= paraBi->biParaParticules+ part0;
    if  ( tp->tpStrlen > 0					&&
	  dpStart->dpStroff == tp->tpStroff+ tp->tpStrlen	)
	{ part0++; tp++;	}

    if  ( part0 < paraBi->biParaParticuleCount )
	{ textAttrNr= tp[ 0].tpTextAttrNr; }
    else{ textAttrNr= tp[-1].tpTextAttrNr; }

    /*  2  */
    if  ( part0 < paraBi->biParaParticuleCount	&&
	  tp->tpStroff != dpStart->dpStroff	)
	{
	if  ( docSplitTextParticule( (TextParticule **)0, &tp, paraBi,
						part0, dpStart->dpStroff ) )
	    { LDEB(part1); return -1;	}

	if  ( part1 >= part0 )
	    { part1++;	}

	part0++;
	}

    /*  3  */
    tp= docMakeSpecialParticule( paraBi, part0,
			    dpStart->dpStroff, DOCkindFIELDHEAD, textAttrNr );
    if  ( ! tp )
	{ XDEB(tp); return -1;	}

    tp->tpObjectNumber= fieldNumber;

    docShiftParticuleOffsets( bd, paraBi, part0+ 1,
				paraBi->biParaParticuleCount, tp->tpStrlen );
    /*  4  */
    if  ( taSetMask && ! utilPropMaskIsEmpty( taSetMask ) )
	{
	if  ( docChangeParticuleAttributes( (int *)0, (PropertyMask *)0, bd,
						paraBi, part0, part0+ 1,
						taSet, taSetMask ) )
	    { LDEB(part1); return -1;	}
	}

    if  ( part1 >= part0 )
	{ part1++;	}

    *pStroffShift= tp->tpStrlen;
    *pHeadPart= part0; *pTailPart= part1; return 0;
    }

/************************************************************************/
/*									*/
/*  Surround a selected range of text by a text level field.		*/
/*									*/
/************************************************************************/

int docSurroundTextSelectionByField(
				BufferDocument *		bd,
				DocumentTree *			dt,
				const DocumentSelection *	ds,
				DocumentField **		pDf,
				DocumentSelection *		dsInside,
				DocumentSelection *		dsAround,
				int *				pHeadPart,
				int *				pTailPart,
				const PropertyMask *		taSetMask,
				const TextAttribute *		taSet,
				int				fieldKind,
				const FieldInstructions *	fi )
    {
    int				headPart;
    int				tailPart;
    int				startShift= 0;

    DocumentSelection		dsBalanced= *ds;

    DocumentField *		df;
    DocumentField *		dfLeft;
    DocumentField *		dfRight;

    int				headMoved= 0;
    int				tailMoved= 0;

    const BufferItem *		sectBi;
    int				headParaNr;
    int				tailParaNr;

    int				singlePara;
    
    singlePara= DOC_FieldKinds[fieldKind].fkiSingleParagraph;

    if  ( singlePara && dsBalanced.dsHead.dpNode != dsBalanced.dsTail.dpNode )
	{
	LXXDEB(singlePara,dsBalanced.dsHead.dpNode,dsBalanced.dsTail.dpNode);
	return -1;
	}

    sectBi= docGetSectNode( dsBalanced.dsHead.dpNode );
    if  ( ! sectBi )
	{ XDEB(sectBi); return -1;	}

    if  ( docFindParticuleOfPosition( &headPart, (int *)0,
					&(dsBalanced.dsHead), PARAfindLAST ) )
	{ LDEB(dsBalanced.dsHead.dpStroff); return -1;	}
    if  ( docFindParticuleOfPosition( &tailPart, (int *)0,
					&(dsBalanced.dsTail), PARAfindFIRST ) )
	{ LDEB(dsBalanced.dsTail.dpStroff); return -1;	}

    if  ( docBalanceFieldSelection( &dfLeft, &dfRight,
			&headMoved, &tailMoved,
			&headPart, &tailPart, 
			&(dsBalanced.dsHead), &(dsBalanced.dsTail), dt, bd ) )
	{ LDEB(1); return -1;	}

#   if 0
    if  ( headMoved || tailMoved )
	{ LLDEB(headMoved,tailMoved); /*return -1;*/	}
#   endif

    df= docClaimField( &(bd->bdFieldList) );
    if  ( ! df )
	{ XDEB(df); return -1;	}

    /*  find end, split?, insert end particule */
    if  ( docFieldInsertEndParticule( bd, &(dsBalanced.dsTail),
					tailPart, &tailPart,
					df->dfFieldNumber, taSetMask, taSet ) )
	{ LDEB(1); return -1;	}

    /*  find begin, split?, insert start particule */
    if  ( docFieldInsertStartParticule( bd, &(dsBalanced.dsHead), &startShift,
					headPart, &headPart, &tailPart,
					df->dfFieldNumber, taSetMask, taSet ) )
	{ LDEB(1); return -1;	}

    if  ( dsBalanced.dsHead.dpNode == dsBalanced.dsTail.dpNode )
	{ dsBalanced.dsTail.dpStroff += startShift;	}

    headParaNr= docNumberOfParagraph( dsBalanced.dsHead.dpNode );
    tailParaNr= docNumberOfParagraph( dsBalanced.dsTail.dpNode );
    df->dfSelectionScope= sectBi->biSectSelectionScope;
    df->dfHeadPosition.epParaNr= headParaNr;
    df->dfHeadPosition.epStroff= dsBalanced.dsHead.dpStroff;
    df->dfTailPosition.epParaNr= tailParaNr;
    df->dfTailPosition.epStroff= dsBalanced.dsTail.dpStroff;

    if  ( taSetMask && ! utilPropMaskIsEmpty( taSetMask ) )
	{
	PropertyMask	taDoneMask;

	utilPropMaskClear( &taDoneMask );

	if  ( docChangeParticuleAttributes( (int *)0, &taDoneMask, bd,
						dsBalanced.dsHead.dpNode,
						headPart, tailPart,
						taSet, taSetMask ) )
	    { LDEB(1);	}
	}

    if  ( docInsertFieldInTree( &(dt->dtRootFields), df ) )
	{ LDEB(1); return -1;	}

    if  ( docDelimitFieldInDoc( dsInside, dsAround,
					    pHeadPart, pTailPart, bd, df ) )
	{ LDEB(df->dfFieldNumber); return -1; }

    df->dfKind= fieldKind;

    if  ( fi )
	{
	if  ( docCopyFieldInstructions( &(df->dfInstructions), fi ) )
	    { LDEB(1); return -1;	}
	}

    *pDf= df; return 0;
    }

/************************************************************************/

static int docDeleteFieldParticule(	EditOperation *		eo,
					BufferItem *		paraBi,
					int			paraNr,
					int			part )
    {
    int				textAttrNr;
    int				stroffShift= 0;
    const TextParticule *	tp= paraBi->biParaParticules+ part;

    int				partStroff= tp->tpStroff;
    int				partStrlen= tp->tpStrlen;

    textAttrNr= paraBi->biParaParticules[0].tpTextAttrNr;
    docParaStringReplace( &stroffShift, paraBi,
				partStroff, partStroff+ partStrlen,
				(const char *)0, 0 );
    docDeleteParticules( paraBi, part, 1 );

    docEditShiftParticuleOffsets( eo, paraBi, paraNr,
				    part, paraBi->biParaParticuleCount,
				    partStroff, -partStrlen );

    if  ( paraBi->biParaParticuleCount == 0 )
	{
	if  ( ! docInsertTextParticule( paraBi,
					0, 0, 0, DOCkindSPAN, textAttrNr ) )
	    { LDEB(1); return -1;	}
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Delete the begin and start particules of a field and the field	*/
/*  itself.								*/
/*									*/
/*  *pPartHead and *pPartTail are set to values that are appropriate to	*/
/*  delete the contents of the field afterward.				*/
/*									*/
/************************************************************************/

int docDeleteField(		DocumentSelection *		dsExInside,
				EditOperation *			eo,
				BufferItem *			paraNodeHead,
				BufferItem *			paraNodeTail,
				int				partHead,
				int				partTail,
				DocumentField *			df )
    {
    int			rval= 0;

    const int		direction= 1;

    DocumentPosition	dpHead;
    DocumentPosition	dpTail;

    if  ( partTail < 0						||
	  partTail >= paraNodeTail->biParaParticuleCount	||
	  partHead < 0						||
	  partHead >= paraNodeHead->biParaParticuleCount	)
	{
	LLDEB(partTail,paraNodeTail->biParaParticuleCount);
	LLDEB(partHead,paraNodeHead->biParaParticuleCount);
	return -1;
	}

    dpTail.dpNode= paraNodeTail;
    dpTail.dpStroff= paraNodeTail->biParaParticules[partTail].tpStroff;

    dpHead.dpNode= paraNodeHead;
    dpHead.dpStroff= paraNodeHead->biParaParticules[partHead].tpStroff;

    if  ( docDeleteFieldParticule( eo,
		paraNodeTail, df->dfTailPosition.epParaNr, partTail )	)
	{ LLDEB(partTail,paraNodeTail->biParaParticuleCount); return -1; }

    if  ( docDeleteFieldParticule( eo,
		paraNodeHead, df->dfHeadPosition.epParaNr, partHead )	)
	{ LLDEB(partHead,paraNodeHead->biParaParticuleCount); return -1; }

    if  ( docDeleteFieldFromParent( eo->eoTree, df ) )
	{ LDEB(1); rval= -1;	}

    docDeleteFieldFromDocument( eo->eoDocument, df );

    if  ( paraNodeTail == paraNodeHead )
	{ dpTail.dpStroff--;	}

    if  ( dsExInside )
	{
	docSetRangeSelection( dsExInside, &dpHead, &dpTail, direction );
	}

    return rval;
    }

