/************************************************************************/
/*									*/
/*  The 'Format Tool':							*/
/*									*/
/*  An inpector with pages for the different formatting properties of	*/
/*  the document.							*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<stddef.h>

#   include	"tedFormatTool.h"
#   include	"tedFontTool.h"
#   include	"tedToolFront.h"
#   include	"tedAppResources.h"
#   include	"tedAppFront.h"
#   include	"tedDocument.h"
#   include	<docTreeType.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Resources for the format tool.					*/
/*									*/
/************************************************************************/

typedef struct TedFormatToolResources
    {
    InspectorSubjectResources	tttrSubjectResources[TEDtsi_COUNT];

    TablePageResources			tttrTableToolResources;
    RowPageResources			tttrRowToolResources;
    ColumnPageResources			tttrColumnToolResources;
    CellPageResources			tttrCellToolResources;
    ParagraphLayoutPageResources	tttrParaLayoutToolResources;
    TabsPageResources			tttrTabsToolResources;
    ParagraphOrnamentsPageResources	tttrParaOrnamentsToolResources;
    TextOrnamentsPageResources		tttrTextOrnamentsToolResources;
    ListsPageResources			tttrListsToolResources;
    SectionPageResources		tttrSectToolResources;
    PageLayoutPageResources		tttrPageLayoutToolResources;
    HeaderFooterPageResources		tttrHeadFootToolResources;
    ThisNotePageResources		tttrThisNoteToolResources;
    NotesPageResources			tttrNotesToolResources;
    AppFontToolResources		tttrFontToolResources;
    RgbChooserPageResources		tttrRgbChooserToolResources;
    LinkToolResources			tttrLinkToolResources;
    BookmarkPageResources		tttrBookmarkToolResources;
    ImagePageResources			tttrImageToolResources;
    FramePageResources			tttrFrameToolResources;
    TocPageResources			tttrTocToolResources;
    SymbolPickerResources		tttrSymbolPickerResources;
    FindToolResources			tttrFindToolResources;
    SpellToolResources			tttrSpellToolResources;
    } TedFormatToolResources;

static void tedFormatFillPages(	const TedFormatToolResources *	tftr,
				TedFormatTool *			tft,
				AppInspector *			ai )
    {
#   if USE_FONT_TOOL
    appFontToolFillPage( &(tft->tftFontTool),
			    &(tftr->tttrFontToolResources),
			    TEDtsiFONT,
			    &(ai->aiSubjects[TEDtsiFONT]),
			    ai->aiSubjects[TEDtsiFONT].isPage,
			    &(tftr->tttrSubjectResources[TEDtsiFONT]) );
#   endif

    tedFormatFillTextOrnamentsPage( &(tft->tftTextOrnamentsTool),
			    &(tftr->tttrTextOrnamentsToolResources),
			    ai, TEDtsiTEXT_ORN,
			    &(ai->aiSubjects[TEDtsiTEXT_ORN]),
			    ai->aiSubjects[TEDtsiTEXT_ORN].isPage,
			    &(tftr->tttrSubjectResources[TEDtsiTEXT_ORN]) );

    tedFormatFillParagraphLayoutPage( &(tft->tftParagraphLayoutTool),
			    &(tftr->tttrParaLayoutToolResources),
			    &(ai->aiSubjects[TEDtsiPARA_LAY]),
			    ai->aiSubjects[TEDtsiPARA_LAY].isPage,
			    &(tftr->tttrSubjectResources[TEDtsiPARA_LAY]) );

#   if USE_TABS_TOOL
    tedFormatFillTabsPage( &(tft->tftTabsTool),
			    &(tftr->tttrTabsToolResources),
			    &(ai->aiSubjects[TEDtsiTABS]),
			    ai->aiSubjects[TEDtsiTABS].isPage,
			    &(tftr->tttrSubjectResources[TEDtsiTABS]) );
#   endif

    tedFormatFillParagraphOrnamentsPage( &(tft->tftParagraphOrnamentsTool),
			    &(tftr->tttrParaOrnamentsToolResources),
			    ai, TEDtsiPARA_ORN,
			    &(ai->aiSubjects[TEDtsiPARA_ORN]),
			    ai->aiSubjects[TEDtsiPARA_ORN].isPage,
			    &(tftr->tttrSubjectResources[TEDtsiPARA_ORN]) );

#   if USE_LIST_TOOL
    tedFormatFillListsPage( &(tft->tftListsTool),
			    &(tftr->tttrListsToolResources),
			    &(ai->aiSubjects[TEDtsiLISTS]),
			    ai->aiSubjects[TEDtsiLISTS].isPage,
			    &(tftr->tttrSubjectResources[TEDtsiLISTS]) );
#   endif

    tedFormatFillSectionPage( &(tft->tftSectionTool),
			    &(tftr->tttrSectToolResources),
			    &(ai->aiSubjects[TEDtsiSECT]),
			    ai->aiSubjects[TEDtsiSECT].isPage,
			    &(tftr->tttrSubjectResources[TEDtsiSECT]) );

    tedFormatFillTablePage( &(tft->tftTableTool),
			    &(tftr->tttrTableToolResources),
			    &(ai->aiSubjects[TEDtsiTABLE]),
			    ai->aiSubjects[TEDtsiTABLE].isPage,
			    &(tftr->tttrSubjectResources[TEDtsiTABLE]) );

    tedFormatFillRowPage( &(tft->tftRowTool),
			    &(tftr->tttrRowToolResources),
			    ai, TEDtsiROW,
			    &(ai->aiSubjects[TEDtsiROW]),
			    ai->aiSubjects[TEDtsiROW].isPage,
			    &(tftr->tttrSubjectResources[TEDtsiROW]) );

    tedFormatFillColumnPage( &(tft->tftColumnTool),
			    &(tftr->tttrColumnToolResources),
			    ai, TEDtsiCOLUMN,
			    &(ai->aiSubjects[TEDtsiCOLUMN]),
			    ai->aiSubjects[TEDtsiCOLUMN].isPage,
			    &(tftr->tttrSubjectResources[TEDtsiCOLUMN]) );

    tedFormatFillCellPage( &(tft->tftCellTool),
			    &(tftr->tttrCellToolResources),
			    ai, TEDtsiCELL,
			    &(ai->aiSubjects[TEDtsiCELL]),
			    ai->aiSubjects[TEDtsiCELL].isPage,
			    &(tftr->tttrSubjectResources[TEDtsiCELL]) );

#   if USE_PAGE_TOOL
    tedFormatFillPageLayoutPage( &(tft->tftPageLayoutTool),
			    &(tftr->tttrPageLayoutToolResources),
			    &(ai->aiSubjects[TEDtsiPAGE]),
			    ai->aiSubjects[TEDtsiPAGE].isPage,
			    &(tftr->tttrSubjectResources[TEDtsiPAGE]) );
#   endif

    tedFormatFillHeaderFooterPage( &(tft->tftHeaderFooterTool),
			    &(tftr->tttrHeadFootToolResources),
			    &(ai->aiSubjects[TEDtsiHEADFOOT]),
			    ai->aiSubjects[TEDtsiHEADFOOT].isPage,
			    &(tftr->tttrSubjectResources[TEDtsiHEADFOOT]) );

    tedFormatFillThisNotePage( &(tft->tftThisNoteTool),
			    &(tftr->tttrThisNoteToolResources),
			    &(ai->aiSubjects[TEDtsiTHIS_NOTE]),
			    ai->aiSubjects[TEDtsiTHIS_NOTE].isPage,
			    &(tftr->tttrSubjectResources[TEDtsiTHIS_NOTE]) );

    tedFormatFillNotesPage( &(tft->tftFootNotesTool),
			    &(tftr->tttrNotesToolResources),
			    &(ai->aiSubjects[TEDtsiFOOT_NOTES]),
			    ai->aiSubjects[TEDtsiFOOT_NOTES].isPage,
			    &(tftr->tttrSubjectResources[TEDtsiFOOT_NOTES]),
			    DOCinFOOTNOTE );
    tedFormatFillNotesPage( &(tft->tftEndNotesTool),
			    &(tftr->tttrNotesToolResources),
			    &(ai->aiSubjects[TEDtsiEND_NOTES]),
			    ai->aiSubjects[TEDtsiEND_NOTES].isPage,
			    &(tftr->tttrSubjectResources[TEDtsiEND_NOTES]),
			    DOCinENDNOTE );

#   if USE_LINK_TOOL
    tedFillLinkTool( &(tft->tftLinkTool),
			    &(tftr->tttrLinkToolResources),
			    &(ai->aiSubjects[TEDtsiLINK]),
			    ai->aiSubjects[TEDtsiLINK].isPage,
			    &(tftr->tttrSubjectResources[TEDtsiLINK]) );
#   endif

#   if USE_BOOKMARK_TOOL
    tedFillBookmarkTool( &(tft->tftBookmarkTool),
			    &(tftr->tttrBookmarkToolResources),
			    &(ai->aiSubjects[TEDtsiBOOKMARK]),
			    ai->aiSubjects[TEDtsiBOOKMARK].isPage,
			    &(tftr->tttrSubjectResources[TEDtsiBOOKMARK]) );
#   endif

    tedFormatFillImagePage( &(tft->tftImageTool),
			    &(tftr->tttrImageToolResources),
			    &(ai->aiSubjects[TEDtsiIMAGE]),
			    ai->aiSubjects[TEDtsiIMAGE].isPage,
			    &(tftr->tttrSubjectResources[TEDtsiIMAGE]) );

#   if USE_FRAME_TOOL
    tedFormatFillFramePage( &(tft->tftFrameTool),
			    &(tftr->tttrFrameToolResources),
			    ai, TEDtsiFRAME,
			    &(ai->aiSubjects[TEDtsiFRAME]),
			    ai->aiSubjects[TEDtsiFRAME].isPage,
			    &(tftr->tttrSubjectResources[TEDtsiFRAME]) );
#   endif

    tedFormatFillTocPage( &(tft->tftTocTool),
			    &(tftr->tttrTocToolResources),
			    ai, TEDtsiTOC,
			    &(ai->aiSubjects[TEDtsiTOC]),
			    ai->aiSubjects[TEDtsiTOC].isPage,
			    &(tftr->tttrSubjectResources[TEDtsiTOC]) );

    appFillSymbolPicker( &(tft->tftSymbolPicker),
			    &(tftr->tttrSymbolPickerResources),
			    ai, TEDtsiSYMBOL,
			    &(ai->aiSubjects[TEDtsiSYMBOL]),
			    ai->aiSubjects[TEDtsiSYMBOL].isPage,
			    &(tftr->tttrSubjectResources[TEDtsiSYMBOL]) );

#   if USE_FIND_TOOL
    tedFillFindTool( &(tft->tftFindTool),
			    &(tftr->tttrFindToolResources),
			    ai, TEDtsiFIND,
			    &(ai->aiSubjects[TEDtsiFIND]),
			    ai->aiSubjects[TEDtsiFIND].isPage,
			    &(tftr->tttrSubjectResources[TEDtsiFIND]) );
#   endif

#   if USE_SPELL_TOOL
    tedFillSpellTool( &(tft->tftSpellTool),
			    &(tftr->tttrSpellToolResources),
			    ai, TEDtsiSPELL,
			    &(ai->aiSubjects[TEDtsiSPELL]),
			    ai->aiSubjects[TEDtsiSPELL].isPage,
			    &(tftr->tttrSubjectResources[TEDtsiSPELL]) );
#   endif

    /**/
    appRgbChooserPageFillPage( &(tft->tftRgbPage),
			    &(tftr->tttrRgbChooserToolResources),
			    &(ai->aiSubjects[TEDtsiRGB]),
			    ai->aiSubjects[TEDtsiRGB].isPage,
			    &(tftr->tttrSubjectResources[TEDtsiRGB]) );

    /**/
    appInspectorSetRgbPage( ai, (void *)&(tft->tftRgbPage), TEDtsiRGB );

    return;
    }

/************************************************************************/
/*									*/
/*  The user destroys the format tool.					*/
/*									*/
/************************************************************************/

static void tedDestroyFormatTool(	void *	voidtft )
    {
    TedFormatTool *	tft= (TedFormatTool *)voidtft;

    EditApplication *	ea= tft->tftApplication;
    TedAppResources *	tar= (TedAppResources *)ea->eaResourceData;

    tedFormatCleanTableTool( &(tft->tftTableTool) );
    tedCleanRowTool( &(tft->tftRowTool) );
    tedCleanColumnTool( &(tft->tftColumnTool) );
    tedCleanCellTool( &(tft->tftCellTool) );
    tedFormatCleanParaLayoutTool( &(tft->tftParagraphLayoutTool) );
#   if USE_TABS_TOOL
    tedFormatCleanParaTabsTool( &(tft->tftTabsTool) );
#   endif
    tedCleanParaOrnamentsTool( &(tft->tftParagraphOrnamentsTool) );
#   if USE_LIST_TOOL
    tedFormatCleanListsTool( &(tft->tftListsTool) );
#   endif
    tedFormatCleanSectionTool( &(tft->tftSectionTool) );
#   if USE_PAGE_TOOL
    tedFormatCleanPageLayoutTool( &(tft->tftPageLayoutTool) );
#   endif
    tedFormatCleanHeaderFooterTool( &(tft->tftHeaderFooterTool) );
    tedFormatCleanThisNoteTool( &(tft->tftThisNoteTool) );
    tedFormatCleanNotesTool( &(tft->tftFootNotesTool) );
    tedFormatCleanNotesTool( &(tft->tftEndNotesTool) );
#   if USE_FONT_TOOL
    appFontChooserCleanPage( &(tft->tftFontTool) );
#   endif
    tedCleanTextOrnamentsTool( &(tft->tftTextOrnamentsTool) );
#   if USE_LINK_TOOL
    tedFormatCleanLinkTool( &(tft->tftLinkTool) );
#   endif
#   if USE_BOOKMARK_TOOL
    tedFormatCleanBookmarkTool( &(tft->tftBookmarkTool) );
#   endif
    tedCleanImageTool( &(tft->tftImageTool) );
#   if USE_FRAME_TOOL
    tedCleanFrameTool( &(tft->tftFrameTool) );
#   endif
    tedCleanTocTool( &(tft->tftTocTool) );
    appCleanSymbolPicker( &(tft->tftSymbolPicker) );
#   if USE_FIND_TOOL
    appCleanFindTool( &(tft->tftFindTool) );
#   endif
#   if USE_SPELL_TOOL
    appCleanSpellTool( &(tft->tftSpellTool) );
#   endif

    appRgbChooserPageCleanPage( &(tft->tftRgbPage) );

    free( tft );

    tar->tarInspector= (void *)0;

    return;
    }

/************************************************************************/
/*									*/
/*  Called when the user switches to another page of the format tool.	*/
/*									*/
/************************************************************************/

static void tedFormatToolNotifySubject(	AppInspector *		ai,
					int			subjectFrom,
					int			subjectTo )
    {
    if  ( subjectFrom == TEDtsiRGB && subjectTo != TEDtsiRGB )
	{
	const int	enabled= 0;

	appEnableInspectorSubject( ai, TEDtsiRGB, enabled );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  make/show the format tool.						*/
/*									*/
/************************************************************************/

void tedShowFormatTool(	APP_WIDGET		option,
			EditApplication *	ea )
    {
    AppInspector *			ai;
    TedFormatTool *			tft;

    static TedFormatToolResources	tftr;
    static int				gotResources;

    TedAppResources *			tar;

    tar= (TedAppResources *)ea->eaResourceData;

    if  ( tar->tarInspector )
	{
	appShowShellWidget( tar->tarInspector->aiApplication,
					    tar->tarInspector->aiTopWidget );

	return;
	}

    if  ( ! gotResources )
	{
#	if USE_FONT_TOOL
	tedFontToolGetResourceTable( ea,
				&(tftr.tttrFontToolResources),
				&(tftr.tttrSubjectResources[TEDtsiFONT]) );
#	endif

	tedFormatToolGetTextOrnamentsResourceTable( ea,
				&(tftr.tttrTextOrnamentsToolResources),
				&(tftr.tttrSubjectResources[TEDtsiTEXT_ORN]) );

	tedFormatToolGetParaLayoutResourceTable( ea,
				&(tftr.tttrParaLayoutToolResources),
				&(tftr.tttrSubjectResources[TEDtsiPARA_LAY]) );

#	if USE_TABS_TOOL
	tedFormatToolGetTabsResourceTable( ea,
				&(tftr.tttrTabsToolResources),
				&(tftr.tttrSubjectResources[TEDtsiTABS]) );
#	endif

	tedFormatToolGetParaOrnamentsResourceTable( ea,
				&(tftr.tttrParaOrnamentsToolResources),
				&(tftr.tttrSubjectResources[TEDtsiPARA_ORN]) );

#	if USE_LIST_TOOL
	tedFormatToolGetListsResourceTable( ea,
				&(tftr.tttrListsToolResources),
				&(tftr.tttrSubjectResources[TEDtsiLISTS]) );
#	endif

	tedFormatToolGetSectResourceTable( ea,
				&(tftr.tttrSectToolResources),
				&(tftr.tttrSubjectResources[TEDtsiSECT]) );

	tedFormatToolGetTableResourceTable( ea,
				&(tftr.tttrTableToolResources),
				&(tftr.tttrSubjectResources[TEDtsiTABLE]) );

	tedFormatToolGetRowResourceTable( ea,
				&(tftr.tttrRowToolResources),
				&(tftr.tttrSubjectResources[TEDtsiROW]) );

	tedFormatToolGetColumnResourceTable( ea,
				&(tftr.tttrColumnToolResources),
				&(tftr.tttrSubjectResources[TEDtsiCOLUMN]) );

	tedFormatToolGetCellResourceTable( ea,
				&(tftr.tttrCellToolResources),
				&(tftr.tttrSubjectResources[TEDtsiCELL]) );

#	if USE_PAGE_TOOL
	appPageLayoutPageGetResourceTable( ea,
				&(tftr.tttrPageLayoutToolResources),
				&(tftr.tttrSubjectResources[TEDtsiPAGE]) );
#	endif

	tedFormatToolGetHeaderFooterResourceTable( ea,
				&(tftr.tttrHeadFootToolResources),
				&(tftr.tttrSubjectResources[TEDtsiHEADFOOT]) );

	tedFormatToolGetThisNoteResourceTable( ea,
			    &(tftr.tttrThisNoteToolResources),
			    &(tftr.tttrSubjectResources[TEDtsiTHIS_NOTE]) );

	tedFormatToolGetNotesResourceTable( ea,
			    &(tftr.tttrNotesToolResources),
			    &(tftr.tttrSubjectResources[TEDtsiFOOT_NOTES]) );

	tftr.tttrSubjectResources[TEDtsiEND_NOTES]=
				tftr.tttrSubjectResources[TEDtsiFOOT_NOTES];
	tftr.tttrSubjectResources[TEDtsiFOOT_NOTES].isrSubjectName=
				tftr.tttrNotesToolResources.nprFootnotesText;
	tftr.tttrSubjectResources[TEDtsiEND_NOTES].isrSubjectName=
				tftr.tttrNotesToolResources.nprEndnotesText;

#	if USE_LINK_TOOL
	tedLinkToolGetResourceTable( ea,
				&(tftr.tttrLinkToolResources),
				&(tftr.tttrSubjectResources[TEDtsiLINK]) );
#	endif

#	if USE_BOOKMARK_TOOL
	tedBookmarkToolGetResourceTable( ea,
				&(tftr.tttrBookmarkToolResources),
				&(tftr.tttrSubjectResources[TEDtsiBOOKMARK]) );
#	endif

	tedImageToolGetResourceTable( ea,
				&(tftr.tttrImageToolResources),
				&(tftr.tttrSubjectResources[TEDtsiIMAGE]) );

#	if USE_FRAME_TOOL
	tedFrameToolGetResourceTable( ea,
				&(tftr.tttrFrameToolResources),
				&(tftr.tttrSubjectResources[TEDtsiFRAME]) );
#	endif

	tedTocToolGetResourceTable( ea,
				&(tftr.tttrTocToolResources),
				&(tftr.tttrSubjectResources[TEDtsiTOC]) );

	appSymbolPickerGetResourceTable( ea,
				&(tftr.tttrSymbolPickerResources),
				&(tftr.tttrSubjectResources[TEDtsiSYMBOL]) );

#	if USE_FIND_TOOL
	appFindToolGetResourceTable( ea,
				&(tftr.tttrFindToolResources),
				&(tftr.tttrSubjectResources[TEDtsiFIND]) );
#	endif

#	if USE_SPELL_TOOL
	appSpellToolGetResourceTable( ea,
				&(tftr.tttrSpellToolResources),
				&(tftr.tttrSubjectResources[TEDtsiSPELL]) );
#	endif

	appRgbChooserPageGetResourceTable( ea,
				&(tftr.tttrRgbChooserToolResources),
				&(tftr.tttrSubjectResources[TEDtsiRGB]) );

	gotResources= 1;
	}

    tft= (TedFormatTool *)malloc( sizeof(TedFormatTool) );
    if  ( ! tft )
	{ XDEB(tft); return;	}

    tft->tftApplication= ea;

    /******/
    tedInitColumnTool( &(tft->tftColumnTool) );
    tedInitCellTool( &(tft->tftCellTool) );
    tedInitParaOrnamentsTool( &(tft->tftParagraphOrnamentsTool) );
    tedInitRowTool( &(tft->tftRowTool) );
    tedInitTextOrnamentsTool( &(tft->tftTextOrnamentsTool) );

    tedFormatInitThisNoteTool( &(tft->tftThisNoteTool) );
    tedFormatInitNotesTool( &(tft->tftFootNotesTool) );
    tedFormatInitNotesTool( &(tft->tftEndNotesTool) );

#   if USE_LINK_TOOL
    tedInitLinkTool( &(tft->tftLinkTool) );
#   endif
#   if USE_BOOKMARK_TOOL
    tedInitBookmarkTool( &(tft->tftBookmarkTool) );
#   endif
    tedInitImageTool( &(tft->tftImageTool) );
#   if USE_FRAME_TOOL
    tedInitFrameTool( &(tft->tftFrameTool) );
#   endif
    tedInitTocTool( &(tft->tftTocTool) );
    appInitSymbolPicker( &(tft->tftSymbolPicker) );
#   if USE_FIND_TOOL
    appInitFindTool( &(tft->tftFindTool) );
#   endif
#   if USE_SPELL_TOOL
    appInitSpellTool( &(tft->tftSpellTool) );
#   endif

    /******/
    tft->tftTableTool.ttApplication= ea;
    tft->tftRowTool.rtApplication= ea;
    tft->tftColumnTool.ctApplication= ea;
    tft->tftCellTool.ctApplication= ea;
    tft->tftParagraphLayoutTool.ptApplication= ea;
#   if USE_TABS_TOOL
    tft->tftTabsTool.ttApplication= ea;
#   endif
    tft->tftParagraphOrnamentsTool.potApplication= ea;
#   if USE_LIST_TOOL
    tft->tftListsTool.ltApplication= ea;
#   endif
    tft->tftSectionTool.stApplication= ea;
#   if USE_PAGE_TOOL
    tft->tftPageLayoutTool.pltApplication= ea;
#   endif
    tft->tftHeaderFooterTool.hftApplication= ea;
    tft->tftThisNoteTool.tntApplication= ea;
    tft->tftFootNotesTool.ntApplication= ea;
    tft->tftEndNotesTool.ntApplication= ea;
#   if USE_FONT_TOOL
    tft->tftFontTool.afcApplication= ea;
#   endif
    tft->tftTextOrnamentsTool.totApplication= ea;
#   if USE_LINK_TOOL
    tft->tftLinkTool.ltApplication= ea;
#   endif
#   if USE_BOOKMARK_TOOL
    tft->tftBookmarkTool.btApplication= ea;
#   endif
    tft->tftImageTool.itApplication= ea;
#   if USE_FRAME_TOOL
    tft->tftFrameTool.ftApplication= ea;
#   endif
    tft->tftTocTool.ttApplication= ea;
    tft->tftSymbolPicker.spApplication= ea;
#   if USE_FIND_TOOL
    tft->tftFindTool.aftApplication= ea;
#   endif
#   if USE_SPELL_TOOL
    tft->tftSpellTool.astApplication= ea;
#   endif
    tft->tftRgbPage.rcpApplication= ea;

    ai= appMakeInspector( ea, option,
				    tftr.tttrSubjectResources, TEDtsi_COUNT,
				    tedDestroyFormatTool, (void *)tft );
    if  ( ! ai )
	{
	tedDestroyFormatTool( tft );
	return;
	}

    tft->tftInspector= ai;
    ai->aiNotifySubject= tedFormatToolNotifySubject;

    /******/
    tft->tftTableTool.ttInspector= ai;
    tft->tftRowTool.rtInspector= ai;
    tft->tftColumnTool.ctInspector= ai;
    tft->tftCellTool.ctInspector= ai;
    tft->tftParagraphLayoutTool.ptInspector= ai;
#   if USE_TABS_TOOL
    tft->tftTabsTool.ttInspector= ai;
#   endif
    tft->tftParagraphOrnamentsTool.potInspector= ai;
#   if USE_LIST_TOOL
    tft->tftListsTool.ltInspector= ai;
#   endif
    tft->tftSectionTool.stInspector= ai;
#   if USE_PAGE_TOOL
    tft->tftPageLayoutTool.pltInspector= ai;
#   endif
    tft->tftHeaderFooterTool.hftInspector= ai;
    tft->tftThisNoteTool.tntInspector= ai;
    tft->tftFootNotesTool.ntInspector= ai;
    tft->tftEndNotesTool.ntInspector= ai;
#   if USE_FONT_TOOL
    tft->tftFontTool.afcInspector= ai;
#   endif
    tft->tftTextOrnamentsTool.totInspector= ai;
#   if USE_LINK_TOOL
    tft->tftLinkTool.ltInspector= ai;
#   endif
#   if USE_BOOKMARK_TOOL
    tft->tftBookmarkTool.btInspector= ai;
#   endif
    tft->tftImageTool.itInspector= ai;
#   if USE_FRAME_TOOL
    tft->tftFrameTool.ftInspector= ai;
#   endif
    tft->tftTocTool.ttInspector= ai;
    tft->tftSymbolPicker.spInspector= ai;
#   if USE_FIND_TOOL
    tft->tftFindTool.aftInspector= ai;
#   endif
#   if USE_SPELL_TOOL
    tft->tftSpellTool.astInspector= ai;
#   endif
    tft->tftRgbPage.rcpInspector= ai;

    /******/

    tedFormatFillPages( &tftr, tft, ai );

    /******/

#   if USE_FONT_TOOL
    tft->tftFontTool.afcSetFont= tedAppFontToolSet;
#   endif

    tft->tftSymbolPicker.spTarget= ea;
    tft->tftSymbolPicker.spInsert= tedSymbolPickerInsert;

    /******/
    /*tedFormatFillTableChoosers()*/
    tedRowToolFillChoosers( &(tft->tftRowTool),
					    &(tftr.tttrRowToolResources) );
    tedColumnToolFillChoosers( &(tft->tftColumnTool),
					&(tftr.tttrColumnToolResources) );
    tedCellToolFillChoosers( &(tft->tftCellTool),
					&(tftr.tttrCellToolResources) );
    tedParaLayoutToolFillChoosers( &(tft->tftParagraphLayoutTool),
				    &(tftr.tttrParaLayoutToolResources) );
#   if USE_TABS_TOOL
    tedTabsToolFillChoosers( &(tft->tftTabsTool) );
#   endif
    tedParaOrnamentsToolFillChoosers( &(tft->tftParagraphOrnamentsTool),
				    &(tftr.tttrParaOrnamentsToolResources) );
#   if USE_LIST_TOOL
    tedFormatFillListChoosers( &(tft->tftListsTool) );
#   endif
    tedFormatFillSectionChoosers( &(tft->tftSectionTool),
					    &(tftr.tttrSectToolResources) );
#   if USE_PAGE_TOOL
    appPageLayoutPageFillChoosers( &(tft->tftPageLayoutTool),
					&(tftr.tttrPageLayoutToolResources) );
#   endif
    tedFormatFillHeaderFooterChoosers( &(tft->tftHeaderFooterTool),
					&(tftr.tttrHeadFootToolResources) );
#   if 0
    tedFormatFillThisNoteChoosers( &(tft->tftThisNoteTool),
					&(tftr.tttrNotesToolResources) );
#   endif
    tedFormatFillNotesChoosers( &(tft->tftFootNotesTool),
					&(tftr.tttrNotesToolResources) );
    tedFormatFillNotesChoosers( &(tft->tftEndNotesTool),
					&(tftr.tttrNotesToolResources) );
#   if  USE_FONT_TOOL
    appFontToolFillChoosers( &(tft->tftFontTool),
					&(tftr.tttrFontToolResources) );
#   endif
    tedFormatFillTextOrnamentsChoosers( &(tft->tftTextOrnamentsTool),
				    &(tftr.tttrTextOrnamentsToolResources) );
#   if USE_LINK_TOOL
    tedLinkToolFillChoosers( &(tft->tftLinkTool) );
#   endif
#   if USE_BOOKMARK_TOOL
    tedBookmarkToolFillChoosers( &(tft->tftBookmarkTool) );
#   endif
    /*tedImageToolFillChoosers()*/
#   if USE_FRAME_TOOL
    tedFrameToolFillChoosers( &(tft->tftFrameTool),
					&(tftr.tttrFrameToolResources) );
#   endif
    tedTocToolFillChoosers( &(tft->tftTocTool),
					&(tftr.tttrTocToolResources) );
    appSymbolPickerFillChoosers( &(tft->tftSymbolPicker),
					&(tftr.tttrSymbolPickerResources) );
#   if USE_SPELL_TOOL
    appSpellToolFillChoosers( &(tft->tftSpellTool),
					&(tftr.tttrSpellToolResources) );
#   endif
    /******/

    appFinishInspector( ai );

    tedFormatFinishRowPage( &(tft->tftRowTool),
					    &(tftr.tttrRowToolResources) );
    tedFinishParaLayoutPage( &(tft->tftParagraphLayoutTool),
					&(tftr.tttrParaLayoutToolResources) );
#   if USE_TABS_TOOL
    tedFormatFinishTabsPage( &(tft->tftTabsTool) );
#   endif
    tedFormatFinishParaOrnamentsPage( &(tft->tftParagraphOrnamentsTool),
				    &(tftr.tttrParaOrnamentsToolResources) );
#   if USE_LIST_TOOL
    tedFormatFinishListPage( &(tft->tftListsTool) );
#   endif
    tedFormatFinishSectionPage( &(tft->tftSectionTool),
					    &(tftr.tttrSectToolResources) );
#   if USE_PAGE_TOOL
    tedFormatFinishPageLayoutPage( &(tft->tftPageLayoutTool),
					&(tftr.tttrPageLayoutToolResources) );
#   endif
    tedFormatFinishHeaderFooterPage( &(tft->tftHeaderFooterTool),
					&(tftr.tttrHeadFootToolResources) );
    tedFormatFinishColumnPage( &(tft->tftColumnTool),
					&(tftr.tttrColumnToolResources) );
    tedFormatFinishCellPage( &(tft->tftCellTool),
					&(tftr.tttrCellToolResources) );
#   if 0
    tedFormatFinishThisNotePage( &(tft->tftThisNoteTool),
					&(tftr.tttrNotesToolResources) );
#   endif

    tedFormatFinishNotesPage( &(tft->tftFootNotesTool),
					&(tftr.tttrNotesToolResources) );
    tedFormatFinishNotesPage( &(tft->tftEndNotesTool),
					&(tftr.tttrNotesToolResources) );
#   if USE_FONT_TOOL
    appFontToolFinishPage( &(tft->tftFontTool),
					&(tftr.tttrFontToolResources) );
#   endif
    tedFinishTextOrnamentsPage( &(tft->tftTextOrnamentsTool),
				    &(tftr.tttrTextOrnamentsToolResources) );
#   if USE_LINK_TOOL
    tedFinishLinkTool( &(tft->tftLinkTool) );
#   endif
#   if USE_BOOKMARK_TOOL
    tedFinishBookmarkTool( &(tft->tftBookmarkTool) );
#   endif
#   if USE_FRAME_TOOL
    tedFinishFrameTool( &(tft->tftFrameTool),
					&(tftr.tttrFrameToolResources) );
#   endif
    tedFinishTocTool( &(tft->tftTocTool),
					&(tftr.tttrTocToolResources) );
    appFinishSymbolPicker( &(tft->tftSymbolPicker),
					&(tftr.tttrSymbolPickerResources) );
#   if USE_SPELL_TOOL
    appFinishSpellTool( &(tft->tftSpellTool),
					&(tftr.tttrSpellToolResources) );
#   endif

    appRgbChooserPageFinishPage( &(tft->tftRgbPage),
					&(tftr.tttrRgbChooserToolResources) );

    tar->tarInspector= ai;

    return;
    }

/************************************************************************/
/*									*/
/*  Ask the table tool to adapt itself to the selection.		*/
/*									*/
/************************************************************************/

static void tedFormatRefreshToolPages(
				int *				enabled,
				int *				prefs,
				TedFormatTool *			tft,
				AppInspector *			ai,
				EditDocument *			ed,
				DocumentTree *			ei,
				const DocumentSelection *	ds,
				const SelectionGeometry *	sg,
				const SelectionDescription *	sd,
				const unsigned char *		cmdEnabled )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;

#   if USE_FONT_TOOL
    tedRefreshFontTool( &(tft->tftFontTool),
				    enabled+ TEDtsiFONT,
				    prefs+ TEDtsiFONT,
				    ai->aiSubjects+ TEDtsiFONT,
				    ds, sd, bd, ed->edDocumentId, cmdEnabled );
#   endif

    tedRefreshTextOrnamentsTool( &(tft->tftTextOrnamentsTool),
				    enabled+ TEDtsiTEXT_ORN,
				    prefs+ TEDtsiTEXT_ORN,
				    ai->aiSubjects+ TEDtsiTEXT_ORN,
				    ed, ds, sg, sd, cmdEnabled );

    tedRefreshParaLayoutTool(  &(tft->tftParagraphLayoutTool),
				    enabled+ TEDtsiPARA_LAY,
				    prefs+ TEDtsiPARA_LAY,
				    ai->aiSubjects+ TEDtsiPARA_LAY,
				    ds, sg, sd, cmdEnabled );

#   if USE_TABS_TOOL
    tedRefreshTabsTool( &(tft->tftTabsTool),
				    enabled+ TEDtsiTABS,
				    prefs+ TEDtsiTABS,
				    ai->aiSubjects+ TEDtsiTABS,
				    ds, sg, sd, bd, cmdEnabled );
#   endif

    tedRefreshParaOrnamentsTool(  &(tft->tftParagraphOrnamentsTool),
				    enabled+ TEDtsiPARA_ORN,
				    prefs+ TEDtsiPARA_ORN,
				    ai->aiSubjects+ TEDtsiPARA_ORN,
				    ds, sg, sd, cmdEnabled );

#   if USE_LIST_TOOL
    tedRefreshListTool(  &(tft->tftListsTool),
				    enabled+ TEDtsiLISTS,
				    prefs+ TEDtsiLISTS,
				    ai->aiSubjects+ TEDtsiLISTS,
				    ds, sd, bd, cmdEnabled );
#   endif

    tedRefreshTableTool( &(tft->tftTableTool),
				    enabled+ TEDtsiTABLE,
				    prefs+ TEDtsiTABLE,
				    ai->aiSubjects+ TEDtsiTABLE,
				    ds, sg, sd, bd, cmdEnabled );

    tedRefreshColumnTool( &(tft->tftColumnTool),
				    enabled+ TEDtsiCOLUMN,
				    prefs+ TEDtsiCOLUMN,
				    ai->aiSubjects+ TEDtsiCOLUMN,
				    ds, sg, sd, cmdEnabled );

    tedRefreshRowTool( &(tft->tftRowTool),
				    enabled+ TEDtsiROW,
				    prefs+ TEDtsiROW,
				    ai->aiSubjects+ TEDtsiROW,
				    ds, sg, sd, bd, cmdEnabled );

    tedRefreshCellTool( &(tft->tftCellTool),
				    enabled+ TEDtsiCELL,
				    prefs+ TEDtsiCELL,
				    ai->aiSubjects+ TEDtsiCELL,
				    ds, sd, bd, cmdEnabled );

    tedRefreshSectionTool( &(tft->tftSectionTool),
				    enabled+ TEDtsiSECT,
				    prefs+ TEDtsiSECT,
				    ai->aiSubjects+ TEDtsiSECT,
				    ds, sd, bd, cmdEnabled );

#   if USE_PAGE_TOOL
    tedRefreshPageLayoutTool( &(tft->tftPageLayoutTool),
				    enabled+ TEDtsiPAGE,
				    prefs+ TEDtsiPAGE,
				    ai->aiSubjects+ TEDtsiPAGE,
				    ds, sd, bd, cmdEnabled );
#   endif

    tedRefreshHeaderFooterTool( &(tft->tftHeaderFooterTool),
				    enabled+ TEDtsiHEADFOOT,
				    prefs+ TEDtsiHEADFOOT,
				    ai->aiSubjects+ TEDtsiHEADFOOT,
				    ds, sd, sg, bd, cmdEnabled );

    tedRefreshThisNoteTool( &(tft->tftThisNoteTool),
				    enabled+ TEDtsiTHIS_NOTE,
				    prefs+ TEDtsiTHIS_NOTE,
				    ai->aiSubjects+ TEDtsiTHIS_NOTE,
				    ds, sd, bd, cmdEnabled );
    tedRefreshNotesTool( &(tft->tftFootNotesTool),
				    enabled+ TEDtsiFOOT_NOTES,
				    prefs+ TEDtsiFOOT_NOTES,
				    ai->aiSubjects+ TEDtsiFOOT_NOTES,
				    ds, sd, bd, cmdEnabled );
    tedRefreshNotesTool( &(tft->tftEndNotesTool),
				    enabled+ TEDtsiEND_NOTES,
				    prefs+ TEDtsiEND_NOTES,
				    ai->aiSubjects+ TEDtsiEND_NOTES,
				    ds, sd, bd, cmdEnabled );

#   if USE_LINK_TOOL
    tedRefreshLinkTool( &(tft->tftLinkTool),
				    enabled+ TEDtsiLINK,
				    prefs+ TEDtsiLINK,
				    ai->aiSubjects+ TEDtsiLINK,
				    ds, sd, bd, cmdEnabled );
#   endif

#   if USE_BOOKMARK_TOOL
    tedRefreshBookmarkTool( &(tft->tftBookmarkTool),
				    enabled+ TEDtsiBOOKMARK,
				    prefs+ TEDtsiBOOKMARK,
				    ai->aiSubjects+ TEDtsiBOOKMARK,
				    ds, sd, ei, bd, cmdEnabled );
#   endif

    tedRefreshImageTool( &(tft->tftImageTool),
				    enabled+ TEDtsiIMAGE,
				    prefs+ TEDtsiIMAGE,
				    ai->aiSubjects+ TEDtsiIMAGE,
				    ds, sd, bd, cmdEnabled );

#   if USE_FRAME_TOOL
    tedRefreshFrameTool( &(tft->tftFrameTool),
				    enabled+ TEDtsiFRAME,
				    prefs+ TEDtsiFRAME,
				    ai->aiSubjects+ TEDtsiFRAME,
				    ds, sd, bd );
#   endif

    tedRefreshTocTool( &(tft->tftTocTool),
				    enabled+ TEDtsiTOC,
				    prefs+ TEDtsiTOC,
				    ai->aiSubjects+ TEDtsiTOC,
				    ds, sd, bd, cmdEnabled );

    tedRefreshSymbolPicker( &(tft->tftSymbolPicker),
				    enabled+ TEDtsiSYMBOL,
				    prefs+ TEDtsiSYMBOL,
				    ai->aiSubjects+ TEDtsiSYMBOL,
				    ds, sd, bd, ed->edDocumentId, cmdEnabled );

#   if USE_FIND_TOOL
    tedRefreshFindTool( &(tft->tftFindTool),
				    enabled+ TEDtsiFIND,
				    prefs+ TEDtsiFIND,
				    ai->aiSubjects+ TEDtsiFIND,
				    ds, sd, bd, ed->edDocumentId, cmdEnabled );
#   endif

#   if USE_SPELL_TOOL
    tedRefreshSpellTool( &(tft->tftSpellTool),
				    enabled+ TEDtsiSPELL,
				    prefs+ TEDtsiSPELL,
				    ai->aiSubjects+ TEDtsiSPELL,
				    ds, sd, bd, ed->edDocumentId, cmdEnabled );
#   endif

    /*  no refresh  */
    enabled[TEDtsiRGB]= 0;

    return;
    }

void tedFormatToolAdaptToSelection( AppInspector *		ai,
				EditDocument *			ed,
				int				choosePage,
				DocumentTree *			ei,
				const DocumentSelection *	ds,
				const SelectionGeometry *	sg,
				const SelectionDescription *	sd,
				const unsigned char *		cmdEnabled )
    {
    TedFormatTool *		tft= (TedFormatTool *)ai->aiTarget;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    int				subject;
    int				enabled[TEDtsi_COUNT];
    int				prefs[TEDtsi_COUNT];
    int				highest= -1;
    int				preferred= -1;

    for ( subject= 0; subject < TEDtsi_COUNT; subject++ )
	{
	enabled[subject]= 1;
	prefs[subject]= 5;
	}

    if  ( ! ds->dsHead.dpNode )
	{ appEnableInspector( ai, 0 ); return; }
    else{
	tedFormatRefreshToolPages( enabled, prefs, tft, ai, ed,
						ei, ds, sg, sd, cmdEnabled );

	/*
	if  ( ed->edFileReadOnly )
	    { appEnableInspector( ai, 0 );	}
	else{ appEnableInspector( ai, 1 );	}
	*/
	}

    for ( subject= 0; subject < TEDtsi_COUNT; subject++ )
	{
	appEnableInspectorSubject( ai, subject,
				    enabled[subject] != 0 );

	if  ( enabled[subject] && prefs[subject] > highest )
	    { preferred= subject; highest= prefs[subject]; }
	}

    if  ( ai->aiCurrentSubject < 0				||
	  choosePage						||
	  ! ai->aiSubjects[ai->aiCurrentSubject].isEnabled	)
	{ appInspectorSelectSubject( ai, preferred );	}

    /*  Actually, this is too late the first time  */
    guiEnableWidget( td->tdToolsSpellingOption,
				    enabled[TEDtsiSPELL] );
    guiEnableWidget( td->tdToolsSymbolOption,
				    enabled[TEDtsiSYMBOL] );
    }

void tedFormatFieldListChanged(	EditApplication *	ea )
    {
    TedAppResources *		tar= (TedAppResources *)ea->eaResourceData;
    TedFormatTool *		tft;

    if  ( ! tar->tarInspector )
	{ return;	}

    tft= (TedFormatTool *)tar->tarInspector->aiTarget;

#   if USE_BOOKMARK_TOOL
    tft->tftBookmarkTool.btCurrentDocumentId= -1;
#   endif
#   if USE_LINK_TOOL
    tft->tftLinkTool.ltCurrentDocumentId= -1;
#   endif

    return;
    }

