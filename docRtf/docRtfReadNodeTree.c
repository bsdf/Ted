/************************************************************************/
/*									*/
/*  Manage documner node hierarchy from the rtf parser.			*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<stdio.h>
#   include	<ctype.h>

#   include	<appDebugon.h>

#   include	"docRtfReaderImpl.h"
#   include	<docTreeType.h>
#   include	<docNodeTree.h>
#   include	<docParaParticules.h>

/************************************************************************/
/*									*/
/*  Apply the row properties that have been collected to a row.		*/
/*									*/
/************************************************************************/

static int docRtfSetRowProperties(	RtfReader *		rr,
					BufferItem *		rowNode )
    {
    PropertyMask		rpDoneMask;

    rr->rrcRowProperties.rpShadingNumber= docItemShadingNumber(
				rr->rrDocument, &(rr->rrcRowShading) );

    rr->rrcRowProperties.rpFrameNumber= docFramePropertiesNumber(
			    rr->rrDocument, &(rr->rrcRowFrameProperties) );

    utilPropMaskClear( &rpDoneMask );

    if  ( docChangeRowProperties( &rpDoneMask, rowNode,
					    &(rr->rrcRowPropertyMask),
					    &(rr->rrcRowProperties) ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Adjust the level in the hierarchy for docRtfReadGroup().		*/
/*									*/
/*  1)  Empty paragraphs get one empty particule to have a height.	*/
/*									*/
/************************************************************************/

static int docRtfCheckParagraph(	RtfReader *		rr,
					BufferItem *		paraNode )
    {
    int			addEmptyParticule= 0;

    if  ( paraNode->biParaParticuleCount == 0 )
	{ addEmptyParticule= 1;	}
    else{
	const TextParticule *	tp;

	tp= paraNode->biParaParticules+ paraNode->biParaParticuleCount- 1;

	if  ( tp->tpKind == DOCkindLINEBREAK	||
	      tp->tpKind == DOCkindPAGEBREAK	||
	      tp->tpKind == DOCkindCOLUMNBREAK	)
	    { addEmptyParticule= 1;	}

	if  ( tp->tpKind == DOCkindFIELDTAIL )
	    {
	    const DocumentFieldList *	dfl= &(rr->rrDocument->bdFieldList);
	    const DocumentField *	df;

	    df= docGetFieldByNumber( dfl, tp->tpObjectNumber );
	    if  ( df->dfKind == DOCfkCHFTN )
		{ addEmptyParticule= 1;	}
	    }
	}

    if  ( addEmptyParticule )
	{
	RtfReadingState *	rrs= rr->rrcState;

	const int		part= paraNode->biParaParticuleCount;
	const int		stroff= docParaStrlen( paraNode );
	const int		count= 0;

	TextAttribute		ta;
	int			textAttrNr;

	docPlainTextAttribute( &ta, rr->rrDocument );

	if  ( rrs )
	    {
	    if  ( rrs->rrsTextShadingChanged )
		{ docRtfRefreshTextShading( rr, rrs );	}

	    ta= rrs->rrsTextAttribute;
	    }

	textAttrNr= docTextAttributeNumber( rr->rrDocument, &ta );
	if  ( textAttrNr < 0 )
	    { LDEB(textAttrNr); return -1;	}

	if  ( ! docInsertTextParticule( paraNode, part,
				    stroff, count, DOCkindSPAN, textAttrNr ) )
	    { LDEB(1); return -1;	}

	rr->rrcAfterNoteref= 0;
	rr->rrAfterParaHeadField= 0;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Finish a section in the document.					*/
/*									*/
/************************************************************************/

static int docRtfCloseSect(	RtfReader *		rr,
				BufferItem *		sectNode )
    {
    if  ( sectNode->biTreeType == DOCinBODY )
	{
	PropertyMask		spUpdMask;

	utilPropMaskClear( &spUpdMask );
	utilPropMaskFill( &spUpdMask, SPprop_COUNT );

	/*  Not clear if this is desirable  */
	if  ( docUpdSectProperties( (PropertyMask *)0,
				&(sectNode->biSectProperties),
				&spUpdMask, &(rr->rrcSectionProperties) ) )
	    { LDEB(1); return -1;	}
	}

    docCleanRowProperties( &(rr->rrcRowProperties) );
    docInitRowProperties( &(rr->rrcRowProperties) );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Finish a row in the document.					*/
/*									*/
/*  In this order: (1) can make that (2) is needed.			*/
/*  1)  To avoid crashes, make sure that the row does not have more	*/
/*	children than columns: Split it until this is the case.		*/
/*  2)  To avoid crashes, make sure that the row does not have more	*/
/*	columns than children: Remove the surplus.			*/
/*									*/
/************************************************************************/

static int docRtfCloseRow(	RtfReader *		rr,
				BufferItem *		rowNode )
    {
    if  ( rowNode->biRowForTable )
	{
	if  ( docRtfSetRowProperties( rr, rowNode ) )
	    { LDEB(1); return -1;	}
	}
    else{
	docCleanRowProperties( &(rowNode->biRowProperties) );
	docInitRowProperties( &(rowNode->biRowProperties) );
	}

    if  ( docIsRowNode( rowNode )				&&
	  rowNode->biRowCellCount != rowNode->biChildCount	)
	{
	LLDEB(rowNode->biRowCellCount,rowNode->biChildCount);
	/*docListNode(0,rowNode,0);*/

	/*  1  */
	while( rowNode->biRowCellCount < rowNode->biChildCount )
	    {
	    CellProperties	cp;

	    const DocumentAttributeMap * const dam= (const DocumentAttributeMap *)0;

	    docInitCellProperties( &cp );
	    docCopyCellProperties( &cp,
		    &(rowNode->biRowCells[rowNode->biRowCellCount-1]), dam );

	    /* Add zero width dummy columns to avoid crashes */
	    while( rowNode->biRowCellCount < rowNode->biChildCount )
		{
		const int	shiftTail= 0;

		if  ( docInsertRowColumn( &(rowNode->biRowProperties),
				rowNode->biRowCellCount, shiftTail, &cp, dam ) )
		    { LDEB(rowNode->biRowCellCount); return -1;	}
		}

	    docCleanCellProperties( &cp );
	    }

	/*  2  */
	if  ( rowNode->biRowCellCount > rowNode->biChildCount )
	    {
	    const int	shiftTail= 0; /* irrelevant at end */

	    docDeleteColumnsFromRow( &(rowNode->biRowProperties),
			    rowNode->biChildCount,
			    rowNode->biRowCellCount- rowNode->biChildCount,
			    shiftTail );
	    }
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Close a cell.							*/
/*									*/
/************************************************************************/

static int docRtfCloseCell(	RtfReader *		rr,
				BufferItem *		cellNode )
    {
    if  ( cellNode->biChildCount == 0 )
	{ LDEB(cellNode->biChildCount); return 0;	}

    return 0;
    }

/************************************************************************/

void docRtfSetForRow(	BufferItem *	node )
    {
    node= docGetRowLevelNode( node );
    while( node )
	{
	if  ( ! node->biRowForTable )
	    { node->biRowForTable= 1;	}

	node= docGetRowLevelNode( node->biParent );
	}
    }

/************************************************************************/
/*									*/
/*  Close a paragraph in the document.					*/
/*									*/
/*  1)  Give up on syntactically incorrect rtf files (more '}' than	*/
/*	'{') in the hope that is we avoid the crash, the user can	*/
/*	decide what to do.						*/
/*									*/
/************************************************************************/

static int docRtfCloseParagraph(	RtfReader *	rr,
					BufferItem *	paraNode )
    {
    RtfReadingState *		rrs= rr->rrcState;
    BufferDocument *		bd= rr->rrDocument;
    const DocumentProperties *	dp= &(bd->bdProperties);

    int				paraNr;
    int				textAttributeNumber;

    ListNumberTreeNode *	root;

    /* 1  */
    if  ( ! rrs )
	{ return 0;	}

    if  ( docRtfCheckParagraph( rr, paraNode ) )
	{ LDEB(1); return -1;	}

    textAttributeNumber= paraNode->biParaParticules->tpTextAttrNr;

    paraNr= docNumberOfParagraph( paraNode );

    if  ( docRtfPopParaFromFieldStack( rr, paraNr ) )
	{ LDEB(1); return -1;	}

    if  ( docRtfAdaptToParaProperties( paraNode, bd, rrs,
					    rr->rrParagraphBreakOverride ) )
	{ LDEB(1); return -1;	}

    rr->rrParagraphBreakOverride= -1;

    if  ( paraNode->biParaListOverride > 0 )
	{
	DocumentField *			dfHead= (DocumentField *)0;
	DocumentSelection		dsInsideHead;
	DocumentSelection		dsAroundHead;
	int				partBegin= -1;
	int				partEnd= -1;

	const ListOverrideTable *	lot;

	lot= &(dp->dpListAdmin->laListOverrideTable);

	if  ( paraNode->biParaListOverride >= lot->lotOverrideCount )
	    {
	    LLDEB(paraNode->biParaListOverride,lot->lotOverrideCount);
	    return -1;
	    }

	if  ( docDelimitParaHeadField( &dfHead, &dsInsideHead, &dsAroundHead,
					&partBegin, &partEnd, paraNode, bd ) )
	    {
	    if  ( docInsertParaHeadField( &dfHead, &dsInsideHead, &dsAroundHead,
			&partBegin, &partEnd, paraNode, bd, rr->rrcTree,
			DOCfkLISTTEXT, textAttributeNumber ) )
		{
		LDEB(paraNode->biParaListOverride); return -1;
		}
	    }

	root= docGetListNumberTree( &(rr->rrcTree->dtListNumberTrees),
						paraNode->biParaListOverride );
	if  ( ! root )
	    { LXDEB(paraNode->biParaListOverride,root); return -1;	}

	if  ( docListNumberTreeInsertParagraph( root,
				    paraNode->biParaListLevel, paraNr ) )
	    { LDEB(paraNr);	}
	}

    if  ( paraNode->biParaOutlineLevel < PPoutlineBODYTEXT )
	{
	if  ( docListNumberTreeInsertParagraph(
				    &(rr->rrcTree->dtOutlineTree),
				    paraNode->biParaOutlineLevel, paraNr ) )
	    { LDEB(paraNr);	}
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Close a node.							*/
/*									*/
/************************************************************************/

static int docRtfCloseNode(	RtfReader *		rr,
				BufferItem *		node )
    {
    switch( node->biLevel )
	{
	case DOClevSECT:
	    if  ( docRtfCloseSect( rr, node ) )
		{ LDEB(1); return -1;	}
	    break;

	case DOClevROW:
	    if  ( docRtfCloseRow( rr, node ) )
		{ LDEB(1); return -1;	}
	    break;

	case DOClevCELL:
	    if  ( docRtfCloseCell( rr, node ) )
		{ LDEB(1); return -1;	}
	    break;

	case DOClevPARA:
	    if  ( docRtfCloseParagraph( rr, node ) )
		{ LDEB(1); return -1;	}
	    break;

	default:
	    break;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Pop a node from the 'stack' of nodes that is built by the parser.	*/
/*									*/
/************************************************************************/

static int docRtfPopNode(		RtfReader *	rr,
					BufferItem *	node )
    {
    while( rr->rrcNode )
	{
	if  ( docRtfCloseNode( rr, rr->rrcNode ) )
	    { LDEB(1); return -1;	}

	if  ( rr->rrcNode == node )
	    {
	    rr->rrcNode= rr->rrcNode->biParent;
	    rr->rrcLevel= rr->rrcNode->biLevel;
	    return 0;
	    }

	rr->rrcNode= rr->rrcNode->biParent;
	}

    XDEB(rr->rrcNode); return -1;
    }

/************************************************************************/
/*									*/
/*  Handle cell tags that relate to the document hierarchy.		*/
/*									*/
/************************************************************************/

static int docRtfHierarchyCell(	const RtfControlWord *	rcw,
				RtfReader *		rr )
    {
    BufferItem *	levelNode= (BufferItem *)0;
    int			paraInCell= 1;

    /*  1  */
    if  ( rr->rrcNode )
	{ levelNode= docGetCellNode( rr->rrcNode );	}

    if ( rcw->rcwID == DOClevCELL		&&
	 rr->rrcNode				&&
	 rr->rrcNode->biLevel == DOClevCELL	)
	{
	int	cc= rr->rrcNode->biChildCount;

	if  ( cc > 0						  &&
	      rr->rrcNode->biChildren[cc-1]->biLevel != DOClevPARA )
	      { paraInCell= 0;	}
	}

    if  ( ! levelNode						||
	  ( rr->rrcNode->biLevel <= DOClevCELL && paraInCell )	)
	{
	BufferItem *	paraNode= docRtfGetParaNode( rr );

	if  ( ! paraNode )
	    { SDEB(rcw->rcwWord); return -1; }

	/* cope with: {\intbl etc\par}\cell */
	if  ( paraNode->biParaTableNesting == 0 )
	    {
	    docSetParaTableNesting( paraNode );
	    docRtfSetForRow( paraNode );
	    if  ( rr->rrcState )
		{
		rr->rrcState->rrsParagraphProperties.ppTableNesting= 
					    paraNode->biParaTableNesting;
		}
	    }
	}
    if  ( ! rr->rrcNode )
	{ SXDEB(rcw->rcwWord,rr->rrcNode); return -1;	}

    /* MS-Word does this: The first clue about a table is \cell */
    if  ( rr->rrcNode->biLevel == DOClevPARA	&&
	  rr->rrcNode->biParaTableNesting == 0	)
	{
	/*LDEB(rr->rrcNode->biParaTableNesting);*/
	rr->rrcNode->biParaTableNesting= 1;
	docRtfSetForRow( rr->rrcNode );
	if  ( rr->rrcState )
	    {
	    rr->rrcState->rrsParagraphProperties.ppTableNesting= 
					rr->rrcNode->biParaTableNesting;
	    }
	}

    levelNode= docGetCellNode( rr->rrcNode );
    if  ( ! levelNode )
	{ SXDEB(rcw->rcwWord,levelNode); return -1;	}
    if  ( docRtfPopNode( rr, levelNode ) )
	{ SDEB(rcw->rcwWord); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Handle tags that relate to the document hierarchy.			*/
/*									*/
/************************************************************************/

int docRtfHierarchy(	const RtfControlWord *	rcw,
			int			arg,
			RtfReader *		rr )
    {
    BufferItem *	levelNode= (BufferItem *)0;

    switch( rcw->rcwID )
	{
	case DOClevSECT:
	    /*  1  */
	    if  ( rr->rrcNode )
		{ levelNode= docGetSectNode( rr->rrcNode );	}
	    if  ( ! levelNode )
		{
		if  ( ! docRtfGetParaNode( rr ) )
		    { SDEB(rcw->rcwWord); return -1; }
		}
	    if  ( ! rr->rrcNode )
		{ SXDEB(rcw->rcwWord,rr->rrcNode); return -1;	}

	    levelNode= docGetSectNode( rr->rrcNode );
	    if  ( ! levelNode )
		{ SXDEB(rcw->rcwWord,levelNode); return -1;	}
	    if  ( docRtfPopNode( rr, levelNode ) )
		{ SDEB(rcw->rcwWord); return -1;	}

	    break;

	case DOClevROW:
	case DOClevNESTROW:
	    /*  1  */
	    if  ( rr->rrcNode )
		{ levelNode= docGetRowLevelNode( rr->rrcNode );	}
	    /* ignore stray \row tags rather than fail */
	    if  ( ! levelNode )
		{ LSDEB(rr->rrCurrentLine,rcw->rcwWord); return 0;	}
	    if  ( docRtfPopNode( rr, levelNode ) )
		{ SDEB(rcw->rcwWord); return -1;	}
	    break;

	case DOClevCELL:
	case DOClevNESTCELL:
	    if  ( docRtfHierarchyCell( rcw, rr ) )
		{ SDEB(rcw->rcwWord); return -1;	}
	    break;

	case DOClevPARA:
	    levelNode= docRtfGetParaNode( rr );
	    if  ( ! levelNode )
		{ SDEB(rcw->rcwWord); return -1; }

	    if  ( docRtfPopNode( rr, levelNode ) )
		{ SDEB(rcw->rcwWord); return -1;	}
	    break;

	case DOClevNONESTTABLES:
	    /* should be a destination */
	    break;

	default:
	    SLDEB(rcw->rcwWord,rcw->rcwID); break;
	}

    return 0;
    }

/************************************************************************/

static int docRtfStartParagraph(	RtfReader *	rr )
    {
    BufferItem *		paraNode;
    RtfReadingState *		rrs= rr->rrcState;
    const int			mindTable= 1;

    paraNode= docInsertNode( rr->rrDocument, rr->rrcNode, -1, DOClevPARA );
    if  ( ! paraNode )
	{ SXDEB(docLevelStr(rr->rrcLevel),paraNode); return -1; }
    rr->rrcNode= paraNode;

    if  ( docRtfSetParaProperties( &(paraNode->biParaProperties),
				    rr->rrDocument, mindTable, rrs, -1 ) )
	{ LDEB(1); return -1;	}

    /*  Silly old Jade  */
    if  ( paraNode->biParaLeftIndentTwips < 0	 )
	{ paraNode->biParaLeftIndentTwips= 0;	}
    if  ( paraNode->biParaFirstIndentTwips+ paraNode->biParaLeftIndentTwips < 0 )
	{ paraNode->biParaFirstIndentTwips= -paraNode->biParaLeftIndentTwips; }

    return 0;
    }

/************************************************************************/
/*									*/
/*  Start/Close a section.						*/
/*									*/
/************************************************************************/

static int docRtfStartSect(	RtfReader *	rr )
    {
    BufferItem *		sectNode;

    sectNode= docInsertNode( rr->rrDocument, rr->rrcNode, -1, DOClevSECT );
    if  ( ! sectNode )
	{ SXDEB(docLevelStr(rr->rrcLevel),sectNode); return -1; }
    rr->rrcNode= sectNode;

    if  ( sectNode->biTreeType == DOCinBODY )
	{
	rr->rrcSelectionScope= sectNode->biSectSelectionScope;

	if  ( docCopySectionProperties( &(sectNode->biSectProperties),
					 &(rr->rrcSectionProperties) ) )
	    { LDEB(1); return -1;	}

	if  ( sectNode->biNumberInParent > 0 )
	    {
	    BufferItem *	prevSectNode;

	    prevSectNode= sectNode->biParent->biChildren[
					sectNode->biNumberInParent- 1];

	    if  ( docCopySectHeadersFooters( sectNode, rr->rrDocument,
					    prevSectNode, rr->rrDocument ) )
		{ LDEB(1); return -1;	}
	    }
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Start a row.							*/
/*									*/
/************************************************************************/

static int docRtfStartRow(	RtfReader *	rr,
				int		forTable )
    {
    BufferItem *		rowNode;

    rowNode= docInsertNode( rr->rrDocument, rr->rrcNode, -1, DOClevROW );
    if  ( ! rowNode )
	{ SXDEB(docLevelStr(rr->rrcLevel),rowNode); return -1; }
    rowNode->biRowForTable= forTable;
    rr->rrcNode= rowNode;

    if  ( docRtfSetRowProperties( rr, rowNode ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

static int docRtfStartCell(	RtfReader *	rr )
    {
    BufferItem *		cellNode;

    cellNode= docInsertNode( rr->rrDocument, rr->rrcNode, -1, DOClevCELL );
    if  ( ! cellNode )
	{ SXDEB(docLevelStr(rr->rrcLevel),cellNode); return -1; }
    rr->rrcNode= cellNode;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Finish the node that is currently under construction.		*/
/*									*/
/************************************************************************/

int docRtfFinishCurrentNode(	const RtfControlWord *	rcw,
				RtfReader *		rr )
    {
    while(  rr->rrcNode )
	{
	if  ( docRtfCloseNode( rr, rr->rrcNode ) )
	    { LDEB(1); return -1;	}

	rr->rrcNode= rr->rrcNode->biParent;
	}

    return 0;
    }

/************************************************************************/

static int docRtfStartNextCell(	int *		pRowNesting,
				int		tableNestingTo,
				RtfReader *	rr )
    {
    int			rowNestingTo;

    rowNestingTo= tableNestingTo;
    if  ( rowNestingTo == 0 )
	{ rowNestingTo= 1;	}

    if  ( tableNestingTo > 0				&&
	  rr->rrcNode->biParent->biLevel == DOClevROW	&&
	  ! rr->rrcNode->biParent->biRowForTable	)
	{
	if  ( docRtfPopNode( rr, rr->rrcNode->biParent ) )
	    { LDEB(1); return -1;	}

	(*pRowNesting)--;
	return 0;
	}

    if  ( rowNestingTo > *pRowNesting )
	{
	if  ( *pRowNesting < 1 )
	    { LDEB(*pRowNesting); return -1;	}

	if  ( docRtfStartRow( rr, tableNestingTo > 0 ) )
	    { LDEB(1); return -1;	}

	(*pRowNesting)++;
	return 0;
	}

    if  ( rowNestingTo < *pRowNesting )
	{ LLDEB(rowNestingTo,*pRowNesting); return -1;	}

    if  ( docRtfStartParagraph( rr ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

/************************************************************************/

static int docRtfStartNode(	int *		pRowNesting,
				RtfReader *	rr )
    {
    RtfReadingState *	rrs= rr->rrcState;
    int			tableNestingTo;

    tableNestingTo= rrs->rrsParagraphProperties.ppTableNesting;

    switch( rr->rrcNode->biLevel )
	{
	case DOClevBODY:
	    if  ( docRtfStartSect( rr ) )
		{ LDEB(1); return -1;	}
	    break;

	case DOClevSECT:
	    if  ( docRtfStartRow( rr, tableNestingTo > 0 ) )
		{ LDEB(1); return -1;	}
	    (*pRowNesting)++;
	    break;

	case DOClevROW:
	    if  ( docRtfStartCell( rr ) )
		{ LDEB(1); return -1;	}
	    break;

	case DOClevCELL:
	    if  ( docRtfStartNextCell( pRowNesting, tableNestingTo, rr ) )
		{ LDEB(tableNestingTo); return -1;	}
	    break;

	default:
	    SDEB(docLevelStr(rr->rrcNode->biLevel));
	    return -1;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Return the current paragraph. If there is none, make one.		*/
/*									*/
/************************************************************************/

BufferItem * docRtfGetParaNode(	RtfReader *	rr )
    {
    int			rowNesting;

    if  ( ! rr->rrcNode )
	{ XDEB(rr->rrcNode); return (BufferItem *)0;	}

    rowNesting= docRowNesting( rr->rrcNode );

    while( rr->rrcNode->biLevel != DOClevPARA )
	{
	if  ( docRtfStartNode( &rowNesting, rr ) )
	    { LDEB(1); return (BufferItem *)0;	}
	}

    return rr->rrcNode;
    }

/************************************************************************/
/*									*/
/*  Return the current section. If there is none, make one.		*/
/*									*/
/************************************************************************/

BufferItem * docRtfGetSectNode(	RtfReader *	rr )
    {
    BufferItem *	sectNode= docGetSectNode( rr->rrcNode );

    if  ( ! sectNode )
	{
	int	rowNesting= 0;

	while( rr->rrcNode->biLevel != DOClevSECT )
	    {
	    if  ( docRtfStartNode( &rowNesting, rr ) )
		{ LDEB(1); return (BufferItem *)0;	}
	    }

	sectNode= rr->rrcNode;
	}

    if  ( ! sectNode || sectNode->biLevel != DOClevSECT )
	{ XDEB(sectNode); return (BufferItem *)0;	}

    return sectNode;
    }

