/************************************************************************/
/*									*/
/*  Print PostScript, include PDF marks to preserve links when the	*/
/*  PostScript is converted to PDF.					*/
/*									*/
/*  Though the Adobe Technical Note #5150: "pdfmark Reference Manual"	*/
/*  exhaustively documents the pdfmark functionality, the book:		*/
/*  Merz, Thomas, "Postscript & Acrobat/PDF", Springer-Verlag, Berlin	*/
/*  &c, 1996, ISBN 3-540-60854-0. gives a summary on page 289 that is	*/
/*  much easier to understand.						*/
/*									*/
/************************************************************************/

#   include	"docLayoutConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>

#   include	<sioFileio.h>
#   include	<drawMetafilePs.h>

#   include	"docDraw.h"
#   include	"docLayout.h"
#   include	"docPsPrintImpl.h"
#   include	"docPsPrint.h"
#   include	<docTreeNode.h>

#   include	<appDebugon.h>

#   define	SHOW_PAGE_GRID	0

/************************************************************************/
/*									*/
/*  Cause subsequent drawing to be done in a certain color.		*/
/*									*/
/*  NOTE that background drawing with color= 0 is against the RTF idea	*/
/*	of the default background color: transparent.			*/
/*									*/
/************************************************************************/

static int docPsSetColorRgb(	DrawingContext *	dc,
				void *			vps,
				const RGB8Color *	rgb8 )
    {
    PrintingState *		ps= (PrintingState *)vps;

    if  ( psSetRgb8Color( ps, rgb8 ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

static int docPsSetFont(	DrawingContext *	dc,
				void *			vps,
				int			textAttrNumber,
				const TextAttribute *	ta )
    {
    const LayoutContext *	lc= &(dc->dcLayoutContext);
    BufferDocument *		bd= lc->lcDocument;
    DocumentFontList *		dfl= bd->bdProperties.dpFontList;
    PrintingState *		ps= (PrintingState *)vps;
    const AfmFontInfo *		afi;
    const IndexSet *		unicodesWanted;

    const PostScriptFontList *	psfl= lc->lcPostScriptFontList;

    afi= (*lc->lcGetFontForAttribute)( &unicodesWanted, ta, dfl, psfl );
    if  ( ! afi )
	{ LDEB(textAttrNumber); return -1; }

    psSetFont( ps, afi, ta );

    return 0;
    }

/************************************************************************/

#   if  SHOW_PAGE_GRID

static void docPsPageBoxes(	DrawingContext *		dc,
				void *				vps,
				const DocumentGeometry *	dg )
    {
    PrintingState *		ps= (PrintingState *)vps;

    DocumentRectangle		drBBox;
    DocumentRectangle		drBody;
    DocumentRectangle		drHead;
    DocumentRectangle		drFoot;

    RGB8Color			rgb8;

    rgb8.rgb8Alpha= 255;

    utilDocumentGeometryGetPageBoundingBox( &drBBox, dg, 1, 1 );
    utilDocumentGeometryGetBodyRect( &drBody, dg );
    utilDocumentGeometryGetHeaderRect( &drHead, dg );
    utilDocumentGeometryGetFooterRect( &drFoot, dg );

    rgb8.rgb8Red= 255;
    rgb8.rgb8Green= 199;
    rgb8.rgb8Blue= 199;
    docPsSetColorRgb( dc, vps, &rgb8 );
    sioOutPrintf( ps->psSos, "newpath" );
    sioOutPrintf( ps->psSos, " %d %d moveto", drHead.drX0, drHead.drY0 );
    sioOutPrintf( ps->psSos, " %d %d lineto", drHead.drX1, drHead.drY0 );
    sioOutPrintf( ps->psSos, " %d %d lineto", drHead.drX1, drHead.drY1 );
    sioOutPrintf( ps->psSos, " %d %d lineto", drHead.drX0, drHead.drY1 );
    sioOutPrintf( ps->psSos, " %d %d lineto", drHead.drX0, drHead.drY0 );
    sioOutPrintf( ps->psSos, " fill\n" );

    rgb8.rgb8Red= 199;
    rgb8.rgb8Green= 199;
    rgb8.rgb8Blue= 255;
    docPsSetColorRgb( dc, vps, &rgb8 );
    sioOutPrintf( ps->psSos, "newpath" );
    sioOutPrintf( ps->psSos, " %d %d moveto", drFoot.drX0, drFoot.drY0 );
    sioOutPrintf( ps->psSos, " %d %d lineto", drFoot.drX1, drFoot.drY0 );
    sioOutPrintf( ps->psSos, " %d %d lineto", drFoot.drX1, drFoot.drY1 );
    sioOutPrintf( ps->psSos, " %d %d lineto", drFoot.drX0, drFoot.drY1 );
    sioOutPrintf( ps->psSos, " %d %d lineto", drFoot.drX0, drFoot.drY0 );
    sioOutPrintf( ps->psSos, " fill\n" );

    rgb8.rgb8Red= 199;
    rgb8.rgb8Green= 255;
    rgb8.rgb8Blue= 199;
    docPsSetColorRgb( dc, vps, &rgb8 );
    sioOutPrintf( ps->psSos, "newpath" );
    sioOutPrintf( ps->psSos, " %d %d moveto", drBody.drX0, drBody.drY0 );
    sioOutPrintf( ps->psSos, " %d %d lineto", drBody.drX1, drBody.drY0 );
    sioOutPrintf( ps->psSos, " %d %d lineto", drBody.drX1, drBody.drY1 );
    sioOutPrintf( ps->psSos, " %d %d lineto", drBody.drX0, drBody.drY1 );
    sioOutPrintf( ps->psSos, " %d %d lineto", drBody.drX0, drBody.drY0 );
    sioOutPrintf( ps->psSos, " fill\n" );

    rgb8.rgb8Red= 0;
    rgb8.rgb8Green= 255;
    rgb8.rgb8Blue= 0;
    docPsSetColorRgb( dc, vps, &rgb8 );
    sioOutPrintf( ps->psSos, "newpath" );
    sioOutPrintf( ps->psSos, " %d %d moveto", drBBox.drX0, drBBox.drY0 );
    sioOutPrintf( ps->psSos, " %d %d lineto", drBBox.drX1, drBBox.drY1 );
    sioOutPrintf( ps->psSos, " %d %d moveto", drBBox.drX0, drBBox.drY1 );
    sioOutPrintf( ps->psSos, " %d %d lineto", drBBox.drX1, drBBox.drY0 );
    sioOutPrintf( ps->psSos, " stroke\n" );
    }

#   endif

/************************************************************************/
/*									*/
/*  Skip to the next page.						*/
/*									*/
/*  1)  If anything is printed on the page, and header/footer printing	*/
/*      is postponed, print headers and footers for this page.		*/
/*  2)  If omitHeadersOnEmptyPages is set, dc->dcPostponeHeadersFooters	*/
/*	is derived from it. So any header printing is covered by (1)	*/
/*  3)  If the page is completely empty, skip it.			*/
/*  4)  if the page is empty, but not have been so if headers and	*/
/*	footers were printed, emit it. Otherwise skip it. This sounds a	*/
/*	bit strange, but the customer is always right. Actually, it has	*/
/*	a purpose: In simple documents, you do not want to waste paper.	*/
/*	So you should not print empty pages. In more complicated	*/
/*	documents, you do not want to print pages with only a header	*/
/*	and a footer to prevent fraud. For pagination purposes, you do	*/
/*	want to emit the page however. Now in stead of make the user	*/
/*	say what she wants with a multitude of parameters, we		*/
/*	distinguish between simple and complicated documents by		*/
/*	assuming that a complicated document has headers and footers.	*/
/*									*/
/************************************************************************/

static int docPsFinishPage(	void *				vps,
				DrawingContext *		dc,
				BufferItem *			bodySectBi,
				int				page,
				int				asLast )
    {
    PrintingState *	ps= (PrintingState *)vps;

    int			pageWasMarked;
    int			pageIsMarked;
    int			pageHasHeader= dc->dcDocHasPageHeaders;
    int			pageHasFooter= dc->dcDocHasPageFooters;
    int			skip= 0;

#   if 0
    docPsPageBoxes( dc, vps,
	&(dc->dcLayoutContext.lcDocument->bdProperties.dpGeometry) );
#   endif

    pageWasMarked= ps->psLastPageMarked >= ps->psPagesPrinted;

    /*
    sioOutPrintf( ps->psSos, "%% pageWasMarked= %d\n", pageWasMarked );
    sioOutPrintf( ps->psSos, "%% dcDocHasPageHeaders= %d\n",
					    dc->dcDocHasPageHeaders );
    sioOutPrintf( ps->psSos, "%% dcDocHasPageFooters= %d\n",
					    dc->dcDocHasPageFooters );
    */

    /*  1  */
    if  ( pageWasMarked			&&
	  dc->dcPostponeHeadersFooters	)
	{
	if  ( dc->dcDocHasPageHeaders				&&
	      docDrawPageHeader( bodySectBi, vps, dc, page )	)
	    { LLDEB(dc->dcDocHasPageHeaders,page);	}

	if  ( dc->dcDocHasPageFooters				&&
	      docDrawPageFooter( bodySectBi, vps, dc, page )	)
	    { LLDEB(dc->dcDocHasPageFooters,page);	}
	}

    /*  2  */

    pageIsMarked= ps->psLastPageMarked >= ps->psPagesPrinted;
    /*
    sioOutPrintf( ps->psSos, "%% pageIsMarked= %d\n", pageIsMarked );
    */

    /*  3  */
    if  ( ! pageIsMarked && ps->psPrintGeometry.pgSkipBlankPages )
	{ skip= 1;	}

    /*  4  */
    if  ( ! pageIsMarked )
	{
	const LayoutContext *	lc= &(dc->dcLayoutContext);
	const BufferDocument *	bd= lc->lcDocument;

	DocumentTree *			tree;
	int				isEmpty;

	if  ( dc->dcDocHasPageHeaders )
	    {
	    tree= (DocumentTree *)0;
	    docWhatPageHeader( &tree, &isEmpty, bodySectBi, page, bd );
	    if  ( ! tree || ! tree->dtRoot || isEmpty )
		{ pageHasHeader= 0;	}
	    }

	if  ( dc->dcDocHasPageFooters )
	    {
	    tree= (DocumentTree *)0;
	    docWhatPageFooter( &tree, &isEmpty, bodySectBi, page, bd );
	    if  ( ! tree || ! tree->dtRoot || isEmpty )
		{ pageHasFooter= 0;	}
	    }
	}

    /*
    sioOutPrintf( ps->psSos, "%% pageHasHeader= %d\n", pageHasHeader );
    sioOutPrintf( ps->psSos, "%% pageHasFooter= %d\n", pageHasFooter );
    */

    if  ( ! pageIsMarked			&&
	  ps->psPrintGeometry.pgSkipEmptyPages	&&
	  ! pageHasHeader			&&
	  ! pageHasFooter			)
	{ skip= 1;	}


    if  ( skip )
	{ psAbortPage( ps, page, asLast );	}
    else{
	if  ( psFinishPage( ps, page, asLast ) )
	    { LDEB(page); return -1;	}
	}

    return 0;
    }

static int docPsPrintStartPage(	void *				vps,
				const DocumentGeometry *	dgPage,
				DrawingContext *		dc,
				int				page )
    {
    PrintingState *	ps= (PrintingState *)vps;

    dc->dcCurrentTextAttributeSet= 0;
    dc->dcCurrentColorSet= 0;

    psRefreshNupSchema( ps, dgPage );

    if  ( psStartPage( ps, page ) )
	{ LDEB(page); return -1;	}

#   if SHOW_PAGE_GRID
    docPsPageBoxes( dc, vps,
	&(dc->dcLayoutContext.lcDocument->bdProperties.dpGeometry) );
#   endif

    return 0;
    }

/************************************************************************/
/*									*/
/*  Print a range of pages in a document.				*/
/*									*/
/************************************************************************/

static int docPsPrintPageRange(	PrintingState *		ps,
				DrawingContext *	dc,
				BufferItem *		bodyBi,
				int			firstPage,
				int			lastPage,
				int			asLast )
    {
    int				i;
    LayoutPosition		lpBelow;

    docInitLayoutPosition( &lpBelow );

    for ( i= 0; i < bodyBi->biChildCount; i++ )
	{
	if  ( bodyBi->biChildren[i]->biBelowPosition.lpPage >= firstPage )
	    { break;	}
	}

    if  ( i >= bodyBi->biChildCount )
	{ LDEB(i); return -1; }

    if  ( docPsPrintStartPage( (void *)ps,
	    &(bodyBi->biChildren[i]->biSectDocumentGeometry), dc, firstPage ) )
	{ LDEB(firstPage); return -1;	}

    if  ( ! dc->dcPostponeHeadersFooters )
	{
	docDrawPageHeader( bodyBi->biChildren[i], (void *)ps, dc, firstPage );
	}

    if  ( docDrawShapesForPage( (void *)ps, dc, 1, firstPage ) )
	{ LDEB(firstPage);	}

    docDrawNode( &lpBelow, bodyBi, (void *)ps, dc );

    if  ( lastPage < 0 )
	{ lastPage= bodyBi->biBelowPosition.lpPage;	}

    for ( i= bodyBi->biChildCount- 1; i >= 0; i-- )
	{
	if  ( bodyBi->biChildren[i]->biTopPosition.lpPage <= lastPage )
	    { break;	}
	}

    if  ( i < 0 )
	{ LDEB(i); return -1;	}

    if  ( docDrawShapesForPage( (void *)ps, dc, 0, lastPage ) )
	{ LDEB(lastPage);	}

    if  ( ! dc->dcPostponeHeadersFooters )
	{
	docDrawPageFooter( bodyBi->biChildren[i], (void *)ps, dc, lastPage );
	}

    docPsFinishPage( (void *)ps, dc, bodyBi->biChildren[i],
						    lastPage, asLast );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Print a document.							*/
/*									*/
/************************************************************************/

int docPsPrintDocument(	SimpleOutputStream *		sos,
			const char *			title,
			const char *			applicationName,
			const char *			applicationReference,
			const MemoryBuffer *		fontDirectory,
			double				shadingMesh,
			int				emitOutline,
			const LayoutContext *		lc,
			const PrintGeometry *		pg )
    {
    int				rval= 0;

    BufferDocument *		bd= lc->lcDocument;
    DocumentProperties *	dp= &(bd->bdProperties);
    DocumentGeometry *		dg= &(dp->dpGeometry);
    BufferItem *		bodyBi= bd->bdBody.dtRoot;

    PostScriptTypeList		pstl;

    DrawingContext		dc;
    PrintingState		ps;

    int				firstPage= pg->pgFirstPage;
    int				lastPage= pg->pgLastPage;

    INIT_LAYOUT_EXTERNAL	initLayoutExternal= (INIT_LAYOUT_EXTERNAL)0;

    psInitPostScriptFaceList( &pstl );
    if  ( utilCopyMemoryBuffer( &(pstl.pstlFontDirectory), fontDirectory ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( firstPage >= 0				||
	  lastPage >= 0					||
	  bd->bdBody.dtOutlineTree.lntnChildCount == 0	)
	{ emitOutline= 0;	}

    docInitDrawingContext( &dc );

    dc.dcSetColorRgb= docPsSetColorRgb;
    dc.dcSetFont= docPsSetFont;
    dc.dcDrawShape= docPsPrintDrawDrawingShape;
    dc.dcDrawObject= docPsPrintObject;
    dc.dcStartField= docPsPrintStartField;
    dc.dcFinishField= docPsPrintFinishField;
    dc.dcDrawTab= docPsPrintTab;
    dc.dcDrawFtnsep= docPsPrintFtnsep;
    dc.dcDrawSpan= docPsPrintSpan;
    dc.dcDrawUnderline= docPsPrintRunUnderline;
    dc.dcDrawStrikethrough= docPsPrintRunStrikethrough;

    dc.dcDrawTextLine= docPsPrintTextLine;
    dc.dcDrawOrnaments= docPsPrintOrnaments;
    dc.dcFinishPage= docPsFinishPage;
    dc.dcStartPage= docPsPrintStartPage;
    dc.dcInitLayoutExternal= initLayoutExternal;

    dc.dcLayoutContext= *lc;

    dc.dcFirstPage= pg->pgFirstPage;
    dc.dcLastPage= pg->pgLastPage;
    dc.dcDrawExternalItems= 1;
    dc.dcPostponeHeadersFooters= 0;

    if  ( pg->pgOmitHeadersOnEmptyPages )
	{ dc.dcPostponeHeadersFooters= 1;	}

    psInitPrintingState( &ps );
    ps.psSos= sos;
    ps.psUsePostScriptFilters= pg->pgUsePostScriptFilters;
    ps.psUsePostScriptIndexedImages= pg->pgUsePostScriptIndexedImages;
    ps.ps7Bits= pg->pg7Bits;

    ps.psPrintGeometry.pgEmbedFonts= pg->pgEmbedFonts;
    ps.psPrintGeometry.pgSkipEmptyPages= pg->pgSkipEmptyPages;
    ps.psPrintGeometry.pgSkipBlankPages= pg->pgSkipBlankPages;
    ps.psPrintGeometry.pgOmitHeadersOnEmptyPages=
					    pg->pgOmitHeadersOnEmptyPages;

    docInquireHeadersFooters( &(dc.dcDocHasPageHeaders),
				    &(dc.dcDocHasPageFooters), bd );

    if  ( ! utilMemoryBufferIsEmpty( &(dp->dpTitle) ) )
	{ title= utilMemoryBufferGetString( &(dp->dpTitle) );	}

    if  ( psSetNupSchema( &ps, dg, pg, dc.dcDocHasPageHeaders,
						    dc.dcDocHasPageFooters ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( docPsPrintGetDocumentFonts( &pstl, lc ) )
	{ LDEB(1); rval= -1; goto ready;	}

    psStartDSCDocument( &ps, &pstl,
			    title, applicationName, applicationReference );

    sioOutPrintf( sos, "%%%%BeginProlog\n" );

    psSetDefinePageFontImplementation( sos );
    psSetUtf8ShowImplementation( sos );
    psSetMvsImplementation( sos );

    docPsSaveTabLeaderProcedures( sos );
    psDefineBorderProcs( sos );

    psSetPdfmarkEmulation( sos );
    psSetRectfillEmulation( sos );
    psSetSelectfontEmulation( sos );

    psDefineEpsProcs( sos );

    if  ( pg->pgEmbedFonts )
	{ psIncludeFonts( sos, &pstl );	}
# if 0
    /* First fix definition of fonts in images */
    psSelectFontProcedures( sos, &pstl, /*allFonts=*/ 0 );
# else
    psSelectFontProcedures( sos, &pstl, /*allFonts=*/ 1 );
# endif

    appMetaDefineProcsetPs( sos );
    psSetPatternImplementation( sos, shadingMesh );

    sioOutPrintf( sos, "%%%%EndProlog\n" );
    sioOutPrintf( sos, "%%%%BeginSetup\n" );

#   if 1
    sioOutPrintf( sos, "<< /PageSize [%d %d] >> setpagedevice\n",
	    ( ps.psPrintGeometry.pgSheetGeometry.dgPageWideTwips+ 19 )/ 20, 
	    ( ps.psPrintGeometry.pgSheetGeometry.dgPageHighTwips+ 19 )/ 20 );
#   endif

    if  ( ps.psUsePostScriptFilters )
	{ psImageQualityDistillerparams( sos );	}

    if  ( pg->pgCustomPsSetupFilename )
	{
	SimpleInputStream *	sis;

	sis= sioInFileioOpenS( pg->pgCustomPsSetupFilename );
	if  ( ! sis )
	    { SXDEB(pg->pgCustomPsSetupFilename,sis);	}
	else{
	    unsigned char		buf[500];
	    int				got;

	    while( ( got= sioInReadBytes( sis, buf, sizeof(buf) ) ) > 0 )
		{ sioOutWriteBytes( sos, buf, got );	}

	    sioInClose( sis );
	    }
	}

    if  ( docHasDocumentInfo( dp ) )
	{
	docPsDocinfoPdfmark( &ps, applicationName, applicationReference, dp );
	}

    if  ( emitOutline )
	{
	docPsOutlinePdfmarks( &ps, bd );
	}

    sioOutPrintf( sos, "%%%%EndSetup\n" );

    if  ( firstPage < 0 )
	{ firstPage= 0;	}

    if  ( pg->pgPrintOddSides		&&
	  pg->pgPrintEvenSides		&&
	  ! pg->pgPrintSheetsReverse	&&
	  ! pg->pgPrintBookletOrder	)
	{
	if  ( docPsPrintPageRange( &ps, &dc, bodyBi,
				    firstPage, lastPage, /* asLast */ 1 ) )
	    { LDEB(firstPage); rval= -1; goto ready;	}
	}
    else{
	if  ( pg->pgPrintBookletOrder )
	    { LDEB(pg->pgPrintBookletOrder); }

	if  ( docPsPrintPageRange( &ps, &dc, bodyBi,
				    firstPage, lastPage, /* asLast */ 1 ) )
	    { LDEB(firstPage); rval= -1; goto ready;	}
	}

  ready:

    psCleanPrintingState( &ps );

    psCleanPostScriptFaceList( &pstl );

    docResetExternalTreeLayout( bd );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Booklet printing.							*/
/*									*/
/*  Document has n pages.						*/
/*  On the front (odd side) of sheet s left,right: page n-2s-1, 2s	*/
/*  On the back (even side) of sheet s left,right: page 2s-1, n-2s-2	*/
/*									*/
/************************************************************************/

