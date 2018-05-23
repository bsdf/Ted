/************************************************************************/
/*									*/
/*  Document statistics for the user.					*/
/*									*/
/************************************************************************/

#   include	"docBaseConfig.h"

#   include	"docStatistics.h"

/************************************************************************/
/*									*/
/*  Statistics about a document. Used in the 'Document Properties'	*/
/*  dialog.								*/
/*									*/
/************************************************************************/

void docInitDocumentStatistics(		DocumentStatistics *	ds )
    {
    ds->dsPageCount= 0;
    ds->dsParagraphCount= 0;
    ds->dsLineCount= 0;
    ds->dsWordCount= 0;
    ds->dsCharacterCount= 0;

    return;
    }

