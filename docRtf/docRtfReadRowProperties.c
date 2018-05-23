/************************************************************************/
/*									*/
/*  Exchange between row properties and Rtf.				*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<stdio.h>
#   include	<ctype.h>

#   include	<appDebugon.h>

#   include	"docRtfReaderImpl.h"

/************************************************************************/
/*									*/
/*  Handle a row property when reading RTF.				*/
/*									*/
/************************************************************************/

static void docRtfResetRowProperties(	RtfReader *	rrc )
    {
    RowProperties *	rp= &(rrc->rrcRowProperties);

    docCleanRowProperties( rp );
    docInitRowProperties(  rp );
    utilPropMaskClear( &(rrc->rrcRowPropertyMask) );
    docInitItemShading(  &(rrc->rrcRowShading) );

    docRtfResetCellProperties( rrc );
    }

int docRtfRememberRowShadingProperty(	const RtfControlWord *	rcw,
					int			arg,
					RtfReader *	rrc )
    {
    if  ( docSetShadingProperty( &(rrc->rrcRowShading), rcw->rcwID, arg ) < 0 )
	{ SLDEB(rcw->rcwWord,arg); return -1;	}

    PROPmaskADD( &(rrc->rrcRowPropertyMask), RPpropSHADING );

    return 0;
    }

int docRtfRememberRowProperty(		const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rrc )
    {
    RowProperties *	rp= &(rrc->rrcRowProperties);

    switch( rcw->rcwID )
	{
	case RPprop_NONE:
	    docRtfResetRowProperties( rrc );
	    return 0;

	case RPpropTOP_BORDER:
	    arg= docRtfReadGetBorderNumber( rrc );
	    if  ( arg < 0 )
		{ LDEB(arg); return -1;	}
	    break;
	case RPpropBOTTOM_BORDER:
	    arg= docRtfReadGetBorderNumber( rrc );
	    if  ( arg < 0 )
		{ LDEB(arg); return -1;	}
	    break;
	case RPpropLEFT_BORDER:
	    arg= docRtfReadGetBorderNumber( rrc );
	    if  ( arg < 0 )
		{ LDEB(arg); return -1;	}
	    break;
	case RPpropRIGHT_BORDER:
	    arg= docRtfReadGetBorderNumber( rrc );
	    if  ( arg < 0 )
		{ LDEB(arg); return -1;	}
	    break;
	case RPpropHORIZ_BORDER:
	    arg= docRtfReadGetBorderNumber( rrc );
	    if  ( arg < 0 )
		{ LDEB(arg); return -1;	}
	    break;
	case RPpropVERT_BORDER:
	    arg= docRtfReadGetBorderNumber( rrc );
	    if  ( arg < 0 )
		{ LDEB(arg); return -1;	}
	    break;

	case RPprop_IGNORED:
	    return 0;
	}

    if  ( docSetRowProperty( rp, rcw->rcwID, arg ) < 0 )
	{ SLDEB(rcw->rcwWord,arg); return -1;	}

    PROPmaskADD( &(rrc->rrcRowPropertyMask), rcw->rcwID );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Cope with Word10 {\trowd ...... } peculiarity.			*/
/*									*/
/************************************************************************/

int docRtfReadRowProperties(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rrc )
    {
    int			rval= 0;

    RowProperties	savedRp;
    CellProperties	savedCp;
    ItemShading		savedRowShading;
    ItemShading		savedCellShading;

    savedRp= rrc->rrcRowProperties;
    savedCp= rrc->rrcCellProperties;
    savedRowShading= rrc->rrcRowShading;
    savedCellShading= rrc->rrcCellShading;

    docInitRowProperties( &(rrc->rrcRowProperties) );
    docInitCellProperties( &(rrc->rrcCellProperties) );
    docInitItemShading(  &(rrc->rrcRowShading) );
    docInitItemShading(  &(rrc->rrcCellShading) );

    if  ( docRtfReadGroup( rcw, 0, 0, rrc,
	    docRtfDocumentGroups, docRtfTextParticule, (RtfCommitGroup)0 ) )
	{ SDEB(rcw->rcwWord); rval= -1; goto ready;	}

  ready:

    docCleanRowProperties( &(rrc->rrcRowProperties) );
    docCleanCellProperties( &(rrc->rrcCellProperties) );

    rrc->rrcRowProperties= savedRp;
    rrc->rrcCellProperties= savedCp;
    rrc->rrcRowShading= savedRowShading;
    rrc->rrcCellShading= savedCellShading;

    return rval;
    }

