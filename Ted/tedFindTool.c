/************************************************************************/
/*									*/
/*  Ted: Interface to the generic find tool.				*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	"tedApp.h"
#   include	"tedAppFront.h"
#   include	"tedFormatTool.h"
#   include	"tedAppResources.h"
#   include	"tedFind.h"
#   include	"tedFindTool.h"
#   include	<guiToolUtil.h>
#   include	<docEditCommand.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Enter a predefined pattern in the format tool.			*/
/*									*/
/************************************************************************/

void tedFormatSetFindPattern(	EditApplication *	ea,
				const char *		findPattern,
				int			findRegexp )
    {
#   if USE_FIND_TOOL
    TedAppResources *		tar= (TedAppResources *)ea->eaResourceData;
    AppInspector *		ai= tar->tarInspector;
    TedFormatTool *		tft;
    FindTool *			aft;

    if  ( ! tar->tarInspector )
	{ XDEB(tar->tarInspector); return;	}
    tft= (TedFormatTool *)ai->aiTarget;
    aft= &(tft->tftFindTool);

    appFindToolSetPattern( aft, findPattern, findRegexp );

#   endif
    return;
    }

/************************************************************************/
/*									*/
/*  Adapt find tool to the current document.				*/
/*									*/
/************************************************************************/

void tedRefreshFindTool(	FindTool *			aft,
				int *				pEnabled,
				int *				pPref,
				InspectorSubject *		is,
				const DocumentSelection *	ds,
				const SelectionDescription *	sd,
				const BufferDocument *		bd,
				int				documentId,
				const unsigned char *		cmdEnabled )
    {
    appFindToolEnableReplace( aft, cmdEnabled[EDITcmdREPLACE] );
    return;
    }

/************************************************************************/
/*									*/
/*  Build a find tool and connect it to Ted.				*/
/*									*/
/************************************************************************/

void tedFillFindTool(		FindTool *			aft,
				const FindToolResources *	aftr,
				AppInspector *			ai,
				int				subjectPage,
				InspectorSubject *		is,
				APP_WIDGET			pageWidget,
				const InspectorSubjectResources * isr )
    {
    appFillFindTool( aft, aftr, ai, subjectPage, is, pageWidget, isr );

    aft->aftFindNext= tedAppFindNext;
    aft->aftFindPrev= tedAppFindPrev;
    aft->aftSetPattern= tedFindToolSetPattern;
    aft->aftReplace= tedAppReplace;

    return;
    }
