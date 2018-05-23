/************************************************************************/
/*									*/
/*  Buffer administration routines relating to the text particules in a	*/
/*  text paragraph.							*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	<stdlib.h>

#   include	<appDebugon.h>

#   include	<uniUtf8.h>

#   include	"docBuf.h"
#   include	"docShape.h"
#   include	"docParaString.h"
#   include	"docParaParticules.h"
#   include	"docDebug.h"
#   include	<docObjectProperties.h>

/*
# define DEB_PARTICULES
*/

/************************************************************************/

static void docInitTextParticule(	TextParticule *		tp )
    {
    tp->tpStroff= 0;
    tp->tpStrlen= 0;
    tp->tpKind= DOCkindUNKNOWN;
    tp->tpFlags= 0;
    tp->tpTwipsWide= 0;	
    tp->tpXContentXPixels= 0;	
    tp->tpObjectNumber= -1;
    tp->tpTextAttrNr= 0;

    return;
    }

/************************************************************************/
/*									*/
/*  Split a text particule.						*/
/*									*/
/*  E.G. To change the attributes of part of its text or to insert	*/
/*  something in the middle.						*/
/*									*/
/************************************************************************/

int docSplitTextParticule(	TextParticule **		pTpPart,
				TextParticule **		pTpNext,
				BufferItem *			paraNode,
				int				part,
				int				stroff )
    {
    TextParticule	tpScratch;
    TextParticule *	tpPart;
    TextParticule *	tpNext;

    if  ( part < 0 || part >= paraNode->biParaParticuleCount )
	{ LLDEB(part,paraNode->biParaParticuleCount); return -1;	}

    tpPart= paraNode->biParaParticules+ part;
    tpScratch= *tpPart;

    if  ( tpPart->tpKind != DOCkindSPAN			||
	  stroff <= tpPart->tpStroff			||
	  stroff >= tpPart->tpStroff+ tpPart->tpStrlen	)
	{
	SLLLDEB(docKindStr(tpPart->tpKind),stroff,tpPart->tpStroff,tpPart->tpStrlen);
	docListNode(0,paraNode,0);
	return -1;
	}

    tpPart= docInsertTextParticule( paraNode, part,
					    tpScratch.tpStroff,
					    stroff- tpScratch.tpStroff,
					    tpScratch.tpKind,
					    tpScratch.tpTextAttrNr );
    if  ( ! tpPart )
	{ LXDEB(part,tpPart); return -1;	}

    tpNext= tpPart+ 1;

    tpNext->tpStroff= stroff;
    tpNext->tpStrlen= ( tpScratch.tpStroff+ tpScratch.tpStrlen )- stroff;

    if  ( pTpPart )
	{ *pTpPart= tpPart;	}
    if  ( pTpNext )
	{ *pTpNext= tpNext;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Add a Particule to a paragraph.					*/
/*									*/
/************************************************************************/

TextParticule *	docInsertParticules(	BufferItem *	paraNode,
					int		part,
					int		count )
    {
    int			i;
    TextParticule *	tp;

    int			allocate= paraNode->biParaParticuleCount+ count+ 1;

    tp= (TextParticule *)realloc( paraNode->biParaParticules,
					allocate * sizeof( TextParticule ) );
    if  ( ! tp )
	{ LLDEB(paraNode->biParaParticuleCount,tp); return tp; }
    paraNode->biParaParticules= tp;

    /*  Move or set terminator */
    if  ( paraNode->biParaParticuleCount > 0 )
	{
	paraNode->biParaParticules[paraNode->biParaParticuleCount+ count]=
			paraNode->biParaParticules[paraNode->biParaParticuleCount];
	}
    else{
	tp= paraNode->biParaParticules+ count;
	docInitTextParticule( tp );
	tp->tpKind= DOCkind_PARA_END;
	}

    /*  Move regular particules if needed */
    if  ( part < 0 )
	{ part= paraNode->biParaParticuleCount;	}
    else{
	int	tail= paraNode->biParaParticuleCount- part;

	tp= paraNode->biParaParticules+ paraNode->biParaParticuleCount+ count- 1;

	for ( i= 0; i < tail; tp--, i++ )
	    { tp[0]= tp[-count];	}
	}

    tp= paraNode->biParaParticules+ part;
    for ( i= 0; i < count; tp++, i++ )
	{
	tp->tpTextAttrNr= -1;

	docInitTextParticule( tp );
	tp->tpKind= DOCkindSPAN;

	paraNode->biParaParticuleCount++;
	}

    return paraNode->biParaParticules+ part;
    }

/************************************************************************/
/*									*/
/*  Insert a text particule.						*/
/*									*/
/************************************************************************/

TextParticule *	docInsertTextParticule(	BufferItem *	paraNode,
					int		part,
					int		off,
					int		len,
					int		kind,
					int		textAttributeNumber )
    {
    const int		count= 1;
    TextParticule *	tp;

    tp= docInsertParticules( paraNode, part, count );
    if  ( ! tp )
	{ XDEB(tp); return tp;	}

    docInitTextParticule( tp );
    tp->tpStroff= off;
    tp->tpStrlen= len;
    tp->tpKind= kind;
    tp->tpTextAttrNr= textAttributeNumber;

    return tp;
    }

/************************************************************************/
/*									*/
/*  Delete a series of particules.					*/
/*									*/
/************************************************************************/

void docDeleteParticules(	BufferItem *	paraNode,
				int		first,
				int		count )
    {
    if  ( first > paraNode->biParaParticuleCount )
	{
	LLDEB(first,paraNode->biParaParticuleCount);
	first= paraNode->biParaParticuleCount;
	}

    if  ( first+ count > paraNode->biParaParticuleCount )
	{
	LLDEB(first+count,paraNode->biParaParticuleCount);
	count= paraNode->biParaParticuleCount- first;
	}

    if  ( count <= 0 )
	{ LDEB(count); return;	}

    paraNode->biParaParticuleCount -= count;

    while( first < paraNode->biParaParticuleCount )
	{
	paraNode->biParaParticules[first]= paraNode->biParaParticules[first+ count];
	first++;
	}

    return;
    }

/************************************************************************/

int docDeleteEmptySpan(		BufferItem *			paraNode )
    {
    if  ( paraNode->biParaParticuleCount == 1			&&
	  paraNode->biParaParticules[0].tpKind == DOCkindSPAN	&&
	  paraNode->biParaParticules[0].tpStrlen == 0		)
	{
	docDeleteParticules( paraNode, 0, 1 );
	return 0;
	}

    return 1;
    }

/************************************************************************/

TextParticule * docMakeSpecialParticule(
				BufferItem *			paraNode,
				int				n,
				int				stroff,
				int				kind,
				int				textAttrNr )
    {
    int			stroffShift= 0;

    char		bytes[8];
    int			count;
    int			sym= '?';

    TextParticule *	tp;

    switch( kind )
	{
	case DOCkindTAB:	sym= ' '; break;
	case DOCkindLINEBREAK:	sym= ' '; break;
	case DOCkindPAGEBREAK:	sym= ' '; break;
	case DOCkindCOLUMNBREAK:sym= ' '; break;

	case DOCkindOPT_HYPH:	sym= '-'; break;
	case DOCkindCHFTNSEP:	sym= '='; break;
	case DOCkindCHFTNSEPC:	sym= '='; break;

	case DOCkindFIELDHEAD:	sym= '{'; break;
	case DOCkindFIELDTAIL:	sym= '}'; break;

	case DOCkindLTR_MARK:	sym= '>'; break;
	case DOCkindRTL_MARK:	sym= '<'; break;

	default:		sym= '@'; break;
	}

    count= uniPutUtf8( bytes, sym );
    if  ( count < 1 )
	{ LDEB(count); return (TextParticule *)0;	}
    bytes[count]= '\0';

    if  ( ! docIsParaNode( paraNode ) )
	{ LDEB(paraNode->biLevel); return (TextParticule *)0;	}

    if  ( docParaStringReplace( &stroffShift, paraNode, stroff, stroff,
						    (char *)bytes, count ) )
	{ LDEB(docParaStrlen(paraNode)); return (TextParticule *)0;	}

    tp= docInsertTextParticule( paraNode, n, stroff, count,
						kind, textAttrNr );
    if  ( ! tp )
	{ LDEB(paraNode->biParaParticuleCount); return (TextParticule *)0; }

    return tp;
    }

/************************************************************************/
/*									*/
/*  Insert a length 1 particule that is used to represent tabs, breaks,	*/
/*  or objects like images in the paragraph and in the string of the	*/
/*  paragraph.								*/
/*									*/
/************************************************************************/

int docSaveSpecialParticule(	BufferDocument *		bd,
				BufferItem *			paraNode,
				const TextAttribute *		ta,
				int				kind )
    {
    int			n= -1;
    int			stroff= docParaStrlen( paraNode );
    int			textAttrNr;

    textAttrNr= docTextAttributeNumber( bd, ta );
    if  ( textAttrNr < 0 )
	{ LDEB(textAttrNr); return -1;	}

    if  ( ! docMakeSpecialParticule( paraNode, n, stroff, kind, textAttrNr ) )
	{ LDEB(paraNode->biParaParticuleCount); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Insert a zero length particule that is used for internal		*/
/*  administration such as delimiting text fields.			*/
/*									*/
/*  Also used to insert the one and only particule in an empty		*/
/*  paragraph.								*/
/*									*/
/*  1)  Paranoia: can be called directly from file readers.		*/
/*									*/
/************************************************************************/

int docInsertAdminParticule(		BufferDocument *	bd,
					BufferItem *		paraNode,
					int			n,
					int			stroff,
					int			objectNumber,
					int			kind,
					const TextAttribute *	ta )
    {
    TextParticule *	tp;
    int			textAttrNr;

    /*  1  */
    if  ( ! bd || ! paraNode )
	{ XXDEB(bd,paraNode); return -1;	}

    textAttrNr= docTextAttributeNumber( bd, ta );
    if  ( textAttrNr < 0 )
	{ LDEB(textAttrNr); return -1;	}

    tp= docMakeSpecialParticule( paraNode, n, stroff, kind, textAttrNr );
    if  ( ! tp )
	{ XDEB(tp); return -1;	}

    tp->tpObjectNumber= objectNumber;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Shift the particules in a paragraph after an insertion or a		*/
/*  deletion.								*/
/*									*/
/************************************************************************/

static void docShiftParticuleOffset(	BufferDocument *	bd,
					const BufferItem *	paraNode,
					TextParticule *		tp,
					int			stroffShift )
    {
    if  ( tp->tpKind == DOCkindFIELDHEAD )
	{
	DocumentField *	df;

	if  ( tp->tpObjectNumber < 0 )
	    { LDEB(tp->tpObjectNumber); docListNode(0,paraNode,0); }
	else{
	    df= docGetFieldByNumber( &(bd->bdFieldList), tp->tpObjectNumber );
	    if  ( ! df )
		{ LXDEB(tp->tpObjectNumber,df);			}
	    else{ df->dfHeadPosition.epStroff += stroffShift;	}
	    }
	}

    if  ( tp->tpKind == DOCkindFIELDTAIL )
	{
	DocumentField *	df;

	if  ( tp->tpObjectNumber < 0 )
	    { LDEB(tp->tpObjectNumber); docListNode(0,paraNode,0); }
	else{
	    df= docGetFieldByNumber( &(bd->bdFieldList), tp->tpObjectNumber );
	    if  ( ! df )
		{ LPDEB(tp->tpObjectNumber,df);			}
	    else{ df->dfTailPosition.epStroff += stroffShift;	}
	    }
	}

    tp->tpStroff += stroffShift;

    return;
    }

int docShiftParticuleOffsets(	BufferDocument *	bd,
				BufferItem *		paraNode,
				int			partFrom,
				int			partUpto,
				int			stroffShift )
    {
    int			part;
    TextParticule *	tp;

    if  ( stroffShift < 0 )
	{
	tp= paraNode->biParaParticules+ partFrom;
	for ( part= partFrom; part < partUpto; tp++, part++ )
	    { docShiftParticuleOffset( bd, paraNode, tp, stroffShift );	}
	}

    if  ( stroffShift > 0 )
	{
	tp= paraNode->biParaParticules+ partUpto- 1;
	for ( part= partUpto- 1; part >= partFrom; tp--, part-- )
	    { docShiftParticuleOffset( bd, paraNode, tp, stroffShift );	}
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Find the particule number for a certain string position.		*/
/*									*/
/*  NOTE: This does not expect the paragraph to be formatted.		*/
/*									*/
/************************************************************************/

int docFindParticuleOfPosition(	int *				pPart,
				int *				pFlags,
				const DocumentPosition *	dp,
				int				lastOne )
    {
    const BufferItem *		paraNode= dp->dpNode;
    const int			stroff= dp->dpStroff;

    int				l= 0;
    int				r= paraNode->biParaParticuleCount;
    int				m= ( l+ r )/ 2;

    const TextParticule *	tp= paraNode->biParaParticules+ m;
    const int			paraStrlen= docParaStrlen( paraNode );

    if  ( r <= 0 )
	{ LDEB(r); return -1;	}
    if  ( stroff < 0 || stroff > paraStrlen )
	{ LDEB(stroff); return -1;	}

    switch( lastOne )
	{
	case PARAfindPAST:
	    if  ( stroff == paraStrlen )
		{
		if  ( pPart )
		    { *pPart= paraNode->biParaParticuleCount;	}

		if  ( pFlags )
		    {
		    *pFlags= POSflagPARA_TAIL;

		    if  ( stroff == 0 )
			{ *pFlags |= POSflagPARA_HEAD; }
		    }
		return 0;
		}

	    /*FALLTHROUGH*/
	case PARAfindLAST:
	    /* Binary search for last: tp->tpStroff <= stroff */
	    while( l < m )
		{
		if  ( tp->tpStroff <= stroff )
		    { l= m;	}
		else{ r= m;	}

		m= ( l+ r )/ 2; tp= paraNode->biParaParticules+ m;
		}

	    if  ( stroff < tp->tpStroff )
		{ m--;	}
	    break;

	case PARAfindFIRST:
	    /* Binary search for first: tp->tpStroff+ tp->tpStrlen >= stroff */
	    while( l < m )
		{
		if  ( tp->tpStroff+ tp->tpStrlen < stroff )
		    { l= m;	}
		else{ r= m;	}

		m= ( l+ r )/ 2; tp= paraNode->biParaParticules+ m;
		}

	    if  ( stroff > tp->tpStroff+ tp->tpStrlen )
		{ m++;	}
	    break;

	default:
	    LDEB(lastOne); return -1;
	}

    if  ( m < 0 || m >= paraNode->biParaParticuleCount )
	{
	const int checkGeometry= 0;

	LLLLDEB(stroff,m,lastOne,paraNode->biParaParticuleCount);
	docListNode( 0, paraNode, checkGeometry );
	return -1;
	}

    if  ( pPart )
	{ *pPart= m;	}

    if  ( pFlags )
	{
	const TextParticule *	tp= paraNode->biParaParticules+ m;

	*pFlags= 0;

	if  ( stroff == tp->tpStroff )
	    { *pFlags |= POSflagPART_HEAD; }
	if  ( stroff == tp->tpStroff+ tp->tpStrlen )
	    { *pFlags |= POSflagPART_TAIL; }

	if  ( stroff == 0 )
	    { *pFlags |= POSflagPARA_HEAD; }
	if  ( stroff == paraStrlen )
	    { *pFlags |= POSflagPARA_TAIL; }

	if  ( m > 0 && stroff == tp->tpStroff )
	    {
	    if  ( tp[-1].tpKind == DOCkindLINEBREAK	||
		  tp[-1].tpKind == DOCkindCOLUMNBREAK	||
		  tp[-1].tpKind == DOCkindPAGEBREAK	)
		{ *pFlags |= POSflagPART_AFTER_BREAK;	}
	    }
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Free a Paragraph level node.					*/
/*									*/
/************************************************************************/

void docCleanParaNode(	BufferDocument *	bd,
			DocumentTree *		dt,
			BufferItem *		paraNode )
    {
    const DocumentFieldList *	dfl= &(bd->bdFieldList);
    const int			fieldCount= dfl->dflPagedList.plItemCount;
    int				i;
    TextParticule *		tp;

    tp= paraNode->biParaParticules;
    for ( i= 0; i < paraNode->biParaParticuleCount; tp++, i++ )
	{
	switch( tp->tpKind )
	    {
	    case DOCkindFIELDHEAD:
	    case DOCkindFIELDTAIL:
		{
		DocumentField *	df;

		if  ( tp->tpObjectNumber < 0 )
		    { continue;	}
		if  ( tp->tpObjectNumber >= fieldCount	)
		    { LLDEB(tp->tpObjectNumber,fieldCount); continue; }

		df= docGetFieldByNumber( &(bd->bdFieldList),
						    tp->tpObjectNumber );
		if  ( df )
		    {
		    if  ( docDeleteFieldFromParent( dt, df ) )
			{ LDEB(df->dfFieldNumber);	}

		    docDeleteFieldFromDocument( bd, df );
		    }
		}
		break;

	    case DOCkindOBJECT:
		{
		InsertedObject *	io;

		io=  docGetObject( bd, tp->tpObjectNumber );
		if  ( ! io )
		    { LPDEB(tp->tpObjectNumber,io);	}
		else{
		    if  ( io->ioKind == DOCokDRAWING_SHAPE )
			{ docDeleteDrawingShape( bd, io->ioDrawingShape ); }
		    }
		}
		break;
	    }
	}

    utilCleanMemoryBuffer( &(paraNode->biParaStringBuffer) );
    if  ( paraNode->biParaParticules )
	{ free( paraNode->biParaParticules );	}
    if  ( paraNode->biParaLines )
	{ free( paraNode->biParaLines );	}

    docCleanParagraphProperties( &(paraNode->biParaProperties) );

    return;
    }

void docInitParaNode(	BufferItem *		paraNode )
    {
    utilInitMemoryBuffer( &(paraNode->biParaStringBuffer) );

    paraNode->biParaParticuleCount= 0;
    paraNode->biParaParticules= (TextParticule *)0;

    paraNode->biParaLineCount= 0;
    paraNode->biParaLines= (TextLine *)0;

    paraNode->biParaMajorityFontAscY0= 0;
    paraNode->biParaMajorityFontDescY1= 0;
    paraNode->biParaMajorityFontSize= 0;
				/********************************/
				/*  Also used to find out	*/
				/*  whether the line extents	*/
				/*  must be recalculated.	*/
				/********************************/

    paraNode->biParaBorderNrAbove= -1;
    paraNode->biParaBorderNrBelow= -1;

    paraNode->biParaTopInset= 0;
    paraNode->biParaBottomInset= 0;

    docInitParagraphProperties( &(paraNode->biParaProperties) );
    }

/************************************************************************/
/*									*/
/*  Intersect a selection (if any) with the paragraph to save.		*/
/*									*/
/*  *pPartFrom is set to the first particule that is relevant to save	*/
/*	the paragraph.							*/
/*  *pPartUpto is set to past the last particule that is relevant to	*/
/*	save the paragraph.						*/
/*  *pStroffFrom is set to the offset that starts the range to save.	*/
/*  *pStroffUpto is set to the offset that ends the range to save.	*/
/*									*/
/************************************************************************/

int docIntersectSelectionWithParagraph(
				    DocumentSelection *		pDsPara,
				    int *			pPartFrom,
				    int *			pPartUpto,
				    int *			pHeadFlags,
				    int *			pTailFlags,
				    const BufferItem *		paraNode,
				    const DocumentSelection *	ds )
    {
    int			partFrom= 0;
    int			partUpto= paraNode->biParaParticuleCount;
    int			headFlags= 0;
    int			tailFlags= 0;

    DocumentSelection	dsPara;

    if  ( docHeadPosition( &(dsPara.dsHead), (BufferItem *)paraNode ) )
	{ LDEB(1); return -1;	}
    if  ( docTailPosition( &(dsPara.dsTail), (BufferItem *)paraNode ) )
	{ LDEB(1); return -1;	}

    if  ( ds )
	{
	if  ( docComparePositions( &(ds->dsTail), &(dsPara.dsHead) ) < 0 )
	    { return 1;	}

	if  ( docComparePositions( &(dsPara.dsTail), &(ds->dsHead) ) < 0 )
	    { return 1;	}

	if  ( ds->dsHead.dpNode == paraNode )
	    {
	    dsPara.dsHead.dpStroff= ds->dsHead.dpStroff;

	    if  ( docFindParticuleOfPosition( &partFrom, &headFlags,
						&(ds->dsHead), PARAfindLAST ) )
		{ LDEB(dsPara.dsHead.dpStroff); return -1;	}

	    headFlags |= POSflagPARA_FOUND;
	    }
	else{
	    headFlags= POSflagPARA_HEAD|POSflagPART_HEAD;
	    }

	if  ( ds->dsTail.dpNode == paraNode )
	    {
	    dsPara.dsTail.dpStroff= ds->dsTail.dpStroff;

	    if  ( docFindParticuleOfPosition( &partUpto, &tailFlags,
						&(ds->dsTail), PARAfindPAST ) )
		{ LDEB(dsPara.dsTail.dpStroff); return -1;	}

	    tailFlags |= POSflagPARA_FOUND;
	    }
	else{
	    tailFlags= POSflagPARA_TAIL|POSflagPART_TAIL;
	    }
	}
    else{
	headFlags= POSflagPARA_HEAD|POSflagPART_HEAD;
	tailFlags= POSflagPARA_TAIL|POSflagPART_TAIL;
	}

    if  ( pDsPara )
	{
	const int	direction= 1;

	docSetRangeSelection( pDsPara,
			    &(dsPara.dsHead), &(dsPara.dsTail), direction );
	}

    if  ( pHeadFlags )
	{ *pHeadFlags= headFlags;	}
    if  ( pTailFlags )
	{ *pTailFlags= tailFlags;	}

    if  ( pPartFrom )
	{ *pPartFrom= partFrom;	}
    if  ( pPartUpto )
	{ *pPartUpto= partUpto;	}

    return 0;
    }

