/************************************************************************/
/*  Editor, File actions.						*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<string.h>

#   include	"appFrame.h"

#   include	<appDebugon.h>

const AppSelectionType * appDocGetSelectionType(
				const EditApplication *		ea,
				const char *			selection )
    {
    int					i;
    const AppSelectionType *		ast;

    ast= ea->eaDocSelectionTypes;
    for ( i= 0; i < ea->eaDocSelectionTypeCount; ast++, i++ )
	{
	if  ( ! strcmp( ast->astSelectionString, selection ) )
	    { break;	}
	}

    if  ( i >= ea->eaDocSelectionTypeCount )
	{ return (const AppSelectionType *)0;	}

    return ast;
    }

