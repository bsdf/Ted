/************************************************************************/
/*									*/
/*  Save a BufferDocument into an RTF file.				*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<stdio.h>
#   include	<ctype.h>

#   include	<bitmap.h>

#   include	<appDebugon.h>

#   include	"docRtfFlags.h"
#   include	"docRtfTags.h"
#   include	<docField.h>
#   include	<docTreeType.h>
#   include	<docTreeNode.h>
#   include	<docNotes.h>
#   include	"docRtfWriterImpl.h"
#   include	<docBookmarkField.h>
#   include	<docTextParticule.h>
#   include	<docDocumentNote.h>

/************************************************************************/
/*									*/
/*  Finish/Begin writing a field.					*/
/*									*/
/************************************************************************/

static void docRtfFinishFldrslt(	RtfWriter *	rwc )
    {
    docRtfWriteDestinationEnd( rwc );
    docRtfWriteDestinationEnd( rwc );
    docRtfWriteDestinationEnd( rwc );

    rwc->rwcInFldrslt--;

    if  ( docRtfPopAttribute( rwc ) )
	{ LDEB(1);	}

    return;
    }

static void docRtfWriteWriteComponent(	RtfWriter *			rwc,
					const InstructionsComponent *	ic )
    {
    if  ( ic->icIsQuoted )
	{ docRtfWriteDocEncodedString( rwc, "\"", 1 );	}

    docRtfWriteDocEncodedString( rwc,
				(const char *)ic->icBuffer.mbBytes,
				ic->icBuffer.mbSize );
    if  ( ic->icIsQuoted )
	{ docRtfWriteDocEncodedString( rwc, "\"", 1 );	}

    return;
    }

static void docRtfWriteFieldInstructions( RtfWriter *			rwc,
					const FieldInstructions *	fi )
    {
    const InstructionsComponent *	ic;
    int					byteCount= 0;
    int					i;

    ic= fi->fiComponents;
    for ( i= 0; i < fi->fiComponentCount; i++ )
	{
	if  ( ic->icIsQuoted )
	    { byteCount += 2;	}
	ic++;
	}

    if  ( fi->fiComponentCount > 1 )
	{ byteCount += fi->fiComponentCount- 1;	}

    docRtfReserveColumns( rwc, byteCount );

    ic= fi->fiComponents;
    if  ( fi->fiComponentCount > 0 )
	{ docRtfWriteWriteComponent( rwc, ic++ );	}
    for ( i= 1; i < fi->fiComponentCount; i++ )
	{
	docRtfWriteDocEncodedString( rwc, " ", 1 );
	docRtfWriteWriteComponent( rwc, ic++ );
	}

    return;
    }

static void docRtfWriteStartField(	RtfWriter *		rwc,
					const DocumentField *	df )
    {
    docRtfWriteDestinationBegin( rwc, "field" );
    docRtfWriteDestinationBegin( rwc, "*\\fldinst" );
    docRtfWriteDestinationBegin( rwc, "" );

    docRtfWriteFieldInstructions( rwc, &(df->dfInstructions) );

    /*?
    docRtfWriteDocEncodedString( rwc, " ", 1 );
    */

    docRtfWriteDestinationEnd( rwc );
    docRtfWriteDestinationEnd( rwc );

    docRtfWriteDestinationBegin( rwc, "fldrslt" );
    docRtfWriteDestinationBegin( rwc, "" );

    rwc->rwcInFldrslt++;

    if  ( docRtfPushAttribute( rwc ) )
	{ LDEB(1);	}

    return;
    }

static void docRtfBookmarkTag(	RtfWriter *		rwc,
				const char *		tag,
				const DocumentField *	df )
    {
    const int			addSemicolon= 0;

    const MemoryBuffer *		mbBookmark;

    if  ( ! docFieldGetBookmark( &mbBookmark, df ) )
	{
	docRtfWriteDocEncodedStringDestination( rwc, tag,
			    (const char *)mbBookmark->mbBytes,
			    mbBookmark->mbSize, addSemicolon );
	}

    return;
    }

static void docRtfStartTc(	RtfWriter *		rwc,
				const DocumentField *	df )
    {
    const FieldKindInformation *	fki= DOC_FieldKinds+ df->dfKind;

    docRtfWriteDestinationBegin( rwc, fki->fkiRtfTag );

    docRtfWriteFieldInstructions( rwc, &(df->dfInstructions) );

    /* The field instructions actually contain tags */
    rwc->rwcAfter= RTFafterTAG;

    return;
    }

/************************************************************************/
/*									*/
/*  Save a note.							*/
/*									*/
/************************************************************************/

static int docRtfSaveNoteField(	RtfWriter *		rw,
				int *			pCount,
				const BufferItem *	paraNode,
				const int		part,
				const TextParticule *	tp,
				const DocumentField *	df )
    {
    const DocumentNote *	dn;
    const NoteProperties *	np;
    int				count;
    int				closed;

    const char *		first= (const char *)0;
    int				attrNr= -1;
    const int			paralen= docParaStrlen( paraNode );
    int				stroff= paralen;

    count= docCountParticulesInField( paraNode, &closed, part,
					    paraNode->biParaParticuleCount );

    /*  Prefer the attributes of the field result */
    if  ( count > 0 && tp[1].tpKind == DOCkindSPAN )
	{
	stroff= tp[1].tpStroff;
	attrNr= tp[1].tpTextAttrNr;
	}
    else{
	stroff= tp[0].tpStroff+ tp[0].tpStrlen;
	attrNr= tp[0].tpTextAttrNr;
	}

    if  ( stroff < paralen )
	{ first= (const char *)docParaString( paraNode, stroff );	}

    docRtfWriteSwitchTextAttributes( rw, attrNr, first );

    dn= docGetNoteOfField( df, rw->rwDocument );
    if  ( ! dn )
	{ XDEB(dn); return -1;	}
    np= &(dn->dnNoteProperties);

    if  ( np->npAutoNumber )
	{
	docRtfWriteTag( rw, RTFtag_chftn );

	if  ( paraNode->biTreeType == DOCinBODY			&&
	      rw->rwTextAttribute.taSuperSub != TEXTvaREGULAR	)
	    {
	    docRtfWriteTag( rw, RTFtag_nosupersub );
	    rw->rwTextAttribute.taSuperSub= TEXTvaREGULAR;
	    }
	}
    else{
	docRtfWriteFontEncodedString( rw,
				(const char *)np->npFixedText.mbBytes,
				np->npFixedText.mbSize );
	}

    if  ( df->dfNoteIndex >= 0 && dn->dnDocumentTree.dtRoot )
	{
	const int		evenIfAbsent= 0;
	const int		forcePar= 0;

	if  ( paraNode->biTreeType == DOCinFOOTNOTE	||
	      paraNode->biTreeType == DOCinENDNOTE	)
	    { SDEB(docTreeTypeStr(paraNode->biTreeType));	}
	else{
	    switch( np->npTreeType )
		{
		case DOCinFOOTNOTE:
		    if  ( docRtfSaveDocumentTree( rw, RTFtag_footnote,
			    &(dn->dnDocumentTree), evenIfAbsent, forcePar ) )
			{ LDEB(np->npTreeType); return -1;	}
		    break;

		case DOCinENDNOTE:
		    if  ( docRtfSaveDocumentTree( rw, "footnote\\ftnalt",
			    &(dn->dnDocumentTree), evenIfAbsent, forcePar ) )
			{ LDEB(np->npTreeType); return -1;	}
		    break;

		default:
		    LDEB(np->npTreeType);
		    break;
		}
	    }
	}

    *pCount= count;
    return 0;
    }

/************************************************************************/
/*									*/
/*  Save a field head particule						*/
/*									*/
/************************************************************************/

int docRtfSaveFieldHead(		RtfWriter *		rwc,
					const BufferItem *	paraNode,
					int			part )
    {
    const DocumentField *		df;
    const FieldKindInformation *	fki;
    const TextParticule *		tp= paraNode->biParaParticules+ part;

    df= docGetFieldByNumber( &(rwc->rwDocument->bdFieldList),
							tp->tpObjectNumber );
    if  ( ! df )
	{ LPDEB(tp->tpObjectNumber,df); return 0;	}

    fki= DOC_FieldKinds+ df->dfKind;

    if  ( fki->fkiIsFieldInRtf )
	{
	if  ( df->dfKind != DOCfkMERGEFIELD				||
	      ! ( rwc->rwSaveFlags & RTFflagNO_MERGEFIELDS )		)
	    { docRtfWriteStartField( rwc, df );	}
	}

    if  ( df->dfKind == DOCfkBOOKMARK			&&
	  ! ( rwc->rwSaveFlags & RTFflagNO_BOOKMARKS )	)
	{ docRtfBookmarkTag( rwc, "*\\bkmkstart", df );	}

    if  ( df->dfKind == DOCfkTC || df->dfKind == DOCfkTCN )
	{ docRtfStartTc( rwc, df ); }

    if  ( df->dfKind == DOCfkCHFTN )
	{
	int		count= 0;

	if  ( docRtfSaveNoteField( rwc, &count, paraNode, part, tp, df ) )
	    { LDEB(df->dfKind); return -1;	}

	return count;
	}

    if  ( df->dfKind == DOCfkLISTTEXT		&&
	  paraNode->biParaListOverride > 0	)
	{
	if  ( docRtfPushAttribute( rwc ) )
	    { LDEB(1);	}

	docRtfWriteDestinationBegin( rwc, "listtext" );
	}

    if  ( fki->fkiIsDestinationInRtf	&&
	  fki->fkiRtfTag		)
	{
	docRtfWriteDestinationBegin( rwc, fki->fkiRtfTag );

	docRtfWriteDocEncodedString( rwc,
		(char *)df->dfData.mbBytes, df->dfData.mbSize );

	docRtfWriteDestinationBegin( rwc, "" );

	docRtfWriteFieldInstructions( rwc, &(df->dfInstructions) );

	docRtfWriteDestinationEnd( rwc );
	docRtfWriteDestinationEnd( rwc );
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Save a field tail particule						*/
/*									*/
/************************************************************************/

int docRtfSaveFieldTail(		RtfWriter *		rwc,
					const BufferItem *	paraNode,
					int			part )
    {
    const DocumentField *		df;
    const FieldKindInformation *	fki;
    const TextParticule *		tp= paraNode->biParaParticules+ part;

    if  ( tp->tpObjectNumber < 0 )
	{ LDEB(tp->tpObjectNumber); return 0; }

    df= docGetFieldByNumber( &(rwc->rwDocument->bdFieldList),
						    tp->tpObjectNumber );
    if  ( ! df )
	{ LPDEB(tp->tpObjectNumber,df); return 0;	}

    fki= DOC_FieldKinds+ df->dfKind;

    if  ( df->dfKind == DOCfkBOOKMARK			&&
	  ! ( rwc->rwSaveFlags & RTFflagNO_BOOKMARKS )	)
	{ docRtfBookmarkTag( rwc, "*\\bkmkend", df ); }

    if  ( df->dfKind == DOCfkTC || df->dfKind == DOCfkTCN )
	{ docRtfWriteDestinationEnd( rwc ); }

    if  ( fki->fkiIsFieldInRtf )
	{
	if  ( df->dfKind != DOCfkMERGEFIELD		    	||
	      ! ( rwc->rwSaveFlags & RTFflagNO_MERGEFIELDS )	)
	    {
	    if  ( rwc->rwcInFldrslt )
		{ docRtfFinishFldrslt( rwc );	}
	    }
	}

    if  ( df->dfKind == DOCfkLISTTEXT		&&
	  paraNode->biParaListOverride > 0	)
	{
	docRtfWriteDestinationEnd( rwc );

	if  ( docRtfPopAttribute( rwc ) )
	    { LDEB(1);	}
	}

    return 0;
    }

