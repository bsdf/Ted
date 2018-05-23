#   include	"docLayoutConfig.h"

#   include	"docDraw.h"
#   include	<docPageGrid.h>
#   include	"docLayout.h"
#   include	<docTreeType.h>
#   include	<docTreeNode.h>
#   include	<docNodeTree.h>
#   include	<docTextLine.h>
#   include	<docNotes.h>
#   include	<docDocumentNote.h>
#   include	"docSelectLayout.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Generic Document drawing machinery.					*/
/*									*/
/************************************************************************/

static int docDrawLineLeftOfColumn(	BufferItem *		nextBodyNode,
					void *			through,
					LayoutPosition *	lpHere,
					BlockFrame *		bf,
					DrawingContext *	dc )
    {
    const LayoutContext *	lc= &(dc->dcLayoutContext);
    const SectionProperties *	sp= &(nextBodyNode->biSectProperties);

    DocumentRectangle		drLLine;
    DocumentRectangle		drRLine;
    int				x0;
    int				x1;

    BlockOrnaments		ornaments;

    struct DocumentNote *	dn0;
    const DocumentField *	df0;

    int				alongNotes= 0;
    BufferItem *		biColBottomRoot;

    DocumentPosition		dpBottom;
    int				lineBottom;
    int				partBottom;
    const TextLine *		tlBottom= (const TextLine *)0;

    if  ( ! docIsSectNode( nextBodyNode ) )
	{ LDEB(nextBodyNode->biLevel); return -1;	}

    df0= docGetFirstNoteOnPage( &dn0, lc->lcDocument,
					    lpHere->lpPage, DOCinFOOTNOTE );
    if  ( df0 )
	{
	if  ( dn0->dnDocumentTree.dtRoot->biSectSelectionScope.ssOwnerSectNr >=
						nextBodyNode->biNumberInParent )
	    { alongNotes= 1;	}
	}

    docInitBlockOrnaments( &ornaments );
    PROPmaskADD( &(ornaments.boPropMask), ORNdrawLEFT_BORDER );

    ornaments.boLeftBorder.bpStyle= DOCbsS;
    ornaments.boLeftBorder.bpPenWideTwips= 15;
    ornaments.boLeftBorderNumber= 1; /* FAKE */

    drLLine= bf->bfFlowRect;

    if  ( nextBodyNode->biTopPosition.lpPage == lpHere->lpPage )
	{ drLLine.drY0= nextBodyNode->biTopPosition.lpPageYTwips; }

    /* Why?
    biColBottomRoot= lc->lcDocument->bdBody.dtRoot;
    */
    biColBottomRoot= nextBodyNode;
    if  ( alongNotes )
	{
	DocumentNote *		dnz;
	const DocumentField *	dfz;

	dfz= docGetLastNoteInColumn( &dnz, lc->lcDocument,
			    lpHere->lpPage, lpHere->lpColumn, DOCinFOOTNOTE );
	if  ( dfz )
	    { biColBottomRoot= dnz->dnDocumentTree.dtRoot;	}
	}

    if  ( nextBodyNode->biTopPosition.lpPage <= lpHere->lpPage )
	{
	if  ( docGetLastInColumnForNode( &dpBottom, &lineBottom, &partBottom,
			biColBottomRoot, lpHere->lpPage, lpHere->lpColumn ) )
	    { LLDEB(lpHere->lpPage,lpHere->lpColumn); return -1;	}

	tlBottom= dpBottom.dpNode->biParaLines+ lineBottom;
	drLLine.drY1= tlBottom->tlTopPosition.lpPageYTwips+
						    tlBottom->tlLineStride;

	if  ( nextBodyNode->biBelowPosition.lpPage == lpHere->lpPage )
	    { drLLine.drY1= nextBodyNode->biBelowPosition.lpPageYTwips; }

	docSectGetColumnX( &(drLLine.drX0), &x0, &x1, sp,
				&(bf->bfPageGeometry), lpHere->lpColumn );
	drRLine= drLLine;
	drRLine.drX0= drLLine.drX0- ornaments.boLeftBorder.bpPenWideTwips;

	if  ( (*dc->dcDrawOrnaments)( &ornaments, lpHere->lpPage,
					    &drRLine, &drLLine, through, dc ) )
	    { LDEB(1); return -1;	}
	}

    docCleanBorderProperties( &bpLine );
    return 0;
    }

/************************************************************************/
/*									*/
/*  Skip to the next page.						*/
/*									*/
/*  A)  Draw the shapes for this page.					*/
/*  1)  Just to make the compiler happy, if nothing is to be printed	*/
/*	anymore, the current position is not going to be used.		*/
/*									*/
/************************************************************************/

int docDrawToNextColumn(		BufferItem *		thisBodyNode,
					BufferItem *		nextBodyNode,
					void *			through,
					LayoutPosition *	lpHere,
					BlockFrame *		bf,
					DrawingContext *	dc )
    {
    const LayoutContext *	lc= &(dc->dcLayoutContext);
    BufferDocument *		bd= lc->lcDocument;

    const int			asLast= 0;

    int				startNewPage= 0;
    int				nextPage;
    int				nextColumn;
    int				isPageBreak= 0;

    thisBodyNode= docGetSectNode( thisBodyNode );
    nextBodyNode= docGetSectNode( nextBodyNode );

    nextPage= lpHere->lpPage;
    nextColumn= lpHere->lpColumn;

    nextColumn++;

    if  ( nextBodyNode )
	{
	if  ( nextColumn >= nextBodyNode->biSectColumnCount )
	    { nextPage++; nextColumn= 0; }
	}
    else{
	if  ( nextColumn >= thisBodyNode->biSectColumnCount )
	    { nextPage++; nextColumn= 0; }
	}

    if  ( nextPage != lpHere->lpPage )
	{ isPageBreak= 1; }

    if  ( dc->dcLastPage >= 0		&&
	  nextPage > dc->dcLastPage	)
	{ return 1; }

    if  ( dc->dcFirstPage < 0				||
	  lpHere->lpPage >= dc->dcFirstPage		)
	{
	const int	belowText= 0;

	startNewPage= 1;

	if  ( thisBodyNode && thisBodyNode->biTreeType != DOCinBODY )
	    { SDEB(docTreeTypeStr(thisBodyNode->biTreeType));	}

	if  ( thisBodyNode					&&
              thisBodyNode->biTreeType == DOCinBODY		&&
	      dc->dcDrawExternalItems				)
	    {
	    if  ( docDrawFootnotesForColumn( lpHere->lpPage, lpHere->lpColumn,
								through, dc ) )
		{ LDEB(lpHere->lpPage); return -1;	}

	    if  ( ! dc->dcPostponeHeadersFooters			&&
		  isPageBreak						&&
		  docDrawPageFooter( thisBodyNode, through, dc,
						    lpHere->lpPage )	)
		{ LDEB(lpHere->lpPage); return -1;	}
	    }

	/*  A  */
	if  ( docDrawShapesForPage( through, dc, belowText, lpHere->lpPage ) )
	    { LDEB(lpHere->lpPage);	}

	if  ( isPageBreak						&&
	      dc->dcFinishPage						&&
	      (*dc->dcFinishPage)( through, dc, thisBodyNode,
					    lpHere->lpPage, asLast )	)
	    { LDEB(1); return -1;	}
	}

    if  ( dc->dcLastPage < 0		||
	  nextPage <= dc->dcLastPage	)
	{
	lpHere->lpColumn= nextColumn;
	lpHere->lpPage= nextPage;

	if  ( nextBodyNode )
	    {
	    const SectionProperties *	sp= &(nextBodyNode->biSectProperties);
	    const DocumentGeometry *	dg= &(sp->spDocumentGeometry);
	    const int			belowText= 1;

	    docLayoutSectColumnTop( lpHere, bf, nextBodyNode, bd );

	    if  ( isPageBreak && startNewPage )
		{
		if  ( dc->dcStartPage					&&
		      (*dc->dcStartPage)( through, dg, dc,
						    lpHere->lpPage )	)
		    { LDEB(1); return -1;	}

		if  ( docDrawShapesForPage(
				    through, dc, belowText, lpHere->lpPage ) )
		    { LDEB(lpHere->lpPage);	}

		if  ( dc->dcDrawExternalItems				&&
		      ! dc->dcPostponeHeadersFooters			&&
		      docDrawPageHeader( nextBodyNode, through, dc,
						    lpHere->lpPage )	)
		    { LDEB(lpHere->lpPage); return -1;	}
		}

	    if  ( lpHere->lpColumn > 0					&&
		  sp->spLineBetweenColumns				&&
		  dc->dcDrawOrnaments					&&
		  docDrawLineLeftOfColumn( nextBodyNode, through,
						    lpHere, bf, dc )	)
		{ LDEB(sp->spLineBetweenColumns);	}
	    }
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Skip to the column where a BufferItem begins.			*/
/*									*/
/*  1)  No activity before the first page to be printed.		*/
/*									*/
/************************************************************************/

int docDrawToColumnOfNode(		BufferItem *		prevBodyNode,
					BufferItem *		thisBodyNode,
					BufferItem *		thisNode,
					void *			through,
					LayoutPosition *	lpHere,
					DrawingContext *	dc,
					const BlockOrigin *	bo )
    {
    int			rval= 0;
    BlockFrame		bf;
    LayoutPosition	lpTop;

    docLayoutInitBlockFrame( &bf );

if  ( thisNode->biTreeType == DOCinSHPTXT )
    { goto ready;	}

    docShiftPosition( &lpTop, bo, &(thisNode->biTopPosition) );

    /*  1  */
    if  ( lpTop.lpPage < dc->dcFirstPage )
	{ goto ready; }

    while( lpHere->lpPage < lpTop.lpPage )
	{
	if  ( docDrawToNextColumn( prevBodyNode, thisBodyNode, through,
						    lpHere, &bf, dc )	)
	    { SDEB(docLevelStr(thisNode->biLevel)); rval= -1; goto ready; }
	}

    while( lpHere->lpPage == lpTop.lpPage	&&
           lpHere->lpColumn < lpTop.lpColumn	)
	{
	if  ( docDrawToNextColumn( prevBodyNode, thisBodyNode, through,
						    lpHere, &bf, dc )	)
	    { SDEB(docLevelStr(thisNode->biLevel)); rval= -1; goto ready; }
	}

  ready:

    docLayoutCleanBlockFrame( &bf );

    return rval;
    }

static void docTableHeaderRectangle(
				DocumentRectangle *	drPixels,
				const BlockFrame *	bf,
				BufferItem *		node,
				DrawingContext *	dc )
    {
    const LayoutContext *	lc= &(dc->dcLayoutContext);

    LayoutPosition	lpHeader= node->biRowAboveHeaderPosition;

    docGetPixelRectForPos( drPixels, lc,
			    bf->bfContentRect.drX0, bf->bfContentRect.drX1,
			    &lpHeader, &(node->biTopPosition) );

    return;
    }

/************************************************************************/

static int docDrawHeaderForClippedRow(	BufferItem *		childNode,
					void *			through,
					DrawingContext *	dc )
    {
    const LayoutContext *	lc= &(dc->dcLayoutContext);
    int				rval= 0;
    BlockFrame			bf;

    DocumentRectangle		drChild;

    docLayoutInitBlockFrame( &bf );

    docBlockFrameTwips( &bf, childNode, lc->lcDocument,
					    childNode->biTopPosition.lpPage,
					    childNode->biTopPosition.lpColumn );

    docTableHeaderRectangle( &drChild, &bf, childNode, dc );

    if  ( geoIntersectRectangle( &drChild, &drChild, dc->dcClipRect ) )
	{
	int		high;

	if  ( docDrawTableHeader( &high,
		    childNode, &bf, through,
		    dc, &(childNode->biRowAboveHeaderPosition) ) )
	    { LDEB(childNode->biRowPrecededByHeader); rval= -1; goto ready; }
	}

  ready:

    docLayoutCleanBlockFrame( &bf );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Draw the children of a group item.					*/
/*									*/
/*  3)  Do not proceed beyond last page. To illustrate that table	*/
/*	headers fit on one page, the situation where we are forced into	*/
/*	a particular page block is excepted.				*/
/*  4)  If the child is on an accessible page, move to the column of	*/
/*	the child.							*/
/*									*/
/************************************************************************/

static int docDrawGroupNode(	LayoutPosition *		lpBelow,
				const BufferItem *		node,
				void *				through,
				DrawingContext *		dc,
				const BlockOrigin *		bo )
    {
    int				i;
    LayoutPosition		lpHere;

    BufferItem *		prevNode= (BufferItem *)0;

    if  ( node->biChildCount > 0 )
	{ prevNode= node->biChildren[0];	}

    lpHere= node->biTopPosition;
    for ( i= 0; i < node->biChildCount; i++ )
	{
	BufferItem *		childNode= node->biChildren[i];

	if  ( dc->dcClipRect )
	    {
	    DocumentRectangle	drChild;

	    docNodeRectangle( &drChild, childNode, &(dc->dcLayoutContext), bo );

	    if  ( ! geoIntersectRectangle( &drChild, &drChild,
							dc->dcClipRect ) )
		{
		if  ( childNode->biLevel == DOClevROW	&&
		      childNode->biRowPrecededByHeader	)
		    {
		    if  ( docDrawHeaderForClippedRow( childNode, through, dc ) )
			{ LDEB(i); return -1;	}
		    }

		continue;
		}
	    }

	if  ( dc->dcSelection					&&
	      docSelectionSameRoot( dc->dcSelection, childNode )	&&
	      docCompareNodePositionToSelection( childNode,
					dc->dcSelection ) > 0	)
	    { break;	}

	/*  3  */
	if  ( dc->dcLastPage >= 0				&&
	      ! bo->boOverrideFrame				&&
	      childNode->biTopPosition.lpPage > dc->dcLastPage	)
	    { break;	}

	/*  4  */
	if  ( dc->dcFirstPage < 0					||
	      childNode->biBelowPosition.lpPage >= dc->dcFirstPage	)
	    {
	    if  ( docDrawToColumnOfNode( prevNode, childNode, childNode,
					    through, &lpHere, dc, bo ) )
		{ SDEB(docLevelStr(childNode->biLevel)); return -1;	}

	    if  ( docDrawNode( lpBelow, childNode, through, dc ) )
		{ LDEB(i); return -1;	}

	    lpHere= childNode->biBelowPosition;
	    }

	prevNode= childNode;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Draw the document root item.					*/
/*									*/
/************************************************************************/

static int docDrawDocNode(	LayoutPosition *		lpBelow,
				const BufferItem *		docBi,
				void *				through,
				DrawingContext *		dc )
    {
    const LayoutContext *	lc= &(dc->dcLayoutContext);
    BufferDocument *		bd= lc->lcDocument;
    const DocumentProperties *	dp= &(bd->bdProperties);
    const NotesProperties *	npEndnotes= &(dp->dpNotesProps.fepEndnotesProps);

    BlockOrigin			bo;

    docInitBlockOrigin( &bo );

    if  ( docDrawGroupNode( lpBelow, docBi, through, dc, &bo ) )
	{ LDEB(1); return -1;	}

    if  ( docBi->biTreeType == DOCinBODY		&&
	  npEndnotes->npPlacement == FTNplaceDOC_END	)
	{
	if  ( docDrawEndnotesForDocument( lpBelow, through, dc ) )
	    { LDEB(1); return -1;	}
	}

    if  ( dc->dcDrawExternalItems )
	{
	docDrawFootnotesForColumn( lpBelow->lpPage, lpBelow->lpColumn,
								through, dc );
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Draw a section and its dependants.					*/
/*									*/
/************************************************************************/

static int docDrawSectNode(	LayoutPosition *		lpBelow,
				const BufferItem *		sectNode,
				void *				through,
				DrawingContext *		dc )
    {
    const LayoutContext *	lc= &(dc->dcLayoutContext);
    BufferDocument *		bd= lc->lcDocument;
    const DocumentProperties *	dp= &(bd->bdProperties);
    const NotesProperties *	npEndnotes= &(dp->dpNotesProps.fepEndnotesProps);

    BlockOrigin			bo;

    docInitBlockOrigin( &bo );

    if  ( docDrawGroupNode( lpBelow, sectNode, through, dc, &bo ) )
	{ LDEB(1); return -1;	}

    if  ( sectNode->biTreeType == DOCinBODY		&&
	  npEndnotes->npPlacement == FTNplaceSECT_END	)
	{
	if  ( docDrawEndnotesForSection( lpBelow, sectNode->biNumberInParent,
							    through, dc ) )
	    { LDEB(1); return -1;	}
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Draw an arbitrary item.						*/
/*									*/
/*  1)  Exception for table rows with 'rowspan' cells.			*/
/*  2)  Intersects clipping rectangle?					*/
/*  3)  Inside selection to draw?					*/
/*  4)  Inside page range to draw.					*/
/*									*/
/************************************************************************/

int docDrawNode(	LayoutPosition *		lpBelow,
			BufferItem *			node,
			void *				through,
			DrawingContext *		dc )
    {
    const DocumentSelection *	ds= dc->dcSelection;

    LayoutPosition		lpBelowNodeOffspring;

    BlockOrigin			bo;

    docInitBlockOrigin( &bo );

    /*  1  */
    lpBelowNodeOffspring= node->biBelowPosition;
    if  ( docIsRowNode( node ) )
	{ lpBelowNodeOffspring= node->biRowBelowAllCellsPosition;	}

    /*  2  */
    if  ( dc->dcClipRect )
	{
	DocumentRectangle	drNode;

	docNodeRectangle( &drNode, node, &(dc->dcLayoutContext), &bo );

	if  ( ! geoIntersectRectangle( &drNode, &drNode, dc->dcClipRect )  )
	    { return 0;	}
	}

    /*  3  */
    if  ( ds && docSelectionSameRoot( ds, node ) )
	{
	if  ( docCompareNodePositionToSelection( node, ds ) != 0 )
	    { return 0;	}
	}

    /*  4  */
    if  ( dc->dcFirstPage >= 0				&&
	  lpBelowNodeOffspring.lpPage < dc->dcFirstPage		)
	{ return 0;	}

    if  ( dc->dcLastPage >= 0				&&
	  node->biTopPosition.lpPage > dc->dcLastPage	)
	{ return 0;	}

    if  ( lpBelow )
	{
	docLayoutPushBottomDown( lpBelow, &(node->biTopPosition) );
	}

    switch( node->biLevel )
	{
	case DOClevBODY:
	    if  ( docDrawDocNode( lpBelow, node, through, dc ) )
		{ LDEB(1); return -1;	}
	    break;

	case DOClevSECT:
	    if  ( docDrawSectNode( lpBelow, node, through, dc ) )
		{ LDEB(1); return -1;	}
	    break;

	case DOClevCELL:
	rowAsGroup:
	    if  ( docDrawGroupNode( lpBelow, node, through, dc, &bo ) )
		{ LDEB(1); return -1;	}
	    break;

	case DOClevROW:
	    if  ( ! docIsRowNode( node ) )
		{ goto rowAsGroup;	}

	    if  ( docDrawRowNode( node, through, dc, &bo ) )
		{ LDEB(1); return -1;	}
	    break;

	case DOClevPARA:
	    if  ( docDrawParaNode( lpBelow, node, through, dc, &bo ) )
		{ LDEB(1); return -1;	}
	    break;

	default:
	    LDEB(node->biLevel); return -1;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Draw a series of pages.						*/
/*									*/
/************************************************************************/

int docDrawPageRange(	DrawingContext *	dc,
			void *			through )
    {
    BufferItem * const		rootNode= dc->dcLayoutContext.lcDocument->bdBody.dtRoot;

    int				sectNr;
    int				page;

    if  ( docDrawNode( (LayoutPosition *)0, rootNode, through, dc ) )
	{ LDEB(1);	}

    for ( sectNr= 0; sectNr < rootNode->biChildCount; sectNr++ )
	{
	BufferItem *	bodySectNode= rootNode->biChildren[sectNr];
	BufferItem *	prevSectNode= (BufferItem *)0;
	int		first= bodySectNode->biTopPosition.lpPage;
	int		last= bodySectNode->biBelowPosition.lpPage;

	if  ( first < dc->dcFirstPage )
	    { first=  dc->dcFirstPage;	}
	if  ( last >  dc->dcLastPage )
	    { last=   dc->dcLastPage;		}

	if  ( sectNr > 0 )
	    { prevSectNode= rootNode->biChildren[sectNr- 1];	}
	/*
	Word draws the footer of the first section on the page.
	if  ( sectNr < rootNode->biChildCount- 1 )
	    { nextSectBi= rootNode->biChildren[sectNr+ 1];	}
	*/

	for ( page= first; page <= last; page++ )
	    {
	    int		col;
	    const int	belowText= 0;

	    if  ( ! prevSectNode				||
		  prevSectNode->biBelowPosition.lpPage < page	)
		{ docDrawPageHeader( bodySectNode, through, dc, page ); }

	    for ( col= 0; col < bodySectNode->biSectColumnCount; col++ )
		{
		docDrawFootnotesForColumn( page, col, through, dc );
		}

	    /* Word draws the footer of the first section on the page. */
	    if  ( ! prevSectNode				||
		  prevSectNode->biBelowPosition.lpPage < page	)
		{ docDrawPageFooter( bodySectNode, through, dc, page ); }

	    if  ( docDrawShapesForPage( through, dc, belowText, page ) )
		{ LDEB(page);	}
	    }
	}

    return 0;
    }

