/************************************************************************/
/*									*/
/*  Geometry calculations about external items.				*/
/*									*/
/************************************************************************/

#   include	"docLayoutConfig.h"

#   include	<stddef.h>

#   include	"docLayout.h"
#   include	"docSelectLayout.h"
#   include	<docPageGrid.h>
#   include	<docField.h>
#   include	<docTreeType.h>
#   include	<docTreeNode.h>
#   include	<docTextLine.h>
#   include	<docNotes.h>
#   include	<docRecalculateFields.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Determine the box around a header or a footer.			*/
/*									*/
/************************************************************************/

static int docNoteSeparatorBox(	DocumentRectangle *		dr,
				int				noteItKind,
				int				noteSepKind,
				int				page,
				int				column,
				const LayoutContext *		lc )
    {
    DocumentTree *		eiNoteSep;
    int				y0Twips;
    struct DocumentNote *	dnFirstNote;
    DocumentField *		dfNote;

    dfNote= docGetFirstNoteInColumn( &dnFirstNote, lc->lcDocument, 
						page, column, noteItKind );

    if  ( ! dfNote )
	{ XDEB(dfNote); return -1;	}

    if  ( docNoteSeparatorRectangle( dr, &eiNoteSep, &y0Twips,
					    dnFirstNote, noteSepKind, lc ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

int docGetBoxAroundTree(	DocumentRectangle *		dr,
				const BufferItem *		bodySectNode,
				const DocumentTree *		tree,
				int				justUsed,
				int				page,
				int				column,
				const LayoutContext *		lc )
    {
    int				rval= 0;
    BufferItem *		treeRoot= tree->dtRoot;

    BlockFrame			bf;
    DocumentRectangle		drTwips;
    DocumentRectangle		drBox;

    docLayoutInitBlockFrame( &bf );

    if  ( ! treeRoot )
	{ XDEB(treeRoot); rval= -1; goto ready;	}

    switch( treeRoot->biTreeType )
	{
	case DOCinFIRST_HEADER:
	case DOCinLEFT_HEADER:
	case DOCinRIGHT_HEADER:
	case DOCinFIRST_FOOTER:
	case DOCinLEFT_FOOTER:
	case DOCinRIGHT_FOOTER:

	    docBlockFrameTwips( &bf, treeRoot, lc->lcDocument, page, column );

	    drTwips= bf.bfContentRect;

	    if  ( justUsed )
		{
		drTwips.drY0= tree->dtY0UsedTwips;
		drTwips.drY1= tree->dtY1UsedTwips;
		}
	    else{
		drTwips.drY0= tree->dtY0ReservedTwips;
		drTwips.drY1= tree->dtY1ReservedTwips;
		}

	    docGetPixelRect( &drBox, lc, &drTwips, page );

	    *dr= drBox; break;

	case DOCinFOOTNOTE:
	case DOCinENDNOTE:

	    {
	    DocumentPosition	dpTop;
	    DocumentPosition	dpBot;

	    int			lineTop;
	    int			lineBot;

	    int			partTop;
	    int			partBot;

	    const TextLine *	tlTop;
	    const TextLine *	tlBot;

	    LayoutPosition	lpTop;
	    LayoutPosition	lpBot;

	    docBlockFrameTwips( &bf, tree->dtRoot, lc->lcDocument,
							    page, column );

	    if  ( docGetFirstInColumnForNode( &dpTop, &lineTop, &partTop,
						tree->dtRoot, page, column ) )
		{
		LLDEB(page,column);
		SDEB(docTreeTypeStr(tree->dtRoot->biTreeType));
		rval= -1; goto ready;
		}

	    if  ( docGetLastInColumnForNode( &dpBot, &lineBot, &partBot,
						tree->dtRoot, page, column ) )
		{ LDEB(page); rval= -1; goto ready;	}

	    tlTop= dpTop.dpNode->biParaLines+ lineTop;
	    tlBot= dpBot.dpNode->biParaLines+ lineBot;

	    lpTop= tlTop->tlTopPosition;
	    lpBot= tlBot->tlTopPosition;
	    lpBot.lpPageYTwips += tlBot->tlLineStride;

	    docGetPixelRectForPos( &drBox, lc,
			     bf.bfContentRect.drX0, bf.bfContentRect.drX1,
			     &lpTop, &lpBot );
	    }

	    *dr= drBox; break;

	case DOCinFTNSEP:
	case DOCinFTNSEPC:
	case DOCinFTNCN:

	    if  ( docNoteSeparatorBox( &drBox,
			    DOCinFOOTNOTE, treeRoot->biTreeType,
			    page, column, lc ) )
		{ LLDEB(page,column); rval= -1; goto ready;	}

	    *dr= drBox; break;

	case DOCinAFTNSEP:
	case DOCinAFTNSEPC:
	case DOCinAFTNCN:

	    if  ( docNoteSeparatorBox( &drBox,
			    DOCinENDNOTE, treeRoot->biTreeType,
			    page, column, lc ) )
		{ LLDEB(page,column); rval= -1; goto ready;	}

	    *dr= drBox; break;

	default:
	    LDEB(treeRoot->biTreeType);
	    rval= -1; goto ready;
	}

  ready:

    docLayoutCleanBlockFrame( &bf );

    return rval;
    }

/************************************************************************/

static void docPlaceHeader(	DocumentTree *			tree,
				const DocumentGeometry *	dgSect )
    {
    tree->dtY0UsedTwips= tree->dtRoot->biTopPosition.lpPageYTwips;
    tree->dtY1UsedTwips= tree->dtRoot->biBelowPosition.lpPageYTwips;

    tree->dtY0ReservedTwips= dgSect->dgHeaderPositionTwips;
    tree->dtY1ReservedTwips= dgSect->dgTopMarginTwips;

    if  ( tree->dtY1ReservedTwips < tree->dtY1UsedTwips )
	{ tree->dtY1ReservedTwips=  tree->dtY1UsedTwips;	}

    return;
    }

static void docPlaceFooter(	DocumentTree *			tree,
				const DocumentGeometry *	dgSect )
    {
    int	high= tree->dtRoot->biBelowPosition.lpPageYTwips-
			    tree->dtRoot->biTopPosition.lpPageYTwips;

    tree->dtY1UsedTwips=
		dgSect->dgPageHighTwips- dgSect->dgFooterPositionTwips;
    tree->dtY0UsedTwips= tree->dtY1UsedTwips- high;

    tree->dtY0ReservedTwips=
		dgSect->dgPageHighTwips- dgSect->dgBottomMarginTwips;
    tree->dtY1ReservedTwips=
		dgSect->dgPageHighTwips- dgSect->dgFooterPositionTwips;

    if  ( tree->dtY0ReservedTwips > tree->dtY0UsedTwips )
	{ tree->dtY0ReservedTwips=  tree->dtY0UsedTwips;	}

    return;
    }

/************************************************************************/
/*									*/
/*  Do a preliminary layout of a document tree.				*/
/*									*/
/*  Inside the tree, geometry is correct. Some trees are however used	*/
/*  in different positions and here we just calculate the layout in	*/
/*  order to know the size of the tree to use it in geometry		*/
/*  calculations about the document as a whole.				*/
/*									*/
/*  1)  Remove the bottom of the page master frame. This is the routine	*/
/*	that calculates it for future use.				*/
/*									*/
/************************************************************************/

int docTreePrelayout(		DocumentTree *		tree,
				const BufferItem *	bodySectNode,
				LayoutJob *		lj )
    {
    int				rval= 0;
    const LayoutContext *	lc= &(lj->ljContext);
    const DocumentGeometry *	dgSect= &(bodySectNode->biSectDocumentGeometry);
    LayoutJob			treeLj;
    LayoutPosition		treeLp;

    BlockFrame			bf;

    const int			recursively= 0;

    docLayoutInitBlockFrame( &bf );

    docInvalidateTreeLayout( tree );

    if  ( ! tree->dtRoot )
	{ goto ready;	}

    docDelimitTables( tree->dtRoot, recursively );

    tree->dtRoot->biSectDocumentGeometry.dgPageWideTwips=
						dgSect->dgPageWideTwips;
    tree->dtRoot->biSectDocumentGeometry.dgLeftMarginTwips=
						dgSect->dgLeftMarginTwips;
    tree->dtRoot->biSectDocumentGeometry.dgRightMarginTwips=
						dgSect->dgRightMarginTwips;

    treeLp.lpPage= tree->dtRoot->biTopPosition.lpPage;
    treeLp.lpColumn= 0;
    treeLp.lpPageYTwips= dgSect->dgHeaderPositionTwips;
    treeLp.lpAtTopOfColumn= 1; /* not really */

    treeLj= *lj;
    treeLj.ljChangedRectanglePixels= (DocumentRectangle *)0;

    treeLj.ljBodySectNode= bodySectNode;
    treeLj.ljChangedNode= lj->ljChangedNode;

    docBlockFrameTwips( &bf, tree->dtRoot, lc->lcDocument,
					treeLp.lpPage, treeLp.lpColumn );
    bf.bfContentRect.drY0= treeLp.lpPageYTwips;
    bf.bfFlowRect.drY0= bf.bfContentRect.drY0;

    /*  1  */
    bf.bfContentRect.drY1= dgSect->dgPageHighTwips;
    bf.bfFlowRect.drY1= bf.bfContentRect.drY1;

    if  ( docLayoutNodeImplementation( &treeLp, &treeLp,
						tree->dtRoot, &bf, &treeLj ) )
	{ LDEB(1); rval= -1; goto ready;	}

    switch( tree->dtRoot->biTreeType )
	{
	case DOCinFIRST_HEADER:
	case DOCinLEFT_HEADER:
	case DOCinRIGHT_HEADER:
	    docPlaceHeader( tree, dgSect );
	    break;

	case DOCinFIRST_FOOTER:
	case DOCinLEFT_FOOTER:
	case DOCinRIGHT_FOOTER:
	    docPlaceFooter( tree, dgSect );
	    break;

	case DOCinFOOTNOTE:
	case DOCinENDNOTE:

	    /*  temporarily: will be placed later on */
	    tree->dtY0UsedTwips= tree->dtRoot->biTopPosition.lpPageYTwips;
	    tree->dtY1UsedTwips= tree->dtRoot->biBelowPosition.lpPageYTwips;
	    break;

	case DOCinFTNSEP:
	case DOCinFTNSEPC:
	case DOCinFTNCN:
	case DOCinAFTNSEP:
	case DOCinAFTNSEPC:
	case DOCinAFTNCN:

	    /*  temporarily */
	    tree->dtY0UsedTwips= tree->dtRoot->biTopPosition.lpPageYTwips;
	    tree->dtY1UsedTwips= tree->dtRoot->biBelowPosition.lpPageYTwips;
	    break;

	default:
	    LDEB(tree->dtRoot->biTreeType); rval= -1; goto ready;
	}

  ready:

    docLayoutCleanBlockFrame( &bf );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Do the preliminary layout of the headers and footers of a section	*/
/*									*/
/************************************************************************/

int docSectHeaderFooterPrelayout(	BufferItem *	bodySectNode,
					LayoutJob *	lj )
    {
    int				hdft;
    int				redraw= 0;

    for ( hdft= 0; hdft < DOC_HeaderFooterTypeCount; hdft++ )
	{
	DocumentTree *		dtHdFt;
	unsigned char		applies;

	int			resY0;
	int			resY1;

	dtHdFt= docSectionHeaderFooter( bodySectNode, &applies,
				    &(lj->ljContext.lcDocument->bdProperties),
				    DOC_HeaderFooterTypes[hdft] );
	if  ( ! dtHdFt )
	    { XDEB(dtHdFt); return -1;	}

	resY0= dtHdFt->dtY0ReservedTwips;
	resY1= dtHdFt->dtY1ReservedTwips;

	if  ( ! dtHdFt->dtRoot || ! applies )
	    {
	    dtHdFt->dtY0ReservedTwips= 0;
	    dtHdFt->dtY1ReservedTwips= 0;
	    }
	else{
	    if  ( docTreePrelayout( dtHdFt, bodySectNode, lj ) )
		{ LDEB(1); return -1;	}
	    }

	if  ( lj->ljChangedRectanglePixels			&&
	      ( dtHdFt->dtY0ReservedTwips != resY0	||
	        dtHdFt->dtY1ReservedTwips != resY1	)	)
	    { redraw++;	}
	}

    if  ( redraw )
	{
	DocumentRectangle	drPixels;

	docGetPixelRectangleForPages( &drPixels, &(lj->ljContext),
					bodySectNode->biTopPosition.lpPage,
					bodySectNode->biBelowPosition.lpPage );

	geoUnionRectangle( lj->ljChangedRectanglePixels,
				    lj->ljChangedRectanglePixels, &drPixels );
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Reset page dependent layout administration of headers/footers etc	*/
/*  fo force a subsequent recalculation.				*/
/*									*/
/************************************************************************/

void docResetExternalTreeLayout(	BufferDocument *	bd )
    {
    int		i;

    docInvalidateTreeLayout( &(bd->bdEiFtnsep) );
    docInvalidateTreeLayout( &(bd->bdEiFtnsepc) );
    docInvalidateTreeLayout( &(bd->bdEiFtncn) );

    docInvalidateTreeLayout( &(bd->bdEiAftnsep) );
    docInvalidateTreeLayout( &(bd->bdEiAftnsepc) );
    docInvalidateTreeLayout( &(bd->bdEiAftncn) );

    for ( i= 0; i < bd->bdBody.dtRoot->biChildCount; i++ )
	{
	BufferItem *	bodySectNode=  bd->bdBody.dtRoot->biChildren[i];

	docInvalidateSectHeaderFooterLayout( bodySectNode );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Verify that the root of a selection is formatted for the current	*/
/*  page, if not format it.						*/
/*									*/
/************************************************************************/

static int docGetY0ForSelectedNoteSeparator(
					int *			pY0Twips,
					BufferItem **		pBodySectNode,
					const LayoutContext *	lc,
					const DocumentTree *	selTree,
					int			noteItKind,
					int			sepItKind )
    {
    struct DocumentNote *	dnFirstNote;
    DocumentTree *		eiBody;
    DocumentTree *		eiNoteSep;
    int				y0Twips;

    DocumentRectangle		drExtern;

    DocumentField *		dfNote;
    BufferItem *		bodySectNode;

    dfNote= docGetFirstNoteInColumn( &dnFirstNote, lc->lcDocument, 
					    selTree->dtPageSelectedUpon,
					    selTree->dtColumnSelectedIn,
					    noteItKind );
    if  ( ! dfNote )
	{
	LLDEB(selTree->dtPageSelectedUpon,selTree->dtColumnSelectedIn);
	return -1;
	}

    if  ( docGetRootOfSelectionScope( &eiBody, &bodySectNode, lc->lcDocument,
						&(dfNote->dfSelectionScope) ) )
	{ LDEB(1); return -1;	}

    if  ( docNoteSeparatorRectangle( &drExtern, &eiNoteSep,
				    &y0Twips, dnFirstNote, sepItKind, lc ) )
	{ LDEB(1); return -1;	}

    *pY0Twips= y0Twips;
    *pBodySectNode= bodySectNode;
    return 0;
    }

int docCheckPageOfSelectedTree(	int *			pChanged,
				BufferItem **		pBodySectNode,
				DocumentRectangle *	drChanged,
				DocumentTree *		selTree,
				const LayoutContext *	lc,
				INIT_LAYOUT_EXTERNAL	initLayoutExternal )
    {
    int				y0Twips;
    BufferItem *		selRootBodySectNode= (BufferItem *)0;
    const SelectionScope *	selRootScope;
    const int			adjustDocument= 0;

    if  ( ! selTree->dtRoot )
	{ XDEB(selTree->dtRoot); return -1;	}

    selRootScope= &(selTree->dtRoot->biSectSelectionScope);

    switch( selTree->dtRoot->biTreeType )
	{
	case DOCinBODY:
	    LDEB(selTree->dtRoot->biTreeType);
	    return -1;

	case DOCinFIRST_HEADER:
	case DOCinLEFT_HEADER:
	case DOCinRIGHT_HEADER:

	case DOCinFIRST_FOOTER:
	case DOCinLEFT_FOOTER:
	case DOCinRIGHT_FOOTER:

	    selRootBodySectNode= docGetBodySectNodeOfScope( selRootScope,
							    lc->lcDocument );

	    y0Twips= selTree->dtY0UsedTwips;
	    break;

	case DOCinFOOTNOTE:
	case DOCinENDNOTE:

	    selRootBodySectNode= docGetBodySectNodeOfScope( selRootScope,
							    lc->lcDocument );

	    *pBodySectNode= selRootBodySectNode;
	    *pChanged= 0;
	    return 0;

	case DOCinFTNSEP:

	    if  ( selTree->dtPageSelectedUpon < 0 )
		{ LDEB(selTree->dtPageSelectedUpon); return -1;	}

	    if  ( docGetY0ForSelectedNoteSeparator( &y0Twips,
						&selRootBodySectNode,
						lc, selTree,
						DOCinFOOTNOTE, DOCinFTNSEP ) )
		{ LDEB(1); return -1;	}

	    break;

	case DOCinFTNSEPC:
	case DOCinFTNCN:
	    LDEB(selTree->dtRoot->biTreeType);
	    return -1;

	case DOCinAFTNSEP:

	    if  ( selTree->dtPageSelectedUpon < 0 )
		{ LDEB(selTree->dtPageSelectedUpon); return -1;	}

	    if  ( docGetY0ForSelectedNoteSeparator( &y0Twips,
						&selRootBodySectNode,
						lc, selTree,
						DOCinENDNOTE, DOCinAFTNSEP ) )
		{ LDEB(1); return -1;	}

	    break;

	case DOCinAFTNSEPC:
	case DOCinAFTNCN:
	    SDEB(docTreeTypeStr(selTree->dtRoot->biTreeType));
	    return -1;

	default:
	    LDEB(selTree->dtRoot->biTreeType);
	    return -1;
	}

    if  ( selTree->dtPageSelectedUpon < 0 )
	{ LSDEB(selTree->dtPageSelectedUpon,docTreeTypeStr(selTree->dtRoot->biTreeType)); return -1;	}

    if  ( selTree->dtPageFormattedFor == selTree->dtPageSelectedUpon &&
	  selTree->dtColumnFormattedFor == selTree->dtColumnSelectedIn )
	{
	*pBodySectNode= selRootBodySectNode;
	*pChanged= 0;
	return 0;
	}

    /*  We do not expect the tree to change height here	*/
    if  ( docLayoutDocumentTree( selTree, drChanged,
				    selTree->dtPageSelectedUpon,
				    selTree->dtColumnSelectedIn,
				    y0Twips, selRootBodySectNode, lc,
				    initLayoutExternal, adjustDocument ) )
	{ LDEB(selTree->dtPageSelectedUpon); return -1; }

    *pBodySectNode= selRootBodySectNode;
    *pChanged= 1; return 0;
    }

/************************************************************************/
/*									*/
/*  Calculate the layout of a document tree such as a page header or	*/
/*  footer. NOT called for the document body.				*/
/*									*/
/************************************************************************/

int docLayoutDocumentTree(	DocumentTree *		tree,
				DocumentRectangle *	drChanged,
				int			page,
				int			column,
				int			y0Twips,
				const BufferItem *	bodySectNode,
				const LayoutContext *	lc,
				INIT_LAYOUT_EXTERNAL	initLayoutExternal,
				int			adjustDocument )
    {
    int				rval= 0;

    RecalculateFields		rf;
    LayoutJob			lj;
    BlockFrame			bf;

    LayoutPosition		lpHere;

    int				oldY1= tree->dtY1UsedTwips;

    docLayoutInitBlockFrame( &bf );
    docInitRecalculateFields( &rf );
    docInitLayoutJob( &lj );

    if  ( page == tree->dtPageFormattedFor	&&
	  column == tree->dtColumnFormattedFor	)
	{ goto ready;	}

    rf.rfDocument= lc->lcDocument;
    rf.rfCloseObject= lc->lcCloseObject;
    rf.rfUpdateFlags=
	    FIELDdoDOC_FORMATTED|FIELDdoDOC_COMPLETE|FIELDdoPAGE_NUMBER;
    rf.rfFieldsUpdated= 0;
    rf.rfBodySectNode= bodySectNode;

    if  ( docRecalculateTextLevelFieldsInDocumentTree( &rf, tree,
						    bodySectNode, page ) )
	{ LDEB(page); return -1;	}

    lpHere.lpPage= page;
    lpHere.lpColumn= column;
    lpHere.lpPageYTwips= y0Twips;
    lpHere.lpAtTopOfColumn= 1; /* not really */

    lj.ljBodySectNode= bodySectNode;

    lj.ljChangedRectanglePixels= drChanged;
    lj.ljContext= *lc;
    /*
    lj.ljChangedNode= lc->lcDocument->bdBody.dtRoot;
    */
    lj.ljChangedNode= tree->dtRoot;

    if  ( initLayoutExternal 					&&
	  (*initLayoutExternal)( &lj, tree, page, column )	)
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( docLayoutGetInitialFrame( &bf, &lj, &lpHere, tree->dtRoot ) )
	{ LDEB(1); return -1;	}

    docLayoutAdjustFrame( &bf, tree->dtRoot );

    if  ( docLayoutNodeImplementation( &lpHere, &lpHere,
						tree->dtRoot, &bf, &lj ) )
	{ LDEB(1); rval= -1; goto ready; }

    tree->dtPageFormattedFor= page;
    tree->dtColumnFormattedFor= column;
    tree->dtY0UsedTwips= y0Twips;
    tree->dtY1UsedTwips= lpHere.lpPageYTwips;

    if  ( adjustDocument			&&
	  tree->dtY0UsedTwips != oldY1		)
	{
	const DocumentGeometry * dgSect= &(bodySectNode->biSectDocumentGeometry);
	if  ( docIsHeaderType( tree->dtRoot->biTreeType ) )
	    {
	    docPlaceHeader( tree, dgSect );
	    }

	if  ( docIsFooterType( tree->dtRoot->biTreeType ) )
	    {
	    docPlaceFooter( tree, dgSect );
	    }

	if  ( docAdjustLayoutToChangedTree( &lpHere, tree->dtRoot, &lj ) )
	    { LDEB(1); return -1;	}
	}

  ready:

    docCleanLayoutJob( &lj );

    return rval;
    }

