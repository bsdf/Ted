#   ifndef	TED_DOCUMENT_H
#   define	TED_DOCUMENT_H

#   include	<textAttribute.h>
#   include	<docSelect.h>
#   include	<docSelectionGeometry.h>
#   include	<docSelectionDescription.h>
#   include	<utilIndexMapping.h>
#   include	<appFrame.h>
#   include	<docEditTrace.h>
#   include	<docRecalculateFields.h>

struct TedDocument;

#   define	RESIZE_BLOCK		10

typedef enum ResizeHandle
    {
    RESIZE_BOTTOM_LEFT= 0,
    RESIZE_BOTTOM_MIDDLE,
    RESIZE_BOTTOM_RIGHT,

    RESIZE_MIDDLE_LEFT,
    RESIZE_MIDDLE_RIGHT,

    RESIZE_TOP_LEFT,
    RESIZE_TOP_MIDDLE,
    RESIZE_TOP_RIGHT,

    RESIZE_COUNT
    } ResizeHandle;

/************************************************************************/
/*									*/
/*  Supported document formats.						*/
/*  Match the array with extensions in tedMain.c.			*/
/*									*/
/************************************************************************/

typedef enum TedDocumentKind
    {
    TEDdockindRTF= 0,
    TEDdockindTEXT_OPEN,
    TEDdockindTEXT_SAVE_WIDE,
    TEDdockindTEXT_SAVE_FOLDED,
    TEDdockindHTML_FILES,
    TEDdockindEML,
    TEDdockindEPUB,
    TEDdockindPS,
    TEDdockindSVG,
    TEDdockindPDF,
    TEDdockindTRACE,
    TEDdockindALL_FILES,

    TEDdockind_COUNT
    } TedDocumentKind;

/************************************************************************/
/*									*/
/*  Private data for a document.					*/
/*									*/
/************************************************************************/

typedef struct TedDocument
    {
    struct BufferDocument *	tdDocument;

    unsigned char		tdTraced;
    unsigned char		tdOverstrike;
				    /**
				     *  The name of the document that is 
				     *  recovered. This is to smuggle the 
				     *  original document name through the 
				     *  generic interface.
				     */
    MemoryBuffer		tdRecoveredName;

    DocumentSelection		tdSelection;
    SelectionGeometry		tdSelectionGeometry;
    SelectionDescription	tdSelectionDescription;
    int				tdBodySectionNumber;
				    /**
				     *  The screen font number corresponding 
				     *  to the text attribute in
				     *  tdSelectionDescription
				     */
    int				tdCurrentScreenFont;

    IndexMapping		tdAttributeToScreenFont;
				    /**
				     *  The magnification factor of the
				     *  document. 1.0 means no magnification.
				     */
    double			tdMagnification;
				    /**
				     *  The number of pixels per twip. The
				     *  magnification of the document is taken 
				     *  into account.
				     */
    double			tdPixelsPerTwip;
				    /**
				     *  The height of the gray strip between 
				     *  pages.
				     */
    int				tdPageGapTwips;
    int				tdPageGapPixels;

    EditTrace			tdEditTrace;

    APP_WIDGET			tdFileOpenOption;
    APP_WIDGET			tdFileSaveOption;
    APP_WIDGET			tdFileSaveAsOption;
    APP_WIDGET			tdFileUnlockOption;
    APP_WIDGET			tdFileRecoverOption;
    APP_WIDGET			tdFilePropertiesOption;
    APP_WIDGET			tdFileCloseOption;
    APP_WIDGET			tdFileQuitSeparator;
    APP_WIDGET			tdFileQuitOption;

    APP_WIDGET			tdUndoOption;
    APP_WIDGET			tdRepeatOption;

    APP_WIDGET			tdCopyOption;
    APP_WIDGET			tdCutOption;
    APP_WIDGET			tdPasteOption;

    APP_WIDGET			tdInsertMenu;
    APP_WIDGET			tdInsertMenuButton;
    APP_WIDGET			tdInsPictOption;
    APP_WIDGET			tdInsFileOption;
    APP_WIDGET			tdInsSymbolOption;
    APP_WIDGET			tdInsHyperlinkOption;
    APP_WIDGET			tdInsBookmarkOption;
    APP_WIDGET			tdInsInsertFootnoteOption;
    APP_WIDGET			tdInsInsertEndnoteOption;
    APP_WIDGET			tdInsInsertChftnsepOption;
    APP_WIDGET			tdInsInsertTableOption;
    APP_WIDGET			tdInsInsertPageNumberOption;
    APP_WIDGET			tdInsInsertLineBreakOption;
    APP_WIDGET			tdInsInsertPageBreakOption;
    APP_WIDGET			tdInsInsertColumnBreakOption;
    APP_WIDGET			tdInsInsertSectBreakOption;

    APP_WIDGET			tdTableMenu;
    APP_WIDGET			tdTableMenuButton;
    APP_WIDGET			tdTabInsertTableOption;
    APP_WIDGET			tdTabAddRowOption;
    APP_WIDGET			tdTabAddColumnOption;
    APP_WIDGET			tdTabInsertSeparator;

    APP_WIDGET			tdTabSelectTableOption;
    APP_WIDGET			tdTabSelectRowOption;
    APP_WIDGET			tdTabSelectColumnOption;
    APP_WIDGET			tdTabSelectSeparator;

    APP_WIDGET			tdTabDeleteTableOption;
    APP_WIDGET			tdTabDeleteRowOption;
    APP_WIDGET			tdTabDeleteColumnOption;
    APP_WIDGET			tdTabDeleteSeparator;

    APP_WIDGET			tdFormatMenu;
    APP_WIDGET			tdFormatMenuButton;
    APP_WIDGET			tdFormatOneParaOption;
    APP_WIDGET			tdFormatOneParaSeparator;

    APP_WIDGET			tdFormatPasteRulerOption;
    APP_WIDGET			tdFormatIncreaseIndentOption;
    APP_WIDGET			tdFormatDecreaseIndentOption;
    APP_WIDGET			tdFormatRulerSeparator;

    APP_WIDGET			tdFormatAlignLeftOption;
    APP_WIDGET			tdFormatAlignRightOption;
    APP_WIDGET			tdFormatAlignCenterOption;
    APP_WIDGET			tdFormatAlignJustifyOption;
    APP_WIDGET			tdFormatAlignSeparator;

    APP_WIDGET			tdDrawTableGridOption;

    APP_WIDGET			tdFontMenu;
    APP_WIDGET			tdFontMenuButton;
    APP_WIDGET			tdFontBoldOption;
    APP_WIDGET			tdFontItalicOption;
    APP_WIDGET			tdFontUnderlinedOption;
    APP_WIDGET			tdFontSuperscriptOption;
    APP_WIDGET			tdFontSubscriptOption;
    APP_WIDGET			tdFontToggleSeparator;
    APP_WIDGET			tdFontPasteOption;

    APP_WIDGET			tdToolsMenu;
    APP_WIDGET			tdToolsMenuButton;
    APP_WIDGET			tdToolsFormatToolOption;

    APP_WIDGET			tdToolsFontOption;
    APP_WIDGET			tdToolsFindOption;
    APP_WIDGET			tdToolsSpellingOption;
    APP_WIDGET			tdToolsPageLayoutOption;
    APP_WIDGET			tdToolsSymbolOption;

    RGB8Color			tdTableColor;
    RGB8Color			tdSelColor;
    RGB8Color			tdCopiedSelColor;

    int				tdVisibleSelectionCopied;
    MemoryBuffer		tdCopiedSelection;
    MemoryBuffer		tdCopiedFont;
    MemoryBuffer		tdCopiedRuler;
    RasterImage			tdCopiedImage;
    int				tdOwnsPrimarySelection;

    void *			tdFindProg;

#   ifdef USE_MOTIF
    XtIntervalId		tdHideIBarId;
    XtIntervalId		tdShowIBarId;
#   endif

#   ifdef USE_GTK
    guint			tdHideIBarId;
    guint			tdShowIBarId;
#   endif

#   ifdef USE_QT
    int				tdHideIBarId;
    int				tdShowIBarId;
#   endif

    APP_WINDOW			tdObjectWindow;
    APP_WINDOW			tdObjectResizeWindows[RESIZE_COUNT];
    int				tdObjectCornerMovedX;
    int				tdObjectCornerMovedY;
    int				tdScaleChangedX;
    int				tdScaleChangedY;
    int				tdObjectResizeCorner;

    int				tdDrawTableGrid;
    int				tdDrawMonochrome;

    int				tdInProgrammaticChange;
    } TedDocument;

/************************************************************************/

extern const char TedTraceExtension[];

/************************************************************************/
/*									*/
/*  Routine Declarations.						*/
/*									*/
/************************************************************************/

extern int tedFirstRecalculateFields(	RecalculateFields *	rf,
					DOC_CLOSE_OBJECT	closeObject,
					BufferDocument *	bd );

#   endif	/*  TED_DOCUMENT_H	*/
