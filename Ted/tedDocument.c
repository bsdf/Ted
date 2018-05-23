#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	<textOfficeCharset.h>
#   include	<appQuestion.h>

#   include	"tedApp.h"
#   include	"tedSelect.h"
#   include	"tedRuler.h"
#   include	"tedAppResources.h"
#   include	"tedLayout.h"
#   include	"tedDocument.h"
#   include	"tedDocFront.h"

#   include	<docDebug.h>
#   include	<docScreenLayout.h>
#   include	<docFind.h>
#   include	<docField.h>
#   include	<docParaParticules.h>
#   include	<docRecalculateTocField.h>
#   include	<docNodeTree.h>
#   include	<docEditCommand.h>

#   include	<guiDrawingWidget.h>
#   include	<psNup.h>

#   include	<appDebugon.h>

#   define VALIDATE_TREE	0

/************************************************************************/
/*									*/
/*  Extension of the trace file.					*/
/*									*/
/************************************************************************/

const char TedTraceExtension[]= "Ted";

/************************************************************************/
/*									*/
/*  Scrolling callbacks.						*/
/*									*/
/************************************************************************/

void tedMoveObjectWindows(	EditDocument *		ed )
    {
    InsertedObject *		io;

    DocumentPosition		dpObj;
    PositionGeometry		pg;
    int				partObj;

    LayoutContext		lc;

    layoutInitContext( &lc );
    tedSetScreenLayoutContext( &lc, ed );

    docInitDocumentPosition( &dpObj );

    if  ( tedGetObjectSelection( ed, &partObj, &dpObj, &io ) )
	{ LDEB(1); return;	}

    tedPositionGeometry( &pg, &dpObj, PARAfindLAST, &lc );

    tedSetObjectWindows( ed, &pg, io, &lc );
    }

/************************************************************************/
/*									*/
/*  Make a document readOnly.						*/
/*									*/
/************************************************************************/

void tedMakeDocumentReadonly(	EditDocument *	ed,
				int		readonly )
    {
    TedDocument *	td= (TedDocument *)ed->edPrivateData;
    const int		visible= ! readonly;

    guiShowMenuOption( td->tdFileSaveOption, visible );

    guiShowMenuOption( td->tdCutOption, visible );
    guiShowMenuOption( td->tdPasteOption, visible );
    guiShowMenuOption( td->tdUndoOption, visible );
    guiShowMenuOption( td->tdRepeatOption, visible );

    guiShowMenuOption( td->tdInsPictOption, visible );
    guiShowMenuOption( td->tdInsSymbolOption, visible );
    guiShowMenuOption( td->tdInsFileOption, visible );
    guiShowMenuOption( td->tdInsInsertFootnoteOption, visible );
    guiShowMenuOption( td->tdInsInsertEndnoteOption, visible );
    guiShowMenuOption( td->tdInsInsertChftnsepOption, visible );
    guiShowMenuOption( td->tdInsInsertTableOption, visible );
    guiShowMenuOption( td->tdInsInsertPageNumberOption, visible );

    guiShowMenuOption( td->tdTabInsertTableOption, visible );
    guiShowMenuOption( td->tdTabAddRowOption, visible );
    guiShowMenuOption( td->tdTabAddColumnOption, visible );
    guiShowMenuOption( td->tdTabInsertSeparator, visible );

    guiShowMenuOption( td->tdTabDeleteTableOption, visible );
    guiShowMenuOption( td->tdTabDeleteRowOption, visible );
    guiShowMenuOption( td->tdTabDeleteColumnOption, visible );
    guiShowMenuOption( td->tdTabDeleteSeparator, visible );

    guiShowMenuOption( td->tdInsInsertLineBreakOption, visible );
    guiShowMenuOption( td->tdInsInsertPageBreakOption, visible );
    guiShowMenuOption( td->tdInsInsertColumnBreakOption, visible );
    guiShowMenuOption( td->tdInsInsertSectBreakOption, visible );

    guiShowMenuOption( td->tdFontBoldOption, visible );
    guiShowMenuOption( td->tdFontItalicOption, visible );
    guiShowMenuOption( td->tdFontUnderlinedOption, visible );
    guiShowMenuOption( td->tdFontSuperscriptOption, visible );
    guiShowMenuOption( td->tdFontSubscriptOption, visible );
    guiShowMenuOption( td->tdFontToggleSeparator, visible );
    guiShowMenuOption( td->tdFontPasteOption, visible );

    guiShowMenuOption( td->tdFormatOneParaOption, visible );
    guiShowMenuOption( td->tdFormatOneParaSeparator, visible );

    guiShowMenuOption( td->tdFormatPasteRulerOption, visible );
    guiShowMenuOption( td->tdFormatIncreaseIndentOption, visible );
    guiShowMenuOption( td->tdFormatDecreaseIndentOption, visible );
    guiShowMenuOption( td->tdFormatRulerSeparator, visible );

    guiShowMenuOption( td->tdFormatAlignLeftOption, visible );
    guiShowMenuOption( td->tdFormatAlignRightOption, visible );
    guiShowMenuOption( td->tdFormatAlignCenterOption, visible );
    guiShowMenuOption( td->tdFormatAlignJustifyOption, visible );
    guiShowMenuOption( td->tdFormatAlignSeparator, visible );

    guiShowMenuOption( td->tdToolsFontOption, visible );
    guiShowMenuOption( td->tdToolsSpellingOption, visible );
    guiShowMenuOption( td->tdToolsPageLayoutOption, visible );
    guiShowMenuOption( td->tdToolsSymbolOption, visible );

    /* Oops wrong call */
    guiShowMenuOption( td->tdInsertMenuButton, visible );
    }

/************************************************************************/
/*									*/
/*  Last phase in building a document window.				*/
/*									*/
/*  1)  Set the position by hand, because the regular routine sends an	*/
/*	expose to a window that does not exist yet.			*/
/*  2)  Forget the name of document templates to force a save-as.	*/
/*	Also forget it is a template, to make sure that it is saved as	*/
/*	an ordinary document.						*/
/*									*/
/************************************************************************/

int tedFinishDocumentSetup(	EditDocument *		ed )
    {
    EditApplication *		ea= ed->edApplication;

    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;
    DocumentProperties *	dp= &(bd->bdProperties);

    const TedAppResources *	tar= (TedAppResources *)ea->eaResourceData;

    LayoutContext		lc;

    int				luma;
    int				chroma;
    int				hue;

    layoutInitContext( &lc );
    tedSetScreenLayoutContext( &lc, ed );

    {
    DocumentPosition		dpFirst;
    const BufferItem *		bodySectNode= (const BufferItem *)0;

    /*  1  */
    if  ( ! docGotoFirstPosition( &dpFirst, bd->bdBody.dtRoot ) )
	{
	const int			lastLine= 0;
	BufferItem *			bodyNode= bd->bdBody.dtRoot;

	if  ( tar->tarFindPattern					&&
	      ! docFindSetPattern( &(td->tdFindProg),
			    tar->tarFindPattern, tar->tarFindRegex )	&&
	      ! docFindFindNextInDocument( &(td->tdSelection),
		    &dpFirst, bd,
		    docFindParaFindNext, td->tdFindProg )	)
	    { bodySectNode= docGetBodySectNode( dpFirst.dpNode, bd );	}
	else{
	    docAvoidParaHeadField( &dpFirst, (int *)0, bd );

	    docSetIBarSelection( &(td->tdSelection), &dpFirst );
	    bodySectNode= bodyNode->biChildren[0];
	    }

	tedSelectionGeometry( &(td->tdSelectionGeometry),
			     &(td->tdSelection), bodySectNode, lastLine,
			     &lc );

	tedDescribeSelection( ed );
	}
    else{ docListNode( 0, bd->bdBody.dtRoot, 0 );	}

    /*  2  */
    if  ( dp->dpIsDocumentTemplate )
	{
	appSetDocumentFilename( ed, (const MemoryBuffer *)0 );
	dp->dpIsDocumentTemplate= 0;
	}

#   if VALIDATE_TREE
    LDEB(1);
    if  ( docCheckNode( bodyNode ) )
	{ LDEB(2); docListNode( 0, bodyNode ); abort();	}
#   endif

    }

    if  ( appFinishDocumentSetup( ed ) )
	{ LDEB(1); return -1;	}

    /*
    if  ( dw->dwColors.acAllocator.caDepth < 4 )
    */
    if  ( 0 )
	{
	td->tdDrawMonochrome= 1;

	utilRGB8SolidBlack( &(td->tdSelColor) );
	utilRGB8SolidBlack( &(td->tdCopiedSelColor) );
	utilRGB8SolidBlack( &(td->tdTableColor) );
	}
    else{
	td->tdDrawMonochrome= 0;

	td->tdSelColor.rgb8Red= 176;
	td->tdSelColor.rgb8Green= 196;
	td->tdSelColor.rgb8Blue= 222;

	td->tdCopiedSelColor.rgb8Red= 176;
	td->tdCopiedSelColor.rgb8Green= 176;
	td->tdCopiedSelColor.rgb8Blue= 176;

	td->tdTableColor.rgb8Red= 192;
	td->tdTableColor.rgb8Green= 192;
	td->tdTableColor.rgb8Blue= 192;
	}

    if  ( utilRGB8LumaChromaHue( &luma, &chroma, &hue,
						&(ed->edBackgroundColor) ) )
	{ LDEB(1);	}
    else{
	const int	maxLuma= 220;
	const int	minLuma= 64;
	int		changed= 0;

	/*  Must be possible to distinguish background from white */
	if  ( luma > maxLuma )
	    { luma= maxLuma; changed= 1;	}
	if  ( luma < minLuma )
	    { luma= minLuma; changed= 1;	}

	/*? td->tdTableColor= ed->edBackgroundColor; */

	if  ( changed )
	    {
	    if  ( utilRGB8FromLumaChromaHue( &(ed->edBackgroundColor),
							luma, chroma, hue ) )
		{ LLLDEB(luma,chroma,hue);	}

	    /*? td->tdTableColor= ed->edBackgroundColor; */

	    if  ( ed->edTopRuler )
		{
		tedTopRulerSetBackground( ed->edTopRuler,
						&(ed->edBackgroundColor) );
		}
	    if  ( ed->edLeftRuler )
		{
		tedVerticalRulerSetBackground( ed->edLeftRuler,
						&(ed->edBackgroundColor) );
		}
	    if  ( ed->edRightRuler )
		{
		tedVerticalRulerSetBackground( ed->edRightRuler,
						&(ed->edBackgroundColor) );
		}
	    if  ( ed->edBottomRuler )
		{
		tedBottomRulerSetBackground( ed->edBottomRuler,
						&(ed->edBackgroundColor) );
		}
	    }
	}

    if  ( tedOpenTreeObjects( &(bd->bdBody), &lc ) )
	{ LDEB(1);	}

    td->tdDrawTableGrid= tar->tarShowTableGridInt;

    appGuiSetToggleItemState( td->tdDrawTableGridOption,
						td->tdDrawTableGrid >= 0 );

    if  ( ! utilMemoryBufferIsEmpty( &(td->tdRecoveredName) ) )
	{
	const EditTrace *	et= &(td->tdEditTrace);
	const int		direction= 1;
	int			n= et->etBase;
	int			lastEdit= -1;
	const TraceStep *	ts;

	while( n < et->etCount )
	    {
	    int		isRepeat= 0;

	    n= docEditGetTraceStep( &ts, &isRepeat, direction, et, n+ 1 );
	    if  ( n < 0 || n >= et->etCount || isRepeat )
		{ break;	}

	    if  ( ts->tsCommand < EDITcmd_EDIT_COUNT		&&
		  ts->tsCommand != EDITcmdEXTEND_REPLACE	)
		{ lastEdit= n;	}
	    }

	while( et->etIndex < lastEdit )
	    {
	    if  ( tedDocRepeat( ed ) )
		{ LLDEB(et->etIndex,lastEdit); break; }
	    }

	td->tdTraced= 1;

	appSetDocumentFilename( ed, &(td->tdRecoveredName) );
	appSetDocumentTitle( ed, &(td->tdRecoveredName) );

	tedSetTracedChangedFlag( ed );
	guiShowMenuOption( td->tdFileUnlockOption, 0 );
	guiShowMenuOption( td->tdFileRecoverOption, 0 );
	}
    else{
	if  ( td->tdEditTrace.etTraceStatus == TRACING_EXIST )
	    {
	    guiShowMenuOption( td->tdFileUnlockOption, 1 );
	    guiShowMenuOption( td->tdFileRecoverOption,
						td->tdEditTrace.etBase >= 0 );

	    if  ( ed->edFormat == TEDdockindTRACE )
		{ guiShowMenuOption( td->tdFileSaveAsOption, 0 );	}

	    ed->edIsReadonly= 1;
	    }
	else{
	    guiShowMenuOption( td->tdFileUnlockOption, 0 );
	    guiShowMenuOption( td->tdFileRecoverOption, 0 );
	    }
	}

    if  ( ed->edIsReadonly )
	{ tedMakeDocumentReadonly( ed, ed->edIsReadonly );	}

    if  ( tedHasSelection( ed ) )
	{
	tedDescribeSelection( ed );
	tedAdaptToolsToSelection( ed );
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Scroll to the selection the first time the document becomes visible	*/
/*									*/
/************************************************************************/

void tedDocumentFirstVisible(	EditDocument *	ed )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    DocumentRectangle		dr= ed->edVisibleRect;

    if  ( td->tdSelectionGeometry.sgRectangle.drX1 > dr.drX1 )
	{
	int sh= td->tdSelectionGeometry.sgRectangle.drX1- dr.drX1;

	dr.drX0 += sh;
	dr.drX1 += sh;
	}
    if  ( td->tdSelectionGeometry.sgRectangle.drY1 > dr.drY1 )
	{
	int sh= td->tdSelectionGeometry.sgRectangle.drY1- dr.drY1;

	dr.drY0 += sh;
	dr.drY1 += sh;
	}

    appScrollToRectangle( ed, &dr, (int *)0, (int *)0 );

    return;
    }

/************************************************************************/
/*									*/
/*  Make the document widget and the rulers.				*/
/*									*/
/*  1)  Calculate the different width of the margins.			*/
/*  2)  Insert a Form in the scrolled window.				*/
/*  3)  Insert the top ruler in the form.				*/
/*  4)  Insert the bottom ruler in the form.				*/
/*  5)  Insert the left ruler in the form.				*/
/*  6)  Insert the right ruler in the form.				*/
/*  7)  Insert the document widget in the form.				*/
/*									*/
/************************************************************************/

void tedFreeDocument(		void *			voidtd,
				int			format )
    {
    TedDocument *	td= (TedDocument *)voidtd;

    tedCleanCursorBlink( td );

    if  ( td->tdDocument )
	{
	docCleanDocumentObjects( td->tdDocument, docScreenCloseObject );
	docFreeDocument( td->tdDocument );
	}

    utilCleanMemoryBuffer( &(td->tdRecoveredName) );

    utilCleanIndexMapping( &(td->tdAttributeToScreenFont) );
    docCleanEditTrace( &(td->tdEditTrace) );

    utilCleanMemoryBuffer( &(td->tdCopiedSelection) );
    utilCleanMemoryBuffer( &(td->tdCopiedFont) );
    utilCleanMemoryBuffer( &(td->tdCopiedRuler) );

    bmCleanRasterImage( &(td->tdCopiedImage) );

    docFindSetPattern( &(td->tdFindProg), (const char *)0, 0 );

    free( td );
    }

int tedMakeDocumentWidget(	EditApplication *	ea,
				EditDocument *		ed )
    {
    if  ( appMakeDocumentWidget( ea, ed ) )
	{ LDEB(1); return -1;	}

    if  ( ! ea->eaDocumentCursor	&&
	  tedMakeDocumentCursor( ea )	)
	{ LDEB(1);	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Perform the setup of the editing environment for a document.	*/
/*									*/
/*  8)  Determine the code page of the document.			*/
/*  9)  Do not use the 1252 codepage as an explicit setting: It is	*/
/*	almost certainly returned by a default 'Ted' installation, and	*/
/*	is almost impossible to get rid of.				*/
/*									*/
/************************************************************************/

int tedFirstRecalculateFields(	RecalculateFields *	rf,
				DOC_CLOSE_OBJECT	closeObject,
				BufferDocument *	bd )
    {
    const int			page= 0;

    rf->rfDocument= bd;
    rf->rfCloseObject= closeObject;
    rf->rfUpdateFlags= FIELDdoDOC_INFO|FIELDdoDOC_COMPLETE|FIELDdoCHFTN;
    rf->rfFieldsUpdated= 0;

    rf->rfBodySectNode= bd->bdBody.dtRoot->biChildren[0];

    if  ( docRecalculateTocFields( rf ) )
	{ LDEB(1); return -1;	}

    if  ( docRecalculateTextLevelFieldsInDocumentTree( rf, &(bd->bdBody),
				    bd->bdBody.dtRoot->biChildren[0], page ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

int tedLayoutDocument(		DocumentRectangle *		drScreen,
				DocumentRectangle *		drVisible,
				void *				privateData,
				int				format,
				DrawingSurface			ds,
				const PostScriptFontList *	psfl,
				const DocumentGeometry *	dgDef )
    {
    TedDocument *		td= (TedDocument *)privateData;
    BufferDocument *		bd= td->tdDocument;

    const int			page= 0;
    int				noteNumbersChanged= 0;
    int				reachedBottom= 0;

    RecalculateFields		rf;
    LayoutContext		lc;

    DocumentRectangle		drOutside;
    DocumentRectangle		drInside;

    BufferItem *		rootNode= bd->bdBody.dtRoot;

    docInitRecalculateFields( &rf );
    layoutInitContext( &lc );

    docPageRectsPixels( &drOutside, &drInside, td->tdPixelsPerTwip,
						rootNode->biChildren[0], bd );

    *drScreen= drOutside;
    *drVisible= drOutside;

    tedSetDocumentLayoutContext( &lc, ds, psfl, td );

    if  ( tedFirstRecalculateFields( &rf, docScreenCloseObject, bd ) )
	{ LDEB(page); return -1;	}

    if  ( tedLayoutDocumentBody( &reachedBottom, &lc ) )
	{ LDEB(1); return -1;	}

    docRenumberNotes( &noteNumbersChanged, bd );

    rf.rfUpdateFlags= FIELDdoDOC_FORMATTED;
    rf.rfFieldsUpdated= 0;

    if  ( noteNumbersChanged )
	{ rf.rfUpdateFlags |= FIELDdoCHFTN;	}

    if  ( docRecalculateTextLevelFieldsInDocumentTree( &rf, &(bd->bdBody),
					    rootNode->biChildren[0], page ) )
	{ LDEB(1); return -1;	}

    if  ( rf.rfFieldsUpdated > 0				&&
	  tedLayoutDocumentBody( &reachedBottom, &lc )	)
	{ LDEB(1); return -1;	}

    docGetPixelRectangleForPages( drScreen, &lc,
					rootNode->biTopPosition.lpPage,
					rootNode->biBelowPosition.lpPage );

    /* LDEB(1); docListNode(0,rootNode,1); */

    return 0;
    }

/************************************************************************/
/*									*/
/*  Allocate private data for a document.				*/
/*									*/
/************************************************************************/

void * tedMakePrivateData( void )
    {
    int			i;
    TedDocument *	td;

    td= (TedDocument *)malloc( sizeof(TedDocument) );
    if  ( ! td )
	{ XDEB(td); return (void *)0;	}

    docInitSelectionDescription( &(td->tdSelectionDescription) );
    td->tdBodySectionNumber= -1;

    td->tdDocument= (BufferDocument *)0;
    td->tdCurrentScreenFont= -1;

    utilInitMemoryBuffer( &(td->tdRecoveredName) );

    td->tdPageGapTwips= 0;

    td->tdTraced= 0;
    td->tdOverstrike= 0;

    utilInitIndexMapping( &(td->tdAttributeToScreenFont) );
    docInitEditTrace( &(td->tdEditTrace) );

    td->tdFormatMenu= (APP_WIDGET)0;
    td->tdFormatMenuButton= (APP_WIDGET)0;
    td->tdFormatOneParaOption= (APP_WIDGET)0;
    td->tdFormatOneParaSeparator= (APP_WIDGET)0;

    td->tdFormatPasteRulerOption= (APP_WIDGET)0;
    td->tdFormatIncreaseIndentOption= (APP_WIDGET)0;
    td->tdFormatDecreaseIndentOption= (APP_WIDGET)0;
    td->tdFormatRulerSeparator= (APP_WIDGET)0;

    td->tdFormatAlignLeftOption= (APP_WIDGET)0;
    td->tdFormatAlignRightOption= (APP_WIDGET)0;
    td->tdFormatAlignCenterOption= (APP_WIDGET)0;
    td->tdFormatAlignJustifyOption= (APP_WIDGET)0;
    td->tdFormatAlignSeparator= (APP_WIDGET)0;

    td->tdFileOpenOption= (APP_WIDGET)0;
    td->tdFileSaveOption= (APP_WIDGET)0;
    td->tdFileSaveAsOption= (APP_WIDGET)0;
    td->tdFileUnlockOption= (APP_WIDGET)0;
    td->tdFileRecoverOption= (APP_WIDGET)0;
    td->tdFilePropertiesOption= (APP_WIDGET)0;
    td->tdFileCloseOption= (APP_WIDGET)0;
    td->tdFileQuitSeparator= (APP_WIDGET)0;
    td->tdFileQuitOption= (APP_WIDGET)0;

    td->tdCopyOption= (APP_WIDGET)0;
    td->tdUndoOption= (APP_WIDGET)0;
    td->tdRepeatOption= (APP_WIDGET)0;
    td->tdCutOption= (APP_WIDGET)0;
    td->tdPasteOption= (APP_WIDGET)0;

    td->tdInsertMenu= (APP_WIDGET)0;
    td->tdInsertMenuButton= (APP_WIDGET)0;
    td->tdInsPictOption= (APP_WIDGET)0;
    td->tdInsFileOption= (APP_WIDGET)0;
    td->tdInsSymbolOption= (APP_WIDGET)0;
    td->tdInsHyperlinkOption= (APP_WIDGET)0;
    td->tdInsBookmarkOption= (APP_WIDGET)0;
    td->tdInsInsertFootnoteOption= (APP_WIDGET)0;
    td->tdInsInsertEndnoteOption= (APP_WIDGET)0;
    td->tdInsInsertChftnsepOption= (APP_WIDGET)0;
    td->tdInsInsertTableOption= (APP_WIDGET)0;
    td->tdInsInsertPageNumberOption= (APP_WIDGET)0;
    td->tdInsInsertLineBreakOption= (APP_WIDGET)0;
    td->tdInsInsertPageBreakOption= (APP_WIDGET)0;
    td->tdInsInsertColumnBreakOption= (APP_WIDGET)0;
    td->tdInsInsertSectBreakOption= (APP_WIDGET)0;

    td->tdTableMenu= (APP_WIDGET)0;
    td->tdTableMenuButton= (APP_WIDGET)0;

    td->tdTabInsertTableOption= (APP_WIDGET)0;
    td->tdTabAddRowOption= (APP_WIDGET)0;
    td->tdTabAddColumnOption= (APP_WIDGET)0;
    td->tdTabInsertSeparator= (APP_WIDGET)0;

    td->tdTabSelectTableOption= (APP_WIDGET)0;
    td->tdTabSelectRowOption= (APP_WIDGET)0;
    td->tdTabSelectColumnOption= (APP_WIDGET)0;
    td->tdTabSelectSeparator= (APP_WIDGET)0;

    td->tdTabDeleteTableOption= (APP_WIDGET)0;
    td->tdTabDeleteRowOption= (APP_WIDGET)0;
    td->tdTabDeleteColumnOption= (APP_WIDGET)0;
    td->tdTabDeleteSeparator= (APP_WIDGET)0;

    td->tdDrawTableGridOption= (APP_WIDGET)0;

    td->tdFontMenu= (APP_WIDGET)0;
    td->tdFontMenuButton= (APP_WIDGET)0;
    td->tdFontBoldOption= (APP_WIDGET)0;
    td->tdFontItalicOption= (APP_WIDGET)0;
    td->tdFontUnderlinedOption= (APP_WIDGET)0;
    td->tdFontSuperscriptOption= (APP_WIDGET)0;
    td->tdFontSubscriptOption= (APP_WIDGET)0;
    td->tdFontToggleSeparator= (APP_WIDGET)0;
    td->tdFontPasteOption= (APP_WIDGET)0;

    td->tdToolsMenu= (APP_WIDGET)0;
    td->tdToolsMenuButton= (APP_WIDGET)0;
    td->tdToolsFormatToolOption= (APP_WIDGET)0;

    td->tdToolsFontOption= (APP_WIDGET)0;
    td->tdToolsFindOption= (APP_WIDGET)0;
    td->tdToolsSpellingOption= (APP_WIDGET)0;
    td->tdToolsPageLayoutOption= (APP_WIDGET)0;
    td->tdToolsSymbolOption= (APP_WIDGET)0;

    docInitDocumentSelection( &(td->tdSelection) );
    docInitSelectionGeometry( &(td->tdSelectionGeometry) );

    td->tdVisibleSelectionCopied= 0;

    utilInitMemoryBuffer( &(td->tdCopiedSelection) );
    utilInitMemoryBuffer( &(td->tdCopiedFont) );
    utilInitMemoryBuffer( &(td->tdCopiedRuler) );

    bmInitRasterImage( &(td->tdCopiedImage) );

    td->tdFindProg= (void *)0;
    td->tdOwnsPrimarySelection= 0;

#   ifdef USE_MOTIF
    td->tdHideIBarId= (XtIntervalId)0;
    td->tdShowIBarId= (XtIntervalId)0;
#   endif
#   ifdef USE_GTK
    td->tdHideIBarId= 0;
    td->tdShowIBarId= 0;
#   endif

    td->tdObjectWindow= (APP_WINDOW)0;
    for ( i= 0; i < RESIZE_COUNT; i++ )
	{ td->tdObjectResizeWindows[i]= (APP_WINDOW)0;	}
    td->tdObjectResizeCorner= -1;
    td->tdObjectCornerMovedX= 0;
    td->tdObjectCornerMovedY= 0;
    td->tdScaleChangedX= 0;
    td->tdScaleChangedY= 0;

    td->tdDrawTableGrid= 1;
    td->tdDrawMonochrome= 0;
    td->tdInProgrammaticChange= 0;

    return (void *)td;
    }

/************************************************************************/
/*									*/
/*  Keep track of focus.						*/
/*									*/
/************************************************************************/

void tedObserveFocus(	EditDocument *	ed,
			int		inout )
    {
    EditApplication *	ea= ed->edApplication;

    if  ( inout > 0 )
	{
	if  ( ea->eaCurrentDocument != ed )
	    {
	    appSetCurrentDocument( ea, ed );

	    tedAdaptPageToolToDocument( ea, ed );

	    tedAdaptFormatToolToDocument( ed, 0 );
	    }

	if  ( tedHasIBarSelection( ed ) )
	    { tedStartCursorBlink( ed );	}
	}

    if  ( inout < 0 )
	{ tedStopCursorBlink( ed ); }

    return;
    }

void tedSuggestPageSetup(	PrintGeometry *	pg,
				void *		privateData,
				int		sheetSize )
    {
    TedDocument *		td= (TedDocument *)privateData;
    BufferDocument *		bd= td->tdDocument;
    DocumentProperties *	dp= &(bd->bdProperties);

    if  ( sheetSize )
	{ pg->pgSheetGeometry= dp->dpGeometry;	}

    if  ( dp->dpTwoOnOne )
	{
	pg->pgGridRows= 2;
	pg->pgGridCols= 1;
	pg->pgGridHorizontal= 1;
	pg->pgScalePagesToFit= 1;
	}

    return;
    }

void tedSetTracedChangedFlag(	EditDocument *	ed )
    {
    const TedDocument *	td= (TedDocument *)ed->edPrivateData;
    const EditTrace *	et= &(td->tdEditTrace);
    int			changed;

    if  ( ! td->tdTraced )
	{ LDEB(td->tdTraced); return;	}

    changed= et->etIndex < et->etBase || et->etIndex > et->etBase+ 1;

    appDocumentChanged( ed, changed );

    return;
    }

