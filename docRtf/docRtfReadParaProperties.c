/************************************************************************/
/*									*/
/*  Read/Write paragraph properties to/from RTF.			*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<stdio.h>
#   include	<ctype.h>

#   include	<appDebugon.h>

#   include	"docRtfReaderImpl.h"
#   include	<docTreeNode.h>
#   include	<docNodeTree.h>

/************************************************************************/
/*									*/
/*  Handle a paragraph property when reading RTF.			*/
/*									*/
/************************************************************************/

int docRtfRememberParaShadingProperty(	const RtfControlWord *	rcw,
					int			arg,
					RtfReader *	rrc )
    {
    RtfReadingState *		rrs= rrc->rrcState;

    if  ( docSetShadingProperty( &(rrs->rrsParagraphShading),
						    rcw->rcwID, arg ) < 0 )
	{ SLDEB(rcw->rcwWord,arg); return -1;	}

    /*
    PROPmaskADD( &(rrs->rrsParaPropertyMask), PPpropSHADING );
    */

    return 0;
    }

int docRtfRememberParagraphProperty(	const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rrc )
    {
    RtfReadingState *		rrs= rrc->rrcState;
    ParagraphProperties *	pp= &(rrs->rrsParagraphProperties);

    switch( rcw->rcwID )
	{
	case PPprop_NONE:
	    docRtfResetParagraphProperties( rrs );
	    return 0;

	case PPpropSTYLE:
	    rrc->rrcStyle.dsLevel= DOClevPARA;
	    break;

	case PPpropLISTOVERRIDE:
	    rrc->rrcListOverride.loIndex= arg;
	    break;

	case PPpropTOP_BORDER:
	case PPpropBOTTOM_BORDER:
	case PPpropLEFT_BORDER:
	case PPpropRIGHT_BORDER:
	case PPprop_BOX_BORDER:
	case PPpropBETWEEN_BORDER:
	case PPpropBAR_BORDER:
	    arg= docRtfReadGetBorderNumber( rrc );
	    if  ( arg < 0 )
		{ SLDEB(rcw->rcwWord,arg); return -1;	}
	    break;
	}

    if  ( docSetParaProperty( pp, rcw->rcwID, arg ) < 0 )
	{ SLDEB(rcw->rcwWord,arg); return -1;	}

    PROPmaskADD( &(rrc->rrcStyle.dsParaMask), rcw->rcwID );

    switch( rcw->rcwID )
	{
	case PPprop_IN_TABLE:  /* intbl */
	    if  ( rrc->rrDocument && pp->ppTableNesting > 0 )
		{ rrc->rrDocument->bdProperties.dpContainsTables= 1;	}
	    break;
	case PPpropTABLE_NESTING:  /* itap */
	    if  ( rrc->rrDocument && pp->ppTableNesting > 0 )
		{ rrc->rrDocument->bdProperties.dpContainsTables= 1;	}
	    break;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Refresh the values in the paragraph properties that indirectly	*/
/*  refer to structures in a numbered property admin.			*/
/*									*/
/************************************************************************/

int docRtfRefreshParagraphProperties(	BufferDocument *	bd,
					RtfReadingState *	rrs )
    {
    rrs->rrsParagraphProperties.ppShadingNumber= docItemShadingNumber( bd,
					&(rrs->rrsParagraphShading) );

    rrs->rrsParagraphProperties.ppTabStopListNumber= docTabStopListNumber( bd,
					&(rrs->rrsTabStopList) );

    rrs->rrsParagraphProperties.ppFrameNumber= docFramePropertiesNumber( bd,
					&(rrs->rrsParaFrameProperties) );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Transfer paragraph properties to a paragraph or to a style.		*/
/*									*/
/************************************************************************/

int docRtfSetParaProperties(	ParagraphProperties *	pp,
				BufferDocument *	bd,
				int			mindTable,
				RtfReadingState *	rrs,
				int			breakOverride )
    {
    PropertyMask		ppSetMask;

    docRtfRefreshParagraphProperties( bd, rrs );

    utilPropMaskClear( &ppSetMask );
    utilPropMaskFill( &ppSetMask, PPprop_FULL_COUNT );
    if  ( ! mindTable )
	{ PROPmaskUNSET( &ppSetMask, PPpropTABLE_NESTING );	}

    /*  3  */
    if  ( docUpdParaProperties( (PropertyMask *)0, pp,
				    &ppSetMask, &(rrs->rrsParagraphProperties),
				    (const DocumentAttributeMap *)0 ) )
	{ LDEB(1); return -1;	}

    if  ( breakOverride >= 0 )
	{ pp->ppBreakKind= breakOverride;	}

    return 0;
    }

int docRtfAdaptToParaProperties(	struct BufferItem *	paraNode,
					BufferDocument *	bd,
					RtfReadingState *	rrs,
					int			breakOverride )
    {
    const int		mindTable= 1;
    BufferItem *	rowNode;

    if  ( docRtfSetParaProperties( &(paraNode->biParaProperties),
					bd, mindTable, rrs, breakOverride ) )
	{ LDEB(1); return -1;	}

    rowNode= docGetRowLevelNode( paraNode );
    if  ( paraNode->biParaTableNesting > 0 )
	{
	if  ( ! rowNode->biRowForTable )
	    {
	    BufferItem *	cellNode= paraNode->biParent;

	    if  ( paraNode->biNumberInParent > 0	||
		  cellNode->biNumberInParent > 0	)
		{
		if  ( docSplitGroupNodeAtLevel( bd,
			    (BufferItem **)0, (BufferItem **)0, cellNode,
			    paraNode->biNumberInParent, DOClevROW ) )
		    { LDEB(1); return -1;	}

		docCleanRowProperties( &(rowNode->biRowProperties) );
		docInitRowProperties( &(rowNode->biRowProperties) );
		}

	    docRtfSetForRow( paraNode );
	    }
	}
    else{
	if  ( rowNode->biRowForTable )
	    {
	    if  ( rowNode->biChildCount == 1 )
		{
		rowNode->biRowForTable= 0;
		}
	    else{
		LDEB(rowNode->biChildCount);
		LLDEB(paraNode->biParaTableNesting,rowNode->biRowForTable);
		}
	    }
	}

    return 0;
    }

void docRtfResetParagraphProperties(	RtfReadingState *	rrs )
    {
    docCleanParagraphProperties( &(rrs->rrsParagraphProperties) );
    docInitParagraphProperties( &(rrs->rrsParagraphProperties) );

    docCleanTabStopList( &(rrs->rrsTabStopList) );
    docInitTabStopList( &(rrs->rrsTabStopList) );

    docInitItemShading( &(rrs->rrsParagraphShading) );
    docInitFrameProperties( &(rrs->rrsParaFrameProperties) );

    return;
    }

