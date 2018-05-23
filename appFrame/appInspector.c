/************************************************************************/
/*									*/
/*  Utility functionality for 'Inspector' like tools.			*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   include	<stdlib.h>
#   include	<stddef.h>
#   include	<stdio.h>

#   include	"appFrame.h"
#   include	"appInspector.h"
#   include	"guiWidgetsImpl.h"

#   include	"appRgbChooserPage.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Pull a particular subject of the inspector to front.		*/
/*									*/
/************************************************************************/

void appInspectorSelectSubject(	AppInspector *		ai,
				int			subject )
    {
    const int	andMenu= 1;

    appInspectorChoosePage( ai, andMenu, subject );

    ai->aiCurrentSubject= subject;
    }

/************************************************************************/
/*									*/
/*  An inspector tool must be destroyed.				*/
/*									*/
/************************************************************************/

static void appDestroyInspector(	AppInspector *	ai )
    {
    if  ( ai->aiDestroy )
	{ (*ai->aiDestroy)( ai->aiTarget );	}

    if  ( ai->aiTopWidget )
	{ appDestroyShellWidget( ai->aiTopWidget );	}

    if  ( ai->aiSubjects )
	{ free( ai->aiSubjects );	}

    free( ai );

    return;
    }

static APP_CLOSE_CALLBACK_H( appInspectorCloseCall, w, voidai )
    {
    AppInspector *	ai= (AppInspector *)voidai;

    appDestroyInspector( ai );

    return;
    }

/************************************************************************/
/*									*/
/*  Make the pages,							*/
/*  Fill the menu for the pages.					*/
/*									*/
/************************************************************************/

static void appInspectorFillSubjects(
				const InspectorSubjectResources *	isr,
				AppInspector *				ai )
    {
    int				subject;
    InspectorSubject *		is;

    is= ai->aiSubjects;
    for ( subject= 0; subject < ai->aiSubjectCount; is++, isr++, subject++ )
	{
	if  ( appMakeVerticalInspectorPage( &(is->isPage), &(is->isMenuitem),
						    ai, isr->isrSubjectName ) )
	    { LDEB(subject);	}

	is->isEnabled= 1;
	}
    }

int appInspectorAddSubject(	AppInspector *				ai,
				const InspectorSubjectResources *	isr )
    {
    InspectorSubject *	is;

    is= realloc( ai->aiSubjects,
			( ai->aiSubjectCount+ 1 )* sizeof(InspectorSubject) );
    if  ( ! is )
	{ LXDEB(ai->aiSubjectCount,is); return -1;	}
    ai->aiSubjects= is;

    is += ai->aiSubjectCount;
    if  ( appMakeVerticalInspectorPage( &(is->isPage), &(is->isMenuitem),
						    ai, isr->isrSubjectName ) )
	{ LDEB(1); return -1;	}

    is->isEnabled= 1;
    ai->aiSubjectCount++;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Initialize an inspector subject.					*/
/*									*/
/************************************************************************/

static void appInitInspectorSubject(	InspectorSubject *	is )
    {
    is->isPage= (APP_WIDGET)0;
    is->isMenuitem= (APP_WIDGET)0;
    is->isPrivate= (void *)0;
    is->isEnabled= 1;

    is->isNextPrevRow= (APP_WIDGET)0;
    is->isPrevButton= (APP_WIDGET)0;
    is->isNextButton= (APP_WIDGET)0;

    is->isMoveUpButton= (APP_WIDGET)0;
    is->isMoveDownButton= (APP_WIDGET)0;

    is->isSelectButton= (APP_WIDGET)0;
    is->isDeleteButton= (APP_WIDGET)0;

    is->isInsertButton= (APP_WIDGET)0;
    is->isAppendButton= (APP_WIDGET)0;

    is->isApplyRow= (APP_WIDGET)0;
    is->isRevertButton= (APP_WIDGET)0;
    is->isApplyButton= (APP_WIDGET)0;

    is->isGotColor= (InspectorSubjectGotColor)0;
    }

/************************************************************************/
/*									*/
/*  Turn an inspector or one of its subjects on or off.			*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( appCloseInspector, w, voidai )
    {
    AppInspector *	ai= (AppInspector *)voidai;

    appDestroyInspector( ai );

    return;
    }

void appEnableInspector(	AppInspector *	ai,
				int		enabled )
    {
    guiEnableWidget( ai->aiPaned, enabled != 0 );

    return;
    }

void appEnableInspectorSubject(		AppInspector *		ai,
					int			subject ,
					int			enabled )
    {
    appInspectorEnablePage( ai, subject, enabled );

    ai->aiSubjects[subject].isEnabled= enabled != 0;

    return;
    }

static AppConfigurableResource APP_InspectorResourceTable[]=
{
    APP_RESOURCE( "inspectorCloseInspector",
		offsetof(AppInspectorResources,airCloseText),
		"Close" ),
};

AppInspector * appMakeInspector(    EditApplication *		ea,
				    APP_WIDGET			option,
				    InspectorSubjectResources * isr,
				    int				subjectCount,
				    AppToolDestroy		destroy,
				    void *			through )
    {
    AppInspector *		rval= (AppInspector *)0;
    AppInspector *		ai;
    int				subject;

    APP_BITMAP_IMAGE		iconPixmap;
    APP_BITMAP_MASK		iconMask;

    const int			userResizable= 0;

    APP_WIDGET			row;

    static AppInspectorResources	air;
    static int				gotResources;

    if  ( ! gotResources )
	{
	appGuiGetResourceValues( &gotResources, ea, (void *)&air,
					APP_InspectorResourceTable,
					sizeof(APP_InspectorResourceTable)/
					sizeof(AppConfigurableResource) );

	gotResources= 1;
	}

    if  ( appGetImagePixmap( ea, ea->eaMainIcon, &iconPixmap, &iconMask )  )
	{ SDEB(ea->eaMainIcon); return (AppInspector *)0; }

    ai= (AppInspector *)malloc( sizeof(AppInspector) );
    if  ( ! ai )
	{ LXDEB(subjectCount,ai); return ai;	}

    ai->aiApplication= (struct EditApplication *)0;
    ai->aiTopWidget= (APP_WIDGET)0;
    ai->aiPaned= (APP_WIDGET)0;

    appInitOptionmenu( &(ai->aiSubjectOptionmenu) );

    ai->aiSeparator= (APP_WIDGET)0;
    ai->aiPageParent= (APP_WIDGET)0;
    ai->aiSeparator2= (APP_WIDGET)0;
    ai->aiCloseButton= (APP_WIDGET)0;

    ai->aiNotifySubject= (InspectorNotifySubject)0;

    ai->aiRgbChooser= (void *)0; /* RgbChooserPage */
    ai->aiRgbSubjectNumber= -1;

    ai->aiSubjects= (InspectorSubject *)0;
    ai->aiSubjectCount= 0;

    ai->aiDestroy= (AppToolDestroy)0;
    ai->aiTarget= (void *)0;

    ai->aiSubjectCount= -1;
    ai->aiCurrentSubject= -1;
    ai->aiSubjects= (InspectorSubject *)0;

    if  ( subjectCount > 0 )
	{
	InspectorSubject *	is;

	ai->aiSubjects= malloc( subjectCount* sizeof(InspectorSubject) );
	if  ( ! ai->aiSubjects )
	    { LXDEB(subjectCount,ai->aiSubjects); goto ready;	}
	ai->aiSubjectCount= subjectCount;

	is= ai->aiSubjects;
	for ( subject= 0; subject < ai->aiSubjectCount; is++, subject++ )
	    { appInitInspectorSubject( is );	}
	}

    ai->aiApplication= ea;
    ai->aiDestroy= destroy;
    ai->aiTarget= through;


    appMakeVerticalTool( &(ai->aiTopWidget), &(ai->aiPaned), ea,
				iconPixmap, iconMask,
				userResizable,
				option, appInspectorCloseCall, (void *)ai );

    appMakeOptionmenuInColumn( &(ai->aiSubjectOptionmenu), ai->aiPaned,
				    appInspectorPageChosen, (void *)ai );

    appGuiInsertSeparatorInColumn( &(ai->aiSeparator), ai->aiPaned );

    appInspectorMakePageParent( ai );

    appInspectorFillSubjects( isr, ai );

    appGuiInsertSeparatorInColumn( &(ai->aiSeparator2), ai->aiPaned );

    {
	const int	heightResizable= 0;
	const int	showAsDefault= 0;
	const int	colspan= 2;

	row= appMakeRowInColumn( ai->aiPaned, 4, heightResizable );

	appMakeButtonInRow( &(ai->aiCloseButton), row,
			air.airCloseText, appCloseInspector, (void *)ai,
			1, colspan, showAsDefault );
    }

    rval= ai; ai= (AppInspector *)0; /* steal */

  ready:
    if  ( ai )
	{ appDestroyInspector( ai );	}

    return rval;
    }

void appFinishInspector(	AppInspector *		ai )
    {
    appShowShellWidget( ai->aiApplication, ai->aiTopWidget );

    appOptionmenuRefreshWidth( &(ai->aiSubjectOptionmenu) );

    return;
    }

/************************************************************************/
/*									*/
/*  Callback of a possible RGB chooser to an inspector.			*/
/*									*/
/************************************************************************/

void appInspectorGotColor(	AppInspector *			ai,
				int				subjectPage,
				int				property,
				const RGB8Color *		rgb8Set )
    {
    RgbChooserPage *	rcp;
    InspectorSubject *	is;

    if  ( ! ai->aiRgbChooser )
	{ XDEB(ai->aiRgbChooser); return;	}
    rcp= (RgbChooserPage *)ai->aiRgbChooser;

    if  ( ai->aiRgbSubjectNumber < 0			||
	  ai->aiRgbSubjectNumber >= ai->aiSubjectCount	)
	{ LLDEB(ai->aiRgbSubjectNumber,ai->aiSubjectCount); return;	}

    is= ai->aiSubjects+ subjectPage;

    if  ( subjectPage < 0 || subjectPage >= ai->aiSubjectCount )
	{ LLDEB(subjectPage,ai->aiSubjectCount); return;	}

    /**/
    if  ( rgb8Set )
	{
	if  ( ! is->isGotColor )
	    { XDEB(is->isGotColor);					}
	else{ (*is->isGotColor)( is->isPrivate, property, rgb8Set );	}
	}

    /**/
    appEnableInspectorSubject( ai, subjectPage, 1 );
    appInspectorSelectSubject( ai, subjectPage );

    /**/
    appEnableInspectorSubject( ai, ai->aiRgbSubjectNumber, 0 );
    appRgbChooserSetContext( rcp, -1, -1 );

    return;
    }

/************************************************************************/
/*									*/
/*  Switch to the RGB color chooser page.				*/
/*									*/
/************************************************************************/

void appInspectorSetRgbPage(		AppInspector *		ai,
					void *			vrcp,
					int			subjectNumber )
    {
    ai->aiRgbChooser= vrcp;
    ai->aiRgbSubjectNumber= subjectNumber;

    return;
    }

void appInspectorShowRgbPage(		AppInspector *		ai,
					int			fromSubject,
					int			fromProperty,
					const RGB8Color *	rgb8 )
    {
    RgbChooserPage *	rcp;
    InspectorSubject *	is;

    int			enabled= 1;

    if  ( ! ai->aiRgbChooser )
	{ XDEB(ai->aiRgbChooser); return;	}
    rcp= (RgbChooserPage *)ai->aiRgbChooser;

    if  ( ai->aiRgbSubjectNumber < 0			||
	  ai->aiRgbSubjectNumber >= ai->aiSubjectCount	)
	{ LLDEB(ai->aiRgbSubjectNumber,ai->aiSubjectCount); return;	}

    is= ai->aiSubjects+ ai->aiRgbSubjectNumber;

    appRgbChooserRefresh( rcp, &enabled, is, rgb8 );

    appRgbChooserSetContext( rcp, fromSubject, fromProperty );

    appEnableInspectorSubject( ai, ai->aiRgbSubjectNumber, 1 );

    appInspectorSelectSubject( ai, ai->aiRgbSubjectNumber );

    return;
    }

/************************************************************************/
/*									*/
/*  Callback when an inspector page is chosen.				*/
/*									*/
/************************************************************************/

void appInspectorPageChosen(		int		subject,
					void *		vai )
    {
    AppInspector *	ai= (AppInspector *)vai;

    const int		andMenu= 0;

    if  ( subject >= ai->aiSubjectCount )
	{ LDEB(ai->aiSubjectCount); return;	}

    if  ( ai->aiNotifySubject )
	{ (*ai->aiNotifySubject)( ai, ai->aiCurrentSubject, subject ); }

    appInspectorChoosePage( ai, andMenu, subject );

    ai->aiCurrentSubject= subject;

    return;
    }

