/************************************************************************/
/*									*/
/*  API for the document. (Editing and navigation.)			*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<string.h>
#   include	<ctype.h>
#   include	<sioGeneral.h>

#   include	"tedApp.h"
#   include	"tedEdit.h"
#   include	"tedSpellTool.h"
#   include	"tedDocFront.h"
#   include	"tedSelect.h"
#   include	"tedCopyPasteImpl.h"
#   include	"tedDocument.h"
#   include	"tedToolFront.h"
#   include	<docTreeType.h>
#   include	<docTextParticule.h>
#   include	<guiDrawingWidget.h>
#   include	<docEditCommand.h>
#   include	<docCollectStatistics.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Handle a replace from one of the tools.				*/
/*  (Used from the Find and Spell tools)				*/
/*									*/
/************************************************************************/

int tedDocReplace(	EditDocument *		ed,
			const char *		word )
    {
    TedDocument *	td= (TedDocument *)ed->edPrivateData;

    if  ( ! word || ! word[0] )
	{ SDEB(word); return -1;	}

    if  ( tedHasSelection( ed )				&&
	  ! tedHasIBarSelection( ed )			&&
	  td->tdSelectionDescription.sdCanReplace	)
	{
	tedDocReplaceSelection( ed, EDITcmdREPLACE, word, strlen( word ),
							    td->tdTraced );
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  'Select All' from the 'Edit' menu.					*/
/*									*/
/************************************************************************/

void tedDocSelAll(		EditDocument *	ed )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;

    DocumentSelection		dsNew;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    const int			lastLine= 0;

    if  ( tedGetSelection( &dsNew, &sg, &sd,
			    (DocumentTree **)0, (struct BufferItem **)0, ed ) )
	{ docInitDocumentSelection( &dsNew );	}

    if  ( docSelectWholeBody( &dsNew, bd ) )
	{ LDEB(1); return;	}

    tedSetSelection( ed, &dsNew, lastLine, (int *)0, (int *)0 );

    return;
    }

/************************************************************************/
/*									*/
/*  Navigation over columns.						*/
/*									*/
/************************************************************************/

void tedDocSelectWholeCell(	EditDocument *	ed,
				int		direction,
				int		allRows )
    {
    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    const int			lastLine= 0;

    if  ( tedGetSelection( &ds, &sg, &sd,
			(DocumentTree **)0, (struct BufferItem **)0, ed  ) )
	{ LDEB(1); return;	}

    if  ( docSelectWholeCell( &ds, direction, allRows ) )
	{ LDEB(1); return;	}

    tedSetSelection( ed, &ds, lastLine, (int *)0, (int *)0 );

    return;
    }

int tedDocSelectColumn(		EditDocument *	ed,
				int		col )
    {
    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    const int			lastLine= 0;

    if  ( tedGetSelection( &ds, &sg, &sd,
			(DocumentTree **)0, (struct BufferItem **)0, ed  ) )
	{ LDEB(1); return -1;	}

    if  ( docSelectColumn( &ds, col ) )
	{ LDEB(col); return -1;	}

    tedSetSelection( ed, &ds, lastLine, (int *)0, (int *)0 );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Navigation over rows.						*/
/*									*/
/************************************************************************/

void tedDocSelectRow(		EditDocument *	ed,
				int		direction,
				int		allColumns )
    {
    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    const int			lastLine= 0;

    if  ( tedGetSelection( &ds, &sg, &sd,
			(DocumentTree **)0, (struct BufferItem **)0, ed  ) )
	{ LDEB(1); return;	}

    if  ( docSelectRow( &ds, direction, allColumns ) )
	{ LDEB(1); return;	}

    tedSetSelection( ed, &ds, lastLine, (int *)0, (int *)0 );

    return;
    }

/************************************************************************/
/*									*/
/*  Select table.							*/
/*									*/
/************************************************************************/

void tedDocSelectTable(	EditDocument *		ed )
    {
    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    const int			lastLine= 0;

    if  ( tedGetSelection( &ds, &sg, &sd,
			(DocumentTree **)0, (struct BufferItem **)0, ed  ) )
	{ LDEB(1); return;	}

    if  ( docSelectWholeTable( &ds ) )
	{ LDEB(1); return;	}

    tedSetSelection( ed, &ds, lastLine, (int *)0, (int *)0 );

    return;
    }

int tedDocShiftRowsInTable(	EditDocument *		ed,
				int			direction )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    return tedEditShiftRowsInTable( ed, direction, td->tdTraced );
    }

/************************************************************************/
/*									*/
/*  Callback from 'Frame' tool.						*/
/*									*/
/************************************************************************/

int tedDocSelectCurrentFrame(	EditDocument *	ed )
    {
    DocumentSelection		dsNew;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    const int			lastLine= 0;

    if  ( tedGetSelection( &dsNew, &sg, &sd,
			    (DocumentTree **)0, (struct BufferItem **)0, ed ) )
	{ docInitDocumentSelection( &dsNew );	}

    if  ( docSelectFrameOfPosition( &dsNew, &(dsNew.dsHead) ) )
	{ LDEB(1); return -1;	}

    tedSetSelection( ed, &dsNew, lastLine, (int *)0, (int *)0 );

    return 0;
    }

APP_MENU_CALLBACK_H( tedDocInsertFootnote, option, voided, e )
    {
    EditDocument *		ed= (EditDocument *)voided;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    tedDocInsertNote( (EditDocument *)voided, DOCinFOOTNOTE, td->tdTraced );
    }

APP_MENU_CALLBACK_H( tedDocInsertEndnote, option, voided, e )
    {
    EditDocument *		ed= (EditDocument *)voided;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    tedDocInsertNote( (EditDocument *)voided, DOCinENDNOTE, td->tdTraced );
    }

APP_MENU_CALLBACK_H( tedDocTableSelectTable, option, voided, e )
    {
    EditDocument *		ed= (EditDocument *)voided;

    tedDocSelectTable( ed );

    return;
    }

APP_MENU_CALLBACK_H( tedDocTableSelectRow, option, voided, e )
    {
    EditDocument *		ed= (EditDocument *)voided;
    const int			direction= 0;

    const int			allColumns= 1;

    tedDocSelectRow( ed, direction, allColumns );

    return;
    }

APP_MENU_CALLBACK_H( tedDocTableSelectColumn, option, voided, e )
    {
    EditDocument *		ed= (EditDocument *)voided;
    const int			direction= 0;

    const int			allRows= 1;

    tedDocSelectWholeCell( ed, direction, allRows );

    return;
    }

APP_MENU_CALLBACK_H( tedDocEditUndo, option, voided, e )
    {
    EditDocument *		ed= (EditDocument *)voided;

    tedDocUndo( ed );
    }

APP_MENU_CALLBACK_H( tedDocEditRepeat, option, voided, e )
    {
    EditDocument *		ed= (EditDocument *)voided;

    tedDocRepeat( ed );
    }

/************************************************************************/
/*									*/
/*  Callback for the 'DrawTableGrid' toggle.				*/
/*									*/
/************************************************************************/

APP_MENU_CALLBACK_H( tedDocTableDrawGrid, option, voided, e )
    {
    EditDocument *		ed= (EditDocument *)voided;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    int				set;

    set= appGuiGetMenuToggleStateFromCallback( option, e );

    if  ( set )
	{ td->tdDrawTableGrid=  1;	}
    else{ td->tdDrawTableGrid= -1;	}

    appDocExposeRectangle( ed, (const DocumentRectangle *)0, 0,0 );

    return;
    }

APP_MENU_CALLBACK_H( tedDocTableDeleteTable, option, voided, e )
    {
    EditDocument *	ed= (EditDocument *)voided;
    TedDocument *	td= (TedDocument *)ed->edPrivateData;

    tedDocDeleteTable( ed, td->tdTraced );
    }

APP_MENU_CALLBACK_H( tedDocTableDeleteRow, option, voided, e )
    {
    EditDocument *	ed= (EditDocument *)voided;
    TedDocument *	td= (TedDocument *)ed->edPrivateData;

    tedDocDeleteRow( ed, td->tdTraced );
    }

APP_MENU_CALLBACK_H( tedDocTableDeleteColumn, option, voided, e )
    {
    EditDocument *	ed= (EditDocument *)voided;
    TedDocument *	td= (TedDocument *)ed->edPrivateData;

    tedDocDeleteColumn( ed, td->tdTraced );
    }

/************************************************************************/
/*									*/
/*  Insert a line/page/column break in the document.			*/
/*  Insert a note separator.						*/
/*									*/
/*  Note that page breaks and column breaks at the head of a paragraph	*/
/*  are modified into a paragraph property by the editing code.		*/
/*									*/
/************************************************************************/

APP_MENU_CALLBACK_H( tedDocInsertLineBreak, option, voided, e )
    {
    EditDocument *	ed= (EditDocument *)voided;
    TedDocument *	td= (TedDocument *)ed->edPrivateData;
    const int		redoLayout= 1;

    tedEditInsertSpecialParticule( ed,
			DOCkindLINEBREAK, EDITcmdREPLACE,
			redoLayout, td->tdTraced );

    return;
    }

APP_MENU_CALLBACK_H( tedDocInsertPageBreak, option, voided, e )
    {
    EditDocument *	ed= (EditDocument *)voided;
    TedDocument *	td= (TedDocument *)ed->edPrivateData;
    const int		redoLayout= 1;

    tedEditInsertSpecialParticule( ed,
		    DOCkindPAGEBREAK, EDITcmdREPLACE_BODY_LEVEL,
		    redoLayout, td->tdTraced );

    return;
    }

APP_MENU_CALLBACK_H( tedDocInsertColumnBreak, option, voided, e )
    {
    EditDocument *	ed= (EditDocument *)voided;
    TedDocument *	td= (TedDocument *)ed->edPrivateData;
    const int		redoLayout= 1;

    tedEditInsertSpecialParticule( ed,
		    DOCkindLINEBREAK, EDITcmdREPLACE_BODY_LEVEL, redoLayout, td->tdTraced );

    return;
    }

APP_MENU_CALLBACK_H( tedDocInsertChftnsep, option, voided, e )
    {
    EditDocument *	ed= (EditDocument *)voided;
    TedDocument *	td= (TedDocument *)ed->edPrivateData;
    const int		redoLayout= 0;

    tedEditInsertSpecialParticule( ed,
			DOCkindLINEBREAK, EDITcmdREPLACE_FTNSEP, redoLayout, td->tdTraced );

    return;
    }

/************************************************************************/
/*									*/
/*  Menu options to change paragraph alignment.				*/
/*									*/
/************************************************************************/

APP_MENU_CALLBACK_H( tedDocFormatAlignLeft, w, voided, e )
    {
    EditDocument *	ed= (EditDocument *)voided;
    TedDocument *	td= (TedDocument *)ed->edPrivateData;

    tedDocFormatSetParaAlignment( ed, DOCthaLEFT, td->tdTraced );
    }

APP_MENU_CALLBACK_H( tedDocFormatAlignRight, w, voided, e )
    {
    EditDocument *	ed= (EditDocument *)voided;
    TedDocument *	td= (TedDocument *)ed->edPrivateData;

    tedDocFormatSetParaAlignment( ed, DOCthaRIGHT, td->tdTraced );
    }

APP_MENU_CALLBACK_H( tedDocFormatAlignCenter, w, voided, e )
    {
    EditDocument *	ed= (EditDocument *)voided;
    TedDocument *	td= (TedDocument *)ed->edPrivateData;

    tedDocFormatSetParaAlignment( ed, DOCthaCENTERED, td->tdTraced );
    }

APP_MENU_CALLBACK_H( tedDocFormatAlignJustify, w, voided, e )
    {
    EditDocument *	ed= (EditDocument *)voided;
    TedDocument *	td= (TedDocument *)ed->edPrivateData;

    tedDocFormatSetParaAlignment( ed, DOCthaJUSTIFIED, td->tdTraced );
    }

/************************************************************************/
/*									*/
/*  Increase/Decrease the indentation level of the paragraphs in the	*/
/*  selection.								*/
/*									*/
/************************************************************************/

APP_MENU_CALLBACK_H( tedDocFormatIncreaseIndent, w, voided, e )
    {
    EditDocument *	ed= (EditDocument *)voided;
    TedDocument *	td= (TedDocument *)ed->edPrivateData;
    const int		direction= 1;

    tedFormatShiftIndent( ed, direction, td->tdTraced );
    }

APP_MENU_CALLBACK_H( tedDocFormatDecreaseIndent, w, voided, e )
    {
    EditDocument *	ed= (EditDocument *)voided;
    TedDocument *	td= (TedDocument *)ed->edPrivateData;
    const int		direction= -1;

    tedFormatShiftIndent( ed, direction, td->tdTraced );
    }

/************************************************************************/
/*									*/
/*  Replace the current selection with a page number field.		*/
/*									*/
/************************************************************************/

APP_MENU_CALLBACK_H( tedDocInsertPageNumber, option, voided, e )
    {
    EditDocument *	ed= (EditDocument *)voided;
    TedDocument *	td= (TedDocument *)ed->edPrivateData;

    tedInsertPageNumber( ed, td->tdTraced );
    }

/************************************************************************/
/*									*/
/*  Table related menu option callbacks.				*/
/*									*/
/************************************************************************/

APP_MENU_CALLBACK_H( tedDocTableInsertTable, option, voided, e )
    {
    EditDocument *	ed= (EditDocument *)voided;
    TedDocument *	td= (TedDocument *)ed->edPrivateData;

    int			rows= 2;
    int			columns= 2;


    if  ( tedInsertTable( ed, rows, columns, td->tdTraced ) )
	{ LLDEB(rows,columns);	}

    return;
    }

APP_MENU_CALLBACK_H( tedDocTableAddRow, option, voided, e )
    {
    EditDocument *	ed= (EditDocument *)voided;
    TedDocument *	td= (TedDocument *)ed->edPrivateData;
    const int		after= 1;

    tedDocAddRowToTable( ed, after, td->tdTraced );

    return;
    }

APP_MENU_CALLBACK_H( tedDocTableAddColumn, option, voided, e )
    {
    EditDocument *	ed= (EditDocument *)voided;
    TedDocument *	td= (TedDocument *)ed->edPrivateData;
    const int		after= 1;

    tedDocAddColumnToTable( ed, after, td->tdTraced );
    }

/************************************************************************/
/*									*/
/*  Font Menu.								*/
/*									*/
/************************************************************************/

APP_MENU_CALLBACK_H( tedDocFontPaste, option, voided, e )
    {
    EditDocument *			ed= (EditDocument *)voided;

    appDocAskForPaste( ed, "RTFFONT" );
    }

/************************************************************************/
/*									*/
/*  Insert a section break.						*/
/*									*/
/************************************************************************/

APP_MENU_CALLBACK_H( tedDocMenuInsertSectBreak, option, voided, e )
    {
    EditDocument *	ed= (EditDocument *)voided;
    TedDocument *	td= (TedDocument *)ed->edPrivateData;

    tedDocInsertSectBreak( ed, td->tdTraced );
    }

/************************************************************************/
/*									*/
/*  Toggle text attributes.						*/
/*									*/
/************************************************************************/

APP_MENU_CALLBACK_H( tedDocFontBold, option, voided, e )
    {
    int	set= appGuiGetMenuToggleStateFromCallback( option, e );
    tedToggleTextAttribute( (EditDocument *)voided, set, TApropFONTBOLD );
    }

APP_MENU_CALLBACK_H( tedDocFontItalic, option, voided, e )
    {
    int	set= appGuiGetMenuToggleStateFromCallback( option, e );
    tedToggleTextAttribute( (EditDocument *)voided, set, TApropFONTSLANTED );
    }

APP_MENU_CALLBACK_H( tedDocFontUnderlined, option, voided, e )
    {
    int	set= appGuiGetMenuToggleStateFromCallback( option, e );
    tedToggleTextAttribute( (EditDocument *)voided, set, TApropTEXTUNDERLINED );
    }

/************************************************************************/
/*									*/
/*  Change the base line of the current selection.			*/
/*									*/
/************************************************************************/

APP_MENU_CALLBACK_H( tedDocFontSupersub, option, voided, e )
    {
    EditDocument *	ed= (EditDocument *)voided;
    TedDocument *	td= (TedDocument *)ed->edPrivateData;

    int			set;
    int			supersub= TEXTvaREGULAR;

    set= appGuiGetMenuToggleStateFromCallback( option, e );
    if  ( set )
	{
	if  ( option == td->tdFontSubscriptOption )
	    { supersub= TEXTvaSUBSCRIPT;	}

	if  ( option == td->tdFontSuperscriptOption )
	    { supersub= TEXTvaSUPERSCRIPT;	}
	}
    else{ supersub= TEXTvaREGULAR;	}

    tedSetFontSupersub( ed, supersub );
    }

/************************************************************************/
/*									*/
/*  Merge the paragraphs in the selection.				*/
/*									*/
/************************************************************************/

APP_MENU_CALLBACK_H( tedDocFormatOnePara, option, voided, e )
    {
    EditDocument *	ed= (EditDocument *)voided;
    TedDocument *	td= (TedDocument *)ed->edPrivateData;

    tedMergeParagraphsInSelection( ed, td->tdTraced );
    }

/************************************************************************/
/*									*/
/*  Show or create a find tool.						*/
/*									*/
/************************************************************************/

APP_MENU_CALLBACK_H( tedDocToolFind, findOption, voided, e )
    {
    EditDocument *	ed= (EditDocument *)voided;

    tedShowFindTool( ed );

    return;
    }

APP_MENU_CALLBACK_H( tedDocToolFindNext, findOption, voided, e )
    {
    EditDocument *	ed= (EditDocument *)voided;
    TedDocument *	td;

    if  ( ! ed )
	{ XDEB(ed); return;	}
    td= (TedDocument *)ed->edPrivateData;

    if  ( ! td->tdFindProg )
	{
	tedShowFindTool( ed );
	return;
	}

    tedDocFindNext( ed );

    return;
    }

/************************************************************************/
/*									*/
/*  Show or create a spelling tool.					*/
/*									*/
/************************************************************************/

APP_MENU_CALLBACK_H( tedDocToolSpell, spellOption, voided, e )
    {
    EditDocument *	ed= (EditDocument *)voided;
    EditApplication *	ea= ed->edApplication;
    TedDocument *	td= (TedDocument *)ed->edPrivateData;

    /*  To get the correct title  */
    tedShowFormatTool( td->tdToolsFormatToolOption, ea );
    tedAdaptFormatToolToDocument( ed, 0 );

    tedFormatShowSpellPage( ea );

    return;
    }

/************************************************************************/
/*									*/
/*  Custom 'Paste' functionality.					*/
/*									*/
/************************************************************************/

APP_MENU_CALLBACK_H( tedDocEditPaste, option, voided, e )
    {
    EditDocument *			ed= (EditDocument *)voided;
    TedDocument *			td= (TedDocument *)ed->edPrivateData;

    if  ( ! td->tdSelectionDescription.sdCanReplace )
	{ LDEB(td->tdSelectionDescription.sdCanReplace); return;	}

    /*  1  */
    appDocAskForPaste( ed, "CLIPBOARD" );

    return;
    }

APP_MENU_CALLBACK_H( tedDocFormatPasteRul, fontsOption, voided, e )
    {
    EditDocument *			ed= (EditDocument *)voided;

    appDocAskForPaste( ed, "RTFRULER" );
    }

APP_MENU_CALLBACK_H( tedDocFontCopy, fontsOption, voided, e )
    {
    tedCopyFont( (EditDocument *)voided );
    }

APP_MENU_CALLBACK_H( tedDocFormatCopyRul, fontsOption, voided, e )
    {
    tedCopyRuler( (EditDocument *)voided );
    }

/************************************************************************/
/*									*/
/*  Ted: The 'Set' button on the font tool was pushed.			*/
/*									*/
/************************************************************************/

int tedDocFontToolSet(		EditDocument *			ed,
				const PropertyMask *		taSetMask,
				const ExpandedTextAttribute *	etaSet )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;
    DocumentProperties *	dp= &(bd->bdProperties);
    DocumentFontList *		dfl= dp->dpFontList;

    PropertyMask		doneMask;
    TextAttribute		taSet;


    utilInitTextAttribute( &taSet );
    utilPropMaskClear( &doneMask );

    docIndirectTextAttribute( &doneMask, &taSet, etaSet, taSetMask,
						dfl, dp->dpColorPalette );

    tedDocChangeTextAttribute( ed, taSetMask, &taSet, td->tdTraced );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Run the 'Link Dialog' from the 'Insert Link' menu option.		*/
/*									*/
/************************************************************************/

APP_MENU_CALLBACK_H( tedDocInsertLink, option, voided, e )
    {
    EditDocument *		ed= (EditDocument *)voided;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    EditApplication *		ea= ed->edApplication;

    tedShowFormatTool( td->tdToolsFormatToolOption, ea );

    tedAdaptFormatToolToDocument( ed, 0 );

    tedFormatShowLinkPage( ea );

    return;
    }

/************************************************************************/
/*									*/
/*  Run the 'Bookmark Dialog' from the 'Insert Bookmark' menu option.	*/
/*									*/
/************************************************************************/

APP_MENU_CALLBACK_H( tedDocInsertBookmark, option, voided, e )
    {
    EditDocument *		ed= (EditDocument *)voided;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    EditApplication *		ea= ed->edApplication;

    tedShowFormatTool( td->tdToolsFormatToolOption, ea );

    tedAdaptFormatToolToDocument( ed, 0 );

    tedFormatShowBookmarkPage( ea );
    }

/************************************************************************/
/*									*/
/*  Table related menu option callbacks.				*/
/*									*/
/************************************************************************/

APP_MENU_CALLBACK_H( tedDocShowPageTool, pageOption, voided, e )
    {
    EditDocument *	ed= (EditDocument *)voided;
    EditApplication *	ea= ed->edApplication;

    {
    TedDocument *	td= (TedDocument *)ed->edPrivateData;

    /*  To get the correct title  */
    tedShowFormatTool( td->tdToolsFormatToolOption, ea );
    tedAdaptFormatToolToDocument( ed, 0 );

    tedFormatShowPagePage( ea );
    }

    return;
    }

/************************************************************************/
/*									*/
/*  Font Menu.								*/
/*									*/
/************************************************************************/

APP_MENU_CALLBACK_H( tedDocToolFont, option, voided, e )
    {
    EditDocument *		ed= (EditDocument *)voided;
    EditApplication *		ea= ed->edApplication;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    /*  To get the correct title  */
    tedShowFormatTool( td->tdToolsFormatToolOption, ea );
    tedAdaptFormatToolToDocument( ed, 0 );

    tedFormatShowFontPage( ea );

    return;
    }

APP_MENU_CALLBACK_H( tedDocToolInsertSymbol, option, voided, e )
    {
    EditDocument *		ed= (EditDocument *)voided;
    EditApplication *		ea= ed->edApplication;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    /*  To get the correct title  */
    tedShowFormatTool( td->tdToolsFormatToolOption, ea );
    tedAdaptFormatToolToDocument( ed, 0 );

    tedFormatShowSymbolPage( ea );

    return;
    }

/************************************************************************/
/*									*/
/*  Show the format tool.						*/
/*									*/
/************************************************************************/

APP_MENU_CALLBACK_H( tedDocFormatTool, option, voided, e )
    {
    EditDocument *	ed= (EditDocument *)voided;
    EditApplication *	ea= ed->edApplication;

    /* option == td->tdToolsFormatToolOption */

    tedShowFormatTool( option, ea );
    tedAdaptFormatToolToDocument( ed, 0 );
    }

/************************************************************************/
/*									*/
/*  Move the rezize handles of the image that is sellected as part of	*/
/*  the scrollbar callback.						*/
/*									*/
/************************************************************************/

APP_SCROLLBAR_CALLBACK_H( tedDocHorizontalScrollbarCallback, w, voided, e )
    {
    EditDocument *		ed= (EditDocument *)voided;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    int				x= appGuiGetScrollbarValueFromCallback( w, e );

    appDocScrollToX( ed, x );

    if  ( td->tdSelectionDescription.sdIsObjectSelection )
	{ tedMoveObjectWindows( ed );	}

    return;
    }

APP_SCROLLBAR_CALLBACK_H( tedDocVerticalScrollbarCallback, w, voided, e )
    {
    EditDocument *		ed= (EditDocument *)voided;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    int				y= appGuiGetScrollbarValueFromCallback( w, e );

    appDocScrollToY( ed, y );

    if  ( td->tdSelectionDescription.sdIsObjectSelection )
	{ tedMoveObjectWindows( ed );	}

    return;
    }

/************************************************************************/
/*									*/
/*  Keep track of focus.						*/
/*									*/
/************************************************************************/

APP_EVENT_HANDLER_H( tedDocObserveFocus, w, voided, event )
    {
    EditDocument *	ed= (EditDocument *)voided;

    int			inout= 0;

    if  ( guiDrawGetInoutFromFocusEvent( &inout, w, event ) )
	{ return;	}

    tedObserveFocus( ed, inout );
    }

/************************************************************************/
/*									*/
/*  Run document properties dialog.					*/
/*									*/
/************************************************************************/

APP_MENU_CALLBACK_H( tedDocFileProps, option, voided, e )
    {
    EditDocument *		ed= (EditDocument *)voided;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;

    DocumentStatistics		ds;

    docCollectDocumentStatistics( &ds, bd );

    tedRunPropertyDialog( ed, &ds, option, "props" );
    }


/************************************************************************/
/*									*/
/*  Delete the table of contents					*/
/*									*/
/************************************************************************/

void tedDocDeleteTocField(	EditDocument *		ed,
				int			traced )
    {
    tedDocFlattenTypedField( ed, DOCfkTOC,
		(const PropertyMask *)0, (const TextAttribute *)0, traced );
    }

/************************************************************************/
/*									*/
/*  Unlock/Recover a document.						*/
/*									*/
/************************************************************************/

APP_MENU_CALLBACK_H( tedDocFileUnlock, option, voided, e )
    {
    EditDocument *		ed= (EditDocument *)voided;

    tedDocUnlock( ed );
    }

APP_MENU_CALLBACK_H( tedDocFileRecover, option, voided, e )
    {
    EditDocument *		ed= (EditDocument *)voided;

    tedDocRecover( ed );
    }
