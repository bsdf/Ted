/************************************************************************/
/*									*/
/*  Layout of a document.						*/
/*									*/
/************************************************************************/

#   include	"docLayoutConfig.h"

#   include	<stddef.h>

#   include	"docLayout.h"
#   include	"docParticuleData.h"

#   include	<docDebug.h>
#   include	<appDebugon.h>
#   include	<docObjectProperties.h>
#   include	<docParaParticules.h>
#   include	<docShape.h>

#   define	SHOW_LINE_CHANGES	0

/************************************************************************/
/*									*/
/*  Layout successive lines of a paragraph.				*/
/*  Claim space above a particular line.				*/
/*									*/
/*  2)  Do not claim 'space before' at the top of the page.		*/
/*									*/
/************************************************************************/

static int docAboveLine(	int *				pStopCode,
				LayoutPosition *		lp,
				const BufferItem *		paraNode,
				int				part,
				const BlockFrame *		bf,
				const ParagraphFrame *		pf )
    {
    int			spaceAboveLineTwips= 0;

    if  ( part == 0 )
	{
	spaceAboveLineTwips= paraNode->biParaTopInset;

	/*  2  */
	if  ( lp->lpAtTopOfColumn )
	    { spaceAboveLineTwips -= paraNode->biParaSpaceBeforeTwips;	}
	}

    if  ( lp->lpPageYTwips+ spaceAboveLineTwips >= bf->bfContentRect.drY1 )
	{ *pStopCode= FORMATstopBLOCK_FULL; return 1;	}

    if  ( lp->lpPageYTwips+ spaceAboveLineTwips >= pf->pfParaContentRect.drY1 )
	{ *pStopCode= FORMATstopFRAME_FULL; return 1;	}

    lp->lpPageYTwips += spaceAboveLineTwips;
    /* still at top */

    return 0;
    }

/************************************************************************/
/*									*/
/*  Layout successive lines of a paragraph.				*/
/*  Claim space below a particular line.				*/
/*									*/
/*  Also decide whether it will fit in the current formatting frame.	*/
/*									*/
/*  1)  Only use the space after paragraph property and the insets	*/
/*	of the table when the paragraph is in a table.			*/
/*  2)  Push the position for the next line down until below any shapes	*/
/*	on this line. Until we implement the various wrap modes, this	*/
/*	is the best we can do.						*/
/*									*/
/************************************************************************/

static int docBelowLine(	int *				pStopCode,
				LayoutPosition *		lp,
				const LayoutPosition *		lpLineTop,
				int				lineStride,
				const BufferItem *		paraNode,
				int				partFrom,
				int				partUpto,
				const BlockFrame *		bf,
				const ParagraphFrame *		pf,
				const NotesReservation *	nrLine )
    {
    int				spaceBelowLineTwips= 0;
    int				lineBottom;
    int				lineHeight;
    LayoutPosition		lpBelowLine;

    int				flowY1WithNotes;

    int				footnoteHeight;

    /*  1  */
    if  ( partUpto == paraNode->biParaParticuleCount			&&
	  paraNode->biParaTableNesting > 0				&&
	  paraNode->biNumberInParent ==
				    paraNode->biParent->biChildCount- 1	)
	{ spaceBelowLineTwips += paraNode->biParaBottomInset; }

    lpBelowLine= *lpLineTop;
    lpBelowLine.lpPageYTwips += lineStride;
					/********************************/
					/*  But add spacing to find	*/
					/*  position for next line	*/
					/********************************/

    lineBottom= lpBelowLine.lpPageYTwips+ spaceBelowLineTwips;
    lineHeight= lineBottom- lpLineTop->lpPageYTwips;

    footnoteHeight= nrLine->nrFtnsepHeight+ nrLine->nrFootnoteHeight;
    flowY1WithNotes= bf->bfFlowRect.drY1- footnoteHeight;

    if  ( lineBottom > flowY1WithNotes					&&
	  lineHeight < bf->bfContentRect.drY1- bf->bfContentRect.drY0	)
	{ *pStopCode= FORMATstopBLOCK_FULL; return 1;	}

    if  ( lineBottom > pf->pfParaContentRect.drY1			&&
	  lineHeight < pf->pfParaContentRect.drY1- pf->pfParaContentRect.drY0 )
	{ *pStopCode= FORMATstopFRAME_FULL; return 1;	}

    *lp= lpBelowLine;
    lp->lpAtTopOfColumn= 0;

    if  ( partUpto == paraNode->biParaParticuleCount )
	{
	spaceBelowLineTwips= paraNode->biParaBottomInset;

	lp->lpPageYTwips += spaceBelowLineTwips;
	lp->lpAtTopOfColumn= 0;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Calculations on line inserts.					*/
/*									*/
/************************************************************************/

static int docPlaceShape(	const BufferItem *	paraNode,
				const LayoutPosition *	lpLineTop,
				InsertedObject *	io,
				const ParagraphFrame *	pf,
				const BlockFrame *	bf,
				int			xChar )
    {
    DrawingShape *		ds= io->ioDrawingShape;
    int				x1;
    LayoutPosition		lpBelowShape;

    if  ( ! ds )
	{ XDEB(ds); return -1;	}

    docShapePageRectangle( &(io->ioY0Position), &lpBelowShape,
				&(io->ioX0Twips), &x1,
				ds, paraNode, lpLineTop, pf, bf, xChar );

    docDrawingShapeInvalidateTextLayout( ds );

    return 0;
    }

static int docPlaceShapeY(	const BufferItem *	paraNode,
				const LayoutPosition *	lpLineTop,
				InsertedObject *	io,
				const BlockFrame *	bf )
    {
    DrawingShape *		ds= io->ioDrawingShape;
    LayoutPosition		lpBelowShape;

    BlockFrame			bfShape;

    docLayoutInitBlockFrame( &bfShape );

    docShapePageY( &(io->ioY0Position), &lpBelowShape, &bfShape,
						ds, paraNode, lpLineTop, bf );

    docDrawingShapeInvalidateTextLayout( ds );

    docLayoutCleanBlockFrame( &bfShape );

    return 0;
    }

static int docPlaceLineInserts(	BufferDocument *	bd,
				const BufferItem *	paraNode,
				const TextLine *	tl,
				const ParticuleData *	pd,
				const ParagraphFrame *	pf,
				const BlockFrame *	bf,
				const LayoutPosition *	lp )
    {
    int				i;
    const TextParticule *	tp;

    tp= paraNode->biParaParticules+ tl->tlFirstParticule;
    for ( i= 0; i < tl->tlParticuleCount; tp++, i++ )
	{
	if  ( tp->tpKind == DOCkindFIELDHEAD )
	    {
	    if  ( docSetPageOfField( &(bd->bdFieldList),
					tp->tpObjectNumber,  lp->lpPage ) )
		{ LDEB(lp->lpPage); docListNode(0,paraNode,0);	}
	    }

	if  ( tp->tpKind == DOCkindOBJECT )
	    {
	    InsertedObject *	io;

	    io= docGetObject( bd, tp->tpObjectNumber );
	    if  ( ! io )
		{ LPDEB(tp->tpObjectNumber,io);	}

	    if  ( io && io->ioKind == DOCokDRAWING_SHAPE )
		{
		if  ( docPlaceShape( paraNode, &(tl->tlTopPosition),
						    io, pf, bf, pd->pdX0 ) )
		    { LDEB(1);	}
		}
	    }
	}

    return 0;
    }

static int docPlaceLineInsertsY(BufferDocument *	bd,
				const BufferItem *	paraNode,
				const TextLine *	tl,
				const BlockFrame *	bf,
				const LayoutPosition *	lp )
    {
    int				i;
    const TextParticule *	tp;

    tp= paraNode->biParaParticules+ tl->tlFirstParticule;
    for ( i= 0; i < tl->tlParticuleCount; tp++, i++ )
	{
	if  ( tp->tpKind == DOCkindFIELDHEAD )
	    {
	    docSetPageOfField( &(bd->bdFieldList),
					tp->tpObjectNumber,  lp->lpPage );
	    }

	if  ( tp->tpKind == DOCkindOBJECT )
	    {
	    InsertedObject *	io;

	    io= docGetObject( bd, tp->tpObjectNumber );
	    if  ( ! io )
		{ LPDEB(tp->tpObjectNumber,io);	}

	    if  ( io && io->ioKind == DOCokDRAWING_SHAPE )
		{
		if  ( docPlaceShapeY( paraNode, &(tl->tlTopPosition), io, bf ) )
		    { LDEB(1);	}
		}
	    }
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Layout successive lines of a paragraph.				*/
/*									*/
/*  1)  Cope with the output of sgmls: Ignore enormous space before's	*/
/*	in footers.							*/
/*									*/
/************************************************************************/

static int docLayout_Line(	int *				pStopCode,
				TextLine *			resTl,
				NotesReservation *		pNrLine,
				const BlockFrame *		bf,
				BufferItem *			paraNode,
				int				redoLineLayout,
				int				part,
				ParticuleData *			pd,
				const LayoutJob *		lj,
				const ParagraphFrame *		pf,
				const LayoutPosition *		lpTop,
				LayoutPosition *		lpBottom )
    {
    const LayoutContext *	lc= &(lj->ljContext);
    BufferDocument *		bd= lc->lcDocument;

    int				accepted;
    int				res;

    TextParticule *		tp= paraNode->biParaParticules+ part;

    TextLine			workTl;
    TextLine *			tl= &(workTl);

    LayoutPosition		lpHere;

    NotesReservation		nrLine;

    lpHere= *lpTop;

    if  ( redoLineLayout )
	{
	docInitTextLine( tl );
	tl->tlStroff= tp->tpStroff;
	}
    else{
	workTl= *resTl;
	}
    tl->tlFirstParticule= part;

    docInitNotesReservation( &nrLine );

    /*  1  */
    res= docAboveLine( pStopCode, &lpHere, paraNode, part, bf, pf );
    if  ( res < 0 )
	{ LDEB(res); return -1;	}
    if  ( res > 0 )
	{ return 0; }

    tl->tlTopPosition= lpHere;
    if  ( redoLineLayout )
	{ accepted= docLayoutLineBox( tl, paraNode, part, lc, pd, pf );	}
    else{ accepted= tl->tlParticuleCount;				}

    if  ( accepted < 1 )
	{ LDEB(accepted); return -1;	}

    if  ( docLayoutCollectParaFootnoteHeight( &nrLine,
		    tl->tlTopPosition.lpPage, tl->tlTopPosition.lpColumn,
		    bd, lj->ljBodySectNode, paraNode,
		    tl->tlFirstParticule, tl->tlFirstParticule+ accepted ) )
	{ LDEB(1); return -1;	}

    res= docBelowLine( pStopCode, &lpHere,
				    &(tl->tlTopPosition), tl->tlLineStride,
				    paraNode, part, part+ accepted,
				    bf, pf, &nrLine );
    if  ( res < 0 )
	{ LDEB(res); return -1;	}
    if  ( res > 0 )
	{ return 0; }

    if  ( redoLineLayout )
	{
	tl->tlStrlen=
	    tp[accepted-1].tpStroff+ tp[accepted-1].tpStrlen- tp->tpStroff;
	tl->tlParticuleCount= accepted;
	tl->tlFlowWidthTwips=
		    pf->pfParaContentRect.drX1- pf->pfParaContentRect.drX0;

	if  ( lj->ljLayoutScreenLine					&&
	      (*lj->ljLayoutScreenLine)( tl, paraNode, lc, pf, pd ) < 0	)
	    { LDEB(accepted); return -1;	}

	if  ( docPlaceLineInserts( bd, paraNode, tl, pd, pf, bf, &lpHere ) )
	    { LDEB(accepted); }
	}
    else{
	if  ( docPlaceLineInsertsY( bd, paraNode, tl, bf, &lpHere ) )
	    { LDEB(accepted); }
	}

    *lpBottom= lpHere;
    *pNrLine= nrLine;
    *resTl= *tl;

    return accepted;
    }

/************************************************************************/
/*									*/
/*  Add the rectangle of the old and the new locations of a line to the	*/
/*  rectangle that must be redrawn.					*/
/*									*/
/************************************************************************/

static void docLayoutAddLineToExpose(
				DocumentRectangle *		drChanged,
				const LayoutJob *		lj,
				const ParagraphFrame *		pf,
				const TextLine *		boxLine,
				const TextLine *		tlLine )
    {
    const LayoutContext *	lc= &(lj->ljContext);
    DocumentRectangle		drBox;

    LayoutPosition		lpTop;
    LayoutPosition		lpBottom;

    lpTop= boxLine->tlTopPosition;
    lpBottom= lpTop;
    lpBottom.lpPageYTwips += boxLine->tlLineStride;

    docGetPixelRectForPos( &drBox, lc,
			    pf->pfRedrawX0Twips, pf->pfRedrawX1Twips,
			    &lpTop, &lpBottom );

    geoUnionRectangle( drChanged, drChanged, &drBox );

    if  ( tlLine )
	{
	DocumentRectangle	drTl;

	lpTop= tlLine->tlTopPosition;
	lpBottom= lpTop;
	lpBottom.lpPageYTwips += tlLine->tlLineStride;

	docGetPixelRectForPos( &drTl, lc,
			    pf->pfRedrawX0Twips, pf->pfRedrawX1Twips,
			    &lpTop, &lpBottom );

	if  ( drTl.drX0 != drBox.drX0	||
	      drTl.drX1 != drBox.drX1	||
	      drTl.drY0 != drBox.drY0	||
	      drTl.drY1 != drBox.drY1	)
	    {
	    geoUnionRectangle( drChanged, drChanged, &drTl );
	    geoUnionRectangle( drChanged, drChanged, &drBox );

#	    if SHOW_LINE_CHANGES
	    appDebug( "EXPOSE [%4d..%4d x %4d..%4d]\n",
				    drChanged->drX0, drChanged->drX1,
				    drChanged->drY0, drChanged->drY1 );
#	    endif
	    }
	}
    else{
	geoUnionRectangle( drChanged, drChanged, &drBox );

#	if SHOW_LINE_CHANGES
	appDebug( "EXPOSE [%4d..%4d x %4d..%4d]\n",
				    drChanged->drX0, drChanged->drX1,
				    drChanged->drY0, drChanged->drY1 );
#	endif
	}
    }

/************************************************************************/
/*									*/
/*  Layout a series of lines in a paragraph.				*/
/*									*/
/*  1)  Make sure that we have scratch space for the layout routines.	*/
/*  2)  As long as there are any particules to be placed/formatted	*/
/*  3)  Can the current line be reused?					*/
/*  4)  If so just place it at a new position.				*/
/*  5)  Otherwise recalculate layout.					*/
/*  6)  If the line does not fit on this page (column) stop.		*/
/*  7)  Cause the line to be redrawn if either it is reformatted, or it	*/
/*	is moved.							*/
/*  8)  Subtract the space needed for the footnotes in this line from	*/
/*	the space left on this page. (column)				*/
/*  9)  Insert into administration.					*/
/*  10) If the line ends in a page break, make sure nothing will fit on	*/
/*	this page (in this column) anymore.				*/
/*  11) Truncate the number of lines when the paragraph is completely	*/
/*	formatted.							*/
/*									*/
/************************************************************************/

int docLayoutParaLines(		int *				pStopCode,
				int				isRedo,
				const ParagraphFrame *		pf,
				LayoutPosition *		lpHere,
				int *				pLine,
				BlockFrame *			bf,
				const LayoutJob *		lj,
				BufferItem *			paraNode,
				int				part )
    {
    int				stopCode= FORMATstopREADY;

    int				line= (*pLine);
    int				done= 0;

    ParticuleData *		pd;

    LayoutPosition		lp= (*lpHere);

    /*  1  */
    if  ( docPsClaimParticuleData( paraNode->biParaParticuleCount, &pd ) )
	{ LDEB(paraNode->biParaParticuleCount); return -1;	}

    /*  2  */
    while( part < paraNode->biParaParticuleCount )
	{
	int			accepted;
	TextLine		boxLine;

	int			placeOldLine= 0;
	TextLine *		tlLine= (TextLine *)0;

	NotesReservation	nrLine;

	DocumentRectangle *	drChanged= lj->ljChangedRectanglePixels;

	int			flowWidth;

	docInitNotesReservation( &nrLine );

	flowWidth= pf->pfParaContentRect.drX1- pf->pfParaContentRect.drX0;

	docInitTextLine( &boxLine );

	/*  3  */
	if  ( line < paraNode->biParaLineCount )
	    {
	    const TextParticule *	tp= paraNode->biParaParticules+ part;

	    tlLine= paraNode->biParaLines+ line;
	    placeOldLine= 1;
	    boxLine= *tlLine;

	    if  ( tlLine->tlFirstParticule+ tlLine->tlParticuleCount >
					paraNode->biParaParticuleCount	||
		  tlLine->tlStroff != tp->tpStroff			||
		  tlLine->tlFlowWidthTwips != flowWidth			)
		{ placeOldLine= 0;	}
	    }

	/*  4, 5  */
	accepted= docLayout_Line( &stopCode, &boxLine, &nrLine, bf,
			paraNode, ! placeOldLine, part, pd, lj, pf, &lp, &lp );

	if  ( accepted < 0 )
	    { LDEB(accepted); return -1;	}

	/*  6  */
	if  ( accepted == 0 )
	    { break;	}

	/*  7  */
	if  ( drChanged	)
	    {
	    docLayoutAddLineToExpose( drChanged, lj, pf, &boxLine, tlLine );
	    }

	/*  8  */
	if  ( ! isRedo )
	    { docLayoutReserveNoteHeight( bf, &nrLine );	}

	/*  9  */
	if  ( line >= paraNode->biParaLineCount )
	    {
	    tlLine= docInsertTextLine( paraNode, -1 );
	    if  ( ! tlLine )
		{ XDEB(tlLine); return -1;		}
	    }
	else{
	    tlLine= paraNode->biParaLines+ line;
	    }

	*tlLine= boxLine;

	part += accepted; done += accepted; line++;

	/*  10  */
	if  ( boxLine.tlFlags & TLflagBLOCKBREAK )
	    {
	    int		part;

	    part= boxLine.tlFirstParticule+ boxLine.tlParticuleCount- 1;

	    switch( paraNode->biParaParticules[part].tpKind )
		{
		case DOCkindPAGEBREAK:
		    stopCode= FORMATstopPAGE_BREAK;
		    break;
		case DOCkindCOLUMNBREAK:
		    stopCode= FORMATstopCOLUMN_BREAK;
		    break;
		default:
		    LDEB(paraNode->biParaParticules[part].tpKind);
		    stopCode= FORMATstopPAGE_BREAK;
		    break;
		}
	    break;
	    }
	}

    *pLine= line;
    *lpHere= lp;
    *pStopCode= stopCode;

    /*  11  */
    if  ( part >= paraNode->biParaParticuleCount	&&
	  paraNode->biParaLineCount > line	)
	{ paraNode->biParaLineCount=  line; }

    return done;
    }

