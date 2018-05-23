/************************************************************************/
/*									*/
/*  Layout of a document. Do the layout inside lies of text.		*/
/*									*/
/************************************************************************/

#   include	"docLayoutConfig.h"

#   include	<stddef.h>
#   include	<math.h>

#   include	"docLayout.h"
#   include	<docTabStop.h>
#   include	<docParaString.h>
#   include	<docParaParticules.h>
#   include	<docParaRulerAdmin.h>
#   include	<docTreeType.h>
#   include	<docDebug.h>
#   include	<docBorderPropertyAdmin.h>
#   include	<docListLevel.h>

#   include	<appDebugon.h>

typedef struct LineLayoutJob
    {
    BufferItem *		lljParaNode;
    TabStopList			lljTabStopList;
    const ParagraphFrame *	lljParagraphFrame;
    int				lljLineHeadStroff;
    const LayoutContext *	lljLayoutContext;
    } LineLayoutJob;

/************************************************************************/

static void docLayoutAddExtraToPrevious(	ParticuleData *	pd,
						int		extra )
    {
    if  ( pd[-1].pdDecWidth == pd[-1].pdTwipsWide )
	{ pd[-1].pdDecWidth += extra;	}

    pd[-1].pdTwipsWide += extra;
    pd[-1].pdVisibleBBox.drX1 += extra;

    return;
    }

static void docLayoutAddRightBorderToPrevious(
					ParticuleData *	pd,
					int		rightBorderThick )
    {
    pd[-1].pdFlags |= TPflagRIGHT_BORDER;
    pd[-1].pdRightBorderWidth= rightBorderThick;

    docLayoutAddExtraToPrevious( pd, rightBorderThick );
    }

static void docLayoutAddSlantToPrevious(	int *			pX,
						ParticuleData *		pd,
						const AfmFontInfo *	afi,
						const TextAttribute *	ta,
						const TextAttribute *	taPr )
    {
    int		extra;

    if  ( taPr->taFontSizeHalfPoints < ta->taFontSizeHalfPoints )
	{ extra= taPr->taFontSizeHalfPoints;		}
    else{ extra= ta->taFontSizeHalfPoints;		}

    extra = (int)( -afi->afiTanItalicAngle* 10/ 2* extra );

    *pX += extra;
    docLayoutAddExtraToPrevious( pd, extra );

    return;
    }

/************************************************************************/
/*									*/
/*  Place a single 'word'.						*/
/*									*/
/*  A 'Word' is a series of TEXT particules that does not contain any	*/
/*  white space.							*/
/*									*/
/*  1)  Check that the assumption below that the loop is entered at	*/
/*	least once is true.						*/
/*  2)  For groups with the same text attributes...			*/
/*  3)  Retrieve font information.					*/
/*  4)  Reserve space for the text border. If necessary also left of	*/
/*	the text.							*/
/*  5)  Calculate text extents.						*/
/*  6)  White space: End of word.					*/
/*  7)  Claim a little more space if the font switches from italic to	*/
/*	regular. ( ////|||| )						*/
/*  8)  Claim space for the right text border if any. As this is inside	*/
/*	the word there is no relation with line folding.		*/
/*  9)  Add the right border to the visible x1. The caller claims space	*/
/*	if the line is not folded.					*/
/*									*/
/************************************************************************/

static int docLayoutWord(	LineLayoutJob *			llj,
				int				part,
				DocumentRectangle *		drWord,
				int *				pAddRBorder,
				int *				pRightBThick,
				int *				pFattestWordB,
				int *				pVisibleX1,
				unsigned char *			pLineFlags,
				ParticuleData *			pd,
				int				particuleUpto,
				int				x0Word )
    {
    TextAttribute		taPr;
    const AfmFontInfo *		afi= (const AfmFontInfo *)0;
    int				thick= 0;
    int				space= 0;
    int				haveBorder= 0;

    int				accepted;
    int				width;
    int				decWidth;

    int				len= 0;
    int				x= x0Word;
    int				visibleX1= x0Word;

    BufferItem *		paraNode= llj->lljParaNode;
    BufferDocument *		bd= llj->lljLayoutContext->lcDocument;

    const TextParticule *	tp= paraNode->biParaParticules+ part;
    const char *		from= (char *)docParaString( paraNode, tp->tpStroff );
    unsigned char		lineFlags= 0;

    drWord->drX0= x0Word;
    drWord->drY0= 0;
    drWord->drY1= 0;
    *pFattestWordB= 0;

    utilInitTextAttribute( &taPr );

    if  ( part > 0 )
	{ docGetEffectiveTextAttributes( &taPr, bd, paraNode, part- 1 ); }

    /*  1  */
    if  ( part >= particuleUpto || tp->tpKind != DOCkindSPAN )
	{ LLLDEB(part,particuleUpto,tp->tpKind); return -1; }

    /*  2  */
    accepted= 0;
    while( part < particuleUpto && tp->tpKind == DOCkindSPAN )
	{
	int			x0Span= x;
	int			textAttrNr0;
	int			textAttrNr;
	TextAttribute		ta;

	int			firstInSpan= 1;
	int			spanHasLeftBorder= 0;
	unsigned char		spanFlags= 0;

	thick= 0;
	space= 0;
	haveBorder= 0;

	/*  3  */
	afi= docLayoutGetAfi( &textAttrNr0, &ta, &spanFlags,
				    llj->lljLayoutContext, paraNode, part );
	if  ( ! afi )
	    {
	    LLXDEB(docNumberOfParagraph(paraNode),tp->tpStroff,afi);
	    docListNode(0,paraNode,0);
	    return -1;
	    }

	textAttrNr= textAttrNr0;
	lineFlags |= spanFlags;

	/*  4  */
	if  ( spanFlags & TLflagBORDER )
	    {
	    BorderProperties	bp;

	    haveBorder= 1;

	    docGetBorderPropertiesByNumber( &bp, bd, ta.taBorderNumber );

	    thick= docBorderThick( &space, &bp );

	    if  ( *pFattestWordB < thick+ space )
		{ *pFattestWordB=  thick+ space;	}

	    if  ( tp->tpStroff == llj->lljLineHeadStroff )
		{ spanHasLeftBorder= 1;		}
	    else{
		if  ( taPr.taBorderNumber != ta.taBorderNumber )
		    { spanHasLeftBorder= 1;	}
		}
	    }

	/*  5  */
	while( part < particuleUpto		&&
	       textAttrNr == textAttrNr0	&&
	       tp->tpKind == DOCkindSPAN	)
	    {
	    DocumentRectangle	drPart;
	    int			sizeTwips;
	    TextAttribute	taTmp;

	    sizeTwips= 10* ta.taFontSizeHalfPoints;

	    if  ( ta.taSuperSub == TEXTvaSUPERSCRIPT ||
		  ta.taSuperSub == TEXTvaSUBSCRIPT   )
		{ sizeTwips= SUPERSUB_SIZE( sizeTwips ); }

	    len += tp->tpStrlen;

	    if  ( docLayoutStringExtents( &width, &decWidth, &drPart,
					&ta, sizeTwips, bd, afi, from, len ) )
		{ LDEB(1); return -1;	}

	    if  ( ta.taBaselineShiftHalfPoints > 0 )
		{ drPart.drY0 -= 10* ta.taBaselineShiftHalfPoints;	}
	    if  ( ta.taBaselineShiftHalfPoints < 0 )
		{ drPart.drY1 -= 10* ta.taBaselineShiftHalfPoints;	}

	    if  ( spanHasLeftBorder )
		{
		width += thick;
		decWidth += thick;
		drPart.drX1 += thick;
		}

	    visibleX1= x0Span+ drPart.drX1;

	    docInitParticuleData( pd );
	    pd->pdAfi= afi;
	    pd->pdX0= x;
	    pd->pdTwipsWide= x0Span+ width- x;
	    pd->pdDecWidth= x0Span+ decWidth- x;
	    pd->pdVisibleBBox= drPart;
	    if  ( ! firstInSpan )
		{
		/*  No need to fix Y's: Are always used for whole word.	*/
		pd->pdVisibleBBox.drX0= 0;
		pd->pdVisibleBBox.drX1= x0Span+ pd->pdVisibleBBox.drX1- x;
		}

	    if  ( firstInSpan && spanHasLeftBorder )
		{
		pd->pdLeftBorderWidth= thick;
		pd->pdFlags |= (TPflagLEFT_BORDER|TPflagXMATCH_LEFT);
		}

	    x= x0Span+ width;

	    includeRectangleY( drWord, &drPart );

	    accepted++; part++; tp++; pd++;

	    /*  6  */
	    if  ( ( len > 0 && from[len- 1] == ' ' )	||
		  part >= particuleUpto			)
		{ break;	}

	    firstInSpan= 0;
	    textAttrNr= docGetEffectiveTextAttributes( &taTmp,
							bd, paraNode, part );
	    }

	taPr= ta;

	/*  6  */
	if  ( ( len > 0 && from[len- 1] == ' ' )	||
	      part >= particuleUpto			)
	    { break;	}

	textAttrNr= docGetEffectiveTextAttributes( &ta, bd, paraNode, part );

	/*  7  */
	if  ( ! ta.taFontIsSlanted && taPr.taFontIsSlanted )
	    {
	    docLayoutAddSlantToPrevious( &x, pd, afi, &ta, &taPr );
	    }

	/*  8  */
	if  ( haveBorder && ta.taBorderNumber != taPr.taBorderNumber )
	    {
	    x += thick;
	    docLayoutAddRightBorderToPrevious( pd, thick );
	    }

	from= (char *)docParaString( paraNode, tp->tpStroff );
	x0Span= x;
	len= 0;
	}

    if  ( part < paraNode->biParaParticuleCount )
	{
	TextAttribute		ta;

	docGetEffectiveTextAttributes( &ta, bd, paraNode, part );

	/*  7  */
	if  ( part < particuleUpto	&&
	      ! ta.taFontIsSlanted	&&
	      taPr.taFontIsSlanted	)
	    { docLayoutAddSlantToPrevious( &x, pd, afi, &ta, &taPr ); }

	/*  9  */
	*pAddRBorder= 0;
	if  ( haveBorder )
	    {
	    visibleX1 += thick;

	    if  ( ta.taBorderNumber != taPr.taBorderNumber )
		{ *pAddRBorder= 1;	}
	    else{ *pAddRBorder= 0;	}
	    }
	}
    else{
	/*  9  */
	*pAddRBorder= 0;
	if  ( haveBorder )
	    { *pAddRBorder= 1;	}
	}

    drWord->drX1= x;
    *pVisibleX1= visibleX1;
    *pRightBThick= thick;
    *pLineFlags |= lineFlags;

    return accepted;
    }

/************************************************************************/
/*									*/
/*  Add an optional hyphen to a line that is full.			*/
/*									*/
/*  It is guaranteed to fit, because otherwise the line would have been	*/
/*  folded to make it fit.						*/
/*									*/
/************************************************************************/

static int docLayoutShowOptionalHyphen(
				LineLayoutJob *			llj,
				int				part,
				unsigned char *			pLineFlags,
				ParticuleData *			pd,
				int				x0 )
    {
    BufferDocument *		bd= llj->lljLayoutContext->lcDocument;

    BufferItem *		paraNode= llj->lljParaNode;
    const TextParticule *	tp= paraNode->biParaParticules+ part;

    int				textAttrNr;
    TextAttribute		ta;
    const AfmFontInfo *		afi;
    int				sizeTwips;

    int				width;
    int				decWidth;

    DocumentRectangle		drPart;

    const char *		from;
    int				len;

    unsigned char		lineFlags= 0;

    from= (char *)docParaString( paraNode, tp->tpStroff );
    len= tp->tpStrlen;

    afi= docLayoutGetAfi( &textAttrNr, &ta, &lineFlags,
				    llj->lljLayoutContext, paraNode, part );
    if  ( ! afi )
	{ XDEB(afi); return -1;	}

    sizeTwips= 10* ta.taFontSizeHalfPoints;

    if  ( docLayoutStringExtents( &width, &decWidth, &drPart,
					&ta, sizeTwips, bd, afi, from, len ) )
	{ LDEB(len); return -1;	}

    docInitParticuleData( pd );
    pd->pdAfi= afi;
    pd->pdX0= x0;
    pd->pdTwipsWide= width;
    pd->pdDecWidth= pd->pdTwipsWide;
    pd->pdVisibleBBox= drPart;

    *pLineFlags |= lineFlags;
    return 0;
    }

/************************************************************************/

static int docLayoutInsertOptionalHyphen(	LineLayoutJob *		llj,
						int			part,
						int			done,
						int			x0,
						ParticuleData *		pd )
    {
    BufferDocument *		bd= llj->lljLayoutContext->lcDocument;

    int				inserted= 0;
    const ParagraphFrame *	pf= llj->lljParagraphFrame;
    const DocumentRectangle *	drParaContent= &(pf->pfParaContentRect);
    BufferItem *		paraNode= llj->lljParaNode;

    int				i;
    int				x1;
    TextParticule *		tpHyphen;

    x1= x0;
    for ( i= 0; i < done; i++ )
	{
	if  ( x1+ pd[i].pdTwipsWide >= drParaContent->drX1 )
	    { break;	}

	x1 += pd[i].pdTwipsWide;
	}

    if  ( i == 0 )
	{
	int			l;
	unsigned char		lineFlags;
	const AfmFontInfo *	afi;
	int			textAttrNr;
	TextAttribute		ta;
	int			from;
	int			sizeTwips;

	from= paraNode->biParaParticules[part].tpStroff;
	afi= docLayoutGetAfi( &textAttrNr, &ta, &lineFlags,
				    llj->lljLayoutContext, paraNode, part );
	if  ( ! afi )
	    { XDEB(afi); return -1;	}

	sizeTwips= 10* ta.taFontSizeHalfPoints;

	for ( l= 0; l < paraNode->biParaParticules[part].tpStrlen- 1; l++ )
	    {
	    int			width;
	    int			decWidth;
	    DocumentRectangle	drPart;

	    if  ( docLayoutStringExtents(
			&width, &decWidth, &drPart,
			&ta, sizeTwips, bd,
			afi, (char *)docParaString( paraNode, from ), l+ 1 ) )
		{ LDEB(l+1); return -1;	}

	    if  ( x0+ width+ sizeTwips/ 3 >= drParaContent->drX1 )
		{ break;	}
	    }

	if  ( l > 0 && l < paraNode->biParaParticules[part].tpStrlen )
	    {
	    if  ( docSplitTextParticule( (TextParticule **)0,
				(TextParticule **)0, paraNode, part,
				paraNode->biParaParticules[part].tpStroff+ l ) )
		{ LDEB(1); return -1;	}

	    i= 1;
	    }
	}

    if  ( i > 0 )
	{
	int	stroffShift;
	int	stroff= paraNode->biParaParticules[part+i].tpStroff;
	int	attrNr= paraNode->biParaParticules[part+i-1].tpTextAttrNr;

	docParaStringReplace( &stroffShift, paraNode,
					stroff, stroff, (const char *)"-", 1 );

	tpHyphen= docInsertTextParticule( paraNode, part+ i,
					stroff, 1, DOCkindOPT_HYPH, attrNr );
	if  ( ! tpHyphen )
	    { XDEB(tpHyphen); return -1;	}

	docShiftParticuleOffsets( bd, paraNode,
				part+ i+ 1, paraNode->biParaParticuleCount, 1 );

	inserted= 1;
	}

    return inserted;
    }

/************************************************************************/
/*									*/
/*  Place as many particules on a line as possible, but stop on a tab.	*/
/*									*/
/************************************************************************/

#   define	PSfoundNOTHING		0
#   define	PSfoundTAB		1
#   define	PSfoundLINEFULL		2
#   define	PSfoundLINEBREAK	3
#   define	PSfoundBLOCKBREAK	4
#   define	PSfoundBULLET_END	5

static int docPsLayoutText(	LineLayoutJob *			llj,
				int				part,
				int *				pFound,
				int *				pWordCount,
				DocumentRectangle *		drText,
				int *				pFattestTextB,
				unsigned char *			pLineFlags,
				ParticuleData *			pd,
				int				particuleUpto,
				int				acceptAtLeast,
				int				x0 )
    {
    BufferDocument *		bd= llj->lljLayoutContext->lcDocument;

    const ParagraphFrame *	pf= llj->lljParagraphFrame;
    const DocumentRectangle *	drParaContent= &(pf->pfParaContentRect);
    BufferItem *		paraNode= llj->lljParaNode;

    int				accepted;
    int				found= PSfoundNOTHING;
    int				wordCount= 0;
    unsigned char		lineFlags= 0;

    drText->drX0= x0;
    drText->drY0= 0;
    drText->drY1= 0;
    *pFattestTextB= 0;

    accepted= 0;
    while( part < particuleUpto && found == PSfoundNOTHING )
	{
	int		done;
	int		fattestWordBorder= 0;
	int		rightBorderThick= 0;
	int		addRightBorder= 0;

	docInitParticuleData( pd );

	switch( paraNode->biParaParticules[part].tpKind )
	    {
	    int			visibleX1;
	    DocumentRectangle	drWord;

	    case DOCkindSPAN:
	    textCase:
		done= docLayoutWord( llj, part, &drWord,
			&addRightBorder, &rightBorderThick, &fattestWordBorder,
			&visibleX1, &lineFlags, pd, particuleUpto, x0 );
		if  ( done < 1 )
		    { LDEB(done); return -1;	}

		/********************************************************/
		/* HACK: Add an optional hyphen if this cannot fit.	*/
		/* Then retry.						*/
		/********************************************************/
		if  ( visibleX1 >= drParaContent->drX1			&&
		      bd->bdAutoHyphenate				&&
		      particuleUpto == paraNode->biParaParticuleCount	&&
		      paraNode->biParaParticules[part].tpStroff ==
					    llj->lljLineHeadStroff	)
		    {
		    int		inserted;

		    inserted= docLayoutInsertOptionalHyphen( llj,
							part, done, x0, pd );
		    if  ( inserted < 0 )
			{ LDEB(inserted); return -1;	}

		    if  ( inserted > 0 )
			{
			done= docLayoutWord( llj, part,
				&drWord, &addRightBorder, &rightBorderThick,
				&fattestWordBorder, &visibleX1, &lineFlags,
				pd, particuleUpto, x0 );
			if  ( done < 1 )
			    { LDEB(done); return -1;	}

			particuleUpto= paraNode->biParaParticuleCount;
			}
		    }

		if  ( visibleX1 >= drParaContent->drX1	&&
		      accepted >= acceptAtLeast		)
		    {
		    if  ( part > 0					&&
			  paraNode->biParaParticules[part- 1].tpKind ==
							DOCkindOPT_HYPH	)
			{
			if  ( docLayoutShowOptionalHyphen( llj, part- 1,
						    &lineFlags, pd- 1, x0 ) )
			    { LDEB(1); return -1;	}
			}

		    if  ( accepted > 0 )
			{
			/* WRONG! fix later */
			pd[-1].pdTwipsWide= pd[-1].pdVisibleBBox.drX1;
			docLayoutAddRightBorderToPrevious( pd,
							rightBorderThick );
			}

		    *pFound= PSfoundLINEFULL;
		    *pWordCount= wordCount;
		    *pLineFlags |= lineFlags;
		    drText->drX1= visibleX1;
		    return accepted;
		    }

		if  ( addRightBorder )
		    {
		    drWord.drX1 += rightBorderThick;
		    docLayoutAddRightBorderToPrevious( pd+ done,
							    rightBorderThick );
		    }

		includeRectangleY( drText, &drWord );
		if  ( *pFattestTextB < fattestWordBorder )
		    { *pFattestTextB=  fattestWordBorder;	}
		x0= drWord.drX1;

		wordCount++;
		lineFlags |= TLflagINLINECONTENT;
		accepted += done; part += done; pd += done;
		break;

	    case DOCkindFIELDHEAD:
		done= 0;
		while( part < particuleUpto				&&
		       paraNode->biParaParticules[part].tpKind ==
						    DOCkindFIELDHEAD	)
		    {
		    docInitParticuleData( pd );
		    pd->pdX0= x0;
		    pd->pdTwipsWide= 0;

		    if  ( accepted < acceptAtLeast )
			{ acceptAtLeast++;	}

		    accepted++; part++; pd++;
		    }

		if  ( part < particuleUpto				   &&
		      paraNode->biParaParticules[part].tpKind == DOCkindSPAN )
		    { goto textCase;	}

		break;

	    case DOCkindTAB:
		lineFlags |= TLflagINLINECONTENT;
		found= PSfoundTAB;
		break;

	    case DOCkindFIELDTAIL:
		pd->pdX0= x0;
		pd->pdTwipsWide= 0;

		accepted++; part++; pd++; break;

	    case DOCkindOBJECT:
		{
		InsertedObject *	io;
		int			width= 0;

		io= docGetObject( bd, 
			    paraNode->biParaParticules[part].tpObjectNumber );
		if  ( ! io )
		    { LPDEB(paraNode->biParaParticules[part].tpObjectNumber,io);}

		if  ( io && io->ioInline )
		    {
		    int			x1;
		    TextAttribute	ta;

		    done= docLayoutInlineObject( &ta, bd,
					paraNode, part, io,
					&drWord, &fattestWordBorder, &x1, x0 );

		    if  ( ta.taBaselineShiftHalfPoints > 0 )
			{ drWord.drY0 -= 10* ta.taBaselineShiftHalfPoints; }
		    if  ( ta.taBaselineShiftHalfPoints < 0 )
			{ drWord.drY1 -= 10* ta.taBaselineShiftHalfPoints; }

		    /* some extra space ? Take leading! */
		    drWord.drY0 -= ( 150* paraNode->biParaMajorityFontSize )/ 100;

		    if  ( done < 1 )
			{ LDEB(done); return -1;	}

		    if  ( x1 >= drParaContent->drX1	&&
			  accepted >= acceptAtLeast	)
			{
			*pFound= PSfoundLINEFULL;
			*pWordCount= wordCount;
			*pLineFlags |= lineFlags;
			drText->drX1= x0;
			return accepted;
			}

		    width= x1- x0;

		    includeRectangleY( drText, &drWord );
		    if  ( *pFattestTextB < fattestWordBorder )
			{ *pFattestTextB=  fattestWordBorder;	}

		    lineFlags |= TLflagINLINECONTENT;
		    }

		pd->pdX0= x0;
		pd->pdTwipsWide= width;
		pd->pdDecWidth= pd->pdTwipsWide;
		pd->pdVisibleBBox= drWord;
		pd->pdVisibleBBox.drX0= 0;
		pd->pdVisibleBBox.drX1= pd->pdTwipsWide;

		x0 += width;

		accepted++; part++; pd++; break;
		}

	    case DOCkindCHFTNSEP:
	    case DOCkindCHFTNSEPC:
		{
		int		frameWide;
		int		width= 2880;
		TextAttribute	ta;

		frameWide= pf->pfParaContentRect.drX1- pf->pfParaContentRect.drX0;
		if  ( width > frameWide )
		    { width= frameWide;	}

		if  ( x0+ width >= drParaContent->drX1	&&
		      accepted >= acceptAtLeast		)
		    {
		    *pFound= PSfoundLINEFULL;
		    *pWordCount= wordCount;
		    *pLineFlags |= lineFlags;
		    drText->drX1= x0;
		    return accepted;
		    }

		if  ( docLayoutFontAscDesc( paraNode,
				&ta, &drWord, &fattestWordBorder,
				llj->lljLayoutContext, part ) < 0 )
		    { LDEB(1); return -1;	}

		if  ( ta.taBaselineShiftHalfPoints > 0 )
		    { drWord.drY0 -= 10* ta.taBaselineShiftHalfPoints; }
		if  ( ta.taBaselineShiftHalfPoints < 0 )
		    { drWord.drY1 -= 10* ta.taBaselineShiftHalfPoints; }

		includeRectangleY( drText, &drWord );
		if  ( *pFattestTextB < fattestWordBorder )
		    { *pFattestTextB=  fattestWordBorder;	}

		pd->pdX0= x0;
		pd->pdTwipsWide= width;
		pd->pdDecWidth= pd->pdTwipsWide;
		pd->pdVisibleBBox= drWord;
		pd->pdVisibleBBox.drX0= 0;
		pd->pdVisibleBBox.drX1= pd->pdTwipsWide;

		x0 += width;

		lineFlags |= TLflagINLINECONTENT;
		accepted++; part++; pd++; break;
		}

	    case DOCkindOPT_HYPH:
		{
		int			textAttrNr;
		TextAttribute		ta;
		const AfmFontInfo *	afi;
		int			sizeTwips;

		int			width;
		int			decWidth;

		DocumentRectangle	drPart;

		int			from;
		int			len;

		unsigned char		ignoredLineFlags= 0;

		from= paraNode->biParaParticules[part].tpStroff;
		len= paraNode->biParaParticules[part].tpStrlen;

		afi= docLayoutGetAfi( &textAttrNr, &ta, &ignoredLineFlags,
				    llj->lljLayoutContext, paraNode, part );
		if  ( ! afi )
		    { XDEB(afi); return -1;	}

		/* Only do border admin when the thing is made visible */

		sizeTwips= 10* ta.taFontSizeHalfPoints;

		if  ( docLayoutStringExtents( &width, &decWidth, &drPart,
			    &ta, sizeTwips, bd, afi,
			    (char *)docParaString( paraNode, from ), len ) )
		    { LDEB(1); return -1;	}

		if  ( x0+ width >= drParaContent->drX1	&&
		      accepted >= acceptAtLeast		)
		    {
		    *pFound= PSfoundLINEFULL;
		    *pWordCount= wordCount;
		    drText->drX1= x0;
		    return accepted;
		    }

		pd->pdAfi= afi;
		pd->pdX0= x0;
		pd->pdTwipsWide= 0;
		pd->pdDecWidth= pd->pdTwipsWide;
		pd->pdVisibleBBox= drPart;
		pd->pdVisibleBBox.drX0= 0;
		pd->pdVisibleBBox.drX1= 0;

		accepted++; part++; pd++; break;
		}

	    case DOCkindLINEBREAK:
	    case DOCkindPAGEBREAK:
	    case DOCkindCOLUMNBREAK:

		pd->pdX0= x0;
		pd->pdTwipsWide= 0;

		if  ( paraNode->biParaParticules[part].tpKind != DOCkindLINEBREAK	&&
		      paraNode->biParaTableNesting == 0		)
		    { found= PSfoundBLOCKBREAK;	}
		else{ found= PSfoundLINEBREAK;	}

		accepted++; part++; pd++; break;

	    default:
		LDEB(paraNode->biParaParticules[part].tpKind); return -1;
	    }
	}

    *pFound= found;
    *pWordCount= wordCount;
    drText->drX1= x0;
    *pLineFlags |= lineFlags;

    return accepted;
    }

/************************************************************************/
/*									*/
/*  Align a strech of text to the tab before it.			*/
/*									*/
/************************************************************************/

static int docLayoutAlignTextToTab(
				ParticuleData *		pdd,
				const TextParticule *	tpp,
				int			part,
				int			done,
				ParticuleData *		pdTab,
				const TextParticule *	tpTab,
				int			tabKind,
				DocumentRectangle *	drText,
				const int		tabX0,
				const int		tabX1 )
    {
    int		visibleSinceTab= 0;
    int		shift= 0;

    int		x1= drText->drX1;

    if  ( done > 0 )
	{
	int	visibleX1= pdd[done-1].pdVisibleBBox.drX1;

	visibleSinceTab=
		    pdd[done-1].pdX0+ visibleX1- tabX0;
	}

    switch( tabKind )
	{
	case DOCtaLEFT:
	    if  ( done > 0 && tpTab && tpTab->tpKind == DOCkindTAB )
		{ pdd[0].pdFlags |= TPflagXMATCH_LEFT;	}
	    shift= 0;
	    break;

	case DOCtaDECIMAL:
	    if  ( done > 0 && pdTab )
		{
		int		d;

		for ( d= done- 1; d >= 0; d-- )
		    {
		    if  ( pdd[d].pdDecWidth < tpp[d].tpTwipsWide )
			{
			visibleSinceTab=
				    pdd[d].pdX0+ pdd[d].pdDecWidth- tabX0;
			break;
			}
		    }

		shift= tabX1- visibleSinceTab- tabX0;
		if  ( shift < 0 )
		    { LDEB(shift); shift= 0;	}
		else{ x1= tabX1;			}
		pdTab->pdTwipsWide= shift;
		pdTab->pdVisibleBBox.drX0= 0;
		pdTab->pdVisibleBBox.drX1= 0;
		}
	    else{
		if  ( ! pdTab )
		    { LXDEB(done,pdTab);	}
		else{
		    x1= tabX1;
		    pdTab->pdTwipsWide= tabX1- tabX0;
		    pdTab->pdVisibleBBox.drX0= 0;
		    pdTab->pdVisibleBBox.drX1= 0;
		    }
		}
	    break;

	case DOCtaRIGHT:

	    if  ( done > 0 && pdTab )
		{
		pdd[done-1].pdFlags |= TPflagXMATCH_RIGHT;

		/*  6  */
		shift= tabX1- visibleSinceTab- tabX0;
		if  ( shift < 0 )
		    { shift= 0;	}
		else{ x1= tabX1;			}
		if  ( pdTab )
		    {
		    pdTab->pdTwipsWide= shift;
		    pdTab->pdVisibleBBox.drX0= 0;
		    pdTab->pdVisibleBBox.drX1= 0;
		    }
		}
	    else{
		if  ( ! pdTab )
		    { LXDEB(done,pdTab);	}
		else{
		    x1= tabX1;
		    pdTab->pdTwipsWide= tabX1- tabX0;
		    pdTab->pdVisibleBBox.drX0= 0;
		    pdTab->pdVisibleBBox.drX1= 0;
		    }
		}
	    break;

	case DOCtaCENTER:
	    if  ( done > 0 && pdTab )
		{
		int	visibleX0= pdd[0].pdVisibleBBox.drX0;

		/*  6  */
		shift= tabX1- visibleSinceTab/ 2- tabX0;
		shift -= visibleX0/ 2;

		if  ( shift < 0 )
		    { shift= 0;	}
		else{ x1 += shift;			}
		pdTab->pdTwipsWide= shift;
		pdTab->pdVisibleBBox.drX0= 0;
		pdTab->pdVisibleBBox.drX1= 0;
		}
	    else{
		if  ( pdTab )
		    {
		    x1= tabX1;
		    pdTab->pdTwipsWide= tabX1- tabX0;
		    pdTab->pdVisibleBBox.drX0= 0;
		    pdTab->pdVisibleBBox.drX1= 0;
		    }
		}
	    break;

	default:
	    LDEB(tabKind);
	    shift= 0; break;
	}

    if  ( shift > 0 )
	{
	int		i;

	for ( i= 0; i < done; i++ )
	    { pdd[i].pdX0 += shift; }
	}

    drText->drX0= drText->drX0+ x1- drText->drX1;
    drText->drX1= x1;
    return 0;
    }

/************************************************************************/
/*									*/
/*  Handle the bullet of a list as if it were aligned to a tab.		*/
/*									*/
/*  Do two things:							*/
/*  1)  Report the alignment of the bullet as the settings of a tab	*/
/*	before any text in the paragraph.				*/
/*  2)  Report the end of the bullet as a temporary limit for the text	*/
/*	formatter to alow for shifting the bullet text as a piece of	*/
/*	text between bullets.						*/
/*									*/
/************************************************************************/

static int docLayoutBulletAsTab(	int *			pParticuleUpto,
					int *			pX0,
					int *			pTabX0,
					int *			pTabX1,
					int *			pTabParticule,
					int *			pTabKind,
					const ParagraphFrame *	pf,
					const BufferItem *	paraNode,
					const BufferDocument *	bd )
    {
    const DocumentRectangle *	drParaContent= &(pf->pfParaContentRect);
    struct ListOverride *	lo;
    struct DocumentList *	dl;
    const ListLevel *		ll;

    int * const			startPath= (int *)0;
    int * const			formatPath= (int *)0;
    const ParagraphProperties *	pp= &(paraNode->biParaProperties);

    int				particuleUpto= *pParticuleUpto;
    int				x0= *pX0;
    int				tabX0= *pTabX0;
    int				tabX1= *pTabX1;
    int				tabParticule= *pTabParticule;
    int				tabKind= *pTabKind;

    if  ( docGetListLevelOfParagraph( startPath, formatPath,
						&lo, &dl, &ll, pp, bd ) )
	{ LLDEB(paraNode->biParaListOverride,paraNode->biParaListLevel);	}
    else{
	switch( ll->llJustification )
	    {
	    case DOCllaLEFT:
		/*  b  */
		tabKind= DOCtaLEFT;
		break;

	    case DOCllaRIGHT:
		tabKind= DOCtaRIGHT;
		tabParticule= 0;
		tabX0= x0= pf->pfCellContentRect.drX0;
		tabX1= drParaContent->drX0+ paraNode->biParaFirstIndentTwips;
		break;

	    case DOCllaCENTER:
		tabKind= DOCtaCENTER;
		tabParticule= 0;
		tabX0= x0= pf->pfCellContentRect.drX0;
		tabX1= drParaContent->drX0+ paraNode->biParaFirstIndentTwips;
		break;

	    default:
		LDEB(ll->llJustification);
		break;
	    }

	if  ( ll->llFollow != DOCllfTAB	&&
	      tabKind != DOCtaLEFT		)
	    {
	    DocumentField *	dfHead= (DocumentField *)0;
	    DocumentSelection	dsInsideHead;
	    DocumentSelection	dsAroundHead;
	    int			partBegin;
	    int			partEnd;

	    if  ( docDelimitParaHeadField( &dfHead,
				    &dsInsideHead, &dsAroundHead,
				    &partBegin, &partEnd, paraNode, bd ) )
		{ LDEB(1);			}
	    else{ particuleUpto= partEnd+ 1;	}
	    }
	}

    *pParticuleUpto= particuleUpto;
    *pX0= x0;
    *pTabX0= tabX0;
    *pTabX1= tabX1;
    *pTabParticule= tabParticule;
    *pTabKind= tabKind;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Place as many particules on a line as possible. The caller can	*/
/*  require that a certain number of particules are accepted even if	*/
/*  these do not fit on the line. This is to prevent loops. A formatter	*/
/*  typically wants to be sure that at least one particule is accepted,	*/
/*  such that it can know that it actually advances.			*/
/*									*/
/*  a)  Paragraph numbers are aligned as if there is a tab at the	*/
/*	first line indent position. The kind of tab depends on the	*/
/*	alignment of the list level.					*/
/*  b)  As formatting starts at that position anyway, there is no need	*/
/*	to do anything special for left aligned bullets or paragraph	*/
/*	numbers.							*/
/*									*/
/*  1)  Negative shifts occur when the text to the right of the tab	*/
/*	does not fit.							*/
/*									*/
/************************************************************************/

static int docLayoutParticules( LineLayoutJob *			llj,
				int				part,
				int *				pFound,
				int *				pWordCount,
				DocumentRectangle *		drLine,
				int *				pFattestLineB,
				unsigned char *			pLineFlags,
				ParticuleData *			pdFrom,
				int				acceptAtLeast,
				int				x0 )
    {
    BufferDocument *		bd= llj->lljLayoutContext->lcDocument;

    const ParagraphFrame *	pf= llj->lljParagraphFrame;
    const DocumentRectangle *	drParaContent= &(pf->pfParaContentRect);
    const DocumentProperties *	dp= &(bd->bdProperties);
    BufferItem *		paraNode= llj->lljParaNode;
    unsigned char		xflags= 0x0;

    int				accepted;
    int				wordCount= 0;
    unsigned char		lineFlags= 0;

    int				tabKind= DOCtaLEFT;
    int				tabX0= 0;
    int				tabX1= 0;
    int				tabParticule= -1;
    int				particuleUpto= paraNode->biParaParticuleCount;

    ParticuleData *		pd= pdFrom;
    TabStop			ts;

    int				textAttrNr;
    TextAttribute		ta;

    llj->lljLineHeadStroff= paraNode->biParaParticules[part].tpStroff;

    drLine->drX0= x0;
    drLine->drY0= 0;
    drLine->drY1= 0;
    *pFattestLineB= 0;

    /*  a  */
    if  ( part == 0 && paraNode->biParaListOverride > 0 )
	{
	if  ( docLayoutBulletAsTab( &particuleUpto,
					&x0, &tabX0, &tabX1,
					&tabParticule, &tabKind,
					pf, paraNode, bd ) )
	    { LDEB(1);	}
	}

    accepted= 0;
    while( accepted < paraNode->biParaParticuleCount )
	{
	int				done;
	int				found;

	int				textWordCount;

	const AfmFontInfo *		afi;
	int				tab;
	int				nextTabX1;

	DocumentRectangle		drText;
	int				fattestTextBorder= 0;

	ParticuleData *			pdTab= (ParticuleData *)0;
	TextParticule *			tpTab= (TextParticule *)0;

	if  ( tabParticule >= 0 )
	    {
	    pdTab= pdFrom+ tabParticule;
	    tpTab= paraNode->biParaParticules+ tabParticule;

	    if  ( tpTab->tpKind == DOCkindTAB )
		{ tpTab->tpObjectNumber= pdTab->pdTabNumber;	}
	    }

	if  ( accepted >= acceptAtLeast )
	    { acceptAtLeast= 0;	}

	done= docPsLayoutText( llj, part,
				    &found, &textWordCount,
				    &drText, &fattestTextBorder,
				    &lineFlags, pd, particuleUpto,
				    acceptAtLeast, x0 );
	if  ( done < 0 )
	    { LDEB(done); return -1;	}

	includeRectangleY( drLine, &drText );
	if  ( *pFattestLineB < fattestTextBorder )
	    { *pFattestLineB=  fattestTextBorder;	}

	if  ( docLayoutAlignTextToTab( pd, paraNode->biParaParticules+ part,
					part, done, pdTab, tpTab, tabKind,
					&drText, tabX0, tabX1 ) )
	    { LDEB(done);	}

	x0= drText.drX1;

	wordCount += textWordCount;
	accepted += done; part += done; pd += done;

	xflags= 0x0;
	if  ( particuleUpto != paraNode->biParaParticuleCount	&&
	      found == PSfoundNOTHING				)
	    {
	    particuleUpto= paraNode->biParaParticuleCount;
	    found= PSfoundBULLET_END;
	    if  ( tabKind != DOCtaLEFT )
		{ xflags= TPflagXMATCH_LEFT;	}
	    }

	switch( found )
	    {
	    case PSfoundTAB:
	    case PSfoundBULLET_END:
		break;

	    case PSfoundNOTHING:
	    case PSfoundLINEFULL:

	    case PSfoundLINEBREAK:

	    case PSfoundBLOCKBREAK:
		*pFound= found;
		*pWordCount= wordCount;
		*pLineFlags |= lineFlags;
		drLine->drX1= x0;
		return accepted;

	    default:
		LDEB(found); return -1;
	    }

	afi= docLayoutGetAfi( &textAttrNr, &ta, &lineFlags,
				    llj->lljLayoutContext, paraNode, part );
	if  ( ! afi )
	    { XDEB(afi); return -1;	}

	tab= -1;
	if  ( docNextTabStop( &ts, &nextTabX1, &tab, &(llj->lljTabStopList),
				pf->pfCellContentRect.drX0, drParaContent->drX0,
				dp->dpTabIntervalTwips, x0 ) )
	    { LDEB(dp->dpTabIntervalTwips); nextTabX1= x0+ 720;	}

	if  ( ( tab < 0 || ts.tsAlignment == DOCtaLEFT )	&&
	      nextTabX1 > drParaContent->drX1			&&
	      accepted >= acceptAtLeast				)
	    {
	    if  ( accepted < 1 )
		{ LLDEB(paraNode->biParaParticuleCount,accepted);	}

	    *pFound= PSfoundLINEFULL;
	    *pWordCount= wordCount;
	    *pLineFlags |= lineFlags;
	    drLine->drX1= x0;
	    return accepted;
	    }

	if  ( tab >= 0 )
	    { tabKind= ts.tsAlignment;	}
	else{ tabKind= DOCtaLEFT;	}
	tabX0= x0;
	tabX1= nextTabX1;
	tabParticule= accepted;

	docInitParticuleData( pd );
	pd->pdAfi= afi;
	pd->pdX0= x0;
	pd->pdTwipsWide= nextTabX1- x0;
	pd->pdDecWidth= nextTabX1- x0;
	geoInitRectangle( &(pd->pdVisibleBBox) );
	pd->pdTabNumber= tab;
	pd->pdTabKind= tabKind;
	pd->pdTabPosition= nextTabX1;
	pd->pdVisiblePixels= 0;
	pd->pdWhiteUnits= 0;
	pd->pdCorrectBy= 0;

	if  ( tabKind == DOCtaLEFT )
	    { x0= nextTabX1;	}

	pd->pdFlags |= xflags;
	accepted++; part++; pd++;
	}

    if  ( accepted < 1 )
	{ LLDEB(paraNode->biParaParticuleCount,accepted);	}

    *pFound= PSfoundNOTHING;
    *pWordCount= wordCount;
    *pLineFlags |= lineFlags;
    drLine->drX1= x0;

    return accepted;
    }

/************************************************************************/
/*									*/
/*  Justify the particules in a line of text.				*/
/*									*/
/*  1)  Start justification after the last tab of the line. Justifying	*/
/*	after anything else than a left tab is ridiculous. Simply	*/
/*	refuse to.							*/
/*  2)  Ignore organisational particules such as the delimitation of	*/
/*	links and bookmarks at the end of the line.			*/
/*									*/
/************************************************************************/

static void docJustifyLine(	const BufferItem *	paraNode,
				const TextParticule *	tpFrom,
				ParticuleData *		pdFrom,
				int			accepted,
				int			x1TextLines )
    {
    int				i;
    int				extra;
    int				totalWeight;
    int				step;
    int				left;
    int				visibleX1;

    ParticuleData *		pd;
    const TextParticule *	tp;

    /*  1  */
    left= 0;
    tp= tpFrom; pd= pdFrom;
    for ( i= 0; i < accepted- 1; tp++, pd++, i++ )
	{
	if  ( tp->tpKind == DOCkindTAB )
	    { left= i+ 1;	}
	}

    if  ( left > 0 )
	{
	if  ( pdFrom[left-1].pdTabKind != DOCtaLEFT )
	    { LDEB(pdFrom[left-1].pdTabKind); return;	}

	tpFrom += left; pdFrom += left; accepted -= left;
	}

    /*  2  */
    while( accepted > 0 && tpFrom[accepted- 1].tpStrlen == 0 )
	{ accepted--;	}

    if  ( accepted < 2 )
	{ /* LDEB(accepted); */ return;	}

    visibleX1=  pdFrom[accepted- 1].pdX0+
			    pdFrom[accepted- 1].pdVisibleBBox.drX1;
    extra= x1TextLines- visibleX1;

    if  ( extra < 0 )
	{
	LLLDEB(extra,x1TextLines,visibleX1);
	tp= tpFrom; pd= pdFrom;
	for ( i= 0; i < accepted; tp++, pd++, i++ )
	    {
	    appDebug( "%-4s: %5d..%5d \"%*.*s\"\n",
		    docKindStr( tp->tpKind ),
		    pd->pdX0, pd->pdX0+ pd->pdVisibleBBox.drX1,
		    tp->tpStrlen, tp->tpStrlen,
		    docParaString( paraNode, tp->tpStroff ) );
	    }

	extra= 0;
	}

    totalWeight= 0;
    tp= tpFrom; pd= pdFrom;
    for ( i= 0; i < accepted- 1; tp++, pd++, i++ )
	{
	pd->pdWhiteUnits= 0;
	pd->pdCorrectBy= 0;

	if  ( docParaPastLastNonBlank( paraNode, tp->tpStroff,
					    tp->tpStroff+ tp->tpStrlen ) <
					    tp->tpStroff+ tp->tpStrlen )
	    {
	    int		visibleWidth= pd[1].pdVisibleBBox.drX1;

	    pd->pdWhiteUnits=
		sqrt( (double)tp[0].tpTwipsWide+ (double)visibleWidth );

	    totalWeight += pd->pdWhiteUnits;
	    }
	}

    if  ( totalWeight == 0 )
	{ totalWeight= 1;	}

    left= extra;
    tp= tpFrom; pd= pdFrom;
    for ( i= 0; i < accepted- 1; tp++, pd++, i++ )
	{
	if  ( pd->pdWhiteUnits > 0 )
	    {
	    step= ( extra* pd->pdWhiteUnits )/ totalWeight;
	    if  ( step > left )
		{ step= left;	}

	    pd->pdCorrectBy += step;

	    left -= step;
	    }
	}

    tp= tpFrom; pd= pdFrom;
    for ( i= 0; i < accepted- 1; tp++, pd++, i++ )
	{
	if  ( pd->pdWhiteUnits > 0 )
	    {
	    step= 1;
	    if  ( step > left )
		{ step= left;	}

	    pd->pdCorrectBy += step;

	    left -= step;
	    }
	}

    step= 0;
    tp= tpFrom; pd= pdFrom;
    for ( i= 0; i < accepted- 1; tp++, pd++, i++ )
	{
	pd->pdX0 += step;
	if  ( pd->pdWhiteUnits > 0 )
	    {
	    pd->pdTwipsWide += pd->pdCorrectBy;
	    step += pd->pdCorrectBy;
	    }
	}

    pd->pdX0 += step;

    return;
    }

/************************************************************************/

static int docLayoutGetLineX0(	const BufferItem *		paraNode,
				int				part,
				const ParagraphFrame *		pf )
    {
    int				x0;

    x0= pf->pfParaContentRect.drX0;
    if  ( part == 0 )
	{ x0 += paraNode->biParaFirstIndentTwips;	}

    return x0;
    }

/************************************************************************/
/*									*/
/*  Place as many particules on a line as possible.			*/
/*									*/
/*  1)  Accept at least one particule.					*/
/*  A)  It looks nicer when fields do not start at the end of the line.	*/
/*	For the formatting it self this is irrelevant, but when a field	*/
/*	opens on the line before its contents, the shading of the	*/
/*	selection looks silly.						*/
/*  B)  Add leading to obey line spacing.				*/
/*  9)  Make \page mean \line in headers and footers and inside tables.	*/
/*									*/
/************************************************************************/

int docLayoutLineBox(	TextLine *			tl,
			BufferItem *			paraNode,
			int				part,
			const LayoutContext *		lc,
			ParticuleData *			pd,
			const ParagraphFrame *		pf )
    {
    int				xShift;
    unsigned char		lineFlags= TLflagINLINECONTENT;
    unsigned char		prevFlags= 0x0;

    int				accepted;
    int				found;
    int				wordCount;
    int				majorityFontSizeTwips;
    int				lineStride;

    int				x0;
    int				visibleX0Twips;
    int				visibleX1Twips;

    int				p;

    DocumentRectangle		drLine;
    int				fattestBorder= 0;

    DocumentRectangle		drTypicalLine;

    LineLayoutJob		llj;

    llj.lljParaNode= paraNode;
    llj.lljLayoutContext= lc;
    llj.lljParagraphFrame= pf;

    docGetTabStopListByNumber( &(llj.lljTabStopList), lc->lcDocument,
					paraNode->biParaTabStopListNumber );

    drTypicalLine= pf->pfParaContentRect;
    drTypicalLine.drY0= paraNode->biParaMajorityFontAscY0;
    drTypicalLine.drY1= paraNode->biParaMajorityFontDescY1;

    x0= docLayoutGetLineX0( paraNode, part, pf );

    /*  1  */
    accepted= docLayoutParticules( &llj, part, &found, &wordCount,
						&drLine, &fattestBorder,
						&lineFlags, pd, 1, x0 );
    if  ( accepted <= 0 )
	{ LDEB(accepted); return -1;	}

    /*  A  */
    while( accepted > 1							&&
	   paraNode->biParaParticules[part+accepted-1].tpKind
						== DOCkindFIELDHEAD	)
	{ accepted--;	}

    tl->tlWordCount= wordCount;

    /********************************************************************/
    /*  Found out by printing text on a piece of paper with a pica	*/
    /*  grid as a background.						*/
    /********************************************************************/

    includeRectangleY( &drLine, &drTypicalLine );

    majorityFontSizeTwips= 10* paraNode->biParaMajorityFontSize;
    lineStride= ( 115* majorityFontSizeTwips )/ 100;
    /*LINEDISTANCE*/

    if  ( paraNode->biParaLineSpacingTwips < 0 )
	{
	int		fixedStride= -paraNode->biParaLineSpacingTwips;
	int		heightLeft= fixedStride- drLine.drY1+ drLine.drY0;
	int		ascLeft= ( heightLeft+ 1 )/ 2;
	int		descLeft= heightLeft/ 2;

	if  ( ascLeft > 0 )
	    { drLine.drY0 -= ascLeft;	}
	if  ( descLeft > 0 )
	    { drLine.drY1 += descLeft;	}

	lineStride= fixedStride;
	}
    else{
	drLine.drY0 -=    fattestBorder;
	drLine.drY1 +=    fattestBorder;
	lineStride  += 2* fattestBorder;
	}

    if  ( paraNode->biParaLineSpacingTwips > 0			&&
	  ( part+ accepted < paraNode->biParaParticuleCount	||
	    paraNode->biParaLineSpacingIsMultiple		)	)
	{
	if  ( lineStride < paraNode->biParaLineSpacingTwips )
	    { lineStride=  paraNode->biParaLineSpacingTwips;	}
	}

    if  ( paraNode->biParaLineSpacingTwips >= 0 )
	{
	if  ( drLine.drY0 < drTypicalLine.drY0 )
	    { lineStride += drTypicalLine.drY0- drLine.drY0;	}
	if  ( drLine.drY1 > drTypicalLine.drY1 )
	    { lineStride += drLine.drY1- drTypicalLine.drY1;	}
	}

    /*  B  */
    tl->tlAscY0= drLine.drY0;
    tl->tlDescY1= drLine.drY1;
    tl->tlLineStride= lineStride;

    visibleX0Twips= x0;
    visibleX1Twips= x0;
    {
    ParticuleData *	pdd= pd+ accepted- 1;

    visibleX0Twips= pd->pdVisibleBBox.drX0;
    visibleX1Twips= pdd->pdX0+ pdd->pdVisibleBBox.drX1;
    }

    switch( paraNode->biParaAlignment )
	{
	case DOCthaLEFT:
	    pd->pdFlags |= TPflagXMATCH_LEFT;
	    xShift= 0;
	    break;

	case DOCthaRIGHT:
	    pd[accepted- 1].pdFlags |= TPflagXMATCH_RIGHT;
	    xShift= pf->pfParaContentRect.drX1- visibleX1Twips;
	    break;

	case DOCthaCENTERED:
	    xShift= ( pf->pfParaContentRect.drX1- visibleX1Twips )/ 2;
	    xShift -= visibleX0Twips/ 2;
	    break;

	case DOCthaJUSTIFIED:
	    pd->pdFlags |= TPflagXMATCH_LEFT;
	    if  ( part+ accepted < paraNode->biParaParticuleCount	)
		{ pd[accepted- 1].pdFlags |= TPflagXMATCH_RIGHT;	}
	    xShift= 0;
	    if  ( found == PSfoundLINEFULL )
		{
		docJustifyLine( paraNode, paraNode->biParaParticules+ part,
				    pd, accepted, pf->pfParaContentRect.drX1 );
		}
	    break;
	default:
	    LDEB(paraNode->biParaAlignment); xShift= 0;
	    break;
	}

    if  ( xShift != 0 )
	{
	ParticuleData *	pdd= pd;

	for ( p= 0; p < accepted; pdd++, p++ )
	    { pdd->pdX0 += xShift;	}
	}

    prevFlags= 0x0;
    for ( p= 0; p < accepted; p++ )
	{
	if  ( prevFlags & TPflagRIGHT_BORDER )
	    { pd[p].pdFlags |= TPflagXMATCH_LEFT;	}

	paraNode->biParaParticules[part+p].tpTwipsWide= pd[p].pdTwipsWide;
	paraNode->biParaParticules[part+p].tpFlags= pd[p].pdFlags;

	prevFlags= pd[p].pdFlags;
	}

    /*  9 */
    if  ( found == PSfoundBLOCKBREAK		&&
	  paraNode->biTreeType == DOCinBODY	&&
	  paraNode->biParaTableNesting == 0	)
	{ lineFlags |=  TLflagBLOCKBREAK;	}
    else{ lineFlags &= ~TLflagBLOCKBREAK;	}

    tl->tlLineIndent= pd->pdX0- pf->pfParaContentRect.drX0;
    tl->tlFlags= lineFlags;

    return accepted;
    }

