/************************************************************************/
/*									*/
/*  Manipulate lists.							*/
/*									*/
/************************************************************************/

#   include	"docEditConfig.h"

#   include	<stdio.h>

#   include	<appDebugon.h>

#   include	<docBuf.h>
#   include	<docField.h>
#   include	<docDocumentList.h>
#   include	"docEdit.h"

/************************************************************************/
/*									*/
/*  Change the properties of the list where the selection starts.	*/
/*									*/
/************************************************************************/

int docEditChangeList(		EditOperation *		eo,
				DocumentList *		dl,
				struct ListOverride *	lo,
				const DocumentList *	dlNew )
    {
    const int				copyIds= 0;
    const int * const			fontMap= (const int *)0;
    const int * const			colorMap= (const int *)0;
    const int * const			rulerMap= (const int *)0;

    if  ( docCopyDocumentList( dl, dlNew,
				    copyIds, fontMap, colorMap, rulerMap ) )
	{ LDEB(1); return -1;	}

    if  ( docApplyListRuler( dl, lo, eo->eoDocument ) )
	{ LDEB(1);	}

    eo->eoFieldUpdate |= FIELDdoLISTTEXT;

    return 0;
    }

