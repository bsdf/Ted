#   include	<appFrame.h>
#   include	<appInspector.h>

#   ifndef	TED_APP_RESOURCES_H
#   define	TED_APP_RESOURCES_H

/************************************************************************/
/*									*/
/*  Ted specific resources.						*/
/*									*/
/*  Also: Some global data items.					*/
/*									*/
/************************************************************************/

typedef struct TedAppResources
    {
				/****************************************/
				/*  Texts.				*/
				/****************************************/

    char *			tarFileNotRtf;
    char *			tarHeadFootNotReachable;
    char *			tarCannotRecover;

    char *			tarAppFileMenuText;
    char *			tarAppWinMenuText;
    char *			tarDocFileMenuText;
    char *			tarDocEditMenuText;
    char *			tarDocInsertMenuText;
    char *			tarDocWinMenuText;
    char *			tarDocFontMenuText;
    char *			tarDocFormatMenuText;
    char *			tarDocToolMenuText;
    char *			tarDocTableMenuText;
    char *			tarHelpMenuText;
    char *			tarAppHelpFileName;

    char *			tarSelectionColor;
    char *			tarCopiedSelectionColor;
    char *			tarPageNumberFormat;

				/****************************************/
				/*  Defaults.				*/
				/****************************************/
    char *			tarShowTableGridString;
    int				tarShowTableGridInt;

    char *			tarAutoHyphenateString;
    int				tarAutoHyphenateInt;

    char *			tarShadingMeshTwipsString;
    double			tarShadingMeshPointsDouble;

    char *			tarPageGapMMString;
    int				tarPageGapMM;

    char *			tarLenientRtfString;
    int				tarLenientRtfInt;

    char *			tarTraceEditsString;
    int				tarTraceEditsInt;

    char *			tarPdfOutlineString;
    int				tarPdfOutlineInt;

    char *			tarOverridePaperSizeString;
    int				tarOverridePaperSizeInt;

				/****************************************/
				/*  Global data.			*/
				/****************************************/

    AppInspector *		tarInspector;
    EditDocument *		tarManualDocument;

				/****************************************/
				/*  To open images:			*/
				/*  From a file chooser.		*/
				/****************************************/
    AppFileExtension *		tarOpenImageExtensions;
    int				tarOpenImageExtensionCount;

				/****************************************/
				/*  For Ted --Find and Ted --RegFind	*/
				/****************************************/

    const char *		tarFindPattern;
    int				tarFindRegex;
    } TedAppResources;

/************************************************************************/
/*									*/
/*  Routine Declarations.						*/
/*									*/
/************************************************************************/

extern void tedDetermineDefaultSettings(	TedAppResources *	tar );

#   endif	/*  TED_APP_RESOURCES_H	*/
