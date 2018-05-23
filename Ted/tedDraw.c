/************************************************************************/
/*									*/
/*  Ted, Screen drawing and forcing drawing through			*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>

#   include	"tedApp.h"
#   include	"tedSelect.h"
#   include	<docScreenLayout.h>
#   include	"tedDraw.h"
#   include	"tedLayout.h"
#   include	"tedDocument.h"
#   include	<docTreeType.h>
#   include	<docParaParticules.h>
#   include	<docDebug.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Cause the smallest rectangle that contains the selection to be	*/
/*  redrawn.								*/
/*									*/
/************************************************************************/

void tedExposeSelection(	EditDocument *			ed,
				const DocumentSelection *	ds,
				const BufferItem *		bodySectNode,
				int				scrolledX,
				int				scrolledY )
    {
    const int			lastLine= 0;
    SelectionGeometry		sg;
    LayoutContext		lc;

    layoutInitContext( &lc );
    tedSetScreenLayoutContext( &lc, ed );

    tedSelectionGeometry( &sg, ds, bodySectNode, lastLine, &lc );

    appDocExposeRectangle( ed, &(sg.sgRectangle), scrolledX, scrolledY );

    return;
    }

/************************************************************************/
/*									*/
/*  Blinking cursor.							*/
/*									*/
/*  1)  Turned off when we are debugging exposures and redraws.		*/
/*									*/
/************************************************************************/

void tedUndrawIBar(	const EditDocument *	ed )
    {
#   if BLINK_IBAR

    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    const int			scrolledX= 0;
    const int			scrolledY= 0;


    if  ( tedGetSelection( &ds, &sg, &sd,
			    (DocumentTree **)0, (BufferItem **)0, ed ) )
	{ LDEB(1); return;	}

#   if LOG_REDRAWS
    docLogRectangle( "BLINK*", &(sg.sgRectangle) );
#   endif

    appDocExposeRectangle( ed, &(sg.sgRectangle), scrolledX, scrolledY );

#   endif

    return;
    }

/************************************************************************/
/*									*/
/*  Cause subsequent drawing to be done in a certain color.		*/
/*									*/
/*  NOTE that background drawing with color= 0 is against the RTF idea	*/
/*	of the default background color: transparent.			*/
/*									*/
/************************************************************************/

int tedDrawSetColorRgb(		DrawingContext *	dc,
				void *			vsdd,
				const RGB8Color *	rgb8 )
    {
    const LayoutContext *	lc= &(dc->dcLayoutContext);

    drawSetForegroundColor( lc->lcDrawingSurface, rgb8 );

    return 0;
    }

static int tedDrawSetFont(	DrawingContext *	dc,
				void *			vsdd,
				int			textAttr,
				const TextAttribute *	ta )
    {
    const LayoutContext *	lc= &(dc->dcLayoutContext);
    int				screenFont;

    screenFont= utilIndexMappingGet( lc->lcAttributeToScreenFont, textAttr );
    if  ( screenFont < 0 )
	{ LLDEB(textAttr,screenFont); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Draw the white rectangle for a page.				*/
/*									*/
/************************************************************************/

static void tedDrawPageRect(	ScreenDrawingData *		sdd,
				DrawingContext *		dc,
				int				page )
    {
    const LayoutContext *	lc= &(dc->dcLayoutContext);

    DocumentRectangle		drPage;

    docGetPageRectPixels( &drPage, lc, page );

    if  ( dc->dcClipRect						&&
	  ! geoIntersectRectangle( &drPage, &drPage, dc->dcClipRect )	)
	{ return;	}

    geoShiftRectangle( &drPage, -lc->lcOx, -lc->lcOy );
    drawFillRectangle( lc->lcDrawingSurface, &drPage );

    return;
    }

/************************************************************************/
/*									*/
/*  Restore/instll original clip rect.					*/
/*									*/
/************************************************************************/

void tedOriginalClipping(	DrawingContext *		dc,
				ScreenDrawingData *		sdd )
    {
    const LayoutContext *	lc= &(dc->dcLayoutContext);

    if  ( dc->dcClipRect )
	{
	DocumentRectangle	drRestore;

	drRestore= *(dc->dcClipRect);
	geoShiftRectangle( &drRestore, -lc->lcOx, -lc->lcOy );

	drawSetClipRect( lc->lcDrawingSurface, &drRestore );
	}
    else{
	drawNoClipping( lc->lcDrawingSurface );
	}
    }

/************************************************************************/
/*									*/
/*  Draw an I Bar.							*/
/*									*/
/************************************************************************/

void tedGetIBarRect(	DocumentRectangle *		drPixels,
			const PositionGeometry *	pg,
			const LayoutContext *		lc )
    {
    docGetPixelRectForPos( drPixels, lc,
			    pg->pgXTwips, pg->pgXTwips,
			    &(pg->pgTopPosition), &(pg->pgBottomPosition) );

    drPixels->drX0= drPixels->drX1= pg->pgXPixels; /* Do not trust XTwips yet */

    return;
    }

int tedDrawIBar(	const DocumentRectangle *	drPixels,
			const LayoutContext *		lc )
    {
    DocumentRectangle	drShifted= *drPixels;

    geoShiftRectangle( &drShifted, -lc->lcOx, -lc->lcOy );

    drawFillRectangle( lc->lcDrawingSurface, &drShifted );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Draw the eight blocks around a selected object.			*/
/*									*/
/************************************************************************/

static void tedDrawObjectBlocks(	const DocumentRectangle *	drObj,
					const Point2DI *		xp,
					DrawingContext *		dc,
					ScreenDrawingData *		sdd )
    {
    const LayoutContext *	lc= &(dc->dcLayoutContext);

    int				i;

    DocumentRectangle		drBox;

    docDrawSetColorRgb( dc, (void *)sdd, &(sdd->sddBehindColor) );

    for ( i= 0; i < RESIZE_COUNT; i++ )
	{
	drBox.drX0= xp[i].x+ 1;
	drBox.drY0= xp[i].y+ 1;
	drBox.drX1= drBox.drX0+ RESIZE_BLOCK- 1;
	drBox.drY1= drBox.drY0+ RESIZE_BLOCK- 1;

	geoShiftRectangle( &drBox, -lc->lcOx, -lc->lcOy );
	drawFillRectangle( lc->lcDrawingSurface, &drBox );
	}

    docDrawSetColorRgb( dc, (void *)sdd, &(sdd->sddForeColor) );

    for ( i= 0; i < RESIZE_COUNT; i++ )
	{
	drBox.drX0= xp[i].x+ 1;
	drBox.drY0= xp[i].y+ 1;
	drBox.drX1= drBox.drX0+ RESIZE_BLOCK- 2;
	drBox.drY1= drBox.drY0+ RESIZE_BLOCK- 2;

	geoShiftRectangle( &drBox, -lc->lcOx, -lc->lcOy );
	drawRectangle( lc->lcDrawingSurface, &drBox );
	}

    drBox= *drObj;

    geoShiftRectangle( &drBox, -lc->lcOx, -lc->lcOy );
    drawRectangle( lc->lcDrawingSurface, &drBox );

    return;
    }

/************************************************************************/
/*									*/
/*  Draw the box around an active footnote, header or footer.		*/
/*									*/
/************************************************************************/

static void tedDrawTreeBox(	ScreenDrawingData *		sdd,
				DrawingContext *		dc,
				const DocumentRectangle *	drBox )
    {
    const LayoutContext *	lc= &(dc->dcLayoutContext);
    static unsigned char	dot[]= { 1, 2 };
    RGB8Color			rgb8;

    utilRGB8SolidBlack( &rgb8 );

    drawSetLineAttributes( lc->lcDrawingSurface,
		    1, LineStyleSingleDash, LineCapButt, LineJoinMiter,
		    dot, sizeof( dot ) );

    docDrawSetColorRgb( dc, (void *)sdd, &rgb8 );

    drawLine( lc->lcDrawingSurface,
			    drBox->drX0- lc->lcOx, drBox->drY0- lc->lcOy,
			    drBox->drX1- lc->lcOx, drBox->drY0- lc->lcOy );
    drawLine( lc->lcDrawingSurface,
			    drBox->drX0- lc->lcOx, drBox->drY1- lc->lcOy,
			    drBox->drX1- lc->lcOx, drBox->drY1- lc->lcOy );
    drawLine( lc->lcDrawingSurface,
			    drBox->drX0- lc->lcOx, drBox->drY0- lc->lcOy,
			    drBox->drX0- lc->lcOx, drBox->drY1- lc->lcOy );
    drawLine( lc->lcDrawingSurface,
			    drBox->drX1- lc->lcOx, drBox->drY0- lc->lcOy,
			    drBox->drX1- lc->lcOx, drBox->drY1- lc->lcOy );

    }

static int tedDrawBoxAroundTree(
			    const BufferItem *		bodySectNode,
			    const DocumentTree *	tree,
			    ScreenDrawingData *		sdd,
			    DrawingContext *		dc,
			    const DocumentRectangle *	drClip )
    {
    const LayoutContext *	lc= &(dc->dcLayoutContext);
    
    DocumentRectangle		drIntersect;
    DocumentRectangle		drBox;

    const int			justUsed= 0;

    if  ( tree->dtPageSelectedUpon < 0 || tree->dtColumnSelectedIn < 0 )
	{
	LLDEB(tree->dtPageSelectedUpon,tree->dtColumnSelectedIn);
	SDEB(docTreeTypeStr(tree->dtRoot->biTreeType));
	return -1;
	}

    if  ( docGetBoxAroundTree( &drBox, bodySectNode, tree, justUsed,
		    tree->dtPageSelectedUpon, tree->dtColumnSelectedIn, lc ) )
	{ LDEB(1); return -1;	}

    if  ( ! geoIntersectRectangle( &drIntersect, &drBox, drClip ) )
	{ return 0;	}

    tedDrawTreeBox( sdd, dc, &drBox );

    return 0;
    }

static int tedDrawCheckPageOfSelectedTree(
				    SelectionGeometry *		sg,
				    const DocumentSelection *	ds,
				    DocumentTree *		selRootTree,
				    const LayoutContext *	lc )
    {
    int			changed= 0;
    DocumentRectangle	drChanged;

    BufferItem *	bodySectNode;

    geoInitRectangle( &drChanged );

    if  ( docCheckPageOfSelectedTree( &changed, &bodySectNode, &drChanged,
			selRootTree, lc, docStartScreenLayoutForTree ) )
	{ LDEB(1); return -1;	}

    if  ( changed )
	{
	const int		lastLine= 1;

	tedSelectionGeometry( sg, ds, bodySectNode, lastLine, lc );
	}

    return 0;
    }

/************************************************************************/

static int tedDrawSelectionBackground(
				TedDocument *			td,
				BufferItem *			selRootNode,
				DocumentTree *			selRootTree,
				ScreenDrawingData *		sdd,
				DrawingContext *		dc,
				const DocumentSelection *	ds,
				SelectionGeometry *		sg,
				const RGB8Color *		selColor)
    {
    LayoutPosition		lpBelow;

    if  ( td->tdDrawMonochrome )
	{
	utilRGB8SolidWhite( &(sdd->sddForeColor) );
	dc->dcDrawTextLine= tedDrawTextReverse;
	}
    else{
	sdd->sddBehindColor= *selColor;
	dc->dcDrawTextLine= tedDrawTextSelected;
	}

    docDrawSetColorRgb( dc, (void *)sdd, &(sdd->sddForeColor) );

    dc->dcDrawOrnaments= (DRAW_ORNAMENTS)0;
    dc->dcFinishPage= (FINISH_PAGE)0;
    dc->dcStartPage= (START_PAGE)0;

    dc->dcInitLayoutExternal= (INIT_LAYOUT_EXTERNAL)0;
    dc->dcDrawExternalItems= 0;
    dc->dcPostponeHeadersFooters= 0;

    dc->dcSelection= ds;
    dc->dcSelectionGeometry= sg;

    if  ( selRootNode->biTreeType != DOCinBODY				&&
	  selRootNode->biTreeType != DOCinSHPTXT			&&
	  tedDrawCheckPageOfSelectedTree( sg, ds,
				selRootTree, &(dc->dcLayoutContext) )	)
	{ LDEB(1); return -1; }

    docInitLayoutPosition( &lpBelow );

    if  ( docDrawNode( &lpBelow, selRootNode, (void *)sdd, dc ) )
	{ LDEB(1);	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Start a page: Draw an external tree box if needed.			*/
/*									*/
/*  1)  Selection not in an external tree?				*/
/*  2)  In range? covers the case where both numbers are -1 because	*/
/*	nothing needs to be done.					*/
/*  3)  Calculate box on this page.					*/
/*  4)  Intersects clip rectangle?					*/
/*  5)  Draw!								*/
/*									*/
/************************************************************************/

static int tedDrawStartPage(	void *				vsdd,
				const DocumentGeometry *	dgPage,
				DrawingContext *		dc,
				int				page )
    {
    ScreenDrawingData *		sdd= (ScreenDrawingData *)vsdd;
    DocumentTree *		selRootTree= sdd->sddSelRootTree;
    BufferItem *		selRootNode= sdd->sddSelRootNode;
    BufferItem *		selRootBodySectNode= sdd->sddSelRootBodySectNode;

    const LayoutContext *	lc= &(dc->dcLayoutContext);

    const int			justUsed= 0;
    DocumentRectangle		drBox;
    DocumentRectangle		drIntersect;

    /*  1  */
    if  ( ! selRootTree || ! selRootNode )
	{ return 0;	}

    /*  2  */
    if  ( sdd->sddBoxFirstPage > page		||
	  sdd->sddBoxLastPage < page		)
	{ return 0;	}

    /*  3  */
    if  ( docGetBoxAroundTree( &drBox, selRootBodySectNode, selRootTree,
					justUsed,
					selRootTree->dtPageSelectedUpon,
					selRootTree->dtColumnSelectedIn,
					lc ) )
	{ LDEB(1); return 0;	}

    /*  4  */
    if  ( dc->dcClipRect						&&
	  ! geoIntersectRectangle( &drIntersect, &drBox, dc->dcClipRect ) )
	{ return 0;	}

    /*  5  */
    tedDrawTreeBox( sdd, dc, &drBox );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Expose handler for documents.					*/
/*									*/
/*  2)  Clear background.						*/
/*  2b) If the selected area overlaps with the exposed region, draw the	*/
/*	selection background.						*/
/*  3)  Draw text.							*/
/*  4)	Draw page separators.						*/
/*  5)  Draw I bar if necessary.					*/
/*									*/
/************************************************************************/

void tedDrawRectangle(		EditDocument *		ed,
				DocumentRectangle *	drClipPixels,
				int			ox,
				int			oy )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    DrawingSurface		drsf= ed->edDrawingSurface;

    BufferDocument *		bd= td->tdDocument;
    BufferItem * const		rootNode= bd->bdBody.dtRoot;

    DrawingContext		dc;
    ScreenDrawingData		sdd;

    BufferItem *		selRootNode= (BufferItem *)0;

    RGB8Color			selColor;

    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    DocumentTree *		selRootTree= (DocumentTree *)0;
    BufferItem *		selRootBodySectNode= (BufferItem *)0;

    if  ( tedGetSelection( &ds, &sg, &sd,
			    (DocumentTree **)0, (BufferItem **)0, ed ) )
	{
	docInitDocumentSelection( &ds );
	docInitSelectionGeometry( &sg );
	}

    if  ( td->tdVisibleSelectionCopied )
	{ selColor= td->tdCopiedSelColor;	}
    else{ selColor= td->tdSelColor;		}

    docInitDrawingContext( &dc );

    tedSetScreenLayoutContext( &(dc.dcLayoutContext), ed );
    dc.dcDrawTableGrid= ( td->tdDrawTableGrid >= 0 );
    dc.dcClipRect= drClipPixels;
    dc.dcSetColorRgb= tedDrawSetColorRgb;
    dc.dcSetFont= tedDrawSetFont;
    dc.dcDrawShape= tedDrawDrawingShape;
    dc.dcDrawObject= tedDrawObject;
    dc.dcDrawTab= tedDrawTab;
    dc.dcDrawFtnsep= tedDrawChftnsep;
    dc.dcDrawSpan= tedDrawSpan;
    dc.dcDrawUnderline= tedDrawRunUnderline;
    dc.dcDrawStrikethrough= tedDrawRunStrikethrough;

    utilRGB8SolidBlack( &(sdd.sddForeColor) );
    utilRGB8SolidWhite( &(sdd.sddBehindColor) );

    sdd.sddGridColor= td->tdTableColor;

    sdd.sddFullRect= &(ed->edFullRect);

#   if LOG_REDRAWS
    docLogRectangle( "REDRAW", drClipPixels );
#   endif

    tedOriginalClipping( &dc, &sdd );

    /*  2  */
    docDrawSetColorRgb( &dc, (void *)&sdd, &(ed->edBackgroundColor) );

    {
    DocumentRectangle	drFill= *drClipPixels;

    geoShiftRectangle( &drFill, -ox, -oy );
    drawFillRectangle( drsf, &drFill );
    }

    /*  2a  */
    if  ( tedHasSelection( ed ) )
	{
	selRootNode= docGetSelectionRoot(
				&selRootTree, &selRootBodySectNode, bd, &ds );
	if  ( ! selRootNode )
	    { XDEB(selRootNode);	}
	}

    /*  3  */
    utilRGB8SolidBlack( &(sdd.sddForeColor) );
    utilRGB8SolidWhite( &(sdd.sddBehindColor) );

    sdd.sddSelRootNode= selRootNode;
    sdd.sddSelRootTree= selRootTree;
    sdd.sddSelRootBodySectNode= selRootBodySectNode;
    sdd.sddBoxFirstPage= -1;
    sdd.sddBoxLastPage= -1;

    if  ( selRootTree )
	{
	const BufferItem *	selRoolSectBi= selRootTree->dtRoot;

	if  ( selRoolSectBi->biTreeType == DOCinFOOTNOTE	||
	      selRoolSectBi->biTreeType == DOCinENDNOTE	)
	    {
	    sdd.sddBoxFirstPage= selRoolSectBi->biTopPosition.lpPage;
	    sdd.sddBoxLastPage= selRoolSectBi->biBelowPosition.lpPage;
	    }
	}

    dc.dcDrawTextLine= tedDrawTextLine;
    dc.dcDrawOrnaments= tedDrawOrnaments;

    dc.dcStartPage= tedDrawStartPage;

    dc.dcInitLayoutExternal= docStartScreenLayoutForTree;
    dc.dcDrawExternalItems= 0;
    dc.dcPostponeHeadersFooters= 0;

    dc.dcSelection= (DocumentSelection *)0;
    dc.dcSelectionGeometry= (SelectionGeometry *)0;

    dc.dcFirstPage= docGetPageForYPixels( &(dc.dcLayoutContext),
						    drClipPixels->drY0 );
    dc.dcLastPage=  docGetPageForYPixels( &(dc.dcLayoutContext),
						    drClipPixels->drY1 );
    if  ( dc.dcFirstPage > rootNode->biBelowPosition.lpPage )
	{ dc.dcFirstPage=  rootNode->biBelowPosition.lpPage; }
    if  ( dc.dcLastPage >  rootNode->biBelowPosition.lpPage )
	{ dc.dcLastPage=   rootNode->biBelowPosition.lpPage; }

    {
    int		page;

    docDrawSetColorRgb( &dc, (void *)&sdd, &(sdd.sddBehindColor) );

    for ( page= dc.dcFirstPage; page <= dc.dcLastPage; page++ )
	{ tedDrawPageRect( &sdd, &dc, page );	}
    }

    docDrawSetColorRgb( &dc, (void *)&sdd, &(sdd.sddForeColor) );

    dc.dcCurrentColorSet= 0;
    dc.dcCurrentTextAttributeSet= 0;

    if  ( docDrawPageRange( &dc, (void *)&sdd ) )
	{ LDEB(1);	}

    if  ( selRootNode			&&
	  ! sd.sdIsIBarSelection	)
	{
	if  ( tedDrawSelectionBackground( td, selRootNode, selRootTree,
					    &sdd, &dc, &ds, &sg, &selColor ) )
	    { LDEB(1); return;	}
	}

    if  ( selRootNode					&&
	  selRootNode->biTreeType != DOCinBODY		&&
	  selRootNode->biTreeType != DOCinSHPTXT	)
	{
	DocumentTree *	x_selRootTree;
	BufferItem *	bodySectNode;

	if  ( tedDrawCheckPageOfSelectedTree( &sg, &ds,
				selRootTree, &(dc.dcLayoutContext) )	)
	    { LDEB(1); return; }

	if  ( docGetRootOfSelectionScope( &x_selRootTree,
				&bodySectNode, bd, &(ds.dsSelectionScope) ) )
	    { LDEB(1);	}
	else{
	    if  ( sdd.sddSelRootTree != x_selRootTree )
		{ XXDEB(sdd.sddSelRootTree,x_selRootTree);	}

	    if  ( tedDrawBoxAroundTree( bodySectNode, x_selRootTree,
						    &sdd, &dc, drClipPixels ) )
		{ LDEB(1);	}
	    }
	}

    docDrawSetColorRgb( &dc, (void *)&sdd, &(sdd.sddForeColor) );

    /*  5  */
    if  ( selRootNode )
	{
	if  ( sd.sdIsIBarSelection )
	    {
	    DocumentRectangle		drIBarPixels;

	    tedGetIBarRect( &drIBarPixels,
				&(sg.sgHead), &(dc.dcLayoutContext) );

	    if  ( geoIntersectRectangle( &drIBarPixels,
					&drIBarPixels, drClipPixels )	&&
		  ! td->tdShowIBarId					)
		{ tedDrawIBar( &drIBarPixels, &(dc.dcLayoutContext) ); }
	    }
	else{
	    InsertedObject *	io;
	    int			partO;
	    DocumentPosition	dpO;

	    docInitDocumentPosition( &dpO );

	    if  ( sd.sdIsObjectSelection				&&
		  ! docGetObjectSelection( &ds, bd, &partO, &dpO, &io  ) )
		{
		PositionGeometry	pgO;

		Point2DI		xp[RESIZE_COUNT];
		DocumentRectangle	drObject;

		if  ( selRootNode->biTreeType != DOCinBODY		&&
		      selRootNode->biTreeType != DOCinSHPTXT		&&
		      tedDrawCheckPageOfSelectedTree( &sg, &ds,
				selRootTree, &(dc.dcLayoutContext) )	)
		    { LDEB(1); return; }

		sdd.sddForeColor.rgb8Red= 0;
		sdd.sddForeColor.rgb8Green= 0;
		sdd.sddForeColor.rgb8Blue= 0;

		docDrawSetColorRgb( &dc, (void *)&sdd, &(sdd.sddForeColor) );

		sdd.sddBehindColor= selColor;

		tedPositionGeometry( &pgO, &dpO,
					PARAfindLAST, &(dc.dcLayoutContext) );

		tedGetObjectRectangle( &drObject, xp, io, &pgO,
					    &(dc.dcLayoutContext), ed );

		if  ( geoIntersectRectangle( (DocumentRectangle *)0,
						    &drObject, drClipPixels ) )
		    { tedDrawObjectBlocks( &drObject, xp, &dc, &sdd );	}
		}
	    }
	}

    if  ( ed->edSelectRectangle.srDirection != DOCselNONE )
	{
	DocumentRectangle	drHair;
	int			blackSet= 0;

	drHair= ed->edVisibleRect;
	drHair.drX0= ed->edSelectRectangle.srSelected.drX0+
					ed->edSelectRectangle.srLTM.drX0;
	drHair.drX1= ed->edSelectRectangle.srSelected.drX0+
					ed->edSelectRectangle.srLTM.drX0;

	if  ( geoIntersectRectangle( &drHair, &drHair, drClipPixels ) )
	    {
	    if  ( ! blackSet )
		{ drawSetForegroundColorBlack( drsf ); blackSet= 1;	}

	    geoShiftRectangle( &drHair, -ox, -oy );
	    drawFillRectangle( drsf, &drHair );
	    }

	if  ( ed->edSelectRectangle.srLTM.drX1 !=
					    ed->edSelectRectangle.srLTM.drX0 )
	    {
	    drHair= ed->edVisibleRect;
	    drHair.drX0= ed->edSelectRectangle.srSelected.drX0+
					ed->edSelectRectangle.srLTM.drX1;
	    drHair.drX1= ed->edSelectRectangle.srSelected.drX0+
					ed->edSelectRectangle.srLTM.drX1;

	    if  ( geoIntersectRectangle( &drHair, &drHair, drClipPixels ) )
		{
		if  ( ! blackSet )
		    { drawSetForegroundColorBlack( drsf ); blackSet= 1;	}

		geoShiftRectangle( &drHair, -ox, -oy );
		drawFillRectangle( drsf, &drHair );
		}
	    }
	}

    return;
    }

