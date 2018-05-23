/************************************************************************/
/*									*/
/*  Read/Write document properties to/from RTF.				*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<stdio.h>
#   include	<ctype.h>
#   include	<stdlib.h>

#   include	<appDebugon.h>

#   include	<appUnit.h>
#   include	<textOfficeCharset.h>
#   include	<textConverter.h>
#   include	"docRtfReaderImpl.h"
#   include	"docRtfTagEnum.h"

/************************************************************************/
/*									*/
/*  Remember Document Properties.					*/
/*									*/
/************************************************************************/

/************************************************************************/
/*									*/
/*  Remember the code page for the document.				*/
/*  = All values from the Word 2003 spec explicitly included.		*/
/*  = String values checked against GNU iconv 'iconv (GNU libc) 2.3.6'	*/
/*  = Check strings against iconv -l					*/
/*									*/
/************************************************************************/

static int docRtfSetAnsicpg(	struct TextConverter *	tc,
				int			ansicpg )
    {
    char			scratch[20];

    switch( ansicpg )
	{
	case 437: case 819: case 850: case 852:
	case 460: case 862: case 863: case 864:
	case 465: case 866: case 874: case 932:
	case 936: case 949: case 950:
	case 1361:
	    sprintf( scratch, "CP%d", ansicpg );
	    textConverterSetNativeEncodingName( tc, scratch );
	    return 0;

	case 708:
	    textConverterSetNativeEncodingName( tc, "ASMO-708" );
	    return 0;
	case 709:
	    textConverterSetNativeEncodingName( tc, "ASMO_449" );
	    return 0;

	case 1250: case 1251: case 1252: case 1253:
	case 1254: case 1255: case 1256: case 1257:
	case 1258:
	    /* sprintf( scratch, "WINDOWS-%d", ansicpg ); */ /* GNU? */
	    sprintf( scratch, "CP%d", ansicpg );
	    textConverterSetNativeEncodingName( tc, scratch );
	    return 0;

	case 10000:
	    textConverterSetNativeEncodingName( tc, "MACINTOSH" );
	    return 0;

	case 710:
	case 711:
	case 720:
	default:
	    LDEB(ansicpg);
	    textConverterSetNativeEncodingName( tc, DOC_RTF_AnsiCharsetName );
	    return -1;
	}
    }

/************************************************************************/
/*									*/
/*  Remember the document charset. It is translated to a code page	*/
/*  where possible.							*/
/*									*/
/************************************************************************/

static int docRtfReadSetDocumentCharset(
				RtfReader *		rrc,
				int			arg )
    {
    struct TextConverter *	tc= rrc->rrRtfTextConverter;

    switch( arg )
	{
	case DOCcharsetANSI:
	    if  ( docRtfSetAnsicpg( tc, 1252 ) )
		{ LDEB(arg); return -1;	}
	    return 0;

	case DOCcharsetPC:
	    if  ( docRtfSetAnsicpg( tc, 437 ) )
		{ LDEB(arg); return -1;	}
	    return 0;

	case DOCcharsetPCA:
	    if  ( docRtfSetAnsicpg( tc, 850 ) )
		{ LDEB(arg); return -1;	}
	    return 0;

	case DOCcharsetMAC:
	    if  ( docRtfSetAnsicpg( tc, 10000 ) )
		{ LDEB(arg); return -1;	}
	    return 0;

	default:
	    LDEB(arg); return -1;
	}
    }

int docRtfRememberDocProperty(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rrc )
    {
    SectionProperties *		sp= &(rrc->rrcSectionProperties);

    int				charset= -1;

    switch( rcw->rcwID )
	{
	case DGpropLEFT_MARGIN:
	    sp->spDocumentGeometry.dgLeftMarginTwips= arg;
	    rrc->rrcGotDocGeometry= 1;
	    break;
	case DGpropTOP_MARGIN:
	    sp->spDocumentGeometry.dgTopMarginTwips= arg;
	    rrc->rrcGotDocGeometry= 1;
	    break;
	case DGpropRIGHT_MARGIN:
	    sp->spDocumentGeometry.dgRightMarginTwips= arg;
	    rrc->rrcGotDocGeometry= 1;
	    break;
	case DGpropBOTTOM_MARGIN:
	    sp->spDocumentGeometry.dgBottomMarginTwips= arg;
	    break;
	case DGpropPAGE_WIDTH:
	    sp->spDocumentGeometry.dgPageWideTwips= arg;
	    rrc->rrcGotDocGeometry= 1;
	    break;
	case DGpropPAGE_HEIGHT:
	    sp->spDocumentGeometry.dgPageHighTwips= arg;
	    rrc->rrcGotDocGeometry= 1;
	    break;
	case DGpropGUTTER:
	    sp->spDocumentGeometry.dgGutterTwips= arg;
	    rrc->rrcGotDocGeometry= 1;
	    break;
	case DGpropMARGMIR:
	    arg= arg != 0;
	    sp->spDocumentGeometry.dgMirrorMargins= arg;
	    rrc->rrcGotDocGeometry= 1;
	    break;

	case DPpropFACING_PAGES:
	    arg= arg != 0;
	    break;
	case DPpropGUTTER_HORIZONTAL:
	    arg= arg != 0;
	    break;
	case DPpropWIDOWCTRL:
	    arg= arg != 0;
	    break;
	case DPpropTWO_ON_ONE:
	    arg= arg != 0;
	    break;
	case DPpropDOCTEMP:
	    arg= arg != 0;
	    break;

	case DPpropDEFF:
	    rrc->rrcDefaultFont= arg;
	    if  ( docRtfReadMapFont( rrc, &arg, &charset,
						rrc->rrcDefaultFont ) < 0 )
		{ SLDEB(rcw->rcwWord,arg);	}
	    break;
	case DPpropSTSHFDBCH:
	    rrc->rrcDefaultFontDbch= arg;
	    if  ( docRtfReadMapFont( rrc, &arg, &charset,
						rrc->rrcDefaultFontDbch ) < 0 )
		{ SLDEB(rcw->rcwWord,arg);	}
	    break;
	case DPpropSTSHFLOCH:
	    rrc->rrcDefaultFontLoch= arg;
	    if  ( docRtfReadMapFont( rrc, &arg, &charset,
						rrc->rrcDefaultFontLoch ) < 0 )
		{ SLDEB(rcw->rcwWord,arg);	}
	    break;
	case DPpropSTSHFHICH:
	    rrc->rrcDefaultFontHich= arg;
	    if  ( docRtfReadMapFont( rrc, &arg, &charset,
						rrc->rrcDefaultFontHich ) < 0 )
		{ SLDEB(rcw->rcwWord,arg);	}
	    break;
	case DPpropSTSHFBI:
	    rrc->rrcDefaultFontBi= arg;
	    if  ( docRtfReadMapFont( rrc, &arg, &charset,
						rrc->rrcDefaultFontBi ) < 0 )
		{ SLDEB(rcw->rcwWord,arg);	}
	    break;

	case DPpropDOC_CHARSET:
	    if  ( docRtfReadSetDocumentCharset( rrc, arg ) )
		{ SLDEB(rcw->rcwWord,arg); return 0;	}
	    break;
	case DPpropANSICPG:
	    if  ( docRtfSetAnsicpg( rrc->rrRtfTextConverter, arg ) )
		{ SLDEB(rcw->rcwWord,arg); return 0;	}
	    break;

	case DPpropTOP_BORDER:
	case DPpropBOTTOM_BORDER:
	case DPpropLEFT_BORDER:
	case DPpropRIGHT_BORDER:
	case DPpropHEAD_BORDER:
	case DPpropFOOT_BORDER:
	    arg= docRtfReadGetBorderNumber( rrc );
	    if  ( arg < 0 )
		{ SLDEB(rcw->rcwWord,arg); return -1;	}
	    break;

	case DPpropNOTETYPES:
	    docInitFootEndNotesProperties(
			    &(rrc->rrDocument->bdProperties.dpNotesProps) );
	    break;

	case DPprop_IGNORED:
	    return 0;

	default:
	    break;
	}

    if  ( rrc->rrcTraceInProps )
	{
	/*  Working copy for undo */
	if  ( docSetDocumentProperty( &(rrc->rrcDocumentProperties),
						    rcw->rcwID, arg ) < 0 )
	    { SLDEB(rcw->rcwWord,arg); return 0;	}

	PROPmaskADD( &(rrc->rrcDocPropertyMask), rcw->rcwID );
	}
    else{
	/*  Document */
	if  ( docSetDocumentProperty( &(rrc->rrDocument->bdProperties),
						    rcw->rcwID, arg ) < 0 )
	    { SLDEB(rcw->rcwWord,arg); return 0;	}
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Remember a document property that has a string value.		*/
/*									*/
/************************************************************************/

int docRtfCommitDocPropText(	const RtfControlWord *	rcw,
				RtfReader *		rrc )
    {
    int		rval= 0;
    const int	removeSemicolon= rcw->rcwID == DPpropGENERATOR;

    char *	text= (char *)0;
    int		size;

    if  ( docRtfStoreSavedText( &text, &size, rrc, removeSemicolon ) )
	{ SDEB(rcw->rcwWord); rval= -1; goto ready;	}

    if  ( rrc->rrcTraceInProps )
	{
	if  ( docSetDocumentPropertyString( &(rrc->rrcDocumentProperties),
						    rcw->rcwID, text, size ) )
	    { SDEB(rcw->rcwWord); rval= -1; goto ready;	}

	PROPmaskADD( &(rrc->rrcDocPropertyMask), rcw->rcwID );
	}
    else{
	if  ( docSetDocumentPropertyString( &(rrc->rrDocument->bdProperties),
						    rcw->rcwID, text, size ) )
	    { SDEB(rcw->rcwWord); rval= -1; goto ready;	}
	}

  ready:

    if  ( text )
	{ free( text );		}

    return rval;
    }

/************************************************************************/
/*									*/
/*  A date/time field in the info group.				*/
/*									*/
/************************************************************************/

int docRtfDocTimeGroup(	const RtfControlWord *	rcw,
			int			arg,
			RtfReader *		rrc )
    {
    utilInvalidateTime( &(rrc->rrcTm) );

    if  ( docRtfReadGroup( rcw, 0, 0, rrc,
		(RtfControlWord *)0, docRtfRefuseText, (RtfCommitGroup)0 ) )
	{ SLDEB(rcw->rcwWord,arg); return -1;	}

    if  ( rrc->rrcTraceInProps )
	{
	if  ( docSetDocumentPropertyTime( &(rrc->rrcDocumentProperties),
						rcw->rcwID, &(rrc->rrcTm) ) )
	    { SDEB(rcw->rcwWord); return -1;	}

	PROPmaskADD( &(rrc->rrcDocPropertyMask), rcw->rcwID );
	}
    else{
	if  ( docSetDocumentPropertyTime( &(rrc->rrDocument->bdProperties),
						rcw->rcwID, &(rrc->rrcTm) ) )
	    { SDEB(rcw->rcwWord); return -1;	}
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Remember fields from dates and times.				*/
/*									*/
/************************************************************************/

int docRtfReadTimeField(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rrc )
    {
    switch( rcw->rcwID )
	{
	case RTFidHR:
	    rrc->rrcTm.tm_hour= arg;
	    break;
	case RTFidMIN:
	    rrc->rrcTm.tm_min= arg;
	    break;
	case RTFidSEC:
	    rrc->rrcTm.tm_sec= arg;
	    break;
	case RTFidDY:
	    rrc->rrcTm.tm_mday= arg;
	    break;
	case RTFidMO:
	    rrc->rrcTm.tm_mon= arg- 1;
	    break;
	case RTFidYR:
	    rrc->rrcTm.tm_year= arg- 1900;
	    break;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Remember fields from dates and times.				*/
/*									*/
/************************************************************************/

int docRtfSelectionOpen(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rrc )
    {
    DocumentProperties *	dp= &(rrc->rrDocument->bdProperties);

    dp->dpHasOpenEnd= 1;

    if  ( docRtfReadGroup( rcw, 0, 0, rrc, (RtfControlWord *)0,
				(RtfAddTextParticule)0, (RtfCommitGroup)0 ) )
	{ SLDEB(rcw->rcwWord,arg); return -1;	}

    return 0;
    }
