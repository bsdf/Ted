/************************************************************************/
/*									*/
/*  Read footnotes, headers, footers, textboxes.			*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<string.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	<appDebugon.h>

#   include	"docRtfReaderImpl.h"
#   include	<docParaParticules.h>
#   include	<docRecalculateFields.h>
#   include	<docTreeType.h>
#   include	<docNodeTree.h>
#   include	<docNotes.h>
#   include	<docDocumentNote.h>

/************************************************************************/
/*									*/
/*  Consume Headers, Footers, Notes &c: Separate item trees that are.	*/
/*  embedded in the document.						*/
/*									*/
/*  1)  Save the current position.					*/
/*  2)  Make the header/note &c.					*/
/*  3)  Consume its contents.						*/
/*  4)  Make sure that no bookmarks protrude beyond the end of the	*/
/*	external item.							*/
/*  5)  If the external item happens to be empty, insert an empty	*/
/*	paragraph to avoid problems later on.				*/
/*  6)  Restore the current position.					*/
/*									*/
/************************************************************************/

int docRtfReadDocumentTree(	const RtfControlWord *	rcw,
				DocumentTree *		dt,
				int *			pExtItKind,
				RtfReader *		rrc,
				int			ignoreEmpty,
				const SelectionScope *	ss )
    {
    int				rval= 0;
    RtfReadingState		internRrs;

    DocumentTree *		savedEi;
    BufferItem *		savedBi;
    int				savedLevel;
    SelectionScope		savedSelectionScope;
    struct RtfFieldStackLevel *	savedFieldStackLevel;
    int				savedLastFieldNumber;
    RowProperties		savedRowProperties;

    int				changed= 0;

    /*  1  */
    savedEi= rrc->rrcTree;
    savedBi= rrc->rrcNode;
    savedLevel= rrc->rrcLevel;
    savedSelectionScope= rrc->rrcSelectionScope;
    savedFieldStackLevel= rrc->rrcFieldStack;
    savedLastFieldNumber= rrc->rrcLastFieldNumber;
    savedRowProperties= rrc->rrcRowProperties;

    docInitRowProperties( &(rrc->rrcRowProperties) );

    docRtfPushReadingState( rrc, &internRrs );

    docRtfResetParagraphProperties( &internRrs );
    docRtfResetTextAttribute( &internRrs, rrc->rrDocument );

    if  ( ! savedBi )
	{ XDEB(savedBi); rval= -1; goto ready;	}

    /*  2  */
    if  ( ! dt->dtRoot							&&
	  docMakeDocumentTree( rrc->rrDocument, dt,
				    ss, &(rrc->rrcSectionProperties) )	)
	{ LDEB(1); rval= -1; goto ready;	}

    /*  3  */
    rrc->rrcTree= dt;
    rrc->rrcNode= dt->dtRoot;
    rrc->rrcLevel= DOClevSECT;
    rrc->rrcSelectionScope= *ss;
    rrc->rrcFieldStack= (struct RtfFieldStackLevel *)0;
    rrc->rrcLastFieldNumber= -1;

    if  ( docRtfReadGroup( rcw, 0, 0, rrc,
			    docRtfDocumentGroups,
			    docRtfTextParticule, docRtfFinishCurrentNode ) )
	{ LDEB(1); rval= -1;	}

    /*  5  */
    {
    DocumentPosition	dp;

    if  ( docTailPosition( &dp, dt->dtRoot ) )
	{
	if  ( ignoreEmpty )
	    { docEraseDocumentTree( rrc->rrDocument, dt );	}
	else{
	    const int	textAttributeNumber= 0;

	    if  ( ! docInsertEmptyParagraph( rrc->rrDocument, dt->dtRoot,
						textAttributeNumber ) )
		{ LDEB(textAttributeNumber);	}
	    }
	}
    else{
	/********************************************************/
	/*  Delete empty paragraph caused by final \par		*/
	/********************************************************/
	if  ( docParaStrlen( dp.dpNode ) ==  0		&&
	      dp.dpNode->biParaTableNesting == 0		&&
	      docNumberOfParagraph( dp.dpNode ) > 1	&&
	      dp.dpNode->biParaParticuleCount == 1	)
	    {
	    BufferItem *	bi= dp.dpNode;

	    while( bi->biParent && bi->biParent->biChildCount == 1 )
		{ bi= bi->biParent;	}

	    docDeleteNode( rrc->rrDocument, dt, bi );
	    }
	}
    }

    /*  4  */
    if  ( docRtfPopScopeFromFieldStack( rrc ) )
	{ LDEB(1); rval= -1; goto ready;	}

    docRenumberSeqFields( &changed, rrc->rrcTree, rrc->rrDocument );

    *pExtItKind= rrc->rrcSelectionScope.ssTreeType;

  ready:
    /*  6  */

    docCleanRowProperties( &(rrc->rrcRowProperties) );
    rrc->rrcRowProperties= savedRowProperties;

    rrc->rrcLastFieldNumber= savedLastFieldNumber;
    rrc->rrcFieldStack= savedFieldStackLevel;
    rrc->rrcSelectionScope= savedSelectionScope;
    rrc->rrcLevel= savedLevel;
    rrc->rrcNode= savedBi;
    rrc->rrcTree= savedEi;

    docRtfPopReadingState( rrc );

    return rval;
    }

int docRtfReadExtTree(		const RtfControlWord *	rcw,
				int			arg,
				RtfReader *	rrc )
    {
    BufferItem *	sectNode;
    BufferDocument *	bd= rrc->rrDocument;
    DocumentTree *	dt;

    SelectionScope	ss;
    int			treeType;
    int			ignoreEmpty= 0;

    docInitSelectionScope( &ss );
    ss.ssSectNr= 0;

    switch( rcw->rcwID )
	{
	case DOCinFIRST_HEADER:
	case DOCinLEFT_HEADER:
	case DOCinRIGHT_HEADER:

	case DOCinFIRST_FOOTER:
	case DOCinLEFT_FOOTER:
	case DOCinRIGHT_FOOTER:

	    sectNode= docRtfGetSectNode( rrc );
	    if  ( ! sectNode )
		{ XDEB(sectNode); return -1;	}

	    dt= docSectionHeaderFooter( sectNode, (unsigned char *)0,
			    &(rrc->rrDocument->bdProperties), rcw->rcwID );
	    if  ( ! dt )
		{ LXDEB(rcw->rcwID,dt); return -1;	}

	    ss.ssOwnerSectNr= sectNode->biNumberInParent;
	    ignoreEmpty= 1;
	    break;

	case DOCinFTNSEP:
	case DOCinFTNSEPC:
	case DOCinFTNCN:
	case DOCinAFTNSEP:
	case DOCinAFTNSEPC:
	case DOCinAFTNCN:

	    if  ( ! bd )
		{ XDEB(bd); return -1;	}

	    dt= docDocumentNoteSeparator( bd, rcw->rcwID );
	    if  ( ! dt )
		{ LXDEB(rcw->rcwID,dt); return -1;	}

	    ignoreEmpty= 1;
	    break;

	default:
	    SDEB(rcw->rcwWord); return -1;
	}

    if  ( dt->dtRoot )
	{ docEraseDocumentTree( bd, dt );	}

    ss.ssTreeType= rcw->rcwID;
    ss.ssOwnerNumber= -1;

    if  ( docRtfReadDocumentTree( rcw, dt, &treeType, rrc, ignoreEmpty, &ss ) )
	{ SDEB(rcw->rcwWord); return -1;	}

    return 0;
    }

/************************************************************************/

static int docExtractFixedTextNote(	DocumentNote *		dn,
					BufferDocument *	bd,
					BufferItem *		ownerNode,
					int			fixedStroff,
					int			fixedStrlen,
					const char *		fieldinst,
					int			fieldsize )

    {
    DocumentSelection	dsField;
    BufferItem *	noteBi;
    const int		part0= 0;

    if  ( utilMemoryBufferSetBytes( &(dn->dnNoteProperties.npFixedText),
		    docParaString( ownerNode,  fixedStroff ), fixedStrlen ) )
	{ LDEB(fixedStrlen);	}

    if  ( docHeadPosition( &(dsField.dsHead),
					dn->dnDocumentTree.dtRoot ) )
	{ LDEB(1); return -1;	}
    noteBi= dsField.dsHead.dpNode;
    docSetDocumentPosition( &(dsField.dsTail), noteBi, fixedStrlen );

    if  ( docParaStrlen( noteBi ) >= fixedStrlen			&&
	  ! memcmp( docParaString( noteBi, 0 ),
		docParaString( ownerNode, fixedStroff ), fixedStrlen )	)
	{
	int			part1;
	TextParticule *		tp1;
	DocumentField *		df;

	if  ( docFindParticuleOfPosition( &part1, (int *)0,
					&(dsField.dsTail), PARAfindFIRST ) )
	    { LDEB(fixedStroff); return -1; }
	tp1= noteBi->biParaParticules+ part1;

	if  ( tp1->tpStroff+ tp1->tpStrlen > fixedStrlen )
	    {
	    if  ( docSplitTextParticule( &tp1, (TextParticule **)0,
					    noteBi, part1, fixedStrlen ) )
		{ LDEB(part1); return -1;	}
	    }

	part1++;

	df= docMakeField( bd, &(dn->dnDocumentTree), &dsField,
		    part0, part1, tp1->tpTextAttrNr, tp1->tpTextAttrNr );
	if  ( ! df )
	    { XDEB(df); return -1;	}

	df->dfKind= DOCfkCHFTN;
	if  ( docSetFieldInst( df, fieldinst, fieldsize ) )
	    { LDEB(1);	}
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Consume a footnote.							*/
/*									*/
/************************************************************************/

int docRtfReadFootnote(		const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rrc )
    {
    int			rval= 0;
    BufferDocument *	bd= rrc->rrDocument;

    DocumentField *	dfNote;
    DocumentNote *	dn;
    int			noteIndex;

    BufferItem *	ownerNode;
    BufferItem *	sectNode;
    SelectionScope	ss;

    int			treeType;
    int			autoNumber= 0;
    const int		ignoreEmpty= 0;

    int			fixedStroff= 0;
    int			fixedStrlen= 0;

    const char *	fieldinst= " -CHFTN ";
    int			fieldsize= 8;

    docInitSelectionScope( &ss );

    ownerNode= docRtfGetParaNode( rrc );
    if  ( ! ownerNode )
	{ XDEB(ownerNode); rval= -1; goto ready;	}
    sectNode= docGetSectNode( ownerNode );
    if  ( ! sectNode )
	{ XDEB(sectNode); rval= -1; goto ready;	}

    if  ( rrc->rrcAfterNoteref )
	{ autoNumber= 1;	}
    else{
	const char *		fieldRslt= (const char *)0;

	DocumentPosition	dp;
	int			part0;

	dfNote= docRtfSpecialField( DOCfkCHFTN,
				    fieldinst, fieldsize, fieldRslt, rrc );
	if  ( ! dfNote )
	    { SDEB(rcw->rcwWord); rval= -1; goto ready; }

	docSetDocumentPosition( &dp, ownerNode,
					dfNote->dfHeadPosition.epStroff );
	if  ( docFindParticuleOfPosition( &part0, (int *)0,
						    &dp, PARAfindFIRST ) )
	    { LDEB(dfNote->dfHeadPosition.epStroff); rval= -1; goto ready; }

	/* Move the fixed text into the note field */
	if  ( ownerNode->biParaParticules[part0   ].tpKind == DOCkindSPAN	&&
	      ownerNode->biParaParticules[part0+ 1].tpKind == DOCkindFIELDHEAD )
	    {
	    fixedStroff= ownerNode->biParaParticules[part0   ].tpStroff;
	    fixedStrlen= ownerNode->biParaParticules[part0   ].tpStrlen;

	    dfNote->dfHeadPosition.epStroff= fixedStroff;

	    ownerNode->biParaParticules[part0+ 1].tpKind= DOCkindSPAN;
	    ownerNode->biParaParticules[part0+ 1].tpObjectNumber= -1;
	    ownerNode->biParaParticules[part0+ 1].tpStroff= fixedStroff;
	    ownerNode->biParaParticules[part0+ 1].tpStrlen= fixedStrlen;

	    ownerNode->biParaParticules[part0   ].tpKind= DOCkindFIELDHEAD;
	    ownerNode->biParaParticules[part0   ].tpObjectNumber=
						    dfNote->dfFieldNumber;
	    ownerNode->biParaParticules[part0   ].tpStrlen= 0;
	    }

	autoNumber= 0;
	if  ( fixedStrlen > 0 )
	    {
	    PROPmaskADD( &(rrc->rrcNotePropertyMask), NOTEpropFIXED_TEXT );
	    }
	}

    dfNote= docGetFieldByNumber( &(bd->bdFieldList), rrc->rrcLastFieldNumber );
    if  ( ! dfNote )
	{ LPDEB(rrc->rrcLastFieldNumber,dfNote); return -1;	}

    noteIndex= docInsertNote( &dn, bd, dfNote, autoNumber );
    if  ( noteIndex < 0 )
	{ LDEB(noteIndex); rval= -1; goto ready;	}

    rrc->rrcAfterNoteref= 0;
    rrc->rrAfterParaHeadField= 0;

    ss.ssTreeType= DOCinFOOTNOTE;
    ss.ssSectNr= 0;
    ss.ssOwnerSectNr= sectNode->biNumberInParent;
    ss.ssOwnerNumber= dfNote->dfFieldNumber;

    if  ( docRtfReadDocumentTree( rcw, &(dn->dnDocumentTree), &treeType,
						rrc, ignoreEmpty, &ss ) )
	{ SDEB(rcw->rcwWord); rval= -1; goto ready;	}

    dn->dnNoteProperties.npAutoNumber= autoNumber;
    dn->dnNoteProperties.npTreeType= treeType;
    docSetTreeTypeOfNode( dn->dnDocumentTree.dtRoot, treeType );

    if  ( ! autoNumber							&&
	  fixedStrlen > 0						&&
	  docExtractFixedTextNote( dn, bd, ownerNode,
					fixedStroff, fixedStrlen,
					fieldinst, fieldsize )		)
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    return rval;
    }

