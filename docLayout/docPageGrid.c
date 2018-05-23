/************************************************************************/
/*									*/
/*  Layout of a document.						*/
/*									*/
/************************************************************************/

#   include	"docLayoutConfig.h"

#   include	<stddef.h>
#   include	<limits.h>

#   include	<docBuf.h>
#   include	<docPageGrid.h>
#   include	<docBlockFrame.h>
#   include	<docStripFrame.h>
#   include	<docTreeType.h>
#   include	<docTreeNode.h>
#   include	<docNodeTree.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Administrative routines: manage a block frame.			*/
/*									*/
/************************************************************************/

void docLayoutInitBlockFrame(	BlockFrame *	bf )
    {
    bf->bfPage= -1;
    bf->bfColumn= -1;
    geoInitRectangle( &(bf->bfFlowRect) );
    geoInitRectangle( &(bf->bfContentRect) );

    utilInitDocumentGeometry( &(bf->bfPageGeometry) );

    bf->bfFootnotesPlaced= 0;
    docInitNotesReservation( &(bf->bfNotesReservation) );
    }

void docLayoutCleanBlockFrame(	BlockFrame *	bf )
    {
    }

void docInitNotesReservation(	NotesReservation *	nr )
    {
    nr->nrDfFirstFootnote= (DocumentField *)0;
    nr->nrFootnoteCount= 0;
    nr->nrFootnoteSectBi= (const struct BufferItem *)0;

    nr->nrFtnsepHeight= 0;
    nr->nrFootnoteHeight= 0;
    }

/************************************************************************/
/*									*/
/*  Derive the frame for a paragraph from the page rectangle and the	*/
/*  paragraph properties.						*/
/*									*/
/*  For paragraphs inside a table cell, geometry is derived from the	*/
/*  table column.							*/
/*									*/
/*  1)  The block starts below what has been reserved for the page	*/
/*	header.								*/
/*  2)  The block starts above what has been reserved for the page	*/
/*	footer.								*/
/*									*/
/************************************************************************/

void docBlockFrameTwips(	BlockFrame *			bf,
				BufferItem *			node,
				BufferDocument *		bd,
				int				page,
				int				column )
    {
    DocumentTree *		dt;
    int				useColumns= 0;

    int				newFrame;
    int				prevY1;

    BufferItem *		sectNode;
    BufferItem *		bodySectNode;
    const NotesProperties *	npEndnotes= &(bd->bdProperties.dpNotesProps.fepEndnotesProps);

    const SectionProperties *	sp;

    sectNode= docGetSectNode( node );
    if  ( ! sectNode )
	{ XDEB(sectNode); return;	}

    sp= &(sectNode->biSectProperties);

    if  ( docGetRootOfSelectionScope( (DocumentTree **)0, &bodySectNode, bd,
					&(sectNode->biSectSelectionScope) ) )
	{ LDEB(1); return;	}

    if  ( sectNode->biTreeType == DOCinENDNOTE		&&
	  npEndnotes->npPlacement == FTNplaceDOC_END	)
	{
	const BufferItem *	rootNode= bd->bdBody.dtRoot;

	bodySectNode= rootNode->biChildren[rootNode->biChildCount- 1];
	}

    newFrame= page != bf->bfPage ||
	( bf->bfNotesReservation.nrFootnoteSectBi == bodySectNode &&
						    column != bf->bfColumn );

    if  ( sectNode->biTreeType != DOCinBODY && ! bodySectNode )
	{ SXDEB(docTreeTypeStr(sectNode->biTreeType),bodySectNode);	}

    if  ( sectNode->biTreeType != DOCinBODY	&&
	  sectNode->biTreeType != DOCinSHPTXT	&&
	  bodySectNode				)
	{ sp= &(bodySectNode->biSectProperties);	}

    bf->bfPageGeometry= sp->spDocumentGeometry;

    bf->bfPage= page;
    bf->bfColumn= column;

    bf->bfContentRect.drX0= bf->bfPageGeometry.dgLeftMarginTwips;
    bf->bfContentRect.drX1= bf->bfPageGeometry.dgPageWideTwips-
					bf->bfPageGeometry.dgRightMarginTwips;

    if  ( sectNode->biTreeType != DOCinSHPTXT )
	{
	const DocumentProperties *	dp= &(bd->bdProperties);

	if  ( sp->spDocumentGeometry.dgMirrorMargins )
	    {
	    if  ( dp->dpHasFacingPages && page % 2 )
		{
		bf->bfPageGeometry.dgLeftMarginTwips=
				    sp->spDocumentGeometry.dgRightMarginTwips;
		bf->bfPageGeometry.dgRightMarginTwips=
				    sp->spDocumentGeometry.dgLeftMarginTwips;
		}
	    }
	else{
	    if  ( dp->dpHasFacingPages && page % 2 )
		{
		bf->bfPageGeometry.dgRightMarginTwips +=
					sp->spDocumentGeometry.dgGutterTwips;
		}
	    else{
		bf->bfPageGeometry.dgLeftMarginTwips +=
					sp->spDocumentGeometry.dgGutterTwips;
		}
	    }

	/*  Should be irrelevant: */
	bf->bfPageGeometry.dgMirrorMargins= 0;
	bf->bfPageGeometry.dgGutterTwips= 0;
	}

    useColumns= docTreeInColumnType( sectNode->biTreeType );

    if  ( useColumns && sp->spColumnCount > 1 )
	{
	int	xLine;

	docSectGetColumnX( &xLine, &(bf->bfContentRect.drX0),
				    &(bf->bfContentRect.drX1),
				    sp, &(bf->bfPageGeometry), column );
	}

    if  ( sectNode->biTreeType != DOCinBODY || newFrame )
	{ docInitNotesReservation( &(bf->bfNotesReservation) );	}

    switch( sectNode->biTreeType )
	{
	int		isEmpty;

	case DOCinBODY:

	case DOCinFOOTNOTE:
	case DOCinENDNOTE:

	case DOCinFTNSEP:
	case DOCinFTNSEPC:
	case DOCinFTNCN:
	case DOCinAFTNSEP:
	case DOCinAFTNSEPC:
	case DOCinAFTNCN:

	    dt= (DocumentTree *)0;
	    docWhatPageHeader( &dt, &isEmpty, sectNode, page, bd );

	    /*  1 Reserved!!  */
	    if  ( dt && dt->dtRoot )
		{ bf->bfContentRect.drY0= dt->dtY1ReservedTwips;	}
	    else{
		bf->bfContentRect.drY0= bf->bfPageGeometry.dgTopMarginTwips;	
		}

	    dt= (DocumentTree *)0;
	    docWhatPageFooter( &dt, &isEmpty, sectNode, page, bd );

	    /*  2 Reserved!!  */
	    if  ( dt && dt->dtRoot )
		{
		bf->bfContentRect.drY1= dt->dtY0ReservedTwips;
		}
	    else{
		bf->bfContentRect.drY1=
			    bf->bfPageGeometry.dgPageHighTwips-
			    bf->bfPageGeometry.dgBottomMarginTwips;
		}

	    /*
	    appDebug( "PAGE %3d BLOCK Y: %5d..%5d %s\n",
			page, bf->bfContentRect.drY0, bf->bfContentRect.drY1,
			docTreeTypeStr( sectNode->biTreeType ) );
	    */

	    break;

	case DOCinFIRST_HEADER:
	case DOCinLEFT_HEADER:
	case DOCinRIGHT_HEADER:

	case DOCinFIRST_FOOTER:
	case DOCinLEFT_FOOTER:
	case DOCinRIGHT_FOOTER:

	    if  ( column != 0 )
		{
		SLLDEB(docTreeTypeStr(sectNode->biTreeType),page,column);
		column= 0;
		}

	    dt= docSectionHeaderFooter( bodySectNode, (unsigned char *)0,
				&(bd->bdProperties), sectNode->biTreeType );
	    if  ( ! dt )
		{ XDEB(dt); return;	}

	    bf->bfContentRect.drY0= dt->dtY0ReservedTwips;
	    bf->bfContentRect.drY1= dt->dtY1ReservedTwips;
	    break;

	case DOCinSHPTXT:

	    bf->bfContentRect.drY0= bf->bfPageGeometry.dgTopMarginTwips;
	    bf->bfContentRect.drY1=
			    bf->bfPageGeometry.dgPageHighTwips-
			    bf->bfPageGeometry.dgBottomMarginTwips;
	    break;

	default:
	    LDEB(sectNode->biTreeType);
	}

    prevY1= bf->bfContentRect.drY1;

    bf->bfFlowRect= bf->bfContentRect;

    if  ( sectNode->biTreeType == DOCinBODY && ! newFrame )
	{ bf->bfFlowRect.drY1= prevY1;	}

    return;
    }

void docParaBlockFrameTwips(	BlockFrame *		bf,
				BufferItem *		paraNode,
				BufferDocument *	bd,
				int			page,
				int			column )
    {
    if  ( paraNode->biLevel != DOClevPARA )
	{ LDEB(paraNode->biLevel);	}
    else{
	FrameProperties	fp;

	docGetFramePropertiesByNumber( &fp, bd, paraNode->biParaFrameNumber );
	if  ( DOCisFRAME( &fp ) )
	    {
	    BlockFrame		bfPage;
	    LayoutPosition	lpScratch;

	    int			frameHigh= fp.fpHighTwips;

	    if  ( frameHigh < 0 )
		{ frameHigh= -frameHigh;	}

	    docInitLayoutPosition( &lpScratch );

	    docLayoutInitBlockFrame( bf );
	    docLayoutInitBlockFrame( &bfPage );

	    docBlockFrameTwips( &bfPage, paraNode, bd, page, column );
	    docLayoutSetTextFrame( bf, &lpScratch, &bfPage, &fp, frameHigh );

	    docLayoutCleanBlockFrame( &bfPage );

	    return;
	    }
	}

    docBlockFrameTwips( bf, paraNode, bd, page, column );
    }

void docParaBlockFrameRectangle( DocumentRectangle *	dr,
				BufferItem *		paraNode,
				BufferDocument *	bd,
				int			page,
				int			column )
    {
    BlockFrame	bf;

    docLayoutInitBlockFrame( &bf );
    docBlockFrameTwips( &bf, paraNode, bd, page, column );

    *dr= bf.bfContentRect;

    docLayoutCleanBlockFrame( &bf );
    }

/************************************************************************/
/*									*/
/*  Shift a cell frame for all cells that surround its table. In	*/
/*  practice, this code is only activated for nested tables.		*/
/*									*/
/************************************************************************/

static void docShiftCellFrame(	ParagraphFrame *	pf,
				const BufferItem *	rowNode )
    {
    int			nrInParent= rowNode->biNumberInParent;
    const BufferItem *	parentBi= rowNode->biParent;

    while( parentBi )
	{
	if  ( parentBi->biLevel == DOClevROW )
	    {
	    const RowProperties *	rp= &(parentBi->biRowProperties);
	    int				x0;

	    if  ( nrInParent == 0 )
		{ x0= rowNode->biRowLeftIndentTwips;			}
	    else{ x0= rp->rpCells[nrInParent-1].cpRightBoundaryTwips;	}

	    x0 += rp->rpHalfGapWidthTwips;

	    pf->pfParaContentRect.drX0 += x0;
	    pf->pfParaContentRect.drX1 += x0;
	    pf->pfCellContentRect.drX0 += x0;
	    pf->pfCellContentRect.drX1 += x0;
	    pf->pfCellRect.drX0 += x0;
	    pf->pfCellRect.drX1 += x0;
	    pf->pfRedrawX0Twips += x0;
	    pf->pfRedrawX1Twips += x0;
	    }

	nrInParent= parentBi->biNumberInParent;
	parentBi= parentBi->biParent;
	}
    }

/************************************************************************/

static void docGetCellWidth(	DocumentRectangle *		drCell,
				int *				pLeftMargin,
				int *				pRightMargin,
				const BufferItem *		cellNode,
				const RowProperties *		rp,
				const BlockFrame *		bf )
    {
    int				col= cellNode->biNumberInParent;
    const CellProperties *	cp= rp->rpCells+ col;

    int				leftMargin= rp->rpHalfGapWidthTwips;
    int				rightMargin= rp->rpHalfGapWidthTwips;

    int				x0= bf->bfContentRect.drX0;
    int				x1= bf->bfContentRect.drX1;

    switch( rp->rpLeftCellPaddingUnit )
	{
	case TRautoNONE:
	    break;
	case TRautoTWIPS:
	    leftMargin= rp->rpLeftCellPadding;
	    break;
	default:
	    LDEB(rp->rpLeftCellPaddingUnit);
	    break;
	}

    switch( cp->cpLeftPaddingUnit )
	{
	case TRautoNONE:
	    break;
	case TRautoTWIPS:
	    leftMargin= cp->cpLeftPadding;
	    break;
	default:
	    LDEB(cp->cpLeftPaddingUnit);
	    break;
	}

    switch( rp->rpRightCellPaddingUnit )
	{
	case TRautoNONE:
	    break;
	case TRautoTWIPS:
	    rightMargin= rp->rpRightCellPadding;
	    break;
	default:
	    LDEB(rp->rpRightCellPaddingUnit);
	    break;
	}

    switch( cp->cpRightPaddingUnit )
	{
	case TRautoNONE:
	    break;
	case TRautoTWIPS:
	    rightMargin= cp->cpRightPadding;
	    break;
	default:
	    LDEB(cp->cpRightPaddingUnit);
	    break;
	}

    if  ( col == 0 )
	{ x0 += rp->rpLeftIndentTwips;		}
    else{ x0 += cp[-1].cpRightBoundaryTwips;	}

    x1= bf->bfContentRect.drX0+ cp->cpRightBoundaryTwips;
    if  ( cp->cpHorizontalMerge == CELLmergeHEAD )
	{
	int		colspan= 1;
	int		x11= docCellRight( &colspan, rp, col );

	x1= bf->bfContentRect.drX0+ x11;
	}

    drCell->drX0= x0;
    drCell->drX1= x1;
    *pLeftMargin= leftMargin;
    *pRightMargin= rightMargin;

    return;
    }

/************************************************************************/
/*									*/
/*  Determine the frame for a table cell.				*/
/*									*/
/*  This is used to calculate the layout.				*/
/*									*/
/************************************************************************/

void docCellFrameTwips(		ParagraphFrame *	pf,
				const BlockFrame *	bf,
				const BufferItem *	cellNode )
    {
    const BufferItem *		rowNode= cellNode->biParent;
    const RowProperties *	rp= &(rowNode->biRowProperties);

    int				leftMargin;
    int				rightMargin;

    pf->pfRedrawX0Twips= bf->bfContentRect.drX0;
    pf->pfRedrawX1Twips= bf->bfContentRect.drX1;

    pf->pfCellRect.drY0= INT_MIN;
    pf->pfCellRect.drY1= INT_MAX;
    pf->pfCellRect.drX0= bf->bfContentRect.drX0;
    pf->pfCellRect.drX1= bf->bfContentRect.drX1;

    if  ( ! docIsRowNode( rowNode ) )
	{ LDEB(rowNode->biRowCellCount); return;	}

    docGetCellWidth( &(pf->pfCellRect), &leftMargin, &rightMargin,
							cellNode, rp, bf );

    if  ( rowNode->biRowCellCount < 1 )
	{ LDEB(rowNode->biRowCellCount);	}
    else{
	pf->pfRedrawX0Twips= -rowNode->biRowLeftIndentTwips;
	pf->pfRedrawX1Twips=
		rp->rpCells[rowNode->biRowCellCount-1].cpRightBoundaryTwips;
	}

    if  ( rowNode->biRowHeightTwips < 0 )
	{
	pf->pfCellRect.drY0= rowNode->biTopPosition.lpPageYTwips;
	pf->pfCellRect.drY1= pf->pfCellRect.drY0- rowNode->biRowHeightTwips;
	}

    pf->pfCellContentRect= pf->pfCellRect;
    pf->pfCellContentRect.drX0 += leftMargin;
    pf->pfCellContentRect.drX1 -= rightMargin;

    docShiftCellFrame( pf, rowNode );

    return;
    }

void docParagraphFrameTwips(	ParagraphFrame *	pf,
				const BlockFrame *	bf,
				const BufferItem *	paraNode )
    {
    if  ( paraNode->biLevel != DOClevPARA )
	{ SDEB(docLevelStr(paraNode->biLevel));	}

    if  ( paraNode->biParaTableNesting > 0 )
	{ docCellFrameTwips( pf, bf, paraNode->biParent );	}
    else{
	pf->pfRedrawX0Twips= bf->bfContentRect.drX0;
	pf->pfRedrawX1Twips= bf->bfContentRect.drX1;

	pf->pfCellRect.drY0= INT_MIN;
	pf->pfCellRect.drY1= INT_MAX;
	pf->pfCellRect.drX0= bf->bfContentRect.drX0;
	pf->pfCellRect.drX1= bf->bfContentRect.drX1;

	pf->pfCellContentRect= pf->pfCellRect;
	}

    pf->pfParaContentRect= pf->pfCellContentRect;
    pf->pfParaContentRect.drX0 += paraNode->biParaLeftIndentTwips;
    pf->pfParaContentRect.drX1 -= paraNode->biParaRightIndentTwips;

    return;
    }

/************************************************************************/
/*									*/
/*  Determine the rectangle occupied by a table cell.			*/
/*									*/
/************************************************************************/

void docCellRectangleTwips(	DocumentRectangle *		drCell,
				const BlockFrame *		bf,
				const BufferItem *		cellNode )
    {
    ParagraphFrame		pf;
    const BufferItem *		rowNode= cellNode->biParent;
    const BufferItem *		sectNode= docGetSectNode( (BufferItem *)rowNode );
    const LayoutPosition *	lpTop;
    LayoutPosition		lpBelow;

    docCellFrameTwips( &pf, bf, cellNode );

    lpTop= &(rowNode->biTopPosition);
    docGetCellBottom( &lpBelow, cellNode );

    if  ( bf->bfPage > lpTop->lpPage			||
	  ( bf->bfPage == lpTop->lpPage		&&
	    bf->bfColumn > lpTop->lpColumn	)	)
	{
	if  ( sectNode->biTopPosition.lpPage == bf->bfPage )
	    {
	    pf.pfCellRect.drY0= sectNode->biTopPosition.lpPageYTwips;
	    }
	else{
	    pf.pfCellRect.drY0= bf->bfContentRect.drY0;
	    }
	}
    else{
	pf.pfCellRect.drY0= lpTop->lpPageYTwips;
	}

    if  ( bf->bfPage < lpBelow.lpPage			||
	  ( bf->bfPage == lpBelow.lpPage	&&
	    bf->bfColumn < lpBelow.lpColumn	)	)
	{
	if  ( sectNode->biBelowPosition.lpPage == bf->bfPage )
	    {
	    pf.pfCellRect.drY1= sectNode->biBelowPosition.lpPageYTwips;
	    }
	else{
	    pf.pfCellRect.drY1= bf->bfContentRect.drY1;
	    }
	}
    else{
	pf.pfCellRect.drY1= lpBelow.lpPageYTwips;
	}

    *drCell= pf.pfCellRect;
    return;
    }

/************************************************************************/
/*									*/
/*  Continue to lay out the text on a subsequent page.			*/
/*									*/
/*  1)  Continuous section wrap to the same position as where they	*/
/*	started on the page.						*/
/*									*/
/************************************************************************/

void docLayoutSectColumnTop(	LayoutPosition *	lpTop,
				BlockFrame *		bf,
				BufferItem *		bodySectNode,
				BufferDocument *	bd )
    {
    if  ( bodySectNode->biTreeType != DOCinBODY )
	{ SDEB(docTreeTypeStr(bodySectNode->biTreeType));	}

    docBlockFrameTwips( bf, bodySectNode, bd, lpTop->lpPage, lpTop->lpColumn );

    lpTop->lpPageYTwips= bf->bfContentRect.drY0;
    lpTop->lpAtTopOfColumn= 1;

    /*  1  */
    if  ( DOC_SECTnodeBELOW_PREVIOUS( bodySectNode )		&&
	  lpTop->lpPage == bodySectNode->biTopPosition.lpPage	&&
	  bodySectNode->biSectColumnCount > 1			&&
	  lpTop->lpColumn > 0					)
	{
	lpTop->lpPageYTwips= bodySectNode->biTopPosition.lpPageYTwips;
	lpTop->lpAtTopOfColumn= 0;
	}

    return;
    }

