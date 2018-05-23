/************************************************************************/
/*									*/
/*  Save a BufferDocument into an RTF file.				*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<stdio.h>
#   include	<ctype.h>

#   include	<appDebugon.h>

#   include	<docTreeType.h>
#   include	<docTreeScanner.h>
#   include	<docTreeNode.h>
#   include	<docNodeTree.h>

#   include	"docRtfWriterImpl.h"
#   include	"docRtfTags.h"
#   include	"docRtfFlags.h"

/************************************************************************/

static int docRtfSavePropertiesOfRow(
				RtfWriter *			rw,
				const RowProperties *		rpSet,
				const DocumentSelection *	ds )
    {
    int			rval= 0;
    int			col0= -1;
    int			col1= -1;

    RowProperties	rpRef;
    PropertyMask	rpDifMask;
    PropertyMask	rpAllMask;

    const DocumentAttributeMap * const dam0= (const DocumentAttributeMap *)0;

    docInitRowProperties( &rpRef );
    utilPropMaskClear( &rpDifMask );
    utilPropMaskClear( &rpAllMask );

    utilPropMaskFill( &rpAllMask, RPprop_FULL_COUNT );

    if  ( ds )
	{ col0= ds->dsCol0; col1= ds->dsCol1; 	}

    docRowPropertyDifference( &rpDifMask, &rpRef, &rpAllMask, rpSet, dam0 );

    if  ( rw->rwRpExtraMask )
	{ utilPropMaskOr( &rpDifMask, &rpDifMask, rw->rwRpExtraMask );	}

    docRtfWriteTag( rw, "trowd" );

    docRtfSaveRowProperties( rw, &rpDifMask, rpSet, col0, col1 );

    if  ( docCopyRowProperties( &(rw->rwRowProperties), rpSet, dam0 ) )
	{ LDEB(rpSet->rpCellCount); rval= -1; goto ready;	}

  ready:

    docCleanRowProperties( &rpRef );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Enter/Leave a table.						*/
/*									*/
/************************************************************************/

static int docRtfPushTable(	RtfWriter *			rw,
				const BufferItem *		rowNode,
				const DocumentSelection *	ds )
    {
    PropertyMask	ppSetMask;

    docRtfWriteNextLine( rw );
    docRtfWriteDestinationBegin( rw, "" );

    rw->rwTableNesting++;

    if  ( docRtfPushAttribute( rw ) )
	{ LDEB(1); return -1;	}

    utilPropMaskClear( &ppSetMask );
    utilPropMaskFill( &ppSetMask, PPprop_FULL_COUNT );

    if  ( docUpdParaProperties( (PropertyMask *)0,
				    &(rw->rwcOutsideTableParagraphProperties),
				    &ppSetMask, &(rw->rwcParagraphProperties),
				    (const DocumentAttributeMap *)0 ) )
	{ LDEB(1); return -1;	}

    docRtfSavePropertiesOfRow( rw, &(rowNode->biRowProperties), ds );

    return 0;
    }

static int docRtfPopTable(	RtfWriter *	rw )
    {
    PropertyMask	ppSetMask;

    docRtfWriteDestinationEnd( rw );
    docRtfWriteNextLine( rw );

    rw->rwTableNesting--;

    docCleanRowProperties( &(rw->rwRowProperties) );
    docInitRowProperties( &(rw->rwRowProperties) );

    if  ( docRtfPopAttribute( rw ) )
	{ LDEB(1); return -1;	}

    utilPropMaskClear( &ppSetMask );
    utilPropMaskFill( &ppSetMask, PPprop_FULL_COUNT );

    if  ( docUpdParaProperties( (PropertyMask *)0,
			    &(rw->rwcParagraphProperties), &ppSetMask,
			    &(rw->rwcOutsideTableParagraphProperties),
			    (const DocumentAttributeMap *)0 ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Save a Row level node.						*/
/*									*/
/************************************************************************/

static int docRtfEnterRowNode(	RtfWriter *			rw,
				const BufferItem *		rowNode,
				const DocumentSelection *	ds )
    {
    int			tableNesting= docTableNesting( rowNode );

    int			flattenCell= 0;

    if  ( ! ( rw->rwSaveFlags & RTFflagEMIT_CELL ) )
	{
	if  ( ds && docPositionsInsideCell( &(ds->dsHead), &(ds->dsTail) ) )
	    { flattenCell= 1;	}
	}

    if  ( tableNesting == 1 && ! flattenCell )
	{
	PropertyMask	rpDifMask;
	PropertyMask	rpAllMask;

	const DocumentAttributeMap * const dam0= (const DocumentAttributeMap *)0;

	utilPropMaskClear( &rpDifMask );
	utilPropMaskClear( &rpAllMask );

	utilPropMaskFill( &rpAllMask, RPprop_FULL_COUNT );

	docRowPropertyDifference( &rpDifMask, &(rw->rwRowProperties),
			    &rpAllMask, &(rowNode->biRowProperties), dam0 );

	if  ( ! utilPropMaskIsEmpty( &rpDifMask ) )
	    {
	    if  ( rw->rwTableNesting == 1 && docRtfPopTable( rw ) )
		{ LDEB(1);	}
	    }

	if  ( rw->rwTableNesting == 0			&&
	      docRtfPushTable( rw, rowNode, ds )	)
	    { LDEB(1);	}
	}

    return ADVICEtsOK;
    }

static int docRtfLeaveRowNode(	RtfWriter *			rw,
				const BufferItem *		rowNode,
				const DocumentSelection *	ds )
    {
    int			tableNesting= docTableNesting( rowNode );

    if  ( tableNesting > 1 )
	{
	int			nip= rowNode->biNumberInParent;
	const BufferItem *	cellBi= rowNode->biParent;

	docRtfWriteNextLine( rw );
	docRtfWriteDestinationBegin( rw, "*\\nesttableprops" );

	docRtfSavePropertiesOfRow( rw, &(rowNode->biRowProperties), ds );

	docRtfWriteTag( rw, "nestrow" );
	docRtfWriteNextLine( rw );

	docRtfWriteDestinationEnd( rw );
	docRtfWriteNextLine( rw );

	if  ( nip == cellBi->biChildCount- 1				||
	      ( nip < cellBi->biChildCount- 1			&&
	        cellBi->biChildren[nip+1]->biLevel != DOClevROW	)	)
	    {
	    docRtfSavePropertiesOfRow( rw,
			    &(cellBi->biParent->biRowProperties), ds );
	    }
	}
    else{
	docRtfWriteTag( rw, "row" );
	docRtfWriteNextLine( rw );
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Start a paragraph node.						*/
/*									*/
/*  0)  While saving a selection.. Make sure that section properties	*/
/*	are saved before the first paragraph in the document.		*/
/*  1)  When we are saving a selection, and the selection is inside a	*/
/*	table cell, do not set the \intbl flag. This is only activated	*/
/*	for copy/paste where it is essential and for undo/redo where it	*/
/*	is irrelevant.							*/
/*  2)  To make WP 8 happy, always save 'intbl' for the first paragraph	*/
/*	in a table row.							*/
/*									*/
/************************************************************************/

static int docRtfEnterParaNode(	RtfWriter *			rw,
				const BufferItem *		paraNode,
				const DocumentSelection *	ds )
    {
    int			flattenCell= 0;
    int			firstInRow= 0;

    /*  0  */
    if  ( ds							&&
	  paraNode->biTreeType == DOCinBODY			&&
	  ! rw->rwSectionPropertiesSaved			)
	{
	const BufferItem *	sectNode;

	sectNode= docGetSectNode( (BufferItem *)paraNode );
	if  ( ! sectNode )
	    { XDEB(sectNode); return -1;	}

	if  ( docRtfSaveSectionPropertiesOfNode( rw, ds, sectNode ) )
	    { LDEB(1); return -1;	}
	}

    /*  1  */
    if  ( ! ( rw->rwSaveFlags & RTFflagEMIT_CELL ) )
	{
	if  ( ds && docPositionsInsideCell( &(ds->dsHead), &(ds->dsTail) ) )
	    { flattenCell= 1;	}
	}
    else{
	if  ( paraNode->biParaTableNesting > 0				&&
	      ds							&&
	      docPositionsInsideCell( &(ds->dsHead), &(ds->dsTail) )	)
	    {
	    const BufferItem *	rowNode;

	    rowNode= docGetRowNode( (BufferItem *)paraNode );
	    if  ( ! rowNode )
		{ XDEB(rowNode); return -1;	}

	    if  ( rw->rwTableNesting == 0		&&
		  docRtfPushTable( rw, rowNode, ds )	)
		{ LDEB(1);	}
	    }
	}

    if  ( paraNode->biParaTableNesting > 0 )
	{
	BufferItem *		rowNode;
	DocumentPosition	dpFirst;

	rowNode= docGetRowNode( (BufferItem *)paraNode );
	if  ( ! rowNode					||
	      docHeadPosition( &dpFirst, rowNode )	)
	    { XDEB(rowNode);	}
	else{ firstInRow= paraNode == dpFirst.dpNode;	}
	}

    if  ( docRtfSaveParaNode( rw, paraNode, ds, flattenCell, firstInRow ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Start an arbitrary node.						*/
/*									*/
/************************************************************************/

static int docRtfEnterNode(	BufferItem *			node,
				const DocumentSelection *	ds,
				const BufferItem *		bodySectNode,
				void *				voidrw )
    {
    RtfWriter *	rw= (RtfWriter *)voidrw;

    switch( node->biLevel )
	{
	case DOClevBODY:
	case DOClevCELL:
	    break;

	case DOClevSECT:

	    if  ( node->biTreeType == DOCinBODY			&&
		  docRtfSaveSectionPropertiesOfNode( rw, ds, node )	)
		{ LDEB(1); return -1;	}

	    if  ( ! ds							&&
		  node->biTreeType == DOCinBODY			&&
		  docRtfSaveSectHeadersFooters( rw, node )		)
		{ LDEB(1); return -1;	}
	    break;

	case DOClevROW:
	    if  ( docIsRowNode( node ) )
		{
		if  ( docRtfEnterRowNode( rw, node, ds ) )
		    { LDEB(1); return -1;	}
		}
	    else{
		if  ( rw->rwTableNesting > 0 && docRtfPopTable( rw ) )
		    { LDEB(rw->rwTableNesting);	}
		}
	    break;

	case DOClevPARA:
	    if  ( docRtfEnterParaNode( rw, node, ds ) )
		{ LDEB(1); return -1;	}
	    break;

	default:
	    LDEB(node->biLevel); return -1;
	}

    return 0;
    }

static int docRtfLeaveNode(	BufferItem *			node,
				const DocumentSelection *	ds,
				const BufferItem *		bodySectNode,
				void *				voidrw )
    {
    RtfWriter *	rw= (RtfWriter *)voidrw;

    switch( node->biLevel )
	{
	BufferItem *	parentNode;

	case DOClevBODY:
	    break;

	case DOClevCELL:
	    parentNode= node->biParent;

	    if  ( docIsRowNode( parentNode ) )
		{
		int	tableNesting= docTableNesting( parentNode );

		if  ( rw->rwcLastNodeLevel != DOClevPARA )
		    { docRtfSaveParaTableNesting( rw, tableNesting );	}

		if  ( tableNesting > 1 )
		    { docRtfWriteTag( rw, "nestcell" );	}
		else{ docRtfWriteTag( rw, "cell" );	}

		if  ( node->biNumberInParent != parentNode->biChildCount- 1 )
		    { docRtfWriteNextLine( rw );	}
		}
	    break;

	case DOClevSECT:
	    while( rw->rwTableNesting > 0 )
		{
		if  ( docRtfPopTable( rw ) )
		    { LDEB(rw->rwTableNesting);	}
		}

	    if  ( node->biParent					&&
		  node->biNumberInParent < node->biParent->biChildCount- 1 )
		{ docRtfWriteTag( rw, "sect" );	}

	    docRtfWriteNextLine( rw );
	    break;

	case DOClevROW:
	    if  ( docIsRowNode( node ) )
		{
		if  ( docRtfLeaveRowNode( rw, node, ds ) )
		    { LDEB(1); return -1;	}
		}
	    break;

	case DOClevPARA:
	    break;

	default:
	    LDEB(node->biLevel); return -1;
	}

    rw->rwcLastNodeLevel= node->biLevel;
    return 0;
    }

/************************************************************************/
/*									*/
/*  Save an inidividual header, footer, or footnote.			*/
/*									*/
/************************************************************************/

int docRtfSaveDocumentTree(		RtfWriter *		rw,
					const char *		tag,
					const DocumentTree *	dt,
					int			evenIfAbsent,
					int			forcePar )
    {
    int			savedTableNesting= rw->rwTableNesting;
    const int		flags= FLAGtsSCAN_MERGED_CELLS;

    if  ( ! dt->dtRoot )
	{
	if  ( evenIfAbsent )
	    {
	    docRtfWriteDestinationBegin( rw, tag );
	    docRtfWriteDestinationEnd( rw );
	    }

	return 0;
	}

    if  ( dt->dtRoot->biLevel != DOClevSECT )
	{ SDEB(docLevelStr(dt->dtRoot->biLevel)); return -1;	}

    docRtfWriteDestinationBegin( rw, tag );
    if  ( docRtfPushAttribute( rw ) )
	{ LDEB(1); return -1;	}

    docCleanParagraphProperties( &(rw->rwcParagraphProperties) );
    docInitParagraphProperties( &(rw->rwcParagraphProperties) );

    rw->rwTableNesting= 0;

    docRtfWriteTag( rw, RTFtag_pard );
    docRtfWriteSwitchToPlain( rw );
    docRtfWriteNextLine( rw );

    if  ( docScanTree( rw->rwDocument, dt,
		    docRtfEnterNode, docRtfLeaveNode, flags, (void *)rw ) < 0 )
	{ LDEB(1); return -1;	}

    /********************************************************************/
    /*  MS-Word does not pick up paragraph properties of headers and	*/
    /*  footers without a \par						*/
    /********************************************************************/
    if  ( forcePar )
	{
	DocumentPosition	dp;

	if  ( ! docTailPosition( &dp, dt->dtRoot )		&&
	      dp.dpNode->biParaTableNesting == 0		)
	    { docRtfWriteTag( rw, RTFtag_par );	}
	}

    docCleanParagraphProperties( &(rw->rwcParagraphProperties) );
    docInitParagraphProperties( &(rw->rwcParagraphProperties) );
    utilInitTextAttribute( &(rw->rwTextAttribute) );
    rw->rwTextCharset= FONTcharsetANSI;

    docRtfWriteDestinationEnd( rw );
    docRtfWriteNextLine( rw );

    if  ( docRtfPopAttribute( rw ) )
	{ LDEB(1); return -1;	}
    rw->rwTableNesting= savedTableNesting;

    return 0;
    }

int docRtfWriteSelection(	RtfWriter *			rw,
				const DocumentSelection *	ds )
    {
    const int		flags= FLAGtsSCAN_MERGED_CELLS;

    if  ( ds )
	{
	if  ( docScanSelection( rw->rwDocument, ds,
		    docRtfEnterNode, docRtfLeaveNode, flags, (void *)rw ) < 0 )
	    { LDEB(1); return -1;	}
	}
    else{
	if  ( docScanTree( rw->rwDocument, &(rw->rwDocument->bdBody),
		    docRtfEnterNode, docRtfLeaveNode, flags, (void *)rw ) < 0 )
	    { LDEB(1); return -1;	}
	}

    while( rw->rwTableNesting > 0 )
	{
	if  ( docRtfPopTable( rw ) )
	    { LDEB(rw->rwTableNesting);	}
	}

    return 0;
    }
