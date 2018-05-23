/************************************************************************/
/*									*/
/*  Read/Write text frame properties to/from RTF.			*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<stdio.h>
#   include	<ctype.h>

#   include	<appDebugon.h>

#   include	"docRtfWriterImpl.h"
#   include	"docRtfReaderImpl.h"

/************************************************************************/
/*									*/
/*  Text Frame Properties.						*/
/*									*/
/************************************************************************/

void docRtfSaveParaFrameProperties( RtfWriter *		rwc,
				const FrameProperties *		fp )
    {
    docRtfWriteArgTag( rwc, "absw", fp->fpWideTwips );
    docRtfWriteArgTag( rwc, "absh", fp->fpHighTwips );

    switch( fp->fpXReference )
	{
	case FXrefCOLUMN:
	    docRtfWriteTag( rwc, "phcol" );
	    break;

	case FXrefPAGE:
	    docRtfWriteTag( rwc, "phpg" );
	    break;

	case FXrefMARGIN:
	    docRtfWriteTag( rwc, "phmrg" );
	    break;

	default:
	    LDEB(fp->fpXReference);
	}

    switch( fp->fpXPosition )
	{
	case FXposXGIVEN:
	    if  ( fp->fpXTwips >= 0 )
		{ docRtfWriteArgTag( rwc, "posx", fp->fpXTwips ); }
	    else{
		docRtfWriteArgTag( rwc, "posnegx", -fp->fpXTwips );
		}
	    break;

	case FXposXL:
	    docRtfWriteTag( rwc, "posxl" );
	    break;
	case FXposXR:
	    docRtfWriteTag( rwc, "posxr" );
	    break;
	case FXposXC:
	    docRtfWriteTag( rwc, "posxc" );
	    break;
	case FXposXI:
	    docRtfWriteTag( rwc, "posxi" );
	    break;
	case FXposXO:
	    docRtfWriteTag( rwc, "posxo" );
	    break;

	default:
	    LDEB(fp->fpXPosition);
	}

    switch( fp->fpYReference )
	{
	case FYrefMARGIN:
	    docRtfWriteTag( rwc, "pvmrg" );
	    break;

	case FYrefPAGE:
	    docRtfWriteTag( rwc, "pvpg" );
	    break;

	case FYrefPARA:
	    docRtfWriteTag( rwc, "pvpara" );
	    break;

	case FYrefLINE:
	    docRtfWriteTag( rwc, "posyil" );
	    break;

	default:
	    LDEB(fp->fpYReference);
	}

    switch( fp->fpYPosition )
	{
	case FYposYGIVEN:
	    if  ( fp->fpXTwips >= 0 )
		{ docRtfWriteArgTag( rwc, "posy", fp->fpYTwips ); }
	    else{
		docRtfWriteArgTag( rwc, "posnegy", -fp->fpYTwips );
		}
	    break;

	case FYposYIN:
	    docRtfWriteTag( rwc, "posyin" );
	    break;
	case FYposYOUT:
	    docRtfWriteTag( rwc, "posyout" );
	    break;
	case FYposYT:
	    docRtfWriteTag( rwc, "posyt" );
	    break;
	case FYposYC:
	    docRtfWriteTag( rwc, "posyc" );
	    break;
	case FYposYB:
	    docRtfWriteTag( rwc, "posyb" );
	    break;

	default:
	    LDEB(fp->fpYPosition);
	}

    if  ( fp->fpLockAnchor )
	{ docRtfWriteTag( rwc, "abslock" );		 }

    switch( fp->fpFrameWrapStyle )
	{
	case FWSwrapWRAP:
	    /* LDEB(fp->fpFrameWrapStyle); */
	    break;
	case FWSwrapNOWRAP:
	    docRtfWriteTag( rwc, "nowrap" );
	    break;
	case FWSwrapOVERLAY:
	    docRtfWriteTag( rwc, "overlay" );
	    break;

	default:
	    LDEB(fp->fpFrameWrapStyle);
	}

    if  ( fp->fpDistanceFromTextTwipsLeft	==
	  fp->fpDistanceFromTextTwipsTop	)
	{
	if  ( fp->fpDistanceFromTextTwipsLeft > 0 )
	    {
	    docRtfWriteArgTag( rwc, "dxfrtext",
				    fp->fpDistanceFromTextTwipsLeft );
	    }
	}
    else{
	if  ( fp->fpDistanceFromTextTwipsLeft > 0 )
	    {
	    docRtfWriteArgTag( rwc, "dfrmtxtx",
				    fp->fpDistanceFromTextTwipsLeft );
	    }
	if  ( fp->fpDistanceFromTextTwipsTop > 0 )
	    {
	    docRtfWriteArgTag( rwc, "dfrmtxty",
				    fp->fpDistanceFromTextTwipsTop );
	    }
	}

    /*  No separate TFPpropDFRMTXTL for paragraph */
    /*  No separate TFPpropDFRMTXTR for paragraph */
    /*  No separate TFPpropDFRMTXTT for paragraph */
    /*  No separate TFPpropDFRMTXTB for paragraph */

    switch( fp->fpTextFlowDirection )
	{
	case TXflowTXLRTB:
	    /*docRtfWriteTag( rwc, "frmtxlrtb" );*/
	    break;
	case TXflowTXTBRL:
	    docRtfWriteTag( rwc, "frmtxtbrl" );
	    break;
	case TXflowTXBTLR:
	    docRtfWriteTag( rwc, "frmtxbtlr" );
	    break;
	case TXflowTXLRTBV:
	    docRtfWriteTag( rwc, "frmtxlrtbv" );
	    break;
	case TXflowTXTBRLV:
	    docRtfWriteTag( rwc, "frmtxtbrlv" );
	    break;

	default:
	    LDEB(fp->fpTextFlowDirection);
	}

    return;
    }

void docRtfSaveRowFrameProperties( RtfWriter *		rwc,
				const FrameProperties *		fp )
    {
    /* no TFPpropABSW for table */
    /* no TFPpropABSH for table */

    switch( fp->fpXReference )
	{
	case FXrefCOLUMN:
	    docRtfWriteTag( rwc, "tphcol" );
	    break;

	case FXrefPAGE:
	    docRtfWriteTag( rwc, "tphpg" );
	    break;

	case FXrefMARGIN:
	    docRtfWriteTag( rwc, "tphmrg" );
	    break;

	default:
	    LDEB(fp->fpXReference);
	}

    switch( fp->fpXPosition )
	{
	case FXposXGIVEN:
	    if  ( fp->fpXTwips >= 0 )
		{ docRtfWriteArgTag( rwc, "tposx", fp->fpXTwips ); }
	    else{
		docRtfWriteArgTag( rwc, "tposnegx", -fp->fpXTwips );
		}
	    break;

	case FXposXL:
	    docRtfWriteTag( rwc, "tposxl" );
	    break;
	case FXposXR:
	    docRtfWriteTag( rwc, "tposxr" );
	    break;
	case FXposXC:
	    docRtfWriteTag( rwc, "tposxc" );
	    break;
	case FXposXI:
	    docRtfWriteTag( rwc, "tposxi" );
	    break;
	case FXposXO:
	    docRtfWriteTag( rwc, "tposxo" );
	    break;

	default:
	    LDEB(fp->fpXPosition);
	}

    switch( fp->fpYReference )
	{
	case FYrefMARGIN:
	    docRtfWriteTag( rwc, "tpvmrg" );
	    break;

	case FYrefPAGE:
	    docRtfWriteTag( rwc, "tpvpg" );
	    break;

	case FYrefPARA:
	    docRtfWriteTag( rwc, "tpvpara" );
	    break;

	case FYrefLINE:
	    docRtfWriteTag( rwc, "tposyil" );
	    break;

	default:
	    LDEB(fp->fpYReference);
	}

    switch( fp->fpYPosition )
	{
	case FYposYGIVEN:
	    if  ( fp->fpXTwips >= 0 )
		{ docRtfWriteArgTag( rwc, "tposy", fp->fpYTwips ); }
	    else{
		docRtfWriteArgTag( rwc, "tposnegy", -fp->fpYTwips );
		}
	    break;

	case FYposYIN:
	    docRtfWriteTag( rwc, "tposyin" );
	    break;
	case FYposYOUT:
	    docRtfWriteTag( rwc, "tposyout" );
	    break;
	case FYposYT:
	    docRtfWriteTag( rwc, "tposyt" );
	    break;
	case FYposYC:
	    docRtfWriteTag( rwc, "tposyc" );
	    break;
	case FYposYB:
	    docRtfWriteTag( rwc, "tposyb" );
	    break;

	default:
	    LDEB(fp->fpYPosition);
	}

    /*  No TFPpropLOCK_ANCHOR for table */
    /*  No TFPpropWRAP_STYLE, FWSwrapNOWRAP for table */
    /*  No TFPpropWRAP_STYLE, FWSwrapOVERLAY for table */

    /*  no TFPpropDFRMTXT shorthand for table */
    /*  no TFPpropDFRMTXTX shorthand for table */
    /*  no TFPpropDFRMTXTY shorthand for table */

    if  ( fp->fpDistanceFromTextTwipsLeft > 0 )
	{
	docRtfWriteArgTag( rwc, "tdfrmtxtLeft",
				fp->fpDistanceFromTextTwipsLeft );
	}
    if  ( fp->fpDistanceFromTextTwipsRight > 0 )
	{
	docRtfWriteArgTag( rwc, "tdfrmtxtRight",
				fp->fpDistanceFromTextTwipsRight );
	}
    if  ( fp->fpDistanceFromTextTwipsTop > 0 )
	{
	docRtfWriteArgTag( rwc, "tdfrmtxtTop",
				fp->fpDistanceFromTextTwipsTop );
	}
    if  ( fp->fpDistanceFromTextTwipsBottom > 0 )
	{
	docRtfWriteArgTag( rwc, "tdfrmtxtBottom",
				fp->fpDistanceFromTextTwipsBottom );
	}

    /*  No TFPpropTEXT_FLOW for table */

    return;
    }

/************************************************************************/
/*									*/
/*  Handle a frame property when reading RTF.				*/
/*									*/
/************************************************************************/

/************************************************************************/
/*									*/
/*  Handle a paragraph frame property when reading RTF.			*/
/*									*/
/************************************************************************/

int docRtfRememberParaFrameProperty(	const RtfControlWord *	rcw,
					int			arg,
					RtfReader *	rrc )
    {
    RtfReadingState *	rrs= rrc->rrcState;
    FrameProperties *	fp= &(rrs->rrsParaFrameProperties);

    if  ( docSetFrameProperty( fp, rcw->rcwID, arg ) )
	{ LSLDEB(rcw->rcwID,rcw->rcwWord,arg); return -1;	}

    /*
    PROPmaskADD( &(rrc->rrcStyle.dsParaMask), rcw->rcwID );
    */

    return 0;
    }

int docRtfRememberRowFrameProperty(	const RtfControlWord *	rcw,
					int			arg,
					RtfReader *	rrc )
    {
    FrameProperties *	fp= &(rrc->rrcRowFrameProperties);

    if  ( docSetFrameProperty( fp, rcw->rcwID, arg ) )
	{ LSLDEB(rcw->rcwID,rcw->rcwWord,arg); return -1;	}

    /*
    PROPmaskADD( &(rrc->rrcStyle.dsRowMask), rcw->rcwID );
    */

    return 0;
    }
