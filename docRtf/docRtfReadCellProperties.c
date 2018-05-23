/************************************************************************/
/*									*/
/*  Read/Write paragraph cell to/from RTF.				*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<stdio.h>
#   include	<ctype.h>

#   include	<appDebugon.h>

#   include	"docRtfReaderImpl.h"
#   include	"docRtfTagEnum.h"

/************************************************************************/
/*									*/
/*  Handle a cell property when reading RTF.				*/
/*									*/
/************************************************************************/

int docRtfRememberCellShadingProperty(	const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rrc )
    {
    if  ( docSetShadingProperty( &(rrc->rrcCellShading), rcw->rcwID, arg ) < 0 )
	{ SLDEB(rcw->rcwWord,arg); return -1;	}

    PROPmaskADD( &(rrc->rrcCellPropertyMask), CLpropSHADING );
    PROPmaskADD( &(rrc->rrcStyle.dsCellMask), CLpropSHADING );

    return 0;
    }

void docRtfResetCellProperties(	RtfReader *	rrc )
    {
    docCleanCellProperties( &(rrc->rrcCellProperties) );
    docInitCellProperties( &(rrc->rrcCellProperties) );
    docInitItemShading( &(rrc->rrcCellShading) );
    utilPropMaskClear( &(rrc->rrcCellPropertyMask) );

    return;
    }

static int docRtfSetCellX(		RtfReader *		rrc,
					int			arg )
    {
    CellProperties *	cp= &(rrc->rrcCellProperties);

    const int		col= -1;
    const int		shiftTail= 0;

    cp->cpRightBoundaryTwips= arg;

    cp->cpShadingNumber= docItemShadingNumber( rrc->rrDocument,
						    &(rrc->rrcCellShading) );
    if  ( cp->cpShadingNumber < 0 )
	{ LDEB(cp->cpShadingNumber);	}

    if  ( docInsertRowColumn( &(rrc->rrcRowProperties),
		    col, shiftTail, cp, (const DocumentAttributeMap *)0 ) )
	{ LDEB(rrc->rrcRowProperties.rpCellCount); return -1; }

    PROPmaskADD( &(rrc->rrcCellPropertyMask), CLpropCELLX );
    PROPmaskADD( &(rrc->rrcStyle.dsCellMask), CLpropCELLX );
    PROPmaskADD( &(rrc->rrcRowPropertyMask), RPpropCELL_LAYOUT );

    docRowMaskApplyCellMask( &(rrc->rrcRowPropertyMask),
						&(rrc->rrcCellPropertyMask) );

    return 0;
    }

int docRtfRememberCellProperty(		const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rrc )
    {
    switch( rcw->rcwID )
	{
	case CLprop_NONE:
	    docRtfResetCellProperties( rrc );
	    return 0;

	case CLpropCELLX:
	    if  ( arg < 0 )
		{
		DocumentGeometry *	dg;

		SLDEB(rcw->rcwWord,arg);

		dg= &(rrc->rrcSectionProperties.spDocumentGeometry);

		arg= dg->dgPageWideTwips-
			    ( dg->dgRightMarginTwips+ dg->dgLeftMarginTwips );
		}

	    if  ( docRtfSetCellX( rrc, arg ) )
		{ SLDEB(rcw->rcwWord,arg); return -1;	}

	    docRtfResetCellProperties( rrc );
	    return 0;

	/**/
	case CLpropTOP_BORDER:
	case CLpropBOTTOM_BORDER:
	case CLpropLEFT_BORDER:
	case CLpropRIGHT_BORDER:
	    arg= docRtfReadGetBorderNumber( rrc );
	    if  ( arg < 0 )
		{ SLDEB(rcw->rcwWord,arg); return -1;	}
	    break;

	case RTFid_NOT_SUPPORTED:
	    return 0;

	case CLpropHOR_MERGE:
	case CLpropTEXT_FLOW:
	case CLpropVALIGN:
	case CLpropVERT_MERGE:
	    arg= rcw->rcwEnumValue;
	    break;
	}

    PROPmaskADD( &(rrc->rrcCellPropertyMask), rcw->rcwID );
    if  ( docSetCellProperty( &(rrc->rrcCellProperties), rcw->rcwID, arg ) < 0 )
	{ SLDEB(rcw->rcwWord,arg); return -1;	}

    PROPmaskADD( &(rrc->rrcStyle.dsCellMask), rcw->rcwID );
    if  ( docSetCellProperty( &(rrc->rrcStyle.dsCellProps),
						rcw->rcwID, arg ) < 0 )
	{ SLDEB(rcw->rcwWord,arg); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Cell instance properties: How table styles are applied.		*/
/*									*/
/************************************************************************/

int docRtfRememberCellInstanceProperty(	const RtfControlWord *	rcw,
					int			arg,
					RtfReader *	rrc )
    {
    return 0;
    }
