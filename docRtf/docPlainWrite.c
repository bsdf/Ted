/************************************************************************/
/*									*/
/*  Save a BufferDocument into a plain text file.			*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<stdio.h>
#   include	<ctype.h>

#   include	<docBuf.h>
#   include	<docNotes.h>
#   include	<docTreeNode.h>
#   include	<docNodeTree.h>
#   include	<docTreeType.h>
#   include	<docTreeScanner.h>
#   include	"docPlainReadWrite.h"
#   include	<docParaParticules.h>
#   include	<docTextParticule.h>
#   include	<docDocumentNote.h>
#   include	<sioGeneral.h>

#   include	<appDebugon.h>

typedef struct PlainWritingContext
    {
    SimpleOutputStream *	pwcSos;
    BufferDocument *		pwcDocument;

    int				pwcFold;
    int				pwcHasOpenEnd;
    int				pwcNoteRefCount;
    int				pwcNoteDefCount;
    } PlainWritingContext;

static void docInitPlainWritingContext(	PlainWritingContext *	pwc )
    {
    pwc->pwcSos= (SimpleOutputStream *)0;
    pwc->pwcDocument= (BufferDocument *)0;

    pwc->pwcFold= 0;
    pwc->pwcHasOpenEnd= 0;
    pwc->pwcNoteRefCount= 0;
    pwc->pwcNoteDefCount= 0;
    }

static int docPlainSaveParaNode(	PlainWritingContext *		pwc,
					const BufferItem *		paraNode,
					const DocumentSelection *	ds )
    {
    TextParticule *		tp;
    int				pos= 0;
    int				lineCount= 0;

    int				part= 0;
    int				partUpto= paraNode->biParaParticuleCount;
    int				stroffFrom= -1;
    int				stroffUpto= -1;
    int				headFlags= 0;
    int				tailFlags= 0;
    int				close= 1;

    BufferDocument *		bd= pwc->pwcDocument;
    SimpleOutputStream *	sos= pwc->pwcSos;

    const DocumentField *	df;

    if  ( ds )
	{
	DocumentSelection	dsPara;

	if  ( docIntersectSelectionWithParagraph( &dsPara,
					&part, &partUpto,
					&headFlags, &tailFlags,
					paraNode, ds ) )
	    { LDEB(1); return -1;	}

	stroffFrom= dsPara.dsHead.dpStroff;
	stroffUpto= dsPara.dsTail.dpStroff;

	if  ( ds->dsTail.dpNode == paraNode )
	    { close= ! pwc->pwcHasOpenEnd;	}
	}
    else{
	if  ( ! docNextParagraph( (BufferItem *)paraNode ) )
	    { close= ! pwc->pwcHasOpenEnd;	}
	}

    tp= paraNode->biParaParticules+ part;
    while( part < paraNode->biParaParticuleCount )
	{
	TextAttribute		ta;

	docGetTextAttributeByNumber( &ta, bd, tp->tpTextAttrNr );

	switch( tp->tpKind )
	    {
	    case DOCkindTAB:
		if  ( sioOutPutByte( '\t', sos ) < 0 )
		    { LDEB(1); return -1;	}
		pos= 8* ( pos+ 8 )/ 8;
		break;

	    case DOCkindSPAN:
		if  ( pwc->pwcFold && pos > 8 && pos+ tp->tpStrlen >= 72 )
		    {
		    if  ( sioOutPutByte( '\n', sos ) < 0 )
			{ LDEB(1); return -1;	}
		    lineCount++; pos= 0;

		    if  ( paraNode->biParaFirstIndentTwips < -9	&&
			  paraNode->biParaLeftIndentTwips > 9	)
			{
			if  ( sioOutPutByte( '\t', sos ) < 0 )
			    { LDEB(1); return -1;	}
			pos= 8;
			}
		    }

		{
		int			stroff= tp->tpStroff;
		int			upto= stroffUpto;
		const unsigned char *	s;

		if  ( stroff < stroffFrom )
		    { stroff=  stroffFrom;	}
		if  ( upto < 0 )
		    { upto= tp->tpStroff+ tp->tpStrlen;	}

		s= docParaString( paraNode, stroff );
		while( stroff < upto )
		    {
		    if  ( sioOutPutByte( *s, sos ) < 0 )
			{ LDEB(1); return -1;	}
		    s++; stroff++; pos++;
		    }
		}

		break;

	    case DOCkindLINEBREAK:
		if  ( sioOutPutByte( '\n', sos ) < 0 )
		    { LDEB(1); return -1;	}
		pos= 0;
		break;

	    case DOCkindPAGEBREAK:
	    case DOCkindCOLUMNBREAK:
		if  ( sioOutPutByte( '\n', sos ) < 0 )
		    { LDEB(1); return -1;	}
		if  ( sioOutPutByte( '\f', sos ) < 0 )
		    { LDEB(1); return -1;	}
		pos= 0;
		break;

	    case DOCkindFIELDHEAD:
		df= docGetFieldByNumber( &(bd->bdFieldList),
						    tp->tpObjectNumber );

		if  ( df->dfKind == DOCfkPAGEREF )
		    {
		    int		count;
		    int		closed;

		    count= docCountParticulesInField( paraNode, &closed, part,
						paraNode->biParaParticuleCount );

		    count++;
		    tp += count; part += count;
		    break;
		    }

		if  ( df->dfKind == DOCfkCHFTN )
		    {
		    int		count;
		    int		closed;
		    char	scratch[20+1];

		    count= docCountParticulesInField( paraNode, &closed, part,
						paraNode->biParaParticuleCount );

		    if  ( paraNode->biTreeType == DOCinBODY )
			{
			sprintf( scratch, "[%d]", pwc->pwcNoteRefCount+ 1 );
			pwc->pwcNoteRefCount++;
			}
		    else{
			sprintf( scratch, "[%d]", pwc->pwcNoteDefCount+ 1 );
			pwc->pwcNoteDefCount++;
			}

		    sioOutPutString( scratch, sos );

		    tp += count; part += count;
		    }
		break;

	    default:
		LDEB(tp->tpKind);
		/*FALLTHROUGH*/
	    case DOCkindOBJECT:
	    case DOCkindFIELDTAIL:
	    case DOCkindLTR_MARK:
	    case DOCkindRTL_MARK:
		break;
	    }

	 part++; tp++;
	 }

    if  ( close )
	{
	if  ( sioOutPutByte( '\n', sos ) < 0 )
	    { LDEB(1); return -1;	}

	lineCount++;
	}

    return 0;
    }

static int docPlainEnterNode(	BufferItem *			node,
				const DocumentSelection *	ds,
				const BufferItem *		bodySectNode,
				void *				voidpwc )
    {
    PlainWritingContext *	pwc= (PlainWritingContext *)voidpwc;

    switch( node->biLevel )
	{
	case DOClevBODY:
	case DOClevSECT:
	case DOClevCELL:
	case DOClevROW:
	    return ADVICEtsOK;

	case DOClevPARA:
	    if  ( docPlainSaveParaNode( pwc, node, ds ) )
		{ LDEB(1); return -1;	}
	    return ADVICEtsOK;

	default:
	    LDEB(node->biLevel); return -1;
	}
    }


int docPlainSaveDocument(	SimpleOutputStream *		sos,
				BufferDocument *		bd,
				const DocumentSelection *	ds,
				int				fold )
    {
    const int			flags= 0;
    PlainWritingContext		pwc;

    docInitPlainWritingContext( &pwc );

    pwc.pwcFold= fold;
    pwc.pwcHasOpenEnd= bd->bdProperties.dpHasOpenEnd;
    pwc.pwcDocument= bd;
    pwc.pwcSos= sos;

    if  ( ds )
	{ pwc.pwcHasOpenEnd= 1;	}

    if  ( ds )
	{
	if  ( docScanSelection( bd, ds, docPlainEnterNode,
				(NodeVisitor)0, flags, (void *)&pwc ) < 0 )
	    { LDEB(1); return -1; }
	}
    else{
	if  ( docScanTree( bd, &(bd->bdBody), docPlainEnterNode,
				(NodeVisitor)0, flags, (void *)&pwc ) < 0 )
	    { LDEB(1); return -1; }
	}

    if  ( ! ds && pwc.pwcNoteRefCount > 0 )
	{
	DocumentField *	dfNote;
	DocumentNote *	dn;

	sioOutPutString( "\n-----\n\n", sos );

	for ( dfNote= docGetFirstNoteOfDocument( &dn, bd, -1 );
	      dfNote;
	      dfNote= docGetNextNoteInDocument( &dn, bd, dfNote, -1 ) )
	    {
	    if  ( docScanTree( bd, &(dn->dnDocumentTree), docPlainEnterNode,
		    (NodeVisitor)0, flags, (void *)&pwc ) < 0 )
		{ LDEB(1); return -1; }
	    }

	if  ( pwc.pwcNoteDefCount != pwc.pwcNoteRefCount )
	    { LLDEB(pwc.pwcNoteDefCount,pwc.pwcNoteRefCount);	}
	}

    return 0;
    }
