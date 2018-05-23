/************************************************************************/
/*									*/
/*  Basic structures for a Gui document manipulator application.	*/
/*									*/
/************************************************************************/

#   ifndef	APP_FRAME_H
#   define	APP_FRAME_H

#   include	<stdio.h>

#   include	"appGuiBase.h"
#   include	"appGuiResource.h"

#   include	<sioGeneral.h>
#   include	<geo2DInteger.h>
#   include	<utilDocumentGeometry.h>
#   include	<utilPrinter.h>
#   include	"appIcons.h"
#   include	"guiWidgets.h"
#   include	<drawDrawingSurface.h>
#   include	<psPostScriptFontList.h>

# ifdef __cplusplus
extern "C"
    {
# endif

struct PrintGeometry;
struct PrintJob;

/************************************************************************/
/*									*/
/*  For configuring texts of 'Really' dialogs.				*/
/*									*/
/************************************************************************/

typedef struct AppFileMessageResources
    {
					/********************************/
					/*  Format for 'New' title	*/
					/********************************/
    char *		afmrNamelessTitleFormat;
					/********************************/
					/*  Really Close? Dialog.	*/
					/********************************/
    char *		afmrReallyCloseQuestion;
    char *		afmrReallyCloseSaveIt;
    char *		afmrReallyCloseDontSave;
    char *		afmrReallyCloseCancel;
					/********************************/
					/*  Really Quit? Dialog.	*/
					/********************************/
    char *		afmrReallyQuitQuestion;
    char *		afmrReallyQuitReview;
    char *		afmrReallyQuitAnyway;
    char *		afmrReallyQuitCancel;
					/********************************/
					/*  Messages about  a file.	*/
					/********************************/
    char *		afmrFileNoAccess;
    char *		afmrFileReadOnly;
    char *		afmrNoSuchFileMessage;
    char *		afmrFileNotWritable;
    char *		afmrMakeItQuestion;
    } AppFileMessageResources;

typedef struct AppMenuItem
    {
    const char *	amiTextResName;
    const char *	amiItemText;

    const char *	amiKeyResName;
    const char *	amiKey;

    const char *	amiKeyTextResName;
    const char *	amiKeyText;

    MenuItemType	amiItemType;
    APP_MENU_CALLBACK_T	amiCallback;
    APP_WIDGET		amiOptionWidget;
    } AppMenuItem;

/************************************************************************/
/*									*/
/*  Kinds of files that an application can open.			*/
/*									*/
/************************************************************************/

#   define	APPFILE_CAN_OPEN	0x01
#   define	APPFILE_CAN_SAVE	0x02
#   define	APPFILE_IS_BASIC_TYPE	0x04
#   define	APPFILE_HIDE_OPEN	0x08

typedef struct AppFileExtension
    {
    const char *	afeId;
    const char *	afeFilter;
    const char *	afeDescription;
    const char *	afeExtension;
    unsigned int	afeUseFlags;
    } AppFileExtension;

typedef struct SelectRectangle
    {
    int				srDirection;
#				define DOCselNONE	0

#				define DOCselN		1
#				define DOCselE		2
#				define DOCselS		3
#				define DOCselW		4

    DocumentRectangle		srSelected;
    DocumentRectangle		srLTM; /* left/top margins */
    DocumentRectangle		srRBM; /* right/bottom margins */
    } SelectRectangle;

typedef struct EditDocument
    {
    struct EditApplication *	edApplication;

    MemoryBuffer		edFilename;
    MemoryBuffer		edTitle;
    int				edFormat;
    int				edFileReadOnly;
				/**
				 *  Unique number. Can be used to distinguish
				 *  EditDocuments wilhout remembering pointers
				 */
    unsigned int		edDocumentId;

    AppToplevel			edToplevel;

    APP_WIDGET			edMenuBar;
    APP_WIDGET			edMainWindow;
    APP_WIDGET				edFileMenu;
    APP_WIDGET				edFileMenuButton;
    APP_WIDGET					edFileCloseOption;
    APP_WIDGET				edEditMenu;
    APP_WIDGET				edEditMenuButton;
    APP_WIDGET				edWindowMenu;
    APP_WIDGET				edWindowMenuButton;
    APP_WIDGET				edHelpMenu;
    APP_WIDGET				edHelpMenuButton;

    APP_WIDGET			edToolbar;
    APP_WIDGET			edScrolledWindow;
    APP_WIDGET				edVerticalScrollbar;
    APP_WIDGET				edHorizontalScrollbar;
    APP_WIDGET				edWorkWidget;
#   ifdef USE_GTK
    GtkAdjustment *			edVerticalAdjustment;
    GtkAdjustment *			edHorizontalAdjustment;
#   endif
    DocumentWidget			edDocumentWidget;
    struct DrawingSurface *		edDrawingSurface;

    APP_WIDGET				edLeftRulerWidget;
    APP_WIDGET				edTopRulerWidget;
    APP_WIDGET				edRightRulerWidget;
    APP_WIDGET				edBottomRulerWidget;

    void *				edLeftRuler;
    void *				edTopRuler;
    void *				edRightRuler;
    void *				edBottomRuler;

    int					edLeftRulerWidePixels;
    int					edTopRulerHighPixels;
    int					edRightRulerWidePixels;
    int					edBottomRulerHighPixels;

    int				edHasBeenChanged;
    int				edIsReadonly;
    int				edIsVisible;

				    /**
				     *  The (pixel) rectangle that the whole
				     *  document would occupy on screen.
				     *  I.E: On a huge screen without 
				     *  scrollbars. X0 and X0 are 0 by 
				     *  definition.
				     */
    DocumentRectangle		edFullRect;
				    /**
				     *  The (pixel) rectangle of the document
				     *  that is visible ob screen. Together 
				     *  with edFullRect, it determines the 
				     *  positions of the scrollbars.
				     */
    DocumentRectangle		edVisibleRect;

				    /**
				     *  The color to draw where the window is
				     *  not covered by the document.
				     */
    RGB8Color			edBackgroundColor;

    void *			edPrivateData;

    struct AppSelectionTargetType *
				edTargetTypes;
    int				edTargetTypeCount;

    int				edMapped;	/*  Struggle with fvwm	*/
    int				edNotYetDrawn;	/*  For FirstVisible	*/

    SelectRectangle		edSelectRectangle;
    } EditDocument;

/************************************************************************/
/*									*/
/*  Special calls to the aplication.					*/
/*									*/
/************************************************************************/

typedef struct SpecialCall
    {
    const char *	scCall;
    int			(*scExecuteCall)(
					struct EditApplication *	ea,
					const char *			prog,
					const char *			call,
					int				argc,
					char **				argv );
				/****************************************/
				/*  Returns the number of parameters	*/
				/*  consumed or -1 on failure.		*/
				/****************************************/
    } SpecialCall;

/************************************************************************/
/*									*/
/*  Describes the application.						*/
/*									*/
/*  1)  Descriptive members.						*/
/*  2)  Allocated at run time.						*/
/*									*/
/************************************************************************/

typedef struct EditApplication
    {
    /*  1  */
    const char *	eaApplicationName;
    const char *	eaOptionalComponents;
    const char *	eaNameAndVersion;
    const char *	eaReference;
    const char *	eaPlatformCompiled;
    const char *	eaHostDateCompiled;

			/**
			 *  The Application Icon
			 */
    const char *	eaMainIcon;
			/**
			 *  The Image on the Splash Screen
			 */
    const char *	eaMainPicture;
    AppFileExtension *	eaFileExtensions;
    int			eaFileExtensionCount;
    const char *	eaDefaultFileFilter;
    void *			eaResourceData;
    AppConfigurableResource *	eaResourceTable;
    int				eaResourceCount;
    AppConfigurableResource *	eaFileMessageResourceTable;
    int				eaFileMessageResourceCount;
    const SpecialCall *		eaSpecialCalls;
    int				eaSpecialCallCount;
    int				eaCreateNewFromCommand;

    double		eaMagnification;

    int			eaLeftRulerWidthMM;
    int			eaTopRulerHeightMM;
    int			eaRightRulerWidthMM;
    int			eaBottomRulerHeightMM;

    int			eaLeftRulerWidthMultiple;
    int			eaTopRulerHeightMultiple;
    int			eaRightRulerWidthMultiple;
    int			eaBottomRulerHeightMultiple;

    const char *	eaUnitString;
    const char *	eaPaperString;
    const char *	eaLeftMarginString;
    const char *	eaTopMarginString;
    const char *	eaRightMarginString;
    const char *	eaBottomMarginString;
    int			eaUnitInt;
    DocumentGeometry	eaDefaultDocumentGeometry;

    char **		eaAppFileMenuText;
    AppMenuItem *	eaAppFileMenuItems;
    int			eaAppFileMenuItemCount;

    char **		eaAppWinMenuText;
    AppMenuItem *	eaAppWinMenuItems;
    int			eaAppWinMenuItemCount;

    char **		eaDocFileMenuText;
    AppMenuItem *	eaDocFileMenuItems;
    int			eaDocFileMenuItemCount;

    char **		eaDocEditMenuText;
    AppMenuItem *	eaDocEditMenuItems;
    int			eaDocEditMenuItemCount;

    char **		eaDocWindowMenuText;
    AppMenuItem *	eaDocWindowMenuItems;
    int			eaDocWindowMenuItemCount;

    char **		eaDocHelpMenuText;
    AppMenuItem *	eaDocHelpMenuItems;
    int			eaDocHelpMenuItemCount;

    char **		eaAppHelpMenuText;
    AppMenuItem *	eaAppHelpMenuItems;
    int			eaAppHelpMenuItemCount;

    void *		(*eaMakePrivateData)( void );

    int			(*eaMakeDocumentWidget)(
				struct EditApplication *	ea,
				EditDocument *			ed );
    int			(*eaOpenDocument)(
				struct EditApplication *	ea,
				void *				privateData,
				int *				pFormat,
				APP_WIDGET			relative,
				APP_WIDGET			option,
				int				readOnly,
				int				suggestStdin,
				int				formatHint,
				const MemoryBuffer *		filename );
    int			(*eaNewDocument)(
				EditDocument *			ed,
				const MemoryBuffer *		filename );
    int			(*eaLayoutDocument)(
				DocumentRectangle *		drScreen,
				DocumentRectangle *		drVisible,
				void *				privateData,
				int				format,
				DrawingSurface			ds,
				const PostScriptFontList *	psfl,
				const DocumentGeometry *	defDg );
    int			(*eaFinishDocumentSetup)(
				EditDocument *			ed );
    void		(*eaDocumentFirstVisible)(
				EditDocument *			ed );
    int			(*eaCanSaveDocument)(
				const void *			privateData,
				int				format );
    int			(*eaSaveDocument)(
				struct EditApplication *	ea,
				DrawingSurface			ds,
				const void *			privateData,
				int				format,
				const MemoryBuffer *		documentTitle,
				int				suggestStdout,
				const MemoryBuffer *		filename,
				int				isDocName );
    void		(*eaFreeDocument)(
				void *				privateData,
				int				format );
    void		(*eaSuggestPageSetup)(
				struct PrintGeometry *		pg,
				void *				privateData,
				int				sheetSize );
    int			(*eaPrintDocument)(
				SimpleOutputStream *		sos,
				const struct PrintJob *		pj,
				const struct PrintGeometry *	pg );
    void		(*eaDrawRectangle)(
				EditDocument *			ed,
				DocumentRectangle *		drClip,
				int				ox,
				int				oy );

    void		(*eaVisibleDocumentCountChanged)(
				    struct EditApplication *	ea,
				    int				from,
				    int				to );

    void		(*eaMakePrivateApplicationMenus)(
				    struct EditApplication *	ea,
				    APP_WIDGET			menubar );

    void		(*eaMakePrivateDocumentMenus)(
				    struct EditApplication *	ea,
				    EditDocument *		ed,
				    APP_WIDGET			menubar );

    void		(*eaDocCopy)( EditDocument *);
    void		(*eaDocCut)( EditDocument *);
    void		(*eaDocSelAll)( EditDocument * );

    void		(*eaSetPageLayout)(
				    struct EditDocument *	ed,
				    const PropertyMask *	setMask,
				    const DocumentGeometry *	dgNew,
				    int				wholeDocument );

			/************************************************/
			/*  User input on the document widget:		*/
			/************************************************/
    APP_EVENT_HANDLER_T	eaDocumentMouseHandler;
    APP_EVENT_HANDLER_T	eaDocumentScrollHandler;
    void		(*eaDocGotString)(	void *		voided,
						const char *	value,
						int		length );
    void		(*eaDocGotKey)(		void *		voided,
						int		key,
						unsigned int	state );
    APP_EVENT_HANDLER_T	eaObserveFocus;

    APP_SCROLLBAR_CALLBACK_T	eaDocHorizontalScrollbarCallback;
    APP_SCROLLBAR_CALLBACK_T	eaDocVerticalScrollbarCallback;

    void		(*eaScrollTopRuler) (	void *, APP_WIDGET, int );
				    /**
				     *  Adapt to a change in size of the 
				     *  document: Remember the visible range.
				     */
    void		(*eaSetTopRulerRange)(	void *		voidtr,
						APP_WIDGET	w,
						int		docX1,
						int		docVisX0,
						int		docVisX1 );

    void		(*eaScrollLeftRuler) (	void *, APP_WIDGET, int );
    void		(*eaSetLeftRulerRange)(	void *		voidtr,
						APP_WIDGET	w,
						int		docY1,
						int		docVisY0,
						int		docVisY1 );

    void		(*eaScrollRightRuler) (	void *, APP_WIDGET, int );
    void		(*eaSetRightRulerRange)(	void *	voidtr,
						APP_WIDGET	w,
						int		docY1,
						int		docVisY0,
						int		docVisY1 );

    void		(*eaScrollBottomRuler) ( void *, APP_WIDGET, int );
				    /**
				     *  Adapt to a change in size of the 
				     *  document: Remember the visible range.
				     */
    void		(*eaSetBottomRulerRange)( void *	voidtr,
						APP_WIDGET	w,
						int		docX1,
						int		docVisX0,
						int		docVisX1 );

    int			(*eaSetTopRuler)(	EditDocument *	ed );
    int			(*eaSetLeftRuler)(	EditDocument *	ed );
    int			(*eaSetRightRuler)(	EditDocument *	ed );
    int			(*eaSetBottomRuler)(	EditDocument *	ed );

    void		(*eaFreeTopRuler)(
					void *			topRuler );
    void		(*eaFreeLeftRuler)(
					void *			leftRuler );
    void		(*eaFreeRightRuler)(
					void *			rightRuler );
    void		(*eaFreeBottomRuler)(
					void *			bottomRuler );

    struct AppSelectionType *	eaDocSelectionTypes;
    int				eaDocSelectionTypeCount;

    struct AppSelectionType *	eaAppSelectionTypes;
    int				eaAppSelectionTypeCount;

    PrintDestination *	eaPrintDestinations;
    int			eaPrintDestinationCount;
    int			eaDefaultPrintDestination;
    int			eaPrintDestinationsCollected;

    NamedPicture *	eaNamedPictures;
    int			eaNamedPictureCount;

    APP_ATOM		eaCloseAtom;

    unsigned int	eaNextDocumentId;

				/**
				 *  Culture name (Language and teritory) 
				 *  determined at startup.
				 */
    char *			eaLocaleName;

    /*  2  */
#   ifdef USE_MOTIF
    XtAppContext		eaContext;
#   endif

    int				eaScreenPixelsWide;
    int				eaScreenPixelsHigh;
    double			eaPixelsPerTwip;

    AppToplevel			eaToplevel;
	APP_INPUT_METHOD	eaInputMethod;
	APP_CURSOR		eaDocumentCursor;
    APP_WIDGET			eaMainWindow;
    APP_WIDGET			eaMenuBar;
    APP_WIDGET			eaFileMenu;
    APP_WIDGET			eaFileMenuButton;
    APP_WIDGET			eaWinMenu;
    APP_WIDGET			eaWinMenuButton;
    APP_WIDGET			eaHelpMenu;
    APP_WIDGET			eaHelpMenuButton;

    void *			eaPageTool;
    void *			eaPrintDialog;

    int				eaArgc;
    char **			eaArgv;

    EditDocument *		eaCurrentDocument;
    EditDocument **		eaOpenDocuments;
    int				eaOpenCount;
    int				eaVisibleDocumentCount;
    int				eaMainVisibleAsAbout;

    char *			eaAfmDirectory;
    char *			eaFontDirectory;
    char *			eaGhostscriptFontmap;
    char *			eaGhostscriptFontToXmapping;
    int				eaGhostscriptMappingsRead;
    char *			eaCustomPrintCommand;
    char *			eaCustomPrinterName;
    char *			eaCustomPrintCommand2;
    char *			eaCustomPrinterName2;
    char *			eaAuthor;
    char *			eaFocusColor;

    int				eaUsePostScriptFiltersInt;
    int				eaUsePostScriptIndexedImagesInt;
    int				ea7BitsPostScriptInt;
    int				eaSkipEmptyPagesInt;
    int				eaSkipBlankPagesInt;
    int				eaOmitHeadersOnEmptyPagesInt;
    int				eaAvoidFontconfigInt;
    int				eaPreferBase35FontsInt;
    int				eaEmbedFontsInt;
    int				eaUseKerningInt;
    int				eaStyleToolInt;

    AppFileMessageResources	eaFileMessageResources;

    char *			eaMagnificationString;
    char *			eaUsePostScriptFiltersString;
    char *			eaUsePostScriptIndexedImagesString;
    char *			ea7BitsPostScriptString;
    char *			eaSkipEmptyPagesString;
    char *			eaSkipBlankPagesString;
    char *			eaOmitHeadersOnEmptyPagesString;
    char *			eaAvoidFontconfigString;
    char *			eaPreferBase35FontsString;
    char *			eaEmbedFontsString;
    char *			eaUseKerningString;
    char *			eaStyleToolString;

    char *			eaCustomPsSetupFilename;

    char *			eaLeftRulerWidthMMString;
    char *			eaTopRulerHeightMMString;
    char *			eaRightRulerWidthMMString;
    char *			eaBottomRulerHeightMMString;

    int				eaGotPaste;

    PostScriptFontList		eaPostScriptFontList;

    void *			eaSystemProperties;
    void *			eaUserProperties;

    int				eaGotResourceTable;
    int				eaGotFileMessageResourceTable;
    int				eaGotApplicationResources;
    } EditApplication;

/************************************************************************/
/*									*/
/*  Describes types of content for Copy/Paste.				*/
/*  The order is the order of desirability for the application.		*/
/*									*/
/************************************************************************/

typedef void (*APP_FORGET_COPY)( APP_WIDGET		w,
				void *			through,
				APP_EVENT *		event );

typedef struct AppSelectionTargetType
    {
    const char *	asttTargetString;		/*  ContentType	*/
    APP_ATOM		asttTargetAtom;
    APP_PASTE_REPLY	asttUsePaste;
    APP_GIVE_COPY	asttGiveCopy;
    } AppSelectionTargetType;

typedef struct AppSelectionType
    {
    const char *		astSelectionString;	/*  What	*/
    APP_ATOM			astSelectionAtom;

    AppSelectionTargetType *	astTargetTypes;
    int				astTargetTypeCount;

    APP_FORGET_COPY		astForgetCopy;
    } AppSelectionType;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern APP_WIDGET appMakeMenu(		APP_WIDGET *		pButton,
					AppToplevel *		at,
					EditApplication *	ea,
					APP_WIDGET		parent,
					const char *		itemText,
					int			isHelp,
					AppMenuItem *		items,
					int			itemCount,
					void *			through );

extern EditDocument * appOpenDocumentFile(
					EditApplication *	ea,
					APP_WIDGET		relative,
					APP_WIDGET		option,
					int			readOnly,
					int			suggestStdin,
					int			formatHint,
					const MemoryBuffer *	filename );

extern EditDocument * appOpenDocument(	EditApplication *	ea,
					APP_WIDGET		relative,
					APP_WIDGET		option,
					int			readOnly,
					const MemoryBuffer *	filename );

extern int appNewDocument(	EditApplication *	ea,
				const MemoryBuffer *	filename );

extern int appMain(	EditApplication *	ea,
			int			argc,
			char *			argv[] );

extern void appSetDocument(	EditApplication *	ea,
				EditDocument *		ed );

extern void appRemoveDocument(	EditApplication *	ea,
				EditDocument *		ed );

extern void appCloseDocument(	EditDocument *		ed );

extern void appSetCurrentDocument(	EditApplication *	ea,
					EditDocument *		ed );

extern void appDocToFront(	APP_WIDGET		option,
				void *			voided,
				void *			e );

extern void appDocumentChanged(	EditDocument *		ed,
				int			changed );

void appQuitApplication(	APP_WIDGET		option,
				APP_WIDGET		relative,
				EditApplication *	ea );

extern APP_MENU_CALLBACK_H( appAppFileQuit, option, voidea, e );

extern void appDocVisible(	EditApplication *	ea,
				EditDocument *		ed,
				int			visible );

extern APP_MENU_CALLBACK_H( appAppFileOpen, option, voidea, e );
extern APP_MENU_CALLBACK_H( appAppFileNew, option, voidea, e );

extern void appDocSetScrollbarValues(	EditDocument *	ed );

extern void appMouseWheelUp(		EditDocument *	ed );
extern void appMouseWheelDown(		EditDocument *	ed );
extern APP_EVENT_HANDLER_H( appScrollEventHandler, w, voided, scrollEvent );

extern APP_MENU_CALLBACK_H( appDocFileSaveAs, option, voided, e );

extern APP_MENU_CALLBACK_H( appDocFileNew, option, voided, e );
extern APP_MENU_CALLBACK_H( appDocFileOpen, option, voided, e );
extern APP_MENU_CALLBACK_H( appDocFileSave, option, voided, e );
extern APP_MENU_CALLBACK_H( appDocFileClose, option, voided, e );
extern APP_MENU_CALLBACK_H( appDocFileQuit, option, voided, e );
extern APP_MENU_CALLBACK_H( appDocEditCopy, option, voided, e );
extern APP_MENU_CALLBACK_H( appDocEditCut, option, voided, e );
extern APP_MENU_CALLBACK_H( appDocEditSelAll, option, voided, e );

extern APP_CLOSE_CALLBACK_H( appDocFileCloseCallback, w, voided );
extern APP_CLOSE_CALLBACK_H( appAppWmClose, w, voidea );

extern void appRunReallyCloseDialog(	APP_WIDGET		option,
					EditDocument *		ed );

extern int appRunReallyQuitDialog(	APP_WIDGET		option,
					APP_WIDGET		relative,
					EditApplication *	ea );

extern void appMakeDocVisible(	EditApplication *	ea,
				EditDocument *		ed );

extern void appShowDefaultsEditor(	APP_WIDGET	prefOption,
					void *		voidea );

extern int appGetImagePixmap(	EditApplication *	ea,
				const char *		name,
				APP_BITMAP_IMAGE *	pImage,
				APP_BITMAP_MASK *	pMask );

extern void appDiscardImagePixmaps(	EditApplication *	ea );

extern int appSetDocumentFilename(	EditDocument *		ed,
					const MemoryBuffer *	filename );

extern int appSetDocumentTitle(		EditDocument *		ed,
					const MemoryBuffer *	title );

extern int appFinishDocumentSetup(	EditDocument *		ed );

extern int appSetupDocument(		EditApplication *	ea,
					EditDocument *		ed );

extern void appRenameDocumentOptions(	EditApplication *	ea,
					EditDocument *		ed,
					const MemoryBuffer *	title );

extern void * appMakePageTool(	EditApplication *	ea,
				APP_WIDGET		printOption,
				const char *		pixmapName );

extern void appShowPageTool(		EditApplication *	ea );

extern void appPageToolSetProperties(	void *				voidapt,
					const DocumentGeometry *	dg );

extern void appEnablePageTool(		void *		voidapt,
					int		enabled );

extern void appRunPrintDialog(		EditDocument *		ed,
					const DocumentGeometry * dgDocument,
					int			pageCount,
					int			firstSelected,
					int			lastSelected,
					APP_WIDGET		printOption,
					const char *		pixmapName );

extern int appPrintDocument(	int				printer,
				const struct PrintJob *		pj );

extern void appDocPrintToFile(	APP_WIDGET			option,
				APP_WIDGET			panel,
				EditDocument *			ed,
				const struct PrintGeometry *	pg );

APP_MENU_CALLBACK_H( appDocAbout, option, voided, e );

extern APP_EVENT_HANDLER_H( appDocCopyPasteHandler, w, voided, event );
extern APP_EVENT_HANDLER_H( appAppGotPasteCall, w, voided, event );

# ifdef USE_GTK

extern void appDocGotPasteReplyGtk(	GtkWidget *		w,
					GtkSelectionData *	gsd,
					guint			time,
					void *			voided );

extern void appAppGotPasteReplyGtk(	GtkWidget *		w,
					GtkSelectionData *	gsd,
					guint			time,
					void *			voidea );
# endif

extern int appDocReleaseSelection(	EditDocument *		ed,
					const char *		selection );

extern int appDocOwnSelection(	EditDocument *			ed,
				const char *			selection,
				AppSelectionTargetType * 	targets,
				int				targetCount );

extern int appDocAskForPaste(	EditDocument *		ed,
				const char *		selection );

extern int appAppAskForPaste(	EditApplication *	ea,
				const char *		selection );

APP_SCROLLBAR_CALLBACK_H( appDocHorizontalScrollbarCallback, bar, voided, e );
APP_SCROLLBAR_CALLBACK_H( appDocVerticalScrollbarCallback, bar, voided, e );

extern void appReportSaveFailure(	EditApplication *	ea,
					APP_WIDGET		option,
					APP_WIDGET		relative,
					const MemoryBuffer *	filename );

extern void appMakeVerticalDialog(	AppDialog *		ad,
					APP_WIDGET *		pPaned,
					EditApplication *	ea,
					APP_CLOSE_CALLBACK_T	closeCallback,
					APP_DESTROY_CALLBACK_T	destroyCallback,
					void *			through );

extern int appCallPrintFunction( SimpleOutputStream *		sos,
				const struct PrintJob *		pj );

extern int appImgMakeFileExtensions(	AppFileExtension **	pAfeList,
					int *			pAfeCount );

extern int appMakeDocumentWidget(	EditApplication *	ea,
					EditDocument *		ed );

extern APP_EVENT_HANDLER_H( appDocExposeHandler, w, d, exposeEvent );

extern int appFileConvert(	EditApplication *	ea,
				const MemoryBuffer *	fromName,
				const MemoryBuffer *	toName );

extern int appPrintToFile(	EditApplication *	ea,
				const char *		fromName,
				const char *		toName,
				const char *		paperString );

extern int appPrintToPrinter(	EditApplication *	ea,
				const char *		fromName,
				const char *		toName,
				const char *		paperString );

APP_DESTROY_CALLBACK_H( appDestroyEditDocument, w, voided );

extern void appScrollToRectangle( EditDocument *		ed,
				const DocumentRectangle *	dr,
				int *				pScrolledX,
				int *				pScrolledY );

extern void appMakeVerticalTool( APP_WIDGET *		pShell,
				APP_WIDGET *		pPaned,
				EditApplication *	ea,
				APP_BITMAP_IMAGE	iconPixmap,
				APP_BITMAP_MASK		iconMask,
				int			userResizable,
				APP_WIDGET		option,
				APP_CLOSE_CALLBACK_T	closeCallback,
				void *			through );

extern void appGuiGetResourceValues(
				int *				pGotResources,
				EditApplication *		ea,
				void *				pValues,
				AppConfigurableResource *	acr,
				int				acrCount );

extern void appSetResourceDefaults(
				EditApplication *		ea,
				AppConfigurableResource *	acr,
				int				acrCount );

extern int appGuiInitApplication(	EditApplication *	ea,
					int *			pArgc,
					char ***		pArgv );

extern void appShowShellWidget(		EditApplication *	ea,
					APP_WIDGET		shell );

/************************************************************************/
/*									*/
/*  Names of the X11 events.						*/
/*									*/
/************************************************************************/

extern void appDocumentRulerWidth(	EditApplication *	ea,
					EditDocument *		ed );

extern int appFinishDocumentWindow(	EditDocument *		ed );

extern void appGuiSetShellTitle(	APP_WIDGET		shell,
					const MemoryBuffer *	title );

extern void appGuiSetIconTitle(		APP_WIDGET		shell,
					const MemoryBuffer *	title );

extern APP_WIDGET appSetMenuItem(	APP_WIDGET		menu,
					AppToplevel *		at,
					AppMenuItem *		ami,
					void *			target );

extern APP_WIDGET appSetMenuSeparator(	APP_WIDGET		menu,
					AppToplevel *		at,
					AppMenuItem *		ami,
					void *			target );

extern APP_WIDGET appSetToggleMenuItem(	APP_WIDGET		menu,
					AppToplevel *		at,
					AppMenuItem *		ami,
					void *			target );

extern APP_WIDGET appMakeMenuInParent(	APP_WIDGET *		pButton,
					AppToplevel *		at,
					APP_WIDGET		menuBar,
					const char *		itemText,
					int			isHelp );

extern void appGuiRunDialog( 		AppDialog *		ad,
					int			initial,
					EditApplication *	ea );

extern void appGuiBreakDialog( 		AppDialog *		ad,
					int			response );

extern void appGuiSetDefaultButtonForDialog(	AppDialog *	ad,
						APP_WIDGET	button );

extern void appGuiSetCancelButtonForDialog(	AppDialog *	ad,
						APP_WIDGET	button );

extern void appGuiShowDialog(			EditApplication * ea,
						AppDialog *	ad,
						APP_WIDGET	relative );

extern void appGuiHideDialog(			AppDialog *	ad );

extern void appDocFillMenu(		EditDocument *		ed );

extern void appRunDragLoop(	APP_WIDGET		w,
				EditApplication *	ea,
				APP_EVENT *		downEvent,
				APP_EVENT_HANDLER_T	upHandler,
				APP_EVENT_HANDLER_T	moveHandler,
				int			timerInterval,
				APP_TIMER_CALLBACK	timerHandler,
				void *			through );

extern void appGuiSetFocusChangeHandler( APP_WIDGET		shell,
					APP_EVENT_HANDLER_T	handler,
					void *			through );

extern void appGuiSetToggleItemState(	APP_WIDGET		toggle,
					int			set );

extern void appSetDestroyCallback(	APP_WIDGET		shell,
					APP_DESTROY_CALLBACK_T	destroyCallback,
					void *			through );

extern void appSetCloseCallback(	APP_WIDGET		shell,
					EditApplication *	ea,
					APP_CLOSE_CALLBACK_T	closeCallback,
					void *			through );

extern void appGuiSetToggleItemLabel(	APP_WIDGET		toggle,
					const char *		label );

extern int appFormatDocumentTitle(	MemoryBuffer *		windowTitle,
					MemoryBuffer *		iconTitle,
					EditApplication *	ea,
					const MemoryBuffer *	title );

extern APP_EVENT_HANDLER_H( appDocConfigure, w, voided, event );

extern void appSetWindowsItemState(	APP_WIDGET	menu,
					EditDocument *	ed,
					int		changed );

extern void appRemoveWindowsOption(	APP_WIDGET		menu,
					EditDocument *		oldEd );

extern void appRenameWindowsOption(	APP_WIDGET		menu,
					EditDocument *		ed,
					const MemoryBuffer *	title );

extern void appAllocateCopyPasteTargetAtoms(	EditApplication *	ea );

extern APP_GIVE_COPY( appDocReplyToCopyRequest, w, gsd, voided );

extern void appCopyPixmapValue(	APP_SELECTION_EVENT *	event,
				APP_BITMAP_IMAGE	pixmapCopied );

extern void appGetApplicationResourceValues(	EditApplication *	ea );

extern void appDocExposeRectangle(
				const EditDocument *		ed,
				const DocumentRectangle *	drChanged,
				int				scrolledX,
				int				scrolledY );

extern void appPrintJobForEditDocument(	struct PrintJob *		pj,
					EditDocument *			ed,
					const struct PrintGeometry *	pg );

extern void appApplicationSettingsToPrintGeometry(
					struct PrintGeometry *	pg,
					EditApplication *	ea );

extern int appDocumentTestCanSave(
				EditApplication *		ea,
				const AppFileExtension *	afe,
				void *				privateData,
				unsigned int			useFlags,
				int				format );

extern int appDocumentGetSaveFormat(
				int *				pSuggestStdout,
				EditApplication *		ea,
				const MemoryBuffer *		filename,
				void *				privateData,
				unsigned int			useFlags,
				int				format );

extern int appFileCanOpen(	const EditApplication *		ea,
				int				format );

int appDocumentGetOpenFormat(	int *				pSuggestStdin,
				const AppFileExtension *	testExts,
				int				testExtCount,
				const MemoryBuffer *		filename,
				int				format );

extern int appPostScriptFontCatalog(		EditApplication *	ea );

extern int appGetPrintDestinations(		EditApplication *	ea );

extern int appReadSystemProperties(	EditApplication *	ea );
extern int appReadUserProperties(	EditApplication *	ea );
extern int appSetUserProperty(		EditApplication *	ea,
					const char *		name,
					const char *		value );

extern int appSetSystemProperty(	EditApplication *	ea,
					const char *		name,
					const char *		value );

extern int appGuiSetFrameTitle(		APP_WIDGET		w,
					const char *		title );

extern void appDocScrollToY(		EditDocument *		ed,
					int			y );
extern void appDocScrollToX(		EditDocument *		ed,
					int			x );

extern void appGuiFocusToWidget(	APP_WIDGET		w );

extern const AppSelectionType * appDocGetSelectionType(
				const EditApplication *		ea,
				const char *			selection );

extern void appInitSelectRectangle(	SelectRectangle *	sr );

extern void guiShowMenuOption(		APP_WIDGET		w,
					int			visible );

extern int appSaveToPs(			EditApplication *	ea,
					DrawingSurface		ds,
					SimpleOutputStream *	sos,
					void *			privateData,
					const MemoryBuffer *	documentTitle,
					int			format );

extern int appMakeDocumentWindow(	EditDocument **		pEd,
					EditApplication *	ea,
					int			readOnly,
					const MemoryBuffer *	title,
					const MemoryBuffer *	filename );

extern void appGetPixelsPerTwip(	EditApplication *	ea );

extern int appDocSaveDocument(	EditDocument *		ed,
				void *			through,
				APP_WIDGET		relative,
				APP_WIDGET		option,
				int			format,
				const MemoryBuffer *	filename );

extern int appDetermineBoolean(	int *			pIval,
				const char *		sVal );

# ifdef __cplusplus
    }
# endif

#   endif
