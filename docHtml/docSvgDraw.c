/************************************************************************/
/*									*/
/*  Draw Scalable Vector Graphics.					*/
/*									*/
/************************************************************************/

#   include	"docHtmlConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<string.h>

#   include	<docDraw.h>
#   include	<docShape.h>
#   include	<docLayout.h>
#   include	"docSvgDrawImpl.h"
#   include	"docWriteCss.h"
#   include	"docSvgDraw.h"
#   include	<psShading.h>
#   include	<docTreeNode.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Cause subsequent drawing to be done in a certain color.		*/
/*									*/
/*  NOTE that background drawing with color= 0 is against the RTF idea	*/
/*	of the default background color: transparent.			*/
/*									*/
/************************************************************************/

static int docSvgSetColorRgb(	DrawingContext *	dc,
				void *			vsw,
				const RGB8Color *	rgb8 )
    {
    return 0;
    }

static int docSvgSetFont(	DrawingContext *	dc,
				void *			vsw,
				int			textAttrNumber,
				const TextAttribute *	ta )
    {
    const LayoutContext *	lc= &(dc->dcLayoutContext);
    BufferDocument *		bd= lc->lcDocument;
    DocumentFontList *		dfl= bd->bdProperties.dpFontList;
    const AfmFontInfo *		afi;
    const IndexSet *		unicodesWanted;

    const PostScriptFontList *	psfl= lc->lcPostScriptFontList;

    afi= (*lc->lcGetFontForAttribute)( &unicodesWanted, ta, dfl, psfl );
    if  ( ! afi )
	{ LDEB(textAttrNumber); return -1; }

    return 0;
    }

/************************************************************************/
/*									*/
/*  Printing of borders.						*/
/*									*/
/*  1)  The border is around paragraphs. Both the border and its	*/
/*      are outside the frame.						*/
/*									*/
/************************************************************************/

static void docSvgWriteBorderAttributes(
				SvgWriter *			sw,
				const BufferDocument *		bd,
				const BorderProperties *	bp,
				int				thick )
    {
    RGB8Color			rgb8;
    XmlWriter *			xw= &(sw->swXmlWriter);

    if  ( thick < 8 )
	{ thick= 8;	}

    docGetColorByNumber( &rgb8, bd, bp->bpColor );

    xmlWriteRgb8Attribute( xw, "stroke", &rgb8 );
    xmlWriteIntAttribute( xw, "stroke-width", thick );

    return;
    }

static void docSvgDrawHorizontalBorder(
				SvgWriter *			sw,
				const BufferDocument *		bd,
				const BorderProperties *	bp,
				const DocumentRectangle *	drBorder )
    {
    XmlWriter *			xw= &(sw->swXmlWriter);

    sioOutPutString( "<line", xw->xwSos );

    xmlWriteIntAttribute( xw, "x1", drBorder->drX0 );
    xmlWriteIntAttribute( xw, "x2", drBorder->drX1 );
    xmlWriteIntAttribute( xw, "y1", ( drBorder->drY0+ drBorder->drY1 )/ 2 );
    xmlWriteIntAttribute( xw, "y2", ( drBorder->drY0+ drBorder->drY1 )/ 2 );
    docSvgWriteBorderAttributes( sw, bd, bp,
					drBorder->drY1- drBorder->drY0+ 1 );

    sioOutPutString( "/>", xw->xwSos );
    return;
    }

static void docSvgDrawVerticalBorder(
				SvgWriter *			sw,
				const BufferDocument *		bd,
				const BorderProperties *	bp,
				const DocumentRectangle *	drBorder )
    {
    XmlWriter *			xw= &(sw->swXmlWriter);

    sioOutPutString( "<line", xw->xwSos );

    xmlWriteIntAttribute( xw, "x1", ( drBorder->drX0+ drBorder->drX1 )/ 2 );
    xmlWriteIntAttribute( xw, "x2", ( drBorder->drX0+ drBorder->drX1 )/ 2 );
    xmlWriteIntAttribute( xw, "y1", drBorder->drY0 );
    xmlWriteIntAttribute( xw, "y2", drBorder->drY1 );
    docSvgWriteBorderAttributes( sw, bd, bp,
					drBorder->drX1- drBorder->drX0+ 1 );

    sioOutPutString( "/>", xw->xwSos );
    return;
    }

/************************************************************************/
/*									*/
/*  Draw background ornaments.						*/
/*									*/
/************************************************************************/

static int docSvgDrawOrnaments(	const BlockOrnaments *		bo,
				int				page,
				const DocumentRectangle *	drOutside,
				const DocumentRectangle *	drInside,
				void *				through,
				struct DrawingContext *		dc )
    {
    SvgWriter *			sw= (SvgWriter *)through;
    XmlWriter *			xw= &(sw->swXmlWriter);
    const LayoutContext *	lc= &(dc->dcLayoutContext);
    const BufferDocument *	bd= lc->lcDocument;

    int				rectBorder= 0;
    DocumentRectangle		drRectBorder;

    drRectBorder.drX0= ( 3* drOutside->drX0- drInside->drX0 )/ 2;
    drRectBorder.drX1= ( 3* drOutside->drX1- drInside->drX1 )/ 2;
    drRectBorder.drY0= ( 3* drOutside->drY0- drInside->drY0 )/ 2;
    drRectBorder.drY1= ( 3* drOutside->drY1- drInside->drY1 )/ 2;

#   if 0
    if  ( PROPmaskISSET( &(bo->boPropMask), ORNdrawTOP_BORDER )		&&
	  PROPmaskISSET( &(bo->boPropMask), ORNdrawLEFT_BORDER )	&&
	  PROPmaskISSET( &(bo->boPropMask), ORNdrawRIGHT_BORDER )	&&
	  PROPmaskISSET( &(bo->boPropMask), ORNdrawBOTTOM_BORDER )	&&
	  bo->boLeftBorderNumber == bo->boTopBorderNumber		&&
	  bo->boRightBorderNumber == bo->boTopBorderNumber		&&
	  bo->boBottomBorderNumber == bo->boTopBorderNumber		)
	{ rectBorder= 1;	}
#   endif

    if  ( PROPmaskISSET( &(bo->boPropMask), ORNdrawSHADE ) )
	{
	int				isFilled= 0;
	RGB8Color			rgb8;
	const DocumentRectangle *	drShade= drInside;

	if  ( rectBorder )
	    { drShade= &drRectBorder;	}

	if  ( docGetSolidRgbShadeOfItem( &isFilled, &rgb8,
						bd, &(bo->boShading) ) )
	    { LDEB(1);	}

	if  ( isFilled )
	    {
	    sioOutPutString( "<rect", xw->xwSos );
	    svgWriteRectangleAttributes( sw, drShade );
	    xmlWriteRgb8Attribute( xw, "fill", &rgb8 );

	    if  ( rectBorder )
		{
		docSvgWriteBorderAttributes( sw, bd, &(bo->boTopBorder),
					    drOutside->drY0- drInside->drY0 );
		}

	    sioOutPutString( "/>", xw->xwSos );
	    xmlNewLine( xw );
	    }

	if  ( bo->boShading.isPattern != DOCspSOLID )
	    { LDEB(bo->boShading.isPattern);	}
	}
    else{
	if  ( rectBorder )
	    {
	    sioOutPutString( "<rect", xw->xwSos );
	    svgWriteRectangleAttributes( sw, &drRectBorder );
	    xmlWriteStringAttribute( xw, "fill", "none" );
	    docSvgWriteBorderAttributes( sw, bd, &(bo->boTopBorder),
					    drOutside->drY0- drInside->drY0 );
	    sioOutPutString( "/>", xw->xwSos );
	    xmlNewLine( xw );
	    }
	}

    if  ( ! rectBorder							&&
	  PROPmaskISSET( &(bo->boPropMask), ORNdrawTOP_BORDER )		)
	{
	DocumentRectangle	drBorder= *drOutside;

	drBorder.drY1= drInside->drY0- 1;

	docSvgDrawHorizontalBorder( sw, bd, &(bo->boTopBorder), &drBorder );
	xmlNewLine( xw );
	/*done= 1;*/
	}

    if  ( ! rectBorder						&&
	  PROPmaskISSET( &(bo->boPropMask), ORNdrawLEFT_BORDER ) )
	{
	DocumentRectangle	drBorder= *drOutside;

	drBorder.drY0= drInside->drY0;
	drBorder.drY1= drInside->drY1;
	drBorder.drX1= drInside->drX0- 1;

	docSvgDrawVerticalBorder( sw, bd, &(bo->boLeftBorder), &drBorder );
	/*done= 1;*/
	}

    if  ( ! rectBorder						&&
	  PROPmaskISSET( &(bo->boPropMask), ORNdrawRIGHT_BORDER ) )
	{
	DocumentRectangle	drBorder= *drOutside;

	drBorder.drY0= drInside->drY0;
	drBorder.drY1= drInside->drY1;
	drBorder.drX0= drInside->drX1+ 1;

	docSvgDrawVerticalBorder( sw, bd, &(bo->boRightBorder), &drBorder );
	/*done= 1;*/
	}

    if  ( ! rectBorder							&&
	  PROPmaskISSET( &(bo->boPropMask), ORNdrawBOTTOM_BORDER )	)
	{
	DocumentRectangle	drBorder= *drOutside;

	drBorder.drY0= drInside->drY1+ 1;

	docSvgDrawHorizontalBorder( sw, bd, &(bo->boBottomBorder), &drBorder );
	/*done= 1;*/
	}

    return 0;
    }

/************************************************************************/

int docSvgEmitFill(	SvgWriter *			sw,
			const struct DrawingShape *	ds )
    {
    XmlWriter *		xw= &(sw->swXmlWriter);
    RGB8Color		rgb8Fill;
    int			fill;

    if  ( docShapeGetFill( &fill, &rgb8Fill, ds ) )
	{ LDEB(ds->dsDrawing.sdShapeType); return -1;	}

    if  ( fill )
	{ xmlWriteRgb8Attribute( xw, "fill", &rgb8Fill );	}
    else{ xmlWriteStringAttribute( xw, "fill", "none" );	}

    return 0;
    }

int docSvgEmitStroke(	SvgWriter *			sw,
			const DrawingShape *		ds )
    {
    const ShapeDrawing *	sd= &(ds->dsDrawing);
    XmlWriter *			xw= &(sw->swXmlWriter);
    RGB8Color			rgb8Stroke;
    int				stroke;

    if  ( docShapeGetLine( &stroke, &rgb8Stroke, ds ) )
	{ LDEB(sd->sdShapeType); return -1;	}

    if  ( stroke )
	{
	int		widthTwips= EMUtoTWIPS( sd->sdLineWidthEmu );
	char		dashArray[100];

	xmlWriteRgb8Attribute( xw, "stroke", &rgb8Stroke );
	xmlWriteIntAttribute( xw, "stroke-width", widthTwips );

	dashArray[0]= '\0';

	switch( sd->sdLineDashing )
	    {
	    case DSdashSOLID:
		break;

	    case DSdashDASHED:
	    case DSdashDASHED_X:
	    case DSdashDASHED_L:
		sprintf( dashArray, "%d %d", 6* widthTwips, 4* widthTwips );
		break;

	    case DSdashDOT:
	    case DSdashDOT_X:
		sprintf( dashArray, "%d %d", 1* widthTwips, 2* widthTwips );
		break;

	    case DSdashDASHDOT:
	    case DSdashDASHDOT_X:
	    case DSdashDASHDOT_L:
		sprintf( dashArray, "%d %d %d %d",
					6* widthTwips, 2* widthTwips,
					1* widthTwips, 2* widthTwips );
		break;

	    case DSdashDASHDOTDOT:
	    case DSdashDASHDOTDOT_L:
		sprintf( dashArray, "%d %d %d %d %d %d",
						6* widthTwips,
						2* widthTwips,
						1* widthTwips,
						2* widthTwips,
						1* widthTwips,
						2* widthTwips );
		break;

	    default:
		LDEB(sd->sdLineDashing);
	    }

	if  ( dashArray[0] )
	    { xmlWriteStringAttribute( xw, "dash-array", dashArray ); }
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Skip to the next page.						*/
/*									*/
/************************************************************************/

static int docSvgFinishPage(	void *				vsw,
				DrawingContext *		dc,
				BufferItem *			bodySectNode,
				int				page,
				int				asLast )
    {
    SvgWriter *		sw= (SvgWriter *)vsw;
    XmlWriter *		xw= &(sw->swXmlWriter);

#   if 0
    if  ( sw->swMultiPage )
	{
	xmlPutString( "</page>", xw );
	xmlNewLine( xw );
	}
#   else
    if  ( sw->swMultiPage )
	{
	xmlPutString( "</g>", xw );
	xmlNewLine( xw );
	}
#   endif

    return 0;
    }

static int docSvgDrawStartPage(	void *				vsw,
				const DocumentGeometry *	dgPage,
				DrawingContext *		dc,
				int				page )
    {
    SvgWriter *		sw= (SvgWriter *)vsw;
    XmlWriter *		xw= &(sw->swXmlWriter);

    char		scratch[120];

    sprintf( scratch, "page%d", page+ 1 );

#   if 0
    if  ( sw->swMultiPage )
	{
	xmlPutString( "<page", xw );
	xmlWriteStringAttribute( xw, "id", scratch );
	xmlPutString( ">", xw );
	xmlNewLine( xw );
	}
#   else
    if  ( sw->swMultiPage )
	{
	xmlPutString( "<g", xw );
	xmlWriteStringAttribute( xw, "id", scratch );
	if  ( page != dc->dcFirstPage )
	    { xmlWriteStringAttribute( xw, "visibility", "hidden" );	}
	xmlPutString( ">", xw );
	xmlNewLine( xw );

	xmlPutString( "<path", xw );
	xmlWriteStringAttribute( xw, "d",
				    "M75,225 l225,-150 l0,300 l-225,-150z" );
	if  ( page > dc->dcFirstPage )
	    {
	    sprintf( scratch, "switchPage( evt, \"page%d\", \"page%d\" );",
						    page+ 1, page );

	    xmlWriteStringAttribute( xw, "fill", "#000000" );
	    xmlWriteStringAttribute( xw, "onclick", scratch );
	    }
	else{
	    xmlWriteStringAttribute( xw, "fill", "#cccccc" );
	    }
	xmlPutString( "/>", xw );
	xmlNewLine( xw );

	xmlPutString( "<path", xw );
	xmlWriteStringAttribute( xw, "d",
				    "M625,225 l-225,-150 l0,300 l225,-150z" );
	if  ( page < dc->dcLastPage )
	    {
	    sprintf( scratch, "switchPage( evt, \"page%d\", \"page%d\" );",
						    page+ 1, page+ 2 );

	    xmlWriteStringAttribute( xw, "fill", "#000000" );
	    xmlWriteStringAttribute( xw, "onclick", scratch );
	    }
	else{
	    xmlWriteStringAttribute( xw, "fill", "#cccccc" );
	    }
	xmlPutString( "/>", xw );
	xmlNewLine( xw );
	}
#   endif

    return 0;
    }

/************************************************************************/
/*									*/
/*  Save a range of pages in a document.				*/
/*									*/
/************************************************************************/

static int docSvgDrawPageRange(	SvgWriter *		sw,
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

    docSvgDrawStartPage( (void *)sw,
	    &(bodyBi->biChildren[i]->biSectDocumentGeometry), dc, firstPage );

    if  ( ! dc->dcPostponeHeadersFooters )
	{
	docDrawPageHeader( bodyBi->biChildren[i], (void *)sw, dc, firstPage );
	}

    if  ( docDrawShapesForPage( (void *)sw, dc, 1, firstPage ) )
	{ LDEB(firstPage);	}

    docDrawNode( &lpBelow, bodyBi, (void *)sw, dc );

    if  ( lastPage < 0 )
	{ lastPage= bodyBi->biBelowPosition.lpPage;	}

    for ( i= bodyBi->biChildCount- 1; i >= 0; i-- )
	{
	if  ( bodyBi->biChildren[i]->biTopPosition.lpPage <= lastPage )
	    { break;	}
	}

    if  ( i < 0 )
	{ LDEB(i); return -1;	}

    if  ( docDrawShapesForPage( (void *)sw, dc, 0, lastPage ) )
	{ LDEB(lastPage);	}

    if  ( ! dc->dcPostponeHeadersFooters )
	{
	docDrawPageFooter( bodyBi->biChildren[i], (void *)sw, dc, lastPage );
	}

    docSvgFinishPage( (void *)sw, dc, bodyBi->biChildren[i],
						    lastPage, asLast );

    return 0;
    }

/************************************************************************/

static void docSvgSetDrawingContext(	DrawingContext *	dc,
					const LayoutContext *	lc,
					int			firstPage,
					int			lastPage )
    {
    INIT_LAYOUT_EXTERNAL	initLayoutExternal= (INIT_LAYOUT_EXTERNAL)0;

    dc->dcSetColorRgb= docSvgSetColorRgb;
    dc->dcSetFont= docSvgSetFont;
    dc->dcDrawShape= docSvgDrawDrawDrawingShape;
    dc->dcDrawObject= docSvgDrawObject;
    dc->dcDrawTab= docSvgDrawTab;
    dc->dcDrawFtnsep= docSvgDrawFtnsep;
    dc->dcDrawSpan= docSvgDrawSpan;

    dc->dcDrawTextLine= docSvgDrawTextLine;
    dc->dcDrawOrnaments= docSvgDrawOrnaments;
    dc->dcFinishPage= docSvgFinishPage;
    dc->dcStartPage= docSvgDrawStartPage;
    dc->dcInitLayoutExternal= initLayoutExternal;

    dc->dcLayoutContext= *lc;

    dc->dcFirstPage= firstPage;
    dc->dcLastPage= lastPage;
    dc->dcDrawExternalItems= 1;
    dc->dcPostponeHeadersFooters= 0;

    return;
    }


/************************************************************************/
/*									*/
/*  Save a document as a SVG drawing.					*/
/*									*/
/************************************************************************/

int docSvgDrawDocument(	SimpleOutputStream *		sos,
			const char *			applicationName,
			const char *			applicationReference,
			const LayoutContext *		lc )
    {
    BufferDocument *		bd= lc->lcDocument;
    DocumentProperties *	dp= &(bd->bdProperties);
    DocumentGeometry *		dg= &(dp->dpGeometry);
    BufferItem *		bodyBi= bd->bdBody.dtRoot;

    DrawingContext		dc;
    SvgWriter			sw;
    XmlWriter *			xw= &(sw.swXmlWriter);

    docInitDrawingContext( &dc );
    docSvgSetDrawingContext( &dc, lc, 0, bodyBi->biBelowPosition.lpPage );

    svgInitSvgWriter( &sw );
    sw.swXmlWriter.xwSos= sos;
    strcpy( sw.swUnit, "pt" );
    sw.swWide= ( dg->dgPageWideTwips+ 19 )/ 20;
    sw.swHigh= ( dg->dgPageHighTwips+ 19 )/ 20;
    sw.swViewBox.drX0= 0;
    sw.swViewBox.drY0= 0;
    sw.swViewBox.drX1= dg->dgPageWideTwips- 1;
    sw.swViewBox.drY1= dg->dgPageHighTwips- 1;
    sw.swMultiPage= dc.dcLastPage > dc.dcFirstPage;

    docInquireHeadersFooters( &(dc.dcDocHasPageHeaders),
				    &(dc.dcDocHasPageFooters), bd );

    svgStartDocument( &sw );

    if  ( sw.swMultiPage )
	{
#	if 0
	xmlPutString( "<pageSet>", xw );
#	else
	xmlPutString( "<script type=\"text/ecmascript\"><![CDATA[", xw );
	xmlNewLine( xw );
	xmlPutString( "function switchPage( evt, fr, to ) {", xw );
	xmlNewLine( xw );
	xmlPutString( "    var efr= document.getElementById( fr );", xw );
	xmlNewLine( xw );
	xmlPutString( "    var eto= document.getElementById( to );", xw );
	xmlNewLine( xw );
	xmlPutString( "    eto.removeAttribute( \"visibility\" );", xw );
	xmlNewLine( xw );
	xmlPutString( "    efr.setAttribute( \"visibility\", \"hidden\" );", xw );
	xmlNewLine( xw );
	xmlPutString( "}", xw );
	xmlNewLine( xw );
	xmlPutString( "]]></script>", xw );
#	endif
	xmlNewLine( xw );
	}

    if  ( docSvgDrawPageRange( &sw, &dc, bodyBi,
			    dc.dcFirstPage, dc.dcLastPage, /* asLast */ 1 ) )
	{ LDEB(dc.dcFirstPage); return -1;	}

#   if 0
    if  ( sw.swMultiPage )
	{
	xmlPutString( "</pageSet>", xw );
	xmlNewLine( xw );
	}
#   endif

    svgFinishDocument( &sw );

    docResetExternalTreeLayout( bd );

    return 0;
    }

/************************************************************************/

int docSvgSaveShapeObject(	SimpleOutputStream *	sos,
				int			page,
				int			pixelsWide,
				int			pixelsHigh,
				const InsertedObject *	io,
				BufferItem *		bodySectNode,
				const LayoutContext *	lc )
    {
    int				rval= 0;
    const DrawingShape *	ds= io->ioDrawingShape;

    DrawingContext		dc;
    SvgWriter			sw;
    IndexSet			attrUsed;
    XmlWriter *			xw= &(sw.swXmlWriter);

    utilInitIndexSet( &attrUsed );

    svgInitSvgWriter( &sw );

    docInitDrawingContext( &dc );
    docSvgSetDrawingContext( &dc, lc, page, page );

    sw.swXmlWriter.xwSos= sos;

    if  ( docGetAttributesUsedInShape( lc->lcDocument, ds, &attrUsed ) )
	{ LDEB(1); rval= -1; goto ready;	}

    /* This is an independent image: Subtract the drawing origin */
    docPlaceRootShapeRect( &(sw.swViewBox),
			    &(io->ioDrawingShape->dsShapeProperties),
			    io->ioX0Twips, io->ioY0Position.lpPageYTwips );
    sw.swWide= pixelsWide;
    sw.swHigh= pixelsHigh;

    svgStartDocument( &sw );

    xmlPutString( "<defs>", xw ); xmlNewLine( xw );
    xmlPutString( "    <style type=\"text/css\"><![CDATA[", xw );
    xmlNewLine( xw );

    docCssSaveTextAttributeStyles( xw->xwSos, &attrUsed, lc->lcDocument );

    xmlPutString( "div { margin:-2px 0; }", xw ); xmlNewLine( xw );

    xmlPutString( "    ]]></style>", xw ); xmlNewLine( xw );
    xmlPutString( "</defs>", xw ); xmlNewLine( xw );

    if  ( docDrawShape( &dc, &sw, bodySectNode, io ) )
	{ LDEB(1); rval= -1; goto ready;;	}

    svgFinishDocument( &sw );

  ready:

    utilCleanIndexSet( &attrUsed );

    return rval;
    }

