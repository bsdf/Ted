/************************************************************************/
/*									*/
/*  A Font/Text attribute Selection Page on an inspector.		*/
/*									*/
/************************************************************************/

#   ifndef APP_FONT_TOOL_H
#   define APP_FONT_TOOL_H

#   include	<docExpandedTextAttribute.h>
#   include	"appFrame.h"
#   include	"appInspector.h"

/************************************************************************/
/*									*/
/*  Represents a font chooser.						*/
/*									*/
/************************************************************************/

typedef struct AppFontToolResources
    {
    char *			aftrFamily;
    char *			aftrFace;
    char *			aftrSize;
    char *			aftrRevert;
    char *			aftrSet;

    char *			aftrTextUnderlined;
    char *			aftrTextStrikethrough;

    char *			aftrSuperscript;
    char *			aftrSubscript;
    char *			aftrBaseline;

    char *			aftrSmallcaps;
    char *			aftrCapitals;
    char *			aftrFaces[FONTface_COUNT];
    } AppFontToolResources;

typedef int (*FontChooserSetFont)(
				void *				target,
				const PropertyMask *		taSetMask,
				const ExpandedTextAttribute *	etaSet );

typedef struct AppFontChooser
    {
    EditApplication *		afcApplication;
    AppInspector *		afcInspector;
    int				afcSubjectPage;
    int				afcCurrentDocumentId;
    int				afcInProgrammaticChange;

    unsigned char		afcCanChange;

    APP_WIDGET			afcSampleDrawing;
    APP_WIDGET			afcScreenName;
    APP_WIDGET			afcPsName;

    APP_WIDGET			afcChooseRow;
    APP_WIDGET				afcFamilyColumn;
    APP_WIDGET					afcFamilyLabel;
    APP_WIDGET					afcFamilyList;
    APP_WIDGET				afcFaceColumn;
    APP_WIDGET					afcFaceLabel;
    APP_WIDGET					afcFaceList;
    APP_WIDGET				afcSizeColumn;
    APP_WIDGET					afcSizeLabel;
    APP_WIDGET					afcSizeList;
    APP_WIDGET					afcSizeText;

    APP_WIDGET			afcToggleRow1;
    APP_WIDGET			afcUnderlinedToggle;
    APP_WIDGET			afcSuperscriptToggle;
    APP_WIDGET			afcSmallcapsToggle;

    APP_WIDGET			afcToggleRow2;
    APP_WIDGET			afcStrikethroughToggle;
    APP_WIDGET			afcSubscriptToggle;
    APP_WIDGET			afcCapitalsToggle;

    APP_WIDGET			afcApplyRow;
    APP_WIDGET			afcRevertButton;
    APP_WIDGET			afcApplyButton;

    APP_WIDGET			afcBaselineRow;
    APP_WIDGET			afcBaselineLabel;
    APP_WIDGET			afcBaselineText;

    int				afcFontSortIndexChosen;
    int				afcFaceChosen;
    PropertyMask		afcChosenMask;
    ExpandedTextAttribute	afcTextAttributeChosen;

    PropertyMask		afcSetMask;
    ExpandedTextAttribute	afcTextAttributeSet;

    FontChooserSetFont		afcSetFont;
    
    DocumentFontList		afcDocumentFontList;

    double			afcPixelsPerTwip;
    DrawingSurface		afcDrawingSurface;
    const PostScriptFontList *	afcPostScriptFontList;
    int				afcScreenFont;
    char			afcChoiceText[120+1];
    char			afcScreenChoiceText[120+1];
    int				afcFaceMapFwd[FONTface_COUNT];
    int				afcFaceMapBck[FONTface_COUNT];
    AppFontToolResources	afcResources;
    } AppFontChooser;

/************************************************************************/
/*									*/
/*  Routine declarations						*/
/*									*/
/************************************************************************/

extern void appFontToolFillPage(AppFontChooser *		afc,
				const AppFontToolResources *	aftr,
				int				subjectPage,
				InspectorSubject *		is,
				APP_WIDGET			pageWidget,
				const InspectorSubjectResources * isr );

extern void appFontToolGetResourceTable(EditApplication *		ea,
					AppFontToolResources *		aftr );

extern void appFontChooserCleanPage( AppFontChooser *	afc );

extern void appFontToolFillChoosers(
				AppFontChooser *		afc,
				const AppFontToolResources *	aftr );

extern void appFontToolFinishPage(
				AppFontChooser *		afc,
				const AppFontToolResources *	aftr );

extern int appFontToolShowCurrentFont(
				AppFontChooser *		afc,
				const PropertyMask *		newMask,
				const TextAttribute *		taNew,
				unsigned int			documentId,
				int				canChange,
				const DocumentFontList *	dfl,
				const ColorPalette *		cp );

#   endif /*	APP_FONT_TOOL_H  */
