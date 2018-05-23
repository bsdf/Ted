/************************************************************************/
/*									*/
/*  Paragraph Layout related administration.				*/
/*									*/
/************************************************************************/

#   include	"docLayoutConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>

#   include	<psFontMetrics.h>
#   include	<docPageGrid.h>
#   include	"docLayout.h"
#   include	"docLayoutObject.h"
#   include	"docParagraphLayout.h"
#   include	"docRowLayout.h"
#   include	<docTreeType.h>
#   include	<docTreeNode.h>
#   include	<docNodeTree.h>
#   include	<docDebug.h>
#   include	<docTextParticule.h>
#   include	<docPropertiesAdmin.h>

#   include	<appDebugon.h>

/************************************************************************/

static int docGetParaTopBorder(	BorderProperties *		bpTop,
				int *				pNrAbove,
				int *				pFillBefore,
				const BufferDocument *		bd,
				const BufferItem *		paraNode,
				const BufferItem *		cellNode )
    {
    int				rval= 0;
    const BufferItem *		prevNode= (const BufferItem *)0;

    if  ( paraNode->biNumberInParent > 0 )
	{ prevNode= cellNode->biChildren[paraNode->biNumberInParent- 1]; }

    /*  5  */
    if  ( docBorderNumberIsBorder( bd, paraNode->biParaTopBorderNumber ) )
	{
	if  ( ! prevNode						||
	      prevNode->biParaFrameNumber != paraNode->biParaFrameNumber ||
	      prevNode->biParaTopBorderNumber !=
					paraNode->biParaTopBorderNumber	)
	    {
	    docGetBorderPropertiesByNumber( bpTop, bd,
					    paraNode->biParaTopBorderNumber );

	    *pNrAbove= paraNode->biParaTopBorderNumber;
	    rval= 1;
	    }

	if  ( prevNode							&&
	      prevNode->biParaFrameNumber ==
				      paraNode->biParaFrameNumber	&&
	      prevNode->biParaBottomBorderNumber ==
				    paraNode->biParaBottomBorderNumber	)
	    { *pFillBefore= 1;	}
	}

    if  ( prevNode							&&
	  docShadingNumberIsShading( bd, paraNode->biParaShadingNumber ) &&
	  docShadingNumberIsShading( bd, prevNode->biParaShadingNumber ) )
	{ *pFillBefore= 1;	}

    return rval;
    }

static int docGetParaBottomBorder( BorderProperties *		bpBottom,
				int *				pNrBelow,
				int *				pFillAfter,
				const BufferDocument *		bd,
				const BufferItem *		paraNode,
				const BufferItem *		cellNode )
    {
    int				rval= 0;
    const BufferItem *		nextNode= (const BufferItem *)0;

    if  ( paraNode->biNumberInParent < cellNode->biChildCount- 1 )
	{ nextNode= cellNode->biChildren[paraNode->biNumberInParent+ 1]; }

    if  ( docBorderNumberIsBorder( bd, paraNode->biParaBottomBorderNumber ) )
	{
	if  ( ! nextNode						||
	      nextNode->biParaFrameNumber != paraNode->biParaFrameNumber ||
	      nextNode->biParaBottomBorderNumber !=
				    paraNode->biParaBottomBorderNumber	)
	    {
	    docGetBorderPropertiesByNumber( bpBottom, bd,
					paraNode->biParaBottomBorderNumber );

	    *pNrBelow= paraNode->biParaBottomBorderNumber;
	    rval= 1;
	    }

	if  ( nextNode							&&
	      nextNode->biParaFrameNumber == paraNode->biParaFrameNumber &&
	      nextNode->biParaBottomBorderNumber ==
				    paraNode->biParaBottomBorderNumber	)
	    { *pFillAfter= 1;	}
	}

    if  ( nextNode							&&
	  docShadingNumberIsShading( bd, paraNode->biParaShadingNumber ) &&
	  docShadingNumberIsShading( bd, nextNode->biParaShadingNumber ) )
	{ *pFillAfter= 1;	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Calculate the height of a series of lines in a paragraph.		*/
/*									*/
/*  1)  The first paragraph in a cell must accomodate space for the	*/
/*	top border of the cells of the row and the cell padding.	*/
/*  2)  Allocate space for the fattest top border in the row.		*/
/*  3)  Allocate space for the row padding.				*/
/*  4)  Allocate space for the cell padding.				*/
/*  5)  Allocate space for the paragraph top border.			*/
/*									*/
/************************************************************************/

void docLayoutCalculateParaTopInset(
				const BufferDocument *	bd,
				BufferItem *		paraNode )
    {
    int				topInset= 0;
    int				nrAbove= -1;
    BorderProperties		bpTop;
    const BufferItem *		cellNode= paraNode->biParent;

    int				cellMargin= 0;
    int				fillBefore= 0;

    topInset= paraNode->biParaSpaceBeforeTwips;

    /*  1  */
    if  ( paraNode->biParaTableNesting > 0	&&
	  paraNode->biNumberInParent == 0	)
	{
	const BufferItem *	rowNode= cellNode->biParent;

	const RowProperties *	rp= &(rowNode->biRowProperties);
	const CellProperties *	cp= rp->rpCells+ cellNode->biNumberInParent;

	if  ( ! docIsRowNode( rowNode ) )
	    { LDEB(1); cp= (const CellProperties *)0;	}

	/*  2  */
	cellMargin += rowNode->biRowTopInset;

	/*  3  */
	switch( rp->rpTopCellPaddingUnit )
	    {
	    case TRautoNONE:
		break;

	    case TRautoTWIPS:
		cellMargin= rp->rpTopCellPadding;
		break;

	    default:
		LDEB(rp->rpTopCellPaddingUnit);
		break;
	    }

	/*  4  */
	if  ( cp ) switch( cp->cpTopPaddingUnit )
	    {
	    case TRautoNONE:
		break;

	    case TRautoTWIPS:
		cellMargin= cp->cpTopPadding;
		break;

	    default:
		LDEB(cp->cpTopPaddingUnit);
		break;
	    }
	}

    if  ( paraNode->biParaTableNesting == 0	&&
	  paraNode->biNumberInParent == 0	&&
	  cellNode->biNumberInParent == 0	)
	{
	const BufferItem *	rowNode= cellNode->biParent;

	topInset += rowNode->biRowTopInset;
	}

    /*  5  */
    if  ( docGetParaTopBorder( &bpTop, &nrAbove, &fillBefore,
						    bd, paraNode, cellNode ) )
	{ docAddBorderToInset( &topInset, &bpTop ); 		}

    paraNode->biParaTopInset= cellMargin+ topInset;
    paraNode->biParaBorderNrAbove= nrAbove;

    return;
    }

void docLayoutCalculateParaBottomInset(
				    const BufferDocument *	bd,
				    BufferItem *		paraNode )
    {
    int				bottomInset= 0;
    int				nrBelow= -1;
    BorderProperties		bpBottom;
    const BufferItem *		cellNode= paraNode->biParent;
    const BufferItem *		rowNode= cellNode->biParent;

    int				cellMargin= 0;
    int				fillAfter= 0;

    if  ( paraNode->biParaTableNesting > 0				&&
	  cellNode->biNumberInParent <
			    rowNode->biRowProperties.rpCellCount	&&
	  paraNode->biNumberInParent == cellNode->biChildCount- 1	)
	{
	const RowProperties *	rp= &(rowNode->biRowProperties);
	const CellProperties *	cp= rp->rpCells+ cellNode->biNumberInParent;

	switch( rp->rpBottomCellPaddingUnit )
	    {
	    case TRautoNONE:
		break;

	    case TRautoTWIPS:
		cellMargin= rp->rpBottomCellPadding;
		break;

	    default:
		LDEB(rp->rpBottomCellPaddingUnit);
		break;
	    }

	switch( cp->cpBottomPaddingUnit )
	    {
	    case TRautoNONE:
		break;

	    case TRautoTWIPS:
		cellMargin= cp->cpBottomPadding;
		break;

	    default:
		LDEB(cp->cpBottomPaddingUnit);
		break;
	    }
	}

    if  ( docGetParaBottomBorder( &bpBottom, &nrBelow, &fillAfter,
						    bd, paraNode, cellNode ) )
	{ docAddBorderToInset( &bottomInset, &bpBottom ); 		}

    bottomInset += paraNode->biParaSpaceAfterTwips;

    paraNode->biParaBottomInset= bottomInset+ cellMargin;
    paraNode->biParaBorderNrBelow= nrBelow;

    return;
    }

/************************************************************************/
/*									*/
/*  Determine the 'majority' font of a paragraph, and get the font	*/
/*  extents for that font.						*/
/*									*/
/*  1)  Note that subscript/superscript is NOT taken into account.	*/
/*									*/
/************************************************************************/

int docLayoutParagraphLineExtents(
				int *				pFontSizeTwips,
				const LayoutContext *		lc,
				BufferItem *			paraNode )
    {
    BufferDocument *		bd= lc->lcDocument;
    const TextParticule *	tp= paraNode->biParaParticules;

    int				sizeHalfPoints;
    int				y0= 0;
    int				y1= 0;

    int				box;
    int				part;

    static int *		counts;
    int *			fresh;

    int				found;
    int				foundCount;

    DocumentPropertyLists *	dpl= bd->bdPropertyLists;
    const NumberedPropertiesList * tal= &(dpl->dplTextAttributeList);
    int				count= tal->nplPagedList.plItemCount;

    fresh= (int *)realloc( counts, count* sizeof(int) );
    if  ( ! fresh )
	{ LXDEB(count,fresh); return -1;	}
    counts= fresh;

    for ( box= 0; box < count; box++ )
	{ counts[box]= 0;	}

    for ( part= 0; part < paraNode->biParaParticuleCount; tp++, part++ )
	{
	if  ( tp->tpKind != DOCkindSPAN		&&
	      tp->tpKind != DOCkindTAB		&&
	      tp->tpKind != DOCkindOBJECT	)
	    { continue;	}

	if  ( tp->tpTextAttrNr < 0			||
	      tp->tpTextAttrNr >= count	)
	    {
	    LLLDEB(part,tp->tpTextAttrNr,count);
	    docListNode(0,paraNode,0);
	    continue;
	    }

	counts[tp->tpTextAttrNr] += tp->tpStrlen+ 1;
	}

    found= -1;
    foundCount= 0;
    for ( box= 0; box < count; box++ )
	{
	if  ( counts[box] > foundCount )
	    { found= box; foundCount= counts[box];	}
	}

    if  ( found >= 0 )
	{
	DocumentProperties *	dp= &(bd->bdProperties);
	DocumentFontList *	dfl= dp->dpFontList;

	TextAttribute			ta;
	const AfmFontInfo *		afi;
	const IndexSet *		unicodesWanted;

	const int			vswap= 1;
	DocumentRectangle		drFontBBox;
	DocumentRectangle		drFontAscDesc;
	int				fontHigh;
	int				sizeTwips;

	docGetTextAttributeByNumber( &ta, bd, found );

	afi= (*lc->lcGetFontForAttribute)( &unicodesWanted,
					&ta, dfl, lc->lcPostScriptFontList );
	if  ( ! afi )
	    { XDEB(afi); return -1;	}

	sizeHalfPoints= ta.taFontSizeHalfPoints;
	sizeTwips= 10* sizeHalfPoints;

	psFontBBox( &drFontBBox, &drFontAscDesc, sizeTwips, vswap, afi );

	y0= drFontAscDesc.drY0;
	y1= drFontAscDesc.drY1;

	/*LINEDISTANCE: scale the position of the baseline based on the bbox */
	fontHigh= drFontBBox.drY1- drFontBBox.drY0;
	if  ( fontHigh < 2 )
	    { LLDEB(ta.taFontSizeHalfPoints,fontHigh); fontHigh= 2;	}
	y0= ( drFontBBox.drY0* sizeTwips )/ fontHigh;
	y1= ( drFontBBox.drY1* sizeTwips )/ fontHigh;
	}
    else{
	/* LDEB(found); */
	sizeHalfPoints= 24;
	y0= -190;
	y1=   50;
	}

    paraNode->biParaMajorityFontSize= sizeHalfPoints;
    paraNode->biParaMajorityFontAscY0= y0;
    paraNode->biParaMajorityFontDescY1= y1;

    *pFontSizeTwips= 10* sizeHalfPoints;
    return 0;
    }

/************************************************************************/
/*									*/
/*  Invalidate the layout of all paragraphs in the modified range.	*/
/*									*/
/************************************************************************/

void docInvalidateParagraphLayout(	BufferItem *	paraNode )
    {
    paraNode->biParaLineCount= 0;

    paraNode->biParaMajorityFontAscY0= 0;
    paraNode->biParaMajorityFontDescY1= 0;
    paraNode->biParaMajorityFontSize= 0;
    }

void docInvalidateNodeLayout(		BufferItem *	node )
    {
    if  ( node->biLevel == DOClevPARA )
	{ docInvalidateParagraphLayout( node );	}
    else{
	int c;

	for ( c= 0; c < node->biChildCount; c++ )
	    { docInvalidateNodeLayout( node->biChildren[c] );	}
	}
    }

/************************************************************************/
/*									*/
/*  Calculate the top inset after a table.				*/
/*									*/
/*  This routine is also used to calculate the inset that is used to	*/
/*  accomodate space for the bottom border of the previous row. This	*/
/*  inset is not used in real rows, but only in the immediate successor	*/
/*  of a real row.							*/
/*									*/
/*  1)  Reserve space for the bottom border of all cells.		*/
/*									*/
/************************************************************************/

void docLayoutCalculateAfterRowTopInset(	BufferItem *		belowBi,
						const BufferDocument *	bd )
    {
    int				col;
    const BufferItem *		rowNode;
    const CellProperties *	cp;

    if  ( belowBi->biNumberInParent == 0 )
	{ return;	}

    rowNode= belowBi->biParent->biChildren[belowBi->biNumberInParent- 1];
    if  ( ! docIsRowNode( rowNode ) )
	{ return;	}

    belowBi->biRowTopInset= 0;

    /*  1  */
    cp= rowNode->biRowCells;
    for ( col= 0; col < rowNode->biChildCount; cp++, col++ )
	{
	const int		atRowBottom= 1;
	int			useBelow= 0;
	BorderProperties	bpBottom;
	int			bottomNr;

	if  ( CELL_MERGED( cp ) )
	    { continue;	}

	docGetCellBottomBorder( &bpBottom, &bottomNr, &useBelow, bd, rowNode,
							    col, atRowBottom );

	{
	int rti= belowBi->biRowTopInset;
	docStretchInsetForBorder( &rti, &bpBottom );
	belowBi->biRowTopInset= rti;
	}

	}

    return;
    }

/************************************************************************/
/*									*/
/*  Determine paragraph border and shading.				*/
/*									*/
/*  Experimentation with MS-Word revealed that MS-Word includes the	*/
/*  space before/after in the borders and shading between two		*/
/*  paragraphs if either:						*/
/*  a)  Both have the same border.					*/
/*  b)  Both have a shading.						*/
/*									*/
/*  1)	To avoid a trellis effect, we must fill the space above the,	*/
/*	paragraph if the paragraph is part of a series of contiguous	*/
/*	shaded paragraphs without a border between them.		*/
/*  2)	To avoid a trellis effect, we must fill the space below the,	*/
/*	paragraph if the paragraph is part of a series of contiguous	*/
/*	shaded paragraphs without a border between them.		*/
/*									*/
/************************************************************************/

void docGetParaOrnaments(
			BlockOrnaments *		ornaments,
			DocumentRectangle *		drOutside,
			DocumentRectangle *		drInside,
			const DocumentRectangle *	drParaIn,
			const BufferDocument *		bd,
			const BufferItem *		paraNode,
			int				atParaTop,
			int				atParaBottom )
    {
    int				thick;
    int				space;

    int				nrAbove= -1;
    BorderProperties		bpTop;
    int				nrBelow= -1;
    BorderProperties		bpBottom;

    DocumentRectangle		drPara= *drParaIn;

    if  ( atParaTop )
	{
	int		fillBefore= 0;

	docGetParaTopBorder( &bpTop, &nrAbove, &fillBefore,
					    bd, paraNode, paraNode->biParent );

	/*  1  */
	if  ( fillBefore )
	    { drPara.drY0 -= paraNode->biParaSpaceBeforeTwips;	}
	}

    if  ( atParaBottom )
	{
	int		fillAfter= 0;

	docGetParaBottomBorder( &bpBottom, &nrBelow, &fillAfter,
					    bd, paraNode, paraNode->biParent );

	/*  2  */
	if  ( fillAfter )
	    { drPara.drY1 += paraNode->biParaSpaceAfterTwips;	}
	}

    *drOutside= drPara;
    *drInside= drPara;

    if  ( paraNode->biParaShadingNumber != 0 )
	{
	docGetItemShadingByNumber( &(ornaments->boShading), bd,
						paraNode->biParaShadingNumber );

	PROPmaskADD( &(ornaments->boPropMask), ORNdrawSHADE );
	}

    docGetBorderPropertiesByNumber( &(ornaments->boLeftBorder), bd,
					    paraNode->biParaLeftBorderNumber );
    docGetBorderPropertiesByNumber( &(ornaments->boRightBorder), bd,
					    paraNode->biParaRightBorderNumber );

    if  ( atParaTop && nrAbove >= 0 )
	{
	ornaments->boTopBorder= bpTop;
	ornaments->boTopBorderNumber= nrAbove;

	PROPmaskADD( &(ornaments->boPropMask), ORNdrawTOP_BORDER );

	thick= docBorderThick( &space, &(ornaments->boTopBorder) );
	/* No! The paragraph above covers the lower one like the 
	 * tiles on a roof.
	drOutside->drY0= drPara->drY0- space- thick;
	drInside->drY0= drPara->drY0- space;
	*/
	drOutside->drY0= drPara.drY0+ space;
	drInside->drY0= drPara.drY0+ space+ thick;
	}

    if  ( atParaBottom && nrBelow >= 0 )
	{
	ornaments->boBottomBorder= bpBottom;
	ornaments->boBottomBorderNumber= nrBelow;

	PROPmaskADD( &(ornaments->boPropMask), ORNdrawBOTTOM_BORDER );

	thick= docBorderThick( &space, &(ornaments->boBottomBorder) );
	drInside->drY1= drPara.drY1+ space;
	drOutside->drY1= drPara.drY1+ space+ thick;
	}

    if  ( paraNode->biParaLeftBorderNumber != 0 )
	{
	docGetBorderPropertiesByNumber( &(ornaments->boLeftBorder), bd,
					paraNode->biParaLeftBorderNumber );
	ornaments->boLeftBorderNumber= paraNode->biParaLeftBorderNumber;

	PROPmaskADD( &(ornaments->boPropMask), ORNdrawLEFT_BORDER );

	thick= docBorderThick( &space, &(ornaments->boLeftBorder) );
	drInside->drX0= drPara.drX0- space;
	drOutside->drX0= drPara.drX0- space- thick;
	}

    if  ( paraNode->biParaRightBorderNumber != 0 )
	{
	docGetBorderPropertiesByNumber( &(ornaments->boRightBorder), bd,
					paraNode->biParaRightBorderNumber );
	ornaments->boRightBorderNumber= paraNode->biParaRightBorderNumber;

	PROPmaskADD( &(ornaments->boPropMask), ORNdrawRIGHT_BORDER );

	thick= docBorderThick( &space, &(ornaments->boRightBorder) );
	drInside->drX1= drPara.drX1+ space;
	drOutside->drX1= drPara.drX1+ space+ thick;
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Initialisation for paragraph layout.				*/
/*									*/
/*  1)  Scale objects if necessary. If a scale was chenged, cause the	*/
/*	lines in the paragraph to be reformatted. There is no need to	*/
/*	completely invalidate the layout: Font related information is	*/
/*	unaffected.							*/
/*  2)  Calculate global line extent properties if necessary.		*/
/*									*/
/************************************************************************/

static int docStartParagraphLayout(
				const ParagraphFrame *		pf,
				const BlockFrame *		bf,
				BufferItem *			paraNode,
				const LayoutContext *		lc )
    {
    int				part;
    const TextParticule *	tp;
    int				fontSize= 0;
    int				pageHigh;

    pageHigh= bf->bfPageGeometry.dgPageHighTwips- 
			    bf->bfPageGeometry.dgTopMarginTwips-
			    bf->bfPageGeometry.dgBottomMarginTwips;

    /*  1  */
    tp= paraNode->biParaParticules;
    for ( part= 0; part < paraNode->biParaParticuleCount; tp++, part++ )
	{
	InsertedObject *	io;
	int			fixed= 0;
	int			changed= 0;

	if  ( tp->tpKind != DOCkindOBJECT )
	    { continue;	}

	io= docGetObject( lc->lcDocument, tp->tpObjectNumber );
	if  ( ! io )
	    { LPDEB(tp->tpObjectNumber,io); continue;	}

	if  ( docCheckObjectLayout( &fixed, io ) )
	    { LDEB(part); continue;	}

	docLayoutScaleObjectToFitParagraphFrame( &changed,
				    io, pageHigh, &(pf->pfParaContentRect) );

	if  ( fixed || changed )
	    {
	    if  ( lc->lcCloseObject )
		{ (*lc->lcCloseObject)( lc->lcDocument, tp );	}

	    paraNode->biParaLineCount= 0;
	    }
	}

    /*  2  */
    if  ( paraNode->biParaMajorityFontSize == 0				&&
	  docLayoutParagraphLineExtents( &fontSize, lc, paraNode )	)
	{ LDEB(1); return -1;	}

    docLayoutCalculateParaTopInset( lc->lcDocument, paraNode );
    docLayoutCalculateParaBottomInset( lc->lcDocument, paraNode );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Initialize the formatting of a paragraph by determining its frame	*/
/*									*/
/************************************************************************/

int docLayoutStartParagraph(	const LayoutJob *		lj,
				int *				pStopCode,
				BufferItem *			paraNode,
				const BlockFrame *		bf,
				ParagraphLayoutPosition *	plp )
    {
    const LayoutContext *	lc= &(lj->ljContext);
    const BufferItem *		sectBi= paraNode;

    sectBi= docGetSectNode( paraNode );
    if  ( ! sectBi )
	{ XDEB(sectBi); return -1;	}

#   if 0
    /**
      * MS-Word does not do this
      */
    if  ( paraNode->biParaListOverride > 0				&&
	  docAdaptParagraphToListLevel( &indentChanged, paraNode, bd )	)
	{ LDEB(1);		}
#   endif

    /*  1  */
    if  ( paraNode->biTreeType == DOCinBODY			&&
	  paraNode->biParaTableNesting == 0			&&
	  paraNode->biParaBreakKind != DOCibkNONE		&&
	  ! plp->plpPos.lpAtTopOfColumn				)
	{
	switch( paraNode->biParaBreakKind )
	    {
	    case DOCibkCOL:
		*pStopCode= FORMATstopCOLUMN_BREAK;
		break;
	    case DOCibkPAGE:
		*pStopCode= FORMATstopPAGE_BREAK;
		break;

	    default:
		LDEB(paraNode->biParaBreakKind); return -1;
	    }

	return 0;
	}

    docParagraphFrameTwips( &(plp->plpParagraphFrame), bf, paraNode );

    if  ( docStartParagraphLayout( &(plp->plpParagraphFrame),
							bf, paraNode, lc ) )
	{ LDEB(1); return -1;	}

    if  ( lj->ljStartScreenParagraph					&&
	  (*lj->ljStartScreenParagraph)( paraNode,
				    &(plp->plpParagraphFrame), lc )	)
	{ LDEB(1); return -1;	}

    *pStopCode= FORMATstopREADY;
    return 0;
    }

