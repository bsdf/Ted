/************************************************************************/
/*									*/
/*  Format tool:							*/
/*									*/
/************************************************************************/

#   ifndef	TED_FORMAT_TOOL_H
#   define	TED_FORMAT_TOOL_H

#   include	"tedParaLayoutTool.h"
#   include	"tedParaOrnamentsTool.h"
#   include	"tedTextOrnamentsTool.h"
#   include	"tedRowTool.h"
#   include	"tedSectionTool.h"
#   include	"tedPageLayoutTool.h"
#   include	"tedThisNoteTool.h"
#   include	"tedNotesTool.h"
#   include	"tedHeaderFooterTool.h"
#   include	"tedTableTool.h"
#   include	"tedColumnTool.h"
#   include	"tedCellTool.h"
#   include	"tedTabsTool.h"
#   include	"tedListTool.h"
#   include	"tedSymbolPicker.h"
#   include	"tedLinkTool.h"
#   include	"tedBookmarkTool.h"
#   include	"tedImageTool.h"
#   include	"tedFrameTool.h"
#   include	"tedTextOrnamentsTool.h"
#   include	"tedTocTool.h"
#   include	"tedFindTool.h"
#   include	"tedSpellTool.h"
#   include	<docSelectionDescription.h>

#   include	<appRgbChooserPage.h>

/************************************************************************/
/*									*/
/*  The different pages in a Format tool:				*/
/*									*/
/************************************************************************/

# define USE_FONT_TOOL 1
# define USE_TABS_TOOL 1
# define USE_LIST_TOOL 1
# define USE_PAGE_TOOL 1
# define USE_FIND_TOOL 1
# define USE_SPELL_TOOL 1
# define USE_LINK_TOOL 1
# define USE_BOOKMARK_TOOL 1

# define USE_FRAME_TOOL 0

typedef enum ToolSubjectIndex
    {
#   if USE_FONT_TOOL
    TEDtsiFONT= 0,
#   endif
    TEDtsiTEXT_ORN,

    TEDtsiPARA_LAY,
#   if USE_TABS_TOOL
    TEDtsiTABS,
#   endif
    TEDtsiPARA_ORN,
#   if USE_LIST_TOOL
    TEDtsiLISTS,
#   endif
    TEDtsiSECT,

    TEDtsiTABLE,
    TEDtsiROW,
    TEDtsiCOLUMN,
    TEDtsiCELL,

#   if USE_PAGE_TOOL
    TEDtsiPAGE,
#   endif
    TEDtsiHEADFOOT,
    TEDtsiTHIS_NOTE,
    TEDtsiFOOT_NOTES,
    TEDtsiEND_NOTES,
#   if USE_LINK_TOOL
    TEDtsiLINK,
#   endif
#   if USE_BOOKMARK_TOOL
    TEDtsiBOOKMARK,
#   endif
    TEDtsiIMAGE,
#   if USE_FRAME_TOOL
    TEDtsiFRAME,
#   endif
    TEDtsiTOC,
    TEDtsiSYMBOL,

#   if USE_FIND_TOOL
    TEDtsiFIND,
#   endif
#   if USE_SPELL_TOOL
    TEDtsiSPELL,
#   endif

    TEDtsiRGB,

    TEDtsi_COUNT

    } ToolSubjectIndex;

/************************************************************************/
/*									*/
/*  Represents a format tool.						*/
/*									*/
/************************************************************************/

typedef struct TedFormatTool
    {
    EditApplication *		tftApplication;
    AppInspector *		tftInspector;

    TableTool			tftTableTool;
    RowTool			tftRowTool;
    ColumnTool			tftColumnTool;
    CellTool			tftCellTool;
    ParagraphLayoutTool		tftParagraphLayoutTool;
#   if USE_TABS_TOOL
    TabsTool			tftTabsTool;
#   endif
    ParagraphOrnamentsTool	tftParagraphOrnamentsTool;
#   if USE_LIST_TOOL
    ListTool			tftListsTool;
#   endif
    SectionTool			tftSectionTool;
#   if USE_PAGE_TOOL
    PageLayoutTool		tftPageLayoutTool;
#   endif
    HeaderFooterTool		tftHeaderFooterTool;
    ThisNoteTool		tftThisNoteTool;
    NotesTool			tftFootNotesTool;
    NotesTool			tftEndNotesTool;
#   if USE_FONT_TOOL
    AppFontChooser		tftFontTool;
#   endif
    TextOrnamentsTool		tftTextOrnamentsTool;
#   if USE_LINK_TOOL
    LinkTool			tftLinkTool;
#   endif
#   if USE_BOOKMARK_TOOL
    BookmarkTool		tftBookmarkTool;
#   endif
    ImageTool			tftImageTool;
#   if USE_FRAME_TOOL
    FrameTool			tftFrameTool;
#   endif
    TocTool			tftTocTool;
    SymbolPicker		tftSymbolPicker;
#   if USE_FIND_TOOL
    FindTool			tftFindTool;
#   endif
#   if USE_SPELL_TOOL
    SpellTool			tftSpellTool;
#   endif

    RgbChooserPage		tftRgbPage;
    } TedFormatTool;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

#   endif	/*  TED_FORMAT_TOOL_H */
