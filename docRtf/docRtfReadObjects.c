/************************************************************************/
/*									*/
/*  Read an RTF text file into a BufferDocument				*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<stdio.h>
#   include	<ctype.h>
#   include	<bitmap.h>

#   include	<docObjectProperties.h>
#   include	<docParaParticules.h>
#   include	<docShape.h>
#   include	<appDebugon.h>

#   include	"docRtfReaderImpl.h"
#   include	"docRtfTagEnum.h"
#   include	"docRtfTags.h"

int docRtfObjectProperty(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *	rrc )
    {
    InsertedObject *	io;

    io= docGetObject( rrc->rrDocument, rrc->rrcInsertedObjectNr );
    if  ( ! io )
	{
	LSXDEB(rrc->rrCurrentLine,rcw->rcwWord,rrc->rrcInsertedObjectNr);
	return 0;
	}

    switch( rcw->rcwID )
	{
	case IOpropRESULT_KIND:
	    io->ioRtfResultKind= arg;
	    break;

	case IOpropEMBED_KIND:
	    io->ioRtfEmbedKind= arg;
	    break;

	case IOpropOBJTWIPS_WIDE:
	    io->ioTwipsWide= arg;
	    break;
	case IOpropOBJTWIPS_HIGH:
	    io->ioTwipsHigh= arg;
	    break;

	case IOpropOBJSCALE_X:
	    if  ( arg < 1 )
		{ LDEB(arg); arg= 1;	}
	    io->ioScaleXSet= arg;
	    io->ioScaleXUsed= io->ioScaleXSet;
	    break;
	case IOpropOBJSCALE_Y:
	    if  ( arg < 1 )
		{ LDEB(arg); arg= 1;	}
	    io->ioScaleYSet= arg;
	    io->ioScaleYUsed= io->ioScaleYSet;
	    break;

	case IOpropOBJCROP_TOP:
	    io->ioTopCropTwips= arg;
	    break;
	case IOpropOBJCROP_BOTTOM:
	    io->ioBottomCropTwips= arg;
	    break;
	case IOpropOBJCROP_LEFT:
	    io->ioLeftCropTwips= arg;
	    break;
	case IOpropOBJCROP_RIGHT:
	    io->ioRightCropTwips= arg;
	    break;

	case IOprop_UNSUPPORTED:
	    break;

	default:
	    SLDEB(rcw->rcwWord,rcw->rcwID);
	    break;
	}

    return 0;
    }

int docRtfPictureProperty(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rrc )
    {
    PictureProperties *	pip= &(rrc->rrcPictureProperties);

    switch( rcw->rcwID )
	{
	case PIPpropTYPE:
	    /* Exception to cover \wmetafile8 */
	    if  ( docSetPictureProperty( pip, rcw->rcwID, rcw->rcwEnumValue ) )
		{ SDEB(rcw->rcwWord); return 0;	}

	    if  ( pip->pipType == DOCokPICTWMETAFILE )
		{ pip->pipMapMode= arg;	}
	    break;

	case PIPpropPICSCALE_X:
	    if  ( docSetPictureProperty( pip, rcw->rcwID, arg ) )
		{ SDEB(rcw->rcwWord); return 0;	}

	    pip->pipScaleXUsed= pip->pipScaleXSet;
	    break;
	case PIPpropPICSCALE_Y:
	    if  ( docSetPictureProperty( pip, rcw->rcwID, arg ) )
		{ SDEB(rcw->rcwWord); return 0;	}

	    pip->pipScaleYUsed= pip->pipScaleYSet;
	    break;

	default:
	    if  ( docSetPictureProperty( pip, rcw->rcwID, arg ) )
		{ SDEB(rcw->rcwWord); return 0;	}

	    break;
	}

    PROPmaskADD( &(rrc->rrcPicturePropMask), rcw->rcwID );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Consume picture property tags.					*/
/*									*/
/*  In particular:							*/
/*	shplid:	The identification of the shape of the picture.		*/
/*	sp:	Various shape properties for the picture.		*/
/*  We make a drawing shape to receive the properties.			*/
/*									*/
/************************************************************************/

static RtfControlWord	docRtfPicpropGroups[]=
    {
	RTF_DEST_XX( "sp",	1,	docRtfShapeProperty ),

	{ (char *)0, 0, 0 }
    };

static int docRtfReadPicprop(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rrc )
    {
    int			rval;
    DrawingShape *	parent= rrc->rrDrawingShape;
    DrawingShape *	ds;

    ds= docClaimDrawingShape( &(rrc->rrDocument->bdShapeList) );
    if  ( ! ds )
	{ XDEB(ds); return -1;	}

    ds->dsDrawing.sdShapeType= rcw->rcwID;

    rrc->rrDrawingShape= ds;

    rval= docRtfConsumeGroup( (const RtfControlWord *)0, 0, -1,
				rrc, docRtfPicpropGroups, docRtfIgnoreText );
    if  ( rval )
	{ SLDEB(rcw->rcwWord,rval);	}

    /* ? */
    docDeleteDrawingShape( rrc->rrDocument, ds );

    rrc->rrDrawingShape= parent;
    return rval;
    }

static RtfControlWord	docRtfPictGroups[]=
    {
	RTF_DEST_XX( "picprop",	SHPtyPICPROP,	docRtfReadPicprop ),

	{ (char *)0, 0, 0 }
    };

static int docRtfSaveObjectData(	RtfReader *		rrc,
					const char *		text,
					int			len )
    {
    InsertedObject *	io;

    io= docGetObject( rrc->rrDocument, rrc->rrcInsertedObjectNr );
    if  ( ! io )
	{ LXDEB(rrc->rrcInsertedObjectNr,io); return -1;	}

    if  ( docObjectSetData( io, text, len ) )
	{ LDEB(len); return -1;	}

    return 0;
    }

static int docRtfSaveResultData(	RtfReader *		rrc,
					const char *		text,
					int			len )
    {
    InsertedObject *	io;

    io= docGetObject( rrc->rrDocument, rrc->rrcInsertedObjectNr );
    if  ( ! io )
	{ LXDEB(rrc->rrcInsertedObjectNr,io); return -1;	}

    if  ( docSetResultData( io, text, len ) )
	{ LDEB(len); return -1;	}

    return 0;
    }

int docRtfReadPict(	const RtfControlWord *	rcw,
			int			arg,
			RtfReader *		rrc )
    {
    int				rval= 0;

    RtfReadingState *		externRrs= rrc->rrcState;
    RtfReadingState		internRrs;

    TextParticule *		tp;

    BufferItem *		paraNode;
    int				ioNrSave= rrc->rrcInsertedObjectNr;

    RtfAddTextParticule		addParticule;

    paraNode= docRtfGetParaNode( rrc );
    if  ( ! paraNode )
	{ XDEB(paraNode); rval= -1; goto ready; }

    utilPropMaskClear( &(rrc->rrcPicturePropMask) );
    docInitPictureProperties( &(rrc->rrcPictureProperties) );

    if  ( rrc->rrcInIgnoredGroup )
	{ addParticule= docRtfIgnoreText;	}
    else{
	addParticule= docRtfSaveObjectData;

	if  ( externRrs->rrsTextShadingChanged )
	    { docRtfRefreshTextShading( rrc, externRrs );	}

	tp= docAppendObject( rrc->rrDocument, paraNode,
					    &(externRrs->rrsTextAttribute) );
	if  ( ! tp )
	    { LDEB(paraNode->biParaParticuleCount); rval= -1; goto ready; }

	rrc->rrcInsertedObjectNr= tp->tpObjectNumber;
	}

    docRtfPushReadingState( rrc, &internRrs );

    rval= docRtfConsumeGroup( (const RtfControlWord *)0, 0, -1,
					rrc, docRtfPictGroups, addParticule );

    if  ( rval )
	{ LDEB(rval);	}
    if  ( rrc->rrcInsertedObjectNr >= 0 )
	{
	InsertedObject *	io;
	PictureProperties *	pip;

	io= docGetObject( rrc->rrDocument, rrc->rrcInsertedObjectNr );
	if  ( ! io )
	    { LXDEB(rrc->rrcInsertedObjectNr,io); rval= -1; goto ready;	}
	pip= &(rrc->rrcPictureProperties);

	io->ioKind= pip->pipType;
	io->ioPictureProperties= rrc->rrcPictureProperties;

	io->ioTwipsWide= pip->pipTwipsWide;
	io->ioTwipsHigh= pip->pipTwipsHigh;
	io->ioScaleXSet= pip->pipScaleXSet;
	io->ioScaleYSet= pip->pipScaleYSet;
	io->ioScaleXUsed= pip->pipScaleXUsed;
	io->ioScaleYUsed= pip->pipScaleYUsed;
	}

    docRtfPopReadingState( rrc );

  ready:

    rrc->rrcInsertedObjectNr= ioNrSave;

    return rval;
    }

int docRtfReadShpXPict(		const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rrc )
    {
    int			rval;
    int			ignore= 0;

    if  ( ! docRtfGetParaNode( rrc ) )
	{ SDEB(rcw->rcwWord); return -1; }

    switch( rcw->rcwID )
	{
	case PICshpSHPPICT:
	    ignore= 0;
	    break;
	case PICshpNONSHPPICT:
	    ignore= 1;
	    break;
	default:
	    SLDEB(rcw->rcwWord,rcw->rcwID);
	    break;
	}

    rrc->rrcInIgnoredGroup += ignore;

    rval= docRtfConsumeGroup( (const RtfControlWord *)0, 0, -1,
			rrc, (const RtfControlWord *)0, docRtfIgnoreText );
    if  ( rval )
	{ SLDEB(rcw->rcwWord,rval);	}

    rrc->rrcInIgnoredGroup -= ignore;

    return rval;
    }

static int docRtfReadResultPict(	const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rrc )
    {
    int				rval;

    RtfAddTextParticule		addParticule;

    if  ( ! docRtfGetParaNode( rrc ) )
	{ SDEB(rcw->rcwWord); return -1; }

    if  ( rrc->rrcInIgnoredGroup )
	{ addParticule= docRtfIgnoreText;	}
    else{ addParticule= docRtfSaveResultData;	}

    rval= docRtfConsumeGroup( (const RtfControlWord *)0, 0, -1,
					rrc, docRtfPictGroups, addParticule );

    if  ( rval )
	{ SLDEB(rcw->rcwWord,rval);	}

    if  ( rrc->rrcInsertedObjectNr >= 0 )
	{
	InsertedObject *	io;
	PictureProperties *	pip;

	io= docGetObject( rrc->rrDocument, rrc->rrcInsertedObjectNr );
	if  ( ! io )
	    { LXDEB(rrc->rrcInsertedObjectNr,io); return -1;	}
	pip= &(rrc->rrcPictureProperties);

	io->ioResultKind= pip->pipType;
	io->ioPictureProperties= rrc->rrcPictureProperties;

	if  ( io->ioTwipsWide < 2 )
	    { io->ioTwipsWide= pip->pipTwipsWide;	}
	if  ( io->ioTwipsHigh < 2 )
	    { io->ioTwipsHigh= pip->pipTwipsHigh;	}
	if  ( io->ioScaleXSet == 100 )
	    {
	    io->ioScaleXSet= pip->pipScaleXSet;
	    io->ioScaleXUsed= pip->pipScaleXUsed;
	    }
	if  ( io->ioScaleYSet == 100 )
	    {
	    io->ioScaleYSet= pip->pipScaleYSet;
	    io->ioScaleYUsed= pip->pipScaleYUsed;
	    }
	}

    return rval;
    }

static RtfControlWord	docRtfReadResultXPpictGroups[]=
    {
	RTF_DEST_XX( RTFtag_pict,	PICshpPICT,	docRtfReadResultPict ),

	{ (char *)0, 0, 0 }
    };

static int docRtfReadResultXPpict(	const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rrc )
    {
    int				rval;
    int				ignore= 0;

    if  ( ! docRtfGetParaNode( rrc ) )
	{ SDEB(rcw->rcwWord); return -1; }

    switch( rcw->rcwID )
	{
	case PICshpSHPPICT:
	    ignore= 0;
	    break;
	case PICshpNONSHPPICT:
	    ignore= 1;
	    break;
	default:
	    SLDEB(rcw->rcwWord,rcw->rcwID);
	    break;
	}

    rrc->rrcInIgnoredGroup += ignore;

    rval= docRtfConsumeGroup( (const RtfControlWord *)0, 0, -1,
			rrc, docRtfReadResultXPpictGroups, docRtfIgnoreText );

    if  ( rval )
	{ SLDEB(rcw->rcwWord,rval);	}

    rrc->rrcInIgnoredGroup -= ignore;

    return rval;
    }

static int docRtfSaveObjectClass(	RtfReader *	rrc,
					const char *		text,
					int			len )
    {
    InsertedObject *	io;

    io= docGetObject( rrc->rrDocument, rrc->rrcInsertedObjectNr );
    if  ( ! io )
	{ LXDEB(rrc->rrcInsertedObjectNr,io); return -1;	}

    if  ( docSetObjectClass( io, text, len ) )
	{ LDEB(len); return -1;	}

    return 0;
    }

static int docRtfSaveObjectName(	RtfReader *	rrc,
					const char *		text,
					int			len )
    {
    InsertedObject *	io;

    io= docGetObject( rrc->rrDocument, rrc->rrcInsertedObjectNr );
    if  ( ! io )
	{ LXDEB(rrc->rrcInsertedObjectNr,io); return -1;	}

    if  ( docSetObjectName( io, text, len ) )
	{ LDEB(len); return -1;	}

    return 0;
    }

static int docRtfObjectClass(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *	rrc )
    {
    int				rval;

    rval= docRtfConsumeGroup( (const RtfControlWord *)0, 0, -1,
			rrc, (RtfControlWord *)0, docRtfSaveObjectClass );

    if  ( rval )
	{ SLDEB(rcw->rcwWord,rval);	}

    return rval;
    }

static int docRtfObjectName(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *	rrc )
    {
    int				rval;

    rval= docRtfConsumeGroup( (const RtfControlWord *)0, 0, -1,
			rrc, (RtfControlWord *)0, docRtfSaveObjectName );
    if  ( rval )
	{ SLDEB(rcw->rcwWord,rval);	}

    return rval;
    }

static int docRtfObjectData(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *	rrc )
    {
    int				rval;

    rval= docRtfConsumeGroup( (const RtfControlWord *)0, 0, -1,
			rrc, (RtfControlWord *)0, docRtfSaveObjectData );
    if  ( rval )
	{ SLDEB(rcw->rcwWord,rval);	}

    return rval;
    }

static RtfControlWord	docRtfResultGroups[]=
{
    RTF_DEST_XX( RTFtag_pict,	PICshpPICT,	docRtfReadResultPict ),
    RTF_DEST_XX( "shppict",	PICshpSHPPICT,	docRtfReadResultXPpict ),
    RTF_DEST_XX( "nonshppict",	PICshpNONSHPPICT, docRtfReadResultXPpict ),

    { (char *)0, 0, 0 }
};

static int docRtfObjectResult(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *	rrc )
    {
    int				rval;

    rval= docRtfConsumeGroup( (const RtfControlWord *)0, 0, -1,
				rrc, docRtfResultGroups, docRtfIgnoreText );
    if  ( rval )
	{ SLDEB(rcw->rcwWord,rval);	}

    return rval;
    }

static RtfControlWord	docRtfObjectGroups[]=
    {
	RTF_DEST_XX( "objname",	RTFidOBJNAME,	docRtfObjectName ),
	RTF_DEST_XX( "objclass",RTFidOBJCLASS,	docRtfObjectClass ),
	RTF_DEST_XX( "objdata",	RTFidOBJDATA,	docRtfObjectData ),
	RTF_DEST_XX( RTFtag_result,	RTFidOBJDATA,	docRtfObjectResult ),

	{ (char *)0, 0, 0 }
    };

int docRtfReadObject(	const RtfControlWord *	rcw,
			int			arg,
			RtfReader *		rrc )
    {
    RtfReadingState *		rrs= rrc->rrcState;
    int				rval;

    int				ioNrSave= rrc->rrcInsertedObjectNr;

    const TextParticule *	tp;
    InsertedObject *		io;

    if  ( ! docRtfGetParaNode( rrc ) )
	{ SDEB(rcw->rcwWord); return -1; }

    utilPropMaskClear( &(rrc->rrcPicturePropMask) );
    docInitPictureProperties( &(rrc->rrcPictureProperties) );

    if  ( rrs->rrsTextShadingChanged )
	{ docRtfRefreshTextShading( rrc, rrs );	}

    tp= docAppendObject( rrc->rrDocument, rrc->rrcNode,
						&(rrs->rrsTextAttribute) );
    if  ( ! tp )
	{ SPDEB(rcw->rcwWord,tp); return -1;	}

    rrc->rrcInsertedObjectNr= tp->tpObjectNumber;
    io= docGetObject( rrc->rrDocument, rrc->rrcInsertedObjectNr );
    if  ( ! io )
	{ LXDEB(rrc->rrcInsertedObjectNr,io); return -1;	}
    io->ioKind= DOCokOLEOBJECT;

    rval= docRtfConsumeGroup( (const RtfControlWord *)0, 0, -1,
			rrc, docRtfObjectGroups, docRtfSaveObjectData );

    rrc->rrcInsertedObjectNr= ioNrSave;

    if  ( rval )
	{ SLDEB(rcw->rcwWord,rval);	}

    return rval;
    }
