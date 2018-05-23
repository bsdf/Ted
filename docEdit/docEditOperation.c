/************************************************************************/
/*									*/
/*  Administration of EditOperations.					*/
/*									*/
/************************************************************************/

#   include	"docEditConfig.h"

#   include	<stdio.h>

#   include	<appDebugon.h>

#   include	"docEditOperation.h"
#   include	<docField.h>
#   include	<docNotes.h>
#   include	<docNodeTree.h>
#   include	<docRtfWriter.h>
#   include	<docParaParticules.h>

void docInitEditOperation(	EditOperation *	eo )
    {
    docInitSelectionScope( &(eo->eoSelectionScope) );
    eo->eoBodySectNode= (BufferItem *)0;
    eo->eoBottomField= (DocumentField *)0;

    eo->eoIBarSelectionOld= 0;
    eo->eoMultiParagraphSelectionOld= 0;

    eo->eoParaAdjustParagraphNumber= -1;
    eo->eoParaAdjustStroffFrom= 0;
    eo->eoParaAdjustStroffShift= 0;
    eo->eoParaAdjustStroffUpto= 0;

    docInitEditRange( &(eo->eoReformatRange) );
    docInitEditRange( &(eo->eoSelectedRange) );
    docInitEditRange( &(eo->eoAffectedRange) );
    eo->eoSplitAtHead= 0;

    utilInitIndexSet( &(eo->eoNoteFieldsAdded) );
    eo->eoSectionsDeleted= 0;
    eo->eoSectionsAdded= 0;
    eo->eoParagraphsInserted= 0;
    eo->eoFieldUpdate= FIELDdoNOTHING;
    eo->eoReformatNeeded= REFORMAT_NOTHING;

    eo->eoDocument= (BufferDocument *)0;
    eo->eoTree= (DocumentTree *)0;
    eo->eoCloseObject= (DOC_CLOSE_OBJECT)0;

    docInitDocumentPosition( &(eo->eoHeadDp) );

    docInitDocumentPosition( &(eo->eoTailDp) );

    eo->eoCol0= -1;
    eo->eoCol1= -1;

    docInitDocumentPosition( &(eo->eoLastDp) );

    eo->eoTraceStream= (SimpleOutputStream *)0;
    eo->eoTraceWriter= (struct RtfWriter *)0;
    return;
    }

void docCleanEditOperation(	EditOperation *	eo )
    {
    utilCleanIndexSet( &(eo->eoNoteFieldsAdded) );

    if  ( eo->eoTraceWriter )
	{ docRtfCloseWriter( eo->eoTraceWriter );	}
    if  ( eo->eoTraceStream )
	{ sioOutClose( eo->eoTraceStream );		}

    return;
    }

/************************************************************************/
/*									*/
/*  Include a position in the range in the document that is to be	*/
/*  reformatted.							*/
/*									*/
/*  0)  Not used for adjusts: Remove the adjust.			*/
/*  1)  The document has been empty. in the course of the modification.	*/
/*	Now that a paragraph is included it is not empty anymore. Move	*/
/*	the start position to the beginning of the document.		*/
/*  2)  Shift beginning if necessary.					*/
/*  3)  Shift end if necessary.						*/
/*									*/
/************************************************************************/

static void docIncludePositionInReformat(	EditOperation *		eo,
						const EditPosition *	ep )
    {
    EditRange *		er= &(eo->eoReformatRange);

    /*  0  */
    if  ( eo->eoReformatNeeded < REFORMAT_RANGE )
	{
	eo->eoReformatNeeded=  REFORMAT_RANGE;
	eo->eoParaAdjustParagraphNumber= -1;
	}

    eo->eoParaAdjustParagraphNumber= -1;
    eo->eoParaAdjustStroffFrom= 0;
    eo->eoParaAdjustStroffShift= 0;
    eo->eoParaAdjustStroffUpto= 0;

    /*  1  */
    if  ( er->erHead.epParaNr == 0 )
	{
	er->erHead.epParaNr= 1;
	er->erHead.epStroff= 0;
	}

    /*  2  */
    if  ( er->erHead.epParaNr < 0				||
	  docCompareEditPositions( &(er->erHead), ep ) > 0	)
	{ er->erHead= *ep;	}

    /*  3  */
    if  ( er->erTail.epParaNr < 0				||
	  docCompareEditPositions( &(er->erTail), ep ) < 0	)
	{ er->erTail= *ep;	}

    return;
    }

static void docSetInitialRange(		EditOperation *			eo,
					const DocumentSelection *	ds )
    {
    docSetEditPosition( &(eo->eoReformatRange.erHead), &(ds->dsHead) );
    docSetEditPosition( &(eo->eoReformatRange.erTail), &(ds->dsTail) );

    if  ( eo->eoReformatRange.erHead.epParaNr	!=
	  eo->eoReformatRange.erTail.epParaNr	)
	{
	eo->eoReformatNeeded= REFORMAT_RANGE;
	}

    eo->eoSelectedRange= eo->eoReformatRange;
    eo->eoAffectedRange= eo->eoReformatRange;

    return;
    }

void docEditIncludeNodeInReformatRange(	EditOperation *		eo,
					BufferItem *		bi )
    {
    const BufferItem *	sectNode= (const BufferItem *)0;

    if  ( eo->eoReformatNeeded < REFORMAT_RANGE )
	{
	eo->eoReformatNeeded=  REFORMAT_RANGE;
	eo->eoParaAdjustParagraphNumber= -1;
	}

    eo->eoParaAdjustParagraphNumber= -1;
    eo->eoParaAdjustStroffFrom= 0;
    eo->eoParaAdjustStroffShift= 0;
    eo->eoParaAdjustStroffUpto= 0;

    sectNode= docGetSectNode( bi );

    if  ( ! sectNode							||
	  docSelectionSameScope( &(eo->eoSelectionScope),
				    &(sectNode->biSectSelectionScope) )	)
	{
	/*
	EditRange *		er= &(eo->eoReformatRange);
	*/
	DocumentPosition	dpFirst;
	DocumentPosition	dpLast;

	docInitDocumentPosition( &dpFirst );
	docInitDocumentPosition( &dpLast );

	if  ( docHeadPosition( &dpFirst, bi ) )
	    { LDEB(1);	}
	else{
	    EditPosition	epFirst;

	    docSetEditPosition( &epFirst, &dpFirst );
	    docIncludePositionInReformat( eo, &epFirst );
	    }

	if  ( docTailPosition( &dpLast, bi ) )
	    { LDEB(1);	}
	else{
	    EditPosition	epLast;

	    docSetEditPosition( &epLast, &dpLast );
	    docIncludePositionInReformat( eo, &epLast );
	    }
	}
    else{
	if  ( eo->eoReformatNeeded < REFORMAT_BODY_SECT )
	    {
	    eo->eoReformatNeeded=  REFORMAT_BODY_SECT;
	    eo->eoParaAdjustParagraphNumber= -1;
	    }
	}

    return;
    }

void docEditIncludeRowsInReformatRange(	EditOperation *		eo,
					BufferItem *		parentNode,
					int			row0,
					int			row1 )
    {
    docEditIncludeNodeInReformatRange( eo, parentNode->biChildren[row0] );
    docEditIncludeNodeInReformatRange( eo, parentNode->biChildren[row1] );

    return;
    }

/************************************************************************/
/*									*/
/*  Set the range within a paragraph that must be reformatted.		*/
/*									*/
/*  By default, entire paragraphs are reformatted. As an optimization	*/
/*  we limit reformatting of the text during typing to exactly those	*/
/*  lines that are affected.						*/
/*									*/
/************************************************************************/

void docSetParagraphAdjust(	EditOperation *		eo,
				BufferItem *		paraNode,
				int			stroffShift,
				int			stroffUpto )
    {
    EditRange *		er= &(eo->eoReformatRange);
    int			paraNr;

    if  ( paraNode->biLevel != DOClevPARA )
	{ LDEB(paraNode->biLevel); return;	}

    if  ( eo->eoParaAdjustParagraphNumber >= 0 )
	{ docEditIncludeNodeInReformatRange( eo, paraNode ); return; }

    paraNr= docNumberOfParagraph( paraNode );

    if  ( er->erHead.epParaNr == paraNr				&&
	  er->erTail.epParaNr == paraNr				&&
	  eo->eoReformatNeeded <= REFORMAT_ADJUST_PARAGRAPH	)
	{
	if  ( eo->eoReformatNeeded < REFORMAT_ADJUST_PARAGRAPH )
	    {
	    eo->eoReformatNeeded=  REFORMAT_ADJUST_PARAGRAPH;
	    }

	eo->eoParaAdjustParagraphNumber= paraNr;
	/* eo->eoParaAdjustStroffFrom is set from the beginning */
	eo->eoParaAdjustStroffShift= stroffShift;
	eo->eoParaAdjustStroffUpto= stroffUpto;
	}
    else{
	docEditIncludeNodeInReformatRange( eo, paraNode );
	}

    return;
    }

void docExtendParagraphAdjust(	EditOperation *		eo,
				BufferItem *		paraNode,
				int			stroffShift )
    {
    int			paraNr;

    if  ( paraNode->biLevel != DOClevPARA )
	{ LDEB(paraNode->biLevel); return;	}

    paraNr= docNumberOfParagraph( paraNode );

    if  ( eo->eoReformatNeeded != REFORMAT_ADJUST_PARAGRAPH )
	{
	LLDEB(eo->eoReformatNeeded,REFORMAT_ADJUST_PARAGRAPH); return; }
    if  ( eo->eoParaAdjustParagraphNumber != paraNr )
	{ LLDEB(eo->eoParaAdjustParagraphNumber,paraNr); return;	}

    eo->eoParaAdjustStroffShift += stroffShift;
    eo->eoParaAdjustStroffUpto += stroffShift;

    return;
    }

void docEditOperationGetSelection(		DocumentSelection *	dsNew,
						const EditOperation *	eo )
    {
    const int	direction= 1;

    docSetRangeSelection( dsNew, &(eo->eoHeadDp), &(eo->eoTailDp), direction );
    }

DocumentField * docEditOperationGetSelectedNote(
					struct DocumentNote **	pDn,
					int *			pSelInNote,
					const EditOperation *	eo )
    {
    DocumentSelection	ds;

    docEditOperationGetSelection( &ds, eo );

    return docGetSelectedNote( pDn, pSelInNote, eo->eoDocument, &ds );
    }

/************************************************************************/
/*									*/
/*  Start an edit operation.						*/
/*									*/
/*  1)  Make a first guess about the particules involved: The last one	*/
/*	at the begin position and the first one at the end position.	*/
/*  2)  Adapt selection not to partially overlap fields, nor to fully	*/
/*	lie inside read-only fields.					*/
/*  3)  Admin on the particules.					*/
/*  3)  Aditional admin on the selection.				*/
/*									*/
/*  6)	Find the offset of the last particule that really starts before	*/
/*	the edit operation. This is the point to start a partial layout	*/
/*	of the paragraph.						*/
/*									*/
/************************************************************************/

int docStartEditOperation(	EditOperation *			eo,
				const DocumentSelection *	ds,
				BufferDocument *		bd )
    {
    const BufferItem *		paraNode;

    DocumentField *		dfLeft;
    DocumentField *		dfRight;
    int				beginMoved= 0;
    int				endMoved= 0;
    int				headPart= -1;
    int				tailPart= -1;
    int				headFlags= 0;
    int				tailFlags= 0;
    int				part;

    DocumentSelection		dsBal= *ds;

    const TextParticule *	tp;
    int				multiPara;

    multiPara= dsBal.dsHead.dpNode != dsBal.dsTail.dpNode;

    /*  1  */
    if  ( docFindParticuleOfPosition( &headPart, &headFlags,
					    &(dsBal.dsHead), PARAfindPAST ) )
	{ LDEB(1); return -1; }
    if  ( docFindParticuleOfPosition( &tailPart, &tailFlags,
					    &(dsBal.dsTail), PARAfindPAST ) )
	{ LDEB(1); return -1; }

    paraNode= dsBal.dsTail.dpNode;
    tp= paraNode->biParaParticules;

    while( ( multiPara || tailPart > headPart )			&&
	   tailPart > 0						&&
	   tp[tailPart-1].tpKind != DOCkindSPAN			&&
	   tp[tailPart-1].tpStrlen == 0				&&
	   tp[tailPart-1].tpStroff == dsBal.dsTail.dpStroff	)
	{ tailPart--;	}

    /*  4  */
    eo->eoSelectionScope= dsBal.dsSelectionScope;

    if  ( docGetRootOfSelectionScope( &(eo->eoTree),
			    (BufferItem **)0, bd, &(eo->eoSelectionScope) ) )
	{ LDEB(1); return -1;	}

    docSetInitialRange( eo, &dsBal );

    /*  2  */
    if  ( docBalanceFieldSelection( &dfLeft, &dfRight,
			    &beginMoved, &endMoved, &headPart, &tailPart, 
			    &(dsBal.dsHead), &(dsBal.dsTail),
			    eo->eoTree, bd ) )
	{ LDEB(1); return -1;	}

    eo->eoBottomField= docFieldGetCommonParent( dfLeft, dfRight );

    /*  3  */
    eo->eoHeadDp= dsBal.dsHead;
    eo->eoTailDp= dsBal.dsTail;
    eo->eoLastDp= dsBal.dsTail;

    eo->eoCol0= ds->dsCol0;
    eo->eoCol1= ds->dsCol1;

    /*  5  */
    eo->eoDocument= bd;

    /*  6  */
    if  ( headPart < dsBal.dsHead.dpNode->biParaParticuleCount )
	{
	part= headPart;
	tp= dsBal.dsHead.dpNode->biParaParticules+ part;
	while( part >= 0 )
	    {
	    if  ( tp->tpStroff < eo->eoHeadDp.dpStroff )
		{
		eo->eoParaAdjustStroffFrom= tp->tpStroff;
		break;
		}

	    part--; tp--;
	    }
	}

#   if 0
    docListNode(0,eo->eoHeadDp.dpNode,0);
    if  ( eo->eoTailDp.dpNode != eo->eoHeadDp.dpNode )
	{ docListNode(0,eo->eoTailDp.dpNode,0); }
#   endif

    return 0;
    }

int docMoveEditOperationToBodySect(	EditOperation *		eo,
					BufferItem *		bodySectNode )
    {
    DocumentSelection		dsSect;
    int				direction= 0;

    if  ( docHeadPosition( &(dsSect.dsHead), bodySectNode )	||
	  docTailPosition( &(dsSect.dsTail), bodySectNode )	||
	  docSelectWholeSection( &dsSect, direction )		)
	{ LDEB(1); return -1;	}

    eo->eoHeadDp= dsSect.dsHead;
    eo->eoTailDp= dsSect.dsTail;
    eo->eoLastDp= dsSect.dsTail;

    eo->eoTree= &(eo->eoDocument->bdBody);
    docEditIncludeNodeInReformatRange( eo, bodySectNode );

    docGetSelectionScope( &(eo->eoSelectionScope), bodySectNode );

    return 0;
    }

