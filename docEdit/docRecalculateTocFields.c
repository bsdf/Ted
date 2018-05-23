/************************************************************************/
/*									*/
/*  Evaluate fields+ the list of kinds of fields.			*/
/*									*/
/************************************************************************/

#   include	"docEditConfig.h"

#   include	<appDebugon.h>

#   include	<docBuf.h>
#   include	"docTocBookmarks.h"
#   include	"docRecalculateTocField.h"
#   include	<docTreeType.h>

/************************************************************************/
/*									*/
/*  Recalculate TOC fields.						*/
/*									*/
/*  1)  See whether there are any TOC fields.				*/
/*  2)  Make sure that all TOC candidates have valid bookmarls.		*/
/*  3)  Calculate the table(s) of contents.				*/
/*									*/
/************************************************************************/

int docRecalculateTocFields( RecalculateFields *		rf )
    {
    BufferDocument *	bdDoc= rf->rfDocument;
    DocumentFieldList *	dfl= &(bdDoc->bdFieldList);
    const int		fieldCount= dfl->dflPagedList.plItemCount;
    int			fieldNr;

    /*  2  */
    docRemoveUnbalancedTocBookmarks( bdDoc );

    if  ( docSetTocBookmarks( bdDoc ) )
	{ LDEB(1); return -1;	}

    /*  3  */
    for ( fieldNr= 0; fieldNr < fieldCount; fieldNr++ )
	{
	DocumentField *		df= docGetFieldByNumber( dfl, fieldNr );

	if  ( ! df						||
	      df->dfKind != DOCfkTOC				||
	      df->dfSelectionScope.ssTreeType != DOCinBODY	)
	    { continue;	}

	if  ( docRecalculateOneTocField( bdDoc, df ) )
	    { LDEB(fieldNr); return -1;	}
	}

    return 0;
    }

