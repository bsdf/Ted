/************************************************************************/
/*									*/
/*  Intermediary functionality for the page layout page.		*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdio.h>
#   include	<stddef.h>

#   include	"tedPageLayoutTool.h"
#   include	<docTreeType.h>
#   include	<guiToolUtil.h>
#   include	<docTreeNode.h>
#   include	<docNodeTree.h>
#   include	<docEditCommand.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Refresh the page layout tool.					*/
/*									*/
/************************************************************************/

void tedRefreshPageLayoutTool(	PageLayoutTool *		plt,
				int *				pEnabled,
				int *				pPref,
				InspectorSubject *		is,
				const DocumentSelection *	ds,
				const SelectionDescription *	sd,
				const BufferDocument *		bd,
				const unsigned char *		cmdEnabled )
    {
    const BufferItem *		bodySectNode;
    const DocumentProperties *	dp= &(bd->bdProperties);

    bodySectNode= docGetBodySectNode( ds->dsHead.dpNode, bd );
    if  ( ! bodySectNode )
	{ XDEB(bodySectNode); *pEnabled= 0; return;	}

    plt->pltCanChangeSelection= cmdEnabled[EDITcmdUPD_SECT_PROPS];
    plt->pltCanChangeDocument= cmdEnabled[EDITcmdUPD_SECTDOC_PROPS];

    appPageLayoutPageRefresh( plt, is,
		&(bodySectNode->biSectDocumentGeometry), &(dp->dpGeometry) );

    *pEnabled= sd->sdInTreeType == DOCinBODY;

    return;
    }

/************************************************************************/
/*									*/
/*  Make a page layout page customized for Ted.				*/
/*									*/
/************************************************************************/

void tedFormatFillPageLayoutPage(
			PageLayoutTool *			plt,
			const PageLayoutPageResources *		plpr,
			InspectorSubject *			is,
			APP_WIDGET				pageWidget,
			const InspectorSubjectResources *	isr )
    {
    const int		doHdFt= 1;
    const int		doSelection= 1;

    appPageLayoutPageFillPage( plt, doHdFt, doSelection,
						plpr, is, pageWidget, isr );
    }


/************************************************************************/
/*									*/
/*  Finish the page layout page.					*/
/*									*/
/************************************************************************/

void tedFormatFinishPageLayoutPage(	PageLayoutTool *		plt,
					const PageLayoutPageResources *	plpr )
    {
    appPageLayoutPageFinishPage( plt, plpr );
    }

/************************************************************************/
/*									*/
/*  Clean the page layout tool.						*/
/*									*/
/************************************************************************/

void tedFormatCleanPageLayoutTool(	PageLayoutTool *	plt )
    {
    appCleanPageLayoutTool( plt );

    return;
    }
