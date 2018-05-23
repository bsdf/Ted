/************************************************************************/
/*									*/
/*  Rulers, Ted specific functionality.					*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	<docPageGrid.h>
#   include	<docParaRulerAdmin.h>
#   include	<geoGrid.h>
#   include	<guiDrawingWidget.h>

#   include	"tedApp.h"
#   include	"tedSelect.h"
#   include	"tedAppResources.h"
#   include	"tedDocFront.h"
#   include	"tedRuler.h"
#   include	"tedLayout.h"
#   include	"tedDocument.h"
#   include	"tedToolFront.h"
#   include	<docTreeNode.h>
#   include	<docNodeTree.h>
#   include	<docTextLine.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Obtain the separator positions for a table.				*/
/*									*/
/************************************************************************/

static int tedGetColumns(	BufferItem *			paraNode,
				const LayoutContext *		lc,
				const BlockFrame *		bf,
				ColumnSeparator **		pCs,
				int *				pCsCount )
    {
    double			xfac= lc->lcPixelsPerTwip;
    static ColumnSeparator *	cs;
    ColumnSeparator *		fresh;

    BufferItem *		rowNode;
    CellProperties *		cp;

    int				i;

    int				rg2p;

    rowNode= docGetRowNode( paraNode );
    if  ( ! rowNode )
	{ XDEB(rowNode); return -1;	}

    fresh= (ColumnSeparator *)realloc( cs, ( rowNode->biRowCellCount+ 1 )*
					sizeof( ColumnSeparator ) );
    if  ( ! fresh )
	{ LXDEB(rowNode->biRowCellCount,fresh); return -1;	}
    cs= fresh;

    rg2p= COORDtoGRID( xfac, rowNode->biRowHalfGapWidthTwips );

    cp= rowNode->biRowCells;
    for ( i= 0; i < rowNode->biRowCellCount; fresh++, cp++, i++ )
	{
	const BufferItem *	cellBi= rowNode->biChildren[i];

	ParagraphFrame		pf;

	docCellFrameTwips( &pf, bf, cellBi );

	if  ( i == 0 )
	    {
	    fresh[0].csX0= COORDtoGRID( xfac, pf.pfCellRect.drX0 )- rg2p;
	    fresh[0].csX1= COORDtoGRID( xfac, pf.pfCellRect.drX0 )+ rg2p;
	    }

	fresh[1].csX0= COORDtoGRID( xfac, pf.pfCellRect.drX1 )- rg2p;
	fresh[1].csX1= COORDtoGRID( xfac, pf.pfCellRect.drX1 )+ rg2p;
	}

    *pCs= cs; *pCsCount= rowNode->biRowCellCount+ 1; return 0;
    }

static int tedUpdateTableColumns(	EditDocument *		ed,
					BufferItem *		paraNode,
					const BlockFrame *	bf,
					const LayoutContext *	lc,
					int			csCountNew,
					const ColumnSeparator *	csNew )
    {
    int			rval= 0;

    double		xfac= lc->lcPixelsPerTwip;

    int			csCount;
    ColumnSeparator *	cs;

    int			changed= 0;

    BufferItem *	parentNode;
    BufferItem *	rowNode;

    RowProperties	rpSet;

    int			row;
    int			row0;
    int			row1;

    const DocumentAttributeMap * const dam0= (const DocumentAttributeMap *)0;

    docInitRowProperties( &rpSet );

    if  ( docDelimitTable( paraNode, &parentNode, (int *)0,
							&row0, &row, &row1 ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( tedGetColumns( paraNode, lc, bf, &cs, &csCount ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( csCountNew != csCount )
	{ LLDEB(csCountNew,csCount); rval= -1; goto ready;	}

    rowNode= parentNode->biChildren[row0];
    if  ( docCopyRowProperties( &rpSet, &(rowNode->biRowProperties), dam0 ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( csNew[0].csX0 != cs[0].csX0 )
	{
	int	rowLeftIndentPixels;

	rowLeftIndentPixels= COORDtoGRID( xfac, rowNode->biRowLeftIndentTwips );

	rowLeftIndentPixels += ( csNew[0].csX0- cs[0].csX0 );
	rpSet.rpLeftIndentTwips= GRIDtoCOORD( rowLeftIndentPixels, xfac );

	changed= 1;
	}
    else{
	CellProperties *	cp= rpSet.rpCells;
	int			csi;

	for ( csi= 1; csi < csCount; cp++, csi++ )
	    {
	    if  ( csNew[csi].csX0 != cs[csi].csX0 )
		{
		int	rightBoundaryPixels=
				COORDtoGRID( xfac, cp->cpRightBoundaryTwips );

		rightBoundaryPixels += ( csNew[csi].csX0- cs[csi].csX0 );
		cp->cpRightBoundaryTwips=
				GRIDtoCOORD( rightBoundaryPixels, xfac );
		changed= 1;
		break;
		}
	    }
	}

    if  ( changed )
	{
	const int		wholeRow= 1;
	const int		wholeColumn= 1;

	PropertyMask		rpDifMask;
	PropertyMask		rpCmpMask;

	TedDocument *		td= (TedDocument *)ed->edPrivateData;

	utilPropMaskClear( &rpDifMask );
	utilPropMaskClear( &rpCmpMask );

	utilPropMaskFill( &rpCmpMask, RPprop_COUNT );

	docRowPropertyDifference( &rpDifMask, &rpSet, &rpCmpMask,
					    &(rowNode->biRowProperties), dam0 );

	tedDocSetTableProperties( ed, wholeRow, wholeColumn,
			(const PropertyMask *)0, (const CellProperties *)0,
			&rpDifMask, &rpSet, td->tdTraced );
	}

  ready:

    docCleanRowProperties( &rpSet );

    return rval;
    }

/************************************************************************/
/*									*/
/*  The user wants something with the mouse on the horizontal ruler	*/
/*									*/
/************************************************************************/

static int tedGetRulerTwips(
			const LayoutContext *		lc,
			const BlockFrame *		bf,
			const ParagraphFrame *		pf,
			BufferDocument *		bd,
			ParagraphProperties *		ppNew,
			int				firstIndentPixels,
			int				leftIndentPixels,
			int				rightIndentPixels,
			const TabStopList *		tslNew )
    {
    int				rval= 0;
    double			xfac= lc->lcPixelsPerTwip;

    int				pfX0Pixels;
    int				pfX1Pixels;

    TabStopList			tsl;
    TabStopList			tslOld;

    docInitTabStopList( &tslOld );

    pfX0Pixels= COORDtoGRID( xfac, pf->pfCellContentRect.drX0 );
    pfX1Pixels= COORDtoGRID( xfac, pf->pfCellContentRect.drX1 );

    leftIndentPixels -= pfX0Pixels;
    ppNew->ppLeftIndentTwips= GRIDtoCOORD( leftIndentPixels, xfac );

    firstIndentPixels -= pfX0Pixels;
    firstIndentPixels -= leftIndentPixels;
    ppNew->ppFirstIndentTwips= GRIDtoCOORD( firstIndentPixels, xfac );

    rightIndentPixels= pfX1Pixels- rightIndentPixels;
    ppNew->ppRightIndentTwips= GRIDtoCOORD( rightIndentPixels, xfac );

    docGetTabStopListByNumber( &tsl, bd, ppNew->ppTabStopListNumber );

    if  ( docCopyTabStopList( &tslOld, &tsl ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( docRulerMergeTabs( &tslOld, pfX0Pixels, xfac, tslNew ) )
	{ LDEB(1); rval= -1; goto ready;	}

    ppNew->ppTabStopListNumber= docTabStopListNumber( bd, &tslOld );

  ready:

    docCleanTabStopList( &tslOld );

    return rval;
    }

static APP_EVENT_HANDLER_H( tedTopRulerButtonDown, w, voided, downEvent )
    {
    EditDocument *		ed= (EditDocument *)voided;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;
    BufferItem *		paraNode;

    int				leftIndentNew;
    int				firstIndentNew;
    int				rightIndentNew;
    TabStopList			tslNew;

    int				csCountNew= 0;
    ColumnSeparator *		csNew= (ColumnSeparator *)0;

    int				prop= PPprop_NONE;

    PropertyMask		ppSetMask;

    ParagraphProperties		ppNew;

    ParagraphFrame		pf;
    BlockFrame			bf;

    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;
    const TextLine *		tl;

    DocumentRectangle *		drParaContent= &(pf.pfParaContentRect);

    LayoutContext		lc;
    double			xfac;

    int				button;
    int				upDown;
    int				seq;
    unsigned int		keyState;
    int				mouseX;
    int				mouseY;


    docInitParagraphProperties( &ppNew );
    docInitTabStopList( &tslNew );

    if  ( ed->edFileReadOnly )
	{ goto ready;	}

    if  ( guiGetCoordinatesFromMouseButtonEvent(
			&mouseX, &mouseY, &button, &upDown, &seq, &keyState,
			w, downEvent ) )
	{ goto ready;	}

    if  ( upDown < 1 || seq > 1 )
	{ goto ready;	}

    switch( button )
	{
	case 1:
	    break;

	case 3:
	    prop= tedTopRulerFindMouse( &seq, w, mouseX, mouseY, ed );
	    switch( prop )
		{
		case PPpropTAB_STOPS:
		case PPprop_TAB_KIND_BUTTON:
		    tedShowFormatTool( td->tdToolsFormatToolOption,
							ed->edApplication );
		    tedAdaptFormatToolToDocument( ed, 0 );
		    tedFormatShowParaTabsPage( ed->edApplication );
		    goto ready;

		case PPpropLEFT_INDENT:
		case PPpropFIRST_INDENT:
		case PPpropRIGHT_INDENT:
		    tedShowFormatTool( td->tdToolsFormatToolOption,
							ed->edApplication );
		    tedAdaptFormatToolToDocument( ed, 0 );
		    tedFormatShowParaLayoutPage( ed->edApplication );
		    goto ready;

		case PPprop_COLUMNS:
		    tedShowFormatTool( td->tdToolsFormatToolOption,
							ed->edApplication );
		    if  ( seq == 0 )
			{
			if  ( tedDocSelectColumn( ed, seq ) )
			    { LDEB(seq); goto ready;	}

			tedFormatShowTablePage( ed->edApplication );
			}
		    else{
			if  ( tedDocSelectColumn( ed, seq- 1 ) )
			    { LDEB(seq); goto ready;	}

			tedFormatShowColumnPage( ed->edApplication );
			}
		    goto ready;

		default:
		    /* LDEB(prop); */ goto ready;
		}
	    break;

	default:
	    goto ready;
	}

    tedSetScreenLayoutContext( &lc, ed );
    xfac= lc.lcPixelsPerTwip;

    if  ( tedGetSelection( &ds, &sg, &sd,
				(DocumentTree **)0, (BufferItem **)0, ed ) )
	{ LDEB(1); goto ready;	}
    paraNode= ds.dsHead.dpNode;
    tl= paraNode->biParaLines+ sg.sgHead.pgLine;

    utilPropMaskClear( &ppSetMask );

    docBlockFrameTwips( &bf, paraNode, bd, tl->tlTopPosition.lpPage,
					    tl->tlTopPosition.lpColumn );

    docParagraphFrameTwips( &pf, &bf, paraNode );

    leftIndentNew= COORDtoGRID( xfac, drParaContent->drX0 );
    firstIndentNew= COORDtoGRID( xfac, drParaContent->drX0+
					    paraNode->biParaFirstIndentTwips );
    rightIndentNew= COORDtoGRID( xfac, drParaContent->drX1 );

    prop= tedTopRulerTrackMouse(
			    &firstIndentNew, &leftIndentNew, &rightIndentNew,
			    &tslNew, &csCountNew, &csNew, w, downEvent, ed );

    switch( prop )
	{
	case PPprop_NONE:
	case PPprop_TAB_KIND_BUTTON:
	    goto ready;

	case PPpropLEFT_INDENT:
	case PPpropFIRST_INDENT:
	    PROPmaskADD( &ppSetMask, PPpropLEFT_INDENT );
	    PROPmaskADD( &ppSetMask, PPpropFIRST_INDENT );
	    break;

	case PPpropRIGHT_INDENT:
	case PPpropTAB_STOPS:
	    PROPmaskADD( &ppSetMask, prop );
	    break;

	case PPprop_COLUMNS:
	    tedUpdateTableColumns( ed, paraNode, &bf, &lc, csCountNew, csNew );
	    goto ready;
	default:
	    LDEB(prop); goto ready;
	}

    if  ( docUpdParaProperties( (PropertyMask *)0, &ppNew, &ppSetMask,
					    &(paraNode->biParaProperties),
					    (const DocumentAttributeMap *)0 ) )
	{ LDEB(1); goto ready;	}

    if  ( tedGetRulerTwips( &lc, &bf, &pf, bd, &ppNew,
			    firstIndentNew, leftIndentNew, rightIndentNew,
			    &tslNew ) )
	{ LDEB(1); goto ready;	}

    if  ( tedDocChangeParagraphProperties( ed, &ppSetMask, &ppNew,
							    td->tdTraced ) )
	{ LDEB(1); goto ready;	}

  ready:

    docCleanParagraphProperties( &ppNew );
    docInitTabStopList( &tslNew );

    return;
    }

/************************************************************************/
/*									*/
/*  Adapt the horizontal ruler to the current paragraph.		*/
/*									*/
/************************************************************************/

void tedDocAdaptTopRuler(	EditDocument *			ed,
				const DocumentSelection *	ds,
				const SelectionGeometry *	sg,
				const SelectionDescription *	sd,
				const BufferItem *		bodySectNode )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;

    int				leftIndent;
    int				firstIndent;
    int				rightIndent;

    BlockFrame			bf;
    ParagraphFrame		pf;

    int				bfX0Pixels;
    int				bfX1Pixels;
    int				pfX0Pixels;
    int				pfX1Pixels;

    DocumentRectangle		drOutside;
    DocumentRectangle		drInside;

    BufferItem *		paraNode= ds->dsHead.dpNode;
    const TextLine *		tl;

    TabStopList			tsl;

    LayoutContext		lc;
    double			xfac;

    docLayoutInitBlockFrame( &bf );

    tedSetScreenLayoutContext( &lc, ed );
    xfac= lc.lcPixelsPerTwip;

    if  ( ! ed->edTopRuler )
	{ goto ready;	}
    if  ( sg->sgHead.pgLine >= paraNode->biParaLineCount )
	{ LLDEB(sg->sgHead.pgLine,paraNode->biParaLineCount); return;	}

    docGetTabStopListByNumber( &tsl, bd, paraNode->biParaTabStopListNumber );

    tl= paraNode->biParaLines+ sg->sgHead.pgLine;
    docBlockFrameTwips( &bf, paraNode, bd,
					tl->tlTopPosition.lpPage,
					tl->tlTopPosition.lpColumn );

    docParagraphFrameTwips( &pf, &bf, paraNode );

    bfX0Pixels= COORDtoGRID( xfac, bf.bfContentRect.drX0 );
    bfX1Pixels= COORDtoGRID( xfac, bf.bfContentRect.drX1 );

    pfX0Pixels= COORDtoGRID( xfac, pf.pfCellContentRect.drX0 );
    pfX1Pixels= COORDtoGRID( xfac, pf.pfCellContentRect.drX1 );

    leftIndent= COORDtoGRID( xfac, pf.pfParaContentRect.drX0 );
    firstIndent= COORDtoGRID( xfac, pf.pfParaContentRect.drX0+
					paraNode->biParaFirstIndentTwips );
    rightIndent= COORDtoGRID( xfac, pf.pfParaContentRect.drX1 );

    docPageRectsPixels( &drOutside, &drInside, xfac, bodySectNode, bd );

    tedAdaptTopRuler( ed->edTopRuler, ed->edTopRulerWidget,
		drOutside.drX0, drOutside.drX1,		/*  doc */
		drInside.drX0, drInside.drX1,		/*  marg */
		firstIndent, leftIndent, rightIndent,
		&tsl, pfX0Pixels, xfac );

    if  ( paraNode->biParaTableNesting > 0 )
	{
	int			csCount;
	ColumnSeparator *	cs;

	BufferItem *		parentBi;

	int			col;
	int			row;
	int			row0;
	int			row1;

	if  ( docDelimitTable( paraNode, &parentBi, &col, &row0, &row, &row1 ) )
	    { LDEB(1); goto ready;	}

	if  ( tedGetColumns( paraNode, &lc, &bf, &cs, &csCount ) )
	    { LDEB(1); goto ready;	}

	tedSetRulerColumns( ed->edTopRulerWidget, ed->edTopRuler,
				    bfX0Pixels,
				    bfX1Pixels,
				    pfX0Pixels,
				    pfX1Pixels,
				    cs, csCount );
	}
    else{
	tedSetRulerColumns( ed->edTopRulerWidget, ed->edTopRuler,
				    bfX0Pixels,
				    bfX1Pixels,
				    pfX0Pixels,
				    pfX1Pixels,
				    (ColumnSeparator *)0, 0 );
	}

  ready:
    docLayoutCleanBlockFrame( &bf );

    return;
    }

/************************************************************************/
/*									*/
/*  Make the ruler administration for the top ruler widget of a		*/
/*  document.								*/
/*									*/
/************************************************************************/

int tedDocSetTopRuler(	EditDocument *	ed )
    {
    EditApplication *		ea= ed->edApplication;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;

    int				topRulerHeight= ed->edTopRulerHighPixels;
    int				unitInt= ea->eaUnitInt;

    DocumentRectangle		drOutside;
    DocumentRectangle		drInside;

    const BufferItem *		bodySectNode= bd->bdBody.dtRoot->biChildren[0];

    docPageRectsPixels( &drOutside, &drInside,
					td->tdPixelsPerTwip, bodySectNode, bd );

    ed->edTopRuler= tedMakeTopRuler(
			topRulerHeight,		/*  sizeAcross		*/
			ea->eaPixelsPerTwip,
			ea->eaMagnification,	/*  magnification	*/
			&(ea->eaPostScriptFontList),

			ed->edLeftRulerWidePixels,	/*  minUnused	*/
			ed->edRightRulerWidePixels,	/*  maxUnused	*/

			drOutside.drX0, drOutside.drX1,
			drInside.drX0, drInside.drX1,
			ed->edVisibleRect.drX0, ed->edVisibleRect.drX1,

			unitInt );		/*  whatUnit		*/

    guiDrawSetConfigureHandler( ed->edTopRulerWidget,
			    tedTopRulerConfigure, (void *)ed->edTopRuler );

    guiDrawSetRedrawHandler( ed->edTopRulerWidget,
				tedRedrawTopRuler, (void *)ed->edTopRuler );

    guiDrawSetButtonPressHandler( ed->edTopRulerWidget,
					tedTopRulerButtonDown, (void *)ed );

#   ifdef USE_GTK
    gtk_widget_add_events( ed->edTopRulerWidget, GDK_POINTER_MOTION_MASK );
#   endif

    return 0;
    }

int tedDocSetBottomRuler(	EditDocument *	ed )
    {
    EditApplication *		ea= ed->edApplication;
    TedAppResources *		tar= (TedAppResources *)ea->eaResourceData;

    int				bottomRulerHigh= ed->edBottomRulerHighPixels;

    ed->edBottomRuler= tedMakeBottomRuler(
			&(ea->eaPostScriptFontList),
			bottomRulerHigh,		/*  sizeAcross	*/
			ed->edLeftRulerWidePixels,	/*  minUnused	*/
			ed->edRightRulerWidePixels,	/*  maxUnused	*/
			tar->tarPageNumberFormat );

    guiDrawSetConfigureHandler( ed->edBottomRulerWidget,
			tedBottomRulerConfigure, (void *)ed->edBottomRuler );

    guiDrawSetRedrawHandler( ed->edBottomRulerWidget,
			    tedRedrawBottomRuler, (void *)ed->edBottomRuler );

    return 0;
    }
