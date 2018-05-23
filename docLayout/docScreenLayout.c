/************************************************************************/
/*									*/
/*  Screen drawing specific geometry calculations.			*/
/*									*/
/*  The layout of a document is completely determined by the PostScript	*/
/*  layout process. For drawing on screen however, we have to live with	*/
/*  the fact that the fonts that we use on screen only approximately	*/
/*  match the ones that go into the PostScript/PDF files. For that	*/
/*  reason, we must apply approximations to the geometry. Those		*/
/*  approximations are made here.					*/
/*									*/
/*  In a left aligned paragraph, the first particule of a line gets a	*/
/*  fixed left position. In a right aligned paragraph the last one gets	*/
/*  a fixed right position. In a justified paragraph, the first one has	*/
/*  a fixed left position and the last one a fixed right one.		*/
/*									*/
/*  Inside a line, tabs get a pixel position that exactly matches the	*/
/*  twips position in PostScript. The same applies for the particules	*/
/*  after the vertical borders around stretches of text. The start and	*/
/*  end of shaded stretches are handled as borders.			*/
/*									*/
/*  If the text between fixed positions is left aligned:		*/
/*  -   The left hand sides of stretches have matching pixel and twips	*/
/*	positions. The rest of the stretch is placed using the pixel	*/
/*	width of the particules. (*)					*/
/*  If the text between fixed positions is right aligned:		*/
/*  -   The right hand sides of stretches have matching pixel and twips	*/
/*	positions. The rest of the stretch is placed using the pixel	*/
/*	width of the particules. (*)					*/
/*  If the text between fixed positions is justified:			*/
/*  -   The left and right hand sides of stretches have matching pixel	*/
/*	and twips positions. The particules in between are spread out	*/
/*	to fit in the space between the two positions.			*/
/*									*/
/*  (*) If more space is needed to draw in pixels than is available	*/
/*	from twips coordinates, the pixel coordinates are squeezed into	*/
/*	the twips space as if the text were justified.			*/
/*									*/
/************************************************************************/

#   include	"docLayoutConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>

#   include	<drawDrawingSurface.h>

#   include	"docLayout.h"
#   include	"docTextRun.h"
#   include	"docScreenLayout.h"
#   include	<geoGrid.h>

#   include	<docTreeNode.h>
#   include	<docShape.h>
#   include	<docTextLine.h>
#   include	<docTextParticule.h>
#   include	<docObjectProperties.h>
#   include	<docDebug.h>
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Calculate the width of a string on screen.				*/
/*									*/
/************************************************************************/

static int docScreenSegmentedWidth(
			DrawingSurface		ds,
			int			fullScreenFont,
			int			scapsScreenFont,
			const char *		s,
			const int *		segments,
			int			segmentCount )
    {
    int				seg;

    int				x= 0;
    int				y= 0;

    DocumentRectangle		drText;

    for ( seg= 0; seg < segmentCount; seg++ )
	{
	if  ( segments[2* seg+ 0] > 0 )
	    {
	    drawGetTextExtents( &drText, ds, x, y,
					    fullScreenFont,
					    s, segments[2* seg+ 0] );

	    s += segments[2* seg+ 0];
	    x= drText.drX1+ 1;
	    }

	if  ( segments[2* seg+ 1] > 0 )
	    {
	    drawGetTextExtents( &drText, ds, x, y,
					    scapsScreenFont,
					    s, segments[2* seg+ 1] );

	    s += segments[2* seg+ 1];
	    x= drText.drX1+ 1;
	    }
	}

    return x;
    }

int docScreenTextWidth(		int				fullScreenFont,
				const TextAttribute *		ta,
				const LayoutContext *		lc,
				const char *			printString,
				int				len )
    {
    char *		upperString= (char *)0;
    int *		segments= (int *)0;
    int			segmentCount= 0;

    int			wide;

    if  ( len < 0 )
	{ LDEB(len);	}
    if  ( len == 0 )
	{ return 0;	}

    if  ( ta->taSmallCaps || ta->taCapitals )
	{
	if  ( docMakeCapsString( &upperString, &segments, &segmentCount,
						    ta, printString, len ) )
	    { LDEB(len);		}
	else{ printString= upperString;	}
	}

    if  ( ta->taSmallCaps && ! ta->taCapitals )
	{
	int		scapsScreenFont= docLayoutScapsScreenFont( lc, ta );
	if  ( scapsScreenFont < 0 )
	    { LDEB(scapsScreenFont); return -1;	}

	wide= docScreenSegmentedWidth( lc->lcDrawingSurface,
				    fullScreenFont, scapsScreenFont,
				    printString, segments, segmentCount );
	}
    else{
	DocumentRectangle	drText;
	int			x= 0;
	int			y= 0;

	drawGetTextExtents( &drText, lc->lcDrawingSurface, x, y,
					fullScreenFont, printString, len );

	wide= drText.drX1- drText.drX0+ 1;
	}

    if  ( upperString )
	{ free( upperString );	}
    if  ( segments )
	{ free( segments );	}

    return wide;
    }

/************************************************************************/
/*									*/
/*  Place the particules in a text span.				*/
/*									*/
/*  NOTE: that the twips coordinates are handled from an input twips	*/
/*	coordinate that might be different from the PostScript twips	*/
/*	coordinates. As the values are ignored that is hardly relevant.	*/
/*									*/
/************************************************************************/

static int docScreenLayoutPlaceSpan(
				int *				pXPixels,
				int *				pVisXPixels,
				const BufferItem *		paraNode,
				int				part,
				int				upto,
				int				xPixels,
				int				xTwips,
				int				x0Frame,
				const LayoutContext *		lc )
    {
    TextParticule *	tp= paraNode->biParaParticules+ part;

    int			done;
    int			i;

    int			x0Span;
    int			len;
    int			wide;
    const char *	from;

    int			fullScreenFont;
    int			visXPixels= xPixels;
    const int		separate= 0;

    TextRun		tr;

    from= (char *)docParaString( paraNode, tp->tpStroff );
    x0Span= xPixels;

    done= docLayoutDelimitRun( &tr, xTwips,
			lc->lcDocument, paraNode, part, upto, separate );

    fullScreenFont= utilIndexMappingGet( lc->lcAttributeToScreenFont,
						    tr.trTextAttributeNr );

    len= 0;
    for ( i= 0; i < done; i++ )
	{
	tp->tpXContentXPixels= xPixels- x0Frame;

	len += tp->tpStrlen;
	wide= docScreenTextWidth( fullScreenFont,
				    &(tr.trTextAttribute), lc, from, len );
	xPixels= x0Span+ wide;

	part++; tp++;
	}

    wide= docScreenTextWidth( fullScreenFont, &(tr.trTextAttribute),
						lc, from, tr.trStrlenNb );
    visXPixels= x0Span+ wide;

    *pXPixels= xPixels;
    *pVisXPixels= visXPixels;
    return done;
    }

/************************************************************************/
/*									*/
/*  Place an individial particule. Text particules are handled by	*/
/*  docScreenLayoutPlaceSpan().						*/
/*									*/
/************************************************************************/

static int docScreenLayoutPlaceOneParticule(
				int *				pXPixels,
				const BufferItem *		paraNode,
				int				part,
				int				xPixels,
				int				x0Frame,
				const LayoutContext *		lc )
    {
    TextParticule *		tp= paraNode->biParaParticules+ part;
    int				wide;

    switch( tp->tpKind )
	{
	case DOCkindSPAN:
	    LDEB(tp->tpKind); return -1;

	case DOCkindTAB:
	    LDEB(tp->tpKind); return -1;

	case DOCkindOBJECT:
	    {
	    const InsertedObject *	io;

	    io= docGetObject( lc->lcDocument, tp->tpObjectNumber );
	    if  ( ! io )
		{ LPDEB(tp->tpObjectNumber,io);	}

	    if  ( io && io->ioInline )
		{ wide= io->ioPixelsWide;	}
	    else{ wide= 0;			}
	    }

	    tp->tpXContentXPixels= xPixels- x0Frame;
	    xPixels += wide;
	    break;

	case DOCkindCHFTNSEP:
	case DOCkindCHFTNSEPC:
	    wide= COORDtoGRID( lc->lcPixelsPerTwip, tp->tpTwipsWide );
	    tp->tpXContentXPixels= xPixels- x0Frame;
	    xPixels += wide;
	    break;

	case DOCkindFIELDHEAD:
	case DOCkindFIELDTAIL:
	case DOCkindLINEBREAK:
	case DOCkindPAGEBREAK:
	case DOCkindCOLUMNBREAK:

	    tp->tpXContentXPixels= xPixels- x0Frame;
	    break;

	case DOCkindOPT_HYPH:
	case DOCkindLTR_MARK:
	case DOCkindRTL_MARK:
	    if  ( tp->tpTwipsWide > 0 )
		{ LLDEB(tp->tpKind,tp->tpTwipsWide); /*return -1;*/	}

	    tp->tpXContentXPixels= xPixels- x0Frame;
	    break;

	default:
	    LDEB(tp->tpKind); return -1;
	}

    *pXPixels= xPixels;
    return 1;
    }

/************************************************************************/
/*									*/
/*  Place successive particules. the caller aligns the result by	*/
/*  shifting the particules.						*/
/*									*/
/************************************************************************/

static int docScreenLayoutPlaceParticules(
				int *				pXPixels,
				int *				pVisXPixels,
				const BufferItem *		paraNode,
				int				part,
				int				upto,
				int				xPixels,
				int				xTwips,
				int				x0Frame,
				const LayoutContext *		lc,
				ParticuleData *			pd )
    {
    TextParticule *		tp= paraNode->biParaParticules+ part;
    int				visXPixels= xPixels;

    while( part < upto )
	{
	int		done;

	if  ( tp->tpKind == DOCkindSPAN )
	    {
	    done= docScreenLayoutPlaceSpan( &xPixels, &visXPixels, paraNode, part, upto,
				    xPixels, xTwips, x0Frame, lc );
	    }
	else{
	    done= docScreenLayoutPlaceOneParticule( &xPixels, paraNode, part,
						    xPixels, x0Frame, lc );
	    visXPixels= xPixels;
	    }

	if  ( done < 1 )
	    { LDEB(done); return -1;	}

	part += done; tp += done; pd += done;
	if  ( part < upto )
	    { xTwips= pd->pdX0;	}
	}

    *pVisXPixels= visXPixels;
    *pXPixels= xPixels;
    return 0;
    }

/************************************************************************/
/*									*/
/*  Make particules fit between two margins.				*/
/*  (They have just been left aligned to the left margin.)		*/
/*  Realize that the PostScript formatter already performed the real	*/
/*  justification: We only have to adjust for the differences in	*/
/*  in geometry between the PostScript font and the screen font.	*/
/*									*/
/*  1)  Find last one to shift. I.E: The last one that is wider than	*/
/*	its visible part. We need at least two particules to change	*/
/*	the width of white space.					*/
/*  2)  No white space to stretch/squeeze?				*/
/*  3)  Position of particule for full shift.				*/
/*  4)  Shift particule offsets.					*/
/*									*/
/************************************************************************/

static void docScreenLayoutJustifyParticules(
				const BufferItem *		paraNode,
				int				part,
				int				upto,
				ParticuleData *			pd,
				int				shift )
    {
    TextParticule *		tp= paraNode->biParaParticules+ part;
    int				x0Twips= pd->pdX0;
    int				x1Twips;
    int				count= upto- part;
    int				last= -1;
    int				i;

    /*  1  */
    for ( i= count- 2; i >= 0; i-- )
	{
	if  ( pd[i].pdVisibleBBox.drX1 != pd[i].pdTwipsWide )
	    { last= i+ 1; break;	}
	}

    /*  2  */
    if  ( last < 0 )
	{ return;	}

    /*  3  */
    x1Twips= pd[last].pdX0;

    /*  4  */
    i= 0;
    while( i < count )
	{
	int	from= i++;

	while( i < last && pd[i].pdVisibleBBox.drX1 == pd[i].pdTwipsWide )
	    { i++;	}

	if  ( i > from && x1Twips > x0Twips )
	    {
	    int		w0= pd[from].pdX0- x0Twips;
	    int		w1= x1Twips- pd[from].pdX0;
	    int		sh= ( w0* shift )/ ( w0+ w1 );

	    while( from < i )
		{
		tp->tpXContentXPixels += sh;
		tp++; from++;
		}
	    }
	}

    return;
    }

static int docScreenLayoutAlignParticules(
				int *				pXPixels,
				const BufferItem *		paraNode,
				int				part,
				int				upto,
				int				align,
				int				x0Frame,
				int				x1Frame,
				const LayoutContext *		lc,
				ParticuleData *			pd )
    {
    TextParticule *		tp= paraNode->biParaParticules+ part;
    int				x0Twips;
    int				x1Twips;
    int				x0Pixels;
    int				xPixels;
    int				x1Pixels;
    int				visXPixels;
    int				shift;

    int				last= upto- part- 1;
    int				i;

    x0Twips= pd->pdX0;
    if  ( tp->tpFlags & TPflagLEFT_BORDER )
	{ x0Twips += pd->pdLeftBorderWidth;	}
    x0Pixels= docLayoutXPixels( lc, x0Twips );

    x1Twips= pd[last].pdX0+ pd[last].pdVisibleBBox.drX1;
    if  ( tp[last].tpFlags & TPflagRIGHT_BORDER )
	{ x1Twips -= pd->pdRightBorderWidth;	}
    x1Pixels= docLayoutXPixels( lc, x1Twips );

    if  ( docScreenLayoutPlaceParticules( &xPixels, &visXPixels,
			paraNode, part, upto,
			x0Pixels, x0Twips, x0Frame, lc, pd ) )
	{ LLDEB(part,upto); return -1;	}

    switch( align )
	{
	case DOCthaLEFT:
	    /************************************************************/
	    /*  Only squeeze the last line if it does not fit between	*/
	    /*  the margins.						*/
	    /************************************************************/
	    if  ( upto >= paraNode->biParaParticuleCount		&&
		  ! ( tp[last].tpFlags & TPflagXMATCH_RIGHT )	)
		{ x1Pixels= x1Frame;	}
	    shift= x1Pixels- visXPixels;

	    if  ( shift < 0 )
		{
		docScreenLayoutJustifyParticules( paraNode, part, upto, pd, shift );
		xPixels= x1Pixels;
		}

	    break;

	case DOCthaRIGHT:
	    shift= x1Pixels- visXPixels;
	    if  ( shift < 0 )
		{
		docScreenLayoutJustifyParticules( paraNode, part, upto, pd, shift );
		}
	    else{
		if  ( shift > 0 )
		    {
		    for ( i= part; i < upto; i++ )
			{ tp->tpXContentXPixels += shift; tp++;	}

		    xPixels= x1Pixels;
		    }
		}
	    break;

	case DOCthaJUSTIFIED:
	    shift= x1Pixels- visXPixels;
	    docScreenLayoutJustifyParticules( paraNode, part, upto, pd, shift );
	    xPixels= x1Pixels;
	    break;

	case DOCthaCENTERED:
	    LDEB(align); return -1;
	default:
	    LDEB(align); return -1;
	}

    *pXPixels= xPixels;
    return 0;
    }

static int docScreenLayoutScreenLine(
				const TextLine *		tl,
				const BufferItem *		paraNode,
				const LayoutContext *		lc,
				const ParagraphFrame *		pf,
				ParticuleData *			pdFrom )
    {
    const DocumentRectangle *	drParaContent= &(pf->pfParaContentRect);

    int				x0Frame= docLayoutXPixels( lc, drParaContent->drX0 );
    int				x1Frame= docLayoutXPixels( lc, drParaContent->drX1 );
    int				xPixels= x0Frame;
    int				from;

    int				part= tl->tlFirstParticule;
    TextParticule *		tp= paraNode->biParaParticules+ part;
    int				pastLine;

    pastLine= tl->tlFirstParticule+ tl->tlParticuleCount;

    from= part;
    tp= paraNode->biParaParticules+ from;
    while( from < pastLine )
	{
	int		align= -1;
	int		upto= from;
	ParticuleData *	pd= pdFrom+ from- tl->tlFirstParticule;
	unsigned char	fromFlags= tp->tpFlags;
	unsigned char	lastFlags= 0x0;

	if  ( tp->tpKind == DOCkindTAB )
	    {
	    tp->tpXContentXPixels= xPixels- x0Frame;
	    xPixels= docLayoutXPixels( lc, pd->pdX0+ pd->pdTwipsWide );
	    from++; tp++; continue;
	    }

	lastFlags= tp->tpFlags;
	upto++; tp++;
	while( upto < pastLine						&&
	       tp->tpKind != DOCkindTAB					&&
	       ! ( tp->tpFlags &
			    ( TPflagXMATCH_LEFT|TPflagXMATCH_RIGHT ) )	)
	    {
	    lastFlags= tp->tpFlags;
	    upto++; tp++;
	    }

	if  ( upto < pastLine						&&
	       tp->tpKind != DOCkindTAB					&&
	      ( tp->tpFlags &
			( TPflagXMATCH_LEFT|TPflagXMATCH_RIGHT ) ) ==
						    TPflagXMATCH_RIGHT	)
	    {
	    lastFlags= tp->tpFlags;
	    upto++; tp++;
	    }

	if  ( fromFlags & TPflagXMATCH_LEFT )
	    {
	    if  ( lastFlags & TPflagXMATCH_RIGHT )
		{ align= DOCthaJUSTIFIED;	}
	    else{ align= DOCthaLEFT;		}
	    }
	else{
	    if  ( lastFlags & TPflagXMATCH_RIGHT )
		{ align= DOCthaRIGHT;	}
	    else{
		/*
		LLXXDEB(from,upto,fromFlags,lastFlags);
		*/
		align= DOCthaLEFT;
		}
	    }

	if  ( from < upto )
	    {
	    if  ( docScreenLayoutAlignParticules( &xPixels, paraNode, from, upto,
					align, x0Frame, x1Frame, lc, pd ) )
		{ LLLDEB(from,upto,align); return -1;	}
	    }

	from= upto;
	}

    return tl->tlParticuleCount;
    }

/************************************************************************/
/*									*/
/*  Derive the frame for a paragraph from the page rectangle and the	*/
/*  paragraph properties.						*/
/*									*/
/*  For paragraphs inside a table cell, geometry is derived from the	*/
/*  table column.							*/
/*									*/
/************************************************************************/

static int docScreenLayoutStartScreenPara(
					BufferItem *		paraNode,
					const ParagraphFrame *	pf,
					const LayoutContext *	lc )
    {
    int			part;
    TextParticule *	tp;

    double		xfac= lc->lcPixelsPerTwip;

    if  ( docScreenLayoutOpenParaFonts( lc, paraNode ) )
	{ LDEB(1); return -1;	}

    tp= paraNode->biParaParticules;
    for ( part= 0; part < paraNode->biParaParticuleCount; tp++, part++ )
	{
	if  ( tp->tpKind != DOCkindOBJECT )
	    { continue;	}

	if  ( 1 )
	    {
	    BufferDocument *	bd= lc->lcDocument;
	    InsertedObject *	io= docGetObject( bd, tp->tpObjectNumber );

	    if  ( ! io )
		{ LPDEB(tp->tpObjectNumber,io); return -1;	}

	    docObjectSetPixelSize( io, xfac );
	    }
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Open screen fonts for all text attributes in a paragraph.		*/
/*									*/
/************************************************************************/

int docScreenLayoutOpenParaFonts(
				const LayoutContext *	lc,
				BufferItem *		paraNode )
    {
    int			part;
    int			textAttrNr0= -1;
    BufferDocument *	bd= lc->lcDocument;

    for ( part= 0; part < paraNode->biParaParticuleCount; part++ )
	{
	TextAttribute	ta;
	int		textAttrNr;

	textAttrNr= docGetEffectiveTextAttributes( &ta, bd, paraNode, part );

	if  ( textAttrNr == textAttrNr0 )
	    { continue;	}

	textAttrNr0= textAttrNr;

	if  ( utilIndexMappingGet( lc->lcAttributeToScreenFont,
							textAttrNr0 ) >= 0 )
	    { continue;	}

	if  ( docOpenScreenFont( lc, textAttrNr0 ) < 0 )
	    {
	    LLDEB(part,textAttrNr0);
	    docListNode(0,paraNode,0);
	    return -1;
	    }
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Various layout routines.						*/
/*									*/
/************************************************************************/

void docSetScreenLayoutFunctions(	LayoutJob *	lj )
    {
    lj->ljStartScreenParagraph= docScreenLayoutStartScreenPara;
    lj->ljLayoutScreenLine= docScreenLayoutScreenLine;

    return;
    }

int docScreenLayoutNode(	int *			pReachedBottom,
				BufferItem *		node,
				const LayoutContext *	lc,
				DocumentRectangle *	drChanged )
    {
    LayoutJob			lj;

    docInitLayoutJob( &lj );

    if  ( lc->lcDrawingSurface )
	{ docSetScreenLayoutFunctions( &lj );	}

    lj.ljChangedRectanglePixels= drChanged;
    lj.ljContext= *lc;
    lj.ljChangedNode= node;

    if  ( docLayoutNodeAndParents( node, &lj ) )
	{ LDEB(1); return -1;	}

    if  ( pReachedBottom && lj.ljReachedDocumentBottom )
	{ *pReachedBottom= 1;	}

    docCleanLayoutJob( &lj );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Delimit tables in the shape texts.					*/
/*									*/
/************************************************************************/

static int docDelimitTablesInShapeText(	int		n,
					void *		vio,
					void *		vdps )
    {
    InsertedObject *		io= (InsertedObject *)vio;
    DrawingShape *		ds;
    const int			recursively= 1;

    if  ( io->ioKind != DOCokDRAWING_SHAPE )
	{ return 0;	}

    ds= io->ioDrawingShape;

    if  ( ds->dsDocumentTree.dtRoot )
	{ docDelimitTables( ds->dsDocumentTree.dtRoot, recursively );	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  (re)Calculate the layout of a whole document.			*/
/*									*/
/************************************************************************/

int docScreenLayoutDocumentBody( int *				pReachedBottom,
				BufferDocument *		bd,
				const LayoutContext *		lc )
    {
    DocumentProperties *	dp= &(bd->bdProperties);

    DocumentRectangle		drChanged;

    geoInitRectangle( &drChanged );

    /*  1  */
    if  ( dp->dpFontList->dflFontCount == 0 )
	{ LDEB(dp->dpFontList->dflFontCount); return -1;	}

    if  ( docScreenLayoutNode( pReachedBottom,
					bd->bdBody.dtRoot, lc, &drChanged ) )
	{ LDEB(1); return -1;	}

    if  ( utilPagedListForAll( &(bd->bdObjectList.iolPagedList),
				docDelimitTablesInShapeText, (void *)0 ) < 0 )
	{ LDEB(1); return -1;	}

    /* LDEB(1);docListNode(0,bd->bdBody.dtRoot); */

    return 0;
    }

/************************************************************************/
/*									*/
/*  Calculate the layout of a page header.				*/
/*									*/
/************************************************************************/

int docStartScreenLayoutForTree(
				LayoutJob *			lj,
				DocumentTree *			ei,
				int				page,
				int				column )
    {
    const LayoutContext *	lc= &(lj->ljContext);

    if  ( lc->lcDrawingSurface )
	{ docSetScreenLayoutFunctions( lj );	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Determine the screen font number of the attribute belonging to the	*/
/*  scaps font size. Note that this is used for the shifted lower case	*/
/*  letters only.							*/
/*									*/
/************************************************************************/

int docLayoutScapsScreenFont(	const LayoutContext *	lc,
				const TextAttribute *	ta )
    {
    int		scapsAttrNr= docScapsAttributeNumber( lc->lcDocument, ta );
    int		scapsScreenFont;

    scapsScreenFont= docOpenScreenFont(	lc, scapsAttrNr );
    if  ( scapsScreenFont < 0 )
	{ LLDEB(scapsAttrNr,scapsScreenFont);	}

    return scapsScreenFont;
    }
