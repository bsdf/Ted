/************************************************************************/
/*									*/
/*  Read/Write border properties to/from RTF.				*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<stdio.h>
#   include	<ctype.h>

#   include	<appDebugon.h>

#   include	"docRtfWriterImpl.h"
#   include	"docRtfReaderImpl.h"
#   include	"docRtfTags.h"

/************************************************************************/
/*									*/
/*  Save border definition.						*/
/*									*/
/************************************************************************/

static void docRtfSaveBorder(		RtfWriter *			rw,
					const char *			tag,
					const BorderProperties *	bp )
    {
    docRtfWriteTag( rw, tag );

    docRtfWriteEnumTag( rw, DOCrtf_BorderStyleTags, bp->bpStyle,
				    DOCrtf_BorderStyleTagCount, DOCbs_COUNT );

    if  ( bp->bpArt != 0 )
	{ docRtfWriteArgTag( rw, "brdrart", bp->bpColor );	}

    if  ( bp->bpColor != 0 )
	{ docRtfWriteArgTag( rw, "brdrcf", bp->bpColor );	}

    if  ( bp->bpPenWideTwips != 0 )
	{ docRtfWriteArgTag( rw, "brdrw", bp->bpPenWideTwips ); }

    if  ( bp->bpSpacingTwips != 0 )
	{ docRtfWriteArgTag( rw, "brsp", bp->bpSpacingTwips );	}

    docRtfWriteNextLine( rw );

    return;
    }

void docRtfSaveBorderByNumber(		RtfWriter *			rwc,
					const char *			tag,
					int				num,
					int				anyway )
    {
    const BufferDocument *		bd= rwc->rwDocument;

    BorderProperties			bp;

    docGetBorderPropertiesByNumber( &bp, bd, num );

    if  ( ! anyway && bp.bpStyle == DOCbsNONE )
	{ return;	}

    docRtfSaveBorder( rwc, tag, &bp );
    }

int docRtfBeginBorder(		const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr )
    {
    docInitBorderProperties( &(rr->rrcBorderProperties) );
    rr->rrcBorderProperties.bpStyle= DOCbsS;
    return 0;
    }

int docRtfReadGetBorderNumber(	RtfReader *	rr )
    {
    int rval= docBorderPropertiesNumber( rr->rrDocument,
						&(rr->rrcBorderProperties) );

    docInitBorderProperties( &(rr->rrcBorderProperties) );

    return rval;
    }

int docRtfBrdrProperty(		const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr )
    {
    if  ( docSetBorderProperty( &(rr->rrcBorderProperties),
						    rcw->rcwID, arg ) < 0 )
	{ SLLDEB(rcw->rcwWord,rcw->rcwID,arg); return -1; }

    return 0;
    }

