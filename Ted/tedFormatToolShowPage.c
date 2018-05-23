/************************************************************************/
/*									*/
/*  Select a particular format tool page.				*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdio.h>
#   include	<stddef.h>

#   include	"tedFormatTool.h"
#   include	"tedToolFront.h"
#   include	"tedAppResources.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Show the page layout page.						*/
/*									*/
/************************************************************************/

void tedFormatShowPagePage(	EditApplication *	ea )
    {
    TedAppResources *		tar= (TedAppResources *)ea->eaResourceData;

    if  ( ! tar->tarInspector )
	{ XDEB(tar->tarInspector); return;	}

#   if USE_PAGE_TOOL
    appEnableInspectorSubject( tar->tarInspector, TEDtsiPAGE, 1 );

    appInspectorSelectSubject( tar->tarInspector, TEDtsiPAGE );
#   endif

    return;
    }

/************************************************************************/
/*									*/
/*  Show the Hyperlink page.						*/
/*									*/
/************************************************************************/

void tedFormatShowLinkPage(	EditApplication *	ea )
    {
    TedAppResources *		tar= (TedAppResources *)ea->eaResourceData;

    if  ( ! tar->tarInspector )
	{ XDEB(tar->tarInspector); return;	}

#   if USE_LINK_TOOL
    appEnableInspectorSubject( tar->tarInspector, TEDtsiLINK, 1 );

    appInspectorSelectSubject( tar->tarInspector, TEDtsiLINK );
#   endif

    return;
    }

void tedFormatShowTablePage(	EditApplication *	ea )
    {
    TedAppResources *		tar= (TedAppResources *)ea->eaResourceData;

    if  ( ! tar->tarInspector )
	{ XDEB(tar->tarInspector); return;	}

    appEnableInspectorSubject( tar->tarInspector, TEDtsiTABLE, 1 );

    appInspectorSelectSubject( tar->tarInspector, TEDtsiTABLE );

    return;
    }

void tedFormatShowColumnPage(	EditApplication *	ea )
    {
    TedAppResources *		tar= (TedAppResources *)ea->eaResourceData;

    if  ( ! tar->tarInspector )
	{ XDEB(tar->tarInspector); return;	}

    appEnableInspectorSubject( tar->tarInspector, TEDtsiCOLUMN, 1 );

    appInspectorSelectSubject( tar->tarInspector, TEDtsiCOLUMN );

    return;
    }

void tedFormatShowBookmarkPage(	EditApplication *	ea )
    {
    TedAppResources *		tar= (TedAppResources *)ea->eaResourceData;

    if  ( ! tar->tarInspector )
	{ XDEB(tar->tarInspector); return;	}

#   if USE_BOOKMARK_TOOL
    appEnableInspectorSubject( tar->tarInspector, TEDtsiBOOKMARK, 1 );

    appInspectorSelectSubject( tar->tarInspector, TEDtsiBOOKMARK );
#   endif

    return;
    }

/************************************************************************/
/*									*/
/*  Switch to the symbol page of the inspector.				*/
/*									*/
/************************************************************************/

void tedFormatShowSymbolPage(	EditApplication *	ea )
    {
    TedAppResources *		tar= (TedAppResources *)ea->eaResourceData;

    if  ( ! tar->tarInspector )
	{ XDEB(tar->tarInspector); return;	}

    appEnableInspectorSubject( tar->tarInspector, TEDtsiSYMBOL, 1 );

    appInspectorSelectSubject( tar->tarInspector, TEDtsiSYMBOL );

    return;
    }

void tedFormatShowFontPage(	EditApplication *	ea )
    {
    TedAppResources *		tar= (TedAppResources *)ea->eaResourceData;

    if  ( ! tar->tarInspector )
	{ XDEB(tar->tarInspector); return;	}

#   if USE_FONT_TOOL
    appEnableInspectorSubject( tar->tarInspector, TEDtsiFONT, 1 );

    appInspectorSelectSubject( tar->tarInspector, TEDtsiFONT );
#   endif

    return;
    }

/************************************************************************/
/*									*/
/*  Switch to the spell page of the inspector.				*/
/*									*/
/************************************************************************/

void tedFormatShowSpellPage(	EditApplication *	ea )
    {
    TedAppResources *		tar= (TedAppResources *)ea->eaResourceData;

    if  ( ! tar->tarInspector )
	{ XDEB(tar->tarInspector); return;	}

#   if USE_SPELL_TOOL
    appEnableInspectorSubject( tar->tarInspector, TEDtsiSPELL, 1 );

    appInspectorSelectSubject( tar->tarInspector, TEDtsiSPELL );
#   endif

    return;
    }

void tedFormatShowParaLayoutPage( EditApplication *	ea )
    {
    TedAppResources *		tar= (TedAppResources *)ea->eaResourceData;

    if  ( ! tar->tarInspector )
	{ XDEB(tar->tarInspector); return;	}

    appEnableInspectorSubject( tar->tarInspector, TEDtsiPARA_LAY, 1 );

    appInspectorSelectSubject( tar->tarInspector, TEDtsiPARA_LAY );
    }

void tedFormatShowParaTabsPage( EditApplication *	ea )
    {
    TedAppResources *		tar= (TedAppResources *)ea->eaResourceData;

    if  ( ! tar->tarInspector )
	{ XDEB(tar->tarInspector); return;	}

#   if USE_TABS_TOOL
    appEnableInspectorSubject( tar->tarInspector, TEDtsiTABS, 1 );

    appInspectorSelectSubject( tar->tarInspector, TEDtsiTABS );
#   endif
    }

/************************************************************************/
/*									*/
/*  Switch to the find page of the inspector.				*/
/*									*/
/************************************************************************/

void tedFormatShowFindPage(	EditApplication *	ea )
    {
    TedAppResources *		tar= (TedAppResources *)ea->eaResourceData;

    if  ( ! tar->tarInspector )
	{ XDEB(tar->tarInspector); return;	}

#   if USE_FIND_TOOL
    appEnableInspectorSubject( tar->tarInspector, TEDtsiFIND, 1 );

    appInspectorSelectSubject( tar->tarInspector, TEDtsiFIND );
#   endif

    return;
    }
