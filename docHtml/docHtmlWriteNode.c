/************************************************************************/
/*									*/
/*  Save a BufferDocument into an HTML file.				*/
/*  Depending on the parameters, this is either an HTML file with	*/
/*  a directory for the images, or a MHTML (rfc2112,rfc2557) aggregate.	*/
/*  RFC 2557 was never validated.					*/
/*									*/
/************************************************************************/

#   include	"docHtmlConfig.h"

#   include	<stdio.h>
#   include	<ctype.h>

#   include	<sioGeneral.h>

#   include	<docTreeType.h>
#   include	<docBuf.h>
#   include	<psShading.h>
#   include	<docPageGrid.h>
#   include	"docHtmlWriteImpl.h"
#   include	"docWriteCss.h"
#   include	<docTreeScanner.h>
#   include	<docTreeNode.h>
#   include	<docNodeTree.h>
#   include	<docLayout.h>

#   include	<appDebugon.h>

static int docHtmlStartTable(	HtmlWritingContext *		hwc,
				const BufferItem *		rowNode,
				const BufferItem *		bodySectNode )
    {
    int				rval= 0;
    int				cellPadding;
    int				width;

    int				tableX0;
    int				tableX1;
    int				parentX0;
    int				parentX1;

    const int			page= 0;
    const int			column= 0;

    char			scratch[100];

    docBlockFrameTwips( &(hwc->hwcBlockFrame), (BufferItem *)rowNode,
					    hwc->hwcDocument, page, column );

    docCellFrameTwips( &(hwc->hwcParagraphFrame), &(hwc->hwcBlockFrame),
							rowNode->biChildren[0] );
    tableX0= hwc->hwcParagraphFrame.pfCellRect.drX0;
    if  ( rowNode->biChildCount > 1 )
	{
	docCellFrameTwips( &(hwc->hwcParagraphFrame), &(hwc->hwcBlockFrame),
			rowNode->biChildren[rowNode->biChildCount- 1] );
	}
    tableX1= hwc->hwcParagraphFrame.pfCellRect.drX1;

    if  ( rowNode->biParent->biLevel == DOClevCELL )
	{
	docCellFrameTwips( &(hwc->hwcParagraphFrame), &(hwc->hwcBlockFrame),
							    rowNode->biParent );
	parentX0= hwc->hwcParagraphFrame.pfCellRect.drX0;
	parentX1= hwc->hwcParagraphFrame.pfCellRect.drX1;
	}
    else{
	parentX0= hwc->hwcBlockFrame.bfContentRect.drX0;
	parentX1= hwc->hwcBlockFrame.bfContentRect.drX1;
	}

    width= ( 100* tableX1- 100* tableX0+ 50 )/ ( parentX1- parentX0 );
    sprintf( scratch, "<table style=\"width:%d%%\" cellspacing=\"0\"", width );
    docHtmlPutString( scratch, hwc );

    cellPadding= TWIPS_TO_PIXELS( rowNode->biRowHalfGapWidthTwips )- 4;
    if  ( cellPadding < 1 )
	{ docHtmlWriteIntAttribute( hwc, "cellpadding", 0 ); }
    if  ( cellPadding > 1 )
	{ docHtmlWriteIntAttribute( hwc, "cellpadding", cellPadding ); }

    docHtmlPutString( ">", hwc );

    hwc->hwcTableNesting++;

  /*ready:*/

    return rval;
    }

static int docHtmlFinishTable(	HtmlWritingContext *		hwc )
    {
    docHtmlPutString( "</table>", hwc );
    docHtmlNewLine( hwc );

    hwc->hwcTableNesting--;

    if  ( hwc->hwcInlineNotes		&&
	  hwc->hwcTableNesting == 0	)
	{
	if  ( docHtmlSaveDeferredNotes( hwc ) )
	    { LDEB(1); return -1;	}
	}

    return 0;
    }

static int docHtmlEnterCellNode(	HtmlWritingContext *	hwc,
					const BufferItem *	bodySectNode,
					const BufferItem *	rowNode,
					int			col,
					const BufferItem *	cellNode )
    {
    int					wide;

    const int				page= 0;
    const int				column= 0;

    BlockFrame				bf;
    ParagraphFrame			pf;

    int					useStyle= 1;

    int					rowspan= 1;
    int					colspan= 1;

    ItemShading				is;

    const CellProperties *		cp= &(rowNode->biRowCells[col]);
    BufferDocument *			bd= hwc->hwcDocument;
    XmlWriter *				xw= &(hwc->hwcXmlWriter);

    docTableDetermineCellspans( &rowspan, &colspan, cellNode );

    docBlockFrameTwips( &bf, cellNode->biChildren[0],
					    bd, page, column );

    docCellFrameTwips( &pf, &bf, cellNode );
    wide= TWIPS_TO_PIXELS( pf.pfCellContentRect.drX1-
					    pf.pfCellContentRect.drX0 );

    if  ( docBorderNumberIsBorder( bd, cp->cpTopBorderNumber ) )
	{ useStyle++;	}
    if  ( docBorderNumberIsBorder( bd, cp->cpLeftBorderNumber ) )
	{ useStyle++;	}
    if  ( docBorderNumberIsBorder( bd, cp->cpRightBorderNumber ) )
	{ useStyle++;	}
    if  ( docBorderNumberIsBorder( bd, cp->cpBottomBorderNumber ) )
	{ useStyle++;	}

    docHtmlPutString( "<td", hwc );

    if  ( colspan > 1 )
	{ docHtmlWriteIntAttribute( hwc, "colspan", colspan );	}
    if  ( rowspan > 1 )
	{ docHtmlWriteIntAttribute( hwc, "rowspan", rowspan );	}

    switch( cp->cpValign )
	{
	case DOCtvaTOP:
	    /* docHtmlWriteStringAttribute( hwc, "valign, "top" ); */
	    break;
	case DOCtvaCENTERED:
	    docHtmlWriteStringAttribute( hwc, "valign", "middle" );
	    break;
	case DOCtvaBOTTOM:
	    docHtmlWriteStringAttribute( hwc, "valign", "bottom" );
	    break;

	default:
	    LDEB(cp->cpValign);
	    break;
	}

    docGetItemShadingByNumber( &is, bd, cp->cpShadingNumber );

    if  ( ! useStyle && is.isPattern == DOCspSOLID )
	{ docHtmlEmitBackgroundProperty( &is, hwc );	}

    if  ( useStyle )
	{
	char			scratch[32];
	SimpleOutputStream *	sos= xw->xwSos;

	docHtmlPutString( " style=\"", hwc );

	sprintf( scratch, "width: %dpx;", wide );
	docHtmlPutString( scratch, hwc );

	if  ( is.isPattern == DOCspSOLID )
	    {
	    docCssEmitBackgroundStyle( &(xw->xwColumn), sos, bd, &is );
	    }

	docCssEmitBorderStyleByNumber( &(xw->xwColumn), sos, bd,
			    "border-top", cp->cpTopBorderNumber );
	docCssEmitBorderStyleByNumber( &(xw->xwColumn), sos, bd,
			    "border-left", cp->cpLeftBorderNumber );
	docCssEmitBorderStyleByNumber( &(xw->xwColumn), sos, bd,
			    "border-right", cp->cpRightBorderNumber );
	docCssEmitBorderStyleByNumber( &(xw->xwColumn), sos, bd,
			    "border-bottom", cp->cpBottomBorderNumber );

	docHtmlPutString( "\"", hwc );
	}

    docHtmlPutString( ">", hwc );
    docHtmlNewLine( hwc );

    return 0;
    }

static int docHtmlEnterRowNode(	HtmlWritingContext *		hwc,
				const BufferItem *		rowNode,
				const BufferItem *		bodySectNode )
    {
    int			high;
    int			tableNesting= docTableNesting( rowNode );

    if  ( hwc->hwcTableNesting == tableNesting			&&
	  rowNode->biRowTableFirst == rowNode->biNumberInParent	)
	{ docHtmlFinishTable( hwc );	}

    if  ( hwc->hwcTableNesting < tableNesting )
	{ docHtmlStartTable( hwc, rowNode, bodySectNode );	}

    docHtmlPutString( "<tr style=\"", hwc );

    docHtmlPutString( "vertical-align: top;", hwc );

    high= rowNode->biRowHeightTwips;
    if  ( high < 0 )
	{ high= -high;	}
    high= TWIPS_TO_PIXELS( high );
    if  ( high > 0 )
	{
	char	scratch[32];

	sprintf( scratch, "height: %dpx;", high );
	docHtmlPutString( scratch, hwc );
	}

    docHtmlPutString( "\">", hwc );

    return 0;
    }

static int docHtmlEmitSectColumnStyleProps(
				const SectionProperties *	sp,
				int				balanced,
				const char *			prefix,
				HtmlWritingContext *		hwc )
    {
    char		scratch[100];
    int			colWide;

    colWide= docSectGetColumnWidth( (int *)0, (int *)0, sp, 0 );

    sprintf( scratch, "%scolumn-count: %d;", prefix,
			    sp->spColumnCount );
    docHtmlPutString( scratch, hwc );

    sprintf( scratch, "%scolumn-width: %dpx;", prefix,
			    TWIPS_TO_PIXELS( colWide ) );
    docHtmlPutString( scratch, hwc );

    sprintf( scratch, "%scolumn-gap: %dpx;", prefix,
			    TWIPS_TO_PIXELS( sp->spColumnSpacingTwips ) );
    docHtmlPutString( scratch, hwc );

    if  ( balanced )
	{ sprintf( scratch, "%scolumn-fill: balance;", prefix );	}
    else{ sprintf( scratch, "%scolumn-fill: auto;", prefix );		}
    docHtmlPutString( scratch, hwc );

    if  ( sp->spLineBetweenColumns )
	{
	sprintf( scratch, "%scolumn-rule: 1px solid;", prefix );
	docHtmlPutString( scratch, hwc );
	}

    return 0;
    }

static int dochtmlEnterSectNode(	BufferItem *		node,
					HtmlWritingContext *	hwc )
    {
    int			rval= 0;
    unsigned char	applies= 0;

    SectionProperties	sp;

    docInitSectionProperties( &sp );

    if  ( docSectionHasHeaderFooter( node, &applies,
				&(hwc->hwcDocument->bdProperties),
				DOCinFIRST_HEADER )			&&
	  applies							)
	{
	DocumentTree *	headerTree;

	headerTree= docSectionHeaderFooter( node, &applies,
				    &(hwc->hwcDocument->bdProperties),
				    DOCinFIRST_HEADER );

	if  ( docHtmlSaveSelection( hwc, headerTree,
				    (const DocumentSelection *)0 ) )
	    { LDEB(1);	}
	}

    docHtmlPutString( "<div style=\"", hwc );

    switch( node->biSectBreakKind )
	{
	case DOCibkNONE:
	    break;

	case DOCibkCOL:
	case DOCibkPAGE:
	    docHtmlPutString( "page-break-before: always;", hwc );
	    break;
	case DOCibkEVEN:
	    docHtmlPutString( "page-break-before: left;", hwc );
	    break;
	case DOCibkODD:
	    docHtmlPutString( "page-break-before: right;", hwc );
	    break;
	}

    if  ( node->biSectColumnCount > 1 )
	{
	int		balanced;

	if  ( docCopySectionProperties( &sp, &(node->biSectProperties) ) )
	    { LDEB(1); rval= -1; goto ready;	}

	docSectSetEqualColumnWidth( &sp );

	balanced= docSectColumnsAreBalanced( node );

	docHtmlEmitSectColumnStyleProps( &sp, balanced, "", hwc );
	docHtmlEmitSectColumnStyleProps( &sp, balanced, "-moz-", hwc );
	docHtmlEmitSectColumnStyleProps( &sp, balanced, "-webkit-", hwc );
	}

    docHtmlPutString( "\">", hwc );

  ready:

    docCleanSectionProperties( &sp );

    return rval;
    }

static int dochtmlLeaveSectNode(	BufferItem *		node,
					HtmlWritingContext *	hwc )
    {
    docHtmlPutString( "</div>", hwc );
    docHtmlNewLine( hwc );

    return 0;
    }

static int docHtmlLeaveNode(	BufferItem *			node,
				const DocumentSelection *	ds,
				const BufferItem *		bodySectNode,
				void *				voidhwc )
    {
    HtmlWritingContext *	hwc= (HtmlWritingContext *)voidhwc;

    switch( node->biLevel )
	{
	case DOClevSECT:
	    if  ( node->biTreeType == DOCinBODY && ! ds )
		{ dochtmlLeaveSectNode( node, hwc );	}
	    break;

	case DOClevBODY:
	    break;

	case DOClevCELL:
	    if  ( docIsRowNode( node->biParent ) )
		{
		docHtmlPutString( "</td>", hwc );
		if  ( node->biNumberInParent < node->biParent->biChildCount- 1 )
		    { docHtmlNewLine( hwc );	}
		}
	    break;

	case DOClevROW:
	    if  ( docIsRowNode( node ) )
		{
		docHtmlPutString( "</tr>", hwc );
		docHtmlNewLine( hwc );
		}
	    break;

	case DOClevPARA:
	    break;

	default:
	    LDEB(node->biLevel); return -1;
	}

    return ADVICEtsOK;
    }

static int docHtmlEnterNode(	BufferItem *			node,
				const DocumentSelection *	ds,
				const BufferItem *		bodySectNode,
				void *				voidhwc )
    {
    HtmlWritingContext *	hwc= (HtmlWritingContext *)voidhwc;
    int				tableNesting= docTableNesting( node );

    while( hwc->hwcTableNesting > tableNesting )
	{ docHtmlFinishTable( hwc );	}

    switch( node->biLevel )
	{
	case DOClevBODY:
	    break;

	case DOClevSECT:
	    if  ( node->biTreeType == DOCinBODY && ! ds )
		{ dochtmlEnterSectNode( node, hwc );	}
	    break;

	case DOClevCELL:
	    if  ( docIsRowNode( node->biParent ) )
		{
		if  ( docHtmlEnterCellNode( hwc, bodySectNode,
			    node->biParent, node->biNumberInParent, node ) )
		    { LDEB(node->biNumberInParent); return -1;	}
		}
	    break;

	case DOClevROW:
	    if  ( docIsRowNode( node ) )
		{
		if  ( docHtmlEnterRowNode( hwc, node, bodySectNode ) )
		    { LDEB(1); return -1;	}
		}
	    break;

	case DOClevPARA:
	    if  ( docHtmlSaveParaNode( hwc, node, bodySectNode, ds ) )
		{ LDEB(1); return -1;	}
	    break;

	default:
	    LDEB(node->biLevel); return -1;
	}

    return ADVICEtsOK;
    }

/************************************************************************/

int docHtmlSaveSelection(	HtmlWritingContext *		hwc,
				DocumentTree *			dt,
				const DocumentSelection *	ds )
    {
    const int			flags= 0;

    if  ( ds )
	{
	if  ( docScanSelection( hwc->hwcDocument, ds,
					docHtmlEnterNode, docHtmlLeaveNode,
					flags, (void *)hwc ) < 0 )
	    { LDEB(1); return -1;	}
	}
    else{
	if  ( docScanTree( hwc->hwcDocument, dt,
					docHtmlEnterNode, docHtmlLeaveNode,
					flags, (void *)hwc ) < 0 )
	    { LDEB(1); return -1;	}
	}

    while( hwc->hwcTableNesting > 0 )
	{ docHtmlFinishTable( hwc );	}

    return 0;
    }

