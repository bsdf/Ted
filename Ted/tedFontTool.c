/************************************************************************/
/*									*/
/*  The 'Font Tool':							*/
/*									*/
/*  Most of the ont tool calls directly go to the AppFontTool.		*/
/*  the document.							*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdio.h>
#   include	<stddef.h>

#   include	"tedFontTool.h"
#   include	"tedAppResources.h"
#   include	"tedAppFront.h"
#   include	<guiToolUtil.h>
#   include	<docListOverride.h>
#   include	<docDocumentList.h>
#   include	<docEditCommand.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Intermediary routine to refresh the text font tool.			*/
/*									*/
/************************************************************************/

void tedRefreshFontTool(	AppFontChooser *		afc,
				int *				pEnabled,
				int *				pPref,
				InspectorSubject *		is,
				const DocumentSelection *	ds,
				const SelectionDescription *	sd,
				const BufferDocument *		bd,
				int				documentId,
				const unsigned char *		cmdEnabled )
    {
    const DocumentProperties *	dp= &(bd->bdProperties);
    const DocumentFontList *	dfl= dp->dpFontList;

    if  ( sd->sdIsObjectSelection )
	{ *pEnabled= 0; return;	}

    if  ( sd->sdIsListBullet )
	{
	ListOverride *		lo;
	DocumentList *		dl;
	ListLevel *		ll;

	afc->afcSetFont= tedAppListFontToolSet;

	if  ( docGetListOfParagraph( &lo, &dl, sd->sdListOverride, bd ) )
	    { LDEB(1); *pEnabled= 0; return;	}
	ll= &(dl->dlLevels[sd->sdListLevel]);

	if  ( appFontToolShowCurrentFont( afc, &(ll->llTextAttributeMask),
				    &(ll->llTextAttribute), documentId,
				    cmdEnabled[EDITcmdUPD_LIST],
				    dfl, dp->dpColorPalette ) )
	    { LDEB(1);	}

	*pEnabled= 1;
	}
    else{
	afc->afcSetFont= tedAppFontToolSet;
	(*pPref)++;

	if  ( appFontToolShowCurrentFont( afc, &(sd->sdTextAttributeMask),
				    &(sd->sdTextAttribute), documentId,
				    cmdEnabled[EDITcmdUPD_SPAN_PROPS],
				    dfl, dp->dpColorPalette ) )
	    { LDEB(1);	}

	*pEnabled= 1;
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Resources.								*/
/*									*/
/************************************************************************/

static AppConfigurableResource TED_FontToolSubjectResourceTable[]=
    {
    APP_RESOURCE( "fontToolFont",
		offsetof(InspectorSubjectResources,isrSubjectName),
		"Font" ),
    APP_RESOURCE( "fontToolSet",
		offsetof(InspectorSubjectResources,isrApplyToSubject),
		"Set" ),
    APP_RESOURCE( "fontToolRevert",
		offsetof(InspectorSubjectResources,isrRevert),
		"Revert" ),
    };

void tedFontToolGetResourceTable(	EditApplication *		ea,
					AppFontToolResources *		aftr,
					InspectorSubjectResources *	isr )
    {
    static int	gotSubjectResources= 0;

    appFontToolGetResourceTable( ea, aftr );

    if  ( ! gotSubjectResources )
	{
	appGuiGetResourceValues( &gotSubjectResources, ea, (void *)isr,
				TED_FontToolSubjectResourceTable,
				sizeof(TED_FontToolSubjectResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    return;
    }

