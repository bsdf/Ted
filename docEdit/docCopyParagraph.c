/************************************************************************/
/*									*/
/*  Buffer administration routines relating to the text particules in a	*/
/*  text paragraph.							*/
/*									*/
/************************************************************************/

#   include	"docEditConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>

#   include	<appDebugon.h>

#   include	<docBuf.h>
#   include	<docField.h>
#   include	<docNodeTree.h>
#   include	"docCopyNode.h"
#   include	"docEdit.h"
#   include	<docParaString.h>
#   include	<docParaParticules.h>
#   include	"docDocumentCopyJob.h"
#   include	<docHyperlinkField.h>
#   include	<docBookmarkField.h>
#   include	<docObjectProperties.h>

/*
# define DEB_PARTICULES
*/

/************************************************************************/
/*									*/
/*  Enter a field while copying. This assumes that copying is done in	*/
/*  document order.							*/
/*									*/
/************************************************************************/

static DocumentField * docStartFieldCopy(
				DocumentCopyJob *		dcj,
				const EditPosition *		epStart,
				int				obnrFrom )
    {
    EditOperation *		eo= dcj->dcjEditOperation;
    BufferDocument *		bdTo= eo->eoDocument;
    const DocumentFieldList *	dflFrom= &(dcj->dcjSourceDocument->bdFieldList);
    const int			fieldCount= dflFrom->dflPagedList.plItemCount;

    DocumentField *		dfFrom;
    DocumentField *		dfTo;

    dfFrom= docGetFieldByNumber( dflFrom, obnrFrom );

    dfTo= docClaimFieldCopy( &(bdTo->bdFieldList), dfFrom,
					&(dcj->dcjTargetSelectionScope), epStart );
    if  ( ! dfTo )
	{ XDEB(dfTo); return (DocumentField *)0;	}

    if  ( obnrFrom < 0 || obnrFrom >= fieldCount )
	{ LDEB(obnrFrom); }
    else{
	dcj->dcjFieldMap[obnrFrom]= dfTo->dfFieldNumber;
	}

    if  ( docPushFieldOnCopyStack( dcj, dfTo ) )
	{ LDEB(1); return (DocumentField *)0;	}

    if  ( dfTo->dfKind >= DOC_FieldKindCount	)
	{ LDEB(dfTo->dfKind);	}
    else{
	eo->eoFieldUpdate |=
		DOC_FieldKinds[dfTo->dfKind].fkiCalculateWhen;
	}

    if  ( dfTo->dfKind == DOCfkHYPERLINK			&&
	  ! utilMemoryBufferIsEmpty( &(dcj->dcjRefFileName) )	)
	{
	docMakeHyperlinkRelative( dfTo, &(dcj->dcjRefFileName) );
	}

    if  ( dfTo->dfKind == DOCfkBOOKMARK )
	{
	const MemoryBuffer *	mbFrom;
	MemoryBuffer		mbTo;

	utilInitMemoryBuffer( &mbTo );

	if  ( docFieldGetBookmark( &mbFrom, dfFrom ) )
	    { utilMemoryBufferSetString( &mbTo, "x" );	}
	else{ utilCopyMemoryBuffer( &mbTo, mbFrom );	}

	if  ( docMakeBookmarkUnique( bdTo, &mbTo ) )
	    { LDEB(1); return (DocumentField *)0;	}

	if  ( docSetBookmarkField( &(dfTo->dfInstructions), &mbTo ) )
	    { LDEB(1); return (DocumentField *)0;	}

	utilCleanMemoryBuffer( &mbTo );
	}

    if  ( docFieldHasNote( dfFrom->dfKind ) && dfFrom->dfNoteIndex >= 0 )
	{
	if  ( docCopyNote( dcj, dfTo, dfFrom ) )
	    { LDEB(1); return (DocumentField *)0;	}

	utilIndexSetAdd( &(dcj->dcjNoteFieldsCopied), dfTo->dfFieldNumber );
	}

    return dfTo;
    }

/************************************************************************/
/*									*/
/*  Leave a field while copying. This assumes that copying is done in	*/
/*  document order.							*/
/*									*/
/************************************************************************/

static int docFinishFieldCopy(	DocumentCopyJob *		dcj,
				const EditPosition *		epTail,
				int				obnrFrom )
    {
    EditOperation *		eo= dcj->dcjEditOperation;
    BufferDocument *		bdTo= eo->eoDocument;
    const DocumentFieldList *	dflFrom= &(dcj->dcjSourceDocument->bdFieldList);
    const int			fieldCount= dflFrom->dflPagedList.plItemCount;

    DocumentField *		dfTo;
    int				obnrTo;

    FieldCopyStackLevel *	fcsl= dcj->dcjFieldStack;

    if  ( obnrFrom < 0				||
	  obnrFrom >= fieldCount	)
	{ LLDEB(obnrFrom,fieldCount); return -1;	}

    if  ( dcj->dcjFieldMap[obnrFrom] < 0 )
	{
	LLDEB(obnrFrom,fieldCount);
	SDEB(docFieldKindStr(docGetFieldKindByNumber(dflFrom,obnrFrom)));
	return -1;
	}

    obnrTo= dcj->dcjFieldMap[obnrFrom];
    dfTo= docGetFieldByNumber( &(bdTo->bdFieldList), obnrTo );
    docSetFieldTail( dfTo, epTail );

    if  ( ! fcsl )
	{ XDEB(fcsl);	}
    else{
	if  ( fcsl->fcslPrev )
	    {
	    if  ( docAddChildToField( fcsl->fcslField,
						fcsl->fcslPrev->fcslField ) )
		{ LDEB(1); return -1;	}
	    }
	else{
	    if  ( docAddRootFieldToTree( dcj->dcjTargetTree,
							    fcsl->fcslField ) )
		{ LDEB(1); return -1;	}
	    }

	dcj->dcjFieldStack= fcsl->fcslPrev;
	free( fcsl );
	}

    return obnrTo;
    }

/************************************************************************/
/*									*/
/*  Copy particule data from one paragraph to another.			*/
/*									*/
/*  9)  Not needed, the document is marked as changed, so this can wait	*/
/*	until it is saved.						*/
/*									*/
/************************************************************************/

static int docCopyParticuleData(	DocumentCopyJob *	dcj,
					BufferItem *		biTo,
					const BufferItem *	biFrom,
					TextParticule *		tpTo,
					const TextParticule *	tpFrom )
    {
    EditOperation *		eo= dcj->dcjEditOperation;
    BufferDocument *		bdTo= eo->eoDocument;
    int				paraNr= docNumberOfParagraph( biTo );

    switch( tpFrom->tpKind )
	{
	case DOCkindSPAN:
	case DOCkindTAB:
	case DOCkindLINEBREAK:
	case DOCkindPAGEBREAK:
	case DOCkindCOLUMNBREAK:
	    break;

	case DOCkindOBJECT:
	    {
	    const InsertedObject *	ioFrom;
	    InsertedObject *		ioTo;
	    int				nr;

	    ioFrom= docGetObject( dcj->dcjSourceDocument,
						    tpFrom->tpObjectNumber );
	    if  ( ! ioFrom )
		{ LPDEB(tpFrom->tpObjectNumber,ioFrom); return -1;	}

	    ioTo= docClaimObjectCopy( bdTo, &nr, ioFrom );
	    if  ( ! ioTo )
		{ XDEB(ioTo); return -1;	}
	    tpTo->tpObjectNumber= nr;

	    ioFrom= docGetObject( dcj->dcjSourceDocument,
						    tpFrom->tpObjectNumber );
	    if  ( ! ioFrom )
		{ LPDEB(tpFrom->tpObjectNumber,ioFrom); return -1;	}

	    if  ( ioFrom->ioKind == DOCokDRAWING_SHAPE )
		{
		if  ( ! ioFrom->ioDrawingShape )
		    { XDEB(ioFrom->ioDrawingShape);	}
		else{
		    ioTo->ioDrawingShape= docCopyDrawingShape( dcj,
						    ioFrom->ioDrawingShape );
		    if  ( ! ioTo->ioDrawingShape )
			{ XDEB(ioTo->ioDrawingShape);	}
		    }
		}

	    /*  9
	    if  ( ioTo->ioBliptag == 0 )
		{ ioTo->ioBliptag= appGetTimestamp();	}
	    */
	    }
	    break;

	case DOCkindFIELDTAIL:
	    {
	    EditPosition		ep;

	    ep.epParaNr= paraNr;
	    ep.epStroff= tpTo->tpStroff;

	    if  ( dcj->dcjCopyFields )
		{
		tpTo->tpObjectNumber= docFinishFieldCopy( dcj, &ep,
						tpFrom->tpObjectNumber );
		}
	    else{ tpTo->tpObjectNumber= tpFrom->tpObjectNumber;	}
	    if  ( tpTo->tpObjectNumber < 0 )
		{ LDEB(tpTo->tpObjectNumber);	}
	    }
	    break;

	case DOCkindFIELDHEAD:
	    if  ( dcj->dcjCopyFields )
		{
		EditPosition		ep;
		DocumentField *		dfTo;

		ep.epParaNr= paraNr;
		ep.epStroff= tpTo->tpStroff;

		dfTo= docStartFieldCopy( dcj, &ep, tpFrom->tpObjectNumber );
		if  ( ! dfTo )
		    { XDEB(dfTo); return -1;	}
		tpTo->tpObjectNumber= dfTo->dfFieldNumber;
		}
	    else{ tpTo->tpObjectNumber= tpFrom->tpObjectNumber;	}
	    break;

	default:
	    LDEB(tpFrom->tpKind); return -1;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Copy particules plus contents from one paragraph to another.	*/
/*									*/
/*  This might be a copy to a different document, so font numbers and	*/
/*  attribute numbers need to be recalculated.				*/
/*									*/
/*  1)  Find out where to insert.					*/
/*	(At the end of the paragraph or before particule[partTo].	*/
/*  2)  Find position and length of the source string.			*/
/*  3)  Find out whether to replace the empty particule that we use to	*/
/*	have at least one particule in a paragraph.			*/
/*  4)  Insert the desired number of particules in the correct		*/
/*	position. Note that that is one less in a paragraph that was	*/
/*	originally empty.						*/
/*  5)  Insert the new text into the paragraph string.			*/
/*  6)  Shift the particules in the tail of the target paragraph to	*/
/*	make place for the new text string bytes. The possible		*/
/*	overwritten 'empty' particule should not be shifted.		*/
/*  7)  Patch the particules in the hole created above to correct	*/
/*	values derived from the source particules.			*/
/*									*/
/************************************************************************/

int docCopyParticules(	DocumentCopyJob *		dcj,
			BufferItem *			biTo,
			const BufferItem *		biFrom,
			int				partTo,
			int				partFrom,
			int				countFrom,
			int *				pParticulesInserted,
			int *				pCharactersCopied )
    {
    EditOperation *		eo= dcj->dcjEditOperation;

    TextParticule *		tpTo;
    const TextParticule *	tpFrom;

    int				stroffTo;
    int				stroffShift;

    int				i;

    int				replaceEmpty= 0;

    int				stroffFrom;
    int				strlenFrom;

    int				paraNrTo= docNumberOfParagraph( biTo );

    /*  1  */
    if  ( partTo > biTo->biParaParticuleCount )
	{
	LLDEB(partTo,biTo->biParaParticuleCount);
	partTo= biTo->biParaParticuleCount;
	}

    if  ( partTo == biTo->biParaParticuleCount )
	{ stroffTo= docParaStrlen( biTo );				}
    else{ stroffTo= biTo->biParaParticules[partTo].tpStroff;	}

    /*  2  */
    tpFrom= biFrom->biParaParticules+ partFrom;
    stroffFrom= tpFrom->tpStroff;
    strlenFrom= tpFrom[countFrom- 1].tpStroff+ tpFrom[countFrom- 1].tpStrlen-
							    tpFrom->tpStroff;

    /*  3  */
    if  ( docParaStrlen( biTo ) == 0 && biTo->biParaParticuleCount == 1 )
	{
	if  ( partTo < 0 || partTo > 1 )
	    { LDEB(partTo);	}
	if  ( partTo != 0 )
	    { partTo= 0;	}

	replaceEmpty= 1;
	}

    /*  4  */
    tpTo= docInsertParticules( biTo, partTo, countFrom- replaceEmpty );
    if  ( ! tpTo )
	{ XDEB(tpTo); return -1;	}

    /*  5  */
    if  ( docParaStringReplace( &stroffShift, biTo, stroffTo, stroffTo,
		    (char *)docParaString( biFrom, stroffFrom ), strlenFrom ) )
	{
	LDEB(strlenFrom);
	docDeleteParticules( biTo, partTo, countFrom- replaceEmpty );
	return -1;
	}

    /*  6  */
    if  ( docEditShiftParticuleOffsets( eo, biTo, paraNrTo,
						partTo+ countFrom,
						biTo->biParaParticuleCount,
						stroffTo, stroffShift ) )
	{ LDEB(stroffShift);	}

    /*  7  */
    tpTo= biTo->biParaParticules+ partTo;
    for ( i= 0; i < countFrom; tpTo++, tpFrom++, i++ )
	{
	int		textAttributeNumberTo;

	textAttributeNumberTo= docMapTextAttributeNumber( dcj,
							tpFrom->tpTextAttrNr );
	if  ( textAttributeNumberTo < 0 )
	    { LDEB(textAttributeNumberTo); return -1;	}

	tpTo->tpStroff= stroffTo;
	tpTo->tpStrlen= tpFrom->tpStrlen;
	tpTo->tpKind= tpFrom->tpKind;
	tpTo->tpTextAttrNr= textAttributeNumberTo;

	if  ( docCopyParticuleData( dcj, biTo, biFrom, tpTo, tpFrom ) )
	    { LLDEB(partTo,i); return -1;	}

	stroffTo += tpTo->tpStrlen;
	}

    *pParticulesInserted += countFrom- replaceEmpty;
    *pCharactersCopied += stroffShift;

    return 0;
    }

int docCopyParticuleAndData(		TextParticule **	pTpTo,
					DocumentCopyJob *	dcj,
					BufferItem *		paraBiTo,
					int			partTo,
					int			stroffTo,
					int			strlenTo,
					const BufferItem *	paraBiFrom,
					const TextParticule *	tpFrom )
    {
    TextParticule	tpSaved;
    int			textAttributeNumberTo;

    TextParticule *	tpTo;

    tpSaved= *tpFrom;

    textAttributeNumberTo= docMapTextAttributeNumber( dcj,
						    tpSaved.tpTextAttrNr );

    if  ( textAttributeNumberTo < 0 )
	{ LDEB(textAttributeNumberTo); return -1;	}

    if  ( partTo < 0 )
	{ partTo= paraBiTo->biParaParticuleCount;	}

    tpTo= docInsertTextParticule( paraBiTo, partTo,
					    stroffTo, strlenTo,
					    tpSaved.tpKind,
					    textAttributeNumberTo );
    if  ( ! tpTo )
	{ LXDEB(partTo,tpTo); return -1;	}

    if  ( docCopyParticuleData( dcj, paraBiTo, paraBiFrom, tpTo, &tpSaved ) )
	{
	docDeleteParticules( paraBiTo, partTo, 1 );
	LDEB(partTo); return -1;
	}

    *pTpTo= tpTo;
    return 0;
    }

/************************************************************************/
/*									*/
/*  Make sure the a paragraph does not end in an explicit break.	*/
/*									*/
/************************************************************************/

int docCheckNoBreakAsLast(	EditOperation *		eo,
				BufferItem *		paraBi )
    {
    const int			part= paraBi->biParaParticuleCount;
    const int			stroff= docParaStrlen( paraBi );
    const int			len= 0;

    const TextParticule *	tp= paraBi->biParaParticules+ part- 1;

    if  ( tp->tpKind != DOCkindLINEBREAK	&&
	  tp->tpKind != DOCkindPAGEBREAK	&&
	  tp->tpKind != DOCkindCOLUMNBREAK	)
	{ return 0;	}

    tp= docInsertTextParticule( paraBi, part, stroff, len,
				    DOCkindSPAN, tp->tpTextAttrNr );
    if  ( ! tp )
	{ XDEB(tp); return -1;	}

    docEditIncludeNodeInReformatRange( eo, paraBi );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Copy a paragraph to make a new one.					*/
/*									*/
/*  1)  Copy paragraph item.						*/
/*  2)  Open a hole in references to the item to possibly receive some	*/
/*	to the fresh item.						*/
/*  3)  Do not copy a possible list text field from the original.	*/
/*  4)  Copy the contents of the original or insert a dummy particule.	*/
/*  5)  Finally adapt the properties of the target paragraph to those	*/
/*	of the original.						*/
/*									*/
/************************************************************************/

BufferItem * docCopyParaNode(		DocumentCopyJob *	dcj,
					const SelectionScope *	ssRoot,
					BufferItem *		biCellTo,
					int			n,
					const BufferItem *	biParaFrom )
    {
    EditOperation *	eo= dcj->dcjEditOperation;
    BufferDocument *	bdTo= eo->eoDocument;
    BufferItem *	biParaTo;

    const int		partTo= 0;
    int			partFrom= 0;

    PropertyMask	ppChgMask;
    PropertyMask	ppUpdMask;

    /*  1  */
    biParaTo= docInsertNode( bdTo, biCellTo, n, DOClevPARA );
    if  ( ! biParaTo )
	{ XDEB(biParaTo); return biParaTo;	}

    docSetParaTableNesting( biParaTo );

    /*  2  */
    {
    int		paraNr;
    const int	stroff= 0;
    const int	sectShift= 0;
    const int	paraShift= 1;

    paraNr= docNumberOfParagraph( biParaTo );

    docEditShiftReferences( eo, ssRoot, paraNr, stroff,
					sectShift, paraShift, -stroff );
    }

    /*  3  */
    if  ( biParaFrom->biParaListOverride > 0 )
	{
	DocumentField *		dfHead= (DocumentField *)0;
	DocumentSelection	dsInsideHead;
	DocumentSelection	dsAroundHead;
	int			bulletPartBegin= -1;
	int			bulletPartEnd= -1;

	if  ( docDelimitParaHeadField( &dfHead, &dsInsideHead, &dsAroundHead,
					&bulletPartBegin, &bulletPartEnd,
					biParaFrom, dcj->dcjSourceDocument ) )
	    { LDEB(1);	}

	if  ( partFrom <= bulletPartEnd )
	    { partFrom= bulletPartEnd+ 1;	}
	}

    /*  4  */
    if  ( partFrom < biParaFrom->biParaParticuleCount )
	{
	int		particulesInserted= 0;
	int		charactersCopied= 0;

	if  ( docCopyParticules( dcj, biParaTo, biParaFrom, partTo,
			partFrom, biParaFrom->biParaParticuleCount- partFrom,
			&particulesInserted, &charactersCopied ) )
	    {
	    LDEB(biParaFrom->biParaParticuleCount);
	    docDeleteNode( eo->eoDocument, eo->eoTree, biParaTo );
	    return (BufferItem *)0;
	    }
	}
    else{
	int			textAttributeNumberTo;
	const TextParticule *	tpFrom;

	tpFrom= biParaFrom->biParaParticules+
				    biParaFrom->biParaParticuleCount- 1;

	textAttributeNumberTo= docMapTextAttributeNumber( dcj,
							tpFrom->tpTextAttrNr );
	if  ( textAttributeNumberTo < 0 )
	    { LDEB(textAttributeNumberTo); return (BufferItem *)0;	}

	if  ( ! docInsertTextParticule( biParaTo, 0, 0, 0,
					DOCkindSPAN, textAttributeNumberTo ) )
	    { LDEB(1); return (BufferItem *)0;	}
	}

    /*  5  */
    utilPropMaskClear( &ppChgMask );

    utilPropMaskClear( &ppUpdMask );
    utilPropMaskFill( &ppUpdMask, PPprop_FULL_COUNT );
    PROPmaskUNSET( &ppUpdMask, PPpropTABLE_NESTING );

    if  ( docEditUpdParaProperties( eo, &ppChgMask, biParaTo, &ppUpdMask,
					&(biParaFrom->biParaProperties),
					&(dcj->dcjAttributeMap) ) )
	{ LDEB(1); return (BufferItem *)0;	}

    eo->eoParagraphsInserted++;
    if  ( biParaTo->biParaListOverride > 0 )
	{ dcj->dcjBulletsCopied++;	}

    return biParaTo;
    }

