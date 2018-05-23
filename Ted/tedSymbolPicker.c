/************************************************************************/
/*									*/
/*  Ted: Interface to the generic symbol picker.			*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	"tedSymbolPicker.h"
#   include	"tedApp.h"
#   include	"tedEdit.h"
#   include	"tedDocument.h"
#   include	"tedAppResources.h"
#   include	<appSymbolPicker.h>
#   include	<docEditCommand.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Adapt fonts tool to the current document.				*/
/*									*/
/************************************************************************/

void tedRefreshSymbolPicker(	SymbolPicker *			sp,
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

    if  ( sd->sdIsListBullet )
	{ *pEnabled= 0; return;	}

    if  ( appAdaptSymbolPickerToFontFamily( sp, documentId, dfl,
			&(sd->sdTextAttributeMask), &(sd->sdTextAttribute) ) )
	{ LDEB(1);	}

    appEnableSymbolPicker( sp, cmdEnabled[EDITcmdREPLACE] );

    *pEnabled= 1;
    return;
    }

/************************************************************************/
/*									*/
/*  Insert callback for the symbol picker.				*/
/*									*/
/************************************************************************/

void tedSymbolPickerInsert(	void *			voidea,
				const char *		bytes,
				int			size,
				const TextAttribute *	taSet,
				const PropertyMask *	taSetMask )
    {
    EditApplication *		ea= (EditApplication *)voidea;
    EditDocument *		ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return;	}

    tedDocInsertStringWithAttribute( ed, bytes, size, taSet, taSetMask, ((TedDocument *)ed->edPrivateData)->tdTraced );

    return;
    }

