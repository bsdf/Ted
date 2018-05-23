/************************************************************************/
/*									*/
/*  Utility routines for the FormatTool.				*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdio.h>
#   include	<stddef.h>
#   include	<limits.h>

#   include	<appUnit.h>

#   include	"tedApp.h"
#   include	"tedToolUtil.h"
#   include	"tedDocument.h"
#   include	<guiTextUtil.h>
#   include	<docEditCommand.h>
#   include	<docTreeNode.h>
#   include	<docNodeTree.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Validate a dimension						*/
/*									*/
/************************************************************************/

int tedFormatValidateDimension(		int *		pNewValue,
					int *		pChanged,
					APP_WIDGET	w,
					int		oldValue )
    {
    int				changed;

    const int			minValue= 1;
    const int			adaptToMin= 0;
    const int			maxValue= INT_MAX;
    const int			adaptToMax= 0;

    if  ( ! appGetLengthFromTextWidget( w, &oldValue, &changed, UNITtyPOINTS,
				minValue, adaptToMin, maxValue, adaptToMax ) )
	{
	appLengthToTextWidget( w, oldValue, UNITtyPOINTS );

	if  ( changed )
	    { *pNewValue= oldValue;	}

	*pChanged= changed; return 0;
	}

    return -1;
    }

BufferDocument * tedFormatCurDoc(	EditDocument **		pEd,
					int *			pTraced,
					EditApplication *	ea )
    {
    EditDocument *		ed= ea->eaCurrentDocument;

    TedDocument *		td;
    BufferDocument *		bd;

    if  ( ! ed )
	{ XDEB(ed); return (BufferDocument *)0;	}

    td= (TedDocument *)ed->edPrivateData;
    bd= td->tdDocument;

    if  ( pTraced )
	{ *pTraced= td->tdTraced;	}

    *pEd= ed; return bd;
    }


void tedRefreshParaSubjectControls(
				InspectorSubject *		is,
				const DocumentSelection *	ds,
				const SelectionGeometry *	sg,
				const SelectionDescription *	sd,
				const unsigned char *		cmdEnabled )
    {
    BufferItem *			paraNode= ds->dsHead.dpNode;

    guiEnableWidget( is->isDeleteButton, cmdEnabled[EDITcmdDELETE_PARA] );

    guiEnableWidget( is->isSelectButton, sd->sdInContiguousParagraphs );
    guiEnableWidget( is->isInsertButton, cmdEnabled[EDITcmdINSERT_PARA] );
    guiEnableWidget( is->isAppendButton, cmdEnabled[EDITcmdAPPEND_PARA] );

    guiEnableWidget( is->isPrevButton,
				sd->sdInContiguousParagraphs		&&
				docPrevParagraph( paraNode ) != (BufferItem *)0 );
    guiEnableWidget( is->isNextButton,
				sd->sdInContiguousParagraphs		&&
				docNextParagraph( paraNode ) != (BufferItem *)0 );

    guiEnableWidget( is->isRevertButton, cmdEnabled[EDITcmdUPD_PARA_PROPS] );
    guiEnableWidget( is->isApplyButton, cmdEnabled[EDITcmdUPD_PARA_PROPS] );

    return;
    }

