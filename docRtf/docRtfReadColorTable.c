/************************************************************************/
/*									*/
/*  Read the various document tables of an RTF text file into a		*/
/*  BufferDocument.							*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<stdio.h>
#   include	<ctype.h>

#   include	<appDebugon.h>

#   include	"docRtfReaderImpl.h"

/************************************************************************/
/*									*/
/*  Read a color table.							*/
/*  Note that like the font table, the color table is flat. It does	*/
/*  not have groups for the colors.					*/
/*									*/
/************************************************************************/

static int docRtfSaveColor(	RtfReader *		rrc,
				const char *		text,
				int			len )
    {
    DocumentProperties *	dp= &(rrc->rrDocument->bdProperties);
    int				n= dp->dpColorPalette->cpColorCount;

    if  ( utilPaletteSetCount( dp->dpColorPalette, n+ 1 ) )
	{ LDEB(n); return -1;	}

    dp->dpColorPalette->cpColors[n]= rrc->rrcColor;

    if  ( ! rrc->rrcGotComponent )
	{
	if  ( dp->dpDefaultColor < 0 )
	    { dp->dpDefaultColor= n;	}
	}

    rrc->rrcGotComponent= 0;
    return 0;
    }

int docRtfColorIgnored(	const RtfControlWord *	rcw,
			int			arg,
			RtfReader *		rrc )
    { return 0;	}

int docRtfColorComp(	const RtfControlWord *	rcw,
			int			arg,
			RtfReader *		rrc )
    {
    switch( rcw->rcwID )
	{
	case RGBAcompRED:
	    rrc->rrcGotComponent= 1;
	    rrc->rrcColor.rgb8Red= arg;
	    break;
	case RGBAcompGREEN:
	    rrc->rrcGotComponent= 1;
	    rrc->rrcColor.rgb8Green= arg;
	    break;
	case RGBAcompBLUE:
	    rrc->rrcGotComponent= 1;
	    rrc->rrcColor.rgb8Blue= arg;
	    break;
	default:
	    /* SLDEB(rcw->rcwWord,arg); */
	    break;
	}

    return 0;
    }

int docRtfColorTable(		const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rrc )
    {
    rrc->rrcGotComponent= 0;

    rrc->rrcColor.rgb8Red= 255;
    rrc->rrcColor.rgb8Green= 255;
    rrc->rrcColor.rgb8Blue= 255;
    rrc->rrcColor.rgb8Alpha= 255;

    if  ( docRtfReadGroup( rcw, 0, 0, rrc,
		    (RtfControlWord *)0, docRtfSaveColor, (RtfCommitGroup)0 ) )
	{ SLDEB(rcw->rcwWord,arg); return -1;	}

    return 0;
    }

