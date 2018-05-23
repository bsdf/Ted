/************************************************************************/
/*									*/
/*  A find/replace tool.						*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   include	<stdio.h>
#   include	<stddef.h>
#   include	<string.h>

#   include	"appFrame.h"
#   include	"appFindTool.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Turn buttons on and off depending on the situation.			*/
/*									*/
/************************************************************************/

static void appFindToolReflectPatternText(	FindTool *	aft )
    {
    int		sensitive;
    char *	pattern;

    pattern= appGetStringFromTextWidget( aft->aftPatternText );

    sensitive= pattern[0] != '\0';

    appFreeStringFromTextWidget( pattern );

    guiEnableWidget( aft->aftFindNextButton, sensitive );
    guiEnableWidget( aft->aftFindPrevButton, sensitive );

    return;
    }

static void appFindToolReflectReplaceText(	FindTool *	aft )
    {
    int		sensitive;
    char *	replacement;

    replacement= appGetStringFromTextWidget( aft->aftReplaceText );

    sensitive= ( replacement[0] != '\0' || aft->aftUseRegex )	&&
		 aft->aftLastFindResult == 0;

    appFreeStringFromTextWidget( replacement );

    guiEnableWidget( aft->aftReplaceButton, sensitive );
    guiEnableWidget( aft->aftReplaceNextButton, sensitive );

    return;
    }

static void appFindToolSetFindResult(	FindTool *	aft,
					int		result )
    {
    int		sensitive;

    aft->aftLastFindResult= result;

    if  ( result )
	{ sensitive= 0;	}
    else{
	char *	replacement;

	guiEnableWidget( aft->aftReplaceFrame, aft->aftReplaceEnabled );
	guiEnableText( aft->aftReplaceText, aft->aftReplaceEnabled );

	replacement= appGetStringFromTextWidget( aft->aftReplaceText );

	sensitive= ( replacement[0] != '\0' || aft->aftUseRegex );

	appFreeStringFromTextWidget( replacement );
	}

    guiEnableWidget( aft->aftReplaceButton, sensitive );
    guiEnableWidget( aft->aftReplaceNextButton, sensitive );

    return;
    }

static APP_TXTYPING_CALLBACK_H( appFindToolPatternChanged, w, voidaft )
    {
    FindTool *	aft= (FindTool *)voidaft;

    appFindToolReflectPatternText( aft );

    return;
    }

static APP_TXTYPING_CALLBACK_H( appFindToolReplacementChanged, w, voidaft )
    {
    FindTool *	aft= (FindTool *)voidaft;

    appFindToolReflectReplaceText( aft );

    return;
    }

/************************************************************************/
/*									*/
/*  'Find' button has been pushed.					*/
/*									*/
/************************************************************************/

static int appFindToolSetProgram(	FindTool *	aft )
    {
    char *		pattern;
    int			res;

    if  ( ! aft->aftSetPattern )
	{ XDEB(aft->aftSetPattern); return -1;	}

    pattern= appGetStringFromTextWidget( aft->aftPatternText );

    res= (*aft->aftSetPattern)( aft->aftApplication, pattern, aft->aftUseRegex );

    appFreeStringFromTextWidget( pattern );

    return res;
    }

static void appFindToolFindNext(	FindTool *	aft )
    {
    int			result;

    if  ( ! aft->aftFindNext )
	{
	XDEB(aft->aftFindNext);
	appFindToolSetFindResult( aft, -1 ); return;
	}

    if  ( appFindToolSetProgram( aft ) )
	{ appFindToolSetFindResult( aft, -1 ); return; }

    result= (*aft->aftFindNext)( aft->aftApplication );
    appFindToolSetFindResult( aft, result );

    return;
    }

static APP_BUTTON_CALLBACK_H( appFindToolFindNextPushed, w, voidaft )
    {
    FindTool *	aft= (FindTool *)voidaft;

    appFindToolFindNext( aft );

    return;
    }

static APP_BUTTON_CALLBACK_H( appFindToolFindPrev, w, voidaft )
    {
    FindTool *	aft= (FindTool *)voidaft;
    int			result;

    if  ( ! aft->aftFindPrev )
	{
	XDEB(aft->aftFindPrev);
	appFindToolSetFindResult( aft, -1 ); return;
	}

    if  ( appFindToolSetProgram( aft ) )
	{ appFindToolSetFindResult( aft, -1 ); return; }

    result= (*aft->aftFindPrev)( aft->aftApplication );
    appFindToolSetFindResult( aft, result );

    return;
    }

/************************************************************************/
/*									*/
/*  'Replace' button has been pushed.					*/
/*									*/
/************************************************************************/

static void appFindToolReplaceSelection( FindTool *	aft )
    {
    char *		replacement;

    if  ( ! aft->aftReplace )
	{ XDEB(aft->aftReplace); return;	}

    replacement= appGetStringFromTextWidget( aft->aftReplaceText );

    (*aft->aftReplace)( aft->aftApplication, replacement );

    appFreeStringFromTextWidget( replacement );

    guiEnableWidget( aft->aftReplaceButton, 0 );

    return;
    }

static APP_BUTTON_CALLBACK_H( appFindToolReplaceSelectionPushed, w, voidaft )
    {
    FindTool *	aft= (FindTool *)voidaft;

    appFindToolReplaceSelection( aft );
    }

static APP_BUTTON_CALLBACK_H( appFindToolReplaceNext, w, voidaft )
    {
    FindTool *	aft= (FindTool *)voidaft;

    appFindToolReplaceSelection( aft );

    appFindToolFindNext( aft );

    return;
    }

static APP_TOGGLE_CALLBACK_H( appFindRegexToggled, w, voidaft, voidtbcs )
    {
    FindTool *	aft= (FindTool *)voidaft;
    int			set;

    set= appGuiGetToggleStateFromCallback( w, voidtbcs );

    aft->aftUseRegex= ( set != 0 );

    appFindToolReflectReplaceText( aft );

    return;
    }

/************************************************************************/
/*									*/
/*  Make a row of buttons for the Find Tool.				*/
/*									*/
/************************************************************************/

static void appFindToolMakeButtonRow(	APP_WIDGET *		pRow,
					APP_WIDGET		parent,
					APP_WIDGET *		pLeftButton,
					APP_WIDGET *		pRightButton,
					const char *		leftLabel,
					const char *		rightLabel,
					APP_BUTTON_CALLBACK_T	leftCallback,
					APP_BUTTON_CALLBACK_T	rightCallback,
					FindTool *		aft )
    {
    APP_WIDGET		row;

    APP_WIDGET		leftButton;
    APP_WIDGET		rightButton;

    const int		heightResizable= 0;
    const int		showAsDefault= 0;
    const int		colspan= 1;

    row= appMakeRowInColumn( parent, 2, heightResizable );

    appMakeButtonInRow( &(leftButton), row, leftLabel,
		    leftCallback, (void *)aft, 0, colspan, showAsDefault );
    appMakeButtonInRow( &(rightButton), row, rightLabel,
		    rightCallback, (void *)aft, 1, colspan, showAsDefault );

#   ifdef USE_MOTIF
    XtVaSetValues( XtParent( parent ),
			XmNdefaultButton,	leftButton,
			NULL );
#   endif


    *pLeftButton= leftButton, *pRightButton= rightButton; return;
    }

/************************************************************************/
/*									*/
/*  Make the button part of the find Tool.				*/
/*									*/
/************************************************************************/

static APP_WIDGET appFindMakeFindFrame(	APP_WIDGET			parent,
					const FindToolResources *	aftr,
					const InspectorSubjectResources * isr,
					FindTool *			aft )
    {
    APP_WIDGET		frame;
    APP_WIDGET		paned;
    APP_WIDGET		row;

    const int		textEnabled= 1;
    const int		heightResizable= 0;

    appMakeColumnFrameInColumn( &frame, &paned, parent, aftr->aftrFindTitle );

    appMakeTextInColumn( &(aft->aftPatternText), paned, 0, textEnabled );

    appGuiSetTypingCallbackForText( aft->aftPatternText,
				    appFindToolPatternChanged, (void *)aft );

#   ifdef USE_MOTIF
    XtVaSetValues( XtParent( paned ),
			XmNinitialFocus,	aft->aftPatternText,
			NULL );
#   endif

    /***************/

    row= appMakeRowInColumn( paned, 1, heightResizable );
    aft->aftRegexToggle= appMakeToggleInRow( row, aftr->aftrUseRegex,
				    appFindRegexToggled, (void *)aft, 0, 1 );

    /***************/

    appFindToolMakeButtonRow( &row, paned,
		&(aft->aftFindNextButton), &(aft->aftFindPrevButton),
		isr->isrNextButtonText, isr->isrPrevButtonText,
		appFindToolFindNextPushed, appFindToolFindPrev,
		aft );

    /***************/

    return frame;
    }

/************************************************************************/
/*									*/
/*  Make the replace part of the find tool.				*/
/*									*/
/************************************************************************/

static void appFindMakeReplaceFrame(
					APP_WIDGET			parent,
					const FindToolResources *	aftr,
					FindTool *			aft )
    {
    APP_WIDGET		row;
    APP_WIDGET		paned;

    const int		textEnabled= 1;

    appMakeColumnFrameInColumn( &(aft->aftReplaceFrame), &paned,
					    parent, aftr->aftrReplaceTitle );

    appMakeTextInColumn( &(aft->aftReplaceText), paned, 0, textEnabled );

    appGuiSetTypingCallbackForText( aft->aftReplaceText,
				appFindToolReplacementChanged, (void *)aft );

#   ifdef USE_MOTIF
    XtVaSetValues( XtParent( paned ),
			XmNinitialFocus,	aft->aftReplaceText,
			NULL );
#   endif

    /***************/

    appFindToolMakeButtonRow( &row, paned,
		&(aft->aftReplaceButton), &(aft->aftReplaceNextButton),
		aftr->aftrReplaceFound, aftr->aftrReplaceNext,
		appFindToolReplaceSelectionPushed, appFindToolReplaceNext,
		aft );

    }

void appFillFindTool(		FindTool *			aft,
				const FindToolResources *	aftr,
				AppInspector *			ai,
				int				subjectPage,
				InspectorSubject *		is,
				APP_WIDGET			pageWidget,
				const InspectorSubjectResources * isr )
    {
    aft->aftFindFrame= appFindMakeFindFrame( pageWidget, aftr, isr, aft );

    appFindMakeReplaceFrame( pageWidget, aftr, aft );

    aft->aftExplicitFocus= 0;
    if  ( aftr->aftrExplicitFocus			&&
	  ! strcmp( aftr->aftrExplicitFocus, "1" )	)
	{ aft->aftExplicitFocus= 1;	}

    appFindToolReflectPatternText( aft );
    appFindToolReflectReplaceText( aft );
    return;
    }

void appFindToolEnableReplace(	FindTool *	aft,
				int		enabled )
    {
    aft->aftReplaceEnabled= enabled;

    guiEnableWidget( aft->aftReplaceFrame, aft->aftReplaceEnabled );
    guiEnableText( aft->aftReplaceText, aft->aftReplaceEnabled );

    return;
    }

void appFindToolSetPattern(	FindTool *		aft,
				const char *		pattern,
				int			useRegex )
    {
    aft->aftUseRegex= useRegex;

    appStringToTextWidget( aft->aftPatternText, (const char *)pattern );

    appGuiSetToggleState( aft->aftRegexToggle, aft->aftUseRegex );

    appFindToolReflectPatternText( aft );

    return;
    }

void appCleanFindTool(		FindTool *		aft )
    {
    return;
    }

void appInitFindTool(		FindTool *		aft )
    {
    aft->aftApplication= (EditApplication *)0;
    aft->aftInspector= (AppInspector *)0;
    aft->aftSubjectPage= -1;

    aft->aftPatternText= (APP_WIDGET)0;
    aft->aftReplaceText= (APP_WIDGET)0;
    aft->aftReplaceFrame= (APP_WIDGET)0;

    aft->aftReplaceFrame= (APP_WIDGET)0;
    aft->aftReplaceButton= (APP_WIDGET)0;
    aft->aftReplaceNextButton= (APP_WIDGET)0;
    aft->aftFindNextButton= (APP_WIDGET)0;
    aft->aftFindPrevButton= (APP_WIDGET)0;

    aft->aftRegexToggle= (APP_WIDGET)0;

    aft->aftFindNext= (FindToolFind)0;
    aft->aftFindPrev= (FindToolFind)0;
    aft->aftReplace= (FindToolReplace)0;
    aft->aftSetPattern= (FindToolSetPattern)0;

    aft->aftReplaceEnabled= 1;
    aft->aftLastFindResult= 1;
    aft->aftUseRegex= 0;
    aft->aftExplicitFocus= 0;
    }

/************************************************************************/
/*									*/
/*  Resources.								*/
/*									*/
/************************************************************************/

static AppConfigurableResource APP_FindToolSubjectResourceTable[]=
    {
    APP_RESOURCE( "findToolFindTitle",
		offsetof(InspectorSubjectResources,isrSubjectName),
		"Find" ),
    APP_RESOURCE( "findToolFindNext",
		offsetof(InspectorSubjectResources,isrNextButtonText),
		"Find" ),
    APP_RESOURCE( "findToolFindPrevious",
		offsetof(InspectorSubjectResources,isrPrevButtonText),
		"Previous" ),
    };

static AppConfigurableResource APP_FindToolResourceTable[]=
    {
    APP_RESOURCE( "findToolFindTitle",
		offsetof(FindToolResources,aftrFindTitle),
		"Find" ),

    APP_RESOURCE( "findToolUseRegex",
		offsetof(FindToolResources,aftrUseRegex),
		"Find Regular Expression" ),

    APP_RESOURCE( "findToolReplaceTitle",
		offsetof(FindToolResources,aftrReplaceTitle),
		"Replace" ),

    APP_RESOURCE( "findToolReplaceFound",
		offsetof(FindToolResources,aftrReplaceFound),
		"Replace" ),
    APP_RESOURCE( "findToolReplaceNext",
		offsetof(FindToolResources,aftrReplaceNext),
		"Replace, Next" ),

    APP_RESOURCE( "findToolExplicitFocus",
		offsetof(FindToolResources,aftrExplicitFocus),
		"1" ),
    };

void appFindToolGetResourceTable(	EditApplication *		ea,
					FindToolResources *		spr,
					InspectorSubjectResources *	isr )
    {
    static int	gotToolResources= 0;
    static int	gotSubjectResources= 0;

    if  ( ! gotToolResources )
	{
	appGuiGetResourceValues( &gotToolResources, ea, (void *)spr,
					APP_FindToolResourceTable,
					sizeof(APP_FindToolResourceTable)/
					sizeof(AppConfigurableResource) );
	}

    if  ( ! gotSubjectResources )
	{
	appGuiGetResourceValues( &gotSubjectResources, ea, (void *)isr,
				APP_FindToolSubjectResourceTable,
				sizeof(APP_FindToolSubjectResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    return;
    }
