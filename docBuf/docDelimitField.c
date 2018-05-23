/************************************************************************/
/*									*/
/*  Manage the nesting of fields inside the text.			*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	<stdlib.h>

#   include	<appDebugon.h>

#   include	"docBuf.h"
#   include	"docDebug.h"
#   include	"docParaParticules.h"
#   include	"docField.h"

# if 0

Memory management of fields during editing:
-------------------------------------------

Before an edit operation is started, the selection that is affected 
is balanced with respect to field nesting: Fields head and tail are either
both inside the selection or both outside the selection.

If the edit operation is a deletion or a replacement all fields inside 
the selection are deleted before the edit operation starts to change 
anything else in the document.

This means that the edit operation only has to care about new fields: 
Existing fields are possibly moved around, but they never have to 
be created, copied or deleted.

# endif

/************************************************************************/
/*									*/
/*  Balance a selection such that it does not partially cover any	*/
/*  field.								*/
/*									*/
/*  A)  Determine particule numbers for the least nested particules	*/
/*	at the beginning and the end of the selection.			*/
/*  B)  Determine actual values of the text field nesting levels.	*/
/*  C)  And look for the matching particules at the same level.		*/
/*									*/
/************************************************************************/

static DocumentField * docGetLeftOuterField(
				    DocumentField *		df,
				    const EditPosition *	epHead,
				    const EditPosition *	epTail )
    {
    DocumentField *		rval= (DocumentField *)0;

    while( df )
	{
	int				bbcmp;
	int				ebcmp;
	int				becmp;
	int				eecmp;

	int				overlap;
	int				inside; /* sel inside field */
	int				readonly;
	EditPosition			fieldTailPosition= df->dfTailPosition;

	fieldTailPosition.epStroff++;

	readonly= ! DOC_FieldKinds[df->dfKind].fkiResultEditable;
	bbcmp= docCompareEditPositions( &(df->dfHeadPosition), epHead );
	ebcmp= docCompareEditPositions( &fieldTailPosition, epHead );
	becmp= docCompareEditPositions( &(df->dfHeadPosition), epTail );
	eecmp= docCompareEditPositions( &fieldTailPosition, epTail );

	overlap= ebcmp >  0 && becmp <  0;
	inside = bbcmp <= 0 && eecmp >= 0;

	/********************************************************/
	/*  Field protrudes to the left and it is readonly.	*/
	/*  or							*/
	/*  Partial overlap: Field protrudes to the left and	*/
	/*  it ends inside the selection.			*/
	/*  ....ssss						*/
	/*  ..ffff..						*/
	/*  or							*/
	/*  Also include fields that have the same extent as	*/
	/*  the selection. This approach avoids issues with	*/
	/*  completely empty fields in the document.		*/
	/********************************************************/
	if  ( ( overlap && readonly )			||
	      ( overlap && ! inside && bbcmp <= 0 )	||
	      ( bbcmp == 0 && eecmp == 0 )		)
	    { rval= df;		}

	df= df->dfParent;
	}

    return rval;
    }

static DocumentField * docGetRightOuterField(
				    DocumentField *		df,
				    const EditPosition *	epHead,
				    const EditPosition *	epTail )
    {
    DocumentField *		rval= (DocumentField *)0;

    while( df )
	{
	int				bbcmp;
	int				ebcmp;
	int				becmp;
	int				eecmp;

	int				overlap;
	int				inside; /* sel inside field */
	int				readonly;
	EditPosition			fieldTailPosition= df->dfTailPosition;

	fieldTailPosition.epStroff++;

	readonly= ! DOC_FieldKinds[df->dfKind].fkiResultEditable;
	bbcmp= docCompareEditPositions( &(df->dfHeadPosition), epHead );
	ebcmp= docCompareEditPositions( &fieldTailPosition, epHead );
	becmp= docCompareEditPositions( &(df->dfHeadPosition), epTail );
	eecmp= docCompareEditPositions( &fieldTailPosition, epTail );

	overlap= ebcmp >  0 && becmp <  0;
	inside = bbcmp <= 0 && eecmp >= 0;

	/********************************************************/
	/*  Field protrudes to the right and it is readonly.	*/
	/*  or							*/
	/*  Partial overlap: Field protrudes to the right and	*/
	/*  it begins inside the selection.			*/
	/*  ssss....						*/
	/*  ..ffff..						*/
	/*  or							*/
	/*  Also include fields that have the same extent as	*/
	/*  the selection. This approach avoids issues with	*/
	/*  completely empty fields in the document.		*/
	/********************************************************/
	if  ( ( overlap && readonly )			||
	      ( overlap && ! inside && eecmp >= 0 )	||
	      ( bbcmp == 0 && eecmp == 0 )		)
	    { rval= df;		}

	df= df->dfParent;
	}

    return rval;
    }

int docBalanceFieldSelection(	DocumentField **	pLeftField,
				DocumentField **	pRightField,
				int *			pHeadMoved,
				int *			pTailMoved,
				int *			pHeadPart,
				int *			pTailPart,
				DocumentPosition *	dpHead,
				DocumentPosition *	dpTail,
				DocumentTree *		dt,
				BufferDocument *	bd )
    {
    int				rval= 0;
    int				leftEditable= 0;

    EditPosition		epHead;
    EditPosition		epTail;

    DocumentField *		dfHead;
    DocumentField *		dfTail;

    DocumentField *		dfRight= (DocumentField *)0;
    DocumentField *		dfLeft= (DocumentField *)0;

    int				headMoved= 0;
    int				tailMoved= 0;

    DocumentField **		pathHead= (DocumentField **)0;
    DocumentField **		pathTail= (DocumentField **)0;

    DocumentSelection		dsInsideB;
    DocumentSelection		dsAroundB;

    dsInsideB.dsHead= dsAroundB.dsHead= *dpHead;
    dsInsideB.dsTail= dsAroundB.dsTail= *dpTail;

    docSetEditPosition( &epHead, dpHead );
    docSetEditPosition( &epTail, dpTail );

    dfHead= docFindChildField( &(dt->dtRootFields), &epHead, 1 );
    if  ( dfHead )
	{
	dfLeft= docGetLeftOuterField( dfHead, &epHead, &epTail );
	if  ( dfLeft )
	    {
	    int					headPart= -1;
	    int					tailPart= -1;

	    DocumentSelection			dsInside;
	    DocumentSelection			dsAround;

	    const FieldKindInformation *	fki;

	    fki= DOC_FieldKinds+ dfLeft->dfKind;
	    leftEditable= fki->fkiResultEditable;

	    if  ( docDelimitFieldInDoc( &dsInside, &dsAround,
					&headPart, &tailPart, bd, dfLeft ) )
		{ LDEB(1); rval= -1; goto ready; }

	    headMoved= 1;
	    dsInsideB.dsHead= dsInside.dsHead;
	    dsAroundB.dsHead= dsAround.dsHead;
	    if  ( pHeadPart )
		{ *pHeadPart= headPart;	}
	    }
	}

    dfTail= docFindChildField( &(dt->dtRootFields), &epTail, 0 );
    if  ( dfTail )
	{
	dfRight= docGetRightOuterField( dfTail, &epHead, &epTail );
	if  ( dfRight )
	    {
	    int			headPart= -1;
	    int			tailPart= -1;

	    DocumentSelection	dsInside;
	    DocumentSelection	dsAround;

	    if  ( docDelimitFieldInDoc( &dsInside, &dsAround,
					&headPart, &tailPart, bd, dfRight ) )
		{ LDEB(1); rval= -1; goto ready; }

	    tailMoved= 1;
	    dsInsideB.dsTail= dsInside.dsTail;
	    dsAroundB.dsTail= dsAround.dsTail;
	    if  ( pTailPart )
		{ *pTailPart= tailPart;	}
	    }
	}

    if  ( dfLeft && dfLeft == dfRight && leftEditable )
	{
	if  ( headMoved )
	    { *dpHead= dsInsideB.dsHead;	}
	if  ( tailMoved )
	    { *dpTail= dsInsideB.dsTail;	}
	}
    else{
	if  ( headMoved )
	    { *dpHead= dsAroundB.dsHead;	}
	if  ( tailMoved )
	    { *dpTail= dsAroundB.dsTail;	}
	}

    *pLeftField= dfLeft;
    *pRightField= dfRight;
    *pHeadMoved= headMoved;
    *pTailMoved= tailMoved;

  ready:

    if  ( pathHead )
	{ free( pathHead );	}
    if  ( pathTail )
	{ free( pathTail );	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Count the number of particules inside a text level field. The	*/
/*  begin and end particules are not counted.				*/
/*									*/
/*  1)  Do some sanity checks on the proper nesting of fields.. Just	*/
/*	to have a safe feeling.						*/
/*									*/
/************************************************************************/

int docCountParticulesInField(		const BufferItem *	paraBi,
					int *			pClosed,
					int			part,
					int			partUpto )
    {
    int				fieldLevel= 0;
    int				partCount= 0;
    int				fieldNumber;

    const TextParticule *	tp;

    tp= paraBi->biParaParticules+ part;
    if  ( tp->tpKind != DOCkindFIELDHEAD )
	{ LDEB(tp->tpKind); return -1;	}
    fieldNumber= tp->tpObjectNumber;

    tp++; part++;
    while( part < partUpto )
	{
	if  ( tp->tpKind == DOCkindFIELDHEAD )
	    {
	    /*  1  */
	    if  ( tp->tpObjectNumber == fieldNumber )
		{ LLDEB(tp->tpObjectNumber,fieldNumber);	}

	    fieldLevel++;
	    }

	if  ( tp->tpKind == DOCkindFIELDTAIL )
	    {
	    if  ( tp->tpObjectNumber == fieldNumber )
		{
		/*  1 
		if  ( fieldLevel != 0 )
		    { LDEB(fieldLevel);	}
		*/

		*pClosed= 1;
		return partCount;
		}

	    fieldLevel--;
	    }

	partCount++, part++; tp++;
	}

    /*  1  */
    *pClosed= 0;
    return partCount;
    }

/************************************************************************/
/*									*/
/*  Find the particules that delimit a field.				*/
/*									*/
/************************************************************************/

static int docFindFieldParticules(
				const BufferDocument *		bd,
				int *				pPart0,
				int *				pPart1,
				DocumentSelection *		dsInside,
				DocumentSelection *		dsAround,
				const DocumentSelection *	dsField,
				const DocumentField *		df )
    {
    int				part0;
    int				part1;

    TextParticule *		tp;

    /*  2  */
    if  ( docFindParticuleOfPosition( &part0, (int *)0,
					&(dsField->dsHead), PARAfindFIRST ) )
	{ LDEB(1); return -1;	}

    if  ( dsInside )
	{ *dsInside= *dsField;	}
    if  ( dsAround )
	{ *dsAround= *dsField;	}

    tp= dsField->dsHead.dpNode->biParaParticules+ part0;
    while( part0 < dsField->dsHead.dpNode->biParaParticuleCount	&&
	   tp->tpStroff < dsField->dsHead.dpStroff		)
	{ part0++; tp++;	}
    while( part0 < dsField->dsHead.dpNode->biParaParticuleCount	&&
	   tp->tpStroff == dsField->dsHead.dpStroff		)
	{
	if  ( tp->tpKind == DOCkindFIELDHEAD		&&
	      tp->tpObjectNumber == df->dfFieldNumber	)
	    { break;	}

	part0++; tp++;
	}
    if  ( part0 >= dsField->dsHead.dpNode->biParaParticuleCount	||
	  tp->tpStroff != dsField->dsHead.dpStroff		)
	{
	LLDEB(part0,dsField->dsHead.dpNode->biParaParticuleCount);
	return -1;
	}

    if  ( dsAround )
	{ dsAround->dsHead.dpStroff= tp->tpStroff;			}
    if  ( dsInside )
	{ dsInside->dsHead.dpStroff= tp->tpStroff+ tp->tpStrlen;	}

    /*  3  */
    if  ( docFindParticuleOfPosition( &part1, (int *)0,
					&(dsField->dsTail), PARAfindLAST ) )
	{ LDEB(1); return -1;	}
    tp= dsField->dsTail.dpNode->biParaParticules+ part1;
    while( part1 >= 0					&&
	   tp->tpStroff == dsField->dsTail.dpStroff	)
	{
	if  ( tp->tpKind == DOCkindFIELDTAIL		&&
	      tp->tpObjectNumber == df->dfFieldNumber	)
	    { break;	}

	part1--; tp--;
	}
    if  ( part1 < 0					||
	  tp->tpStroff != dsField->dsTail.dpStroff	)
	{
	LLLDEB(part1,tp->tpStroff,dsField->dsTail.dpStroff);
	LDEB(df->dfFieldNumber);docListNode(0,dsField->dsTail.dpNode,0);
	return -1;
	}

    if  ( dsInside )
	{ dsInside->dsTail.dpStroff= tp->tpStroff;			}
    if  ( dsAround )
	{ dsAround->dsTail.dpStroff= tp->tpStroff+ tp->tpStrlen;	}

    if  ( pPart0 )
	{ *pPart0= part0;	}
    if  ( pPart1 )
	{ *pPart1= part1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Delimit a field in a document.					*/
/*									*/
/*  1)  Find selection.							*/
/*  2)  Find start particule.						*/
/*  3)  Find end particule.						*/
/*									*/
/************************************************************************/

int docDelimitFieldInDoc(	DocumentSelection *	dsInside,
				DocumentSelection *	dsAround,
				int *			pPart0,
				int *			pPart1,
				const BufferDocument *	bd,
				const DocumentField *	df )
    {
    DocumentSelection	dsField;

    /*  1  */
    if  ( docSelectionForEditPositionsInDoc( &dsField, bd,
						&(df->dfSelectionScope),
						&(df->dfHeadPosition),
						&(df->dfTailPosition) ) )
	{ LDEB(1); return -1;	}

    /*  2  */
    if  ( ! pPart0 && ! pPart1 && ! dsInside && ! dsAround )
	{ return 0;	}

    return docFindFieldParticules( bd, pPart0, pPart1,
					    dsInside, dsAround, &dsField, df );
    }

int docDelimitFieldInTree(	DocumentSelection *	dsInside,
				DocumentSelection *	dsAround,
				int *			pPart0,
				int *			pPart1,
				const BufferDocument *	bd,
				const DocumentTree *	dt,
				const DocumentField *	df )
    {
    DocumentSelection	dsField;

    /*  1  */
    if  ( docSelectionForEditPositionsInTree( &dsField, dt,
			    &(df->dfHeadPosition), &(df->dfTailPosition) ) )
	{ LDEB(1); return -1;	}

    /*  2  */
    return docFindFieldParticules( bd, pPart0, pPart1,
					    dsInside, dsAround, &dsField, df );
    }

/************************************************************************/
/*									*/
/*  Delimit the field that must be at the head of a paragraph.		*/
/*  - The list text field in a paragraph that is part of a list.	*/
/*  - The note number at the head of a foot/endnote.			*/
/*									*/
/************************************************************************/

int docDelimitParaHeadField(	DocumentField **	pDfHead,
				DocumentSelection *	dsInsideHead,
				DocumentSelection *	dsAroundHead,
				int *			pHeadPart,
				int *			pTailPart,
				const BufferItem *	paraBi,
				const BufferDocument *	bd )
    {
    int				part;
    const TextParticule *	tp;
    int				fieldKind;

    fieldKind= docParaHeadFieldKind( paraBi, bd );
    if  ( fieldKind < 0 )
	{ return 1;	}

    tp= paraBi->biParaParticules;
    for ( part= 0; part < paraBi->biParaParticuleCount; tp++, part++ )
	{
	DocumentField *	df;

	if  ( tp->tpKind == DOCkindFIELDHEAD )
	    {
	    df= docGetFieldByNumber( &(bd->bdFieldList), tp->tpObjectNumber );
	    if  ( ! df )
		{ LXDEB(tp->tpObjectNumber,df); continue;	}

	    if  ( df->dfKind == fieldKind )
		{
		if  ( docDelimitFieldInDoc( dsInsideHead, dsAroundHead,
					    pHeadPart, pTailPart, bd, df ) )
		    { LDEB(1); return -1; }

		if  ( pDfHead )
		    { *pDfHead= df;	}

		return 0;
		}
	    }
	}

    return 1;
    }

