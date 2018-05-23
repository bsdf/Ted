/************************************************************************/
/*									*/
/*  A find/replace tool.						*/
/*									*/
/************************************************************************/

#   ifndef		APP_FIND_TOOL_H
#   define		APP_FIND_TOOL_H

#   include	"appFrame.h"
#   include	"appInspector.h"

/************************************************************************/
/*									*/
/*  Callbacks from the Find Tool.					*/
/*									*/
/************************************************************************/

typedef int (*FindToolFind)(		void *			target );
typedef void (*FindToolReplace)(	void *			target,
					const char *		guess );
typedef int (*FindToolSetPattern)(	void *			voidea,
					const char *		pattern,
					int			useRegex );

/************************************************************************/
/*									*/
/*  Represents a find tool.						*/
/*									*/
/************************************************************************/

typedef struct FindToolResources
    {
    char *	aftrFindTitle;

    char *	aftrUseRegex;

    char *	aftrReplaceTitle;
    char *	aftrReplaceFound;
    char *	aftrReplaceNext;

    char *	aftrExplicitFocus;
    } FindToolResources;

typedef struct FindTool
    {
    EditApplication *		aftApplication;
    AppInspector *		aftInspector;
    int				aftSubjectPage;

    APP_WIDGET			aftFindFrame;
    APP_WIDGET			aftPatternText;
    APP_WIDGET			aftReplaceText;
    APP_WIDGET			aftReplaceFrame;
    APP_WIDGET			aftReplaceButton;
    APP_WIDGET			aftReplaceNextButton;
    APP_WIDGET			aftFindNextButton;
    APP_WIDGET			aftFindPrevButton;

    APP_WIDGET			aftRegexToggle;

    FindToolFind		aftFindNext;
    FindToolFind		aftFindPrev;
    FindToolReplace		aftReplace;
    FindToolSetPattern		aftSetPattern;

    int				aftReplaceEnabled;
    int				aftLastFindResult;
    int				aftUseRegex;
    int				aftExplicitFocus;
    } FindTool;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void appFillFindTool(	FindTool *			aft,
				const FindToolResources *	aftr,
				AppInspector *			ai,
				int				subjectPage,
				InspectorSubject *		is,
				APP_WIDGET			pageWidget,
				const InspectorSubjectResources * isr );

extern void appFindToolEnableReplace(	FindTool *	aft,
					int		enabled );

extern void appFindToolSetPattern(	FindTool *		aft,
					const char *		pattern,
					int			useRegex );

extern void appFindToolGetResourceTable( EditApplication *		ea,
					FindToolResources *		spr,
					InspectorSubjectResources *	isr );

extern void appInitFindTool(		FindTool *			aft );
extern void appCleanFindTool(		FindTool *			aft );

#   endif	/*	APP_FIND_TOOL_H	*/
