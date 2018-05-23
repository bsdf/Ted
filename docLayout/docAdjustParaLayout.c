/************************************************************************/
/*									*/
/*  Layout of a document.						*/
/*									*/
/************************************************************************/

#   include	"docLayoutConfig.h"

#   include	<stddef.h>

#   include	"docLayout.h"
#   include	<docPageGrid.h>
#   include	<docTreeType.h>
#   include	<docTreeNode.h>
#   include	<docTextLine.h>
#   include	<docDebug.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Redo the layout of a paragraph after an editing action.		*/
/*									*/
/*  To avoid screen flashing, a minimalistic approach is used: As	*/
/*  little of the layout is redone, and an administration is kept of	*/
/*  what has been changed, in order to redraw a minimal screen		*/
/*  rectangle.								*/
/*									*/
/*  a)  Shift the offsets of the lines in the rest of the paragraph.	*/
/*	This means that we optimistically assume that the change in the	*/
/*	text of the paragraph only affected the current line, and that	*/
/*	the lines below are unchanged. This assumption is verified by	*/
/*	docLayoutParaLines() so out naive optimism does no harm.	*/
/*  b)  Invalidate the column with of the lines that are to be		*/
/*	reformatted anyway to force a reformat by docLayoutParaLines().	*/
/*	[stroffUpto is in terms of the new line and particule offsets.]	*/
/*	Below, fromLine can change value. This forced invalidation	*/
/*	refers to the original value however: The beginning of the	*/
/*	edited stretch of text.						*/
/*  c)  Remember the end of the reformatting range.			*/
/*  d)  With widow/orphan control enabled, and after a replacement, it	*/
/*	might be possible that the current paragraph becomes lower and	*/
/*	can be moved to the previous page. So start a little earlier to	*/
/*	make sure that all special cases are covered by the regular	*/
/*	formatter algorithm.						*/
/*  2)  Calculate the frame in which the text is to be laid out.	*/
/*  3)  Reserve space for the footnotes upto the beginning of this line	*/
/*	and subtract the height from the buttom of the frame.		*/
/*  5)  Pagination of footnotes, headers and footers and separators is	*/
/*	handled after we find that the height of the external item has	*/
/*	changed.							*/
/*  6)  Set the start position for the layout operation to the current	*/
/*	line. Try only to redo the layout of the current paragraph.	*/
/*									*/
/************************************************************************/

int docAdjustParaLayout(	BufferItem *		paraBi,
				int			afterReplace,
				int			stroffFrom,
				int			stroffShift,
				int			stroffUpto,
				LayoutJob *		lj )
    {
    const LayoutContext *	lc= &(lj->ljContext);
    int				rval= 0;
    BufferDocument *		bd= lc->lcDocument;
    int				fontSize;

    BlockFrame			bf;
    int				fromPart;
    int				line;

    ParagraphLayoutJob		plj;

    LayoutPosition		oldLpBelow;
    int				paraUpto;
    const BufferItem *		prevParaBi= (const BufferItem *)0;
    BufferItem *		cellBi= paraBi->biParent;

    int				fromPara;
    int				fromLine= 0;
    BufferItem *		biParaFrom;

    LayoutPosition		lpHere;

    docInitParagraphLayoutJob( &plj );
    docLayoutInitBlockFrame( &bf );

    if  ( paraBi->biLevel != DOClevPARA )
	{ LLDEB(paraBi->biLevel,DOClevPARA); rval= -1; goto ready; }
    /*
    if  ( fromLine < 0 || fromLine >= paraBi->biParaLineCount )
	{ LLDEB(fromLine, paraBi->biParaLineCount); rval= -1; goto ready; }
    */

    /*  a,b  */
    {
    TextLine *	tlShift;

    tlShift= paraBi->biParaLines;
    for ( fromLine= 0; fromLine < paraBi->biParaLineCount;
						    tlShift++, fromLine++ )
	{
	if  ( tlShift->tlStroff+ tlShift->tlStrlen >= stroffFrom )
	    {
	    docInvalidateTextLine( tlShift );
	    break;
	    }
	}

    tlShift= paraBi->biParaLines+ fromLine;
    docInvalidateTextLine( tlShift );
    tlShift++;
    for ( line= fromLine+ 1; line < paraBi->biParaLineCount; tlShift++, line++ )
	{
	tlShift->tlStroff += stroffShift;

	if  ( tlShift->tlStroff < stroffUpto )
	    { docInvalidateTextLine( tlShift );	}
	}
    }

    paraBi->biParaMajorityFontAscY0= 0;
    paraBi->biParaMajorityFontDescY1= 0;
    paraBi->biParaMajorityFontSize= 0;

    /*  c  */
    oldLpBelow= paraBi->biBelowPosition;
    paraUpto= paraBi->biNumberInParent+ 1;

    if  ( paraBi->biNumberInParent > 0 )
	{ prevParaBi= cellBi->biChildren[paraBi->biNumberInParent- 1]; }

    /*  d  */
    biParaFrom= paraBi;
    fromPara= biParaFrom->biNumberInParent;
    if  ( afterReplace			&&
	  paraBi->biParaWidowControl	&&
	  fromLine < 3			&&
	  prevParaBi			&&
	  paraBi->biNumberInParent > 0	)
	{
	fromLine= 0;
	fromPart= 0;

	while( prevParaBi->biNumberInParent > 0 )
	    {
	    prevParaBi= cellBi->biChildren[prevParaBi->biNumberInParent- 1];

	    fromPara--;
	    biParaFrom= cellBi->biChildren[fromPara];

	    if  ( ! prevParaBi->biParaKeepWithNext )
		{ break;	}
	    }

	lpHere= prevParaBi->biBelowPosition;
	}
    else{
	const TextLine *	tl;

	tl= paraBi->biParaLines+ fromLine;
	fromPart= tl->tlFirstParticule;
	if  ( fromPart == 0 )
	    {
	    if  ( paraBi->biNumberInParent == 0 || ! prevParaBi )
		{ lpHere= paraBi->biTopPosition;	}
	    else{ lpHere= prevParaBi->biBelowPosition;	}
	    }
	else{ lpHere= tl->tlTopPosition;		}
	}

    /*  2  */
    docLayoutBlockFrame( &bf, biParaFrom, lj, lpHere.lpPage, lpHere.lpColumn );

    /*  3  */
    if  ( biParaFrom->biTreeType == DOCinBODY )
	{
	TextLine *			tlHere;
	DocumentPosition		dpHere;

	tlHere= biParaFrom->biParaLines+ fromLine;
	dpHere.dpNode= biParaFrom;
	dpHere.dpStroff= tlHere->tlStroff;

	if  ( docCollectFootnotesFromColumn( &bf, &dpHere, fromPart, bd,
					    lpHere.lpPage, lpHere.lpColumn ) )
	    { LDEB(lpHere.lpPage); rval= -1; goto ready;	}
	}

    /*  4  */
    docLayoutAdjustFrame( &bf, biParaFrom );

    docParagraphFrameTwips( &(plj.pljPos.plpParagraphFrame), &bf, biParaFrom );

    if  ( docLayoutParagraphLineExtents( &fontSize, lc, biParaFrom ) )
	{ LDEB(1); rval= -1; goto ready;	}

    docLayoutCalculateParaTopInset( bd, paraBi );
    docLayoutCalculateParaBottomInset( bd, paraBi );

    if  ( lj->ljStartScreenParagraph					&&
	  (*lj->ljStartScreenParagraph)( biParaFrom,
			&(plj.pljPos.plpParagraphFrame), lc )	)
	{ LDEB(1); rval= -1; goto ready;	}

    /*  6  */
    docBeginParagraphLayoutProgress( &plj,
			biParaFrom->biNumberInParent, fromLine, fromPart,
			paraUpto, &lpHere );

    docFindStripLayoutOrigin( &plj, lpHere.lpPage, lpHere.lpColumn, cellBi );

    if  ( docLayoutStackedStrip( cellBi, &bf, lj, &plj ) )
	{ LDEB(1); rval= -1; goto ready;	}

    lpHere= plj.pljPos.plpPos;

    if  ( paraBi->biParaLineCount < 1 )
	{
	LDEB(paraBi->biParaLineCount); docListNode(0,paraBi,0);
	rval= -1; goto ready;
	}

    if  ( ! DOC_SAME_POSITION( &(paraBi->biBelowPosition), &lpHere ) )
	{
	LLDEB(paraBi->biBelowPosition.lpPage,lpHere.lpPage);
	LDEB(paraBi->biBelowPosition.lpPageYTwips);
	LDEB(lpHere.lpPageYTwips);

	paraBi->biBelowPosition= lpHere;
	}

    if  ( ! DOC_SAME_POSITION( &(paraBi->biBelowPosition), &oldLpBelow ) )
	{
	/*  5  */
	if  ( docAdjustParentLayout( &lpHere, paraBi, &bf, lj ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}

  ready:

    docLayoutCleanBlockFrame( &bf );
    docCleanParagraphLayoutJob( &plj );

    return rval;
    }
