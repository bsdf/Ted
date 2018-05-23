/************************************************************************/
/*									*/
/*  Shift the indentation level of the selection.			*/
/*									*/
/************************************************************************/

#   include	"docEditConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	"docEdit.h"
#   include	<docTreeNode.h>
#   include	<docNodeTree.h>
#   include	<docDocumentList.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Increase/Decrease the indentation level of the paragraphs in the	*/
/*  selection.								*/
/*									*/
/************************************************************************/

int docEditShiftIndent(		EditOperation *	eo,
				int		step )
    {
    int				rval= 0;

    BufferDocument *		bd= eo->eoDocument;

    BufferItem *		paraNode;

    ParagraphProperties		ppSet;

    docInitParagraphProperties( &ppSet );

    paraNode= eo->eoHeadDp.dpNode;
    while( paraNode )
	{
	PropertyMask	ppDoneMask;
	PropertyMask	ppSetMask;
	int		apply= 0;

	utilPropMaskClear( &ppDoneMask );
	utilPropMaskClear( &ppSetMask );

	if  ( paraNode->biParaListOverride >= 1 )
	    {
	    struct ListOverride *	lo;
	    DocumentList *		dl;
	    int				ilvl;

	    ilvl= paraNode->biParaListLevel;
	    ilvl += step;

	    if  ( docGetListOfParagraph( &lo, &dl,
					    paraNode->biParaListOverride, bd ) )
		{ LDEB(paraNode->biParaListOverride); rval= -1; goto ready; }

	    if  ( ilvl >= 0 )
		{
		if  ( ilvl < dl->dlLevelCount )
		    {
		    PROPmaskADD( &ppSetMask, PPpropLISTLEVEL );

		    ppSet.ppListLevel= ilvl;
		    apply= 1;
		    }
		}
	    }
	else{
	    const DocumentProperties *	dp= &(bd->bdProperties);

	    ppSet.ppLeftIndentTwips= paraNode->biParaLeftIndentTwips+
						step* dp->dpTabIntervalTwips;

	    if  ( paraNode->biParaLeftIndentTwips > 0	&&
		  ppSet.ppLeftIndentTwips < 0		)
		{ ppSet.ppLeftIndentTwips= 0;	}

	    if  ( ppSet.ppLeftIndentTwips >= 0 )
		{
		PROPmaskADD( &ppSetMask, PPpropLEFT_INDENT );
		apply= 1;

		if  ( ppSet.ppLeftIndentTwips+ ppSet.ppFirstIndentTwips < 0 )
		    {
		    ppSet.ppFirstIndentTwips= -ppSet.ppLeftIndentTwips;
		    PROPmaskADD( &ppSetMask, PPpropFIRST_INDENT );
		    }
		}
	    }

	if  ( apply )
	    {
	    if  ( docEditUpdParaProperties( eo, &ppDoneMask, paraNode,
				    &ppSetMask, &ppSet,
				    (const DocumentAttributeMap *)0 ) )
		{ LDEB(1); rval= -1; goto ready; }

	    if  ( ! utilPropMaskIsEmpty( &ppDoneMask ) )
		{
		docEditIncludeNodeInReformatRange( eo, paraNode );
		}
	    }

	if  ( paraNode == eo->eoTailDp.dpNode )
	    { break;	}

	paraNode= docNextParagraph( paraNode );
	if  ( ! paraNode )
	    { XDEB(paraNode);	}
	}

  ready:

    docCleanParagraphProperties( &ppSet );

    return rval;
    }

