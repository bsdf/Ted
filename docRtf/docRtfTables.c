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

int docRtfReadInfo(		const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rrc )
    {
    int		res;

    res= docRtfReadGroup( rcw, 0, 0, rrc,
		    (RtfControlWord *)0, docRtfIgnoreText, (RtfCommitGroup)0 );
    if  ( res )
	{ SLDEB(rcw->rcwWord,res);	}

    return res;
    }

