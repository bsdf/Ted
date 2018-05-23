/************************************************************************/
/*									*/
/*  Handle paragraph numbers in the RTF input.				*/
/*  (Should eventually be translated to lists)				*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	<appDebugon.h>

#   include	"docRtfReaderImpl.h"
#   include	"docRtfTagEnum.h"

/************************************************************************/
/*									*/
/*  Consume the description of a Paragraph Numbering level for a	*/
/*  section.								*/
/*									*/
/************************************************************************/

static int docRtfCommitPnTxta(	const RtfControlWord *	rcw,
				RtfReader *		rrc )
    {
    int			size;
    const int		removeSemicolon= 0;

    if  ( docRtfStoreSavedText( &(rrc->rrcParagraphNumber.pnTextAfter),
						&size, rrc, removeSemicolon ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

static int docRtfCommitPnTxtb(	const RtfControlWord *	rcw,
				RtfReader *		rrc )
    {
    int			size;
    const int		removeSemicolon= 0;

    if  ( docRtfStoreSavedText( &(rrc->rrcParagraphNumber.pnTextBefore),
						&size, rrc, removeSemicolon ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

static RtfControlWord	docRtfPndescGroups[]=
    {
	RTF_TEXT_GROUP( "pntxta",	RTFidPNTXTA,	docRtfCommitPnTxta ),
	RTF_TEXT_GROUP( "pntxtb",	RTFidPNTXTB,	docRtfCommitPnTxtb ),

	{ (char *)0, 0, 0 }
    };

int docRtfReadPnseclvl(	const RtfControlWord *	rcw,
			int			arg,
			RtfReader *		rrc )
    {
    int			res;
    int			level= arg- 1;
    struct BufferItem *	sectNode;

    if  ( level < 0 )
	{ LDEB(level);	}

    docCleanParagraphNumber( &(rrc->rrcParagraphNumber) );
    docInitParagraphNumber( &(rrc->rrcParagraphNumber) );

    sectNode= docRtfGetSectNode( rrc );
    if  ( ! sectNode )
	{ XDEB(sectNode); return -1;	}

    res= docRtfReadGroup( rcw, 0, 0, rrc,
				docRtfPndescGroups,
				docRtfRefuseText, (RtfCommitGroup)0 );
    if  ( res )
	{ SLDEB(rcw->rcwWord,res); return res;	}

    rrc->rrcParagraphNumber.pnLevel= level;

    if  ( level >= rrc->rrcParagraphNumberCount )
	{
	ParagraphNumber *	fresh;

	fresh= (ParagraphNumber *)realloc( rrc->rrcParagraphNumbers,
				( level+ 1 )* sizeof( ParagraphNumber ) );
	if  ( ! fresh )
	    { LXDEB(level,fresh); return -1;	}

	rrc->rrcParagraphNumbers= fresh;

	while( rrc->rrcParagraphNumberCount <= level )
	    {
	    docInitParagraphNumber( rrc->rrcParagraphNumbers+ 
					    rrc->rrcParagraphNumberCount++ );
	    }
	}

    rrc->rrcParagraphNumbers[level]= rrc->rrcParagraphNumber;

    return res;
    }

int docRtfSkipPn(	const RtfControlWord *	rcw,
			int			arg,
			RtfReader *		rrc )
    {
    int		res;

    docCleanParagraphNumber( &(rrc->rrcParagraphNumber) );
    docInitParagraphNumber( &(rrc->rrcParagraphNumber) );

    res= docRtfReadGroup( rcw, 0, 0, rrc,
				docRtfPndescGroups,
				docRtfRefuseText, (RtfCommitGroup)0 );
    if  ( res )
	{ SLDEB(rcw->rcwWord,res);	}

    return res;
    }

