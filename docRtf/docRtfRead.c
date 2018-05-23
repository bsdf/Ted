/************************************************************************/
/*									*/
/*  Read an RTF text file into a BufferDocument				*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<stdio.h>
#   include	<ctype.h>

#   include	<appDebugon.h>

#   include	"docRtfReaderImpl.h"
#   include	"docRtfReadWrite.h"
#   include	"docRtfTagEnum.h"
#   include	"docRtfFlags.h"
#   include	<docRecalculateFields.h>
#   include	<docTreeType.h>
#   include	<docTreeNode.h>
#   include	<docNotes.h>
#   include	<docDebug.h>

/************************************************************************/

const char DOC_RTF_LENIENT_MESSAGE[]= "Use lenientRtf setting";

/************************************************************************/
/*									*/
/*  Groups (Most of them handeled at the document level).		*/
/*									*/
/************************************************************************/

RtfControlWord	docRtfDocumentGroups[]=
{
    RTF_DEST_XX( "tc",		DOCfkTC,	docRtfLookupEntry ),
    RTF_DEST_XX( "tcn",		DOCfkTCN,	docRtfLookupEntry ),

    RTF_DEST_XX( "nonesttables", 1,		docRtfSkipGroup ),

    RTF_DEST_XX( "trowd",	DOClevROW,	docRtfReadRowProperties ),

    { (char *)0, 0, 0 }
};

/************************************************************************/
/*									*/
/*  Read a document as a whole.						*/
/*									*/
/************************************************************************/

static int docRtfReadDoc(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rrc )
    {
    int		rval= 0;
    int		changed= 0;

    rrc->rrcSelectionScope.ssTreeType= DOCinBODY;

    rval= docRtfReadGroup( rcw, 0, 0, rrc,
				docRtfDocumentGroups,
				docRtfTextParticule,
				docRtfFinishCurrentNode );
    if  ( rval )
	{ SLDEB(rcw->rcwWord,rval);	}

    docRenumberSeqFields( &changed, rrc->rrcTree, rrc->rrDocument );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Read a whole document.						*/
/*									*/
/*  As at least MS-Word 2000 crashes on lists without an override,	*/
/*  and on the other hand staroffice/openoffice creates them under	*/
/*  certain circumstances: Make an empty override for lists without	*/
/*  overrides.								*/
/*									*/
/************************************************************************/

static RtfControlWord	docRtfOutsideGroups[]=
    {
	RTF_DEST_XX( "rtf",	0,	docRtfReadDoc ),

	{ (char *)0, 0, 0 }
    };

BufferDocument * docRtfReadFile(	SimpleInputStream *		sis,
					unsigned int			flags )
    {
    BufferDocument *		rval= (BufferDocument *)0;
    BufferDocument *		bd= (BufferDocument *)0;
    RtfReader *			rr= (RtfReader *)0;

    int				res;
    const RtfControlWord *	rcw;

    char			controlWord[TEDszRTFCONTROL+1];
    int				gotArg;
    int				arg= -1;
    int				c;

    const DocumentField *	dfNote;
    struct DocumentNote *	dn;
    int				changed;

    bd= docNewDocument( (BufferDocument *)0 );
    if  ( ! bd )
	{ XDEB(bd); goto ready;	}

    rr= docRtfOpenReader( sis, bd, flags );
    if  ( ! rr )
	{ XDEB(rr); goto ready;	}

    rr->rrcTree= &(rr->rrDocument->bdBody);
    rr->rrcNode= rr->rrcTree->dtRoot;
    rr->rrcLevel= DOClevBODY;

    res= docRtfFindControl( rr, &c, controlWord, &gotArg, &arg );
    if  ( res != RTFfiCTRLGROUP )
	{ goto ready; }

    rcw= docRtfFindWord( controlWord, docRtfOutsideGroups );
    if  ( ! rcw )
	{ LDEB(1); goto ready; }
    if  ( rcw->rcwType != RTCtypeDEST )
	{ SLDEB(rcw->rcwWord,rcw->rcwType); goto ready;	}

    res= docRtfApplyControlWord( rcw, gotArg, arg, rr );
    if  ( res )
	{ LDEB(1); goto ready; }

    /*  Check against trailing garbage. Spaces are not allowed either, 	*/
    /*  but we do not want to scare the user with invisible things.	*/

    if  ( ! ( rr->rrReadFlags & RTFflagLENIENT ) )
	{
	c= sioInGetByte( sis );
	while( c != EOF )
	    {
	    if  ( c != '\r' && c != '\n' && c != '\0' && c != ' ' )
		{
		const char * message= DOC_RTF_LENIENT_MESSAGE;
		LCSDEB(rr->rrCurrentLine,c,message); goto ready;
		}

	    c= sioInGetByte( sis );
	    }
	}

    if  ( ! rr->rrcGotDocGeometry		&&
	  bd->bdBody.dtRoot->biChildCount > 0	)
	{
	bd->bdProperties.dpGeometry=
		    bd->bdBody.dtRoot->biChildren[0]->biSectDocumentGeometry;
	}

    docRenumberNotes( &changed, bd );

    dfNote= docGetFirstNoteOfDocument( &dn, bd, DOCinFOOTNOTE );
    if  ( dfNote )
	{
	if  ( docCheckSeparatorItemForNoteType( bd, DOCinFOOTNOTE ) )
	    { LDEB(DOCinFTNSEP); goto ready; }
	}

    dfNote= docGetFirstNoteOfDocument( &dn, bd, DOCinENDNOTE );
    if  ( dfNote )
	{
	if  ( docCheckSeparatorItemForNoteType( bd, DOCinENDNOTE ) )
	    { LDEB(DOCinAFTNSEP); goto ready; }
	}

    if  ( docMakeOverrideForEveryList( bd->bdProperties.dpListAdmin ) )
	{ LDEB(1); goto ready;	}

    if  ( docGetCharsUsed( bd ) )
	{ LDEB(1); goto ready;	}

    rval= bd; bd= (BufferDocument *)0; /* steal */

    /* LDEB(1); docListNode(0,rval->bdBody.dtRoot,0); */

  ready:

    if  ( rr )
	{ docRtfCloseReader( rr );	}

    if  ( bd )
	{ docFreeDocument( bd );	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Remember properties to be used subsequently.			*/
/*									*/
/*  References are to be replaced with feature specific routine		*/
/*  refrerences.							*/
/*									*/
/************************************************************************/

int docRtfRememberProperty(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rrc )
    {
    RtfReadingState *	rrs= rrc->rrcState;

    switch( rcw->rcwID )
	{
	case RTFidUC:
	    if  ( rrc->rrcState )
		{ rrc->rrcState->rrsBytesPerUnicode= arg;	}
	    break;
				/****************************************/
				/*  Set/Unset Text Attributes.		*/
				/****************************************/
	case RTFidULNONE:
	    rrs->rrsTextAttribute.taTextIsUnderlined= 0;
	    break;
				/****************************************/
				/*  Set/Unset Text Attributes.		*/
				/*  In Paragraph Numbering.		*/
				/****************************************/
	case RTFidPNULNONE:
	    rrc->rrcParagraphNumber.pnTextAttribute.taTextIsUnderlined= 0;
	    break;

				/****************************************/
				/*  Paragraph Numbering.		*/
				/****************************************/
	case RTFidPNHANG:
	    rrc->rrcParagraphNumber.pnUseHangingIndent= 1;
	    break;
				/****************************************/
				/*  Unknown				*/
				/****************************************/
	default:
	    /*SLDEB(rcw->rcwWord,rcw->rcwID);*/
	    break;
	}

    return 0;
    }
