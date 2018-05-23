/************************************************************************/
/*									*/
/*  Ted, main module.							*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>

#   include	"tedApp.h"
#   include	"tedAppResources.h"
#   include	"tedAppFront.h"
#   include	"tedDocFront.h"
#   include	"tedRuler.h"
#   include	"tedDocument.h"

#   include	<appUnit.h>
#   include	<psBuildConfigFiles.h>
#   include	<sioGeneral.h>
#   include	<sioStdout.h>
#   include	"tedFileConvert.h"

#   include	<appDebugon.h>

/************************************************************************/

static AppConfigurableResource TEDApplicationResourceTable[]=
{
    /********************************************************************/
    /*  Application defaults.						*/
    /*  2)  "magnification" is duplicate. It exists for the		*/
    /*	application and for the font chooser tool.			*/
    /********************************************************************/
    APP_RESOURCE( "fileFileNotRtf",
		offsetof(TedAppResources,tarFileNotRtf),
		"File is not in Rich Text Format. Open anyway?" ),
    APP_RESOURCE( "headFootNotReachable",
	    offsetof(TedAppResources,tarHeadFootNotReachable),
	    "There are not enough pages in the section to edit "
	    "this Header/Footer." ),

    APP_RESOURCE( "cannotRecover",
	    offsetof(TedAppResources,tarCannotRecover),
	    "Can not recover from this trace file. "
	    "Try to unlock." ),

    /********************************/
    /*  Application file menu.	*/
    /*  Application windows menu.	*/
    /********************************/
    APP_RESOURCE( "appFileMenuText",
		offsetof(TedAppResources,tarAppFileMenuText),
		"File" ),
    APP_RESOURCE( "appWinMenuText",
		offsetof(TedAppResources,tarAppWinMenuText),
		"Window" ),

    /********************************/
    /*  Document file menu.		*/
    /*  Document edit menu.		*/
    /*  Document windows menu.	*/
    /*  Document Font menu.		*/
    /*  Document Format menu.	*/
    /*  Document Table menu.	*/
    /*  Document Tool menu.		*/
    /*  Document Help menu.		*/
    /********************************/
    APP_RESOURCE( "docFileMenuText",
		offsetof(TedAppResources,tarDocFileMenuText),
		"File" ),
    APP_RESOURCE( "docEditMenuText",
		offsetof(TedAppResources,tarDocEditMenuText),
		"Edit" ),
    APP_RESOURCE( "docInsertMenuText",
		offsetof(TedAppResources,tarDocInsertMenuText),
		"Insert" ),
    APP_RESOURCE( "docWinMenuText",
		offsetof(TedAppResources,tarDocWinMenuText),
		"Window" ),
    APP_RESOURCE( "docFontMenuText",
		offsetof(TedAppResources,tarDocFontMenuText),
		"Font" ),
    APP_RESOURCE( "docFormatMenuText",
		offsetof(TedAppResources,tarDocFormatMenuText),
		"Format" ),
    APP_RESOURCE( "docTableMenuText",
		offsetof(TedAppResources,tarDocTableMenuText),
		"Table" ),
    APP_RESOURCE( "docToolsMenuText",
		offsetof(TedAppResources,tarDocToolMenuText),
		"Tools" ),
    APP_RESOURCE( "helpMenuText",
		offsetof(TedAppResources,tarHelpMenuText),
		"Help" ),

    /**/
#   ifdef __VMS
    APP_RESOURCE( "documentFileName",
		offsetof(TedAppResources,tarAppHelpFileName),
		"TED$ROOT:[Ted]TedDocument-en_US.rtf" ),
#   else
    APP_RESOURCE( "documentFileName",
		offsetof(TedAppResources,tarAppHelpFileName),
		DOCUMENT_DIR "/TedDocument-en_US.rtf" ),
#   endif

    /**/
    APP_RESOURCE( "pageNumberFormat",
	    offsetof(TedAppResources,tarPageNumberFormat),
	    "Page %p/%P, Section %s/%S" ),

    /**/
    APP_RESOURCE( "selectionColor",
		offsetof(TedAppResources,tarSelectionColor),
		"LightSteelBlue" ),
    APP_RESOURCE( "copiedSelectionColor",
		offsetof(TedAppResources,tarCopiedSelectionColor),
		"gray70" ),

    /**/
    APP_RESOURCE( "showTableGrid",
		offsetof(TedAppResources,tarShowTableGridString),
		"1" ),

    /**/
    APP_RESOURCE( "autoHyphenate",
		offsetof(TedAppResources,tarAutoHyphenateString),
		"0" ),

    /**/
    APP_RESOURCE( "shadingMeshTwips",
		offsetof(TedAppResources,tarShadingMeshTwipsString),
		(char *)0 ),

    /**/
    APP_RESOURCE( "pageGapMM",
		offsetof(TedAppResources,tarPageGapMMString),
		(char *)0 ),
    /**/
    APP_RESOURCE( "lenientRtf",
		offsetof(TedAppResources,tarLenientRtfString),
		"0" ),
    APP_RESOURCE( "traceEdits",
		offsetof(TedAppResources,tarTraceEditsString),
		"1" ),
    APP_RESOURCE( "pdfOutline",
		offsetof(TedAppResources,tarPdfOutlineString),
		"1" ),
    APP_RESOURCE( "overridePaperSize",
		offsetof(TedAppResources,tarOverridePaperSizeString),
		"0" ),
};


static AppConfigurableResource TEDFileMessageResourceTable[]=
    {
	/********************************/
	/*  Format for 'New' title	*/
	/********************************/
	APP_RESOURCE( "namelessFormat",
		    offsetof(AppFileMessageResources,afmrNamelessTitleFormat),
		    "Nameless %d" ),

	/********************************/
	/*  Really Close? Dialog.	*/
	/********************************/
	APP_RESOURCE( "reallyCloseQuestion",
		    offsetof(AppFileMessageResources,afmrReallyCloseQuestion),
		    "Do you want to save the changes you made to this text?" ),
	APP_RESOURCE( "reallyCloseSave",
		    offsetof(AppFileMessageResources,afmrReallyCloseSaveIt),
		    "Yes" ),
	APP_RESOURCE( "reallyCloseDontSave",
		    offsetof(AppFileMessageResources,afmrReallyCloseDontSave),
		    "No" ),
	APP_RESOURCE( "reallyCloseCancel",
		    offsetof(AppFileMessageResources,afmrReallyCloseCancel),
		    "Cancel" ),

	/********************************/
	/*  Really Quit? Dialog.	*/
	/********************************/
	APP_RESOURCE( "reallyQuitQuestion",
		    offsetof(AppFileMessageResources,afmrReallyQuitQuestion),
		    "Do you want to save changes to texts?" ),
	APP_RESOURCE( "reallyQuitReview",
		    offsetof(AppFileMessageResources,afmrReallyQuitReview),
		    "Yes" ),
	APP_RESOURCE( "reallyQuitQuitAnyway",
		    offsetof(AppFileMessageResources,afmrReallyQuitAnyway),
		    "No" ),
	APP_RESOURCE( "reallyQuitCancel",
		    offsetof(AppFileMessageResources,afmrReallyQuitCancel),
		    "Cancel" ),

	/********************************/
	/*  Messages about  a file.	*/
	/********************************/
	APP_RESOURCE( "fileFileNoAccess",
		    offsetof(AppFileMessageResources,afmrFileNoAccess),
		    "Cannot open file." ),
	APP_RESOURCE( "fileFileReadOnly",
		    offsetof(AppFileMessageResources,afmrFileReadOnly),
		    "File is Readonly." ),
	APP_RESOURCE( "fileNoSuchFile",
		    offsetof(AppFileMessageResources,afmrNoSuchFileMessage),
		    "This file does not exist." ),
	APP_RESOURCE( "fileNotWritable",
		    offsetof(AppFileMessageResources,afmrFileNotWritable),
		    "You cannot write to this file." ),
	APP_RESOURCE( "fileMakeIt",
		    offsetof(AppFileMessageResources,afmrMakeItQuestion),
		    "There is no such file. Make it?" ),

    };

static TedAppResources			TEDResources;

/************************************************************************/
/*									*/
/*  Display the online manual.						*/
/*									*/
/************************************************************************/

static APP_MENU_CALLBACK_H( tedDocManual, option, voided, e )
    {
    EditDocument *	ed= (EditDocument *)voided;
    EditApplication *	ea= ed->edApplication;

    tedManual( option, ea, ed->edToplevel.atTopWidget );
    }

static APP_MENU_CALLBACK_H( tedAppManual, option, voidea, e )
    {
    EditApplication *	ea= (EditApplication *)voidea;

    tedManual( option, ea, ea->eaToplevel.atTopWidget );
    }

/************************************************************************/
/*									*/
/*  Application file menu.						*/
/*									*/
/************************************************************************/

static AppMenuItem TED_AppFileMenuItems[]=
{
    {
    "appFileMenuNewText",	"New",
    "appFileMenuNewKey",	"Ctrl <Key>n",
    "appFileMenuNewKeyText",	"^N",		ITEMtyOPTION, appAppFileNew,
    },
    {
    "appFileMenuOpenText",	"Open",
    "appFileMenuOpenKey",	"Ctrl <Key>o",
    "appFileMenuOpenKeyText",	"^O",		ITEMtyOPTION, appAppFileOpen,
    },
    {
    "appFileMenuQuitText",	"Quit",
    "appFileMenuQuitKey",	(char *)0,
    "appFileMenuQuitKeyText",	(char *)0,	ITEMtyOPTION, appAppFileQuit,
    },
};

/************************************************************************/
/*									*/
/*  Application windows menu.						*/
/*									*/
/************************************************************************/

static AppMenuItem TED_AppWinMenuItems[]=
{
    {
    "appFileMenuNewText",	"New",
    "appFileMenuNewKey",	"Ctrl <Key>n",
    "appFileMenuNewKeyText",	"^N",		ITEMtyOPTION, appAppFileNew,
    },
};

/************************************************************************/
/*									*/
/*  Document file menu.							*/
/*									*/
/************************************************************************/

static AppMenuItem TED_DocFileMenuItems[]=
{
#   define	TEDmiDocFileNew			0
    {
    "docFileMenuNewText",	"New",
    "docFileMenuNewKey",	"Ctrl <Key>n",
    "docFileMenuNewKeyText",	"^N",		ITEMtyOPTION, appDocFileNew,
    },
#   define	TEDmiDocFileOpen		1
    {
    "docFileMenuOpenText",	"Open",
    "docFileMenuOpenKey",	"Ctrl <Key>o",
    "docFileMenuOpenKeyText",	"^O",		ITEMtyOPTION, appDocFileOpen,
    },
#   define	TEDmiDocFileSave		2
    {
    "docFileMenuSaveText",	"Save",
    "docFileMenuSaveKey",	"Ctrl <Key>s",
    "docFileMenuSaveKeyText",	"^S",		ITEMtyOPTION, appDocFileSave,
    },
#   define	TEDmiDocFileSaveAs		3
    {
    "docFileMenuSaveAsText",	"Save As ...",
    "docFileMenuSaveAsKey",	(char *)0,
    "docFileMenuSaveAsKeyText",	(char *)0,	ITEMtyOPTION, appDocFileSaveAs,
    },
#   define	TEDmiDocFilePrint		4
    {
    "docFileMenuPrintText",	"Print ...",
    "docFileMenuPrintKey",	"Ctrl <Key>p",
    "docFileMenuPrintKeyText",	"^P",		ITEMtyOPTION, tedDocFilePrint,
    },
#   define	TEDmiDocFileProperties		5
    {
    "docFileMenuPropsText",	"Properties ...",
    "docFileMenuPropsKey",	(char *)0,
    "docFileMenuPropsKeyText",	(char *)0,	ITEMtyOPTION, tedDocFileProps,
    },
#   define	TEDmiDocFileUnlock		6
    {
    "docFileMenuUnlockText",	"Unlock",
    "docFileMenuUnlockKey",	(char *)0,
    "docFileMenuUnlockKeyText",	(char *)0,	ITEMtyOPTION, tedDocFileUnlock,
    },
#   define	TEDmiDocFileRecover		7
    {
    "docFileMenuRecoverText",	"Recover",
    "docFileMenuRecoverKey",	(char *)0,
    "docFileMenuRecoverKeyText",(char *)0,	ITEMtyOPTION, tedDocFileRecover,
    },
#   define	TEDmiDocFileClose		8
    {
    "docFileMenuCloseText",	"Close",
    "docFileMenuCloseKey",	(char *)0,
    "docFileMenuCloseKeyText",	(char *)0,	ITEMtyOPTION, appDocFileClose,
    },

#   define	TEDmiDocFileQuitSeparator	9
    { "",	"", "", (char *)0, "", (char *)0, ITEMtySEPARATOR, },

#   define	TEDmiDocFileQuit		10
    {
    "docFileMenuQuitText",	"Quit",
    "docFileMenuQuitKey",	(char *)0,
    "docFileMenuQuitKeyText",	(char *)0,	ITEMtyOPTION, appDocFileQuit,
    },
};

/************************************************************************/
/*									*/
/*  Document edit menu.							*/
/*									*/
/************************************************************************/

static AppMenuItem TED_DocEditMenuItems[]=
{
#   define	TEDmiDocEditUndo	0
    {
    "docEditMenuUndoText",	"Undo",
    "docEditMenuUndoKey",	"Ctrl <Key>z",
    "docEditMenuUndoKeyText",	"^C",		ITEMtyOPTION, tedDocEditUndo,
    },
#   define	TEDmiDocEditRepeat	1
    {
    "docEditMenuRepeatText",	"Repeat",
    "docEditMenuRepeatKey",	"Ctrl <Key>y",
    "docEditMenuRepeatKeyText",	"^Y",		ITEMtyOPTION, tedDocEditRepeat,
    },
#   define	TEDmiDocEditCopy	2
    {
    "docEditMenuCopyText",	"Copy",
    "docEditMenuCopyKey",	"Ctrl <Key>c",
    "docEditMenuCopyKeyText",	"^C",		ITEMtyOPTION, appDocEditCopy,
    },
#   define	TEDmiDocEditCut		3
    {
    "docEditMenuCutText",	"Cut",
    "docEditMenuCutKey",	"Ctrl <Key>x",
    "docEditMenuCutKeyText",	"^X",		ITEMtyOPTION, appDocEditCut,
    },
#   define	TEDmiDocEditPaste	4
    {
    "docEditMenuPasteText",	"Paste",
    "docEditMenuPasteKey",	"Ctrl <Key>v",
    "docEditMenuPasteKeyText",	"^V",		ITEMtyOPTION, tedDocEditPaste,
    },
#   define	TEDmiDocEditSelectAll	5
    {
    "docEditMenuSelAllText",	"Select All",
    "docEditMenuSelAllKey",	"Ctrl <Key>a",
    "docEditMenuSelAllKeyText",	"^A",	ITEMtyOPTION, appDocEditSelAll,
    },

				    /*  6  */
    { "",	"", "", (char *)0, "", (char *)0, ITEMtySEPARATOR, },

#   define	TEDmiDocEditFind	7
    {
    "docToolMenuFindText",		"Find",
    "docToolMenuFindKey",		"Ctrl <Key>f",
    "docToolMenuFindKeyText",		"^F",
					ITEMtyOPTION, tedDocToolFind,
    },
#   define	TEDmiDocEditFindNext	8
    {
    "docToolMenuFindNextText",		"Find Next",
    "docToolMenuFindNextKey",		"<Key>F3",
    "docToolMenuFindNextKeyText",	"F3",
					ITEMtyOPTION, tedDocToolFindNext,
    },
};

/************************************************************************/
/*									*/
/*  Document edit menu.							*/
/*									*/
/************************************************************************/

static AppMenuItem TED_DocInsertMenuItems[]=
{
#   define	TEDmiDocInsertInsPict		0
    {
    "docInsertMenuInsPictText",		"Image ...",
    "docInsertMenuInsPictKey",		(char *)0,
    "docInsertMenuInsPictKeyText",	(char *)0,
					ITEMtyOPTION, tedDocInsertImage,
    },
#   define	TEDmiDocInsertInsSymbol		1
    {
    "docInsertMenuInsertSymbolText",	"Symbol",
    "docInsertMenuInsertSymbolKey",	(char *)0,
    "docInsertMenuInsertSymbolKeyText",	(char *)0,
					ITEMtyOPTION, tedDocToolInsertSymbol,
    },
#   define	TEDmiDocInsertInsHyperlink	2
    {
    "docInsertMenuHyperlinkText",	"Hyperlink",
    "docInsertMenuHyperlinkKey",	"Ctrl <Key>k",
    "docInsertMenuHyperlinkKeyText",	"^K",
					ITEMtyOPTION, tedDocInsertLink,
    },
#   define	TEDmiDocInsertInsBookmark	3
    {
    "docInsertMenuBookmarkText",	"Bookmark",
    "docInsertMenuBookmarkKey",		(char *)0,
    "docInsertMenuBookmarkKeyText",	(char *)0,
					ITEMtyOPTION, tedDocInsertBookmark,
    },
#   define	TEDmiDocInsertInsFootnote	4
    {
    "docInsertMenuFootnoteText",	"Footnote",
    "docInsertMenuFootnoteKey",		(char *)0,
    "docInsertMenuFootnoteKeyText",	(char *)0,
					ITEMtyOPTION, tedDocInsertFootnote,
    },
#   define	TEDmiDocInsertInsEndnote	5
    {
    "docInsertMenuEndnoteText",		"Endnote",
    "docInsertMenuEndnoteKey",		(char *)0,
    "docInsertMenuEndnoteKeyText",	(char *)0,
					ITEMtyOPTION, tedDocInsertEndnote,
    },
#   define	TEDmiDocInsertInsChftnsep	6
    {
    "docInsertMenuChftnsepText",	"Footnote Separator",
    "docInsertMenuChftnsepKey",		(char *)0,
    "docInsertMenuChftnsepKeyText",	(char *)0,
					ITEMtyOPTION, tedDocInsertChftnsep,
    },
#   define	TEDmiDocInsertInsFile		7
    {
    "docInsertMenuInsertFileText",	"File ...",
    "docInsertMenuInsertFileKey",	(char *)0,
    "docInsertMenuInsertFileKeyText",	(char *)0,
					ITEMtyOPTION, tedDocInsertFile,
    },
#   define	TEDmiDocInsertInsTable		8
    {
    "docInsertMenuInsertTableText",	"Table",
    "docInsertMenuInsertTableKey",	(char *)0,
    "docInsertMenuInsertTableKeyText",	(char *)0,
					ITEMtyOPTION, tedDocTableInsertTable,
    },
#   define	TEDmiDocInsertInsPageNumber	9
    {
    "docInsertMenuInsertPageNumberText",	"Page Number",
    "docInsertMenuInsertPageNumberKey",		(char *)0,
    "docInsertMenuInsertPageNumberKeyText",	(char *)0,
				    ITEMtyOPTION, tedDocInsertPageNumber,
    },

    /*  10  */
    { "",	"", "", (char *)0, "", (char *)0, ITEMtySEPARATOR, },

#   define	TEDmiDocInsertInsLineBreak	11
    {
    "docInsertMenuInsertLineBreakText",	"Line Break",
    "docInsertMenuInsertLineBreakKey",		(char *)0,
    "docInsertMenuInsertLineBreakKeyText",	(char *)0,
				    ITEMtyOPTION, tedDocInsertLineBreak,
    },
#   define	TEDmiDocInsertInsPageBreak	12
    {
    "docInsertMenuInsertPageBreakText",	"Page Break",
    "docInsertMenuInsertPageBreakKey",		(char *)0,
    "docInsertMenuInsertPageBreakKeyText",	(char *)0,
				    ITEMtyOPTION, tedDocInsertPageBreak,
    },
#   define	TEDmiDocInsertInsColumnBreak	13
    {
    "docInsertMenuInsertColumnBreakText",	"Column Break",
    "docInsertMenuInsertColumnBreakKey",	(char *)0,
    "docInsertMenuInsertColumnBreakKeyText",	(char *)0,
				    ITEMtyOPTION, tedDocInsertColumnBreak,
    },
#   define	TEDmiDocInsertInsSectBreak	14
    {
    "docInsertMenuInsertSectBreakText",	"Section Break",
    "docInsertMenuInsertSectBreakKey",		(char *)0,
    "docInsertMenuInsertSectBreakKeyText",	(char *)0,
				    ITEMtyOPTION, tedDocMenuInsertSectBreak,
    },

};

/************************************************************************/
/*									*/
/*  Document windows menu.						*/
/*									*/
/************************************************************************/

static AppMenuItem TED_DocWinMenuItems[]=
{
    {
    "docWinMenuCloseText",	"Close",
    "docWinMenuCloseKey",	(char *)0,
    "docWinMenuCloseKeyText",	(char *)0,	ITEMtyOPTION, appDocFileClose,
    },
};

/************************************************************************/
/*									*/
/*  Document font menu.							*/
/*									*/
/************************************************************************/

static AppMenuItem TED_DocFontMenuItems[]=
{
#   define	TEDmiDocFontBold	0
    {
    "docFontMenuBoldText",	"Bold",
    "docFontMenuBoldKey",	"Ctrl <Key>b",
    "docFontMenuBoldKeyText",	"^B",
				ITEMtyTOGGLE_OFF, tedDocFontBold,
    },
#   define	TEDmiDocFontItalic	1
    {
    "docFontMenuItalicText",	"Italic",
    "docFontMenuItalicKey",	"Ctrl <Key>i",
    "docFontMenuItalicKeyText",	"^I",
				ITEMtyTOGGLE_OFF, tedDocFontItalic,
    },
#   define	TEDmiDocFontUnderlined	2
    {
    "docFontMenuUnderlText",	"Underlined",
    "docFontMenuUnderlKey",	"Ctrl <Key>u",
    "docFontMenuUnderlKeyText",	"^U",
				ITEMtyTOGGLE_OFF, tedDocFontUnderlined,
    },
#   define	TEDmiDocFontSuperscript	3
    {
    "docFontMenuSuperscriptText",	"Superscript",
    "docFontMenuSuperscriptKey",	(char *)0,
    "docFontMenuSuperscriptKeyText",	(char *)0,
				ITEMtyTOGGLE_OFF, tedDocFontSupersub,
    },
#   define	TEDmiDocFontSubscript	4
    {
    "docFontMenuSubscriptText",		"Subscript",
    "docFontMenuSubscriptKey",		(char *)0,
    "docFontMenuSubscriptKeyText",	(char *)0,
				ITEMtyTOGGLE_OFF, tedDocFontSupersub,
    },

#   define	TEDmiDocFontToggleSeparator	5
    { "",	"", "", (char *)0, "", (char *)0, ITEMtySEPARATOR, },

    /*  6  */
    {
    "docFontMenuCopyText",	"Copy Font",
    "docFontMenuCopyKey",	(char *)0,
    "docFontMenuCopyKeyText",	(char *)0,	ITEMtyOPTION, tedDocFontCopy,
    },
#   define	TEDmiDocFontPaste	7
    {
    "docFontMenuPasteText",	"Paste Font",
    "docFontMenuPasteKey",	(char *)0,
    "docFontMenuPasteKeyText",	(char *)0,	ITEMtyOPTION, tedDocFontPaste,
    },

    /*  8  */
    { "",	"", "", (char *)0, "", (char *)0, ITEMtySEPARATOR, },

    /*  9  */
    {
    "docFontMenuToolText",	"Font Tool",
    "docFontMenuToolKey",	(char *)0,
    "docFontMenuToolKeyText",	(char *)0,	ITEMtyOPTION, tedDocToolFont,
    },
};

/************************************************************************/
/*									*/
/*  Document 'Table' menu.						*/
/*									*/
/************************************************************************/

static AppMenuItem TED_DocTableMenuItems[]=
{
#   define	TEDmiDocTableInsertTable	0
    {
    "docTableInsertTableText",		"Insert Table",
    "docTableInsertTableKey",		(char *)0,
    "docTableInsertTableKeyText",	(char *)0,
					ITEMtyOPTION, tedDocTableInsertTable,
    },
#   define	TEDmiDocTableAddRow		1
    {
    "docTableAddRowText",		"Add Row",
    "docTableAddRowKey",		(char *)0,
    "docTableAddRowKeyText",		(char *)0,
					ITEMtyOPTION, tedDocTableAddRow,
    },
#   define	TEDmiDocTableAddColumn		2
    {
    "docTableAddColumnText",		"Add Column",
    "docTableAddColumnKey",		(char *)0,
    "docTableAddColumnKeyText",	(char *)0,
					ITEMtyOPTION, tedDocTableAddColumn,
    },

#   define	TEDmiDocTableInsertSeparator	3
    { "",	"", "", (char *)0, "", (char *)0, ITEMtySEPARATOR, },

#   define	TEDmiDocTableSelectTable	4
    {
    "docTableSelectTableText",		"Select Table",
    "docTableSelectTableKey",		(char *)0,
    "docTableSelectTableKeyText",	(char *)0,
					ITEMtyOPTION, tedDocTableSelectTable,
    },
#   define	TEDmiDocTableSelectRow		5
    {
    "docTableSelectRowText",		"Select Row",
    "docTableSelectRowKey",		(char *)0,
    "docTableSelectRowKeyText",	(char *)0,
					ITEMtyOPTION, tedDocTableSelectRow,
    },
#   define	TEDmiDocTableSelectColumn	6
    {
    "docTableSelectColumnText",		"Select Column",
    "docTableSelectColumnKey",		(char *)0,
    "docTableSelectColumnKeyText",	(char *)0,
					ITEMtyOPTION, tedDocTableSelectColumn,
    },

#   define	TEDmiDocTableSelectSeparator	7
    { "",	"", "", (char *)0, "", (char *)0, ITEMtySEPARATOR, },

#   define	TEDmiDocTableDeleteTable	8
    {
    "docTableDeleteTableText",		"Delete Table",
    "docTableDeleteTableKey",		(char *)0,
    "docTableDeleteTableKeyText",	(char *)0,
					ITEMtyOPTION, tedDocTableDeleteTable,
    },
#   define	TEDmiDocTableDeleteRow		9
    {
    "docTableDeleteRowText",		"Delete Row",
    "docTableDeleteRowKey",		(char *)0,
    "docTableDeleteRowKeyText",	(char *)0,
					ITEMtyOPTION, tedDocTableDeleteRow,
    },
#   define	TEDmiDocTableDeleteColumn	10
    {
    "docTableDeleteColumnText",		"Delete Column",
    "docTableDeleteColumnKey",		(char *)0,
    "docTableDeleteColumnKeyText",	(char *)0,
					ITEMtyOPTION, tedDocTableDeleteColumn,
    },

#   define	TEDmiDocTableDeleteSeparator	11
    { "",	"", "", (char *)0, "", (char *)0, ITEMtySEPARATOR, },

#   define	TEDmiDocTableDrawGrid		12
    {
    "docTableDrawGridText",		"Draw Table Grid",
    "docTableDrawGridKey",		(char *)0,
    "docTableDrawGridKeyText",		(char *)0,
					ITEMtyTOGGLE_ON, tedDocTableDrawGrid,
    },
};

/************************************************************************/
/*									*/
/*  Document format menu.						*/
/*									*/
/************************************************************************/

static AppMenuItem TED_DocFormatMenuItems[]=
{
#   define	TEDmiDocFormatOnePara			0
    {
    "docFormatMenuOneParaText",		"Make One Paragraph",
    "docFormatMenuOneParaKey",		(char *)0,
    "docFormatMenuOneParaKeyText",	(char *)0,
					ITEMtyOPTION, tedDocFormatOnePara,
    },

    /*  1  */
#   define	TEDmiDocFormatOneParaSep		1
    { "",	"", "", (char *)0, "", (char *)0, ITEMtySEPARATOR, },

    /*  2  */
    {
    "docFormatMenuCopyRulText",		"Copy Ruler",
    "docFormatMenuCopyRulKey",		(char *)0,
    "docFormatMenuCopyRulKeyText",	(char *)0,
					ITEMtyOPTION, tedDocFormatCopyRul,
    },

#   define	TEDmiDocFormatPasteRuler		3
    {
    "docFormatMenuPasteRulText",	"Paste Ruler",
    "docFormatMenuPasteRulKey",		(char *)0,
    "docFormatMenuPasteRulKeyText",	(char *)0,
					ITEMtyOPTION, tedDocFormatPasteRul,
    },

#   define	TEDmiDocFormatIncIndent		4
    {
    "docFormatMenuIncreaseIndentText",	"Increase Indent",
    "docFormatMenuIncreaseIndentKey",		(char *)0,
    "docFormatMenuIncreaseIndentKeyText",	(char *)0,
				    ITEMtyOPTION, tedDocFormatIncreaseIndent,
    },

#   define	TEDmiDocFormatDecIndent		5
    {
    "docFormatMenuDecreaseIndentText",	"Decrease Indent",
    "docFormatMenuDecreaseIndentKey",		(char *)0,
    "docFormatMenuDecreaseIndentKeyText",	(char *)0,
				    ITEMtyOPTION, tedDocFormatDecreaseIndent,
    },

#   define	TEDmiDocFormatRulerSep		6
    { "",	"", "", (char *)0, "", (char *)0, ITEMtySEPARATOR, },

#   define	TEDmiDocFormatAlignLeft			7
    {
    "docFormatMenuAlignLeftText",	"Align Left",
    "docFormatMenuAlignLeftKey",	"Ctrl <Key>l",
    "docFormatMenuAlignLeftKeyText",	"^L",
				    ITEMtyOPTION, tedDocFormatAlignLeft,
    },

#   define	TEDmiDocFormatAlignRight		8
    {
    "docFormatMenuAlignRightText",	"Align Right",
    "docFormatMenuAlignRightKey",	"Ctrl <Key>r",
    "docFormatMenuAlignRightKeyText",	"^R",
				    ITEMtyOPTION, tedDocFormatAlignRight,
    },

#   define	TEDmiDocFormatAlignCenter		9
    {
    "docFormatMenuAlignCenterText",	"Center Text",
    "docFormatMenuAlignCenterKey",	"Ctrl <Key>e",
    "docFormatMenuAlignCenterKeyText",	"^E",
				    ITEMtyOPTION, tedDocFormatAlignCenter,
    },

#   define	TEDmiDocFormatAlignJustify		10
    {
    "docFormatMenuAlignJustifyText",	"Justify Text",
    "docFormatMenuAlignJustifyKey",	"Ctrl <Key>j",
    "docFormatMenuAlignJustifyKeyText",	"^J",
				    ITEMtyOPTION, tedDocFormatAlignJustify,
    },

#   define	TEDmiDocFormatAlignSep			11
    { "",	"", "", (char *)0, "", (char *)0, ITEMtySEPARATOR, },

#   define	TEDmiDocFormatTool			12
    {
    "docToolMenuFormatText",		"Format Tool",
    "docToolMenuFormatKey",		(char *)0,
    "docToolMenuFormatKeyText",		(char *)0,
					ITEMtyOPTION, tedDocFormatTool,
    }

};

/************************************************************************/
/*									*/
/*  Document tools menu.						*/
/*									*/
/************************************************************************/

static AppMenuItem TED_DocToolMenuItems[]=
{
#   define	TEDmiDocToolsFont		0
    {
    "docToolMenuFontText",		"Font Tool",
    "docToolMenuFontKey",		(char *)0,
    "docToolMenuFontKeyText",		(char *)0,
					ITEMtyOPTION, tedDocToolFont,
    },
#   define	TEDmiDocToolsFind		1
    {
    "docToolMenuFindText",		"Find",
    "docToolMenuFindKey",		"Ctrl <Key>f",
    "docToolMenuFindKeyText",		"^F",
					ITEMtyOPTION, tedDocToolFind,
    },
#   define	TEDmiDocToolsSpelling		2
    {
    "docToolMenuSpellText",		"Spelling",
    "docToolMenuSpellKey",		"<Key>F7",
    "docToolMenuSpellKeyText",		"F7",
					ITEMtyOPTION, tedDocToolSpell,
    },
#   define	TEDmiDocToolsPageLayout		3
    {
    "docToolMenuPageLayoutText",	"Page Layout",
    "docToolMenuPageLayoutKey",		(char *)0,
    "docToolMenuPageLayoutKeyText",	(char *)0,
					ITEMtyOPTION, tedDocShowPageTool,
    },
#   define	TEDmiDocToolsInsertSymbol	4
    {
    "docToolMenuInsertSymbolText",	"Insert Symbol",
    "docToolMenuInsertSymbolKey",	(char *)0,
    "docToolMenuInsertSymbolKeyText",	(char *)0,
					ITEMtyOPTION, tedDocToolInsertSymbol,
    },
#   define	TEDmiDocToolsFormatTool		5
    {
    "docToolMenuFormatText",		"Format Tool",
    "docToolMenuFormatKey",		(char *)0,
    "docToolMenuFormatKeyText",		(char *)0,
					ITEMtyOPTION, tedDocFormatTool,
    },
};

/************************************************************************/
/*									*/
/*  Document, Application help menu.					*/
/*									*/
/************************************************************************/

static AppMenuItem TED_DocHelpMenuItems[]=
{
    {
    "docWinMenuAboutText",	"About Ted",
    "docWinMenuAboutKey",	(char *)0,
    "docWinMenuAboutKeyText",	(char *)0,
				ITEMtyOPTION, appDocAbout,
    },
    {
    "docHelpMenuDocumentText",	"Document",
    "docHelpMenuDocumentKey",	(char *)0,
    "docHelpMenuDocumentKeyText", (char *)0,
				ITEMtyOPTION, tedDocManual,
    },
};

static AppMenuItem TED_AppHelpMenuItems[]=
{
    {
    "docHelpMenuDocumentText",	"Document",
    "docHelpMenuDocumentKey",	(char *)0,
    "docHelpMenuDocumentKeyText", (char *)0,
				ITEMtyOPTION, tedAppManual,
    },
};

/************************************************************************/
/*									*/
/*  Make private menus per document.					*/
/*									*/
/*  Remember Widget pointers of the menu options to be able to turn	*/
/*  on and off when appropriate.					*/
/*									*/
/*  In this files because it uses the TEDmiDocFileNew &c defines.	*/
/*									*/
/************************************************************************/

static void tedMakePrivateDocumentMenus( EditApplication *	ea,
					EditDocument *		ed,
					APP_WIDGET		menubar )
    {
    TedDocument *	td= (TedDocument *)ed->edPrivateData;

    td->tdFileOpenOption=
	    TED_DocFileMenuItems[TEDmiDocFileOpen].amiOptionWidget;
    td->tdFileSaveOption=
	    TED_DocFileMenuItems[TEDmiDocFileSave].amiOptionWidget;
    td->tdFileSaveAsOption=
	    TED_DocFileMenuItems[TEDmiDocFileSaveAs].amiOptionWidget;
    td->tdFileUnlockOption=
	    TED_DocFileMenuItems[TEDmiDocFileUnlock].amiOptionWidget;
    td->tdFileRecoverOption=
	    TED_DocFileMenuItems[TEDmiDocFileRecover].amiOptionWidget;
    td->tdFilePropertiesOption=
	    TED_DocFileMenuItems[TEDmiDocFileProperties].amiOptionWidget;
    td->tdFileCloseOption=
	    TED_DocFileMenuItems[TEDmiDocFileClose].amiOptionWidget;
    td->tdFileQuitSeparator=
	    TED_DocFileMenuItems[TEDmiDocFileQuitSeparator].amiOptionWidget;
    td->tdFileQuitOption=
	    TED_DocFileMenuItems[TEDmiDocFileQuit].amiOptionWidget;

    td->tdUndoOption=
	    TED_DocEditMenuItems[TEDmiDocEditUndo].amiOptionWidget;
    td->tdRepeatOption=
	    TED_DocEditMenuItems[TEDmiDocEditRepeat].amiOptionWidget;

    td->tdCopyOption=
	    TED_DocEditMenuItems[TEDmiDocEditCopy].amiOptionWidget;
    td->tdCutOption=
	    TED_DocEditMenuItems[TEDmiDocEditCut].amiOptionWidget;
    td->tdPasteOption=
	    TED_DocEditMenuItems[TEDmiDocEditPaste].amiOptionWidget;

    td->tdInsertMenu= appMakeMenu( &(td->tdInsertMenuButton),
			&(ed->edToplevel), ea, menubar,
			TEDResources.tarDocInsertMenuText, 0,
			TED_DocInsertMenuItems,
			sizeof(TED_DocInsertMenuItems)/sizeof(AppMenuItem),
			(void *)ed );

    td->tdInsPictOption=
	    TED_DocInsertMenuItems[TEDmiDocInsertInsPict].amiOptionWidget;
    td->tdInsFileOption=
	    TED_DocInsertMenuItems[TEDmiDocInsertInsFile].amiOptionWidget;
    td->tdInsSymbolOption=
	    TED_DocInsertMenuItems[TEDmiDocInsertInsSymbol].amiOptionWidget;
    td->tdInsHyperlinkOption=
	    TED_DocInsertMenuItems[TEDmiDocInsertInsHyperlink].amiOptionWidget;
    td->tdInsBookmarkOption=
	    TED_DocInsertMenuItems[TEDmiDocInsertInsBookmark].amiOptionWidget;
    td->tdInsInsertFootnoteOption=
	    TED_DocInsertMenuItems[TEDmiDocInsertInsFootnote].amiOptionWidget;
    td->tdInsInsertEndnoteOption=
	    TED_DocInsertMenuItems[TEDmiDocInsertInsEndnote].amiOptionWidget;
    td->tdInsInsertChftnsepOption=
	    TED_DocInsertMenuItems[TEDmiDocInsertInsChftnsep].amiOptionWidget;
    td->tdInsInsertTableOption=
	    TED_DocInsertMenuItems[TEDmiDocInsertInsTable].amiOptionWidget;
    td->tdInsInsertPageNumberOption=
	    TED_DocInsertMenuItems[TEDmiDocInsertInsPageNumber].amiOptionWidget;
    td->tdInsInsertLineBreakOption=
	    TED_DocInsertMenuItems[TEDmiDocInsertInsLineBreak].amiOptionWidget;
    td->tdInsInsertPageBreakOption=
	    TED_DocInsertMenuItems[TEDmiDocInsertInsPageBreak].amiOptionWidget;
    td->tdInsInsertColumnBreakOption=
	    TED_DocInsertMenuItems[TEDmiDocInsertInsColumnBreak].amiOptionWidget;
    td->tdInsInsertSectBreakOption=
	    TED_DocInsertMenuItems[TEDmiDocInsertInsSectBreak].amiOptionWidget;

    td->tdFontMenu= appMakeMenu( &(td->tdFontMenuButton),
			&(ed->edToplevel), ea, menubar,
			TEDResources.tarDocFontMenuText, 0,
			TED_DocFontMenuItems,
			sizeof(TED_DocFontMenuItems)/sizeof(AppMenuItem),
			(void *)ed );

    td->tdFontBoldOption=
	    TED_DocFontMenuItems[TEDmiDocFontBold].amiOptionWidget;
    td->tdFontItalicOption=
	    TED_DocFontMenuItems[TEDmiDocFontItalic].amiOptionWidget;
    td->tdFontUnderlinedOption=
	    TED_DocFontMenuItems[TEDmiDocFontUnderlined].amiOptionWidget;
    td->tdFontSuperscriptOption=
	    TED_DocFontMenuItems[TEDmiDocFontSuperscript].amiOptionWidget;
    td->tdFontSubscriptOption=
	    TED_DocFontMenuItems[TEDmiDocFontSubscript].amiOptionWidget;
    td->tdFontToggleSeparator=
	    TED_DocFontMenuItems[TEDmiDocFontToggleSeparator].amiOptionWidget;
    td->tdFontPasteOption=
	    TED_DocFontMenuItems[TEDmiDocFontPaste].amiOptionWidget;

    td->tdTableMenu= appMakeMenu( &(td->tdTableMenuButton),
			&(ed->edToplevel), ea, menubar,
			TEDResources.tarDocTableMenuText, 0,
			TED_DocTableMenuItems,
			sizeof(TED_DocTableMenuItems)/sizeof(AppMenuItem),
			(void *)ed );

    td->tdTabInsertTableOption=
	    TED_DocTableMenuItems[TEDmiDocTableInsertTable].amiOptionWidget;
    td->tdTabAddRowOption=
	    TED_DocTableMenuItems[TEDmiDocTableAddRow].amiOptionWidget;
    td->tdTabAddColumnOption=
	    TED_DocTableMenuItems[TEDmiDocTableAddColumn].amiOptionWidget;
    td->tdTabInsertSeparator=
	    TED_DocTableMenuItems[TEDmiDocTableInsertSeparator].amiOptionWidget;

    td->tdTabSelectTableOption=
	    TED_DocTableMenuItems[TEDmiDocTableSelectTable].amiOptionWidget;
    td->tdTabSelectRowOption=
	    TED_DocTableMenuItems[TEDmiDocTableSelectRow].amiOptionWidget;
    td->tdTabSelectColumnOption=
	    TED_DocTableMenuItems[TEDmiDocTableSelectColumn].amiOptionWidget;
    td->tdTabSelectSeparator=
	    TED_DocTableMenuItems[TEDmiDocTableSelectSeparator].amiOptionWidget;

    td->tdTabDeleteTableOption=
	    TED_DocTableMenuItems[TEDmiDocTableDeleteTable].amiOptionWidget;
    td->tdTabDeleteRowOption=
	    TED_DocTableMenuItems[TEDmiDocTableDeleteRow].amiOptionWidget;
    td->tdTabDeleteColumnOption=
	    TED_DocTableMenuItems[TEDmiDocTableDeleteColumn].amiOptionWidget;
    td->tdTabDeleteSeparator=
	    TED_DocTableMenuItems[TEDmiDocTableDeleteSeparator].amiOptionWidget;

    td->tdDrawTableGridOption=
	    TED_DocTableMenuItems[TEDmiDocTableDrawGrid].amiOptionWidget;

    td->tdFormatMenu= appMakeMenu( &(td->tdFormatMenuButton),
			&(ed->edToplevel), ea, menubar,
			TEDResources.tarDocFormatMenuText, 0,
			TED_DocFormatMenuItems,
			sizeof(TED_DocFormatMenuItems)/sizeof(AppMenuItem),
			(void *)ed );

    td->tdFormatOneParaOption=
	    TED_DocFormatMenuItems[TEDmiDocFormatOnePara].amiOptionWidget;
    td->tdFormatOneParaSeparator=
	    TED_DocFormatMenuItems[TEDmiDocFormatOneParaSep].amiOptionWidget;
    td->tdFormatPasteRulerOption= 
	    TED_DocFormatMenuItems[TEDmiDocFormatPasteRuler].amiOptionWidget;
    td->tdFormatIncreaseIndentOption= 
	    TED_DocFormatMenuItems[TEDmiDocFormatIncIndent].amiOptionWidget;
    td->tdFormatDecreaseIndentOption= 
	    TED_DocFormatMenuItems[TEDmiDocFormatDecIndent].amiOptionWidget;
    td->tdFormatRulerSeparator= 
	    TED_DocFormatMenuItems[TEDmiDocFormatRulerSep].amiOptionWidget;

    td->tdFormatAlignLeftOption= 
	    TED_DocFormatMenuItems[TEDmiDocFormatAlignLeft].amiOptionWidget;
    td->tdFormatAlignRightOption= 
	    TED_DocFormatMenuItems[TEDmiDocFormatAlignRight].amiOptionWidget;
    td->tdFormatAlignCenterOption= 
	    TED_DocFormatMenuItems[TEDmiDocFormatAlignCenter].amiOptionWidget;
    td->tdFormatAlignJustifyOption= 
	    TED_DocFormatMenuItems[TEDmiDocFormatAlignJustify].amiOptionWidget;
    td->tdFormatAlignSeparator= 
	    TED_DocFormatMenuItems[TEDmiDocFormatAlignSep].amiOptionWidget;

    td->tdToolsMenu= appMakeMenu( &(td->tdToolsMenuButton),
			&(ed->edToplevel), ea, menubar,
			TEDResources.tarDocToolMenuText, 0,
			TED_DocToolMenuItems,
			sizeof(TED_DocToolMenuItems)/sizeof(AppMenuItem),
			(void *)ed );

    td->tdToolsFontOption=
	    TED_DocToolMenuItems[TEDmiDocToolsFont].amiOptionWidget;
    td->tdToolsFindOption=
	    TED_DocToolMenuItems[TEDmiDocToolsFind].amiOptionWidget;
    td->tdToolsFormatToolOption=
	    TED_DocToolMenuItems[TEDmiDocToolsFormatTool].amiOptionWidget;
    td->tdToolsSpellingOption=
	    TED_DocToolMenuItems[TEDmiDocToolsSpelling].amiOptionWidget;
    td->tdToolsPageLayoutOption=
	    TED_DocToolMenuItems[TEDmiDocToolsPageLayout].amiOptionWidget;
    td->tdToolsSymbolOption=
	    TED_DocToolMenuItems[TEDmiDocToolsInsertSymbol].amiOptionWidget;

    ed->edFileCloseOption= td->tdFileCloseOption;

    return;
    }

/************************************************************************/
/*									*/
/*  Ted, description of the application.				*/
/*									*/
/************************************************************************/

static AppFileExtension	TedFileExtensions[TEDdockind_COUNT]=
{
    { "rtfFiles",	"*.rtf",
			"Rich Text Files ( *.rtf )",
			"rtf"	 ,
			( APPFILE_CAN_OPEN|APPFILE_CAN_SAVE ),
			},
    { "txtFiles",	"*.txt",
			"Text Files ( *.txt )",	
			"txt"	 ,
			( APPFILE_CAN_OPEN ),
			},
    { "txtFilesWide",	"*.txt",
			"Wide Text Files ( *.txt )",	
			"txt"	 ,
			( APPFILE_CAN_SAVE|APPFILE_CAN_OPEN|APPFILE_HIDE_OPEN ),
			},
    { "txtFilesFolded",	"*.txt",
			"Folded Text Files ( *.txt )",	
			"txt"	 ,
			( APPFILE_CAN_SAVE|APPFILE_CAN_OPEN|APPFILE_HIDE_OPEN ),
			},
    { "htmlFiles",	"*.html",
			"HTML Files ( *.html )",
			"html",
			( APPFILE_CAN_SAVE ),
    },
    { "emlFiles",	"*.eml",
			"HTML Mail message (rfc 2557) ( *.eml )",
			"eml",
			( APPFILE_CAN_SAVE ),
    },
    { "epubFiles",	"*.epub",
			"EPUB e-book ( *.epub )",
			"epub",
			( APPFILE_CAN_SAVE ),
    },
    { "psFiles",	"*.ps",
			"PostScript ( *.ps )",
			"ps",
			( APPFILE_CAN_SAVE ),
    },
    { "svgFiles",	"*.svg",
			"Scalable Vector Graphics ( *.svg )",
			"svg",
			( APPFILE_CAN_SAVE ),
    },
    { "pdfFiles",	"*.pdf",
			"Portable Document Format ( *.pdf )",
			"pdf",
			( APPFILE_CAN_SAVE ),
    },
    { "traceFiles",	"*.Ted",
			"Ted Recovery File ( *.Ted )",
			"Ted",
			( APPFILE_CAN_OPEN ),
    },
    { "allFiles",	"*",
			"All Files ( *.* )",
			(char *)0,
			( APPFILE_CAN_OPEN ),
    },
};

static AppSelectionTargetType TedClipboardTargets[]=
{
    { "application/rtf", (APP_ATOM)0,	tedPasteClipboardRtf,
					tedCopyClipboardRtf		},
    { "UTF8_STRING", (APP_ATOM)0,	tedPasteClipboardString,
					tedCopyClipboardString		},
    { "STRING",	(APP_ATOM)0,		tedPasteClipboardString,
					tedCopyClipboardString		},
    { "image/png", (APP_ATOM)0,		tedPasteClipboardPng,
					tedCopyClipboardPng		},
    { "image/jpeg", (APP_ATOM)0,	tedPasteClipboardJfif,
					tedCopyClipboardJfif		},
};

AppSelectionTargetType TedPrimaryTargets[]=
{
    { "UTF8_STRING", (APP_ATOM)0,	tedPastePrimaryString,
					tedCopyPrimaryString		},
    { "STRING",	(APP_ATOM)0,		tedPastePrimaryString,
					tedCopyPrimaryString		},
};

const int TedPrimaryTargetCount=
	    sizeof(TedPrimaryTargets)/sizeof(AppSelectionTargetType);

AppSelectionTargetType TedClipboardTextTargets[]=
{
    { "application/rtf", (APP_ATOM)0,	tedPasteClipboardRtf,
					tedCopyClipboardRtf		},
    { "UTF8_STRING", (APP_ATOM)0,	tedPasteClipboardString,
					tedCopyClipboardString		},
    { "STRING",	(APP_ATOM)0,		tedPasteClipboardString,
					tedCopyClipboardString		},
};

const int TedClipboardTextTargetCount=
	    sizeof(TedClipboardTextTargets)/sizeof(AppSelectionTargetType);

AppSelectionTargetType TedRulerTargets[]=
{
    { "application/rtf",	(APP_ATOM)0,	tedPasteRulerTed,
						tedCopyRulerTed		},
};

const int TedRulerTargetCount=
	    sizeof(TedRulerTargets)/sizeof(AppSelectionTargetType);

AppSelectionTargetType TedFontTargets[]=
{
    { "application/rtf",	(APP_ATOM)0,	tedPasteFontTed,
						tedCopyFontTed		},
};

const int TedFontTargetCount=
	    sizeof(TedFontTargets)/sizeof(AppSelectionTargetType);

static AppSelectionType TedDocSelectionTypes[]=
{
    { "CLIPBOARD", (APP_ATOM)0,
	    TedClipboardTargets,
	    sizeof(TedClipboardTargets)/sizeof(AppSelectionTargetType),
	    tedClipboardLost },
    { "RTFFONT", (APP_ATOM)0,
	    TedFontTargets,
	    sizeof(TedFontTargets)/sizeof(AppSelectionTargetType), },
    { "RTFRULER", (APP_ATOM)0,
	    TedRulerTargets,
	    sizeof(TedRulerTargets)/sizeof(AppSelectionTargetType), },
    { "PRIMARY", (APP_ATOM)0,
	    TedPrimaryTargets,
	    sizeof(TedPrimaryTargets)/sizeof(AppSelectionTargetType),
	    tedPrimaryLost	},
};

/************************************************************************/

static void tedMainSetPageLayout( EditDocument *		ed,
				const PropertyMask *		dgSetMask,
				const DocumentGeometry *	dgSet,
				int				wholeDocument )
    {

    tedDocSetPageLayout( ed, dgSetMask, dgSet, wholeDocument, ((TedDocument *)ed->edPrivateData)->tdTraced );
    }

/************************************************************************/

static int tedFind(		EditApplication *		ea,
				const char *			prog,
				const char *			call,
				int				argc,
				char **				argv )
    {
    if  ( argc < 1 )
	{ SLDEB(call,argc); return -1;	}

    TEDResources.tarFindPattern= (const char *)argv[0];
    TEDResources.tarFindRegex= 0;

    return 1;
    }

static int tedRegFind(		EditApplication *		ea,
				const char *			prog,
				const char *			call,
				int				argc,
				char **				argv )
    {
    if  ( argc < 1 )
	{ SLDEB(call,argc); return -1;	}

    TEDResources.tarFindPattern= (const char *)argv[0];
    TEDResources.tarFindRegex= 1;

    return 1;
    }

static int tedGSFontmapForFiles(EditApplication *		ea,
				const char *			prog,
				const char *			call,
				int				argc,
				char **				argv )
    {
    int		ret= argc;

    SimpleOutputStream *	sosOut= sioOutStdoutOpen();
    if  ( ! sosOut )
	{ XDEB(sosOut); return -1;	}

    if  ( psFontmapForFiles( sosOut, argc, (const char * const *)argv ) )
	{ SLDEB(call,argc); ret= -1;	}

    sioOutClose( sosOut );

    return ret;
    }

static int tedRtfToPs(		EditApplication *		ea,
				const char *			prog,
				const char *			call,
				int				argc,
				char **				argv )
    {
    fprintf( stderr, "Use Ted --printToFile -rtf- -ps-\n" );
    return -1;
    }

static int tedRtfToPsPaper(	EditApplication *		ea,
				const char *			prog,
				const char *			call,
				int				argc,
				char **				argv )
    {
    fprintf( stderr, "Use Ted --printToFilePaper -rtf- -ps- %s\n", argv[0] );
    return 1;
    }

static SpecialCall TedSpecialCalls[]=
{
    { "TtfToAfm",		tedTtfToAfm,			},
    { "AfmToGSFontmap",		tedAfmToGSFontmap,		},
    { "GSFontmapForFiles",	tedGSFontmapForFiles,		},
    { "AfmForFontFiles",	tedAfmForFontFiles,		},
    { "RtfToPs",		tedRtfToPs,			},
    { "RtfToPsPaper",		tedRtfToPsPaper,		},
    { "Find",			tedFind,			},
    { "RegFind",		tedRegFind,			},
    { "FontsDocuments",		tedFontsDocuments,		},
};

/************************************************************************/

#   ifdef USE_FONTCONFIG
#	include	<fontconfig/fontconfig.h> /* keep for version */
#	include	<ft2build.h> /* keep for version */
#   endif

#   ifdef USE_XFT
#	include	FT_FREETYPE_H /* keep for version */
#   endif

# define VSS(x) #x
# define VSSS(x) VSS(x)

#   ifdef XmVERSION_STRING
    # define MOTIF_VSS "\"" XmVERSION_STRING "\" "
#   else
    # define MOTIF_VSS "Motif "
#   endif

# define GTK_VSS \
	"GTK " VSSS(GTK_MAJOR_VERSION) "." \
	VSSS(GTK_MINOR_VERSION) "." \
	VSSS(GTK_MICRO_VERSION) " "

# define FT2_VSS \
    "FT2 " VSSS(FREETYPE_MAJOR) "." VSSS(FREETYPE_MINOR) " "

# define XFT_VSS \
    "XFT " VSSS(XFT_MAJOR) "." VSSS(XFT_MINOR) "." VSSS(XFT_REVISION) " "

# define FC_VSS \
    "FONTCONFIG " VSSS(FC_MAJOR) "." VSSS(FC_MINOR) "." VSSS(FC_REVISION) " "

static const char TED_ConfigString[]=

#   ifdef USE_MOTIF
    MOTIF_VSS
#   endif

#   ifdef USE_GTK
    GTK_VSS
#   endif

#   ifdef USE_FONTCONFIG
    FC_VSS
#   endif

#   ifdef USE_XFT
    FT2_VSS
#   endif

    "";

/************************************************************************/

static EditApplication	TedApplication=
    {
		    /****************************************************/
		    /*  Ted Application name for Xt.			*/
		    /*  Version information for application window.	*/
		    /*  Reference to site for application window.	*/
		    /*  Document shell name for Xt.			*/
		    /*  Page Tool shell name for Xt.			*/
		    /*  Print Dialog shell name for Xt.			*/
		    /*  Mail Dialog shell name for Xt.			*/
		    /*  Message Dialog shell name for Xt.		*/
		    /*  Name of the icon for the application window.	*/
		    /*  Name of the picture for the application window.	*/
		    /****************************************************/
    "Ted",
    TED_ConfigString,
    "Ted 2.23, Feb 4, 2013",
    "http://www.nllgg.nl/Ted",
    MY_PLATFORM " " MY_RELEASE,
    "\"" MY_HOST_DATE "\"",

    "tedmain",
    "tedabout",
		    /****************************************************/
		    /*  File extensions, count.				*/
		    /****************************************************/
    TedFileExtensions, sizeof(TedFileExtensions)/sizeof(AppFileExtension),
		    /****************************************************/
		    /*  Default filter for file choosers.		*/
		    /****************************************************/
    "*.rtf",
    (void *)&TEDResources,
    TEDApplicationResourceTable,
    sizeof(TEDApplicationResourceTable)/sizeof(AppConfigurableResource),
    TEDFileMessageResourceTable,
    sizeof(TEDFileMessageResourceTable)/sizeof(AppConfigurableResource),
    TedSpecialCalls, sizeof(TedSpecialCalls)/sizeof(SpecialCall),
		    /****************************************************/
		    /*  Create new document from command line?		*/
		    /****************************************************/
    1,

    1.0,

		    /****************************************************/
		    /*  Ruler Geometry.					*/
		    /****************************************************/
    4, 8, 4, 4,
    6, 6, 6, 6,
		    /****************************************************/
		    /*  Default Geometry resources.			*/
		    /****************************************************/
    "inch",
    "letter",
    "90pt",
    "72pt",
    "90pt",
    "72pt",
    UNITtyINCH,
	{ 12240, 15840, },
		    /****************************************************/
		    /*  Application File Menu.				*/
		    /****************************************************/
    &(TEDResources.tarAppFileMenuText),
    TED_AppFileMenuItems,
    sizeof(TED_AppFileMenuItems)/sizeof(AppMenuItem),
		    /****************************************************/
		    /*  Application Window Menu.			*/
		    /****************************************************/
    &(TEDResources.tarAppWinMenuText),
    TED_AppWinMenuItems,
    sizeof(TED_AppWinMenuItems)/sizeof(AppMenuItem),
		    /****************************************************/
		    /*  Document File Menu.				*/
		    /****************************************************/
    &(TEDResources.tarDocFileMenuText),
    TED_DocFileMenuItems,
    sizeof(TED_DocFileMenuItems)/sizeof(AppMenuItem),
		    /****************************************************/
		    /*  Document Edit Menu.				*/
		    /****************************************************/
    &(TEDResources.tarDocEditMenuText),
    TED_DocEditMenuItems,
    sizeof(TED_DocEditMenuItems)/sizeof(AppMenuItem),
		    /****************************************************/
		    /*  Document Window Menu.				*/
		    /****************************************************/
    &(TEDResources.tarDocWinMenuText),
    TED_DocWinMenuItems,
    sizeof(TED_DocWinMenuItems)/sizeof(AppMenuItem),
		    /****************************************************/
		    /*  Document, Application Help Menu.		*/
		    /****************************************************/
    &(TEDResources.tarHelpMenuText),
    TED_DocHelpMenuItems,
    sizeof(TED_DocHelpMenuItems)/sizeof(AppMenuItem),

    &(TEDResources.tarHelpMenuText),
    TED_AppHelpMenuItems,
    sizeof(TED_AppHelpMenuItems)/sizeof(AppMenuItem),
		    /****************************************************/
		    /*  Document Widgets.				*/
		    /*  Their initialisation.				*/
		    /*  How to save them.				*/
		    /****************************************************/
    tedMakePrivateData,
    tedMakeDocumentWidget,
    tedOpenDocument,
    tedNewDocument,
    tedLayoutDocument,
    tedFinishDocumentSetup,
    tedDocumentFirstVisible,
    NULL,					/*  CanSave		*/
    tedSaveDocument,				/*  Save		*/
    tedFreeDocument,				/*  Free		*/
    tedSuggestPageSetup,			/*  SuggestPageSetup	*/
    tedPrintDocument,				/*  PrintDocument	*/
    tedDrawRectangle,
		    /****************************************************/
		    /*  Track document count.				*/
		    /****************************************************/
    tedVisibleDocumentCountChanged,
		    /****************************************************/
		    /*  Private application,document menus.		*/
		    /****************************************************/
    NULL,
    tedMakePrivateDocumentMenus,
		    /****************************************************/
		    /*  Cut and paste functions. (Document Edit Menu)	*/
		    /****************************************************/
    tedDocCopy,
    tedDocCut,
    tedDocSelAll,
		    /****************************************************/
		    /*							*/
		    /*  Set page layout.				*/
		    /*							*/
		    /****************************************************/
    tedMainSetPageLayout,
		    /****************************************************/
		    /*  User input on the document widget:		*/
		    /****************************************************/
    tedMouseButtonPressed,
    tedScrollEventHandler,
    tedDocGotString,
    tedDocGotKey,
    tedDocObserveFocus,

    tedDocHorizontalScrollbarCallback,
    tedDocVerticalScrollbarCallback,

    tedScrollTopRuler,
    tedSetTopRulerRange,
    NULL,				/*  ScrollLeftRuler		*/
    NULL,				/*  SetLeftRulerRange		*/
    NULL,				/*  ScrollRightRuler		*/
    NULL,				/*  SetRightRulerRange		*/
    NULL,				/*  ScrollBottomRuler		*/
    NULL,				/*  SetBottomRulerRange		*/

    tedDocSetTopRuler,			/*  eaSetTopRuler		*/
    tedDocSetLeftRuler,			/*  eaSetLeftRuler		*/
    tedDocSetRightRuler,		/*  eaSetRightRuler		*/
    tedDocSetBottomRuler,		/*  eaSetBottomRuler		*/

    tedFreeTopRuler,			/*  eaFreeTopRuler		*/
    tedFreeVerticalRuler,		/*  eaFreeLeftRuler		*/
    tedFreeVerticalRuler,		/*  eaFreeRightRuler		*/
    tedFreeBottomRuler,			/*  eaFreeBottomRuler		*/

    TedDocSelectionTypes,
    sizeof(TedDocSelectionTypes)/ sizeof(AppSelectionType),

    (AppSelectionType *)0,
    0/ sizeof(AppSelectionType),

    0,0,0,0,
    };

/************************************************************************/
/*									*/
/*  Main() of the 'Ted' Application.					*/
/*									*/
/*  1)  Sanity checks							*/
/*  2)  Initialisation of the application wide data.			*/
/*									*/
/************************************************************************/

int main(	int		argc,
		char *		argv[]	)
    {
    EditApplication *	ea= &TedApplication;

    /*  1  */
    if  ( RPprop_FULL_COUNT > PROPmaskMAXPROPS )
	{ LLDEB(RPprop_FULL_COUNT,PROPmaskMAXPROPS); return 1;	}
    if  ( PPprop_FULL_COUNT > PROPmaskMAXPROPS )
	{ LLDEB(PPprop_FULL_COUNT,PROPmaskMAXPROPS); return 1;	}
    if  ( CLprop_COUNT > PROPmaskMAXPROPS )
	{ LLDEB(CLprop_COUNT,PROPmaskMAXPROPS); return 1;	}
    if  ( DPprop_COUNT > PROPmaskMAXPROPS )
	{ LLDEB(DPprop_COUNT,PROPmaskMAXPROPS); return 1;	}
    if  ( TFPprop_FULL_COUNT > PROPmaskMAXPROPS )
	{ LLDEB(TFPprop_FULL_COUNT,PROPmaskMAXPROPS); return 1;	}

    /*  2  */
    TEDResources.tarShowTableGridInt= 0;
    TEDResources.tarShadingMeshPointsDouble= -1;

    TEDResources.tarFindPattern= (const char *)0;
    TEDResources.tarFindRegex= 0;

    tedGetNamedPictures( ea );

    return appMain( ea, argc, argv );
    }

